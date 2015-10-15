#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#ifndef _WIN32
#include <sys/wait.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "MemoryDebug.h"
#include "CommonUtil.h"
#include "Chunk.h"

int		m_nSeekFileNo = 0;

#ifndef _WIN32
int Spawn(int &nChild, int bNoMessage, const char *pszBasePath)
{
	pid_t	pid;
	int		fd;

	// In case of fork is error.
#ifdef __uClinux__
	pid = vfork();
#else
	pid = fork();
#endif
	if (pid < 0)
    {
		perror("fork error");
      	return -1;
    }

	// In case of this is parent process.
	nChild = pid;
	if (pid != 0)
    	return -2;

	// Become session leader and get pid.
	pid = setsid();
	if (pid < -1)
    {
		perror("setsid error");
		return -1;
    }

	// Change directory to root.
	if (pszBasePath && *pszBasePath)
		chdir(pszBasePath);

	// File descriptor close.
	if (!bNoMessage)
    {
		fd = open("/dev/null", O_RDWR, 0);
		if (fd != -1)
		{
			dup2(fd, STDIN_FILENO);
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			if (fd > 2)
				close (fd);
		}
	}

	// Change File Mask
	umask(0027);

	return pid;
}

int SystemCall(const char *pszPath, const char *pszBasePath, char **pszMessage, int nNoWait, int nShell)
{
	char	szFileName[64];
	char	szBuffer[1024];
	int		pid, child, status;
	int		fd, len=0, retry=0;
	FILE	*fp;
	char	*p = NULL;

	if (pszMessage)
		*pszMessage = NULL;

	if (!pszPath || !*pszPath)
		return -1;

	m_nSeekFileNo++;
	if (m_nSeekFileNo > 1000)
		m_nSeekFileNo = 0;
	sprintf(szFileName, "/tmp/tmp%X", m_nSeekFileNo);
	
	pid = Spawn(child, 0, pszBasePath);
	if (pid > 0)
	{
		fd = open(szFileName, O_CREAT|O_TRUNC|O_RDWR, 0666);
		if (fd != -1)
		{
			if (!nNoWait)
				dup2(fd, STDOUT_FILENO);
			close(fd);
	
			if (nShell)
			     execl("/bin/sh", "/bin/sh", pszPath, NULL);
			else execl(pszPath, pszPath, NULL);
		}
		exit(0);
	}

	while(waitpid(child, &status, WNOHANG) == 0)
	{
		USLEEP(1000000);
		retry++;
		if (retry > 5 )
			break;
	}

	if (!nNoWait && pszMessage)
	{
		fp = fopen(szFileName, "r");
		if (fp != NULL)
		{
#ifdef __STATIC_LAUNCHER__
			p = (char *)malloc(1024*20);
#else
			p = (char *)MALLOC(1024*20);
#endif
			memset(p, 0, 1024*20);
			while(fgets(szBuffer, 1024, fp))
			{
				len = strlen(szBuffer);
				if (len > 0)
				{
					szBuffer[len-1] = '\0';
					strcat(p, szBuffer);
				}
				strcat(p, "\xd\xa");
			}
			*pszMessage = p;
			fclose(fp);
		}
	}
	remove(szFileName);

	return 0;
}

void ForkProcess(const char *pszPath, int bWait)
{
	pid_t	pid;
	int		status;

	if (!pszPath || !*pszPath)
		return;

	// In case of fork is error.
#ifdef __uClinux__
	pid = vfork();
#else
	pid = fork();
#endif
	if (pid == -1)
	{
		// Fork Error
      	return;
	}
	else if (pid != 0)
	{
		// Parent Process
		wait(&status);
		return;
	}

	close(0);
	close(1);
	close(2);

    // Become session leader and get pid.
    setsid(); 

    // Change File Mask
    umask(0027);

	execl(pszPath, pszPath, NULL);
	exit(0);
	return;
}

#endif

/** 실행 후 결과를 return 
 *
 *
 */
int SystemExec(const char *pszCommand)
{
    int nResult=-1;

    switch(nResult=system(pszCommand))
    {
        case 127:
        case -1:
            break;
        default:
            nResult = WEXITSTATUS(nResult);
            break;
    }
    return nResult;
}

/** Shell command를 실행하고 그 결과를 얻어온다.
 *
 * @return 실행된 command의 exit code
 */
int GetShellCommandResult(CChunk *pChunk, char *pszCommand)
{
    int     maxLine = 512;
    char    szLine[maxLine];
    FILE    *fp = NULL;
    int     res=-1;

	if (!pszCommand)
		return -1;

    if((fp = popen(pszCommand, "r")) != NULL) {
        while(fgets(szLine, maxLine, fp) != NULL)
        {
            if(pChunk != NULL)
            {
                pChunk->Add(szLine, strlen(szLine));
                pChunk->Add("\xd", 1);
            }
        }
        if(pChunk != NULL)
        {
            pChunk->Add("\x0", 1);
        }
        res = pclose(fp);
        res = WEXITSTATUS(res);
    }
	return res;
}

