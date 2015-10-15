#ifndef __CLI_TERMINAL_H__
#define __CLI_TERMINAL_H__

class CCLITerminal 
{
public:
	CCLITerminal();
	virtual ~CCLITerminal();

public:
	virtual CLISESSION *NewTerminalSession(void *pSession, int nType);
	virtual void DeleteTerminalSession(CLISESSION *pSession);
	virtual BOOL SetupTerminal(CLISESSION *pSession);
	virtual void RestartTerminalSession(CLISESSION *pSession);

	virtual BOOL InputStream(CLISESSION *pSession, const unsigned char *pszBuffer, int nLength);
	virtual BOOL WriteChar(CLISESSION *pSession, unsigned char c);
	virtual BOOL WriteStream(CLISESSION *pSession, const char *pszBuffer, int nLength=-1);

	virtual BOOL LocalDoOption(CLISESSION *pSession, int nOption, BOOL bEnable, BOOL bRemote);
	virtual BOOL RemoteDoOption(CLISESSION *pSession, int nOption, BOOL bEnable, BOOL bRemote);

	virtual int FindVirtualKey(CLISESSION *pSession);
	virtual void DisplaySplash(CLISESSION *pSession);
	virtual void DisplayPrompt(CLISESSION *pSession);

	virtual BOOL IsPrintable(unsigned char c);

	virtual BOOL SpliteParameter(CLISESSION *pSession);
	virtual CLIHANDLER *FindCommandHandler(CLISESSION *pSession, int &nDepts, char *pszNode);
	virtual CLIHANDLER *FindCommand(CLISESSION *pSession, CLIHANDLER *pList, int &nDepts, char *pszNode);
	virtual BOOL CheckUser(CLISESSION *pSession);
	virtual BOOL CheckLogin(CLISESSION *pSession);
	virtual void FirmwareDownload(CLISESSION *pSession);
	virtual BOOL ExecuteCommand(CLISESSION *pSession, BOOL bComplete=FALSE, BOOL bAddHistory=TRUE);
	virtual BOOL CommandTask(CLISESSION *pSession, BOOL bComplete=FALSE, BOOL bAddHistory=TRUE);
	virtual void AddHistory(CLISESSION *pSession, char *pszCommand);
	virtual void ClearHistory(CLISESSION *pSession);
	virtual BOOL ExecuteBackspace(CLISESSION *pSession);
	virtual BOOL ExecuteDelete(CLISESSION *pSession);
	virtual int UntilCopy(char *pszDest, char *pszSrc, int nMax);
	virtual CLIPARAMCHECKER *FindParamHandler(char *pszName);
	virtual int GetParamCount(CLIHANDLER *pHandler);
	virtual int GetMinParamCount(CLIHANDLER *pHandler);
	virtual int ValidateParameter(CLISESSION *pSession, CLIHANDLER *pHandler);
	virtual int DisplayAllParameter(CLISESSION *pSession, CLIHANDLER *pHandler, int nIndex=0);
	virtual int DisplayWideParameter(CLISESSION *pSession, CLIHANDLER *pHandler);

	virtual int CheckCommandSyntax(CLISESSION *pSession, CLIHANDLER *pList, int &nDepts, CLIHANDLER **pHandler, CLIHANDLER **pLast);
	virtual int FindBestMatchCommand(CLISESSION *pSession, CLIHANDLER **pHandler, BOOL bComment);
	virtual BOOL ExecuteInlineHelp(CLISESSION *pSession);
	virtual BOOL ExecuteTab(CLISESSION *pSession);
	virtual void ReplaceCommand(CLISESSION *pSession, const char *pszCommand);
	virtual void DisplayUserMenu(CLISESSION *pSession);
	virtual BOOL ExecuteVirtualKey(CLISESSION *pSession, int nKey);

public:
	static	void TaskExecute(void *pHandle, CLISESSION *pSession, BOOL bComplete, BOOL bAddHistory);

protected:
	virtual void SetOption(CLISESSION *pSession, int nOption, BOOL bEnable);
};

#endif	// __CLI_FIBER_H__
