#ifndef __SCHEDULE_JOB_H__
#define __SCHEDULE_JOB_H__

#include "Locker.h"
#include "ZigbeeWorker.h"

#define MAX_ZIGBEE_WORKER			5
/** TODO : Issue #11
  * 
  * End Device를 고려해서 MAX값을 증가시킨다. 
  *
  * Worker를 할당 할 때 Router Device와 End Device간의 가중치 조정이 필요하다.
  *
#define MAX_ZIGBEE_WORKER			10      // End Device를 고려해서 MAX값을 증가
  *
  */

class CScheduleJob
{
public:
	CScheduleJob();
	virtual ~CScheduleJob();

// Attribute
public:
	BOOL	IsBusy();
	BOOL	IsUserRequest();

	int		GetMaxWorker();
	int		GetActiveWorker();

	void	SetBusy(BOOL bBusy=TRUE);
	void	SetUserRequest(BOOL bRequest=TRUE);

    void    SetSchedulerType(BYTE nSchedulerType);

// Operations
public:
    BOOL   	Startup(void *pCaller, int nWorker);
    void   	Shutdown();

	BOOL	StartWorker(PFNWORKERPROC pfnWorker, void *pJob);
	void	ResetWorker();

// Static functions
public:
	static	void StartJob(void *pThis, void *pWorker, void *pJob);
	static	void EndJob(void *pThis, void *pWorker, void *pJob);
	virtual void FreeJob(void *pThis, void *pWorker, void *pJob);

// Member functions
private:
	int		GetWorker();

protected:
    BOOL    m_bSchedulerChange;
    BYTE    m_nScheduler;

private:
	void	*m_pCaller;
	int		m_nMaxWorker;
	int		m_nActiveWorker;
	BOOL	m_bActiveQuery;
	BOOL	m_bUserRequest;

	CLocker			m_WriteLocker;	
	CLocker			m_WorkerLocker;
	CZigbeeWorker	m_Worker[MAX_ZIGBEE_WORKER];
};

#endif	// __SCHEDULE_JOB_H__
