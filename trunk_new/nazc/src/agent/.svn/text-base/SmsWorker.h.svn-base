#if 0
#ifndef __SMS_WORKER_H__
#define __SMS_WORKER_H__

#if defined(__TI_AM335X__)

#include "Queue.h"
#include "Locker.h"
#include "TimeoutThread.h"

class CSmsWorker : public CTimeoutThread
{
public:
	CSmsWorker();
	virtual ~CSmsWorker();

// Operations
public:
    BOOL   	Initialize();
    void   	Destroy();

	BOOL    AddMessage(char *szMessage, int nLength);

protected:
    int     ReadLine(char *szBuffer, int nLength);
    void    ProcessMessage(const char *sender, const char *date, const char *time, const char *message);
    void    EscapeMessage(const char *payload, int payloadLen, char *escapedPayload);

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

private:
	CLocker	m_Locker;
	CQueue  *m_pQueue;
    char    m_szBuffer[1024];
    int     m_nBufferOffset;
    BOOL    m_bInitialize;
    BOOL    m_bSmsNotification;
};

extern CSmsWorker *m_pSmsWorker;

#endif  // __TI_AM335X__

#endif	// __SMS_WORKER_H__
#endif
