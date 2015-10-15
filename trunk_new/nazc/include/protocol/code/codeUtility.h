////////////////////////////////////////////////////////////////////////////////
//
// Protocol Code Utility
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __PROTOCOL_CODE_UTILITY_H__
#define __PROTOCOL_CODE_UTILITY_H__

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

const char * GetManufacturerFullString(char * szMCode);
const char * GetManufacturerString(char * szMCode);
BYTE GetManufacturerVendorCode(char * szMCode);
const char *ConvManufacturerCode(const char *mcode, char *szManufact);

// AiMiR Code
const char * GetUnitStr(BYTE nUnit);
const char * GetChannelStr(BYTE nChType);

#ifdef __cplusplus
}
#endif

#endif	// __PROTOCOL_CODE_UTILITY_H__
