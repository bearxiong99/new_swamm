#include "common.h"
#include "codiapi.h"
#include "ScheduleJob.h"
#include "EndDeviceList.h"
#include "CommonUtil.h"
#include "Ondemander.h"
#include "DebugUtil.h"


extern BYTE m_nSchedulerType;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScheduleJob::CScheduleJob()
{
	m_pCaller		= NULL;
	m_nMaxWorker	= 0;
	m_nActiveWorker = 0;
	m_bActiveQuery	= FALSE;
	m_bUserRequest	= FALSE;
    m_nScheduler = m_nSchedulerType;
    m_bSchedulerChange = FALSE;
}

CScheduleJob::~CScheduleJob()
{
}

//////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////

BOOL CScheduleJob::IsBusy()
{
	return m_bActiveQuery;
}

BOOL CScheduleJob::IsUserRequest()
{
	return m_bUserRequest;
}

int CScheduleJob::GetMaxWorker()
{
	return m_nMaxWorker;
}

int CScheduleJob::GetActiveWorker()
{
	return m_nActiveWorker;
}

void CScheduleJob::SetBusy(BOOL bBusy)
{
	m_bActiveQuery = bBusy;
}

void CScheduleJob::SetUserRequest(BOOL bRequest)
{
	m_bUserRequest = bRequest;
}

/** Issue #10 : Scheduler Type이 변경되었을 때 진행중이던 Scheduler를 조정할 수 있게 한다.
  */
void CScheduleJob::SetSchedulerType(BYTE nSchedulerType)
{
    if(m_nScheduler != nSchedulerType)
    {
        m_bSchedulerChange = TRUE;
        m_nScheduler = nSchedulerType;
    }
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CScheduleJob::Startup(void *pCaller, int nWorker)
{
	int		i;

	m_pCaller = pCaller;
	m_nMaxWorker = MIN(nWorker, MAX_ZIGBEE_WORKER);

	// Worker를 생성한다.
	for(i=0; i<m_nMaxWorker; i++)
		m_Worker[i].Initialize();

	return TRUE;
}

void CScheduleJob::Shutdown()
{
	int		i;

	// Worker를 모두 종료한다.
	for(i=0; i<m_nMaxWorker; i++)
		m_Worker[i].Destroy();
}

void CScheduleJob::StartJob(void *pThis, void *pWorker, void *pJob)
{
	CScheduleJob	*pSelf = (CScheduleJob *)pThis;

	pSelf->m_WorkerLocker.Lock();
	pSelf->m_nActiveWorker++;
	pSelf->m_WorkerLocker.Unlock();
}

void CScheduleJob::EndJob(void *pThis, void *pWorker, void *pJob)
{
	CScheduleJob	*pSelf = (CScheduleJob *)pThis;

	pSelf->m_WorkerLocker.Lock();
	pSelf->m_nActiveWorker--;
	pSelf->m_WorkerLocker.Unlock();

    pSelf->FreeJob(pThis, pWorker, pJob);
}

void CScheduleJob::FreeJob(void *pThis, void *pWorker, void *pJob)
{
}

BOOL CScheduleJob::StartWorker(PFNWORKERPROC pfnWorker, void *pJob)
{
	int		nIndex;

	nIndex = GetWorker();
	if (nIndex == -1)
		return FALSE;

	m_Worker[nIndex].StartJob(m_pCaller, (void *)this, StartJob, EndJob, pfnWorker, pJob);	
	return TRUE;
}

void CScheduleJob::ResetWorker()
{
	m_WorkerLocker.Lock();
	m_nActiveWorker = 0;
	m_WorkerLocker.Unlock();
}

int CScheduleJob::GetWorker()
{
	int		i;

	for(i=0; i<m_nMaxWorker; i++)
	{
		if (!m_Worker[i].IsActive())
			return i;
	}
	return -1;	
}

