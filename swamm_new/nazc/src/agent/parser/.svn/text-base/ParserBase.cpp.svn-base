//////////////////////////////////////////////////////////////////////
//
//  ParserBase.cpp: implementation of the CBaseParser class.
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

#include "ParserBase.h"

//////////////////////////////////////////////////////////////////////
// CBaseParser Class
//////////////////////////////////////////////////////////////////////

#if     defined(__PATCH_4_1647__)
extern BOOL    *m_pbPatch_4_1647;
#endif

extern BYTE     m_nDefaultMeteringOption;

//////////////////////////////////////////////////////////////////////
// CBaseParser Class
//////////////////////////////////////////////////////////////////////

CBaseParser::CBaseParser(int nType, int nService, int nAttribute, 
	char **pszNameArr, UINT nParserType, const char *pszDescr, const char *pszPrefixVendor, 
	const char *pszModel, int nConnectionTimeout, int nReplyTimeout, int nRetry)
	: CMeterParser(nType, nService, nAttribute, pszNameArr, nParserType, pszDescr,
	  pszPrefixVendor, pszModel, nConnectionTimeout, nReplyTimeout, nRetry)
{
}

CBaseParser::~CBaseParser()
{
}

//////////////////////////////////////////////////////////////////////
// CBaseParser Operations
//////////////////////////////////////////////////////////////////////

/** Open 
  */
int CBaseParser::StateOpen(HANDLE codi, ONDEMANDPARAM *pOndemand, BOOL *bConnected)
{
    int             nAttr;
    int nError = endiOpen(codi, (HANDLE *)&pOndemand->endi, &pOndemand->id);

    if (nError != CODIERR_NOERROR)
    {   
        XDEBUG("endiOpen Error(%d): %s\r\n", nError, codiErrorMessage(nError));
        return STATE_RETRY;
    }
               
    endiSetOption(pOndemand->endi, ENDIOPT_SEQUENCE, 1);
    gettimeofday((struct timeval *)&pOndemand->tmStart, NULL);
    
    nAttr = m_pEndDeviceList->GetSensorAttribute(&pOndemand->id);
    nError = sensorConnect(pOndemand->endi, GetConnectionTimeout(nAttr));

    if (nError != CODIERR_NOERROR)
    {
        XDEBUG("endiConnect Error(%d): %s\r\n", nError, codiErrorMessage(nError));
        return STATE_RETRY;
    }

    gettimeofday((struct timeval *)&pOndemand->tmConnect, NULL);
    gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);

    *bConnected = TRUE;
    if(pOndemand->nMeteringType == METERING_TYPE_BYPASS)
    {
        return STATE_INITIALIZE;
    }
    else
    {
        return STATE_READ_NODEINFO;
    }
}

int CBaseParser::ReadRomMap(WORD addr, int len, ONDEMANDPARAM *pOndemand, BYTE *szBuffer, 
        int nRetryState, int nSuccState)
{
    int readLen=0, nError;

    pOndemand->stat.nCount ++;
    pOndemand->stat.nSendCount ++;
    pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
    gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
    nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &readLen, GetReplyTimeout());
    if (nError != CODIERR_NOERROR)
    {
        pOndemand->stat.nError ++;
        if (pOndemand->nRetry < GetRetryCount())				// 3회 이상 연속 실패시 종료
        {
            pOndemand->nRetry++;
            return nRetryState;
        }else {
            pOndemand->nRetry = 0;
            return STATE_ERROR;   
        }
    }

    if (readLen != len)
    {
        XDEBUG(ANSI_COLOR_RED " ****** PARSER: Invalid ROM DATA Length ******\r\n" ANSI_NORMAL);
        XDEBUG(ANSI_COLOR_RED " Request Size = %d, Read Size = %d\r\n" ANSI_NORMAL, len, readLen);
        pOndemand->nRetry++;
        return nRetryState;
    }

    pOndemand->nRetry = 0;
    UpdateStatistics(pOndemand, (BYTE *)szBuffer, readLen);
    pOndemand->stat.nCount ++;
    pOndemand->stat.nRecvCount ++;

    return nSuccState;
}

int CBaseParser::ReadAmrData(ONDEMANDPARAM *pOndemand, BYTE *szBuffer, int nRetryState, int nSuccState)
{
    int readLen=0, nError;

    pOndemand->stat.nCount ++;
    pOndemand->stat.nSendCount ++;
    pOndemand->stat.nSend += sizeof(ENDI_AMRREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
    gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
    nError = endiReadAmr(pOndemand->endi, (BYTE *)szBuffer, &readLen, GetReplyTimeout());
    if (nError != CODIERR_NOERROR)
    {
        pOndemand->stat.nError ++;
        if (pOndemand->nRetry < GetRetryCount())				// 3회 이상 연속 실패시 종료
        {
            pOndemand->nRetry++;
            return nRetryState;
        }else {
            pOndemand->nRetry = 0;
            return STATE_ERROR;   
        }
    }

    pOndemand->nRetry = 0;
    UpdateStatistics(pOndemand, (BYTE *)szBuffer, readLen);
    pOndemand->stat.nCount ++;
    pOndemand->stat.nRecvCount ++;

    return nSuccState;
}

int CBaseParser::StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    pOndemand->nResult = ONDEMAND_ERROR_OK;
    return STATE_OK;
}

int CBaseParser::StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    return STATE_OK;
}

int CBaseParser::StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    return STATE_OK;
}

int CBaseParser::StateError(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    return STATE_OK;
}

//////////////////////////////////////////////////////////////////////

int CBaseParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
    return METERING(pCommand, pStat);
}

int CBaseParser::METERING(CMDPARAM *pCommand, METER_STAT *pStat)
{
    int                 nOption=0, nOffset=1, nCount=1;

    nOption = pCommand->nOption;
    if(nOption < 0) 
    {
        nOption = m_nDefaultMeteringOption;
    }

    nOffset = pCommand->nOffset >= 0 ? pCommand->nOffset : 1;
    if (nOffset > 39) nOffset = 39;

    nCount = pCommand->nCount > 0 ? MIN(40-nOffset, pCommand->nCount) : 2;
    if (nCount <= 0) nCount = 2;

    if(nCount > nOffset + 1) nCount = nOffset + 1;

    return DoMetering(pCommand, nOption, nOffset, nCount, NULL, pStat);
}


//////////////////////////////////////////////////////////////////////
// CBaseParser Operations
//////////////////////////////////////////////////////////////////////


