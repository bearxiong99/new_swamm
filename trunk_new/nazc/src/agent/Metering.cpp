#include "common.h"
#include "codiapi.h"
#include "AgentService.h"
#include "Metering.h"
#include "Recovery.h"
#include "Polling.h"
#include "MeterReader.h"
#include "Distributor.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "AgentLog.h"
#include "ShellCommand.h"

#include "EndDeviceList.h"
#include "MobileUtility.h"
#include "Utility.h"


/** Metering Memory leak을 검사하고자 할 때 enable 시킨다.
#define __METERING_MEMORY_LEAK_CHECK__
  */

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////

#define MTRFILENAME					"/tmp/data/meter.dat"

#define MAX_METERING_WORKER			MAX_ZIGBEE_WORKER

#define METERING_DEFAULT_OPTION		-1
#define METERING_DEFAULT_OFFSET		-1
#define METERING_DEFAULT_COUNT		-1

CMetering   	*m_pMetering = NULL;

extern int		m_nSystemID;
extern char		m_szFepServer[];

extern int		m_nUploadType;
extern BYTE		m_nAllUploadHour; 
extern int		m_nMeteringThreadCount;

extern BYTE     m_nSchedulerType;
extern UINT     m_nMeteringInterval;
extern UINT		m_nMeterDayMask;
extern UINT		m_nMeterHourMask;
extern BYTE		m_nMeterStartMin;
extern UINT		m_nMeteringRetry;
extern UINT		m_nMeteringPeriod;
extern UINT		m_nUploadTryTime;

extern UINT		m_nEventReadDayMask;
extern UINT		m_nEventReadHourMask;
extern BOOL		m_bEnableMeterTimesync;
extern UINT		m_nTimesyncDayMask;
extern UINT		m_nTimesyncHourMask;

extern BOOL		m_bEnableAutoMetering;
extern BOOL		m_bGprsPerfTestMode;
extern BOOL     m_bPingTestMode;

extern BYTE     m_nMeteringStrategy;

extern int		m_nEventLogSize;
extern int		m_nAlarmLogSize;
extern int		m_nCmdLogSize;
extern int		m_nCommLogSize;
extern int		m_nMobileLogSize;

#if     defined(__PATCH_3_1573__)
extern BOOL    *m_pbPatch_3_1573;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMetering::CMetering() : CJobObject("Metering", 60*60*2)
{
	m_pMetering 	= this;
	m_nOption		= -1;
	m_nLastMetering	= 0;
	m_nLastStart	= 0;
	m_bInitialized	= FALSE;
}

CMetering::~CMetering()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CMetering::Initialize()
{
	// Worker를 생성한다.
	CScheduleJob::Startup((void *)this, MAX_METERING_WORKER);
	
	// 쓰레드를 시작 시킨다.
	if (!CTimeoutThread::StartupThread(7))
        return FALSE;

	m_bInitialized = TRUE;
    m_nScheduler = m_nSchedulerType;
	return TRUE;
}

void CMetering::Destroy()
{
	if (!m_bInitialized)
		return;
	
	// Worker를 모두 종료한다.
	CScheduleJob::Shutdown();

	// 쓰레드를 종료한다.
	CTimeoutThread::ShutdownThread();

	m_bInitialized = FALSE;
}

int CMetering::Query(int nOption, int nOffset, int nCount)
{
	// 초기화 되지 않은 경우
	if (!m_bInitialized)
		return IF4ERR_BUSY;

    if (IsBusyCoreService()) return IF4ERR_BUSY;

	// 옵션을 저장한다.
	m_Locker.Lock();
	m_nOption = nOption;
	m_nOffset = nOffset;
	m_nCount = nCount;
	m_bTimesync = FALSE;
	m_bEventLog = FALSE;
	SetUserRequest(TRUE);
	m_Locker.Unlock();

	// 쓰레드가 Active되게 한다.
	ActiveThread();
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Metering procedure
//////////////////////////////////////////////////////////////////////

void CMetering::MeteringWorker()
{
	METERINGJOB	*pArray;
	TIMESTAMP	tmNow, tmMetering;
	char		szCommand[128];
	char		szFileName[128];
	char		szGUID[20];
	EUI64		*pList;
    ENDIENTRY   endi;
	int			i, nCount = 0;
	int			nRetry, nSeek=0, nSuccess=0;
	int			nTotal = 0;
    int         nSubTotal = 0;
    int         nTarget = 0;
    int         nState;
	time_t		tmStartup;
	time_t		tmStart, now;
	struct	tm	when;
#if defined(__METERING_MEMORY_LEAK_CHECK__)
	UINT		nCheckPoint;
#endif
    UINT        nPeriod;
    UINT        nUsed=0;
	
#if defined(__METERING_MEMORY_LEAK_CHECK__)
	CheckPoint(&nCheckPoint);
#endif

	// Coordinator의 상태를 확인한다.
    nState = codiGetState(GetCoordinator());
    if (nState != CODISTATE_NORMAL)
	{
        XDEBUG("METERING: Coordinator is not ready\r\n");
        return;
    }

    // Issue #10 : Scheduler 변경을 초기화 한다
    m_bSchedulerChange = FALSE;

	time(&m_nLastStart);
	GetTimestamp(&tmNow, NULL);
	GetTimestamp(&tmMetering, NULL);
	XDEBUG("------------- Query startup -------------\r\n");
	XDEBUG("METERING TIME = %04d/%02d/%02d %02d:%02d:%02d\r\n",
			tmNow.year, tmNow.mon, tmNow.day,
			tmNow.hour, tmNow.min, tmNow.sec);
	
	// 전원 노드만 선택한다.
	pList = m_pEndDeviceList->GetListByID(&nCount, 
            ENDDEVICE_ATTR_POLL,         // Polling Node 선택
            0x00);                       // 
	if (pList == NULL)
	{
		XDEBUG("Metering: No line power node.\r\n");
		return;
	}

	// 선택된 노드에 대한 작업 구조체를 할당한다.
	pArray = (METERINGJOB *)MALLOC(sizeof(METERINGJOB)*nCount);
	if (pArray == NULL)
	{
		XDEBUG("Metering: memory allocation fail.\r\n");
		FREE(pList);
		return;
	}

	memset(pArray, 0, sizeof(METERINGJOB)*nCount);
	for(i=0; i<nCount; i++)
	{
		memcpy(&pArray[i].id, &pList[i], sizeof(EUI64));
		pArray[i].nOption  	= m_nOption;
		pArray[i].nOffset  	= m_nOffset;
		pArray[i].nCount  	= m_nCount;
		pArray[i].bTimesync = m_bTimesync;
		pArray[i].bEventLog = m_bEventLog;
	}

	// 반환된 End Device 목록을 반환한다.
	FREE(pList);

	// 임시 파일을 지운다.
	unlink(MTRFILENAME);

	// 검침 시작 로그를 기록한다.
	time(&tmStartup);
	METERING_LOG("Metering start. (Thread=%d, DayMask=0x%08X, HourMask=0x%08X, Period=%d, Retry=%d)\r\n",
					m_nMeteringThreadCount, m_nMeterDayMask, m_nMeterHourMask, 
					m_nMeteringPeriod, m_nMeteringRetry);

	// 최소값 범위로 보정한다.
	m_nMeteringRetry = MAX(m_nMeteringRetry, 1);
	m_nMeteringPeriod = MAX(m_nMeteringPeriod, 1);
	m_nMeteringThreadCount = MAX(m_nMeteringThreadCount, 1);

	for(nRetry=0; nRetry<(int)m_nMeteringRetry; nRetry++)
	{
		time(&tmStart);
		if ((tmStart-tmStartup) > (int)(m_nMeteringPeriod*60))
			break;

        if(nRetry == (int)(m_nMeteringRetry-1)) nPeriod = m_nMeteringPeriod - nUsed;
        else nPeriod = (UINT) (m_nMeteringPeriod / (nRetry + 2));

        nUsed += nPeriod;
        nTarget = nCount - nSuccess;
        nSubTotal = 0;

		XDEBUG("\r\n");
		XDEBUG("************************************************\r\n");
		XDEBUG("METERING: Start (Try=%d, Count=%d, Target=%d)\r\n", nRetry+1, nCount, nTarget); 
		XDEBUG("************************************************\r\n");
		XDEBUG("\r\n");

		nSeek = 0;
		ResetWorker();
		for(;(GetActiveWorker() > 0) || (nSeek < nCount);)
		{
            /** Issue #10 : 검침 도중에 Scheduler가 변경되면 검침을 중지한다 */
            if(m_bSchedulerChange == TRUE)
            {
                m_bSchedulerChange = FALSE;
		        METERING_LOG("\r\nCANCEL METERING : Scheduler Changed\r\n\r\n");
                break;
            }

			// 이미 검침을 완료한 경우 다음을 처리한다.
			if (pArray[nSeek].bOk || pArray[nSeek].bSkip)
			{
				nSeek++;
				continue;
			}

			time(&now);

            if(m_nScheduler == SCHEDULER_TYPE_INTERVAL)
            {
                // Issue #10 : 다음 검침 60초전에 완료 되어야 한다.
                if((now-tmStartup) > (int)((m_nMeteringInterval-1)*60)) break;
            }
            else
            {
                // 지정된 검침 시간이 초과되었는지 검사
                if ((now-tmStartup) > (int)(m_nMeteringPeriod*60)) break;

#if     defined(__PATCH_3_1573__)
                if(*m_pbPatch_3_1573 == FALSE) {
#endif
                // 사용자 요청 Query 일 경우 Keepalive time에 구속되지 않는다.
                if(!IsUserRequest())
                {
			        // 매시 44분~0분까지는 검침을 하지 않는다.
			        when = *localtime(&now);
			        if (when.tm_min >= 44)
			        {
                        WatchdogClear();
                        USLEEP(1000000);
                        continue;
			        }
                }
#if     defined(__PATCH_3_1573__)
                }
#endif
            }
	
			// 남은것이 있으면서 검침 Thread 갯수보다 Worker가 남아 있는 경우
			// Worker에게 작업을 지시한다.
			if ((nSeek < nCount) && (GetActiveWorker() < m_nMeteringThreadCount))
			{
				if (StartWorker(WorkerJob, (void *)&pArray[nSeek]))
				{
					nTotal++;
					nSubTotal++;
					nSeek++;
					continue;
				}	
			}

			// 쓰레드 갯수 제한 또는 모든 Worker가 작업중이면 200ms간 대기한다.
			USLEEP(200000);
		}

    	// Watchdog을 clear한다.
	    WatchdogClear();

	    // 검침 성공한 갯수를 산정한다.
	    for(i=0, nSuccess=0; i<nCount; i++)
	    {
		    if (pArray[i].bOk)
			    nSuccess++;
	    }

		time(&now);
		XDEBUG("\r\n");
		XDEBUG("********************************************************************************\r\n");
		XDEBUG("METERING: End (Try=%d, Count=%d, Target=%d, Success=%d, Elapse=%d seconds)\r\n", nRetry+1, nCount, nTarget, nSuccess, now-tmStart); 
		XDEBUG("********************************************************************************\r\n");
		XDEBUG("\r\n");

	}

	time(&now);
    for(i=0;i<nCount;i++) {
        /*-- 검침 시도 자체를 못한 센서들 --*/
        if(!pArray[i].bComplete) {
	        eui64toa(&pArray[i].id, szGUID);
	        if (!m_pEndDeviceList->CopyEndDeviceByID(&pArray[i].id, &endi))
	        {
		        XDEBUG("METERING-ERROR: Cannot find EUID=%s\r\n", szGUID);
		        METERING_LOG("%s Metering fail(cannot find EUID).\r\n", szGUID);
	        }else {
	            // 검침 실패 시간 저장
		        m_pEndDeviceList->SetLastMeteringFailTime(&pArray[i].id, now);

		        XDEBUG("%s [%s] %s Metering fail(metering timeout).\r\n",
						szGUID, endi.szSerial, endi.szModel);
		        // 검침 실패 로그 저장
		        METERING_LOG("%s [%s] %s Metering fail(metering timeout).\r\n",
						szGUID, endi.szSerial, endi.szModel);

	            m_pEndDeviceList->VerifyEndDevice(&pArray[i].id);
            }
        }
    }
	
	XDEBUG("------------------- result -------------\r\n");
	XDEBUG("Query total sensor        = %d\r\n", nCount);
	XDEBUG("Query total try count     = %d\r\n", nTotal);
	XDEBUG("Query total success count = %d\r\n", nSuccess);
	XDEBUG("Query total elapse        = %d min %d seconds\r\n",
				(int)(now-tmStartup) / 60,
				(int)(now-tmStartup) % 60);
	XDEBUG("----------------------------------------\r\n");

	METERING_LOG("Metering end.\r\n"
	             " Total=%d, Success=%d, Fail=%d\r\n"
	             " Elapse time=%d min %d seconds\r\n\r\n",
                        nCount, nSuccess, nCount-nSuccess,
				        (int)(now-tmStartup) / 60, (int)(now-tmStartup) % 60);

	FREE(pArray);

	// 수동의 경우 마지막 검침 시간을 갱신하지 않는다.
	if (!IsUserRequest())
		time(&m_nLastMetering);

	// 성공한 센서가 존재하는 경우
	if (nSuccess > 0)
	{
		// 검침파일을 복사한다.
		sprintf(szFileName, "/app/data/%04d%02d%02d%02d",
				tmMetering.year, tmMetering.mon, tmMetering.day, tmMetering.hour);	
        while(m_pMeterUploader->SetBusyFile(szFileName)) USLEEP(500000);
		sprintf(szCommand, "cat %s >> %s", MTRFILENAME, szFileName);
		SystemExec(szCommand);
        m_pMeterUploader->ResetBusyFile(szFileName);

		// 메모리의 임시 파일을 삭제한다.
		unlink(MTRFILENAME);

        /** Issue #10 : Interval Scheduler의 경우에도 즉시 전송한다 */
		// 즉시 전송시 업로드한다.
		// 사용자 요청시 즉시 Upload 한다.
		if (IsUserRequest() || m_nUploadType == UPLOAD_TYPE_IMMEDIATELY || m_nScheduler == SCHEDULER_TYPE_INTERVAL)
        {
            m_pMeterUploader->ImmediatelyUpload();
        }
	}

#if defined(__METERING_MEMORY_LEAK_CHECK__)
	Difference(nCheckPoint);
#endif
}

void CMetering::WorkerJob(void *pThis, void *pWorker, void *pJob)
{
	CMetering	*pSelf = (CMetering *)pThis;

	pSelf->SingleMetering((CZigbeeWorker *)pWorker, (METERINGJOB *)pJob);
}

void CMetering::SingleMetering(CZigbeeWorker *pWorker, METERINGJOB *pJob)
{
	METER_STAT		state;
	CMDPARAM		param;
	CChunk			chunk(1024);
    ENDIENTRY       endi;
	char			szGUID[20];
    WORD            path[CODI_MAX_ROUTING_PATH];
    BYTE            hops=0;
	int				nError;
    time_t          now;

	eui64toa(&pJob->id, szGUID);
	if (!m_pEndDeviceList->CopyEndDeviceByID(&pJob->id, &endi))
	{
		XDEBUG("METERING-ERROR: Cannot find EUID=%s\r\n", szGUID);
		return;
	}

	if (endi.nState == ENDISTATE_INIT)
	{
		XDEBUG("METERING-ERROR: Not ready (ENDISTATE_INIT) EUID=%s\r\n", szGUID);
		return;
	}
	
	XDEBUG("\r\nMetering: Start (ID=%s, Option=%d, Offset=%d, Count=%d, Parser=%s)\r\n", 
        	szGUID, pJob->nOption, pJob->nOffset, pJob->nCount, endi.szParser);

	memset(&state, 0, sizeof(METER_STAT));
	memset(&param, 0, sizeof(CMDPARAM));
	memcpy(&param.id, &pJob->id, sizeof(EUI64));
    param.codi 		= GetCoordinator();
    param.nMeteringType = METERING_TYPE_ROUTINE;
	//param.nType		= PARSE_TYPE_READ;
	param.nOption	= pJob->nOption;
	param.nOffset	= pJob->nOffset;
	param.nCount	= pJob->nCount;
	param.bTimesync	= pJob->bTimesync;
	param.bEventLog	= pJob->bEventLog;
	param.pChunk	= &chunk;

	nError = m_pOndemander->Command(endi.szParser, &param, &state);
	if (((nError == ONDEMAND_ERROR_OK) && (chunk.GetSize() > 0)) || (nError == ONDEMAND_ERROR_NOT_SAVE))
	{
		XDEBUG("\r\n##################################\r\n");
		XDEBUG("Metering: %s, Length=%d\r\n", szGUID, chunk.GetSize());
		XDEBUG("##################################\r\n\r\n");

		// 검침 완료 시간
		time(&now);
		if (m_pEndDeviceList->SetLastMeteringTime(&pJob->id, now))
		{
			char	szTemp[6*CODI_MAX_ROUTING_PATH+1], ch[6];
			int		i;
			
            if(nError != ONDEMAND_ERROR_NOT_SAVE)
            {
	            //CMeterParser	*pParser;
	            //pParser = m_pMeterParserFactory->SelectParser(endi.szParser);
                /** Push Metering 이라 할 지라도 검침값이 있으면 저장 한다 */
                //if(IsUserRequest() || pParser == NULL /* || pParser->IsPushMetering() == FALSE*/)
                //{
			        // 검침 데이터를 저장한다.
			        WriteData(MTRFILENAME, &pJob->id, chunk.GetBuffer(), chunk.GetSize());
                //}
            }

			// 로그를 저장한다.
			m_pEndDeviceList->CopyEndDeviceByID(&pJob->id, &endi);
			
            m_pEndDeviceList->QueryRouteRecord(&endi, NULL, &hops, path);
			*szTemp = '\0';
			for(i=0; i<hops; i++)
			{
				if (i) sprintf(ch, " %04X", path[i]);
				else   sprintf(ch, "%04X", path[i]);
				strcat(szTemp, ch);
			}
	
			METERING_LOG("%s [%s] %s Metering ok. (Elapse=%d sec) shortid=%04X, hop=%d path=%s\r\n",
						szGUID, endi.szSerial, endi.szModel, state.nUseTime/1000,
						endi.shortid, endi.hops, szTemp);
		}
		else
		{
			// 검침 진행중에 End Device가 삭제된 경우
			METERING_LOG("%s [%s] %s Cannot found end device. (Elapse=%d sec)\r\n",
						szGUID, endi.szSerial, endi.szModel, state.nUseTime/1000);
		}
        if(nError != ONDEMAND_ERROR_NOT_SAVE) pJob->bOk = TRUE;
        else pJob->bSkip = TRUE;
	}
	else
	{
		// 검침 실패
		XDEBUG("\r\n##################################\r\n");
		XDEBUG("Metering: %s Fail.\r\n", szGUID);
		XDEBUG("##################################\r\n\r\n");

		// 검침 실패 시간 저장
		time(&now);
		m_pEndDeviceList->SetLastMeteringFailTime(&pJob->id, now);

		// 검침 실패 로그 저장
		METERING_LOG("%s [%s] %s Metering fail.\r\n",
						szGUID, endi.szSerial, endi.szModel);
	}

    // 성공이든 실패든 검침을 수행한 센서를 기록
    pJob->bComplete = TRUE;

	m_pEndDeviceList->VerifyEndDevice(&pJob->id);
	XDEBUG("\r\nMetering: End (ID=%s)\r\n", szGUID);
}

//////////////////////////////////////////////////////////////////////
// Thread Override functions
//////////////////////////////////////////////////////////////////////

BOOL CMetering::OnActiveThread()
{
    if (IsBusyCoreService()) return TRUE;

	SetBusy(TRUE);
	MeteringWorker();
	SetBusy(FALSE);

	// Watchdog을 clear한다.
	WatchdogClear();

	// 초기 옵션으로 환원한다.
	m_Locker.Lock();
	SetUserRequest(FALSE);
	m_Locker.Unlock();
	return TRUE;
}

BOOL CMetering::OnTimeoutThread()
{
	time_t	now;
	struct	tm	when, prev;
	UINT	bit, bit1;

	// Watchdog을 clear한다.
	WatchdogClear();

	// 자동 검침이 Disable 되어 있으면 검침하지 않는다.
	if (!m_bEnableAutoMetering)
		return TRUE;

	// GPRS 성능 테스트중에는 수행하지 않는다.
	if (m_bGprsPerfTestMode || m_bPingTestMode)
		return TRUE;

	// 수동으로 시작한 경우는 확인하지 않는다.
	// Busy가 선택되지 않은 경우는 
	if (!IsUserRequest())
	{
		time(&now);
		when = *localtime(&now);
		prev = *localtime(&m_nLastStart);

		// 검침일 일자 Mask 검사
		bit = 1 << (when.tm_mday-1);
		if ((m_nMeterDayMask & bit) == 0)
			return TRUE;

		// 검침 시간 Mask 검사
		bit = 1 << when.tm_hour;
		if ((m_nMeterHourMask & bit) == 0)
			return TRUE;

        switch(m_nScheduler)
        {
            /** Issue #10 : Interval Scheduler 추가 */
            case SCHEDULER_TYPE_INTERVAL:       // Interval scheduler
                // 값을 정규화 한다
                m_nMeteringInterval = GetValidIntervalSchedulerValue(m_nMeteringInterval);

		        // 이전 검침 같은날 같은 시간 같는 시간 블럭이었을 때 검침을 다시 하지 않는다.
		        if (m_nLastStart != 0 && (when.tm_mday == prev.tm_mday) && (when.tm_hour == prev.tm_hour) 
                        && (((int)((when.tm_min+1) / m_nMeteringInterval)) == ((int)((prev.tm_min+1) / m_nMeteringInterval))))
                {
			        return TRUE;
                }

                // 검침 시작분은 현재 분을 m_nMeteringInterval로 나머지 연산해서 0이 나와야 한다
                if((when.tm_min % m_nMeteringInterval)!=0)
                {
                    return TRUE;
                }

                /** 초기 시작 시간에 Gap을 둔다. 이는 미터와 모뎀의 시간 오차에 의한 LP 미생성 문제를
                  * 회피하기 위해 20초의 시간 Gap을 둔다.
                  */
                if(when.tm_sec < 20)
                {
                    return TRUE;
                }

		        bit = 1 << (when.tm_mday-1);
		        bit1 = 1 << when.tm_hour;

		        // Event Log를 읽는 시간이 검사한다.
		        if (((m_nEventReadDayMask & bit) > 0) && ((m_nEventReadHourMask & bit1) > 0) && 
		                (m_nLastStart == 0 || !((when.tm_mday == prev.tm_mday) && (when.tm_hour == prev.tm_hour))))
                {
			        m_bEventLog = TRUE;
                }
                else
                {
			        m_bEventLog = FALSE;
                }

		        // 시간 동기화를 하는 시간인지 확인한다.
		        if (m_bEnableMeterTimesync && ((m_nTimesyncDayMask & bit) > 0) && ((m_nTimesyncHourMask & bit1) > 0) &&
		                (m_nLastStart == 0 || !((when.tm_mday == prev.tm_mday) && (when.tm_hour == prev.tm_hour))))
                {
			        m_bTimesync = TRUE;
                }
                else
                {
			        m_bTimesync = FALSE;
                }

                break;
            default:                            // Mask scheduler
		        // 이전 검침 같은날 같은 시간에는 검침을 다시 하지 않는다.
		        if (m_nLastStart != 0 && (when.tm_mday == prev.tm_mday) && (when.tm_hour == prev.tm_hour))
			        return TRUE;

		        // 검침 시작 시작분이 경과 했는지 확인
		        if (when.tm_min < m_nMeterStartMin)
			        return TRUE;

		        // 검침 시작분 보다 5분이 초과되면 재 부팅해도 다시 검침을 수행하지 않는다.
		        if (when.tm_min > (m_nMeterStartMin+5))
			        return TRUE;

		        bit = 1 << (when.tm_mday-1);
		        bit1 = 1 << when.tm_hour;

		        // Event Log를 읽는 시간이 검사한다.
		        if (((m_nEventReadDayMask & bit) > 0) && ((m_nEventReadHourMask & bit1) > 0))
			        m_bEventLog = TRUE;
                else
			        m_bEventLog = FALSE;

		        // 시간 동기화를 하는 시간인지 확인한다.
		        if (m_bEnableMeterTimesync && ((m_nTimesyncDayMask & bit) > 0) && ((m_nTimesyncHourMask & bit1) > 0))
			        m_bTimesync = TRUE;
                else
			        m_bTimesync = FALSE;

                break;
        }

		// 정기 검침 옵션을 지정한다.
		m_Locker.Lock();
		m_nOption = METERING_DEFAULT_OPTION;
		m_nOffset = METERING_DEFAULT_OFFSET;
		m_nCount  = METERING_DEFAULT_COUNT;
		m_Locker.Unlock();
	}
	else
	{
		// 수동 요청의 경우 EventLog와 Timesync를 수행하지 않는다.
		m_bTimesync = FALSE;
		m_bEventLog = FALSE;
	}

	return OnActiveThread();
}

