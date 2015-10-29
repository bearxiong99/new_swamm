#include "common.h"
#include "copyright.h"
#include "MobileClient.h"
#include "DebugUtil.h"
#include "gpiomap.h"

#define STATE_INIT				0
#define STATE_ATZ				1
#define STATE_CRM				2
#define STATE_DIAL				3
#define STATE_SELECT			4
#define STATE_ACCEPT			5
#define STATE_DONE				100
#define STATE_TRY_CONNECT		200
#define STATE_SELECT_SVC		201
#define STATE_EMMUL				202

#define CTRL_C					0x03
#define CTRL_D					0x04

#define DEVICE_TYPE_GSM			0
#define DEVICE_TYPE_CDMA		1
#define DEVICE_TYPE_SINK		2
#define DEVICE_TYPE_GPS			3

BOOL	m_bExitSignalPending = FALSE;

char	m_szDevice[32] 	= "/dev/ttySO1";
int		m_nDeviceType 	= DEVICE_TYPE_GSM;
int		m_nDisplayMode 	= DISPLAY_MODE_ASCII;
int		m_nSpeed 		= B115200;
int		m_nOption 		= 0;

void Usage()
{
	printf("Usage: mterm [<option> <device> <speed>]\r\n");
	printf("     device      Device name (default =/dev/ttySO1) ex), /dev/ttyS00, /dev/ttySO1, ..)\r\n");
	printf("     speed       Communication serial baud rate ((default =115200   ex, 1200, .., 38400, 115200)\r\n");
	printf("\r\n");
	printf("     -gsm        Mobile GSM setting.\r\n");
	printf("     -cdma       Mobile CDMA setting.\r\n");
	printf("     -sink       Zigbee setting.\r\n");
	printf("     -gps        GPS setting.\r\n");
	printf("\r\n");
	printf("     -rtscts     Use RTS/CTS signal.\r\n");
	printf("     -hupcl      Use hangup signal.\r\n");
	printf("\r\n");
	printf("example:\r\n");
	printf("     GSM/CDMA    mterm -rtscts -hupcl\r\n");
	printf("                 mterm -gsm\r\n");
	printf("                 mterm -cdma\r\n");
	printf("     SINK        mterm /dev/ttyS01 38400\r\n");
	printf("                 mterm -sink\r\n");
	printf("     GPS         mterm /dev/ttyS03 9600\r\n");
	printf("                 mterm -gps\r\n");
	printf("\r\n");
}

void Splash()
{
	printf("\033[H\033[J");
	printf("Welcome to mterm for SWAMM sungyeung 1.1.1 2015-10-28\r\n");
	printf(COPYRIGHT "\r\n");
    if(m_nDeviceType != DEVICE_TYPE_SINK) {
	    printf("\r\n"); 
	    printf("F1=AT Command Test, F2=PPP Connect Test, F3=PPP Disconnect, F4=Exit\r\n"); 
    }
	printf("\r\n"); 
	printf("Press CTRL+C then exit.\r\n"); 
	printf("\r\n"); 
}

void InitGpio()
{   

}

void Disconnect()
{    

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

int Terminal(CMobileClient *pClient)
{
	struct	termios	oldset;
	struct	termios	newset;
	char	szBuffer[256];
	int		i, n, fd;
	BYTE	c;

	tcgetattr(0, &oldset);
	memcpy(&newset, &oldset, sizeof(newset));
    newset.c_cc[VTIME]  = 10;              // Timeout TIME*0.1
    newset.c_cc[VMIN]   = 0;
    newset.c_cc[IGNBRK] = 1;
    newset.c_cc[BRKINT] = 0;
    newset.c_lflag &= (~ECHO);
    newset.c_lflag &= (~ICANON);
	tcsetattr(0, TCSANOW, &newset);

	// sungyeung 2015-10-28
	//
	if ((m_nDeviceType == DEVICE_TYPE_CDMA) || (m_nDeviceType == DEVICE_TYPE_GSM))
		pClient->WriteToModem(const_cast<char *>("ATZ\r\n"), 5); // ATZ for rest GSM module



	//modified by sungyeung
	for(;!m_bExitSignalPending;)
	{
	   n = read(0, szBuffer, 255);
	   if (n > 0)
	   {
		   if ((szBuffer[0] == 0x1b) && (szBuffer[1] == 0x4F))
		   {
			   switch(szBuffer[2]) {
				 case 0x50 : // if hit F1 key
					  printf("\r\n------------ AT COMMAND TEST ----------for GE910\r\n");
					  printf("\r\n--- Display additional identification information : ATI ???\r\n");
					  if (m_nDeviceType != DEVICE_TYPE_CDMA)
					  {
						  pClient->WriteToModem(const_cast<char *>("ATI\r\n"), 4);
						  usleep(500000);

						  printf("\r\n--- Display SIM card identification number : AT+CCID\r\n");
						  pClient->WriteToModem(const_cast<char *>("AT+CCID\r\n"), 9);
						  usleep(500000);

						  printf("\r\n--- Enter PIN (SIM PIN authentication) : AT+CPIN?\r\n");
						  pClient->WriteToModem(const_cast<char *>("AT+CPIN?\r\n"), 10);
						  usleep(500000);

						  printf("\r\n--- Enter PIN2 : AT+CPIN2?\r\n");
						  pClient->WriteToModem(const_cast<char *>("AT+CPIN2?\r\n"), 11);
						  usleep(500000);

						  printf("\r\n--- Request international mobile subscriber identity : AT+CIMI\n");
						  pClient->WriteToModem(const_cast<char *>("AT+CIMI\r\n"), 9);
						  usleep(500000);

						  printf("\r\n--- Real Time Clock : AT+CCLK?\n");
						  pClient->WriteToModem(const_cast<char *>("AT+CCLK?\r\n"), 10);
						  usleep(500000);

						  printf("\r\n--- Operator selection : AT+COPS?\n");
						  pClient->WriteToModem(const_cast<char *>("AT+COPS?\r\n"), 10);
						  usleep(500000);

						  printf("\r\n--- Network registration : AT+CREG?\n");
						  pClient->WriteToModem(const_cast<char *>("AT+CREG?\r\n"), 10);
						  usleep(500000);

						  printf("\r\n--- Signal quality(0=-113dBm, 1=-111dBm, .. 30:-53dBm, ~99) : AT+CSQ\n");
						  pClient->WriteToModem(const_cast<char *>("AT+CSQ\r\n"), 8);
						  usleep(500000);

						  printf("\r\n--- Monitoring power supply : AT^SBV\n");   //??? unqualified AT Command
						  pClient->WriteToModem(const_cast<char *>("AT^SBV\r\n"), 8);
						  usleep(500000);

						  printf("\r\n--- Monitor idle mode and dedicated mode : AT^MONI\n");
						  pClient->WriteToModem(const_cast<char *>("AT^MONI\r\n"), 9);
						  usleep(1000000);

						  printf("\r\n--- Monitor neighbour cells : AT^MONP\n");
						  pClient->WriteToModem(const_cast<char *>("AT^MONP\r\n"), 9);
						  usleep(1000000);

						  printf("\r\n--- Cell Monitoring : AT^SMONC\n");
						  pClient->WriteToModem(const_cast<char *>("AT^SMONC\r\n"), 10);
						  usleep(1000000);

						  printf("\r\n--- GPRS Monitor : AT^SMONG\n");
						  pClient->WriteToModem(const_cast<char *>("AT^SMONG\r\n"), 10);
					  }
					  else
					  {
	                      printf("\r\n--- Manufacturer\r\n"); 
	                      pClient->WriteToModem(const_cast<char *>("AT+GMI\r\n"), 8);
	                      usleep(500000);
                      
	                      printf("\r\n--- Device Model\r\n"); 
	                      pClient->WriteToModem(const_cast<char *>("AT+GMM\r\n"), 8);
	                      usleep(500000);
                      
	                      printf("\r\n--- Version, Revision Level, or Date\r\n");
	                      pClient->WriteToModem(const_cast<char *>("AT+GMR\r\n"), 8);
	                      usleep(500000);
                      
	                      printf("\r\n--- Device Identification (ESN)\r\n");
	                      pClient->WriteToModem(const_cast<char *>("AT+GSN\r\n"), 8);
	                      usleep(500000);
                      
	                      printf("\r\n--- Mobile Number\r\n"); 
	                      pClient->WriteToModem(const_cast<char *>("AT+MIN?\r\n"), 9);
	                      usleep(500000);
                      
	                      printf("\r\n--- Real Time Clock\n");
	                      pClient->WriteToModem(const_cast<char *>("AT+TIME?\r\n"), 10);
	                      usleep(500000);
                      
	                      printf("\r\n--- Signal quality (0~31=Signal Quality, 99: not detectable)\r\n");
	                      pClient->WriteToModem(const_cast<char *>("AT+CSQ?\r\n"), 9);
	                      usleep(500000);
					  }
					  usleep(1000000);
					  break;

				 case 0x51 :  //if hit F2 key
					  if (m_nDeviceType != DEVICE_TYPE_CDMA)
					  {
						  printf("\r\n------------ PPP Try ----------for GE910\r\n");
						  pClient->WriteToModem(const_cast<char *>("AT+CGDCONT=1,\"IP\",\"mdamip\",,0,0\r\n"), 33);
						  usleep(500000);
						  pClient->WriteToModem(const_cast<char *>("ATD*99***1#\r\n"), 13);
					  }
					  else
					  {
	                      printf("\r\n------------ PPP Try ----------\r\n");
	                      pClient->WriteToModem(const_cast<char *>("AT+CRM=1\r\n"), 10);
	                      usleep(500000);
	                      pClient->WriteToModem(const_cast<char *>("ATDT 1501\r\n"), 11);
					  }
					  break;

				 case 0x52 :  // if hit F3 key
                      printf("\r\n-------- PPP Disconnect -------\r\n");
                      /** TODO TI 에서 이 부분에 대한 Code가 추가되어야 한다 */
#if !defined(__TI_AM335X__)
                      GPIOAPI_WriteGpio(GP_GSM_DTR_OUT, GPIO_HIGH);
                      usleep(1000000);
                      GPIOAPI_WriteGpio(GP_GSM_DTR_OUT, GPIO_LOW);
#endif	
                      printf("Wait for disconnect.\r\n");
                      usleep(3000000);
                      pClient->WriteToModem(const_cast<char *>("ATZ\r\n"), 5);
					  break;

				 case 0x53 :
				      m_bExitSignalPending = TRUE;
					  break;
			   }
			   continue;
		   }

		   for(i=0; i<n; i++)
		   {
			   c = (BYTE)szBuffer[i];
			   if ((c == 0x0d) || (c == 0xa))
			   {
		   		   pClient->WriteToModem(const_cast<char *>("\r"), 1);
				   continue;
			   }

			   if (c == CTRL_C)
			   {
				   m_bExitSignalPending = TRUE;
				   break;
			   }
		   }

		   pClient->WriteToModem(szBuffer, n);
	   }
	}

	Disconnect();

	tcsetattr(0, TCSANOW, &oldset);
	printf("\r\n");
	printf("Connection closed.\r\n");
	return 0;
}

BOOL CheckParam(int argc, char **argv)
{
	int		i;

	for(i=1; i<argc; i++)
	{
		if (atoi(argv[i]) == 1200)
			m_nSpeed = B1200;
		else if (atoi(argv[i]) == 2400)
			m_nSpeed = B2400;
		else if (atoi(argv[i]) == 4800)
			m_nSpeed = B4800;
		else if (atoi(argv[i]) == 9600)
			m_nSpeed = B9600;
		else if (atoi(argv[i]) == 38400)
			m_nSpeed = B38400;
		else if (atoi(argv[i]) == 57600)
			m_nSpeed = B57600;
		else if (atoi(argv[i]) == 115200)
			m_nSpeed = B115200;
		else if (strncmp(argv[i], "/dev/tty", 8) == 0)
			strcpy(m_szDevice, argv[i]);
		else if ((strcmp(argv[i], "-rtscts") == 0) || (strcmp(argv[i], "-RTSCTS") == 0))
			m_nOption |= CRTSCTS;
		else if ((strcmp(argv[i], "-hupcl") == 0) || (strcmp(argv[i], "-HUPCL") == 0))
			m_nOption |= HUPCL;
		else if ((strcmp(argv[i], "-gsm") == 0) || (strcmp(argv[i], "-GSM") == 0))
		{
			m_nSpeed 	  = B9600;
			m_nOption 	 |= (CRTSCTS | HUPCL);
			m_nDeviceType = DEVICE_TYPE_GSM;
			strcpy(m_szDevice, "/dev/ttyS00");
		}
		else if ((strcmp(argv[i], "-cdma") == 0) || (strcmp(argv[i], "-CDMA") == 0))
		{
			m_nSpeed 	  = B115200;
			m_nOption 	 |= (CRTSCTS | HUPCL);
			m_nDeviceType = DEVICE_TYPE_CDMA;
			strcpy(m_szDevice, "/dev/ttyS00");
		}
		else if ((strcmp(argv[i], "-sink") == 0) || (strcmp(argv[i], "-SINK") == 0))
		{
			m_nOption 	  = 0;
			m_nSpeed 	  = B115200;
			m_nDeviceType = DEVICE_TYPE_SINK;
			strcpy(m_szDevice, "/dev/ttyS01");
		}
		else if ((strcmp(argv[i], "-gps") == 0) || (strcmp(argv[i], "-GPS") == 0))
		{
			m_nOption 	  = 0;
			m_nSpeed 	  = B9600;
			m_nDeviceType = DEVICE_TYPE_GPS;
			strcpy(m_szDevice, "/dev/ttyS03");
		}
		else return FALSE;
	}
	return TRUE;
}

int main(int argc, char **argv)
{
	CMobileClient	client;
    struct 	sigaction   handler;

    // Set Interrrupt Signal Handler
	memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_handler = SignalCatcher;
    sigfillset(&handler.sa_mask);
	sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);

	if (!CheckParam(argc, argv))
	{
		Usage();
		exit(0);
	}

	InitGpio();
	Splash();

	if (!client.Initialize(m_szDevice, m_nSpeed, m_nOption, m_nDisplayMode))
		return 0;

	Terminal(&client);
	client.Destroy();

	Disconnect();
	return 0;
}
