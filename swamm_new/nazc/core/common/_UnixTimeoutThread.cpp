#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#ifndef _WIN32
#include <netdb.h>
#endif
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

#include "TimeoutThread.h"

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeoutThread::CTimeoutThread()
{
    m_bStarted		= FALSE;
    m_bExitPending	= FALSE;
    m_nTimeout		= 0;
        
    pthread_mutex_init(&m_threadMutex, NULL);
    pthread_cond_init(&m_threadCond, NULL);
}

CTimeoutThread::~CTimeoutThread()
{
	pthread_cond_destroy(&m_threadCond);
	pthread_mutex_destroy(&m_threadMutex);
}

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Attribute
//////////////////////////////////////////////////////////////////////

BOOL CTimeoutThread::IsThreadStarted() const
{
	return m_bStarted;
}

BOOL CTimeoutThread::IsThreadExitPending() const
{
	return m_bExitPending;
}

void CTimeoutThread::SetThreadTimeout(int nTimeout)
{
	m_nTimeout = nTimeout;
}

int CTimeoutThread::GetThreadTimeout() const
{
	return m_nTimeout;
}

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Attribute
//////////////////////////////////////////////////////////////////////

BOOL CTimeoutThread::StartupThread(int nTimeout)
{
	if (IsThreadStarted())
		return FALSE;

	m_nTimeout		= nTimeout;
    m_bExitPending	= FALSE;
        
    if (pthread_create(&m_watchThreadID, NULL, watchThread, (void *)this) != 0)
        return FALSE;

	m_bStarted = TRUE;
	return TRUE;
}

void CTimeoutThread::ShutdownThread()
{
	void *	nStatus;
    
	if (!IsThreadStarted())
		return;

	m_bExitPending = TRUE;
	pthread_cond_signal(&m_threadCond); 
	pthread_join(m_watchThreadID, &nStatus);

	m_bExitPending	= FALSE;
	m_bStarted		= FALSE;
}

BOOL CTimeoutThread::DetachThread()
{
	if (!IsThreadStarted())
		return FALSE;

	pthread_detach(m_watchThreadID); 
	return TRUE;
}

BOOL CTimeoutThread::ActiveThread()
{
	if (!IsThreadStarted())
		return FALSE;

	pthread_cond_signal(&m_threadCond); 
	return TRUE;
}

void CTimeoutThread::RunThread()
{
	struct	timespec	timeout;
	int		rc;

	if (!OnEnterThread())
		return;

	pthread_mutex_lock(&m_threadMutex);
	for(;!m_bExitPending;)
	{
        struct  timeval    tv;
        gettimeofday(&tv, NULL);
        timeout.tv_nsec = 0;
		timeout.tv_sec = tv.tv_sec + m_nTimeout;

		rc = pthread_cond_timedwait(&m_threadCond, &m_threadMutex, &timeout);
		if (m_bExitPending)
			break;

		if (rc == ETIMEDOUT)
		{
			if (!OnTimeoutThread())
				break;
			continue;
		}

		if (!OnActiveThread())
			break;
    }
    pthread_mutex_unlock(&m_threadMutex);

	OnLeaveThread();
    m_bStarted = FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CTimeoutThread::OnEnterThread()
{
	return TRUE;
}

void CTimeoutThread::OnLeaveThread()
{
}

BOOL CTimeoutThread::OnActiveThread()
{
	return TRUE;
}

BOOL CTimeoutThread::OnTimeoutThread()
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Static Functions
//////////////////////////////////////////////////////////////////////

void *CTimeoutThread::watchThread(void *pParam)
{
	CTimeoutThread	*pThis;

	pThis = (CTimeoutThread *)pParam;
	pThis->RunThread();
	return 0;
}

