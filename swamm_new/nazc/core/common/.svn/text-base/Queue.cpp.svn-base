//////////////////////////////////////////////////////////////////////
//
//	Queue.cpp: implementation of the CQueue class.
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

#include "Queue.h"
#include "MemoryDebug.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueue::CQueue(int nMaximum)
{
	m_pBuffer  = (char *)MALLOC(nMaximum);
	m_nMaximum = nMaximum;
	m_nCount   = 0;
	m_nHead	   = 0;
	m_nTail	   = 0;
}

CQueue::~CQueue()
{
	if (m_pBuffer)
		FREE(m_pBuffer);
	m_pBuffer = NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CQueue::IsEmpty()
{
	return (m_nHead == m_nTail) ? TRUE : FALSE;
}

int CQueue::GetQLength()
{
	return m_nCount;
}

char *CQueue::GetBuffer() const
{
	return m_pBuffer;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CQueue::AddQ(const char *pszBuffer, int nLength)
{
	int		i;
	
	if (!pszBuffer || !nLength)
		return -1;

	m_Locker.Lock();
	for(i=0; i<nLength; i++)
	{
		if (AddQueueChar(pszBuffer[i]) == -1)
			break;
	}
	m_Locker.Unlock();
	return i;
}

int CQueue::GetQ(char *pszBuffer, int nLength)
{
	int		c, i;

	if (!pszBuffer || !nLength)
		return -1;

	m_Locker.Lock();
	for(i=0; i<nLength; i++)
	{
		c = GetQueueChar();
		if (c == -1)
			break;
		pszBuffer[i] = c;
	}
	m_Locker.Unlock();
	return i;
}

int CQueue::GetQ()
{
	int		c;

	m_Locker.Lock();
	c = GetQueueChar();
	m_Locker.Unlock();
	return c;
}

void CQueue::FlushQ()
{
	m_Locker.Lock();
	m_nCount = 0;
	m_nHead = 0;
	m_nTail = 0;
	m_Locker.Unlock();
}

//////////////////////////////////////////////////////////////////////
// Member Function 
//////////////////////////////////////////////////////////////////////

int CQueue::AddQueueChar(BYTE c)
{
	m_pBuffer[m_nHead] = c;
	m_nHead++;
	if (m_nHead >= m_nMaximum)
		m_nHead = 0;
	m_nCount++;
	return 0;
}

int CQueue::GetQueueChar()
{
	BYTE	c;

	if (m_nHead == m_nTail)
		return -1;

	c = m_pBuffer[m_nTail];
	m_nTail++;
	if (m_nTail >= m_nMaximum)
		m_nTail = 0;
	m_nCount--;
	return c;	
}

