#include "common.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "BCC.h"
#include "Parity.h"

#include "iecError.h"
#include "iec62056_21Functions.h"

#include "osakiFrame.h"
#include "osakiDefine.h"

#include "OsakiCheckCode.h"
#include "OsakiDataFrame.h"

int MakeOsakiAddressConfirm(BYTE *pszBuffer, BYTE *address, int addrLen)
{
    OSAKI_ADDR_REQUEST * frame;

    frame = (OSAKI_ADDR_REQUEST *)pszBuffer;
    memset(frame, 0, sizeof(OSAKI_ADDR_REQUEST));

    frame->hdr.start = IEC_21_START;
    frame->hdr.request = IEC_21_REQUEST;
    memcpy(frame->addr, address, MIN(addrLen, (int)sizeof(frame->addr)));

    frame->tail.end = IEC_21_END;
    frame->tail.cr = IEC_21_CR;
    frame->tail.lf = IEC_21_LF;
    frame->bcc = GetBCC(pszBuffer, sizeof(OSAKI_ADDR_REQUEST) - 1);

    return sizeof(OSAKI_ADDR_REQUEST);
}

int MakeOsakiRequest(BYTE *pszBuffer, BYTE *address, int addrLen, BYTE reqCode)
{
    OSAKI_REQUEST * frame;

    frame = (OSAKI_REQUEST *)pszBuffer;
    memset(frame, 0, sizeof(OSAKI_REQUEST));

    frame->hdr.start = IEC_21_START;
    frame->hdr.request = IEC_21_REQUEST;
    memcpy(frame->addr, address, MIN(addrLen, (int)sizeof(frame->addr)));
    frame->code = reqCode;

    frame->tail.end = IEC_21_END;
    frame->tail.cr = IEC_21_CR;
    frame->tail.lf = IEC_21_LF;
    frame->bcc = GetBCC(pszBuffer, sizeof(OSAKI_REQUEST) - 1);

    return sizeof(OSAKI_REQUEST);
}

/** OSAKI Address Setting.
 *
 * Address는 () 로 쌓여 있어야 한다.
 */
int MakeOsakiAddressRequest(BYTE *pszBuffer, BYTE *address, int addrLen)
{
    int nFrameLen = 0;
    int nError = iec21MakeCommand(pszBuffer, IEC_21_CMD_WRITE, '1', (BYTE *)"0010", 4,
            address, addrLen, AIMIR_PARITY_NONE, &nFrameLen);
    if(nError == IECERR_NOERROR)
    {
        return nFrameLen;
    }
    return 0;
}

/** OSAKI Meter Password Request에 알 수 없는 0x57이 붙어 있다.
 *
 * 도저히 이유를 알 수 없지만 일단 되게 해야 하니 Hack 한다.
 */
int _osakiHackPasswordRequest(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, BYTE *data, int dataLen, BYTE parity)
{
    IEC21_COMMAND_FRAME * frame;
    int frameLength = sizeof(IEC21_COMMAND_HEADER) + idAddrLen + dataLen + sizeof(IEC21_COMMAND_TAIL)+1;
    BYTE * tail;

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
    
    tail = (BYTE *)(pszBuffer + sizeof(IEC21_COMMAND_HEADER) + dataLen + idAddrLen);
    tail[0] = IEC_21_ETX;
    tail[1] = 0x57; // 왜?
    //setParity(pszBuffer, frameLength - 1, parity);
    tail[2] = GetBCC(pszBuffer, frameLength - 1);

    return frameLength;
}

/** OSAKI Meter Command Request에서 BCC 계산이 기존과 다른것 같다(확인 필요)
 *
 * 도저히 이유를 알 수 없지만 일단 되게 해야 하니 Hack 한다.
 */
int _osakiHackCommandRequest(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, BYTE *data, int dataLen)
{
    IEC21_COMMAND_FRAME * frame;
    int frameLength = sizeof(IEC21_COMMAND_HEADER) + idAddrLen + dataLen + sizeof(IEC21_COMMAND_TAIL);
    IEC21_COMMAND_TAIL * tail;

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
    tail->bcc = GetBCC(pszBuffer, frameLength - 1);

    return frameLength;
}

int MakeOsakiCommandRequest(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, BYTE *data, int dataLen)
{
    return _osakiHackCommandRequest(pszBuffer, cmd, cmdType, idAddr, idAddrLen, data, dataLen);
}

/** OSAKI Password Setting.
 *
 * Password는 () 로 쌓여 있어야 한다.
 */
int MakeOsakiPasswordConfirm(BYTE *pszBuffer, BYTE *passwd, int passwdLen)
{
    return _osakiHackPasswordRequest(pszBuffer, IEC_21_CMD_PASSWORD, '1', (BYTE *)NULL, 0,
            passwd, passwdLen, AIMIR_PARITY_NONE);
}
