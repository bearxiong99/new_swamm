/////////////////////////////////////////////////////////////////////
//
//  mbus.cpp: implementation of the CMbusParser class.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "Chunk.h"
#include "EndDeviceList.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"
#include "MeterDeviceList.h"

#include "mbusFunctions.h"
#include "mbusFrame.h"

#include "ParserMbus.h"
#include "Utility.h"

//////////////////////////////////////////////////////////////////////
// CMbusParser Class
//////////////////////////////////////////////////////////////////////

#define MBUS_SLAVE_TYPE_KAMSTRUP        0
#define MBUS_SLAVE_TYPE_LANDISGYR       1

#if     defined(__PATCH_4_1647__)
extern BOOL    *m_pbPatch_4_1647;
#endif

#ifdef _WIN32
#pragma pack(push, 1)
#endif

typedef struct _tagMBUSINFO
{
    BOOL        bInstall;
    BYTE        nPort;
    union
    {
        BYTE        nLpPoint;
        struct {
            BYTE        nType;
            BYTE        nLpPoint;
        } tl;
    } lp;
} __ATTRIBUTE_PACKED__ MBUSINFO;

#ifdef _WIN32
#pragma pack(pop)
#endif

WORD m_MbusRomMapAddr[3] = {
    MBUS1_ENABLE,
    MBUS2_ENABLE,
    MBUS3_ENABLE
};

WORD m_MbusBaseAddr[3] = {
    MBUS1_LPLOGDATA,
    MBUS2_LPLOGDATA,
    MBUS3_LPLOGDATA
};

static const char *parserNameArray[] = {"MBUS", "NAMR-H101MG", NULL};

//////////////////////////////////////////////////////////////////////
// CMbusParser Class
//////////////////////////////////////////////////////////////////////

CMbusParser::CMbusParser():CMeterParser(SENSOR_TYPE_ZEUMBUS, SERVICE_TYPE_HEAT, 
                                    ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_MBUS_MASTER | ENDDEVICE_ATTR_POLL, 
									const_cast<char **>(parserNameArray), PARSER_TYPE_MBUS, "MBUS Parser", "MBUS", parserNameArray[0],
                                    6000, 3*6000, 2)
{
}

CMbusParser::~CMbusParser()
{
}

//////////////////////////////////////////////////////////////////////
// CI210Parser Operations
//////////////////////////////////////////////////////////////////////

int CMbusParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{

    switch(pCommand->nType) {
      default:
           return MBUS_MAIN(pCommand, pStat);

    }
    return 0;
}

int CMbusParser::MBUS_MAIN(CMDPARAM *pCommand, METER_STAT *pStat)
{
    char                szGUID[20];
    EUI64               id;
    ONDEMANDPARAM       *pOndemand = NULL;
    int                 nRetry, nState;
    int                 nSaveState;
    int                 nResult, nError;
    int                 nOption=0, nOffset=0, nCount=0;
    time_t              tmStart, tmEnd, tmLast=0, tmLastEvent=0;
    CChunk              *pChunk;
    TIMESTAMP           tmLastMetering;
    TIMESTAMP           tmLastEventLog;
    BOOL                bConnected = FALSE;
    
    ENDI_NODE_INFO      *nodeInfo;
    AMR_PARTIAL_INFO    *amrInfo;
    BYTE                szBuffer[4096];
    BYTE                permitMode=0, permitState=0, alarmFlag=0, testFlag=0;
    BYTE                hwVersion=0, fwVersion=0, fwBuild=0;
    WORD                subFwVersion=0, subFwBuild=0;
    WORD                alarmMask=0;
    WORD                addr;
    int                 len, i, n, nSize, nSeek=0, nDay;
    unsigned long       nElapse=0;

    BYTE                mbusLength;
    char                szMeterId[20+1], szModel[18+1];
    int                 nFailCount = 0;
    BYTE                nComPort = 0;

    BYTE                nMbusIdx = 0;
    MBUSINFO            pMbusInfos[3];
    MBUSINFO            *pMbusInfo = NULL;
    BOOL                bNewRomMap = FALSE;         // MBUS H101 v21 이후로 새로운 ROMAMP 사용
    char                *pszParam = NULL;
    BYTE                nMeterType;
    BYTE                nLpPoint;
    int                 nMeteringType;
    BYTE                nPort=0, nAction=0, nCommand=0, nDataLen=0;
    BYTE                pBypassData[4096];
    BYTE                szReq[64], nReqLen;
    MIBValue 	        *argv;						// Function parameter array
    int			        argc;						// Function parameter count


    memcpy(&id, &pCommand->id, sizeof(EUI64));      // Sensor ID
    pChunk          = pCommand->pChunk;                 // 검침 데이터를 리턴할 Chunk
    nMeteringType   = pCommand->nMeteringType;
    if(nMeteringType == METERING_TYPE_BYPASS)
    {
        argc        = pCommand->argc;
        argv        = pCommand->argv;
        nPort       = argv[1].stream.u8;
        nAction     = argv[2].stream.u8;
        nCommand    = argv[3].stream.u8;
        memset(pBypassData, 0, sizeof(pBypassData));
        if(argc>4)
        {
            nDataLen = MIN(argv[4].len, (int)sizeof(pBypassData));
            memcpy(pBypassData, argv[4].stream.p, nDataLen);
        }
    }
    else
    {
        nOption     = pCommand->nOption;                // Function
        nOffset     = pCommand->nOffset;                // Read interval of meter data
        nCount      = pCommand->nCount;                 // LP Count
        pszParam    = pCommand->pszParam;

        if(nOffset > 9) nOffset = 9;
        if(nOffset < 0) nOffset = 1;
        if(nCount < 1) nCount = 1;
        if(nCount > nOffset + 1 ) nCount = nOffset + 1;

        if(nOption < 0) nOption = 0;
        if(nOption > 0) nComPort = (BYTE) nOption;
    }

    /** MBUS 관련 Info Structure 초기화 */
    memset(pMbusInfos, 0, sizeof(pMbusInfos));

    nState = STATE_OPEN;
    if (codiGetState(pCommand->codi) != CODISTATE_NORMAL)
        return ONDEMAND_ERROR_NOT_CONNECT;

    nResult = ONDEMAND_ERROR_NOT_CONNECT;
    
    nSaveState = m_pEndDeviceList->GetState(&id);
    if (nSaveState == ENDISTATE_INIT)
        return ONDEMAND_ERROR_INIT;

    if (m_pEndDeviceList->IsBusy(&id))
        return ONDEMAND_ERROR_BUSY;
        
    eui64toa(&id, szGUID);
    m_pEndDeviceList->SetBusy(&id, TRUE);

    if(nMeteringType == METERING_TYPE_BYPASS)
    {
        XDEBUG(" MBUS: Port=%d, Action=%d, Command=%d, Param=\r\n", nPort, nAction, nCommand);
        XDUMP((const char *)pBypassData, nDataLen, TRUE);
    }
    else
    {
        // 마지막 검침 시간을 구한다.
        m_pEndDeviceList->GetLastMeteringTime(&id, &tmLast);
        GetTimestamp(&tmLastMetering, &tmLast);    

        // 마지막 이벤트 로그 시간을 구한다.
        m_pEndDeviceList->GetLastEventLogTime(&id, &tmLastEvent);
        GetTimestamp(&tmLastEventLog, &tmLastEvent);    

        memset(szMeterId, 0, sizeof(szMeterId));
        memset(szModel, 0, sizeof(szModel));

        XDEBUG(" MBUS: Option=%d, Offset=%d, Count=%d\r\n",
                nOption, nOffset, nCount);

        XDEBUG("    LAST METERING = %04d/%02d/%02d %02d:%02d:%02d, LAST EVENT = %04d/%02d/%02d %02d:%02d:%02d\r\n",
                tmLastMetering.year, tmLastMetering.mon, tmLastMetering.day,
                tmLastMetering.hour, tmLastMetering.min, tmLastMetering.sec,
                tmLastEventLog.year, tmLastEventLog.mon, tmLastEventLog.day,
                tmLastEventLog.hour, tmLastEventLog.min, tmLastEventLog.sec);
    }

    for(nRetry=0; (nRetry<GetRetryCount()) && (nState!=STATE_DONE);)
    {
        switch(nState) {
            case STATE_OPEN :
                time(&tmStart);
                m_Locker.Lock();
                pOndemand = FindOperation(&id);
                if (pOndemand != NULL)
                {
                    m_Locker.Unlock();
	                m_pEndDeviceList->SetBusy(&id, FALSE);
                    return ONDEMAND_ERROR_BUSY;
                } 

                pOndemand = NewOperation(ONDEMAND_OPR_DATA, &id, 2048);
                pOndemand->nMeteringType = pCommand->nMeteringType;
                pOndemand->nOffset = nOffset;
                pOndemand->nCount = nCount;
                pOndemand->pParser = (void *)this;
                memcpy(&pOndemand->tmLastMetering, &tmLastMetering, sizeof(TIMESTAMP));
                memcpy(&pOndemand->tmLastEventLog, &tmLastEventLog, sizeof(TIMESTAMP));
                pOndemand->nVendor = METER_VENDOR_UNKNOWN;
                m_Locker.Unlock();

                gettimeofday((struct timeval *)&pOndemand->tmStart, NULL);

                bConnected = FALSE;
                nError = endiOpen(pCommand->codi, (HANDLE *)&pOndemand->endi, &id);
                if (nError != CODIERR_NOERROR)
                {
                    XDEBUG(" endiOpen Error(%d): %s\r\n", nError, codiErrorMessage(nError));
                    nState = STATE_RETRY;
                    break;
                }

                // SEQ 검사 옵션을 켠다. 
                endiSetOption(pOndemand->endi, ENDIOPT_SEQUENCE, 1);

                nError = sensorConnect(pOndemand->endi, GetConnectionTimeout(ENDDEVICE_ATTR_POWERNODE));
                if (nError != CODIERR_NOERROR)
                {
                    XDEBUG(" endiConnect Error(%d): %s\r\n", nError, codiErrorMessage(nError));
                    nState = STATE_RETRY;
                    break;
                }

                bConnected = TRUE;
                pChunk->Rollback();

                gettimeofday((struct timeval *)&pOndemand->tmConnect, NULL);
                gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);

                if(nMeteringType == METERING_TYPE_BYPASS)
                {
                    nState = STATE_INITIALIZE;
                }
                else
                {
                    nState = STATE_READ_NODEINFO;
                }
                break;
        
            case STATE_READ_NODEINFO :                         // NODE_INFO를 읽어 본다.
                addr = ENDI_ROMAP_NODE_KIND;
                len  = sizeof(ENDI_NODE_INFO);

                pOndemand->stat.nCount ++;
                pOndemand->stat.nSendCount ++;
                pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
                gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
                nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &n, GetReplyTimeout());
                if (nError != CODIERR_NOERROR)
                {
                    if (pOndemand->nRetry < GetRetryCount())                // 3회 이상 연속 실패시 종료
                    {
                        pOndemand->nRetry++;
                        nState = STATE_READ_NODEINFO;
                    }else nState = STATE_OK;   
                    pOndemand->stat.nError ++;
                    break;
                }

                pOndemand->nRetry = 0;
                pOndemand->stat.nCount ++;
                pOndemand->stat.nRecvCount ++;
                pOndemand->stat.nRecv += n + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
                UpdateStatistics(pOndemand, (BYTE *)szBuffer, n);

                nodeInfo = (ENDI_NODE_INFO *) szBuffer;

                fwVersion = nodeInfo->FIRMWARE_VERSION;
                fwBuild = nodeInfo->FIRMWARE_BUILD;
                hwVersion = nodeInfo->HARDWARE_VERSION;

                nState = STATE_READ_SUBINFO;
                break;     

            case STATE_READ_SUBINFO :                         // SUB_INFO를 읽어 본다.
                addr = ENDI_ROMAP_SUB_FW_VERSION;
                len  = 2;

                pOndemand->stat.nCount ++;
                pOndemand->stat.nSendCount ++;
                pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
                gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
                nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &n, GetReplyTimeout());
                if (nError != CODIERR_NOERROR)
                {
                    if (pOndemand->nRetry < GetRetryCount())                // 3회 이상 연속 실패시 종료
                    {
                        pOndemand->nRetry++;
                        nState = STATE_READ_NODEINFO;
                    }else nState = STATE_OK;   
                    pOndemand->stat.nError ++;
                    break;
                }

                pOndemand->nRetry = 0;
                pOndemand->stat.nCount ++;
                pOndemand->stat.nRecvCount ++;
                pOndemand->stat.nRecv += n + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
                UpdateStatistics(pOndemand, (BYTE *)szBuffer, n);

                nodeInfo = (ENDI_NODE_INFO *) szBuffer;

                subFwVersion = ((szBuffer[0] >> 4) << 8) | (szBuffer[0] & 0xf);
                subFwBuild = ((szBuffer[1]>> 4) * 10) + (szBuffer[1] & 0xf);

                /** ARM Core의 Version 정보를 비교해서 2.0 이상일 때는 신규 F/W로 인식한다 */
                if(subFwVersion > 0x0200)
                {
                    bNewRomMap = TRUE;
                }

                nState = STATE_READ_AMRINFO;
                break;     

            case STATE_READ_AMRINFO :                         // AMR_INFO를 읽어 본다.
                addr = ENDI_ROMAP_TEST_FLAG;
                len  = sizeof(AMR_PARTIAL_INFO);

                pOndemand->stat.nCount ++;
                pOndemand->stat.nSendCount ++;
                pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
                gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
                nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &n, GetReplyTimeout());
                if (nError != CODIERR_NOERROR)
                {
                    if (pOndemand->nRetry < GetRetryCount())                // 3회 이상 연속 실패시 종료
                    {
                        pOndemand->nRetry++;
                        nState = STATE_READ_AMRINFO;
                    }else nState = STATE_OK;   
                    pOndemand->stat.nError ++;
                    break;
                }

                pOndemand->nRetry = 0;
                pOndemand->stat.nCount ++;
                pOndemand->stat.nRecvCount ++;
                pOndemand->stat.nRecv += n + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
                UpdateStatistics(pOndemand, (BYTE *)szBuffer, n);

                amrInfo = (AMR_PARTIAL_INFO *) szBuffer;

                permitMode = amrInfo->permit_mode;
                permitState = amrInfo->permit_state;
                alarmMask = amrInfo->alarm_mask;
                testFlag = amrInfo->test_flag;

                nState = STATE_MBUS_LPPOINT;
                break;     

            case STATE_MBUS_LPPOINT :                         // LP POINTER 관련 정보를 얻어온다.
                if(nMbusIdx > 2) 
                {
                    /** Meter 구조를 검사해서 Comport/MeterSerial이 존재하는 Meter는 삭제한다
                     */
                    m_pMeterDeviceList->SetRemove(&id);
                    for(i=0;i<3;i++)
                    {
                        if(pMbusInfos[i].bInstall) m_pMeterDeviceList->ResetRemove(&id, pMbusInfos[i].nPort);
                    }
                    m_pMeterDeviceList->Clear(&id);

                    if(nMeteringType == METERING_TYPE_ROUTINE)
                    {
                        nState = STATE_OK;
                    }
                    else
                    {
                        nState = STATE_MBUS_LPDATA;
                        nSeek = 0;
                        nMbusIdx = 0;

                        if(nComPort) 
                        {
                            nMbusIdx = 3;
                            for(i=0;i<3;i++)
                            {
                                if(nComPort == pMbusInfos[i].nPort)
                                {
                                    nMbusIdx = i;
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
                addr = m_MbusRomMapAddr[nMbusIdx];
                /** Modem 버전에 따라 다른 Size로 읽어야 한다 */
                len  = sizeof(MBUSINFO);

                pOndemand->stat.nCount ++;
                pOndemand->stat.nSendCount ++;
                pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
                gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
                nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &n, GetReplyTimeout());
                if (nError != CODIERR_NOERROR)
                {
                    if (pOndemand->nRetry < GetRetryCount())                // 3회 이상 연속 실패시 종료
                    {
                        pOndemand->nRetry++;
                        nState = STATE_MBUS_LPPOINT;
                    }else nState = STATE_OK;   
                    pOndemand->stat.nError ++;
                    break;
                }

                pMbusInfo = (MBUSINFO *) szBuffer;
                memcpy(&pMbusInfos[nMbusIdx], pMbusInfo, sizeof(MBUSINFO));
                if(bNewRomMap)
                {
                    XDEBUG(" MBUSN[%d] install=%s port=%d type=%d pointer=0x%04X\r\n",
                            nMbusIdx,
                            pMbusInfos[nMbusIdx].bInstall ? "true" : "false",
                            pMbusInfos[nMbusIdx].nPort,
                            pMbusInfos[nMbusIdx].lp.tl.nType,
                            pMbusInfos[nMbusIdx].lp.tl.nLpPoint);
                }
                else
                {
                    XDEBUG(" MBUSO[%d] install=%s port=%d pointer=0x%04X\r\n",
                            nMbusIdx,
                            pMbusInfos[nMbusIdx].bInstall ? "true" : "false",
                            pMbusInfos[nMbusIdx].nPort,
                            pMbusInfos[nMbusIdx].lp.nLpPoint);
                }
                nMbusIdx ++;
                break;     

            case STATE_MBUS_LPDATA:
                if(nMbusIdx > 2) 
                {
                    nState = STATE_OK;
                    pOndemand->nResult = ONDEMAND_ERROR_OK;
                    break;
                }
                pMbusInfo = &pMbusInfos[nMbusIdx];
                
                if(!pMbusInfo->bInstall)
                {
                    if(nComPort)
                    {
                        nState = STATE_ERROR;
                    }
                    else
                    {
                        nSeek = 0;
                        nMbusIdx++;
                    }
                    break;
                }

                if(bNewRomMap)
                {
                    nMeterType = pMbusInfo->lp.tl.nType;
                    nLpPoint = pMbusInfo->lp.tl.nLpPoint;
                }
                else
                {
                    /** 구현 MIU 지원 */
                    nMeterType = 0;
                    nLpPoint = pMbusInfo->lp.nLpPoint;
                }
                pChunk->Add(nMeterType);

			    nDay  = (nOffset - nSeek);
                switch(nMeterType)
                {
                    case 1:
			            addr  = MBUS_TYPE1_LP_ADDRESS(m_MbusBaseAddr[nMbusIdx], nLpPoint, nDay);
			            nSize = 879;
                        break;
                    default:
			            addr  = MBUS_TYPE0_LP_ADDRESS(m_MbusBaseAddr[nMbusIdx] - (bNewRomMap?0:1), nLpPoint, nDay);
			            nSize = 1676;
                        break;
                }

                XDEBUG(" READ LPDATA Port %d Offset %d Day %d Slot %d\r\n", pMbusInfo->nPort, nOffset, nDay, nMbusIdx);
                XDEBUG("             Type %d Base %04X LpPoint %02X ADDR %04X\r\n", nMeterType, m_MbusBaseAddr[nMbusIdx], nLpPoint, addr);

                pOndemand->nRetry = 0;
			    for(i=0, n=0, len=0; i<nSize; i+=n)
		   	    { 
			   	    len = MIN((nSize-i), MAX_ROM_READ_BYTES);
                    pOndemand->stat.nCount ++;
                    pOndemand->stat.nSendCount ++;
                    pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
			        gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
		   	   	    nError = endiReadRom(pOndemand->endi, addr+i, len, (BYTE *)szBuffer+i, &n, 2000);
			   	    if (nError != CODIERR_NOERROR || len != n)
			   	    {
               		    pOndemand->nRetry++;
                        pOndemand->stat.nError ++;
				   	    if (pOndemand->nRetry > GetRetryCount())
					    {
						    nState = STATE_ERROR;
					   	    break;
					    }
				   	    n = 0;
			   	    } else pOndemand->nRetry = 0;
                    pOndemand->stat.nCount ++;
                    pOndemand->stat.nRecvCount ++;
                    pOndemand->stat.nRecv += n + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
			    }
                if(nState != STATE_ERROR)
                {
                    pChunk->Add((const char *)szBuffer, nSize);
                }

			    nSeek++;
			    if (nSeek >= nCount)
                {
                    if(!nComPort)
                    {
                        nSeek = 0;
                        nMbusIdx++;
                    }
                    else
                    {
                        /** 한번에 하나의 Meter만 Ondemand 한다 */
                        nState = STATE_OK;
                        pOndemand->nResult = ONDEMAND_ERROR_OK;
                    }
                }
                break;     

            case STATE_INITIALIZE:
                XDEBUG(" MBUS Initialize : %s %d\r\n", szGUID, nPort);

                mbusLength = 0; 
                nError = mbusMakeShortFrame(szBuffer, 
                                MBUS_CTRL_SND_NKE,                 // Initialization of Slave
                                /*nPort ? nPort :*/ MBUS_ADDR_BROADCAST_NOREPLY,
                                &mbusLength);   // MBUS_ADDR_BROADCAST_REPLY -> NOREPLY 수정
                if(nError == MBUSERR_NOERROR)
                {
                    SendCommand(pOndemand, &id, (const char *)szBuffer, mbusLength);
                }
                else
                {
                    XDEBUG(" MakeFrameError : %s\r\n", mbusGetErrorMessage(nError));
                    nState = STATE_ERROR;
                    break;
                }
/*
                m_Locker.Lock();
                ClearOperation(pOndemand);
                pOndemand->nState = nState;
                pOndemand->bReply = FALSE;
                ResetEvent(pOndemand->hEvent);
                m_Locker.Unlock();

                nState = STATE_WAIT_REPLY;  // 서로 다른 기종의 Kamstrup미터 간 충돌 방지 위해 주석처리
*/
                nState = STATE_MBUS_COMSET;
                break;

            case STATE_MBUS_COMSET:
                XDEBUG(" MBUS COMM Set : %s\r\n", szGUID);

                mbusLength = 0; 
                nError = mbusMakeControlFrame(szBuffer, 
                                MBUS_CTRL_SND_UD | MBUS_CTRL_MASK_FCV /*| MBUS_CTRL_MASK_FCB*/,  // Send User Data
                                nPort ? nPort : MBUS_ADDR_BROADCAST_REPLY,
                                MBUS_CI_B300,
                                &mbusLength);
                if(nError == MBUSERR_NOERROR)
                {
                    SendCommand(pOndemand, &id, (const char *)szBuffer, mbusLength);
                }
                else
                {
                    XDEBUG(" MakeFrameError : %s\r\n", mbusGetErrorMessage(nError));
                    nState = STATE_ERROR;
                    break;
                }

                m_Locker.Lock();
                ClearOperation(pOndemand);
                pOndemand->nState = nState;
                pOndemand->bReply = FALSE;
                ResetEvent(pOndemand->hEvent);
                m_Locker.Unlock();

                nMbusIdx = 0;
                nState = STATE_WAIT_REPLY;
                break;

            case STATE_MBUS_GET_UD2:
                XDEBUG(" MBUS Get : %s\r\n", szGUID);

                mbusLength = 0; 
                nError = mbusMakeShortFrame(szBuffer, 
                                MBUS_CTRL_REQ_UD2 | MBUS_CTRL_MASK_FCV ,  // Send User Data
                                nPort ? nPort : MBUS_ADDR_BROADCAST_REPLY,
                                &mbusLength);
                if(nError == MBUSERR_NOERROR)
                {
                    SendCommand(pOndemand, &id, (const char *)szBuffer, mbusLength);
                }
                else
                {
                    XDEBUG(" MakeFrameError : %s\r\n", mbusGetErrorMessage(nError));
                    nState = STATE_ERROR;
                    break;
                }

                m_Locker.Lock();
                ClearOperation(pOndemand);
                pOndemand->nState = nState;
                pOndemand->bReply = FALSE;
                ResetEvent(pOndemand->hEvent);
                m_Locker.Unlock();

                nState = STATE_WAIT_REPLY;
                break;

            case STATE_MBUS_WRITE_ADDR:
                if(nDataLen < 1) 
                {
                    nState = STATE_ERROR;
                    break;
                }
                XDEBUG(" MBUS WRITE Address : %s Prev %d New %d\r\n", szGUID, nPort, pBypassData[0]);

                mbusLength = 0; 
                memset(szReq, 0, sizeof(szReq));
                szReq[0] = 0x01;            // DIF
                szReq[1] = 0x7A;            // VIF
                szReq[2] = pBypassData[0];  // Address
                nReqLen = 3;
                nError = mbusMakeLongFrame(szBuffer, 
                                MBUS_CTRL_SND_UD | MBUS_CTRL_MASK_FCV ,  // Send User Data
                                nPort ? nPort : MBUS_ADDR_BROADCAST_REPLY,
                                MBUS_CI_DATA_MODE1,
                                szReq, nReqLen,
                                &mbusLength);
                if(nError == MBUSERR_NOERROR)
                {
                    SendCommand(pOndemand, &id, (const char *)szBuffer, mbusLength);
                }
                else
                {
                    XDEBUG(" MakeFrameError : %s\r\n", mbusGetErrorMessage(nError));
                    nState = STATE_ERROR;
                    break;
                }

                m_Locker.Lock();
                ClearOperation(pOndemand);
                pOndemand->nState = nState;
                pOndemand->bReply = FALSE;
                ResetEvent(pOndemand->hEvent);
                m_Locker.Unlock();

                nState = STATE_WAIT_REPLY;
                break;

            case STATE_WAIT_REPLY:
                WaitForSingleObject(pOndemand->hEvent, (int)(GetReplyTimeout()/1000));
                m_Locker.Lock();
                if(pOndemand->bPartialWait)
                {
                    pOndemand->bPartialWait = FALSE;
                    m_Locker.Unlock();
                    continue;
                }
                m_Locker.Unlock();
                if(pOndemand->bReply)
                {
                    switch(pOndemand->nState)
                    {
                        case STATE_INITIALIZE:
                            nState = STATE_MBUS_COMSET;
                            break;
                        case STATE_MBUS_COMSET:
                            switch(nAction)
                            {
                                case 0x01:      // Write
                                    switch(nCommand)
                                    {
                                        case 0x02:
                                            nState = STATE_MBUS_WRITE_ADDR;
                                            break;
                                        default:
                                            nState = STATE_ERROR;
                                    }
                                    break;
                                default:        // Read;
                                    nState = STATE_MBUS_GET_UD2;
                            }
                            break;
                        //case STATE_MBUS_GET_UD2:
                        default:
                            pChunk->Add(pOndemand->pszBuffer, pOndemand->nLength);
                            nState = STATE_OK;
                            pOndemand->nResult = ONDEMAND_ERROR_OK;
                            break;
                        /*
                        default:
                            XDEBUG(" STATE_WAIT_REPLAY : Unknown STATE %d\r\n", pOndemand->nState);
                            nState = STATE_ERROR;
                            break;
                        */
                    }
                }
                else
                {
                    /** Timeout */
                    nFailCount ++;
                    XDEBUG(" FAIL COUNT SUCC %d\r\n", nFailCount);
                    if (nFailCount >= 3)
                        nState = STATE_ERROR;
                    else
                        nState = pOndemand->nState;
                }
                break;

            case STATE_ERROR : /** STATE_OK 로 자동 연결된다 */
                XDEBUG (" STATE_ERROR\r\n");

            case STATE_OK :
                endiDisconnect(pOndemand->endi);

                gettimeofday((struct timeval *)&pOndemand->tmEnd, NULL);
                time(&tmEnd);

                if (pOndemand->nResult == ONDEMAND_ERROR_OK)
                {
					nResult = ONDEMAND_ERROR_OK;
                    if(nMeteringType != METERING_TYPE_BYPASS)
                    {
                        m_pEndDeviceList->UpdateSensorInventory(&id, NULL, PARSER_TYPE_NOT_SET, NULL, 
                            (((WORD) (hwVersion & 0xf0)) << 4) + (hwVersion & 0x0f),
                            (((WORD) (fwVersion & 0xf0)) << 4) + (fwVersion & 0x0f),
                            (((WORD) (fwBuild >> 4)) * 10) + (fwBuild & 0x0f));
                        m_pEndDeviceList->UpdateSubInventory(&id, "MSTR711", subFwVersion, subFwBuild);
                        m_pEndDeviceList->SetRouteDeviceInfo(&id, permitMode ? TRUE : FALSE, 
                            permitState ? TRUE : FALSE, 0xFF, 0xFF);
                        m_pEndDeviceList->SetEndDeviceFlag(&id, ntoshort(alarmMask), alarmFlag, testFlag);
                    }

                    XDEBUG("\r\n------------------[ OK ]-----------------\r\n");
                    // 실제 데이터 길이는 처음 Chunk를 넘결 받을때 넘겨 받은 길이를 빼고 계산한다.
                    XDEBUG("   Metering Data Size = %d byte(s)\r\n", pChunk->GetSize()-pChunk->GetCommitSize());

                    pOndemand->nSensorError = CODIERR_NOERROR;
                    pOndemand->nMeterError = 0;
                }
                else
                {
                    nResult = ONDEMAND_ERROR_FAIL;
                    XDEBUG("\r\n");
                    XDEBUG("-----------------[ FAIL ]----------------\r\n");
                }

			    if (pOndemand->stat.nCount == 0 || pOndemand->stat.nRecvCount == 0)
					 nElapse = 0;
			    else nElapse = pOndemand->stat.nResponseTime / pOndemand->stat.nRecvCount;
			    pOndemand->stat.nUseTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmEnd);
			    pOndemand->stat.nConnectTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmConnect);

                if (pStat != NULL)
                   memcpy(pStat, &pOndemand->stat, sizeof(METER_STAT));

                XDEBUG("      Metering Elapse = %d second(s)\r\n", tmEnd-tmStart);
                XDEBUG("\r\n");
                XDEBUG("       Transmit Count = %d frams(s)\r\n", pOndemand->stat.nCount);
                XDEBUG("           Send Count = %d frams(s)\r\n", pOndemand->stat.nSendCount);
                XDEBUG("        Receive Count = %d frams(s)\r\n", pOndemand->stat.nRecvCount);
                XDEBUG("           Total Send = %d byte(s)\r\n", pOndemand->stat.nSend);
                XDEBUG("        Total Receive = %d byte(s)\r\n", pOndemand->stat.nRecv);
                XDEBUG("       Total Use Time = %d ms\r\n", pOndemand->stat.nUseTime);
                XDEBUG("         Connect Time = %d ms\r\n", pOndemand->stat.nConnectTime);
                XDEBUG("-----------------------------------------\r\n");
                XDEBUG("\r\n");

                endiClose(pOndemand->endi);
    
                m_Locker.Lock();
                DeleteOperation(pOndemand);    
                pOndemand = NULL;
                m_Locker.Unlock();

                nState = STATE_DONE;
                break;

            case STATE_RETRY :
                if (pOndemand->endi != NULL)
                {
                    if (bConnected) endiDisconnect(pOndemand->endi);
                    endiClose(pOndemand->endi);
                    pOndemand->endi = NULL;
                }

                m_Locker.Lock();
                DeleteOperation(pOndemand);    
                pOndemand = NULL;
                m_Locker.Unlock();

                nRetry++;
                if (nRetry >= GetRetryCount())
                {
                    nState = STATE_DONE;
                    break;
                }

                nState = STATE_OPEN;
                break;
        }
    }

    if (!bConnected)
        nState = ENDISTATE_CONNECTION_ERROR;
    else nState = (nResult == ONDEMAND_ERROR_OK) ? ENDISTATE_NORMAL : ENDISTATE_METER_ERROR;

    m_pEndDeviceList->SetBusy(&id, FALSE);
    if(nMeteringType != METERING_TYPE_BYPASS)
    {
        m_pEndDeviceList->SetState(&id, nState);
        m_pEndDeviceList->VerifyEndDevice(&id);
    }

    if (nState != nSaveState)
        commZruErrorEvent(&id, (nState == ENDISTATE_NORMAL) ? 0 : 1);

    if((nMeteringType == METERING_TYPE_ROUTINE) && (nResult == ONDEMAND_ERROR_OK)) return ONDEMAND_ERROR_NOT_SAVE;
    return nResult;
}

void CMbusParser::OnData(EUI64 *pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
{
    MBUS_HEADER     *pHeader;
    DATASTREAM      *pStream;

    if(pOndemand)
    {
        pOndemand->stat.nCount ++;
        pOndemand->stat.nRecvCount ++;
        pOndemand->stat.nRecv += nLength + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);

        pStream = pOndemand->pDataStream;
        if(pStream && !ParseStream(pStream, pszData, nLength, pOndemand))
        {
            pHeader =  (MBUS_HEADER *)pStream->pszBuffer;
            if(pHeader == NULL)
            {
                XDEBUG(" MBUS STREAM PARSING FAIL\r\n");
                return;
            }

            /** 아직 Frame이 완료되지 않았을 때 */
            XDEBUG(" PENDDING FRAME CURR=%d\r\n", pStream->nLength);
        }
    }
}

BOOL CMbusParser::OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
    char            szGUID[20];
    MBUS_HEADER     *pHeader;
    MBUS_SHORT_HEADER *pSHeader;
    MBUS_LONG_HEADER *pLHeader;
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    BOOL            bRes = TRUE;

    eui64toa(&pOndemand->id, szGUID);

    pHeader =  (MBUS_HEADER *)pStream->pszBuffer;
    if(pHeader == NULL)
    {
        XDEBUG(" MBUS STREAM PARSING FAIL\r\n");
        return FALSE;
    }


    switch(pHeader->start)
    {
        case MBUS_FRAME_SINGLE:
            XDEBUG(" SINGLE FRAME %s\r\n", szGUID);
            break;
        case MBUS_FRAME_SHORT:
            pSHeader = (MBUS_SHORT_HEADER *)pStream->pszBuffer;
            XDEBUG(" SHORT FRAME %s CTRL 0x%02X ADDR 0x%02X\r\n", szGUID, pSHeader->ctrl, pSHeader->addr);
            break;
        case MBUS_FRAME_LONG:
            pLHeader = (MBUS_LONG_HEADER *)pStream->pszBuffer;
            if(pLHeader->len == 3)
            {
                XDEBUG(" CONTROL FRAME %s CTRL 0x%02X ADDR 0x%02X CI 0x%02X\r\n", 
                        szGUID, pLHeader->ctrl, pLHeader->addr, pLHeader->ci);
            }
            else
            {
                pOndemand->nLength = pLHeader->len - 3;
                memcpy(pOndemand->pszBuffer, pStream->pszBuffer + sizeof(MBUS_LONG_HEADER), pOndemand->nLength);
                XDEBUG(" LONG FRAME %s CTRL 0x%02X ADDR 0x%02X CI 0x%02X LEN %d\r\n", 
                        szGUID, pLHeader->ctrl, pLHeader->addr, pLHeader->ci, pLHeader->len - 3);
                XDUMP((const char *)(pStream->pszBuffer + sizeof(MBUS_LONG_HEADER)), pLHeader->len - 3);
            }
            break;
        default:
            XDEBUG(" UNKNOWN FRAME %s 0x%02X\r\n", szGUID, pHeader->start);
            XDUMP((const char *)pStream->pszBuffer, pStream->nLength);
            bRes = FALSE;
            break;
    }

    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    m_Locker.Unlock();

    // Event signal
    SetEvent(pOndemand->hEvent);

    return bRes;
}

