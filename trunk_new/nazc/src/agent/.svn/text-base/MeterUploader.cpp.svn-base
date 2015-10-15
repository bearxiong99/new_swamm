#include <sys/stat.h>
#include <dirent.h>
#include "common.h"
#include "AgentService.h"
#include "MeterUploader.h"
#include "MeterWriter.h"
#include "Variable.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "SystemUtil.h"
#include "MobileUtility.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"
#include "Chunk.h"
#include "ShellCommand.h"

#include "AgentLog.h"

//////////////////////////////////////////////////////////////////////
// CMeterUploader Data Definitions
//////////////////////////////////////////////////////////////////////

typedef struct	_tagLASTUPLOAD_T
{
		char	szFileName[64];
		int		nSize;
		time_t	ctime;
		time_t	mtime;
}	LASTUPLOAD_T;

CMeterUploader 	*m_pMeterUploader = NULL;

extern int      m_nSystemID;
extern char 	m_szFepServer[];
extern BOOL    	m_bEnableAutoUpload;
extern UINT		m_nDataSaveDay;

extern BYTE     m_nSchedulerType;

extern int		m_nUploadType;
extern int 		m_nUploadCycle;
extern int		m_nUploadStartHour;
extern int		m_nUploadStartMin;
extern UINT		m_nUploadTryTime;

extern BOOL		m_bGprsPerfTestMode;

//////////////////////////////////////////////////////////////////////
// CMeterUploader Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeterUploader::CMeterUploader() : CJobObject("MeterUploader", 60*60)
{
	m_pMeterUploader 	= this;
	m_nLastReduce		= 0;
    m_nLastUpload 		= 0;
	m_bUserRequest		= FALSE;
	m_bImmediatelyRequest	= FALSE;
}

CMeterUploader::~CMeterUploader()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterUploader Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CMeterUploader::Initialize()
{
	TIMESTAMP	tmNow;

	GetTimestamp(&tmNow, NULL);
    memset(m_szBusyFile, 0, sizeof(m_szBusyFile));
	if (!CTimeoutThread::StartupThread(9))
        return FALSE;

	return TRUE;
}

void CMeterUploader::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

void CMeterUploader::Upload(TIMESTAMP *pStart, TIMESTAMP *pEnd)
{
	if (pStart && pEnd)
	{
		memcpy(&m_tmUserStart, pStart, sizeof(TIMESTAMP));
		memcpy(&m_tmUserEnd, pEnd, sizeof(TIMESTAMP));
	}
	else
	{
		GetTimestamp(&m_tmUserStart, NULL);
		GetTimestamp(&m_tmUserEnd, NULL);
		m_tmUserStart.hour = 0;
		m_tmUserStart.min = 0;
		m_tmUserStart.sec = 0;
	}

	XDEBUG("START = %04d/%02d/%02d %02d:%02d:%02d\r\n",
		m_tmUserStart.year, m_tmUserStart.mon, m_tmUserStart.day,
		m_tmUserStart.hour, m_tmUserStart.min, m_tmUserStart.sec);
	XDEBUG("  END = %04d/%02d/%02d %02d:%02d:%02d\r\n",
		m_tmUserEnd.year, m_tmUserEnd.mon, m_tmUserEnd.day,
		m_tmUserEnd.hour, m_tmUserEnd.min, m_tmUserEnd.sec);

    m_nLastUpload  = 0;
	m_bUserRequest = TRUE;
	ActiveThread();
}

void CMeterUploader::ImmediatelyUpload()
{
    m_nLastUpload  = 0;
	m_bImmediatelyRequest = TRUE;
	ActiveThread();
}

//////////////////////////////////////////////////////////////////////
// CMeterUploader Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CMeterUploader::CheckUploadTime()
{
	time_t	now;
	struct	tm	prev, when;
	UINT	bit;

	time(&now);
	prev = *localtime(&m_nLastUpload);
	when = *localtime(&now);

    /** Issue #13 : Push 방식으로 올라오는 데이터를 위해 
      *     Interval Scheduler는 5분 간격으로 검침데이터를 전송한다.
      *     이 때 min % 5 에서 나머지가 1 일 때 전송한다.
      */
    if(m_nSchedulerType == SCHEDULER_TYPE_INTERVAL)
    {
	    if ((prev.tm_year == when.tm_year) &&
	   	    (prev.tm_mon == when.tm_mon) &&
	   	    (prev.tm_mday == when.tm_mday) &&
	   	    (prev.tm_hour == when.tm_hour) &&
	   	    (prev.tm_min == when.tm_min))
	   	return FALSE;

        if((when.tm_min % 5) == 1) return TRUE;

        return FALSE;
    }

	if ((prev.tm_year == when.tm_year) &&
	   	(prev.tm_mon == when.tm_mon) &&
	   	(prev.tm_mday == when.tm_mday) &&
	   	(prev.tm_hour == when.tm_hour))
	   	return FALSE;

	switch(m_nUploadType) {
	  case UPLOAD_TYPE_WEEKLY :
		   // 마지막 전송 주간과 같으면 전송 안함
		   if ((prev.tm_year == when.tm_year) &&
			   (prev.tm_mon == when.tm_mon) &&
			   (prev.tm_wday == when.tm_wday))
			   	return FALSE;

		   // 전송일자가 아니면 전송 안함.
		   // 0:일요일, 1:월요일 ..
		   bit = 1 << when.tm_wday;
		   if ((m_nUploadCycle & bit) == 0) 
				return FALSE;

		   if (when.tm_hour != m_nUploadStartHour)
				return FALSE;

		   if (when.tm_min < m_nUploadStartMin)
			   return FALSE;

		   if (when.tm_min > (m_nUploadStartMin+5))
			   return FALSE;
		   return TRUE; 

	  case UPLOAD_TYPE_DAILY :
		   // 마지막 전송 년월일이 같으면 전송 안함
		   if ((prev.tm_year == when.tm_year) &&
			   (prev.tm_mon == when.tm_mon) &&
			   (prev.tm_mday == when.tm_mday))
			   return FALSE;

		   // 선택된 날짜인지 확인한다.
		   bit = 1 << (when.tm_mday-1);
		   if ((m_nUploadCycle & bit) == 0)
				return FALSE;

		   // 업로드 시작 시간이 아니면 전송 안함.
		   if (when.tm_hour != m_nUploadStartHour)
			   return FALSE;

		   if (when.tm_min < m_nUploadStartMin)
			   return FALSE;

		   if (when.tm_min > (m_nUploadStartMin+5))
			   return FALSE;
		   return TRUE;

      case UPLOAD_TYPE_IMMEDIATELY :
		   return FALSE;

	  case UPLOAD_TYPE_DEFAULT :
	  case UPLOAD_TYPE_HOURLY :
	  default :
		   // 선택된 날짜인지 확인한다.
		   bit = 1 << when.tm_hour;
		   if ((m_nUploadCycle & bit) == 0)
				return FALSE;

		   if (when.tm_min < m_nUploadStartMin)
			   return FALSE;

		   if (when.tm_min > (m_nUploadStartMin+5))
			   return FALSE;
		   return TRUE;
	}
	return FALSE;
}

BOOL CMeterUploader::SetBusyFile(char * pszFileName)
{
    BOOL isBusy = FALSE;

    if(pszFileName == NULL) return isBusy;

	m_Locker.Lock();
    if(strcmp(m_szBusyFile, pszFileName)) {
        memset(m_szBusyFile, 0, sizeof(m_szBusyFile));
        strncpy(m_szBusyFile, pszFileName, 
            strlen(pszFileName) > sizeof(m_szBusyFile) ? sizeof(m_szBusyFile) - 1 : strlen(pszFileName));
        isBusy = TRUE;
    }
	m_Locker.Unlock();
    return isBusy;
}

void CMeterUploader::ResetBusyFile(char * pszFileName)
{
    if(pszFileName == NULL) return;

	m_Locker.Lock();
    m_szBusyFile[0] = 0x00;
	m_Locker.Unlock();
}

void CMeterUploader::UploadComplete(char * pszFileName)
{
    time_t now;
    char pszNewFileName[1024];
    char *indexPtr = rindex(pszFileName,'/');

    if(indexPtr==NULL) indexPtr = pszFileName;
    else indexPtr++;
    if(strlen(indexPtr) > DATE_TIME_LEN) return ;

	time(&now);
    sprintf(pszNewFileName,"%s.uploaded.%ld", pszFileName, now);

    rename(pszFileName, pszNewFileName);
}

void CMeterUploader::GetUploadRange(TIMESTAMP *pStart, TIMESTAMP *pEnd)
{
    memcpy(pStart, &m_tmUserStart, sizeof(TIMESTAMP));
	memcpy(pEnd, &m_tmUserEnd, sizeof(TIMESTAMP));
}

void CMeterUploader::SortFileList(char **pList, int nCount)
{
	char	*p;
	int		i, j;

	for(i=0; i<(nCount-1); i++)
	{
		for(j=i+1; j<nCount; j++)
		{
			if (strcmp(pList[i], pList[j]) > 0)
			{
				p = pList[i];
				pList[i] = pList[j];
				pList[j] = p;
			}
		}
	}
}

int CMeterUploader::UploadFileList(char **pList, int nCount)
{
	time_t	    tmUpload, now;
	int		i,j, nSuccess=0;
    int         retryCount=10;
    BOOL    getPermit;

	time(&tmUpload);

	for(i=0; i<nCount; i++)
	{	
		// 제한 시간이 초과되면 중단.
		time(&now);
		if ((int)abs(((now-tmUpload)/60)) > m_nUploadTryTime)
			break;

        for(j=0; j<retryCount; j++) {
            if((getPermit=SetBusyFile(pList[i]))) break;
            usleep(100000);
        }
        if(!getPermit) {
            XDEBUG("Get File Handle fail : %s\r\n", pList[i]);
            break;
        } 
		XDEBUG("Upload try : %s\r\n", pList[i]);
		if (!UploadFile(pList[i]))
		{
		    XDEBUG("Upload fail : %s\r\n", pList[i]);
            ResetBusyFile(pList[i]);
			break;
		}
		XDEBUG("Upload success : %s\r\n", pList[i]);

        UploadComplete(pList[i]);        
        ResetBusyFile(pList[i]);

        nSuccess ++;
	}

	return nSuccess;
}

void CMeterUploader::UploadMeteringData()
{
	DIR			*dir_fdesc;
	dirent		*dp;
	time_t		now;
	char		szTime[30];
	char		szFileName[128];
	char		szStart[25], szEnd[25];
	TIMESTAMP	tmStart, tmEnd;
	int			i, nFileCount = 0, nSuccess=0;

    if(!m_bUserRequest && !m_bImmediatelyRequest)
    {
	    m_bImmediatelyRequest = FALSE;
        if(!CheckUploadTime()) return;
    }
	m_bImmediatelyRequest = FALSE;

	time(&now);
	MakeTimeString(szTime, &now, TRUE);	

	// 최소값을 비교하여 보정한다.
	m_nUploadTryTime = MAX(m_nUploadTryTime, 1);

	XDEBUG("==================================================\r\n");
	XDEBUG(" Start metering upload (TIME=%s for %d min).\r\n", szTime, m_nUploadTryTime);
	XDEBUG("==================================================\r\n");

	UPLOAD_LOG("Start upload\r\n");

    if(m_bUserRequest) {
	    GetUploadRange(&tmStart, &tmEnd);
	    sprintf(szStart, "%04d%02d%02d%02d", 
			tmStart.year, tmStart.mon, tmStart.day, tmStart.hour);
	    sprintf(szEnd, "%04d%02d%02d%02d", 
			tmEnd.year, tmEnd.mon, tmEnd.day, tmEnd.hour);
    }

	if (m_bGprsPerfTestMode == FALSE)
	{
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

                if(m_bUserRequest) {
                    if (strncmp(dp->d_name, szStart, DATE_TIME_LEN) < 0) continue;
                    if (strncmp(dp->d_name, szEnd, DATE_TIME_LEN) > 0) continue;
                }else if(strlen(dp->d_name) > DATE_TIME_LEN) continue;

				sprintf(szFileName, "/app/data/%s", dp->d_name);
				m_pList[nFileCount] = strdup(szFileName);
				nFileCount++;
				if (nFileCount >= MAX_UPLOAD_FILE_COUNT)
					break;
			}
			closedir(dir_fdesc);

			if (nFileCount > 0)
			{
				// 목록을 Sort하고 업로드한다.
				SortFileList(m_pList, nFileCount);
				nSuccess = UploadFileList(m_pList, nFileCount);
			
				for(i=0; i<nFileCount; i++)
					free(m_pList[i]);	
			}
		}
	}
	else
	{
		printf("UPLOAD: GPRS Backhaul latency test . . .\r\n");
		UploadFile("/app/sw/test.dat");
	}

	UPLOAD_LOG("Total %d of %d file.\r\n", nSuccess, nFileCount);
	UPLOAD_LOG("End upload\r\n");

	time(&now);
	MakeTimeString(szTime, &now, TRUE);	
	if (!m_bUserRequest)
		time(&m_nLastUpload);

	XDEBUG("==================================================\r\n");
	XDEBUG(" Terminate metering upload (TIME=%s).\r\n", szTime);
	XDEBUG("==================================================\r\n");
	XDEBUG("\r\n");
}

void CMeterUploader::ReduceDataDirectory()
{
	DIR		*dir_fdesc;
	dirent	*dp;
	time_t	now, old;
	struct	tm	when;
	char	szPath[128];
	char	szDate[20];

	time(&now);
	if ((now-m_nLastReduce) <= (60*60))
		return;

	when = *localtime(&now);
	/** 매 0시에만 파일을 지우던 것을 매시간 지우도록 변경
      * if ((when.tm_hour != 0) || (when.tm_min < 50))
      */
	if (when.tm_min < 50)
		return;

	time(&old);
	old -= m_nDataSaveDay * (24*(60*60));
	when = *localtime(&old);
	sprintf(szDate, "%04d%02d%02d%02d",
			when.tm_year+1900,
			when.tm_mon+1,
			when.tm_mday,
			when.tm_hour);

	time(&m_nLastReduce);
	dir_fdesc = opendir("/app/data");
	if (dir_fdesc == NULL)
		return;

	for(; (dp=readdir(dir_fdesc));)
	{
#if     defined(_DIRENT_HAVE_D_TYPE)
		if (dp->d_type == DT_DIR)
			continue;
#endif

		if (dp->d_name[0] == '.')
			continue;

		if (strncmp(dp->d_name, szDate, strlen(szDate)) >= 0)
			continue;

		XDEBUG("Reduce /app/data/%s (%s)...\xd\xa", dp->d_name, szDate);
		sprintf(szPath, "/app/data/%s", dp->d_name);
		unlink(szPath);
	}

	closedir(dir_fdesc);
	SystemExec("sync");
}

//////////////////////////////////////////////////////////////////////
// CMeterUploader Thread Override
//////////////////////////////////////////////////////////////////////

BOOL CMeterUploader::OnActiveThread()
{
	// Watchdog을 clear한다.
	WatchdogClear();

	if (m_bEnableAutoUpload || m_bUserRequest)
	{
		UploadMeteringData();
		m_bUserRequest = FALSE;
	}

	ReduceDataDirectory();
	return TRUE;
}

BOOL CMeterUploader::OnTimeoutThread()
{
	return OnActiveThread();
}

