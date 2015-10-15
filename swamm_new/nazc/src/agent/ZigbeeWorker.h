#ifndef __ZIGBEE_WORKER_H__
#define __ZIGBEE_WORKER_H__

#include "Locker.h"
#include "TimeoutThread.h"

typedef void (*PFNWORKERSTART)(void *pThis, void *pWorker, void *pJob);
typedef void (*PFNWORKEREND)(void *pThis, void *pWorker, void *pJob);
typedef void (*PFNWORKERPROC)(void *pThis, void *pWorker, void *pJob);

class CZigbeeWorker : public CTimeoutThread
{
public:
	CZigbeeWorker();
	virtual ~CZigbeeWorker();

// Attribute
public:
	BOOL	IsActive();
	
// Operations
public:
    BOOL   	Initialize();
    void   	Destroy();

	void	StartJob(void *pCaller, void *pParent, PFNWORKERSTART pfnStart, PFNWORKEREND pfnEnd, PFNWORKERPROC pfnWorker, void *pJob);

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

private:
	CLocker			m_SaveLocker;
	PFNWORKERPROC	m_pfnStart;
	PFNWORKERPROC	m_pfnEnd;
	PFNWORKERPROC	m_pfnWorker;
	void			*m_pCaller;
	void			*m_pParent;
	void			*m_pSaveJob;
};

#endif	// __ZIGBEE_WORKER_H__
