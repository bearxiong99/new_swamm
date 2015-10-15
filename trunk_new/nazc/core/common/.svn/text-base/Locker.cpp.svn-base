//////////////////////////////////////////////////////////////////////
//
//  Locker.cpp: implementation of the CLocker class.
//
//  This file is a part of the AIMIR-GCP.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
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

#include "Locker.h"

//////////////////////////////////////////////////////////////////////
// CLocker Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocker::CLocker()
{
#ifndef _WIN32
	pthread_mutex_init(&m_mutex, NULL);
#else
	m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
#endif
}

CLocker::~CLocker()
{
#ifndef _WIN32
	pthread_mutex_destroy(&m_mutex);
#else
	if (m_hMutex)
		CloseHandle(m_hMutex);
#endif
}

//////////////////////////////////////////////////////////////////////
// CLocker Functions
//////////////////////////////////////////////////////////////////////

#ifndef _WIN32
int CLocker::Lock()
#else
DWORD CLocker::Lock(int nTimeout)
#endif
{
#ifndef _WIN32
	return pthread_mutex_lock(&m_mutex);
#else
	DWORD	dwWaitResult;

	dwWaitResult = WaitForSingleObject(m_hMutex, nTimeout);
	return dwWaitResult;
#endif
}

void CLocker::Unlock()
{
#ifndef _WIN32
	pthread_mutex_unlock(&m_mutex);
#else
	ReleaseMutex(m_hMutex);
#endif
}

