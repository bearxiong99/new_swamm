#ifndef __MBUS_CHECKSUM_H__
#define __MBUS_CHECKSUM_H__

#include "typedef.h"

BYTE GetMbusChecksum(BYTE *pszBuffer, BYTE len);
BOOL VerifyMbusChecksum(BYTE *pszBuffer, BYTE len, BYTE checksum);

#endif	// __MBUS_CHECKSUM_H__
