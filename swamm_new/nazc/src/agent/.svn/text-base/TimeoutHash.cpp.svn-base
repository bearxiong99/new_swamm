#include "common.h"
#include "AgentService.h"
#include "TimeoutHash.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "Utility.h"

CTimeoutHash   *m_pTimeoutHash = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeoutHash::CTimeoutHash()
{
	m_pTimeoutHash = this;
	m_nHashTimeout = 40; 
	m_nMaximum	   = 200;

	m_pHashList	= (HASHITEM **)MALLOC(sizeof(HASHITEM *)*m_nMaximum);
	if (m_pHashList != NULL)
		memset(m_pHashList, 0, sizeof(HASHITEM *)*m_nMaximum);
}

CTimeoutHash::~CTimeoutHash()
{
	RemoveAll();

	if (m_pHashList != NULL)
	{
		FREE(m_pHashList);
		m_pHashList = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CTimeoutHash::Initialize()
{
	if (!CTimeoutThread::StartupThread(2))
        return FALSE;
	return TRUE;
}

void CTimeoutHash::Destroy()
{
	RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CTimeoutHash::Add(const char *pszKey)
{
	HASHITEM	*pItem;
	int			nIndex;

	if (!pszKey || !*pszKey)
		return FALSE;

	m_Locker.Lock();
	nIndex = FindItem(pszKey);
	if (nIndex == -1)
	{
		pItem = (HASHITEM *)MALLOC(sizeof(HASHITEM));
		if (pItem != NULL)
		{
			memset(pItem, 0, sizeof(HASHITEM));
			pItem->pszKey = strdup(pszKey);
			pItem->tmHash = uptime();
			StoreIt(pItem);
		}
	}
	m_Locker.Unlock();
	return (nIndex == -1) ? TRUE : FALSE;
}

void CTimeoutHash::RemoveAll()
{
	int		i;

	m_Locker.Lock();
	for(i=0; i<m_nMaximum; i++)
	{
		if (m_pHashList[i] != NULL)
		{
			if (m_pHashList[i]->pszKey != NULL)
				FREE(m_pHashList[i]->pszKey);
			FREE(m_pHashList[i]);
			m_pHashList[i] = NULL;
		}
	}
	m_Locker.Unlock();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

void CTimeoutHash::StoreIt(HASHITEM *pItem)
{
	int		i;

	for(i=0; i<m_nMaximum; i++)
	{
		if (m_pHashList[i] != NULL)
			continue;

		m_pHashList[i] = pItem;
		break;
	}
}

int CTimeoutHash::FindItem(const char *pszKey)
{
	int		i;

	for(i=0; i<m_nMaximum; i++)
	{
		if (m_pHashList[i] == NULL)
			continue;

		if (strcmp(m_pHashList[i]->pszKey, pszKey) == 0)
			return i;	
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CTimeoutHash::OnActiveThread()
{
	int		i;
	time_t	now;

	m_Locker.Lock();
//	time(&now);
	now = uptime();
	for(i=0; i<m_nMaximum; i++)
	{
		if (m_pHashList[i] == NULL)
			continue;

		if ((now-m_pHashList[i]->tmHash) >= m_nHashTimeout)
		{
			XDEBUG("Deleting Hash (%s)\xd\xa", m_pHashList[i]->pszKey);
			if (m_pHashList[i]->pszKey)
				FREE(m_pHashList[i]->pszKey);
			FREE(m_pHashList[i]);
			m_pHashList[i] = NULL;
		}
	}
	m_Locker.Unlock();
	return TRUE;
}

BOOL CTimeoutHash::OnTimeoutThread()
{
	return OnActiveThread();
}

