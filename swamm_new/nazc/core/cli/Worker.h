#ifndef __WORKER_H__
#define __WORKER_H__

#include "TimeoutThread.h"

typedef void (*PFNWORKERPROC)(void *pThis, CLISESSION *pSession, BOOL bComplete, BOOL bAddHistory);

class CWorker : public CTimeoutThread
{
public:
	CWorker();
	virtual ~CWorker();

// Attribute
public:
	BOOL	IsActive();
	
// Operations
public:
    BOOL   	Initialize();
    void   	Destroy();

	void	Execute(void *pCaller, PFNWORKERPROC pfnWorker, CLISESSION *pSession, BOOL bComplete, BOOL bAddHistory);

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

private:
	PFNWORKERPROC	m_pfnWorker;
	void			*m_pCaller;
	CLISESSION 		*m_pSession;
	BOOL			m_bComplete;
	BOOL			m_bAddHistory;
};

#endif	// __WORKER_H__
