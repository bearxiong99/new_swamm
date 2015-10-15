
/** Nuritelecom AiMiR Project.
 *
 * Put File
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>

#include "aimir.h"
#include "typedef.h"
#include "version.h"
#include "config.h"
#include "if4api.h"
#include "varapi.h"
#include "mcudef.h"
#include "Utils.h"
#include "Variable.h"
#include "DebugUtil.h"

#include "CommandHandler.h"

extern VAROBJECT	m_Root_node[];

unsigned int putfile(char * ipaddr, char * filename, char * path, unsigned int mode)
{
	CIF4Invoke		invoke(ipaddr, 8000, 600);
	int				nError;
    int             len,fd;
    unsigned int    size, totallen=0;
    struct stat     statbuf;
    TIMESTAMP       timestamp;
    char            buffer[2048];
    int             fcnt=0;

    fd = open(filename, O_RDONLY);
    if (fd <= 0)
        return fd;
    fstat(fd, &statbuf);

    size = (unsigned int) statbuf.st_size;

    memset(&timestamp, 0, sizeof(TIMESTAMP));

    if(path) {
        invoke.AddParamFormat("1.11", VARSMI_STRING, path, strlen(path));
    } else {
        invoke.AddParamFormat("1.11", VARSMI_STRING, filename, strlen(filename));
    }
    invoke.AddParam("1.6", size);
    invoke.AddParam("1.6", mode);
    invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &timestamp, sizeof(TIMESTAMP));

    while((len=read(fd,buffer,sizeof(buffer)))>0) {
        invoke.AddParamFormat("1.12", VARSMI_STREAM, buffer, len);
        totallen += len;
        fcnt ++;
    }

    close(fd);

    fprintf(stdout, "put file : size=%d, read size=%d frame cnt=%d\n", size, totallen, fcnt);

	nError =  Call(invoke.GetHandle(), "198.2");
    if(nError) {
        fprintf(stderr, " error=[%d %s]\n", nError, IF4API_GetErrorMessage(nError));
		return -nError;
    }

    return size;
}

int usage(char * name)
{
    fprintf(stderr,"%s [-t] ipaddr filename\n", name);
    fprintf(stderr,"  -t path : target file path\n"); 
    fprintf(stderr,"  -d : enable debug\n");
    exit(1);
}

int main(int argc, char * argv[])
{
    char *ipaddr = NULL;
    char *filename = NULL;
    char *path = NULL;
    unsigned int mode = S_IRWXU | S_IRWXG | S_IRWXO;
    int  opt;
    int  nDebug=0;

    while((opt=getopt(argc, argv, "dt:")) != -1) {
        switch(opt) {
            case 't': path =  strdup(optarg);
                break;
            case 'd': nDebug = 1; break;
            default :
                usage(argv[0]);
        }
    }
    if(argc - optind < 2) {
        usage(argv[0]);
    }

    if(nDebug) SET_DEBUG_MODE(0);

    ipaddr = strdup(argv[optind]);
    filename = strdup(argv[optind+1]);

    VARAPI_Initialize(NULL, m_Root_node, FALSE);
    IF4API_Initialize(IF4_DEFAULT_PORT, NULL);

    fprintf(stdout,"put file : target=%s, size=%d, file=%s, path=%s\n", 
            ipaddr, putfile(ipaddr, filename, path, mode), filename, path?path:"NULL");

    IF4API_Destroy();
    VARAPI_Destroy();

    return 0;
}
