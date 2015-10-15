#ifndef __SERVICE_CONTROLLER_H__
#define __SERVICE_CONTROLLER_H__

#include "common.h"
#include "IdleApplication.h"
#include "LinkedList.h"
#include "Locker.h"

typedef struct	_tagGWMESSAGE
{
		int			nCommand;
		void		*pParam;
		int			nPosition;
}	GWMESSAGE, *PGWMESSAGE;

class CServiceController : public CIdleApplication
{
// Construction/Destruction
public:
	CServiceController();
	virtual ~CServiceController();

// Operations
public:
	BOOL		PostMessage(int nMessage, void *pParam);
	GWMESSAGE 	*GetMessage();
	BOOL		ExecuteMessage(int nMessage, void *pParam);

// Override Functions
protected:
	BOOL		OnStarted();
	void		OnIdle();
	void		OnShutdown();

protected:
	virtual 	BOOL OnStartService();
	virtual 	void OnStopService();
	virtual 	BOOL OnMessage(int nMessage, void *pParam);

protected:
	CLocker		m_MessageLocker;
	CLinkedList<GWMESSAGE *>	m_MessageList;
};

#endif	// __SERVICE_CONTROLLER_H__
