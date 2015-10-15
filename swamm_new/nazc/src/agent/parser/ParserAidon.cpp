//////////////////////////////////////////////////////////////////////
//
//  ParserAidon.cpp: implementation of the CAidonParser class.
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
#include "Parity.h"

#include "vendor/pollmiu.h"
#include "ParserAidon.h"

#include "iec62056_21Define.h"
#include "iec62056_21Functions.h"
#include "iecError.h"

#define AIDON_FIX_NUMBER			"73500286"

typedef enum {
    IDX_REQUEST=0,
    IDX_ACK_OPT=1
} MeteringIndex;

//////////////////////////////////////////////////////////////////////
// CAidonParser Class
//////////////////////////////////////////////////////////////////////

static const char *parserNameArray[] = {"AIDON", NULL};

//////////////////////////////////////////////////////////////////////
// CAidonParser Class
//////////////////////////////////////////////////////////////////////

CAidonParser::CAidonParser() :  CPollBaseParser(SENSOR_TYPE_ZRU, SERVICE_TYPE_ELEC, 
                                            ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL, 
											const_cast<char **>(parserNameArray), PARSER_TYPE_AIDON, 
                                            "AIDON Parser", "AIDON", parserNameArray[0],
                                            6000, 6000, 2)
{
}

CAidonParser::~CAidonParser()
{
}

//////////////////////////////////////////////////////////////////////
// CAidonParser Override method
//////////////////////////////////////////////////////////////////////
void CAidonParser::ActPreAction(ONDEMANDPARAM *pOndemand)
{
    if(pOndemand != NULL) {
        pOndemand->bMeterConstants = FALSE;
        pOndemand->bVoltageScale = FALSE;
        /** Aidon의 Meter 상수는 100으로 고정 */
        pOndemand->nMeterConstant = 100;
    }
}

int CAidonParser::StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    pOndemand->nIndex = 0;
    pOndemand->nRetry = 0;
    return STATE_QUERY;
}

int CAidonParser::StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    int                 nFrameLen = 0;
    int                 nIndex = pOndemand->nIndex;
    int                 nError;

    switch(nIndex)
    {
        case IDX_REQUEST:     // Request
            XDEBUG(ANSI_COLOR_GREEN " IEC21 REQUEST\r\n" ANSI_NORMAL);
            nError = iec21MakeRequest(szBuffer, NULL, 0, AIMIR_PARITY_NONE, &nFrameLen);
            break;
        case IDX_ACK_OPT:     // Request ID
            XDEBUG(ANSI_COLOR_GREEN " IEC21 ACK&OPT\r\n" ANSI_NORMAL);
            nError = iec21MakeAckOption(szBuffer, '0', '0', IEC_21_BAUD_CE_9600, AIMIR_PARITY_NONE, &nFrameLen);
            break;
        default:
            return STATE_OK;
    }

    if(nError == IECERR_NOERROR)
    {
        SendCommand(pOndemand, &pOndemand->id, (const char *)szBuffer, nFrameLen);
    }
    else
    {
        XDEBUG(" MakeFrameError : %s\r\n", iec21GetErrorMessage(nError));
        return STATE_ERROR;
    }

    m_Locker.Lock();
    ClearOperation(pOndemand);
    pOndemand->bReply = FALSE;
    ResetEvent(pOndemand->hEvent);
    m_Locker.Unlock();
    return STATE_WAIT_REPLY;

}

int CAidonParser::StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    WaitForSingleObject(pOndemand->hEvent, (int)(GetReplyTimeout()/1000));
    if(pOndemand->bPartialWait)
    {
        pOndemand->bPartialWait = FALSE;
        m_Locker.Unlock();
        return STATE_WAIT_REPLY;
    }
    if(pOndemand->bReply)
    {
        pOndemand->nRetry = 0;
        pOndemand->nResult = ONDEMAND_ERROR_OK;
        if(pOndemand->nIndex == 0xFF) {
            return STATE_OK;
        }
        return STATE_QUERY;
    }
    else
    {
        /** Timeout */
        pOndemand->nRetry ++;
        XDEBUG(" FAIL COUNT SUCC %d\r\n", pOndemand->nRetry);
        if (pOndemand->nRetry >= 3)
            return STATE_ERROR;
        else
            return STATE_QUERY;
    }
}

//////////////////////////////////////////////////////////////////////
void CAidonParser::OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
{
    DATASTREAM      *pStream;

    if(pOndemand)
    {
        pOndemand->stat.nCount ++;
        pOndemand->stat.nRecvCount ++;
        pOndemand->stat.nRecv += nLength + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);

        pStream = pOndemand->pDataStream;
        if(pStream && !ParseStream(pStream, pszData, nLength, pOndemand))
        {
            /** 아직 Frame이 완료되지 않았을 때 */
            XDEBUG(" PENDDING FRAME CURR=%d\r\n", pStream->nLength);
        }
    }
}


//////////////////////////////////////////////////////////////////////
// CAidonParser Operations
//////////////////////////////////////////////////////////////////////

BOOL CAidonParser::OnAck(DATASTREAM *pStream, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex ++;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return CIec21DataStream::OnAck(pStream, pCallData);
}

BOOL CAidonParser::OnNak(DATASTREAM *pStream, void *pCallData)
{
    return CIec21DataStream::OnNak(pStream, pCallData);
}

BOOL CAidonParser::OnBreakFrame(DATASTREAM *pStream, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex = 0xFF;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return CIec21DataStream::OnBreakFrame(pStream, pCallData);
}

BOOL CAidonParser::OnIdentificationFrame(DATASTREAM *pStream, BYTE* manufacturerId, BYTE baud, BYTE *serialSetting, BYTE * identification, 
        int nLength, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    char manuf[4];
    char model[32];
    int meterType=0;

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex ++;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    /** Aidon Identification Message의 경우 Specification에는 /ADN[SP]55X0 으로 오게 되어 있고
      * IEC62056-21 Spec 상에는 /ADN555X0 으로 와야 하지만 실제로는 /ADN55X0 으로 전달되고 있다.
      */
    memset(manuf, 0, sizeof(manuf));
    memset(model, 0, sizeof(model));
    sscanf((const char *)identification, "%d", &meterType);
    strncpy(manuf, (const char *)manufacturerId, 3);
    sprintf(model, "%s%c%d", manuf, baud, meterType);
    XDEBUG(" METER TYPE : %s\r\n", model);

   	m_pEndDeviceList->UpdateMeterInventory(&pOndemand->id, 
            UNDEFINED_PORT_NUMBER, PARSER_TYPE_AIDON,
            GetPrefixVendor(), METER_VENDOR_AIDON, model, NULL, 0, 0);

    return CIec21DataStream::OnIdentificationFrame(pStream, manufacturerId, baud, serialSetting, identification, nLength, pCallData);
}

BOOL CAidonParser::OnCommandFrame(DATASTREAM *pStream, BYTE cmd, BYTE cmdType, BYTE *pszBuffer, int nLength, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex ++;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return CIec21DataStream::OnCommandFrame(pStream, cmd, cmdType, pszBuffer, nLength, pCallData);
}

BOOL CAidonParser::OnDataFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    char strBuffer[64];
    char strSerial[64];

    memset(strBuffer, 0, sizeof(strBuffer));
    if(iec21GetData((const char *)pszBuffer, "0.0.1", strBuffer)) {
        memset(strSerial, 0, sizeof(strSerial));
        sprintf(strSerial, "%s%s", AIDON_FIX_NUMBER, strBuffer);
        XDEBUG(" METER SERIAL : %s\r\n", strSerial);
        m_pEndDeviceList->UpdateMeterInventory(&pOndemand->id, 
                UNDEFINED_PORT_NUMBER, PARSER_TYPE_AIDON,
                GetPrefixVendor(), METER_VENDOR_AIDON, 
                NULL, strSerial, 0, 0);
    }
    memset(strBuffer, 0, sizeof(strBuffer));
    if(iec21GetData((const char *)pszBuffer, "1.8.0", strBuffer)) {
        float fPulse = 0.0;
        sscanf(strBuffer, "%f", &fPulse);
        pOndemand->nCurPulse = (UINT)(fPulse * pOndemand->nMeterConstant);
        XDEBUG(" CURRENT PULSE : %d\r\n", pOndemand->nCurPulse);
    }
    pOndemand->pMtrChunk->Add((const char *)pszBuffer, nLength);

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex ++;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return CIec21DataStream::OnDataFrame(pStream, pszBuffer, nLength, pCallData);
}

