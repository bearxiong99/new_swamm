////////////////////////////////////////////////////////////////////////////////
//
// MBUS Protocol Library
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MBUS_FUNCTIONS_H__
#define __MBUS_FUNCTIONS_H__

#define PROTOCOL_MBUS_INIT      mbusInit()

#ifdef __cplusplus
extern "C" {
#endif

#include "mbusType.h"

// Initialize
int mbusInit(void);

// MBUS functions
int mbusMakeShortFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE *nFrameLength);
int mbusMakeControlFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE ci, BYTE *nFrameLength);
int mbusMakeLongFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE ci, BYTE *userData, BYTE dataLen, BYTE *nFrameLength);

// Utility functions
const char *mbusGetErrorMessage(int nError);
const char *mbusGetMedium(BYTE nType);
WORD mbusGetDIFVIFValue(BYTE dif, BYTE edif, BYTE vif, BYTE *value, UNIONVALUE *uv, BYTE *buffer);

#ifdef __cplusplus
}
#endif

#endif	// __MBUS_FUNTIONS_H__
