#if 0
#if defined(__TI_AM335X__)

#include "common.h"
#include "SmsWorker.h"
#include "DebugUtil.h"
#include "AgentLog.h"
#include "CRC16.h"
#include "CommonUtil.h"
#include "EventInterface.h"
#include "Metering.h"
#include "MeterUploader.h"
#include "SensorTimesync.h"
#include "AgentService.h"
#include "SystemMonitor.h"
#include "Patch.h"
#include "ShellCommand.h"
#include "MobileClient.h"
#include "Variable.h"

/** PDU Utility */
#include "pdu.h"

#define KEY_POS     3
#define CMD_POS     5
#define LEN_POS     8

CSmsWorker *m_pSmsWorker = NULL;

extern void CoordinatorPowerControl();
extern int m_nLocalPort;
extern char		m_szFepServer[];

#if     defined(__PATCH_12_969__)
extern int		m_nLocalPort;
extern UINT     m_nSystemID;
#endif

extern MOBILECFGENTRY    m_sMobileConfig;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmsWorker::CSmsWorker()
{
    m_pSmsWorker = this;
	m_pQueue = new CQueue(4096);
    m_bInitialize = FALSE;
    m_bSmsNotification = FALSE;
    m_nBufferOffset = 0;
    memset(m_szBuffer, 0, sizeof(m_szBuffer));
}

CSmsWorker::~CSmsWorker()
{
    if(m_pQueue != NULL)
    {
        delete m_pQueue;
    }
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CSmsWorker::Initialize()
{
    if(m_bInitialize) return TRUE;

	if (!CTimeoutThread::StartupThread(31))
        return FALSE;

    m_bInitialize = TRUE;
	return TRUE;
}

void CSmsWorker::Destroy()
{
	CTimeoutThread::ShutdownThread();
    m_bInitialize = FALSE;
}

BOOL CSmsWorker::AddMessage(char *szMessage, int nLength)
{
    if(nLength <= 0 || szMessage == NULL) return FALSE;

    m_Locker.Lock();
    m_pQueue->AddQ(szMessage, nLength);
    m_Locker.Unlock();

	return ActiveThread();
}

/** Queue에서 한줄을 읽는다.
 */
int CSmsWorker::ReadLine(char *pszBuffer, int nLength)
{
	int		c, nBytes = m_nBufferOffset;

    m_Locker.Lock();
	while(true)
	{
		c = m_pQueue->GetQ();
		if (c == -1)
		{
            pszBuffer[nBytes] = 0;
            break;
		}

		if ((c == 0xd) || (c == 0xa))
		{
			pszBuffer[nBytes] = 0;
            nBytes++;
            m_nBufferOffset = 0;
			break;
		}

		pszBuffer[nBytes] = c;
		nBytes++;
        m_nBufferOffset++;
		if (nBytes >= nLength)
        {
			pszBuffer[nLength-1] = 0;
            nBytes = nLength;
            m_nBufferOffset = 0;
			break;
        }
	}
    m_Locker.Unlock();

	return nBytes;
}

void CSmsWorker::EscapeMessage(const char *payload, int payloadLen, char *escapedPayload)
{
    int i, idx;

    for(i=0,idx=0; i<payloadLen; i++,idx++)
    {
        if(payload[i] == '\\')
        {
            i++;
        }
        else if(payload[i] == ',') // parameter separator
        {
            escapedPayload[idx] = 0x01; // 0x01 로 치환
            continue;
        }
        escapedPayload[idx] = payload[i];
    }
}

/** SMS Command 처리.
  */
void CSmsWorker::ProcessMessage(const char *sender, const char *date, const char *time, const char *message)
{
    if(!(m_sMobileConfig.moduleFlag & MOBILE_MODULE_FLAG_SMS_MONITOR_MASK)) return;  // SMS Monitoring이 setting 되지 않았을 경우

    if(sender == NULL || date == NULL || time == NULL || message == NULL)
    {
        MOBILE_LOG("Invalid Message : NULL {sender,date,time,message}\n");
        return;
    }

    //MOBILE_LOG(
    if(strncmp(message, "NSC", 3)==0)
    {
        // Command
        char key[3]={0,};
        char cmd[4]={0,};
        char len[3]={0,};
        char crc[5]={0,};
        char payload[128]={0,};
        char messageCopy[128]={0,};
        int msgLen = strlen(message);
        int payloadLen = 0;
        unsigned short nCrc, nGenCrc;

        char *p, *last=NULL;

        memset(messageCopy, 0, sizeof(messageCopy));

        memcpy(key, message + KEY_POS, 2);    // key
        memcpy(cmd, message + CMD_POS, 3);    // cmd
        memcpy(len, message + LEN_POS, 2);    // len

        payloadLen = (int)strtol(len, (char **)NULL, 10);

        if(msgLen != payloadLen + 14) // payloadlen + header(10) + tail(4)
        {
            MOBILE_LOG("Invalid Message Length : msglen %d, payload %d, header 10, tail 4\n", msgLen, payloadLen);
            return;
        }

        memcpy(crc, message + LEN_POS + 2 + payloadLen, 4);    // crc

        nCrc = (unsigned short)strtol(crc, (char **)NULL, 16);
        nGenCrc = BigToHostShort(GetCrc16((unsigned char *)message, 10 + payloadLen));

        if(nCrc != nGenCrc)
        {
            MOBILE_LOG("Invalid Message CRC : msg %s gen %04X\n", crc, nGenCrc);
            return;
        }
        EscapeMessage(message + LEN_POS + 2, payloadLen, payload);
        memcpy(messageCopy, message + LEN_POS + 2, payloadLen);
        MOBILE_LOG("SMS: %s %s %s cmd %s key %s %s\n", date, time, sender, cmd, key, messageCopy);

        // Do Action
        switch(cmd[0])
        {
            case 'E': // Event Group
                if(!strncmp(cmd+1, "01",2)) // DCU Install
                {
                    mcuInstallEvent();
                }
                else if(!strncmp(cmd+1, "02",2)) // DCU Startup
                {
                    mcuStartupEvent();
                }
                break;
            case 'F': // Fail Recover Group
                if(!strncmp(cmd+1, "01",2)) // Remove All Data
                {
                    SystemExec("rm -f /app/data/*; rm -f /app/log/*");
                    // log가 지워져서 다시 추가한다
                    MOBILE_LOG("SMS: %s %s %s cmd %s key %s %s\n", date, time, sender, cmd, key, messageCopy);
                }
                break;
            case 'M': // Metering Group
                if(!strncmp(cmd+1, "01",2)) // Metering
                {
                     m_pMetering->Query(-1, -1, -1);
                }
                else if(!strncmp(cmd+1, "02",2)) // Upload
                {
	                m_pMeterUploader->Upload();
                }
                else if(!strncmp(cmd+1, "03",2)) // Upload Date
                {
                    TIMESTAMP start, end;
                    memset(&start, 0, sizeof(TIMESTAMP));
                    memset(&end, 0, sizeof(TIMESTAMP));

                    if((p=strtok_r(payload, "\x01", &last)) != NULL)
                    {
                        start.year = end.year = (WORD)strtol(p, (char **)NULL, 10);
                        if((p=strtok_r(NULL, "\x01", &last)) != NULL)
                        {
                            start.mon = end.mon = (WORD)strtol(p, (char **)NULL, 10);
                            if((p=strtok_r(NULL, "\x01", &last)) != NULL)
                            {
                                start.day = end.day = (WORD)strtol(p, (char **)NULL, 10);
                                end.hour = 23; end.min = 59; end.sec = 59;
	                            m_pMeterUploader->Upload(&start, &end);
                                return;
                            }
                        }

                    }
                    MOBILE_LOG(" + Error : Invalid Parameter\n");
                }
                else if(!strncmp(cmd+1, "04",2)) // Refresh Network
                {
	                m_pSensorTimesync->Timesync(TIMESYNC_TYPE_LONG);
                }
                else if(!strncmp(cmd+1, "05",2)) // Meter Timesync
                {
                    EUI64 id;

                    if((p=strtok_r(payload, "\x01", &last)) != NULL)
                    {
	                    CMDPARAM	param;
	                    ENDIENTRY   *pEndi;
	                    CChunk		chunk(1024);
	                    METER_STAT	stat;

                        memset(&id, 0, sizeof(EUI64));
	                    memset(&param, 0, sizeof(CMDPARAM));
	                    memset(&stat, 0, sizeof(METER_STAT));

                        StrToEUI64(p, &id);
	                    pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);
	                    if (pEndi == NULL)
	                    {
		                    XDEBUG(" + Error : Invalid ID: %s\xd\xa", p);
                            return;
                        }

	                    memcpy(&param.id, &id, sizeof(EUI64));
                        param.codi      = GetCoordinator();
	                    param.nMeteringType = METERING_TYPE_SPECIAL;
	                    param.nOption	= ONDEMAND_OPTION_TIME_SYNC;
	                    param.pChunk	= &chunk;

	                    m_pOndemander->Command(pEndi->szParser, &param, &stat);
                        return;
                    }
                    MOBILE_LOG(" + Error : Invalid Parameter\n");
                }
                break;
#if     defined(__PATCH_12_969__)
            case 'N': // Network Group
                char script[1024];
                if(!strncmp(cmd+1, "01",2)) // Rebuild Reverse Connection
                {
                    char serveraddr[64];
                    int  serverport =  m_nSystemID + BASE_PORT_AGENT;
                    int  localport = m_nLocalPort;
                    int  sshport = 22;

                    strcpy(serveraddr, m_szFepServer);

                    if((p=strtok_r(payload, "\x01", &last)) != NULL)
                    {
                        strncpy(serveraddr, p, MIN(sizeof(serveraddr)-1, strlen(p)));
                        if((p=strtok_r(NULL, "\x01", &last)) != NULL)
                        {
                            serverport = (int)strtol(p, (char **)NULL, 10);
                            if((p=strtok_r(NULL, "\x01", &last)) != NULL)
                            {
                                localport = (int)strtol(p, (char **)NULL, 10);
                                if((p=strtok_r(NULL, "\x01", &last)) != NULL)
                                {
                                    sshport = (int)strtol(p, (char **)NULL, 10);
                                }
                            }
                        }

                    }

                    // Rebuild Reverse tunnel 
                    sprintf(script, "su -c \"%s -s %s -r %d -l %d -S %d -R\" nobody", REVERSE_TUNNEL_SCRIPT, 
                            serveraddr, serverport, localport, sshport);
                    XDEBUG("RUN Script : %s\r\n", script);
                    SystemExec(script);
                }
                else if(!strncmp(cmd+1, "02",2)) // Remove All Reverse Connection
                {
                    // Delete all Reverse tunnel 
                    sprintf(script, "su -c \"%s -D\" nobody", REVERSE_TUNNEL_SCRIPT); 
                    XDEBUG("RUN Script : %s\r\n", script);
                    SystemExec(script);
                }
                break;
#endif
            case 'R': // Reset Group
                if(!strncmp(cmd+1, "01",2)) // Reset system
                {
	                mcuResetEvent(4, sender);
	                m_pService->SetResetState(RST_COMMAND);
	                m_pService->PostMessage(GWMSG_RESET, NULL);
                }
                else if(!strncmp(cmd+1, "02",2)) // Reset Sub Network
                {
                    // Parameter를 검사하지 않고 ZB를 Reset 한다
                    CoordinatorPowerControl();
                }
                else if(!strncmp(cmd+1, "03",2)) // Reset WAN
                {
                    m_pSystemMonitor->PowerResetModem(FALSE);
                }
                break;
            case 'S': // Set Group
                if(!strncmp(cmd+1, "01",2)) // Set Value
                {
                    if((p=strtok_r(payload, "\x01", &last)) != NULL)
                    {
                        char *oidStr = p;
                        if((p=strtok_r(NULL, "\x01", &last)) != NULL)
                        {
                            int nType = (int)strtol(p, (char **)NULL, 10);

                            if((p=strtok_r(NULL, "\x01", &last)) != NULL)
                            {
                                CIF4Invoke  invoke("127.0.0.1", m_nLocalPort);

                                switch(nType)
                                {
                                    case VARSMI_CHAR :
                                        invoke.AddParam(oidStr, (signed char)strtol(p, (char**)NULL, 10));
                                        break;
                                    case VARSMI_BOOL :
                                        invoke.AddParam(oidStr, (BOOL)strtol(p, (char**)NULL, 10));
                                        break;
                                    case VARSMI_BYTE :
                                        invoke.AddParam(oidStr, (BYTE)strtol(p, (char**)NULL, 10));
                                        break;
                                    case VARSMI_SHORT :
                                        invoke.AddParam(oidStr, (signed short)strtol(p, (char**)NULL, 10));
                                        break;
                                    case VARSMI_WORD :
                                        invoke.AddParam(oidStr, (WORD)strtol(p, (char**)NULL, 10));
                                        break;
                                    case VARSMI_INT :
                                        invoke.AddParam(oidStr, (INT)strtol(p, (char**)NULL, 10));
                                        break;
                                    case VARSMI_UINT :
                                        invoke.AddParam(oidStr, (UINT)strtol(p, (char**)NULL, 10));
                                        break;
                                    case VARSMI_OID :
                                        {
                                            OID3 oid;
                                            VARAPI_StringToOid(p, &oid);
                                            invoke.AddParam(oidStr, &oid);
                                        }
                                        break;
                                    case VARSMI_STRING :
                                        invoke.AddParamFormat(oidStr, nType, p, strlen(p));
                                        break;
                                    case VARSMI_EUI64 :
                                        {
                                            EUI64 id;
                                            StrToEUI64(p, &id);
                                            invoke.AddParamFormat(oidStr, nType, &id, sizeof(EUI64));
                                        }
                                        break;
                                    case VARSMI_IPADDR :
                                    case VARSMI_TIMESTAMP :
                                    case VARSMI_TIMEDATE :
                                    case VARSMI_STREAM :
                                    case VARSMI_OPAQUE :
                                    default:
                                        MOBILE_LOG(" + Error : Not Supported Type %d\n", nType);
                                        return;
                                }

	                            IF4API_Command(invoke.GetHandle(), "199.2", IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE);
                                return;
                            }
                         }
                    }
                    MOBILE_LOG(" + Error : Invalid Parameter\n");
                }
                else if(!strncmp(cmd+1, "11",2)) // FEP Address Port
                {
                    CIF4Invoke  invoke("127.0.0.1", m_nLocalPort);
                    if((p=strtok_r(payload, "\x01", &last)) != NULL)
                    {
                        invoke.AddParam("2.1.20", p); // server address
                        if((p=strtok_r(NULL, "\x01", &last)) != NULL)
                        {
                            invoke.AddParam("2.1.21", (UINT)strtol(p,(char**)NULL,10)); // server port
                            if((p=strtok_r(NULL, "\x01", &last)) != NULL)
                            {
                                invoke.AddParam("2.1.22", (UINT)strtol(p,(char**)NULL,10)); // alarm port
	                            IF4API_Command(invoke.GetHandle(), "199.2", IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE);
                                return;
                            }
                         }
                    }
                    MOBILE_LOG(" + Error : Invalid Parameter\n");
                }
                break;
        }
    }
    else
    {
        MOBILE_LOG("Unknown Msg Recv : %s %s %s %s\n", date, time, sender, message);
    }
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CSmsWorker::OnActiveThread()
{
    if(!(m_sMobileConfig.moduleFlag & MOBILE_MODULE_FLAG_SMS_MONITOR_MASK)) return TRUE;  // SMS Monitoring이 setting 되지 않았을 경우

    int nBufferLen = sizeof(m_szBuffer);
    int nRead = 0; 
    
    do 
    {
        nRead = ReadLine(m_szBuffer, nBufferLen); // null 까지 count에 포함된다

        if(nRead > 1 && m_nBufferOffset == 0)
        {
            if(strncmp(m_szBuffer, "+CMT:",5)==0) 
            {
                m_bSmsNotification = TRUE;
            } 
            else if(strncmp(m_szBuffer, "+CMGL:",6)==0) 
            {
                m_bSmsNotification = TRUE;
            }
            else if(strncmp(m_szBuffer, "+CMTI:",6)==0) 
            {
                m_pMobileClient->WriteToModem("AT+CMGL=0\r\n",11);
            }
            else if(m_bSmsNotification)
            {
                m_bSmsNotification = FALSE;
                XDEBUG("  READ SMS: %d Bytes\r\n", nRead - 1);
                {
                    PDU pdu(m_szBuffer);
                    if(!pdu.parse())
                    {
                        XDEBUG(ANSI_COLOR_RED "PDU parsing failed> Error: %s\r\n" ANSI_NORMAL, pdu.getError());
                    }
                    else
                    {
                        XDEBUG("       PDU: %s\r\n" , pdu.getPDU());
                        XDEBUG("      SMSC: %s\r\n" , pdu.getSMSC());
                        XDEBUG("    Sender: %s\r\n" , pdu.getNumber());
                        XDEBUG("   No Type: %s\r\n" , pdu.getNumberType());
                        XDEBUG("      Date: %s\r\n" , pdu.getDate());
                        XDEBUG("      Time: %s\r\n" , pdu.getTime());
                        XDEBUG("  UDH Type: %s\r\n" , pdu.getUDHType());
                        XDEBUG("  UDH Data: %s\r\n" , pdu.getUDHData());
                        XDEBUG("   Message: %s\r\n" , pdu.getMessage());
                        ProcessMessage(pdu.getNumber(), pdu.getDate(), pdu.getTime(), pdu.getMessage());
                    }
                }
            }
        }
    } while(nRead > 0 && m_nBufferOffset == 0);

	return TRUE;
}

BOOL CSmsWorker::OnTimeoutThread()
{
    if(!(m_sMobileConfig.moduleFlag & MOBILE_MODULE_FLAG_SMS_MONITOR_MASK)) return TRUE;  // SMS Monitoring이 setting 되지 않았을 경우
    if(!strcasecmp(m_sMobileConfig.moduleType, MOBILE_MODULE_TYPE_AME5210))
    {
        m_pMobileClient->WriteToModem("AT+CMGL=0\r\n",11);
    }
	return OnActiveThread();
}

#endif  // __TI_AM335X__
#endif
