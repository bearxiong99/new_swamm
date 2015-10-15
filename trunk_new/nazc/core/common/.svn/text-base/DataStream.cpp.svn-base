//////////////////////////////////////////////////////////////////////
//
//	DataStream.cpp: implementation of the CDataStream class.
//
//	This file is a part of the AIMIR-GCP.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "DataStream.h"
#include "MemoryDebug.h"

//////////////////////////////////////////////////////////////////////
// CDataStream Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataStream::CDataStream()
{
}

CDataStream::~CDataStream()
{
}

//////////////////////////////////////////////////////////////////////
// CDataStream Operations
//////////////////////////////////////////////////////////////////////

DATASTREAM *CDataStream::NewStream(UINT_PTR dwParam, int nGrow, int nMode)
{
	DATASTREAM	*pStream;

	pStream = (DATASTREAM *)MALLOC(sizeof(DATASTREAM));
	if (pStream == NULL)
		return NULL;

	memset(pStream, 0, sizeof(DATASTREAM));
	pStream->nMode		= nMode;
	pStream->nGrow		= nGrow;
	pStream->dwParam	= dwParam;
	pStream->nLastSeq	= -1;
	return pStream;
}

void CDataStream::DeleteStream(DATASTREAM *pStream)
{
	if (pStream == NULL)
		return;

	if (pStream->pszBuffer)
    {
		FREE(pStream->pszBuffer);
        pStream->pszBuffer = NULL;
    }

	FREE(pStream);
}

BOOL CDataStream::AddStream(DATASTREAM *pStream, const char* pszBuffer, int nLength)
{
	if ((pStream->nLength+nLength) >= pStream->nAllocation)
	{
		if (!GrowStream(pStream, nLength))
			return FALSE;
	}

	memcpy(&pStream->pszBuffer[pStream->nLength], pszBuffer, nLength);
	pStream->nLength += nLength;
	return TRUE;
}

BOOL CDataStream::AddStream(DATASTREAM *pStream, unsigned char c)
{
	if ((pStream->nLength+1) >= pStream->nAllocation)
	{
		if (!GrowStream(pStream, pStream->nGrow))
			return FALSE;
	}

	pStream->pszBuffer[pStream->nLength] = c;
	pStream->nLength++;
	return TRUE;
}

BOOL CDataStream::GrowStream(DATASTREAM *pStream, int nWanted)
{
	int		nGrow;

	nGrow = (pStream->nGrow > nWanted) ? pStream->nGrow : nWanted;
    if(pStream->pszBuffer)
    {
	    pStream->pszBuffer = (char *)REALLOC(pStream->pszBuffer, pStream->nAllocation+nGrow);
    }
    else
    {
	    pStream->pszBuffer = (char *)MALLOC(pStream->nAllocation+nGrow);
    }
	if (pStream->pszBuffer == NULL)
		return FALSE;

	pStream->nAllocation += nGrow;
	return TRUE;
}

void CDataStream::ClearStream(DATASTREAM *pStream)
{
	if (pStream == NULL)
		return;

    /**
	if (pStream->pszBuffer)
    {
		FREE(pStream->pszBuffer);
	    pStream->pszBuffer	= NULL;
    }

	pStream->nAllocation	= 0;
    */
	pStream->nLength		= 0;
}

void CDataStream::Detach(DATASTREAM *pStream)
{
	if (pStream == NULL)
		return;

	pStream->nAllocation	= 0;
	pStream->nLength		= 0;
	pStream->pszBuffer		= NULL;
}

//////////////////////////////////////////////////////////////////////
// CDataStream Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CDataStream::OnStreamDone(DATASTREAM *pStream)
{
	return TRUE;
}
