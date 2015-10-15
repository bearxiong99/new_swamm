////////////////////////////////////////////////////////////////////////////////
//
// ANSI C12.18 Client Library
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __ANSI_FUNCTIONS_H__
#define __ANSI_FUNCTIONS_H__

#define PROTOCOL_ANSI_INIT      ansiInit()

#ifdef __cplusplus
extern "C" {
#endif

#include "ansiType.h"
#include "ansiFrame.h"
#include "ansiSessions.h"

// Initialize
int ansiInit(void);

// ANSI functions
int ansiMakeSingleFrame(ANSISESSION *pSession, BYTE *pszPayload, WORD nPayloadLen, BOOL bWithAck, BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakeAckFrame(ANSISESSION *pSession, BYTE *pszAnsiFrame);

// ANSI Frames
int ansiMakeIdentificationFrame(ANSISESSION *pSession, BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakeNegotiateFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nPacketSize, BYTE nNbrPacket, BYTE nBaudRate,
                BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakeLogonFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nUserId, BYTE *pszUser, int nUserLength,
        BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakeSecurityFrame(ANSISESSION *pSession, BOOL bWithAck, BYTE *pszPassword, int nPasswordLength,
        BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakeLogoffFrame(ANSISESSION *pSession, BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakeTerminateFrame(ANSISESSION *pSession, BYTE **pszAnsiFrame, WORD *nFrameLength);

int ansiMakeFullReadFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakePReadIndexFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, WORD nIndex, WORD nCount, BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakePReadOffsetFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, UINT nOffset, WORD nCount, BYTE **pszAnsiFrame, WORD *nFrameLength);

int ansiMakeFullWriteFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, 
        BYTE *pTableData, WORD nDataLength, BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakePWriteIndexFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, WORD nIndex, 
        BYTE *pTableData, WORD nDataLength, BYTE **pszAnsiFrame, WORD *nFrameLength);
int ansiMakePWriteOffsetFrame(ANSISESSION *pSession, BOOL bWithAck, WORD nTableId, UINT nOffset, 
        BYTE *pTableData, WORD nDataLength, BYTE **pszAnsiFrame, WORD *nFrameLength);

// Session
ANSISESSION * ansiNewSession(void);
void ansiDeleteSession(ANSISESSION * session);

#ifdef __cplusplus
}
#endif

#endif	// __ANSI_FUNTIONS_H__
