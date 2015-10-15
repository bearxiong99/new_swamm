//////////////////////////////////////////////////////////////////////
//
//	SerialServer.h: interface for the CSerialServer class.
//
//	This file is a part of the AIMIR-GCP.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __UNIX_SERIAL_SERVER_H__
#define __UNIX_SERIAL_SERVER_H__

#include <pthread.h>
#ifndef _WIN32
#include <termios.h>
#else
#include <semaphore.h>
#endif
#include "typedef.h"
#include "LinkedList.h"
#include "Locker.h"

#define NONE_PARITY			0
#define EVEN_PARITY			1
#define ODD_PARITY			2

class CSerialServer
{
// Construction/Destruction
public:
	CSerialServer();
	virtual	~CSerialServer();

// Attribute
public:
	virtual BOOL IsCloseWait() const;
	virtual	BOOL IsStarted() const;
	virtual	BOOL IsSession();
	virtual	int GetMaxSession();
	virtual	int GetSessionCount();
	virtual void DisableSendFail(BOOL bDisable=TRUE);
	virtual int	GetHandle();
	virtual int GetTimeout() const;
	virtual void SetTimeout(int nTimeout=-1);
	virtual BOOL IsEmptyStream();
	virtual int GetSendQSize();
	BOOL 	GetPassiveMode();

// Operations
public:
	virtual	BOOL Startup(const char *pszDevice, int nMaxSession, int nTimeout=-1, int nSpeed=-1, int nParity=NONE_PARITY, int nOptions=0);
	virtual	void Shutdown();

	void	RestoreSerialSetting();
	int 	AddSendStream(WORKSESSION *pSession, const char *pszBuffer, int nLength);
	int		DirectSendToSession(WORKSESSION *pSession, const char *pszBuffer, int nLength=-1);
	void 	SetPassiveMode(BOOL bPassive);
	BOOL	EnableFlowControl(BOOL bEnable);
	BOOL	DisableEcho();

	virtual int	SendToSession(WORKSESSION *pSession, const char *pszBuffer, int nLength=-1);
	virtual int	SendToSession(WORKSESSION *pSession, char c);

// Member Functions
protected:
	BOOL 	CreateDaemonDevice(const char *pszDevice, int nMaxSession, int nSpeed, int nParity, int nOptions);
	void 	CloseDaemonDevice();
	WORKSESSION *NewSession(int fd, LPSTR pszAddress);
	void 	DeleteSession(WORKSESSION *pSession);
	void 	DeleteAllSession();
	void 	RemoveAllStream(WORKSESSION *pSession);
	int 	SendStreamToSession(int fd, const char *pszBuffer, int nLength);
	void 	SessionError(WORKSESSION *pSession, int nType);

// Override Functions
public:
	virtual	BOOL EnterDaemon();
	virtual	void LeaveDaemon();
	virtual	BOOL RunDaemon();
	virtual	BOOL SessionSender();

protected:
	virtual	BOOL EnterSession(WORKSESSION *pSession);
	virtual	void LeaveSession(WORKSESSION *pSession);
	virtual	WORKSESSION *FindSession(int fd);
	virtual BOOL AddSessionRecvStream(WORKSESSION *pSession, char *pszBuffer, int nLength);
	virtual	SIODATASTREAM *GetStream();
	virtual int WriteData(int fd, char *pszBuffer, int nLength);

// Message Handlers
protected:
	virtual	BOOL OnEnterDaemon();
	virtual	void OnLeaveDaemon();

	virtual	BOOL OnEnterSession(WORKSESSION *pSession);
	virtual	void OnLeaveSession(WORKSESSION *pSession);
	virtual	BOOL OnReceiveSession(WORKSESSION *pSession, char* pszBuffer, int nLength);
	virtual	void OnPrevSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength);
	virtual	void OnSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength);
	virtual	void OnErrorSession(WORKSESSION *pSession, int nType);
	virtual	void OnCloseSession(WORKSESSION *pSession);
	virtual	BOOL OnTimeoutSession(WORKSESSION *pSession);

// Work Thread
public:
	static	void *WatchThread(void *pParam);
	static	void *SendThread(void *pParam);

// Member Variable
private:
	BOOL							m_bWatchExit;
	BOOL							m_bDisableSendFail;
	BOOL							m_bStarted;
	BOOL							m_bExitPending;
	BOOL							m_bSendReady;
	BOOL							m_bRecvReady;
	BOOL							m_bPassiveMode;
	int								m_nMaxSession;
	int								m_nFD;
	int								m_nTimeout;
	char							m_szDevice[64];
#ifndef _WIN32
	struct	termios					m_OldTIO;
#endif
	pthread_t						m_watchThreadID;
	pthread_t						m_sendThreadID;
	sem_t							m_sendSem;

protected:
	WORKSESSION						*m_pSaveSession;

public:
	CLocker							m_SessionLocker;
	CLocker							m_StreamLocker;
	CLinkedList<WORKSESSION *>		m_SessionList;
	CLinkedList<TCPDATASTREAM *>	m_StreamList;
};

#endif	// __SERIAL_SERVER_H__
