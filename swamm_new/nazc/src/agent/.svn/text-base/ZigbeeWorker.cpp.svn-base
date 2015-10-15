#include "common.h"
#include "ZigbeeWorker.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZigbeeWorker::CZigbeeWorker()
{
	m_pSaveJob	= NULL;
	m_pfnStart	= NULL;
	m_pfnEnd	= NULL;
	m_pfnWorker	= NULL;
	m_pCaller	= NULL;
	m_pParent	= NULL;
}

CZigbeeWorker::~CZigbeeWorker()
{
}

//////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////

BOOL CZigbeeWorker::IsActive()
{
	return (m_pSaveJob != NULL) ? TRUE : FALSE;
}
	
//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CZigbeeWorker::Initialize()
{
	if (!CTimeoutThread::StartupThread(3))
        return FALSE;
	return TRUE;
}

void CZigbeeWorker::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

void CZigbeeWorker::StartJob(void *pCaller, void *pParent, PFNWORKERSTART pfnStart, PFNWORKEREND pfnEnd, PFNWORKERPROC pfnWorker, void *pJob)
{
	m_SaveLocker.Lock();
	m_pfnStart 	= pfnStart;
	m_pfnEnd	= pfnEnd;
	m_pfnWorker	= pfnWorker;
	m_pCaller	= pCaller;
	m_pParent	= pParent;
	m_pSaveJob 	= pJob;

	// 시작 함수를 호출한다.
	pfnStart(m_pParent, (void *)this, pJob);
	m_SaveLocker.Unlock();

	ActiveThread();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CZigbeeWorker::OnActiveThread()
{
	if (m_pSaveJob != NULL)
	{
		m_pfnWorker(m_pCaller, (void *)this, m_pSaveJob);

		m_SaveLocker.Lock();
		// 종료 함수를 호출한다.
		m_pfnEnd(m_pParent, (void *)this, m_pSaveJob);

		m_pfnStart 	= NULL;
		m_pfnEnd	= NULL;
		m_pfnWorker	= NULL;
		m_pParent	= NULL;
		m_pSaveJob 	= NULL;
		m_SaveLocker.Unlock();
	}
	return TRUE;
}

BOOL CZigbeeWorker::OnTimeoutThread()
{
	return OnActiveThread();
}

