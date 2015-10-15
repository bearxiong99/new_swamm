#include <sys/stat.h>
#include <dirent.h>

#include "common.h"
#include "copyright.h"
#include "common/CommonUtil.h"
#include "MonitorService.h"
#include "TelnetClient.h"
#include "SystemUtil.h"
#include "SysMonUtil.h"
#include "logdef.h"
#include "DebugUtil.h"
#include "version.h"
#include "Variable.h"
#include "if4api.h"
#include "gpiomap.h"

//////////////////////////////////////////////////////////////////////
// CMonitorService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonitorService		*m_pService = NULL;

//////////////////////////////////////////////////////////////////////
// CMonitorService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonitorService::CMonitorService()
{
	m_pService = this;
	m_bExitSignalPending = FALSE;

	m_nTelnetError	= 0;
	m_nAgentError	= 0;
	m_nTotal		= 0;
	m_nUse			= 0;
	m_nFree			= 0;
	m_nCache		= 0;
	m_nBuffer		= 0;
    m_nPort         = 0;
}

CMonitorService::~CMonitorService()
{
}

//////////////////////////////////////////////////////////////////////
// CMonitorService Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CMonitorService::Startup()
{
    struct  sigaction   handler;
    VAROBJECT *pObject;

    // Set Interrrupt Signal Handler
    handler.sa_handler = InterruptSignalHandler;
    sigfillset(&handler.sa_mask);
    sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);
    sigaction(SIGCHLD, &handler, 0);

	DisplaySplash();

	VARAPI_Initialize(VARCONF_FILENAME, (VAROBJECT *)m_Root_node, FALSE);

    // Local Port에 대한 설정 값을 얻어온다
	pObject = VARAPI_GetObjectByName("sysLocalPort");
	if (pObject != NULL) 
    {
        m_nPort = pObject->var.stream.u32;
    }

    if(m_nPort == 0)
    {
        m_nPort = 8000; // Default Port
    }

	IF4API_Initialize(m_nPort, NULL);

	GetMemoryInfo(&m_nTotal, &m_nUse, &m_nFree, &m_nCache, &m_nBuffer);

    usleep(60*1000000);
    m_tmLastCheck = uptime();
	for(;!m_bExitSignalPending;)
	{
		usleep(10*1000000);
		WatchSystem();
	}
	return TRUE;
}

void CMonitorService::Shutdown()
{
}

//////////////////////////////////////////////////////////////////////
// CMonitorService Member Functions
//////////////////////////////////////////////////////////////////////

void CMonitorService::DisplaySplash()
{
    char szVersionString[255] = {0,};

    GetVersionString(szVersionString);

	printf("AIMIR System Monitor %s\r\n", szVersionString);
	printf(COPYRIGHT "\r\n");
	printf("\r\n");
}

void CMonitorService::WatchSystem()
{
	time_t	now;

    now = uptime();
	if ((now-m_tmLastCheck) < 60)
		return;

	if (!CheckMemory())
	{
		UpdateLogFile(LOG_DIR, EVENT_LOG_FILE, 0, TRUE, "SYSMON::MEMORY FULL\xd\xa");
		RebootSystem();
	}
	
	if (!CheckFlash())
	{
		ReduceFileSystem();
		UpdateLogFile(LOG_DIR, EVENT_LOG_FILE, 0, TRUE, "SYSMON::FLASH FULL\xd\xa");
		RebootSystem();
	}

	if (!CheckLauncher())
	{
		UpdateLogFile(LOG_DIR, EVENT_LOG_FILE, 0, TRUE, "SYSMON::LAUNCHER FAIL\xd\xa");
		RebootSystem();

	}
	if (!CheckAgent())
	{
		UpdateLogFile(LOG_DIR, EVENT_LOG_FILE, 0, TRUE, "SYSMON::AGENT FAIL\xd\xa");
		RebootSystem();

	}
	if (!CheckTelnet())
	{
		UpdateLogFile(LOG_DIR, EVENT_LOG_FILE, 0, TRUE, "SYSMON::TELNET FAIL\xd\xa");
		KillTelnet();
	}
    m_tmLastCheck = uptime();
}

BOOL CMonitorService::CheckMemory()
{
	UINT	nTotal, nUse, nFree, nCache, nBuffer;
	UINT	nFlashTotal, nFlashUse, nFlashFree;
	double	fSize, fFlashSize;
    BOOL    bReturn = TRUE;

	GetMemoryInfo(&nTotal, &nUse, &nFree, &nCache, &nBuffer);
	GetFlashInfo("/dev/root", &nFlashTotal, &nFlashUse, &nFlashFree);

	fSize = (double)nFree / (double)(1024*1024);
	fFlashSize = (double)nFlashFree / (double)(1024*1024);

	if (nFlashFree < (100*1024)) bReturn = FALSE;
	if (nFree < (5000*1024)) bReturn = FALSE;

    if(!bReturn) {
	    printf("Memory [Share] Total=%d, Use=%d, Free=%d(%.2fM)\r\n", nTotal, nUse, nFree, fSize);
	    printf("        [root] Total=%d, Use=%d, Free=%d(%.2fM)\r\n", nFlashTotal, nFlashUse, nFlashFree, fFlashSize);
    }

	return bReturn;
}

BOOL CMonitorService::CheckFlash()
{
	UINT	nTotal, nUse, nFree;
	double	fSize;
    BOOL    bReturn;

	GetFlashInfo("/dev/mtdblock2", &nTotal, &nUse, &nFree);
	fSize = (double)nFree / (double)(1024*1024);
	bReturn = (nFree < (200*1024)) ? FALSE : TRUE;

    if(!bReturn) {
	    printf("Flash : Total=%d, Use=%d, Free=%d(%.2fM)\r\n", nTotal, nUse, nFree, fSize);
    }
    return bReturn;
}

BOOL CMonitorService::CheckTelnet()
{
	int		nRetry;
	BOOL	bLive = FALSE;

	if (!IsProcess("cli"))
	{
		printf("SYSMON: CLI is not execute.\r\n");
		m_nTelnetError = 0;
		return FALSE;
	}

    CTelnetClient   telnet;
	if (telnet.NewConnection(23))
	{
        for(nRetry=0; nRetry<100; nRetry++)
        {
            if (telnet.IsReceived())
            {
                bLive = TRUE;
                break;
            }
            usleep(200000);
        }
        usleep(100000);
        telnet.DeleteConnection();
    }

	if (!bLive && IsProcess("lrz"))
	{
		XDEBUG("SYSMON: lrz is activate.\r\n");
		m_nTelnetError = 0;
		return TRUE;
	}

	if (!bLive)
	{
		m_nTelnetError++;
		if (m_nTelnetError < 10)
			return TRUE;
		m_nTelnetError = 0;
	    printf("SYSMON: Telnet is not alive.\r\n");
		return FALSE;
	}
	m_nTelnetError = 0;
	return TRUE;
}

BOOL CMonitorService::CheckAgent()
{
	BYTE	bState;
	int		nError;
	BOOL	bLive = FALSE;

	if (!IsProcess("agent"))
	{
		printf("SYSMON: Agent is not execute.\r\n");
		m_nAgentError = 0;
		return FALSE;
	}

	CIF4Invoke  invoke("127.0.0.1", m_nPort, 10);
    nError = invoke.Command("197.12", IF4_CMDATTR_REQUEST | IF4_CMDATTR_MCUIPC | IF4_CMDATTR_RESPONSE);
	if (nError != IF4ERR_NOERROR)
	{
		bLive = FALSE;
	}
	else
	{
		bState = invoke.GetHandle()->pResult[0]->stream.u8;
		bLive = TRUE;
	}

	if (!bLive)
	{
		m_nAgentError++;
		if (m_nAgentError < 10)
			return TRUE;
		m_nAgentError = 0;
		printf("SYSMON: Agent is not alive.\r\n");
		return FALSE;
	}
	m_nAgentError = 0;
	return TRUE;
}

BOOL CMonitorService::CheckLauncher()
{
	if (!IsProcess("launcher"))
	{
		printf("SYSMON: Launcher is not execute.\r\n");
		return FALSE;
	}

	return TRUE;
}

void CMonitorService::RebootSystem()
{
	printf("\r\n");
	printf("SYSMON: Reboot System.\r\n");
	printf("\r\n");
	printf("Broadcast terminate signal.\r\n");
	system("sync");
	usleep(1000000);
	system("sync");
	usleep(1000000);
	printf("System Reset.\r\n");
	usleep(1000000);

    system("reboot");
}

void CMonitorService::ReduceFileSystem()
{
	DIR			*dir_fdesc;
	dirent		*dp;
	time_t		now;
    struct tm   when;
	char		szTime[30];
	char		szFileName[128];

	printf("SYSMON: Reduce FileSystem.\r\n");
	system("rm -rf /app/log/*");
	UpdateLogFile(LOG_DIR, EVENT_LOG_FILE, 0, TRUE, "SYSMON: Reduce FileSystem.\r\n");

	time(&now);
	when = *localtime(&now);
    sprintf(szTime, "%04d%02d%02d",
        when.tm_year + 1900,
        when.tm_mon + 1,
        when.tm_mday);

	dir_fdesc = opendir("/app/data");
	if (dir_fdesc != NULL)
	{
		for(; (dp=readdir(dir_fdesc));)
		{
#if     defined(_DIRENT_HAVE_D_TYPE)
			if (dp->d_type == DT_DIR)
				continue;
#endif
	
			if (dp->d_name[0] == '.')
				continue;

            if(strlen(dp->d_name) <= DATE_TIME_LEN) continue;

            if(strncmp(szTime,dp->d_name,8)) {
	            printf("SYSMON: Reduce Data File [%s]\r\n", dp->d_name);
	            UpdateLogFile(LOG_DIR, EVENT_LOG_FILE, 0, TRUE, "SYSMON: Reduce Data File [%s]\r\n", dp->d_name);
			    sprintf(szFileName, "/app/data/%s", dp->d_name);
                remove(szFileName);
            }
		}
	}

	RebootSystem();
}

BOOL CMonitorService::KillAgent()
{
	int		pid;

	pid = FindProcess("agent");
	if (pid > 0)
	{
		printf("SYSMON: Kill Agent. (pid=%d)\r\n", pid);
		kill(pid, SIGTERM);
		usleep(1000000);
	}
	return TRUE;
}

BOOL CMonitorService::KillTelnet()
{
	int		pid;

	pid = FindProcess("cli");
	if (pid > 0)
	{
		printf("SYSMON: Kill Telnet. (pid=%d)\r\n", pid);
		kill(pid, SIGTERM);
		usleep(1000000);
	}
	return TRUE;
}

BOOL CMonitorService::IsProcess(const char *pszName)
{
	if (FindProcess(pszName) > 0)
		return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMonitorService Member Functions
//////////////////////////////////////////////////////////////////////

void CMonitorService::SignalCatcher(int nSignal)
{
	if ((nSignal == SIGINT) || (nSignal == SIGTERM))
		m_bExitSignalPending = TRUE;
}

void CMonitorService::InterruptSignalHandler(int nSignal)
{
	m_pService->SignalCatcher(nSignal);
}

