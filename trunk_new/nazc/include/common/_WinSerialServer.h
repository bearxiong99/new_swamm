//////////////////////////////////////////////////////////////////////
//
//	SerialComm.h: interface for the CSerialServer class.
//
//	This file is a part of the DAS-NMS project.
//	(c)Copyright 2003 NETCRA Software International All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __WIN_SERIAL_COMM_H__
#define __WIN_SERIAL_COMM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedef.h"
#include "_WinSerialWriter.h"

#define DEFAULT_SIO_BUFFER					4096
#define MAXIMUM_SIO_BUFFER					DEFAULT_SIO_BUFFER*4

class CSerialServer : public CSerialWriter
{
// Construction/Destruction
public:
	CSerialServer();
	virtual ~CSerialServer();

// Attribute
public:
	virtual BOOL IsConnected() const;
	virtual int GetError() const;
	virtual int GetHandle();
	virtual void SetPortParam(DWORD dwParam);
	virtual DWORD GetPortParam() const;
	virtual int SetKeepAliveTime() const;
	virtual void SetKeepAliveTime(int nKeepAliveTime=INFINITE);

// Operations
public:
	virtual	BOOL Connect(char *pszDevice, DWORD dwBaudRate=CBR_115200, int nParity=NOPARITY, int nDataBit=8, int nStopBit=ONESTOPBIT, BOOL bFlowControl=FALSE);
	virtual	void Disconnect();
	virtual BOOL EnableFlowControl(BOOL bEnable);

// Override Functions
protected:
	virtual	void OnError(DWORD dwErrors, LPCOMSTAT *pStatus);

	virtual void OnUpdateModemStatus(DWORD dwModemStatus);
	virtual void OnUpdateCommStatus(DWORD dwErrors, COMSTAT *pComStat);

	virtual	BOOL OnEnterSession(WORKSESSION *pSession);
	virtual	void OnLeaveSession(WORKSESSION *pSession);
	virtual	BOOL OnReceiveSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength);
	virtual	void OnPrevSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength);
	virtual	void OnSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength);
	virtual	void OnErrorSession(WORKSESSION *pSession, int nType);
	virtual	void OnCloseSession(WORKSESSION *pSession);
	virtual	BOOL OnTimeoutSession(WORKSESSION *pSession);

	WORKSESSION *NewSession(HANDLE hFile, LPSTR pszAddress);
	void		DeleteSession(WORKSESSION *pSession);

protected:
	void	OnPrevWrite(LPSTR pszBuffer, int nLength);
	void	OnWrite(LPSTR pszBuffer, int nLength);

// Member Functions
protected:
	virtual BOOL InitializeVariable();
	virtual	void DestroyVariable();
	virtual BOOL UpdateConnection();
	virtual BOOL StartThread();
	virtual void StopThread(int nWaitTimeout=10000);
	virtual	void ErrorHandler(char *pszText);
	virtual	void CheckModemStatus(BOOL bUpdateNow);
	virtual	void CheckComStat(BOOL bUpdateNow);
	virtual	void ReportStatusEvent(DWORD dwStatus);

	virtual	DWORD ReadProc();
	virtual	DWORD SendProc();

// Static Functions
public:
	static	DWORD ReadThread(LPVOID pParam);
	static	DWORD SendThread(LPVOID pParam);

// Member Variable
protected:
	WORKSESSION	*m_pSaveSession;
	HANDLE		m_hComPort;
	BOOL		m_bUseFlowControl;
	BOOL		m_bConnected;
	BOOL		m_bDisconnectPending;
	DWORD		m_dwParam;
	DWORD		m_dwOldModemStatus;
	DWORD		m_dwModemStatus;
	COMSTAT		m_ComStatOld;
	DWORD		m_dwErrorsOld;
    COMSTAT		m_ComStatNew;
    DWORD		m_dwErrors;
	int			m_nKeepAliveTime;
	BOOL		m_bLocalEcho;
	BOOL		m_bCTSOutFlow;
	BOOL		m_bDSROutFlow;
	BOOL		m_bDSRInFlow;
	BOOL		m_bXonXoffOutFlow;
	BOOL		m_bXonXoffInFlow;
	BOOL		m_bTXafterXoffSent;
	BOOL		m_bNoReading;
	BOOL		m_bNoWriting;
	BOOL		m_bNoEvents;
	BOOL		m_bNoStatus;
	BOOL		m_bDisplayTimeouts;
	BOOL		m_bRecvReady;
    char		m_szDevice[65];
	int			m_nByteSize;
	int			m_nParity;
	int			m_nStopBits;
    DWORD		m_dwBaudRate;
    WORD		m_wXONLimit;
	WORD		m_wXOFFLimit;
    DWORD		m_dwRtsControl;
    DWORD		m_dwDtrControl;
    DWORD		m_dwEventFlags;
    char		m_chFlag;
	char		m_chXON;
	char		m_chXOFF;
	int			m_nLastError;
	HANDLE		m_hReadStopEvent;
	HANDLE		m_hReadTerminateEvent;
	HANDLE		m_hSendStopEvent;
	HANDLE		m_hSendTerminateEvent;
	HANDLE		m_hSendEvent;
	HANDLE		m_hReadThread;
	DWORD		m_dwReadThreadID;
	HANDLE		m_hSendThread;
	DWORD		m_dwSendThreadID;
	HANDLE		m_hWriterHeap;
	HANDLE		m_hWriterEvent;
	HANDLE		m_hTransferCompleteEvent;
	DCB			m_saveDCB;
	WRITEREQUEST	*m_pWriterHead;
	WRITEREQUEST	*m_pWriterTail;
    COMMTIMEOUTS	m_timeoutOld;
    COMMTIMEOUTS	m_timeoutNew;
	unsigned char	m_szBuffer[DEFAULT_SIO_BUFFER+1];
};

#endif //	__WIN_SERIAL_COMM_H__
