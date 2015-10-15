#ifndef __JOB_OBJECT_H__
#define __JOB_OBJECT_H__

#include "Locker.h"

class CJobObject
{
public:
	CJobObject(const char *pszName, unsigned long nInterval);
	virtual ~CJobObject();

public:
	char	*GetJobName();
	unsigned long GetJobInterval();

public:
	void	WatchdogClear();
	unsigned long WatchdogReset(unsigned long nTimer);

private:
	char			m_szJobName[33];
	unsigned long	m_nJobInterval;
	BOOL			m_bSaveJobFlag;
	unsigned long	m_nSaveJobTimer;
	CLocker			m_FlagLocker;
};

#endif	// __JOB_OBJECT_H__
