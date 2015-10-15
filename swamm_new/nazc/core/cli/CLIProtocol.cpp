//////////////////////////////////////////////////////////////////////
//
//  CLIProtocol.cpp: implementation of the CCLIProtocol class.
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
#include "CLIProtocol.h"
#include "CLIFiber.h"
#include "CLIUtility.h"
#include "MemoryDebug.h"
#include "CommonUtil.h"

//////////////////////////////////////////////////////////////////////
// CCLIProtocol Class
//////////////////////////////////////////////////////////////////////

CCLIProtocol::CCLIProtocol()
{
}

CCLIProtocol::~CCLIProtocol()
{
}

//////////////////////////////////////////////////////////////////////
// CCLIProtocol Functions
//////////////////////////////////////////////////////////////////////

BOOL CCLIProtocol::AddProtocol(const char* pszName, int nType, const char* pszDevice, int nPort, int nMaxSession, int nTimeout, void *pFiber)
{
	CLIPROTOCOL	*pProtocol;

	if (!pszName || !*pszName)
		return FALSE;

	if (pFiber == NULL)
		return FALSE;

	m_Locker.Lock();
	pProtocol = FindEntry(pszName);
	m_Locker.Unlock();

	if (pProtocol != NULL)
		return FALSE;

	m_Locker.Lock();
	pProtocol = AddEntry(pszName, nType, pszDevice, nPort, nMaxSession, nTimeout, pFiber);
	m_Locker.Unlock();

	return pProtocol ? TRUE : FALSE;
}

void CCLIProtocol::RemoveProtocol(LPSTR pszName)
{
	CLIPROTOCOL	*pProtocol;

	if (!pszName || !*pszName)
		return;

	m_Locker.Lock();
	pProtocol = FindEntry(pszName);
	m_Locker.Unlock();

	if (pProtocol == NULL)
		return;

	m_Locker.Lock();
	DeleteEntry(pProtocol);
	m_Locker.Unlock();
}

void CCLIProtocol::RemoveAllProtocols()
{
	CLIPROTOCOL	*pProtocol, *pDelete;
	POSITION	pos;

	m_Locker.Lock();
	pProtocol = m_List.GetFirst(pos);
	while(pProtocol)
	{
		pDelete	  = pProtocol;
		pProtocol = m_List.GetNext(pos);

		DeleteEntry(pDelete);
	}
	m_List.RemoveAll();
	m_Locker.Unlock();
}

BOOL CCLIProtocol::StartProtocol(LPSTR pszName)
{
	CLIPROTOCOL	*pProtocol;

	if (!pszName || !*pszName)
		return FALSE;

	m_Locker.Lock();
	pProtocol = FindEntry(pszName);
	m_Locker.Unlock();

	if (pProtocol == NULL)
		return FALSE;

	if (!pProtocol->bStarted)
		return FALSE;

	m_Locker.Lock();
	StartEntry(pProtocol);
	m_Locker.Unlock();
	return TRUE;
}

BOOL CCLIProtocol::StartProtocols()
{
	CLIPROTOCOL	*pProtocol;
	POSITION	pos;

	m_Locker.Lock();
	pProtocol = m_List.GetFirst(pos);
	while(pProtocol)
	{
		if (!pProtocol->bStarted)
			StartEntry(pProtocol);

		pProtocol = m_List.GetNext(pos);
	}
	m_Locker.Unlock();

	return TRUE;
}

void CCLIProtocol::StopProtocol(LPSTR pszName)
{
	CLIPROTOCOL	*pProtocol;

	if (!pszName || !*pszName)
		return;

	pProtocol = FindEntry(pszName);
	if (pProtocol == NULL)
		return;

	if (!pProtocol->bStarted)
		return;

	StopEntry(pProtocol);
}

void CCLIProtocol::StopProtocols()
{
	CLIPROTOCOL	*pProtocol;
	POSITION	pos;

	m_Locker.Lock();
	pProtocol = m_List.GetFirst(pos);
	while(pProtocol)
	{
		if (pProtocol->bStarted)
			StopEntry(pProtocol);

		pProtocol = m_List.GetNext(pos);
	}
	m_Locker.Unlock();
}

BOOL CCLIProtocol::OutText(CLISESSION *pSession, char *pszBuffer, int nLength)
{
	CCLIFiber	*pDest;

	pDest = (CCLIFiber *)pSession->pProtocol;
	return pDest->Write(pSession, pszBuffer, nLength);
}

//////////////////////////////////////////////////////////////////////
// CCLIProtocol Member Functions
//////////////////////////////////////////////////////////////////////

CLIPROTOCOL *CCLIProtocol::AddEntry(const char* pszName, int nType, const char* pszDevice, int nPort, int nMaxSession, int nTimeout, void *pFiber)
{
	CLIPROTOCOL	*pProtocol;
	CCLIFiber	*pDest;

	pProtocol = (CLIPROTOCOL *)MALLOC(sizeof(CLIPROTOCOL));
	if (pProtocol == NULL)
		return NULL;

	memset(pProtocol, 0, sizeof(CLIPROTOCOL));
	pProtocol->nType		= nType;
	pProtocol->pszName		= MCU_STRDUP(pszName);
	pProtocol->pszDevice	= pszDevice && *pszDevice ? MCU_STRDUP(pszDevice) : NULL;
	pProtocol->nPort		= nPort;
	pProtocol->nMaxSession	= nMaxSession;
	pProtocol->nTimeout		= nTimeout;
	pProtocol->pFiber		= pFiber;
	pDest					= (CCLIFiber *)pFiber;

	// Set Protocol Instance
	pDest->SetInstance(pProtocol);

	// Add List
	pProtocol->nPosition	= (int)m_List.AddTail(pProtocol);
	return pProtocol;
}

void CCLIProtocol::DeleteEntry(CLIPROTOCOL *pProtocol)
{
	// Remove List
	m_List.RemoveAt((POSITION)pProtocol->nPosition);

	// Free Allcation Memory
	if (pProtocol->pszDevice) FREE(pProtocol->pszDevice);
	if (pProtocol->pszName) FREE(pProtocol->pszName);
	FREE(pProtocol);
}

BOOL CCLIProtocol::StartEntry(CLIPROTOCOL *pProtocol)
{
	CCLIFiber	*pFiber;

	pFiber = (CCLIFiber *)pProtocol->pFiber;
	pFiber->Start();
	pProtocol->bStarted = TRUE;
	return TRUE;
}

void CCLIProtocol::StopEntry(CLIPROTOCOL *pProtocol)
{
	CCLIFiber	*pFiber;

	pFiber = (CCLIFiber *)pProtocol->pFiber;
	pFiber->Stop();
	pProtocol->bStarted = FALSE;
}

CLIPROTOCOL *CCLIProtocol::FindEntry(const char* pszName)
{
	CLIPROTOCOL	*pProtocol;
	POSITION	pos=INITIAL_POSITION;

	pProtocol = m_List.GetFirst(pos);
	while(pProtocol)
	{
		if (strcmp(pProtocol->pszName, pszName) == 0)
			break;

		pProtocol = m_List.GetNext(pos);
	}

	return pProtocol;
}

