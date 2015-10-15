#include "common.h"
#include "JobObject.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CJobObject Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJobObject::CJobObject(const char *pszName, unsigned long nInterval)
{
	strcpy(m_szJobName, pszName);
	m_nJobInterval = nInterval;
	m_bSaveJobFlag = FALSE;
	m_nSaveJobTimer = 0;
}

CJobObject::~CJobObject()
{
}

//////////////////////////////////////////////////////////////////////
// CJobObject Construction/Destruction
//////////////////////////////////////////////////////////////////////

char *CJobObject::GetJobName()
{
	return (char *)m_szJobName;	
}

unsigned long CJobObject::GetJobInterval()
{
	return m_nJobInterval;
}

void CJobObject::WatchdogClear()
{
	m_FlagLocker.Lock();
	m_bSaveJobFlag = FALSE;
	m_FlagLocker.Unlock();
}

unsigned long CJobObject::WatchdogReset(unsigned long nTimer)
{
	m_FlagLocker.Lock();
	if (!m_bSaveJobFlag)
	{
		m_nSaveJobTimer = nTimer;
		m_bSaveJobFlag = TRUE;
	}
	m_FlagLocker.Unlock();
	
	return m_nSaveJobTimer;
}

