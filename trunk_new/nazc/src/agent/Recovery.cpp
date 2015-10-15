#include "common.h"
#include "codiapi.h"
#include "AgentService.h"
#include "Metering.h"
#include "Recovery.h"
#include "Polling.h"
#include "Distributor.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "AgentLog.h"
#include "ShellCommand.h"

#include "EndDeviceList.h"
#include "MobileUtility.h"
#include "Utility.h"

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////

CRecovery   	*m_pRecovery = NULL;

extern int		m_nSystemID;
extern char		m_szFepServer[];

extern int		m_nUploadType;
extern BYTE		m_nAllUploadHour; 
extern int		m_nMeteringThreadCount;

extern BYTE     m_nSchedulerType;
extern UINT		m_nRecoveryDayMask;
extern UINT		m_nRecoveryHourMask;
extern BYTE		m_nRecoveryStartMin;
extern UINT		m_nRecoveryRetry;
extern UINT		m_nRecoveryPeriod;

extern UINT		m_nEventReadDayMask;
extern UINT		m_nEventReadHourMask;
extern BOOL		m_bEnableMeterTimesync;
extern UINT		m_nTimesyncDayMask;
extern UINT		m_nTimesyncHourMask;

extern BOOL     m_bEnableRecovery;
extern BOOL     m_bEnableAutoMetering;
extern BOOL		m_bGprsPerfTestMode;
extern BOOL     m_bPingTestMode;

extern BYTE     m_nMeteringStrategy;

extern UINT	    m_nMeterDayMask;

#if     defined(__PATCH_3_1573__)
extern BOOL    *m_pbPatch_3_1573;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecovery::CRecovery() : CJobObject("Recovery", 60*60*2)
{
	m_pRecovery 	= this;
	m_nOption		= -1;
	m_nLastRecovery	= 0;
	m_nLastStart	= 0;
	m_bInitialized	= FALSE;
}

CRecovery::~CRecovery()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CRecovery::Initialize()
{
	// Worker를 생성한다.
	CScheduleJob::Startup((void *)this, MAX_RECOVERY_WORKER);
	
	// 쓰레드를 시작 시킨다.
	if (!CTimeoutThread::StartupThread(10))
        return FALSE;

	m_bInitialized = TRUE;
    m_nScheduler = m_nSchedulerType;
	return TRUE;
}

void CRecovery::Destroy()
{
	if (!m_bInitialized)
		return;
	
	// Worker를 모두 종료한다.
	CScheduleJob::Shutdown();

	// 쓰레드를 종료한다.
	CTimeoutThread::ShutdownThread();

	m_bInitialized = FALSE;
}

int CRecovery::Recovery(int nOption, int nOffset, int nCount)
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

void CRecovery::MeteringWorker()
{
	RECOVERYJOB	*pArray;
	TIMESTAMP	tmNow, tmMetering;
	char		szFileName[128];
	char		szCommand[128];
	EUI64		*pList;
	int			i, nCount = 0;
	int			nRetry, nSeek=0, nSuccess=0;
	int			nTotal=0, nRecoveryCount=0;
    int         nSubTotal=0;
    int         nTarget = 0;
    int         nState;
	time_t		tmStartup;
	time_t		tmStart, now;
	struct	tm	when;
    UINT        nPeriod;
    UINT        nUsed=0;

	// Coordinator의 상태를 확인한다.
    nState = codiGetState(GetCoordinator());
    if (nState != CODISTATE_NORMAL)
	{
        XDEBUG("RECOVERY: Coordinator is not ready\r\n");
        return;
    }

    // Issue #10 : Scheduler 변경을 초기화 한다
    m_bSchedulerChange = FALSE;

	time(&m_nLastStart);
	GetTimestamp(&tmNow, NULL);
	GetTimestamp(&tmMetering, NULL);
	XDEBUG("------------- Recovery startup -------------\r\n");
	XDEBUG("RECOVERY TIME = %04d/%02d/%02d %02d:%02d:%02d\r\n",
			tmNow.year, tmNow.mon, tmNow.day,
			tmNow.hour, tmNow.min, tmNow.sec);
	
	// NAZC 전원 노드만 선택한다.
	pList = m_pEndDeviceList->GetListByID(&nCount, 
            ENDDEVICE_ATTR_POLL,         // Polling Node 선택
            0x00);
	if (pList == NULL)
	{
		XDEBUG("Recovery: No line power node.\r\n");
		return;
	}

	// 선택된 노드에 대한 작업 구조체를 할당한다.
	pArray = (RECOVERYJOB *)MALLOC(sizeof(RECOVERYJOB)*nCount);
	if (pArray == NULL)
	{
		XDEBUG("Recovery: memory allocation fail.\r\n");
		FREE(pList);
		return;
	}

	memset(pArray, 0, sizeof(RECOVERYJOB)*nCount);
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
	unlink(RECOVERYFILENAME);

	// Skip할 목록과 합쳐질 파일명을 구한다.
	sprintf(szFileName, "/app/data/%04d%02d%02d%02d",
				tmMetering.year, tmMetering.mon,
				tmMetering.day, tmMetering.hour);	
	CheckSkipNode(pArray, nCount);

	// 
	for(i=0; i<nCount; i++)
	{
		if (!pArray[i].bOk && !pArray[i].bSkip)
		{
			pArray[i].bRecovery = TRUE;
			nRecoveryCount++;
		}
	}

	// 검침 시작 로그를 기록한다.
	time(&tmStartup);
	METERING_LOG("Recovery start. (Thread=%d, DayMask=0x%08X, HourMask=0x%08X, Period=%d, Retry=%d)\r\n",
                    m_nMeteringThreadCount, m_nRecoveryDayMask, m_nRecoveryHourMask,
                    m_nRecoveryPeriod, m_nRecoveryRetry);

	m_nRecoveryRetry = MAX(m_nRecoveryRetry, 1);
	m_nRecoveryPeriod = MAX(m_nRecoveryPeriod, 1);
	m_nMeteringThreadCount = MAX(m_nMeteringThreadCount, 1);

	for(nRetry=0; nRetry<(int)m_nRecoveryRetry; nRetry++)
	{
		time(&tmStart);
		if ((tmStart-tmStartup) > (int)(m_nRecoveryPeriod*60))
			break;

        if(nRetry == (int)(m_nRecoveryRetry-1)) nPeriod = m_nRecoveryPeriod - nUsed;
        else nPeriod = (UINT) (m_nRecoveryPeriod / (nRetry + 2));
        nUsed += nPeriod;
        nTarget = nRecoveryCount - nSuccess;
        nSubTotal = 0;

		XDEBUG("\r\n");
		XDEBUG("************************************************\r\n");
		XDEBUG("RECOVERY: Start (Try=%d, Count=%d, Target=%d)\r\n", nRetry+1, nRecoveryCount, nTarget); 
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
		        METERING_LOG("\r\nCANCEL RECOVERY : Scheduler Changed\r\n\r\n");
                break;
            }

			// 이미 검침을 완료한 경우 다음을 처리한다.
			if (pArray[nSeek].bOk || pArray[nSeek].bSkip)
			{
				nSeek++;
				continue;
			}

			time(&now);

            // 지정된 검침 시간이 초과되었는지 검사
            if ((now-tmStartup) > (int)(m_nRecoveryPeriod*60)) break;

#if     defined(__PATCH_3_1573__)
            if(*m_pbPatch_3_1573 == FALSE) {
#endif
            // 44분에서 0분대까지 실패 검침을 수행하지 않는다.
			when = *localtime(&now);
			if (when.tm_min >= 44)
			{
	    		WatchdogClear();
				usleep(10000000);
				continue;
			}
#if     defined(__PATCH_3_1573__)
            }
#endif

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
			usleep(200000);
		}

    	// Watchdog을 clear한다.
	    WatchdogClear();

	    // 검침 성공한 갯수를 산정한다.
	    for(i=0, nSuccess=0; i<nCount; i++)
	    {
		    if (pArray[i].bOk && pArray[i].bRecovery)
			    nSuccess++;
	    }

		time(&now);
		XDEBUG("\r\n");
		XDEBUG("********************************************************************************\r\n");
		XDEBUG("RECOVERY: End (Try=%d, Count=%d, Target=%d, Success=%d, Elapse=%d seconds)\r\n", nRetry+1, nRecoveryCount, nTarget, nSuccess, now-tmStart); 
		XDEBUG("********************************************************************************\r\n");
		XDEBUG("\r\n");

	}
	
	time(&now);

	XDEBUG("------------------- result -------------\r\n");
	XDEBUG("Recovery total sensor        = %d\r\n", nRecoveryCount);
	XDEBUG("Recovery total try count     = %d\r\n", nTotal);
	XDEBUG("Recovery total success count = %d\r\n", nSuccess);
	XDEBUG("Recovery total elapse        = %d min %d seconds\r\n",
				(int)(now-tmStartup) / 60,
				(int)(now-tmStartup) % 60);
	XDEBUG("----------------------------------------\r\n");

	METERING_LOG("Recovery end.\r\n"
	             " Total=%d, Success=%d, Fail=%d\r\n" 
	             " Elapse time=%d min %d seconds\r\n\r\n",
                        nRecoveryCount, nSuccess, nRecoveryCount-nSuccess,
				        (int)(now-tmStartup) / 60, (int)(now-tmStartup) % 60);

	FREE(pArray);

	// 성공한 센서가 존재하는 경우
	if (nSuccess > 0)
	{
		// 검침파일을 병합한다. 
        while(m_pMeterUploader->SetBusyFile(szFileName)) usleep(500000);
		sprintf(szCommand, "cat %s >> %s", RECOVERYFILENAME, szFileName);
		SystemExec(szCommand);
        m_pMeterUploader->ResetBusyFile(szFileName);

		// 메모리의 임시 파일을 삭제한다.
		unlink(RECOVERYFILENAME);

        /** Issue #10 : Interval Scheduler의 경우에도 즉시 전송한다 */
		// 즉시 전송시 업로드한다.
		// 사용자 요청시 즉시 Upload 한다.
		if (IsUserRequest() || m_nUploadType == UPLOAD_TYPE_IMMEDIATELY || m_nScheduler == SCHEDULER_TYPE_INTERVAL)
        {
            m_pMeterUploader->ImmediatelyUpload();
        }
	}

	// 수동의 경우 마지막 검침 시간을 갱신하지 않는다.
	if (!IsUserRequest())
		time(&m_nLastRecovery);
}

void CRecovery::WorkerJob(void *pThis, void *pWorker, void *pJob)
{
	CRecovery	*pSelf = (CRecovery *)pThis;

	pSelf->SingleMetering((CZigbeeWorker *)pWorker, (RECOVERYJOB *)pJob);
}

void CRecovery::SingleMetering(CZigbeeWorker *pWorker, RECOVERYJOB *pJob)
{
	METER_STAT		state;
	CMDPARAM		param;
	CChunk			chunk(1024);
    ENDIENTRY       endi;
    WORD            path[CODI_MAX_ROUTING_PATH];
    BYTE            hops=0;
	char			szGUID[20];
	int				nError;
    time_t          now;

	eui64toa(&pJob->id, szGUID);
	if (!m_pEndDeviceList->CopyEndDeviceByID(&pJob->id, &endi))
	{
		XDEBUG("RECOVERY-ERROR: Cannot find EUID=%s\r\n", szGUID);
		return;
	}
	
	if (endi.nState == ENDISTATE_INIT)
	{
		XDEBUG("METERING-ERROR: Not ready (ENDISTATE_INIT) EUID=%s\r\n", szGUID);
		return;
	}
	
	XDEBUG("\r\nRecovery: Start (ID=%s, Option=%d, Offset=%d, Count=%d, Parser=%s)\r\n", 
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
	param.bEventLog = pJob->bEventLog;
	param.pChunk	= &chunk;

	nError = m_pOndemander->Command(endi.szParser, &param, &state);
	if (((nError == ONDEMAND_ERROR_OK) && (chunk.GetSize() > 0)) || (nError == ONDEMAND_ERROR_NOT_SAVE))
	{
		XDEBUG("\r\n##################################\r\n");
		XDEBUG("Recovery: %s, Length=%d\r\n", szGUID, chunk.GetSize());
		XDEBUG("##################################\r\n\r\n");

		// 검침 완료 시간
		time(&now);
		if (m_pEndDeviceList->SetLastMeteringTime(&pJob->id, now))
		{
			char	szTemp[6*CODI_MAX_ROUTING_PATH+1], ch[5];
			int		i;

            if(nError != ONDEMAND_ERROR_NOT_SAVE)
            {
	            //CMeterParser	*pParser;
	            //pParser = m_pMeterParserFactory->SelectParser(endi.szParser);
                /** Push Metering 이라 할 지라도 검침값이 있으면 저장 한다 */
                //if(pParser == NULL || pParser->IsPushMetering() == FALSE)
                //{
			        // 검침 데이터를 저장한다.
			        WriteData(RECOVERYFILENAME, &pJob->id, chunk.GetBuffer(), chunk.GetSize());
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

			METERING_LOG("%s [%s] %s Recovery ok. (Elapse=%d sec) shortid=%04X, hop=%d path=%s\r\n",
						szGUID, endi.szSerial, endi.szModel, state.nUseTime/1000,
						endi.shortid, endi.hops, szTemp);
		}
		else
		{
			// 검침 진행중에 End Device가 삭제된 경우
			METERING_LOG("%s [%s] %s Cannot found end device. (Elapse=%d sec)\r\n",
						szGUID, endi.szSerial, endi.szModel, state.nUseTime/1000);
		}
		pJob->bOk = TRUE;
	}
	else
	{
		// 검침 실패
		XDEBUG("\r\n##################################\r\n");
		XDEBUG("Recovery: %s Fail.\r\n", szGUID);
		XDEBUG("##################################\r\n\r\n");

		time(&now);
		m_pEndDeviceList->SetLastMeteringFailTime(&pJob->id, now);

		METERING_LOG("%s [%s] %s Recovery fail.\r\n",
						szGUID, endi.szSerial, endi.szModel);
		pJob->bOk = FALSE;
	}

	m_pEndDeviceList->VerifyEndDevice(&pJob->id);
	XDEBUG("\r\nRecovery: End (ID=%s)\r\n", szGUID);
}

void CRecovery::CheckSkipNode(RECOVERYJOB *pArray, int nCount)
{
    ENDIENTRY	endi;
	char 		szGUID[17];
	int			i;
    time_t      now;

    time(&now);
	for(i=0; i<nCount; i++)
	{ 
		eui64toa(&pArray[i].id, szGUID);
		if (m_pEndDeviceList->CopyEndDeviceByID(&pArray[i].id, &endi))
		{
			if ((endi.tmMetering != 0) && (endi.tmMeteringFail < endi.tmMetering))
			{
				// 이미 검침된 노드인 경우 Skip
				pArray[i].bSkip = TRUE;	
				pArray[i].bOk = TRUE;
			}
			else
			{
				// 검침되지 않은 노드
				XDEBUG("RECOVERY: Select (%s)\r\n", szGUID);
			}
		}
		else
		{
			// 목록에서 삭제된 경우	
			XDEBUG("RECOVERY: Cannot found end device (%s)\r\n", szGUID);
			pArray[i].bSkip = TRUE;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Thread Override functions
//////////////////////////////////////////////////////////////////////

BOOL CRecovery::OnActiveThread()
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

BOOL CRecovery::OnTimeoutThread()
{
	time_t	now;
	struct	tm	when, prev;
	UINT	bit, bit1;

	// Watchdog을 clear한다.
	WatchdogClear();

	// 실패 검침이 Disable이면 검침하지 않는다.
	if (!m_bEnableRecovery)
		return TRUE;

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
        /** Issue #10 : Interval Scheduler 에서는 Recovery가 동작하지 않는다 */
        if(m_nScheduler == SCHEDULER_TYPE_INTERVAL)
        {
            return TRUE;
        }

		time(&now);
		when = *localtime(&now);
		prev = *localtime(&m_nLastStart);

		// 검침일 일자 Mask 검사
		bit = 1 << (when.tm_mday-1);
		if ((m_nRecoveryDayMask & bit) == 0)
			return TRUE;

		// 검침 시간 Mask 검사
		bit = 1 << when.tm_hour;
		if ((m_nRecoveryHourMask & bit) == 0)
			return TRUE;

		// 이전 검침 같은날 같은 시간에는 검침을 다시 하지 않는다.
		if (m_nLastStart != 0 &&(when.tm_mday == prev.tm_mday) && (when.tm_hour == prev.tm_hour))
			return TRUE;

		// 검침 시작 시작분이 경과 했는지 확인
		if (when.tm_min < m_nRecoveryStartMin)
			return TRUE;

		// 검침 시작분 보다 5분이 초과되면 재 부팅해도 다시 검침을 수행하지 않는다.
		if (when.tm_min > (m_nRecoveryStartMin+5))
			return TRUE;

		bit = 1 << (when.tm_mday-1);
		bit1 = 1 << when.tm_hour;

		// Event Log를 읽는 시간이 검사한다.
		if (((m_nEventReadDayMask & bit) > 0) && ((m_nEventReadHourMask & bit1) > 0))
			m_bEventLog = TRUE;

		// 시간 동기화를 하는 시간인지 확인한다.
		if (m_bEnableMeterTimesync && ((m_nTimesyncDayMask & bit) > 0) && ((m_nTimesyncHourMask & bit1) > 0))
			m_bTimesync = TRUE;

		// 정기 검침 옵션을 지정한다.
		m_Locker.Lock();
		m_nOption = RECOVERY_DEFAULT_OPTION;
        m_nOffset = RECOVERY_DEFAULT_OFFSET;
        m_nCount  = RECOVERY_DEFAULT_COUNT;
		m_Locker.Unlock();
	}
	else
	{
		m_bTimesync = FALSE;
		m_bEventLog = FALSE;
	}

	return OnActiveThread();
}

