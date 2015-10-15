#include "common.h"
#include "if4frame.h"
#include "IF4Worker.h"
#include "IF4TcpService.h"
#include "IF4Util.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIF4Worker::CIF4Worker()
{
	m_pParam = NULL;
	m_bWorking = FALSE;
}

CIF4Worker::~CIF4Worker()
{
}

//////////////////////////////////////////////////////////////////////
// Initialize/Destroy
//////////////////////////////////////////////////////////////////////

BOOL CIF4Worker::Initialize()
{
	if (!CTimeoutThread::StartupThread(60*60))
        return FALSE;

	return TRUE;
}

void CIF4Worker::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

//////////////////////////////////////////////////////////////////////

BOOL CIF4Worker::IsWorking()
{
	return m_bWorking;
}

void CIF4Worker::SetWorking()
{
	m_bWorking = TRUE;
}

BOOL CIF4Worker::Execute(EXECPARAM *pParam)
{
	if (pParam == NULL)
		return FALSE;

	m_pParam = pParam;
	ActiveThread();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Override Functions
//////////////////////////////////////////////////////////////////////

void CIF4Worker::ExecuteCommand()
{
	EXECPARAM		*pParam = m_pParam;
	CIF4TcpService	*pServer = (CIF4TcpService *)pParam->pThis;	
	WORKSESSION		*pSession;
	IF4Wrapper		wrapper;
	int				nError;

	pSession	= pParam->pSession;
	memset(&wrapper, 0, sizeof(IF4Wrapper));
	wrapper.pSession = pParam->pSession;
	wrapper.pStream	 = pParam->pStream;

	try
	{
		nError = pParam->pCommand->pfnCommand(&wrapper, pParam->pValues, pParam->nCount);
		if (pServer->GetSafeClient(pSession))
		{
			if (nError != IF4ERR_NOERROR)
				 pServer->SendCommandError(pSession, pParam->pStream, &pParam->cmd, pParam->tid, nError);
			else pServer->SendCommandResult(pSession, pParam->pStream, &pParam->cmd, pParam->tid, nError,
									wrapper.pResult, wrapper.nResult, 
									(pParam->attr & IF4_ATTR_DO_NOT_COMPRESS) ? FALSE : TRUE);
		}
	}
	catch(...)
	{
		if (pServer->GetSafeClient(pSession))
		{
			nError = IF4ERR_EXCPTION;
			pServer->SendCommandError(pSession, pParam->pStream, &pParam->cmd, pParam->tid, nError);
		}
	}

	FreeLinkedValue(wrapper.pResult);
	VARAPI_FreeValueList(pParam->pValues, pParam->nCount);
	FREE(pParam);
}

BOOL CIF4Worker::OnActiveThread()
{
	m_bWorking = TRUE;
	ExecuteCommand();
	m_bWorking = FALSE;
	return TRUE;
}

BOOL CIF4Worker::OnTimeoutThread()
{
	return TRUE;
}

