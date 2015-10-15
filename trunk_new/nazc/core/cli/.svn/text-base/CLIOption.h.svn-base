#ifndef __CLI_OPTION_H__
#define __CLI_OPTION_H__

class CCLIOption
{
public:
	CCLIOption();
	virtual ~CCLIOption();

// Override Functions
public:
	void	DefaultOption();

	BOOL	LoadOption();
	BOOL	SaveOption();

protected:
	BOOL	GetOptionBool(const char *pszBuffer, const char *pszOption, BOOL bDefault);
	int		GetOptionInt(const char *pszBuffer, const char *pszOption, int nDefault);
	BOOL	GetOptionString(const char *pszBuffer, const char *pszOption, char *pszValue);

	BOOL	SetOptionBool(FILE *fp, const char *pszOption, BOOL bEnable);
	BOOL	SetOptionInt(FILE *fp, const char *pszOption, int nValue);
	BOOL	SetOptionString(FILE *fp, const char *pszOption, char *pszValue);

private:
	void	strcpyex(char *pszDest, char *pszSource);

public:
	BOOL	m_bEnableSerial;
	BOOL	m_bEnableTelnet;
	BOOL	m_bEnableLog;
	int		m_nTelnetPort;
	int		m_nTelnetMaxSession;
	int		m_nInitWaitTime;
	int		m_nIdleTimeout;
	int		m_nMaxHistory;
	int		m_nLogSaveDay;

	char	m_szDefaultPrompt[16];
	char	m_szUserPrompt[16];
	char	m_szPasswordPrompt[16];
	char	m_szDebugPrompt[16];
	char	m_szSerialDevice[32];
	char	m_szLogPath[80];	
	char	m_szVirtualKey[4][80];
};

#endif	// __CLI_OPTION_H__
