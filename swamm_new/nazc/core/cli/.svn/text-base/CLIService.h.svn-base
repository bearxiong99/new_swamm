#ifndef __CLI_SERVICE_H__
#define __CLI_SERVICE_H__

#include "common.h"
#include "IdleApplication.h"
#include "CLIProtocol.h"
#include "CLISerialProtocol.h"
#include "CLITcpipProtocol.h"
#include "CLIOption.h"
#include "Worker.h"

#define CLI_WORKER_COUNT		3

class CCLIService : public CIdleApplication, public CCLIProtocol, public CCLIOption
{
public:
	CCLIService(CLICONSTRUCT *pConstruct);
	virtual ~CCLIService();

public:
	BOOL	IsExpert();
	int		GetHwType() const;
	int		GetRunLevel() const;
	BOOL	SetExpert(BOOL bEnable);
    UINT    GetAgentPort();

// Override Functions
public:
	BOOL	ParseParameters(int argc, char **argv);
	BOOL	OnStarted();
	void	OnShutdown();

	CWorker *GetWorker();
	CWorker	*Execute(PFNWORKERPROC pfnFunc, void *pThis, CLISESSION *pSession, BOOL bComplete, BOOL bAddHistory);

	BOOL	WaittingForIPC(int nWaitTime);
	BOOL	LoadSetting();
	BOOL	SaveSetting();
	BOOL	DoLogin(CLISESSION *pSession, char *pszID, char *pszPassword);
	
	void	AddLog(char *pszUser, char *pszString);
	BOOL	ShowLog(CLISESSION *pSession, char *pDate);
	BOOL	ClearLog();

public:
	CLICONSTRUCT		*m_pConstruct;

private:
	int					m_nHwType;
	int					m_nRunLevel;
	char				m_szMessage[1024];
    UINT                m_nAgentPort;
	CCLISerialProtocol	m_theSerial;
	CCLITcpipProtocol	m_theTcpip;
	CWorker				m_Worker[CLI_WORKER_COUNT];
	CLISETTING			m_Setting;
	CLocker				m_ConfigLocker;
	CLocker				m_LogLocker;
	CLocker				m_WorkerLocker;
};

extern CCLIService		*m_pCLIService;

#endif	// __CLI_SERVICE_H__
