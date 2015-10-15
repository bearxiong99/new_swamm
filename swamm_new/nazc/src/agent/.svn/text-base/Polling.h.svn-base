#ifndef __POLLING_H__
#define __POLLING_H__

#include "Locker.h"
#include "TimeoutThread.h"
#include "ScheduleJob.h"

#define POLLING_TYPE_ALL			0
#define POLLING_TYPE_CHECK			1
#define POLLING_TYPE_TIMESYNC		2

typedef struct	_tagPOLLINGJOB
{
		EUI64		id;					// Sensor id
		int			nOption;			// Reading Option
		int			nOffset;			// Reading Offset
		int			nCount;				// Reading Count
		int			nResult;			// Result code
		BOOL		bSkip;				// Skip flag
		BOOL		bOk;				// Ok flag
		void 		*cbData;			// Callback data
		METER_STAT	stat;				
}	POLLINGJOB;

class CPolling : public CScheduleJob, public CTimeoutThread
{
public:
	CPolling();
	virtual ~CPolling();

// Operations
public:
    BOOL   	Initialize();
    void   	Destroy();
	int		Polling(int nType=POLLING_TYPE_ALL);

// Polling procedure
protected:
	void	PollingWorker();
	void	SinglePolling(CZigbeeWorker *pWorker, POLLINGJOB *pJob);

// Static function
public:
	static	void WorkerJob(void *pThis, void *pWorker, void *pJob);

// Thread override functions
protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

// Member variables
private:
	BOOL	m_bInitialized;
	time_t	m_nLastPolling;
	time_t	m_nLastStart;
	int		m_nPollingType;
	int		m_nOption;

	CLocker	m_Locker;
};

extern CPolling	*m_pPolling;

#endif	// __POLLING_H__
