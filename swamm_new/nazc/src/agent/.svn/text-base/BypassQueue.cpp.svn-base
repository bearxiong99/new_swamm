#include "common.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "Utility.h"
#include "BypassQueue.h"

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////

#define MAX_BYPASS_WORKER                   3
#define DEFAULT_TIMEOUT		                3

CBypassQueue   *m_pBypassQueue = NULL;

extern char		m_szFepServer[];
extern int      m_nFepPort;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBypassQueue::CBypassQueue() 
{
    m_pBypassQueue     = this;
    m_nSeq = 0;
}

CBypassQueue::~CBypassQueue()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CBypassQueue::Initialize()
{
    // Worker를 생성한다.
    CScheduleJob::Startup((void *)this, MAX_BYPASS_WORKER);

    if (!CTimeoutThread::StartupThread(3))
        return FALSE;

    return TRUE;
}

void CBypassQueue::Destroy()
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

BOOL CBypassQueue::Add(EUI64 *pId, BYTE nSeq, BYTE *szData, WORD nLength)
{
    BITEM    *pItem;
    struct timeval tv;

    pItem = (BITEM *)MALLOC(sizeof(BITEM));
    if (pItem == NULL)
        return FALSE;

    gettimeofday(&tv, NULL);
    memset(pItem, 0, sizeof(BITEM));
    memcpy(&pItem->id, pId, sizeof(EUI64));
    pItem->nSeq  = nSeq;
    pItem->szData = (BYTE *)MALLOC(nLength);
    pItem->nLength = nLength;    
    memcpy(pItem->szData, szData, nLength); 
    memcpy(&pItem->invokeTime, &tv, sizeof(struct timeval)); 

    m_Locker.Lock();
    pItem->trSeq = ++m_nSeq;
    pItem->nPosition = (int)m_List.AddTail(pItem);
    m_Locker.Unlock();
    ActiveThread();
    return TRUE;
}

void CBypassQueue::RemoveAll(void)
{
    BITEM    *pItem;

    m_Locker.Lock();
    pItem = m_List.GetHead();
    while((pItem = m_List.GetHead())) 
    {
        m_List.RemoveAt((POSITION)pItem->nPosition);
        FreeItem(pItem);
    }
    m_Locker.Unlock();
}

void CBypassQueue::FreeItem(BITEM *pItem)
{
    if(pItem)
    {
        if(pItem->szData) {
            FREE(pItem->szData);
        }
        FREE(pItem);
    }
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CBypassQueue::SingleJob(CZigbeeWorker * pWorker, BITEM *pItem)
{
	TIMETICK  tmNow;
	int		nElapse, nError;
	BYTE	nAttr;

    GetTimeTick(&tmNow);
	nElapse = GetTimeInterval(&pItem->invokeTime, &tmNow);

    // IF4 Forwarding을 한다
	CIF4Invoke  invoke(m_szFepServer, m_nFepPort, DEFAULT_TIMEOUT);
    invoke.AddParamFormat("1.14", VARSMI_EUI64, &pItem->id, sizeof(EUI64));    // Source ID
    invoke.AddParam("1.6", pItem->trSeq);    // Transaction Sequence
    invoke.AddParam("1.4", pItem->nSeq);     // Message Sequence
    invoke.AddParam("1.5", pItem->nLength);  // Message Length
    invoke.AddParamFormat("1.12", VARSMI_STREAM, pItem->szData, pItem->nLength); // Message
#ifdef _WIN32
    invoke.AddParam("1.6", (UINT)pItem->invokeTime.t);    // Invoke Time
#else
    invoke.AddParam("1.6", (UINT)pItem->invokeTime.t.tv_sec);    // Invoke Time
#endif
    invoke.AddParam("1.6", (UINT)nElapse);               // Waiting Delay

	nAttr  = IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE;
	nError = IF4API_Command(invoke.GetHandle(), "111.1", nAttr);

    if(nError == IF4ERR_NOERROR)
    {
        pItem->bSent = TRUE;
    }
    else
    {
        char szGUID[17];
        EUI64ToStr(&pItem->id,szGUID);
        XDEBUG("Bypass Fail %s : %s(%d)\r\n", szGUID, IF4API_GetErrorMessage(nError), nError);
    }

    return TRUE;
}

void CBypassQueue::WorkerJob(void *pThis, void *pWorker, void *pJob)
{
    CBypassQueue    *pSelf = (CBypassQueue *)pThis;

    pSelf->SingleJob((CZigbeeWorker *)pWorker, (BITEM *)pJob);
}

void CBypassQueue::FreeJob(void *pThis, void *pWorker, void *pJob)
{
	CBypassQueue	*pSelf = (CBypassQueue *)pThis;
    BITEM * pItem = (BITEM *)pJob;
    /** 한번 전송에 실패하면 두번 전송하지 않는다 */
    if(pItem)
    {
        pSelf->FreeItem(pItem);
    }
}


BOOL CBypassQueue::BatchWorker(BITEM *pItem)
{
    int         nMaxWorker = MIN(GetMaxWorker(), 10);
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

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CBypassQueue::OnActiveThread()
{
    BITEM    *pItem;
    POSITION    pos = INITIAL_POSITION;
    int      nCnt;
    BOOL     bOk = FALSE;

    m_Locker.Lock();
    pItem = m_List.GetFirst(pos);
    for(;!IsThreadExitPending();)
    {
        bOk = FALSE;
        if (pItem)
        {
            for(nCnt=0;nCnt<3;nCnt++)
            {
                if(BatchWorker(pItem)) 
                {
                    bOk = TRUE;
                    break;
                }
                usleep(5 * 100000);
            }
            m_List.RemoveAt((POSITION)pItem->nPosition);
        }
        if(bOk == FALSE) break;
        pItem = m_List.GetFirst(pos);
    }
    m_Locker.Unlock();

    return TRUE;
}

BOOL CBypassQueue::OnTimeoutThread()
{
    /** 추후에 Timeout이 발생하는 Item에 대한 Remove Code를 추가해야 한다
      */
    return OnActiveThread();
}

