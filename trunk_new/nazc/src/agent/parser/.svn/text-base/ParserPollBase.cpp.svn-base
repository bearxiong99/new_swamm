//////////////////////////////////////////////////////////////////////
//
//  ParserPollBase.cpp: implementation of the CPollBaseParser class.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "common.h"
#include "codiapi.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "Chunk.h"
#include "Event.h"
#include "MeterUploader.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "ParserPollBase.h"

#include "vendor/pollmiu.h"
#include "rep/MeterHelper.h"

//////////////////////////////////////////////////////////////////////
// CPollBaseParser Class
//////////////////////////////////////////////////////////////////////

#define LP_BLOCK_SIZE                48

//////////////////////////////////////////////////////////////////////
// CPollBaseParser Class
//////////////////////////////////////////////////////////////////////

CPollBaseParser::CPollBaseParser(int nType, int nService, int nAttribute, 
	char **pszNameArr, UINT nParserType, const char *pszDescr, const char *pszPrefixVendor, 
	const char *pszModel, int nConnectionTimeout, int nReplyTimeout, int nRetry)
	: CBaseParser(nType, nService, nAttribute, pszNameArr, nParserType, pszDescr,
	  pszPrefixVendor, pszModel, nConnectionTimeout, nReplyTimeout, nRetry)
{
}

CPollBaseParser::~CPollBaseParser()
{
}

//////////////////////////////////////////////////////////////////////
// CPollBaseParser Operations
//////////////////////////////////////////////////////////////////////

void CPollBaseParser::ActPreAction(ONDEMANDPARAM *pOndemand)
{
    if(pOndemand != NULL) {
        pOndemand->bMeterConstants = FALSE;
        pOndemand->bVoltageScale = FALSE;
    }
}

/** MeterConstant 값은 읽고 VoltageScale은 읽지 않는다 */
int CPollBaseParser::StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    return ReadRomMap(ENDI_ROMAP_NODE_KIND, sizeof(ENDI_NODE_INFO), pOndemand, szBuffer, 
            STATE_READ_NODEINFO, STATE_READ_METERTYPE);
}

int CPollBaseParser::StateMeterType(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    /** 일반적으로 Meter Type을 읽지 않는다
      */
    return STATE_READ_METERSERIAL;
}

int CPollBaseParser::StateMeterSerial(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    /** 일반적으로 Meter Serial을 설정하지 않는다
      */
    return STATE_READ_ENERGY_LEVEL;
}

int CPollBaseParser::StateEnergyLevel(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    /** 일반적으로 Energy Level 값을 읽지 않는다
      */
    return STATE_READ_AMRINFO;
}

int CPollBaseParser::StateAmrInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    return ReadRomMap(ENDI_ROMAP_TEST_FLAG, sizeof(AMR_PARTIAL_INFO), pOndemand, szBuffer, 
            STATE_READ_AMRINFO, STATE_READ_METERCONSTANT);
}

int CPollBaseParser::StateMeterConstant(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    WORD nMeterConstant = 1;
    int nState = 0;
   
    if(pOndemand->bMeterConstants == FALSE) return STATE_READ_LPSET;

    nState = ReadRomMap(ENDI_ROMAP_METER_CONSTANT, sizeof(WORD), pOndemand, szBuffer, 
            STATE_READ_METERCONSTANT, STATE_READ_LPSET);
    if(nState == STATE_READ_LPSET) {
        memcpy(&nMeterConstant, szBuffer, sizeof(WORD));
        nMeterConstant = BigToHostShort(nMeterConstant);
        pOndemand->nMeterConstant = nMeterConstant;
    }
    return nState;
}

int CPollBaseParser::StateLpSet(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    return ReadRomMap(ENDI_ROMAP_METER_LPPERIOD, sizeof(ENDI_LPLOG_INFO), pOndemand, szBuffer, 
            STATE_READ_LPSET, STATE_READ_LPDATA);
}

void CPollBaseParser::ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData)
{
    POLLBASED_METERING_DATA data;
    ENDI_LPLOG_INFO         *pLpLog = (ENDI_LPLOG_INFO *)pExtraData;

    if(pOndemand->nMeteringType != METERING_TYPE_BYPASS)
    {
        memset(&data, 0, sizeof(POLLBASED_METERING_DATA));
        GetGmtTimestamp(&data.time);
        data.curPulse    = pOndemand->nCurPulse;
        data.lpPeriod    = pLpLog->LPPERIOD;
        /** data.unit 은 아직 지원하지 않는다 */
        data.meterConstant = pOndemand->nMeterConstant;
        data.dateCount   = pOndemand->nCount;
   
        pChunk->Add((char *)&data, sizeof(POLLBASED_METERING_DATA));        // LP 정보
        pChunk->Add(pOndemand->pLpChunk->GetBuffer(), pOndemand->pLpChunk->GetSize());        // LP 
    }

    if(pOndemand->pMtrChunk->GetSize() > 0) {
        pChunk->Add(pOndemand->pMtrChunk->GetBuffer(), pOndemand->pMtrChunk->GetSize());        // Bypass 정보
    }

    if(pOndemand->nMeteringType != METERING_TYPE_BYPASS)
    {
        m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("MC"), VARSMI_WORD, sizeof(WORD), (BYTE *) &data.meterConstant);
        m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("CP"), VARSMI_UINT, sizeof(UINT), (BYTE *) &data.curPulse);
    }
}

void CPollBaseParser::ActPostFailAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData)
{
    // Do Noting
}

/** LP Pointer Block 정보를 읽는다.
 *
 * Issue #977 : Kamstrup 신규 Format을 위해 추가되었다.
 *
 * @param pOndemand Ondemand Parameter
 * @param szBuffer ROM Read 해서 읽은 값이 들어 있는 Buffer
 * @param pLpInfo LP Pointer 정보를 가지고 있는 구조체 pointer. void * 형으로 되어 있고 적당한 값으로 변경해야 한다.
 */
int CPollBaseParser::ActPostLpSet(ONDEMANDPARAM *pOndemand, const BYTE *szBuffer, void *pLpInfo, int *nLpPeriod) 
{
    ENDI_LPLOG_INFO*    lpLog;

    if(pOndemand == NULL || szBuffer == NULL || pLpInfo == NULL) return STATE_ERROR;

    lpLog = (ENDI_LPLOG_INFO*) pLpInfo;
    memcpy(lpLog, szBuffer, sizeof(ENDI_LPLOG_INFO));

    // Issue #1191: LPPERIOD가 이상한 값이 들어 있으면 죽을 수 있으므로 범위 확인
	if (((lpLog->LPPERIOD != 1) && (lpLog->LPPERIOD != 2) && (lpLog->LPPERIOD != 4)) ||
	                (lpLog->LPPOINTER > 39))
	{
	    XDEBUG(ANSI_COLOR_RED " ****** PARSER: Invalid ROM DATA ******\r\n" ANSI_NORMAL);
	    XDEBUG(ANSI_COLOR_RED " LP POINTER = %d, LP PERIOD = %d\r\n" ANSI_NORMAL,
	                lpLog->LPPOINTER, lpLog->LPPERIOD);
	    return STATE_ERROR;
	}
    if(nLpPeriod != NULL)
    {
        *nLpPeriod = lpLog->LPPERIOD;
    }
    return STATE_READ_LPDATA;
}

/** LP Information block 정보를 이용해서 읽어야 하는 LP Block address와 size 그리고 다음번 Seek에 더해줄 값을 결정해 준다.
 */
BOOL CPollBaseParser::GetLpAddressSize(ONDEMANDPARAM *pOndemand, void *lpLogBuff, 
        int nOffset, int nCount, WORD *nAddr, int *nSize, int *nAddSeek)
{
    ENDI_LPLOG_INFO *lpLog = (ENDI_LPLOG_INFO *)lpLogBuff;

    if(nAddr)
    {
        *nAddr = METER_LP_ADDRESS(ENDI_ROMAP_METER_LPLOGDATA, (lpLog->LPPOINTER-nOffset), lpLog->LPPERIOD);
    }
    if(nSize)
    {
        *nSize = lpLog->LPPERIOD * LP_BLOCK_SIZE + 8;
    }
    if(nAddSeek)
    {
        *nAddSeek = 1; 
    }
    return TRUE;
}


//////////////////////////////////////////////////////////////////////

int CPollBaseParser::DoMetering(CMDPARAM *pCommand, int nOption, int nOffset, int nCount, void * pBypassData, METER_STAT *pStat)
{
    ONDEMANDPARAM       *pOndemand = NULL;
    char                szGUID[20];
    EUI64               id;
    int                 nRetry;
    int                 nState, nSaveState;
    int                 nResult;
    CChunk              *pChunk;
    BOOL                bConnected = FALSE;
    WORD                addr;
    int                 len, nSize, nSeek=0, nAddSeek=1, n, i;
    int                 nRemainOffset, nRemainCount;
    ENDI_LP_ENTRY       *pLpEntry;
    //ENDI_LPLOG_INFO     lpLog;
    BYTE                lpLogBuff[256];
    TIMESTAMP           tmLastMetering;
    TIMESTAMP           tmLastEventLog;
    time_t              tmStart, tmEnd, tmLast=0, tmLastEvent=0;

    ENDI_NODE_INFO      *nodeInfo;
    AMR_PARTIAL_INFO    *amrInfo;
    BYTE                szBuffer[512];
    BYTE                permitMode=0, permitState=0, testFlag=0;
    signed char         alarmFlag = -1;
    BYTE                hwVersion=0, fwVersion=0, fwBuild=0;
    BOOL                bUpdateVersion = FALSE;
    BOOL                bUpdateFlag = FALSE;
    WORD                alarmMask=0;
    int                 nLpPeriod=-1;

    memcpy(&id, &pCommand->id, sizeof(EUI64));      // Sensor ID
    pChunk = pCommand->pChunk;            // 검침데이터를 리턴할 Chunk

    if (codiGetState(pCommand->codi) != CODISTATE_NORMAL)
        return ONDEMAND_ERROR_NOT_CONNECT;

    nResult = ONDEMAND_ERROR_NOT_CONNECT;

    eui64toa(&id, szGUID);
    nSaveState = m_pEndDeviceList->GetState(&id);
    if (nSaveState == ENDISTATE_INIT)
        return ONDEMAND_ERROR_INIT;

    m_pEndDeviceList->SetBusy(&id, TRUE);

    // 마지막 검침 시간을 구한다.
    m_pEndDeviceList->GetLastMeteringTime(&id, &tmLast);
    GetTimestamp(&tmLastMetering, &tmLast);    

    // 마지막 이벤트 로그 시간을 구한다.
    m_pEndDeviceList->GetLastEventLogTime(&id, &tmLastEvent);
    GetTimestamp(&tmLastEventLog, &tmLastEvent);    

    XDEBUG(" PARSER: Option=%d, Offset=%d, Count=%d\r\n",
            nOption, nOffset, nCount);

    XDEBUG("    LAST METERING = %04d/%02d/%02d %02d:%02d:%02d, LAST EVENT = %04d/%02d/%02d %02d:%02d:%02d\r\n",
            tmLastMetering.year, tmLastMetering.mon, tmLastMetering.day,
            tmLastMetering.hour, tmLastMetering.min, tmLastMetering.sec,
            tmLastEventLog.year, tmLastEventLog.mon, tmLastEventLog.day,
            tmLastEventLog.hour, tmLastEventLog.min, tmLastEventLog.sec);

    nState = STATE_OPEN;

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
                pOndemand->nOption = nOption;
                pOndemand->nOffset = nOffset;
                pOndemand->nCount = nCount;
                pOndemand->pBypassData = pBypassData;
                pOndemand->pParser = (void *)this;
                memcpy(&pOndemand->tmLastMetering, &tmLastMetering, sizeof(TIMESTAMP));
                memcpy(&pOndemand->tmLastEventLog, &tmLastEventLog, sizeof(TIMESTAMP));
                pOndemand->nVendor = -1;
                pOndemand->nResult = ONDEMAND_ERROR_FAIL;
                m_Locker.Unlock();

                bConnected = FALSE;

                /** Pre Action */
                ActPreAction(pOndemand);
                /** PreAction에서 Option, Offset, Count가 변경되었을 수 있기 때문에 다시 설정해 준다 */
                nOption = pOndemand->nOption;
                nOffset = pOndemand->nOffset;
                nCount = pOndemand->nCount;

                nState = StateOpen(pCommand->codi, pOndemand, &bConnected);
                pChunk->Rollback();
                break;
        
            case STATE_READ_NODEINFO :                         // NODE_INFO를 읽어 본다.
                XDEBUG(" PARSER: Read Node Info : %s\r\n", szGUID);
                nState = StateNodeInfo(pOndemand, szBuffer);
                if(nState != STATE_READ_NODEINFO) {
                    nodeInfo = (ENDI_NODE_INFO *) szBuffer;
                    fwVersion = nodeInfo->FIRMWARE_VERSION;
                    fwBuild = nodeInfo->FIRMWARE_BUILD;
                    hwVersion = nodeInfo->HARDWARE_VERSION;
                    bUpdateVersion = TRUE;
                }
                break;     

            case STATE_READ_METERTYPE :                       // Meter Type을 읽는다
                XDEBUG(" PARSER: Read Meter Type : %s\r\n", szGUID);
                nState = StateMeterType(pOndemand, szBuffer);
                break;

            case STATE_READ_METERSERIAL :                       // Meter Serial
                XDEBUG(" PARSER: Read Meter Serial : %s\r\n", szGUID);
                nState = StateMeterSerial(pOndemand, szBuffer);
                break;

            case STATE_READ_ENERGY_LEVEL :                      // Energy level
                XDEBUG(" PARSER: Read Energy Level : %s\r\n", szGUID);
                nState = StateEnergyLevel(pOndemand, szBuffer);
                break;

            case STATE_READ_AMRINFO :                           // AMR_INFO를 읽어 본다.
                XDEBUG(" PARSER: Read AMR Info : %s\r\n", szGUID);
                nState = StateAmrInfo(pOndemand, szBuffer);
                if(nState != STATE_READ_AMRINFO) {
                    amrInfo = (AMR_PARTIAL_INFO *) szBuffer;
                    permitMode = amrInfo->permit_mode;
                    permitState = amrInfo->permit_state;
                    alarmMask = amrInfo->alarm_mask;
                    alarmFlag = amrInfo->alarm_flag;
                    testFlag = amrInfo->test_flag;
                    bUpdateFlag = TRUE;
                    
                    if(pCommand->nOption == ONDEMAND_OPTION_READ_IDENT_ONLY) {
                        pOndemand->nResult = ONDEMAND_ERROR_OK;
			            nState = STATE_OK;
                    }
                }
			    break;	 

            case STATE_READ_METERCONSTANT :                         // Meter 상수 읽기
                XDEBUG(" PARSER: Read Meter Constant : %s\r\n", szGUID);
                nState = StateMeterConstant(pOndemand, szBuffer);
                if(nState != STATE_READ_METERCONSTANT) {
                    if(pOndemand->nMeterConstant > 0) {
                        XDEBUG(" METER CONSTANT %d\r\n", pOndemand->nMeterConstant);
                    }
                }
                break;     

            case STATE_READ_LPSET :             // LP 정보 설정을 구한다.
                XDEBUG(" PARSER: Read LP Set : %s\r\n", szGUID);
                nState = StateLpSet(pOndemand, szBuffer);

                if(nState != STATE_READ_LPSET) {
                    nState = ActPostLpSet(pOndemand, (const BYTE*)szBuffer, (void *)lpLogBuff, &nLpPeriod);
			        pOndemand->pLpChunk->Empty();
			        nSeek = 0;

                    /** LP를 읽지 않게 Option이 왔을 경우 */
                    if(nCount <= 0)
                    {
                        nState = STATE_INITIALIZE;
                    }
                }
                break;     

          case STATE_READ_LPDATA :
                XDEBUG(" PARSER: Read LP Data : %s\r\n", szGUID);
			    // LP위치 = LP Period * 1시간LP데이터길이 + 일자 + 베이스값
			    nRemainOffset  = (nOffset - nSeek);
			    nRemainCount  = (nCount - nSeek);

                nAddSeek = 1;
                if(!GetLpAddressSize(pOndemand, (void *)lpLogBuff, nRemainOffset, nRemainCount, &addr, &nSize, &nAddSeek))
                {
                    XDEBUG(" PARSER: Get LP Address Fail nOffset:%d, nCount:%d, nSeek:%d, addr :%d, nSize:%d\r\n", 
                            nRemainOffset, nRemainCount, nSeek, addr, nSize);
                    nState = STATE_ERROR;
                    break;
                }

                //XDEBUG(" lpLog.LPPERIOD : %d\r\n", lpLog.LPPERIOD);
                XDEBUG(" PARSER: STATE_READ_LPDATA nOffset:%d, nCount:%d, nSeek:%d, addr :0x%X, nSize:%d\r\n", 
                        nRemainOffset, nRemainCount, nSeek, addr, nSize);
                pOndemand->nRetry = 0;
			    for(i=0, n=0, len=0; i<nSize && nState != STATE_ERROR ; i+=n)
		   	    { 
			   	    len = MIN((nSize-i), MAX_ROM_READ_BYTES);
                    nState =  ReadRomMap(addr+i, len, pOndemand, (BYTE *)szBuffer, 
                            STATE_READ_LPDATA, STATE_INITIALIZE);
                    if(nState == STATE_READ_LPDATA) {
                        n = 0;
                    }else if(nState == STATE_INITIALIZE) {
                        n = len;

                        if(i==0)
                        {
			                pLpEntry = (ENDI_LP_ENTRY *)szBuffer;
			                XDEBUG("DATE = %04d/%02d/%02d, Base Pulse=%d\r\n",
						                ntoshort(pLpEntry->LPDATE.year),
						                pLpEntry->LPDATE.mon,
						                pLpEntry->LPDATE.day,
						                ntoint(pLpEntry->BASE_PULSE));
                        }
			            pOndemand->pLpChunk->Add((char *)szBuffer, len);
                    }
			    }

			    if (nState != STATE_INITIALIZE || i<nSize) {
                    nState = STATE_ERROR;
				    break;
                }

			    nSeek += nAddSeek;
			    if (nSeek < nCount)
                {
                    nState = STATE_READ_LPDATA;
				    break;
                }
                break;

            case STATE_INITIALIZE:
                XDEBUG(" PARSER: Bypass Metering Initialize : %s\r\n", szGUID);
                nState = StateBypassInitialize(pOndemand, szBuffer);
                break;

            case STATE_QUERY:
                XDEBUG(" PARSER: Query : %s\r\n", szGUID);
                nState = StateQuery(pOndemand, szBuffer);
                break;

            case STATE_WAIT_REPLY:
                XDEBUG(" PARSER: Wait Reply : %s\r\n", szGUID);
                nState = StateWaitReply(pOndemand, szBuffer);
                break;

            case STATE_ERROR : /** STATE_OK 로 자동 연결된다 */
                XDEBUG(" PARSER: Error : %s\r\n", szGUID);
                nState = StateError(pOndemand, szBuffer);

            case STATE_OK :
                XDEBUG(" PARSER: Ok : %s\r\n", szGUID);
                endiDisconnect(pOndemand->endi);

                gettimeofday((struct timeval *)&pOndemand->tmEnd, NULL);
                time(&tmEnd);

                if (pOndemand->nResult == ONDEMAND_ERROR_OK)
                {
                    nResult = ONDEMAND_ERROR_OK;

                    /** Post Action */
                    ActPostSuccAction(pChunk, pOndemand, (void *)lpLogBuff);

                    if(pOndemand->nMeteringType != METERING_TYPE_BYPASS)
                    {
                        if(bUpdateVersion) {
                            m_pEndDeviceList->UpdateSensorInventory(&id, NULL, PARSER_TYPE_NOT_SET, NULL, 
                                (((WORD) (hwVersion & 0xf0)) << 4) + (hwVersion & 0x0f),
                                (((WORD) (fwVersion & 0xf0)) << 4) + (fwVersion & 0x0f),
                                (((WORD) (fwBuild >> 4)) * 10) + (fwBuild & 0x0f));
                        }
                        if(bUpdateFlag)
                        {
                            m_pEndDeviceList->SetRouteDeviceInfo(&id, permitMode ? TRUE : FALSE, 
                                permitState ? TRUE : FALSE, 0xFF, 0xFF);
                            m_pEndDeviceList->SetEndDeviceFlag(&id, ntoshort(alarmMask), alarmFlag, testFlag);
                        }

                        if(bUpdateVersion && nLpPeriod > 0) {
                            /** Issue #84 : DB Meter 정보 갱신 */
                            m_pMeterHelper->Update(&id, 
                                        0,                          // nPortNum
                                        (const char *)pOndemand->szMeterSerial,   // szSerial 
                                        m_nParserType, 
                                        m_nServiceType,
                                        pOndemand->nVendor, 
                                        (double)  ((hwVersion >> 4) + (hwVersion & 0xf) / 10.0),
                                        (double)  ((fwVersion >> 4) + (fwVersion & 0xf) / 10.0),
                                        nLpPeriod
                                    );
                        }
                    }

                    XDEBUG("\r\n------------------[ OK ]-----------------\r\n");
                    // 실제 데이터 길이는 처음 Chunk를 넘결 받을때 넘겨 받은 길이를 빼고 계산한다.
                    XDEBUG("   Metering Data Size = %d byte(s)\r\n", pChunk->GetSize()-pChunk->GetCommitSize());

                    pOndemand->nSensorError = CODIERR_NOERROR;
                    pOndemand->nMeterError = 0;
                }
                else
                {
                    /** Post Action */
                    ActPostFailAction(pChunk, pOndemand, (void *)lpLogBuff);

                    nResult = ONDEMAND_ERROR_FAIL;
                    XDEBUG("\r\n");
                    XDEBUG("-----------------[ FAIL ]----------------\r\n");
                }

                /*
                if (pOndemand->stat.nCount == 0 || pOndemand->stat.nRecvCount == 0)
                     nElapse = 0;
                else nElapse = pOndemand->stat.nResponseTime / pOndemand->stat.nRecvCount;
                */
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

            default :
                /** 알수 없는 State에 도달했을 때 */
                nState = STATE_DONE;
                break;
        }
    }

    if (!bConnected)
         nState = ENDISTATE_CONNECTION_ERROR;
    else nState = (nResult == ONDEMAND_ERROR_OK) ? ENDISTATE_NORMAL : ENDISTATE_METER_ERROR;

    m_pEndDeviceList->SetState(&id, nState);
    m_pEndDeviceList->SetBusy(&id, FALSE);
    m_pEndDeviceList->VerifyEndDevice(&id);
               
    if (nState != nSaveState)
        commZruErrorEvent(&id, (nState == ENDISTATE_NORMAL) ? 0 : 1);
    return nResult;
}

//////////////////////////////////////////////////////////////////////
// CPollBaseParser Operations
//////////////////////////////////////////////////////////////////////


