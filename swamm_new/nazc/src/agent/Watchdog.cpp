#include "common.h"
#include "Watchdog.h"
#include "SystemMonitor.h"
#include "AgentService.h"
#include "EventManager.h"
#include "Metering.h"
#include "Recovery.h"
#include "MeterUploader.h"
#include "Distributor.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

//////////////////////////////////////////////////////////////////////
// CWatchdog Data Definitions
//////////////////////////////////////////////////////////////////////

#define WATCHDOG_DEV_FILE "/dev/watchdog"

#define _IOC_NRBITS    8
#define _IOC_TYPEBITS    8
#define _IOC_SIZEBITS    14
#define _IOC_DIRBITS    2

#define _IOC_NRMASK    ((1 << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK    ((1 << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK    ((1 << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK    ((1 << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT    0
#define _IOC_TYPESHIFT    (_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT    (_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT    (_IOC_SIZESHIFT+_IOC_SIZEBITS)

#define    WATCHDOG_IOCTL_BASE    'W'

#define _IOC_NONE    0U
#define _IOC_WRITE    1U
#define _IOC_READ    2U

#define _IOC(dir,type,nr,size) \
    (((dir)  << _IOC_DIRSHIFT) | \
     ((type) << _IOC_TYPESHIFT) | \
     ((nr)   << _IOC_NRSHIFT) | \
     ((size) << _IOC_SIZESHIFT))

/* used to create numbers */
#define _IO(type,nr)            _IOC(_IOC_NONE,(type),(nr),0)
#define _IOR(type,nr,size)      _IOC(_IOC_READ,(type),(nr),sizeof(size))
#define _IOW(type,nr,size)      _IOC(_IOC_WRITE,(type),(nr),sizeof(size))
#define _IOWR(type,nr,size)     _IOC(_IOC_READ|_IOC_WRITE,(type),(nr),sizeof(size))

#define WDIOC_GETSTATUS         _IOR(WATCHDOG_IOCTL_BASE, 1, int)
#define WDIOC_GETBOOTSTATUS     _IOR(WATCHDOG_IOCTL_BASE, 2, int)
#define WDIOC_GETTEMP           _IOR(WATCHDOG_IOCTL_BASE, 3, int)
#define WDIOC_SETOPTIONS        _IOR(WATCHDOG_IOCTL_BASE, 4, int)
#define WDIOC_KEEPALIVE         _IOR(WATCHDOG_IOCTL_BASE, 5, int)
#define WDIOC_SETTIMEOUT        _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_GETTIMEOUT        _IOR(WATCHDOG_IOCTL_BASE, 7, int)

#define CREATE_WATCHDOG         {if(access(WATCHDOG_DEV_FILE,F_OK)!=0)mknod(WATCHDOG_DEV_FILE,S_IRWXU|S_IRWXG|S_IFCHR,(10<<8|(130)));}
#define DELETE_WATCHDOG         {if(access(WATCHDOG_DEV_FILE,F_OK)==0)remove(WATCHDOG_DEV_FILE);}
#define SET_WATCHDOG_TIME(T)    {int t=T;int fd=open(WATCHDOG_DEV_FILE,O_RDWR|O_NDELAY );if(fd>0){ioctl(fd,WDIOC_SETTIMEOUT,&t);close(fd);}}
#define WATCHDOG_CLEAR          {int fd=open(WATCHDOG_DEV_FILE,O_RDWR|O_NDELAY );if(fd>0){ioctl(fd,WDIOC_KEEPALIVE,0);close(fd);}}

//////////////////////////////////////////////////////////////////////
CWatchdog       *m_pWatchdog = NULL;

//////////////////////////////////////////////////////////////////////
// CWatchdog Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWatchdog::CWatchdog()
{
    m_pWatchdog    = this;
}

CWatchdog::~CWatchdog()
{
    // Issue #843 : OS Watchdog
    XDEBUG("delete OS WATCHDOG service\r\n");
    DELETE_WATCHDOG
}

//////////////////////////////////////////////////////////////////////
// CWatchdog Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CWatchdog::Initialize()
{
    Add(m_pSystemMonitor);
    Add(m_pAutoReset);
    Add(m_pEventManager);
    Add(m_pMetering);
    Add(m_pRecovery);
    Add(m_pMeterUploader);
    Add(m_pDistributor);

    // Issue #843 : OS Watchdog 
    XDEBUG("create OS WATCHDOG service\r\n");
    CREATE_WATCHDOG
    SET_WATCHDOG_TIME(120) // Watchdog time setup 120sec

    if (!CTimeoutThread::StartupThread(5))
        return FALSE;

    return TRUE;
}

void CWatchdog::Destroy()
{
    CTimeoutThread::ShutdownThread();
    RemoveAll();

    // Issue #843 : OS Watchdog
    XDEBUG("delete OS WATCHDOG service\r\n");
    DELETE_WATCHDOG
}

BOOL CWatchdog::Add(CJobObject *pObject)
{
    WATCHDOGENTRY    *pEntry;

    if (pObject == NULL)
        return FALSE;

    pEntry = (WATCHDOGENTRY *)MALLOC(sizeof(WATCHDOGENTRY));
    if (pEntry == NULL)
        return FALSE;

    memset(pEntry, 0, sizeof(WATCHDOGENTRY));
    pEntry->pObject = pObject;

    m_Locker.Lock();
    pEntry->nPosition = m_List.AddTail(pEntry);
    m_Locker.Unlock();
    return TRUE;
}

void CWatchdog::RemoveAll()
{
    WATCHDOGENTRY    *pEntry, *pDelete;
    POSITION        pos;

    m_Locker.Lock();
    pEntry = m_List.GetFirst(pos);
    while(pEntry)
    {
        pDelete = pEntry;
        pEntry = m_List.GetNext(pos);
        FREE(pDelete);
    }
    m_List.RemoveAll();
    m_Locker.Unlock();
}

//////////////////////////////////////////////////////////////////////
// CWatchdog Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CWatchdog::OnActiveThread()
{
    CJobObject        *pObject;
    WATCHDOGENTRY    *pEntry;
    POSITION        pos;
    unsigned long    nTimer, nPrevious, nElapse;
    int i;

    m_Locker.Lock();
    nTimer = (unsigned long)uptime();
    pEntry = m_List.GetFirst(pos);
    while(pEntry)
    {
        pObject = (CJobObject *)pEntry->pObject;    
        nPrevious = pObject->WatchdogReset(nTimer);
        nElapse = (unsigned long)(nTimer - nPrevious);

        if (nElapse > pObject->GetJobInterval())
        {
            // 주어진 임계 시간을 초과한 경우, 이벤트를 기록하고
            // 시스템을 리부팅한다.
            XDEBUG("------------ WATCHDOG (%s:%d) ----------\r\n", pObject->GetJobName(), nElapse);
            malfWatchdogResetEvent(pObject->GetJobName());
            m_pService->SetResetState(RST_WATCHDOG);
            m_pService->PostMessage(GWMSG_RESET, NULL);

            // Reset message 전송후 1분동안 죽지 않으면 강제 리셋
            usleep(1000000*60);
            system("sync");
            usleep(1000000*3);
            for(i=0; i<5; i++)
            {
#if     defined(__INCLUDE_GPIO__)
                GPIOAPI_WriteGpio(GP_SW_RST_OUT, 0);
#endif
                usleep(1000000);
            }
        }

        pEntry = m_List.GetNext(pos);
    }
    m_Locker.Unlock();
    return TRUE;
}

BOOL CWatchdog::OnTimeoutThread()
{
    // Issue #843 : OS Watchdog
    WATCHDOG_CLEAR

    return OnActiveThread();
}

