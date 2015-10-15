#ifndef __PING_CLIENT_H__
#define __PING_CLIENT_H__

#include "TCPClient.h"
#include "Locker.h"

class CPingClient : public CTCPClient
{
public:
	CPingClient();
	virtual ~CPingClient();

public:
	BOOL	Ping(char *pszAddress, int nPort, int nTimeout, int nRetry);

protected:
    BOOL	OnReceive(LPSTR pszBuffer, int nLength);

protected:
	BOOL	m_bReplied;
	HANDLE	m_hEvent;
};

#endif	// __PING_CLIENT_H__
