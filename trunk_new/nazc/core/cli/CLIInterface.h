#ifndef __CLI_INTERFACE_H__
#define __CLI_INTERFACE_H__

#include "CLIUtility.h"

void CLIAPI_Startup(CLICONSTRUCT *pConstruct, int argc, char **argv);
void CLIAPI_Shutdown();

int	CLIAPI_GetRunLevel();
void CLIAPI_Exit();

void CLIAPI_DisplayHelp(CLISESSION *pSession, char *pszCommand);
void CLIAPI_DisplayHistory(CLISESSION *pSession);

BOOL CLIAPI_Command(CLISESSION *pSession, IF4Invoke *pInvoke, const char *pszCommand);
BOOL CLIAPI_CommandTest(CLISESSION *pSession, IF4Invoke *pInvoke, char *pszCommand);

int CLIAPI_ShowCommandLog(CLISESSION *pSession, char *pDate);
int CLIAPI_ClearCommandLog(CLISESSION *pSession);
BOOL CLIAPI_FirmwareDownload(CLISESSION *pSession);
BOOL CLIAPI_PolicyDownload(CLISESSION *pSession);
BOOL CLIAPI_Store(CLISESSION *pSession);

UINT CLIAPI_GetAgentPort();

#endif	// __CLI_INTERFACE_H__
