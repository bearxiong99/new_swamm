#include "common.h"
#include "AnsiDataFrame.h"
#include "AnsiCrcCheck.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "ansiDefine.h"
#include "ansiError.h"

int _makeAnsiAckNak(ANSISESSION * pSession, BYTE * message, BYTE code, WORD *nLength)
{
    if(message == NULL) 
    {
        if(nLength) *nLength = 0;
        return ANSIERR_INVALID_BUFFER;
    }

    message[0] = code;

    if(nLength) *nLength = 1;
    return ANSIERR_NOERROR;
}

/** ANSI_ACK message 생성.
  *
  */ 
int MakeAnsiAck(ANSISESSION *pSession, BYTE * message, WORD *nLength)
{
    return _makeAnsiAckNak(pSession, message, ANSI_ACK, nLength);
}

/** ANSI_NAK message 생성.
  *
  */ 
int MakeAnsiNak(ANSISESSION *pSession, BYTE * message, WORD *nLength)
{
    return _makeAnsiAckNak(pSession, message, ANSI_NAK, nLength);
}

/** ANSI Message Header 생성
  *
  */
int MakeAnsiHeader(ANSISESSION *pSession, ANSI_HEADER *pHeader, BYTE ctrl, BYTE nTotPacket, BYTE nIndex, WORD payloadLen, WORD *nHeaderLength) 
{
    BYTE 		nCtrl;

    if(pHeader == NULL) 
    {
        if(nHeaderLength) *nHeaderLength = 0;
        return ANSIERR_INVALID_BUFFER;
    }

    memset(pHeader, 0, sizeof(ANSI_HEADER));

    /** Control
     *
     * Bit 7 : If true -> Multiple Packet Transmission
     * Bit 6 : If true -> First packet of a multiple
     * Bit 5 : Toggle
     */

    /** nSendToggle은 실제로 Send가 성공해야 Toggle 된다.
      * nSendSeq는 실제로 Send가 성공해야 증가한다.
      */
    nCtrl = pSession->nSendToggle;
    nCtrl |= ctrl & (ANSI_CTRL_TOGGLE ^ 0xFF);

    pHeader->stp = ANSI_STP;
    pHeader->ctrl = nCtrl;
    pHeader->seq = nTotPacket - nIndex - 1;    // Multipacket일 경우 전체 Packet 수에서 현재 보낼 Index를 뺀 수
    pHeader->length = HostToBigShort(payloadLen);

    if(nHeaderLength) *nHeaderLength = sizeof(ANSI_HEADER);

    return ANSIERR_NOERROR;
}

void MakeAnsiTail(ANSISESSION *pSession, ANSI_FRAME *pFrame)
{
	ANSI_TAIL *pTail;
    BYTE * pBuff = (BYTE *)pFrame;

    pTail = (ANSI_TAIL *)(pBuff + sizeof(ANSI_HEADER) + BigToHostShort(pFrame->hdr.length));
	pTail->crc		= AnsiGetCrc(pBuff, sizeof(ANSI_HEADER) + BigToHostShort(pFrame->hdr.length));
    //XDEBUG("MakeAnsiTail : %04X %d %d \r\n", pTail->crc, sizeof(ANSI_HEADER), BigToHostShort(pFrame->hdr.length));
}

/** Single Frame 전용
  */
int MakeAnsiSingleFrame(ANSISESSION *pSession, BYTE *pszBuffer, BYTE *pszPayload, WORD nPayloadLen, WORD *frameLength)
{
    WORD        nHLen=0;
    int         nError;

	if((nError=MakeAnsiHeader(pSession, (ANSI_HEADER *)pszBuffer, 0x00, 1, 0, nPayloadLen, &nHLen)) != ANSIERR_NOERROR) {
        return ANSIERR_FRAME_ERROR;
    }

    if(nPayloadLen > 0) {
	    memcpy(&pszBuffer[nHLen], pszPayload, nPayloadLen);
    }

	MakeAnsiTail(pSession, (ANSI_FRAME *)pszBuffer);

    if(frameLength) *frameLength = nHLen + nPayloadLen + sizeof(ANSI_TAIL);

	return nError;
}
