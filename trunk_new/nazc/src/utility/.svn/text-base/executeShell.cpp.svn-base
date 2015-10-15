
/** Nuritelecom AiMiR Project.
 *
 * Execute Shell
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "aimir.h"
#include "typedef.h"
#include "if4api.h"
#include "varapi.h"
#include "Variable.h"
#include "DebugUtil.h"
#include "Utils.h"

#include "CommandHandler.h"

extern VAROBJECT	m_Root_node[];

int executeShell(char * ipaddr, int nPort, char * cmd, int timeout)
{
	CIF4Invoke		invoke(ipaddr, nPort, timeout);
	int				nError;

    invoke.AddParamFormat("1.11", VARSMI_STRING, cmd, strlen(cmd));

	nError =  Call(invoke.GetHandle(), "197.4");
    if(nError != IF4ERR_NOERROR) 
    {
        fprintf(stderr, " error=[%d %s]\n", nError, IF4API_GetErrorMessage(nError));
    }
    else 
    {
        if(invoke.GetHandle()->nResultCount>0) {
            fprintf(stdout,"%s", (char *)invoke.GetHandle()->pResultNode[0].stream.p?
                (char *)invoke.GetHandle()->pResultNode[0].stream.p : "");
        }
        fprintf(stdout,"\n");
    }

    return nError;
}

int usage(char * name)
{
    fprintf(stderr,"%s [-pt] ipaddr ...\n", name);
    fprintf(stderr,"  -p port : connection port(default 8000)\n"); 
    fprintf(stderr,"  -t timeout : timeout sec (default 10sec)\n"); 
    fprintf(stderr,"  -d : enable debug\n");
    exit(1);
}

int main(int argc, char * argv[])
{
    char *ipaddr = NULL;
    char cmd[1024] = {0,};
    int  opt,i;
    int  timeout = 10;
    int  res=0;
    int  nDebug=0;
    int  nPort = 8000;

    while((opt=getopt(argc, argv, "dp:t:")) != -1) {
        switch(opt) {
            case 'p': nPort =  (int)strtol(optarg, (char **)NULL, 10);
                break;
            case 't': timeout =  (int)strtol(optarg, (char **)NULL, 10);
                break;
            case 'd': nDebug = 1; break;
            default :
                usage(argv[0]);
        }
    }
    if(argc - optind < 1) {
        usage(argv[0]);
    }

    ipaddr = strdup(argv[optind]);

    if(nDebug) SET_DEBUG_MODE(0);

    for(i=optind+1;i<argc;i++)
    {
        strcat(cmd,argv[i]);
        strcat(cmd," ");
        if(strlen(cmd) > 127)
        {
            fprintf(stderr,"Command Length Error : Max 127 %d\n", strlen(cmd)); 
            return 1;
        }
    }

    VARAPI_Initialize(NULL, m_Root_node, FALSE);
    IF4API_Initialize(IF4_DEFAULT_PORT, NULL);

    res = executeShell(ipaddr, nPort, cmd, timeout);

    IF4API_Destroy();
    VARAPI_Destroy();

    return res;
}
