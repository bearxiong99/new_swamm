#ifndef __OSAKI_DATA_FRAME_H__
#define __OSAKI_DATA_FRAME_H__

#include "osakiFrame.h"

int MakeOsakiRequest(BYTE *pszBuffer, BYTE *address, int addrLen, BYTE reqCode);
int MakeOsakiAddressConfirm(BYTE *pszBuffer, BYTE * address, int addrLen);
int MakeOsakiAddressRequest(BYTE *pszBuffer, BYTE *address, int addrLen);
int MakeOsakiPasswordConfirm(BYTE *pszBuffer, BYTE *passwd, int passwdLen);
int MakeOsakiCommandRequest(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, BYTE *data, int dataLen);

#endif	// __OSAKI_DATA_FRAME_H__
