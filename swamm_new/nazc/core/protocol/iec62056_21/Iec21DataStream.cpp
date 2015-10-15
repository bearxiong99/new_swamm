#include "common.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "Iec21DataFrame.h"
#include "Iec21Code.h"
#include "iec62056_21Define.h"
#include "iec62056_21DataStream.h"

//////////////////////////////////////////////////////////////////////
// CIec21DataStream Data Definitions
//////////////////////////////////////////////////////////////////////

#define STATE_IDENT					0
#define STATE_IDENT_HDR             1
#define STATE_IDENT_WIDE            2
#define STATE_IDENT_BAUD            3
#define STATE_IDENT_IDENT           4
#define STATE_DATA                  5
#define STATE_CMD                   6
#define STATE_CMD_TYPE              7
#define STATE_CR				    100
#define STATE_LF				    101
#define STATE_STX				    102
#define STATE_ETX				    103
#define STATE_START				    104
#define STATE_END				    105
#define STATE_DONE                  200


#define IEC_IDENT_MSG               1
#define IEC_DATA_MSG                2
#define IEC_ACK_MSG                 3
#define IEC_NAK_MSG                 4
#define IEC_CMD_MSG                 5
#define IEC_ERR_MSG                 6
#define IEC_BREAK_MSG               7

//////////////////////////////////////////////////////////////////////
// CIec21DataStream Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIec21DataStream::CIec21DataStream()
{
}

CIec21DataStream::~CIec21DataStream()
{
}

//////////////////////////////////////////////////////////////////////
// CIec21DataStream Operations
//////////////////////////////////////////////////////////////////////

BOOL CIec21DataStream::ParseStream(DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData)
{
	int			i, nSeek;
	BOOL		bResult = FALSE;
	BYTE    	c;

	for(i=0; i<nLength; i+=nSeek)
	{
		nSeek = 1;
		c = (pszStream[i] & 0x7F);
		switch(pStream->nState) {
            case STATE_IDENT :
                switch(c) {
                    case IEC_21_START:      // Identification   (Request는 보내기만 한다)
                        pStream->nSize = 4;
                        pStream->nState = STATE_IDENT_HDR;
                        pStream->nService = IEC_IDENT_MSG;
                        break;
                    case IEC_21_ACK:        // Ack          (Ack & Option 은 보내기만 한다)
			            bResult = OnAck(pStream, pCallData);
                        nSeek = 0;
                        pStream->nState = STATE_DONE;
                        break;  
                    case IEC_21_NAK:        // Nak
			            bResult = OnNak(pStream, pCallData);
                        nSeek = 0;
                        pStream->nState = STATE_DONE;
                        break;
                    case IEC_21_STX:        // Data, Error
                        pStream->nState = STATE_DATA;
                        pStream->nService = IEC_DATA_MSG;
                        break;
                    case IEC_21_SOH:        // Command, Break
                        pStream->nState = STATE_CMD;
                        break;  
                    default:
                        break;
                }
                break;

            case STATE_STX:
                if (c != IEC_21_STX) {
			        CDataStream::ClearStream(pStream);
			        pStream->nState = STATE_IDENT;
                    break;
                }
                pStream->nState = STATE_DATA;
                break;

		    case STATE_IDENT_HDR :
			    nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
			    CDataStream::AddStream(pStream, (const char *)pszStream+i, nSeek);
			    if (pStream->nLength < pStream->nSize)
				    break;
			    pStream->nState = STATE_IDENT_WIDE;
			    break;

            case STATE_IDENT_WIDE:
                if(c == 0x5C) { // '\'
			        CDataStream::AddStream(pStream, c);
                    pStream->nSize = 16;
                    pStream->nState = STATE_IDENT_BAUD;
                }else {
                    nSeek = 0;
                    pStream->nState = STATE_IDENT_IDENT;
                }
                break;

		    case STATE_IDENT_BAUD :
			    nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
			    CDataStream::AddStream(pStream, (const char *)pszStream+i, nSeek);
			    if (pStream->nLength < pStream->nSize)
				    break;
			    pStream->nState = STATE_IDENT_IDENT;
			    break;

            case STATE_IDENT_IDENT :
                if (c == IEC_21_CR)
                {
                    pStream->nState = STATE_LF;
                    break;
                }
                if (!CDataStream::AddStream(pStream, c))
                {
			        CDataStream::ClearStream(pStream);
			        pStream->nState = STATE_IDENT;
                    return FALSE;
                }
                break;

		    case STATE_DATA :
                if (c == IEC_21_END)
                {
                    pStream->nState = STATE_CR;
                    break;
                }
                if (c == IEC_21_ETX) 
                {
                    nSeek = 0;
                    pStream->nState = STATE_ETX;
                    break;
                }
                if (!CDataStream::AddStream(pStream, c))
                {
			        CDataStream::ClearStream(pStream);
			        pStream->nState = STATE_IDENT;
                    return FALSE;
                }
                break;

            case STATE_CMD:
			    CDataStream::AddStream(pStream, c);
                switch(c) {
                    case IEC_21_CMD_BREAK:
                        pStream->nService = IEC_BREAK_MSG;
                        break;
                    default:
                        pStream->nService = IEC_CMD_MSG;
                        break;
                }
                pStream->nState = STATE_CMD_TYPE;
                break;

            case STATE_CMD_TYPE:
			    CDataStream::AddStream(pStream, c);
                switch(pStream->nService) {
                    case IEC_BREAK_MSG:
                        pStream->nState = STATE_ETX;
                        break;
                    default:
                        pStream->nState = STATE_STX;
                        break;
                }
                break;

		    case STATE_END :
                if (c == IEC_21_END)
                {
                    pStream->nState = STATE_CR;
                    break;
                }
			    CDataStream::ClearStream(pStream);
			    pStream->nState = STATE_IDENT;
                break;

		    case STATE_CR :
                if (c == IEC_21_CR)
                {
                    pStream->nState = STATE_LF;
                    break;
                }
			    CDataStream::ClearStream(pStream);
			    pStream->nState = STATE_IDENT;
                break;

		    case STATE_LF :
                if (c == IEC_21_LF)
                {
                    switch(pStream->nService) {
                        case IEC_IDENT_MSG:
                            bResult = OnIdentificationFrame(pStream, 
                                    (BYTE *)pStream->pszBuffer,                 // Manufacturer ident (3)
                                    (BYTE)pStream->pszBuffer[3],                // Baud
                                    pStream->pszBuffer[4] == 0x5C ?             // Wide Serial Setting 
                                            (BYTE *) (pStream->pszBuffer+5) :       // (16bytes)
                                            NULL,
                                    pStream->pszBuffer[4] == 0x5C ?             // Identification
                                            (BYTE *) (pStream->pszBuffer+5+16) :   
                                            (BYTE *) (pStream->pszBuffer+4) ,   
                                    pStream->pszBuffer[4] == 0x5C ?             // Length
                                            pStream->nLength - (5 + 16): 
                                            pStream->nLength - 4, 
                                    pCallData);
                            nSeek = 0;
                            pStream->nState = STATE_DONE;
                            break;
                        case IEC_DATA_MSG:
                            pStream->nState = STATE_ETX;
                            break;
                        default:
			                CDataStream::ClearStream(pStream);
			                pStream->nState = STATE_IDENT;
                            break;
                    }
                }else {
			        CDataStream::ClearStream(pStream);
			        pStream->nState = STATE_IDENT;
                }
                break;

		    case STATE_ETX :
                if (c != IEC_21_ETX)
                {
			        CDataStream::ClearStream(pStream);
			        pStream->nState = STATE_IDENT;
                    break;
                }

                switch(pStream->nService) {
                    case IEC_BREAK_MSG:
                        bResult = OnBreakFrame(pStream, pCallData);
                        break;
                    case IEC_CMD_MSG:
			            bResult = OnCommandFrame(pStream, 
                                (BYTE)pStream->pszBuffer[0],        // Command
                                (BYTE)pStream->pszBuffer[1],        // Command Type
                                (BYTE *)(pStream->pszBuffer+2), pStream->nLength-2, pCallData);
                        break;
                    default:
			            bResult = OnDataFrame(pStream, (BYTE *)pStream->pszBuffer, pStream->nLength, pCallData);
                        break;
                }
                /** Skip BCC 
                 *  nSeek = 0;
                 */
			    pStream->nState = STATE_DONE;
			    break;

            case STATE_DONE:
			    CDataStream::ClearStream(pStream);
			    pStream->nState = STATE_IDENT;
                return bResult;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CIec21DataStream Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CIec21DataStream::OnAck(DATASTREAM *pStream, void *pCallData)
{
	XDEBUG(" IEC62056-21 Ack \xd\xa");
	return TRUE;
}

BOOL CIec21DataStream::OnNak(DATASTREAM *pStream, void *pCallData)
{
	XDEBUG(" IEC62056-21 Nak \xd\xa");
	return TRUE;
}

BOOL CIec21DataStream::OnBreakFrame(DATASTREAM *pStream, void *pCallData)
{
	XDEBUG(" IEC62056-21 Break \xd\xa");
	return TRUE;
}

BOOL CIec21DataStream::OnIdentificationFrame(DATASTREAM *pStream, BYTE* manufacturerId, BYTE baud, BYTE *serialSetting, BYTE * identification, 
        int nLength, void *pCallData)
{
    BYTE manuf[4];
    memset(manuf, 0, sizeof(manuf));
    memcpy(manuf, manufacturerId, 3);

	XDEBUG(" IEC62056-21 Identification : %s %c Receive %0d Byte(s)\xd\xa", manuf, baud, nLength);
#if 0
    if(nLength > 0) {
        XDUMP((const char *)identification, nLength, TRUE);
    }
#endif
	return TRUE;
}

BOOL CIec21DataStream::OnCommandFrame(DATASTREAM *pStream, BYTE cmd, BYTE cmdType, BYTE *pszBuffer, int nLength, void *pCallData)
{
	XDEBUG(" IEC62056-21 Command : %c %c Receive %0d Byte(s)\xd\xa", cmd, cmdType, nLength);
#if 0
    if(nLength > 0) {
        XDUMP((const char *)pszBuffer, nLength, TRUE);
    }
#endif
	return TRUE;
}

BOOL CIec21DataStream::OnDataFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
	XDEBUG(" IEC62056-21 Data : Receive %0d Byte(s)\xd\xa", nLength);
#if 0
    if(nLength > 0) {
        XDUMP((const char *)pszBuffer, nLength, TRUE);
    }
#endif
	return TRUE;
}

