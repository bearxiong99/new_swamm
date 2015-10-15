#ifndef __POWER_OUTAGE_H__
#define __POWER_OUTAGE_H__

#include "TimeoutThread.h"
#include "Locker.h"
#include "Chunk.h"

#define MAX_OUTAGE_COUNT			2000
#define MAX_OUTAGE_EVENT			4

typedef struct	_tagOUTAGEITEM
{
		GMTTIMESTAMP	gmt;					// 이벤트 최초 발생 시간
		CChunk			*pChunk;				// EUI64 Chunk
		int				nCount;					// 갯수
		BOOL			bActive;				// Active 여부
		time_t			tmLastActive;			// 최초 발생 시간
}	OUTAGEITEM, *POUTAGEITEM;

class CPowerOutage : public CTimeoutThread
{
public:
	CPowerOutage();
	virtual ~CPowerOutage();

// Attribute
public:
	BOOL	IsActiveTimer(BYTE type);
	void	SetActiveTimer(BYTE type, BOOL bActive);

// Operations
public:
    BOOL    Initialize();
    void    Destroy();
	BOOL	Add(EUI64 *id, GMTTIMESTAMP *gmt, BYTE type, BYTE *status);

// Functions
protected:
	BOOL	AddTimeoutItem(EUI64 *id, GMTTIMESTAMP *gmt, BYTE type);

// Member functions
protected:
	BOOL	OnTimeoutThread();
	BOOL	OnActiveThread();

private:
    CLocker		m_ItemLocker;
	BOOL		m_bActiveOutage;
	OUTAGEITEM	m_arOutageList[MAX_OUTAGE_EVENT];
};

extern CPowerOutage	*m_pPowerOutage;

#endif	// __POWER_OUTAGE_H__
