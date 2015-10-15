#ifndef __IEC62056_21_DATA_FRAME_H__
#define __IEC62056_21_DATA_FRAME_H__

#include "iec62056_21Frame.h"

int MakeIec21Request(BYTE *pszBuffer, BYTE * address, int addrLen, BYTE parity);
int MakeIec21AckOption(BYTE *pszBuffer, BYTE protocol, BYTE mode, BYTE baud, BYTE parity);
int MakeIec21Ack(BYTE *pszBuffer, BYTE parity);
int MakeIec21Nak(BYTE *pszBuffer, BYTE parity);
int MakeIec21Data(BYTE *pszBuffer, BYTE *data, int dataLen, BYTE parity);
int MakeIec21Command(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, 
        BYTE *data, int dataLen, BYTE parity);
int MakeIec21Break(BYTE *pszBuffer, BYTE parity);

#endif	// __IEC62056_21_DATA_FRAME_H__
