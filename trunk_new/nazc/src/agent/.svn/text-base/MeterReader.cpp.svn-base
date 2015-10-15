#include "common.h"
#include "AgentService.h"
#include "MeterReader.h"
#include "Recovery.h"
#include "Polling.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MobileUtility.h"
#include "MemoryDebug.h"
#include "Utility.h"
#include "AgentLog.h"

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////

#define MTRFILENAME		"/tmp/data/meterreader"
#define MAX_METERREADER_WORKER  5

CMeterReader   	*m_pMeterReader = NULL;

extern int		m_nMeteringThreadCount;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeterReader::CMeterReader()
{
	m_pMeterReader 	= this;
	m_bInitialized	= FALSE;
}

CMeterReader::~CMeterReader()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CMeterReader::Initialize()
{
	// Worker를 생성한다.
	CScheduleJob::Startup((void *)this, MAX_METERREADER_WORKER);
	
	// 쓰레드를 시작 시킨다.
	if (!CTimeoutThread::StartupThread(13))
        return FALSE;

	m_bInitialized = TRUE;
	return TRUE;
}

void CMeterReader::Destroy()
{
	if (!m_bInitialized)
		return;
	
	// Worker를 모두 종료한다.
	CScheduleJob::Shutdown();

	// 쓰레드를 종료한다.
	CTimeoutThread::ShutdownThread();
	m_bInitialized = FALSE;
}

int CMeterReader::Metering(EUI64 * id, int nOption, int nOffset, int nCount)
{
    METERITEM * item;
    char        szId[16+1];

	if (!m_bInitialized)
		return IF4ERR_BUSY;

    item = (METERITEM *)MALLOC(sizeof(METERITEM));
    if (item == NULL)
		return IF4ERR_MEMORY_ERROR;

    memset(item, 0, sizeof(METERITEM));
    memcpy(&item->id, id, sizeof(EUI64));
    item->nOption = nOption;
    item->nOffset = nOffset;
    item->nCount  = nCount;

    eui64toa(&item->id, szId);

    XDEBUG("Recovery Metering : ID=%s, Option=%d, Offset=%d, Count=%d\r\n",
				szId, nOption, nOffset, nCount);

	m_Locker.Lock();
    item->nPosition = m_List.AddTail(item);
	m_Locker.Unlock();

	ActiveThread();
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

void CMeterReader::MeterReaderWorker()
{
	int			nIndex;
    int         nState;
	long		sleeptime=1000000;
	long		magic=1;
    
    METERITEM * pItem;
    METERINGJOB	 *pJob = NULL;

    nState = codiGetState(GetCoordinator());
    if (nState != CODISTATE_NORMAL)
	{
        XDEBUG("METERING: Coordinator is not ready\r\n");
        return;
    }

	while(TRUE)
	{
		usleep(sleeptime/magic); magic = 1;

		m_Locker.Lock();
       	pItem = m_List.GetHead();
   		if (pItem) m_List.RemoveAt((POSITION)pItem->nPosition);
		m_Locker.Unlock();

		if (pItem == NULL)
			break;

		magic = 10;

		for(;(GetActiveWorker() > 0) || (pItem != NULL);)
		{
			if ((GetActiveWorker() < m_nMeteringThreadCount) && (pItem != NULL))
			{
				nIndex = GetActiveWorker();
				if (nIndex != -1)
				{
					pJob = (METERINGJOB *)MALLOC(sizeof(METERINGJOB));
					if (pJob != NULL)
					{
						memset(pJob, 0, sizeof(METERINGJOB));
    	        		memcpy(&pJob->id, &pItem->id, sizeof(EUI64));
       	  		   		pJob->nOption = pItem->nOption;
       	    	 		pJob->nOffset = pItem->nOffset;
            			pJob->nCount  = pItem->nCount;
						FREE(pItem);

					    if (StartWorker(WorkerJob, (void *)pJob))
						{

						    m_Locker.Lock();
        				    pItem = m_List.GetHead();
       					    if (pItem) m_List.RemoveAt((POSITION)pItem->nPosition);
						    m_Locker.Unlock();
						}
                    }
				}
			}

            usleep(100000);
		}
	}
}

void CMeterReader::WorkerJob(void *pThis, void *pWorker, void *pJob)
{
	CMeterReader	*pSelf = (CMeterReader *)pThis;

	pSelf->SingleMetering((CZigbeeWorker *)pWorker, (METERINGJOB *)pJob);

	FREE(pJob);
}

void CMeterReader::SingleMetering(CZigbeeWorker *pWorker, METERINGJOB *pJob)
{
    char            fileName[64];
	METER_STAT		state;
	CMDPARAM		param;
	CChunk			chunk(1024);
    ENDIENTRY       endi;
	char			szGUID[20];
	int				nError;
    time_t          now;

	eui64toa(&pJob->id, szGUID);
	if (!m_pEndDeviceList->CopyEndDeviceByID(&pJob->id, &endi))
	{
		XDEBUG("METERREADER-ERROR: Cannot find EUID=%s\r\n", szGUID);
		return;
	}
	
	XDEBUG("\r\nMeterReader: Start (ID=%s, Option=%d, Offset=%d, Count=%d, Parser=%s)\r\n", 
        	szGUID, pJob->nOption, pJob->nOffset, pJob->nCount, endi.szParser);

	memset(&state, 0, sizeof(METER_STAT));
	memset(&param, 0, sizeof(CMDPARAM));
	memcpy(&param.id, &pJob->id, sizeof(EUI64));
    param.codi 		= GetCoordinator();
    param.nMeteringType = METERING_TYPE_SPECIAL;
	//param.nType		= PARSE_TYPE_READ;
	param.nOption	= pJob->nOption;
	param.nOffset	= pJob->nOffset;
	param.nCount	= pJob->nCount;
	param.pChunk	= &chunk;

	nError = m_pOndemander->Command(endi.szParser, &param, &state);
	if ((nError == ONDEMAND_ERROR_OK) && (chunk.GetSize() > 0))
	{
		XDEBUG("\r\n##################################\r\n");
		XDEBUG("MeterReader: %s, Length=%d\r\n", szGUID, chunk.GetSize());
		XDEBUG("##################################\r\n\r\n");

		// 검침 완료 시간
        time(&now);

        GetMtrFileName(&pJob->id, fileName);
        unlink(fileName);
		// 검침 데이터를 저장한다.
		WriteData(fileName, &pJob->id, chunk.GetBuffer(), chunk.GetSize());

		// 로그를 저장한다.
		m_pEndDeviceList->CopyEndDeviceByID(&pJob->id, &endi);
		METERING_LOG("%s [%s] %s Metering ok. (Elapse=%d sec)\r\n",
					szGUID, endi.szSerial, endi.szModel, state.nUseTime/1000);
		pJob->bOk = TRUE;

        XDEBUG("Recovery Metering : Data Uploading...  ");
	    UploadFile(fileName);
        XDEBUG("[Done]\r\n");
	    unlink(fileName);
	}
	else
	{
		// 검침 실패
		XDEBUG("\r\n##################################\r\n");
		XDEBUG("MeterReader: %s Fail.\r\n", szGUID);
		XDEBUG("##################################\r\n\r\n");

		// 검침 실패 로그 저장
		METERING_LOG("%s [%s] %s Metering fail.\r\n",
						szGUID, endi.szSerial, endi.szModel);
	}

	m_pEndDeviceList->VerifyEndDevice(&pJob->id);
	XDEBUG("\r\nMeterReader: End (ID=%s)\r\n", szGUID);
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CMeterReader::OnActiveThread()
{
    if (IsBusyCoreService()) return TRUE;

	SetBusy(TRUE);
	MeterReaderWorker();
	SetBusy(FALSE);
	
	return TRUE;
}

BOOL CMeterReader::OnTimeoutThread()
{
	if (m_List.GetCount() == 0)
		return TRUE;

	return OnActiveThread();
}

void CMeterReader::GetMtrFileName(EUI64 * id, char * fileName)
{
    char szId[16+1];

    eui64toa(id, szId);
    sprintf(fileName,"%s-%s.dat", MTRFILENAME, szId);
}
