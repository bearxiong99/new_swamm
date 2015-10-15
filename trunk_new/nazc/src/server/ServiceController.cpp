#include "common.h"
#include "ServiceController.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CServiceController Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceController::CServiceController()
{
}

CServiceController::~CServiceController()
{
}

//////////////////////////////////////////////////////////////////////
// CServiceController Member Functions
//////////////////////////////////////////////////////////////////////

BOOL CServiceController::OnStarted()
{
	return OnStartService();
}

void CServiceController::OnIdle()
{
	GWMESSAGE	*pMsg;

    for(;!m_bExitSignalPending;)
    {
        sem_wait(&m_exitSem);
        if (m_bExitSignalPending)
            break;

		pMsg = GetMessage();
		if (pMsg != NULL)
		{
			 ExecuteMessage(pMsg->nCommand, pMsg->pParam);
			 free(pMsg);
		}
        else usleep(100000);
    }
}

void CServiceController::OnShutdown()
{
	OnStopService();
}

BOOL CServiceController::PostMessage(int nMessage, void *pParam)	
{
	GWMESSAGE	*pMsg;

	pMsg = (GWMESSAGE *)malloc(sizeof(GWMESSAGE));
	if (pMsg == NULL)
		return FALSE;
	
	memset(pMsg, 0, sizeof(GWMESSAGE));
	pMsg->nCommand 	= nMessage;
	pMsg->pParam	= pParam;
	m_MessageLocker.Lock();
	pMsg->nPosition	= (int)m_MessageList.AddTail(pMsg);
	m_MessageLocker.Unlock();

	sem_post(&m_exitSem);	
	return TRUE;
}

GWMESSAGE *CServiceController::GetMessage()
{
	GWMESSAGE	*pMsg;

	m_MessageLocker.Lock();
	pMsg = m_MessageList.GetHead();
	if (pMsg != NULL)
		m_MessageList.RemoveAt((POSITION)pMsg->nPosition);
	m_MessageLocker.Unlock();

	return pMsg;
}

BOOL CServiceController::ExecuteMessage(int nMessage, void *pParam)
{
	return OnMessage(nMessage, pParam);
}

//////////////////////////////////////////////////////////////////////
// CServiceController Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CServiceController::OnStartService()
{
	return TRUE;
}

void CServiceController::OnStopService()
{
}

BOOL CServiceController::OnMessage(int nCommand, void *pParam)
{
	return TRUE;
}

