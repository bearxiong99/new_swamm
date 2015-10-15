/////////////////////////////////////////////////////////////////////
//
//  ParserKukdong.cpp: implementation of the CKukdongParser class.
//
//  This file is a part of the AIMIR.
//  (c)Copyright 2010 NURITELECOM Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@paran.com
//  http://www.nuritelecom.com
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
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "ParserKukdong.h"

#include "kukdongFrame.h"

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////


#define PARSE_SOF					0
#define PARSE_HEADER				1
#define PARSE_DATA					2
#define PARSE_CRC					3
#define PARSE_DONE					100

//////////////////////////////////////////////////////////////////////
// Static variable definitions
//////////////////////////////////////////////////////////////////////

typedef struct {
    BYTE        nType;
    int         nPulse;
    BYTE        szSerial[12];
} KUKDONGPARAM;

// CODIAPI functions
extern UINT GetCrc16(unsigned char *pszStream, int nLength);

static const char *keukdongNameArray[] = { "KEUKDONG-PULSE", "NAPC-G202SR", "NAPC-W202SR", "KEUKDONG", 
        "KEUKDONG-WATER", "KEUKDONG-GAS", NULL };
     
//////////////////////////////////////////////////////////////////////
// CKukdongParser Class
//////////////////////////////////////////////////////////////////////

CKukdongParser::CKukdongParser() : CPulseBaseParser(SENSOR_TYPE_ZEUPLS, SERVICE_TYPE_UNKNOWN, 
            ENDDEVICE_ATTR_RFD | ENDDEVICE_ATTR_BATTERYNODE,
			const_cast<char **>(keukdongNameArray), PARSER_TYPE_KEUKDONG, 
			"Kukdong Meter Parser", "KEUKDONG", "PULSE-KEUKDONG", 
			100000, 6000, 1)
{
}

CKukdongParser::~CKukdongParser()
{
}

//////////////////////////////////////////////////////////////////////
// CKukdongParser Operations
//////////////////////////////////////////////////////////////////////

int CKukdongParser::METERING(CMDPARAM *pCommand, METER_STAT *pStat)
{
    UINT                nPulse;
    int                 nDataLen=0;
    BYTE                pBypassData[256];
    int                 nOption=0, nOffset=1, nCount=2;

    memset(pBypassData, 0, sizeof(pBypassData));

    /*
 *		cmdSetMeterPulse(105.12)
 *				4.3.1		EUI64		EUI64 ID
 *				1.4			BYTE		Data Mask (0x00:Pulse만 수정)
 *				1.6			UINT		Current Pulse
 *				1.11		STRING		Meter serial (Do not used)
     */
    switch(pCommand->nOption)
    {
        case WRITE_OPTION_KD_CONFIG:
            if(pCommand->argc>2)
            {
                nPulse = HostToBigInt(pCommand->argv[2].stream.u32);
                memcpy(pBypassData, &pCommand->argv[1].stream.u8, sizeof(BYTE)); nDataLen += 1;
                memcpy(pBypassData + nDataLen, &nPulse, sizeof(UINT)); nDataLen += 4;

                if(pCommand->argv[1].stream.u8!= 0x00 && pCommand->argc>3) // Pulse + Meter Serial
                {
                    memcpy(pBypassData + nDataLen, pCommand->argv[3].stream.p, pCommand->argv[3].len); nDataLen += 12;
                }
            }
            break;
        case READ_OPTION_KD_METER_STATUS:
        case READ_OPTION_KD_METER_VERSION:
        case WRITE_OPTION_KD_VALVE_OPEN:
        case WRITE_OPTION_KD_VALVE_CLOSE:
        case WRITE_OPTION_KD_VALVE_STANDBY:
            break;
        default:
            // Issue #855 Metering Option 얻어오기
            if(m_pEndDeviceList->GetMeteringOption(&pCommand->id, 
                        pCommand->nOption < 0 ? &nOption : NULL,
                        pCommand->nOffset < 0 ? &nOffset : NULL,
                        pCommand->nCount <= 0 ? &nCount : NULL))
            {
                XDEBUG("Select Metering Options %d %d %d\r\n", nOption, nOffset, nCount);
            }
            
            if(pCommand->nOption >= 0) nOption = pCommand->nOption;
            if(pCommand->nOffset >= 0) nOffset = pCommand->nOffset;
            if(pCommand->nCount > 0) nCount = pCommand->nCount;

            if (nOffset > 39) nOffset = 39;

            nCount = MIN(40, nCount);
            if (nCount <= 0) nCount = 2;

            if(nCount > nOffset + 1) nCount = nOffset + 1;
            return DoMetering(pCommand, nOption, nOffset, nCount, NULL, pStat);
    }

    return DoMetering(pCommand, pCommand->nOption, 0, nDataLen, pBypassData, pStat);
}

int CKukdongParser::StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    int nState = STATE_READ_AMRDATA;

    switch(pOndemand->nOption)
    {
        case READ_OPTION_KD_METER_STATUS:
        case READ_OPTION_KD_METER_VERSION:
        case WRITE_OPTION_KD_VALVE_OPEN:
        case WRITE_OPTION_KD_VALVE_CLOSE:
        case WRITE_OPTION_KD_VALVE_STANDBY:
        case WRITE_OPTION_KD_CONFIG:
            nState = STATE_INITIALIZE;
            break;
    }

    return ReadRomMap(ENDI_ROMAP_NODE_KIND, sizeof(ENDI_NODE_INFO), pOndemand, szBuffer, 
            STATE_READ_NODEINFO, nState);
}

int CKukdongParser::StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    return STATE_QUERY;
}

int CKukdongParser::StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
	OADI_FRAME	*pFrame;
	BYTE	szOaidBuffer[128];
	OADI_KD_VALVE_PAYLOAD	*pValve;
	int		nError, nLength = 0;
	int		nState = STATE_WAIT_REPLY;

	// 극동 모뎀의 경우 배터리 문제로 바로 바로 연속으로 명령을 전달하면 문제가 발생한다.
	// 다음 명령을 전송할때까지 1초를 대기한다.
	usleep(200000);

    if(pOndemand->nOption <= 0) // 기본 Option 일 경우 Meter Status 와 Version 정보를 읽는다.
    {
        pOndemand->nOption = READ_OPTION_KD_METER_STATUS;
    }

    /*
     *      cmdSetMeterSerial(105.12)
     *              4.3.1       EUI64       EUI64 ID
     *              1.4         BYTE        Data Mask (0x01:Pulse, serial 모두 수정)
     *              1.6         UINT        Current Pulse
     *              1.11        STRING      Meter serial (Do not used)
     */
	pFrame = (OADI_FRAME *)szOaidBuffer;
	switch(pOndemand->nOption) {
        case READ_OPTION_KD_METER_STATUS:
		   nLength = oadiReadCommandFrame(pFrame, OADI_KD_STATUS);
		   break;

        case READ_OPTION_KD_METER_VERSION:
		   nLength = oadiReadCommandFrame(pFrame, OADI_KD_VERSION);
		   break;

        case WRITE_OPTION_KD_CONFIG:
           if(pOndemand->nCount > 4 && pOndemand->pBypassData != NULL)
           {
		        nLength = oadiWriteCommandFrame(pFrame, OADI_KD_AMR, (BYTE *)pOndemand->pBypassData, pOndemand->nCount);
           }
		   break;

        case WRITE_OPTION_KD_VALVE_OPEN:
		   pValve = (OADI_KD_VALVE_PAYLOAD *)szBuffer;
		   pValve->valve = (BYTE)0;
		   nLength = oadiWriteCommandFrame(pFrame, OADI_KD_VALVE, (BYTE *)pValve, sizeof(OADI_KD_VALVE_PAYLOAD));
		   break;

        case WRITE_OPTION_KD_VALVE_CLOSE:
		   pValve = (OADI_KD_VALVE_PAYLOAD *)szBuffer;
		   pValve->valve = (BYTE)1;
		   nLength = oadiWriteCommandFrame(pFrame, OADI_KD_VALVE, (BYTE *)pValve, sizeof(OADI_KD_VALVE_PAYLOAD));
		   break;

        case WRITE_OPTION_KD_VALVE_STANDBY:
		   pValve = (OADI_KD_VALVE_PAYLOAD *)szBuffer;
		   pValve->valve = (BYTE)2;
		   nLength = oadiWriteCommandFrame(pFrame, OADI_KD_VALVE, (BYTE *)pValve, sizeof(OADI_KD_VALVE_PAYLOAD));
		   break;
        default:
           nState = STATE_OK;
           break;
	}

	if (nLength <= 0 || nState == STATE_OK)
		return STATE_OK;

	nError = endiBypass(pOndemand->endi, (BYTE *)pFrame, nLength, GetReplyTimeout());
	if(nError !=  CODIERR_NOERROR)
    {
        return STATE_ERROR;
    }

    m_Locker.Lock();
    ClearOperation(pOndemand);
    pOndemand->bReply = FALSE;
    ResetEvent(pOndemand->hEvent);
    m_Locker.Unlock();

	return nState;
}

int CKukdongParser::StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
	OADI_FRAME					*pFrame;
	OADI_KD_STATUS_PAYLOAD	*pStatus;
	OADI_KD_AMR_PAYLOAD		*pAmr;
	OADI_KD_VALVE_PAYLOAD	*pValve;
	OADI_KD_VERSION_PAYLOAD	*pVersion;
	char	szSerial[30], szHw[10], szSw[10], szDateTime[100];
	BYTE	nType, nCommand;
	WORD	ctrl;
	int		nState = STATE_SEND_COMMAND;

	if (pOndemand == NULL)
		return STATE_ERROR;

    WaitForSingleObject(pOndemand->hEvent, (int)(GetReplyTimeout()/1000));
    if(pOndemand->bPartialWait)
    {
        pOndemand->bPartialWait = FALSE;
        return STATE_WAIT_REPLY;
    }

    if(!pOndemand->bReply)
    {
        /** Timeout */
        pOndemand->nRetry ++;
        XDEBUG(" FAIL COUNT SUCC %d\r\n", pOndemand->nRetry);
        if (pOndemand->nRetry >= 3)
            return STATE_ERROR;
        else
            return STATE_QUERY;
    }

    pOndemand->nRetry = 0;

	pFrame = (OADI_FRAME *)pOndemand->pszBuffer;
	if ((pFrame == NULL) || (pOndemand->nLength <= 0))
		return STATE_ERROR;

	ctrl	 = ntohs(pFrame->hdr.fctrl);
	nType	 = ctrl >> 8;
	nCommand = ctrl & 0xff;

	XDEBUG("OADI-REPLY: TYPE=0x%02X, COMMAND=%d, LENGTH=%d\r\n", nType, nCommand, pFrame->hdr.len);

	switch(nCommand) {
	  case OADI_KD_STATUS :
		   // 다른 시퀀스 프레임이면 버리기
		   if (pOndemand->nOption != READ_OPTION_KD_METER_STATUS)
		   {
			    XDEBUG(" \033[1;40;31m** Invalid meter response **\033[0m\r\n");
				break;
		   }
		   pStatus = (OADI_KD_STATUS_PAYLOAD *)pFrame->payload;
		   memset(szSerial, 0, sizeof(szSerial));
		   strncpy(szSerial, pStatus->meterSerial, 12);
		   XDEBUG("  OADI-STATUS: PULSE=%d, SERIAL='%s', ALARM=0x%02X, STATUS=0x%02X\r\n",
					ntohl(pStatus->curPulse), szSerial, pStatus->alarmStatus, pStatus->meterStatus);

           pOndemand->pMtrChunk->Add((char *)pStatus, sizeof(OADI_KD_STATUS_PAYLOAD));
           pOndemand->nOption = READ_OPTION_KD_METER_VERSION; // Meter Version을 읽는다.

		   m_pEndDeviceList->UpdateMeterInventory(&pOndemand->id, 0, PARSER_TYPE_KEUKDONG, 
                    "NURI", METER_VENDOR_NURI, NULL, szSerial, 0, 0);
		   m_pEndDeviceList->VerifyEndDevice(&pOndemand->id);

		   nState = STATE_QUERY;
		   break;

	  case OADI_KD_AMR :
		   // 다른 시퀀스 프레임이면 버리기
		   if (pOndemand->nOption != WRITE_OPTION_KD_CONFIG)
		   {
			   XDEBUG(" \033[1;40;31m** Invalid meter response **\033[0m\r\n");
			   break;
		   }
		   pAmr = (OADI_KD_AMR_PAYLOAD *)pFrame->payload;
		   if (pAmr->writeType == 0x01)
		   {
			   memset(szSerial, 0, sizeof(szSerial));
			   strncpy(szSerial, pAmr->meterSerial, 12);
		   }
		   XDEBUG("  OADI-AMR: TYPE=%0d, PULSE=%d, SERIAL='%s'\r\n",
					pAmr->writeType, ntohl(pAmr->curPulse), szSerial);
		   pOndemand->pMtrChunk->Add((char *)pAmr, sizeof(OADI_KD_AMR_PAYLOAD));
		   nState = STATE_OK;
		   break;

	  case OADI_KD_VALVE :
		   // 다른 시퀀스 프레임이면 버리기
		   if (pOndemand->nOption != WRITE_OPTION_KD_VALVE_OPEN && pOndemand->nOption != WRITE_OPTION_KD_VALVE_CLOSE
                   && pOndemand->nOption != WRITE_OPTION_KD_VALVE_STANDBY)
		   {
			   XDEBUG(" \033[1;40;31m** Invalid meter response **\033[0m\r\n");
			   break;
		   }
		   pValve = (OADI_KD_VALVE_PAYLOAD *)pFrame->payload;
		   XDEBUG("  OADI-VALVE: VALVE=%0d\r\n", pValve->valve);
		   pOndemand->pMtrChunk->Add((char *)pValve, sizeof(OADI_KD_VALVE_PAYLOAD));
		   nState = STATE_OK;
		   break;

	  case OADI_KD_VERSION :
		   // 다른 시퀀스 프레임이면 버리기
		   if (pOndemand->nOption != READ_OPTION_KD_METER_VERSION)
		   {
			   XDEBUG(" \033[1;40;31m** Invalid meter response **\033[0m\r\n");
			   break;
		   }
		   pVersion = (OADI_KD_VERSION_PAYLOAD *)pFrame->payload;
		   memset(szHw, 0, sizeof(szHw));
		   memset(szSw, 0, sizeof(szSw));
		   memset(szDateTime, 0, sizeof(szDateTime));
		   strncpy(szHw, pVersion->hwVersion, 7);
		   strncpy(szSw, pVersion->swVersion, 7);
		   strncpy(szDateTime, pVersion->testDate, 6);
		   XDEBUG("  OADI-VERSION: DATE=%s, TEST=%c, HW=%s, SW=%s\r\n", 
					szDateTime, pVersion->testResult > 0 ? pVersion->testResult : '?', szHw, szSw);
		   pOndemand->pMtrChunk->Add((char *)pVersion, sizeof(OADI_KD_VERSION_PAYLOAD));
		   nState = STATE_OK;
		   break;
	}
	return nState;
}

//////////////////////////////////////////////////////////////////////
// CKukdongParser Operations
//////////////////////////////////////////////////////////////////////

void CKukdongParser::OnData(EUI64 *pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
{
	OADI_FRAME	*pFrame;
	OADI_TAIL	*pTail;
	WORD		ctrl, crc;
	BYTE		c;
	int			i, nSeek;

	// pOndemand는 NULL일 수 있다.
	if (pOndemand == NULL) return;

	// 프레임이 분할되어서 수신되지 않으므로 조합하지 않는다.
	// 추후 조합이 필요하면, 이부분을 제거한다.
	pOndemand->nState = PARSE_SOF;

	// 프레임을 해석한다.
	for(i=0; i<nLength; i+=nSeek)
	{
		nSeek = 1;
		c = pszData[i];

		switch(pOndemand->nState) {
		  case PARSE_SOF :
			   if (c != OADI_IDENT)
				   break;
			   pOndemand->nLength = 0;
			   pOndemand->pszBuffer[pOndemand->nLength] = c;
			   pOndemand->nLength++;
			   pOndemand->nState = PARSE_HEADER;
			   break;

		  case PARSE_HEADER :
			   nSeek = MIN((int)sizeof(OADI_HEADER)-pOndemand->nLength, nLength-i);
			   memcpy(&pOndemand->pszBuffer[pOndemand->nLength], &pszData[i], nSeek);
			   pOndemand->nLength += nSeek;
			   if ((UINT)pOndemand->nLength < sizeof(OADI_HEADER))
				   break;
			   pFrame = (OADI_FRAME *)pOndemand->pszBuffer;
			   pOndemand->nSize = sizeof(OADI_HEADER) + pFrame->hdr.len + sizeof(OADI_TAIL);
			   if (pOndemand->nSize > 128)
			   {
				   pOndemand->nLength = 0;
				   pOndemand->nState = PARSE_SOF;
				   break;
			   }
			   pOndemand->nState = PARSE_DATA;
			   break;

		  case PARSE_DATA :
			   nSeek = MIN(pOndemand->nSize-pOndemand->nLength, nLength-i);
			   memcpy(&pOndemand->pszBuffer[pOndemand->nLength], &pszData[i], nSeek);
			   pOndemand->nLength += nSeek;
			   if (pOndemand->nLength < pOndemand->nSize)
				   break;
			   pOndemand->nState = PARSE_CRC;
			   // break하지 않는다.

		  case PARSE_CRC :
			   pFrame = (OADI_FRAME *)pOndemand->pszBuffer;
			   pTail  = (OADI_TAIL *)(pOndemand->pszBuffer + sizeof(OADI_HEADER) + pFrame->hdr.len);

			   crc = GetCrc16((BYTE *)&pFrame->payload, pFrame->hdr.len);
			   if (crc == pTail->crc)
			   {
				   // Request는 Echo된 문장이므로 버린다.
				   ctrl = ntohs(pFrame->hdr.fctrl);
				   if (((ctrl >> 8) & OADI_ACTION_RESPONSE) == OADI_ACTION_RESPONSE)
				   {
					   pOndemand->nResult = 0;
                        // Event signal
                        m_Locker.Lock();
                        pOndemand->bReply = TRUE;
                        m_Locker.Unlock();
                        SetEvent(pOndemand->hEvent);
				   }
			   }
			   else
			   {
		       	   XDEBUG("CRC Error (gen=0x%04X, crc=0x%04X)\r\n", crc, pTail->crc); 
			   }
		  
			   // 하나의 프레임 이상 처리하지 않는다.
			   pOndemand->nState = PARSE_SOF;

			   // 한번에 여러 프레임이 수신시 이부분을 break로 대체한다.
			   return;
		}
	}
}

int CKukdongParser::oadiReadCommandFrame(OADI_FRAME *pFrame, BYTE nCommand)
{
	int		nLength;
	WORD	ctrl;

	if (pFrame == NULL)
		return 0;

	ctrl  = (OADI_ACTION_READ_REQUEST | OADI_TYPE_EM250) << 8;
	ctrl += nCommand;

	nLength = oadiMakeFrame(pFrame, ctrl, NULL, 0);
	return nLength;
}

int CKukdongParser::oadiWriteCommandFrame(OADI_FRAME *pFrame, BYTE nCommand, BYTE *pszData, BYTE len)
{
	int		nLength;
	WORD	ctrl;

	if (pFrame == NULL)
		return 0;

	if ((len > 0) && (pszData == NULL))
		return 0;

	ctrl  = (OADI_ACTION_WRITE_REQUEST | OADI_TYPE_EM250) << 8;
	ctrl += nCommand;

	nLength = oadiMakeFrame(pFrame, ctrl, pszData, len);
	return nLength;
}

int CKukdongParser::oadiMakeFrame(OADI_FRAME *pFrame, WORD ctrl, BYTE *pszData, BYTE len)
{
	OADI_TAIL	*pTail;
	int			nLength;

	pFrame->hdr.ident = OADI_IDENT;
	pFrame->hdr.fctrl = htons(ctrl);
	pFrame->hdr.len   = len;

	memcpy(&pFrame->payload, pszData, len);

	pTail = (OADI_TAIL *)((BYTE *)pFrame + sizeof(OADI_HEADER) + len);
    pTail->crc = GetCrc16((BYTE *)pFrame->payload, len); 

	nLength = sizeof(OADI_HEADER) + sizeof(OADI_TAIL) + len;
	return nLength;
}

