#include "common.h"
#include "DataStream.h"
#include "DlmsCrcCheck.h"
#include "CommonUtil.h"
#include "DlmsUtil.h"
#include "DebugUtil.h"

#include "dlmsDataStream.h"
#include "dlmsUtils.h"

//////////////////////////////////////////////////////////////////////
// CDlmsDataStream Data Definitions
//////////////////////////////////////////////////////////////////////

#define STATE_IDENT					0
#define STATE_CHECK_START			1
#define STATE_HEADER				2
#define STATE_DATA					3
#define STATE_CRC					100	
#define STATE_DONE					200


//////////////////////////////////////////////////////////////////////
// CDlmsDataStream Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlmsDataStream::CDlmsDataStream()
{
}

CDlmsDataStream::~CDlmsDataStream()
{
}

//////////////////////////////////////////////////////////////////////
// CDlmsDataStream Operations
//////////////////////////////////////////////////////////////////////

BOOL CDlmsDataStream::ParseStream(DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData)
{
	HDLC_HEADER	*pHeader;
	int			i, len, nSeek;
	BOOL		bResult = FALSE;
	BYTE    	c;

	for(i=0; i<nLength; i+=nSeek)
	{
		nSeek = 1;
		c = (pszStream[i] & 0xff);
		switch(pStream->nState) {
		  case STATE_IDENT :
			   CDataStream::ClearStream(pStream);
			   if (c != HDLC_FLAG)
			   {
#ifdef __CHECK_START_FLAG__
				   XDEBUG(" \033[1;40;31mInvalid Start Flag = 0x%02X\033[0m\r\n", c);
				   break;
#else
				   // 시작이 틀린 경우 수신된 전체 프레임을 무시한다.
				   XDEBUG("Receive %0d Byte(s)\xd\xa", nLength);
				   XDUMP((char *)pszStream, nLength);
				   XDEBUG(" \033[1;40;31mInvalid Start Flag = 0x%02X\033[0m\r\n", c);
				   return FALSE;
#endif
			   }
               CDataStream::AddStream(pStream, HDLC_FLAG);

			   // HEADER + HCS or CRC
			   pStream->nSize  = sizeof(HDLC_HEADER) + sizeof(WORD);
			   pStream->nState = STATE_CHECK_START;
			   break;

		  case STATE_CHECK_START :
			   pStream->nState = STATE_HEADER;
			   if (c == HDLC_FLAG)
				   break;

		  case STATE_HEADER :
			   nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
			   CDataStream::AddStream(pStream, (const char *)pszStream+i, nSeek);
			   if (pStream->nLength < pStream->nSize)
				   break;

               pHeader = (HDLC_HEADER *)pStream->pszBuffer;
			   len     = dlmsOrderByShort(pHeader->typeLen) & 0x07FF; 
			   if (len > 8)
			   {
				   // HCS Check
			   	   if (!DlmsVerifyHCS((BYTE *)pStream->pszBuffer))
				   {
			   		   CDataStream::ClearStream(pStream);
			   		   pStream->nState = STATE_IDENT;
					   break;
				   }
			   }
			   else if (len < 8)
			   {
				   // 잘못된 프레임 길이
				   XDEBUG(" \033[1;40;31mInvalid length = %d\033[0m\xd\xa", len);
			   	   CDataStream::ClearStream(pStream);
			   	   pStream->nState = STATE_IDENT;
				   break;
			   }

               pStream->nSize  = len + 2;
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
			   if ((pStream->pszBuffer[pStream->nLength-1] != HDLC_FLAG) ||
			   	   (!DlmsVerifyCrc((BYTE *)pStream->pszBuffer, pStream->nSize)))
			   {
				   XDEBUG("Receive %0d Byte(s)\xd\xa", pStream->nLength);
				   XDUMP((char *)pStream->pszBuffer, pStream->nLength);
				   XDEBUG(" \033[1;40;31mCRC Error!!\033[0m\xd\xa");

				   CDataStream::ClearStream(pStream);
				   pStream->nState = STATE_IDENT;
				   break;
			   }

               // Thread Debugging
               __PTHREAD_INFO__;

			   OnFrame(pStream, (BYTE *)pStream->pszBuffer, pStream->nLength, nLength - pStream->nLength, pCallData);
               //XDEBUG(ANSI_COLOR_BLUE " == FREE pStream %p %p\r\n" ANSI_NORMAL, pStream, pStream->pszBuffer);
			   CDataStream::ClearStream(pStream);
			   pStream->nState = STATE_IDENT;
               bResult = TRUE;
		}
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CDlmsDataStream Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CDlmsDataStream::OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, int nRemain, void *pCallData)
{
	//XDEBUG(" DLMS OnFrame : Receive %0d Byte(s)\xd\xa", pStream->nLength);
    //XDUMP((char *)pStream->pszBuffer, pStream->nLength);
	return TRUE;
}

