//////////////////////////////////////////////////////////////////////
//
//	IdleApplication.cpp: implementation of the CIdleApplication class.
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
#include <unistd.h>
#ifndef _WIN32
#include <netdb.h>
#endif
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "IdleApplication.h"
#include "CommonUtil.h"

//////////////////////////////////////////////////////////////////////
// CIdleApplication Data Definitions
//////////////////////////////////////////////////////////////////////

CIdleApplication	*m_pIdleAppThis = NULL;

//////////////////////////////////////////////////////////////////////
// CIdleApplication Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIdleApplication::CIdleApplication()
{
	m_pIdleAppThis	= this;
	m_bStarted		= FALSE;
	m_bExitSignalPending = FALSE;

	sem_init(&m_exitSem, 0, 0);
}

CIdleApplication::~CIdleApplication()
{
	Shutdown();

	sem_destroy(&m_exitSem);
}

//////////////////////////////////////////////////////////////////////
// CIdleApplication Attribute
//////////////////////////////////////////////////////////////////////

BOOL CIdleApplication::IsStarted() const
{
	return m_bStarted;
}

BOOL CIdleApplication::IsExitPending()
{
	return m_bExitSignalPending;
}

void CIdleApplication::DropSignal()
{
	m_bExitSignalPending = TRUE;
	sem_post(&m_exitSem);
	sem_post(&m_exitSem);
	sem_post(&m_exitSem);
}

//////////////////////////////////////////////////////////////////////
// CIdleApplication Operations
//////////////////////////////////////////////////////////////////////

BOOL CIdleApplication::Startup(BOOL bNoMsg)
{
#ifndef _WIN32
	struct	sigaction	handler;
#endif

	if (IsStarted())
		return FALSE;

#ifndef _WIN32
    // Set Interrrupt Signal Handler
    handler.sa_handler = InterruptSignalHandler;
    sigfillset(&handler.sa_mask);
    sigaction(SIGTERM, &handler, 0);
#endif

	if (bNoMsg)
	{
    	int		fd;

		fd = open ("/dev/null", O_RDWR, 0);
		if (fd != -1)
		{ 
			dup2(fd, STDIN_FILENO);
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			if (fd > 2)
				close (fd);
		}
	}

	// File mode mask
	umask(0027);

	if (!OnStarted())
		return FALSE;

	m_bStarted = TRUE;

	// Wait exit event
	OnIdle();

	return TRUE;
}

void CIdleApplication::Shutdown()
{
	if (!IsStarted())
		return;

	OnShutdown();
	m_bStarted	= FALSE;
}

//////////////////////////////////////////////////////////////////////
// CIdleApplication Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CIdleApplication::OnStarted()
{
	return TRUE;
}

void CIdleApplication::OnIdle()
{
	for(;;)
	{
		sem_wait(&m_exitSem);
		if (m_bExitSignalPending)
			break;

		// Semaphore가 생성되면서 Signal될때 방지
		USLEEP(1000);		
	}
}

void CIdleApplication::OnShutdown()
{
}

//////////////////////////////////////////////////////////////////////
// CIdleApplication Interrupt Handler
//////////////////////////////////////////////////////////////////////

void CIdleApplication::InterruptSignalHandler(int nSignal)
{
	m_pIdleAppThis->DropSignal();
}

