//////////////////////////////////////////////////////////////////////
//
//	SyncThread.cpp: implementation of the CTimeoutThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TimeoutThread.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeoutThread::CTimeoutThread()
{
	m_hActiveEvent	= NULL;
}

CTimeoutThread::~CTimeoutThread()
{
	ShutdownThread();
	WaitForThreadExit();
}

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Oprations
//////////////////////////////////////////////////////////////////////

BOOL CTimeoutThread::StartupThread(DWORD dwTimeout, int nPriority)
{
	if (IsStarted())
		return FALSE;

	// Setup timeout
	m_nPriority		= nPriority;
	m_dwTimeout		= dwTimeout * 1000;
	m_bExitThread	= FALSE;
	m_bAbnormal		= FALSE;
	m_bEndSession	= FALSE;

	// Create Event Handle
	m_hStopEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hExitEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hActiveEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);

	// Create Thread
	m_hThread = CreateThread(NULL, 
							 10240, 
							 (LPTHREAD_START_ROUTINE)WorkThread, 
							 (LPVOID)this, 
							 CREATE_SUSPENDED, 
							 &m_dwThreadID);
	SetThreadPriority(m_hThread, m_nPriority);

	OnStartup(dwTimeout * 1000, nPriority);
	ResumeThread(m_hThread);

	m_bStarted = TRUE;
	return TRUE;
}

void CTimeoutThread::ShutdownThread(DWORD dwTimeout)
{
	DWORD	dwExitCode;

	if (!IsStarted())
		return;

	m_bExitThread = TRUE;
	SetEvent(m_hStopEvent);

	OnShutdown();

	if (!m_bEndSession && !IsAbnormalTerminate() &&
		(WaitForSingleObject(m_hExitEvent, dwTimeout) == WAIT_TIMEOUT))
	{
		if (GetExitCodeThread(m_hThread, &dwExitCode))
		{
			if (dwExitCode == STILL_ACTIVE)
				TerminateThread(m_hThread, 0);
		}
	}

	CloseHandle(m_hStopEvent);
	CloseHandle(m_hExitEvent);
	CloseHandle(m_hActiveEvent);
	CloseHandle(m_hThread);

	m_bStarted = FALSE;
}

void CTimeoutThread::ActiveThread()
{
	SetEvent(m_hActiveEvent);
}

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Event Handler
//////////////////////////////////////////////////////////////////////

void CTimeoutThread::OnEnterThread(THREADEVENTLIST *pEventList)
{
    //XDEBUG("OnEnterThread\r\n");
}

void CTimeoutThread::OnLeaveThread()
{
    //XDEBUG("OnLeaveThread\r\n");
}

void CTimeoutThread::OnTerminate()
{
    //XDEBUG("OnTerminate\r\n");
}

void CTimeoutThread::OnAbnormalTerminate()
{
    //XDEBUG("OnAbnormalThread\r\n");
}

BOOL CTimeoutThread::OnActiveThread()
{
    //XDEBUG("OnActiveThread\r\n");
	return TRUE;
}

BOOL CTimeoutThread::OnTimeoutThread()
{
    //XDEBUG("OnTimeoutThread\r\n");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Thread
//////////////////////////////////////////////////////////////////////

DWORD CTimeoutThread::WorkThread(LPVOID lpParam)
{
	CTimeoutThread		*pThis;
	THREADEVENTLIST	WaitEvent;
	DWORD			nWaitStatus;
	BOOL			bForever;

	pThis	 = (CTimeoutThread *)lpParam;

	memset(&WaitEvent, 0, sizeof(THREADEVENTLIST));
	WaitEvent.nEvents			= 2;
	WaitEvent.arEventHandle[0]	= pThis->m_hStopEvent;
	WaitEvent.arEventHandle[1]	= pThis->m_hActiveEvent;

	pThis->OnEnterThread(&WaitEvent);

	for(bForever=TRUE; bForever;)
	{
		nWaitStatus = WaitForMultipleObjects(WaitEvent.nEvents, WaitEvent.arEventHandle, FALSE, pThis->m_dwTimeout);
        //XDEBUG("Wait %d === \r\n", nWaitStatus);
		switch(nWaitStatus) {
		  case EVENT_TYPE_TERMINATE :
               //XDEBUG("Wait Terminate\r\n");
			   pThis->OnTerminate();
			   bForever = FALSE;
			   break;
		  case EVENT_TYPE_ACTIVE :
		  case EVENT_TYPE_ACTIVE+1 :
		  case EVENT_TYPE_ACTIVE+2 :
		  case EVENT_TYPE_ACTIVE+3 :
		  case EVENT_TYPE_ACTIVE+4 :
		  case EVENT_TYPE_ACTIVE+5 :
               //XDEBUG("Wait Active\r\n");
			   bForever = pThis->OnActiveThread();
			   break;
		  case WAIT_TIMEOUT :
               //XDEBUG("Wait Timeout\r\n");
			   bForever = pThis->OnTimeoutThread();
			   break;
		}
	}

	pThis->OnLeaveThread();

	pThis->m_bAbnormal = pThis->IsExitPending() ? FALSE : TRUE;
	if (pThis->m_bAbnormal)
		pThis->OnAbnormalTerminate();

	pThis->m_bEndSession = TRUE;
	SetEvent(pThis->m_hExitEvent);

	return 0;
}
