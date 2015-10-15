//////////////////////////////////////////////////////////////////////
//
//  acd.cpp: implementation of the CACDParser class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2009 Nuritelecom Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "AgentService.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "codiapi.h"
#include "Chunk.h"
#include "Event.h"
#include "MeterUploader.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "Patch.h"

#include "dr/drdef.h"
#include "ParserAcd.h"

#if     defined(__PATCH_11_65__)
/**< Issue #65 JEJU SP */
extern BOOL    *m_pbPatch_11_65;
#endif

static const char *parserNameArray[] = {"ACD", "NACD", NULL};

//////////////////////////////////////////////////////////////////////
// CACDParser Class
//////////////////////////////////////////////////////////////////////

CACDParser::CACDParser()
		   : CPulseBaseParser(SENSOR_TYPE_ACD, SERVICE_TYPE_ELEC, 
                                ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL,
								const_cast<char **>(parserNameArray), PARSER_TYPE_ACD, 
                                "ACD Parser", "ACD", parserNameArray[0],
                                6000, 6000, 2)
{
}

CACDParser::~CACDParser()
{
}

//////////////////////////////////////////////////////////////////////
// CACDParser Overrriding Operations
//////////////////////////////////////////////////////////////////////
/** DR 가능 장비인지 여부를 리턴한다.
 */
BOOL CACDParser::IsDRAsset(EUI64 *pId)
{
    return TRUE;
}

/** 현재 Energy Level 값 
 *
 * @param pId EUI64 ID
 *
 * @return 
 *      IF4ERR_NOERROR 성공
 *      IF4ERR_INVALID_ID 잘못된 ID
 *      IF4ERR_INVALID_PARAM 잘못된 pEnergyLevel pointer
 *      IF4ERR_DO_NOT_SUPPORT 지원하지 않음
 *      IF4ERR_CANNOT_CONNECT 장비에 연결되지 않음
 */
int CACDParser::GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel)
{
    ENDI_DIO_PAYLOAD   payload;
    ENDI_DIO_PAYLOAD   *pRes;
    BYTE        szResBuffer[100];
    char szId[32]={0,};
    ENDIENTRY *pEndi;
    int     nResLen=0;
    int     nAttr;
    BYTE    nEnergyLevel = 0;
    int     nError;

    if(pId == NULL)
    {
        return IF4ERR_INVALID_ID;
    }

    eui64toa(pId, szId);
	pEndi = m_pEndDeviceList->GetEndDeviceByID(pId);
    if(pEndi == NULL) 
    {
        return IF4ERR_INVALID_ID;
    }

    if(pEnergyLevel == NULL) 
    {
        return IF4ERR_INVALID_PARAM;
    }

    nAttr = m_pEndDeviceList->GetSensorAttribute(pId);

    memset(&payload, 0, sizeof(ENDI_DIO_PAYLOAD));
    memset(szResBuffer, 0, sizeof(szResBuffer));
    payload.type = DIGITAL_STATUS_REQUEST;
    payload.mask = (BYTE)0xFF;
    payload.value = (BYTE)0x00;

	if((nError = m_pOndemander->EndiSend((HANDLE)GetCoordinator(), pId, CODI_ACCESS_WRITE, ENDI_DATATYPE_DIO, 
                    (BYTE *)&payload, sizeof(ENDI_DIO_PAYLOAD), szResBuffer, &nResLen, 
                    GetConnectionTimeout(nAttr))) != IF4ERR_NOERROR) 
    {
        XDEBUG("Energy Level Control Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
        return IF4ERR_CANNOT_CONNECT;
    } 
    else
    {
        pRes = (ENDI_DIO_PAYLOAD *)szResBuffer;
        switch(pRes->value & 0x01)
        {
            case 0: // On
                nEnergyLevel = 1;
                break;
            case 1: // Off
                nEnergyLevel = 15;
                break;
        }
    }

    /** Energy Level Update */
    m_pEndDeviceList->SetEnergyLevel(pId, nEnergyLevel, 0);

    *pEnergyLevel = nEnergyLevel;

/** 제주실증단지 DR Level 적용 Patch */
#if     defined(__PATCH_11_65__)
    if(*m_pbPatch_11_65) {
        *pEnergyLevel = Patch_11_65_Nuri2Jeju(nEnergyLevel);
    }
#endif

    return IF4ERR_NOERROR;
}

/** Energy Level 설정
 *
 * @param pId EUI64 ID
 * @param nEnergyLevel 유요한 level 값
 *
 * @return 
 *      IF4ERR_NOERROR 성공
 *      IF4ERR_INVALID_ID 잘못된 ID
 *      IF4ERR_INVALID_PARAM 잘못된 Level 값
 *      IF4ERR_DO_NOT_SUPPORT 지원하지 않음
 *      IF4ERR_CANNOT_CONNECT 장비에 연결되지 않음
 *
 * @see CACDParser#IsDRAsset(EUI64 *);
 */
int CACDParser::SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen)
{
    int 	nError = IF4ERR_NOERROR;
    ENDI_DIO_PAYLOAD   payload;
    BYTE        szResBuffer[100];
    char szId[32]={0,};
    ENDIENTRY *pEndi;
    int     nResLen=0;
    int     nAttr;

    if(pId == NULL)
    {
        return IF4ERR_INVALID_ID;
    }

	pEndi = m_pEndDeviceList->GetEndDeviceByID(pId);
    if(pEndi == NULL) 
    {
        return IF4ERR_INVALID_ID;
    }

    eui64toa(pId, szId);
    if(!ISVALID_ENERGY_LEVEL(nEnergyLevel)) 
    {
        XDEBUG("Energy Level Control Fail: id=%s, invalid level %d\r\n", szId, nEnergyLevel);
        return IF4ERR_INVALID_PARAM;
    }

    nAttr = m_pEndDeviceList->GetSensorAttribute(pId);

    memset(&payload, 0, sizeof(ENDI_DIO_PAYLOAD));
    memset(szResBuffer, 0, sizeof(szResBuffer));
    payload.type = DIGITAL_OUTPUT_REQUEST;
    payload.mask = (BYTE)0x01;
    switch(nEnergyLevel) {
        /** Patch 11-65 의 경우 Set은 Patch를 적용 여부와 상관없이 동일한 코드를 적용한다 */
        case 1:
            payload.value = 0x00;   // On
            break;
        default:
            nEnergyLevel = 0x0F;    // 두가지 Level 만 지원
            payload.value = 0x01;   // Off
            break;
    }

	if((nError = m_pOndemander->EndiSend((HANDLE)GetCoordinator(), pId, CODI_ACCESS_WRITE, ENDI_DATATYPE_DIO, 
                    (BYTE *)&payload, sizeof(ENDI_DIO_PAYLOAD), szResBuffer, &nResLen, 
                    GetConnectionTimeout(nAttr))) != IF4ERR_NOERROR) 
    {
        XDEBUG("Energy Level Control Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
        return nError;
    } 

    /** Energy Level Update */
    m_pEndDeviceList->SetEnergyLevel(pId, nEnergyLevel, 0, pszCallback, nCallbakLen);

    return nError;
}

/** 장비에서 유효한 Level 값 배열 리턴.
 *
 * @return 
 *      0 지원하지 않음
 *      positive 배열 개 멤버 수
 *      -1 NULL Parameter
 *
 * @see CACDParser#IsDRAsset();
 *
 * @warning pEnergyLevelArray는 최소 15bytes 이상의 길이를 가져야 한다.
 */
int CACDParser::GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray)
{
    pEnergyLevelArray[0] = 0x01;
    pEnergyLevelArray[1] = 0x0F;

    return 2;
}

//////////////////////////////////////////////////////////////////////
int CACDParser::StateEnergyLevel(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    /** ACD의 경우 Port 값을 읽어서 확인한다.
      */
    ENDI_DIO_PAYLOAD payload;
    int nResLen = 0;

    memset(&payload, 0, sizeof(ENDI_DIO_PAYLOAD));
    payload.type = DIGITAL_STATUS_REQUEST;
    payload.mask = (BYTE)0xFF;

    pOndemand->stat.nCount ++;
    pOndemand->stat.nSendCount ++;
    pOndemand->stat.nSend += sizeof(ENDI_DIO_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
    gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);

	if(endiSendRecv(pOndemand->endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_DIO, 
                    (BYTE *)&payload, sizeof(ENDI_DIO_PAYLOAD), szBuffer, &nResLen, GetReplyTimeout()) == CODIERR_NOERROR) 
    {
        ENDI_DIO_PAYLOAD * resPayload =  (ENDI_DIO_PAYLOAD *)szBuffer;
        BYTE nEnergyLevel = 0;

        pOndemand->nRetry = 0;
        UpdateStatistics(pOndemand, (BYTE *)szBuffer, nResLen);
        pOndemand->stat.nCount ++;
        pOndemand->stat.nRecvCount ++;

        switch(resPayload->value & 0x01)
        {
            case 0: // On
                nEnergyLevel = 0x01;
                break;
            case 1: // Off
                nEnergyLevel = 0x0F;
                break;
        }

        pOndemand->nEnergyLevel = nEnergyLevel;

        /** Energy Level Update */
        m_pEndDeviceList->SetEnergyLevel(&pOndemand->id, nEnergyLevel, 0);

        return STATE_READ_AMRINFO;
    }
    else 
    {
        pOndemand->stat.nError ++;
        if (pOndemand->nRetry < GetRetryCount())				// 3회 이상 연속 실패시 종료
        {
            pOndemand->nRetry++;
            return STATE_READ_ENERGY_LEVEL;
        }else {
            pOndemand->nRetry = 0;
            return STATE_ERROR;   
        }
    } 

}

