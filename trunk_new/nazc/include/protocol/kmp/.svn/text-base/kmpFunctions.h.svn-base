////////////////////////////////////////////////////////////////////////////////
//
// KMP Protocol Library
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __KMP_FUNCTIONS_H__
#define __KMP_FUNCTIONS_H__

#define PROTOCOL_KMP_INIT      kmpInit()

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
const char * kmpGetKamstrupUnit(BYTE unit);
BYTE kmpGetRegValue(BYTE * szStream, double * nValue, char * valueStr, bool bDiff=false);

const char * kmpGetCutOffState(BYTE cutOff);
const char *kmpGetCutOffFeedback(BYTE cutOff, BYTE feedback, BYTE * szBuffer);

const char *kmpGetEventStatus(BYTE eventIdx, BYTE status, BYTE mask);
const char *kmpGetLogType(BYTE logType);
const char *kmpGetLogInfo(BYTE logInfo, BYTE mask);
const char *kmpGetLogInfo(WORD logInfo, WORD mask);
const char *kmpGetVoltageQuality(BYTE quality, BYTE mask);
const char *kmpGetRTCStatus(BYTE status, BYTE mask);

BYTE * kmpGetTimeStream(BYTE *szBuffer, int nOffset);
BYTE * kmpGetTimeDateStream(BYTE *szBuffer);

BOOL kmpIsDlmsMeter(BYTE mainType, BYTE subType);

#endif	// __KMP_FUNTIONS_H__
