//////////////////////////////////////////////////////////////////////
//
//  Event.cpp: implementation of the CEvent class.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>

#include "Event.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Event functions
//////////////////////////////////////////////////////////////////////

#ifndef _WIN32
HANDLE CreateEvent(void *pEventAttributes, BOOL bManualReset, BOOL bInitialState, LPSTR lpName)
{
    EVENTHANDLE     *pHandle;

    pHandle = (EVENTHANDLE *)MALLOC(sizeof(EVENTHANDLE));
    if (pHandle == NULL)
        return NULL;

    memset(pHandle, 0, sizeof(EVENTHANDLE));
    pthread_mutex_init(&pHandle->sig_mutex, NULL);
    pthread_cond_init(&pHandle->sig_cond, NULL);
    return (HANDLE)pHandle;
}
#endif

BOOL CloseHandle(HANDLE hEvent)
{
    EVENTHANDLE *pHandle;

    if (hEvent == NULL)
        return FALSE;

    pHandle = (EVENTHANDLE *)hEvent;
    pthread_cond_destroy(&pHandle->sig_cond);
    pthread_mutex_destroy(&pHandle->sig_mutex);
    FREE(pHandle);
    return TRUE;
}

BOOL SetEvent(HANDLE hEvent)
{
    EVENTHANDLE *pHandle;

    if (hEvent == NULL)
        return FALSE;

    pHandle = (EVENTHANDLE *)hEvent;
    pHandle->bApplied = TRUE;
    pthread_cond_signal(&pHandle->sig_cond);
    return TRUE;
}

#ifndef _WIN32
void ResetEvent(HANDLE hEvent)
{
    EVENTHANDLE *pHandle;

    if (hEvent == NULL)
        return;

    pHandle = (EVENTHANDLE *)hEvent;
    pHandle->bApplied = FALSE;
}
#endif

int WaitForSingleObject(HANDLE hEvent, int nSecond)
{
    EVENTHANDLE *pHandle;
    struct  timespec    timeout;
    int     rc = 0;

    if (hEvent == NULL)
        return ETIMEDOUT;

    pHandle = (EVENTHANDLE *)hEvent;
    if (!pHandle->bApplied)
    {
        struct  timeval    tv;
        pthread_mutex_lock(&pHandle->sig_mutex);
        gettimeofday(&tv, NULL);
        timeout.tv_nsec = 0;
        timeout.tv_sec = tv.tv_sec + nSecond;
        rc = pthread_cond_timedwait(&pHandle->sig_cond, &pHandle->sig_mutex, &timeout);
        pthread_mutex_unlock(&pHandle->sig_mutex);
    }
    return rc;
}

