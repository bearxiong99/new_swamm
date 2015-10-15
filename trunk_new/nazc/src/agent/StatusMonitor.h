#ifndef __STATUS_MONITOR_H__
#define __STATUS_MONITOR_H__

#include "Locker.h"
#include "TimeoutThread.h"

class CStatusMonitor : public CTimeoutThread
{
public:
	CStatusMonitor();
	virtual ~CStatusMonitor();

// Attribute
public:
	void	SetState(BOOL bSet, int nState);
	int	    GetState();
#if     defined(__SUPPORT_NZC1__) || defined(__SUPPORT_NZC2__)   // Issue #959
	int		GetDcdState();
#endif
	UINT	GetErrorMask();
#if     !defined(__EXCLUDE_LOW_BATTERY__)
	int		GetLowBat();
#endif
// Operations
public:
    BOOL    Initialize();
    void    Destroy();

protected:
	BOOL	Monitoring();

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();
    void    LazyPowerMonitoring();

public:
	static	void GpioNotify(int nNotify, int nState, void *pParam, void *pArgment);

private:
#if     defined(__SUPPORT_NZC1__) || defined(__SUPPORT_NZC2__)   // Issue #959
	int		m_nDcdState;
#endif
#if     defined(__TI_AM335X__)
    int     m_nCaseOpen;
#endif
	UINT	m_nErrorMask;
	CLocker	m_Locker;
	time_t	m_nLastTime;
	time_t	m_nLastLowBattery;
    time_t  m_nLastPowerFail;
    time_t  m_nLastPowerRestore;
    BOOL    m_bInitialize;
    unsigned int m_nAdcBattVolt;
    unsigned int m_nAdcMainVolt;
};

extern CStatusMonitor	*m_pStatusMonitor;

#endif	// __STATUS_MONITOR_H__
