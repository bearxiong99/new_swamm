#ifndef __METERREADER_H__
#define __METERREADER_H__

#include "Locker.h"
#include "TimeoutThread.h"
#include "ScheduleJob.h"
#include "MeteringService.h"

typedef struct _tagMETERITEM {
    EUI64   id;
    int     nOption;
    int     nOffset;
    int     nCount;
    POSITION    nPosition;
}METERITEM, *PMETERITEM;

class CMeterReader : public CScheduleJob, public CTimeoutThread, public CMeteringService
{
public:
	CMeterReader();
	virtual ~CMeterReader();

// Operations
public:
    BOOL   	Initialize();
    void   	Destroy();

	int		Metering(EUI64 *id, int nOption, int nOffset, int nCount);
	
protected:
	void	MeterReaderWorker();
	void	SingleMetering(CZigbeeWorker *pWorker, METERINGJOB *pJob);
    void    GetMtrFileName(EUI64 *id, char * fileName);

// Static function
public:
	static	void WorkerJob(void *pThis, void *pWorker, void *pJob);


protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

private:
	CLocker	m_Locker;

	BOOL	m_bInitialized;
	CLinkedList<METERITEM *> m_List;	
};

extern CMeterReader	*m_pMeterReader;

#endif	// __METERING_H__
