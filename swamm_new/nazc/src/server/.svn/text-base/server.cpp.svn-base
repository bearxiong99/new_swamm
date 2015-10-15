#include "common.h"
#include "copyright.h"
#include "McuService.h"

#include "DebugUtil.h"

extern int	m_nDebugLevel;
extern int  m_nServerPort;
BOOL m_bKetiDemo = FALSE;

int usage(char * name)
{
    fprintf(stderr,"%s [-dph]\n", name);
    fprintf(stderr,"  -k : keti demo\n");
    fprintf(stderr,"  -p port : listen port\n"); 
    fprintf(stderr,"  -d : enable debug\n");
    fprintf(stderr,"  -h : print this message\n");
    exit(1);
}

int main(int argc, char **argv)
{
    int  opt;

    while((opt=getopt(argc, argv, "kdp:")) != -1) {
        switch(opt) {
            case 'k': 
                m_bKetiDemo = TRUE;
                break;
            case 'p': 
                m_nServerPort = (int)strtol(optarg, (char **)NULL, 10);
                break;
            case 'd': 
                SET_DEBUG_FILE(stderr);
                SET_DEBUG_MODE(0);
                break;
            default :
                usage(argv[0]);
        }
    }

	CMcuService		svc;

    XDEBUG("Debug Server Start...\r\n");

	svc.Startup();
	svc.Shutdown();
}
