#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include "Hash.h"
#include "JobObject.h"
#include "TimeoutThread.h"
#include "LinkedList.h"
#include "Locker.h"
#include "Variable.h"

typedef struct	_tagEVENTITEM
{
		int			nClass;				// Event Class
		CIF4Invoke	*pInvoke;			// Invoke Class
		char		szOid[32];
		BYTE		nType;
		BYTE		srcID[8];
		TIMESTAMP	tmEvent;
		time_t		tmSave;						
		int			nPosition;			// Event Position
} __attribute__ ((packed)) EVENTITEM, *PEVENTITEM;


class CEventManager : public CTimeoutThread, public CJobObject
{
public:
	CEventManager();
	virtual ~CEventManager();

public:
	BOOL	Initialize();
	void	Destroy();
	void	RemoveAll();

	BOOL	IsIdle();
	BOOL	AddEvent(int nClass, CIF4Invoke *pInvoke, const char *pszEvent, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime);

public:
	void	ChangeLocalAddress(EVENTITEM *pItem);
	BOOL	SendEvent(EVENTITEM *pItem);
    BOOL    WriteEvent(EVENTITEM *pItem);
	BOOL	AppendEventList(EVENTITEM *pItem);
	void	SendEventByMobile();
	void	ReduceLog(const char *pszDirectory, const char *pszType, int nDays);
	void	ReduceEventAndAlarm();
    BOOL    UpdateEventConfigure(const char * szOid, BOOL bUseDefaultClass, int nClass, int nNotify);
    int     GetEventConfigure(EVTCFGENTRY ** ppEvent);

protected:
    BOOL    LoadEventConfigure(const char *pszPath);
    BOOL    SaveEventConfigure(EVTCFGENTRY *pConfigure);
    EVTCFGENTRY * SearchEventConfigure(const char * eventName);

// Thread Override Functions
protected:
	BOOL 	OnActiveThread();
	BOOL 	OnTimeoutThread();

protected:
	CLinkedList<EVENTITEM *>	m_List;
	CLocker						m_Locker;
	time_t						m_tmEvent;
	time_t						m_nLastReduce;
	BOOL						m_bIdle;
	time_t						m_tmCircuit;
	char						*m_pszEventBuffer;
	int 						m_nEventCount;
};

extern CEventManager			*m_pEventManager;

#define NOTIFY_EVENT			m_pEventManager->AddEvent

#endif	// __EVENT_MANAGER_H__
