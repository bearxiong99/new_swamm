#ifndef __METERING_H__
#define __METERING_H__

#include "Locker.h"
#include "TimeoutThread.h"
#include "ScheduleJob.h"
#include "JobObject.h"
#include "MeteringService.h"

typedef struct	_tagMETERINGJOB
{
		EUI64		id;					// Sensor id
		int			nOption;			// Reading Option
		int			nOffset;			// Reading Offset
		int			nCount;				// Reading Count
		int			nResult;			// Result code
		BOOL		bTimesync;			// Timesync flag
		BOOL		bEventLog;			// Event Log flag
		BOOL		bSkip;				// Skip flag
		BOOL		bOk;				// Ok flag
		BOOL		bComplete;			// Completed flag
		void 		*cbData;			// Callback data
		METER_STAT	stat;				
}	METERINGJOB;

class CMetering : public CScheduleJob, public CTimeoutThread, public CJobObject, public CMeteringService
{
public:
	CMetering();
	virtual ~CMetering();

// Operations
public:
    BOOL   	Initialize();
    void   	Destroy();
	int		Query(int nOption, int nOffset, int nCount);

// Metering procedure
protected:
	void	MeteringWorker();
	void	SingleMetering(CZigbeeWorker *pWorker, METERINGJOB *pJob);

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
	time_t	m_nLastMetering;
	time_t	m_nLastStart;
	int		m_nOption;
	int		m_nOffset;
	int		m_nCount;
	BOOL	m_bTimesync;
	BOOL	m_bEventLog;

	CLocker	m_Locker;
};

extern CMetering	*m_pMetering;

#endif	// __METERING_H__
