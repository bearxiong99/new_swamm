#ifndef __KMP_UTILS_H__
#define __KMP_UTILS_H__

#include "typedef.h"

BYTE * GetTimeStream(BYTE *szBuffer, int nOffset);
BYTE * GetTimeDateStream(BYTE *szBuffer);

const char *KmpGetErrorMessage(int nError);
const char *GetCidString(BYTE cid);
char * SwMajorVersion(BYTE major, char *szBuffer);
char * SwMinorVersion(BYTE minor, char *szBuffer);
const char * GetMeterType(BYTE mainType, BYTE subType);
const char * GetKamstrupUnit(BYTE unit);
const char * GetRegisterString(WORD regId);
BYTE GetRegValue(BYTE * szStream, double * nValue, char * valueStr);

const char * GetCutOffState(BYTE cutOff);
const char *GetCutOffFeedback(BYTE cutOff, BYTE feedback, BYTE * szBuffer);

const char *GetEventStatus0(BYTE status, BYTE mask);
const char *GetEventStatus1(BYTE status, BYTE mask);
const char *GetEventStatus2(BYTE status, BYTE mask);
const char *GetEventStatus3(BYTE status, BYTE mask);
const char *GetVoltageQuality(BYTE quality, BYTE mask);
const char *GetRTCStatus(BYTE status, BYTE mask);

const char *GetLogType(BYTE logType);
const char *GetLogInfo(BYTE loginfo, BYTE mask);

BOOL IsDlmsMeter(BYTE mainType, BYTE subType);

#endif	// __KMP_UTILS_H__
