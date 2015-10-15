#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include "typedef.h"
#define MAX_OPTION_BUFFER			4096

class COptions
{
public:
	COptions(char *pszFileName);
	virtual ~COptions();

// Override Functions
public:
	BOOL	Load();
	BOOL	Save();

	BOOL	GetOptionBool(char *pszKey, BOOL bDefault);
	int		GetOptionInt(char *pszKey, int nDefault);
	BOOL	GetOptionString(char *pszKey, char *pszValue, char *pszDefault);
	BOOL	WriteOptionBool(char *pszKey, BOOL bDefault);
	int		WriteOptionInt(char *pszKey, int nDefault);
	BOOL	WriteOptionString(char *pszKey, char *pszValue);

protected:
	char	*GetTag(char *pszKey);
	void	Replace(char *pszKey, char *pszValue);
	void	strextract(char *pszDest, char *pszSource);
	int		strlenex(char *pszString);
	void	strcpyex(char *pszDest, char *pszSource);

protected:
	char	m_szFileName[_MAX_PATH+1];
	int		m_nLength;
	char	m_szBuffer[MAX_OPTION_BUFFER+1];
};

#endif	// __OPTIONS_H__
