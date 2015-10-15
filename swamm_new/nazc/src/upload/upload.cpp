#include "common.h"
#include "UploadService.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

void usage(char * name)
{
    fprintf(stderr,"%s [-dhnp] ipaddr filename\n", name);
    fprintf(stderr,"    -d               : enable debug message\n");
    fprintf(stderr,"    -n nzcnumber     : nzc number(default 0)\n");
    fprintf(stderr,"    -p port          : connection port(default 8000)\n");
    fprintf(stderr,"    -h               : display this message\n");
    exit(1);
}

int main(int argc, char **argv)
{
	//UINT		nCheckPoint;
	BOOL	bOK = TRUE;
    BOOL    bDebug = FALSE;
    int     nPort = 8000;
    UINT    nNzcNumber = 0;
    char    ipaddr[64] = {0,};
    char    fname[512] = {0,};
    int     opt;

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

    if((argc - optind) < 2) {
        usage(argv[0]);
    }
    strncpy(ipaddr, argv[optind], MIN(sizeof(ipaddr)-1, strlen(argv[optind])));
    strncpy(fname, argv[optind+1], MIN(sizeof(fname)-1, strlen(argv[optind+1])));

	//CheckPoint(&nCheckPoint);
	CUploadService svc(nNzcNumber, ipaddr, nPort, fname, bDebug);

	if (!svc.Startup()) bOK = FALSE;
	svc.Shutdown();

	//Difference(nCheckPoint);

	return bOK ? 0 : -1;
}
