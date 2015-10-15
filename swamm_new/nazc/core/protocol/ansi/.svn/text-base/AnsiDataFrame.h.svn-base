#ifndef __ANSI_DATA_FRAME_H__
#define __ANSI_DATA_FRAME_H__

#include "typedef.h"

#include "ansiFrame.h"
#include "AnsiSession.h"

int     MakeAnsiAck(ANSISESSION *pSession, BYTE * message, WORD *nLength);
int     MakeAnsiNak(ANSISESSION *pSession, BYTE * message, WORD *nLength);

int     MakeAnsiHeader(ANSISESSION *pSession, ANSI_HEADER *pHeader, BYTE ctrl, BYTE nTotPacket, BYTE nIndex, WORD payloadLen, WORD *nHeaderLength);
void    MakeAnsiTail(ANSISESSION *pSession, ANSI_FRAME *pFrame);

int     MakeAnsiSingleFrame(ANSISESSION *pSession, BYTE *pszBuffer, BYTE *pszPayload, WORD nPayloadLen, WORD *frameLength);

#endif	// __ANSI_DATA_FRAME_H__
