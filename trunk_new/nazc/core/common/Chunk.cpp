//////////////////////////////////////////////////////////////////////
//
//	Chunk.cpp: implementation of the CChunk class.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@paran.com
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "Chunk.h"
#include "MemoryDebug.h"

//////////////////////////////////////////////////////////////////////
// CChunk Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChunk::CChunk(int nGrow)
{
	memset(&m_Chunk, 0, sizeof(CHUNK));
	m_Chunk.nGrow		= nGrow;
	m_Chunk.dwHandle	= (void *)this;
}

CChunk::~CChunk()
{
	Flush();
}	

//////////////////////////////////////////////////////////////////////
// CChunk Operations
//////////////////////////////////////////////////////////////////////

CHUNK *CChunk::GetChunk()
{
	return &m_Chunk;
}

void CChunk::SetChunk(CHUNK *pChunk)
{
	if (!pChunk)
		return;

	m_Chunk.nAllocation = pChunk->nAllocation;
	m_Chunk.nCommitLength = pChunk->nCommitLength;
	m_Chunk.nLength = pChunk->nLength;
	m_Chunk.nGrow = pChunk->nGrow;
	m_Chunk.pszBuffer = pChunk->pszBuffer;
}

int	CChunk::GetSize() const
{
	return m_Chunk.nLength;
}

int CChunk::GetCommitSize() const
{
	return m_Chunk.nCommitLength;
}

int	CChunk::GetGrowSize() const
{
	return m_Chunk.nGrow;
}

int	CChunk::GetBufferSize() const
{
	return m_Chunk.nAllocation;
}

char *CChunk::GetBuffer() const
{
	return m_Chunk.pszBuffer;
}

void CChunk::SetGrowSize(int nGrow)
{
	m_Chunk.nGrow = nGrow;
}

//////////////////////////////////////////////////////////////////////
// CChunk Operations
//////////////////////////////////////////////////////////////////////

int	CChunk::Add(const char *pszBuffer, int nLength)
{
	int		nGrow;

	if ((m_Chunk.nLength+nLength) >= m_Chunk.nAllocation)
	{
		nGrow = MAX(m_Chunk.nGrow, nLength*2);
		Grow(nGrow);
	}

	memcpy(&m_Chunk.pszBuffer[m_Chunk.nLength], pszBuffer, nLength);
	m_Chunk.nLength += nLength;
	return m_Chunk.nLength;
}

int	CChunk::Add(char c)
{
	if ((m_Chunk.nLength+1) >= m_Chunk.nAllocation)
		Grow(1);

	m_Chunk.pszBuffer[m_Chunk.nLength] = c;
	m_Chunk.nLength++;
	return m_Chunk.nLength;
}

BOOL CChunk::Delete(int nLength)
{
	if (m_Chunk.nLength <= 0)
		return FALSE;

	nLength = (nLength > m_Chunk.nLength) ? m_Chunk.nLength : nLength;
	m_Chunk.nLength -= nLength;
	return TRUE;
}

void CChunk::Close()
{
	if ((m_Chunk.nLength+1) >= m_Chunk.nAllocation)
		Grow(1);

	m_Chunk.pszBuffer[m_Chunk.nLength] = '\0';
}

void CChunk::Grow(int nWanted)
{
	int		nGrow;

	nGrow = (nWanted >= m_Chunk.nGrow) ? nWanted+1 : m_Chunk.nGrow+1;
    if(m_Chunk.pszBuffer)
    {
	    m_Chunk.pszBuffer = (char *)REALLOC(m_Chunk.pszBuffer, m_Chunk.nAllocation+nGrow);
    }
    else
    {
	    m_Chunk.pszBuffer = (char *)MALLOC(m_Chunk.nAllocation+nGrow);
    }
	m_Chunk.nAllocation += nGrow;
}

void CChunk::Empty()
{
	m_Chunk.nCommitLength = 0;
	m_Chunk.nLength	= 0;
}

void CChunk::Flush()
{
	if (m_Chunk.pszBuffer)
		FREE(m_Chunk.pszBuffer);

	m_Chunk.pszBuffer	= NULL;
	m_Chunk.nAllocation	= 0;
	m_Chunk.nCommitLength = 0;
	m_Chunk.nLength		= 0;
}

void CChunk::Commit()
{
	m_Chunk.nCommitLength = m_Chunk.nLength;
}

void CChunk::Rollback()
{
	m_Chunk.nLength	= m_Chunk.nCommitLength;
}

void CChunk::Attach(char *pszBuffer, int nLength)
{
	Flush();
	
	m_Chunk.pszBuffer = pszBuffer;
	m_Chunk.nAllocation = nLength;
	m_Chunk.nLength = nLength;
}

char *CChunk::Detach(CChunk *pChunk)
{
	char	*pszBuffer;

	pszBuffer = m_Chunk.pszBuffer;
	if (pChunk != NULL)
	{
		pChunk->Flush();
		pChunk->SetChunk(&m_Chunk);
	}

	// 메모리 소유가 넘어 갔으므로 Free하지 않는다.
	m_Chunk.pszBuffer = NULL;
	m_Chunk.nAllocation	= 0;
	m_Chunk.nCommitLength = 0;
	m_Chunk.nLength	= 0;

	return pszBuffer;
}

