#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#ifndef _WIN32
#include <netdb.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include "Options.h"

//////////////////////////////////////////////////////////////////////
// COptions Class
//////////////////////////////////////////////////////////////////////

COptions::COptions(char *pszFileName)
{
	strcpy(m_szFileName, pszFileName ? pszFileName : "");
	memset(m_szBuffer, 0, MAX_OPTION_BUFFER+1);
}

COptions::~COptions()
{
}

//////////////////////////////////////////////////////////////////////
// COptions Functions
//////////////////////////////////////////////////////////////////////

BOOL COptions::Load()
{
	FILE	*fp;

	fp = fopen(m_szFileName, "rb");
	if (fp == NULL)
		return FALSE;

	fread(m_szBuffer, MAX_OPTION_BUFFER, 1, fp);
	fclose(fp);
	return TRUE;
}

BOOL COptions::Save()
{
	FILE	*fp;

	fp = fopen(m_szFileName, "w+b");
	if (fp == NULL)
		return FALSE;

	fwrite(m_szBuffer, strlen(m_szBuffer), 1, fp);
	fclose(fp);
	return TRUE;
}

BOOL COptions::GetOptionBool(char *pszKey, BOOL bDefault)
{
	char	*p;

	p = GetTag(pszKey);
	if (p != NULL)
		return (strncmp(p, "true", 4)==0) || (strncmp(p, "TRUE", 4)==0) ? TRUE : FALSE;

	return bDefault;
}

int COptions::GetOptionInt(char *pszKey, int nDefault)
{
	char	*p;

	p = GetTag(pszKey);
	if (p != NULL)
		return atoi(p);

	return nDefault;
}

BOOL COptions::GetOptionString(char *pszKey, char *pszValue, char *pszDefault)
{
	char	*p;

	p = GetTag(pszKey);
	if (p != NULL)
	{
		strcpyex(pszValue, p);
		return TRUE;
	}
	strcpy(pszValue, pszDefault);
	return TRUE;
}

BOOL COptions::WriteOptionBool(char *pszKey, BOOL bValue)
{
	char	szValue[10];

	strcpy(szValue, bValue ? "true" : "false");
	Replace(pszKey, szValue);
	return TRUE;
}

int	COptions::WriteOptionInt(char *pszKey, int nValue)
{
	char	szValue[20];

	sprintf(szValue, "%0d", nValue);
	Replace(pszKey, szValue);
	return 0;
}

BOOL COptions::WriteOptionString(char *pszKey, char *pszValue)
{
	Replace(pszKey, pszValue);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// COptions Member Functions
//////////////////////////////////////////////////////////////////////

char *COptions::GetTag(char *pszKey)
{
	char	szTag[40];
	char	*p;

	sprintf(szTag, "%s=", pszKey);
	p = strstr(m_szBuffer, szTag);
	if (p == NULL)
		return NULL;

	p += strlen(szTag);
	return p;
}

void COptions::Replace(char *pszKey, char *pszValue)
{
	char	szTag[40];
	char	*p;

	sprintf(szTag, "%s=", pszKey);
	p = GetTag(pszKey);
	if (p == NULL)
	{
		strcat(m_szBuffer, "\n");
		strcat(m_szBuffer, szTag);
		strcat(m_szBuffer, pszValue);
		strcat(m_szBuffer, "\n");
	}
	else
	{
		strextract(p, pszValue);
	}
}

void COptions::strextract(char *pszDest, char *pszSource)
{
	int		n, n1, n2, n3;
	char	*p1, *p2;

	n  = strlen(pszSource);
	n1 = strlenex(pszDest);
	n2 = n - n1;
	
	if (n2 == 0)
	{
		memcpy(pszDest, pszSource, strlen(pszSource));
	}
	else if (n2 > 0)
	{
		n3 = strlen(pszDest);
		for(p1=pszDest+n3+n2, p2=pszDest+n3; p2>=pszDest; p1--, p2--)
			*p1 = *p2;
		memcpy(pszDest, pszSource, strlen(pszSource));
		*(pszDest+n3+n2+1) = '\0';
	}
	else
	{
		n = -n2;
		for(p1=pszDest, p2=pszDest+n; *p2; p1++, p2++)
			*p1 = *p2;
		*p1 = '\0';
		memcpy(pszDest, pszSource, strlen(pszSource));
	}
}

int COptions::strlenex(char *pszString)
{
	int		nLength = 0;
	
	for(;*pszString; nLength++, pszString++)
	{
		if ((*pszString == 0xd) || (*pszString == 0xa))
			break;
	}
	return nLength;
}

void COptions::strcpyex(char *pszDest, char *pszSource)
{
	for(;*pszSource;)
	{
		if ((*pszSource == 0xd) || (*pszSource == 0xa))
			break;
		
		*pszDest = *pszSource;
		pszDest++;
		pszSource++;
	}
	*pszDest = '\0';
}

