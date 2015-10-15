//////////////////////////////////////////////////////////////////////
//
//  Ondemander.cpp: implementation of the COperation class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
//  http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codiapi.h"
#include "Operation.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// COperation Class
//////////////////////////////////////////////////////////////////////

COperation	*m_pOperation = NULL;

//////////////////////////////////////////////////////////////////////
// COperation Class
//////////////////////////////////////////////////////////////////////

COperation::COperation()
{
	m_pOperation = this;
}

COperation::~COperation()
{
}

//////////////////////////////////////////////////////////////////////
// COperation Functions
//////////////////////////////////////////////////////////////////////

int	COperation::GetWorkerCount()
{
	return m_List.GetCount();
}

void COperation::lockOperation()
{
	m_Locker.Lock();
}

void COperation::unlockOperation()
{
	m_Locker.Unlock();
}

ONDEMANDPARAM *COperation::newOperation(BYTE nOperation, EUI64 *id, BYTE nPage, char *pszBuffer, BYTE nLength)
{
	ONDEMANDPARAM	*pOndemand;

	pOndemand = (ONDEMANDPARAM *)malloc(sizeof(ONDEMANDPARAM));
	if (pOndemand == NULL)
		return NULL;

	memset(pOndemand, 0, sizeof(ONDEMANDPARAM));
	memcpy(&pOndemand->id, id, sizeof(EUI64));
	pOndemand->nOperation	= nOperation;
	pOndemand->nPage		= nPage;
	pOndemand->pszBuffer	= pszBuffer;
	pOndemand->nLength		= nLength;
	pOndemand->hEvent 		= CreateEvent(NULL, FALSE, FALSE, NULL);
	pOndemand->pos 			= m_List.AddTail(pOndemand);
	return pOndemand;
}

void COperation::clearOperation(ONDEMANDPARAM *pOndemand)
{
	if (pOndemand == NULL)
		return;

	pOndemand->nLength = 0;	
}

void COperation::deleteOperation(ONDEMANDPARAM *pOndemand)
{
	m_List.RemoveAt(pOndemand->pos);

	if (pOndemand->hEvent != NULL)
		CloseHandle(pOndemand->hEvent);
	
	free(pOndemand);
}

ONDEMANDPARAM *COperation::findOperation(EUI64 *id)
{
	ONDEMANDPARAM	*pNext;
	POSITION		pos=0;
	
	pNext = m_List.GetFirst(pos);
	while(pNext)
	{
		if (memcmp(&pNext->id, id, sizeof(EUI64)) == 0)
			return pNext;
		pNext = m_List.GetNext(pos);
	}
	return NULL;
}

BOOL COperation::sendCommand(ONDEMANDPARAM *pOndemand, EUI64 *id, const char *pszCommand, int nLength)
{
	pOndemand->stat.nSend += nLength;
	endiBypass(pOndemand->endi, (BYTE *)const_cast<char *>(pszCommand), nLength, 1000);
	return TRUE;
}

