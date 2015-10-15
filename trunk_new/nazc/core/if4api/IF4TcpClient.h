//////////////////////////////////////////////////////////////////////
//
//	IF4TcpClient.h: interface for the CIF4TcpClient class.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@paran.com
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __IF4_TCP_CLIENT_H__
#define __IF4_TCP_CLIENT_H__

#include "TCPClient.h"
#include "IF4Stream.h"

class CIF4TcpClient : public CTCPClient, public CIF4Stream
{
// Construction/Destruction
public:
	CIF4TcpClient();
	virtual ~CIF4TcpClient();

public:
	BOOL	IsHandshake();

public:
	BOOL	TryConnectServer(char *pszAddress, int nPort);
	void	DisconnectServer();

	void	OnConnect();
	void 	OnDisconnect();
	void 	OnConnectError();
	void 	OnSend(LPSTR pszBuffer, int nLength);
	BOOL 	OnReceive(LPSTR pszBuffer, int nLength);
	void 	OnError(int nType);

public:
	void	SendAck(BYTE nSeq);
	void	SendNak(BYTE nSeq);
	void	SendWck(BYTE nStart, BYTE nEnd);
	void	SendEot();
	void	SetInvoke(IF4Invoke *pInvoke);

	int		SendRequest();
	void	SendFrame();
	BOOL	SendMultiFrame();
	void	RetryMultiFrame();
	BOOL	Idle();

public:
	BOOL	OnFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	OnRecvFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	ControlFrame(BYTE *pszBuffer, int nLength);
	BOOL	CommandFrame(BYTE *pszBuffer, int nLength);
	BOOL	AlarmFrame(BYTE *pszBuffer, int nLength);
	BOOL	EventFrame(BYTE *pszBuffer, int nLength);
	BOOL	DataFrame(BYTE *pszBuffer, int nLength);
	BOOL	TransferFrame(BYTE *pszBuffer, int nLength);

protected:
	WORKSESSION	m_thisSession;				// Issue #2036
	DATASTREAM	*m_pStream;
	IF4Invoke	*m_pInvoke;
	BYTE		m_nLastSeq;
	time_t		m_nIdleTime;
	int			m_nWindowSize;
	int			m_nWindowCount;
	int			m_nServerVersion;
	BYTE		m_nStartSEQ;
	BYTE		m_nEndSEQ;
	BYTE		m_nSeqList[MAX_SESSION_WINDOW];
	BOOL		m_nSendFlag[MAX_SESSION_WINDOW];
	int			m_nBaseSeek;
	BOOL		m_bHandshake;
	BOOL		m_bMultiPart;
	BOOL		m_bMultiPartDone;
	BOOL		m_bExit;
};

#endif // __IF4_TCP_CLIENT_H__
