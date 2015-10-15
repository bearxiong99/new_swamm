#ifndef __BATCH_JOB_H__
#define __BATCH_JOB_H__

#include "TimeoutThread.h"
#include "LinkedList.h"
#include "ScheduleJob.h"
#include "Locker.h"

#define BATCH_DISCOVERY_RESPONSE        0
#define BATCH_ASYNC_OPERATION           1
#define BATCH_ALARM_BROADCASTING        2
#define BATCH_TABLE_JOIN				3
typedef struct	_tagBATCHITEM
{
		int			nType;				// 작업 구분 (BATCH_XXXX, ..)
		void		*pParam;			// Parameter
        BOOL        bFree;
		UINT		nTimeGap;
		int		    nCreateTime;
		int			nPosition;
}	BATCHITEM, *PBATCHITEM;

class CBatchJob : public CScheduleJob, public CTimeoutThread
{
public:
	CBatchJob();
	virtual ~CBatchJob();

// Operations
public:
    BOOL    	Initialize();
    void    	Destroy();

public:
	BOOL		Add(int nType, void *pParam=NULL, BOOL bFree=TRUE, UINT nTimeGap=0);
	void		RemoveTransaction(WORD nTrID);
	void		RemoveAllTransaction(void);
	void		RemoveAll(void);
    UINT        GetQueueingTransaction(void);

protected:
	BOOL		BatchWorker(BATCHITEM *pItem);
    BOOL        SingleJob(CZigbeeWorker * pWorker, BATCHITEM *pItem);
    static void WorkerJob(void *pThis, void *pWorker, void *pJob);
    void FreeJob(void *pThis, void *pWorker, void *pJob);
    void        FreeItem(BATCHITEM *pItem);
    BOOL        RemoveDuplicateAlarm(BATCHITEM *pItem);
	int			TableRejoin(BATCHITEM *pItem);
	void		DiscoveryResponse(BATCHITEM *pItem);
    void        AsyncOperation(BATCHITEM *pItem);
    void        AlarmBroadcasting(BATCHITEM *pItem);

protected:
	BOOL		OnActiveThread();
	BOOL		OnTimeoutThread();

private:
	CLocker		m_Locker;
	CLinkedList<BATCHITEM *>	m_List;
};

extern CBatchJob	*m_pBatchJob;

#endif	// __BATCH_JOB_H__
