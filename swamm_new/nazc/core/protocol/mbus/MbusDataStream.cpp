#include "common.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "MbusDataFrame.h"
#include "MbusChecksum.h"
#include "mbusDataStream.h"

//////////////////////////////////////////////////////////////////////
// CMbusDataStream Data Definitions
//////////////////////////////////////////////////////////////////////

#define STATE_IDENT					0
#define STATE_HEADER				2
#define STATE_DATA					3
#define STATE_TAIL				    100	

//////////////////////////////////////////////////////////////////////
// CMbusDataStream Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMbusDataStream::CMbusDataStream()
{
}

CMbusDataStream::~CMbusDataStream()
{
}

//////////////////////////////////////////////////////////////////////
// CMbusDataStream Operations
//////////////////////////////////////////////////////////////////////

BOOL CMbusDataStream::ParseStream(DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData)
{
	MBUS_HEADER *pHeader;
	MBUS_LONG_HEADER *pLHeader;
	MBUS_TAIL *pTail;
	int			i, nSeek;
	BOOL		bResult;
	BYTE    	c;

	for(i=0; i<nLength; i+=nSeek)
	{
		nSeek = 1;
		c = (pszStream[i] & 0xff);
		switch(pStream->nState) {
            case STATE_IDENT :
                switch(c)
                {
                    case MBUS_FRAME_SINGLE: 
                        CDataStream::AddStream(pStream, c);
			            bResult = OnFrame(pStream, (BYTE *)pStream->pszBuffer, pStream->nLength, pCallData);
			            CDataStream::ClearStream(pStream);
			            pStream->nState = STATE_IDENT;
			            return bResult;
                    case MBUS_FRAME_SHORT: 
			            pStream->nSize  = sizeof(MBUS_SHORT_HEADER);
			            pStream->nState = STATE_HEADER;
                        break;
                    case MBUS_FRAME_LONG: 
			            pStream->nSize  = sizeof(MBUS_LONG_HEADER);
			            pStream->nState = STATE_HEADER;
                        break;
                    default: 
                        // 시작이 틀린 경우 수신된 전체 프레임을 무시한다. 
                        XDEBUG("Receive %0d Byte(s)\xd\xa", nLength); 
                        XDUMP((char *)pszStream, nLength); 
                        XDEBUG(" \033[1;40;31mInvalid Start Flag = 0x%02X\033[0m\r\n", c);
				        return FALSE;
                }
                CDataStream::AddStream(pStream, c);
			   break;

		    case STATE_HEADER :
			    nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
			    CDataStream::AddStream(pStream, (const char *)pszStream+i, nSeek);
			    if (pStream->nLength < pStream->nSize)
				    break;
    
                pHeader = (MBUS_HEADER *)pStream->pszBuffer;
                switch(pHeader->start)
                {
                    case MBUS_FRAME_SHORT:
                        pStream->nSize += sizeof(MBUS_TAIL);
			            pStream->nState = STATE_TAIL;
                        break;
                    case MBUS_FRAME_LONG:
                        pLHeader = (MBUS_LONG_HEADER *)pStream->pszBuffer;
                        if(pLHeader->start != pLHeader->start2 || pLHeader->len != pLHeader->len2 || pLHeader->len < 3)
                        {
                            XDEBUG("Receive %0d Byte(s)\xd\xa", nLength); 
                            XDUMP((char *)pszStream, nLength); 
                            XDEBUG(" \033[1;40;31mInvalid Long Header\033[0m\r\n");

				            CDataStream::ClearStream(pStream);
			   	            pStream->nState = STATE_IDENT;
                            return FALSE;
                        }
                        pStream->nSize += pLHeader->len - 3;
			            pStream->nState = STATE_DATA;
                        break;
                }
			    break;

		    case STATE_DATA :
                nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
                if (!CDataStream::AddStream(pStream, (const char *)pszStream+i, nSeek))
                    return FALSE;

                if (pStream->nLength < pStream->nSize)
                    break;

                pStream->nSize += sizeof(MBUS_TAIL);
			    pStream->nState = STATE_TAIL;
                break;

            case STATE_TAIL:
                nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
                if (!CDataStream::AddStream(pStream, (const char *)pszStream+i, nSeek))
                    return FALSE;

                if (pStream->nLength < pStream->nSize)
                    break;

                pHeader = (MBUS_HEADER *)pStream->pszBuffer;
                pTail = (MBUS_TAIL *)&pStream->pszBuffer[pStream->nLength-2];

                switch(pHeader->start)
                {
                    case MBUS_FRAME_SHORT:
                        if(!VerifyMbusChecksum((BYTE *)(pStream->pszBuffer+1), 2, pTail->checksum))
                        {
				            XDEBUG("Receive %0d Byte(s)\xd\xa", pStream->nLength);
				            XDUMP((char *)pStream->pszBuffer, pStream->nLength);
				            XDEBUG(" \033[1;40;31mChecksum Error!!\033[0m\xd\xa");

				            CDataStream::ClearStream(pStream);
				            pStream->nState = STATE_IDENT;
                            return FALSE;
                        }
                        break;
                    case MBUS_FRAME_LONG:
                        pLHeader = (MBUS_LONG_HEADER *)pStream->pszBuffer;
                        if(!VerifyMbusChecksum((BYTE *)(pStream->pszBuffer+4), pLHeader->len, pTail->checksum))
                        {
				            XDEBUG("Receive %0d Byte(s)\xd\xa", pStream->nLength);
				            XDUMP((char *)pStream->pszBuffer, pStream->nLength);
				            XDEBUG(" \033[1;40;31mChecksum Error!!\033[0m\xd\xa");

				            CDataStream::ClearStream(pStream);
				            pStream->nState = STATE_IDENT;
                            return FALSE;
                        }
                        break;
                }

			   bResult = OnFrame(pStream, (BYTE *)pStream->pszBuffer, pStream->nLength, pCallData);
			   CDataStream::ClearStream(pStream);
			   pStream->nState = STATE_IDENT;
			   return bResult;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMbusDataStream Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CMbusDataStream::OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
	XDEBUG(" MBUS OnFrame : Receive %0d Byte(s)\xd\xa", pStream->nLength);
	return TRUE;
}

