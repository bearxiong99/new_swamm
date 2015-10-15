#ifndef __IF4_TCP_SERVICE_H__
#define __IF4_TCP_SERVICE_H__

#include "TCPMultiplexServer.h"
#include "IF4Fiber.h"
#include "IF4Stream.h"
#include "IF4Worker.h"
#include "Locker.h"

#define MAX_SVC_WORKER_COUNT		10

class CIF4TcpService : public CIF4Fiber, CTCPMultiplexServer, public CIF4Stream
{
public:
	CIF4TcpService();
	virtual ~CIF4TcpService();

public:
	BOOL	GetSafeClient(WORKSESSION *pSession);

public:
	BOOL	Start(int nPort, int nMaxSession);
	void	Stop();

	void	Broadcast(char *pszBuffer, int nLength);
	BOOL	WriteChar(WORKSESSION *pSession, unsigned char c);
	BOOL	WriteStream(WORKSESSION *pSession, const char *pszBuffer, int nLength);

// Message Handlers
protected:
    void 	OnAcceptSession(WORKSESSION *pSession);
    BOOL 	OnEnterSession(WORKSESSION *pSession);
    void 	OnLeaveSession(WORKSESSION *pSession);
    void 	OnSendSession(WORKSESSION *pSession, const char* pszBuffer, int nLength);
    BOOL 	OnReceiveSession(WORKSESSION *pSession, const char* pszBuffer, int nLength);
    void 	OnErrorSession(WORKSESSION *pSession, int nType);
    void 	OnCloseSession(WORKSESSION *pSession);
    BOOL 	OnTimeoutSession(WORKSESSION *pSession);

public:
	void	HelloMessage(WORKSESSION *pSession, DATASTREAM *pStream);
	void	SendRedirection(WORKSESSION *pSession, char *pszAddress, int nPort);
	void	SendRetry(WORKSESSION *pSession, int nDelay);
	void	SendAck(WORKSESSION *pSession, BYTE nSeq);
	void	SendNak(WORKSESSION *pSession, BYTE nSeq);
	void	SendNakWindow(WORKSESSION *pSession, BYTE *pList, BYTE nCount);
	void	SendCommandError(WORKSESSION *pSession, DATASTREAM *pStream, OID3 *oid, BYTE tid, BYTE nError);
	void	SendCommandResult(WORKSESSION *pSession, DATASTREAM *pStream, OID3 *oid, BYTE tid, BYTE nError,
								MIBValue *pLink, int nCount, BOOL bCompress=TRUE);

	void	InitWindow(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	void	CloseWindow(WORKSESSION *pSession, DATASTREAM *pStream);
	void	NextWindow(WORKSESSION *pSession, DATASTREAM *pStream, BYTE nextSEQ);
	void	SetWindow(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	int 	IsAcceptWindow(WORKSESSION *pSession, DATASTREAM *pStream, BYTE nSEQ);
	void	SendResultToClient(WORKSESSION *pSession, BYTE *pFrame, int nLength);

protected:
	BOOL	OnFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	ParseFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	void	OnDownloadAck(WORKSESSION *pSession, DATASTREAM *pStream, BYTE seq);
	BOOL	ControlFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	CommandFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	AlarmFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	EventFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	DataFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	TypeRFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	DataFileFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	TransferFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	BOOL	ExecuteCommand(IF4_COMMAND_TABLE *pCommand, WORKSESSION *pSession, DATASTREAM *pStream, OID3 *cmd, BYTE tid, BYTE attr, MIBValue *pValues, WORD nCount);

protected:
	CIF4Worker		m_Worker[MAX_SVC_WORKER_COUNT];
	CLocker			m_WorkerLocker;
};

#endif	// __IF4_SERVICE_H__
