//////////////////////////////////////////////////////////////////////
//
//	SinkDataStream.cpp: implementation of the CCoordinatorDataStream class.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "codiapi.h"
#include "CoordinatorDataStream.h"
#include "CoordinatorCrcCheck.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CCoordinatorDataStream Data Definitions
//////////////////////////////////////////////////////////////////////

#define ASCII(c)				(c >= ' ') && (c <= 'z') ? c : ' '

#define CODI_HEADER_LENGTH		(int)sizeof(CODI_HEADER)
#define CODI_TAIL_LENGTH		(int)sizeof(CODI_TAIL)

#define STATE_STX				0
#define STATE_STX_NEXT			1
#define STATE_HEADER			2
#define STATE_DATA				3
#define STATE_ERROR				100
#define STATE_DONE				200

//////////////////////////////////////////////////////////////////////
// CCoordinatorDataStream Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordinatorDataStream::CCoordinatorDataStream()
{
}

CCoordinatorDataStream::~CCoordinatorDataStream()
{
}

//////////////////////////////////////////////////////////////////////
// CCoordinatorDataStream Functions
//////////////////////////////////////////////////////////////////////

BOOL CCoordinatorDataStream::AddStream(DATASTREAM *pStream, const char * pszBuffer, int nLength)
{
	CODI_HEADER	*pHeader;
	BYTE	c;
	int		i, nSeek=0;

	for(i=0; i<nLength; i+=nSeek)
	{
		c = pszBuffer[i];
	  	nSeek = 1;

		switch(pStream->nState) {
		  case STATE_STX :
			   if (c != CODI_START_FLAG_FIRST)
			   {
				   XDEBUG("\033[1;40;31mCOORDINATOR: ***** Frame error = 0x%02X(%c) *****\033[0m\xd\xa",
							c, ASCII(c));
				   break;
			   }

			   CDataStream::ClearStream(pStream);
			   CDataStream::AddStream(pStream, c);
			   pStream->nState = STATE_STX_NEXT;
			   break;

		  case STATE_STX_NEXT :
			   if (c != CODI_START_FLAG_SECOND)
			   {
				   XDEBUG("\033[1;40;31mCOORDINATOR: ***** Frame error1 = 0x%02X(%c) *****\033[0m\xd\xa",
							c, ASCII(c));
				   pStream->nState = STATE_STX;
				   nSeek = 0;
				   break;
			   }

			   CDataStream::AddStream(pStream, c);
			   pStream->nState = STATE_HEADER;
			   break;

		  case STATE_HEADER :
			   nSeek = MIN(CODI_HEADER_LENGTH-pStream->nLength, nLength-i);
			   if (!CDataStream::AddStream(pStream, pszBuffer+i, nSeek))
				   return TRUE;
			  
			   if (pStream->nLength < (int)CODI_HEADER_LENGTH)
				   break;

			   pHeader = (CODI_HEADER *)pStream->pszBuffer;
			   pStream->nSize	= pHeader->len + CODI_HEADER_LENGTH + CODI_TAIL_LENGTH;
#if defined (__NETWORK_NODE__)
               /** Coordinator일 때는 Size 검사를 하지 않고 Network Node 일 때만 한다 */
			   if (pHeader->len > CODI_MAX_PAYLOAD_SIZE )
			   {
				   XDEBUG("\033[1;40;31mCOORDINATOR: Invalid payload size = %d\033[0m\r\n", pHeader->len);
			   	   CDataStream::ClearStream(pStream);
			   	   pStream->nState = STATE_STX;
				   break;
			   }
#endif
			   pStream->nState  = STATE_DATA;
			   break;

		  case STATE_DATA :
			   nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
			   if (!CDataStream::AddStream(pStream, pszBuffer+i, nSeek))
				   return TRUE;
			  
			   if (pStream->nLength < pStream->nSize)
				   break;

		  case STATE_DONE :
			   if (VerifyCrcCheck((BYTE *)pStream->pszBuffer, pStream->nLength))
			   { 
                   CODI_GENERAL_FRAME * pFrame = (CODI_GENERAL_FRAME*)pStream->pszBuffer;
                   int pFrameLen = pStream->nLength;

				   // 스트림이 오류가 없는것은 버퍼를 그대로 넘기므로 반드시 Detach한다.
			   	   CDataStream::Detach(pStream);

			   	   OnReceiveCoordinator((COORDINATOR *)pStream->dwParam, pFrame, pFrameLen);

			   }
			   else
			   {
				   XDEBUG("\033[1;40;31mCOORDINATOR: ******** CRC Error *******\033[0m\r\n");
				   XDUMP((char *)pStream->pszBuffer, pStream->nLength, TRUE);
			   	   CDataStream::ClearStream(pStream);
			   }

			   pStream->nState = STATE_STX;
			   break;
		}
	}

	return TRUE;
}

BOOL CCoordinatorDataStream::AddStream(DATASTREAM *pStream, unsigned char c)
{
    char buff[1];
    
    buff[0] = c;

    return AddStream(pStream, buff, 1);
}

//////////////////////////////////////////////////////////////////////
// CCoordinatorDataStream Override Functions
//////////////////////////////////////////////////////////////////////

void CCoordinatorDataStream::OnReceiveCoordinator(COORDINATOR *codi, CODI_GENERAL_FRAME *pFrame, int nLength)
{
}

