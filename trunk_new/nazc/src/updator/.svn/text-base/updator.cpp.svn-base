#include <unistd.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include "copyright.h"
#include "UpdateService.h"
#include "DebugUtil.h"

BOOL	m_bExitSignalPending = FALSE;
BOOL	m_bEnableInstall = FALSE;
int		m_nPacketSize = 4096;
int		m_nBaudRate = 115200;
char	m_szTtyName[64] = "/dev/ttyS0";
int		m_nMobileType	= MOBILE_CDMA;
char	*m_pRemoteFile = const_cast<char*>("/tmp/updator.file");

void Usage(char * filename)
{
	printf("Usage: %s [-bifmpth] <Phonenum> <filename>\r\n", filename);
	printf("   Phonenum      Remote mobile phone number.\r\n");
	printf("   filename      Filename.\r\n");
	printf(" \r\n");
	printf("         -b      Baud rate (default -b115200).\r\n");
	printf("         -i      Enable install.\r\n");
	printf("         -f      Remote file name (default -f/tmp/updator.file).\r\n");
	printf("         -m      Mobile type. cdma or gsm (default -mcdma).\r\n");
	printf("         -p      Packet size (default -p4096).\r\n");
	printf("         -t      Device (default -t/dev/ttyS0).\r\n");
	printf("\r\n");
}

BOOL ParseParameters(int argc, char **argv)
{
    int opt, n;

    while((opt=getopt(argc, argv, "b:if:m:p:t:h")) != -1) {
        switch(opt) {
            case 'b': 
                n = (int)strtol(optarg,(char **)NULL,10);
                if ((n <= 115200) && (n >= 1200)) {
                    m_nBaudRate = n;
                }
                break;
            case 'i': 
                m_bEnableInstall = TRUE;
                break;
            case 'f': 
                m_pRemoteFile = strdup(optarg);
                break;
            case 'm': 
                if(strncasecmp(optarg, "gsm", 3)==0) {
                    m_nMobileType = MOBILE_GSM;
                }
                break;
            case 'p': 
                n = (int)strtol(optarg,(char **)NULL,10);
                if ((n <= 4096) && (n >= 100)) {
                    m_nPacketSize = n;
                }
                break;
            case 't': 
				strcpy(m_szTtyName, optarg);
                break;
            case 'h':
            default :
                Usage(argv[0]);
                return FALSE;
        }
    }
	return TRUE;
}

void Splash()
{
	printf("AIMIR Mobile Firmware Updator v1.1\r\n");
	printf(COPYRIGHT "\r\n");
	printf("\r\n"); 
}

void SignalCatcher(int nSignal)
{
	switch(nSignal) {
	  case SIGINT :
	  case SIGTERM :
		   m_bExitSignalPending = TRUE;
		   break;
	}
}

int main(int argc, char **argv)
{
	CUpdateService		svc;
    struct 	sigaction   handler;
	int					fd;

	Splash();

	if (!ParseParameters(argc, argv))
	{
		return 0;	
	}

	if ((argc - optind) < 2)
	{
		Usage(argv[0]);
		return 0;	
	}


    // Set Interrrupt Signal Handler
	memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_handler = SignalCatcher;
    sigfillset(&handler.sa_mask);
	sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);

	fd = open("lock.monet", O_CREAT, 0644);
	if (flock(fd, LOCK_EX | LOCK_NB) != 0)
	{	
		printf("이미 다른 사람이 사용 중 입니다.\r\n");
		printf("\r\n");
		exit(0);
	}

	svc.Updator(argv[optind+0], argv[optind+1]);

	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}
