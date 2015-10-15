#include "common.h"
#include "MobileClient.h"
#include "AgentService.h"
#include "SystemMonitor.h"
#include "RealTimeClock.h"
#include "StatusMonitor.h"
#include "SystemUtil.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "Utility.h"
#include "ShellCommand.h"

//////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////

typedef enum {
    M_INIT, M_SIMINIT, M_EXTRAINIT,
    M_MANF, M_MODEL, M_REV, M_SERIAL,
    M_NUMBER, M_READANSW, M_SETANSW, M_STORE,
    M_IMSI, M_SIM, M_PIN, M_NET, M_CSQ,
    M_NONE
} MOBILE_STATE;

typedef struct {
    MOBILE_STATE    nState;
    const char *    pState;
    char *          pCommand;
} MOBILE_STATE_MAP;

MOBILE_STATE_MAP m_state_map [] = {
    { M_INIT,           "INIT",         NULL},
    { M_SIMINIT,        "SIMINIT",      NULL},
    { M_EXTRAINIT,      "EXTRAINIT",    NULL},
    { M_MANF,           "MANF",         NULL},
    { M_MODEL,          "MODEL",        NULL},
    { M_REV,            "REV",          NULL},
    { M_SERIAL,         "SERIAL",       NULL},
    { M_NUMBER,         "NUMBER",       NULL},
    { M_READANSW,       "READANSW",     NULL},
    { M_SETANSW,        "SETANSW",      NULL},
    { M_STORE,          "STORE",        NULL},
    { M_IMSI,           "IMSI",         NULL},
    { M_SIM,            "SIM",          NULL},
    { M_PIN,            "PIN",          NULL},
    { M_NET,            "NET",          NULL},
    { M_CSQ,            "CSQ",          NULL},
    { M_NONE,           NULL,           NULL}
};

#define COMPRESS_OFF			0
#define COMPRESS_ON				1


extern int      m_nSystemID;
extern char 	m_szFepServer[];
extern int	 	m_nFepPort;
extern BOOL    	m_bEnableAutoUpload;
extern UINT		m_nUploadTryTime;
extern int		m_nUploadLogSize;
extern int	    m_nMobileCSQ;
//extern MOBILECFGENTRY   m_sMobileConfig;

CLocker			m_BusyLocker;
CLocker			m_RingLocker;
BOOL			m_bBusyMobile = FALSE;
BOOL			m_bRingSignalPending = FALSE;


//////////////////////////////////////////////////////////////////////
// Utiliti functions
//////////////////////////////////////////////////////////////////////

void _init_state_map()
{
    for(int i=0; m_state_map[i].nState != M_NONE; i++)
    {
        if(m_state_map[i].pCommand != NULL)
        {
            FREE(m_state_map[i].pCommand); 
            m_state_map[i].pCommand = NULL;
        }
    }
}

void _update_state_map(const char* pState, const char* pCommand)
{
    if(pState == NULL || pCommand == NULL) return;
    for(int i=0; m_state_map[i].nState != M_NONE; i++)
    {
        if(!strcasecmp(m_state_map[i].pState, pState))
        {
            m_state_map[i].pCommand = (char*)MALLOC(strlen(pCommand)+1);
            strcpy(m_state_map[i].pCommand, pCommand);
            break;
        }
    }
}

void MOBILE_LoadState()
{
    _init_state_map();
    if("MOBILE_TYPE_GSM" == NULL || strlen("MOBILE_TYPE_GSM")<=0) return;

    char path[256];
    char sState[64], sAtCmd[128];
    FILE * fp;

    sprintf(path, "/app/sw/ppp/at/%s.at", MOBILE_MODULE_TYPE_GE910);
    if((fp=fopen(path,"r")) != NULL)
    {
        XDEBUG("Load Module AT Commands\r\n");
        while(fscanf(fp,"%s %s", sState, sAtCmd) != EOF)
        {
            XDEBUG("[%s, %s]\r\n", sState, sAtCmd);
            _update_state_map(sState, sAtCmd);
        }
        fclose(fp);
    }
}

void MOBILE_Initialize()
{
    MOBILE_LoadState();
}

BOOL MOBILE_IsBusy()
{
	return m_bBusyMobile;
}

BOOL MOBILE_Lock()
{
	m_BusyLocker.Lock();	
	m_bBusyMobile = TRUE;
	return TRUE;	
}

void MOBILE_Unlock()
{
	m_bBusyMobile = FALSE;
	m_BusyLocker.Unlock();	
}

void MOBILE_Ring()
{
	m_RingLocker.Lock();	
	m_bRingSignalPending = TRUE;
	m_RingLocker.Unlock();
}

void StoreValue(const char* pState, char *pszString)
{
    FILE    *fp;
    char    szBuffer[128];
    char    *printPtr = pszString;

    if(pszString == NULL || pszString[0] == 0x00) return;
    if(pszString[0] == '+')
    {
        printPtr = strchr(pszString, ' ');
        if(printPtr != NULL)
        {
            printPtr ++; // space skip
        }
        else // store 할 값이 없을 경우
        {
            return;
        }
    }

    sprintf(szBuffer, "%s %s \n", pState, printPtr);
    fp = fopen("/tmp/mobileinfo", "a+b");
    if (fp != NULL)
    {
        fwrite(szBuffer, 1, strlen(szBuffer), fp);
        fclose(fp);
    }
}

int GetModemProperty(CMobileClient *pClient)
{
    char    szBuffer[256]={0,};
    int     nLength;
    char    phoneNum[128]={0,};
    int     phoneNumLen = 0;
    MIBValue mibValue;

    // Read Buffer 초기화
    pClient->Flush();

	m_RingLocker.Lock();	
	m_bRingSignalPending = FALSE;
	m_RingLocker.Unlock();

    unlink("/tmp/mobileinfo");
    for(int i=0; m_state_map[i].nState != M_NONE; i++)
    {
		if (m_bRingSignalPending) {
            return 0;
        }

        if(m_state_map[i].pCommand == NULL) continue;

        XDEBUG("Mobile get property %s : %s\r\n", m_state_map[i].pState, m_state_map[i].pCommand);

        sprintf(szBuffer,"%s\r\n", m_state_map[i].pCommand);
        pClient->WriteToModem(szBuffer);

        for(nLength=1; nLength!=0;)
        {
            nLength = pClient->ReadLineFromModem(szBuffer, sizeof(szBuffer)-2, 1000);
            if (nLength <= 0)
                break;

			if (m_bRingSignalPending) 
            {
                return 0;
            }

            if (nLength == 1) 
                continue;

			szBuffer[nLength] = '\0';

            XDEBUG("Answer %s\r\n", szBuffer);

            if(strncasecmp(szBuffer,"OK",2) == 0) continue;

            ////// Phone Number Check

            /* CDMA 방식 */
			if (strncmp(szBuffer, "+MIN: ", 6) == 0)
			{
                if(strlen(szBuffer)>6)
                {
                    sprintf(phoneNum,"%s",&szBuffer[6]);
                    phoneNumLen = strlen(phoneNum);
                }
			}
            /* GSM 방식 */
            else if (strncmp(szBuffer, "+CNUM: ", 7) == 0)
            {
                char *tok[3] = { NULL, NULL, NULL };
                char *buf = NULL;
                char *last = NULL;
                char *dupStr;
                char *tmpPhoneNum=NULL;
                char *stripPhoneNum=NULL;
                UINT i;
                
                dupStr = strdup(szBuffer);
                /** AT+CNUM Specification
                  *
                  * [+CNUM: [<alpha>], <number>, <type>]
                  * 
                  * Number를 추출해야 한다.
                  */
                buf = &dupStr[6];
                for(i=0;i<sizeof(tok);i++)
                {
                    if((tok[i] = strtok_r(buf, ",", &last))==NULL) break;
                    buf = NULL;
                }
                if(i>1)
                {
                    char * ptr;
                    /** alpha가 optional 하기 때문에 위치를 구해야 한다 */
                    tmpPhoneNum = tok[1];
                    while((ptr = strchr(tmpPhoneNum, '"')) != NULL)
                    {
                        ptr[0] = ' ';
                    }
                    stripPhoneNum = strip(tmpPhoneNum);
                    if(stripPhoneNum != NULL)
                    {
                        sprintf(phoneNum,"%s",stripPhoneNum);
                        phoneNumLen = strlen(phoneNum);
                        FREE(stripPhoneNum); // free duplicate string 
                    }

                }
                free(dupStr); // free duplicate string 
            }
            else if (strncmp(szBuffer, "+CSQ: ", 6) == 0)
			{
				m_nMobileCSQ = (int)strtol(&szBuffer[6], (char**)NULL, 10);
			}

            ///// SET Values
            if(phoneNumLen > 0) 
            {
                memset(&mibValue, 0, sizeof(MIBValue));
                /** OID 2.1.10 sysPhoneNumber */
                if(VARAPI_StringToOid("2.1.10", &mibValue.oid))
                {
                    mibValue.stream.p = (char *)MALLOC(phoneNumLen + 1);
                    memset(mibValue.stream.p, 0, phoneNumLen + 1);
                    memcpy(mibValue.stream.p, phoneNum, phoneNumLen);
                    mibValue.type = VARSMI_STRING;
                    mibValue.len = phoneNumLen;

                    VARAPI_SetValue(&mibValue, NULL);
                    FREE(mibValue.stream.p);
                }
                else
                {
                    XDEBUG("GetModemProperty : OID Converting Fail : 2.1.10\r\n");
                }
            }

            StoreValue(m_state_map[i].pState, szBuffer);
        }
    }

    if (IsExists("/tmp/mobileinfo"))
    {
        SystemExec("cp -f /tmp/mobileinfo /app/conf/mobileinfo");
        return 1;
    }

    return 0;
}

BOOL MOBILE_GetMobileProperty()
{
	BOOL			bResult = FALSE;

#if     !defined(__TI_AM335X__)    // Issue #959
	if (m_pStatusMonitor->GetDcdState() == 0)
		return FALSE;
#endif

	CMobileClient	client;
	if (client.Initialize(2)) //sy
    {
        bResult = GetModemProperty(&client);
    }
#if 0
    m_pMobileClient->Initialize();
    bResult = GetModemProperty(m_pMobileClient);
    m_pMobileClient->Destroy();
#endif

	return bResult;	
}

BOOL UploadFile(const char *pszFileName, int nSeek)
{
    char    szCommand[128];
    int     nMode, nType;
    BOOL    bConnected;
	BOOL	bResult = FALSE;
	int		nResult;

	if (!IsExists(pszFileName))
        return FALSE;

    nType = m_pService->GetEthernetType();
	if (nType != ETHER_TYPE_PPP)
	{
	    sprintf(szCommand, "/app/sw/upload %s -n %d -p %d %s %s", 
			m_nDebugLevel ? "" : "-d", m_nSystemID, m_nFepPort, m_szFepServer, pszFileName);	
        nResult = SystemExec(szCommand);
		return (nResult == 0) ? TRUE : FALSE;
    }

    nMode   = m_pService->GetMobileMode();

	MOBILE_Lock();
#if !defined(__TI_AM335X__)    // Issue #959
    {
    BOOL    bFind = FALSE;
    /** Issue #959 : TI AM3352 Porting
     * TI에 실장된 AMTelecom AME5210 Module의 경우 별도의 PPP 없이 자체적으로 IP를 할당받아서
     * Interface를 생성해 줄 수 있다. 따라서 mserver 를 사용하지 않는다
     */
	for(;;)
	{
		// 전화가 걸려 있는 상태인지 본다.
		if ((nMode == MOBILE_MODE_CSD) && (m_pStatusMonitor->GetDcdState() == 0))
		{
			bFind = TRUE;
			XDEBUG("SEND-PENDING: DCD active. (WAIT)\r\n");
			usleep(5000000);
			continue;
		}
		else if (FindProcess("mserver") > 0)
		{
			bFind = TRUE;
			XDEBUG("SEND-PENDING: mserver active. (WAIT)\r\n");
			usleep(5000000);
			continue;
		}
		else
		{
			if (bFind) usleep(3000000);
			break;
		}
	}
    }
#endif

	if (nMode != MOBILE_MODE_CSD)
	{
		bConnected = m_pSystemMonitor->NewConnection();
		XDEBUG("MOBILE Status: %s\r\n", bConnected ? "Mobile Connected." : "Mobile Not connected");

		if (bConnected)
		{
	        sprintf(szCommand, "/app/sw/upload %s -n %d -p %d %s %s", 
			    m_nDebugLevel ? "" : "-d", m_nSystemID, m_nFepPort, m_szFepServer, pszFileName);	
			nResult = SystemExec(szCommand);
			bResult = (nResult == 0) ? TRUE : FALSE;
		}
		else
		{
			XDEBUG("MOBILE-CONNECTION-ERROR: Cannot connect mobile network.\r\n");
		}

		m_pSystemMonitor->DeleteConnection();
	}
	else
	{
		sprintf(szCommand, "/app/sw/send \"%s\" %d %d %s %d %d %d",
		    MOBILE_MODULE_TYPE_GE910,
			m_nSystemID, m_nFepPort, pszFileName, m_nUploadTryTime,
			COMPRESS_ON,
			(m_nDebugLevel == 0) ? 1 : 0);
		nResult = SystemExec(szCommand);
		bResult = (nResult == 0) ? TRUE : FALSE;
	}
	MOBILE_Unlock();

	return bResult;
}

