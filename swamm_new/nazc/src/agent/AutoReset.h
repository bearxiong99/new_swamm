#ifndef __AUTO_RESET_H__
#define __AUTO_RESET_H__

#include "TimeoutThread.h"
#include "Locker.h"
#include "JobObject.h"

class CAutoReset : public CTimeoutThread, public CJobObject
{
public:
	CAutoReset();
	virtual ~CAutoReset();

public:
	BOOL	Initialize();
	void	Destroy();

protected:
	BOOL 	OnActiveThread();
	BOOL	OnTimeoutThread();

protected:
	CLocker	m_Locker;
	time_t	m_nLastReset;
	time_t	m_nLastCheck;
};

extern CAutoReset			*m_pAutoReset;

#endif	// __AUTO_RESET_H__
