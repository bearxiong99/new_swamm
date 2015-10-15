#include <unistd.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
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

BOOL	m_bExitSignalPending = FALSE;

void Usage()
{
	printf("Usage: gsmnet <Phonenum>\r\n");
	printf("   Phonenum   Remote phone number\r\n");
	printf("\r\n");
}

void Splash()
{
	printf("AIMIR Mobile telnet console v1.0\r\n");
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

void Message(const char *pszMessage)
{
	char    szBuffer[128];
    
	memset(szBuffer, ' ', 79);
	szBuffer[79] = 0;
	memcpy(szBuffer, pszMessage, strlen(pszMessage));
	printf("\r%s\r", szBuffer);
	fflush(0);
}   

void InitGpio()
{    

}

void Disconnect()
{    
  
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
			   pClient->WriteToModem(const_cast<char *>("ATZ\r\n"));
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
			   else
			   {
			   	  Message("No response.");
				  usleep(200000);
			   }
			   nRetry++;
			   if (nRetry >= 3)
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
						   p = strstr(p, "/RLP");
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
			   pClient->WriteToModem(const_cast<char *>("NURI/1.0 TELNET 23\n\n"));
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
					   if (c == CTRL_C)
					   {
						   m_bExitSignalPending = TRUE;
						   break;
					   }
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

	Splash();
	if (argc != 2)
	{
		Usage();
		return 0;	
	}

    // Set Interrrupt Signal Handler
	memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_handler = SignalCatcher;
    sigfillset(&handler.sa_mask);
	sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);

	InitGpio();
	if (client.Initialize())
	{
		Terminal(&client, argv[1]);
		client.Destroy();
		Disconnect();
	}
	return 0;
}
