#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>

#include "typedef.h"
#include "SystemUtil.h"
#include "DebugUtil.h"
#include "CommonUtil.h"

//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

typedef long long unsigned int t_mzapi_qword; 

typedef struct proc_s {
    char 		cmd[16];			// basename of executable file in call to exec(2)
    int 		ruid;				// real only (sorry)
    int 		pid;				// process id
    int 		ppid;				// pid of parent process
    char 		state;				// single-char code for process state (S=sleeping)
    unsigned 	int vmsize;			// size of process as far as the vm is concerned
    char 		ttyname[9];			// tty device
} proc_t;

int file2str(char *filename, char *ret, int cap);
int file2value(char *filename);
void parse_proc_status(char *S, proc_t *p);

//////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////

int FindProcess(const char *pszName)
{
	DIR		*dir;
	proc_t	p;
	struct 	dirent *entry;
	char 	path[32], sbuf[512];
	int		pid = -1;

	dir = opendir("/proc");
	if (!dir)
		return -1;

	while((entry=readdir(dir)) != NULL)
	{
		if (!isdigit(*entry->d_name))
			continue;

        sprintf(path, "/proc/%s/status", entry->d_name);
		if ((file2str(path, sbuf, sizeof(sbuf))) != -1)
		{
			parse_proc_status(sbuf, &p);
			if (strcmp(p.cmd, pszName) == 0 && p.state != 'Z')
			{
				pid = p.pid;
                break;
			}
		}
	}

	closedir(dir);
	return pid;
}

void KillProcess(const char *pszName)
{
    /** Window 버전에서는 지원하지 않느다 */
#ifndef _WIN32
	int		i, pid;

	pid = FindProcess(pszName);
	if (pid <= 0)
		return;

#ifdef __STATIC_LAUNCHER__
	fprintf(stderr,"Kill process %s.\r\n", pszName);
#else
	XDEBUG("Kill process %s.\r\n", pszName);
#endif
	kill(pid, SIGTERM); 		
	USLEEP(1000000);

	for(i=0; i<10; i++)
	{
		pid = FindProcess(pszName);
		if (pid <= 0)
			break;
	    kill(pid, SIGTERM); 		
		USLEEP(1000000);
	}
#endif
}

BOOL IsPid(int pid)
{
	char 	path[32], sbuf[512];
	proc_t	p;

	sprintf(path, "/proc/%d/status", pid);
	if ((file2str(path, sbuf, sizeof(sbuf))) != -1)
	{
		parse_proc_status(sbuf, &p);
		if (p.state != 'Z')
		{
            return TRUE;
		}
	}
    return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Local Functions
//////////////////////////////////////////////////////////////////////

int file2str(char *filename, char *ret, int cap)
{   
    int		fd, num_read;
    
    fd = open(filename, O_RDONLY, 0);
    if (fd > 0)
	{
    	num_read = read(fd, ret, cap - 1);
    	ret[num_read] = 0;
    	close(fd);
    	return num_read;
	}
	return -1;
}       

int file2value(char *filename)
{   
    int		n, fd;
	char	szBuffer[65];
    
    fd = open(filename, O_RDONLY, 0);
    if (fd > 0)
	{
    	n = read(fd, szBuffer, 64);
    	close(fd);
    	szBuffer[n] = 0;
    	return atoi(szBuffer);
	}
	return 0;
}       

void parse_proc_status(char *S, proc_t * P)
{
    char *tmp;
   
	memset(P, 0, sizeof(proc_t)); 
    sscanf(S, "Name:\t%15c", P->cmd);
    tmp = strchr(P->cmd, '\n');
    if (tmp)
        *tmp = '\0';
    tmp = strstr(S, "State");
    sscanf(tmp, "State:\t%c", &P->state);
    
    P->vmsize = 0;
    tmp = strstr(S, "Pid:");
    if (tmp)
        sscanf(tmp, "Pid:\t%d\n" "PPid:\t%d\n", &P->pid, &P->ppid);
        
    tmp = strstr(S, "Uid:"); 
    if (tmp)
        sscanf(tmp, "Uid:\t%d", &P->ruid);
} 

