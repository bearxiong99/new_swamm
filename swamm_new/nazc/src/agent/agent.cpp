#include "common.h"
#include "common/CommonUtil.h"
#include "AgentService.h"
#include "DebugServer.h"
#include "version.h"
#include "copyright.h"

extern int	m_nDebugLevel;

void zombie_handler(int nSignal)
{
    while (waitpid(-1, NULL, WNOHANG) > 0) { usleep(1000); }
}

void segmentfault_handler(int nSignal)
{
    printf("------------ SEGMENT FAULT ------------\r\n\r\n");
    if(m_pSystemMonitor != NULL)
    {
        m_pSystemMonitor->ResetSystem();
    }
}

int xproc_signal(int nSignalNo, void (*pfHandler)(int))
{
    struct sigaction sSetAction, sGetAction;

    sSetAction.sa_handler = pfHandler;
    sigemptyset(&sSetAction.sa_mask);
    sSetAction.sa_flags = 0;

/* for SunOS */
#ifdef SA_INTERRUPT
    sSetAction.sa_flags |= SA_INTERRUPT;
#endif

    if(sigaction(nSignalNo, &sSetAction, &sGetAction) < 0)
        return -1;
    else
        return 0;
}

void no_cld_wait(void)
{
#if     !defined(__CYGWIN__)
    struct sigaction ac;

    ac.sa_handler = SIG_IGN;
    sigemptyset(&ac.sa_mask);
    ac.sa_flags = SA_NOCLDWAIT;

    if(sigaction(SIGCHLD, &ac, NULL)<0) {
        printf("sigaction fail\r\n");
    }
#endif
}

void usage(char * name)
{
    fprintf(stderr,"%s [-dh]\n", name);
    fprintf(stderr,"    -d : enable console debug message\n");
    fprintf(stderr,"    -h : display this message\n");
    exit(1);
}

int main(int argc, char **argv)
{
    char szVersionString[255] = {0,};
    cetime_t t;
    int opt;

    time(&t);
    srandom((unsigned int)t);

    GetVersionString(szVersionString);
    printf("AIMIR MCU Agent 4.3 sweden %s\r\n", szVersionString);
    printf(COPYRIGHT "\r\n");

    no_cld_wait();
    signal(SIGTERM, SIG_IGN);
//  xproc_signal(SIGCHLD, zombie_handler);
    xproc_signal(SIGSEGV, segmentfault_handler);

    while((opt=getopt(argc, argv, "dh")) != -1) {
        switch(opt) {
            case 'd': m_nDebugLevel = 0; break;
            case 'h':
            default :
                usage(argv[0]);
        }
    }

    CAgentService    svc;
    svc.Startup();
    svc.Shutdown();
}
