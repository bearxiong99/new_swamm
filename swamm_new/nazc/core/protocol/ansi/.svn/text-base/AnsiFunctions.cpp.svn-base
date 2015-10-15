
#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "AnsiDataFrame.h"
#include "AnsiUtil.h"
#include "AnsiSession.h"
#include "AnsiCrcCheck.h"

#include "ansiError.h"
#include "ansiDataStream.h"
#include "ansiFunctions.h"
#include "ansiUtils.h"


int ansiInit(void)
{
    return 0;
}

int ansiMakeSingleFrame(ANSISESSION *pSession, BYTE *pszPayload, WORD nPayloadLen, BOOL bWithAck, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_FRAME frame;
    WORD nSize=0;
    int nError;
    int nWithAck = bWithAck ? 1 : 0;

    if(pSession == NULL || pszAnsiFrame == NULL || nFrameLength == NULL) return ANSIERR_INVALID_PARAM;

    if((nError=MakeAnsiSingleFrame(pSession, (BYTE *)&frame, pszPayload, nPayloadLen, &nSize))!=ANSIERR_NOERROR)
    {
        return nError;    
    }

    if(nSize <= 0)
        return ANSIERR_SYSTEM_ERROR;

    (*pszAnsiFrame) = (BYTE *)MALLOC(nSize + nWithAck);
    if(*pszAnsiFrame == NULL) return ANSIERR_MEMORY_ERROR;

    if(bWithAck)
    {
        MakeAnsiAck(pSession, *pszAnsiFrame, NULL);
    }

    memcpy(*pszAnsiFrame + nWithAck, (BYTE *)&frame, nSize);
    *nFrameLength = nSize + nWithAck;

    return nError;;
}

/** pszAnsiFrame은 이미 MALLOC 이 되어 있어야 한다.
 */
int ansiMakeAckFrame(ANSISESSION *pSession, BYTE *pszAnsiFrame)
{
    if (pszAnsiFrame == NULL) return ANSIERR_INVALID_PARAM;
    return MakeAnsiAck(pSession, pszAnsiFrame, NULL);
}

ANSISESSION * ansiNewSession(void)
{
    return NewAnsiSession();
}

void ansiDeleteSession(ANSISESSION * session)
{
    if(session == NULL) return;

    return DeleteAnsiSession(session);
}

////////////////////////////////////////////////////////////////////////////

int ansiMakeIdentificationFrame(ANSISESSION *pSession, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_IDENTIFICATION_PAYLOAD payload;
    payload.code = ANSI_CMD_IDENT; 
    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, sizeof(ANSI_IDENTIFICATION_PAYLOAD), FALSE, pszAnsiFrame, nFrameLength);
}

int ansiMakeNegotiateFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nPacketSize, BYTE nNbrPacket, BYTE nBaudRate,
        BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_NEGOTIATE_PAYLOAD payload;

    memset(&payload, 0, sizeof(ANSI_NEGOTIATE_PAYLOAD));
    payload.code = (BYTE)(ANSI_CMD_NEGO | 0x01);       // 1 <baud reate> include in request
    payload.packetSize = BigToHostShort(nPacketSize);
    payload.nbrPacket = nNbrPacket;
    payload.baudRate = nBaudRate;

    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, sizeof(ANSI_NEGOTIATE_PAYLOAD), bWithAck, pszAnsiFrame, nFrameLength);
}

int ansiMakeLogonFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nUserId, BYTE *pszUser, int nUserLength,
        BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_LOGON_PAYLOAD payload;

    memset(&payload, 0, sizeof(ANSI_LOGON_PAYLOAD));
    payload.code = ANSI_CMD_LOGON; 
    payload.userId = BigToHostShort(nUserId);
    if(pszUser != NULL)
    {
        memcpy(payload.user, pszUser, MIN(nUserLength, (int)sizeof(payload.user)));
    }

    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, sizeof(ANSI_LOGON_PAYLOAD), bWithAck, pszAnsiFrame, nFrameLength);
}

int ansiMakeSecurityFrame(ANSISESSION *pSession, BOOL bWithAck, BYTE *pszPassword, int nPasswordLength,
        BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_SECURITY_PAYLOAD payload;

    memset(&payload, 0, sizeof(ANSI_SECURITY_PAYLOAD));
    payload.code = ANSI_CMD_SECURITY; 
    if(pszPassword != NULL)
    {
        memcpy(payload.passwd, pszPassword, MIN(nPasswordLength, (int)sizeof(payload.passwd)));
    }

    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, sizeof(ANSI_SECURITY_PAYLOAD), bWithAck, pszAnsiFrame, nFrameLength);
}

int ansiMakeFullReadFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_FULLREAD_PAYLOAD payload;

    memset(&payload, 0, sizeof(ANSI_FULLREAD_PAYLOAD));
    payload.code = ANSI_CMD_READ; 
    payload.tableId = BigToHostShort(nTableId);

    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, sizeof(ANSI_FULLREAD_PAYLOAD), bWithAck, pszAnsiFrame, nFrameLength);
}

int ansiMakePReadIndexFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, WORD nIndex, WORD nCount, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_PREAD_INDEX_PAYLOAD payload;

    memset(&payload, 0, sizeof(ANSI_PREAD_INDEX_PAYLOAD));
    payload.code = ANSI_CMD_PREAD_INDEX; 
    payload.tableId = BigToHostShort(nTableId);
    payload.index = BigToHostShort(nIndex);
    payload.count = BigToHostShort(nCount);

    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, sizeof(ANSI_PREAD_INDEX_PAYLOAD), bWithAck, pszAnsiFrame, nFrameLength);
}

int ansiMakePReadOffsetFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, UINT nOffset, WORD nCount, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_PREAD_OFFSET_PAYLOAD payload;
    WORD24 offset;

    UintToWord24(nOffset & 0x00FFFFFF, &offset);

    memset(&payload, 0, sizeof(ANSI_PREAD_OFFSET_PAYLOAD));
    payload.code = ANSI_CMD_PREAD_OFFSET; 
    payload.tableId = BigToHostShort(nTableId);
    BigToHostWord24(offset, &payload.offset);
    payload.count = BigToHostShort(nCount);

    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, sizeof(ANSI_PREAD_OFFSET_PAYLOAD), bWithAck, pszAnsiFrame, nFrameLength);
}

int ansiMakeFullWriteFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, 
        BYTE *pTableData, WORD nDataLength, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_FULLWRITE_PAYLOAD payload;
    WORD nLen = 0;

    memset(&payload, 0, sizeof(ANSI_FULLWRITE_PAYLOAD));
    payload.code = ANSI_CMD_WRITE; 
    payload.tableId = BigToHostShort(nTableId);
    if(pTableData != NULL && nDataLength > 0)
    {
        memcpy(payload.data, pTableData, nDataLength);
        nLen = nDataLength;
    }

    payload.count = BigToHostShort(nLen);
    payload.data[nLen] = AnsiGetCheckSum(pTableData, nLen);

    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, 
            sizeof(ANSI_FULLWRITE_PAYLOAD) - sizeof(payload.data) + nLen + 1, bWithAck, pszAnsiFrame, nFrameLength);
}

int ansiMakePWriteIndexFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, WORD nIndex, 
        BYTE *pTableData, WORD nDataLength, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_PWRITE_INDEX_PAYLOAD payload;
    WORD nLen = 0;

    memset(&payload, 0, sizeof(ANSI_PWRITE_INDEX_PAYLOAD));
    payload.code = ANSI_CMD_PWRITE_INDEX; 
    payload.tableId = BigToHostShort(nTableId);
    payload.index = BigToHostShort(nIndex);
    if(pTableData != NULL && nDataLength > 0)
    {
        memcpy(payload.data, pTableData, nDataLength);
        nLen = nDataLength;
    }

    payload.count = BigToHostShort(nLen);
    payload.data[nLen] = AnsiGetCheckSum(pTableData, nLen);

    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, 
            sizeof(ANSI_PWRITE_INDEX_PAYLOAD) - sizeof(payload.data) + nLen + 1, bWithAck, pszAnsiFrame, nFrameLength);
}

int ansiMakePWriteOffsetFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, UINT nOffset, 
        BYTE *pTableData, WORD nDataLength, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_PWRITE_OFFSET_PAYLOAD payload;
    WORD24 offset;
    WORD nLen = 0;

    UintToWord24(nOffset & 0x00FFFFFF, &offset);

    memset(&payload, 0, sizeof(ANSI_PWRITE_OFFSET_PAYLOAD));
    payload.code = ANSI_CMD_PWRITE_OFFSET; 
    payload.tableId = BigToHostShort(nTableId);
    BigToHostWord24(offset, &payload.offset);
    if(pTableData != NULL && nDataLength > 0)
    {
        memcpy(payload.data, pTableData, nDataLength);
        nLen = nDataLength;
    }

    payload.count = BigToHostShort(nLen);
    payload.data[nLen] = AnsiGetCheckSum(pTableData, nLen);

    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, 
            sizeof(ANSI_PWRITE_OFFSET_PAYLOAD) - sizeof(payload.data) + nLen + 1, bWithAck, pszAnsiFrame, nFrameLength);
}

int ansiMakeLogoffFrame(ANSISESSION *pSession, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_LOGOFF_PAYLOAD payload;
    payload.code = ANSI_CMD_LOGOFF; 
    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, sizeof(ANSI_LOGOFF_PAYLOAD), TRUE, pszAnsiFrame, nFrameLength);
}

int ansiMakeTerminateFrame(ANSISESSION *pSession, BYTE **pszAnsiFrame, WORD *nFrameLength)
{
    ANSI_TERMINATE_PAYLOAD payload;
    payload.code = ANSI_CMD_TERMI; 
    return ansiMakeSingleFrame(pSession, (BYTE *)&payload, sizeof(ANSI_TERMINATE_PAYLOAD), TRUE, pszAnsiFrame, nFrameLength);
}

////////////////////////////////////////////////////////////////////////////

