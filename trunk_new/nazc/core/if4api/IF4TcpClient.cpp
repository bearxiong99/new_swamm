//////////////////////////////////////////////////////////////////////
//
//	IF4TcpClient.cpp: implementation of the CIF4TcpClient class.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@paran.com
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "varapi.h"
#include "if4def.h"
#include "IF4TcpClient.h"
#include "IF4Service.h"
#include "IF4DataFrame.h"
#include "IF4Util.h"
#ifndef _WIN32
#include "Event.h"
#endif
#include "Chunk.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "zlib.h"

//////////////////////////////////////////////////////////////////////
// CIF4TcpClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIF4TcpClient::CIF4TcpClient()
{
	memset(&m_thisSession, 0, sizeof(WORKSESSION));		// Issue #2036
	m_pInvoke 	 	 = NULL;
	m_nLastSeq	 	 = 0;
	m_nWindowSize	 = IF4_WINDOW_SIZE;
	m_nWindowCount	 = IF4_WINDOW_COUNT;
	m_nBaseSeek		 = 0;
	m_nStartSEQ 	 = 0;
	m_nEndSEQ   	 = 0;
	m_nServerVersion = 100;
	m_pStream 	 = NewStream((UINT_PTR)this);
	m_bHandshake = FALSE;
	m_bMultiPart	 = FALSE;
	m_bMultiPartDone = FALSE;
	m_bExit		 = FALSE;
}

CIF4TcpClient::~CIF4TcpClient()
{
	if (m_pStream != NULL)
	{
		DeleteStream(m_pStream);
		m_pStream = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// CIF4TcpClient Overrides
//////////////////////////////////////////////////////////////////////

BOOL CIF4TcpClient::TryConnectServer(char *pszAddress, int nPort)
{
	unsigned long	addr;
	int		nTime = 0;

	if (nPort <= 0)
		return FALSE;

	if (!pszAddress || !*pszAddress)
		return FALSE;

	addr = inet_addr(pszAddress);
	if (addr == 0 || addr == INADDR_NONE)
	{
	    struct	hostent	*host = gethostbyname(pszAddress);
		if (host == NULL)
			return FALSE;

		addr = (unsigned long)*((unsigned long *)host->h_addr);
	}

//	XDEBUG("Try connect to %s\r\n", pszAddress);
	time(&m_nIdleTime);
 	
	m_bHandshake = FALSE;

	if (!Connect(pszAddress, nPort))
		return FALSE;

	for(;!m_bHandshake;)
	{
        USLEEP(50000);
		nTime++;
		if (nTime > 100)
		{
			XDEBUG("IF4CLIENT: Connect timeout.\r\n");
			return FALSE;
		}
	}

//	XDEBUG("IF4CLIENT: Connected. (nTime=%d ms)\r\n", nTime*50);
	return TRUE;
}

void CIF4TcpClient::DisconnectServer()
{
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CIF4TcpClient Overrides
//////////////////////////////////////////////////////////////////////

BOOL CIF4TcpClient::IsHandshake()
{
	return m_bHandshake;
}

//////////////////////////////////////////////////////////////////////
// CIF4TcpClient Overrides
//////////////////////////////////////////////////////////////////////

void CIF4TcpClient::OnConnect()
{
//	XDEBUG("IF4CLIENT: Connected.\xd\xa");
}

void CIF4TcpClient::OnDisconnect()
{
//	XDEBUG("IF4CLIENT: Disconnected.\xd\xa");
	m_bExit = TRUE;
	if (m_pInvoke != NULL)
		SetEvent(m_pInvoke->hEvent);
}

void CIF4TcpClient::OnConnectError()
{
//	XDEBUG("IF4CLIENT: Connect Error.\xd\xa");
	m_bExit = TRUE;
	if (m_pInvoke != NULL)
		SetEvent(m_pInvoke->hEvent);
}

void CIF4TcpClient::OnSend(LPSTR pszBuffer, int nLength)
{
//	XDEBUG("IF4CLIENT: Sending %0d Byte(s)\xd\xa", nLength);
//	XDUMP(pszBuffer, MIN(nLength, 50), TRUE);
}

BOOL CIF4TcpClient::OnReceive(LPSTR pszBuffer, int nLength)
{
//	XDEBUG("IF4CLIENT: Receive %0d Byte(s)\xd\xa", nLength);
//	XDUMP(pszBuffer, nLength, TRUE);

	return CIF4Stream::ParseStream((WORKSESSION *)&m_thisSession, m_pStream, (BYTE *)pszBuffer, nLength);
}

void CIF4TcpClient::OnError(int nType)
{
//	XDEBUG("IF4CLIENT: Error.\xd\xa");
}

//////////////////////////////////////////////////////////////////////
// CIF4TcpClient Overrides
//////////////////////////////////////////////////////////////////////

void CIF4TcpClient::SendAck(BYTE nSeq)
{
	IF4_CTRL_FRAME	frame;
	int		nLength;
	BYTE	arg[1];

	XDEBUG("IF4CLIENT: ACK (SEQ=%0d)\xd\xa", nSeq);
	arg[0] = nSeq;
	nLength = MakeControlFrame(&frame, IF4_ACK, (char *)arg, 1);
	AddSendStream((char *)&frame, nLength);
}

void CIF4TcpClient::SendNak(BYTE nSeq)
{
	IF4_CTRL_FRAME	frame;
	int		nLength;
	BYTE	arg[1];

	XDEBUG("IF4CLIENT: NAK (SEQ=%0d)\xd\xa", nSeq);
	arg[0] = nSeq;
	nLength = MakeControlFrame(&frame, IF4_NAK, (char *)arg, 1);
	AddSendStream((char *)&frame, nLength);
}

void CIF4TcpClient::SendWck(BYTE nStart, BYTE nEnd)
{
	IF4_CTRL_FRAME	frame;
	int		nLength;
	BYTE	arg[2];

	XDEBUG("IF4CLIENT: WCK (START=%0d, END=%0d)\xd\xa", nStart, nEnd);
	arg[0] = nStart;
	arg[1] = nEnd;
	nLength = MakeControlFrame(&frame, IF4_WCK, (char *)arg, 2);
	AddSendStream((char *)&frame, nLength);
}

void CIF4TcpClient::SendEot()
{
	IF4_CTRL_FRAME	frame;
	int		nLength;
	BYTE	arg[1];

	XDEBUG("IF4CLIENT: EOT\xd\xa");
	arg[0]  = 0;
	nLength = MakeControlFrame(&frame, IF4_EOT, (char *)arg, 1);
	AddSendStream((char *)&frame, nLength);
}

void CIF4TcpClient::SetInvoke(IF4Invoke *pInvoke)
{
	m_pInvoke = pInvoke;
}

int CIF4TcpClient::SendRequest()
{
	int		nRetry, nResult;

	if (m_pInvoke == NULL)
		return FALSE;

	m_nBaseSeek		 = 0;
	m_bMultiPart	 = FALSE;
	m_bMultiPartDone = FALSE;

	// Single Frame (크기가 작거나 서버가 IF4 Protocol V1.0일때
	if ((m_pInvoke->nLength <= IF4_MIN_MULTI_PART_SIZE) || (m_nServerVersion == 100))
	{
		SendFrame();
		return 1;
	}

	// Multi Part Frame V1.1만 지원됨
	m_pInvoke->nFlag = IF4_FLAG_ACK;
	m_bMultiPart = TRUE;
	SendMultiFrame();

	for(nRetry=0;;)
	{
		nResult = WaitForSingleObject(m_pInvoke->hEvent, 3);
		ResetEvent(m_pInvoke->hEvent);

		if (nResult == ETIMEDOUT)
		{
			XDEBUG("IF4CLIENT: TIMEOUT\r\n");
			nRetry++;
			if (nRetry > 3)
			{
				// 요청 전송 실패
				return -1;
			}

			// 3초동안 응답이 없으면 WCK를 다시 전송한다.
			SendWck(m_nStartSEQ, m_nEndSEQ);
		}
		else
		{
//			XDEBUG("IF4CLIENT: REPLY %s\r\n", m_pInvoke->nCtrlCode == IF4_CTRLCODE_ACK ? "ACK" : "NAK");
			if (m_pInvoke->nCtrlCode == IF4_CTRLCODE_ACK)
			{
				// 마지막까지 모두 성공했는지 확인한다.
				if (m_pInvoke->nLength <= m_nBaseSeek)
				{
					XDEBUG("IF4CLIENT: SEND OK\r\n");
					m_nBaseSeek = 0;
					break;
				}

				// 다음 부분을 전송한다.
				nRetry = 0;
				SendMultiFrame();
			}
		    else if (m_pInvoke->nCtrlCode == IF4_CTRLCODE_NAK)
		    {
				RetryMultiFrame();
		    }
			else
			{
				// Cancel or Connection close	
				break;
			}
		}
	}
	return 0;
}

void CIF4TcpClient::SendFrame()
{
	if (m_pInvoke == NULL)
		return;

	// Single Frame
	AddSendStream(m_pInvoke->pFrame, m_pInvoke->nLength);
}

BOOL CIF4TcpClient::SendMultiFrame()
{
	char	*pSeek, *pszBuffer, *pszSource;
	int		i, nSeek, nLength=0, len, nSize, nSrcLen, n;
	BYTE	nCount=0, seq, attr;

	if (m_pInvoke == NULL)
		return FALSE;

	pszBuffer = (char *)MALLOC(m_nWindowSize+sizeof(IF4_HEADER)+sizeof(IF4_TAIL));
	if (pszBuffer == NULL)
		return FALSE;

	memset(m_nSeqList, 0, m_nWindowCount);
	memset(m_nSendFlag, 1, m_nWindowCount);

	pszSource = m_pInvoke->pFrame;
	nSize = m_pInvoke->nLength;
	nSrcLen = m_pInvoke->nSourceLength;

//	XDEBUG("MULTI-PART-FRAME: %0d bytes\r\n", nSize);
//	XDUMP(pszSource, sizeof(IF4_HEADER), TRUE);
//	XDUMP(pszSource+(nSize-sizeof(IF4_TAIL)), sizeof(IF4_TAIL), TRUE);

	for(i=0; i<m_nWindowCount; i++)
	{
		nSeek = m_nBaseSeek + (m_nWindowSize * i) - sizeof(IF4_COMPRESS_HEADER);
		pSeek = pszSource + nSeek;
		nLength = nSize - nSeek;
		if (nLength <= 0)
			break;

		seq = m_nStartSEQ + nCount;
		m_nSeqList[i] = seq;

		attr  = (nSeek <= 0) ? IF4_ATTR_START : 0;
		attr |= ((nLength-m_nWindowSize) <= 0) ? IF4_ATTR_END : 0;

		if ((attr & IF4_ATTR_START) > 0)
		{
			XDEBUG("IF4CLIENT: SOURCE SIZE = %d, TOTAL SIZE = %d\xd\xa", nSrcLen, nSize+sizeof(IF4_COMPRESS_HEADER));
			n = MIN((int)(m_nWindowSize-sizeof(IF4_COMPRESS_HEADER)), m_nWindowSize);
			XDEBUG("IF4CLIENT: PARTIAL (SEQ=%0d) attr=0x%02X, len=%0d\xd\xa", seq, attr, n);
			len = MakeIF4MultipartFrame((IF4_MULTI_FRAME *)pszBuffer, attr, seq, 
							1, nSrcLen, (BYTE *)pszSource, n);
		}
		else
		{
			XDEBUG("IF4CLIENT: PARTIAL (SEQ=%0d) attr=0x%02X, len=%0d\xd\xa", seq, attr, MIN(nLength, m_nWindowSize));
			len = MakeIF4MultipartFrame((IF4_MULTI_FRAME *)pszBuffer, attr, seq, (BYTE *)pSeek, MIN(nLength, m_nWindowSize));
		}

		AddSendStream(pszBuffer, len);
		nCount++;
        USLEEP(50000);
	}

	FREE(pszBuffer);

	// Check window
	m_nEndSEQ = m_nStartSEQ + (nCount - 1);
	SendWck(m_nStartSEQ, m_nEndSEQ);
	
	return (nLength > 0) ? TRUE : FALSE;
}

void CIF4TcpClient::RetryMultiFrame()
{
	char	*pSeek, *pszBuffer, *pszSource;
	int		i, nSeek, nLength=0, len, nSize, nSrcLen, n;
	BYTE	seq, attr;

	if (m_pInvoke == NULL)
		return;

	pszBuffer = (char *)MALLOC(m_nWindowSize+sizeof(IF4_HEADER)+sizeof(IF4_TAIL));
	if (pszBuffer == NULL)
		return;

	pszSource = m_pInvoke->pFrame;
	nSize = m_pInvoke->nLength;
	nSrcLen = m_pInvoke->nSourceLength;

	for(i=0; i<m_nWindowCount; i++)
	{
//		XDEBUG("SEQ=%d, FLAG=%d\r\n", m_nSeqList[i], m_nSendFlag[i]);
		if (m_nSendFlag[i] == TRUE)
			continue;

		nSeek = m_nBaseSeek + (m_nWindowSize * i) - sizeof(IF4_COMPRESS_HEADER);
		pSeek = pszSource + nSeek;
		nLength = nSize - nSeek;
		if (nLength <= 0)
			break;

		seq   = m_nSeqList[i];
		attr  = (nSeek <= 0) ? IF4_ATTR_START : 0;
		attr |= ((nLength-m_nWindowSize) <= 0) ? IF4_ATTR_END : 0;

		if ((attr & IF4_ATTR_START) > 0)
		{
			n = MIN((int)(m_nWindowSize-sizeof(IF4_COMPRESS_HEADER)), m_nWindowSize);
			XDEBUG("IF4CLIENT: PARTIAL RETRY (SEQ=%0d) attr=0x%02X, len=%0d\xd\xa", seq, attr, n);
			len = MakeIF4MultipartFrame((IF4_MULTI_FRAME *)pszBuffer, attr, seq, 
							1, nSrcLen, (BYTE *)pszSource, n);
		}
		else
		{
			XDEBUG("IF4CLIENT: PARTIAL RETRY (SEQ=%0d) attr=0x%02X, len=%0d\xd\xa", seq, attr, MIN(nLength, m_nWindowSize));
			len = MakeIF4MultipartFrame((IF4_MULTI_FRAME *)pszBuffer, attr, seq, (BYTE *)pSeek, MIN(nLength, m_nWindowSize));
		}

		AddSendStream(pszBuffer, len);
	}

	FREE(pszBuffer);

	// Check window
	SendWck(m_nStartSEQ, m_nEndSEQ);
}

BOOL CIF4TcpClient::Idle()
{
	time_t	now;

	for(;!m_bExit;)
	{	
        USLEEP(1000000);

	 	time(&now);
		if ((now-m_nIdleTime) > 60)
			return FALSE;

		if ((now-m_pInvoke->tmLastActive) > 10)
		{
			SendAck(m_nLastSeq);
			time(&m_pInvoke->tmLastActive);
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CIF4TcpClient Overrides
//////////////////////////////////////////////////////////////////////

BOOL CIF4TcpClient::OnFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
	IF4_HEADER	*pHeader;
	char		*pszAllocate = NULL;
	uLong		nDestLen;
	int			nError;
	BOOL		bResult;

	pHeader = (IF4_HEADER *)pszBuffer;
	if (pHeader->attr & IF4_ATTR_COMPRESS)
	{
		pszAllocate = (char *)MALLOC(500000);
		if (pszAllocate == NULL)
			return TRUE;

		memcpy(pszAllocate, pszBuffer, sizeof(IF4_HEADER));
		nDestLen = 500000 - sizeof(IF4_HEADER);
		nError = uncompress((Bytef*)&pszAllocate[sizeof(IF4_HEADER)],
							&nDestLen,
							(const Bytef*)&pszBuffer[sizeof(IF4_HEADER)],
							nLength-(sizeof(IF4_HEADER)+sizeof(IF4_TAIL)));
		XDEBUG("UNCOMPRESS: SOURCE=%d byte(s), DEST=%d byte(s), nError=%d\r\n",
							nLength-(sizeof(IF4_HEADER)+sizeof(IF4_TAIL)),
							(int)nDestLen, 
							nError);

		if ((nDestLen > 0) && (nError == Z_OK))
		{
			nLength = nDestLen + sizeof(IF4_HEADER) + sizeof(IF4_TAIL);
			pszBuffer = (BYTE *)pszAllocate;
			pHeader->len = HostToLittleInt(nDestLen);
        }
        else
        {
			FREE(pszAllocate);
			return TRUE;
        }
	}

	bResult = OnRecvFrame(pSession, pStream, pszBuffer, nLength);

	if (pszAllocate != NULL) FREE(pszAllocate);
	return bResult;
}

BOOL CIF4TcpClient::OnRecvFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
	IF4_HEADER	*pHeader;
	BYTE		seq;
	BOOL		bOK;

	if (m_pInvoke == NULL)
		return TRUE;

 	time(&m_pInvoke->tmLastActive);
	pHeader = (IF4_HEADER *)pszBuffer;
	if (pHeader->attr & IF4_ATTR_CTRLFRAME)
	{
		// Control Frame
		return ControlFrame(pszBuffer, nLength);
	}
	else
	{
		// Calculate Next Sequence
		if (pStream->nLastSeq == -1)
			 seq = pHeader->seq;
		else seq = (BYTE)(pStream->nLastSeq + 1);
		bOK = (seq == pHeader->seq) ? TRUE : FALSE;

		// Check Want Confirmation
		if (pHeader->attr & IF4_ATTR_MUSTCONFIRM)
		{
			XDEBUG("IF4CLIENT-REPLY: [SEQ=%0d][%0d] Confirmation %s\xd\xa",
					pHeader->seq, seq,
					(bOK == TRUE) ? "ACK" : "NAK");
			if (bOK)
			{
				SendAck(pHeader->seq);
			}
			else
			{
				SendNak(seq);
				return TRUE;
			}
		}
		else
		{
			XDEBUG("IF4CLIENT: [SEQ=%0d][%0d] Don't Care Sequence!!\xd\xa",
					pHeader->seq, seq);
		}

		// Store Last Sequence
		m_nLastSeq		  = pHeader->seq;
		pStream->nLastSeq = pHeader->seq;

		// Check Multi-Part Frame
		if ((pHeader->attr & IF4_ATTR_START) && (pHeader->attr & IF4_ATTR_END))
		{
			// Single Frame
			XDEBUG("IF4CLIENT-REPLY: Single Frame (SEQ=%0d, LEN=%0d)\xd\xa",
						pHeader->seq, LittleToHostInt(pHeader->len));
		}
		else
		{
			// Multi-Part Frame
			if (pHeader->attr & IF4_ATTR_START)
			{
				pStream->nTotalSize = 0;
			    XDEBUG("IF4CLIENT-REPLY: ----- Start Multi-Part Frame (SEQ=%0d) -----\xd\xa", pHeader->seq);
			}
			else if (pHeader->attr & IF4_ATTR_END)
			{
				XDEBUG("IF4CLIENT-REPLY: ----- End Multi-Part Frame (SEQ=%0d) -----\xd\xa", pHeader->seq);
			}
		
			pStream->nTotalSize += LittleToHostInt(pHeader->len);
			XDEBUG("IF4CLIENT-REPLY: Partial Block (SEQ=%0d, LEN=%0d, TOTAL=%0d)\xd\xa",
						pHeader->seq, LittleToHostInt(pHeader->len), pStream->nTotalSize);
			return TRUE;
		}

		// Service Frame
		switch(pHeader->svc) {
		  case IF4_SVC_COMMAND :	return CommandFrame(pszBuffer, nLength);
		  case IF4_SVC_ALARM :		return AlarmFrame(pszBuffer, nLength);
		  case IF4_SVC_EVENT :		return EventFrame(pszBuffer, nLength);
		  case IF4_SVC_DATA :		return DataFrame(pszBuffer, nLength);
		  case IF4_SVC_TRANSFER :	return TransferFrame(pszBuffer, nLength);
		  default :					XDEBUG("ERROR: Unknown Service Frame\xd\xa"); break;
		}
	}
	return TRUE;
}

BOOL CIF4TcpClient::ControlFrame(BYTE *pszBuffer, int nLength)
{
	IF4_CTRL_FRAME	*pCtrl;
	IF4_CTRL_ENQ	*pENQ;
	int				i, j, ver;

	pCtrl = (IF4_CTRL_FRAME *)pszBuffer;
	switch(pCtrl->code) {
	  case IF4_ENQ :
		   pENQ = (IF4_CTRL_ENQ *)&pCtrl->arg;

		   // Check version 1.1		  
		   ver = (pENQ->ver[0] * 100) + pENQ->ver[1];
		   m_nServerVersion = ver;

		   if (ver >= 101)
		   {
			   // Version 1.1
			   m_nWindowSize  = LittleToHostShort(pENQ->window_size);
			   m_nWindowCount = MIN(pENQ->window_count, MAX_SESSION_WINDOW);

               if(m_nWindowSize > 4096 || m_nWindowCount > 8)
               {
		            XDEBUG("\033[1;40;31mIF4CLIENT-REPLY: INVALID [ENQ][%0d] Version=%0d.%0d, WindowSize=%0d, WindowCount=%0d\033[0m\xd\xa",
						 LittleToHostShort(pCtrl->len),
						 BCD2BYTE(pENQ->ver[0]), BCD2BYTE(pENQ->ver[1]),
						 m_nWindowSize, m_nWindowCount);

			        m_nWindowSize  = IF4_WINDOW_SIZE;
			        m_nWindowCount = IF4_WINDOW_COUNT;
               }
		   }
		   if (ver < 101 || !m_nWindowSize || !m_nWindowCount)
		   {
			   // Version 1.0
			   m_nWindowSize  = IF4_WINDOW_SIZE;
			   m_nWindowCount = IF4_WINDOW_COUNT;
		   }
 
		   XDEBUG("IF4CLIENT-REPLY: [ENQ][%0d] Version=%0d.%0d, WindowSize=%0d, WindowCount=%0d\xd\xa",
						 LittleToHostShort(pCtrl->len),
						 BCD2BYTE(pENQ->ver[0]), BCD2BYTE(pENQ->ver[1]),
						 m_nWindowSize, m_nWindowCount);

		   m_bHandshake = TRUE;
		   break;
	  case IF4_ACK :
		   XDEBUG("IF4CLIENT-REPLY: [ACK] Last Sequence=%0d\xd\xa", pCtrl->arg[0]);
		
		   // Multi Part Frame	
		   if (m_bMultiPart)
		   {
			   m_nStartSEQ = m_nEndSEQ;
			   m_nStartSEQ++;
			   m_nBaseSeek += (m_nWindowSize * m_nWindowCount);
		   }

		   // Single Frame
		   m_pInvoke->nCtrlCode = IF4_CTRLCODE_ACK;
		   if (m_pInvoke->nFlag & IF4_FLAG_ACK)
			   SetEvent(m_pInvoke->hEvent);
		   break;
	  case IF4_NAK :
		   XDEBUG("IF4CLIENT-REPLY: [NAK] %0d Packet Error\xd\xa",
					LittleToHostShort(pCtrl->len));
		   for(i=0; i<pCtrl->len; i++)
			   XDEBUG("     SEQ(%0d): Missing\r\n", pCtrl->arg[i]);

		   // Multi Part Frame	
		   if (m_bMultiPart)
		   {
			   // Clear send flag
			   for(i=0; i<LittleToHostShort(pCtrl->len); i++)
			   {
				   for(j=0; j<IF4_WINDOW_COUNT; j++)
				   {
					   if (m_nSeqList[j] == pCtrl->arg[i])
					   {
			   		   	   m_nSendFlag[j] = FALSE;
						   break;
					   }
				   }
			   }
		   }

		   // Single Frame
		   m_pInvoke->nCtrlCode = IF4_CTRLCODE_NAK;
		   if (m_pInvoke->nFlag & IF4_FLAG_ACK)
			   SetEvent(m_pInvoke->hEvent);
		   break;
	  case IF4_CAN :
		   XDEBUG("IF4CLIENT-REPLY: [CAN][%0d] Cancel Frame TID=%0d\xd\xa",
						 LittleToHostShort(pCtrl->len), pCtrl->arg[0]);
		   m_pInvoke->nCtrlCode = IF4_CTRLCODE_CAN;
		   m_bMultiPart = FALSE;
		   m_bMultiPartDone = FALSE;
		   m_bExit = TRUE;
		   if (m_pInvoke->nFlag & IF4_FLAG_ACK)
			   SetEvent(m_pInvoke->hEvent);
		   break;
	  case IF4_EOT :
		   XDEBUG("IF4CLIENT-REPLY: [EOT][%0d] Request Disconnect.\xd\xa", LittleToHostShort(pCtrl->len));
		   m_pInvoke->nCtrlCode = IF4_CTRLCODE_CAN;
		   m_bMultiPart = FALSE;
		   m_bMultiPartDone = FALSE;
		   m_bExit = TRUE;
		   if (m_pInvoke->nFlag & IF4_FLAG_ACK)
			   SetEvent(m_pInvoke->hEvent);
		   return FALSE;
	  case IF4_AUT :
		   XDEBUG("IF4CLIENT-CTRL: [AUT][%0d] Session Key=", LittleToHostShort(pCtrl->len));
		   XDUMP(pCtrl->arg, LittleToHostShort(pCtrl->len));
		   break;
	  case IF4_WCK :
		   XDEBUG("IF4CLIENT-REPLY: [WCK][%0d] Star=%0d, End=%0d\xd\xa",
						 LittleToHostShort(pCtrl->len), pCtrl->arg[0], pCtrl->arg[1]);

		   SendAck(pCtrl->arg[1]);
		   break;
	  default :
		   XDEBUG("IF4CLIENT-CTRL: Unknown Control Frame.\xd\xa");
		   break;
	}
	return TRUE;
}

BOOL CIF4TcpClient::CommandFrame(BYTE *pszBuffer, int nLength)
{
	IF4_CMD_FRAME		*pFrame;
	VAROBJECT			*pObject;
	SMIValue			*pSeek;
	MIBValue			*pValue;
	char				szOID[20];
	int					i;

	pFrame 	 = (IF4_CMD_FRAME *)pszBuffer;
	OidToString(&pFrame->ch.cmd, szOID);

	if (!(pFrame->ch.attr & IF4_CMDATTR_REQUEST))
	{
		XDEBUG("IF4CLIENT-REPLY: CMD=\"%s\", TID=%0d, REPLY=%s, RESULT-CNT=%0d\xd\xa",
				szOID,
				pFrame->ch.tid,
				pFrame->ch.attr & IF4_CMDATTR_RESPONSE ? "WANT-REPLY" : "NO-REPLY",
				LittleToHostShort(pFrame->ch.cnt));
		XDEBUG("          ERRCODE(%0d)=%s\xd\xa",
				pFrame->ch.errcode,
				GetIF4ErrorMessage(pFrame->ch.errcode));

        SmiToHostValue((char *)&pFrame->args, LittleToHostShort(pFrame->ch.cnt));
		DumpSmiValue((char *)&pFrame->args, LittleToHostShort(pFrame->ch.cnt));

		pSeek = (SMIValue *)&pFrame->args;
		for(i=0; i<LittleToHostShort(pFrame->ch.cnt); i++)
		{
			pValue = (MIBValue *)MALLOC(sizeof(MIBValue));
			if (pValue == NULL)
				break;
			
			memset(pValue, 0, sizeof(MIBValue));
			pObject = VARAPI_GetObject(&pSeek->oid);
			if (pObject != NULL)
				 CopySMI2MIBValue(pValue, pSeek, pObject->var.type);
			else CopySMI2MIBValue(pValue, pSeek, VARSMI_OPAQUE);
	
			if (m_pInvoke->pResultNode == NULL)
			{
				m_pInvoke->pResultNode = pValue;
				m_pInvoke->pLastResult = pValue;
			}
			else
			{
				m_pInvoke->pLastResult->pNext = pValue;
				m_pInvoke->pLastResult = pValue;
			}
			m_pInvoke->nResultCount++;
			pSeek = (SMIValue *)((BYTE *)pSeek + (pSeek->len + 5)); 
		}

		if (m_pInvoke->nResultCount > 0)
		{
			pValue = m_pInvoke->pResultNode;
			m_pInvoke->pResult = (MIBValue **)MALLOC(sizeof(MIBValue *)*m_pInvoke->nResultCount);
			for(i=0; i<m_pInvoke->nResultCount; i++)
			{
				m_pInvoke->pResult[i] = pValue;
				pValue = pValue->pNext;
			}
		}
	
		m_pInvoke->nError = pFrame->ch.errcode;
		SetEvent(m_pInvoke->hEvent);
	}
	else
	{
		XDEBUG("IF4CLIENT-REQUST-FROM-SERVER: CMD=%s, TID=%0d, ARG-CNT=%0d, ERRCODE=%s(%0d)\xd\xa",
				szOID,
				pFrame->ch.tid,
				LittleToHostShort(pFrame->ch.cnt),
				GetIF4ErrorMessage(pFrame->ch.errcode),
				pFrame->ch.errcode);
//		DumpSmiValue((char *)&pFrame->args, LittleToHostShort(pFrame->ch.cnt));
	}
	return TRUE;
}

BOOL CIF4TcpClient::AlarmFrame(BYTE *pszBuffer, int nLength)
{
	return TRUE;
}

BOOL CIF4TcpClient::EventFrame(BYTE *pszBuffer, int nLength)
{
	return TRUE;
}

BOOL CIF4TcpClient::DataFrame(BYTE *pszBuffer, int nLength)
{
	return TRUE;
}

BOOL CIF4TcpClient::TransferFrame(BYTE *pszBuffer, int nLength)
{
//	time(&m_nIdleTime);
//	if (m_pInvoke->pfnCallback)
//		return m_pInvoke->pfnCallback((void *)m_pInvoke, (char *)pszBuffer, nLength);
	return TRUE;
}

