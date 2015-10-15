#ifndef __RECOVERY_H__
#define __RECOVERY_H__

#include "Locker.h"
#include "TimeoutThread.h"
#include "ScheduleJob.h"
#include "JobObject.h"
#include "MeteringService.h"

#define RECOVERYFILENAME			"/tmp/data/recovery.dat"

#define MAX_RECOVERY_WORKER			5

#define RECOVERY_DEFAULT_OPTION		-1
#define RECOVERY_DEFAULT_OFFSET		-1
#define RECOVERY_DEFAULT_COUNT		-1

typedef struct	_tagRECOVERYJOB
{
		EUI64		id;					// Sensor id
		int			nOption;			// Reading Option
		int			nOffset;			// Reading Offset
		int			nCount;				// Reading Count
		int			nResult;			// Result code
		BOOL		bTimesync;			// Timesync
		BOOL		bEventLog;			// Event log
		BOOL		bSkip;				// Skip flag
		BOOL		bOk;				// Ok flag
		BOOL		bRecovery;			// Recovery node
		void 		*cbData;			// Callback data
		METER_STAT	stat;				
}	RECOVERYJOB;

class CRecovery : public CScheduleJob, public CTimeoutThread, public CJobObject, public CMeteringService
{
public:
	CRecovery();
	virtual ~CRecovery();

// Operations
public:
    BOOL   	Initialize();
    void   	Destroy();
	int		Recovery(int nOption=RECOVERY_DEFAULT_OPTION, int nOffset=RECOVERY_DEFAULT_OFFSET, int nCount=RECOVERY_DEFAULT_COUNT);

// Metering procedure
protected:
	void	MeteringWorker();
	void	SingleMetering(CZigbeeWorker *pWorker, RECOVERYJOB *pJob);
	void	CheckSkipNode(RECOVERYJOB *pArray, int nCount);

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
	time_t	m_nLastRecovery;
	time_t	m_nLastStart;
	int		m_nOption;
	int		m_nOffset;
	int		m_nCount;
	BOOL	m_bTimesync;
	BOOL	m_bEventLog;

	CLocker	m_Locker;
};

extern CRecovery	*m_pRecovery;

#endif	// __RECOVERY_H__
