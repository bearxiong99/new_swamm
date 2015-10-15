//////////////////////////////////////////////////////////////////////
//
//  MeterParser.cpp: implementation of the CMeterParser class.
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
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "MeterParser.h"
#include "BypassQueue.h"
#include "Utility.h"
#include "DebugUtil.h"

#include "dr/drdef.h"

#include "rep/MeteringHelper.h"
#include "rep/LogHelper.h"

//////////////////////////////////////////////////////////////////////
// CMeterParser Class
//////////////////////////////////////////////////////////////////////

CMeterParser::CMeterParser(int nType, int nService, int nAttribute, 
        char **pszNames, UINT nParserType, const char *pszDescr, const char *pszPrefixVendor, const char *pszModel, 
        int nConnectionTimeout, int nReplyTimeout, int nRetry)
{
	m_nSensorType = nType;
	m_nServiceType = nService;
	m_nSensorAttribute = nAttribute;
	m_nSensorConnectionTimeout = nConnectionTimeout;
	m_nSensorReplyTimeout = nReplyTimeout;
	m_nSensorRetryCount = nRetry;
    m_bSubCore = FALSE;
    m_bHasSubCore = FALSE;
    m_nParserType = nParserType;
    m_pSubCoreParser = NULL;

	strcpy(m_szPrefixVendor, pszPrefixVendor);
	//strcpy(m_szParserName, pszName);
    m_pNameArr = pszNames;
	strcpy(m_szParserDescr, pszDescr);
    if(pszModel!=NULL) strcpy(m_szModel, pszModel);
    else memset(m_szModel,0,sizeof(m_szModel));
}

CMeterParser::~CMeterParser()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterParser Operations
//////////////////////////////////////////////////////////////////////
void CMeterParser::SetIsSubCore()
{
    m_bSubCore = TRUE;
}

void CMeterParser::SetSubCoreParser(CMeterParser *pSubCoreParser)
{
    m_pSubCoreParser = pSubCoreParser;

    if(m_pSubCoreParser)
    {
        m_bHasSubCore = TRUE;
        m_pSubCoreParser->SetIsSubCore();
    }
}

CMeterParser *CMeterParser::GetSubCoreParser()
{
    return m_pSubCoreParser;
}


//////////////////////////////////////////////////////////////////////
// CMeterParser Virtual Operations 
//////////////////////////////////////////////////////////////////////
int CMeterParser::GetType()
{
	return m_nSensorType;
}

int CMeterParser::GetService()
{
	return m_nServiceType;
}

int CMeterParser::GetAttribute(char * szParser)
{

#ifdef  strcasestr
    if(szParser && strcasestr(szParser,"SOLAR")) {
#else
    if(szParser && strstr(szParser,"SOLAR")) {
#endif
        return (m_nSensorAttribute & (0xFFFFFFFF ^ ENDDEVICE_ATTR_POWERNODE)) | ENDDEVICE_ATTR_SOLARNODE;
#ifdef  strcasestr
    } else if(szParser && strcasestr(szParser,"BATT")) {
#else
    } else if(szParser && strstr(szParser,"BATT")) {
#endif
        return (m_nSensorAttribute & (0xFFFFFFFF ^ ENDDEVICE_ATTR_POWERNODE)) | ENDDEVICE_ATTR_BATTERYNODE;
    }
	return m_nSensorAttribute;
}

int CMeterParser::GetConnectionTimeout(int nAttribute)
{
	return m_nSensorConnectionTimeout;
}

int CMeterParser::GetReplyTimeout()
{
	return m_nSensorReplyTimeout;
}

int CMeterParser::GetRetryCount()
{
	return m_nSensorRetryCount;
}

char *CMeterParser::GetName()
{
	return (char *)m_pNameArr[0];
}

char **CMeterParser::GetNames()
{
	return (char **)m_pNameArr;
}

char *CMeterParser::GetDescription()
{
	return (char *)m_szParserDescr;
}

char *CMeterParser::GetPrefixVendor()
{
	return (char *)m_szPrefixVendor;
}

char *CMeterParser::GetModel()
{
    if(m_szModel[0]) return (char *)m_szModel;
    else return NULL;
}

/** Push Metering 장비 여부를 리턴한다.
 *
 * Push Metering 장비일 경우 정기 검침 후 File로 검침데이터를 저장하는 기능을 수행하지 않는다.
 */
BOOL CMeterParser::IsPushMetering()
{
    return FALSE;
}

BOOL CMeterParser::IsSubCore()
{
	return m_bSubCore;
}

BOOL CMeterParser::HasSubCore()
{
    return m_bHasSubCore;
}

UINT CMeterParser::GetParserType()
{
	return m_nParserType;
}

void CMeterParser::UpdateStatistics(ONDEMANDPARAM *pOndemand, BYTE *pszData, BYTE nLength)
{
	int				nTime;

	gettimeofday((struct timeval *)&pOndemand->tmRecv, NULL);
	nTime = GetTimeInterval(&pOndemand->tmSend, &pOndemand->tmRecv);
	pOndemand->stat.nResponseTime += nTime;
	if (pOndemand->stat.nMinTime == 0)
	     pOndemand->stat.nMinTime = nTime;
	else pOndemand->stat.nMinTime = MIN(nTime, pOndemand->stat.nMinTime);

	if (pOndemand->stat.nMaxTime == 0)
		 pOndemand->stat.nMaxTime = nTime;
	else pOndemand->stat.nMaxTime = MAX(nTime, pOndemand->stat.nMaxTime);
}


/** DR 가능 장비인지 여부를 리턴한다.
 */
BOOL CMeterParser::IsDRAsset(EUI64 *pId)
{
    // 기본은 FALSE
    return FALSE;
}

/** 현재 Energy Level 값 
 *
 * @param pId EUI64 ID
 * @param pEnergyLevel Energy Level
 *
 * @return 
 *      IF4ERR_NOERROR 성공
 *      IF4ERR_INVALID_ID 잘못된 ID
 *      IF4ERR_INVALID_PARAM 잘못된 pEnergyLevel pointer
 *      IF4ERR_DO_NOT_SUPPORT 지원하지 않음
 *      IF4ERR_CANNOT_CONNECT 장비에 연결되지 않음
 */
int CMeterParser::GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel)
{
    return IF4ERR_DO_NOT_SUPPORT;
}

/** Energy Level 설정
 *
 * @param pId EUI64 ID
 * @param nEnergyLevel 유요한 level 값
 * @param pszCallback       Callback data
 * @param nCallbackLen      Callback data length
 *
 * @return 
 *      IF4ERR_NOERROR 성공
 *      IF4ERR_INVALID_ID 잘못된 ID
 *      IF4ERR_INVALID_PARAM 잘못된 Level 값
 *      IF4ERR_DO_NOT_SUPPORT 지원하지 않음
 *      IF4ERR_CANNOT_CONNECT 장비에 연결되지 않음
 *
 * @see CMeterParser#IsDRAsset();
 */
int CMeterParser::SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbackLen)
{
    return IF4ERR_DO_NOT_SUPPORT;
}

/** 장비에서 유효한 Level 값 배열 리턴.
 *
 * @return 
 *      0 지원하지 않음
 *      positive 배열 개 멤버 수
 *      -1 NULL Parameter
 *
 * @see CMeterParser#IsDRAsset();
 *
 * @warning pEnergyLevelArray는 최소 15bytes 이상의 길이를 가져야 한다.
 */
int CMeterParser::GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray)
{
    return 0;
}

/** Meter Instance가 최초 생성되었을 때 불려진다.
 * 
 * @param pId Mode ID
 * @param nPortNum port Number
 */
BOOL CMeterParser::PostCreate(const EUI64 *pId, int nPortNum)
{
    //XDEBUG(" MeterParser::PostCreate\r\n");
    return FALSE;
}

/** Parser에 맞게 BasePulse를 계산한다.
 *
 * @param order Ordering
 * @param nLastPulseSize current pulse의 data size(byte)
 * @param nChannelCount channel 갯수
 * @param pEndiLastPulse End Device에서 전달된 source data
 * @param pLastPulse 새롭게 계산된 Data
 *
 */
BOOL CMeterParser::UpdateLastPulse(ORDER order, int nLastPulseSize, int nChannelCount, void *pEndiLastPulse, void *pLastPulse)
{
    METERINGINS *pMeteringIns = (METERINGINS *)pLastPulse;
    int size = sizeof(unsigned long long);
    char *ptr;

    if(nLastPulseSize <=0 || nChannelCount <= 0 || pEndiLastPulse == NULL || pLastPulse == NULL) return FALSE;

    pMeteringIns->pLastValue = (unsigned long long *)MALLOC(size*nChannelCount);
    ptr = (char*)pMeteringIns->pLastValue;
    memcpy(ptr, (BYTE *)pEndiLastPulse, size*nChannelCount);

    return  TRUE;
}

/** Parser에 맞게 BasePulse를 계산한다.
 *
 * @param order Ordering
 * @param nBasePulseSize base pulse의 data size(byte)
 * @param nChannelCount channel 갯수
 * @param pEndiBasePulse End Device에서 전달된 source data
 * @param pBasePulse 새롭게 계산된 Data
 *
 */
BOOL CMeterParser::UpdateBasePulse(ORDER order, int nBasePulseSize, int nChannelCount, void *pEndiBasePulse, void *pBasePulse)
{
    METERINGINS *pMeteringIns = (METERINGINS *)pBasePulse;
    int size = sizeof(unsigned long long);
    char *ptr;

    if(nBasePulseSize <=0 || nChannelCount <= 0 || pEndiBasePulse == NULL || pBasePulse == NULL) return FALSE;

    pMeteringIns->pBaseValue = (unsigned long long *)MALLOC(size*nChannelCount);
    ptr = (char*)pMeteringIns->pBaseValue;
    memcpy(ptr, (BYTE *)pEndiBasePulse, size*nChannelCount);

    return  TRUE;
}


/** Parser에 맞게 LP를 계산한다.
 *
 * @param order Ordering
 * @param nLpSize load profile의 data size(byte)
 * @param nChannelCount channel 갯수
 * @param nInterval LP interval
 * @param nLpCount LP count
 * @param pEndiLp End Device에서 전달된 source data
 * @param pLoadProfile 새롭게 계산된 Data
 *
 */
BOOL CMeterParser::UpdateLoadProfile(ORDER order, int nLpSize, int nChannelCount, int nInterval, int nLpCount, void *pEndiLp, void *pLoadProfile)
{
    METERINGINS *pMeteringIns = (METERINGINS *)pLoadProfile;
    ENDI_PUSH_LP *pLp = (ENDI_PUSH_LP *)pEndiLp;
    int size = sizeof(unsigned int);
    int i;
    cetime_t timev, ctimev;
    TIMESTAMP timeStamp;
    char *ptr;

    XDEBUG(" %s LpSize %d ChCnt %d Interval %d LpCnt %d\r\n", 
            order == ORDER_LSB ? "Little" : "Big" , nLpSize, nChannelCount, nInterval, nLpCount);
    if(nLpSize <=0 || nChannelCount <= 0 || nLpCount <= 0 || pEndiLp == NULL || pLoadProfile == NULL) return FALSE;

    memset(&timeStamp, 0, sizeof(TIMESTAMP));
    Timestamp5ToTimestamp(&pLp->lastTime, &timeStamp);
    timev = MakeTime(&timeStamp);
    for(i=0;i<nLpCount;i++) {
        pMeteringIns->pLoadProfile[i] = (LOADPROFILEINS*)MALLOC(sizeof(LOADPROFILEINS));
        memset(pMeteringIns->pLoadProfile[i], 0, sizeof(LOADPROFILEINS));
        ctimev = timev - ((nLpCount - i - 1) * (nInterval * 60));
        GetTimestamp(&pMeteringIns->pLoadProfile[i]->_snapShotTime, &ctimev);

        pMeteringIns->pLoadProfile[i]->pValue = (unsigned int *)MALLOC(size*nChannelCount);
        ptr = (char*)pMeteringIns->pLoadProfile[i]->pValue;
        memcpy(ptr, (BYTE *)pLp->lp, size*nChannelCount);
    }

    return  TRUE;
}

/** Push Metering으로 전달된 Log Data 처리.
 *
 * @param order Ordering
 * @param nEntryCount Entry count
 * @param pCurrentTime Meter Current Time
 * @param pEndiLog End Device에서 전달된 source data
 * @param pLogData 새롭게 계산된 Data
 *
 */
BOOL CMeterParser::UpdateLog(ORDER order, int nEntryCount, TIMESTAMP *pCurrentTime, void *pEndiLog, void *pLogData)
{
    LOGENTRYINS *pLogEntryIns = (LOGENTRYINS *)pLogData;
    PUSH_EVENT *pEvent;
    char * szData = (char *)pEndiLog;
    int i, j, idx, nSeek = 0;

    for(i=0,idx=0; i<nEntryCount; i++)
    {
        pEvent = (PUSH_EVENT *)(szData + nSeek);
        nSeek += sizeof(PUSH_EVENT);
        for(j=0; j<pEvent->eventCount; j++, idx++)
        {
            memcpy(&pLogEntryIns[idx].logTime, pCurrentTime, sizeof(TIMESTAMP));    
            pLogEntryIns[idx].nCategory = pEvent->eventType;
            if(pEvent->eventLength > 0)
            {
                pLogEntryIns[idx].nLogSize = pEvent->eventLength;
                pLogEntryIns[idx].szLog = (BYTE *)MALLOC(pEvent->eventLength);
                memcpy(pLogEntryIns[idx].szLog, pEvent->data + (pEvent->eventLength * j), pEvent->eventLength);
                nSeek += pEvent->eventLength;
            }
        }
    }

    return TRUE;
}

int CMeterParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
	return 0;
}

void CMeterParser::OnData(EUI64 *pid, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
{
	char	szGUID[17];

	eui64toa(pid, szGUID);
    XDEBUG("MeterParser.OnData:(%s:%d)\r\n",szGUID, nLength);
    //XDUMP((const char *)pszData, nLength, TRUE);

    m_pBypassQueue->Add(pid, nSeq, pszData, nLength);
}

