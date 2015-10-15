////////////////////////////////////////////////////////////////////////////////
//
// DLMS/COSEM Client Library
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __DLMS_UTILS_H__
#define __DLMS_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "dlmsType.h"

unsigned short dlmsOrderByShort(unsigned short value);
unsigned int dlmsOrderByInt(unsigned int value);
unsigned long long int dlmsOrderByInt64(unsigned long long int value);

BYTE dlmsGetTagSize(BYTE *pTag);

const char *dlmsGetErrorMessage(int nError);
const char * dlmsGetResponseCode(BYTE code);
const char *dlmsGetHdlcControlType(BYTE nCtrl);
const char *dlmsGetDataType(BYTE nType);
const char *dlmsGetMethodType(BYTE nType);
const char *dlmsGetFlowType(BYTE nFlow);
const char *dlmsGetResType(BYTE nType);
const char *dlmsGetFrameType(BYTE nType);
const char *dlmsGetInterfaceClass(BYTE cls);

int dlmsObisToString(OBISCODE *obis, char *pszString);
int dlmsStringToObis(const char *pszString, OBISCODE *obis);

void dlmsDumpValue(BYTE *pszData, int nLength);
void dlmsDumpConformanceValue(BYTE associationType, UINT conformance);

#ifdef __cplusplus
}
#endif

#endif	// __DLMS_FUNTIONS_H__
