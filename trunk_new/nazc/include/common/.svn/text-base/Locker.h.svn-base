#ifndef __LOCKER_H__
#define __LOCKER_H__

#include "typedef.h"
#ifndef _WIN32
    #include <pthread.h>
#else
    #if _MSC_VER > 1000
    #pragma once
    #endif // _MSC_VER > 1000
#endif


class	CLocker
{
public:
	CLocker();
	virtual ~CLocker();

public:
#ifndef _WIN32
	int		Lock();
	void	Unlock();
#else
	virtual DWORD Lock(int nTimeout=INFINITE);
	virtual void Unlock();
#endif

private:
#ifndef _WIN32
	pthread_mutex_t	m_mutex;
#else
	HANDLE		m_hMutex;
#endif
};

#endif	// __LOCKER_H__

