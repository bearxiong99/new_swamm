#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include "Locker.h"
#include "TimeoutThread.h"
#include "JobObject.h"

typedef struct	_tagWATCHDOGENTRY
{
	CJobObject		*pObject;
	int				nPosition;
}	WATCHDOGENTRY;

class CWatchdog : public CTimeoutThread
{
public:
	CWatchdog();
	virtual ~CWatchdog();

// Operations
public:
    BOOL    Initialize();
    void    Destroy();

	BOOL	Add(CJobObject *pObject);
	void	RemoveAll();

protected:
	BOOL	OnTimeoutThread();
	BOOL	OnActiveThread();

private:
    CLocker		m_Locker;
	CLinkedList<WATCHDOGENTRY *> m_List;
};

extern CWatchdog	*m_pWatchdog;

#endif	// __WATCHDOG_H__
