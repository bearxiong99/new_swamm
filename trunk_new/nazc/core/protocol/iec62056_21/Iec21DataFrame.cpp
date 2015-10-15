#include "common.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "Parity.h"
#include "BCC.h"

#include "iec62056_21Frame.h"
#include "iec62056_21Define.h"

#include "Iec21Code.h"
#include "Iec21DataFrame.h"

void setParity(BYTE *pszBuffer, int nLength, BYTE parity)
{
    switch(parity) {
        case AIMIR_PARITY_EVEN:
            SetEvenParity(pszBuffer, nLength);
            break;
        /** ODD Parity는 구현하지 않는다 */
    }
}

int MakeIec21Request(BYTE *pszBuffer, BYTE *address, int addrLen, BYTE parity)
{
    IEC21_REQUEST_FRAME * frame;
    IEC21_REQUEST_TAIL * tail;
    int addrLength = MIN(addrLen, 32);
    int frameLength = sizeof(IEC21_REQUEST_HEADER) + addrLength + sizeof(IEC21_REQUEST_TAIL);

    if(pszBuffer == NULL || addrLength < 0) return 0;

    frame = (IEC21_REQUEST_FRAME *)pszBuffer;
    memset(frame, 0, frameLength);

    frame->hdr.start = IEC_21_START;
    frame->hdr.request = IEC_21_REQUEST;
    if(addrLength > 0) {
        if(address == NULL) return 0;
        memcpy(frame->addr, address, addrLength);
    }
    
    tail = (IEC21_REQUEST_TAIL *)(pszBuffer + sizeof(IEC21_REQUEST_HEADER) + addrLength);
    
    tail->end = IEC_21_END;
    tail->cr = IEC_21_CR;
    tail->lf = IEC_21_LF;

    setParity(pszBuffer, frameLength, parity);

    return frameLength;
}

int MakeIec21AckOption(BYTE *pszBuffer, BYTE protocol, BYTE mode, BYTE baud, BYTE parity)
{
    IEC21_ACK_OPTION_SELECT_FRAME * frame;
    int frameLength = sizeof(IEC21_ACK_OPTION_SELECT_FRAME);

    if(pszBuffer == NULL) return 0;

    frame = (IEC21_ACK_OPTION_SELECT_FRAME *)pszBuffer;
    memset(frame, 0, frameLength);

    frame->ack = IEC_21_ACK;
    frame->protocol = protocol;
    frame->baud = baud;
    frame->mode = mode;
    frame->cr = IEC_21_CR;
    frame->lf = IEC_21_LF;

    setParity(pszBuffer, frameLength, parity);

    return frameLength;
}

int MakeIec21Ack(BYTE *pszBuffer, BYTE parity)
{
    int frameLength = 1;
    if(pszBuffer == NULL) return 0;

    pszBuffer[0] = IEC_21_ACK;
    setParity(pszBuffer, frameLength, parity);
    return frameLength;
}

int MakeIec21Nak(BYTE *pszBuffer, BYTE parity)
{
    int frameLength = 1;
    if(pszBuffer == NULL) return 0;

    pszBuffer[0] = IEC_21_NAK;
    setParity(pszBuffer, frameLength, parity);
    return frameLength;
}

int MakeIec21Data(BYTE *pszBuffer, BYTE *data, int dataLen, BYTE parity)
{
    IEC21_DATA_FRAME * frame;
    IEC21_DATA_NP_TAIL * tail;
    int frameLength = sizeof(IEC21_DATA_HEADER) + dataLen + sizeof(IEC21_DATA_NP_TAIL);

    if(pszBuffer == NULL || dataLen < 0) return 0;

    frame = (IEC21_DATA_FRAME *)pszBuffer;
    memset(frame, 0, frameLength);

    frame->hdr.stx = IEC_21_STX;
    if(dataLen > 0) {
        if(data == NULL) return 0;
        memcpy(frame->body, data, dataLen);
    }
    
    tail = (IEC21_DATA_NP_TAIL *)(pszBuffer + sizeof(IEC21_DATA_HEADER) + dataLen);
    tail->end = IEC_21_END;
    tail->cr = IEC_21_CR;
    tail->lf = IEC_21_LF;
    tail->etx = IEC_21_END;
    setParity(pszBuffer, frameLength - 1, parity);
    tail->bcc = GetBCC(pszBuffer+1, frameLength - 2);

    return frameLength;
}

int MakeIec21Command(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, BYTE *data, int dataLen, BYTE parity)
{
    IEC21_COMMAND_FRAME * frame;
    IEC21_COMMAND_TAIL * tail;
    int frameLength = sizeof(IEC21_COMMAND_HEADER) + idAddrLen + dataLen + sizeof(IEC21_COMMAND_TAIL);

    if(pszBuffer == NULL || dataLen < 0 || idAddrLen < 0) return 0;

    frame = (IEC21_COMMAND_FRAME *)pszBuffer;
    memset(frame, 0, frameLength);

    frame->hdr.soh = IEC_21_SOH;
    frame->hdr.cmd = cmd;
    frame->hdr.cmdtype = cmdType;
    frame->hdr.stx = IEC_21_STX;
    if(idAddrLen > 0) {
        if(idAddr == NULL) return 0;
        memcpy(frame->body, idAddr, idAddrLen);
    }
    if(dataLen > 0) {
        if(data == NULL) return 0;
        memcpy(frame->body + idAddrLen, data, dataLen);
    }
    
    tail = (IEC21_COMMAND_TAIL *)(pszBuffer + sizeof(IEC21_COMMAND_HEADER) + dataLen + idAddrLen);
    tail->etx = IEC_21_ETX;
    setParity(pszBuffer, frameLength - 1, parity);
    tail->bcc = GetBCC(pszBuffer+1, frameLength - 2);

    return frameLength;
}

int MakeIec21Break(BYTE *pszBuffer, BYTE parity)
{
    IEC21_BREAK_FRAME * frame;
    int frameLength = sizeof(IEC21_BREAK_FRAME);

    if(pszBuffer == NULL) return 0;

    frame = (IEC21_BREAK_FRAME *)pszBuffer;
    memset(frame, 0, frameLength);

    frame->soh = IEC_21_SOH;
    frame->cmd = IEC_21_CMD_BREAK;
    frame->cmdtype = '0';
    frame->etx = IEC_21_ETX;
    setParity(pszBuffer, frameLength - 1, parity);
    frame->bcc = GetBCC(pszBuffer+1, frameLength - 2);

    return frameLength;
}



