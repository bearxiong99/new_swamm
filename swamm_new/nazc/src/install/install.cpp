#include "common.h"
#include "kbhit.h"

int	m_bExitSignalPending = 0;
int m_bFirmwareInstalled = 0;
struct  termios oldtio;

void SignalCatcher(int nSignal)
{
	switch(nSignal) {
	  case SIGINT :
	  case SIGTERM :
		   break;
	}
}

void display_menu()
{
    m_bFirmwareInstalled = !access("/app/sw/launcher", R_OK);

	printf("\r\n");
	printf("  AIMIR MCU Firmware Install Menu\r\n");
	printf("-----------------------------------\r\n");
	printf("\r\n");
    if(!m_bFirmwareInstalled)
	    printf("      1. MCU Firmware Install\r\n");
    else
	    printf("      2. MCU H/W Test\r\n");
	printf("\r\n");
	printf("      0. Exit Menu\r\n");
	printf("\r\n");
	printf("-----------------------------------\r\n");
	printf("      Select : ");
	fflush(0);
}

int main(int argc, char **argv)
{
    struct 	sigaction   handler;
	char	num[100];
	int		n = 0;

    // Set Interrrupt Signal Handler
	memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_handler = SignalCatcher;
    sigfillset(&handler.sa_mask);
	sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);

    tcgetattr(0, &oldtio);
    init_keyboard();

	for(;!m_bExitSignalPending;)
	{
		display_menu();

		memset(num, 0, 100);
        n = readch();
		printf("\r\n");

		switch(n) {
		  case '0' :	
			   m_bExitSignalPending = 1;
			   break;

		  case '1' :
               if(m_bFirmwareInstalled) break;
			   printf("\r\n");
               system("/usr/bin/tftp -g -r factoryInstall 192.168.10.20");
               system("chmod +x /tmp/factoryInstall");
               system("/tmp/factoryInstall factory");
			   printf("\r\n");
			   break;

		  case '2' :
               if(!m_bFirmwareInstalled) break;
			   system("/app/sw/hwtest");
			   break;
		}
	}

    close_keyboard();

	return 0;
}

