//////////////////////////////////////////////////////////////////////
//
//  Hash.cpp: implementation of the CHash class.
//
//  This file is a part of the AIMIR.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@paran.com
//  http://www.netcra.com
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

#include "Hash.h"

//////////////////////////////////////////////////////////////////////
// CHash Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHash::CHash(int nGrowSize)
{
    m_nGrow			= nGrowSize;
    m_nAllocation	= 0;
	m_nCount 		= 0;
	m_pList  		= NULL;
}

CHash::~CHash()
{
	if (m_pList != NULL)
		free(m_pList);

	m_pList = NULL;
	m_nAllocation = 0;
	m_nCount = 0;
}

//////////////////////////////////////////////////////////////////////
// CHash attribute
//////////////////////////////////////////////////////////////////////

int CHash::GetCount()
{
	return m_nCount;
}

//////////////////////////////////////////////////////////////////////
// CHash Functions
//////////////////////////////////////////////////////////////////////

BOOL CHash::Insert(const void *key, BYTE nKeyLen, int nIndex)
{
	HASHENTRY	*pItem = NULL;

	m_Locker.Lock();
	pItem = Search(key, nKeyLen);
	if (pItem == NULL)
	{
		if ((m_nCount+1) > m_nAllocation)
			Grow();

		// 값을 저장한다.
		pItem = &m_pList[m_nCount];
        pItem->keyLen = MAX_HASH_KEY_SIZE < nKeyLen? MAX_HASH_KEY_SIZE : nKeyLen;
		memcpy(pItem->key, key, pItem->keyLen);
		pItem->index = nIndex;
		m_nCount++;

		// 재 정렬한다.
		Sort();
	}
	else
	{
		// 데이터만 업데이트 한다.
		pItem->index = nIndex;
	}
	m_Locker.Unlock();
	return pItem ? TRUE : FALSE;
}

void CHash::Delete(const void *key, BYTE nKeyLen)
{
	HASHENTRY	*pItem = NULL;

	m_Locker.Lock();
	pItem = Search(key, nKeyLen);
	if (pItem != NULL)
	{
		// 키값을 FF로 채운후 맨뒤로 이동시킨다.
		memset(pItem->key, 0xFF, MAX_HASH_KEY_SIZE);
        pItem->keyLen = 0;

		// 재정렬 시킨다.		
		Sort();

		// 마지막 한개를 제거한다.
		m_nCount--;
		if (m_nCount < 0)
			m_nCount = 0;
	}
	m_Locker.Unlock();
}
    
void CHash::DeleteAll()
{
	m_Locker.Lock();
	m_nCount = 0;
	m_nAllocation = 0;
	if (m_pList) free(m_pList);
	m_pList = NULL;
	m_Locker.Unlock();
}

BOOL CHash::Update(const void *key, BYTE nKeyLen, int nIndex)
{
	return Insert(key, nKeyLen, nIndex);
}

int CHash::Find(const void *key, BYTE nKeyLen)
{
	HASHENTRY	*pItem = NULL;

	m_Locker.Lock();
	pItem = Search(key, nKeyLen);
	m_Locker.Unlock();

	return pItem ? pItem->index : -1;
}

//////////////////////////////////////////////////////////////////////
// CHash Functions
//////////////////////////////////////////////////////////////////////

int CHash::SortCompare(const void *p1, const void *p2)
{
	const HASHENTRY	*item1 = (HASHENTRY const *)p1;
	const HASHENTRY	*item2 = (HASHENTRY const *)p2;
    int len = item1->keyLen > item2->keyLen ? item1->keyLen : item2->keyLen;

	return memcmp(item1->key, item2->key, len);	
}
    
int CHash::SearchCompare(const void *p1, const void *p2)      
{
	const HASHENTRY	*item1 = (HASHENTRY const *)p1;
	const HASHENTRY	*item2 = (HASHENTRY const *)p2;
    int len = item1->keyLen > item2->keyLen ? item1->keyLen : item2->keyLen;

	return memcmp(item1->key, item2->key, len);	
}

//////////////////////////////////////////////////////////////////////
// CHash Functions
//////////////////////////////////////////////////////////////////////

BOOL CHash::Grow()
{
	m_pList = (HASHENTRY *)realloc(m_pList, sizeof(HASHENTRY)*(m_nAllocation+m_nGrow));
	if (m_pList == NULL)
	{
		m_nCount = 0;
		m_nAllocation = 0;
		return FALSE;
	}

	m_nAllocation += m_nGrow;
	return TRUE;
}

void CHash::Sort()
{
	if (m_nCount > 0)
		qsort(m_pList, m_nCount, sizeof(HASHENTRY), SortCompare);
}

HASHENTRY *CHash::Search(const void *key, BYTE nKeyLen)
{
	HASHENTRY	*pItem;
    HASHENTRY   item;

	if (m_nCount <= 0)
		return NULL;

    memset(&item, 0xFF, sizeof(HASHENTRY));
    item.keyLen = MAX_HASH_KEY_SIZE < nKeyLen? MAX_HASH_KEY_SIZE : nKeyLen;
    memcpy(item.key, key, item.keyLen);

	pItem = (HASHENTRY *)bsearch(&item, m_pList, m_nCount, sizeof(HASHENTRY), SearchCompare);
	return pItem;
}

