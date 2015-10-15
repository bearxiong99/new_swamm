#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "DbUploadService.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

void usage(char * name)
{
    fprintf(stderr,"%s [-dhnp] ipaddr\n", name);
    fprintf(stderr,"    -d               : enable debug message\n");
    fprintf(stderr,"    -n nzcnumber     : nzc number(default 0)\n");
    fprintf(stderr,"    -p port          : connection port(default 8000)\n");
    fprintf(stderr,"    -h               : display this message\n");
    exit(0);
}

int main(int argc, char **argv)
{
	UINT		nCheckPoint;
    BOOL    bDebug = FALSE;
    int     nPort = 8000;
    UINT    nNzcNumber = 0;
    char    ipaddr[64] = {0,};
    int     opt;
    int     ret=0;

    while((opt=getopt(argc, argv, "dhn:p:")) != -1) {
        switch(opt) {
            case 'd': bDebug = TRUE; break;
            case 'p': nPort = (int)strtol(optarg, (char **)NULL, 10); break;
            case 'n': nNzcNumber = strtol(optarg, (char **)NULL, 10); break;
            case 'h':
            default :
                usage(argv[0]);
        }
    }

    if((argc - optind) < 1) {
        usage(argv[0]);
    }
    strncpy(ipaddr, argv[optind], MIN(sizeof(ipaddr)-1, strlen(argv[optind])));

	CheckPoint(&nCheckPoint);
	CDbUploadService svc(nNzcNumber, ipaddr, nPort, bDebug);

	ret = svc.Startup();
	svc.Shutdown();

	Difference(nCheckPoint);
	return ret>0?1:0;
}
