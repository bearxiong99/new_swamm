//////////////////////////////////////////////////////////////////////
//
//  ParserPulseBase.cpp: implementation of the CPulseBaseParser class.
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
#include "MemoryDebug.h"
#include "Patch.h"

#include "ParserPulseBase.h"
#include "rep/MeterHelper.h"

//////////////////////////////////////////////////////////////////////
// CPulseBaseParser Class
//////////////////////////////////////////////////////////////////////

#define LP_BLOCK_SIZE                48

#if     defined(__PATCH_11_65__)
extern BOOL    *m_pbPatch_11_65;
#endif

#ifdef _WIN32
#pragma pack(push, 1)
#endif	/* _WIN32 */

typedef struct {
    UINT           voltage;
    UINT           current;
}	__ATTRIBUTE_PACKED__ SCALE_CONSTANT;

#ifdef _WIN32
#pragma pack(pop)
#endif	/* _WIN32 */


//////////////////////////////////////////////////////////////////////
// CPulseBaseParser Class
//////////////////////////////////////////////////////////////////////

CPulseBaseParser::CPulseBaseParser(int nType, int nService, int nAttribute, 
	char **pszNameArr, UINT nParserType, const char *pszDescr, const char *pszPrefixVendor, 
	const char *pszModel, int nConnectionTimeout, int nReplyTimeout, int nRetry)
	: CBaseParser(nType, nService, nAttribute, pszNameArr, nParserType, pszDescr,
	  pszPrefixVendor, pszModel, nConnectionTimeout, nReplyTimeout, nRetry)
{
}

CPulseBaseParser::~CPulseBaseParser()
{
}

//////////////////////////////////////////////////////////////////////
// CPulseBaseParser Operations
//////////////////////////////////////////////////////////////////////

void CPulseBaseParser::ActPreAction(ONDEMANDPARAM *pOndemand)
{
    if(pOndemand != NULL) {
        pOndemand->bMeterConstants = FALSE;
        pOndemand->bVoltageScale = FALSE;
    }
}

int CPulseBaseParser::StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    return ReadRomMap(ENDI_ROMAP_NODE_KIND, sizeof(ENDI_NODE_INFO), pOndemand, szBuffer, 
            STATE_READ_NODEINFO, STATE_READ_METERSERIAL);
}

int CPulseBaseParser::StateMeterSerial(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    /** 일반적으로 Meter Serial을 설정하지 않는다
      */
    return STATE_READ_ENERGY_LEVEL;
}

int CPulseBaseParser::StateEnergyLevel(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    /** 일반적으로 Energy Level 값을 읽지 않는다
      */
    return STATE_READ_AMRINFO;
}

int CPulseBaseParser::StateAmrInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    int nNextState = ReadRomMap(ENDI_ROMAP_TEST_FLAG, sizeof(AMR_PARTIAL_INFO), pOndemand, szBuffer, 
            STATE_READ_AMRINFO, STATE_READ_AMRDATA);
   
    return nNextState;
}

int CPulseBaseParser::StateAmrData(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    int nNextState = ReadAmrData(pOndemand, szBuffer, STATE_READ_AMRDATA, STATE_READ_VOLTAGESCALE);
   
    return nNextState;
}

int CPulseBaseParser::StateVoltageScale(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    SCALE_CONSTANT scale;
    int nNextState;

    if(pOndemand->bVoltageScale == FALSE) return STATE_READ_METERCONSTANT;

    nNextState = ReadRomMap(ENDI_ROMAP_VOLTAGE_SCALE, sizeof(SCALE_CONSTANT), pOndemand, szBuffer, 
            STATE_READ_VOLTAGESCALE, STATE_READ_METERCONSTANT);
    if(nNextState == STATE_READ_METERCONSTANT) {
        memcpy(&scale, szBuffer, sizeof(SCALE_CONSTANT));
        m_pEndDeviceList->UpdateConstantScale(&pOndemand->id,
            (double)BigToHostInt(scale.voltage)/1000000.0, 
            (double)BigToHostInt(scale.current)/1000000.0);
    }
    return nNextState;
}

int CPulseBaseParser::StateMeterConstant(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    WORD nMeterConstant = 1;
    int nNextState = 0;
   
    if(pOndemand->bMeterConstants == FALSE) return STATE_READ_LPSET;

    nNextState = ReadRomMap(ENDI_ROMAP_METER_CONSTANT, sizeof(WORD), pOndemand, szBuffer, 
            STATE_READ_METERCONSTANT, STATE_READ_LPSET);
    if(nNextState == STATE_READ_LPSET) {
        memcpy(&nMeterConstant, szBuffer, sizeof(WORD));
        nMeterConstant = BigToHostShort(nMeterConstant);
        pOndemand->nMeterConstant = nMeterConstant;
    }
    return nNextState;
}

int CPulseBaseParser::StateLpSet(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    return ReadRomMap(ENDI_ROMAP_METER_LPPERIOD, sizeof(ENDI_LPLOG_INFO), pOndemand, szBuffer, 
            STATE_READ_LPSET, STATE_READ_LPDATA);
}

int CPulseBaseParser::StateBatteryPoint(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    /* ACD/HMU의 경우
	if(IsBattery()) {
        return ReadRomMap(ENDI_ROMAP_BATTERY_POINT, sizeof(ENDI_BATERYLOG_INFO), pOndemand, szBuffer, 
            STATE_READ_BATTERYPOINT, STATE_READ_BATTERYLOG);
    }
    else {
    */
        return STATE_READ_LPSET;
        /*
    }
    */
}

void CPulseBaseParser::ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData)
{
    ENDI_METERING_INFO         *pMeteringInfo = (ENDI_METERING_INFO *)pExtraData;

    if(pOndemand->nMeteringType != METERING_TYPE_BYPASS)
    {
        pChunk->Add((char *)pMeteringInfo, sizeof(ENDI_METERING_INFO));        
        pChunk->Add(pOndemand->pLpChunk->GetBuffer(), pOndemand->pLpChunk->GetSize());        // LP 
    }

    if(pOndemand->pMtrChunk->GetSize() > 0) {
        pChunk->Add(pOndemand->pMtrChunk->GetBuffer(), pOndemand->pMtrChunk->GetSize());        // Bypass 정보
    }

    if(pOndemand->nMeteringType != METERING_TYPE_BYPASS)
    {
        UINT	tmpUV;

        tmpUV = BigToHostInt(pMeteringInfo->basePulse);
	    m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("BP"), VARSMI_UINT, sizeof(UINT), (BYTE *) &tmpUV);

        tmpUV = BigToHostInt(pMeteringInfo->curPulse);
	    m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("CP"), VARSMI_UINT, sizeof(UINT), (BYTE *) &tmpUV);
    }
}

void CPulseBaseParser::ActPostFailAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData)
{
}

int CPulseBaseParser::StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    return CBaseParser::StateBypassInitialize(pOndemand, szBuffer);
}


//////////////////////////////////////////////////////////////////////

int CPulseBaseParser::DoMetering(CMDPARAM *pCommand, int nOption, int nOffset, int nCount, void * pBypassData, METER_STAT *pStat)
{
    ONDEMANDPARAM       *pOndemand = NULL;
    char                szGUID[20];
    EUI64               id;
    int                 nRetry;
    int                 nState, nSaveState;
    int                 nResult/*, nElapse*/;
    CChunk              *pChunk;
    BOOL                bConnected = FALSE;
    WORD                addr;
    int                 len, nSize, nSeek=0, n, i;
    int                 nDay;
    ENDI_LPLOG_INFO     lpLog;
    ENDI_LP_ENTRY       *pLpEntry;
    ENDI_METERING_INFO  mdata;
	ENDI_BATTERYLOG_INFO *battInfo;
	ENDI_BATTERY_ENTRY *battLog;
    TIMESTAMP           tmLastMetering;
    TIMESTAMP           tmLastEventLog;
    time_t              tmStart, tmEnd, tmLast=0, tmLastEvent=0;

    ENDI_NODE_INFO      *nodeInfo;
    AMR_PARTIAL_INFO    *amrInfo;
    ENDI_AMR_DATA       *amrData;
    BYTE                szBuffer[512];
    BYTE                permitMode=0, permitState=0, testFlag=0;
    WORD                alarmMask=0;
	BYTE			    battPointer=0;
    BOOL                bUpdateVersion = FALSE;
    BOOL                bUpdateFlag = FALSE;
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
    memset(&lpLog, 0, sizeof(ENDI_LPLOG_INFO));

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
                pOndemand->nVendor = METER_VENDOR_UNKNOWN;
                pOndemand->nResult = ONDEMAND_ERROR_FAIL;
                m_Locker.Unlock();

                bConnected = FALSE;

                /** Pre Action */
                ActPreAction(pOndemand);
                nState = StateOpen(pCommand->codi, pOndemand, &bConnected);
                pChunk->Rollback();
                memset(&mdata, 0, sizeof(ENDI_METERING_INFO));
                break;
        
            case STATE_READ_NODEINFO :                         // NODE_INFO를 읽어 본다.
                nState = StateNodeInfo(pOndemand, szBuffer);
                if(nState != STATE_READ_NODEINFO) {
                    nodeInfo = (ENDI_NODE_INFO *) szBuffer;

                    mdata.fwVersion = nodeInfo->FIRMWARE_VERSION;
                    mdata.fwBuild = nodeInfo->FIRMWARE_BUILD;
                    mdata.swVersion = nodeInfo->SOFTWARE_VERSION;
                    mdata.hwVersion = nodeInfo->HARDWARE_VERSION;

                    bUpdateVersion = TRUE;
                }
                break;     

            case STATE_READ_METERSERIAL :                       // Meter Serial
                nState = StateMeterSerial(pOndemand, szBuffer);
                break;

            case STATE_READ_ENERGY_LEVEL :                      // Energy level
                nState = StateEnergyLevel(pOndemand, szBuffer);
                if(nState == STATE_READ_AMRINFO) {
                    mdata.energyLevel =  pOndemand->nEnergyLevel;
/** 제주실증단지 DR Level 적용 Patch */
#if     defined(__PATCH_11_65__)
                    if(*m_pbPatch_11_65) {
                        mdata.energyLevel =  Patch_11_65_Nuri2Jeju(mdata.energyLevel);
                    }
#endif
                }
                break;

            case STATE_READ_AMRINFO :                           // AMR_INFO를 읽어 본다.
                nState = StateAmrInfo(pOndemand, szBuffer);
                if(nState == STATE_READ_AMRDATA) {
                    amrInfo = (AMR_PARTIAL_INFO *) szBuffer;
                    permitMode = amrInfo->permit_mode;
                    permitState = amrInfo->permit_state;
                    alarmMask = amrInfo->alarm_mask;
                    testFlag = amrInfo->test_flag;
                    mdata.alarmFlag = amrInfo->alarm_flag;
                    mdata.networkType = amrInfo->network_type;
                    
                    bUpdateFlag = TRUE;
                    if(pCommand->nOption == ONDEMAND_OPTION_READ_IDENT_ONLY) {
                        pOndemand->nResult = ONDEMAND_ERROR_OK;
			            nState = STATE_OK;
                    }
                }
			    break;	 

            case STATE_READ_AMRDATA :                           // AMR_DATA를 읽어 본다.
                nState = StateAmrData(pOndemand, szBuffer);
                if(nState == STATE_READ_VOLTAGESCALE) {
                    amrData = (ENDI_AMR_DATA *) szBuffer;

                    memcpy(&mdata.curTime, &amrData->gmttime, sizeof(GMTTIMESTAMP));
                    mdata.oprDay = amrData->oprday;
                    mdata.actMin = amrData->actmin;
                    mdata.alarmFlag = amrData->alarm;
                    mdata.curPulse = amrData->pulse;
                    mdata.lpChoice = amrData->choice;
                    mdata.lpPeriod = amrData->period;
                }

			    break;	 

            case STATE_READ_VOLTAGESCALE :                         // 전압 전류 상수 읽기 (ACD/HMU)
                nState = StateVoltageScale(pOndemand, szBuffer);
                break;     

            case STATE_READ_METERCONSTANT :                         // Meter 상수 읽기
                nState = StateMeterConstant(pOndemand, szBuffer);
                if(nState == STATE_READ_BATTPOINT) {
                    if(pOndemand->nMeterConstant > 0) {
                        XDEBUG(" METER CONSTANT %d\r\n", pOndemand->nMeterConstant);
                    }
                }
                break;     
		    case STATE_READ_BATTPOINT : 						// BATTERY Pointer를 읽어 온다
                nState = StateBatteryPoint(pOndemand, szBuffer);

                if(nState == STATE_READ_BATTLOG) {
                    battInfo = (ENDI_BATTERYLOG_INFO *) szBuffer;
                    battPointer = battInfo->BATTERY_POINTER;
                }
                break;

		    case STATE_READ_BATTLOG : 						// BATTERY LOG를 읽어 본다.
			    addr = ENDI_ROMAP_BATTERY_LOGDATA + ((battPointer + 50) % 50) * sizeof(ENDI_BATTERY_ENTRY);
			    len  = sizeof(ENDI_BATTERY_ENTRY);

                nState =  ReadRomMap(addr, len, pOndemand, (BYTE *)szBuffer, STATE_READ_BATTLOG, STATE_READ_LPSET);

                if(nState == STATE_READ_LPSET) {
                    battLog = (ENDI_BATTERY_ENTRY *) szBuffer;

                    mdata.batteryVolt = battLog->BATTERY_VOLT;
                    mdata.curConsum = battLog->CONSUMPTION_CURRENT;
                    mdata.offset = battLog->OFFSET;
                }
                break;

            case STATE_READ_LPSET :             // LP 정보 설정을 구한다.
                nState = StateLpSet(pOndemand, szBuffer);

                if(nState == STATE_READ_LPDATA) {
                    memcpy(&lpLog, szBuffer, sizeof(ENDI_LPLOG_INFO));

			        // Issue #1191: LPPERIOD가 이상한 값이 들어 있으면 죽을 수 있으므로 범위 확인
			        if (((lpLog.LPPERIOD != 1) && (lpLog.LPPERIOD != 2) && (lpLog.LPPERIOD != 4)) ||
			   	           (lpLog.LPPOINTER > 39))
			        {
				        XDEBUG(ANSI_COLOR_RED " ****** PARSER: Invalid ROM DATA ******\r\n" ANSI_NORMAL);
				        XDEBUG(ANSI_COLOR_RED " LP POINTER = %d, LP PERIOD = %d\r\n" ANSI_NORMAL,
									    lpLog.LPPOINTER, lpLog.LPPERIOD);
				        nState = STATE_ERROR;
				        break;
			        }
                    nLpPeriod = lpLog.LPPERIOD;

			        pOndemand->pLpChunk->Empty();
			        nSeek = 0;
                }
                break;     

          case STATE_READ_LPDATA :
			    // LP위치 = LP Period * 1시간LP데이터길이 + 일자 + 베이스값
			    nDay  = (nOffset - nSeek);
			    addr  = METER_LP_ADDRESS(ENDI_ROMAP_METER_LPLOGDATA, (lpLog.LPPOINTER-nDay), lpLog.LPPERIOD);
			    nSize = lpLog.LPPERIOD * LP_BLOCK_SIZE + 8;

                XDEBUG("lpLog.LPPERIOD : %d\r\n", lpLog.LPPERIOD);
                XDEBUG("PARSER: STATE_READ_LPDATA nDay:%d, nSeek:%d, addr :%d, nSize %d\r\n", nDay, nSeek, addr, nSize);
                pOndemand->nRetry = 0;
			    for(i=0, n=0, len=0; i<nSize; i+=n)
		   	    { 
			   	    len = MIN((nSize-i), MAX_ROM_READ_BYTES);
                    nState =  ReadRomMap(addr+i, len, pOndemand, (BYTE *)szBuffer+i, 
                            STATE_READ_LPDATA, STATE_INITIALIZE);
                    if(nState == STATE_READ_LPDATA) {
                        n = 0;
                    }else if(nState == STATE_INITIALIZE) {
                        n = len;
                    }
			    }

			    if (nState != STATE_INITIALIZE || i<nSize) {
                    nState = STATE_ERROR;
				    break;
                }

                pLpEntry = (ENDI_LP_ENTRY *) szBuffer;
                memcpy(&mdata.lpDate, &pLpEntry->LPDATE, sizeof(TIMEDATE));
                memcpy(&mdata.basePulse, &pLpEntry->BASE_PULSE, sizeof(UINT));
			    XDEBUG("DATE = %04d/%02d/%02d, Base Pulse=%d\r\n",
						    ntoshort(pLpEntry->LPDATE.year),
						    pLpEntry->LPDATE.mon,
						    pLpEntry->LPDATE.day,
						    ntoint(pLpEntry->BASE_PULSE));

			    pOndemand->pLpChunk->Add((char *)szBuffer, nSize);
			    nSeek++;
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
                nState = StateQuery(pOndemand, szBuffer);
                break;

            case STATE_WAIT_REPLY:
                nState = StateWaitReply(pOndemand, szBuffer);
                break;

            case STATE_ERROR : /** STATE_OK 로 자동 연결된다 */
                XDEBUG (" STATE_ERROR\r\n");
                nState = StateError(pOndemand, szBuffer);

            case STATE_OK :
                endiDisconnect(pOndemand->endi);

                gettimeofday((struct timeval *)&pOndemand->tmEnd, NULL);
                time(&tmEnd);

                if (pOndemand->nResult == ONDEMAND_ERROR_OK)
                {
                    nResult = ONDEMAND_ERROR_OK;

                    /** Post Action */
                    ActPostSuccAction(pChunk, pOndemand, (void *)&mdata);

                    if(pOndemand->nMeteringType != METERING_TYPE_BYPASS)
                    {
                        BYTE lqi=0;
                        signed char rssi=0;

                        if(m_pEndDeviceList->GetQuality(&id, &lqi, &rssi))
                        {
                            mdata.LQI = lqi;
                            mdata.RSSI = rssi;
                        }

                        if(bUpdateVersion)
                        {
                            m_pEndDeviceList->UpdateSensorInventory(&id, NULL, PARSER_TYPE_NOT_SET, NULL, 
                             (((WORD) (mdata.hwVersion & 0xf0)) << 4) + (mdata.hwVersion & 0x0f),
                             (((WORD) (mdata.fwVersion & 0xf0)) << 4) + (mdata.fwVersion & 0x0f),
                             (((WORD) (mdata.fwBuild >> 4)) * 10) + (mdata.fwBuild & 0x0f));
                        }
                        if(bUpdateFlag)
                        {
                            m_pEndDeviceList->SetRouteDeviceInfo(&id, permitMode ? TRUE : FALSE, 
                                permitState ? TRUE : FALSE, 0xFF, 0xFF);
                            m_pEndDeviceList->SetEndDeviceFlag(&id, ntoshort(alarmMask), mdata.alarmFlag, testFlag);
		                    if(m_pEndDeviceList->SetEndDeviceNetworkType(&id, mdata.networkType)) {
			                    /*-- MIU의 Network Type이 변경되었다. Event를 정의해서 FEP로 쏘아주어야 한다 --*/
			                    XDEBUG("MIU Network Type Change : [%s] [%s]\r\n", szGUID, mdata.networkType ? "MESH" : "STAR");
		                    }
                        }
                        if(bUpdateVersion && nLpPeriod > 0) {
                            /** Issue #84 : DB Meter 정보 갱신 */
                            m_pMeterHelper->Update(&id, 
                                        0,                          // nPortNum
                                        (const char *)pOndemand->szMeterSerial,   // szSerial 
                                        m_nParserType, 
                                        m_nServiceType,
                                        pOndemand->nVendor, 
                                        (double)  ((mdata.hwVersion >> 4) + (mdata.hwVersion & 0xf) / 10.0),
                                        (double)  ((mdata.fwVersion >> 4) + (mdata.fwVersion & 0xf) / 10.0),
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
                    ActPostFailAction(pChunk, pOndemand, (void *)&mdata);

                    nResult = ONDEMAND_ERROR_FAIL;
                    XDEBUG("\r\n");
                    XDEBUG("-----------------[ FAIL ]----------------\r\n");
                }

#if 0
                if (pOndemand->stat.nCount == 0 || pOndemand->stat.nRecvCount == 0)
                     nElapse = 0;
                else nElapse = pOndemand->stat.nResponseTime / pOndemand->stat.nRecvCount;
#endif
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
// CPulseBaseParser Operations
//////////////////////////////////////////////////////////////////////


#if 0
void CPulseBaseParser::OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
{
    return;
}

BOOL CPulseBaseParser::OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
    return TRUE;
}
#endif
