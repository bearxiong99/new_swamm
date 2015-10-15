//////////////////////////////////////////////////////////////////////
//
//  Ondemander.cpp: implementation of the CI210Parser class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
//  http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codiapi.h"
#include "i210.h"
#include "Ondemander.h"
#include "Chunk.h"
#include "Event.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CI210Parser Class
//////////////////////////////////////////////////////////////////////

#define ASCII_STX				0x02
#define ASCII_EOT				0x03

#define STATE_OPEN				0
#define STATE_WAIT_CONNECT		1
#define STATE_SEND_COMMAND		2
#define STATE_WAIT_REPLY		3
#define STATE_RETRY				4
#define	STATE_OK				5
#define STATE_DONE				6

typedef struct  _tagGETABLE
{
        char    table[4];
        WORD    len; 
        BYTE    data[0];
}   __attribute__ ((packed)) GETABLE;
    
// 12 Bytes = HEADER(6)+ACK(1)+LENGTH(2)+...+CHKSUM(1)+CRC16(2)
// 12 + DATA(47) = 59 Bytes
// 59 Bytes = Max 1 Packet Ondemand Send Length

//#define	ANSI_PREAD_MAX_COUNT	47
#define	ANSI_PREAD_MAX_COUNT	60
#define	ANSI_TABLE_MAX_SIZE		4096

BOOL	m_bPingTestMode = FALSE;
UINT	m_nMeterReadInterval = 60;

BYTE ge_negotiate[4] = { 0x01, 0x00, 0x01, 0x06 };
BYTE ge_logon[10] 	 = { 'a','i','m','i','r',0,0,0,0,0 };
BYTE ge_security[20] = { 'M','A','S','T','E','R','_','0','1','2',0,0,0,0,0,0,0,0,0,0 };
const char *dayofweek[] 	 = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
   
unsigned short BigToHostShort(unsigned short nValue);
unsigned int BigToHostInt(unsigned int nValue);

//////////////////////////////////////////////////////////////////////
// CI210Parser Class
//////////////////////////////////////////////////////////////////////

CI210Parser::CI210Parser() : CMeterParser(1, "GE", "GE I210/KV2C Parser")
{
}

CI210Parser::~CI210Parser()
{
}

//////////////////////////////////////////////////////////////////////
// CI210Parser Operations
//////////////////////////////////////////////////////////////////////

char	m_szModel[64] = "";
char	m_szSerial[64] = "";
BYTE	hwVersion = 0;
BYTE	fwVersion = 0;

void Setup(EUI64 *id, char *model, char *serial, BYTE hw, BYTE fw)
{
	strcpy(m_szModel, model);
	strcpy(m_szSerial, serial);
	hwVersion = hw;
	fwVersion = fw;
}

int CI210Parser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
	BYTE	nOperCode;

	*pCommand->pnReply = 0;
	switch(pCommand->nType) {
	  case PARSE_TYPE_READ :
		   return GE_MAIN(pCommand, pStat);

	  case PARSE_TYPE_WRITE :
		   if ((pCommand->nTable != 131) || (pCommand->nLength == 0))
			   return ONDEMAND_ERROR_PARAM;

		   nOperCode = (pCommand->pszParam[0] & 0xff);
		   switch(nOperCode) {
	  		 case RELAY_ACTIVATION_OFF :
				  pCommand->nTable = WRITE_OPTION_ACTOFF;
				  break;
	  		 case RELAY_ACTIVATION_ON :
				  pCommand->nTable = WRITE_OPTION_ACTON;
				  break;
	  		 case RELAY_SWITCH_ON :
				  pCommand->nTable = WRITE_OPTION_RELAYON;
				  break;
	  		 case RELAY_SWITCH_OFF :
				  pCommand->nTable = WRITE_OPTION_RELAYOFF;
				  break;
		     default :
				  return ONDEMAND_ERROR_PARAM;
		   }
	  	   return GE_MAIN(pCommand, pStat);

	  case PARSE_TYPE_TIMESYNC :
		   return GE_MAIN(pCommand, pStat);
	}
    return 0;
}

int CI210Parser::GE_MAIN(CMDPARAM *pCommand, METER_STAT *pStat)
{
	char			szGUID[20];
	EUI64			*id;
	char			*pszData;
	int				*nReturnSize;
	int				nParam;
	STAT_TABLE		stat_tbl;
	ONDEMANDPARAM	*pOndemand = NULL;
	int				nRetry, nLength = 0;
	int				nWaitTime, nState;
	int				nError, nTotal=0, nReplyTime;
	int				nResult, nElapse, nReplyCount; //, nWorker;
	int				nInterval;
	time_t			tmStart, tmEnd;
	CChunk			chunk(512);
	TIMETICK       	startTime, endTime;
	int				nnnn = 0;
    
	id 			= &pCommand->id;					// Sensor ID
	pszData 	= pCommand->pszReply;				// Return buffer
	nReturnSize = pCommand->pnReply;				// Return buffer size
	nParam 		= pCommand->nTable;					// Function
	nInterval 	= pCommand->nInterval;				// Read interval of meter data
	if (nInterval <= 0)
		nInterval = m_nMeterReadInterval;			// Default 48 intervals

	if (nParam > OPTION_MAX)
		return ONDEMAND_ERROR_PARAM;

	nState = STATE_OPEN;
	nWaitTime = 0;

	nResult = ONDEMAND_ERROR_NOT_CONNECT;
	eui64toa(id, szGUID);
	nReplyCount = 0;

	XDEBUG("GE: TYPE=%s, TABLE=%d, Interval=%d\r\n",
			pCommand->nMeteringType == METERING_TYPE_SPECIAL ? "SPECIAL" : "ROUTINE",
			nParam, nInterval);
	
	for(nRetry=0; (nRetry<3) && (nState!=STATE_DONE);)
	{
		switch(nState) {
		  case STATE_OPEN :
			   nTotal = 0;
			   time(&tmStart);
			   LockOperation();
			   pOndemand = FindOperation(id);
			   if (pOndemand != NULL)
			   {
			   	   UnlockOperation();
				   return ONDEMAND_ERROR_BUSY;
			   } 
			   pOndemand = NewOperation(ONDEMAND_OPR_DATA, id, 0, pszData);
			   pOndemand->nMeteringType = pCommand->nMeteringType;
			   pOndemand->nInterval = nInterval;
			   pOndemand->pParser = (void *)this;
			   pOndemand->nType = METER_TYPE_GE;	
			   UnlockOperation();

			   nWaitTime = 0;
			   GetTimeTick(&pOndemand->tmStart);

			   nError = endiOpen(pCommand->codi, (HANDLE *)&pOndemand->endi, id);
			   if (nError != CODIERR_NOERROR)
			   {
				   nState = STATE_RETRY;
				   break;
			   }

			   endiSetOption(pOndemand->endi, ENDIOPT_SEQUENCE, 1);

			   nError = endiConnect(pOndemand->endi, 3000, TRUE);
			   if (nError != CODIERR_NOERROR)
			   {
				   nState = STATE_RETRY;
				   break;
			   }

			   pOndemand->ansi.packet_cmd = ANSI_IDENT;
			   pOndemand->ansi.packet_retry = 0;
			   chunk.Flush();

			   GetTimeTick(&pOndemand->tmConnect);

			   SendCommand(pOndemand, id, "\x06", 1);
			   usleep(500000);
		       nState = STATE_SEND_COMMAND;
			   break;
		
		  case STATE_SEND_COMMAND :
			   LockOperation();
			   ClearOperation(pOndemand);
			   ResetEvent(pOndemand->hEvent);
			   UnlockOperation();

			   nState = STATE_WAIT_REPLY;
			   pOndemand->bResult = FALSE;
			   pOndemand->nFrameState = ANSI_FRAME_READ1;
			   pOndemand->nFrameLength = 0;

			   GetTimeTick(&startTime);
			   GetTimeTick(&pOndemand->tmSend);

			   SendMessage(pOndemand, id);

//			   if (m_bPingTestMode)
//				   GetTimeTick(&startTime);
			   break;

		  case STATE_WAIT_REPLY :
			   WaitForSingleObject(pOndemand->hEvent, 2);
			   GetTimeTick(&endTime);
			   nReplyTime = GetTimeInterval(&startTime, &endTime);
			   nnnn += nReplyTime;
			   XDEBUG("-------- Elapse time = %d ms (Total %d ms) -----------\r\n", nReplyTime, nnnn);

    		   if (pOndemand->bResult == FALSE)
			   {
        		   if (pOndemand->ansi.packet_retry < METER_RETRY)
				   {	
            			pOndemand->ansi.packet_retry++;
						nState = STATE_SEND_COMMAND;
        		   }   
        		   else nState = STATE_OK;
    		   }
			   else
			   {
			   	   if (m_bPingTestMode)
					   nReplyCount++;

				   if (pOndemand->ansi.packet_cmd != ANSI_TERMI)
				   {
					   //chunk.Add(pOndemand->pszBuffer, pOndemand->nLength);
					   nTotal += pOndemand->nLength;
					   GetMessage(pOndemand, &chunk, pOndemand->pszBuffer, pOndemand->nLength, nParam);
					   nState = STATE_SEND_COMMAND;
				   }
				   else
				   {
					   SendCommand(pOndemand, id, "\x06", 1);
					   nState = STATE_OK;
				   }
			   }
			   break;

		  case STATE_OK :
			   endiDisconnect(pOndemand->endi);

			   GetTimeTick(&pOndemand->tmEnd);
			   time(&tmEnd);

			   nLength = pOndemand->nLength;
			   if (pOndemand->bResult == TRUE)
			   {
					nResult = ONDEMAND_ERROR_OK;
				    if (pOndemand->stat.nCount == 0)
						 nElapse = 0;
					else nElapse = pOndemand->stat.nResponseTime / pOndemand->stat.nCount;
					pOndemand->stat.nUseTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmEnd);
					pOndemand->stat.nConnectTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmConnect);

					memset(&stat_tbl, 0, sizeof(STAT_TABLE));
					stat_tbl.TRANSMIT_PKT_COUNT		= (UINT)pOndemand->stat.nCount;
					stat_tbl.SEND_PKT_COUNT			= (UINT)pOndemand->stat.nSendCount;
					stat_tbl.RECV_PKT_COUNT			= (UINT)pOndemand->stat.nRecvCount;
					stat_tbl.ERROR_PKT_COUNT		= (UINT)pOndemand->stat.nError;
					stat_tbl.TOTAL_SEND_BYTES		= (UINT)pOndemand->stat.nSend;
					stat_tbl.TOTAL_RECV_BYTES		= (UINT)pOndemand->stat.nRecv;
					stat_tbl.TOTAL_USE_MSEC			= (UINT)pOndemand->stat.nUseTime;
					stat_tbl.SENSOR_CONNECT_MSEC	= (UINT)pOndemand->stat.nConnectTime;
					stat_tbl.TOTAL_REPLY_WAIT_MSEC	= (UINT)pOndemand->stat.nResponseTime;	
					stat_tbl.MIN_REPLY_MSEC			= (UINT)pOndemand->stat.nMinTime;
					stat_tbl.MAX_REPLY_MSEC			= (UINT)pOndemand->stat.nMaxTime;

		   			AppendTable(&chunk, "T001", (char *)&stat_tbl, (int)sizeof(STAT_TABLE));

			   		XDEBUG("\r\n");
			   		XDEBUG("------------------[ OK ]-----------------\r\n");
			   		XDEBUG("        Total Receive = %d byte(s)\r\n", nTotal);
			   		XDEBUG("     Total File Write = %d byte(s)\r\n", pOndemand->bResult ? chunk.GetSize() : 0);
			   		XDEBUG("      Metering Elapse = %d second(s)\r\n", tmEnd-tmStart);
			   		XDEBUG("\r\n");
			   		XDEBUG("       Transmit Count = %d\r\n", pOndemand->stat.nCount);
			   		XDEBUG("   Error Packet Count = %d\r\n", pOndemand->stat.nError);
			   		XDEBUG("           Total Send = %d byte(s)\r\n", pOndemand->stat.nSend);
			   		XDEBUG("        Total Receive = %d byte(s)\r\n", pOndemand->stat.nRecv);
			   		XDEBUG("  Total Response Time = %d ms\r\n", pOndemand->stat.nResponseTime);
			   		XDEBUG("Average Response Time = %d ms\r\n", nElapse);
			   		XDEBUG("    Min Response Time = %d ms\r\n", pOndemand->stat.nMinTime);
			   		XDEBUG("    Max Response Time = %d ms\r\n", pOndemand->stat.nMaxTime);
			   		XDEBUG("       Total Use Time = %d ms\r\n", pOndemand->stat.nUseTime);
			   		XDEBUG("         Connect Time = %d ms\r\n", pOndemand->stat.nConnectTime);
			   		XDEBUG("\r\n");
			   		XDEBUG("------------------[ OK ]-----------------\r\n");
			   		XDEBUG("\r\n");
					XDEBUG("MODEL      = %s\r\n", m_szModel);
				    XDEBUG("SERIAL     = %s\r\n", m_szSerial);
					XDEBUG("HW VERSION = %d.%d\r\n", hwVersion >> 4, hwVersion & 0xf);
					XDEBUG("FW VERSION = %d.%d\r\n", fwVersion >> 4, fwVersion & 0xf);
			   		XDEBUG("\r\n");

					if (pStat != NULL)
						memcpy(pStat, &pOndemand->stat, sizeof(METER_STAT));
					*nReturnSize = chunk.GetSize();
					memcpy(pszData, chunk.GetBuffer(), chunk.GetSize());
			   }
			   else
			   {
					nResult = ONDEMAND_ERROR_FAIL;
			   		XDEBUG("\r\n");
			   		XDEBUG("-----------------[ FAIL ]----------------\r\n");
			   		XDEBUG("       Total Receive = %d byte(s)\r\n", nTotal);
			   		XDEBUG("     Metering Elapse = %d second(s)\r\n", tmEnd-tmStart);
			   		XDEBUG("-----------------[ FAIL ]----------------\r\n");
			   		XDEBUG("\r\n");
			   }

			   endiClose(pOndemand->endi);

			   LockOperation();
			   DeleteOperation(pOndemand);	
			   UnlockOperation();
			   nState = STATE_DONE;
			   break;

		  case STATE_RETRY :
			   if (pOndemand->endi != NULL)
			   {
				   endiDisconnect(pOndemand->endi);
				   endiClose(pOndemand->endi);
			   }

			   LockOperation();
			   DeleteOperation(pOndemand);	
			   UnlockOperation();

			   nRetry++;
			   if (nRetry >= 3)
			   {
				   nState = STATE_DONE;
				   break;
			   }
			   nState = STATE_OPEN;
			   usleep(1000000);
			   break;
		}
	}

	return nResult;
}

void CI210Parser::OnData(ONDEMANDPARAM *pOndemand, char *pszData, BYTE nLength)
{
	int				i, nTime, stp;
	ANSI_FRAME  	*pFrame;

	pOndemand->stat.nRecv += nLength;

	GetTimeTick(&pOndemand->tmRecv);
	nTime = GetTimeInterval(&pOndemand->tmSend, &pOndemand->tmRecv);
	pOndemand->stat.nResponseTime += nTime;
	if (pOndemand->stat.nMinTime == 0)
	     pOndemand->stat.nMinTime = nTime;
	else pOndemand->stat.nMinTime = MIN(nTime, pOndemand->stat.nMinTime);

	if (pOndemand->stat.nMaxTime == 0)
		 pOndemand->stat.nMaxTime = nTime;
	else pOndemand->stat.nMaxTime = MAX(nTime, pOndemand->stat.nMaxTime);

	switch(pOndemand->nFrameState) {
	  case ANSI_FRAME_READ1 :				
		   stp = 0;
		   if (pOndemand->nLength == 0)
		   {
			   stp = -1;
			   for(i=0; i<nLength; i++)
			   {
				  if (pszData[i] == (char)ANSI_STP)
				  {
					  stp = i;
			  		  pOndemand->stat.nRecvCount++;
					  break;
				  }
			   }
		   }

		   if (stp != -1)
		   {
    		  pFrame = (ANSI_FRAME *)&pszData[stp];

		 	  memcpy(pOndemand->pszBuffer+pOndemand->nLength, pszData+stp, nLength - stp);
			  pOndemand->nLength += (nLength - stp);

			  // 8 = stp(1)+reserved(1)+ctrl(1)+seq(1)+length(2)+crc(2)
			  pOndemand->nFrameLength = BigToHostShort(pFrame->hdr.length) + 8; 	
			  if (pOndemand->nLength == pOndemand->nFrameLength)
			  {	
			 	  pOndemand->bResult = GeCheckDataFrame(pOndemand, (BYTE *)pOndemand->pszBuffer, pOndemand->nLength);
				  SetEvent(pOndemand->hEvent);
			  }
			  else
			  {
				  pOndemand->nFrameState = ANSI_FRAME_READ2;
			  }		
		   }
		   break;

	  case ANSI_FRAME_READ2 :
		   memcpy(pOndemand->pszBuffer+pOndemand->nLength, pszData, nLength);
		   pOndemand->nLength += nLength;

		   if (pOndemand->nLength == pOndemand->nFrameLength)
		   {
               pOndemand->bResult = GeCheckDataFrame(pOndemand, (BYTE *)pOndemand->pszBuffer, pOndemand->nLength);
               SetEvent(pOndemand->hEvent);
		   }
		   else
		   {
			   pOndemand->nFrameState = ANSI_FRAME_READ2;
		   }
		   break;
	}
}

//////////////////////////////////////////////////////////////////////
// GE Meter Reading
//////////////////////////////////////////////////////////////////////

void CI210Parser::SendMessage(ONDEMANDPARAM *pOndemand, EUI64 *id)
{           
    BYTE   i;
    BYTE   msg[100];
            
    switch(pOndemand->ansi.packet_cmd) {
      case ANSI_IDENT : 	// Identify
           pOndemand->ansi.packet_ctrl = 0x20;
           pOndemand->ansi.packet_length = 1;
           GeMakeFrameHeader(pOndemand, 0, msg);

		   XDEBUG("ANSI_IDENT\r\n");
           break;

      case ANSI_NEGO : 		// Negotiate
           pOndemand->ansi.packet_length = 5;
           GeMakeFrameHeader(pOndemand, 1, msg);

           for(i=0 ; i<4 ; i++) msg[8 + i] = ge_negotiate[i];

		   XDEBUG("ANSI_NEGO\r\n");
           break;

      case ANSI_LOGON :   	// Log on
           pOndemand->ansi.packet_length = 13;
           GeMakeFrameHeader(pOndemand, 1, msg);

           // user id : 2, 
           msg[8] = 0x00;
           msg[9] = 0x02;

           // user name : aimir
           for(i=0 ; i<10 ; i++) msg[10 + i] = ge_logon[i];

		   XDEBUG("ANSI_LOGON\r\n");
           break;

      case ANSI_SECURITY : 	// Security
           pOndemand->ansi.packet_length = 21;
           GeMakeFrameHeader(pOndemand, 1, msg);

           // master password : MASTER_012
           for(i=0 ; i<20 ; i++) msg[8 + i] = ge_security[i];
           //for(i=0 ; i<20 ; i++) msg[8 + i] = 0x66;

		   XDEBUG("ANSI_SECURITY\r\n");
           break;

      case ANSI_READ : 		// Read
           pOndemand->ansi.packet_length = 3;
           GeMakeFrameHeader(pOndemand, 1, msg);

           msg[8] = (BYTE)(pOndemand->ansi.packet_table_id >> 8);
           msg[9] = (BYTE)pOndemand->ansi.packet_table_id;

		   XDEBUG("ANSI_READ: Table=%d\r\n", pOndemand->ansi.packet_table_id);
           break;

      case ANSI_PREAD : 	// Partial read offset
           pOndemand->ansi.packet_length = 8;
           GeMakeFrameHeader(pOndemand, 1, msg);

           msg[8]  = (BYTE)(pOndemand->ansi.packet_table_id >> 8);
           msg[9]  = (BYTE)pOndemand->ansi.packet_table_id;
           msg[10] = (BYTE)(pOndemand->ansi.packet_pread_offset >> 16);     // offset MSB
           msg[11] = (BYTE)(pOndemand->ansi.packet_pread_offset >> 8);   	// offset 
           msg[12] = (BYTE)pOndemand->ansi.packet_pread_offset;
           msg[13] = (BYTE)(pOndemand->ansi.packet_pread_count >> 8);    	// count
           msg[14] = (BYTE)pOndemand->ansi.packet_pread_count;

		   XDEBUG("ANSI_PARTIAL: Table=%d, Offset=%d, Length=%d\r\n",
						   pOndemand->ansi.packet_table_id,
						   pOndemand->ansi.packet_pread_offset,
						   pOndemand->ansi.packet_pread_count);
           break;

      case ANSI_RR : 		// Reading continue
		   XDEBUG("ANSI_ACK\r\n");
           msg[0] = ANSI_ACK;									// 0x06
           break;

      case ANSI_WRITE : 	// Write
           pOndemand->ansi.packet_length = pOndemand->ansi.packet_write_table_count + 6;
           GeMakeFrameHeader(pOndemand, 1, msg);

           msg[8] = (BYTE)(pOndemand->ansi.packet_table_id >> 8);
           msg[9] = (BYTE)pOndemand->ansi.packet_table_id;

           msg[10] = (BYTE)(pOndemand->ansi.packet_write_table_count >> 8);
           msg[11] = (BYTE)pOndemand->ansi.packet_write_table_count;

           for(i=0 ; i<pOndemand->ansi.packet_write_table_count ; i++)
			   	msg[12 + i] = pOndemand->ansi.packet_write_table_buf[i];

           msg[pOndemand->ansi.packet_write_table_count + 12] = 
					GeCksumCalc((BYTE *)&pOndemand->ansi.packet_write_table_buf[0],
								pOndemand->ansi.packet_write_table_count);

		   XDEBUG("ANSI_WRITE: Table=%d, Length=%d\r\n",
						   pOndemand->ansi.packet_table_id,
						   pOndemand->ansi.packet_write_table_count);
           break;

      case ANSI_WAIT : 		// Wait
           break;

      case ANSI_LOGOFF : 	// Log off
      case ANSI_TERMI : 	// Terminate        
           pOndemand->ansi.packet_length = 1;
           GeMakeFrameHeader(pOndemand, 1, msg);
           break;

      default :
           break;
    }

	// Command Send
    if (pOndemand->ansi.packet_cmd == ANSI_IDENT)
	{
        GeCrcAdd((BYTE *)msg, 0, (pOndemand->ansi.packet_length + 9) - 1);
		GetTimeTick(&pOndemand->tmSend);
		SendCommand(pOndemand, id, (char *)msg, (pOndemand->ansi.packet_length + 9) - 1);
    }
    else if(pOndemand->ansi.packet_cmd == ANSI_RR)
	{
		GetTimeTick(&pOndemand->tmSend);
		SendCommand(pOndemand, id, (char *)msg, 1);
    }
    else
	{
        GeCrcAdd((BYTE *)msg, 1, pOndemand->ansi.packet_length+9);
		GetTimeTick(&pOndemand->tmSend);
		SendCommand(pOndemand, id, (char *)msg, pOndemand->ansi.packet_length + 9);
    }
	pOndemand->stat.nCount++;
	pOndemand->stat.nSendCount++;
}

void CI210Parser::GeMakeFrameHeader(ONDEMANDPARAM *pOndemand, BYTE kind, BYTE *msg)
{
	ANSI_FRAME	*pFrame;
    BYTE 		ctl_data;

    if (pOndemand->ansi.packet_ctrl == 0x20)
		 ctl_data = 0x00;
    else ctl_data = 0x20;

    if (kind == 1)
	{ 
		msg[0] = ANSI_ACK;								// <ack>		0x06
		pFrame = (ANSI_FRAME *)&msg[1];
	}
	else
	{
		pFrame = (ANSI_FRAME *)msg;
	}

    pFrame->hdr.stp 		= ANSI_STP;					// <stp> 		0xEE
    pFrame->hdr.reserved 	= 0;						// <reserved> 	0x00
    pFrame->hdr.ctrl 		= ctl_data;					// <ctrl>		0x00 | 0x20
    pFrame->hdr.seq 		= 0;						// <seq_nbr>	0x00
    pFrame->hdr.length 		= BigToHostShort(pOndemand->ansi.packet_length);		// <length>	
    pFrame->data[0] 		= pOndemand->ansi.packet_cmd;			// <data.cmd>

    pOndemand->ansi.packet_ctrl = ctl_data;
}

BYTE CI210Parser::GeCksumCalc(BYTE *buff, WORD len)
{
    WORD  	i;
    BYTE   	sum, value;

    sum = 0;
    for(i=0 ; i<len ; i++)
		sum += (BYTE)buff[i];

    value = ~sum;
    return value + 1;
}

void CI210Parser::GeCrcAdd(BYTE *buf, BYTE kind, WORD len)
{
    WORD     c_crc;

    c_crc = GeCrcCalc(buf, kind, len - (2 + kind));

    buf[len - 2] = (BYTE)(c_crc >> 8);
    buf[len - 1] = (BYTE)c_crc;
}

BYTE CI210Parser::GeCrcCheck(BYTE *buf, WORD len)
{
    BYTE   	kind;
    WORD  	c_crc;

    c_crc = (WORD)buf[len - 2] << 8;
    c_crc |= (WORD)buf[len - 1];
	kind = 0;

    if(c_crc == (WORD)(GeCrcCalc(buf, kind, len - (2 + kind)))) return TRUE;
    return FALSE;
}

WORD CI210Parser::GeCrcCalc(BYTE *buff, BYTE kind, WORD len)
{
    WORD  	c_crc, d_crc, mem, i;
    BYTE   	j;

    c_crc = 0xffff;
    d_crc = 0;

    for(i=0 ; i<len ; i++)
	{
        d_crc = buff[i + kind] ^ (c_crc & 0x0ff);

        for(j=0 ; j<8; j++)
        {
            mem = d_crc & 0x0001;
            d_crc /= 2;

            if(mem) d_crc ^= 0x8408;
        }

        c_crc = (c_crc >> 8) ^ d_crc;
    }
    c_crc = (~c_crc << 8) | (~c_crc >> 8 & 0xff);
    return(c_crc);
}

BYTE CI210Parser::GeCheckDataFrame(ONDEMANDPARAM *pOndemand, BYTE *pRcvFrame, WORD nLength)
{
	ANSI_FRAME	*pFrame;
	
	pFrame = (ANSI_FRAME *)pRcvFrame;
	if ((BigToHostShort(pFrame->hdr.length) + 8) != nLength)
	{
		XDEBUG("P_STAT_ERR_LENGTH : Frame (%d) bytes & Read (%d) bytes\r\n",
				BigToHostShort(pFrame->hdr.length) + 8, nLength);
		pOndemand->stat.nError++;
        return FALSE;
	}

	if (pFrame->data[0] != ANSI_NO_ERROR)
	{
		XDEBUG("P_STAT_ERR_CODE : Error code (0x%x)\r\n",
				pFrame->data[0]);
		pOndemand->stat.nError++;
        return FALSE;
	}

    // check for crc
    if (GeCrcCheck(pRcvFrame, nLength) == FALSE)
	{
		XDEBUG("P_STAT_ERR_CRC\r\n");
		pOndemand->stat.nError++;
        return FALSE;
    }

    pOndemand->ansi.packet_retry = 0;
//	XDEBUG("P_STAT_SUCCESS\r\n");
    return TRUE;
}

void CI210Parser::SetNextTable(ONDEMANDPARAM *pOndemand, int nTable, BOOL bClear)
{
	pOndemand->ansi.packet_pread_offset	= 0;
	pOndemand->ansi.packet_pread_count 	= ANSI_PREAD_MAX_COUNT;
	pOndemand->ansi.packet_table_id 	= nTable;
	pOndemand->config.meter_rr_count 	= 0;

	if (bClear) pOndemand->config.table_length = 0;
}

WORD CI210Parser::PreadTableSum(ONDEMANDPARAM *pOndemand, char *pszFrame, BYTE nIndex, WORD nLength)
{
	WORD	i;
	WORD	nReadLength;

	nReadLength = nLength - 12;
	for(i=0 ; i<nReadLength ; i++)
		pOndemand->config.table_data[(nIndex * ANSI_PREAD_MAX_COUNT) + i] = pszFrame[9 + i];

	return i;
}

void CI210Parser::WriteMessage(ONDEMANDPARAM *pOndemand, int nOption)
{
	switch(pOndemand->ansi.packet_table_id) {
      case ANSI_TABLE_ST_07 : 	// procedure initiate table
		   switch(nOption) {
			 case READ_OPTION_FULL :
			 case READ_OPTION_CURRENT :
			 case READ_OPTION_PREVIOUS :
			 case READ_OPTION_EVENTLOG :
			 case READ_IDENT_ONLY :
               	  pOndemand->ansi.packet_cmd = ANSI_PREAD;
				  SetNextTable(pOndemand, ANSI_TABLE_ST_01, TRUE);
				  break;
			 case READ_OPTION_RELAY :
               	  pOndemand->ansi.packet_cmd = ANSI_PREAD;
				  SetNextTable(pOndemand, ANSI_TABLE_ST_130, TRUE);
				  break;
			 case WRITE_OPTION_RELAYON :
			 case WRITE_OPTION_RELAYOFF :
			 case WRITE_OPTION_ACTON :
			 case WRITE_OPTION_ACTOFF :
           		  pOndemand->ansi.packet_cmd = ANSI_PREAD;
				  SetNextTable(pOndemand, ANSI_TABLE_ST_130, TRUE);
				  break;
			 case WRITE_OPTION_TIMESYNC :
	           	  pOndemand->ansi.packet_cmd = ANSI_PREAD;
				  SetNextTable(pOndemand, ANSI_TABLE_ST_55, TRUE);
				  break;
		   }
           break;

      case ANSI_TABLE_ST_131 :
		   switch(nOption) {
			 case WRITE_OPTION_ACTON :
			 case WRITE_OPTION_ACTOFF :						
			 case WRITE_OPTION_RELAYON :
			 case WRITE_OPTION_RELAYOFF :
               	  pOndemand->ansi.packet_cmd = ANSI_PREAD;
				  SetNextTable(pOndemand, ANSI_TABLE_ST_130, TRUE);
				  break;
		   }
		   break;
	}
}

void CI210Parser::RelayStatusTable(ONDEMANDPARAM *pOndemand, CChunk *pChunk, char *pszFrame, WORD nLength, int nOption)
{
	switch(nOption) {
	  case READ_OPTION_FULL :
	  case READ_OPTION_CURRENT :
	  case READ_OPTION_PREVIOUS :
	  case READ_OPTION_RELAY :
		   AppendTable(pChunk, "S130", &pszFrame[9], nLength-12);	 
		   if (nOption == READ_OPTION_RELAY)
				pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_132;
		   else pOndemand->ansi.packet_table_id = ANSI_TABLE_MT_67;
		   break;

	  case WRITE_OPTION_RELAYON :
		   if (pOndemand->config.meter_relay_switch == 0)
		   {	
			   // RELAY_OFF
			   if (pOndemand->config.meter_relay_activation == 1)
			   {
                   pOndemand->ansi.packet_cmd = ANSI_WRITE;
				   pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_131;
				   pOndemand->ansi.packet_write_table_buf[0] = RELAY_SWITCH_ON;
				
  				   // Relay Switch On
				   pOndemand->ansi.packet_write_table_count = 1;
			   }
			   else
			   {
		   	   	   XDEBUG("WRITE: Relay on - FAIL!! (Relay activation is off)\r\n");
		   		   AppendTable(pChunk, "S130", &pszFrame[9], nLength-12);	 
			   	   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
			   }
		   }
		   else
		   {
		   	   XDEBUG("WRITE: Already relay on\r\n");
		  	   AppendTable(pChunk, "S130", &pszFrame[9], nLength-12);	 
			   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   }
		   pOndemand->ansi.packet_write_ok = 1;
		   break;

	  case WRITE_OPTION_RELAYOFF :
		   if (pOndemand->config.meter_relay_switch == 1)
		   {						
			   // RELAY_ON
               pOndemand->ansi.packet_cmd = ANSI_WRITE;
			   pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_131;
			   pOndemand->ansi.packet_write_table_buf[0] = RELAY_SWITCH_OFF;      // Relay Switch Off
			   pOndemand->ansi.packet_write_table_count = 1;
		   }
		   else
		   {
		   	   XDEBUG("WRITE: Already relay off\r\n");
		  	   AppendTable(pChunk, "S130", &pszFrame[9], nLength-12);	 
			   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   }
		   pOndemand->ansi.packet_write_ok = 1;
		   break;
							
	  case WRITE_OPTION_ACTON :		// RELAY_OFF
		   if (pOndemand->config.meter_relay_switch == 0)
		   {
			   if (pOndemand->config.meter_relay_activation == 0)
			   {
          	   	   pOndemand->ansi.packet_cmd = ANSI_WRITE;
			   	   pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_131;

			   	   // Relay Activation On
               	   pOndemand->ansi.packet_write_table_buf[0] = RELAY_ACTIVATION_ON;
               	   pOndemand->ansi.packet_write_table_count = 1;
			   }
			   else
			   {
		   	   	   XDEBUG("WRITE: Already relay activation on\r\n");
		 	 	   AppendTable(pChunk, "S130", &pszFrame[9], nLength-12);	 
			   	   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
			   }
		   }
		   else
		   {
		   	   XDEBUG("WRITE: Relay activation on - FAIL!! (Relay is on)\r\n");
		  	   AppendTable(pChunk, "S130", &pszFrame[9], nLength-12);	 
			   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   }
		   pOndemand->ansi.packet_write_ok = 1;
		   break;
							
	  case WRITE_OPTION_ACTOFF : 		// RELAY_OFF
		   if (pOndemand->config.meter_relay_switch == 0)
		   {
			   if (pOndemand->config.meter_relay_activation == 1)
			   {
               	   pOndemand->ansi.packet_cmd = ANSI_WRITE;
			   	   pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_131;
			   
			   	   // Relay Activation Off
               	   pOndemand->ansi.packet_write_table_buf[0] = RELAY_ACTIVATION_OFF;
               	   pOndemand->ansi.packet_write_table_count = 1;
			   }
			   else
			   {
		   	       XDEBUG("WRITE: Already relay activation off\r\n");
		 	 	   AppendTable(pChunk, "S130", &pszFrame[9], nLength-12);	 
			       pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
			   }
		   }
		   else
		   {
		   	   XDEBUG("WRITE: Relay activation off - FAIL!! (Relay is on)\r\n");
		  	   AppendTable(pChunk, "S130", &pszFrame[9], nLength-12);	 
			   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   }
		   pOndemand->ansi.packet_write_ok = 1;
		   break;
	}	
}

void CI210Parser::PartialReadMessage(ONDEMANDPARAM *pOndemand, CChunk *pChunk, char *pszFrame, WORD nLength, int nOption)
{
	ANSI_CONFIG		*pProtocol;
	METER_CONFIG	*pConfig;
	ANSI_TABLE1		*pTable1;
	ANSI_TABLE5		*pTable5;
	ANSI_TABLE8		*pTable8;
	ANSI_TABLE61	*pTable61;
	ANSI_TABLE63	*pTable63;
	char			temp[33];
	char			model[33];
	WORD			nPartialLength;
	WORD			hw, fw;
	int				len;
	UINT			n;

	pConfig   = &pOndemand->config;
	pProtocol = &pOndemand->ansi;

	switch(pProtocol->packet_table_id) {
	  case ANSI_TABLE_ST_01 : 	// Manufacturer Identification Table
	  	   pTable1 = (ANSI_TABLE1 *)&pszFrame[9];

		   memcpy(temp, pTable1->MANUFACTURER, 4); temp[4] = 0;
		   XDEBUG("    MANUFACTURER       = '%s'\r\n", temp);
		   memcpy(temp, pTable1->ED_MODEL, 8); temp[8] = 0;
		   memcpy(model, temp, 9);
		   
		   if (!strncmp((char *)&pTable1->ED_MODEL, "I210", 4))
			   pConfig->meter_kind = GE_IEC_I210;
		   else if(!strncmp((char *)&pTable1->ED_MODEL, "kV2", 3))
			   pConfig->meter_kind = GE_IEC_KV2C;
		   else
		   {
			   printf("----- Not support [%s] meter type !!! -----\r\n", temp);
			   pConfig->meter_kind = GE_IEC_I210;
			   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
			   return;			
		   }
		   
		   XDEBUG("TABLE 1: General Manufacturer Identification Table\r\n");
		   XDEBUG("    ED_MODEL           = '%s'\r\n", temp);
		   XDEBUG("    HW_VERSION_NUMBER  = %d\r\n", pTable1->HW_VERSION_NUMBER);
		   XDEBUG("    HW_REVISION_NUMBER = %d\r\n", pTable1->HW_REVISION_NUMBER);
		   XDEBUG("    FW_VERSION_NUMBER  = %d\r\n", pTable1->FW_VERSION_NUMBER);
		   XDEBUG("    FW_REVISION_NUMBER = %d\r\n", pTable1->FW_REVISION_NUMBER);
		   memcpy(temp, pTable1->MFG_SERIAL_NUMBER, 16); temp[16] = 0;
		   XDEBUG("    MFG_SERIAL_NUMBER  = '%s'\r\n", temp);

		   hw = (pTable1->HW_VERSION_NUMBER << 8) | pTable1->HW_REVISION_NUMBER;
		   fw = (pTable1->FW_VERSION_NUMBER << 8) | pTable1->FW_REVISION_NUMBER;

//		   m_pGroupMember->Setup(&pOndemand->id, model, temp, hw, fw);
		   Setup(&pOndemand->id, model, temp, hw, fw);
		   
	  	   memset(pConfig->meter_serial_num, 0, sizeof(pConfig->meter_serial_num));
		   memcpy(pConfig->meter_serial_num, pTable1->MFG_SERIAL_NUMBER, 16);
		   AppendTable(pChunk, "S001", &pszFrame[9], nLength-12);

		   if (nOption == READ_IDENT_ONLY)
			   	pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   else if (nOption == READ_OPTION_EVENTLOG)
			    SetNextTable(pOndemand, ANSI_TABLE_ST_71, TRUE);
		   else SetNextTable(pOndemand, ANSI_TABLE_ST_03, FALSE);
		   break;

	  case ANSI_TABLE_ST_03 : 	// End Device Mode & Status Table
		   AppendTable(pChunk, "S003", &pszFrame[9], nLength-12);	 
		   SetNextTable(pOndemand, ANSI_TABLE_ST_05, FALSE);
		   break;

	  case ANSI_TABLE_ST_05 : 	// Device Identification Table
	 	   AppendTable(pChunk, "S005", &pszFrame[9], nLength-12);	 
	  	   pTable5 = (ANSI_TABLE5 *)&pszFrame[9];

		   XDEBUG("TABLE 5: Device Identification Table\r\n");
		   memcpy(temp, pTable5->METER_ID, 20); temp[20] = 0;
		   XDEBUG("    METER_ID = %s\r\n", temp);

	  	   memset(pConfig->meter_id, 0, sizeof(pConfig->meter_id));
		   memcpy(pConfig->meter_id, pTable5->METER_ID, 20);
		   SetNextTable(pOndemand, ANSI_TABLE_ST_21, FALSE);
		   break;

	  case ANSI_TABLE_ST_08 : 	// Procedure Response Table
		   pTable8 = (ANSI_TABLE8 *)&pszFrame[9];
#ifdef DEBUG
		   XDEBUG("TABLE 8: Procedure Response Table\r\n");
		   XDEBUG("    TBL_PROC    = %d\r\n", pTable8->TBL_PROC);
		   XDEBUG("    SEQ_NBR     = %d\r\n", pTable8->SEQ_NBR);
		   XDEBUG("    RESULT_CODE = %d\r\n", pTable8->RESULT_CODE);
		   XDUMP((char *)pTable8->RESP_DATA_RCD, 8, FALSE);
#endif
		   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   break;

	  case ANSI_TABLE_ST_21 : 	// Actual Register Table
		   AppendTable(pChunk, "S021", &pszFrame[9], nLength-12);	 
		   SetNextTable(pOndemand, ANSI_TABLE_ST_22, FALSE);
		   break;

	  case ANSI_TABLE_ST_22 : 	// Data Selection Table 
		   AppendTable(pChunk, "S022", &pszFrame[9], nLength-12);	 
		   if (nOption == READ_OPTION_PREVIOUS)
			    SetNextTable(pOndemand, ANSI_TABLE_ST_25, TRUE);
		   else SetNextTable(pOndemand, ANSI_TABLE_ST_23, TRUE);
		   break;

	  case ANSI_TABLE_ST_23 :	// Present Data Table
		   len = PreadTableSum(pOndemand, pszFrame, pConfig->meter_rr_count - 1, nLength);
		   pConfig->table_length += len;
		   nPartialLength = (WORD)((pszFrame[7] << 8) | pszFrame[8]);
									
		   pProtocol->packet_pread_offset = (UINT)pConfig->meter_rr_count * ANSI_PREAD_MAX_COUNT;
		   pProtocol->packet_pread_count = ANSI_PREAD_MAX_COUNT;

		   if (nPartialLength < ANSI_PREAD_MAX_COUNT)
		   {
		       AppendTable(pChunk, "S023", pConfig->table_data, pConfig->table_length);	 
           	  // if (nOption == READ_OPTION_FULL)
              //      SetNextTable(pOndemand, ANSI_TABLE_ST_25, TRUE);
			  /* else*/ SetNextTable(pOndemand, ANSI_TABLE_ST_55, TRUE);
		   }
		   break;

	  case ANSI_TABLE_ST_25 : 	// Prev Demand Reset Data Table
		   len = PreadTableSum(pOndemand, pszFrame, pConfig->meter_rr_count - 1, nLength);
		   pConfig->table_length += len;
		   nPartialLength = (WORD)((pszFrame[7] << 8) | pszFrame[8]);
					
		   pProtocol->packet_pread_offset = (UINT)pConfig->meter_rr_count * ANSI_PREAD_MAX_COUNT;
		   pProtocol->packet_pread_count = ANSI_PREAD_MAX_COUNT;
					
		   if (nPartialLength < ANSI_PREAD_MAX_COUNT)
		   {
		       AppendTable(pChunk, "S025", pConfig->table_data, pConfig->table_length);	 
               SetNextTable(pOndemand, ANSI_TABLE_ST_55, FALSE);
		   }
		   break;

	  case ANSI_TABLE_ST_55 : 	// Clock State Table
#ifdef DEBUG
		   XDEBUG("TABLE 55: Clock State Table\r\n");
		   XDEBUG("    CLOCK CALENDAR     = %02d/%02d/%02d %02d:%02d:%02d\r\n", 
						pszFrame[9] & 0xff,
						pszFrame[10] & 0xff,
						pszFrame[11] & 0xff,
						pszFrame[12] & 0xff,
						pszFrame[13] & 0xff,
						pszFrame[14] & 0xff);
		   XDEBUG("    DAY OF WEEK        = %d\r\n", pszFrame[15] & 7);
		   XDEBUG("    DST_FLAG           = %d\r\n", (pszFrame[15] & 0x08) >> 3);
		   XDEBUG("    GMT_FLAG           = %d\r\n", (pszFrame[15] & 0x10) >> 4);
		   XDEBUG("    TM_ZN APPLIED FLAG = %d\r\n", (pszFrame[15] & 0x20) >> 5);
		   XDEBUG("    DST APPLIED FLAG   = %d\r\n", (pszFrame[15] & 0x40) >> 6);
#endif
		   AppendTable(pChunk, "S055", &pszFrame[9], nLength-12);	 
		   if (nOption == WRITE_OPTION_TIMESYNC)
		   {
			   if (pOndemand->ansi.packet_write_ok == 0)
			   {
				   time_t	now;
				   struct tm when;

				   time(&now);
				   when = *localtime(&now);
			
	               pOndemand->ansi.packet_cmd = ANSI_WRITE;
	               pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_07;
	               pOndemand->ansi.packet_write_table_buf[0] = 0x0A;
	               pOndemand->ansi.packet_write_table_buf[1] = 0;
	               pOndemand->ansi.packet_write_table_buf[2] = 17;
		           pOndemand->ansi.packet_write_table_buf[3] = 0x7;
	               pOndemand->ansi.packet_write_table_buf[4] = (when.tm_year+1900) % 100;
	               pOndemand->ansi.packet_write_table_buf[5] = when.tm_mon + 1;
 	               pOndemand->ansi.packet_write_table_buf[6] = when.tm_mday;
	               pOndemand->ansi.packet_write_table_buf[7] = when.tm_hour;
	               pOndemand->ansi.packet_write_table_buf[8] = when.tm_min;
	               pOndemand->ansi.packet_write_table_buf[9] = when.tm_sec;
	               pOndemand->ansi.packet_write_table_buf[10] = when.tm_wday;
	               pOndemand->ansi.packet_write_table_count = 11;
	         	   pOndemand->ansi.packet_write_ok = 1;
			   }
			   else
			   {
			       pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
			   }
		   	   break;
		   }
		   SetNextTable(pOndemand, ANSI_TABLE_ST_61, FALSE);
// LP제외하고 싶으면
//		   SetNextTable(pOndemand, ANSI_TABLE_ST_130, FALSE);
		   break;

	  case ANSI_TABLE_ST_61 : 	// Actual Load Profile Table
		   AppendTable(pChunk, "S061", &pszFrame[9], nLength-12);
	  	   pTable61 = (ANSI_TABLE61 *)&pszFrame[9];
		   pConfig->meter_lp_len = ((((pTable61->NBR_CHNS_SET1/2)+1)
									 + (pTable61->NBR_CHNS_SET1*2))
						   		     * (pTable61->NBR_BLK_INTS_SET1) + 5
									 + (pTable61->NBR_CHNS_SET1*6));
#ifdef DEBUG
		   XDEBUG("TABLE 61: Actual Load Profile Table\r\n");
		   XDEBUG("    LP_MEMORY_LEN     = %d\r\n", pTable61->LP_MEMORY_LEN);
		   XDEBUG("    LP_FLAGS          = 0x%04X\r\n", pTable61->LP_FLAGS);
		   XDEBUG("    LP_FORMATS        = 0x%02X\r\n", pTable61->LP_FORMATS);
		   XDEBUG("    NBR_BLKS_SET1     = %d\r\n", pTable61->NBR_BLKS_SET1);
		   XDEBUG("    NBR_BLK_INTS_SET1 = %d\r\n", pTable61->NBR_BLK_INTS_SET1);
		   XDEBUG("    NBR_CHNS_SET1     = %d\r\n", pTable61->NBR_CHNS_SET1);
		   XDEBUG("    MAX_INT_TIME_SET1 = %d\r\n", pTable61->MAX_INT_TIME_SET1);
		   XDEBUG("    LP LENGTH         = %d\r\n", pConfig->meter_lp_len);
#endif		  
		   SetNextTable(pOndemand, ANSI_TABLE_ST_62, FALSE);
		   break;

	  case ANSI_TABLE_ST_62 : 	// Load Profile Control Table
		   AppendTable(pChunk, "S062", &pszFrame[9], nLength-12);	 
		   SetNextTable(pOndemand, ANSI_TABLE_ST_63, FALSE);
		   break;

	  case ANSI_TABLE_ST_63 : 	// Load Profile Status Table
		   AppendTable(pChunk, "S063", &pszFrame[9], nLength-12);	 
				
	  	   pTable63 = (ANSI_TABLE63 *)&pszFrame[9];
		   pConfig->meter_lp_valid_blocks = pTable63->NBR_VALID_BLOCKS;
		   pConfig->meter_lp_valid_int = pTable63->NBR_VALID_INT;
#ifdef DEBUG
		   XDEBUG("TABLE 63: Load Profile Status Table\r\n");
		   XDEBUG("    LP_SET_STATUS_FLAGS = 0x%02X\r\n", pTable63->LP_SET_STATUS_FLAGS);
		   XDEBUG("    NBR_VALID_BLOCKS    = %d\r\n", pTable63->NBR_VALID_BLOCKS);
		   XDEBUG("    LAST_BLOCK_ELEMENT  = %d\r\n", pTable63->LAST_BLOCK_ELEMENT);
		   XDEBUG("    LAST_BLOCK_SEQ_NBR  = %d\r\n", pTable63->LAST_BLOCK_SEQ_NBR);
		   XDEBUG("    NBR_UNREAD_BLOCKS   = %d\r\n", pTable63->NBR_UNREAD_BLOCKS);
		   XDEBUG("    NBR_VALID_INT       = %d\r\n", pTable63->NBR_VALID_INT);
#endif		   
		   pProtocol->packet_pread_offset = 0x0000;
           pProtocol->packet_pread_count = ANSI_PREAD_MAX_COUNT;
	  	   pConfig->meter_rr_count = 0;
		   pConfig->table_length = 0;

		   if (pOndemand->config.meter_lp_valid_blocks == 1)
		   {
			   if (nOption == READ_OPTION_PREVIOUS)
			   {
				   //pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_130;
				   if(pConfig->meter_kind == GE_IEC_I210)
				   		pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_130;
				   else if(pConfig->meter_kind == GE_IEC_KV2C)
					   	pOndemand->ansi.packet_table_id = ANSI_TABLE_MT_67;
			   }
			   else
			   {
               	   pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_64;
				   pOndemand->config.meter_rr_max = pConfig->meter_lp_len / ANSI_PREAD_MAX_COUNT;
			   }
		   }
		   else if (pOndemand->config.meter_lp_valid_blocks > 1)
		   {
			   if (nOption == READ_OPTION_CURRENT)
			   {
				   n = (UINT)((UINT)pOndemand->config.meter_lp_valid_blocks - 1) * (UINT)pConfig->meter_lp_len;
				   pOndemand->ansi.packet_pread_offset = n;
				   pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_64;
                   pOndemand->config.meter_rr_max = pConfig->meter_lp_len / ANSI_PREAD_MAX_COUNT;
			   }
			   else if(nOption == READ_OPTION_PREVIOUS)
			   {
				   n = (UINT)((UINT)pOndemand->config.meter_lp_valid_blocks - 2) * (UINT)pConfig->meter_lp_len;
				   pOndemand->ansi.packet_pread_offset = n;
				   pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_64;
                   pOndemand->config.meter_rr_max = pConfig->meter_lp_len / ANSI_PREAD_MAX_COUNT;
			   }
			   else
			   {
				   // Full (2 Block)
				   n = (pOndemand->config.meter_lp_valid_blocks - 2) * pConfig->meter_lp_len;
				   pOndemand->ansi.packet_pread_offset = n;
               	   pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_64;
				   pOndemand->config.meter_rr_max = (pConfig->meter_lp_len * 2) / ANSI_PREAD_MAX_COUNT;
			   }
		   }
		   else
		   {
			   // load profile empty
			   if (pConfig->meter_kind == GE_IEC_I210)
					pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_130;
			   else if(pConfig->meter_kind == GE_IEC_KV2C)
					pOndemand->ansi.packet_table_id = ANSI_TABLE_MT_67;
		   }
		   break;

	  case ANSI_TABLE_ST_64 : 	// IEC-I210 : 1 block = 593bytes (2 Days), Load Profile Data Set 1 Table
		   						// IEC-KV2C : 1 block = 1085bytes (2 Days), Load Profile Data Set 1 Table
		   len = PreadTableSum(pOndemand, pszFrame, pOndemand->config.meter_rr_count - 1, nLength);
		   pOndemand->config.table_length += len;
		   if (pOndemand->config.meter_rr_count < pOndemand->config.meter_rr_max)
		   {
			   pOndemand->ansi.packet_pread_offset += ANSI_PREAD_MAX_COUNT;
           	   pOndemand->ansi.packet_pread_count = ANSI_PREAD_MAX_COUNT;
               pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_64;
		   }
		   else if(pOndemand->config.meter_rr_count == pOndemand->config.meter_rr_max)
		   {
			   pOndemand->ansi.packet_pread_offset += ANSI_PREAD_MAX_COUNT;
               pOndemand->ansi.packet_table_id = ANSI_TABLE_ST_64;

			   if (nOption == READ_OPTION_FULL)
			   {
				   if (pOndemand->config.meter_lp_valid_blocks == 1)
				        pOndemand->ansi.packet_pread_count = pConfig->meter_lp_len  % ANSI_PREAD_MAX_COUNT;
				   else pOndemand->ansi.packet_pread_count = (pConfig->meter_lp_len * 2) % ANSI_PREAD_MAX_COUNT;
			   }
			   else pOndemand->ansi.packet_pread_count = pConfig->meter_lp_len % ANSI_PREAD_MAX_COUNT;
		   }
		   else
		   {
			   AppendTable(pChunk, "S064", pOndemand->config.table_data,
										   pOndemand->config.table_length);	 
			
				if(pConfig->meter_kind == GE_IEC_I210)
					SetNextTable(pOndemand, ANSI_TABLE_ST_130, TRUE);
				else if(pConfig->meter_kind == GE_IEC_KV2C)
					SetNextTable(pOndemand, ANSI_TABLE_MT_67, TRUE);
		   }
           break;

	  case ANSI_TABLE_ST_71 : 	// Actual Log Table
		   AppendTable(pChunk, "S071", &pszFrame[9], nLength-12);	 
           SetNextTable(pOndemand, ANSI_TABLE_ST_72, FALSE);
		   break;

	  case ANSI_TABLE_ST_72 : 	// Events Identification Table
		   AppendTable(pChunk, "S072", &pszFrame[9], nLength-12);	 
           SetNextTable(pOndemand, ANSI_TABLE_ST_76, TRUE);
		   break;

	  case ANSI_TABLE_ST_76 : 	// Event Log Data Table
           len = PreadTableSum(pOndemand, pszFrame, pOndemand->config.meter_rr_count - 1, nLength);
           pOndemand->config.table_length += len;

		   if (pOndemand->config.meter_rr_count == 1)
		   {
			   // LAST_VALID_ENTRIES : pszFrame[10][11]
			   pOndemand->config.meter_event_length = (WORD)((pszFrame[11] << 8) | pszFrame[10]) + 11;
			   pOndemand->config.meter_rr_max = (BYTE)pOndemand->config.meter_event_length / ANSI_PREAD_MAX_COUNT;	
		   }
					
           if (pOndemand->config.meter_rr_count < pOndemand->config.meter_rr_max)
		   {
               pOndemand->ansi.packet_pread_offset = (UINT)pOndemand->config.meter_rr_count * ANSI_PREAD_MAX_COUNT;
               pOndemand->ansi.packet_pread_count = ANSI_PREAD_MAX_COUNT;
           }
           else if (pOndemand->config.meter_rr_count == pOndemand->config.meter_rr_max)
		   {
               pOndemand->ansi.packet_pread_offset = (UINT)pOndemand->config.meter_rr_count * ANSI_PREAD_MAX_COUNT;
               pOndemand->ansi.packet_pread_count = pOndemand->config.meter_event_length % ANSI_PREAD_MAX_COUNT;
           }
           else
		   {
			   AppendTable(pChunk, "S076", pOndemand->config.table_data, pOndemand->config.table_length);	 
			   
			   if(pConfig->meter_kind == GE_IEC_I210)
			   		pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
			   else if(pConfig->meter_kind == GE_IEC_KV2C)
           			SetNextTable(pOndemand, ANSI_TABLE_MT_112, TRUE);				   
			   else 
			   		pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
           }
		   break;

      case ANSI_TABLE_ST_130 : 	// Relay Status Table
		   pOndemand->config.meter_relay_switch = pszFrame[9];
		   pOndemand->config.meter_relay_activation = pszFrame[10];
#ifdef DEBUG
		   XDEBUG("TABLE 130: Relay status table\r\n");
		   XDEBUG("    relay_run_status    = %d\r\n", pszFrame[9]);
		   XDEBUG("    active_ready_status = %d\r\n", pszFrame[10]);
#endif
		   if (pOndemand->ansi.packet_write_ok == 0)
		   {
		   	   pOndemand->ansi.packet_pread_offset = 0x0000;
		   	   pOndemand->ansi.packet_pread_count = ANSI_PREAD_MAX_COUNT;
	 	   	   pOndemand->config.meter_rr_count = 0;
		   	   pOndemand->config.table_length = 0;
				
	  	   	   RelayStatusTable(pOndemand, pChunk, pszFrame, nLength, nOption);
		   }
		   else
		   {
		   	   AppendTable(pChunk, "S130", &pszFrame[9], nLength-12);	 
			   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   }
           break;

	  case ANSI_TABLE_MT_67 : 	// Meter Program Constants - 2
           len = PreadTableSum(pOndemand, pszFrame, pOndemand->config.meter_rr_count - 1, nLength);
           pOndemand->config.table_length += len;
	  	   nPartialLength = (WORD)((pszFrame[7] << 8) | pszFrame[8]);

           pOndemand->ansi.packet_pread_offset = (UINT)pOndemand->config.meter_rr_count * ANSI_PREAD_MAX_COUNT;
           pOndemand->ansi.packet_pread_count = ANSI_PREAD_MAX_COUNT;

		   if (nPartialLength < ANSI_PREAD_MAX_COUNT)
		   {
			   AppendTable(pChunk, "M067", pOndemand->config.table_data,
										   pOndemand->config.table_length);	 

               SetNextTable(pOndemand, ANSI_TABLE_MT_70, TRUE);
           }
		   break;

	  case ANSI_TABLE_MT_70 : 	// Display Configuration
		   AppendTable(pChunk, "M070", &pszFrame[9], nLength-12);	 
           SetNextTable(pOndemand, ANSI_TABLE_MT_75, TRUE);
		   break;

	  case ANSI_TABLE_MT_75 : 	// Scale Factors
		   AppendTable(pChunk, "M075", &pszFrame[9], nLength-12);	 
           SetNextTable(pOndemand, ANSI_TABLE_MT_78, TRUE);
		   break;
	
	  case ANSI_TABLE_MT_78 : 	// Security Log Table
           len = PreadTableSum(pOndemand, pszFrame, pOndemand->config.meter_rr_count - 1, nLength);
           pOndemand->config.table_length += len;
		   nPartialLength = (WORD)((pszFrame[7] << 8) | pszFrame[8]);

           pOndemand->ansi.packet_pread_offset = (UINT)pOndemand->config.meter_rr_count * ANSI_PREAD_MAX_COUNT;
           pOndemand->ansi.packet_pread_count = ANSI_PREAD_MAX_COUNT;

		   if (nPartialLength < ANSI_PREAD_MAX_COUNT)
		   {
			   AppendTable(pChunk, "M078", pOndemand->config.table_data,
											pOndemand->config.table_length);	 

			   //SetNextTable(pOndemand, ANSI_TABLE_MT_113, TRUE);
				if (pConfig->meter_kind == GE_IEC_I210)
					 SetNextTable(pOndemand, ANSI_TABLE_MT_113, TRUE);
				else if(pConfig->meter_kind == GE_IEC_KV2C)
					 SetNextTable(pOndemand, ANSI_TABLE_MT_110, TRUE);
           }
		   break;

	  case ANSI_TABLE_MT_113 : 	// Power Quality Data Table
		   AppendTable(pChunk, "M113", &pszFrame[9], nLength-12);	 
		   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   break;

	  case ANSI_TABLE_ST_132 : 	// Relay Switch Status Table
           len = PreadTableSum(pOndemand, pszFrame, pOndemand->config.meter_rr_count - 1, nLength);
           pOndemand->config.table_length += len;
		   nPartialLength = (WORD)((pszFrame[7] << 8) | pszFrame[8]);

           pOndemand->ansi.packet_pread_offset = (UINT)pOndemand->config.meter_rr_count * ANSI_PREAD_MAX_COUNT;
           pOndemand->ansi.packet_pread_count = ANSI_PREAD_MAX_COUNT;

		   if (nPartialLength < ANSI_PREAD_MAX_COUNT)
		   {
			   AppendTable(pChunk, "S132", pOndemand->config.table_data,
										   pOndemand->config.table_length);	 

			   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
           }
		   break;
		   
	  case ANSI_TABLE_MT_110 :  // Present Register Data Table
           len = PreadTableSum(pOndemand, pszFrame, pOndemand->config.meter_rr_count - 1, nLength);
           pOndemand->config.table_length += len;
		   nPartialLength = (WORD)((pszFrame[7] << 8) | pszFrame[8]);

           pOndemand->ansi.packet_pread_offset = (UINT)pOndemand->config.meter_rr_count * ANSI_PREAD_MAX_COUNT;
           pOndemand->ansi.packet_pread_count = ANSI_PREAD_MAX_COUNT;

		   if (nPartialLength < ANSI_PREAD_MAX_COUNT)
		   {
			   AppendTable(pChunk, "M110", pOndemand->config.table_data,
										   pOndemand->config.table_length);	 

		   	   SetNextTable(pOndemand, ANSI_TABLE_MT_72, TRUE);
           }
		   break;

	  case ANSI_TABLE_MT_72 :  // Line-Side Diagnostics / Power Quality Data Table
		   AppendTable(pChunk, "M072", &pszFrame[9], nLength-12);	 
		   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   break;

	  case ANSI_TABLE_MT_112 :
           len = PreadTableSum(pOndemand, pszFrame, pOndemand->config.meter_rr_count - 1, nLength);
           pOndemand->config.table_length += len;

		   if (pOndemand->config.meter_rr_count == 1)
		   {
			   // LAST_VALID_ENTRIES : pszFrame[10][11]
			   pOndemand->config.meter_event_length = (WORD)((pszFrame[11] << 8) | pszFrame[10]) + 11;			   
			   pOndemand->config.meter_rr_max = (BYTE)pOndemand->config.meter_event_length / ANSI_PREAD_MAX_COUNT;	
#ifdef DEBUG
		   	   XDEBUG("TABLE M112: Voltage Event Monitor Log Table\r\n");
		   	   XDEBUG("    NBR_VALID_ENTRIES   = %d\r\n", (WORD)((pszFrame[11] << 8) | pszFrame[10]));
		   	   XDEBUG("    LAST_ENTRY_ELEMENT  = %d\r\n", (WORD)((pszFrame[13] << 8) | pszFrame[12]));
		   	   XDEBUG("    LAST_ENTRY_SEQ_NBR  = %d\r\n", (DWORD)((pszFrame[17] << 24) | (pszFrame[16] << 16) | \
					   											 (pszFrame[15] << 8) | pszFrame[14]));
		   	   XDEBUG("    NBR_UNREAD_ENTRIES  = %d\r\n", (WORD)((pszFrame[19] << 8) | pszFrame[18]));
#endif
		   }
					
           if (pOndemand->config.meter_rr_count < pOndemand->config.meter_rr_max)
		   {
               pOndemand->ansi.packet_pread_offset = (UINT)pOndemand->config.meter_rr_count * ANSI_PREAD_MAX_COUNT;
               pOndemand->ansi.packet_pread_count = ANSI_PREAD_MAX_COUNT;
           }
           else if (pOndemand->config.meter_rr_count == pOndemand->config.meter_rr_max)
		   {
               pOndemand->ansi.packet_pread_offset = (UINT)pOndemand->config.meter_rr_count * ANSI_PREAD_MAX_COUNT;
               pOndemand->ansi.packet_pread_count = pOndemand->config.meter_event_length % ANSI_PREAD_MAX_COUNT;
           }
           else
		   {
			   AppendTable(pChunk, "M112", pOndemand->config.table_data, pOndemand->config.table_length);	 			   
			   pOndemand->ansi.packet_cmd = ANSI_LOGOFF;
		   }
		   break;
	}
}

const char *CI210Parser::GetPacketName(int nType)
{
	switch(nType) {
	  case ANSI_IDENT :		return "IDENTIFY";
	  case ANSI_NEGO :		return "NEGOTIATE";
	  case ANSI_LOGON :		return "LOGON";
	  case ANSI_SECURITY :	return "SECURITY";
	  case ANSI_WRITE :		return "TABLE WRITE";
	  case ANSI_READ :		return "TABLE READ";
	  case ANSI_PREAD :		return "TABLE READ"; 
	  case ANSI_LOGOFF :	return "LOGOFF"; 
	  case ANSI_TERMI :		return "TERMINATE"; 
	}
	return "UNKNOWN";
}

void CI210Parser::GetMessage(ONDEMANDPARAM *pOndemand, CChunk *pChunk, char *pszFrame, WORD nLength, int nOption)
{
	ANSI_CONFIG		*pProtocol;
	METER_CONFIG	*pConfig;

	pConfig = &pOndemand->config;
	pProtocol = &pOndemand->ansi;

    switch(pProtocol->packet_cmd) {
      case ANSI_IDENT : 		// identify
		   if (nOption == READ_OPTION_TEST)
				pProtocol->packet_cmd = ANSI_TERMI;
		   else pProtocol->packet_cmd = ANSI_NEGO;
           break;

      case ANSI_NEGO : 			// negotiate
           pProtocol->packet_cmd = ANSI_LOGON;
		   break;

      case ANSI_LOGON : 		// log on
           pProtocol->packet_cmd = ANSI_SECURITY;
           break;

      case ANSI_SECURITY : 		// Security 
           pProtocol->packet_cmd = ANSI_WRITE;
           pProtocol->packet_table_id = ANSI_TABLE_ST_07;
           pProtocol->packet_write_table_buf[0] = 0x54;
           pProtocol->packet_write_table_buf[1] = 0x08;
           pProtocol->packet_write_table_buf[2] = 0x00;
           pProtocol->packet_write_table_count = 3;
		   break;

      case ANSI_WRITE : 		// Table Write
           WriteMessage(pOndemand, nOption);
           break;

      case ANSI_RR : 			// Reading Continue
           pConfig->meter_rr_count++;
           break;

      case ANSI_READ : 			// Full Read
           break;

      case ANSI_PREAD : 		// Partial Read
		   pConfig->meter_rr_count++;
		   PartialReadMessage(pOndemand, pChunk, pszFrame, nLength, nOption);
           break;

      case ANSI_LOGOFF : 		// Log off           
           pProtocol->packet_cmd = ANSI_TERMI;
           break;

      case ANSI_TERMI : 		// Terminate
           break;

      default :
           break;
    }
}

