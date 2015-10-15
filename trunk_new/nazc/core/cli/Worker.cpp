#include "common.h"
#include "Worker.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorker::CWorker()
{
	m_pSession	  = NULL;
	m_pfnWorker	  = NULL;
	m_pCaller	  = NULL;
	m_bComplete	  = FALSE;
	m_bAddHistory = FALSE;
}

CWorker::~CWorker()
{
}

//////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////

BOOL CWorker::IsActive()
{
	return (m_pSession != NULL) ? TRUE : FALSE;
}
	
//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CWorker::Initialize()
{
	if (!CTimeoutThread::StartupThread(5*60))
        return FALSE;

	return TRUE;
}

void CWorker::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

void CWorker::Execute(void *pCaller, PFNWORKERPROC pfnWorker, CLISESSION *pSession, BOOL bComplete, BOOL bAddHistory)
{
	m_pCaller	  = pCaller;
	m_pfnWorker	  = pfnWorker;
	m_bComplete   = bComplete;
	m_bAddHistory = bAddHistory;
	m_pSession 	  = pSession;

	ActiveThread();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CWorker::OnActiveThread()
{
    PFNWORKERPROC   pfnWorker;
    void            *pCaller;
	CLISESSION 		*pSession;
	BOOL			bComplete, bAddHistory;

	if (m_pSession != NULL)
	{
		pCaller		  = m_pCaller;
		pfnWorker	  = m_pfnWorker;
		pSession	  = m_pSession;
		bComplete	  = m_bComplete;
		bAddHistory   = m_bAddHistory;

		pfnWorker(pCaller, pSession, bComplete, bAddHistory);

		m_pCaller	  = NULL;
		m_pfnWorker   = NULL;
		m_bComplete	  = FALSE;
		m_bAddHistory = FALSE;
		m_pSession->pWorker = NULL;
		m_pSession    = NULL;
	}
	return TRUE;
}

BOOL CWorker::OnTimeoutThread()
{
	return TRUE;
}

