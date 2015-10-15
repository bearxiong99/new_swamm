#ifndef __LAUNCHER_SERVICE_H__
#define __LAUNCHER_SERVICE_H__

enum	pid_enum
{
	PIDINDEX_SYSMON	= 0,
	PIDINDEX_AGENT,
	PIDINDEX_CLI,
	PIDINDEX_DHCP,
	PIDINDEX_PPPOE,
	PIDINDEX_PPP,
	PIDINDEX_PMON,
	PIDINDEX_SECUWAY_SSL,
	PIDINDEX_MAX
};

typedef struct	_tagTASKITEM
{
	char	szName[64];
	int		nIndex;
	int		nPid;
	BOOL	bNoEcho;
	BOOL	bNeedParam;
}	TASKITEM, *PTASKITEM;

class CLauncherService
{
public:
	CLauncherService();
	virtual ~CLauncherService();

public:
	int		GetEthernetType() const;
	int		GetMobileType() const;
	int		GetMobileMode() const;
	int		GetMobileVendor() const;
	char	*GetServerAddress();

// Override Functions
public:
	BOOL	Startup(int argc, char **argv);
	void	Shutdown();

	BOOL	CheckKernelParam(int argc, char **argv);
	void	Monitoring();	
	void	RingMonitoring();

public:
	static	void InterruptSignalHandler(int nSignal);

	void	DisplaySplash();
	BOOL	LoadSetting();
    void    FirmUpdate();
    void	ResetSystem();
	void	TaskSpawn(const char *pszName, int nIndex, BOOL bNoEcho, BOOL bNeedParam);
	void	Respawn(int nPid);

protected:
	TASKITEM *FindTask(int nPid);
	void	SignalCatcher(int nSignal);

protected:
	TASKITEM	m_arrTask[PIDINDEX_MAX];
	BOOL		m_IgnoreParam;
	int			m_nMobile;
	int			m_nDebug;
	int			m_nEthernetType;
	int			m_nMobileType;
	int			m_nMobileMode;
	int			m_nMobileVendor;
	UINT		m_nPowerType;
	char		m_szServer[64];
	char		m_szMcuType[64];
};

extern CLauncherService	*m_pService;

#endif	// __LAUNCHER_SERVICE_H__
