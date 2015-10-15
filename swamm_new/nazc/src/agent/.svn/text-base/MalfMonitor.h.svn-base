#ifndef __MALF_MONITOR_H__
#define __MALF_MONITOR_H__

#include "Locker.h"
#include "TimeoutThread.h"

class CMalfMonitor : public CTimeoutThread
{
public:
	CMalfMonitor();
	virtual ~CMalfMonitor();

public:
	void	SetMemoryRatio(double fRatio);
	void	SetDiskRatio(double fRatio);

// Operations
public:
    BOOL    Initialize();
    void    Destroy();
	void	Check();
	void	CheckExceptCleaning();

protected:
	BOOL	Monitoring();

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

private:
	UINT	m_nLastCleaningTick;
	UINT	m_nLastTransactionCleaningTick;
	double	m_fMemoryRatio;
	double	m_fDiskRatio;
	BOOL	m_bFaultMemory;
	BOOL	m_bFaultDisk;
	BOOL	m_bUserRequest;
	BOOL	m_bCleaningRequest;
};

extern CMalfMonitor	*m_pMalfMonitor;

#endif	// __MALF_MONITOR_H__
