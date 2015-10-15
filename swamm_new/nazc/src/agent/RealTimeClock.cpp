#include "common.h"

#include <linux/rtc.h>

#include "AgentService.h"
#include "RealTimeClock.h"
#include "MobileUtility.h"
#include "StatusMonitor.h"
#include "SystemUtil.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "Utility.h"

#include "Metering.h"
#include "Recovery.h"
#include "Polling.h"
#include "MeterReader.h"

//////////////////////////////////////////////////////////////////////
// CRealTimeClock Data Definitions
//////////////////////////////////////////////////////////////////////

#define RTC_TIME_FORMAT		"0000-00-00 00:00:00"

CRealTimeClock   *m_pRealTimeClock = NULL;

extern	BOOL	m_bEnableAutoTimesync;
extern 	int     m_nAutoTimeSyncInterval; 

//////////////////////////////////////////////////////////////////////
// CRealTimeClock Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRealTimeClock::CRealTimeClock()
{
	m_pRealTimeClock	= this;
	m_bInstalled		= FALSE;
	m_bSuccess			= FALSE;
	m_bFirst			= TRUE;
	m_nLastSyncTime		= 0;
	m_bNtpSync			= FALSE;
    *m_szRtcNameSpace   = '\0';
}

CRealTimeClock::~CRealTimeClock()
{
}

//////////////////////////////////////////////////////////////////////
// CRealTimeClock Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CRealTimeClock::IsInstalled() const
{
	return m_bInstalled;
}

BOOL CRealTimeClock::IsAutoSync() const
{
	return m_bEnableAutoTimesync;
}

BOOL CRealTimeClock::IsNtpSync() const
{
  return m_bNtpSync;
}

BOOL CRealTimeClock::IsSuccess() const
{
	return m_bSuccess;
}

BOOL CRealTimeClock::SetTime(TIMESTAMP *pTime)
{
	if (!pTime)
		return FALSE;

    GetUtcTimestamp(pTime, pTime, 0);

	m_Locker.Lock();

    int rtcFd = open("/dev/rtc" , O_RDWR | O_NOCTTY);
    if(rtcFd < 0)
    {
        if(m_szRtcNameSpace[0] != 0)
        {
	        char	szBuffer[100];
	        int		fd;
            fd = open(m_szRtcNameSpace, O_RDWR | O_NOCTTY);
            if (fd >= 0)
	        {
		        sprintf(szBuffer, "set %04d-%02d-%02d %02d:%02d:%02d",
				        pTime->year, pTime->mon, pTime->day,
				        pTime->hour, pTime->min, pTime->sec);

		        XDEBUG("RealTImeClock: %s\xd\xa", szBuffer);
		        write(fd, szBuffer, strlen(szBuffer));
		
		        usleep(1000000);

		        sprintf(szBuffer, "sync");
		        write(fd, szBuffer, strlen(szBuffer));
    	        close(fd);

		        mcuClockChangeEvent();
            }
        }
	}
    else
    {
        int nRet = 0;
        struct rtc_time rtc_tm;
        rtc_tm.tm_mday = pTime->day;
        rtc_tm.tm_mon = pTime->mon-1;
        rtc_tm.tm_year = pTime->year-1900;
        rtc_tm.tm_hour = pTime->hour;
        rtc_tm.tm_min = pTime->min;
        rtc_tm.tm_sec = pTime->sec;
        nRet = ioctl( rtcFd , RTC_SET_TIME , &rtc_tm );

        close(rtcFd);

        if ( !nRet )
        {
            struct stat file_info;

            /** Issue #857 Ramdisk v2.2의 S01setup 에서 hwclock을 system time 으로
             *  변경 시킬 때 -l(localtime) option을 주지 않는 버그가 있다.
             *  이 문제를 해결하기 위해서 v2.2에서는 rtc를 설정 후 현재 시스템
             *  시간을 hwclock으로 localtime을 포함해서 다시 설정하게 한다.
             */
            if(stat("/etc/init.d/S01setup", &file_info))
            {
                // Ramdisk v1.3
                system("hwclock -su");
            }
            else
            {
                // Ramdisk v2.2
                system("hwclock -su; hwclock -wl");
            }
    	    mcuClockChangeEvent();
        }
    }
	m_Locker.Unlock();
	return TRUE;
}

BOOL CRealTimeClock::UpdateTime(char *pszTime)
{
	char		szBuffer[100];
	char		v[10];
	TIMESTAMP	tmNow;
	int			fd;

	if (!pszTime || !*pszTime)
		return FALSE;

	memcpy(v, &pszTime[0], 4); v[4] = '\0';
	tmNow.year = atoi(v);
	if (tmNow.year < 2007)
		return FALSE;

	memcpy(v, &pszTime[4], 2); v[2] = '\0';
	tmNow.mon = atoi(v);
	if (tmNow.mon > 12)
		return FALSE;

	memcpy(v, &pszTime[6], 2); v[2] = '\0';
	tmNow.day = atoi(v);
	if (tmNow.day > 31)
		return FALSE;

	memcpy(v, &pszTime[8], 2); v[2] = '\0';
	tmNow.hour = atoi(v);
	if (tmNow.hour > 23)
		return FALSE;

	memcpy(v, &pszTime[10], 2); v[2] = '\0';
	tmNow.min = atoi(v);
	if (tmNow.min > 59)
		return FALSE;

	memcpy(v, &pszTime[12], 2); v[2] = '\0';
	tmNow.sec = atoi(v);
	if (tmNow.sec > 59)
		return FALSE;
	
    /*-- 시간이 3초 이내로 오차일 때는 time동기화를 하지 않느다 --*/
    if(!GetUtcTimestamp(&tmNow, &tmNow, 4)) return FALSE;

	m_Locker.Lock();
    fd = open(m_szRtcNameSpace, O_RDWR | O_NOCTTY);
    if (fd >= 0)
	{
		sprintf(szBuffer, "set %04d-%02d-%02d %02d:%02d:%02d",
				tmNow.year, tmNow.mon, tmNow.day,
				tmNow.hour, tmNow.min, tmNow.sec);

		XDEBUG("RealTImeClock: %s\xd\xa", szBuffer);
		write(fd, szBuffer, strlen(szBuffer));

		usleep(1000000);

		sprintf(szBuffer, "sync");
		write(fd, szBuffer, strlen(szBuffer));
    	close(fd);

		mcuClockChangeEvent();
	}
	m_Locker.Unlock();
	return TRUE;
}

BOOL CRealTimeClock::TimeSync()
{
	char	szBuffer[20];
	int		fd;
	time_t	now;

	m_Locker.Lock();
	time(&now);
    fd = open(m_szRtcNameSpace, O_RDWR | O_NOCTTY);
    if (fd >= 0)
	{
		sprintf(szBuffer, "sync");
		write(fd, szBuffer, strlen(szBuffer));
    	close(fd);
	}
	XDEBUG("REALTIME: Timesync (error range=%d second)\r\n", m_nLastSyncTime > 0 ? (m_nLastSyncTime-now) : 0);
	m_Locker.Unlock();
	return TRUE;
}

BOOL CRealTimeClock::IsChangeTime()
{
	char	szBuffer1[101];
	char	szBuffer2[101];
	int		fd;

	if (!IsInstalled())
		return FALSE;

	m_Locker.Lock();
    fd = open(m_szRtcNameSpace, O_RDWR | O_NOCTTY);
    if (fd >= 0)
	{
		read(fd, szBuffer1, 100);
		sleep(1);
		usleep(1000);
		read(fd, szBuffer2, 100);
		szBuffer1[100] = '\0';
		szBuffer2[100] = '\0';
    	close(fd);
	}
	m_Locker.Unlock();

	if (strcmp(szBuffer1, szBuffer2) != 0)
		return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CRealTimeClock Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CRealTimeClock::Initialize()
{
	// RTC 드라이버가 설치 되어 있는지 확인한다.
	DetectRealTimeClock();

	// RTC 드라이버가 없으면 동작하지 않는다.
	if (!IsInstalled())
		return FALSE;

	// 최초에는 시간 동기화가 되기 위하여 초기화 한다.
	m_nLastSyncTime = 0;

	// 쓰레드를 시작시킨다.
	if (!CTimeoutThread::StartupThread(7))
        return FALSE;

	return TRUE;
}

void CRealTimeClock::Destroy()
{
	// RTC 드라이버가 없으면 동작하지 않는다.
	if (!IsInstalled())
		return;

	// 쓰레드가 종료될때까지 대기 한다.
	CTimeoutThread::ShutdownThread();
}

BOOL CRealTimeClock::Sync()
{
	if (!IsInstalled())
		return FALSE;

	CTimeoutThread::ActiveThread();
	return TRUE;	
}

BOOL CRealTimeClock::DetectRealTimeClock()
{
    int     fd;

    strcpy(m_szRtcNameSpace, RTC1307);
    fd = open(m_szRtcNameSpace, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        strcpy(m_szRtcNameSpace, RTC1339);
        fd = open(m_szRtcNameSpace, O_RDWR | O_NOCTTY);
		if (fd < 0)
		{
        	strcpy(m_szRtcNameSpace, RTC1340);
        	fd = open(m_szRtcNameSpace, O_RDWR | O_NOCTTY);
		}
    }

    if (fd >= 0)
    {
        m_bInstalled = TRUE;
        close(fd);
    }

    XDEBUG("RealTimeClock: %s is %s.\xd\xa", m_szRtcNameSpace,
                m_bInstalled ? "Installed" : "Not installed");
	return TRUE;
}

BOOL CRealTimeClock::GetUtcTimestamp(TIMESTAMP * localTime, TIMESTAMP * utcTime, int gap)
{
    struct tm tm_t;
    struct tm res_t;
    time_t t,ct;
    BOOL   bValid=TRUE;

    memset(&tm_t, 0, sizeof(struct tm));
    tm_t.tm_year = localTime->year - 1900;
    tm_t.tm_mon = localTime->mon - 1;
    tm_t.tm_mday = localTime->day;
    tm_t.tm_hour = localTime->hour;
    tm_t.tm_min = localTime->min;
    tm_t.tm_sec = localTime->sec;

    t = mktime(&tm_t);

    time(&ct);
    if(abs(t-ct) < gap) bValid = FALSE;

    memset(&res_t, 0, sizeof(struct tm));
    gmtime_r(&t, &res_t);

    utcTime->year = res_t.tm_year + 1900;
    utcTime->mon = res_t.tm_mon + 1;
    utcTime->day = res_t.tm_mday;
    utcTime->hour = res_t.tm_hour;
    utcTime->min = res_t.tm_min;
    utcTime->sec = res_t.tm_sec;

    return bValid;
}



BOOL CRealTimeClock::GetTimeFromNetwork()
{
	BOOL	bConnected;

	bConnected = m_pSystemMonitor->NewConnection();
	XDEBUG("Network Status: %s\r\n", bConnected ? "ON-LINE" : "OFF-LINE");

	if (bConnected){
		 m_bNtpSync = TRUE;
		 system("/app/sw/timesync");
	}
	else XDEBUG("MOBILE-CONNECTION-ERROR: Cannot connect network.\r\n");

	m_pSystemMonitor->DeleteConnection();
	return bConnected ? TRUE : FALSE;
}

BOOL CRealTimeClock::TimesyncFromNetworkOrMobile()
{


	// NTP Time을 구한다.
	return GetTimeFromNetwork();
}

//////////////////////////////////////////////////////////////////////
// CRealTimeClock Thread Override functions
//////////////////////////////////////////////////////////////////////

BOOL CRealTimeClock::OnActiveThread()
{
    cetime_t  now;
	struct	tm	when;
	UINT	nInterval;
	BOOL	bResult;
	
	//  이기능은 사용자 부주의를 막기위하여 무조건으로 변경한다. (호주이후)
	//	if (!IsAutoSync())
	//		return TRUE;

	// 시간이 변경되면서 대기시간이 Timeout 될수 있으므로
	// 검침중에는 시간을 동기화 하지 않는다.
    /* 시간 동기화는 중요하기 때문에 검침중에 동기화 하지 않는 코드를 삭제한다.
	if (m_pMetering->IsBusy() || m_pRecovery->IsBusy() || 
		m_pPolling->IsBusy() || m_pMeterReader->IsBusy())
		return TRUE;
    */


	// 처음에는 무조건 동기화, 다음부터는 주기적으로 동기화
	if (m_nLastSyncTime > 0)
	{
	    // 현재 시간을 구한다.
	    time(&now);
	    when = *localtime(&now);

		// 50분대에 시간 동기화가 이루어지도록 한다.
		if ((when.tm_min < 49) || (when.tm_min > 50))
			return TRUE;

		// 시간 동기화 주기가 되었는지 확인한다.
		// 프로세스가 약간 늦게 동작하는것을 막기 위하여 1분전에 대기하도록 한다.
		nInterval = (m_nAutoTimeSyncInterval) - 60;
		if ((GetSysUpTime()-m_nLastSyncTime) < nInterval)
			return TRUE; 

		// 정확히 50분이 되는 시점이 되기를 기다린다.
		for(;;)
		{
			time(&now);
			when = *localtime(&now);
			if (when.tm_min >= 50)
				break;

			usleep(1000000);
		}
	}

	// 마지막 동기화 시간을 기록한다.
	m_nLastSyncTime = GetSysUpTime();

	// 외부로 부터 시간 동기화가 실패하면 내장 RTC 시간으로 동기화 한다.
	bResult = TimesyncFromNetworkOrMobile();
	if (bResult == FALSE)
		TimeSync();

	return TRUE;
}

BOOL CRealTimeClock::OnTimeoutThread()
{
	return OnActiveThread();
}
