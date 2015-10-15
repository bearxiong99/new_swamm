#ifndef __CLI_PROTOCOL_H__
#define __CLI_PROTOCOL_H__

#include "LinkedList.h"
#include "Locker.h"

class CCLIProtocol
{
public:
	CCLIProtocol();
	virtual ~CCLIProtocol();

public:
	BOOL	AddProtocol(const char* pszName, int nType, const char* pszDevice, int nPort, int nMaxSession, int nTimeout, void *pFiber);
	void	RemoveProtocol(LPSTR pszName);
	void 	RemoveAllProtocols();
	BOOL	StartProtocol(LPSTR pszName);
	BOOL	StartProtocols();
	void	StopProtocol(LPSTR pszName);
	void	StopProtocols();
	BOOL	OutText(CLISESSION *pSession, char *pszBuffer, int nLength=-1);

private:	
	CLIPROTOCOL *AddEntry(const char* pszName, int nType, const char* pszDevice, int nPort, int nMaxSession, int nTimeout, void *pFiber);
	void	DeleteEntry(CLIPROTOCOL *pProtocol);
	BOOL	StartEntry(CLIPROTOCOL *pProtocol);
	void	StopEntry(CLIPROTOCOL *pProtocol);
	CLIPROTOCOL *FindEntry(const char* pszName);

private:
	CLocker						m_Locker;
	CLinkedList<CLIPROTOCOL *>	m_List;
};

#endif	// __CLI_PROTOCOL_H__
