#include "common.h"
#include "AgentService.h"
#include "BatchJob.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "Utility.h"

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////

#define MAX_BATCHJOB_WORKER             MAX_ZIGBEE_WORKER

CBatchJob   *m_pBatchJob = NULL;

time_t  m_tmLastDiscovery = 0;

extern int		m_nMeteringThreadCount;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBatchJob::CBatchJob() //: CJobObject("BatchJob", 60*60*2)
{
    m_pBatchJob     = this;
}

CBatchJob::~CBatchJob()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CBatchJob::Initialize()
{
    // Worker를 생성한다.
    CScheduleJob::Startup((void *)this, MAX_BATCHJOB_WORKER);
    if (!CTimeoutThread::StartupThread(3))
        return FALSE;
    return TRUE;
}

void CBatchJob::Destroy()
{
    RemoveAll();
    // Worker를 모두 종료한다.
    CScheduleJob::Shutdown();

    // 쓰레드를 종료한다.
    CTimeoutThread::ShutdownThread();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

/** True를 리턴하면 중복이 발견된 것이기 때문에 BatchJob에 추가하지 않는다 */
BOOL CBatchJob::RemoveDuplicateAlarm(BATCHITEM *pItem)
{
#if     defined(__PATCH_8_2305__)
    /** 1) 이미 Fire Alarm type이 같은게 들어 있다면 새롭게 추가하지 않는다.
      * 2) Active를 추가하려고 할 때 Deactivate가 있다면 Deactivate는 삭제된다.
      * 2) Deactivate를 추가하려고 할 때 Activate가 있다면 Activate는 삭제된다.
      */
    BATCHITEM   *pTemp;
    POSITION    pos;
    ENDI_EVENT_PAYLOAD *evt, *tevt;
    if(pItem->nType != BATCH_ALARM_BROADCASTING) return FALSE;

    evt = (ENDI_EVENT_PAYLOAD *) pItem->pParam;

    m_Locker.Lock();
    pTemp = m_List.GetFirst(pos);
    while(pTemp)
    {
        if(pTemp->nType == BATCH_ALARM_BROADCASTING) {
            tevt = (ENDI_EVENT_PAYLOAD *) pTemp->pParam;

            if(evt->type == tevt->type) return TRUE;
            m_List.RemoveAt((POSITION)pItem->nPosition);
            FreeItem(pItem);
            return FALSE;
        }
        pItem = m_List.GetNext(pos);
    }
    m_Locker.Unlock();

#endif
    return FALSE;
}

BOOL CBatchJob::Add(int nType, void *pParam, BOOL bFree, UINT nTimeGap)
{
    BATCHITEM    *pItem;

    pItem = (BATCHITEM *)MALLOC(sizeof(BATCHITEM));
    if (pItem == NULL)
        return FALSE;
    memset(pItem, 0, sizeof(BATCHITEM));
    pItem->nType  = nType;
    pItem->pParam = pParam;    
    pItem->bFree = bFree;    
    pItem->nTimeGap = nTimeGap;    
    pItem->nCreateTime = uptime();

#if     defined(__PATCH_8_2305__)
    if(RemoveDuplicateAlarm(pItem)) {
        FreeItem(pItem);
        return TRUE;
    }
#endif

    m_Locker.Lock();
    pItem->nPosition = (int)m_List.AddTail(pItem);
    m_Locker.Unlock();
    ActiveThread();
    return TRUE;
}

void CBatchJob::RemoveAll(void)
{
    BATCHITEM    *pItem;
    m_Locker.Lock();
    while((pItem = m_List.GetHead())) 
    {
        m_List.RemoveAt((POSITION)pItem->nPosition);
        FreeItem(pItem);
    }
    m_Locker.Unlock();
}

void CBatchJob::RemoveTransaction(WORD nTrID)
{
    BATCHITEM   *pItem;
    POSITION    pos=INITIAL_POSITION;
    m_Locker.Lock();
    pItem = m_List.GetFirst(pos);
    while(pItem)
    {
        if(pItem->nType == BATCH_ASYNC_OPERATION) {
            WORD *nTrId = (WORD *)pItem->pParam;
            if(*nTrId == nTrID) {
                m_List.RemoveAt((POSITION)pItem->nPosition);
                FreeItem(pItem);
                break;
            }
        }
        pItem = m_List.GetNext(pos);
    }
    m_Locker.Unlock();
}

void CBatchJob::RemoveAllTransaction(void)
{
    BATCHITEM   *pItem;
    POSITION    pos;
    m_Locker.Lock();
    pItem = m_List.GetFirst(pos);
    while(pItem)
    {
        if(pItem->nType == BATCH_ASYNC_OPERATION) {
            m_List.RemoveAt((POSITION)pItem->nPosition);
            FreeItem(pItem);
        }
        pItem = m_List.GetNext(pos);
    }
    m_Locker.Unlock();
}

UINT CBatchJob::GetQueueingTransaction(void)
{
    BATCHITEM   *pItem;
    POSITION    pos;
    UINT        trCnt = 0;
    m_Locker.Lock();
    pItem = m_List.GetFirst(pos);
    while(pItem)
    {
        if(pItem->nType == BATCH_ASYNC_OPERATION) {
            trCnt ++;
        }
        pItem = m_List.GetNext(pos);
    }
    m_Locker.Unlock();

    return trCnt;
}

void CBatchJob::FreeItem(BATCHITEM *pItem)
{
    if(pItem)
    {
        if(pItem->bFree) {
            FREE(pItem->pParam);
        }
        FREE(pItem);
    }
}

void CBatchJob::FreeJob(void *pThis, void *pWorker, void *pJob)
{
	CBatchJob	*pSelf = (CBatchJob *)pThis;
    pSelf->FreeItem((BATCHITEM *)pJob);
}

BOOL CBatchJob::SingleJob(CZigbeeWorker * pWorker, BATCHITEM *pItem)
{
    switch(pItem->nType) {
        case BATCH_DISCOVERY_RESPONSE :
            DiscoveryResponse(pItem);
            break;
        case BATCH_ASYNC_OPERATION :
            AsyncOperation(pItem);
#if     defined(__PATCH_8_2305__)
        case BATCH_ALARM_BROADCASTING :
            AlarmBroadcasting(pItem);
#endif
            break;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

void CBatchJob::WorkerJob(void *pThis, void *pWorker, void *pJob)
{
    CBatchJob    *pSelf = (CBatchJob *)pThis;

    pSelf->SingleJob((CZigbeeWorker *)pWorker, (BATCHITEM *)pJob);
}

BOOL CBatchJob::BatchWorker(BATCHITEM *pItem)
{
    int         nMaxWorker = MIN(GetMaxWorker(), m_nMeteringThreadCount);
    BOOL        bOk = FALSE;
    if(GetActiveWorker() < nMaxWorker)
    {
        if(StartWorker(WorkerJob, (void *)pItem))
        {
            bOk = TRUE;
        }    
    }

    return bOk;
}

void CBatchJob::DiscoveryResponse(BATCHITEM *pItem)
{
    HANDLE            codi;
    ENDI_DISCOVERY_PAYLOAD *pDiscovery;
    BYTE            payload[256];
    int                nState;
    time_t          now;
    codi = GetCoordinator();
    if (codi == NULL)
    {
        XDEBUG("Discovery Response: Coordinator not ready. (NULL Coordinator)\r\n");
        return ;
    }

    nState = codiGetState(codi);
    if (nState != CODISTATE_NORMAL)
    {
        XDEBUG("Discovery Response: Coordinator not ready. (nState=%d)\r\n", nState);
        return;
    }

    time(&now);
    if(abs(now - m_tmLastDiscovery) < 10 ) return;

    pDiscovery = (ENDI_DISCOVERY_PAYLOAD *)&payload;
    pDiscovery->cmd_type =  ENDI_CMD_DISCOVERY_RESPONSE;

    usleep(1000000);
    time(&m_tmLastDiscovery);
    codiMulticast(codi, ENDI_DATATYPE_DISCOVERY, (BYTE *)&payload, sizeof(ENDI_DISCOVERY_PAYLOAD));
}

/** 동일한 Id에 대해서 Job이 있을 경우 어떻게 해야 하는지 결정해야 한다 
  * 반복적으로 Job을 실행시킬 경우 Binding Error가 발생하게 된다.
  * 이 문제를 해결하기 위해서는 동일한 Id에 대해서는 Try를 하지 않거나
  * 끝난다음에 Try를 해야 한다.
  */
void CBatchJob::AsyncOperation(BATCHITEM *pItem)
{
    WORD            *nTrId = (WORD *)pItem->pParam;
    /*-- 요청이 들어온 즉시 실행하면 통신이 안되는 경우가 발생한다
     *-- 이 경우는 MIU에서 검침데이터를 올리고 있을 때 Ondemand를 요청한다던지
     *-- 하는 경우가 된다. 이 문제를 방지하기 위해 일단 3sec의 Delay를 설정한다.
     *-- 추후에 보다 근본적인 원인 분석과 해결책을 찾아야 한다.
     --*/
    /** 3초 delay를 nTimeGap을 이용해서 처리한다 
      * usleep(3000000);
      */

    m_pTransactionManager->ExecuteTransaction(*nTrId);
}

void CBatchJob::AlarmBroadcasting(BATCHITEM *pItem)
{
    /** Fire Detect 시 20초 후, deactivate 시 3초 후에 Broadcasting */
#if     defined(__PATCH_8_2305__)
    ENDI_EVENT_PAYLOAD *evt;
    ENDI_DIO_PAYLOAD  *payload;
    BYTE        szBuffer[100];
    int         nError;
    int         i;

    memset(szBuffer, 0, sizeof(szBuffer));
    payload = (ENDI_DIO_PAYLOAD *) &szBuffer;

    evt = (ENDI_EVENT_PAYLOAD *) pItem->pParam;

    payload->type = DIGITAL_OUTPUT_REQUEST;
    payload->mask = 0x01;
    payload->value = evt->status.val.u32 ? 0x00 : 0x01;

    for(i=0;i<2;i++) {
	    nError = codiMulticast(GetCoordinator(), ENDI_DATATYPE_DIO, (BYTE *)&szBuffer, sizeof(ENDI_DIO_PAYLOAD));

	    if (nError != CODIERR_NOERROR)
	    {
	        XDEBUG("---------------------------------------------\r\n");
	        XDEBUG("FIRE ALARM BROADCASTING-ERROR: %s(%d)\r\n", codiErrorMessage(nError), nError);
	        XDEBUG("---------------------------------------------\r\n");
	    }
        usleep(1000000);
    }
#endif
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CBatchJob::OnActiveThread()
{
    BATCHITEM    *pItem;
    int          nNow;
    POSITION    pos = INITIAL_POSITION;
    m_Locker.Lock();
    pItem = m_List.GetFirst(pos);
    for(;!IsThreadExitPending();)
    {
        nNow = uptime();
        if (pItem) {
		
            if(((nNow - pItem->nCreateTime) >= (int)pItem->nTimeGap) && BatchWorker(pItem)) {
                m_List.RemoveAt((POSITION)pItem->nPosition);
            }
        }else break;
	
        usleep(200000);

        pItem = m_List.GetNext(pos);
    }
    m_Locker.Unlock();

    return TRUE;
}

BOOL CBatchJob::OnTimeoutThread()
{
    return OnActiveThread();
}

