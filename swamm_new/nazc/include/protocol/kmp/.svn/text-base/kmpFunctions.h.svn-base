////////////////////////////////////////////////////////////////////////////////
//
// KMP Protocol Library
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __KMP_FUNCTIONS_H__
#define __KMP_FUNCTIONS_H__

#define PROTOCOL_KMP_INIT      kmpInit()

#ifdef __cplusplus
extern "C" {
#endif

#include "kmpType.h"

// Initialize
int kmpInit(void);

// KMP functions
int kmpMakeFrame(BYTE *pszBuffer, BYTE cid, BYTE *userData, BYTE dataLen, BYTE *nFrameLength);

// Utility functions
const char *kmpGetErrorMessage(int nError);
const char *kmpGetCidString(BYTE cid);

char * kmpSwMajorVersion(BYTE major, char *szBuffer);
char * kmpSwMinorVersion(BYTE minor, char *szBuffer);
const char * kmpGetMeterType(BYTE mainType, BYTE subType);
const char * kmpGetRegisterString(WORD regId);
BYTE kmpGetRegValue(BYTE * szStream, double * nValue, char * valueStr);

const char * kmpGetCutOffState(BYTE cutOff);
const char *kmpGetCutOffFeedback(BYTE cutOff, BYTE feedback, BYTE * szBuffer);

const char *kmpGetEventStatus(BYTE eventIdx, BYTE status, BYTE mask);
const char *kmpGetLogType(BYTE logType);
const char *kmpGetLogInfo(BYTE logInfo, BYTE mask);
const char *kmpGetVoltageQuality(BYTE quality, BYTE mask);
const char *kmpGetRTCStatus(BYTE status, BYTE mask);

BYTE * kmpGetTimeStream(BYTE *szBuffer, int nOffset);
BYTE * kmpGetTimeDateStream(BYTE *szBuffer);

BOOL kmpIsDlmsMeter(BYTE mainType, BYTE subType);

#ifdef __cplusplus
}
#endif

#endif	// __KMP_FUNTIONS_H__
