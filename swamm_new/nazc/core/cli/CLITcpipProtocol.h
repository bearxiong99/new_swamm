#ifndef __CLI_TCPIP_PROTOCOL_H__
#define __CLI_TCPIP_PROTOCOL_H__

#include "TCPMultiplexServer.h"
#include "CLIFiber.h"
#include "CLITerminal.h"

class CCLITcpipProtocol : public CCLIFiber, public CTCPMultiplexServer, public CCLITerminal
{
public:
	CCLITcpipProtocol();
	virtual ~CCLITcpipProtocol();

public:
	BOOL	Start();
	void	Stop();
	BOOL	Write(CLISESSION *pSession, char *pszBuffer, int nLength);

// Message Handlers
protected:
    void 	OnAcceptSession(WORKSESSION *pSession);
    BOOL 	OnEnterSession(WORKSESSION *pSession);
    void 	OnLeaveSession(WORKSESSION *pSession);
    BOOL 	OnReceiveSession(WORKSESSION *pSession, const char* pszBuffer, int nLength);
    void 	OnErrorSession(WORKSESSION *pSession, int nType);
    void 	OnCloseSession(WORKSESSION *pSession);
    BOOL 	OnTimeoutSession(WORKSESSION *pSession);

	BOOL	WriteChar(CLISESSION *pSession, unsigned char c);
	BOOL	WriteStream(CLISESSION *pSession, const char *pszBuffer, int nLength=-1);
};

extern CCLITcpipProtocol	*m_pTcpipProtocol;

#endif	// __CLI_TCPIP_PROTOCOL_H__
