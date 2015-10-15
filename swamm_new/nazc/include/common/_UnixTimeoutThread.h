#ifndef __UNIX_TIMEOUT_THREAD_H__
#define __UNIX_TIMEOUT_THREAD_H__

#include <pthread.h>
#include "typedef.h"

class CTimeoutThread
{
// Construction/Destruction
public:
	CTimeoutThread();
	virtual ~CTimeoutThread();

// Attribute
public:
	virtual BOOL IsThreadStarted() const;
	virtual BOOL IsThreadExitPending() const;
	virtual void SetThreadTimeout(int nTimeout);
	virtual int GetThreadTimeout() const;

// Operations
public:
	virtual BOOL StartupThread(int nTimeout);
	virtual void ShutdownThread();
	virtual BOOL DetachThread();
	virtual BOOL ActiveThread();
	virtual void RunThread();

// Override Functions
protected:
	virtual BOOL OnEnterThread();
	virtual void OnLeaveThread();
	virtual BOOL OnActiveThread();
	virtual BOOL OnTimeoutThread();

// Static Functions	
public:
	static	void *watchThread(void *pParam);

// Member variable	
private:
	BOOL			m_bStarted;
    BOOL            m_bExitPending;
    pthread_t       m_watchThreadID;
    pthread_cond_t  m_threadCond;
    pthread_mutex_t m_threadMutex;
	int				m_nTimeout;
};

#endif	// __UNIX_TIMEOUT_THREAD_H__
