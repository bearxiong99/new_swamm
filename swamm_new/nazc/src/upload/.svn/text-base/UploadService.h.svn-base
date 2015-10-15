#ifndef __UPLOAD_SERVICE_H__
#define __UPLOAD_SERVICE_H__

class CUploadService
{
// Construction/Destruction
public:
    CUploadService(int nNzcNumber, char *ipaddr, int nPort, char *fileName, BOOL bDebug);
	virtual ~CUploadService();

// Operation
public:
	BOOL	Startup();
	void	Shutdown();

// Override Functions
public:
	BOOL	OnStartService();
	void	OnStopService();
	BOOL	OnExecuteService();

// Member Variable
protected:
	int		m_nNzcNumber;
	char	m_szServer[64];
	int		m_nPort;
	char	m_szFileName[512];
    BOOL    m_bDebug;
};

extern CUploadService	*m_pService;

#endif	// __UPLOAD_SERVICE_H__

