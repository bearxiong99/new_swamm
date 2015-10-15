#ifndef __BYPASS_QUEUE_H__
#define __BYPASS_QUEUE_H__

#include "TimeoutThread.h"
#include "LinkedList.h"
#include "ScheduleJob.h"
#include "Locker.h"

typedef struct	_tagBITEM
{
    EUI64           id;
    TIMETICK        invokeTime;
    BYTE            nSeq;
    UINT            trSeq;
    BYTE            *szData;
    WORD            nLength;
    BOOL            bSent;
    int             nPosition;
}	BITEM, *PBITEM;

class CBypassQueue : public CScheduleJob, public CTimeoutThread
{
public:
	CBypassQueue();
	virtual ~CBypassQueue();

// Operations
public:
    BOOL    	Initialize();
    void    	Destroy();

public:
	BOOL		Add(EUI64 *pId, BYTE nSeq, BYTE * szData, WORD nLength);
    //UINT        GetQueueCount(void);

protected:
    void        RemoveAll(void);
    void        FreeItem(BITEM *pItem);

	BOOL		BatchWorker(BITEM *pItem);
    BOOL        SingleJob(CZigbeeWorker * pWorker, BITEM *pItem);
    void        FreeJob(void *pThis, void *pWorker, void *pJob);
    static void WorkerJob(void *pThis, void *pWorker, void *pJob);

protected:
	BOOL		OnActiveThread();
	BOOL		OnTimeoutThread();

private:
    UINT        m_nSeq;
	CLocker		m_Locker;
	CLinkedList<BITEM *>    m_List;
};

extern CBypassQueue	*m_pBypassQueue;

#endif	// __BYPASS_QUEUE_H__
