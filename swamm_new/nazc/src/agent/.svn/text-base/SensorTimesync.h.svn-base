#ifndef __SENSOR_TIMESYNC_H__
#define __SENSOR_TIMESYNC_H__

#include "TimeoutThread.h"
#include "Locker.h"

class CSensorTimesync : public CTimeoutThread
{
public:
	CSensorTimesync();
	virtual ~CSensorTimesync();

public:
	BOOL	Initialize();
	void	Destroy();
    void    SyncDiscovery();
	void	Timesync(BYTE nTimeSyncType=TIMESYNC_TYPE_LONG);
    void    NeedBroadcastTime(void);

protected:
	void 	TimesyncSensor(HANDLE codi, BOOL bDirect, BYTE nTimeSyncType);
	void 	MtorSensor(HANDLE codi);
    int     RenewNetwork(BYTE nSec);
    BOOL    IsNeedBroadcastTime(void);
    void    BroadcastTime(HANDLE codi);
    BOOL    IsUserRequest(void);

protected:
	BOOL 	OnActiveThread();
	BOOL 	OnTimeoutThread();

protected:
	cetime_t    m_tmLastMtor;
	cetime_t    m_tmLastTimesync;
    cetime_t    m_nNeedBroadcastTime;
	BOOL		m_bDirect;
	BOOL		m_bMtorReq;
	BOOL		m_bTrActive;
    BOOL        m_bDiscovery;
    BYTE        m_nTimeSyncType;

	CLocker     m_TimeLocker;
};

extern CSensorTimesync		*m_pSensorTimesync;

#endif	// __SENSOR_TIMESYNC_H__
