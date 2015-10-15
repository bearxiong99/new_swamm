//////////////////////////////////////////////////////////////////////
//
//  CLIOption.cpp: implementation of the CCLIOption class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
//  http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "CLIOption.h"
#include "CLIUtility.h"

//////////////////////////////////////////////////////////////////////
// CCLIOption Class
//////////////////////////////////////////////////////////////////////

CCLIOption::CCLIOption()
{
	DefaultOption();	
}

CCLIOption::~CCLIOption()
{
}

//////////////////////////////////////////////////////////////////////
// CCLIOption Functions
//////////////////////////////////////////////////////////////////////

BOOL CCLIOption::LoadOption()
{
	FILE	*fp;
	char	szBuffer[4096];

	fp = fopen(CLISETUP_FILENAME, "rb");
	if (fp == NULL)
	{
		DefaultOption();
		SaveOption();
		return FALSE;
	}

	memset(szBuffer, 0, 4096);
	fread(szBuffer, 4096, 1, fp);
	fclose(fp);

	m_bEnableSerial		= GetOptionBool(szBuffer, "EnableSerial", TRUE);
	m_bEnableTelnet		= GetOptionBool(szBuffer, "EnableTelnet", TRUE);
	m_bEnableLog		= GetOptionBool(szBuffer, "EnableLog", TRUE);
	m_nTelnetPort		= GetOptionInt(szBuffer, "TelnetPort", CLIDEFAULT_TELNET_PORT);
	m_nTelnetMaxSession = GetOptionInt(szBuffer, "TelnetMaxSession", CLIMAX_SESSION_COUNT);
	m_nInitWaitTime		= GetOptionInt(szBuffer, "InitialWaitTime", 10);
	m_nIdleTimeout		= GetOptionInt(szBuffer, "IdleTimeout", CLIDEFAULT_TIMEOUT);
	m_nMaxHistory		= GetOptionInt(szBuffer, "MaxHistory", CLIMAX_HISTORY);
	m_nLogSaveDay		= GetOptionInt(szBuffer, "LogSaveDay", CLIMAX_LOGSAVEDAY);

	GetOptionString(szBuffer, "Prompt", m_szDefaultPrompt);
	GetOptionString(szBuffer, "UserPrompt", m_szUserPrompt);
	GetOptionString(szBuffer, "PasswordPrompt", m_szPasswordPrompt);
	GetOptionString(szBuffer, "DebugPrompt", m_szDebugPrompt);
	GetOptionString(szBuffer, "SerialDevice", m_szSerialDevice);
	GetOptionString(szBuffer, "LogPath", m_szLogPath);

	GetOptionString(szBuffer, "F1", m_szVirtualKey[0]);
	GetOptionString(szBuffer, "F2", m_szVirtualKey[1]);
	GetOptionString(szBuffer, "F3", m_szVirtualKey[2]);
	GetOptionString(szBuffer, "F4", m_szVirtualKey[3]);
	return TRUE;
}

BOOL CCLIOption::SaveOption()
{
	FILE	*fp;

	fp = fopen(CLISETUP_FILENAME, "wb");
	if (fp == NULL)
		return FALSE;

	SetOptionBool(fp, "EnableSerial", m_bEnableSerial);
	SetOptionBool(fp, "EnableTelnet", m_bEnableTelnet);
	SetOptionBool(fp, "EnableLog", m_bEnableLog);

	SetOptionInt(fp, "TelnetPort", m_nTelnetPort);
	SetOptionInt(fp, "TelnetMaxSession", m_nTelnetMaxSession);
	SetOptionInt(fp, "InitialWaitTime", m_nInitWaitTime);
	SetOptionInt(fp, "IdleTimeout", m_nIdleTimeout);
	SetOptionInt(fp, "MaxHistory", m_nMaxHistory);
	SetOptionInt(fp, "LogSaveDay", m_nLogSaveDay);

	SetOptionString(fp, "Prompt", m_szDefaultPrompt);
	SetOptionString(fp, "UserPrompt", m_szUserPrompt);
	SetOptionString(fp, "PasswordPrompt", m_szPasswordPrompt);
	SetOptionString(fp, "DebugPrompt", m_szDebugPrompt);
	SetOptionString(fp, "SerialDevice", m_szSerialDevice);
	SetOptionString(fp, "LogPath", m_szLogPath);

	SetOptionString(fp, "F1", m_szVirtualKey[0]);
	SetOptionString(fp, "F2", m_szVirtualKey[1]);
	SetOptionString(fp, "F3", m_szVirtualKey[2]);
	SetOptionString(fp, "F4", m_szVirtualKey[3]);

	fclose(fp);
	return TRUE;
}

void CCLIOption::DefaultOption()
{
	m_bEnableSerial		= TRUE;
	m_bEnableTelnet		= TRUE;
	m_bEnableLog		= TRUE;
	m_nTelnetPort		= CLIDEFAULT_TELNET_PORT;
	m_nTelnetMaxSession = CLIMAX_SESSION_COUNT;
	m_nInitWaitTime		= 10;
	m_nIdleTimeout		= CLIDEFAULT_TIMEOUT;
	m_nMaxHistory		= CLIMAX_HISTORY;
	m_nLogSaveDay		= CLIMAX_LOGSAVEDAY;

	strcpy(m_szDefaultPrompt, CLIDEFALUT_PROMPT);
	strcpy(m_szUserPrompt, CLIDEFALUT_USER_PROMPT);
	strcpy(m_szPasswordPrompt, CLIDEFALUT_PASSWORD_PROMPT);
	strcpy(m_szDebugPrompt, DEBUG_PROMPT);
	strcpy(m_szSerialDevice, CLIDEFAULT_SERIAL_DEVICE);
	strcpy(m_szLogPath, CLILOG_FILENAME);

	strcpy(m_szVirtualKey[0], "show system");
	strcpy(m_szVirtualKey[1], "show coordinator list");
	strcpy(m_szVirtualKey[2], "show sensor list");
	strcpy(m_szVirtualKey[3], "show option");
}

//////////////////////////////////////////////////////////////////////
// CCLIOption Functions
//////////////////////////////////////////////////////////////////////

BOOL CCLIOption::GetOptionBool(const char *pszBuffer, const char *pszOption, BOOL bDefault)
{
	char	szTag[40];
	char	*p;

	sprintf(szTag, "%s=", pszOption);
	p = (char *)strstr(pszBuffer, szTag);
	if (p == NULL)
		return bDefault;

	p += strlen(szTag);
	return (strncasecmp(p, "true", 4)==0) ? TRUE : FALSE;
}

int CCLIOption::GetOptionInt(const char *pszBuffer, const char *pszOption, int nDefault)
{
	char	szTag[40];
	char	*p;

	sprintf(szTag, "%s=", pszOption);
	p = (char *)strstr(pszBuffer, szTag);
	if (p == NULL)
		return nDefault;

	p += strlen(szTag);
	return atoi(p);
}

void CCLIOption::strcpyex(char *pszDest, char *pszSource)
{
	for(;*pszSource;)
	{
		if (*pszSource == 0xd)
			break;
		if (*pszSource == 0xa)
			break;
		
		*pszDest = *pszSource;
		
		*pszDest = *pszSource;
		pszDest++;
		pszSource++;
	}
	*pszDest = '\0';
}

BOOL CCLIOption::GetOptionString(const char *pszBuffer, const char *pszOption, char *pszValue)
{
	char	szTag[40];
	char	*p;

	sprintf(szTag, "%s=", pszOption);
	p = (char *)strstr(pszBuffer, szTag);
	if (p == NULL)
		return FALSE;

	p += strlen(szTag);
	strcpyex(pszValue, p);
	return TRUE;
}

BOOL CCLIOption::SetOptionBool(FILE *fp, const char *pszOption, BOOL bEnable)
{
	char	szBuffer[255];

	if ((fp == NULL) || (pszOption == NULL))
		return FALSE;

	sprintf(szBuffer, "%s=%s\n", pszOption, bEnable ? "true" : "false");
	fwrite(szBuffer, strlen(szBuffer), 1, fp);
	return TRUE;
}

BOOL CCLIOption::SetOptionInt(FILE *fp, const char *pszOption, int nValue)
{
	char	szBuffer[255];

	if ((fp == NULL) || (pszOption == NULL))
		return FALSE;

	sprintf(szBuffer, "%s=%d\n", pszOption, nValue);
	fwrite(szBuffer, strlen(szBuffer), 1, fp);
	return TRUE;
}

BOOL CCLIOption::SetOptionString(FILE *fp, const char *pszOption, char *pszValue)
{
	char	szBuffer[255];

	if ((fp == NULL) || (pszOption == NULL) || (pszValue == NULL))
		return FALSE;

	sprintf(szBuffer, "%s=%s\n", pszOption, pszValue);
	fwrite(szBuffer, strlen(szBuffer), 1, fp);
	return TRUE;
}

