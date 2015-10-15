#include "common.h"
#include "logdef.h"
#include "copyright.h"
#include "ShellCommand.h"
#include "SystemUtil.h"
#include "CommonUtil.h"
//#include "DebugUtil.h"
#include "gpiomap.h"
#include "version.h"
#include "mcudef.h"
#include <dirent.h>
#include "LauncherService.h"
#include "LauncherUtil.h"

//////////////////////////////////////////////////////////////////////
// CLauncherService Construction/Destruction
//////////////////////////////////////////////////////////////////////

char fullVersionString[255];
CLauncherService    *m_pService = NULL;

//////////////////////////////////////////////////////////////////////
// CLauncherService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLauncherService::CLauncherService()
{
    int        i;

    m_pService         = this;
    m_IgnoreParam    = FALSE;
    m_nMobile         = 0;
    m_nDebug        = 0;
    m_nEthernetType = 0;
    m_nMobileType    = 0;
    m_nMobileMode    = 0;
    m_nPowerType    = NZC_POWER_TYPE_LINE | NZC_POWER_TYPE_BATTERY;
    *m_szServer        = '\0';
    *m_szMcuType    = '\0';

    for(i=0; i<PIDINDEX_MAX; i++)
    {
        *m_arrTask[i].szName = '\0';
        m_arrTask[i].nPid    = -1;
        m_arrTask[i].nIndex  = -1;
        m_arrTask[i].bNeedParam = FALSE;
    }
}

CLauncherService::~CLauncherService()
{
}

//////////////////////////////////////////////////////////////////////
// CLauncherService Override Functions
//////////////////////////////////////////////////////////////////////

int    CLauncherService::GetEthernetType() const
{
    return m_nEthernetType;
}

int    CLauncherService::GetMobileType() const
{
    return m_nMobileType;
}

int    CLauncherService::GetMobileMode() const
{
    return m_nMobileMode;
}

int    CLauncherService::GetMobileVendor() const
{
    return m_nMobileVendor;
}

char *CLauncherService::GetServerAddress()
{
    return (char *)m_szServer;
}

//////////////////////////////////////////////////////////////////////
// CLauncherService Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CLauncherService::Startup(int argc, char **argv)
{
    struct  sigaction   handler;

    if (!CheckKernelParam(argc, argv))
        return FALSE;

    // Set Interrrupt Signal Handler
    handler.sa_handler = InterruptSignalHandler;
    sigfillset(&handler.sa_mask);
    sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);
    sigaction(SIGCHLD, &handler, 0);
    FirmUpdate();
    DisplaySplash();
    LoadSetting();
    Monitoring();

    return TRUE;
}

void CLauncherService::Shutdown()
{
}

void CLauncherService::Monitoring()
{
#if 0
    //secuwayssl/secuway_client
    /** 원래 이 부분은 Ramdisk image에 포함되어 있어야 하지만 
      * update 된 Image를 사용하지 않을 경우 여기서 처리해 준다.
      */

    // 제주 실증단지에서 퓨쳐 ssl vpn이 있을 경우 
    int pid = FindProcess("secuway_client");
    if(pid < 0)
    {
        // secuway client가 있는지 검사
        struct         stat file_info;
    
        if(!stat("/app/sw/secuwayssl/secuway_client", &file_info) && !stat("/app/sw/driver/tun.o", &file_info))
        {
            // device file이 없을 경우 생성
            if(stat("/dev/net", &file_info))
            {
                system("mkdir /dev/net");
            }
            if(stat("/dev/net/tun", &file_info))
            {
                system("/bin/mknod /dev/net/tun c 10 200");
            }
            system("/sbin/insmod /app/sw/driver/tun.o 2>&1 > /dev/null");
            TaskSpawn("/app/sw/secuwayssl/secuway_client", PIDINDEX_SECUWAY_SSL, FALSE, TRUE);

            usleep(1000000);
        }
    }
#endif

    TaskSpawn("/app/sw/sysmon", PIDINDEX_SYSMON, FALSE, TRUE);
    usleep(10000);
    TaskSpawn("/app/sw/agent", PIDINDEX_AGENT, FALSE, TRUE);

    usleep(2000000);
    printf("Initializing ...\r\n");
    usleep(8000000);

    TaskSpawn("/app/sw/cli", PIDINDEX_CLI, FALSE, TRUE);

    if(!(m_nPowerType & NZC_POWER_TYPE_LINE) && (m_nPowerType & NZC_POWER_TYPE_SOLAR)) 
    {
        usleep(5000000);
        TaskSpawn("/app/sw/pmon", PIDINDEX_PMON, FALSE, TRUE);
    }

    RingMonitoring();
}

void CLauncherService::RingMonitoring()
{           
    for(;;)
    {
        usleep(1000000); 
    }
}

BOOL CLauncherService::CheckKernelParam(int argc, char **argv)
{
    FILE    *fp;
    char    szBuffer[1024];
    BOOL    bResult = TRUE;
    int        i;

    if (argc > 1)
    {
        for(i=1; i<argc; i++)
        {
            if (strcmp(argv[i], "-ignore") == 0)
                m_IgnoreParam = TRUE;
        }
    }

    if (m_IgnoreParam)
        return TRUE;

    fp = fopen("/proc/cmdline", "rb");
    if (fp != NULL)
    {
        memset(szBuffer, 0, 1024);
        fread(szBuffer, 1024, 1, fp);
        if (strstr(szBuffer, "USERBOOT") != 0)
            bResult = FALSE;
        fclose(fp);
    }
    return bResult;
}

void CLauncherService::ResetSystem()
{
	printf("Launcher Service Reset System \n"); 
	UpdateLauncherLogFile(LOG_DIR, CMD_HISTORY_LOG_FILE, 0, TRUE, "LANUCHER::TRY REBOOT\r\n");

    system("sync");
    system("sync");
    system("sync");
	system("reboot");
}

//////////////////////////////////////////////////////////////////////
// CLauncherService Member Functions
//////////////////////////////////////////////////////////////////////

void CLauncherService::DisplaySplash()
{
    printf("\033[H\033[J");
    printf("   #####    ##  ###    ###  ##  #######\r\n");
    printf("  ##   ##   ##  ####  ####  ##  ##    ##\r\n");
    printf(" ##     ##  ##  ## #### ##  ##  ##    ##\r\n");
    printf(" #########  ##  ##  ##  ##  ##  #######\r\n");
    printf(" ##     ##  ##  ##      ##  ##  ##   ##\r\n");
    printf(" ##     ##  ##  ##      ##  ##  ##    ##\r\n");
    printf(" ##     ##  ##  ##      ##  ##  ##      ##\r\n");
    printf("\r\n");
    printf("AIMIR Firmware Launcher %s\r\n", fullVersionString);
    printf(COPYRIGHT "\r\n");
    printf("\r\n");
}

void CLauncherService::FirmUpdate()
{
    DIR            *dir_fdesc;
    int         pid;
    char        relaunch[256];
    
    pid = FindProcess("launcher");

    dir_fdesc = opendir("/app/upgrade");
    if (dir_fdesc != NULL)      //ready to upgrad)
    {
        memset(relaunch, 0, sizeof(relaunch));
        sprintf(relaunch, "/app/upgrade/relaunching %d", pid);
        system(relaunch);
        closedir(dir_fdesc);
    }
}

BOOL CLauncherService::LoadSetting()
{
    FILE    *fp;

    fp = fopen(VARCONF_FILENAME, "rb");
    if (fp == NULL)
    {
        fp = fopen(VARCONF_OLD_FILENAME, "rb");
        if (fp == NULL)
        {
            system("rm -rf /app/data/*");
            system("rm -rf /app/sensor/*");
            system("rm -rf /app/member/*");
            system("rm -rf /app/log/*");

            SetFileValue("/app/conf/ethernet.type", ETHER_TYPE_PPP);
            SetFileValue("/app/conf/mobile.type", MOBILE_TYPE_GSM);
            SetFileValue("/app/conf/mobile.mode", MOBILE_MODE_CSD);
            SetFileString("/app/conf/server.addr", "0.0.0.0");    // Server address
        }
        else
        {
            fclose(fp);
        }
    }
    else
    {
        fclose(fp);
    }
    
    GetVersionString(fullVersionString);

    SetFileString("/app/conf/version", fullVersionString);
    GetFileValue("/app/conf/ethernet.type", &m_nEthernetType);
    GetFileValue("/app/conf/mobile.type", &m_nMobileType);
    GetFileValue("/app/conf/mobile.mode", &m_nMobileMode);
    GetFileValue("/app/conf/mobile.vendor", &m_nMobileVendor);
    GetFileString("/app/conf/server.addr", m_szServer, sizeof(m_szServer));
    GetFileValue("/app/conf/powertype.mcu", (int *)&m_nPowerType);

    /** Issue #109 
     *  Database 관련 정리를 위한 Script 실행 
     */
    system("/app/sw/script/RunAllTask.script 2> /dev/null");
    system("/app/sw/script/CheckTask.script & 2> /dev/null");

    return TRUE;
}

void CLauncherService::TaskSpawn(const char *pszName, int nIndex, BOOL bNoEcho, BOOL bNeedParam)
{
    int        pid, child=0;
    int        status;
    char    *p;
    FILE    *fp;

    if (nIndex != -1)
    {
        if (nIndex >= PIDINDEX_MAX)
            return;

        if (pszName && *pszName)
        {
            fp = fopen(pszName, "rb");
            if (fp == NULL)
                return;
            fclose(fp);

            strcpy(m_arrTask[nIndex].szName, pszName);
            m_arrTask[nIndex].bNoEcho = bNoEcho;
            m_arrTask[nIndex].bNeedParam = bNeedParam;
            m_arrTask[nIndex].nIndex = nIndex;
        }
    }

    pid = Spawn(child, TRUE, "/app/sw");
    if (pid > 0)
    {
        p = strrchr(m_arrTask[nIndex].szName, '/');
        p++;
        if (m_arrTask[nIndex].bNeedParam)
        {
            if (m_nDebug)
                  execl(m_arrTask[nIndex].szName, p, "-D", NULL);
            else execl(m_arrTask[nIndex].szName, p, NULL);
        }
        else
        {
            execl(m_arrTask[nIndex].szName, m_arrTask[nIndex].szName, NULL);
        }
        waitpid(pid, &status, 0);
        exit(0);
    }
    else
    {
        printf("\r\nLaunching : %s (pid=%0d)\r\n", pszName, child);
        m_arrTask[nIndex].nPid = child;
    }
}

void CLauncherService::Respawn(int nPid)
{
    TASKITEM    *pTask;
    char        *p;
    int            pid;

    pTask = FindTask(nPid);
    if (pTask == NULL)
        return;

    if (strstr(pTask->szName, "agent") != 0)
    {
        // Event log를 남긴다
        UpdateLauncherLogFile(LOG_DIR, EVENT_LOG_FILE, 0, TRUE, "LANUCHER::RESET SYSTEM\r\n");

        // 에이전트인 경우 집중기를 리부팅한다.
        system("sync");
        usleep(1000000);
        system("sync");
        usleep(1000000);

        // 리셋을 한다.
        ResetSystem();

        // 리셋을 실패하면 프로세스를 재기동한다.
        UpdateLauncherLogFile(LOG_DIR, EVENT_LOG_FILE, 0, TRUE, "LANUCHER::RESTART AGENT\r\n");
    }

    usleep(1000000);
    p = strrchr(pTask->szName, '/');
    p++;
    pid = FindProcess(p);

    if (pid > 0)
    {
        kill(pid, SIGTERM);
        usleep(1000000);
    }

    printf("\r\nWARNING: TASK PID(%0d) Immediately terminated.\r\n", nPid);
    printf("Respawn %s\r\n", pTask->szName);
    usleep(3000000);

    TaskSpawn(pTask->szName, pTask->nIndex, pTask->bNoEcho, pTask->bNeedParam);
}

TASKITEM *CLauncherService::FindTask(int nPid)
{
    int        i;

    for(i=0; i<PIDINDEX_MAX; i++)
    {
        if (m_arrTask[i].nPid == nPid)
            return &m_arrTask[i];
    }

    return NULL;
}

void CLauncherService::SignalCatcher(int nSignal)
{
    pid_t    pid;
    int        nReturn;

    if (nSignal != SIGCHLD)
        return;

   pid = waitpid(-1, &nReturn, WNOHANG);
   if (pid > 0)
          Respawn(pid);
}

void CLauncherService::InterruptSignalHandler(int nSignal)
{
    m_pService->SignalCatcher(nSignal);
}

