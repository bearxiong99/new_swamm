//////////////////////////////////////////////////////////////////////
//
//  repeater.cpp: implementation of the CRepeaterParser class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
//  http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "codiapi.h"
#include "Chunk.h"
#include "Event.h"
#include "MeterUploader.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "ParserFireAlarm.h"

//////////////////////////////////////////////////////////////////////
// CFireAlarmParser Class
//////////////////////////////////////////////////////////////////////

#if     defined(__PATCH_4_1647__)
extern BOOL    *m_pbPatch_4_1647;
#endif

#define DEFAULT_METERING_OPTION        REPEATER_OPTION_READ_FULL_SOLAR_LOG

static const char *parserNameArray[] = {"SMOKE&FIRE-ALARM", NULL};

//////////////////////////////////////////////////////////////////////
// CFireAlarmParser Class
//////////////////////////////////////////////////////////////////////

CFireAlarmParser::CFireAlarmParser() : CMeterParser(SENSOR_TYPE_REP, SERVICE_TYPE_ELEC,
	ENDDEVICE_ATTR_REPEATER | ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL,
	const_cast<char **>(parserNameArray), PARSER_TYPE_FIREALARM, "SMOKE&FIRE-ALARM Parser", "NURITELECOM", parserNameArray[0],
	6000, 6000, 2)
{
}

CFireAlarmParser::~CFireAlarmParser()
{
}

//////////////////////////////////////////////////////////////////////
// CFireAlarmParser Operations
//////////////////////////////////////////////////////////////////////

int CFireAlarmParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
    return FIREALARM_MAIN(pCommand, pStat);
}

int CFireAlarmParser::FIREALARM_MAIN(CMDPARAM *pCommand, METER_STAT *pStat)
{
	ONDEMANDPARAM	*pOndemand = NULL;
	char			szGUID[20];
	EUI64			id;
	int				nRetry;
	int				nState, nSaveState;
	int				nError=0;
	int				nResult;
	time_t			tmStart, tmEnd;
	CChunk			*pChunk;
	CChunk			dataChunk(512);
	BOOL			bConnected = FALSE;
	char			szBuffer[1024];
	WORD			addr;
	int				len, n;
	BOOL			bDone = FALSE;
	ENDI_NODE_INFO  *nodeInfo;
	REPEATER_METERING_DATA mdata;
	time_t          nCurrentTime/*, nTmpTime*/;
	struct tm       *pDay;
	BYTE            nAttr = 0;

    memcpy(&id, &pCommand->id, sizeof(EUI64));      // Sensor ID
	pChunk	= pCommand->pChunk;			// 검침데이터를 리턴할 Chunk

	time(&nCurrentTime);
	pDay = localtime(&nCurrentTime);

	pDay->tm_hour	= pDay->tm_min = pDay->tm_sec = 0;
	nCurrentTime	= mktime(pDay);

	if (codiGetState(pCommand->codi) != CODISTATE_NORMAL)
		return ONDEMAND_ERROR_NOT_CONNECT;

	nSaveState = m_pEndDeviceList->GetState(&id);
	if (nSaveState == ENDISTATE_INIT)
		return ONDEMAND_ERROR_INIT;

	m_pEndDeviceList->SetBusy(&id, TRUE);

	nResult = ONDEMAND_ERROR_NOT_CONNECT;
	nState	= STATE_OPEN;
	//
	eui64toa(&id, szGUID);

	for(nRetry=0; (nRetry<GetRetryCount()) && (nState!=STATE_DONE);)
	{
		switch(nState) 
		{
		case STATE_OPEN :
			time(&tmStart);
			//
			LockOperation();
			pOndemand = FindOperation(&id);
			if (pOndemand != NULL)
			{
				UnlockOperation();
				m_pEndDeviceList->SetBusy(&id, FALSE);
				return ONDEMAND_ERROR_BUSY;
			} 
			pOndemand = NewOperation(ONDEMAND_OPR_DATA, &id, 1024);
			pOndemand->pParser = (void *)this;
			UnlockOperation();

			bConnected = FALSE;
			nError = endiOpen(pCommand->codi, (HANDLE *)&pOndemand->endi, &id);
			if (nError != CODIERR_NOERROR)
			{   
				XDEBUG("endiOpen Error(%d): %s\r\n", nError, codiErrorMessage(nError));
				nState = STATE_RETRY;
				break;
			}

			endiSetOption(pOndemand->endi, ENDIOPT_SEQUENCE, 1);

            nError = sensorConnect(pOndemand->endi, GetConnectionTimeout(ENDDEVICE_ATTR_POWERNODE));
			if (nError != CODIERR_NOERROR)
			{
				XDEBUG("endiConnect Error(%d): %s\r\n", nError, codiErrorMessage(nError));
				nState = STATE_RETRY;
				break;
			}

			gettimeofday((struct timeval *)&pOndemand->tmStart, NULL);
			gettimeofday((struct timeval *)&pOndemand->tmConnect, NULL);
			gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);

			bConnected	= TRUE;
			nState		= STATE_METERING;
			break;

		// 검침 초기화
		case STATE_METERING :
			pChunk->Rollback();							// Chunk 버퍼의 길이를 자리로 복원한다.

			memset(&mdata, 0, sizeof(REPEATER_METERING_DATA));
			mdata.infoLength = sizeof(REPEATER_METERING_DATA) - sizeof(BYTE) - sizeof(BYTE);

			if(!UpdatePowerType(&id, &mdata, &nAttr)) 
			{
				XDEBUG("UpdatePowerType Error : %s\r\n", szGUID);
			}

			gettimeofday((struct timeval *)&pOndemand->tmConnect, NULL);
			gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);

			pOndemand->nRetry	= 0;
			nState				= STATE_READ_NODEINFO;

			break;

		// Read NODE_INFO
		case STATE_READ_NODEINFO :
			addr = ENDI_ROMAP_NODE_KIND;
			len  = sizeof(ENDI_NODE_INFO);

			pOndemand->stat.nCount ++;
			pOndemand->stat.nSendCount ++;
			pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
			gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
			//
			nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &n, GetReplyTimeout());
			pOndemand->stat.nCount ++;
			if (nError != CODIERR_NOERROR)
			{
				if (pOndemand->nRetry < GetRetryCount())				// 3회 이상 연속 실패시 종료
				{
					pOndemand->nRetry++;
					nState = STATE_READ_NODEINFO;
				}
				else
				{
					nState = STATE_OK;   
				}

				pOndemand->stat.nError ++;
				break;
			}
			pOndemand->stat.nRecvCount ++;

			pOndemand->nRetry = 0;
			UpdateStatistics(pOndemand, (BYTE *)szBuffer, n);
			pOndemand->stat.nRecv += n + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);

			nodeInfo = (ENDI_NODE_INFO *) szBuffer;

			mdata.fwVersion			= nodeInfo->FIRMWARE_VERSION;
			mdata.fwBuild			= nodeInfo->FIRMWARE_BUILD;
			mdata.swVersion			= nodeInfo->SOFTWARE_VERSION;
			mdata.hwVersion			= nodeInfo->HARDWARE_VERSION;
			mdata.resetReason		= nodeInfo->RESET_REASON;
			//
			mdata.solarAdVolt		= nodeInfo->SOLAR_AD_VOLT;;
			mdata.solarChgBattVolt	= nodeInfo->SOLAR_CHG_BATT_VOLT;;
			mdata.solarBoardDcVolt	= nodeInfo->SOLAR_B_DC_VOLT;;

			bDone	= TRUE;
			nState	= STATE_OK;

			break;	 

		case STATE_OK :
			endiDisconnect(pOndemand->endi);
			gettimeofday((struct timeval *)&pOndemand->tmEnd, NULL);
			time(&tmEnd);

			if (bDone == TRUE)
			{
				nResult = ONDEMAND_ERROR_OK;

				UpdateMeteringData(&id, pCommand, &mdata, &dataChunk);

				XDEBUG("\r\n");
				XDEBUG("------------------[ OK ]-----------------\r\n");
				XDEBUG("   Metering Data Size = %d byte(s)\r\n", pChunk->GetSize()-pChunk->GetCommitSize());
			}
			else													// 검침 실패
			{
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
			//XDEBUG("   Error Packet Count = %d frams(s)\r\n", pOndemand->stat.nError);
			XDEBUG("           Total Send = %d byte(s)\r\n", pOndemand->stat.nSend);
			XDEBUG("        Total Receive = %d byte(s)\r\n", pOndemand->stat.nRecv);
#if 0
			XDEBUG("  Total Response Time = %d ms\r\n", pOndemand->stat.nResponseTime);
			XDEBUG("Average Response Time = %d ms\r\n", nElapse);
			XDEBUG("    Min Response Time = %d ms\r\n", pOndemand->stat.nMinTime);
			XDEBUG("    Max Response Time = %d ms\r\n", pOndemand->stat.nMaxTime);
#endif
			XDEBUG("       Total Use Time = %d ms\r\n", pOndemand->stat.nUseTime);
			XDEBUG("         Connect Time = %d ms\r\n", pOndemand->stat.nConnectTime);
			XDEBUG("-----------------------------------------\r\n");
			XDEBUG("\r\n");

			endiClose(pOndemand->endi);

			LockOperation();
			DeleteOperation(pOndemand);							// 할당 영역 해제
            pOndemand = NULL;
			UnlockOperation();
			//
			nState = STATE_DONE;								// 종료
			break;

		case STATE_RETRY :										// 재시도
			if (pOndemand->endi != NULL)
			{
				if (bConnected) endiDisconnect(pOndemand->endi);
				endiClose(pOndemand->endi);
				pOndemand->endi = NULL;
			}

			LockOperation();
			DeleteOperation(pOndemand);							// 할당된 영역 해제
            pOndemand = NULL;
			UnlockOperation();

			nRetry++;											// 재시도 횟수 계산
			if (nRetry >= GetRetryCount())										// 재시도가 3회 초과 확인
			{
				nState = STATE_DONE;								// 검침 실패
				break;
			}
			usleep(1000000);										// 다음 재시도까지 1초 대기
			nState = STATE_OPEN;									// 처음으로 이동
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

BOOL CFireAlarmParser::UpdatePowerType(EUI64 * id, REPEATER_METERING_DATA * pMeterData, BYTE *nAttr)
{
	ENDIENTRY   *pEndDevice;

	if( (pEndDevice = m_pEndDeviceList->GetEndDeviceByID(id))==NULL ) 
	{
		return FALSE;
	}

	*nAttr = pEndDevice->nAttribute;
	pMeterData->powerType = pEndDevice->nAttribute & ENDDEVICE_ATTR_POWERNODE ? 0x01 : (
		pEndDevice->nAttribute & ENDDEVICE_ATTR_SOLARNODE ? 0x04 : (
		pEndDevice->nAttribute & ENDDEVICE_ATTR_BATTERYNODE ? 0x02 : 0x00
		)
		);

	return TRUE;
}

void CFireAlarmParser::UpdateMeteringData(EUI64 * id, CMDPARAM *pCommand, REPEATER_METERING_DATA * pMeterData, CChunk *pChunk)
{
	WORD tmpV;

	m_pEndDeviceList->UpdateSensorInventory(id, NULL, PARSER_TYPE_NOT_SET, NULL, 
		(((WORD) (pMeterData->hwVersion & 0xf0)) << 4) + (pMeterData->hwVersion & 0x0f),
		(((WORD) (pMeterData->fwVersion & 0xf0)) << 4) + (pMeterData->fwVersion & 0x0f),
		(((WORD) (pMeterData->fwBuild >> 4)) * 10) + (pMeterData->fwBuild & 0x0f));

	m_pEndDeviceList->SetRouteDeviceInfo(id, pMeterData->permitMode ? TRUE : FALSE, 
		pMeterData->permitState ? TRUE : FALSE, 0xFF, 0xFF);

	m_pEndDeviceList->SetEndDeviceFlag(id, ntoshort(pMeterData->alarmMask), 
		pMeterData->alarmFlag, pMeterData->testFlag);

	/** Issue #2567: Repeater는 Solar AD/ BATT/ VCC 값을 저장한다 */
	m_pEndDeviceList->ClearValue(id);
	tmpV = ntoshort(pMeterData->solarBoardDcVolt); m_pEndDeviceList->AddValue(id, const_cast<char *>("SOLARVCC"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpV);
	tmpV = ntoshort(pMeterData->solarChgBattVolt); m_pEndDeviceList->AddValue(id, const_cast<char *>("SOLARCHG"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpV);
	tmpV = ntoshort(pMeterData->solarAdVolt); m_pEndDeviceList->AddValue(id, const_cast<char *>("SOLARAD"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpV);

	pCommand->pChunk->Add((char *)pMeterData, sizeof(REPEATER_METERING_DATA));
	pCommand->pChunk->Add(pChunk->GetBuffer(), pChunk->GetSize());
}
