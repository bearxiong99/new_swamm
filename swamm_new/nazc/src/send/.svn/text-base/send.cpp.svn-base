#include "common.h"
#include "MobileClient.h"
#include "Transfer.h"
#include "DebugUtil.h"

BOOL	m_bExitSignalPending = FALSE;

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
	CMobileClient	client;
	CTransfer		ts;
    struct 	sigaction   handler;
	BOOL	bCompress = FALSE;

//	m_nDebugLevel = 0;

	if ((argc != 6) && (argc != 7) && (argc != 8))
	{
		printf("usage: send <type> <nzc> <port> <filename> [<compress>]\r\n");
		printf("     <type>       GSM/CDMA\r\n");
		printf("     <nzc>        NZC Number\r\n");
		printf("     <port>       Server Port Number\r\n");
		printf("     <filename>   File Name\r\n");
		printf("     <time>       Try time (min)\r\n");
		printf("     <compress>   0:OFF, 1:ON\r\n");
		printf("     <debug>      0:OFF, 1:ON\r\n");
		return 0;
	}

	if (argc == 7)
		bCompress = atoi(argv[6]);

	if (argc == 8)
		m_nDebugLevel = atoi(argv[7]) == 1 ? 0 : 1;

    // Set Interrrupt Signal Handler
	memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_handler = SignalCatcher;
    sigfillset(&handler.sa_mask);
	sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);

	if (client.Initialize())
	{
		ts.Transfer(&client, argv[1], atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5])*60, bCompress);
		ts.SetupModem(&client);
	//	XDEBUG("CIRCUIT: Sending 'ATH'\r\n");
	//	client.WriteToModem("ATH\r\n");
	//	usleep(1000000);
	//	XDEBUG("CIRCUIT: Sending 'ATS0=1'\r\n");
	//	client.WriteToModem("ATS0=1\r\n\r\n");
	//	usleep(1000000);

		client.Destroy();
	}
	return 0;
}

