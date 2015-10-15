#include "common.h"
#include "IF4Stream.h"
#include "IF4CrcCheck.h"
#include "CommonUtil.h"
#include "Chunk.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

//////////////////////////////////////////////////////////////////////
// CIF4Stream Data Definitions
//////////////////////////////////////////////////////////////////////

#define STATE_IDENT					0
#define STATE_HEADER				1
#define STATE_DATA					2
#define STATE_ETX					3
#define STATE_CRC					4
#define STATE_DOWNLOAD_HEADER		10
#define STATE_DOWNLOAD_DATA			11
#define STATE_DOWNLOAD_DONE			12
#define STATE_DONE					100

//////////////////////////////////////////////////////////////////////
// CIF4Stream Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIF4Stream::CIF4Stream()
{
}

CIF4Stream::~CIF4Stream()
{
}

//////////////////////////////////////////////////////////////////////
// CIF4Stream Operations
//////////////////////////////////////////////////////////////////////

BOOL CIF4Stream::ParseStream(WORKSESSION *pSession, DATASTREAM *pStream, const BYTE *pszStream, int nLength)
{
	IF4_HEADER	*pHeader;
	DOWNLOAD_HEADER	*pDownload;
	DOWNLOAD_TAIL	*pTail;
	int			i, nSeek;
	WORD		crc;
	BOOL		bResult;
	BYTE    	c;
    BYTE *      pszSp;
    TIMETICK    timeTick;

	// 프레임이 시작되어서 종료되는데까지 지연시간이 3초 이상이면 무시 시킨다.
	if (pSession->bStreamTimeout)
	{
        GetTimeTick(&timeTick);
		if (GetTimeInterval(&timeTick, &pSession->lastTick) >= 3000)
		{
			CDataStream::ClearStream(pStream);
			pStream->nState = STATE_IDENT;
		}
	}

    pszSp = const_cast<BYTE *>(pszStream);

	// 스트림을 생성한다.
	for(i=0; i<nLength; i+=nSeek)
	{
		nSeek = 1;
		c = pszSp[i];
		switch(pStream->nState) {
		  case STATE_IDENT :
			   if (c == DOWNLOAD_IDENTIFICATION)
			   {
                   GetTimeTick(&pSession->lastTick);
			   	   CDataStream::AddStream(pStream, c);
			   	   pStream->nSize  = sizeof(DOWNLOAD_HEADER);
				   pStream->nState = STATE_DOWNLOAD_HEADER;
				   pSession->bStreamTimeout = TRUE;
				   break;
			   }
			   else if (c != IF4_SOH)
				   break;

               GetTimeTick(&pSession->lastTick);
			   CDataStream::AddStream(pStream, c);
			   pStream->nSize  = sizeof(IF4_HEADER);
			   pStream->nState = STATE_HEADER;
			   break;

		  case STATE_HEADER :
			   nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
			   CDataStream::AddStream(pStream, (char *)pszSp+i, nSeek);
			   if (pStream->nLength < pStream->nSize)
				   break;

               pHeader         = (IF4_HEADER *)pStream->pszBuffer;
               pStream->nSize  = sizeof(IF4_HEADER) + LittleToHostInt(pHeader->len) + sizeof(IF4_TAIL);
			   pStream->nState = STATE_DATA;
			   break;

		  case STATE_DATA :
               nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
               if (!CDataStream::AddStream(pStream, (char *)pszSp+i, nSeek))
                   return FALSE;

               if (pStream->nLength < pStream->nSize)
                   break;

			   pStream->nState = STATE_DONE;

		  case STATE_DONE :
			   if (!IF4VerifyCrc((BYTE *)pStream->pszBuffer, pStream->nLength-2, 0))
			   {
				   CDataStream::ClearStream(pStream);
				   pStream->nState = STATE_IDENT;
				   break;
			   }
			   bResult = OnFrame(pSession, pStream, (BYTE *)pStream->pszBuffer, pStream->nLength);
			   CDataStream::ClearStream(pStream);
			   pStream->nState = STATE_IDENT;
			   if (!bResult)
				   return FALSE;
			   break;

		  case STATE_DOWNLOAD_HEADER :
			   nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
			   CDataStream::AddStream(pStream, (char *)pszSp+i, nSeek);
			   if (pStream->nLength < pStream->nSize)
				   break;

               pDownload = (DOWNLOAD_HEADER *)pStream->pszBuffer;
			   if (LittleToHostShort(pDownload->len) > 4096)
			   {
				   CDataStream::ClearStream(pStream);
				   pStream->nState = STATE_IDENT;
				   pSession->bStreamTimeout = FALSE;
			   	   break;
			   }

               pStream->nSize  = sizeof(DOWNLOAD_HEADER) + LittleToHostShort(pDownload->len) + sizeof(DOWNLOAD_TAIL);
			   pStream->nState = STATE_DOWNLOAD_DATA;
			   break;

		  case STATE_DOWNLOAD_DATA :
               nSeek = MIN(pStream->nSize-pStream->nLength, nLength-i);
               if (!CDataStream::AddStream(pStream, (char *)pszSp+i, nSeek))
                   return FALSE;

               if (pStream->nLength < pStream->nSize)
                   break;

			   pStream->nState = STATE_DOWNLOAD_DONE;

		  case STATE_DOWNLOAD_DONE :
			   pTail = (DOWNLOAD_TAIL *)&pStream->pszBuffer[pStream->nLength-2];
			   crc = IF4GetCrc((BYTE *)pStream->pszBuffer, pStream->nLength-2, 0);
			   if (pTail->crc != crc)
			   {
				   XDEBUG("---------- DOWNLOAD: CRC Error (Source=0x%02X, Gen=0x%02X) ---------\r\n", pTail->crc, crc);
				   CDataStream::ClearStream(pStream);
				   pStream->nState = STATE_IDENT;
				   pSession->bStreamTimeout = FALSE;
				   return TRUE;
			   }

			   OnDownloadFrame(pSession, pStream, (BYTE *)pStream->pszBuffer, pStream->nLength);
			   CDataStream::ClearStream(pStream);
			   pStream->nState = STATE_IDENT;
			   pSession->bStreamTimeout = FALSE;
			   break;
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CIF4Stream Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CIF4Stream::OnFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
#ifdef DEBUG
	XDEBUG("REQUEST %0d Byte(s)\xd\xa", pStream->nLength);
    XDUMP(pStream->pszBuffer, pStream->nLength);
#endif
	return TRUE;
}

void CIF4Stream::OnDownloadAck(WORKSESSION *pSession, DATASTREAM *pStream, BYTE seq)
{
}

BOOL CIF4Stream::OnDownloadFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
	DOWNLOAD_FILE_FORMAT	*pDownload;
	DOWNLOAD_PACKET			*pPacket;
	DOWNLOAD_HEADER			*pHeader;
	CChunk					*pChunk;
	BYTE					seq;

	pHeader = (DOWNLOAD_HEADER *)pszBuffer;
	XDEBUG("------- DOWNLOAD: SEQ=%d, LEN=%d -------\xd\xa", pHeader->seq, LittleToHostShort(pHeader->len));

	// 프레임의 시퀀스를 검사한다.
	seq = pSession->nDownloadSeq;
	if (pHeader->seq != seq)
	{
		XDEBUG("MULTI-FRAME-DOWNLOAD : Sequence error!! (SOURCE=%d, SEQ=%d)\xd\xa", pHeader->seq, seq);
		return FALSE;
	}

	OnDownloadAck(pSession, pStream, pHeader->seq);	
	pSession->nDownloadSeq++; 

	// 프레임의 시작과 끝을 찾아 낸다.
	if ((pHeader->seq == 0) && (pHeader->attr & DOWNLOAD_ATTR_HEADER))
	{
		pPacket = (DOWNLOAD_PACKET *)pszBuffer;
		pDownload = (DOWNLOAD_FILE_FORMAT *)&pPacket->data;
		XDEBUG("-------------- MULTI-FRAME-DOWNLOAD START --------------\xd\xa");
		XDEBUG(" NAME   = %s\xd\xa", pDownload->name);
		XDEBUG(" LENGTH = %d byte(s)\xd\xa", pDownload->nLength);
		XDEBUG("-------------- MULTI-FRAME-DOWNLOAD START --------------\xd\xa");

		pDownload = (DOWNLOAD_FILE_FORMAT *)MALLOC(sizeof(DOWNLOAD_FILE_FORMAT));
		if (pDownload == NULL)
			return TRUE;

		memcpy(pDownload, pPacket->data, sizeof(DOWNLOAD_FILE_FORMAT));
		pSession->pDownload = (void *)pDownload;
		pSession->pChunk	= (void *)new CChunk(10240);
	}
	else if (pHeader->attr & DOWNLOAD_ATTR_END)
	{
		XDEBUG("-------------- MULTI-FRAME-DOWNLOAD END --------------\xd\xa");
		if (pSession->pDownload && pSession->pChunk)
		{
			pPacket   = (DOWNLOAD_PACKET *)pStream->pszBuffer;
			pDownload = (DOWNLOAD_FILE_FORMAT *)pSession->pDownload;
			pChunk    = (CChunk *)pSession->pChunk;

			pChunk->Add((char *)pPacket->data, (int)LittleToHostShort(pPacket->hdr.len));
			OnSaveFile(pSession, pDownload, pChunk);
		}

		if (pSession->pDownload != NULL)
			FREE(pSession->pDownload);

		if (pSession->pChunk != NULL)
			delete ((CChunk *)pSession->pChunk);

		pSession->pDownload = NULL;
		pSession->pChunk = NULL;
	}
	else
	{
		// 프레임 버퍼에 다운로드 한다.
		if (pSession->pDownload && pSession->pChunk)
		{
			pPacket   = (DOWNLOAD_PACKET *)pStream->pszBuffer;
			pDownload = (DOWNLOAD_FILE_FORMAT *)pSession->pDownload;
			pChunk    = (CChunk *)pSession->pChunk;

			pChunk->Add((char *)pPacket->data, (int)LittleToHostShort(pPacket->hdr.len));
		}
	}
	return TRUE;
}

BOOL CIF4Stream::OnSaveFile(WORKSESSION *pSession, DOWNLOAD_FILE_FORMAT *pDownload, CChunk *pChunk)
{
#ifndef _WIN32
	char	szPath[_MAX_PATH+1];
	FILE	*fp;

	// 파일의 크기를 비교한다.
	if (LittleToHostInt(pDownload->nLength) != (unsigned long)pChunk->GetSize())
	{
		XDEBUG("MULTI-FRAME-DOWNLOAD : Length error!! (source=%d, current=%d)\xd\xa",
				LittleToHostInt(pDownload->nLength), pChunk->GetSize());
		return FALSE;
	}

	// MD5 Key 값을 비교한다.

	sprintf(szPath, "/app/sw/%s", pDownload->name);
	fp = fopen(szPath, "wb");
	if (fp != NULL)
	{
		fwrite(pChunk->GetBuffer(), pChunk->GetSize(), 1, fp);
		fclose(fp);
	}
#endif
	return TRUE;
}

