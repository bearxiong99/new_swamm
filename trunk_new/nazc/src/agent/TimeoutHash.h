#ifndef __TIMEOUT_HASH_H__
#define __TIMEOUT_HASH_H__

#include "TimeoutThread.h"
#include "Locker.h"

typedef struct	_tagHASHITEM
{
		char	*pszKey;
		time_t	tmHash;
}	HASHITEM, *PHASHITEM;

class CTimeoutHash : public CTimeoutThread
{
public:
	CTimeoutHash();
	virtual ~CTimeoutHash();

// Operations
public:
    BOOL    	Initialize();
    void    	Destroy();

public:
	BOOL		Add(const char *pszKey);
	void		RemoveAll();

protected:
	void		StoreIt(HASHITEM *pItem);
	int			FindItem(const char *pszKey);

protected:
	BOOL		OnActiveThread();
	BOOL		OnTimeoutThread();

private:
	CLocker		m_Locker;
	int			m_nMaximum;
	int			m_nHashTimeout;
	HASHITEM	**m_pHashList;
};

extern CTimeoutHash	*m_pTimeoutHash;

#endif	// __TIMEOUT_HASH_H__
