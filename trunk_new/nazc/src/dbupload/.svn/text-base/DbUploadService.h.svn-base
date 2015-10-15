#ifndef __DBUPLOAD_SERVICE_H__
#define __DBUPLOAD_SERVICE_H__

#include "typedef.h"
#include "if4api.h"
#include "rep/DBRepository.h"

class CDbUploadService
{
// Construction/Destruction
public:
    CDbUploadService(int nNzcNumber, char *ipaddr, int nPort, BOOL bDebug);
	virtual ~CDbUploadService();

// Operation
public:
	int     Startup();
	void	Shutdown();

// Override Functions
public:
	BOOL	OnStartService();
	void	OnStopService();
	int     OnExecuteService();
    
protected:
    void    UpdateLog(const IF4Invoke *pInvoke, const char * szType, const int nError);
    int     ProcessInventory(IF4Invoke *pInvoke);
    int     ProcessChannelConfigure(IF4Invoke *pInvoke);
    int     ProcessLoadProfile(IF4Invoke *pInvoke);

// Member Variable
protected:
	int		m_nNzcNumber;
	char	m_szServer[64];
	int		m_nPort;
    BOOL    m_bDebug;

    CDBRepository theRepository;
};

extern CDbUploadService	*m_pService;

#endif	// __DBUPLOAD_SERVICE_H__

