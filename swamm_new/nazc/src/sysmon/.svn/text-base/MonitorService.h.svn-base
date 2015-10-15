#ifndef __MONITOR_SERVICE_H__
#define __MONITOR_SERVICE_H__

class CMonitorService
{
public:
	CMonitorService();
	virtual ~CMonitorService();

// Override Functions
public:
	BOOL	Startup();
	void	Shutdown();	
	void	WatchSystem();	

protected:
	void	DisplaySplash();
	BOOL 	CheckMemory();
	BOOL 	CheckFlash();
	BOOL 	CheckTelnet();
	BOOL 	CheckAgent();
	BOOL 	CheckLauncher();
	void	RebootSystem();
	void	ReduceFileSystem();
	BOOL	KillAgent();
	BOOL	KillTelnet();
	BOOL	IsProcess(const char *pszName);

protected:
	static	void InterruptSignalHandler(int nSignal);
	void	SignalCatcher(int nSignal);

protected:
	BOOL	m_bExitSignalPending;
	time_t	m_tmLastCheck;
	int		m_nTelnetError;
	int		m_nAgentError;
	UINT	m_nTotal;
	UINT	m_nUse;
	UINT	m_nFree;
	UINT	m_nCache;
	UINT	m_nBuffer;
    UINT    m_nPort;
};

#endif	// __MONITOR_SERVICE_H__
