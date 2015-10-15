#include <dirent.h>
#include "common.h"
#include "AgentService.h"
#include "EndDeviceList.h"
#include "MeterDeviceList.h"
#include "InventoryScanner.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "InventoryHash.h"
#include "OtaWorker.h"
#include "Patch.h"

#include "rep/MeterSecurityHelper.h"

extern BOOL m_bSuperParent;
#if     defined(__PATCH_7_2097__)
extern BOOL    *m_pbPatch_7_2097;
#endif
#if     defined(__PATCH_11_65__)
/**< Issue #65 JEJU SP */
extern BOOL    *m_pbPatch_11_65;
#endif

CEndDeviceList   *m_pEndDeviceList = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEndDeviceList::CEndDeviceList()
{
	m_pEndDeviceList = this;
}

CEndDeviceList::~CEndDeviceList()
{
}

//////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////

int CEndDeviceList::GetCount()
{
	return m_List.GetCount();
}

BOOL CEndDeviceList::IsBusy(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;
	BOOL		bBusy = FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL) bBusy = pEndDevice->bBusy;
	m_Locker.Unlock();
	return bBusy;
}

BOOL CEndDeviceList::IsModified(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;
	BOOL		bModified = FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL) bModified = pEndDevice->bModified;
	m_Locker.Unlock();
	return bModified;
}

BOOL CEndDeviceList::SetBusy(const EUI64 *id, BOOL bBusy)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL) pEndDevice->bBusy = bBusy;
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BYTE CEndDeviceList::GetState(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;
	BYTE		nState = ENDISTATE_ERROR;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL) nState = pEndDevice->nState;
	m_Locker.Unlock();
	return nState;
}

BOOL CEndDeviceList::SetState(const EUI64 *id, BYTE nState)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
		pEndDevice->nState = nState;
		pEndDevice->bModified = TRUE;
		ceTime(&pEndDevice->tmChange);
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::CommitMeteringDate(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(pEndDevice->nLastTempMeteringDate && pEndDevice->nLastMeteringDate != pEndDevice->nLastTempMeteringDate) {
            pEndDevice->nLastMeteringDate = pEndDevice->nLastTempMeteringDate;
		    pEndDevice->bModified = TRUE;
        }
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::SetTempMeteringDate(const EUI64 *id, WORD year, BYTE month, BYTE day)
{
	ENDIENTRY	*pEndDevice;
    UINT        lDate = 0x00;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        pEndDevice->nLastTempMeteringDate = ((((lDate | year) << 8) | month) << 8) | day;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::GetMeteringDate(const EUI64 *id, UINT *date, WORD *year, BYTE *month, BYTE *day)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
    {
        if(date) *date = pEndDevice->nLastMeteringDate;
        if(year) *year = (pEndDevice->nLastMeteringDate >> 16) & 0xFFFF;
        if(month) *month = (pEndDevice->nLastMeteringDate >> 8) & 0xFF;
        if(day) *day = pEndDevice->nLastMeteringDate & 0xFF;
    }
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::GetLastMeteringTime(const EUI64 *id, time_t *t)
{
	ENDIENTRY	*pEndDevice;

	if (t == NULL)
		return FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL) *t = pEndDevice->tmMetering;
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::SetLastMeteringTime(const EUI64 *id, time_t t)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
		pEndDevice->tmMetering = t;
		pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::GetLastMeteringFailTime(const EUI64 *id, time_t *t)
{
	ENDIENTRY	*pEndDevice;

	if (t == NULL)
		return FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL) *t = pEndDevice->tmMeteringFail;
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::SetInstallState(const EUI64 *id, BYTE state)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL && pEndDevice->installState != state)
	{
		pEndDevice->installState = state;
        ceTime(&pEndDevice->tmLastFwUpdateTime);
		pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::SetLastMeteringFailTime(const EUI64 *id, time_t t)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
		pEndDevice->tmMeteringFail = t;
		pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::GetQuality(const EUI64 *id, BYTE *LQI, signed char *RSSI)
{
	ENDIENTRY	*pEndDevice;
    BOOL        isOk = FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
		*LQI = pEndDevice->LQI;
		*RSSI = pEndDevice->RSSI;
        isOk = TRUE;
	}
	m_Locker.Unlock();
	return isOk;
}

BOOL CEndDeviceList::SetQuality(const EUI64 *id, BYTE LQI, signed char RSSI)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL && (LQI || RSSI) && (pEndDevice->LQI != LQI || pEndDevice->RSSI != RSSI))
	{
		pEndDevice->LQI = LQI;
		pEndDevice->RSSI = RSSI;
	    ceTime(&pEndDevice->tmRouteRecord);
		pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::GetEnergyLevel(const EUI64 *id, BYTE *pEnergyLevel, UINT *pRelayStatus)
{
	ENDIENTRY	*pEndDevice;
    BOOL        isOk = FALSE;

    if(id == NULL || pEnergyLevel == NULL) return isOk;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
		*pEnergyLevel = pEndDevice->nEnergyLevel;
        isOk = TRUE;
        if(pRelayStatus != NULL)
        {
            *pRelayStatus = pEndDevice->nRelayStatus;
        }
	}
	m_Locker.Unlock();
	return isOk;
}

/** Device Energy Level 변경.
 *
 * 만약 이전 값과 다를 경우 EnergyLevelChangeEvent(203.109) 발생.
 *
 * @param id                EUI64 ID
 * @param nEnergyLevel      Energy Level
 * @param nRelayStatus      Relay Status
 * @param pszCallback       Callback data
 * @param nCallbackLength   Callback data length
 *
 */
BOOL CEndDeviceList::SetEnergyLevel(const EUI64 *id, BYTE nEnergyLevel, UINT nRelayStatus, BYTE *pszCallback, int nCallbackLength)
{
	ENDIENTRY	*pEndDevice;
    BOOL        isOk = FALSE;
    BYTE        nPrevLevel=0;
    cetime_t    nIssue;

    if(id == NULL) return isOk;

    ceTime(&nIssue);
	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL && (pEndDevice->nEnergyLevel != nEnergyLevel))
	{
        nPrevLevel = pEndDevice->nEnergyLevel;
		pEndDevice->nEnergyLevel = nEnergyLevel;
		pEndDevice->tmLevelChange = nIssue;
		pEndDevice->bModified = TRUE;
        isOk = TRUE;
	}
	if (pEndDevice != NULL && (pEndDevice->nRelayStatus != nRelayStatus))
	{
		pEndDevice->nRelayStatus = nRelayStatus;
		pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();

    if(isOk)
    {
/** 제주실증단지 DR Level 적용 Patch */
#if     defined(__PATCH_11_65__)
        if(*m_pbPatch_11_65) {
            BYTE nJejuPrev = Patch_11_65_Nuri2Jeju(nPrevLevel);
            BYTE nJejuCurr = Patch_11_65_Nuri2Jeju(nEnergyLevel);
            if(nJejuPrev != nJejuCurr) 
            {
                /** Jeju Level에서 변경이 있을 경우에만 전송 */
                energyLevelChangeEvent(id, nIssue, IF4ERR_NOERROR, 
                        nJejuPrev, nJejuCurr, pszCallback, nCallbackLength);
            }
        }else
#endif
        {
            /** Energy Level이 변경되었으면 자동으로 Event를 전송한다. */
            energyLevelChangeEvent(id, nIssue, IF4ERR_NOERROR, 
                    nPrevLevel, nEnergyLevel, pszCallback, nCallbackLength);
        }

    }
	return isOk;
}

/** Device Energy Level 변경.
 *
 * 만약 이전 값과 다를 경우 EnergyLevelChangeEvent(203.109) 발생.
 *
 * @param id                EUI64 ID
 * @param nEnergyLevel      Energy Level
 * @param nRelayStatus      Relay Status
 *
 */
BOOL CEndDeviceList::SetEnergyLevel(const EUI64 *id, BYTE nEnergyLevel, UINT nRelayStatus)
{
    return SetEnergyLevel(id, nEnergyLevel, nRelayStatus, NULL, 0);
}

/** Route Device에 대한 추가 정보를 Update 한다.
 *
 * @param id                EUI64 ID
 * @param bPermitMode       Automatic Permit control(Enable/Disable)
 * @param bPermitState      Zigbee Permit (On/Off)
 * @param nMaxChildTbl      MAX Child Table Count
 * @param nChildTbl         Current Child Table Count
 *
 */
BOOL CEndDeviceList::SetRouteDeviceInfo(const EUI64 *id, BOOL bPermitMode, BOOL bPermitState, BYTE nMaxChildTbl, BYTE nChildTbl) 
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL && (pEndDevice->bPermitMode != bPermitMode 
            || pEndDevice->bPermitState != bPermitState
            || (nMaxChildTbl != 0xFF && pEndDevice->nMaxChildTbl != nMaxChildTbl)
            || (nChildTbl != 0xFF && pEndDevice->nChildTbl != nChildTbl)
    ))
	{
	    pEndDevice->bPermitMode = bPermitMode;
        pEndDevice->bPermitState = bPermitState;
        if(nMaxChildTbl != 0xFF) pEndDevice->nMaxChildTbl = nMaxChildTbl;
        if(nChildTbl != 0xFF) pEndDevice->nChildTbl = nChildTbl;

	    ceTime(&pEndDevice->tmRouteDevice);
		pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

/** Solar System에 대한 정보를 Update 한다
 *
 * @param id                EUI64 ID
 * @param nSolarAdVolt      Solar Volt (little endian)
 * @param nSolarChgBattVolt Solar Charge Battery Volt (little endian)
 * @param nSolarBoardVolt   Solar Board Volt (little endian)
 * @param bSolarChgState    Solar Battery Charging State
 *
 */
BOOL CEndDeviceList::SetSolarDeviceInfo(const EUI64 *id, WORD nSolarAdVolt, WORD nSolarChgBattVolt, WORD nSolarBoardVolt, signed char nSolarChgState)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL && (pEndDevice->nSolarAdVolt != nSolarAdVolt
            || pEndDevice->nSolarChgBattVolt != nSolarChgBattVolt
            || pEndDevice->nSolarBoardVolt != nSolarBoardVolt
            || (nSolarChgState >= 0 && pEndDevice->nSolarChgState != nSolarChgState)
    ))
	{
        pEndDevice->nSolarAdVolt = nSolarAdVolt;
        pEndDevice->nSolarChgBattVolt = nSolarChgBattVolt;
        pEndDevice->nSolarBoardVolt = nSolarBoardVolt;
        if(nSolarChgState >= 0) pEndDevice->nSolarChgState = nSolarChgState;

	    ceTime(&pEndDevice->tmSolarDevice);
		pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::GetLastEventLogTime(const EUI64 *id, time_t *t)
{
	ENDIENTRY	*pEndDevice;

	if (t == NULL)
		return FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL) *t = pEndDevice->tmEventLog;
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::SetLastEventLogTime(const EUI64 *id, time_t t)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
		pEndDevice->tmEventLog = t;
		pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

int CEndDeviceList::GetSensorAttribute(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;
	int		    nAttr = 0x00;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL) nAttr = pEndDevice->nAttribute;
	m_Locker.Unlock();
	return nAttr;
}

/** Metering에 관련된 Attribute를 초기화 한다.
 */
#if 0
BOOL CEndDeviceList::ResetMeteringAttribute(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        pEndDevice->lpIndex = 0;
        pEndDevice->nTotalSize = 0;
        pEndDevice->nCurrentSize = 0;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}
#endif

/** Pushe Metering에 필요한 Size 값을 설정한다.
 */
BOOL CEndDeviceList::SetMeteringDataSize(const EUI64 *id, UINT nTotalSize, BYTE nCurrentFrameSize)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        pEndDevice->nTotalSize = nTotalSize;
        //pEndDevice->nCurrentSize = nCurrentFrameSize;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

/** Metering에 관련된 Attribute를 재설정 한다.
 * 이 때 필요한 정보를 Clear 한다.
 */
BOOL CEndDeviceList::SetMeteringAttribute(const EUI64 *id, int nPortNum, BYTE lastMask, BYTE period, BYTE choice, signed char alarmFlag)
{
	ENDIENTRY	*pEndDevice;
    BOOL        bDbUpdate = FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(pEndDevice->lastDataMask != lastMask) {
		    pEndDevice->lastDataMask = lastMask;
			pEndDevice->bModified = TRUE;
        }
        if(period != 0 && pEndDevice->lpPeriod != period) {
		    pEndDevice->lpPeriod = period;
			pEndDevice->bModified = TRUE;
            bDbUpdate = TRUE;
        }
        pEndDevice->lpIndex = 0;
        pEndDevice->nTotalSize = 0;
        pEndDevice->nCurrentSize = 0;
        if(pEndDevice->lpChoice != choice) {
		    pEndDevice->lpChoice = choice;
            /*-- Issue #1538: LP Choice가 변경되면 기존 검침 시간을 Clear 한다 
             *-- Issue #2072: LastMeteringDate 값을 추가하여 tmMetering을 초기화 하던 코드를 제거한다.
		     * pEndDevice->tmMetering = 0;
            --*/
			pEndDevice->bModified = TRUE;
	    }
        if(alarmFlag >=0 && pEndDevice->nAlarmFlag != alarmFlag) {
            pEndDevice->nAlarmFlag = alarmFlag;
			pEndDevice->bModified = TRUE;
        }
	}
    
	m_Locker.Unlock();

	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::SetPulseSensorLpTimePulse(const EUI64 *id, TIMEDATE * timedate, UINT basePulse)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(memcmp(&pEndDevice->lpTimeDate, timedate, sizeof(TIMEDATE))) {
            memcpy(&pEndDevice->lpTimeDate, timedate, sizeof(TIMEDATE));
			pEndDevice->bModified = TRUE;
        }
        if(pEndDevice->lpBasePulse != basePulse) {
            pEndDevice->lpBasePulse = basePulse;
            pEndDevice->bModified = TRUE;
        }
	}
    
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::IsValidLpIndex(const EUI64 *id, BYTE idx, UINT nPayloadSize, BYTE *prevIdx, BOOL *isLast)
{
	ENDIENTRY	*pEndDevice;
    BOOL        isValid = FALSE;
    BOOL        last = FALSE;
    BYTE        nCurrIdx = idx, nExpIdx;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        /** Sequence Validation Check */
		if(prevIdx) *prevIdx = pEndDevice->lpIndex;
        if(idx == 0 && pEndDevice->lpIndex == 0) {
            /* 최소 무조건 Add 되지 않고 IsValidLpIndex를 거치는 경우(Push) 예외 처리를 위해 idx 0
             * lpIndex 0 일 때 ExpIdx를 0 으로 설정한다.
             */
            nExpIdx = 0;        
        } else {
            nExpIdx = pEndDevice->lpIndex + 1;
        }
        if(pEndDevice->lastDataMask & LAST_DATA_MASK_MSB)
        {
            nCurrIdx = idx & 0x7F;
            if(nExpIdx > 0x7F) nExpIdx = 0;
        }

        //XDEBUG(" REWRITER nCurrIdx %d, nExpIdx %d\r\n", nCurrIdx, nExpIdx);
        if(nCurrIdx == nExpIdx) {
            isValid = TRUE;
            pEndDevice->lpIndex = nCurrIdx;
        }
        //XDEBUG(" REWRITER isValid %d\r\n", isValid);

        /** Last Frame Check */
        if(pEndDevice->lastDataMask & LAST_DATA_MASK_LPPERIOD)
        {
		    if(pEndDevice->lpPeriod == nCurrIdx) last = TRUE;
        }
        if(pEndDevice->lastDataMask & LAST_DATA_MASK_MSB)
        {
		    if(idx & 0x80) last = TRUE;
        }
        /** 아직 지원하지 않는다.
        if(pEndDevice->lastDataMask & LAST_DATA_MASK_LSB)
        {
		    if(idx & 0x01) last = TRUE;
        }
        */
        /** Issue #84 Push Metering을 위해서 구현한다 */
        if(pEndDevice->lastDataMask & LAST_DATA_MASK_SIZE)
        {
            //XDEBUG(" REWRITER Size Check last %d\r\n", last);
            if(isValid) {   // 잘못된 sequence에 대한 검사는 하지 않는다.
                //XDEBUG(" REWRITER nCurrentSize %d, nPayloadSize %d\r\n", pEndDevice->nCurrentSize, nPayloadSize);
                pEndDevice->nCurrentSize += nPayloadSize;
                //XDEBUG(" REWRITER nCurrentSize %d, nTotalSize %d\r\n", pEndDevice->nCurrentSize, pEndDevice->nTotalSize);
                if(pEndDevice->nTotalSize <= pEndDevice->nCurrentSize) {
		            last = TRUE;
                }
                //XDEBUG(" REWRITER last %d\r\n", last);
            }
        }
	}
	m_Locker.Unlock();
    if(isLast) *isLast = last;

	return isValid;
}

/** Issue #1538: Pulse Sensor에서 전달되는 검침 데이터를 저장할 필요가 있는지 검사한다.
 *
 *  검사 원리는 다음과 같다.
 * 
 *   1) LP Choice가 0(당일) 이라면 항상 저장한다
 *   2) LP Choice가 0이 아니라면 마지막 Metering Date와 같다면 저정하지 않는다.
 */
BOOL CEndDeviceList::IsDuplicateLpData(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;
    BOOL        isDup = FALSE;

#if     defined(__PATCH_7_2097__)
    if(*m_pbPatch_7_2097) return FALSE;
#endif

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(pEndDevice->lpChoice) {
            if(pEndDevice->nLastMeteringDate && (pEndDevice->nLastMeteringDate == pEndDevice->nLastTempMeteringDate)) {
                isDup = TRUE;
            }
        }
	}
	m_Locker.Unlock();
	return isDup;
}

BOOL CEndDeviceList::IsDuplicateLpData(const EUI64 *id, WORD year, BYTE month, BYTE day)
{
	ENDIENTRY	*pEndDevice;
    BOOL        isDup = FALSE;
    UINT        lDate = 0x00;;

#if     defined(__PATCH_7_2097__)
    if(*m_pbPatch_7_2097) return FALSE;
#endif

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(pEndDevice->lpChoice) {
            if(pEndDevice->nLastMeteringDate == (((((lDate | year) << 8) | month) << 8) | day)) {
                isDup = TRUE;
            }
        }
	}
	m_Locker.Unlock();
	return isDup;
}


ENDIENTRY *CEndDeviceList::SetEndDeviceByID(const EUI64 *id, int nPortNum, 
        const char *pszNodeKind, CMeterParser *pParser, const char *pszParser, 
        UINT nParserType, int nAttribute)
{
	ENDIENTRY 	*pEndDevice, *pParentEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
		if (pszParser && (strcmp(pEndDevice->szParser, pszParser) != 0))
		{
			strcpy(pEndDevice->szParser, pszParser);
			pEndDevice->bModified = TRUE;
		}
		if (pszNodeKind && (strcmp(pEndDevice->szNodeKind, pszNodeKind) != 0))
		{
			strcpy(pEndDevice->szNodeKind, pszNodeKind);
			pEndDevice->bModified = TRUE;
		}
		if (nParserType != PARSER_TYPE_NOT_SET && (pEndDevice->nParser != nParserType))
		{
            pEndDevice->nParser = nParserType;
			pEndDevice->bModified = TRUE;
		}
		if (pEndDevice->nAttribute != nAttribute)
		{
			pEndDevice->nAttribute = nAttribute;
			pEndDevice->bModified = TRUE;
		}
	}
	else
	{
		pEndDevice = (ENDIENTRY *)MALLOC(sizeof(ENDIENTRY));
		if (pEndDevice != NULL)
		{
			memset(pEndDevice, 0, sizeof(ENDIENTRY));
			memcpy(&pEndDevice->id, id, sizeof(EUI64));

            if(pszNodeKind != NULL)
            {
			    strcpy(pEndDevice->szNodeKind, pszNodeKind);
            }

            /** 기본 Service Type 설정 */
            if(pParser != NULL)
            {
                pEndDevice->nServiceType = pParser->GetService();
            }
            else
            {
                pEndDevice->nServiceType = SERVICE_TYPE_UNKNOWN;
            }

            if(pszParser != NULL)
            {
			    strcpy(pEndDevice->szParser, pszParser);
            }
            pEndDevice->nParser = nParserType;

            if(m_pInventoryHash->GetAndRemove(id, &pEndDevice->parentid, &pEndDevice->shortid)) {
                if (nAttribute & ENDDEVICE_ATTR_RFD) {
                    /*-- Issue #1973: parentid와 shortid만 설정되고 hop은 설정 안되는 문제 수정 --*/
                    if((pParentEndDevice=FindEndDeviceByShortID(pEndDevice->parentid))!=NULL) {
                        pEndDevice->hops = pParentEndDevice->hops + 1;
                    }
                }
            }

            pEndDevice->magic = ENDIENTRY_MAGIC_NUMBER;
            pEndDevice->nAlarmFlag = -1; // Issue #1864 : 최초 값은 -1(Undefined)
			pEndDevice->nAttribute	= nAttribute;
			pEndDevice->nState		= ENDISTATE_INIT;
            strcpy(pEndDevice->szSerial,"");
			ceTime(&pEndDevice->tmInstall);			
			pEndDevice->nPosition	= (int)m_List.AddTail(pEndDevice);
			m_Hash.Update(id, sizeof(EUI64), pEndDevice->nPosition);
            if(pEndDevice->shortid) { 
			    m_ShortIdHash.Update(&pEndDevice->shortid, sizeof(WORD), pEndDevice->nPosition);
            }
			SaveEndDevice(pEndDevice);
		}
	}
	m_Locker.Unlock();

    if(pEndDevice) {
        if(pParser){
            pParser->PostCreate(id, nPortNum);
        }
    }
	return pEndDevice;
}

BOOL CEndDeviceList::UpdateSensorInventory(const EUI64 *id, const char *pszParser, UINT nParserType, 
        const char *pszModel, WORD hw, WORD fw, WORD build)
{
	ENDIENTRY 	*pEndDevice;
    char        *stripedStr;
    BOOL        bFwUpdate = FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(pszParser != NULL) {
		    if (strcmp(pEndDevice->szParser, pszParser) != 0)
		    {
			    strcpy(pEndDevice->szParser, pszParser);
			    pEndDevice->bModified = TRUE;
		    }
        }

        if(nParserType != PARSER_TYPE_NOT_SET)
        {
            if(pEndDevice->nParser != nParserType)
            {
                pEndDevice->nParser = nParserType;
			    pEndDevice->bModified = TRUE;
            }
        }

        if(pszModel != NULL) {
		    if((stripedStr = strip(pszModel)) != NULL) {
		        if (strcmp(pEndDevice->szModel, stripedStr) != 0)
		        {
			        strcpy(pEndDevice->szModel, stripedStr);
			        pEndDevice->bModified = TRUE;
		        }
            }
            else
            {
                XDEBUG("FREE stripedStr Memory !!!\r\n");
                FREE(stripedStr);
            }
        }

		if (pEndDevice->hw != hw)
		{
			pEndDevice->hw	= hw;
			pEndDevice->bModified = TRUE;
		}

		if (pEndDevice->fw != fw)
		{
			pEndDevice->fw = fw;
			pEndDevice->bModified = TRUE;
            bFwUpdate = TRUE;
		}

		if (pEndDevice->build != build)
		{
			pEndDevice->build = build;
			pEndDevice->bModified = TRUE;
            bFwUpdate = TRUE;
		}

        if(bFwUpdate) {
		    pEndDevice->installState = OTA_STEP_INIT;
            ceTime(&pEndDevice->tmLastFwUpdateTime);
        }
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

// M -bus 처럼 Group에만 적용한다. 
BOOL CEndDeviceList::UpdateSubInventory(const EUI64 *id, const char *pszModel, WORD fw, WORD build)
{
	ENDIENTRY 	*pEndDevice;
    BOOL        bFwUpdate = FALSE;
    char        *stripedStr;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        pEndDevice->nGroup = 1;             // Group인것을 나타낸다. 
        if(pszModel != NULL) {
		    if((stripedStr = strip(pszModel)) != NULL) {
		        if (strcmp(pEndDevice->szSubModel, stripedStr) != 0)
		        {
			        strcpy(pEndDevice->szSubModel, stripedStr);
			        pEndDevice->bModified = TRUE;
                    bFwUpdate = TRUE;
		        }
                FREE(stripedStr);
            }
        }

		if (pEndDevice->subfw != fw)
		{
			pEndDevice->subfw = fw;
			pEndDevice->bModified = TRUE;
            bFwUpdate = TRUE;
		}

		if (pEndDevice->subbuild != build)
		{
			pEndDevice->subbuild = build;
			pEndDevice->bModified = TRUE;
            bFwUpdate = TRUE;
		}

        if(bFwUpdate) {
		    pEndDevice->installState = OTA_STEP_INIT;
            ceTime(&pEndDevice->tmLastFwUpdateTime);
        }
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::SetEndDeviceNetworkType(const EUI64 *id, BYTE networkType)
{
	ENDIENTRY	*pEndDevice;
    BOOL        isModified = FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if((networkType==NETWORK_TYPE_MESH) && 
                    (pEndDevice->nAttribute & ENDDEVICE_ATTR_RFD)) { // RFD -> FFD
            pEndDevice->nAttribute &= (ENDDEVICE_ATTR_RFD ^ 0xFFFFFFFF);
			pEndDevice->bModified = TRUE;
            isModified = TRUE;
        }else if ((networkType==NETWORK_TYPE_STAR) && 
                    !(pEndDevice->nAttribute & ENDDEVICE_ATTR_RFD)) { // FFD -> RFD
            pEndDevice->nAttribute |= ENDDEVICE_ATTR_RFD;
			pEndDevice->bModified = TRUE;
            isModified = TRUE;
        }
	}
	m_Locker.Unlock();
	return isModified;
}

BOOL CEndDeviceList::SetEndDeviceFlag(const EUI64 *id, WORD alarmMask, BOOL alarmFlag, BOOL testFlag)
{
	ENDIENTRY	*pEndDevice;
    signed char nalarm = alarmFlag ? 1 : 0;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(pEndDevice->nAlarmMask != alarmMask ) {
		    pEndDevice->nAlarmMask = testFlag;
			pEndDevice->bModified = TRUE;
        }
        if(pEndDevice->nAlarmFlag != nalarm ) {
		    pEndDevice->nAlarmFlag = nalarm;
			pEndDevice->bModified = TRUE;
        }
        if(pEndDevice->bTestFlag != testFlag ) {
		    pEndDevice->bTestFlag = testFlag;
			pEndDevice->bModified = TRUE;
        }
	}
    
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

/** Meter 정보 Update.
  *
  * nVendor가 0보다 크면 값이 설정되고 음수면 무시된다.
  * nVendor의 값이 255보다 클 경우에도 무시된다.
  */
BOOL CEndDeviceList::UpdateMeterInventory(const EUI64 *id, int nPortNum, int nParser, const char *pszVendor, int nVendor, 
        const char *pszModel, const char *pszSerial, WORD hw, WORD fw)
{
	ENDIENTRY 	*pEndDevice;
    char        *stripedStr;
    char        *stripedSerial = NULL;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(pszSerial != NULL) {
		    if((stripedSerial = strip(pszSerial)) != NULL) {
		        if (strcmp(pEndDevice->szSerial, stripedSerial) != 0)
		        {
			        strcpy(pEndDevice->szSerial, stripedSerial);
			        pEndDevice->bModified = TRUE;
		        }
            }
        }

        if(pszVendor != NULL) {
		    if((stripedStr = strip(pszVendor)) != NULL) {
		        if (strcmp(pEndDevice->szVendor, stripedStr) != 0)
		        {
			        strcpy(pEndDevice->szVendor, stripedStr);
			        pEndDevice->bModified = TRUE;
		        }
                FREE(stripedStr);
            }
        }

        if(nVendor >= 0 && nVendor <= 255)
        {
            if(pEndDevice->nVendor != (BYTE)nVendor)
            {
                pEndDevice->nVendor = (BYTE)nVendor;
                pEndDevice->bModified = TRUE;
            }
        }

        if(pszModel != NULL) {
		    if((stripedStr = strip(pszModel)) != NULL) {
		        if (strcmp(pEndDevice->szModel, stripedStr) != 0)
		        {
			        strcpy(pEndDevice->szModel, stripedStr);
			        pEndDevice->bModified = TRUE;
		        }
                FREE(stripedStr);
            }
        }

		if (hw != 0 && pEndDevice->hwVersion != hw)
		{
			pEndDevice->hwVersion = hw;
			pEndDevice->bModified = TRUE;
		}

		if (fw != 0 && pEndDevice->fwVersion != fw)
		{
			pEndDevice->fwVersion = fw;
			pEndDevice->bModified = TRUE;
		}
	}
	m_Locker.Unlock();

    if(stripedSerial != NULL)
    {
        FREE(stripedSerial);
    }

	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::DeleteEndDeviceByID(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;
	char		szCommand[128];
	char		szGUID[20];

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        m_pMeterDeviceList->Remove(id);
		m_Hash.Delete(id, sizeof(EUI64));
		m_ShortIdHash.Delete(&pEndDevice->shortid, sizeof(WORD));
		m_List.RemoveAt((POSITION)pEndDevice->nPosition);

		if (m_bSuperParent)
			m_pInventoryHash->Remove(id);

		eui64toa(id, szGUID);
		sprintf(szCommand, "/app/member/%s.mbr", szGUID);
		unlink(szCommand);
		sprintf(szCommand, "%s/%s.value", ENDI_VALUE_PATH, szGUID);
		unlink(szCommand);
		FREE(pEndDevice);
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::DeleteEndDeviceAll()
{
	m_Locker.Lock();

	if (m_bSuperParent)
		m_pInventoryHash->RemoveAll();

	RemoveAll();
	m_List.RemoveAll();

	m_Locker.Unlock();
    m_pMeterDeviceList->RemoveAll();

	system("rm -rf /app/member/*");
	system("rm -rf /app/data/*");
    /*-- 너무 많은 파일이 있을 경우 delete error가 날것을 염려해서 Directory를 삭제한다 --*/
	system("rm -rf /app/member");
	system("rm -rf /app/data");
	system("rm -rf /app/value");

	mkdir("/app/member", 0777);
	mkdir("/app/data",  0777);
	mkdir("/app/value",  0777);

	return TRUE;
}

BOOL CEndDeviceList::VerifyEndDevice(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if ((pEndDevice != NULL))
    {
        if(pEndDevice->bModified)
        {
		    SaveEndDevice(pEndDevice);
        }
        if(pEndDevice->bValueModified)
        {
            SaveEndDeviceValue(pEndDevice);
        }
        /** Issue #2571: Memory 절약을 위해 pMibValue 값을 Free 한다.
          *
          * @Todo File에서 값을 읽는 부분의 성능 향상을 위해 추후에 last value를
          *     항상 저장하게 하는 것도 고려할 수 있다.
          */
        if(pEndDevice->pMibValue != NULL)
        {
            VARAPI_FreeNode(pEndDevice->pMibValue);
            pEndDevice->pMibValue = NULL;
        }
    }
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

ENDIENTRY *CEndDeviceList::GetEndDeviceByID(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	m_Locker.Unlock();
	return pEndDevice;
}

ENDIENTRY *CEndDeviceList::GetEndDeviceByShortID(WORD shortid)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByShortID(shortid);
	m_Locker.Unlock();
	return pEndDevice;
}

BOOL CEndDeviceList::CopyEndDeviceByID(const EUI64 *id, ENDIENTRY *endi)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice) memcpy(endi, pEndDevice, sizeof(ENDIENTRY));
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

int CEndDeviceList::EnumEndDevice(PFNENUMENDDEVICE pfnCallback, void *pParam, void *pParam1, void *pParam2,
        int (*compar)(const void *, const void *))
{
	ENDIENTRY	*pEndDevice;
	POSITION	pos = INITIAL_POSITION;
	int			nCount=0;
    int         nSize, idx;

	m_Locker.Lock();
    if(compar != NULL)
    {
        ENDIENTRY **ppEndiList;

        nSize = m_List.GetCount();
        if(nSize > 0)
        {
            ppEndiList = (ENDIENTRY **)MALLOC(sizeof(ENDIENTRY *) * nSize);
            memset(ppEndiList, 0, sizeof(ENDIENTRY *) * nSize);

	        pEndDevice = m_List.GetFirst(pos);
	        for(idx=0;pEndDevice!=NULL && idx < nSize;idx++, pEndDevice = m_List.GetNext(pos))
	        {
                ppEndiList[idx] = pEndDevice;
	        }
            qsort(ppEndiList, nSize, sizeof(ENDIENTRY *), compar);
            for(idx=0; idx < nSize; idx++)
            {
		        if(pfnCallback(ppEndiList[idx], pParam, pParam1, pParam2)) nCount++;
            }
            FREE(ppEndiList);
        }
    }
    else
    {
	    pEndDevice = m_List.GetFirst(pos);
	    while(pEndDevice)
	    {
		    if(pfnCallback(pEndDevice, pParam, pParam1, pParam2)) nCount++;
		    pEndDevice = m_List.GetNext(pos);
	    }
    }
	m_Locker.Unlock();
	return nCount;
}

/** nIncludeAttr 0x00 이 아닐 경우 nIncludeAttr과 And Mask가 되어야 하고
  * nExcludeAttr이 0x00이 아닐 경우 nExcludeAttr과 And Mask가 되지 않아야 목록에 포함된다.
  *
  * 만약 nIncludeAttr이 0x00일 경우 nExcludeAttr과 And Mask 되지 않으면 목록에 포함된다.
  */
EUI64 *CEndDeviceList::GetListByID(int *nItemCount, int nIncludeAttr, int nExcludeAttr)
{
	ENDIENTRY	*pEndDevice;
	EUI64		*pList;
	POSITION	pos = INITIAL_POSITION;
	int			nSeek, nCount=0;

	pList = NULL;
	*nItemCount = 0;

	m_Locker.Lock();
	pEndDevice = m_List.GetFirst(pos);
	while(pEndDevice)
	{
        if((!nIncludeAttr || (pEndDevice->nAttribute & nIncludeAttr)) &&
                !(pEndDevice->nAttribute & nExcludeAttr)) 
        {
			nCount++;
        }

		pEndDevice = m_List.GetNext(pos);
	}

	pList = (EUI64 *)MALLOC(sizeof(EUI64)*nCount);
	if (pList != NULL)
	{
		nSeek = 0;
		memset(pList, 0, sizeof(EUI64)*nCount);
		pEndDevice = m_List.GetFirst(pos);
		while(pEndDevice)
		{
            if((!nIncludeAttr || (pEndDevice->nAttribute & nIncludeAttr)) &&
                !(pEndDevice->nAttribute & nExcludeAttr)) 
			{
				memcpy(&pList[nSeek], &pEndDevice->id, sizeof(EUI64));
				nSeek++;
			}
			pEndDevice = m_List.GetNext(pos);
		}
		*nItemCount = nCount;
	}
	m_Locker.Unlock();
	return pList;
}
int CEndDeviceList::GetModelCount(const char *pszModel)
{
	ENDIENTRY	*pEndDevice;
	POSITION	pos = INITIAL_POSITION;
	int			nCount=0;

	if (!pszModel || !*pszModel)
		return 0;

	m_Locker.Lock();
	pEndDevice = m_List.GetFirst(pos);
	while(pEndDevice)
	{
		if (strncasecmp(pEndDevice->szParser, pszModel, strlen(pszModel)) == 0)
        {
			nCount++;
        /** Sub Core 도 검사한다 */
        }else if (strncmp(pEndDevice->szSubModel, pszModel, strlen(pszModel)) == 0)
        {
			nCount++;
        }
		pEndDevice = m_List.GetNext(pos);
	}
	m_Locker.Unlock();
	return nCount;
}

#if 0
int CEndDeviceList::GetSubModelCount(const char *pszModel)
{
	ENDIENTRY	*pEndDevice;
	POSITION	pos = INITIAL_POSITION;
	int			nCount=0;

	if (!pszModel || !*pszModel)
		return 0;

	m_Locker.Lock();
	pEndDevice = m_List.GetFirst(pos);
	while(pEndDevice)
	{
		if (strncmp(pEndDevice->szSubModel, pszModel, strlen(pszModel)) == 0)
			nCount++;
		pEndDevice = m_List.GetNext(pos);
	}
	m_Locker.Unlock();
	return nCount;
}
#endif

/** OTA 대상 Sensor의 List를 구한다.
 */
OTANODE *CEndDeviceList::GetOtaListByModel(const char *pszModel, int *nNodeCount, WORD oldHwVer, WORD oldFwVer, WORD oldBuild)
{
	ENDIENTRY	*pEndDevice;
	OTANODE		*pList;
	POSITION	pos = INITIAL_POSITION;
	int			nSeek, nCount=0;

	pList = NULL;
	*nNodeCount = 0;

	if (!pszModel || !*pszModel)
		return NULL;

	m_Locker.Lock();
	pEndDevice = m_List.GetFirst(pos);
	while(pEndDevice)
	{
		if (strncmp(pEndDevice->szParser, pszModel, strlen(pszModel)) == 0 &&
                (oldHwVer == 0 || pEndDevice->hw == oldHwVer) && 
                (oldFwVer == 0 || pEndDevice->fw == oldFwVer) && 
                (oldBuild == 0 || pEndDevice->build == oldBuild))
        {
			nCount++;
        }
		pEndDevice = m_List.GetNext(pos);
	}

	pList = (OTANODE *)MALLOC(sizeof(OTANODE)*nCount);
	if (pList != NULL)
	{
		nSeek = 0;
		memset(pList, 0, sizeof(OTANODE)*nCount);
		pEndDevice = m_List.GetFirst(pos);
		while(pEndDevice)
		{
			if (strncmp(pEndDevice->szParser, pszModel, strlen(pszModel)) == 0 && 
                (oldHwVer == 0 || pEndDevice->hw == oldHwVer) && 
                (oldFwVer == 0 || pEndDevice->fw == oldFwVer) && 
                (oldBuild == 0 || pEndDevice->build == oldBuild))
			{
				memcpy(&pList[nSeek].id, &pEndDevice->id, sizeof(EUI64));
				strcpy(pList[nSeek].szModel, pEndDevice->szParser);
				nSeek++;
			}
			pEndDevice = m_List.GetNext(pos);
		}
		*nNodeCount = nCount;
	}
	m_Locker.Unlock();
	return pList;
}

/** OTA 대상 Sensor의 List를 구한다.
 */
OTANODE *CEndDeviceList::GetOtaListById(const char *pszModel, int *nNodeCount, EUI64 *ids, int nCount, WORD oldHwVer, WORD oldFwVer, WORD oldBuild)
{
	ENDIENTRY	*pEndDevice;
	OTANODE		*pList;
	int			i;
    int         nSeek = 0;

	pList = NULL;
	*nNodeCount = 0;

	m_Locker.Lock();
	pList = (OTANODE *)MALLOC(sizeof(OTANODE)*nCount);
	if (pList != NULL)
	{
		memset(pList, 0, sizeof(OTANODE)*nCount);
		for(i=0; i<nCount; i++)
		{
			pEndDevice = FindEndDeviceByID(&ids[i]);
			if (pEndDevice != NULL && strncmp(pEndDevice->szParser, pszModel, strlen(pszModel)) == 0 && 
                (pEndDevice->hw == oldHwVer && pEndDevice->fw == oldFwVer && pEndDevice->build == oldBuild))
			{
			    memcpy(&pList[nSeek].id, &ids[i], sizeof(EUI64));
				strcpy(pList[nSeek].szModel, pEndDevice->szParser);
                nSeek ++;
			}
		}
		*nNodeCount = nSeek;
	}
	m_Locker.Unlock();
	return pList;
}

BOOL CEndDeviceList::UpdateRouteRecord(const EUI64 *id, WORD shortid, BYTE hops, WORD *path)
{
	ENDIENTRY	*pEndDevice, *pParentEndDevice;
	BOOL		bModified = FALSE;
	WORD		parentid;
    BYTE        nHops = hops > CODI_MAX_ROUTING_PATH ? CODI_MAX_ROUTING_PATH : hops;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{

		if ((pEndDevice->shortid != shortid) || (pEndDevice->hops != nHops))
			bModified = TRUE;

        /** Issue #1988: Sensor를 Add 할 때 Route Record 전달 안된 상태일 경우
          * Shortid가 ShortIDHash에 등록되지 않아서 집중기 Reset등으로 다시
          * Load 하기 전에는 정상적으로 Path가 표시되지 않는 문제를 해결한다
          * 
          * 발생할 수 있는 문제 : Short ID가 변경되는 경우 (leave/join)에 기존
          * shortid 정보가 남아 있게 된다. 기존 shortid를 삭제하는 경우 Shortid
          * 변경 도중 다른 sensor가 shortid를 사용하는 문제가 발생할 것을 걱정하여
          * 추가하지 않는다. 문제가 좀더 명확해 질 경우 이 부분을 최종적으로 완료해야
          * 한다.
          */
        if (pEndDevice->shortid != shortid) {
		    m_ShortIdHash.Update(&shortid, sizeof(WORD), pEndDevice->nPosition);
        }

		parentid = (nHops == 0) ? 0 : path[0];
		if (pEndDevice->parentid != parentid)
		{
			pEndDevice->parentid = parentid;
			bModified = TRUE;
		}
	
		pEndDevice->shortid = shortid;
		pEndDevice->hops	= nHops;

        /** End Device일 경우 **/
        if(pEndDevice->nAttribute & ENDDEVICE_ATTR_RFD) {
	        pParentEndDevice = FindEndDeviceByShortID(pEndDevice->parentid);
            if(pParentEndDevice) {
	            m_Locker.Unlock();
                UpdateRouteRecord(&pParentEndDevice->id, pEndDevice->parentid, hops - 1, path + 1);
	            m_Locker.Lock();
            }
        } else {
		    if (nHops > 0) {
                if(pEndDevice->hops == nHops && memcmp(pEndDevice->path, path, sizeof(WORD)*nHops)) 
				    bModified = TRUE;
                if(bModified) memcpy(pEndDevice->path, path, sizeof(WORD)*nHops);
            }
        }

		if (bModified || pEndDevice->tmRouteRecord == 0)
		{
			ceTime(&pEndDevice->tmRouteRecord);
			pEndDevice->bModified = bModified;

			if (bModified) SaveEndDevice(pEndDevice);
		}
	}
	else
	{
		// 등록되지 않은 센서의 Route record 정보가 오면 Cache에 등록한다.
		m_pInventoryScanner->CacheRouteRecord(id, shortid, nHops, path);
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::QueryRouteRecord(const EUI64 *id, WORD *shortid, BYTE *hops, WORD *path)
{
	ENDIENTRY	*pEndDevice;
    BOOL        bReturn = FALSE;

	m_Locker.Lock();
    pEndDevice = FindEndDeviceByID(id);
    bReturn = QueryRouteRecord(pEndDevice, shortid, hops, path, TRUE);
	m_Locker.Unlock();
	return bReturn;
}

BOOL CEndDeviceList::QueryRouteRecord(ENDIENTRY * endi, WORD *shortid, BYTE *hops, WORD *path, BOOL bLock)
{
	ENDIENTRY	*pEndDevice = endi, *pParentEndDevice;
    WORD        pshortid=0;
    BYTE        phops=0;

	if (pEndDevice != NULL)
	{
	    if(!bLock) m_Locker.Lock();

		if(shortid)     *shortid = pEndDevice->shortid;
		if(hops)        *hops = pEndDevice->hops;

        if(pEndDevice->nAttribute & ENDDEVICE_ATTR_RFD) {
		    if (pEndDevice->hops > 0) {
                if(path != NULL) path[0] = pEndDevice->parentid;
                if(!bLock) m_Locker.Unlock();

	            pParentEndDevice = FindEndDeviceByShortID(pEndDevice->parentid);
                if(pParentEndDevice) {
                    QueryRouteRecord(pParentEndDevice, &pshortid, &phops, path ? path+1 : NULL, TRUE); 
                    *hops = phops+1;
                    return TRUE;
                }
                return FALSE;
            }
	        if(!bLock) m_Locker.Unlock();
            return FALSE;
        } else {
		    if (pEndDevice->hops > 0 && path != NULL)
			    memcpy(path, pEndDevice->path, sizeof(WORD)*pEndDevice->hops);
        }
	    if(!bLock) m_Locker.Unlock();
	}
	return pEndDevice ? TRUE : FALSE;
}

BYTE CEndDeviceList::GetEndDeviceHops(ENDIENTRY *endi, BYTE *hops)
{
	BYTE		nHops = 0xff;

	m_Locker.Lock();
	if (endi != NULL) nHops = endi->hops;
	m_Locker.Unlock();

    *hops = nHops;
	return nHops;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CEndDeviceList::Initialize()
{
	LoadEndDevice("/app/member");
	return TRUE;
}

void CEndDeviceList::Destroy()
{
    m_Locker.Lock();
	RemoveAll();
	m_Locker.Unlock();
}

void CEndDeviceList::Reset()
{
	ENDIENTRY	*pEndDevice;
	POSITION	pos = INITIAL_POSITION;
	BOOL		bSave;
	int			i;
  
	// Source routing 정보를 초기화 함
	m_Locker.Lock();
	pEndDevice = m_List.GetFirst(pos);
	while(pEndDevice)
	{
		bSave = ((pEndDevice->shortid != 0) || (pEndDevice->hops != 0)) ? TRUE : FALSE;
		pEndDevice->shortid = 0;
		pEndDevice->hops	= 0;
		for(i=0; i<CODI_MAX_ROUTING_PATH; i++)
			pEndDevice->path[i] = 0;		

		// 초기화 되어 있지 않는 EndDevice 정보만 새로 저장한다.
		if (bSave) SaveEndDevice(pEndDevice);

		pEndDevice = m_List.GetNext(pos);
	}
	m_Locker.Unlock();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CEndDeviceList::LoadEndDevice(const char *pszPath)
{
	ENDIENTRY	*pEndDevice, member;
    DIR			*dir_fdesc;
    dirent		*dp;
	int			fd, n, sn;
    BOOL        bInvalidStructure = FALSE;
	char		*p, szFileName[128];

    dir_fdesc = opendir(pszPath);
    if (!dir_fdesc)
        return FALSE;

    sn = sizeof(ENDIENTRY);

    m_Locker.Lock();
    for(;(dp=readdir(dir_fdesc));)
    {
        p = strrchr(dp->d_name, '.');
        if ((p == NULL) || (strcmp(p, ".mbr") != 0))
            continue;

        sprintf(szFileName, "%s/%s", pszPath, dp->d_name);

        if(bInvalidStructure) {
            XDEBUG("Delete %s (invalid structure)\r\n", dp->d_name);
            remove(szFileName);
        }

        fd = open(szFileName, O_RDONLY);
        if (fd > 0)
        {
			memset(&member, 0, sn);
			n = read(fd, &member, sn);
			close(fd);

			if (n == sn)
			{
				pEndDevice = (ENDIENTRY *)MALLOC(sizeof(ENDIENTRY));
				if (pEndDevice != NULL)
				{
					memset(pEndDevice, 0, sizeof(ENDIENTRY));
					memcpy(pEndDevice, &member, sizeof(ENDIENTRY));
                    if(pEndDevice->magic != ENDIENTRY_MAGIC_NUMBER) {
                        BYTE cm = ENDIENTRY_MAGIC_NUMBER;

                        bInvalidStructure = TRUE;

	                    XDEBUG("EndDeviceList: Invalid Magic Number 0x%02X - 0x%02X\r\n", pEndDevice->magic, ENDIENTRY_MAGIC_NUMBER);
                        mcuAlert(MCU_ALERT_INVALID_MAGIC_NUMBER, (BYTE *)&pEndDevice->magic, sizeof(BYTE), (BYTE *)&cm, sizeof(BYTE));
                        break;
                    }
                    if(pEndDevice->installState > OTA_STEP_ALL) pEndDevice->installState = OTA_STEP_INIT;
                    pEndDevice->lpIndex     = 0;
					pEndDevice->bBusy		= FALSE;
					pEndDevice->bModified   = FALSE;
					pEndDevice->bValueModified   = FALSE;
					pEndDevice->pMibValue   = NULL;
					pEndDevice->nLastTempMeteringDate   = pEndDevice->nLastMeteringDate;
                    pEndDevice->nTotalSize = 0;
                    pEndDevice->nCurrentSize = 0;
					pEndDevice->nPosition  = (int)m_List.AddTail(pEndDevice);
					m_Hash.Update(&pEndDevice->id, sizeof(EUI64), pEndDevice->nPosition);
		            m_ShortIdHash.Update(&pEndDevice->shortid, sizeof(WORD), pEndDevice->nPosition);
				}
			}
			else
			{
                bInvalidStructure = TRUE;

	            XDEBUG("EndDeviceList: Invalid Structure Size\r\n");
                mcuAlert(MCU_ALERT_INVALID_ENDDEVICE_SIZE, (BYTE *)&n, sizeof(UINT), (BYTE *)&sn, sizeof(UINT));
                break;
			}
		}
	}
	closedir(dir_fdesc);
	m_Locker.Unlock();

    if(bInvalidStructure) {
	    XDEBUG("Delete All Members - Invalid Structure\r\n");
        DeleteEndDeviceAll();
    }

	return TRUE;
}

BOOL CEndDeviceList::SaveEndDevice(ENDIENTRY *pEndDevice)
{
	FILE	*fp;
	char	szFileName[128];
	char	szGUID[20];

	eui64toa(&pEndDevice->id, szGUID);	
	sprintf(szFileName, "/app/member/%s.mbr", szGUID);

	fp = fopen(szFileName, "wb");
	if (fp == NULL)
		return FALSE;

	fwrite(pEndDevice, sizeof(ENDIENTRY), 1, fp);
	pEndDevice->bModified = FALSE;

	fflush(fp);
	fclose(fp);
	return TRUE;
}

BOOL CEndDeviceList::RemoveAll()
{
	ENDIENTRY	*pEndDevice, *pDelete;
	POSITION	pos = INITIAL_POSITION;

	m_Hash.DeleteAll();
	m_ShortIdHash.DeleteAll();

	pEndDevice = m_List.GetFirst(pos);
	while(pEndDevice)
	{
		pDelete = pEndDevice;
		pEndDevice = m_List.GetNext(pos);
		FREE(pDelete);
	}
	m_List.RemoveAll();
	return TRUE;
}

ENDIENTRY *CEndDeviceList::FindEndDeviceByID(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;
	int			nIndex;

	nIndex = m_Hash.Find(id, sizeof(EUI64));
	if (nIndex == -1)
		return NULL;

	pEndDevice = m_List.Get((POSITION)nIndex);
	return pEndDevice;
}

ENDIENTRY *CEndDeviceList::FindEndDeviceByShortID(WORD shortid)
{
	ENDIENTRY	*pEndDevice;
	int			nIndex;

	nIndex = m_ShortIdHash.Find(&shortid, sizeof(WORD));
	if (nIndex == -1)
		return NULL;

	pEndDevice = m_List.Get((POSITION)nIndex);
	return pEndDevice;
}

void CEndDeviceList::SetModify(ENDIENTRY *pEndDevice, BOOL bModified)
{
	pEndDevice->bModified = bModified;
}

/** Issue #1565: 지속적으로 EndDeviceList가 변경되는 현상에 대처하기 위해서
 *  현재 EndDeviceList 구조체가 저장된 정보와 Agent의 EndDeviceList 구조체 
 *  정보의 크기차이가 있을 경우 전체 목록을 삭제하고 Timesync를 전송하도록 
 *  검사하는 기능 추가
 */
BOOL CEndDeviceList::CheckInternalStructure(const char * pszPath)
{
    DIR			*dir_fdesc;
    dirent		*dp;
	char		*p, szFileName[128];
    struct stat buf;
    BOOL        bState = TRUE;

    dir_fdesc = opendir(pszPath);
    if (!dir_fdesc)
        return bState;

    m_Locker.Lock();
    for(;(dp=readdir(dir_fdesc));)
    {
        p = strrchr(dp->d_name, '.');
        if ((p == NULL) || (strcmp(p, ".mbr") != 0))
            continue;

        sprintf(szFileName, "%s/%s", pszPath, dp->d_name);
        if(stat(szFileName, &buf)) continue;

        if(buf.st_size != sizeof(ENDIENTRY)) bState = FALSE;
        break;
	}
	closedir(dir_fdesc);
	m_Locker.Unlock();

	return bState;
}

/** TRUE를 return 하면 meterStatus에 정확한 값이 들어 있는것이도
  * FALSE를 return 하면 meterStatus의 값을 믿을 수 없는 것이다.
  */
BOOL CEndDeviceList::GetMeterError(const EUI64 *id, UINT errorType, BOOL *meterStatus)
{
	ENDIENTRY	*pEndDevice;
    BOOL        isOk = FALSE;

    /*-- Bit 보다 큰 값을 요청하면 FALSE --*/
    if(errorType > sizeof(UINT) * 8) return isOk;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        *meterStatus = (pEndDevice->meterError & (0x01 << errorType)) ? TRUE : FALSE;
        isOk = TRUE;
	}
	m_Locker.Unlock();
	return isOk;
}

/** TRUE를 return 하면 meterStatus에 정확한 값을 설정한 것이고
  * FALSE를 return 하면 meterStatus의 값을 설정하지 못한것이다.
  */
BOOL CEndDeviceList::SetMeterError(const EUI64 *id, UINT errorType, BOOL status)
{
	ENDIENTRY	*pEndDevice;
    UINT        oldStatus, newV;

    /*-- Bit 보다 큰 값을 요청하면 FALSE --*/
    if(errorType > sizeof(UINT) * 8) return FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        oldStatus = pEndDevice->meterError;
        newV = 0x01 << errorType;
        if(status) {
            pEndDevice->meterError |= newV;
        } else {
            pEndDevice->meterError &= (newV ^ 0xFFFFFFFF);
        }
        if(oldStatus != pEndDevice->meterError) 
            pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

/** ACD등에서 사용될 PQ Scale 값 저장
 *
 * @param id                EUI64 ID
 * @param fVoltageScale     Voltage Scale
 * @param fCurrentScale     Current Scale
 *
 * @return 정상적인 id 면 TRUE, 잘못된 id 면 FALSE
 *
 */
BOOL CEndDeviceList::UpdateConstantScale(const EUI64 *id, double fVoltageScale, double fCurrentScale)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL && (pEndDevice->fVoltageScale != fVoltageScale
            || pEndDevice->fCurrentScale != fCurrentScale)
    )
	{
        pEndDevice->fVoltageScale = fVoltageScale;
        pEndDevice->fCurrentScale = fCurrentScale;

		pEndDevice->bModified = TRUE;
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

BOOL CEndDeviceList::ClearValue(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(pEndDevice->pMibValue != NULL)
        {
            VARAPI_FreeNode(pEndDevice->pMibValue);
            pEndDevice->pMibValue = NULL;
        }
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

/** Todo: 현재 무조건 저장하고 있는 방식을 값을 비교해서 틀릴경우에만 저장하는 방식으로 변경해야 한다.
  */
BOOL CEndDeviceList::AddValue(const EUI64 *id, const char *attrName, BYTE nType, UINT nLength, BYTE * pValue)
{
	ENDIENTRY	*pEndDevice;
    BOOL        bOk = FALSE;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL && attrName)
	{
        MIBValue * pHead =  IF4API_NewMIBValue("1.11", VARSMI_STRING, 0, attrName, strlen(attrName));
        MIBValue * pLast = pHead;

        if(pLast != NULL)
        {
            pLast->pNext = IF4API_NewMIBValue("1.4", VARSMI_BYTE, nType, NULL, sizeof(BYTE)); pLast = pLast->pNext;
            pLast->pNext = IF4API_NewMIBValue("1.6", VARSMI_UINT, nLength, NULL, sizeof(UINT)); pLast = pLast->pNext;
            pLast->pNext = IF4API_NewMIBValue("1.12", VARSMI_STREAM, 0, (char *)pValue, nLength); pLast = pLast->pNext;

            pLast->pNext = pEndDevice->pMibValue;
            pEndDevice->pMibValue = pHead;

		    ceTime(&pEndDevice->tmValueChange);
		    pEndDevice->bModified = TRUE;
		    pEndDevice->bValueModified = TRUE;
            bOk = TRUE;
        }
	}
	m_Locker.Unlock();
	return bOk;
}

BOOL CEndDeviceList::SaveEndDeviceValue(ENDIENTRY *pEndDevice)
{
	char	szFileName[256];
	char	szGUID[20];

	eui64toa(&pEndDevice->id, szGUID);	
	sprintf(szFileName, "%s/%s.value", ENDI_VALUE_PATH, szGUID);

    VARAPI_Serialize(szFileName, pEndDevice->pMibValue);

	pEndDevice->bValueModified = FALSE;

	return TRUE;
}

/** EndDevice의 Service Type  설정
  */
BOOL CEndDeviceList::SetServiceType(const EUI64 *id, BYTE nServiceType)
{
	ENDIENTRY	*pEndDevice;

	m_Locker.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
        if(pEndDevice->nServiceType != nServiceType)
        {
            pEndDevice->nServiceType = nServiceType;
            pEndDevice->bModified = TRUE;
        }
	}
	m_Locker.Unlock();
	return pEndDevice ? TRUE : FALSE;
}

/** 해당 Device에 Meter Security가 설정되어 있다면 정보를 얻어온다 (Issue #320).
 * Meter Security는 id, node kind, parser type 의 우선순위로 검색하게 되며
 * Meter Security가 select 되었을 때 TRUE, 실패했거나 없을 경우 FALSE를 리턴한다.
 *
 * @param id
 * @param szBuffer Meter Security가 들어갈 Buffer. method 내부에서 검사해 주지 
 *          않기 때문에 충분한 공간을 확보한 후 호출해야 한다.
 * @param nLength key 길이
 *
 */
BOOL CEndDeviceList::GetMeterSecurity(const EUI64 *id, BYTE* szBuffer, int* nLength)
{
	ENDIENTRY	*pEndDevice;
    DBFetchData fetchData;

    if(id == NULL || szBuffer == NULL) return FALSE;

	if((pEndDevice = FindEndDeviceByID(id)) == NULL)
    {
        return FALSE;
    }

    memset(&fetchData, 0, sizeof(DBFetchData));
    m_pMeterSecurityHelper->Select(pEndDevice->szParser, pEndDevice->szNodeKind, &pEndDevice->id, &fetchData);

    if(fetchData.nCount > 0)
    {
        Row* pRow = m_pMeterSecurityHelper->GetHeadRow(&fetchData);
        METERSECURITYDATAINS* ins = m_pMeterSecurityHelper->GetData(pRow);

        if(ins != NULL)
        {
            if(ins->pKey != NULL && ins->nKeyLen > 0)
            {
                if(nLength != NULL)
                {
                    *nLength = ins->nKeyLen;
                }
                memcpy(szBuffer, ins->pKey, ins->nKeyLen);
                m_pMeterSecurityHelper->FreeResult(&fetchData);
                return TRUE;
            }
            m_pMeterSecurityHelper->FreeResult(&fetchData);
        }
    }

    return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Compare Functions
//////////////////////////////////////////////////////////////////////
int CEndDeviceList::compAttrModelId(const void *p1, const void *p2)
{
	ENDIENTRY   *m1, *m2;
    int res;

    m1 = *(ENDIENTRY * const *) p1;
    m2 = *(ENDIENTRY * const *) p2;
	
    res = m1->nAttribute - m2->nAttribute; if(res) return res;
    res = strcmp(m1->szModel, m2->szModel); if(res) return res;
    return memcmp(&m1->id, &m2->id, sizeof(EUI64));  
}
