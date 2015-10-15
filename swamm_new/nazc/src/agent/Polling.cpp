#include "common.h"
#include "codiapi.h"
#include "AgentService.h"
#include "Polling.h"
#include "Metering.h"
#include "Recovery.h"
#include "Distributor.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "EndDeviceList.h"
#include "MobileUtility.h"
#include "Utility.h"

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////

#define MAX_POLLING_WORKER			5

CPolling		*m_pPolling = NULL;

extern BOOL		m_bPingTestMode;
extern BOOL		m_bEnableMeterCheck;
extern BOOL		m_bEnableMeterTimesync;
extern int      m_nMeteringThreadCount;

extern UINT		m_nCheckDayMask;
extern UINT		m_nCheckHourMask;
extern BYTE		m_nCheckStartMin;
extern UINT		m_nCheckRetry;
extern UINT		m_nCheckPeriod;

extern UINT		m_nTimesyncDayMask;
extern UINT		m_nTimesyncHourMask;
extern BOOL		m_bGprsPerfTestMode;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolling::CPolling()
{
	m_pPolling 		= this;
	m_nPollingType	= 0;
	m_nOption		= 0;
	m_nLastPolling	= 0;
	m_nLastStart	= 0;
	m_bInitialized	= FALSE;
}

CPolling::~CPolling()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CPolling::Initialize()
{
	// Worker를 생성한다.
	CScheduleJob::Startup((void *)this, MAX_POLLING_WORKER);
	
	// 쓰레드를 시작 시킨다.
	if (!CTimeoutThread::StartupThread(3))
        return FALSE;

	m_bInitialized = TRUE;
	return TRUE;
}

void CPolling::Destroy()
{
	if (!m_bInitialized)
		return;
	
	// Worker를 모두 종료한다.
	CScheduleJob::Shutdown();

	// 쓰레드를 종료한다.
	CTimeoutThread::ShutdownThread();

	m_bInitialized = FALSE;
}

int CPolling::Polling(int nType)
{
	// 초기화 되지 않은 경우
	if (!m_bInitialized)
		return IF4ERR_BUSY;

    if (IsBusyCoreService()) return IF4ERR_BUSY;

	// 옵션을 저장한다.
	m_Locker.Lock();
	switch(nType) {
	  case POLLING_TYPE_ALL : 		m_nOption = ONDEMAND_OPTION_TIME_SYNC; break;
      case POLLING_TYPE_CHECK : 	m_nOption = ONDEMAND_OPTION_READ_IDENT_ONLY; break;
      case POLLING_TYPE_TIMESYNC :	m_nOption = ONDEMAND_OPTION_TIME_SYNC; break;
    }
	SetUserRequest(TRUE);
	m_Locker.Unlock();

	// 쓰레드가 Active되게 한다.
	ActiveThread();
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Polling procedure
//////////////////////////////////////////////////////////////////////

void CPolling::PollingWorker()
{
	POLLINGJOB	*pArray;
	TIMESTAMP	tmNow;
	EUI64		*pList;
	int			i, nCount = 0, nOkCount=0;
	int			nThreadCount, nRetryCount, nPeriod;
	int			nRetry, nSeek=0, nSuccess=0;
	int			nTotal = 0;
	int			nTotalSend=0, nTotalRecv=0;
	int			nTotalError=0, nTotalLost=0;
    int         nState;
	time_t		tmStartup;
	time_t		tmStart, now;
	
    nState = codiGetState(GetCoordinator());
    if (nState != CODISTATE_NORMAL)
	{
        XDEBUG("POLLING: Coordinator is not ready\r\n");
        return;
    }

	time(&m_nLastStart);
	GetTimestamp(&tmNow, NULL);

	XDEBUG("------------- %s startup -------------\r\n",
			m_nOption == ONDEMAND_OPTION_TIME_SYNC ? "Timesync" : "Ping");
	XDEBUG("%s TIME = %04d/%02d/%02d %02d:%02d:%02d\r\n",
			m_nOption == ONDEMAND_OPTION_TIME_SYNC ? "TIMESYNC" : "PING",
			tmNow.year, tmNow.mon, tmNow.day,
			tmNow.hour, tmNow.min, tmNow.sec);

	if (m_bPingTestMode)
	{
		printf("----------------------------------------------------------\r\n");
		printf("%04d/%02d/%02d %02d:%02d:%02d Ping start.\r\n",
			tmNow.year, tmNow.mon, tmNow.day,
			tmNow.hour, tmNow.min, tmNow.sec);
		printf("----------------------------------------------------------\r\n");
		printf("\r\n");
	}
	
	// Select Line power node
	pList = m_pEndDeviceList->GetListByID(&nCount, 
            ENDDEVICE_ATTR_POLL,         // Polling Node 선택
            0x00);                       // 
	if ((pList == NULL) || (nCount <= 0))
	{
		XDEBUG("POLLING: No line power node.\r\n");
		return;
	}

	pArray = (POLLINGJOB *)MALLOC(sizeof(POLLINGJOB)*nCount);
	if (pArray == NULL)
	{
		XDEBUG("POLLING: Memory allocation fail.\r\n");
		FREE(pList);
		return;
	}

	memset(pArray, 0, sizeof(POLLINGJOB)*nCount);
	for(i=0; i<nCount; i++)
	{
		memcpy(&pArray[i].id, &pList[i], sizeof(EUI64));
		pArray[i].nOption = m_nOption;
	}
	FREE(pList);

	// 최소값 범위를 검사한다.
	nThreadCount = MAX(m_nMeteringThreadCount, 1);
	nRetryCount = MAX(m_nCheckRetry, 1);
	nPeriod = MAX(m_nCheckPeriod, 1);

	if (m_bPingTestMode)
	{
		nThreadCount = 1;
		nRetryCount = 1;
		nPeriod = 60;
	}

	time(&tmStartup);
	for(nRetry=0; nRetry<(int)nRetryCount; nRetry++)
	{
		time(&tmStart);

		// 유효 시간을 초과하면 중지한다.
		if ((tmStart-tmStartup) > (int)(nPeriod*60))
			break;

		// Ping Test 모드는 재시도를 하지 않는다.
	 	if (m_bPingTestMode && (nRetry > 0))
			break;

		XDEBUG("\r\n");
		XDEBUG("************************************************\r\n");
		XDEBUG(" Start (Try=%d, Total=%d)\r\n", nRetry+1, nCount); 
		XDEBUG("************************************************\r\n");
		XDEBUG("\r\n");

		nSeek = 0;
		ResetWorker();
		for(;(GetActiveWorker() > 0) || (nSeek < nCount);)
		{
			// Check complete node
			if (pArray[nSeek].bOk || pArray[nSeek].bSkip)
			{
				nSeek++;
				continue;
			}

			// Check remain node?	
			if ((nSeek < nCount) && (GetActiveWorker() < nThreadCount))
			{
				if (StartWorker(WorkerJob, (void *)&pArray[nSeek]))
				{   
					nTotal++;
					nSeek++;
					continue;
				}
			}

			usleep(100000);
		}

		time(&now);
		XDEBUG("\r\n");
		XDEBUG("************************************************************\r\n");
		XDEBUG(" End (Try=%d, Total=%d, Elapse=%d seconds)\r\n", nRetry+1, nCount, now-tmStart); 
		XDEBUG("************************************************************\r\n");
		XDEBUG("\r\n");

		// Check all done?
		if (nOkCount >= nCount)
			break;
	}

	time(&now);
	for(i=0; i<nCount; i++)
	{
		if (pArray[i].bOk && !pArray[i].bSkip)
			nSuccess++;

		nTotalSend  += pArray[i].stat.nSendCount;
		nTotalRecv  += pArray[i].stat.nRecvCount;
		nTotalError += pArray[i].stat.nError;
		nTotalLost  += (pArray[i].stat.nSendCount - pArray[i].stat.nRecvCount);
	}

	XDEBUG("------------- %s result -------------\r\n",
			m_nOption == ONDEMAND_OPTION_TIME_SYNC ? "Timesync" : "Ping");
	XDEBUG(" Total sensor        = %d\r\n", nCount);
	XDEBUG(" Total try count     = %d\r\n", nTotal);
	XDEBUG(" Total success count = %d\r\n", nSuccess);
	XDEBUG(" Total elapse        = %d min %d seconds\r\n",
				(int)(now-tmStartup) / 60,
				(int)(now-tmStartup) % 60);
	XDEBUG("----------------------------------------\r\n");


	if (m_bPingTestMode)
	{
		printf("\r\n");
		printf("----------------------- PING SUMMARY ---------------------\r\n");
		printf("Total ping meters ....................................... %d\r\n", nCount);
		printf("Total number of send packets ............................ %d packet(s)\r\n", nTotalSend);
		printf("Total number of received packets ........................ %d packet(s)\r\n", nTotalRecv);
		printf("Total number of error packets ........................... %d packet(s)\r\n", nTotalError);
		printf("Total number of lost packets ............................ %d packet(s)\r\n", nTotalLost);
		printf("Total ping elapse ....................................... %d min %d seconds\r\n",
				(int)(now-tmStartup) / 60,
				(int)(now-tmStartup) % 60);
		printf("Overall Success rate = received packets/sent packets .... %d %%\r\n",
				(nTotalSend > 0) ? (nTotalRecv*100) / nTotalSend : 0);
		GetTimestamp(&tmNow, NULL);
		printf("----------------------------------------------------------\r\n");
		printf("%04d/%02d/%02d %02d:%02d:%02d Ping end.\r\n",
			tmNow.year, tmNow.mon, tmNow.day,
			tmNow.hour, tmNow.min, tmNow.sec);
		printf("----------------------------------------------------------\r\n\r\n");
	}

	FREE(pArray);
	time(&m_nLastPolling);
}

void CPolling::WorkerJob(void *pThis, void *pWorker, void *pJob)
{
	CPolling	*pSelf = (CPolling *)pThis;

	pSelf->SinglePolling((CZigbeeWorker *)pWorker, (POLLINGJOB *)pJob);
}

void CPolling::SinglePolling(CZigbeeWorker *pWorker, POLLINGJOB *pJob)
{
	METER_STAT		state;
	CMDPARAM		param;
	CChunk			chunk(1024);
	TIMESTAMP		tmNow;
    ENDIENTRY     	endi;
	char			szGUID[20];
	int				nError, nElapse;

	eui64toa(&pJob->id, szGUID);
	XDEBUG("\r\nPolling: Start (ID=%s, Option=%d, Offset=%d, Count=%d)\r\n", 
            szGUID, pJob->nOption, pJob->nOffset, pJob->nCount);

	if (!m_pEndDeviceList->CopyEndDeviceByID(&pJob->id, &endi))
	{
		XDEBUG("POLLING-ERROR: Cannot find EUID=%s\r\n", szGUID);
		return;
	}

	if (endi.nState == ENDISTATE_INIT)
	{
		XDEBUG("METERING-ERROR: Not ready (ENDISTATE_INIT) EUID=%s\r\n", szGUID);
		return;
	}
	
	if (m_bPingTestMode)
	{
		GetTimestamp(&tmNow, NULL);
		printf("%04d/%02d/%02d %02d:%02d:%02d PING: ID=%s\r\n",
				tmNow.year, tmNow.mon, tmNow.day,
				tmNow.hour, tmNow.min, tmNow.sec, szGUID);
		printf("\r\n");
	}

	memset(&state, 0, sizeof(METER_STAT));
	memset(&param, 0, sizeof(CMDPARAM));
	memcpy(&param.id, &pJob->id, sizeof(EUI64));
	param.codi = GetCoordinator();
	param.nMeteringType = METERING_TYPE_ROUTINE;

	if (pJob->nOption == ONDEMAND_OPTION_TIME_SYNC)
	{
		param.nType  = PARSE_TYPE_TIMESYNC;
		param.nOption = ONDEMAND_OPTION_TIME_SYNC;
	}
	else
	{
		param.nType	 = PARSE_TYPE_READ;
		param.nOption = ONDEMAND_OPTION_READ_IDENT_ONLY;
	}
	param.pChunk	= &chunk;

	nError = m_pOndemander->Command(endi.szParser, &param, &state);
	if ((nError == ONDEMAND_ERROR_OK) && (chunk.GetSize() > 0))
	{
		XDEBUG("\r\n##################################\r\n");
		XDEBUG(" %s: %s, Length=%d\r\n", param.nType == PARSE_TYPE_TIMESYNC ? "Timesync" : "Ping", szGUID, chunk.GetSize());
		XDEBUG("##################################\r\n\r\n");
		pJob->bOk = TRUE;
	}

	if (m_bPingTestMode)
	{
		memcpy(&pJob->stat, &state, sizeof(METER_STAT));
		nElapse = (state.nCount == 0) ? 0 : (state.nResponseTime / state.nCount);
		if (pJob->bOk)
		{
			if (!m_pEndDeviceList->CopyEndDeviceByID(&pJob->id, &endi))
			{
				printf("\r\n");
				printf("EUI64 ID     = %s\r\n", szGUID);
				printf("Model        = %s\r\n", endi.szModel);
				printf("MFG. Serial  = %s\r\n", endi.szSerial);
				printf("\r\n");
				printf("Total number of send packets ..................... %d packet(s)\r\n",
						state.nSendCount);
				printf("Number of received packets ....................... %d packet(s)\r\n",
						state.nRecvCount);
				printf("Number of error packets .......................... %d packet(s)\r\n",
						state.nError);
				printf("Number of lost packets ........................... %d packet(s)\r\n",
						state.nSendCount-(state.nRecvCount+state.nError));
				printf("Reading elapse ................................... %d ms\r\n",
						state.nUseTime);
				printf("Success rate = received packets/sent packets ..... %d %%\r\n",
						state.nRecvCount*100 / state.nSendCount);
			}
		}
		else
		{
			printf("PING: Result = Fail.\r\n");
		}
		printf("\r\n");
	}

	XDEBUG("\r\nPolling: End (ID=%s)\r\n", szGUID);
}

//////////////////////////////////////////////////////////////////////
// Thread Override functions
//////////////////////////////////////////////////////////////////////

BOOL CPolling::OnActiveThread()
{
    if (IsBusyCoreService()) return IF4ERR_BUSY;
	
	SetBusy(TRUE);
	PollingWorker();
	SetBusy(FALSE);

	// 초기 옵션으로 환원한다.
	m_Locker.Lock();
	SetUserRequest(FALSE);
	m_Locker.Unlock();
	return TRUE;
}

BOOL CPolling::OnTimeoutThread()
{
	time_t	now;
	struct	tm	when, prev;
	UINT	bit;

	// Ping 테스트 모드
	if (m_bPingTestMode)
		return OnActiveThread();

	// 폴링이나 시간 동기화가 설정되지 않으면 수행하지 않는다.
	if (!m_bEnableMeterCheck)
		return TRUE;

	// GPRS 성능 테스트중에는 수행하지 않는다.
	if (m_bGprsPerfTestMode)
		return TRUE;

	// 수동으로 시작한 경우는 확인하지 않는다.
	if (!IsUserRequest())
	{
		time(&now);
		when = *localtime(&now);
		prev = *localtime(&m_nLastStart);

		// 같은날 같은 시간에는 수행하지 않는다.
		if ((when.tm_mday == prev.tm_mday) && (when.tm_hour == prev.tm_hour))
			return TRUE;

		// 날자 마스크가 선택되었는지 확인한다.
		bit = 1 << (when.tm_mday-1);
		if ((m_nCheckDayMask & bit) == 0)
			return TRUE;

		// 시간 마스크가 되었는지 확인한다.
		bit = 1 << when.tm_hour;
		if ((m_nCheckHourMask & bit) == 0)
			return TRUE;

		// 시작 분이 되었는지 확인한다.
        if (when.tm_min < m_nCheckStartMin)
            return TRUE;

		// 시작 분을 5분이상 초과하면 진행하지 않는다.
        if (when.tm_min > (m_nCheckStartMin+5))
            return TRUE;

		m_Locker.Lock();
		m_nPollingType = POLLING_TYPE_CHECK;
		m_nOption = ONDEMAND_OPTION_READ_IDENT_ONLY;
		m_Locker.Unlock();
	}

	return OnActiveThread();
}

