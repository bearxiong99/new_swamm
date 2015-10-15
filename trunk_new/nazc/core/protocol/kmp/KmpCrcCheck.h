#ifndef __KMP_CRCCHECK_H__
#define __KMP_CRCCHECK_H__

#include "typedef.h"

WORD KmpGetCrc(BYTE *pszBuffer, int nLength);
BOOL KmpVerifyCrc(BYTE *pszBuffer, int nLength);

#endif	// __KMP_CRCCHECK_H__
