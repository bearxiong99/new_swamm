//////////////////////////////////////////////////////////////////////
//
//  IF4TcpService.cpp: implementation of the CIF4TcpService class.
//
//  This file is a part of the AIMIR.
//  (c)Copyright 2005 NURI Telecom Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
//  http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "IF4TcpService.h"
#include "IF4Service.h"
#include "IF4DataFrame.h"
#include "IF4CrcCheck.h"
#include "IF4Util.h"
#include "varapi.h"
#include "zlib.h"
#include "Chunk.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "logdef.h"

#define COMMAND_LOG( ...)      UpdateLogFile(LOG_DIR, CMD_HISTORY_LOG_FILE, m_nIF4CmdLogSize, TRUE, __VA_ARGS__)
extern int m_nIF4CmdLogSize;

//////////////////////////////////////////////////////////////////////
// CIF4TcpService Class
//////////////////////////////////////////////////////////////////////

CIF4TcpService::CIF4TcpService()
{
}

CIF4TcpService::~CIF4TcpService()
{
}

//////////////////////////////////////////////////////////////////////
// CIF4TcpService Operations 
//////////////////////////////////////////////////////////////////////

BOOL CIF4TcpService::Start(int nPort, int nMaxSession)
{
    int        i;

    // Initialize Worker
    for(i=0; i<MAX_SVC_WORKER_COUNT; i++)
        m_Worker[i].Initialize();

    // Startup Local Server
    CTCPMultiplexServer::Startup(nPort, nMaxSession);
    return TRUE;
}

void CIF4TcpService::Stop()
{
    int        i;

    CTCPMultiplexServer::Shutdown();

    // Destroy Worker
    for(i=0; i<MAX_SVC_WORKER_COUNT; i++)
        m_Worker[i].Destroy();
}

void CIF4TcpService::Broadcast(char *pszBuffer, int nLength)
{
    if (!pszBuffer || !nLength)
        return;

    CTCPMultiplexServer::Broadcast(pszBuffer, nLength);
}

//////////////////////////////////////////////////////////////////////
// CIF4TcpService Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CIF4TcpService::WriteChar(WORKSESSION *pSession, unsigned char c)
{
    return SendToSession(pSession, (char)c) == -1 ? FALSE : TRUE;
}

BOOL CIF4TcpService::WriteStream(WORKSESSION *pSession, const char *pszBuffer, int nLength)
{
    nLength = nLength == -1 ? strlen(pszBuffer) : nLength;
    return SendToSession(pSession, pszBuffer, nLength) == -1 ? FALSE : TRUE;
}

//////////////////////////////////////////////////////////////////////
// Message Handlers
//////////////////////////////////////////////////////////////////////

void CIF4TcpService::OnAcceptSession(WORKSESSION *pSession)
{
    int        nOptValue;

    // turn on KEEPALIVE so if the client crashes, we'll know about it
    nOptValue    = 1;
    setsockopt(pSession->sSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&nOptValue, sizeof(nOptValue));
}

BOOL CIF4TcpService::OnEnterSession(WORKSESSION *pSession)
{
    DATASTREAM  *pStream;

//    XDEBUG("IF4TCP(%s): Enter Session.\xd\xa", pSession->szAddress);
//    IF4API_AddLog("Connect '%s'", pSession->szAddress);

    pStream = NewStream((UINT_PTR)pSession);
    if (pStream == NULL)
        return FALSE;

    pSession->dwUserData = (UINT_PTR)pStream;
    HelloMessage(pSession, pStream);
    return TRUE;
}

void CIF4TcpService::OnLeaveSession(WORKSESSION *pSession)
{
    DATASTREAM                *pStream;
    DOWNLOAD_FILE_FORMAT    *pDownload;
    CChunk                    *pChunk;

//    XDEBUG("IF4TCP(%s): Leave Session.\xd\xa", pSession->szAddress);
//    IF4API_AddLog("Disconnect '%s'", pSession->szAddress);

    pDownload = (DOWNLOAD_FILE_FORMAT *)pSession->pDownload;
    pChunk = (CChunk *)pSession->pChunk;

    if (pDownload) free(pDownload);
    if (pChunk) delete pChunk;

    pStream = (DATASTREAM *)pSession->dwUserData;
    if (pStream)
    {
        pSession->dwUserData = (UINT_PTR)0;
        CloseWindow(pSession, pStream);
        DeleteStream(pStream);
    }
}

void CIF4TcpService::OnSendSession(WORKSESSION *pSession, const char* pszBuffer, int nLength)
{
//    XDEBUG("IF4TCP(%s): Send %0d Byte(s)\xd\xa", pSession->szAddress, nLength);
//    XDUMP(pszBuffer, nLength, TRUE);
}

BOOL CIF4TcpService::OnReceiveSession(WORKSESSION *pSession, const char* pszBuffer, int nLength)
{
//    XDEBUG("IF4TCP(%s): Receive %0d Byte(s)\xd\xa", pSession->szAddress, nLength);
//    XDUMP(pszBuffer, nLength, TRUE);

    return CIF4Stream::ParseStream(pSession, (DATASTREAM *)pSession->dwUserData,
                    (const unsigned char *)pszBuffer, nLength);
}

void CIF4TcpService::OnErrorSession(WORKSESSION *pSession, int nType)
{
//    XDEBUG("IF4TCP(%s): Error Session.\xd\xa", pSession->szAddress);
}

void CIF4TcpService::OnCloseSession(WORKSESSION *pSession)
{
//    XDEBUG("IF4TCP(%s): Close Session.\xd\xa", pSession->szAddress);
}

BOOL CIF4TcpService::OnTimeoutSession(WORKSESSION *pSession)
{
//    XDEBUG("IF4TCP(%s): Timeout Session.\xd\xa", pSession->szAddress);
    return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Command Override Functions
//////////////////////////////////////////////////////////////////////

void CIF4TcpService::HelloMessage(WORKSESSION *pSession, DATASTREAM *pStream)
{
    IF4_CTRL_FRAME    frame;
    IF4_CTRL_ENQ    enq;
    int        nLength;

//    XDEBUG("IF4SERVER: Send [ENQ] (Version=%0d.%0d, WindowSize=%0d, WindowCount=%0d)\xd\xa",
//                IF4VER_PROTOCOL_HIGH, IF4VER_PROTOCOL_LOW,
//                IF4_WINDOW_SIZE, IF4_WINDOW_COUNT);

    memset(&enq, 0, sizeof(IF4_CTRL_ENQ));
    enq.ver[0] = IF4VER_PROTOCOL_HIGH;
    enq.ver[1] = IF4VER_PROTOCOL_LOW;

    // IF4 Protocol V1.1 Spec
    enq.window_size  = HostToLittleShort(IF4_WINDOW_SIZE);
    enq.window_count = IF4_WINDOW_COUNT;

    nLength = MakeControlFrame(&frame, IF4_ENQ, (char *)&enq, sizeof(IF4_CTRL_ENQ));
    WriteStream(pSession, (char *)&frame, nLength);
}

void CIF4TcpService::SendRedirection(WORKSESSION *pSession, char *pszAddress, int nPort)
{
    IF4_CTRL_FRAME        frame;
    IF4_CTRL_REDIRECT    redir;
    unsigned long        addr;
    struct    hostent        *host;
    int        nLength;

    if (!pszAddress || !*pszAddress)
    {
        XDEBUG("Invalid redirection address\r\n");
        return;
    }

    if (nPort <= 0)
    {
        XDEBUG("Invalid redirection port = %0d\r\n", nPort);
        return;
    }

//    XDEBUG("IF4SERVER: Send [REDIRECT] (Address=%s, Port=%0d)\xd\xa", pszAddress, nPort);

    addr = inet_addr(pszAddress);
    if (addr == INADDR_NONE)
    {
        host = gethostbyname(pszAddress);
        if (host != NULL) addr = (unsigned long)*((unsigned long *)host->h_addr);
    }

    memset(&redir, 0, sizeof(IF4_CTRL_REDIRECT));
    memcpy(&redir.addr, &addr, 4);
    redir.port = HostToLittleInt(nPort);

    nLength = MakeControlFrame(&frame, IF4_REDIRECT, (char *)&redir, sizeof(IF4_CTRL_REDIRECT));
    WriteStream(pSession, (char *)&frame, nLength);
}

void CIF4TcpService::SendRetry(WORKSESSION *pSession, int nDelay)
{
    IF4_CTRL_FRAME    frame;
    IF4_CTRL_RETRY    retry;
    int        nLength;

//    XDEBUG("IF4SERVER: Send [RETRY] (Retry=%0d)\xd\xa", nDelay);

    memset(&retry, 0, sizeof(IF4_CTRL_RETRY));
    retry.delay = HostToLittleInt(nDelay);

    nLength = MakeControlFrame(&frame, IF4_RETRY, (char *)&retry, sizeof(IF4_CTRL_RETRY));
    WriteStream(pSession, (char *)&frame, nLength);
}

void CIF4TcpService::SendAck(WORKSESSION *pSession, BYTE nSeq)
{
    IF4_CTRL_FRAME    frame;
    int        nLength;
    BYTE    arg[1];

//    XDEBUG("IF4SERVER: Send [ACK] (SEQ=%0d)\xd\xa", nSeq);

    arg[0] = nSeq;
    nLength = MakeControlFrame(&frame, IF4_ACK, (char *)arg, 1);
    WriteStream(pSession, (char *)&frame, nLength);
}

void CIF4TcpService::SendNak(WORKSESSION *pSession, BYTE nSeq)
{
    IF4_CTRL_FRAME    frame;
    int        nLength;
    BYTE    arg[1];

//    XDEBUG("IF4SERVER: Send [NAK] (SEQ=%0d)\xd\xa", nSeq);

    arg[0] = nSeq;
    nLength = MakeControlFrame(&frame, IF4_NAK, (char *)arg, 1);
    WriteStream(pSession, (char *)&frame, nLength);
}

void CIF4TcpService::SendNakWindow(WORKSESSION *pSession, BYTE *pList, BYTE nCount)
{
    IF4_CTRL_FRAME    frame;
    int        i, nLength;

    XDEBUG("IF4SERVER: Send [NAK] (Multi-Count=%0d)\xd\xa", nCount);

    for(i=0; i<nCount; i++)
        XDEBUG("     SEQ(%0d) NAK\r\n", pList[i] & 0xff);
    nLength = MakeControlFrame(&frame, IF4_NAK, (char *)pList, nCount);
    WriteStream(pSession, (char *)&frame, nLength);
}

void CIF4TcpService::InitWindow(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_HEADER    *pSource;
    int            i;

    pSource = (IF4_HEADER *)pszBuffer;
    pStream->bWindow       = TRUE;
    pStream->bTerminate      = FALSE;
    pStream->pszWindow       = NULL;
    pStream->nTotalSize      = 0;
    pStream->nWindowSize  = IF4_WINDOW_SIZE;
    pStream->nWindowCount = IF4_WINDOW_COUNT;
    pStream->nMultiSeek   = 0;
    pStream->nWindow       = 0;
    pStream->nWindowAlloc = 0;
    pStream->nService = pSource->svc;

    for(i=0; i<IF4_WINDOW_COUNT; i++)
    {
        pStream->arNeed[i]   = pSource->seq + i;
        pStream->arWindow[i] = -1;
    }
}

void CIF4TcpService::CloseWindow(WORKSESSION *pSession, DATASTREAM *pStream)
{
    if (pStream->pszWindow)
        FREE(pStream->pszWindow);

    pStream->pszWindow       = NULL;
    pStream->nTotalSize      = 0;
    pStream->nWindowSize  = 0;
    pStream->nMultiSeek   = 0;
    pStream->nWindow       = 0;
    pStream->nWindowAlloc = 0;
    pStream->bTerminate      = FALSE;
    pStream->bWindow       = FALSE;
}

void CIF4TcpService::NextWindow(WORKSESSION *pSession, DATASTREAM *pStream, BYTE nextSEQ)
{
    int        i;

    pStream->nMultiSeek += pStream->nWindow;
    for(i=0; i<IF4_WINDOW_COUNT; i++)
    {
        pStream->arNeed[i]   = nextSEQ + i;
           pStream->arWindow[i] = -1;
    }
    pStream->nWindow = 0;
}

void CIF4TcpService::SetWindow(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_PACKET    *pPacket;
    char        *pSeek;    
    int            index;

    pPacket = (IF4_PACKET *)pszBuffer;
    index = IsAcceptWindow(pSession, pStream, pPacket->hdr.seq);
    if (index == -1)
    {
        XDEBUG("IF4SERVER: Invalid Sequence!! (SEQ=%0d)\r\n", pPacket->hdr.seq);
        SendNak(pSession, pPacket->hdr.seq);
        return;
    }

    if (pStream->arWindow[index] != -1)
    {
        XDEBUG("IF4SERVER: Duplicate Sequence. (SEQ=%0d, INDEX=%0d, arWindow=%0d)\r\n",
                 pPacket->hdr.seq, index, pStream->arWindow[index]);
        return;
    }

    if ((pStream->nTotalSize+LittleToHostInt(pPacket->hdr.len)) > (UINT)pStream->nWindowAlloc)
    {
        if(pStream->pszWindow)
        {
            pStream->pszWindow = (char *)REALLOC(pStream->pszWindow, pStream->nWindowAlloc+IF4_WINDOW_GROW_SIZE);
        }
        else
        {
            pStream->pszWindow = (char *)MALLOC(pStream->nWindowAlloc+IF4_WINDOW_GROW_SIZE);
        }
        pStream->nWindowAlloc += IF4_WINDOW_GROW_SIZE;
    }

    pSeek  = pStream->pszWindow + (pStream->nWindowSize * pStream->nMultiSeek);
    pSeek += (pStream->nWindowSize * index);
    memcpy(pSeek, pPacket->arg, LittleToHostInt(pPacket->hdr.len));
    pStream->nTotalSize += LittleToHostInt(pPacket->hdr.len);
    pStream->arWindow[index] = pPacket->hdr.seq;    
    pStream->nWindow++;
}

int CIF4TcpService::IsAcceptWindow(WORKSESSION *pSession, DATASTREAM *pStream, BYTE nSEQ)
{
    int        i;

    for(i=0; i<IF4_WINDOW_COUNT; i++)
    {
        if (pStream->arNeed[i] == nSEQ)
            return i;
    }
    return -1;
}

void CIF4TcpService::SendCommandError(WORKSESSION *pSession, DATASTREAM *pStream, OID3 *oid, BYTE tid, BYTE nError)
{
    IF4_CMD_FRAME    frame;
    int                nLength;

    char    szOID[20];
    OidToString(oid, szOID);
    XDEBUG("IF4SERVER-ERROR: CMD=%s, TID=%0d, MSG=%s(%0d)\xd\xa",
                szOID, tid,
                GetIF4ErrorMessage(nError), nError);

    nLength = MakeCommandFrame(&frame, oid, tid, 0, nError, 0, NULL);
    WriteStream(pSession, (char *)&frame, nLength);
}

void CIF4TcpService::SendCommandResult(WORKSESSION *pSession, DATASTREAM *pStream, OID3 *oid, BYTE tid, BYTE nError, MIBValue *pLink, int nCount, BOOL bCompress)
{
    CChunk            chunk(256);
    SMIValue        var;
    MIBValue        *pNode;
    IF4_CMD_FRAME    *pFrame;
    IF4_COMMAND_TABLE    *pCommand;
    int                i, nLength;
    char            szOID[20];
    BOOL            bDisplay = TRUE;

    OidToString(oid, szOID);
    if (strcmp(szOID, "197.12") != 0)
    {
        XDEBUG("IF4SERVER-RESULT: CMD=%s, TID=%0d, MSG=%s(%0d)\xd\xa",
                szOID, tid,
                GetIF4ErrorMessage(nError), nError);
    }
    else
    {
        bDisplay = FALSE;
    }

    pCommand = m_pIF4Service->FindCommand(oid);
    if (pCommand && pCommand->bSave)
    {
        COMMAND_LOG("%s REPLY %s (%s)\xd\xa",
            pSession->szAddress,
            pCommand->pszName,
            GetIF4ErrorMessage(nError));
    }

    pNode = pLink;
    for(i=0; pNode && (i<nCount); i++)
    {
        memcpy(&var.oid, &pNode->oid, sizeof(OID3));
        var.len = pNode->len;
        chunk.Add((char *)&var, 5);

        switch(pNode->type) {
          case VARSMI_CHAR :
          case VARSMI_BOOL :
          case VARSMI_BYTE :
          case VARSMI_SHORT :
          case VARSMI_WORD :
          case VARSMI_INT :
          case VARSMI_UINT :
               chunk.Add((char *)&pNode->stream.u32, pNode->len);
               break;
          case VARSMI_OID :
               chunk.Add((char *)&pNode->stream.id, pNode->len);
               break;
          case VARSMI_STRING :
          case VARSMI_STREAM :
          case VARSMI_OPAQUE :
          case VARSMI_EUI64 :
          case VARSMI_IPADDR :
          case VARSMI_TIMESTAMP :
          case VARSMI_TIMEDATE :
               chunk.Add((char *)pNode->stream.p, pNode->len);
               break;
        }
        pNode = pNode->pNext;
    }

    if (bDisplay) DumpSmiValue(chunk.GetBuffer(), nCount);
    
    if (pCommand && pCommand->bSave)
        LogSmiValue(chunk.GetBuffer(), nCount);

    pFrame = (IF4_CMD_FRAME *)MALLOC(sizeof(IF4_CMD_FRAME) + chunk.GetSize() + 24);
    if (pFrame != NULL)
    {
        nLength = MakeCommandFrame(pFrame, oid, tid, 0, nError, nCount,
                                (SMIValue *)chunk.GetBuffer(), NULL, bCompress);
        SendResultToClient(pSession, (BYTE *)pFrame, nLength);
        FREE(pFrame);
    }
}

void CIF4TcpService::SendResultToClient(WORKSESSION *pSession, BYTE *pStream, int nLength)
{
    // 분할 하지 않는 최소 길이 이면 그냥 전송한다.
    if (nLength < IF4_MIN_MULTI_PART_SIZE)
    {
        WriteStream(pSession, (char *)pStream, nLength);
        return;
    }

    // 분할 전송 한다.
    // 기존 버젼과의 호환성 문제로 결과는 Window 처리를 하지 않는다.
    // 모든 버젼이 프로토콜 v1.1이상이 되면 구현한다.
    
    WriteStream(pSession, (char *)pStream, nLength);
}

BOOL CIF4TcpService::GetSafeClient(WORKSESSION *pSession)
{
    if (GetSession(pSession))
        return TRUE;
    return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Command Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CIF4TcpService::OnFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_HEADER    *pHeader;
    char        *pszAllocate = NULL;
    uLong        nDestLen;    
    BOOL        bResult;
    int            nError;


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
	    XDEBUG("Uncompress result=%d Byte(s), nError=%d, Source=%d\r\n",
                            (int)nDestLen, nError,
                            nLength-(sizeof(IF4_HEADER)+sizeof(IF4_TAIL)));
            
        if (nDestLen > 0)
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

    bResult = ParseFrame(pSession, pStream, pszBuffer, nLength);

    if (pszAllocate != NULL) FREE(pszAllocate);
    return bResult;
}

BOOL CIF4TcpService::ParseFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_HEADER    *pHeader;
    BYTE        seq;
    BOOL        bOK;

    pHeader = (IF4_HEADER *)pszBuffer;
#ifdef __DEBUG__
    char        szString[128] = "", ch[10], szService[40]="";

    if (pHeader->attr & IF4_ATTR_CTRLFRAME)
        strcat(szString, "CTRL ");
    if (pHeader->attr & IF4_ATTR_MUSTCONFIRM)
        strcat(szString, "MUSTCONFIRM ");
    if (pHeader->attr & IF4_ATTR_CRYPT)
        strcat(szString, "CRYPT ");
    if (pHeader->attr & IF4_ATTR_COMPRESS)
        strcat(szString, "COMPRESS ");
    if (pHeader->attr & IF4_ATTR_START)
        strcat(szString, "START ");
    if (pHeader->attr & IF4_ATTR_END)
        strcat(szString, "END ");

    if (pHeader->svc >= ' ')
         sprintf(ch, "'%c'", pHeader->svc);
    else sprintf(ch, "0x%02X", pHeader->svc);

    switch(pHeader->svc) {
      case IF4_SVC_CONTROL :         strcpy(szService, "Control"); break;
      case IF4_SVC_COMMAND :        strcpy(szService, "Command Service"); break;
      case IF4_SVC_DATA :            strcpy(szService, "Measurement Metering Data Service (V1.0)"); break;
      case IF4_SVC_METERINGDATA :    strcpy(szService, "Measurement Metering Data Service (V3.0)"); break;
      case IF4_SVC_NEWMETERDATA :    strcpy(szService, "Measurement Metering Data Service (V4.0)"); break;
      case IF4_SVC_DATAFILE :        strcpy(szService, "Data File Service"); break;
      case IF4_SVC_ALARM :            strcpy(szService, "Alarm Service"); break;
      case IF4_SVC_EVENT :            strcpy(szService, "Event Service"); break;
      case IF4_SVC_TRANSFER :        strcpy(szService, "Transfer Service"); break;
      case IF4_SVC_TELNET :            strcpy(szService, "Telnet Service"); break;
      case IF4_SVC_PARTIAL :        strcpy(szService, "Partial Frame"); break;
      default :                        strcpy(szService, "Unknown Service"); break;
    }

    XDEBUG("\xd\xa");
    XDEBUG("REQUEST %0d Byte(s)\xd\xa", pStream->nLength);
    XDUMP(pStream->pszBuffer, pStream->nLength);
    XDEBUG("\xd\xa");
    XDEBUG("SEQ       = %d\xd\xa", pHeader->seq);
    XDEBUG("ATTRIBUTE = %s\xd\xa", szString);
    XDEBUG("LENGTH    = %d\xd\xa", LittleToHostInt(pHeader->len));
    XDEBUG("SERVICE   = %s %s\xd\xa", ch, szService);
#endif

    if (pHeader->attr & IF4_ATTR_CTRLFRAME)
    {
        // Control Frame
        return ControlFrame(pSession, pStream, pszBuffer, nLength);
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
//            XDEBUG("IF4SERVER: [SEQ=%0d][%0d] Confirmation %s\xd\xa",
//                    pHeader->seq, seq,
//                    (bOK == TRUE) ? "ACK" : "NAK");
            if (bOK)
            {
                SendAck(pSession, pHeader->seq);
            }
            else
            {
                SendNak(pSession, seq);
                return TRUE;
            }
        }
        else
        {
//            XDEBUG("IF4SERVER: [SEQ=%0d][%0d] Don't Care Sequence!!\xd\xa",
//                    pHeader->seq, seq);
        }

        // Store Last Sequence
        pStream->nLastSeq = pHeader->seq;

        // Check Multi-Part Frame
        if ((pHeader->attr & IF4_ATTR_START) && (pHeader->attr & IF4_ATTR_END))
        {
            // Single Frame
//            XDEBUG("IF4SERVER: Single Frame (SEQ=%0d, LEN=%0d)\xd\xa",
//                        pHeader->seq, LittleToHostInt(pHeader->len));
        }
        else
        {
            // Multi-Part Frame
            if (LittleToHostInt(pHeader->len) > 9152)
            {
                XDEBUG("IF4SERVER: Out of packet size (SEQ=%0d, LENGTH=%0d)\xd\xa",
                        pHeader->seq, LittleToHostInt(pHeader->len));
                SendNak(pSession, pHeader->seq);
                return TRUE;
            }

            if (pHeader->attr & IF4_ATTR_START)
            {
                CloseWindow(pSession, pStream);
                InitWindow(pSession, pStream, pszBuffer, nLength);
                XDEBUG("IF4SERVER: ----- Start Multi-Part Frame (SEQ=%0d) -----\xd\xa", pHeader->seq);
            }
            else if (pHeader->attr & IF4_ATTR_END)
            {
                XDEBUG("IF4SERVER: ----- End Multi-Part Frame (SEQ=%0d) -----\xd\xa", pHeader->seq);
                XDEBUG("IF4SERVER: Total-Frame=%0d, Total-Length=%0d\xd\xa",
                        pStream->nMultiSeek, pStream->nTotalSize);
                pStream->bTerminate = TRUE;
            }
            
            if (pStream->nWindow < IF4_WINDOW_COUNT)
            {
                SetWindow(pSession, pStream, pszBuffer, nLength);
                XDEBUG("IF4SERVER: Partial Block (SEQ=%0d, LEN=%0d, TOTAL=%0d)\xd\xa",
                        pHeader->seq, LittleToHostInt(pHeader->len), pStream->nTotalSize);
            }
            else
            {
                XDEBUG("IF4SERVER: ----- Out Of Window Size (SEQ=%0d) -----\xd\xa", pHeader->seq);
                SendNak(pSession, pHeader->seq);
            }
            return TRUE;
        }

        // Service Frame
        switch(pHeader->svc) {
          case IF4_SVC_COMMAND :
               return CommandFrame(pSession, pStream, pszBuffer, nLength);
          case IF4_SVC_ALARM :
               return AlarmFrame(pSession, pStream, pszBuffer, nLength);
          case IF4_SVC_EVENT :
               return EventFrame(pSession, pStream, pszBuffer, nLength);
          case IF4_SVC_DATA :
          case IF4_SVC_METERINGDATA :
               return DataFrame(pSession, pStream, pszBuffer, nLength);
          case IF4_SVC_NEWMETERDATA :
               return TypeRFrame(pSession, pStream, pszBuffer, nLength);
          case IF4_SVC_DATAFILE :
               return DataFileFrame(pSession, pStream, pszBuffer, nLength);
          case IF4_SVC_TRANSFER :
               return TransferFrame(pSession, pStream, pszBuffer, nLength);
          case IF4_SVC_TELNET :
               XDEBUG("ERROR: Do not support.\xd\xa");
               break; 
          default :
               XDEBUG("ERROR: Unknown Service Frame(%c)\xd\xa", pHeader->svc);
               break;
        }
    }
    return TRUE;
}

void CIF4TcpService::OnDownloadAck(WORKSESSION *pSession, DATASTREAM *pStream, BYTE seq)
{
    DOWNLOAD_PACKET        *pPacket;
    DOWNLOAD_TAIL        *pTail;
    BYTE                szBuffer[64];
    WORD                len = 1;

    pPacket = (DOWNLOAD_PACKET *)szBuffer;
    pTail = (DOWNLOAD_TAIL *)&szBuffer[sizeof(DOWNLOAD_HEADER)+1];

    memset(pPacket, 0, sizeof(DOWNLOAD_HEADER)+sizeof(DOWNLOAD_TAIL)+1);
    pPacket->hdr.ident  = DOWNLOAD_IDENTIFICATION;
    pPacket->hdr.seq    = seq;
    pPacket->hdr.attr     = DOWNLOAD_ATTR_CONTROL;
    pPacket->hdr.len    = HostToLittleShort(len);
    pPacket->data[0]    = IF4_ACK;
    pTail->crc = IF4GetCrc((BYTE *)szBuffer, sizeof(DOWNLOAD_HEADER)+1, 0);

    WriteStream(pSession, (char *)pPacket, sizeof(DOWNLOAD_HEADER)+sizeof(DOWNLOAD_TAIL)+1);
}

BOOL CIF4TcpService::ControlFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_HEADER        *pHeader;
    IF4_CTRL_FRAME    *pCtrl;
    BYTE            arNak[IF4_WINDOW_COUNT];
    BYTE            index, seq, start, end, last, cnt;

    IF4_COMPRESS_HEADER    *pCompress;
    IF4_TAIL    *pTail;
    BYTE        *pFrame, *pszAllocate = NULL;
    int            nFrameSize, nSrcLen, len, nError;
    uLong        nDestLen;    

    pCtrl = (IF4_CTRL_FRAME *)pszBuffer;
#ifdef __DEBUG__
    XDEBUG("CTRL: CODE=0x%02X, ARG-LENGTH=%d, ARGMENT=", pCtrl->code, LittleToHostShort(pCtrl->len));
    XDUMP((char *)pCtrl->arg, LittleToHostShort(pCtrl->len));
#endif

    switch(pCtrl->code) {
      case IF4_ENQ :
           XDEBUG("IF4SERVER-CTRL: Recv [ENQ][%0d] Protocol Version=%0d.%0d\xd\xa",
                         LittleToHostShort(pCtrl->len),
                         BCD2BYTE(pCtrl->arg[0]), BCD2BYTE(pCtrl->arg[1]));
           break;
      case IF4_ACK :
           XDEBUG("IF4SERVER-CTRL: Recv [ACK][%0d] Last Sequence=%0d\xd\xa",
                         LittleToHostShort(pCtrl->len), pCtrl->arg[0]);
           break;
      case IF4_NAK :
           XDEBUG("IF4SERVER-CTRL: Recv [NAK][%0d] Error Sequence=%0d\xd\xa",
                         LittleToHostShort(pCtrl->len), pCtrl->arg[0]);
           break;
      case IF4_CAN :
           XDEBUG("IF4SERVER-CTRL: Recv [CAN][%0d] Cancel Frame TID=%0d\xd\xa",
                         LittleToHostShort(pCtrl->len), pCtrl->arg[0]);
           break;

      case IF4_EOT :
           XDEBUG("IF4SERVER-CTRL: Recv [EOT][%0d] Request Disconnect.\r\n\r\n", LittleToHostShort(pCtrl->len));
           return FALSE;

      case IF4_AUT :
           XDEBUG("IF4SERVER-CTRL: Recv [AUT][%0d] Session Key=", LittleToHostShort(pCtrl->len));
           XDUMP(pCtrl->arg, LittleToHostShort(pCtrl->len));
           break;

      case IF4_WCK :
           XDEBUG("IF4SERVER-CTRL: Recv [WCK][%0d] Star=%0d, End=%0d\xd\xa",
                 LittleToHostShort(pCtrl->len), pCtrl->arg[0], pCtrl->arg[1]);
           cnt   = index = 0;
           start = pCtrl->arg[0];
           end   = pCtrl->arg[1];
           last  = end + 1;
           memset(arNak, 0, IF4_WINDOW_COUNT);
           for(seq=start; (seq!=last) && (index < IF4_WINDOW_COUNT); seq++, index++)
           {
               if (pStream->arWindow[index] != seq)
               {
//                   XDEBUG("   arWindow[%0d] = %0d, SEQ=%0d\r\n", index, pStream->arWindow[index], seq);
                    arNak[cnt] = seq;
                     cnt++;
               }
           }
           if (cnt > 0)
           {
                  SendNakWindow(pSession, arNak, cnt);
               break;
           }

           SendAck(pSession, end);
           if (!pStream->bTerminate)
           {
                  NextWindow(pSession, pStream, end+1);
               break;
           }

           if(pStream->nService == IF4_SVC_PARTIAL)
           {
               pCompress = (IF4_COMPRESS_HEADER *)pStream->pszWindow;
               XDEBUG("IF4SERVER: COMPRESS = %0d, SOURCE SIZE = %0d, TOTAL SIZE = %0d\r\n", 
                    pCompress->compress,
                    LittleToHostInt(pCompress->source_size),
                    pStream->nTotalSize);

               pFrame = (BYTE *)pStream->pszWindow + sizeof(IF4_COMPRESS_HEADER);
               nFrameSize = pStream->nTotalSize - sizeof(IF4_COMPRESS_HEADER);
               nSrcLen = LittleToHostInt(pCompress->source_size);
           }
           else
           {
               pFrame = (BYTE *)pStream->pszWindow;
               nFrameSize = pStream->nTotalSize;
               nSrcLen = 500000;
           }

           // 수신하여 조합한 프레임에 대한 CRC를 검사한다.
           if (!IF4VerifyCrc(pFrame, nFrameSize-2, 0))
           {
               XDEBUG("IF4SERVER: ---------- Multi-Frame Crc Error!! ---------\r\n");
               CloseWindow(pSession, pStream);
               return IF4ERR_CRC_ERROR;
           }

           // 압축된 프레임의 경우 압축을 해제한다.
           pHeader = (IF4_HEADER *)pFrame;
           if (pHeader->attr & IF4_ATTR_COMPRESS)
           {
               len = LittleToHostInt(pHeader->len);
               pszAllocate = (BYTE *)MALLOC(nSrcLen);
               if (pszAllocate == NULL)
               {
                   XDEBUG("Memory allocation Fail: Size = %0d\r\n", nSrcLen);
                   break;
               }
               memcpy(pszAllocate, pHeader, sizeof(IF4_HEADER));   
               nDestLen = nSrcLen - (sizeof(IF4_HEADER) + sizeof(IF4_TAIL));
               nError = uncompress((Bytef*)&pszAllocate[sizeof(IF4_HEADER)],
                            &nDestLen,
                            (const Bytef*)&pFrame[sizeof(IF4_HEADER)],
                            nFrameSize-(sizeof(IF4_HEADER)+sizeof(IF4_TAIL)));
//                XDEBUG("Uncompress result=%d Byte(s), nError=%d, Source=%d\r\n",
//                            (int)nDestLen, nError,
//                            nFrameSize-(sizeof(IF4_HEADER)+sizeof(IF4_TAIL)));
            
               if (nDestLen > 0)
               {
                      pHeader = (IF4_HEADER *)pszAllocate;
                   pTail = (IF4_TAIL *)(pszAllocate+nDestLen+sizeof(IF4_HEADER));

//                   XDEBUG("GEN SIZE = %d\r\n", nDestLen+sizeof(IF4_HEADER)+sizeof(IF4_TAIL));
                   pHeader->len = HostToLittleInt(nDestLen);
                   memcpy(pTail, pFrame+sizeof(IF4_HEADER)+len, sizeof(IF4_TAIL));

                   pFrame = (BYTE *)pszAllocate;
                   nFrameSize = nDestLen + sizeof(IF4_HEADER) + sizeof(IF4_TAIL);
               }
               else
               {
                   if (pszAllocate) FREE(pszAllocate);
                   break;
               }
           }

           switch(pHeader->svc) {
             case IF4_SVC_COMMAND :
                  return CommandFrame(pSession, pStream, pFrame, nFrameSize);
             case IF4_SVC_ALARM :
                  return AlarmFrame(pSession, pStream, pFrame, nFrameSize);
             case IF4_SVC_EVENT :
                  return EventFrame(pSession, pStream, pFrame, nFrameSize);
             case IF4_SVC_DATA :
             case IF4_SVC_METERINGDATA :
                  return DataFrame(pSession, pStream, pFrame, nFrameSize);
             case IF4_SVC_NEWMETERDATA :
                  return TypeRFrame(pSession, pStream, pFrame, nFrameSize);
             case IF4_SVC_DATAFILE :
                  return DataFileFrame(pSession, pStream, pFrame, nFrameSize);
             case IF4_SVC_TRANSFER :
                  return TransferFrame(pSession, pStream, pFrame, nFrameSize);
             case IF4_SVC_TELNET :
                  XDEBUG("ERROR: Do not support.\xd\xa");
                  break; 
             default :
                  XDEBUG("ERROR: Unknown Service Frame(%c)\xd\xa", pHeader->svc);
                  break;
           }

           if (pszAllocate) FREE(pszAllocate);
           break;

      default :
           XDEBUG("IF4SERVER-CTRL: Recv Unknown Control Frame (0x%02X).\xd\xa", pCtrl->code);
           break;
    }
    return TRUE;
}

BOOL CIF4TcpService::CommandFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_CMD_FRAME        *pFrame;
    IF4_COMMAND_TABLE    *pCommand;
    VAROBJECT            *pObject;
    SMIValue            *pSeek;
    MIBValue            *pValues=NULL;
    char                szOID[20];
    int                    i;

    pFrame      = (IF4_CMD_FRAME *)pszBuffer;
    pCommand = m_pIF4Service->FindCommand(&pFrame->ch.cmd);
    OidToString(&pFrame->ch.cmd, szOID);

    if (pFrame->ch.attr & IF4_CMDATTR_REQUEST)
    {
        SmiToHostValue((char *)&pFrame->args, LittleToHostShort(pFrame->ch.cnt));
        // WARNING: cmdLiveCheck는 로그를 찍지 않는다.
        if (strcmp(szOID, "197.12") != 0)
        {
            XDEBUG("IF4SERVER-REQUEST: MCUID=%d, CMD=%s(%s), TID=%0d, REPLY=%s, ARG-CNT=%0d\xd\xa",
                LittleToHostInt(pFrame->sh.mcuid),
                pCommand == NULL ? "Unknown" : pCommand->pszName,
                szOID,
                pFrame->ch.tid,
                pFrame->ch.attr & IF4_CMDATTR_RESPONSE ? "WANT-REPLY" : "NO-REPLY",
                LittleToHostShort(pFrame->ch.cnt));
            DumpSmiValue((char *)&pFrame->args, LittleToHostShort(pFrame->ch.cnt));
        }

        // Check Command
        if ((pCommand == NULL) || (pCommand->pfnCommand == NULL))
        {
            SendCommandError(pSession, pStream, &pFrame->ch.cmd, pFrame->ch.tid,
                (!pCommand || !pCommand->pfnCommand) ? IF4ERR_DO_NOT_SUPPORT : IF4ERR_INVALID_COMMAND);
            return TRUE;
        }

        if (pCommand->bSave)
        {
            COMMAND_LOG("%s REQUEST %s\xd\xa", pSession->szAddress, pCommand->pszName);
            LogSmiValue((char *)&pFrame->args, LittleToHostShort(pFrame->ch.cnt));
        }

        // Check Parameter    
        pSeek = (SMIValue *)&pFrame->args;
        for(i=0; i<LittleToHostShort(pFrame->ch.cnt); i++)
        {
            pObject = VARAPI_GetObject(&pSeek->oid);
            if (pObject == NULL)
            {
                XDEBUG("   ***** Invalid Parameter (Index=%0d)\xd\xa", i+1);
                SendCommandError(pSession, pStream,
                            &pFrame->ch.cmd,
                            pFrame->ch.tid,
                            IF4ERR_INVALID_PARAM);
                return TRUE;
            }
            pSeek = (SMIValue *)((BYTE *)pSeek + (pSeek->len + 5)); 
        }

        pValues = (MIBValue *)MALLOC(sizeof(MIBValue)*LittleToHostShort(pFrame->ch.cnt));
        if (pValues != NULL)
        {
            memset(pValues, 0, sizeof(MIBValue)*LittleToHostShort(pFrame->ch.cnt));
            pSeek = (SMIValue *)&pFrame->args;
            for(i=0; i<LittleToHostShort(pFrame->ch.cnt); i++)
            {
                pObject = VARAPI_GetObject(&pSeek->oid);
                CopySMI2MIBValue(&pValues[i], pSeek, pObject->var.type);
                pSeek = (SMIValue *)((BYTE *)pSeek + (pSeek->len + 5)); 
                if(i) pValues[i-1].pNext = &pValues[i];
            }

            ExecuteCommand(pCommand, pSession, pStream,
                            &pFrame->ch.cmd, pFrame->ch.tid,
                            pFrame->ch.attr,
                            pValues, LittleToHostShort(pFrame->ch.cnt)); 
        }
    }
    else
    {
        XDEBUG("IF4SERVER-REPLY: CMD=%s(%s), TID=%0d, ARG-CNT=%0d, ERRCODE=%s(%0d)\xd\xa",
                pCommand == NULL ? "Unknown" : pCommand->pszName,
                szOID,
                pFrame->ch.tid,
                LittleToHostShort(pFrame->ch.cnt),
                GetIF4ErrorMessage(pFrame->ch.errcode),
                pFrame->ch.errcode);
        SmiToHostValue((char *)&pFrame->args, LittleToHostShort(pFrame->ch.cnt));
        DumpSmiValue((char *)&pFrame->args, LittleToHostShort(pFrame->ch.cnt));
    }
    return TRUE;
}

BOOL CIF4TcpService::ExecuteCommand(IF4_COMMAND_TABLE *pCommand, WORKSESSION *pSession, DATASTREAM *pStream, OID3 *cmd, BYTE tid, BYTE attr, MIBValue *pValues, WORD nCount)
{
    EXECPARAM    *pParam;
    CIF4Worker    *pWorker;
    int            i, nRetry, nMax;

    pParam = (EXECPARAM *)MALLOC(sizeof(EXECPARAM));
    if (pParam == NULL)
        return FALSE;

    nMax = MAX_SVC_WORKER_COUNT - 1;
    if (strcmp(pSession->szAddress, "127.0.0.1") == 0)
        nMax++;

    memset(pParam, 0, sizeof(EXECPARAM));
    memcpy(&pParam->cmd, cmd, sizeof(OID3));
    pParam->pSession    = pSession;
    pParam->pStream        = pStream;
    pParam->pCommand    = pCommand;
    pParam->pThis        = (void *)this;
    pParam->tid            = tid;
    pParam->attr        = attr;
    pParam->pValues        = pValues;
    pParam->nCount        = nCount;

    nRetry = 0;
    for(pWorker=NULL; pWorker==NULL;)
    {
        m_WorkerLocker.Lock();
        for(i=0; i<nMax; i++)
        {
            if (!m_Worker[i].IsWorking())
            {
                pWorker = &m_Worker[i];
                m_Worker[i].SetWorking();
                break;
            }
        }
        m_WorkerLocker.Unlock();

        if (pWorker == NULL)
        {
            USLEEP(1000000);
            nRetry++;
            if (nRetry > 10)
                break;
                
            continue;
        }
    }

    if (pWorker == NULL)
    {
        SendCommandError(pSession, pStream, cmd, tid, IF4ERR_NO_MORE_WORKER);
        FREE(pParam);
        return TRUE;
    }

    pWorker->Execute(pParam);
    return TRUE;
}

BOOL CIF4TcpService::AlarmFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_ALARM_FRAME    *pAlarm;

    pAlarm = (IF4_ALARM_FRAME *)pszBuffer;
    XDEBUG("IF4SERVER-ALARM: INDEX=%0d\xd\xa", pAlarm->ah.idx);
    CIF4Fiber::OnAlarm(pSession, LittleToHostInt(pAlarm->sh.mcuid), pAlarm, nLength);

    SendAck(pSession, pAlarm->hdr.seq);
    return TRUE;
}

BOOL CIF4TcpService::EventFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_EVENT_FRAME    *pEvent;
    VAROBJECT        *pObject;
    char            szOID[20];

    pEvent = (IF4_EVENT_FRAME *)pszBuffer;
    OidToString(&pEvent->eh.eventCode, szOID);
    pObject = VARAPI_GetObject(&pEvent->eh.eventCode);

    XDEBUG("IF4SERVER-EVENT: MCUID=%d, CMD=%s(%s), Param-Count=%0d\xd\xa",
            LittleToHostInt(pEvent->sh.mcuid),
            szOID, 
            pObject ? pObject->pszName : "Unknown",
            LittleToHostShort(pEvent->eh.cnt));
    SmiToHostValue((char *)&pEvent->args, LittleToHostShort(pEvent->eh.cnt));
    DumpSmiValue((char *)&pEvent->args, LittleToHostShort(pEvent->eh.cnt));
    CIF4Fiber::OnEvent(pSession, LittleToHostInt(pEvent->sh.mcuid), pEvent, nLength);

    SendAck(pSession, pEvent->hdr.seq);
    return TRUE;
}

BOOL CIF4TcpService::DataFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_DATA_FRAME    *pData;
    DATA_MTR_HEADER    *pEntry;
    char            *p;
    int                i;
    char            szId[64];

    pData = (IF4_DATA_FRAME *)pszBuffer;
    XDEBUG("IF4SERVER-METERING-DATA: MCUID=%d, Count=%0d\xd\xa",
            LittleToHostInt(pData->sh.mcuid),
            LittleToHostShort(pData->dh.cnt));

    p = pData->args;
    for(i=0; i<LittleToHostShort(pData->dh.cnt); i++)
    {
        pEntry = (DATA_MTR_HEADER *)p;
        EUI64ToStr(&pEntry->id, szId);
        XDEBUG("%4d: [%s] [%04d/%02d/%02d %02d:%02d:%02d] Length=%0d\r\n",
                i+1, szId,
                pEntry->timestamp.year, pEntry->timestamp.mon, pEntry->timestamp.day,
                pEntry->timestamp.hour, pEntry->timestamp.min, pEntry->timestamp.sec,
                LittleToHostShort(pEntry->length)-sizeof(TIMESTAMP));
        p += (LittleToHostShort(pEntry->length) + sizeof(DATA_MTR_HEADER) - sizeof(TIMESTAMP));
    }
    CIF4Fiber::OnData(pSession, LittleToHostInt(pData->sh.mcuid), pData, nLength);

    SendAck(pSession, pData->hdr.seq);
    return TRUE;
}

BOOL CIF4TcpService::TypeRFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_TYPER_FRAME     *pData;
    IF4_TYPER_HEADER    *pHeader;
    char                *p;
    int                 i, nCnt, nPayloadLen;

    pData = (IF4_TYPER_FRAME *)pszBuffer;
    nCnt = LittleToHostShort(pData->dh.cnt);
    XDEBUG("IF4SERVER-TYPER-DATA: MCUID=%d, Count=%0d\xd\xa",
            LittleToHostInt(pData->sh.mcuid), nCnt);

    p = pData->data;
    for(i=0; i<nCnt; i++)
    {
        pHeader = (IF4_TYPER_HEADER *) p;
        nPayloadLen = BigToHostShort(pHeader->length);
        XDEBUG("%4d: [%d] Length=%0d\r\n",
                i+1, pHeader->type, nPayloadLen);
        p += (nPayloadLen + sizeof(IF4_TYPER_HEADER));
    }
    CIF4Fiber::OnTypeR(pSession, LittleToHostInt(pData->sh.mcuid), pData, nLength);

    SendAck(pSession, pData->hdr.seq);
    return TRUE;
}

BOOL CIF4TcpService::DataFileFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_DATAFILE_FRAME    *pFrame;
    char    szFileName[33];

    pFrame = (IF4_DATAFILE_FRAME *)pszBuffer;
    strncpy(szFileName, pFrame->fh.fname, 32);
    szFileName[32] = '\0';

    XDEBUG("\r\n");
    XDEBUG("-------------------------- DATA FILE -----------------------\r\n");
    XDEBUG("MCUID = %0d\r\n", LittleToHostInt(pFrame->sh.mcuid));
    XDEBUG("NAME = '%s', FILE TYPE = %s, SOURCE LENGTH = %d\r\n",
            szFileName, GetDataFileType(pFrame->fh.ftype), LittleToHostInt(pFrame->fh.fsize));
    XDEBUG("------------------------------------------------------------\r\n");

    CIF4Fiber::OnDataFile(pSession, LittleToHostInt(pFrame->sh.mcuid), pFrame->fh.ftype,
                    szFileName, (BYTE *)&pFrame->data, LittleToHostInt(pFrame->fh.fsize));

    SendAck(pSession, pFrame->hdr.seq);
    return TRUE;
}

BOOL CIF4TcpService::TransferFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength)
{
    IF4_TRANSFER_FRAME    *pTransfer;

    pTransfer = (IF4_TRANSFER_FRAME *)pszBuffer;
    XDEBUG("IF4SERVER-TRANSFER: MCUID=%d, Name=%s, Length=%0d, Mode=0x%02X, Channel=%d\xd\xa",
            LittleToHostInt(pTransfer->sh.mcuid),
            pTransfer->th.fileName,
            LittleToHostInt(pTransfer->th.fileLength),
            pTransfer->th.fileChannel);
    XDEBUG("                    TIME=%04d/%02d/%02d %02d:%02d:%02d\xd\xa",
            LittleToHostShort(pTransfer->th.tmFile.year),
            pTransfer->th.tmFile.mon,
            pTransfer->th.tmFile.day,
            pTransfer->th.tmFile.hour,
            pTransfer->th.tmFile.min,
            pTransfer->th.tmFile.sec);
    return TRUE;
}

