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

