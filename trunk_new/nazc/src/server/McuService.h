#ifndef __MCU_SERVICE_H__
#define __MCU_SERVICE_H__

#include "ServiceController.h"

class CMcuService : public CServiceController
{
// Construction/Destruction
public:
	CMcuService();
	virtual ~CMcuService();

// Override Functions
public:
	BOOL	OnStartService();
	void	OnStopService();
	BOOL	OnMessage(int nMessage, void *pParam);

// Member Functions
protected:
	void	LoadVariable();
	void	ImmediatelyShutdown();
};

extern CMcuService	*m_pService;

#endif	// __MCU_SERVICE_H__

