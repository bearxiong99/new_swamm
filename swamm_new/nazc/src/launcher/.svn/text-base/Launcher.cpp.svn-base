#include "common.h"
#include "LauncherService.h"

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

    if(rlim.rlim_cur != RLIM_INFINITY) {
        rlim.rlim_max = RLIM_INFINITY;
        rlim.rlim_cur = RLIM_INFINITY;
        if(setrlimit(RLIMIT_NPROC, &rlim)) {
            printf("LIMIT CONTROL: SET LIMIT FAIL\r\n");
        }
    }
#endif
}

int main(int argc, char **argv)
{
	CLauncherService	theService;

    proc_limit();

	if (!theService.Startup(argc, argv))
		return 0;
	theService.Shutdown();

	return 0;
}
