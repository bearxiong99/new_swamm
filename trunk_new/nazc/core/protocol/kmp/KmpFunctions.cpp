
#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "KmpDataFrame.h"
#include "KmpUtils.h"

#include "kmpFunctions.h"

time_t   KMP_2000_TIME_T = 0;

int kmpInit(void)
{
    struct tm tms;

    memset(&tms, 0, sizeof(struct tm));

    tms.tm_year = 2000 - 1900;
    tms.tm_mday = 1;

    KMP_2000_TIME_T = mktime(&tms);

    return 0;
}

int kmpMakeFrame(BYTE *pszBuffer, BYTE cid, BYTE * userData, BYTE dataLen, BYTE *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return KMPERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeKmpFrame(pszBuffer, cid, userData, dataLen);
    if(nSize <= 0)
        return KMPERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return KMPERR_NOERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *kmpGetErrorMessage(int nError)
{
    return KmpGetErrorMessage(nError);
}

const char *kmpGetCidString(BYTE cid)
{
    return GetCidString(cid);
}

char * kmpSwMajorVersion(BYTE major, char *szBuffer)
{
    return SwMajorVersion(major, szBuffer);
}

char * kmpSwMinorVersion(BYTE minor, char *szBuffer)
{
    return SwMinorVersion(minor, szBuffer);
}

const char * kmpGetMeterType(BYTE mainType, BYTE subType)
{
    return GetMeterType(mainType, subType);
}

BOOL kmpIsDlmsMeter(BYTE mainType, BYTE subType)
{
    return IsDlmsMeter(mainType, subType);
}

/** Get Register값을 Parsing 해 준다.
  * 
  * @param szStream RegFormat+RegValue 이 전달되어야 한다 (RID 제외)
  * @param nValue NULL이 아닐 경우 double형 value 전달
  * @param valuestr NULL이 아닐 경우 출력 가능한 포멧으로 전달
  * @param bDeff diff 값인지 여부
  *
  * @return Value의 Length
  */
BYTE kmpGetRegValue(BYTE * szStream, double * nValue, char * valueStr, bool bDiff)
{
    if(!szStream) return 0;

    return GetRegValue(szStream, nValue, valueStr, bDiff);
}

const char * kmpGetRegisterString(WORD regId)
{
    return GetRegisterString(regId);
}

const char * kmpGetKamstrupUnit(BYTE unit)
{
    return GetKamstrupUnit(unit);
}

const char * kmpGetCutOffState(BYTE cutOff)
{
    return GetCutOffState(cutOff);
}

const char *kmpGetCutOffFeedback(BYTE cutOff, BYTE feedback, BYTE * szBuffer)
{
    if(!szBuffer) return "";

    return GetCutOffFeedback(cutOff, feedback, szBuffer);
}

/** EventStatus 에 대한 출력 가능한 문자열을 구한다.
  *
  * @param eventIdx Event Status Number (0~3)
  * @param status Event Status
  * @param mask Event Mask
  * 
  * @return status와 mask를 이용해서 해당 Event가 발생했을 경우 문자열을 리턴한다
  *     만약 발생하지 않았다면 NULL을 Return 한다.
  */
const char *kmpGetEventStatus(BYTE eventIdx, BYTE status, BYTE mask)
{
    switch(eventIdx)
    {
        case 0: return GetEventStatus0(status, mask);
        case 1: return GetEventStatus1(status, mask);
        case 2: return GetEventStatus2(status, mask);
        case 3: return GetEventStatus3(status, mask);
    }

    return NULL;
}

/** Log Type에 대한 출력 가능한 문자열을 구한다.
  *
  * @param logType log type
  *
  * @return Log Type 문자열
  */
const char *kmpGetLogType(BYTE logType)
{
    return GetLogType(logType);
}

/** Log Info에 대한 출력 가능한 문자열을 구한다.
  *
  * @param logInfo log info
  * @param mask Log Info Mask
  *
  * @return Log Info 문자열 만약 설정되지 않았다면 NULL을 Return 한다.
  */
const char *kmpGetLogInfo(BYTE logInfo, BYTE mask)
{
    return GetLogInfo(logInfo, mask);
}

/** Log Info에 대한 출력 가능한 문자열을 구한다 (OMNIPower).
  *
  * @param logInfo log info
  * @param mask Log Info Mask
  *
  * @return Log Info 문자열 만약 설정되지 않았다면 NULL을 Return 한다.
  */
const char *kmpGetLogInfo(WORD logInfo, WORD mask)
{
    return GetLogInfo(logInfo, mask);
}

/** KMP Time Stream을 만들어서 Return 해 준다 (GetLogTimePresent에서 사용).
  *
  * @param szBuffer NULL이 아니여야 하며 8Byte 이상 되어야 한다.
  * @param nOffset 시간 Offset(양의 정수이면 그만큼 과거로 계산한다. 날 기준)
  *
  * @return szBuffer를 Return 한다.
  */
BYTE * kmpGetTimeStream(BYTE *szBuffer, int nOffset)
{
    if(!szBuffer) return NULL;
    return GetTimeStream(szBuffer, nOffset);
}

/** KMP Time Date Stream을 만들어서 Return 해 준다 (SetClock에서 사용).
  *
  * @param szBuffer NULL이 아니여야 하며 8Byte 이상 되어야 한다.
  *
  * @return szBuffer를 Return 한다.
  */
BYTE * kmpGetTimeDateStream(BYTE *szBuffer)
{
    if(!szBuffer) return NULL;
    return GetTimeDateStream(szBuffer);
}

/** Voltage quality 에 대한 출력 가능한 문자열을 구한다.
  *
  * @param quality voltage quality
  * @param mask Mask
  * 
  * @return quality와 mask를 이용해서 해당 Event가 발생했을 경우 문자열을 리턴한다
  *     만약 발생하지 않았다면 NULL을 Return 한다.
  */
const char *kmpGetVoltageQuality(BYTE quality, BYTE mask)
{
    return GetVoltageQuality(quality, mask);
}

/** RTC Event 에 대한 출력 가능한 문자열을 구한다.
  *
  * @param status RTC event status
  * @param mask Mask
  * 
  * @return status와 mask를 이용해서 해당 Event가 발생했을 경우 문자열을 리턴한다
  *     만약 발생하지 않았다면 NULL을 Return 한다.
  */
const char *kmpGetRTCStatus(BYTE status, BYTE mask)
{
    return GetRTCStatus(status, mask);
}

