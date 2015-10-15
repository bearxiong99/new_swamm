#ifndef __MBUS_DATA_FRAME_H__
#define __MBUS_DATA_FRAME_H__

#include "mbusType.h"
#include "mbusFrame.h"

int     MakeMbusTail(BYTE *pszBuffer, BYTE len);
int     MakeMbusShortHeader(MBUS_SHORT_HEADER *pHeader, BYTE ctrl, BYTE addr);
int     MakeMbusLongHeader(MBUS_LONG_HEADER *pHeader, BYTE ctrl, BYTE addr, BYTE ci, BYTE dataLen);

int     MakeMbusShortFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr);
int     MakeMbusLongFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE ci, BYTE *userData, BYTE dataLen);
int     MakeMbusControlFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE ci);

#endif	// __MBUS_DATA_FRAME_H__
