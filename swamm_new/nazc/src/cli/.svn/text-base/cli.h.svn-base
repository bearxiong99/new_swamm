#ifndef __CLI_H__
#define __CLI_H__

void OnParseParameter(int argc, char **argv);
void OnStartup();
void OnShutdown();
void OnNewSession(CLISESSION *pSession);
void OnDeleteSession(CLISESSION *pSession);
BOOL DoLogin(CLISESSION *pSession, char *pszID, char *pszPassword);
void OnLogin(CLISESSION *pSession);
void OnLogout(CLISESSION *pSession);
void OnCommand(CLISESSION *pSession, int argc, char **argv, CLIHANDLER *pCmdTable);

void InitializeVariable();
#endif	// __CLI_H__
