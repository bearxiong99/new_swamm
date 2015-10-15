//////////////////////////////////////////////////////////////////////
//
//  CLITerminal.cpp: implementation of the CCLITerminal class.
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

#ifndef _WIN32
#include <sys/ioctl.h>
#include <arpa/telnet.h>
#include <unistd.h>
#include <termio.h>
#include <wait.h>
#endif
#include "common.h"
#include "copyright.h"
#include "TerminalKeyMap.h"
#include "CLITerminal.h"
#include "CLIService.h"
#include "CLIUtility.h"
#include "MemoryDebug.h"
#include "ShellCommand.h"

extern  CLIHANDLER  *m_pCommandHandlers;
extern  CLIHANDLER  m_CliCommandHandlers[];
extern  CLIHANDLER  m_CliDebugCommandHandlers[];
extern  CLIPARAMCHECKER m_CliParameters[];
extern	char *m_pMcuVersionString;

#if     defined(__PATCH_6_2084__)
BOOL    m_bEnableHiddenCommand = TRUE;
#endif

//////////////////////////////////////////////////////////////////////
// CCLITerminal Data Definitions
//////////////////////////////////////////////////////////////////////

#define TS_DATA					0		// base state
#define TS_IAC					1		// look for double IAC's
#define TS_CR					2		// CR-LF ->'s CR
#define TS_SB					3		// throw away begin's...
#define TS_SE					4		// end's (suboption negotiation)
#define TS_WILL					5		// will option negotiation
#define TS_WONT					6		// wont
#define TS_DO					7		// do
#define TS_DONT					8		// dont
#define TS_ESC					10		// ESC 
#define TS_ESCSEQ				11		// ESC [

#define CLI_STATE_WHITE				0
#define CLI_STATE_TEXT				1
#define CLI_STATE_OPEN				2

unsigned char doopt[] 		= 	{ IAC, DO,   '%', 'c', 0 };
unsigned char dont[] 		= 	{ IAC, DONT, '%', 'c', 0 };
unsigned char will[] 		= 	{ IAC, WILL, '%', 'c', 0 };
unsigned char wont[] 		= 	{ IAC, WONT, '%', 'c', 0 };
unsigned char aoreply[]		= 	{ IAC, DM, 0 };
unsigned char aytreply[]	=	{ "\r\n[Yes]\r\n" };

unsigned char kludge_telopt[] = 
{
		IAC, DO, TELOPT_TTYPE, 		\
		IAC, DO, TELOPT_TSPEED,    	\
		IAC, DO, TELOPT_SGA,   		\
		IAC, DO, TELOPT_XDISPLOC,  	\
		IAC, DO, TELOPT_NAWS,  		\
		IAC, DO, TELOPT_LFLOW, 		\
		IAC, DO, TELOPT_LINEMODE,  	\
		IAC, WILL, TELOPT_STATUS,  	\
		IAC, DO, TELOPT_ECHO,  		\
		IAC, WILL, TELOPT_ECHO 
};

CLIKEYMAP	m_arKeyMap[]	= 
{
		{ "[A",			VKEY_UP	},
		{ "[B",			VKEY_DOWN },
		{ "[C",			VKEY_RIGHT },
		{ "[D",			VKEY_LEFT },
		{ "[P",			VKEY_BREAK },
		{ "[1~",		VKEY_HOME },
		{ "[2~",		VKEY_INS },
		{ "[4~",		VKEY_END },
		{ "[5~",		VKEY_PGUP },
		{ "[6~",		VKEY_PGDN },
		{ "OP",			VKEY_F1 },
		{ "OQ",			VKEY_F2 },
		{ "OR",			VKEY_F3 },
		{ "OS",			VKEY_F4 },
		{ "[15~",		VKEY_F5 },
		{ "[17~",		VKEY_F6 },
		{ "[18~",		VKEY_F7 },
		{ "[19~",		VKEY_F8 },
		{ "[20~",		VKEY_F9 },
		{ "[21~",		VKEY_F10 },
		{ "[23~",		VKEY_F11 },
		{ "[24~",		VKEY_F12 },
		{ "[@NURI@",	VKEY_USERMENU },
		{ (char *)NULL,		0 }
};

//////////////////////////////////////////////////////////////////////
// CCLITerminal Class
//////////////////////////////////////////////////////////////////////

CCLITerminal::CCLITerminal()
{
}

CCLITerminal::~CCLITerminal()
{
}

//////////////////////////////////////////////////////////////////////
// CCLITerminal Override Functions
//////////////////////////////////////////////////////////////////////

CLISESSION *CCLITerminal::NewTerminalSession(void *pSession, int nType)
{
	CLISESSION	*pTerminal;

	if (pSession == NULL)
		return NULL;

	pTerminal = (CLISESSION *)MALLOC(sizeof(CLISESSION));
	if (pTerminal == NULL)
		return NULL;

	memset(pTerminal, 0, sizeof(CLISESSION));
	pTerminal->nType		= nType;
	pTerminal->bNeedUser	= TRUE;
	pTerminal->nMode		= CLIMODE_COMMAND;
	pTerminal->pSession		= pSession;
	pTerminal->nState		= TS_DATA;
	pTerminal->bLogined		= m_pCLIService->GetRunLevel() == RUNLEVEL_NORMAL ? FALSE : TRUE;
	pTerminal->nLoginRetry	= 0;
	if (m_pCLIService->GetRunLevel() == RUNLEVEL_DEBUG)
	     strcpy(pTerminal->szPrompt, m_pCLIService->m_szDebugPrompt);
	else strcpy(pTerminal->szPrompt, m_pCLIService->m_szUserPrompt);
	return pTerminal;
}

void CCLITerminal::DeleteTerminalSession(CLISESSION *pSession)
{
	int		i;

	if (pSession == NULL)
		return;

	for(i=0; i<CLIMAX_HISTORY; i++)
	{
		if (pSession->pszHistory[i])
			FREE(pSession->pszHistory[i]);
	}

	FREE(pSession);
}

BOOL CCLITerminal::SetupTerminal(CLISESSION *pSession)
{
/*
    struct termios new_settings;
    struct termios stored_settings;

    // Change Local Echo Option
    tcgetattr(pSession->sSocket, &stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ECHO);
    new_settings.c_lflag &= (~ICANON);
    tcsetattr(pSession->sSocket,TCSANOW, &new_settings);
*/
	if (pSession->nType != CLITYPE_SERIAL)
    	WriteStream(pSession, (char *)kludge_telopt, 31);
//  LocalDoOption(pTerminal, TELOPT_ECHO, FALSE, FALSE);

	return TRUE;
}

void CCLITerminal::RestartTerminalSession(CLISESSION *pSession)
{
	if (pSession == NULL)
		return;

	// Callback Login
	if (m_pCLIService->m_pConstruct &&
		m_pCLIService->m_pConstruct->pfnOnLogout)
		m_pCLIService->m_pConstruct->pfnOnLogout(pSession);

	if (m_pCLIService->GetRunLevel() == RUNLEVEL_NORMAL)
	{
		pSession->bNeedUser	= TRUE;
		strcpy(pSession->szUser, "");
		strcpy(pSession->szPassword, "");
		ClearHistory(pSession);
	}

	pSession->nMode 		= CLIMODE_COMMAND;
	pSession->bLogined		= m_pCLIService->GetRunLevel() == RUNLEVEL_NORMAL ? FALSE : TRUE;
	pSession->nLoginRetry	= 0;
	pSession->nCmdLength 	= 0;
	pSession->szCommand[0]	= '\0';
	if (m_pCLIService->GetRunLevel() == RUNLEVEL_DEBUG)
	     strcpy(pSession->szPrompt, m_pCLIService->m_szDebugPrompt);
	else strcpy(pSession->szPrompt, m_pCLIService->m_szUserPrompt);

	DisplaySplash(pSession);
}

BOOL CCLITerminal::InputStream(CLISESSION *pSession, const unsigned char *pszBuffer, int nLength)
{
	unsigned char	cc;
 	int				i, nVKey;

	if (pSession->pWorker != NULL)
	{
		// 이미 수행중인 작업이 있는 경우
		return TRUE;
	}

	for(i=0; i<nLength; i++)
	{
		cc = pszBuffer[i];

retry_loop:
		switch(pSession->nState) {
		  case TS_CR :
			   if (cc == '\n')
				  break;

			   // Do not break
			   pSession->nState = TS_DATA;

		  case TS_DATA :
			   switch(cc) {
				 case 0x00 :
					  break;
				 case IAC :
				      pSession->nState = TS_IAC;
					  break;
				 case KEY_BS :
					  ExecuteBackspace(pSession);
					  break;
				 case KEY_DEL :
					  ExecuteDelete(pSession);
					  break;
				 case KEY_TAB :
					  if (!pSession->bLogined)
						  break;
					  ExecuteTab(pSession);
					  break;
				 case '?' :
					  if (!pSession->bLogined)
						  break;
					  ExecuteInlineHelp(pSession);
					  break;
				 case KEY_ESC :
					  pSession->nState = TS_ESC;
					  break;
				 case KEY_CTRL_D :
//					  exit(0);
					  return FALSE;
				 default :
//				   	  if (!pSession->nLocalOptions[TELOPT_BINARY] && (cc == '\r'))
				   	  if (cc == '\r')
				   	  {
					      if (!ExecuteCommand(pSession))
							  return FALSE;
					   	  pSession->nState = TS_CR;
					   	  break;
				   	  }
					  if (IsPrintable(cc) && ((pSession->nCmdLength+1)<=CLIMAX_COMMAND_SIZE))
					  {
					  	  pSession->szCommand[pSession->nCmdLength] = cc;
					  	  pSession->nCmdLength++;
						  if (pSession->bNeedUser)
					  	  	   WriteChar(pSession, cc);
					  	  else WriteChar(pSession, pSession->bLogined ? cc : '*');
					  }
					  break;
			   }
			   break;

		  case TS_ESC :
			   if ((cc != '[') && (cc != 'O'))
			   {
				  pSession->nState = TS_DATA;
 				  goto retry_loop;
			   }
			   pSession->nVKeyCount = 0;
			   pSession->nState = TS_ESCSEQ;

		  case TS_ESCSEQ :
			   if (pSession->nVKeyCount >= MAX_VIRTUAL_KEY_SIZE)
			   {
				   pSession->nState = TS_DATA;
				   goto retry_loop;
			   }

			   pSession->szVKeyBuffer[pSession->nVKeyCount] = cc;
			   pSession->nVKeyCount++;
			   pSession->szVKeyBuffer[pSession->nVKeyCount] = '\0';

			   nVKey = FindVirtualKey(pSession);
			   if (nVKey >= 0)
			   {
				   ExecuteVirtualKey(pSession, nVKey);
			   	   pSession->nState = TS_DATA;
			   }
			   else if (nVKey == -1)
			   {
				   pSession->nState = TS_DATA;
				   goto retry_loop;
			   }
			   break;

		  case TS_IAC :
			   switch(cc) {
				 case BREAK :
				 case IP :
					  pSession->nState = TS_DATA;
					  break;
				 case AYT :
					  WriteStream(pSession, (char *)aytreply, strlen((char *)aytreply));
					  pSession->nState = TS_DATA;
					  break;
				 case AO :
					  WriteStream(pSession, (char *)aoreply, 2);
					  pSession->nState = TS_DATA;
					  break;
				 case EC :
					  WriteStream(pSession, "\b", 1);
					  pSession->nState = TS_DATA;
					  break;
				 case EL :
					  WriteStream(pSession, "\025", 1);
					  pSession->nState = TS_DATA;
					  break;
				 case DM :
					  pSession->nState = TS_DATA;
					  break;
				 case SB :
					  pSession->nState = TS_SB;
					  break;
				 case WILL :
					  pSession->nState = TS_WILL;
					  break;
				 case WONT :
					  pSession->nState = TS_WONT;
					  break;
				 case DO :
					  pSession->nState = TS_DO;
					  break;
				 case DONT :
					  pSession->nState = TS_DONT;
					  break;
				 case IAC :
					  WriteChar(pSession, cc);
					  pSession->nState = TS_DATA;
					  break;
				 default :
					  pSession->nState = TS_DATA;
					  break;
			   }
			   break;

		  case TS_SB :
			   if (cc == IAC)
				   pSession->nState = TS_SE;
			   break;

		  case TS_SE :
			   pSession->nState = (cc == SE) ? TS_DATA : TS_SB;
			   break; 

		  case TS_WILL :
			   //printf("IAC WILL %0d\n", cc);
			   RemoteDoOption(pSession, (int)cc, TRUE, TRUE);
			   pSession->nState = TS_DATA;
			   break;

		  case TS_WONT :
			   //printf("IAC WONT %0d\n", cc);
			   RemoteDoOption(pSession, (int)cc, FALSE, TRUE);
			   pSession->nState = TS_DATA;
			   break;

		  case TS_DO :
			   //printf("IAC DO %0d\n", cc);
			   LocalDoOption(pSession, (int)cc, TRUE, TRUE);
			   pSession->nState = TS_DATA;
			   break;

		  case TS_DONT :
			   //printf("IAC DONT %0d\n", cc);
			   LocalDoOption(pSession, (int)cc, FALSE, TRUE);
			   pSession->nState = TS_DATA;
			   break;
		}
	}

	return TRUE;
}

BOOL CCLITerminal::WriteChar(CLISESSION *pSession, unsigned char c)
{
	return TRUE;
}

BOOL CCLITerminal::WriteStream(CLISESSION *pSession, const char *pszBuffer, int nLength)
{
	return TRUE;
}

BOOL CCLITerminal::LocalDoOption(CLISESSION *pSession, int nOption, BOOL bEnable, BOOL bRemote)
{
	BOOL	bWill = bEnable;
	
	if (pSession->nLocalOptions[nOption] == bEnable)
		return TRUE;

	switch(nOption) {
	  case TELOPT_BINARY :
	  case TELOPT_ECHO :
		   SetOption(pSession, nOption, bEnable);
		   break;
	  case TELOPT_SGA :
		   break;
	  default :
		   bWill = FALSE;
		   break;
	}

	if ((pSession->nLocalOptions[nOption] != bWill) || bRemote)
	{
		unsigned char	msg[3];

		msg[0]	= IAC;
		msg[1]	= bWill ? WILL : WONT;
		msg[2]	= nOption;

		WriteStream(pSession, (char *)msg, 3);
		pSession->nLocalOptions[nOption] = bWill;
	}

	return (bWill == bEnable) ? TRUE : FALSE;
}

BOOL CCLITerminal::RemoteDoOption(CLISESSION *pSession, int nOption, BOOL bEnable, BOOL bRemote)
{
	BOOL	doFlag = bEnable;

	if (pSession->nRemoteOptions[nOption] == bEnable)
		return TRUE;

	switch(nOption) {
	  case TELOPT_BINARY :
	  case TELOPT_ECHO :
		   SetOption(pSession, nOption, bEnable);
		   break;
	  case TELOPT_SGA :
		   break;
	  default :
		   doFlag = FALSE;
		   break;
	}

	if ((pSession->nRemoteOptions[nOption] != doFlag) || bRemote)
	{
		unsigned char	msg[3];

		msg[0]	= IAC;
		msg[1]	= doFlag ? DO : DONT;
		msg[2]	= nOption;

		WriteStream(pSession, (char *)msg, 3);
		pSession->nRemoteOptions[nOption] = doFlag;
	}

	return (doFlag == bEnable) ? TRUE : FALSE;
}

void CCLITerminal::SetOption(CLISESSION *pSession, int nOption, BOOL bEnable)
{
}

BOOL CCLITerminal::IsPrintable(unsigned char c)
{
	if (c < 0x20)
		return FALSE;
	return TRUE;
}

int CCLITerminal::FindVirtualKey(CLISESSION *pSession)
{
	char	*pVKey;
	int		i, nCount;

	// Skip ESC
	pVKey 	= pSession->szVKeyBuffer;
	nCount	= pSession->nVKeyCount;

	// Find Same Key
	for(i=0; m_arKeyMap[i].pszKeyString; i++)
	{
		if (strcmp(m_arKeyMap[i].pszKeyString, pVKey) == 0)
		{
			pSession->nVKey = m_arKeyMap[i].nVKey;
			return i;
		}
	}

	// Find Match Key
	for(i=0; m_arKeyMap[i].pszKeyString; i++)
	{
		if (strncmp(m_arKeyMap[i].pszKeyString, pVKey, nCount) >= 0)
			return -2;
	}

	return -1;
}

void CCLITerminal::DisplaySplash(CLISESSION *pSession)
{
	char	szVersion[128];

	if (pSession->nMode == CLIMODE_USER)
	{
	  	DisplayUserMenu(pSession);
		return;
	}

    WriteStream(pSession, "\033[H\033[J");
    sprintf(szVersion, "AIMIR Management Console (%s)\xd\xa", m_pMcuVersionString);
    WriteStream(pSession, szVersion);
    WriteStream(pSession, COPYRIGHT "\xd\xa");
    WriteStream(pSession, "\xd\xa");

	DisplayPrompt(pSession);
}

void CCLITerminal::DisplayPrompt(CLISESSION *pSession)
{
	if (pSession->nMode == CLIMODE_USER)
		 DisplayUserMenu(pSession);
	else WriteStream(pSession, pSession->szPrompt);
}

BOOL CCLITerminal::SpliteParameter(CLISESSION *pSession)
{
	int		i, nState, nSeek;
	unsigned char	c;

	nState = CLI_STATE_WHITE;
	nSeek  = 0;
	pSession->argc	= 0;

	for(i=0; i<pSession->nCmdLength; i++)
	{
		c = (unsigned char)pSession->pszArgString[i];
		switch(nState) {
		  case CLI_STATE_WHITE :
			   if (c == ' ')
				   break;
			   nSeek = 0;
			   pSession->argv[pSession->argc] = &pSession->pszArgString[i];
			   nState = CLI_STATE_TEXT;

		  case CLI_STATE_TEXT :
			   if (c == ' ')
			   {
				   pSession->pszArgString[i] = '\0';
			   	   nSeek = 0;
				   pSession->argc++;
				   nState = CLI_STATE_WHITE;
				   break;
			   }
			   else if ((nSeek == 0) && (c == '"'))
			   {
			   	   pSession->argv[pSession->argc] = &pSession->pszArgString[i+1];
				   nState = CLI_STATE_OPEN;
				   break;
			   }
			   nSeek++;
			   break;

		  case CLI_STATE_OPEN :	
			   if (c == '"')
			   {
				   pSession->pszArgString[i] = '\0';
				   pSession->argc++;
			   	   nSeek = 0;
				   nState = CLI_STATE_WHITE;
				   break;
			   }
			   nSeek++;
			   break;
		}

		if ((pSession->argc+1) > CLIMAX_PARAM_COUNT)
			break;
	}

	// Close Remain
	if (nSeek > 0)
	{
		pSession->pszArgString[i] = '\0';
		pSession->argc++;
	}

	return TRUE;
}

CLIHANDLER *CCLITerminal::FindCommandHandler(CLISESSION *pSession, int &nDepts, char *pszNode)
{
	CLIHANDLER	*pHandler;

	if (m_pCLIService->GetRunLevel() == RUNLEVEL_DEBUG)
		 pHandler = FindCommand(pSession, m_CliDebugCommandHandlers, nDepts, pszNode);
	else pHandler = FindCommand(pSession, m_pCommandHandlers, nDepts, pszNode);
	return pHandler;
}

CLIHANDLER *CCLITerminal::FindCommand(CLISESSION *pSession, CLIHANDLER *pList, int &nDepts, char *pszNode)
{
	char	szBuffer[1024];
	int		i, j;

	// 명령어 테이블이 명령어 파라메터 노드보다 긴경우
	if (nDepts >= pSession->argc)
		return NULL;

	// 일치하는 명려어 탐색
	for(i=0; pList[i].pszCommand; i++)
	{
		if (strcmp(pList[i].pszCommand, pSession->argv[nDepts]) == 0)
		{
			strcat(pszNode, pList[i].pszCommand);
			strcat(pszNode, " ");
			if (pList[i].pNext != NULL)
			{
				if (pSession->argc <= (nDepts+1))
				{
					if (m_pCLIService->GetRunLevel() != RUNLEVEL_DEBUG)
					{
						sprintf(szBuffer, "Next syntax : %s <entry>\xd\xa", pList[i].pszCommand);
						WriteStream(pSession, szBuffer);
						for(j=0; pList[i].pNext[j].pszCommand; j++)
						{
#if     defined(__PATCH_6_2084__)
                            if(m_bEnableHiddenCommand && pList[i].pNext[j].bHidden) continue;
#else
                            if(pList[i].pNext[j].bHidden) continue;
#endif
	                        if(pList[i].nGroup < pSession->nGroup) continue;
							sprintf(szBuffer, "   %c%-20s %s\xd\xa", 
                                      pList[i].pNext[j].bHidden ? '*' : ' ',
									  pList[i].pNext[j].pszCommand,
									  pList[i].pNext[j].pszDescr ? pList[i].pNext[j].pszDescr : "");
							WriteStream(pSession, szBuffer);
						}
					}
					WriteStream(pSession, "\xd\xa");
					return NULL;
				}
				nDepts++;
				return FindCommand(pSession, pList[i].pNext, nDepts, pszNode);
			}
			return &pList[i];
		}
	}

	if (m_pCLIService->GetRunLevel() != RUNLEVEL_DEBUG)
	{
		WriteStream(pSession, "Unknown Command.\xd\xa");	
		for(i=0; pList[i].pszCommand; i++)
		{
#if     defined(__PATCH_6_2084__)
            if(m_bEnableHiddenCommand && pList[i].bHidden) continue;
#else
            if(pList[i].bHidden) continue;
#endif
	        if(pList[i].nGroup < pSession->nGroup) continue;
            
			sprintf(szBuffer, "   %c%-20s %s\xd\xa", 
                pList[i].bHidden ? '*' : ' ',
			  	pList[i].pszCommand,
			  	pList[i].pszDescr ? pList[i].pszDescr : "");
			WriteStream(pSession, szBuffer);
		}
	}
	WriteStream(pSession, "\xd\xa");

	return NULL;
}

BOOL CCLITerminal::CheckUser(CLISESSION *pSession)
{
	if (pSession->nCmdLength == 0)
		return FALSE;

	if (pSession->nCmdLength > 16)
		return FALSE;

	pSession->szCommand[pSession->nCmdLength] = '\0';
	strcpy(pSession->szUser, pSession->szCommand);
	strcpy(pSession->szPrompt, m_pCLIService->m_szPasswordPrompt);
	pSession->bNeedUser = FALSE;
	return TRUE;
}

BOOL CCLITerminal::CheckLogin(CLISESSION *pSession)
{
	if (pSession->nCmdLength == 0)
		return FALSE;

	if (pSession->nCmdLength > 16)
		return FALSE;

	pSession->szCommand[pSession->nCmdLength] = '\0';
	if (!m_pCLIService->DoLogin(pSession, pSession->szUser, pSession->szCommand))
		return FALSE;

	pSession->bDebugMode = FALSE;
	return TRUE;
}

void CCLITerminal::FirmwareDownload(CLISESSION *pSession)
{
    WORKSESSION *pWorker;
    int     pid, child, status;
   
    pid = Spawn(child, 1, "/tmp");
    if (pid > 0)
    {   
        if (pSession->nType == CLITYPE_TELNET)
        {   
			pWorker = (WORKSESSION *)pSession->pSession;
            dup2(pWorker->sSocket, STDIN_FILENO);
            dup2(pWorker->sSocket, STDOUT_FILENO);
            dup2(pWorker->sSocket, STDERR_FILENO);
        }
#if defined(__TI_AM335X__)
        execl("/usr/bin/lrz", "lrz", "-b", "-Z", "-y", NULL);
#else
        execl("/usr/bin/rz", "rz", "-b", "-Z", "-y", NULL);
#endif
        exit(0);
    }
    
    while(waitpid(child, &status, WNOHANG) == 0)
        usleep(1000000);
}

BOOL CCLITerminal::ExecuteCommand(CLISESSION *pSession, BOOL bComplete, BOOL bAddHistory)
{
    /** Issue #2642: 두가지 문제로 rollback 한다.
      *  1. 명령어가 Buffering 되지 않는 문제 (사용시 불편함)
      *  2. logout 명령이 동작하지 않는 문제 (return TRUE가 문제)
      *
      * 위 두 문제는 결국 Thread를 생성해 돌리기 때문에 발생하고 있다.
      */
#if 0
	// 콘솔인 경우 자기 프로세스로 동작한다.
    if (pSession->nType == CLITYPE_SERIAL)
		return CommandTask(pSession, bComplete, bAddHistory);

	// 텔넷의 경우 Task로 실행한다.
	m_pCLIService->Execute(TaskExecute, (void *)this, pSession, bComplete, bAddHistory);
	return TRUE;
#else
    return CommandTask(pSession, bComplete, bAddHistory);
#endif
}

void CCLITerminal::TaskExecute(void *pHandle, CLISESSION *pSession, BOOL bComplete, BOOL bAddHistory)
{
	CCLITerminal	*pThis;

	pThis = (CCLITerminal *)pHandle;
	pThis->CommandTask(pSession, bComplete, bAddHistory);
}

BOOL CCLITerminal::CommandTask(CLISESSION *pSession, BOOL bComplete, BOOL bAddHistory)
{
	CLIHANDLER	*pHandler;
	char		szCommand[256] = "";
	int			nResult, nDepts=0, nCommand;
	int			i, nIndex, argc, nParam, nPos;

	// Make New Line
	WriteStream(pSession, "\n\r");
	pSession->szCommand[pSession->nCmdLength] = '\0';

	if (pSession->nMode == CLIMODE_USER)
	{
		nCommand = atoi(pSession->szCommand);
		pSession->nCmdLength = 0;
		pSession->szCommand[0] = '\0';
		
		switch(nCommand) {
		  case 1 :
			   FirmwareDownload(pSession);
			   WriteStream(pSession, "\n\r");
			   break;
		  case 2 : 
			   pSession->nMode = CLIMODE_COMMAND;
			   DisplayPrompt(pSession);
			   return TRUE; 
		}
		DisplaySplash(pSession);
		return TRUE;
	}

	// Check Login State
	if (!pSession->bLogined)
	{
		if (pSession->bNeedUser)
		{
			if (!CheckUser(pSession))
			{
				// Display Login Error Message
				WriteStream(pSession, "Invalid User.\r\n");
			}

			// Clear Command Buffer
			pSession->nCmdLength 	= 0;
			pSession->szCommand[0]	= '\0';
			DisplayPrompt(pSession);
			return TRUE;

		}
		else if (!CheckLogin(pSession))
		{
			// Clear Command Buffer
			pSession->nCmdLength 	= 0;
			pSession->szCommand[0]	= '\0';
			pSession->bNeedUser		= TRUE;

			// Display Login Error Message
			WriteStream(pSession, "Invalid account or password.\r\n\r\n");
			strcpy(pSession->szPrompt, m_pCLIService->m_szUserPrompt);
			pSession->nLoginRetry++;
			if (pSession->nLoginRetry >= 3)
			{
				if (pSession->nType == CLITYPE_SERIAL)
					sleep(3);
				return FALSE;
			}

			DisplayPrompt(pSession);
			return TRUE;
		}

		// Clear Command Buffer
		pSession->nCmdLength 	= 0;
		pSession->szCommand[0]	= '\0';

		// Callback Login
		if (m_pCLIService->m_pConstruct &&
			m_pCLIService->m_pConstruct->pfnOnLogin)
			m_pCLIService->m_pConstruct->pfnOnLogin(pSession);

		// Login Complete
		pSession->bLogined = TRUE;
		WriteStream(pSession, "\n\r");
		strcpy(pSession->szPrompt, m_pCLIService->m_szDefaultPrompt);
		DisplayPrompt(pSession);
		return TRUE;
	}

	if (pSession->nCmdLength > 0)
	{
		// Find Command
		nResult = CLIERR_OK;
		pSession->szCommand[pSession->nCmdLength] = '\0';

		if (pSession->szCommand[0] == '!')
		{
			if (strcmp(pSession->szCommand, "!!") == 0)
			{
				strcpy(pSession->szCommand, pSession->pszHistory[pSession->nHistoryCount-1]);
				pSession->nCmdLength = strlen(pSession->szCommand);
			}
			else
			{
				nIndex = atoi(&pSession->szCommand[1]);
				if ((nIndex > 0) && (nIndex <= pSession->nHistoryCount))
				{
					strcpy(pSession->szCommand, pSession->pszHistory[nIndex-1]);
					pSession->nCmdLength = strlen(pSession->szCommand);
				}
			}
		}

		pSession->pszArgString = strdup(pSession->szCommand);
		if (bAddHistory)
			AddHistory(pSession, pSession->szCommand);

		SpliteParameter(pSession);
		pHandler = FindCommandHandler(pSession, nDepts, szCommand);
		if (pHandler != NULL)
		{
			// Execute Command
			nParam = GetParamCount(pHandler);
			argc = pSession->argc - (nDepts + 1);
			for(i=0; i<argc; i++)
				pSession->argv[i] = pSession->argv[nDepts+i+1];

			// 파라메터의 갯수가 더 많은 경우를 막을때, add apn 명령 때문에 허용하도록 변경 2007/9/5
			// if ((nParam == argc) || (pHandler->pszParam && (argc >= GetMinParamCount(pHandler)) && (argc <= nParam)))

			if ((nParam == argc) || (pHandler->pszParam && (argc >= GetMinParamCount(pHandler))))
			{
				nPos = ValidateParameter(pSession, pHandler);
				if (nPos == -1)
				{
					pSession->nCmdLength 	= 0;
					pSession->szCommand[0]	= '\0';

					if (m_pCLIService->m_pConstruct &&
						m_pCLIService->m_pConstruct->pfnOnCommand)
						m_pCLIService->m_pConstruct->pfnOnCommand(pSession, argc, pSession->argv, pHandler);

					if (pHandler->nGroup >= pSession->nGroup)
					{
						if (m_pCLIService->m_bEnableLog && pHandler->bLogFlag)
							m_pCLIService->AddLog(pSession->szUser, pSession->szCommand);

						nResult = pHandler->pfnCommand(pSession, argc, pSession->argv, (void *)pHandler);
						WriteStream(pSession, "\xd\xa");
					}
					else
					{
						WriteStream(pSession, "Permission Denied.");					
						WriteStream(pSession, "\xd\xa");
					}
				}
				else
				{
					WriteStream(pSession, "Invalid parameter : '");					
					WriteStream(pSession, pSession->argv[nPos]);					
					WriteStream(pSession, "'\xd\xa");
					WriteStream(pSession, "\xd\xa");
				}
			}
			else
			{
				WriteStream(pSession, "usage: ");
				WriteStream(pSession, szCommand);
				DisplayWideParameter(pSession, pHandler);
				DisplayAllParameter(pSession, pHandler);
				WriteStream(pSession, "\xd\xa");
			}
		}

		if (pSession->pszArgString)
			FREE(pSession->pszArgString);
		pSession->pszArgString = NULL;

		if (nResult == CLIERR_ERROR)
			return FALSE;
	}

	// Clear Command Buffer
	pSession->nCmdLength 	= 0;
	pSession->szCommand[0]	= '\0';

	// Display Prompt
	DisplayPrompt(pSession);
	return TRUE;
}

void CCLITerminal::AddHistory(CLISESSION *pSession, char *pszCommand)
{
	int		i;

	if (!pszCommand || !*pszCommand)
		return;

	if (pSession->nHistoryCount < CLIMAX_HISTORY)
	{
		pSession->pszHistory[pSession->nHistoryCount] = strdup(pszCommand);
		pSession->nHistoryCount++;
		pSession->nCurHistory = pSession->nHistoryCount;
		return;
	}

	FREE(pSession->pszHistory[0]);
	for(i=1; i<CLIMAX_HISTORY; i++)
		pSession->pszHistory[i-1] = pSession->pszHistory[i];

	pSession->pszHistory[pSession->nHistoryCount-1] = strdup(pszCommand);	
	pSession->nCurHistory = pSession->nHistoryCount;
}

void CCLITerminal::ClearHistory(CLISESSION *pSession)
{
	int		i;

	for(i=0; i<CLIMAX_HISTORY; i++)
	{
		if (pSession->pszHistory[i])
			FREE(pSession->pszHistory[i]);
		pSession->pszHistory[i] = NULL;
	}
	pSession->nCurHistory 	= 0;
	pSession->nHistoryCount = 0;
}

BOOL CCLITerminal::ExecuteBackspace(CLISESSION *pSession)
{
	if (pSession->nCmdLength == 0)
		return FALSE;

	pSession->nCmdLength--;
	WriteStream(pSession, "\x8 \x8");
	return TRUE;
}

BOOL CCLITerminal::ExecuteDelete(CLISESSION *pSession)
{
	return TRUE;
}

int CCLITerminal::UntilCopy(char *pszDest, char *pszSrc, int nMax)
{
	int		i, n=0;

	for(i=0; (i<nMax) && *pszSrc; i++, pszDest++, pszSrc++)
	{
		if ((*pszSrc == '%') || (*pszSrc == '*'))
			break;
		*pszDest = *pszSrc;
	}
	*pszDest = '\0';
	return n;
}

CLIPARAMCHECKER *CCLITerminal::FindParamHandler(char *pszName)
{
	int		i;

	if (!pszName || !*pszName)
		return NULL;

	for(i=0; m_CliParameters[i].pszFormat; i++)
	{
		if (strcmp(m_CliParameters[i].pszFormat, pszName) == 0)
			return &m_CliParameters[i];
	}
	return NULL;
}

int CCLITerminal::GetParamCount(CLIHANDLER *pHandler)
{
	int		i, n=0;

	if ((pHandler == NULL) || (!pHandler->pszParam))
		return 0;

	for(i=0; pHandler->pszParam[i]; i++)
	{
		if (pHandler->pszParam[i] == '%')
			n++;
	}
	return n;
}

int CCLITerminal::GetMinParamCount(CLIHANDLER *pHandler)
{
	int		i, n=0;

	if ((pHandler == NULL) || (!pHandler->pszParam))
		return 0;

	for(i=0; pHandler->pszParam[i]; i++)
	{
		if (pHandler->pszParam[i] == '*')
			break;

		if (pHandler->pszParam[i] == '%')
			n++;
	}
	return n;
}

int CCLITerminal::ValidateParameter(CLISESSION *pSession, CLIHANDLER *pHandler)
{
	CLIPARAMCHECKER	*pChecker;
	int		i, n, nCount=0;
	char	szTag[32];

	if (!pHandler || !pHandler->pszParam)
		return -1;

	for(i=0; pHandler->pszParam[i]; i++)
	{
		if (pHandler->pszParam[i] == '*')
			return -1;
		
		if (pHandler->pszParam[i] == '%')
		{
			n = UntilCopy(szTag, const_cast<char *>(&pHandler->pszParam[i+1]), 32);
			pChecker = FindParamHandler(szTag);
			if (pChecker != NULL)
			{	
				if (!pChecker->pfnCheck(pSession, pSession->argv[nCount]))
					return nCount;
			}
			i += n;
			nCount++;
		}
	}	
	return -1;	
}

int CCLITerminal::DisplayAllParameter(CLISESSION *pSession, CLIHANDLER *pHandler, int nIndex)
{
	CLIPARAMCHECKER	*pChecker;
	int		i, n, nCount = 0;
	char	szTag[32];
	char	szName[64];
	char	szBuffer[1024];

	if (!pHandler)
		return -1;

	WriteStream(pSession, "\xd\xa");
	if (!pHandler->pszParam)
		return 0;

	for(i=0; pHandler->pszParam[i]; i++)
	{
		if (pHandler->pszParam[i] == '*')
		{
			WriteStream(pSession, "    <enter>\xd\xa");
		}
		else if (pHandler->pszParam[i] == '%')
		{
			n = UntilCopy(szTag, const_cast<char *>(&pHandler->pszParam[i+1]), 32);
			pChecker = FindParamHandler(szTag);
			if (pChecker != NULL)
			{
				sprintf(szName, "<%s>", pChecker->pszLabel);
				sprintf(szBuffer, "    %-20s %s\xd\xa", szName, pChecker->pszDescr);
				WriteStream(pSession, szBuffer);  			
			}
			else
			{
				sprintf(szBuffer, "  '%s'\xd\xa", szTag);
				WriteStream(pSession, szBuffer);
			}
			i += n;
			nCount++;
		}
	}	
	return nCount;	
}

int CCLITerminal::DisplayWideParameter(CLISESSION *pSession, CLIHANDLER *pHandler)
{
	CLIPARAMCHECKER	*pChecker;
	int		i, n, nCount = 0;
	char	szTag[32];
	char	szBuffer[64];

	if (!pHandler)
		return -1;

	if (!pHandler->pszParam)
		return 0;

	for(i=0; pHandler->pszParam[i]; i++)
	{
		if (pHandler->pszParam[i] == '*')
			continue;
		
		if (pHandler->pszParam[i] == '%')
		{
			n = UntilCopy(szTag, const_cast<char *>(&pHandler->pszParam[i+1]), 32);
			pChecker = FindParamHandler(szTag);
			if (pChecker != NULL)
			{
				sprintf(szBuffer, "<%s> ", pChecker->pszLabel);
				WriteStream(pSession, szBuffer);  			
			}
			else
			{
				sprintf(szBuffer, "'%s' ", szTag);
				WriteStream(pSession, szBuffer);
			}
			i += n;
			nCount++;
		}
	}	
	return nCount;	
}

int CCLITerminal::CheckCommandSyntax(CLISESSION *pSession, CLIHANDLER *pList, int &nDepts, CLIHANDLER **pHandler, CLIHANDLER **pLast)
{
	int		i, nSize, nFind=0;
	int		n, nIndex = -1;

	nSize = strlen(pSession->argv[nDepts]);
	for(i=0; pList[i].pszCommand; i++)
	{
#if     defined(__PATCH_6_2084__)
        if(m_bEnableHiddenCommand && pList[i].bHidden) continue;
#else
        if(pList[i].bHidden) continue;
#endif
	    if (pList[i].nGroup < pSession->nGroup) continue;

		if ((nDepts+1) == pSession->argc)
			 n = strncmp(pList[i].pszCommand, pSession->argv[nDepts], nSize);
		else n = strcmp(pList[i].pszCommand, pSession->argv[nDepts]);

		if (n == 0)
		{
			nIndex = i;
			nFind++;
		}	
	}

	if (!nFind || (nFind > 1))
	{
		*pLast = NULL;
		*pHandler = pList;
		return nFind;
	}

	if (pList[nIndex].pNext == NULL)
	{
		*pLast = &pList[nIndex];
		*pHandler = &pList[nIndex];
		return nFind;
	}

	if ((nDepts+1) >= pSession->argc)
	{
		*pLast = &pList[nIndex];
		*pHandler = pList[nIndex].pNext;
		return -1;
	}

	nDepts++;
	nFind = CheckCommandSyntax(pSession, pList[nIndex].pNext, nDepts, pHandler, pLast);
	return nFind;
}

int CCLITerminal::FindBestMatchCommand(CLISESSION *pSession, CLIHANDLER **pHandler, BOOL bComment)
{
	CLIHANDLER *pLast = NULL;
	char	szBuffer[1024];
	int		i, nResult, nSize;
	int		nDepts=0;

	*pHandler = NULL;
	pSession->pszArgString = strdup(pSession->szCommand);
	SpliteParameter(pSession);

	if (pSession->argc == 0)
	{
		if (!bComment)
			return 0;

		WriteStream(pSession, "\xd\xa");
	    for(i=0; m_pCommandHandlers[i].pszCommand; i++)
	    {
#if     defined(__PATCH_6_2084__)
           if(m_bEnableHiddenCommand && m_pCommandHandlers[i].bHidden) continue;
#else
           if(m_pCommandHandlers[i].bHidden) continue;
#endif
	       if (m_pCommandHandlers[i].nGroup < pSession->nGroup) continue;

	  	   sprintf(szBuffer, "   %c%-20s %s\xd\xa", 
                    m_pCommandHandlers[i].bHidden ? '*' : ' ',
					m_pCommandHandlers[i].pszCommand, m_pCommandHandlers[i].pszDescr);
	   	   WriteStream(pSession, szBuffer);
	    }
		DisplayPrompt(pSession);

		if (pSession->pszArgString)
			FREE(pSession->pszArgString);
		pSession->pszArgString = NULL;
		return 0;
	}

	nResult	= CheckCommandSyntax(pSession, m_pCommandHandlers, nDepts, pHandler, &pLast);
	switch(nResult) {
	  case -1 :
		   if (strlen(pLast->pszCommand) != strlen(pSession->argv[nDepts]))
		   {
		  	   strcat(pSession->szCommand, &pLast->pszCommand[strlen(pSession->argv[nDepts])]);
		   	   if (!bComment)
		  	   	   WriteStream(pSession, &pLast->pszCommand[strlen(pSession->argv[nDepts])]);
			   if (pLast->pNext || pLast->pszParam)
			   {
		   	   	   if (!bComment)
		  	   	   	   WriteStream(pSession, " ");
				   strcat(pSession->szCommand, " ");
			   }
		   	   pSession->nCmdLength = strlen(pSession->szCommand);
		   	   if (!bComment)
				   break;
			   WriteStream(pSession, "\xd\xa");
	   		   for(i=0; (*pHandler)[i].pszCommand; i++)
	   		   {
#if     defined(__PATCH_6_2084__)
                   if(m_bEnableHiddenCommand && (*pHandler)[i].bHidden) continue;
#else
                   if((*pHandler)[i].bHidden) continue;
#endif
	               if ((*pHandler)[i].nGroup < pSession->nGroup) continue;
	   	   		   sprintf(szBuffer, "   %c%-20s %s\xd\xa", 
                        (*pHandler)[i].bHidden ? '*' : ' ',
                        (*pHandler)[i].pszCommand, (*pHandler)[i].pszDescr);
	   	   		   WriteStream(pSession, szBuffer);
	   		   }
			   DisplayPrompt(pSession);
			   WriteStream(pSession, pSession->szCommand);
	   	   }
		   else if (pLast != *pHandler)
		   {
		   	   if (pSession->szCommand[pSession->nCmdLength-1] != ' ')
			   {
			   	   strcat(pSession->szCommand, " ");
		     	   pSession->nCmdLength = strlen(pSession->szCommand);
		   	   	   if (!bComment)
		  	   	   	   WriteStream(pSession, " ");
			   }
		   	   if (!bComment)
				   break;
			   WriteStream(pSession, "\xd\xa");
	   		   for(i=0; (*pHandler)[i].pszCommand; i++)
	   		   {
#if     defined(__PATCH_6_2084__)
                   if(m_bEnableHiddenCommand && (*pHandler)[i].bHidden) continue;
#else
                   if((*pHandler)[i].bHidden) continue;
#endif
	               if ((*pHandler)[i].nGroup < pSession->nGroup) continue;
	   	   		   sprintf(szBuffer, "   %c%-20s %s\xd\xa", 
                        (*pHandler)[i].bHidden ? '*' : ' ',
                        (*pHandler)[i].pszCommand, (*pHandler)[i].pszDescr);
	   	   		   WriteStream(pSession, szBuffer);
	   		   }
			   DisplayPrompt(pSession);
			   WriteStream(pSession, pSession->szCommand);
		   }
		   break;
	  case 0 :
		   if (!bComment)
			   break;
		   WriteStream(pSession, "\xd\xa");
	   	   for(i=0; (*pHandler)[i].pszCommand; i++)
	   	   {
#if     defined(__PATCH_6_2084__)
               if(m_bEnableHiddenCommand && (*pHandler)[i].bHidden) continue;
#else
               if((*pHandler)[i].bHidden) continue;
#endif
	           if ((*pHandler)[i].nGroup < pSession->nGroup) continue;
	   	 	   sprintf(szBuffer, "   %c%-20s %s\xd\xa", 
                    (*pHandler)[i].bHidden ? '*' : ' ',
                    (*pHandler)[i].pszCommand, (*pHandler)[i].pszDescr);
	   	   	   WriteStream(pSession, szBuffer);
	   	   }
		   *pSession->szCommand = '\0';
		   for(i=0; i<nDepts; i++)
		   {
			   strcat(pSession->szCommand, pSession->argv[i]);
			   strcat(pSession->szCommand, " ");
		   }
		   pSession->nCmdLength = strlen(pSession->szCommand);	
		   DisplayPrompt(pSession);
		   WriteStream(pSession, pSession->szCommand);
		   break;
	  case 1 :
		   if (strlen(pLast->pszCommand) != strlen(pSession->argv[nDepts]))
		   {
			   strcat(pSession->szCommand, &pLast->pszCommand[strlen(pSession->argv[nDepts])]);
			   if (pLast->pNext || pLast->pszParam)
				   strcat(pSession->szCommand, " ");
			   pSession->nCmdLength = strlen(pSession->szCommand);
			   if (!bComment)
			   {
			   	   WriteStream(pSession, &pLast->pszCommand[strlen(pSession->argv[nDepts])]);
			       if (pLast->pNext || pLast->pszParam)
				       WriteStream(pSession, " ");
			   }
		   } 
		   else if (pLast->pszParam && (pSession->szCommand[pSession->nCmdLength-1] != ' '))
		   {
			   if (pSession->argc <= (nDepts+1))
			   {
			       strcat(pSession->szCommand, " ");
		   	   	   pSession->nCmdLength = strlen(pSession->szCommand);
				   if (!bComment)
				   {
			   	       if (pLast->pNext || pLast->pszParam)
				   	       WriteStream(pSession, " ");
				   }
			   }
		   }
		   if (!pLast->pszParam)
			   break;
		   if (!bComment)
			   break;
		   DisplayAllParameter(pSession, pLast);
		   DisplayPrompt(pSession);
		   WriteStream(pSession, pSession->szCommand);
		   break;
	  default :
		   if (!bComment)
			   break;
		   WriteStream(pSession, "\xd\xa");
		   nSize = strlen(pSession->argv[nDepts]);
		   for(i=0; (*pHandler)[i].pszCommand; i++)
		   {
			   if (strncmp((*pHandler)[i].pszCommand, pSession->argv[nDepts], nSize) != 0)
				   continue;

#if     defined(__PATCH_6_2084__)
               if(m_bEnableHiddenCommand && (*pHandler)[i].bHidden) continue;
#else
               if((*pHandler)[i].bHidden) continue;
#endif
	           if ((*pHandler)[i].nGroup < pSession->nGroup) continue;

		   	   sprintf(szBuffer, "   %c%-20s %s\xd\xa", 
                    (*pHandler)[i].bHidden ? '*' : ' ',
                    (*pHandler)[i].pszCommand, (*pHandler)[i].pszDescr);
		   	   WriteStream(pSession, szBuffer);
		   }
		   DisplayPrompt(pSession);
		   WriteStream(pSession, pSession->szCommand); 
		   break;
	}

	if (pSession->pszArgString)
		FREE(pSession->pszArgString);
	pSession->pszArgString = NULL;

	return nResult;
}

BOOL CCLITerminal::ExecuteInlineHelp(CLISESSION *pSession)
{
	CLIHANDLER	*pHandler;

	if (m_pCLIService->GetRunLevel() == RUNLEVEL_DEBUG)
		return TRUE;

	if (pSession->nMode == CLIMODE_USER)
		return TRUE;

	pSession->szCommand[pSession->nCmdLength] = '\0';
	pSession->szCommand[pSession->nCmdLength] = '\0';
	FindBestMatchCommand(pSession, &pHandler, TRUE);
	return TRUE;
}

BOOL CCLITerminal::ExecuteTab(CLISESSION *pSession)
{
	CLIHANDLER	*pHandler;

	if (m_pCLIService->GetRunLevel() == RUNLEVEL_DEBUG)
		return TRUE;

	if (pSession->nMode == CLIMODE_USER)
		return TRUE;

	pSession->szCommand[pSession->nCmdLength] = '\0';
	FindBestMatchCommand(pSession, &pHandler, FALSE);
	return TRUE;
}

void CCLITerminal::ReplaceCommand(CLISESSION *pSession, const char *pszCommand)
{
	char	szWhite[256];
    char * pszCmd = NULL;

	if (pszCommand == NULL)
		pszCmd = const_cast<char*>("");
    else 
        pszCmd = const_cast<char*>(pszCommand);

	strcpy(pSession->szCommand, pszCmd);
	pSession->nCmdLength = strlen(pszCmd);

	WriteStream(pSession, "\xd");
	memset(szWhite, ' ', 79);
	szWhite[79] = 0;
	WriteStream(pSession, szWhite);
	WriteStream(pSession, "\xd");
	DisplayPrompt(pSession);
	WriteStream(pSession, pSession->szCommand);
}

void CCLITerminal::DisplayUserMenu(CLISESSION *pSession)
{
	WriteStream(pSession, "\xd\xa");
	WriteStream(pSession, "FIRMWARE DOWNLOAD MENU\xd\xa");
	WriteStream(pSession, "\xd\xa");
	WriteStream(pSession, "1. Download\xd\xa");
	WriteStream(pSession, "2. Run\xd\xa");
	WriteStream(pSession, "\xd\xa");
	WriteStream(pSession, "SELECT> ");
}

BOOL CCLITerminal::ExecuteVirtualKey(CLISESSION *pSession, int nVKey)
{
	if (m_pCLIService->GetRunLevel() == RUNLEVEL_DEBUG)
		return TRUE;

	switch(pSession->nVKey) {
	  case VKEY_F1 :
		   if (!pSession->bLogined)
			   break;
		   strcpy(pSession->szCommand, (char *)m_pCLIService->m_szVirtualKey[0]);
		   WriteStream(pSession, pSession->szCommand);
		   pSession->nCmdLength = strlen(pSession->szCommand);
		   ExecuteCommand(pSession, FALSE, FALSE);
		   break;

	  case VKEY_F2 :
		   if (!pSession->bLogined)
			   break;
		   strcpy(pSession->szCommand, (char *)m_pCLIService->m_szVirtualKey[1]);
		   WriteStream(pSession, pSession->szCommand);
		   pSession->nCmdLength = strlen(pSession->szCommand);
		   ExecuteCommand(pSession, FALSE, FALSE);
		   break;

	  case VKEY_F3 :
		   if (!pSession->bLogined)
			   break;
		   strcpy(pSession->szCommand, (char *)m_pCLIService->m_szVirtualKey[2]);
		   WriteStream(pSession, pSession->szCommand);
		   pSession->nCmdLength = strlen(pSession->szCommand);
		   ExecuteCommand(pSession, FALSE, FALSE);
		   break;

	  case VKEY_F4 :
		   if (!pSession->bLogined)
			   break;
		   strcpy(pSession->szCommand, (char *)m_pCLIService->m_szVirtualKey[3]);
		   WriteStream(pSession, pSession->szCommand);
		   pSession->nCmdLength = strlen(pSession->szCommand);
		   ExecuteCommand(pSession, FALSE, FALSE);
		   break;

	  case VKEY_UP :
		   if (!pSession->bLogined || (pSession->nCurHistory <= 0))
			  break;
		   pSession->nCurHistory--;
		   ReplaceCommand(pSession, pSession->pszHistory[pSession->nCurHistory]);
		   break;

	  case VKEY_DOWN :
		   if (!pSession->bLogined)
			   break;
		   if (pSession->nCurHistory >= pSession->nHistoryCount)
		   {
			   if (pSession->nCmdLength != 0)
		   	   	   ReplaceCommand(pSession, "");
			   break;
		   }
		   pSession->nCurHistory++;
		   ReplaceCommand(pSession, pSession->pszHistory[pSession->nCurHistory]);
		   break;

	  case VKEY_LEFT :
		   break;

	  case VKEY_RIGHT :
		   break;

	  case VKEY_HOME :
		   break;

	  case VKEY_END :
		   break;

	  case VKEY_USERMENU :
		   if (pSession->nMode == CLIMODE_USER)
			   break;

		   if (pSession->nType == CLITYPE_TELNET)
       		   break;

		   pSession->nMode = CLIMODE_USER;
		   DisplayUserMenu(pSession);
		   break;
	}
	return TRUE;
}

