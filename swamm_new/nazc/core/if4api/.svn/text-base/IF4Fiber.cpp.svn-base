#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "common.h"
#include "if4frame.h"
#include "if4api.h"
#include "IF4Fiber.h"
#include "IF4Service.h"

//////////////////////////////////////////////////////////////////////
// CIF4Fiber Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIF4Fiber::CIF4Fiber()
{
}

CIF4Fiber::~CIF4Fiber()
{
}

//////////////////////////////////////////////////////////////////////
// CIF4Fiber Operations
//////////////////////////////////////////////////////////////////////

BOOL CIF4Fiber::Start(int nPort, int nMaxSession)
{
	return TRUE;
}

void CIF4Fiber::Stop()
{
}

//////////////////////////////////////////////////////////////////////
// CIF4Fiber callback
//////////////////////////////////////////////////////////////////////

void CIF4Fiber::OnDataFile(WORKSESSION *pSession, UINT nSourceId, BYTE nType, char *pszName, BYTE *pData, int nLength)
{
	if (pSession == NULL)
		return;

	if (m_pIF4Service->m_pfnOnDataFile == NULL)
		return;

	m_pIF4Service->m_pfnOnDataFile((char *)pSession->szAddress, nSourceId, nType, pszName, pData, nLength); 
}

void CIF4Fiber::OnData(WORKSESSION *pSession, UINT nSourceId, IF4_DATA_FRAME *pData, int nLength)
{
	if (pSession == NULL)
		return;

	if (m_pIF4Service->m_pfnOnData == NULL)
		return;

	m_pIF4Service->m_pfnOnData((char *)pSession->szAddress, nSourceId, (BYTE *)pData, nLength); 
}

void CIF4Fiber::OnTypeR(WORKSESSION *pSession, UINT nSourceId, IF4_TYPER_FRAME *pData, int nLength)
{
	if (pSession == NULL)
		return;

	if (m_pIF4Service->m_pfnOnTypeR == NULL)
		return;

	m_pIF4Service->m_pfnOnTypeR((char *)pSession->szAddress, nSourceId, (BYTE *)pData, nLength); 
}

void CIF4Fiber::OnEvent(WORKSESSION *pSession, UINT nSourceId, IF4_EVENT_FRAME *pEvent, int nLength)
{
	if (pSession == NULL)
		return;

	if (m_pIF4Service->m_pfnOnEvent == NULL)
		return;

	m_pIF4Service->m_pfnOnEvent((char *)pSession->szAddress, nSourceId, (BYTE *)pEvent, nLength);
}

void CIF4Fiber::OnAlarm(WORKSESSION *pSession, UINT nSourceId, IF4_ALARM_FRAME *pAlarm, int nLength)
{
	if (pSession == NULL)
		return;

	if (m_pIF4Service->m_pfnOnAlarm == NULL)
		return;

	m_pIF4Service->m_pfnOnAlarm((char *)pSession->szAddress, nSourceId, (BYTE *)pAlarm, nLength);
}

