//////////////////////////////////////////////////////////////////////
//
//	MultiThread.h: interface for the CMultiThread class.
//
//	This file is a part of the DAS-NMS project.
//	(c)Copyright 2003 NETCRA Software International All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __MULTI_THREAD_H__
#define __MULTI_THREAD_H__

#include "typedef.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_TERMINATE_TIMEOUT			10*1000

#define EVENT_TYPE_TERMINATE				0
#define EVENT_TYPE_ACTIVE					1

typedef struct	_THREADEVENTLIST
{
		int		nEvents;
		HANDLE	arEventHandle[10];
}	THREADEVENTLIST, *PTHREADEVENTLIST;

class CMultiThread  
{
// Construction/Destruction
public:
	CMultiThread();
	virtual ~CMultiThread();

// Attribute
public:
	virtual BOOL IsStarted() const;
	virtual BOOL IsExitPending() const;
	virtual BOOL IsThreadExitPending() const;
	virtual BOOL IsAbnormalTerminate() const;
	virtual BOOL IsEndSession() const;

	virtual void SetTimeout(DWORD dwTimeout);
	virtual DWORD GetTimeout();
	virtual void SetPriority(int nPriority);
	virtual int	GetPriority();
	virtual HANDLE GetStopEventHandle() const;

// Operations
public:
	virtual void Drop();
	virtual void Pause();
	virtual void Resume();

// Override functions
public:
	virtual	BOOL Startup(DWORD dwTimeout, int nPriority);
	virtual	void Shutdown(DWORD dwTimeout);
	virtual	void WaitForThreadExit();

	virtual	void OnStartup(int dwTimeout, int nPriority);
	virtual	void OnShutdown();

// Member variables
public:
	BOOL		m_bExitThread;

protected:
	BOOL		m_bStarted;
	BOOL		m_bAbnormal;
	BOOL		m_bEndSession;

	HANDLE		m_hStopEvent;
	HANDLE		m_hExitEvent;
	HANDLE		m_hThread;
	DWORD		m_dwThreadID;
	DWORD		m_dwTimeout;
	int			m_nPriority;
};

#endif // __MULTI_THREAD_H__
