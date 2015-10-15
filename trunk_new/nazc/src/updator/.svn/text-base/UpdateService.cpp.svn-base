#include "common.h"
#include "UpdateService.h"
#include "CrcCheck.h"
#include "DebugUtil.h"
#include "Chunk.h"

//////////////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////////////

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
#define STATE_TERM_WAIT			210

#define CTRL_C					0x03
#define CTRL_D					0x04

#define PROGRESSBAR	"Uploading [..................................................] %.1f%% [%c] %s     \r"
#define PLAYLIST	"-\\|/"

#define RETRY_LIMIT             10

extern BOOL	m_bExitSignalPending;

extern int m_nPacketSize;
extern int m_nMobileType;
extern char * m_pRemoteFile;
extern BOOL m_bEnableInstall;

//////////////////////////////////////////////////////////////////////////////
// Construction/Destructions
//////////////////////////////////////////////////////////////////////////////

CUpdateService::CUpdateService()
{
	m_nToggle = 0;
}

CUpdateService::~CUpdateService()
{
}

//////////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////////

int CUpdateService::Updator(char *pszNumber, char *pszFileName)
{
	CMobileClient   theClient;

	m_nLength = GetFileSize(pszFileName);
	if (m_nLength == -1)
	{
		printf("ERROR: Cannot open file. (%s)\r\n", pszFileName);
		return 0;
	}

	if (!theClient.Initialize())
		return 0;

	Upload(&theClient, pszNumber, pszFileName);
	theClient.Destroy();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////////////

int CUpdateService::GetFileSize(char *pszFileName)
{
	struct 	stat statbuf;
	int		fd;

	fd = open(pszFileName, O_RDONLY);
	if (fd <= 0)
		return -1;
	fstat(fd, &statbuf);
	close(fd);

	return (int)statbuf.st_size;
}

int CUpdateService::Upload(CMobileClient *pClient, char *pszNumber, char *pszFileName)
{
	int		nState, nError;
	BOOL	bOk, bConnected = FALSE;
	int		n, i, retry, nSeek, nPacket, nElapse, len;
	int		nTotal, nErrorCount, nGoodCount, nLength;
	time_t	start, now;
	char	*p1, szBuffer[128], tmp[128], msg[128];
	CChunk	chunk;
	BYTE	*p;
	double	fLoss;
	WORD	crc;
	BYTE	seq, c;
	FILE	*fp;

	nTotal  	= m_nLength;
	nSeek		= 0;
	seq			= 0;
	nPacket		= m_nPacketSize;
	nErrorCount	= 0;
	nGoodCount	= 0;
	m_nToggle   = 0;

	for(nState=STATE_INIT; !m_bExitSignalPending;)
	{
		switch(nState) {
 		  case STATE_INIT :
			   nState = STATE_TRY_CONNECT;
			   break;

		  case STATE_TRY_CONNECT :
			   nError = ConnectToHost(pClient, pszNumber, pszFileName);
			   if (nError != 0)
			   {
				   if (nError == -2)
				   {
				        printf("Modem is not ready.            \r\n");
						return -2;
				   }
		       	   else printf("Cannot connect to host.        \r\n");
				   return 0;
			   }
			   printf("\r\n");
			   printf("\r\n");
			   bConnected = TRUE;
			   nState = STATE_SELECT_SVC;
		 	   break;

		  case STATE_SELECT_SVC :
			   printf("  File Name : %s\r\n", pszFileName);
			   printf("  File Size : %d Byte(s)\r\n", nTotal);
			   printf("Packet Size : %d Byte(s)\r\n", nPacket);
			   printf("      Retry : %d\r\n", RETRY_LIMIT);
	  		   printf("\r\n");
			   DisplayProgress(nTotal, nSeek, TRUE);
			   nState = STATE_EMMUL;
		 	   break;

		  case STATE_EMMUL :
			   DisplayProgress(nTotal, nSeek, TRUE);
			   time(&start);

			   fp = fopen(pszFileName, "rb");
			   if (fp == NULL)
			   {
					fclose(fp);
			   		printf("\r\n");
			   		printf("\r\nERROR: Cannot open file.\r\n");
					return 0;
			   }

			   for(nSeek=0; nSeek<nTotal;)
			   {
				   if (m_bExitSignalPending)
				   {
					   fclose(fp);
			   		   printf("\r\n");
			   		   printf("\r\nUpload cancel.\r\n");
					   return 0;
				   }
				   
				   len     = MIN(nPacket, nTotal-nSeek);
				   nLength = sizeof(MFRAMEHEADER) + len + sizeof(MFRAMETAIL);
				   memset(&send_frame, 0, sizeof(MFRAME));
				   send_frame.hdr.ident1 = '[';
				   send_frame.hdr.ident2 = '@';
				   send_frame.hdr.seq    = seq;
				   send_frame.hdr.len    = len;
				   fread(send_frame.data, 1, len, fp);
				   crc = GetCrc((BYTE *)&send_frame, len+sizeof(MFRAMEHEADER), 0);
				   p   = (BYTE *)((BYTE *)&send_frame + len + sizeof(MFRAMEHEADER));
				   memcpy(p, (char *)&crc, sizeof(WORD));

				   bOk = FALSE;
				   for(retry=0; (!bOk) && (retry<RETRY_LIMIT); retry++)
				   {
					   if (m_bExitSignalPending)
						   break;

					   n = 0;
					   chunk.Flush();
				   	   pClient->WriteToModem((char *)&send_frame, nLength);
					   for(i=0; i<50; i++)
					   {
						   if (m_bExitSignalPending)
							   break;

			   		   	   n = pClient->ReadFromModem(szBuffer, 100, 100);
						   if (n > 0)
						   {
							   c = szBuffer[0];
							   if (c == 0x06)
							   {  
							       bOk = TRUE;
							       break;
							   }

							   chunk.Add(szBuffer, n);
							   chunk.Close();
							   if (strstr(chunk.GetBuffer(), "NO CARRIER") != NULL)
							   {
			   					   fclose(fp);
				   			       m_bExitSignalPending = TRUE;
								   printf("\r\n");
								   printf("\r\nMobile connection closed. (NO CARRIER)\r\n");
								   return 0;
							   }
							   else
							   {
								   nErrorCount++;
			   	   				   DisplayProgress(nTotal, nSeek, FALSE, retry+1);
							       break;
						       }
						   }
					   }
					   if (n <= 0)	
			   	   		   DisplayProgress(nTotal, nSeek, FALSE, retry+1);
				   }
				   if (!bOk)
			       {
					   fclose(fp);
			   		   printf("\r\n");
			   		   printf("\r\nSending Fail.");
					   return 0;
				   }
				   nSeek += len;
				   nGoodCount++;
				   seq++;
			   	   DisplayProgress(nTotal, nSeek, TRUE);
			   }

			   fclose(fp);
			   printf("\r\n");
			   printf("\r\n");
			   time(&now);
			   nElapse = (int)(now - start);
			   printf("100%% Upload ok. Total elapse=%d min %d seconds.\r\n",
						nElapse/60, nElapse%60);
			   fLoss = (double)(nErrorCount*100) / (double)(nGoodCount+nErrorCount);
			   printf("Error Packet=%d, Good Packet=%d, %.1f%% Loss\r\n",
						nErrorCount, nGoodCount, fLoss); 
			   if(m_bEnableInstall) {
			   	   printf("\r\n");
			   	   printf("Starting firmware install...\r\n");
			   	   nState = STATE_TERM_WAIT;
               } else {
				   usleep(1000000);
                   return 0;
			   }
		 	   break;
			   
		  case STATE_TERM_WAIT :
			   n = pClient->ReadFromModem(szBuffer, 100, 500);
			   if (n > 0)
			   {
			      szBuffer[n] = 0;
				  acceptstr(tmp, szBuffer); 
			   	  sprintf(msg, "Reply '%s'", tmp);
			   	  Message(msg);
				  p1 = strstr(szBuffer, " 101 ");
				  if (p1 != NULL)
				  {	
					  printf("\r\n");
					  printf("\r\nInstall Done.\r\n");
					  return 0;
				  }
			   }
		 	   break;
		}
	}

	printf("\r\n");
	printf("\r\nUpload fail.\r\n");
	return 0;
}

int CUpdateService::ConnectToHost(CMobileClient *pClient, char *pszNumber, char *pszFileName)
{
	char	tmp[256], msg[256];
	char	*p, *p1, szBuffer[256];
	int		i, n, len, nRetry, nState;
	
	nRetry = 0;
	for(nState=STATE_INIT; (!m_bExitSignalPending) && (nState!=STATE_DONE);)
	{
		switch(nState) {
		  case STATE_INIT :
			   printf("Trying %s ...\r\n", pszNumber);
			   nState = STATE_ATZ;
			   break;

		  case STATE_ATZ :
			   Message(const_cast<char*>("Sending 'ATZ'"));
			   pClient->WriteToModem(const_cast<char*>("ATZ\r\n"));
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
			   	  Message(const_cast<char*>("No response."));
				  usleep(200000);
			   }
			   nRetry++;
			   if (nRetry >= 3)
				   return -2;
			   break;

		  case STATE_CRM :
			   Message(const_cast<char*>("Sending 'AT+CRM=129'"));
			   pClient->WriteToModem(const_cast<char*>("AT+CRM=129\r\n"));
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
			   sprintf(szBuffer, "Sending 'ATDT %s'", pszNumber);
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
						   nState = STATE_SELECT;
						   break;
					   }
				   }
			   }
			   break;
	
		  case STATE_SELECT :
			   Message(const_cast<char*>("Sending 'NURI/1.0 PUT'"));
			   usleep(1000000);
			   sprintf(szBuffer, "NURI/1.0 PUT %s\nLength: %d\nInstall: %s\n\n", 
                    m_pRemoteFile, m_nLength, m_bEnableInstall ? "yes" : "no");
			   pClient->WriteToModem(szBuffer);
			   nState = STATE_ACCEPT;
			   break;

		  case STATE_ACCEPT :
			   for(i=0, len=0; !m_bExitSignalPending && (i<10); i++)
			   {
			   	   n = pClient->ReadFromModem(&szBuffer[len], 255, 1000);
			   	   if (n > 0)
			   	   {
					   len += n;
			      	   szBuffer[len] = 0;
				  	   acceptstr(tmp, szBuffer); 
			   	  	   sprintf(msg, "Reply '%s'", tmp);
			   	  	   Message(msg);

				  	   p = strstr(szBuffer, "NO CARRIER");
				  	   if (p != NULL)
					  	   return -1;

				       p = strstr(szBuffer, "NURI/");
				  	   if (p != NULL)
					   {
						   p1 = strstr(p, "\n\n");
						   if (p1 != NULL)
						   {
							   *p1 = '\0';
			   				   printf("\rReply '%s'                  \r", p);
							   fflush(0);
							   sscanf(p, "%s%s", tmp, msg);
							   if (atoi(msg) == 230)
						   	   	   return 0;
							   return -1;
						   }
					   }
				   }
			   }
			   break;
		}
	}
	return -3;
}

void CUpdateService::acceptstr(char *dest, char *src)
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

void CUpdateService::Message(char *pszMessage)
{
	char	szBuffer[128];

	memset(szBuffer, ' ', 79);
	szBuffer[79] = 0;
	memcpy(szBuffer, pszMessage, strlen(pszMessage));
	printf("\r%s\r", szBuffer);
	fflush(0);
}

void CUpdateService::DisplayProgress(int nTotal, int nCurrent, BOOL bError, int nRetry)
{
	double	fRate, fCount;
	char	szBuffer[128];
	char	szError[32];

	fRate  = (double)nCurrent / (double)nTotal;
	fCount = fRate * (double)50;

	sprintf(szError, "ERROR(%d)", nRetry);
	sprintf(szBuffer, PROGRESSBAR,
			fRate*100, PLAYLIST[m_nToggle % 4], !bError ? szError : "        ");
	m_nToggle++;
	memset(&szBuffer[11], '#', (int)fCount);
	printf(szBuffer);
	fflush(0);
}

