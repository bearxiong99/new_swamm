
/** Nuritelecom AiMiR Project.
 *
 * Merge And Install
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

int mergeAinstall(char *ipaddr, char *orig, char *nfile, char *diff, char *script, BOOL reset)
{
	CIF4Invoke		invoke(ipaddr, 8000, 600);
	int				nError;

    invoke.AddParamFormat("1.11", VARSMI_STRING, diff, strlen(diff));
    invoke.AddParamFormat("1.11", VARSMI_STRING, orig, strlen(orig));
    invoke.AddParamFormat("1.11", VARSMI_STRING, nfile, strlen(nfile));

    if(script || reset) {
        invoke.AddParamFormat("1.11", VARSMI_STRING, script, script ? strlen(script) : 0);
    }
    if(reset) {
        invoke.AddParam("1.3", reset);
    }

	nError =  Call(invoke.GetHandle(), "198.12");
    if(nError) {
        fprintf(stderr, " error=[%d %s]\n", nError, IF4API_GetErrorMessage(nError));
		return -nError;
    }

    return 0;
}

int usage(char * name)
{
    fprintf(stderr,"%s [-rs] ipaddr orig new diff\n", name);
    fprintf(stderr,"  -r : reset system\n"); 
    fprintf(stderr,"  -s script : install script\n"); 
    exit(1);
}

int main(int argc, char * argv[])
{
    char *ipaddr = NULL;
    char *orig = NULL;
    char *nfile = NULL;
    char *diff = NULL;
    char *script = NULL;
    BOOL bReset = FALSE;
    int  opt;

    while((opt=getopt(argc, argv, "rs:")) != -1) {
        switch(opt) {
            case 's': script =  strdup(optarg);
                break;
            case 'r': bReset = TRUE;
                break;
            default :
                usage(argv[0]);
        }
    }
    if(argc - optind < 4) {
        usage(argv[0]);
    }

    ipaddr = strdup(argv[optind]);
    orig = strdup(argv[optind+1]);
    nfile = strdup(argv[optind+2]);
    diff = strdup(argv[optind+3]);

    VARAPI_Initialize(NULL, m_Root_node, FALSE);
    IF4API_Initialize(IF4_DEFAULT_PORT, NULL);

    fprintf(stdout,"merge file : ipaddr=%s, orig=%s, new=%s, diff=%s, script=%s, reset=%s\n",
            ipaddr, orig, nfile, diff, script?script:"NULL", bReset?"TRUE":"FAILSE");
    fprintf(stdout," result = %s\n",
            mergeAinstall(ipaddr, orig, nfile, diff, script, bReset)?"FAIL":"SUCC");

    IF4API_Destroy();
    VARAPI_Destroy();

    return 0;
}
