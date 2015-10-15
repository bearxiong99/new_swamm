//////////////////////////////////////////////////////////////////////
//
//	MemoryDebug.cpp: implementation of Memory Debugging Functions.
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
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "LinkedList.h"
#include "Locker.h"

typedef struct	_tagMEMORYBLOCK
{
		char	*file;
		int		line;
		void	*p;
		int		len;
		unsigned int	nIndex;
		int		nPosition;
}	MEMORYBLOCK;

CLocker			m_MemoryLocker;
unsigned int			m_debugMemoryCount = 0;
CLinkedList<MEMORYBLOCK *>	m_debugMemoryList;

MEMORYBLOCK *debugFindMemory(void *p);

void debugAddMemory(const char *file, int linenum, void *p, size_t size)
{
	MEMORYBLOCK	*pBlock;

    //XDEBUG(" MEMORY ADD %s %d %p\r\n", file, linenum, p); 

	pBlock = (MEMORYBLOCK *)malloc(sizeof(MEMORYBLOCK));
	if (pBlock == NULL)
		return;

	memset(pBlock, 0, sizeof(MEMORYBLOCK));
	pBlock->file	= (char *)malloc(strlen(file)+1);
	strcpy(pBlock->file, file);
	pBlock->line	= linenum;
	pBlock->p		= p;
	pBlock->len		= (int)size;

	m_MemoryLocker.Lock();
	pBlock->nIndex = m_debugMemoryCount;
	pBlock->nPosition = (int)m_debugMemoryList.AddTail(pBlock);
	m_debugMemoryCount++;
	m_MemoryLocker.Unlock();
}

BOOL debugDeleteMemory(const char *file, int linenum, void *p)
{
	MEMORYBLOCK	*pBlock;

    //XDEBUG(" MEMORY DELETE %s %d %p\r\n", file, linenum, p); 
	pBlock = debugFindMemory(p);
	if (pBlock == NULL)
	{
		XDEBUG("----- ERROR: %s(%d): Not allocation memory (Pointer=%p) -----\r\n", file, linenum, p);
		return FALSE;
	}

	m_MemoryLocker.Lock();
	m_debugMemoryList.RemoveAt((POSITION)pBlock->nPosition);
	m_MemoryLocker.Unlock();

	free(pBlock->file);
	free(pBlock);

    return TRUE;
}

MEMORYBLOCK *debugFindMemory(void *p)
{
	MEMORYBLOCK	*pBlock;
	POSITION	pos=INITIAL_POSITION;

	m_MemoryLocker.Lock();
	pBlock = m_debugMemoryList.GetFirst(pos);
	while(pBlock)
	{
		if (pBlock->p == p)
			break;

		pBlock = m_debugMemoryList.GetNext(pos);
	}
	m_MemoryLocker.Unlock();

	return pBlock;
}

void debugCheckPoint(unsigned int *nIndex)
{
#if 1
    /*
    RF-13
    Remote Debugging 시 DebugDifference에서 agent가 Bloking 되는 현상이 발생하고 있다. 
    정확한 원인을 알 때 까지 DebugDifference를 막는다.
    */

#ifdef __DEBUG_MEMORY__
	m_MemoryLocker.Lock();
	*nIndex = m_debugMemoryCount;
	m_MemoryLocker.Unlock();
#endif
#endif
}

void debugDifference(unsigned int nCheckPoint)
{
#if 1
    /*
    RF-13
    Remote Debugging 시 DebugDifference에서 agent가 Bloking 되는 현상이 발생하고 있다. 
    정확한 원인을 알 때 까지 DebugDifference를 막는다.
    */

#ifdef __DEBUG_MEMORY__
	MEMORYBLOCK	*pBlock;
	POSITION	pos;
	int			n = 0;
	int			nSize = 0;

	XDEBUG("-------------- Memory Difference -------------\r\n");

	m_MemoryLocker.Lock();
	pBlock = m_debugMemoryList.GetFirst(pos);
	while(pBlock)
	{
		if (pBlock->nIndex >= nCheckPoint)
		{
			XDEBUG("%4d: Block=%p, File=%s(%d), Size=%d ", 
					n+1, pBlock->p, pBlock->file, pBlock->line, pBlock->len);
			XDUMP((char *)pBlock->p, MIN(pBlock->len, 16), FALSE);

			nSize += pBlock->len;
			n++;
		}
		pBlock = m_debugMemoryList.GetNext(pos);
	}
	m_MemoryLocker.Unlock();

	XDEBUG("----------------------------------------------\r\n");
	XDEBUG(" Total count=%d, Total memory size=%d byte(s)\r\n", n, nSize);
	XDEBUG("----------------------------------------------\r\n");
#endif
#endif
}

void *debugMALLOC(const char *file, int linenum, size_t size)
{
	void	*p;

	p = malloc(size);
	if (p != NULL)
		debugAddMemory(file, linenum, p, size);
	return p;
}

void *debugREALLOC(const char *file, int linenum, void *memblock, size_t size)
{
	void	*p;

	if (memblock != NULL)
		debugDeleteMemory(file, linenum, memblock);

	p = realloc(memblock, size);
	if (p != NULL)
		debugAddMemory(file, linenum, p, size);

	return p;
}

void debugFREE(const char *file, int linenum, void *memblock)
{
	if (memblock == NULL)
	{
		XDEBUG("----- ERROR: %s(%d): Null point free -----\r\n", file, linenum);
		return;
	}

	debugDeleteMemory(file, linenum, memblock);
	free(memblock);
    memblock = NULL;
}

