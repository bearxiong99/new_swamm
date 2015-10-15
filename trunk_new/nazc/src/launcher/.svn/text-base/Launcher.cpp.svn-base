#include "common.h"
#include "LauncherService.h"

#ifndef RLIM_INFINITY
#define PROC_USER_MAX	1024
#else
#define PROC_USER_MAX	RLIM_INFINITY
#endif

void proc_limit(void)
{
#ifdef  RLIMIT_NPROC
    struct rlimit rlim;

    memset(&rlim,0,sizeof(struct rlimit));

    if(getrlimit(RLIMIT_NPROC,&rlim)) {
        printf("LIMIT CONTROL: GET LIMIT FAIL\r\n");
        return;
    }

    printf("NPROC max=%d cur=%d\r\n", (int) rlim.rlim_max, (int) rlim.rlim_cur);

    if(rlim.rlim_cur < PROC_USER_MAX) {
           rlim.rlim_max = PROC_USER_MAX;
           rlim.rlim_cur = PROC_USER_MAX;
           if(setrlimit(RLIMIT_NPROC, &rlim)) {
               printf("LIMIT CONTROL: SET LIMIT FAIL\r\n");
           }
       }
    if(getrlimit(RLIMIT_NPROC,&rlim)) {
            printf("LIMIT CONTROL: GET LIMIT FAIL\r\n");
            return;
        }

        printf("NPROC max=%d cur=%d\r\n", (int) rlim.rlim_max, (int) rlim.rlim_cur);

#endif
}

#if 0
void zombie_handler(int nSignal)
{
    while (waitpid(-1, NULL, WNOHANG) > 0) { usleep(1000); }
}
#endif

void segmentfault_handler(int nSignal)
{
    printf("------------ SEGMENT FAULT ------------\r\n\r\n");
    system("sync;sync;sync");
    system("reboot");
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

int main(int argc, char **argv)
{
	CLauncherService	theService;

    proc_limit();

    xproc_signal(SIGSEGV, segmentfault_handler);


	if (!theService.Startup(argc, argv))
		return 0;
	theService.Shutdown();

	return 0;
}
