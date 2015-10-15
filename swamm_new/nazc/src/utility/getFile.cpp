
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

#include "CommandHandler.h"

extern VAROBJECT	m_Root_node[];

unsigned int getfile(char * ipaddr, int port, char * remote, char * local)
{
	CIF4Invoke		invoke(ipaddr, port, 600);
	int				nError;
    int             i,ncnt;
    unsigned int    size=0, mode;
    FILE            * fp;

    invoke.AddParamFormat("1.11", VARSMI_STRING, remote, strlen(remote));

	nError =  Call(invoke.GetHandle(), "198.1");
    if(nError) {
        fprintf(stderr, " error=[%d %s]\n", nError, IF4API_GetErrorMessage(nError));
		return -nError;
    }

    ncnt = invoke.GetHandle()->nResultCount;
    size = invoke.GetHandle()->pResult[0]->stream.u32;
    mode = invoke.GetHandle()->pResult[1]->stream.u32;

    if((fp=fopen(local,"w")) == NULL) return 0;

    for(i=3;i<ncnt;i++) {
        fwrite(invoke.GetHandle()->pResult[i]->stream.p, invoke.GetHandle()->pResult[i]->len, 1, fp);
    }
    fclose(fp);

    return size;
}

void usage(char * name)
{
    fprintf(stderr,"%s [-p] ipaddr remote local\n", name);
    fprintf(stderr,"  -p port : connection port(default 8000)\n"); 
    exit(1);
}


int main(int argc, char * argv[])
{
    char *ipaddr = NULL;
    char *remote = NULL;
    char *local = NULL;
    int  nPort = 8000;
    int  opt;

    while((opt=getopt(argc, argv, "p:")) != -1) {
        switch(opt) {
            case 'p': nPort =  (int)strtol(optarg, (char **)NULL, 10);
                break;
            default :
                usage(argv[0]);
        }
    }
    if(argc - optind < 3) {
        usage(argv[0]);
    }

    ipaddr = strdup(argv[optind]);
    remote = strdup(argv[optind+1]);
    local = strdup(argv[optind+2]);

    VARAPI_Initialize(NULL, m_Root_node, FALSE);
    IF4API_Initialize(IF4_DEFAULT_PORT, NULL);

    fprintf(stdout,"get file : target=%s, port=%d, remote=%s, local=%s ", 
            ipaddr, nPort, remote, local);
    fflush(stdout);
    fprintf(stdout,"length=%d\n", getfile(ipaddr, nPort, remote, local));

    IF4API_Destroy();
    VARAPI_Destroy();

    return 0;
}
