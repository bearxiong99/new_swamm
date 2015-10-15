#include <unistd.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include "copyright.h"
#include "MobileClient.h"
#include "PrivateProfile.h"
#include "DebugUtil.h"

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

#define MOBILE_GSM				0
#define	MOBILE_CDMA				1

char	m_szDevice[64]			= "/dev/ttyS0";
char	m_szNumber[64]			= "";
int		m_nMobileType			= MOBILE_GSM;
int		m_nSpeed				= B9600;
BOOL	m_bExitSignalPending 	= FALSE;
char	m_szSyntax[20]			= "0123456789";

void SignalCatcher(int nSignal)
{
	switch(nSignal) {
	  case SIGINT :
	  case SIGTERM :
		   m_bExitSignalPending = TRUE;
		   break;
	}
}

void Splash()
{
	printf("AIMIR Mobile telnet console v1.3\r\n");
	printf(COPYRIGHT "\r\n");
	printf("\r\n"); 
}

void Usage()
{
	printf("Usage: monet [-options] <phonenum> <device> <speed>\r\n");
	printf("   <phonenum>      Remote phone number\r\n");
	printf("   <device>        Serial device ('/dev/ttyS0', '/dev/ttyUSB0', ..)\r\n");
	printf("   <speed>         Serial speed (9600, .., 115200)\r\n");
	printf("Options:\r\n");
	printf("   -config         Change default configuration\r\n");
	printf("   -unlock         Unlock monet\r\n");
	printf("   -cdma           Using CDMA\r\n");
	printf("   -gsm            Using GSM\r\n");
	printf("Examples:\r\n");
	printf("   monet 04410001234\r\n");
	printf("   monet 04410001234 /dev/ttyS0 9600\r\n");
	printf("   monet -config -unlock 04410001234 /dev/ttyS0 9600\r\n");
	printf("   monet -config /dev/ttyS0 9600\r\n");
	printf("   monet -unlock\r\n");
	printf("\r\n");
}

void acceptstr(char *dest, char *src)
{              
    for(;*src; src++)
    {          
        if ((*src >= ' ') && (*src < 0x7F))
        {      
            *dest = *src;
            dest++;
        }      
    }          
    *dest = '\0';  
}                  

BOOL isnum(char *src)
{              
    for(;*src; src++)
    {          
        if (!strchr(m_szSyntax, *src))
			return FALSE;
    }
	return TRUE;    
}                  

void Message(char *pszMessage)
{
	char    szBuffer[128];
    
	memset(szBuffer, ' ', 79);
	szBuffer[79] = 0;
	memcpy(szBuffer, pszMessage, strlen(pszMessage));
	printf("\r%s\r", szBuffer);
	fflush(0);
}   

int GetSpeed(int n)
{
	switch(n) {
	  case 9600 :	return B9600;
	  case 19200 :	return B19200;
	  case 38400 :	return B38400;
	  case 57600 :	return B57600;
	  case 115200 :	return B115200;
	}
	return B9600;
}

BOOL CheckParameters(int argc, char **argv)
{
	CPrivateProfile	config;
	char	szType[32];
	BOOL	bSave = TRUE;
	int		i, nSpeed=9600;

	if (config.Load("monet.ini"))
	{
		config.GetValue("option", "type", szType, "gsm");
		m_nMobileType = (strcmp(szType, "cdma") == 0) ? MOBILE_CDMA : MOBILE_GSM;
		config.GetValue("option", "device", m_szDevice, "/dev/ttyS0");
		config.GetValueInt("option", "speed", &nSpeed, 9600);
		bSave = FALSE;
	}

	for(i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "9600") == 0)
			nSpeed = 9600;
		else if (strcmp(argv[i], "19200") == 0)
			nSpeed = 19200;
		else if (strcmp(argv[i], "38400") == 0)
			nSpeed = 38400;
		else if (strcmp(argv[i], "57600") == 0)
			nSpeed = 57600;
		else if (strcmp(argv[i], "115200") == 0)
			nSpeed = 115200;
		else if (strcmp(argv[i], "-cdma") == 0)
			m_nMobileType = MOBILE_CDMA;
		else if (strcmp(argv[i], "-gsm") == 0)
			m_nMobileType = MOBILE_GSM;
		else if (strcmp(argv[i], "-config") == 0)
			bSave = TRUE;
		else if (strcmp(argv[i], "-unlock") == 0)
			unlink("lock.monet");
		else if (strncmp(argv[i], "/dev/tty", 8) == 0)
			strcpy(m_szDevice, argv[i]);	
		else if (isnum(argv[i]))
		    strcpy(m_szNumber, argv[i]);
		else return FALSE;
	}

	m_nSpeed = GetSpeed(nSpeed);
	if (bSave)
	{
  		config.SetValueString("option", "type", (m_nMobileType == MOBILE_GSM) ? (char *)"gsm" : (char *)"cdma");
		config.SetValueString("option", "device", m_szDevice);
		config.SetValueInt("option", "speed", nSpeed);
		config.Save();
	}
	return TRUE;
}

int ConnectToHost(CMobileClient *pClient, char *pszNumber)
{
	char	*p, szBuffer[256];
	char	tmp[256], msg[256];
	int		i, n, len, nRetry, nState;
	
	nRetry = 0;
	for(nState=STATE_INIT; (!m_bExitSignalPending) && (nState!=STATE_DONE);)
	{
		switch(nState) {
		  case STATE_INIT :
			   printf("Trying %s ...           \r\n", pszNumber);
			   nState = STATE_ATZ;
			   break;

		  case STATE_ATZ :
			   Message("Modem initialize.      ");
			   pClient->WriteToModem("ATZ\r\n");
			   n = pClient->ReadFromModem(szBuffer, 255, 500);
			   if (n > 0)
			   {
			      szBuffer[n] = 0;
				  acceptstr(tmp, szBuffer);
				  sprintf(msg, "Reply '%s'", tmp);
				  Message(msg);
				  p = strstr(szBuffer, "OK");
				  if (p != NULL)
				  {	
			   		  nRetry = 0; 
			          if(m_nMobileType == MOBILE_CDMA)
			   		    nState = STATE_CRM;
                      else
			   		    nState = STATE_DIAL;
					  break;
				  }
			   }
			   else
			   {
			   	  Message("No response.");
				  usleep(200000);
			   }
			   nRetry++;
			   if (nRetry >= 3)
				   return -2;
			   break;

          case STATE_CRM :
               Message("Sending 'AT+CRM=129'");
               pClient->WriteToModem("AT+CRM=129\r\n");
               n = pClient->ReadFromModem(szBuffer, 255, 500);
               if (n > 0)
               {
                  szBuffer[n] = 0;
                  acceptstr(tmp, szBuffer);
                  sprintf(msg, "Reply '%s'", tmp);
                  Message(msg);
                  p = strstr(szBuffer, "OK");
                  if (p != NULL)
                  {
                      nRetry = 0;
                      nState = STATE_DIAL;
                      break;
                  }
               }
               nRetry++;
               if (nRetry > 3)
                   return -2;
               break;

		  case STATE_DIAL :
			   sprintf(szBuffer, "Dialing '%s'        ", pszNumber);
			   Message(szBuffer);
			   sprintf(szBuffer, "ATDT %s\r\n", pszNumber);
			   pClient->WriteToModem(szBuffer);
			   for(i=0, len=0; !m_bExitSignalPending && (i<30); i++)
			   {
			   	   n = pClient->ReadFromModem(&szBuffer[len], 255, 1000);
			   	   if (n > 0)
			   	   {
					   len += n;
			      	   szBuffer[len] = 0;
					   acceptstr(tmp, szBuffer);
					   sprintf(msg, "Reply '%s'", tmp);
					   Message(msg);

				  	   p = strstr(szBuffer, "BUSY");
				  	   if (p != NULL)
					  	   return -1;

				  	   p = strstr(szBuffer, "NO CARRIER");
				  	   if (p != NULL)
					  	   return -1;

				       p = strstr(szBuffer, "CONNECT");
				  	   if (p != NULL)
					   {
			               if(m_nMobileType == MOBILE_GSM) {
						        p = strstr(p, "/RLP");
                           }
						   if (p != NULL)
						   {
						   	   nState = STATE_SELECT;
						   	   break;
						   }
					   }
				   }
			   }
			   break;
	
		  case STATE_SELECT :
			   Message("Request telnet service.            ");
			   usleep(1000000);
			   pClient->WriteToModem("NURI/1.0 TELNET 23\n\n");
			   return 0;
		}
	}
	return -3;
}

int Terminal(CMobileClient *pClient, char *pszNumber)
{
	int		nState, nError;
	BOOL	bConnected = FALSE;
	struct	termios	oldset;
	struct	termios	newset;
	char	szBuffer[256];
	BYTE	c;
	int		n;

	for(nState=STATE_INIT; !m_bExitSignalPending;)
	{
		switch(nState) {
 		  case STATE_INIT :
			   nState = STATE_TRY_CONNECT;
			   break;

		  case STATE_TRY_CONNECT :
			   nError = ConnectToHost(pClient, pszNumber);
			   if (nError != 0)
			   {
				   if (nError == -2)
				        printf("Modem is not ready.\r\n");
		       	   else printf("Cannot connect to host.                 \r\n");
				   return 0;
			   }
		       printf("\r\nConnected to %s.\r\n", pszNumber);
			   printf("\r\n");
			   bConnected = TRUE;
			   pClient->SetDisplay(TRUE);
			   nState = STATE_SELECT_SVC;
		 	   break;

		  case STATE_SELECT_SVC :
			   tcgetattr(0, &oldset);
			   memcpy(&newset, &oldset, sizeof(newset));
			   newset.c_cc[VTIME]  = 10;              // Timeout TIME*0.1
			   newset.c_cc[VMIN]   = 0;
			   newset.c_cc[IGNBRK] = 1;
			   newset.c_cc[BRKINT] = 0;
		 	   newset.c_oflag &= (~ONLCR);
		 	   newset.c_iflag &= (~ICRNL);
		 	   newset.c_lflag &= (~ECHO);
			   newset.c_lflag &= (~ICANON);
			   tcsetattr(0, TCSANOW, &newset);          

			   nState = STATE_EMMUL;
		 	   break;

		  case STATE_EMMUL :
			   n = read(0, szBuffer, 32);
			   if (n > 0)
			   {
				   if (n == 1)
				   {
					   c = (BYTE)szBuffer[0];
					   if (c == CTRL_D)
					   {
						   m_bExitSignalPending = TRUE;
						   break;
					   } else if(c == CTRL_C) break;
				   }
				   pClient->WriteToModem(szBuffer, n);
			   }
		 	   break;
		}
	}

	if (bConnected)
	{
		tcsetattr(0, TCSANOW, &oldset);
		printf("\r\n");
		printf("Connection closed.\r\n");
	}
	return 0;
}

int main(int argc, char **argv)
{
	CMobileClient	client;
    struct 	sigaction   handler;
	int		fd;

	Splash();
	if (argc < 2)
	{
		Usage();
		return 0;	
	}

	if (!CheckParameters(argc, argv))
	{
		Usage();
		return 0;	
	}

	if (!*m_szNumber)
		return 0;

	fd = open("lock.monet", O_CREAT, 0644);
	if (flock(fd, LOCK_EX | LOCK_NB) != 0)
	{
		printf("ERROR: monet is in use.\r\n");
		printf("\r\n");
		system("/bin/ps -eaf | grep /monet");
		printf("\r\n");
		exit(0);
    }


    // Set Interrrupt Signal Handler
	memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_handler = SignalCatcher;
    sigfillset(&handler.sa_mask);
	sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);

	if (client.Initialize(m_szDevice, m_nSpeed))
	{
		Terminal(&client, m_szNumber);
		client.Destroy();
	}

	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}
