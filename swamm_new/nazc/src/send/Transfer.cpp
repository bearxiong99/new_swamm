#include "common.h"
#include "IF4DataFrame.h"
#include "Transfer.h"
#include "MobileClient.h"
#include "MemoryDebug.h"
#include "logdef.h"
#include "DebugUtil.h"
#include "gpiomap.h"
#include "Chunk.h"

#define STATE_INIT				0
#define STATE_EOFF				1
#define STATE_ATZ				2
#define STATE_AT_RETRY			3
#define STATE_ATS0				4
#define STATE_CRM				10
#define STATE_SCID				20
#define STATE_CPIN				21
#define STATE_CSQ				22				
#define STATE_SBV				23				
#define STATE_ATH				40
#define STATE_DIAL				50
#define STATE_SELECT			51
#define STATE_WAIT_ACCEPT		52
#define STATE_ACCEPT			53

#define STATE_ENQ				60
#define STATE_READ_PACKET		61
#define STATE_SEND_PACKET		62
#define STATE_RECV_ACK			63
#define STATE_ERROR			    98	
#define STATE_OK			    99	
#define STATE_DONE				100

#define STATE_TRY_CONNECT		200
#define STATE_SEND				201
#define STATE_RETRY				202
#define STATE_TERMINATE			203

#define SENDERR_OK							0
#define SENDERR_RECV_TIMEOUT				-1
#define SENDERR_NO_ENQ						-2
#define SENDERR_FILE_OPEN_ERROR				-3
#define SENDERR_NO_RESPNOSE					-4
#define SENDERR_UNKNOWN_RESPONSE			-5
#define SENDERR_NAK							-6

#define CTRL_C					0x03
#define CTRL_D					0x04

#define MOBILE_CDMA				0
#define MOBILE_GSM				1

#define MAX_RETRY_COUNT			1000
#define MAX_TRY_TIME			10*60

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

extern	BOOL	m_bExitSignalPending;

//inserted by sungyeung naju 2015-10-26
extern int m_nDebugLevel;


BOOL	m_nMobileType = MOBILE_GSM;
int		m_nPhoneCount = 0;
char	szCPIN[100]	  = "";
char	szCSQ[100]	  = "";
char	szSBV[100] 	  = "";
char	*m_pszBuffer  = NULL;
char	m_szPhoneList[20][32];

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

CTransfer::CTransfer()
{
	//inserted by sungyeung naju 2015-10-26
	m_nDebugLevel=0;



	m_bEOF			= FALSE;
	m_bCompress		= FALSE;
	m_nNzcNumber	= 0;
	m_nSentLine		= 0;
	m_pszReadBuffer	= NULL;
}

CTransfer::~CTransfer()
{
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

void CTransfer::acceptstr(char *dest, char *src, int len)
{             
	int		i;
 
    for(i=0; *src && (i<len); src++, i++)
    {          
        if ((*src >= ' ') && (*src < 0x7F))
        {      
            *dest = *src;
            dest++;
        }      
    }          
    *dest = '\0';  
}                  

void CTransfer::Message(char *pszMessage)
{
	XDEBUG("CURCUIT: %s\r\n", pszMessage);
	fflush(0);
}   

void CTransfer::LoadPhoneList()
{
	char	szBuffer[128];
	FILE	*fp;

	m_nPhoneCount = 0;
	fp = fopen("/app/sw/phonelist", "rt");
	if (fp == NULL)
		return;

	while(fgets(szBuffer, 63, fp))
	{
		szBuffer[strlen(szBuffer)-1] = '\0';
		if (!*szBuffer)
			continue;

		strncpy(m_szPhoneList[m_nPhoneCount], szBuffer, 31);
		m_szPhoneList[m_nPhoneCount][31] = '\0';
		m_nPhoneCount++;
	}
	fclose(fp);
}

// 주의: SIM 카드가 안꼽혀 있을때 계속 리셋하면 모듈이
// 손상될 수 있으므로 주의, SIM ERROR여도 리셋하지 말것

void CTransfer::ResetModem()
{

}

void CTransfer::IgtControl()
{

}

int CTransfer::GetDCD()
{

	return 1 ;
}

void CTransfer::DisconnectModem()
{

	return;
}

int CTransfer::GetBatteryState()
{
	return 1;
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

void CTransfer::SendAck(CMobileClient *pClient, BYTE nSeq)
{
    IF4_CTRL_FRAME  frame;
    int     nLength;
    BYTE    arg[1];

    arg[0] = nSeq;
    nLength = MakeControlFrame(&frame, IF4_ACK, (char *)arg, 1);
    pClient->WriteToModem((char *)&frame, nLength);
}

void CTransfer::SendNak(CMobileClient *pClient, BYTE nSeq)
{
    IF4_CTRL_FRAME  frame;
    int     nLength;
    BYTE    arg[1];

    arg[0] = nSeq;
    nLength = MakeControlFrame(&frame, IF4_NAK, (char *)arg, 1);
    pClient->WriteToModem((char *)&frame, nLength);
}

void CTransfer::SendEot(CMobileClient *pClient)
{
    IF4_CTRL_FRAME  frame;
    int     nLength;
    BYTE    arg[1];

    arg[0]  = 0;
    nLength = MakeControlFrame(&frame, IF4_EOT, (char *)arg, 1);
    pClient->WriteToModem((char *)&frame, nLength);
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

int CTransfer::Transfer(CMobileClient *pClient, char *pszType, int nNzcNumber, int nPort, char *pszFileName, int nTryTime, BOOL bCompress)
{
	char	szNumber[64] = "";
	int		nState, nError;
	BOOL	bConnected = FALSE;
	BOOL	bCDMA, bEvent;
	int		nRetry = 0;
	int		nIndex = 0;
	int		nResult;
	time_t	tmStart, tmNow;

	m_bCompress	= bCompress;
	m_nNzcNumber = nNzcNumber;
	m_nMobileType = MOBILE_GSM;
	bCDMA = (strcmp(pszType, "cdma") == 0) ? TRUE : FALSE;
	if (bCDMA) m_nMobileType = MOBILE_CDMA;
	bEvent = strstr(pszFileName, "event") != NULL ? TRUE : FALSE;

	LoadPhoneList();
	if (m_nPhoneCount == 0)
	{
		XDEBUG("File not found: /app/sw/phonelist\r\n");
		return 0;
	}

	time(&tmStart);
	for(nState=STATE_INIT; !m_bExitSignalPending;)
	{
		switch(nState) {
 		  case STATE_INIT :
			   nRetry = 0;
			   DisconnectModem();
			   nState = STATE_TRY_CONNECT;
			   break;

		  case STATE_TRY_CONNECT :
			   strcpy(szNumber, m_szPhoneList[nIndex]);
		       XDEBUG("CIRCUIT: Try Connect to %s. (%s)\r\n",
						szNumber, bEvent ? "Event" : "Data");
			   UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Try connect.\n", szNumber, nPort);

			   nError = ConnectToHost(pClient, szNumber, bCDMA, nPort);
			   if (nError != 0)
			   {
				   switch(nError) {
					 case -4 :
					      XDEBUG("CIRCUIT: Server is not accept. (CPIN=%s, CSQ=%s, SBV=%s)\r\n",
								szCPIN, szCSQ, szSBV);
			       	      UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Server is not accept. (CPIN=%s, CSQ=%s, SBV=%s)\n",
								szNumber, nPort, szCPIN, szCSQ, szSBV);
						  break;
				     case -10 :
					      XDEBUG("CIRCUIT: Modem or SIM error!!.\r\n");
			       	      UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Modem or SIM error.\n", szNumber, nPort);
						  break;
				     case -1 :
					 case -2 :
					 case -3 :
					  default:
					      XDEBUG("CIRCUIT: Cannot connect to host. (CPIN=%s, CSQ=%s, SBV=%s)\r\n",
								szCPIN, szCSQ, szSBV);
			       	      UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Cannot connect to host. (CPIN=%s, CSQ=%s, SBV=%s)\n",
								szNumber, nPort, szCPIN, szCSQ, szSBV);
						  break;
				   }	
				   nState = STATE_RETRY;
				   break;
			   }

		       XDEBUG("CIRCUIT: Connected.\r\n");
			   UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Connected. (%s)\n", szNumber, nPort, bEvent ? "Event" : "Data");
			   bConnected = TRUE;
			   nState = STATE_SEND;
		 	   break;

		  case STATE_SEND :
			   XDEBUG("CIRCUIT: Sending %s...\r\n", bEvent ? "Event" : "Data");
			   nResult = -1;
			   m_pszReadBuffer = (char *)MALLOC(1024*30);
			   if (m_pszReadBuffer != NULL)
			   {
				   if (bEvent)
			   		    nResult = SendEvent(pClient, pszFileName);
			   	   else nResult = SendMetering(pClient, pszFileName);
				   FREE(m_pszReadBuffer);
			   }
			   nState = (nResult == SENDERR_OK) ? STATE_TERMINATE : STATE_RETRY;
			   if (nState == STATE_RETRY)
			   {
			   	   UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d %s Sending fail (ERROR=%d).\n",
								szNumber, nPort,
								bEvent ? "Event" : "Data",
								nResult);
			   }
		 	   break;

		  case STATE_RETRY :
			   XDEBUG("CIRCUIT: Disconnect mobile session.\r\n");
			   DisconnectModem();

			   nIndex++;
			   if (nIndex >= m_nPhoneCount)
			       nIndex = 0;

			   nRetry++;
			   if (nRetry >= MAX_RETRY_COUNT)
			   {
				   XDEBUG("CIRCUIT: Out of MAX_RETRY_COUNT(%d).\r\n", MAX_RETRY_COUNT);
				   DisconnectModem();
				   return 0;
			   }

			   time(&tmNow);
			   if ((tmNow-tmStart) > nTryTime)
			   {
				   XDEBUG("CIRCUIT: Max try timeout(%d).\r\n", (int)(tmNow-tmStart));
				   DisconnectModem();
				   return 0;
			   }

			   XDEBUG("\r\n");
			   XDEBUG("CIRCUIT: SEND RETRY (Retry=%d, Time=%d min %d seconds)\r\n",
						nRetry,
						(int)(tmNow-tmStart) / 60,
						(int)(tmNow-tmStart) % 60);
			   nState = STATE_TRY_CONNECT;
		 	   break;

		  case STATE_TERMINATE :
			   time(&tmNow);
			   XDEBUG("\r\n");
			   XDEBUG("CIRCUIT: Total send time = %d min %d seconds\r\n",
						(int)(tmNow-tmStart) / 60,
						(int)(tmNow-tmStart) % 60);
			   XDEBUG("CIRCUIT: Disconnect mobile session.\r\n");
			   UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d %s Sending ok (Total=%d).\n",
						szNumber, nPort,
						bEvent ? "Event" : "Data",
						m_nSentLine);
			   DisconnectModem();
			   return 1;
		}
	}

	if (m_bExitSignalPending)
	{
	    XDEBUG("CIRCUIT: User stop. Disconnect mobile.\r\n");
		DisconnectModem();
	}
	
	return 0;
}

#define STATE_SETUP_ATH			10
#define STATE_SETUP_ATS0		11

int CTransfer::SetupModem(CMobileClient *pClient)
{
	char	szBuffer[256], *p;
	int		nState, nNextState;
	int		n, nTimeout;
	int		nRetry=0, nError=0;
	BOOL	bWaitMessage;
	time_t	now, start;

	for(nState=STATE_INIT; nState!=STATE_DONE;)
	{
		nTimeout = 3;
		nNextState = nState;

		// Send AT Command
		switch(nState) {
		  case STATE_INIT :
			   if (GetDCD() == 0)
			   {
				   XDEBUG("DCD Active, Disconnect ...\r\n");
				   DisconnectModem();
			   }
			   nState = STATE_SETUP_ATH;

		  case STATE_SETUP_ATH :
			   XDEBUG("CIRCUIT: Sending 'ATH'\r\n");
			   pClient->WriteToModem(const_cast<char *>("ATH\r\n"));
			   nTimeout = 3;
			   nNextState = STATE_SETUP_ATS0;
			   break;

		  case STATE_SETUP_ATS0 :
			   XDEBUG("CIRCUIT: Sending 'ATS0=1'\r\n");
			   pClient->WriteToModem(const_cast<char *>("ATS0=1\r\n\r\n"));
			   nTimeout = 3;
			   nNextState = STATE_DONE;
			   break;

		  case STATE_DONE :
			   return 0;
		}

		time(&start);
		bWaitMessage = TRUE;

		// Receive Result
		for(;bWaitMessage;)
		{
			n = pClient->ReadLineFromModem(szBuffer, 255, 3000);
			if (n > 0)
			{
				XDEBUG("CIRCUIT: Recv '%s'\r\n", szBuffer);
			    p = strstr(szBuffer, "OK");
				if (p != NULL)
				{
				    nState = nNextState;
					bWaitMessage = FALSE;
				}
			    p = strstr(szBuffer, "ERROR");
			    if (p != NULL)
				{
				    nError++;
					if (nError >= 10)
					{
					    XDEBUG("Drop. (IGNORE)\r\n");
						nState = nNextState;
					}
					else
					{
					    XDEBUG("Retry=%d\r\n", nError);
					    usleep(1000000);
					}
					bWaitMessage = FALSE;
				}
			}

			time(&now);
			if ((now-start) > nTimeout)
			{
			    nState = STATE_INIT;
			    nRetry++;
			    if (nRetry >= 5)
				    return -1;
				break;
			}
		}
	}
	return 0;
}

int CTransfer::ConnectToHost(CMobileClient *pClient, char *pszNumber, BOOL bCDMA, int nPort)
{
	char	szBuffer[256];
	char	msg[256];
	char	*p, *p1, *p2, *p3;
	time_t	now, start;
	int		n, nRetry, nState;
	int		nTimeout, nNextState;
	int		nErrorCount = 0;
	int		nAtzError = 0;
	int		nAtsError = 0;
	BOOL	bWaitMessage;
	
	nRetry = 0;
	XDEBUG("Trying %s ...\r\n", pszNumber);

	for(n=0; n<5; n++)
		pClient->ReadFromModem(szBuffer, 255, 100);

	for(nState=STATE_INIT; (!m_bExitSignalPending) && (nState!=STATE_DONE);)
	{
		nTimeout = 3;
		nNextState = nState;

		// Send AT Command
		switch(nState) {
		  case STATE_INIT :
			   XDEBUG("\r\n");
			   XDEBUG("CIRCUIT: TRY Count = %d, BATT CHARGE STATE = %d\r\n",
					nRetry+1, GetBatteryState());
			   nErrorCount = 0;
			   nAtzError = 0;
			   nState = STATE_ATZ;

		  case STATE_EOFF :
			   nAtzError = 0;
			   if (nState == STATE_EOFF)
			   {
			   	   ResetModem();
			   	   nState = STATE_ATZ;
			   }

		  case STATE_ATZ :
			   nState = STATE_ATZ;
			   usleep(1000000);
			   XDEBUG("CIRCUIT: DTR Control.\r\n");
			   DisconnectModem();

		  case STATE_AT_RETRY :
			   usleep(1000000);
			   XDEBUG("CIRCUIT: Sending 'AT'\r\n");
			   pClient->WriteToModem(const_cast<char *>("AT\r\n"));
			   nTimeout = 3;
			   nAtsError = 0;
			   nNextState = STATE_ATS0;
			   break;

		  case STATE_ATS0 :
			   XDEBUG("CIRCUIT: Sending 'ATS0=0'\r\n");
			   pClient->WriteToModem(const_cast<char *>("ATS0=0\r\n"));
			   nTimeout = 3;
			   nNextState = bCDMA ? STATE_CRM : STATE_SCID;
			   break;

		  case STATE_CRM :
			   XDEBUG("CIRCUIT: Sending 'AT+CRM=129'\r\n");
			   pClient->WriteToModem(const_cast<char *>("AT+CRM=129\r\n"));
			   nTimeout = 3;
			   nNextState = STATE_ATH;
			   break;

		  case STATE_SCID :
			   XDEBUG("CIRCUIT: Sending 'AT^SCID'\r\n");
			   pClient->WriteToModem(const_cast<char *>("AT^SCID\r\n"));
			   nTimeout = 3;
			   nNextState = STATE_CPIN;
			   break;

		  case STATE_CPIN :
			   XDEBUG("CIRCUIT: Sending 'AT+CPIN?'\r\n");
			   strcpy(szCPIN, "");
			   pClient->WriteToModem(const_cast<char *>("AT+CPIN?\r\n"));
			   nTimeout = 3;
			   nNextState = STATE_CSQ;
			   break;

		  case STATE_CSQ :
			   XDEBUG("CIRCUIT: Sending 'AT+CSQ'\r\n");
			   strcpy(szCSQ, "");
			   pClient->WriteToModem(const_cast<char *>("AT+CSQ\r\n"));
			   nTimeout = 3;
			   nNextState = STATE_SBV;
			   break;

		  case STATE_SBV :
			   XDEBUG("CIRCUIT: Sending 'AT^SBV'\r\n");
			   strcpy(szSBV, "");
			   pClient->WriteToModem(const_cast<char *>("AT^SBV\r\n"));
			   nTimeout = 3;
			   nNextState = STATE_ATH;
			   break;

		  case STATE_ATH :
			   XDEBUG("CIRCUIT: Sending 'ATH'\r\n");
			   pClient->WriteToModem(const_cast<char *>("ATH\r\n"));
			   nTimeout = 3;
			   nNextState = STATE_DIAL;
			   break;

		  case STATE_DIAL :
			   sprintf(szBuffer, "Sending 'ATDT %s'", pszNumber);
			   XDEBUG("CIRCUIT: %s\r\n", szBuffer);
			   sprintf(szBuffer, "ATDT %s\r\n", pszNumber);
			   pClient->WriteToModem(szBuffer);
			   nTimeout = 30;
			   nNextState = STATE_SELECT;
			   break;

		  case STATE_SELECT :
			   usleep(1000000);
			   XDEBUG("CIRCUIT: Sending 'NURI/1.0 NACS'\r\n");
			   sprintf(msg, "NURI/1.0 NACS %0d\n\n", nPort);
			   pClient->WriteToModem(msg);
			   nTimeout = 3;
			   nState = STATE_WAIT_ACCEPT;
			   break;

		  case STATE_WAIT_ACCEPT :
		  case STATE_DONE :
			   break;
		}

		time(&start);
		bWaitMessage = TRUE;

		// Receive Result
		for(;!m_bExitSignalPending && bWaitMessage;)
		{
			n = pClient->ReadLineFromModem(szBuffer, 255, 3000);
			if (n > 0)
			{
				XDEBUG("CIRCUIT: Recv '%s'\r\n", szBuffer);

				switch(nState) {
				  case STATE_ATZ :
				  case STATE_CRM :
				  case STATE_AT_RETRY :
				  	   p = strstr(szBuffer, "OK");
					   if (p != NULL)
					   {
						   nState = nNextState;
						   bWaitMessage = FALSE;
					   }
					   p = strstr(szBuffer, "ERROR");
					   if (p != NULL)
					   {
						   nAtzError++;
						   if (nAtzError >= 3)
						   {
							   nState = STATE_EOFF;
						   }
					   	   bWaitMessage = FALSE;
					   }
					   p = strstr(szBuffer, "NO CARRIER");
					   if (p != NULL)
					   {
						   nState = STATE_AT_RETRY;
					   	   bWaitMessage = FALSE;
					   }
					   break;
				  case STATE_ATS0 :
				  	   p = strstr(szBuffer, "OK");
					   if (p != NULL)
				       {
						   nState = nNextState;
					       bWaitMessage = FALSE;
				       }
					   p = strstr(szBuffer, "ERROR");
					   if (p != NULL)
					   {
						   nAtsError++;
						   if (nAtsError >= 10)
						   {
							   XDEBUG("ATS Change drop. (IGNORE)\r\n");
						   	   nState = nNextState;
						   }
						   else
						   {
							   XDEBUG("ATS Error (Retry=%d)\r\n", nAtsError);
							   usleep(1000000);
						   }
					       bWaitMessage = FALSE;
					   }
					   break;
				  case STATE_SCID :
				  case STATE_CPIN :
				  case STATE_CSQ :
		  		  case STATE_SBV :
		  		  case STATE_ATH :
				  	   p = strstr(szBuffer, "+CPIN: ");
					   if (p != NULL) strcpy(szCPIN, p+7);
				  	   p = strstr(szBuffer, "+CSQ: ");
					   if (p != NULL) strcpy(szCSQ, p+6);
				  	   p = strstr(szBuffer, "^SBV: ");
					   if (p != NULL) strcpy(szSBV, p+6);

				  	   p = strstr(szBuffer, "OK");
					   if (p != NULL)
					   {
					   	   nState = nNextState;
						   bWaitMessage = FALSE;
						   break;
					   }
					   p = strstr(szBuffer, "ERROR");
					   if (p != NULL)
					   {
						   nErrorCount++;
						   XDEBUG("CIRCUIT: \r\n");
						   if (nErrorCount > 10)
							   return -10;
						   usleep(3000000);
					       bWaitMessage = FALSE;
						   break;
					   }
					   break;
				  case STATE_DIAL :
				       p = strstr(szBuffer, "CONNECT");
				  	   p1 = strstr(szBuffer, "BUSY");
				  	   p2 = strstr(szBuffer, "ERROR");
				  	   p3 = strstr(szBuffer, "NO CARRIER");
				  	   if (p != NULL)
					   {
						   nState = STATE_SELECT;
						   bWaitMessage = FALSE;
						   break;
					   }
					   else if (p1 || p2 || p3)
					   {
						   // Retry
						   nState = STATE_INIT;
						   bWaitMessage = FALSE;

						   nRetry++;
						   if (nRetry >= 3)
							   return -1;
					   }
					   break;
				  case STATE_WAIT_ACCEPT :
				  	   p1 = strstr(szBuffer, "NO CARRIER");
					   p2 = strstr(szBuffer, "BUSY");
					   p3 = strstr(szBuffer, "ERROR");
					   if (p1 || p2 || p3)
					   {
						   nState = STATE_INIT;
						   bWaitMessage = FALSE;
						   nRetry++;
						   if (nRetry >= 3)
							   return -1;
						   break;
					   }

					   p = strstr(szBuffer, "NURI/");
					   if (p != NULL)
					   {			   
						   p2 = strchr(p, ' ');
						   if (p2 == NULL)
							   return -1;

						   n = atoi(p2);
						   XDEBUG("CIRCUIT: ACCEPT Result = %0d\r\n", n);
						   if (n != 250)
						   {
							   XDEBUG("ERROR: Accept error!!\r\n");
						       return -4;
						   }
						   return 0;
					   }
					   break;
				}
			}

			time(&now);
			if ((now-start) > nTimeout)
			{
			    nState = STATE_INIT;
			    nRetry++;
			    if (nRetry >= 3)
				{
					if (n == 0) IgtControl();
				    return -2;
				}
				break;
			}
		}
	}
	return -3;
}

int CTransfer::SendEvent(CMobileClient *pClient, char *pszFileName)
{
	char			szBuffer[256];
	char			*pszData = NULL;
	IF4_HEADER		*pHeader;
	IF4_CTRL_FRAME	*pCtrl;
	int				nRetry = 0;
	int				nResult = -1;
	int				fd=0, n, nState, nLength=0;
	
	for(nState=STATE_ENQ; (!m_bExitSignalPending) && (nState!=STATE_DONE);)
	{
		switch(nState) {
		  case STATE_ENQ :
			   if (!pClient->ReadPacketFromModem(szBuffer, &n, 5000))
				   return SENDERR_RECV_TIMEOUT;

			   XDUMP(szBuffer, n);
			   pCtrl = (IF4_CTRL_FRAME *)szBuffer;
			   if ((pCtrl->hdr.svc != IF4_SVC_CONTROL) || (pCtrl->code != IF4_ENQ))
			   {
				   XDEBUG("Invalid ENQ\r\n");
				   return SENDERR_NO_ENQ;
			   }

			   fd = open(pszFileName, O_RDONLY);
			   if (fd <= 0)
			   {
				   XDEBUG("File open error: %s\r\n", pszFileName);
				   return -1;
			   }
			   nState = STATE_READ_PACKET;
			   break;

		  case STATE_READ_PACKET :
			   XDEBUG("CIRCUIT: Read packet.\r\n");
			   pszData = NULL;
			   nLength = read(fd, szBuffer, sizeof(IF4_HEADER));
			   if (nLength == (int)sizeof(IF4_HEADER))
			   {
				   pHeader = (IF4_HEADER *)szBuffer;
				   pszData = (char *)MALLOC(pHeader->len + sizeof(IF4_HEADER) + sizeof(IF4_TAIL));
				   if (pszData != NULL)
				   {
					   memcpy(pszData, szBuffer, sizeof(IF4_HEADER));
					   n = read(fd, pszData+nLength, pHeader->len+sizeof(IF4_TAIL));
					   if (n == (int)(pHeader->len+sizeof(IF4_TAIL)))
					   {
					       nLength += n;
			   			   nState = STATE_SEND_PACKET;
						   break;
					   }
				   }
			   }
			   if (pszData != NULL)
				   FREE(pszData);
			   nState = STATE_OK;
			   break;

		  case STATE_SEND_PACKET :
			   XDEBUG("CIRCUIT: Send packet. (Length=%0d)\r\n", nLength);
//			   XDUMP(pszData, nLength);
		   	   pClient->WriteToModem(pszData, nLength);
		   	   nState = STATE_RECV_ACK;
			   break;

		  case STATE_RECV_ACK :
			   XDEBUG("CIRCUIT: Waitting for reply.\r\n");
			   if (!pClient->ReadPacketFromModem(szBuffer, &n, 10000))
			   {
				   nRetry++;
				   nResult = SENDERR_NO_RESPNOSE;
				   if (nRetry <= 3)
				   {
					   nState = STATE_SEND_PACKET;
					   break;
				   }
			   	   FREE(pszData);
				   nState = STATE_ERROR;
				   break;
			   }

			   pCtrl = (IF4_CTRL_FRAME *)szBuffer;
			   if (pCtrl->hdr.svc != IF4_SVC_CONTROL)
			   {
				   XDEBUG("CIRCUIT: Not control frame.\r\n");
				   nResult = SENDERR_UNKNOWN_RESPONSE;
				   nRetry++;
				   if (nRetry <= 3)
				   {
					   nState = STATE_SEND_PACKET;
					   break;
				   }
			   	   FREE(pszData);
				   nState = STATE_ERROR;
				   break;
			   }
			
			   switch(pCtrl->code) {
			     case IF4_NAK :
			          XDEBUG("CIRCUIT: Recv NAK ~~~~\r\n");
					  nResult = SENDERR_NAK;
				      nRetry++;
				      if (nRetry <= 3)
				      {
					      nState = STATE_SEND_PACKET;
					      break;
				      }
			   	      FREE(pszData);
					  nState = STATE_ERROR;
				      break;

				 case IF4_ACK :
				      XDEBUG("CIRCUIT: Recv ACK ~~~~\r\n");
					  m_nSentLine++;
			   	      FREE(pszData);
			   		  nState = STATE_READ_PACKET;
				      break;

				 case IF4_EOT :
			   		  nResult = SENDERR_OK;
			   		  close(fd);
			   	      FREE(pszData);
			   		  nState = STATE_DONE;
				      break;

				 default :
			   		  nResult = 0;
			   	      FREE(pszData);
			   		  nState = STATE_ERROR;
				      break;
			   }
			   break;

		  case STATE_OK :
			   nResult = SENDERR_OK;
			   close(fd);
			   SendEot(pClient);
			   usleep(3000000);
			   nState = STATE_DONE;
			   break;

		  case STATE_ERROR :
			   close(fd);
			   SendEot(pClient);
			   usleep(3000000);
			   nState = STATE_DONE;
			   break;
		}
	}
	return nResult;
}

int CTransfer::SendMetering(CMobileClient *pClient, char *pszFileName)
{
	CChunk			chunk;
	char			szBuffer[128];
	char			*pFrame;
	BYTE			nSeq = 0;
	IF4_CTRL_FRAME	*pCtrl;
	METERINGDATA    *pList;
	int				nRetry = 0, len, nSize;
	int				fd=0, n, nState, nLength=0;
	int				nResult = -1;
	int				nLine = 0;

	m_nGoodLine = 0;	
	for(nState=STATE_ENQ; (!m_bExitSignalPending) && (nState!=STATE_DONE);)
	{
		switch(nState) {
		  case STATE_ENQ :
			   if (!pClient->ReadPacketFromModem(szBuffer, &n, 5000))
				   return SENDERR_RECV_TIMEOUT;

			   pCtrl = (IF4_CTRL_FRAME *)szBuffer;
			   if ((pCtrl->hdr.svc != IF4_SVC_CONTROL) || (pCtrl->code != IF4_ENQ))
			   {
				   XDEBUG("CIRCUIT: Not ENQ, return.\r\n");
				   return SENDERR_NO_ENQ;
			   }

			   XDEBUG("CIRCUIT: Recv 'ENQ'\r\n");
			   fd = open(pszFileName, O_RDONLY);
			   if (fd <= 0)
			   {	
			   	   printf("CIRCUIT: File Open error '%s'\r\n", pszFileName);
				   return SENDERR_FILE_OPEN_ERROR;
			   }
			   nSeq = 0;
			   chunk.Flush();
			   nState = STATE_READ_PACKET;
			   break;

		  case STATE_READ_PACKET :
               nLength = read(fd, m_pszReadBuffer, sizeof(METERINGDATA));
			   XDEBUG("CIRCUIT: Read header %d bytes\r\n", nLength);

			   if (nLength == (int)sizeof(METERINGDATA))
			   {
            	   pList = (METERINGDATA *)m_pszReadBuffer;
				   nSize = pList->length - sizeof(TIMESTAMP);
				   n = read(fd, m_pszReadBuffer+sizeof(METERINGDATA), nSize);
			   	   XDEBUG("CIRCUIT: Read data %d bytes\r\n", n);
				   if (n == nSize) 
				   {
			   		   nLine++;
				       nLength += n;
			   		   nState = STATE_SEND_PACKET;
			   	   	   XDEBUG("CIRCUIT[%d]: Total data %d bytes\r\n", nLine, nLength);
					   break;
				   }
			   }
			   XDEBUG("CIRCUIT: End of file\r\n");
			   nState = STATE_OK;
			   break;

		  case STATE_SEND_PACKET :
			   if (m_nSentLine >= nLine)
			   {
			   	   XDEBUG("CIRCUIT: Skip packet. (Join=%d, Line=%d, Length=%0d)\r\n", m_nSentLine, nLine, nLength);
			   	   nState = STATE_READ_PACKET;
				   break;
			   }

			   XDEBUG("CIRCUIT: Send packet. (Length=%0d)\r\n", nLength);
			   pFrame = (char *)MALLOC(sizeof(IF4_DATA_FRAME)+nLength+20);
			   if (pFrame != NULL)
			   {
				   len = MakeFrame(pFrame, m_pszReadBuffer, nLength, nSeq);
		   	   	   pClient->WriteToModem(pFrame, len);
				   FREE(pFrame);
			   }
		   	   nState = STATE_RECV_ACK;
			   break;

		  case STATE_RECV_ACK :
			   XDEBUG("CIRCUIT: Waitting for reply.\r\n");
			   if (!pClient->ReadPacketFromModem(szBuffer, &n, 10000))
			   {
			   	   XDEBUG("CIRCUIT: Waitting timeout.\r\n");
				   nResult = SENDERR_NO_RESPNOSE;
				   nRetry++;
				   if (nRetry <= 3)
				   {
					   nState = STATE_SEND_PACKET;
					   break;
				   }
				   nState = STATE_ERROR;
				   break;
			   }

			   pCtrl = (IF4_CTRL_FRAME *)szBuffer;
			   if (pCtrl->hdr.svc != IF4_SVC_CONTROL)
			   {
				   XDEBUG("CIRCUIT: Not control frame.\r\n");
				   nResult = SENDERR_UNKNOWN_RESPONSE;
				   nRetry++;
				   if (nRetry <= 3)
				   {
					   nState = STATE_SEND_PACKET;
					   break;
				   }
				   nState = STATE_ERROR;
				   break;
			   }
			
			   switch(pCtrl->code) {
			     case IF4_NAK :
			          XDEBUG("CIRCUIT: Recv 'NAK'\r\n");
					  nResult = SENDERR_NAK;
				      nRetry++;
				      if (nRetry <= 3)
				      {
					      nState = STATE_SEND_PACKET;
					      break;
				      }
					  nState = STATE_ERROR;
				      break;

				 case IF4_ACK :
				      XDEBUG("CIRCUIT: Recv 'ACK'\r\n");
					  nSeq++;
					  m_nGoodLine++;
					  m_nSentLine++;
			   		  chunk.Flush();
			   		  nState = STATE_READ_PACKET;
				      break;

				 case IF4_EOT :
				      XDEBUG("CIRCUIT: Recv 'EOT'\r\n");
			   		  nState = STATE_OK;
				      break;

				 default :
			   		  nState = STATE_ERROR;
				      break;
			   }
			   break;

		  case STATE_OK :
			   close(fd);
			   SendEot(pClient);
			   usleep(3000000);
			   nResult = 0;
			   nState = STATE_DONE;
			   break;

		  case STATE_ERROR :
			   close(fd);
			   SendEot(pClient);
			   usleep(3000000);
			   nState = STATE_DONE;
			   break;
		}
	}
	return nResult;
}

int CTransfer::MakeFrame(char *pszBuffer, char *pszData, int len, BYTE nSeq)
{   
    IF4_DATA_FRAME  *pFrame;
    int             nLength;
    int             nSourceLength = 0;

    pFrame = (IF4_DATA_FRAME *)pszBuffer;
    memset(pFrame, 0, sizeof(IF4_DATA_FRAME));
    nLength = MakeIF4MeteringDataFrame(pFrame, (WORD)1, (BYTE *)pszData, len, &nSourceLength, nSeq);
	return nLength;
}
