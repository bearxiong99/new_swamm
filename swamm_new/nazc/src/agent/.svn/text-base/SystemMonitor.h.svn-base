#ifndef __SYSTEM_MONITOR_H__
#define __SYSTEM_MONITOR_H__

#include "TimeoutThread.h"
#include "Locker.h"
#include "JobObject.h"

#define MOBERR_NOERROR				0
#define MOBERR_PPP_NOT_FOUND		1
#define MOBERR_DCD_OFF				2
#define MOBERR_PING_ERROR			3

class CSystemMonitor : public CTimeoutThread, public CJobObject
{
public:
	CSystemMonitor();
	virtual ~CSystemMonitor();

public:
	BOOL	IsConnected();
	BOOL	IsGsmMode();
	int		GetCount();

public:
	BOOL	Initialize();
	void	Destroy();

	BOOL	NewConnection();
	void	DeleteConnection();
	void	CheckConnection();

	void	PowerResetModem(BOOL bWait);
	void	ResetModem();
	void	ResetSystem();

	void	KeepConnection();
	BOOL	ICMP_Ping(char *pszAddress, int nTimeout, int nRetry);
	BOOL	IF4_Ping(char *pszAddress, int nPort, int nTimeout, int nRetry);
	BOOL	Ping(char *pszAddress);

	const char 	*GetMobileError(int nError);
	void	AddMobileLog(int nType, int nError=0);
	int		GetCSQ();
	void	GetMobileInfo(char *id, BOOL *auth, int *nCSQ);

protected:
	BOOL 	OnActiveThread();
	BOOL	OnTimeoutThread();

protected:
	CLocker	m_Locker;
	CLocker	m_MobileLocker;
	BOOL	m_bConnected;
	BOOL	m_bCheck;
	BOOL	m_bGsmMode;
	int		m_nAliveCount;
};

extern CSystemMonitor			*m_pSystemMonitor;

#endif	// __SYSTEM_MONITOR_H__
