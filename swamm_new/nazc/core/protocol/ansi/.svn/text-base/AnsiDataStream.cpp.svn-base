#include "common.h"
#include "DataStream.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "AnsiUtil.h"
#include "AnsiCrcCheck.h"

#include "ansiFrame.h"
#include "ansiDataStream.h"
#include "ansiUtils.h"

//////////////////////////////////////////////////////////////////////
// CAnsiDataStream Data Definitions
//////////////////////////////////////////////////////////////////////

#define STATE_IDENT					0
#define STATE_CHECK_START			1
#define STATE_HEADER				2
#define STATE_DATA					3
#define STATE_CRC					100	
#define STATE_DONE					200


//////////////////////////////////////////////////////////////////////
// CAnsiDataStream Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnsiDataStream::CAnsiDataStream()
{
}

CAnsiDataStream::~CAnsiDataStream()
{
}

//////////////////////////////////////////////////////////////////////
// CAnsiDataStream Operations
//////////////////////////////////////////////////////////////////////

BOOL CAnsiDataStream::ParseStream(ANSISESSION *pSession, DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData)
{
    ANSI_HEADER *pHeader;
	int			i, len, nSeek;
	BOOL		bResult = FALSE;
	BYTE    	c;

	for(i=0; i<nLength; i+=nSeek)
	{
		nSeek = 1;
		c = (pszStream[i] & 0xff);
		switch(pStream->nState) 
        {
		  case STATE_IDENT :
               if (c == ANSI_ACK)
               {
                   // ACK
                   continue;
               }
			   if (c != ANSI_STP)
			   {
				   // 시작이 틀린 경우 수신된 전체 프레임을 무시한다.
				   XDEBUG("Receive %0d Byte(s)\xd\xa", nLength);
				   XDUMP((char *)pszStream, nLength);
				   XDEBUG(" \033[1;40;31mInvalid Start Flag = 0x%02X\033[0m\r\n", c);
				   return FALSE;
			   }
               CDataStream::AddStream(pStream, ANSI_STP);

			   // HEADER 
			   pStream->nSize  = sizeof(ANSI_HEADER);
			   pStream->nState = STATE_HEADER;
			   break;

		  case STATE_HEADER :
			   nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
			   CDataStream::AddStream(pStream, (const char *)pszStream+i, nSeek);
			   if (pStream->nLength < pStream->nSize)
				   break;

               pHeader = (ANSI_HEADER *)pStream->pszBuffer;
			   len     = BigToHostShort(pHeader->length); 

               pStream->nSize  += len + 2;      // payload + crc
			   pStream->nState = STATE_DATA;
			   break;

		  case STATE_DATA :
               nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
               if (!CDataStream::AddStream(pStream, (const char *)pszStream+i, nSeek))
                   return FALSE;

               if (pStream->nLength < pStream->nSize)
                   break;

			   pStream->nState = STATE_DONE;

		  case STATE_DONE :
			   if (!AnsiVerifyCrc((BYTE *)pStream->pszBuffer, pStream->nSize))
			   {
				   XDEBUG("Receive %0d Byte(s)\xd\xa", pStream->nLength);
				   XDUMP((char *)pStream->pszBuffer, pStream->nLength);
				   XDEBUG(" \033[1;40;31mCRC Error!!\033[0m\xd\xa");

				   CDataStream::ClearStream(pStream);
				   pStream->nState = STATE_IDENT;
				   break;
			   }

               // Toggle Check
               pHeader = (ANSI_HEADER *)pStream->pszBuffer;
               if (pSession != NULL)
               { 
                   /** GE I210+ Meter의 경우 Toggle을 지원하지 않는다 */
                   if (pSession->bDisableToggleCheck == FALSE &&
                           (pSession->nRecvToggle & ANSI_CTRL_TOGGLE) != (ansiToggle(pHeader->ctrl) & ANSI_CTRL_TOGGLE)) 
                   { 
                       XDEBUG("Receive %0d Byte(s)\xd\xa", pStream->nLength);
                       XDUMP((char *)pStream->pszBuffer, pStream->nLength);
                       XDEBUG(" \033[1;40;31mDuplicate Frame\033[0m\xd\xa");
                       CDataStream::ClearStream(pStream);
                       pStream->nState = STATE_IDENT;
				       break;
                   }
               }
               pSession->nRecvToggle = pHeader->ctrl;

			   OnFrame(pSession, pStream, (BYTE *)pStream->pszBuffer, pStream->nLength, nLength - pStream->nLength, pCallData);
			   CDataStream::ClearStream(pStream);
			   pStream->nState = STATE_IDENT;
               bResult = TRUE;
		}
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CAnsiDataStream Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CAnsiDataStream::OnFrame(ANSISESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength, int nRemain, void *pCallData)
{
	//XDEBUG(" ANSI OnFrame : Receive %0d Byte(s)\xd\xa", pStream->nLength);
    //XDUMP((char *)pStream->pszBuffer, pStream->nLength);
	return TRUE;
}

