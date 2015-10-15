#ifndef __SHELL_COMMAND_H__
#define __SHELL_COMMAND_H__

#include "Chunk.h"
#ifndef _WIN32
int Spawn(int &nChild, int bNoMessage=FALSE, const char *pszBasePath=NULL);
int SystemCall(const char *pszPath, const char *pszBasePath, char **pszMessage, int nNoWait=0, int nShell=0);
void ForkProcess(const char *pszPath, int bWait=1);

#endif
int GetShellCommandResult(CChunk *pChunk, char *pszCommand);

/** system(3) 을 대체하기 위해 만듬 */
int SystemExec(const char *pszCommand);

#endif	// __SHELL_COMMAND_H__
