//////////////////////////////////////////////////////////////////////
//
//	SyncThread.h: interface for the CSyncedThread class.
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

#ifndef __WIN_SYNCTHREAD_H__
#define __WIN_SYNCTHREAD_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "_WinMultiThread.h"

class CTimeoutThread : public CMultiThread
{
// Construction/Destruction
public:
	CTimeoutThread();
	virtual ~CTimeoutThread();

// Operations
public:
	virtual	BOOL StartupThread(DWORD dwTimeout=INFINITE, int nPriority=THREAD_PRIORITY_NORMAL);
	virtual	void ShutdownThread(DWORD dwTimeout=10000);
	virtual	void ActiveThread();

// Override functions
public:
	virtual	void OnEnterThread(THREADEVENTLIST *pEventList);
	virtual	void OnLeaveThread();
	virtual	void OnTerminate();
	virtual	void OnAbnormalTerminate();
	virtual	BOOL OnActiveThread();
	virtual	BOOL OnTimeoutThread();

// Thread procedure
public:
	static	DWORD WorkThread(LPVOID lpParam);

// Member variable
protected:
	HANDLE	m_hActiveEvent;
};

#endif // __SYNCTHREAD_H__
