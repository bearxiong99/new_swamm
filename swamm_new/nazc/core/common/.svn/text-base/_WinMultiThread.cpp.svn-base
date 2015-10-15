//////////////////////////////////////////////////////////////////////
//
//	MultiThread.cpp: implementation of the CMultiThread class.
//
//	This file is a part of the DAS-NMS project.
//	(c)Copyright 2003 NETCRA Software International All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	http://www.netcra.com
//	casir@com.ne.kr
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_WinMultiThread.h"

//////////////////////////////////////////////////////////////////////
// CMultiThread Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiThread::CMultiThread()
{
	m_dwTimeout		= DEFAULT_TERMINATE_TIMEOUT;
	m_bExitThread	= FALSE;
	m_bStarted		= FALSE;
	m_bAbnormal		= FALSE;
	m_bEndSession	= TRUE;

	m_hStopEvent	= NULL;
	m_hExitEvent	= NULL;
	m_hThread		= NULL;
	m_dwThreadID	= 0;
	m_dwTimeout		= 0;
	m_nPriority		= THREAD_PRIORITY_BELOW_NORMAL;
}

CMultiThread::~CMultiThread()
{
}

//////////////////////////////////////////////////////////////////////
// CSyncThread Attribute
//////////////////////////////////////////////////////////////////////

BOOL CMultiThread::IsStarted() const
{
	return m_bStarted;
}

BOOL CMultiThread::IsExitPending() const
{
	return m_bExitThread;
}

BOOL CMultiThread::IsThreadExitPending() const
{
	return m_bExitThread;
}

BOOL CMultiThread::IsAbnormalTerminate() const
{
	return m_bAbnormal;
}

BOOL CMultiThread::IsEndSession() const
{
	return m_bEndSession;
}

void CMultiThread::SetTimeout(DWORD dwTimeout)
{
	m_dwTimeout = dwTimeout;
}

DWORD CMultiThread::GetTimeout()
{
	return m_dwTimeout;
}

void CMultiThread::SetPriority(int nPriority)
{
	m_nPriority	= nPriority;
	if (IsStarted())
	{
		Pause();
		SetThreadPriority(m_hThread, nPriority);
		Resume();
	}
	else
	{
		SetThreadPriority(m_hThread, nPriority);
	}
}

int CMultiThread::GetPriority()
{
	return m_nPriority;
}

HANDLE CMultiThread::GetStopEventHandle() const
{
	return m_hStopEvent;
}

//////////////////////////////////////////////////////////////////////
// CSyncThread Oprations
//////////////////////////////////////////////////////////////////////

BOOL CMultiThread::Startup(DWORD dwTimeout, int nPriority)
{
	return TRUE;
}

void CMultiThread::Shutdown(DWORD dwTimeout)
{
}

void CMultiThread::Drop()
{
	if (!IsStarted())
		return;

	m_bExitThread = TRUE;
	SetEvent(m_hStopEvent);
}

void CMultiThread::Pause()
{
	if (!IsStarted())
		return;

	SuspendThread(m_hThread);
}

void CMultiThread::Resume()
{
	if (!IsStarted())
		return;

	ResumeThread(m_hThread);
}

void CMultiThread::WaitForThreadExit()
{
	if (!IsStarted())
		return;

	for(int i=0; i<30; i++)
	{
		if (!IsStarted())
			return;

		Sleep(100);
	}
}

//////////////////////////////////////////////////////////////////////
// CMultiThread Overrides
//////////////////////////////////////////////////////////////////////

void CMultiThread::OnStartup(int dwTimeout, int nPriority)
{
}

void CMultiThread::OnShutdown()
{
}


