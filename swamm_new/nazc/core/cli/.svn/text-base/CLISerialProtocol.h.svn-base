#ifndef __CLI_SERIAL_PROTOCOL_H__
#define __CLI_SERIAL_PROTOCOL_H__

#include "SerialServer.h"
#include "CLIFiber.h"
#include "CLITerminal.h"

class CCLISerialProtocol : public CCLIFiber, public CSerialServer, public CCLITerminal
{
public:
	CCLISerialProtocol();
	virtual ~CCLISerialProtocol();

public:
	BOOL	Start();
	void	Stop();
	BOOL	Write(CLISESSION *pSession, char *pszBuffer, int nLength);

// Message Handlers
protected:
	BOOL	OnEnterSession(WORKSESSION *pSession);
	void	OnLeaveSession(WORKSESSION *pSession);
	BOOL	OnReceiveSession(WORKSESSION *pSession, char* pszBuffer, int nLength);
	void	OnErrorSession(WORKSESSION *pSession, int nType);
	void	OnCloseSession(WORKSESSION *pSession);
	BOOL	OnTimeoutSession(WORKSESSION *pSession);

	BOOL	WriteChar(CLISESSION *pSession, unsigned char c);
	BOOL	WriteStream(CLISESSION *pSession, const char *pszBuffer, int nLength=-1);
};

extern CCLISerialProtocol	*m_pSerialProtocol;

#endif	// __CLI_SERIAL_PROTOCOL_H__
