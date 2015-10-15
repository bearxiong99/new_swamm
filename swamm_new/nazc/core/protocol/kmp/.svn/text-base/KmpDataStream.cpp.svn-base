#include "common.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "KmpDataFrame.h"
#include "KmpCrcCheck.h"
#include "kmpDataStream.h"

//////////////////////////////////////////////////////////////////////
// CKmpDataStream Data Definitions
//////////////////////////////////////////////////////////////////////

#define STATE_IDENT					0
#define STATE_HEADER				2
#define STATE_DATA_TAIL				3

//////////////////////////////////////////////////////////////////////
// CKmpDataStream Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKmpDataStream::CKmpDataStream()
{
}

CKmpDataStream::~CKmpDataStream()
{
}

//////////////////////////////////////////////////////////////////////
// CKmpDataStream Operations
//////////////////////////////////////////////////////////////////////

BOOL CKmpDataStream::ParseStream(DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData)
{
	KMP_HEADER *pHeader;
    BYTE        szBuffer[256];
	int			i, nSeek, nLen;
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
                    case KMP_ACK: 
                        CDataStream::AddStream(pStream, c);
			            bResult = OnFrame(pStream, (BYTE *)pStream->pszBuffer, pStream->nLength, pCallData);
			            CDataStream::ClearStream(pStream);
			            pStream->nState = STATE_IDENT;
			            return bResult;
                    case KMP_SOF_RES: 
			            pStream->nSize  = sizeof(KMP_HEADER);
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
    
                pHeader = (KMP_HEADER *)pStream->pszBuffer;
                pStream->nSize += sizeof(KMP_TAIL);
			    pStream->nState = STATE_DATA_TAIL;
			    break;

		    case STATE_DATA_TAIL :
                nSeek = nLength-i;
                if (!CDataStream::AddStream(pStream, (const char *)pszStream+i, nSeek))
                    return FALSE;

                if (pStream->pszBuffer[pStream->nLength-1] != KMP_EOF)
                    break;

                /** Head/Tail Byte는 뺀다 */
                nLen = StripFrame((BYTE *)(pStream->pszBuffer+1), pStream->nLength - 2, szBuffer);
                memcpy(pStream->pszBuffer+1, szBuffer, nLen);
                pStream->pszBuffer[1+nLen] = pStream->pszBuffer[pStream->nLength-1];
                pStream->nLength = nLen + 2;

                if(!KmpVerifyCrc((BYTE *)pStream->pszBuffer, pStream->nLength))
                {
				    XDEBUG("Receive %0d Byte(s)\xd\xa", pStream->nLength);
				    XDUMP((char *)pStream->pszBuffer, pStream->nLength);
				    XDEBUG(" \033[1;40;31mCRC Error!!\033[0m\xd\xa");

				    CDataStream::ClearStream(pStream);
				    pStream->nState = STATE_IDENT;
                    return FALSE;
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
// CKmpDataStream Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CKmpDataStream::OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
	XDEBUG(" KMP OnFrame : Receive %0d Byte(s)\xd\xa", pStream->nLength);
	return TRUE;
}

