//////////////////////////////////////////////////////////////////////
//
//	IdleApplication.h: interface for the CIdleApplication class.
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

#include <pthread.h>
#include <semaphore.h>
#include "typedef.h"

class CIdleApplication
{
// Construction/Destruction
public:
	CIdleApplication();
	virtual	~CIdleApplication();

// Attribute
public:
	virtual BOOL IsStarted() const;
	virtual BOOL IsExitPending();
	virtual	void DropSignal();

// Operations
public:
	virtual	BOOL Startup(BOOL bNoMsg=FALSE);
	virtual	void Shutdown();

// Override Functions
protected:
	virtual BOOL OnStarted();
	virtual void OnIdle();
	virtual void OnShutdown();

// Static Functions
public:
	static void InterruptSignalHandler(int nSignal);

// Member Variable
public:
	BOOL	m_bStarted;
	BOOL	m_bExitSignalPending;
	sem_t	m_exitSem;
};
