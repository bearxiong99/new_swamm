#include <math.h>

#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "mcudef.h"

#include "kmpDefine.h"

#include "KmpUtils.h"

extern time_t   KMP_2000_TIME_T;

static const char *_week[] = {"Mon ", "Tue ", "Wed ", "Thu ", "Fri ", "Sat ", "Sun " };

/** KMP Time Stream을 만들어서 Return 해 준다 (GetLogTimePresent에서 사용).
  *
  * @param szBuffer NULL이 아니여야 하며 8Byte 이상 되어야 한다.
  * @param nOffset 시간 Offset(양의 정수이면 그만큼 과거로 계산한다. 날 기준)
  *
  * @return szBuffer를 Return 한다.
  *     YY MM DD hh mm ss 00 00
  */
BYTE * GetTimeStream(BYTE *szBuffer, int nOffset)
{
    time_t now;
    struct tm *when;

    time(&now);
    if(nOffset > 0)
    {
        now -= (nOffset * 60 * 60 * 24);
    }
    when = localtime(&now);

    memset(szBuffer, 0, 8);

    szBuffer[0] = (when->tm_year) % 100 & 0xFF;     // 1900은 빼도 된다 
    szBuffer[1] = (when->tm_mon + 1) & 0xFF;
    szBuffer[2] = when->tm_mday & 0xFF;
    szBuffer[3] = when->tm_hour & 0xFF;
    szBuffer[4] = when->tm_min & 0xFF;
    szBuffer[5] = when->tm_sec & 0xFF;

    return szBuffer;
}

/** KMP Time Date Stream을 만들어서 Return 해 준다 (SetClock에서 사용).
  *
  * @param szBuffer NULL이 아니여야 하며 8Byte 이상 되어야 한다.
  *
  * @return szBuffer를 Return 한다.
  */
BYTE * GetTimeDateStream(BYTE *szBuffer)
{
    time_t now;
    struct tm *when;
    UINT   uv=0;

    time(&now);
    when = localtime(&now);

    memset(szBuffer, 0, 8);

    uv = 0;
    uv += (when->tm_year % 100) & 0xFF;     uv *= 100;
    uv += (when->tm_mon + 1) & 0xFF;        uv *= 100;
    uv += when->tm_mday & 0xFF;             uv = BigToHostInt(uv);
    memcpy(&szBuffer[0], &uv, sizeof(INT));

    uv = 0;
    uv += when->tm_hour & 0xFF;             uv *= 100;
    uv += when->tm_min & 0xFF;              uv *= 100;
    uv += when->tm_sec & 0xFF;              uv = BigToHostInt(uv);
    memcpy(&szBuffer[4], &uv, sizeof(INT));

    return szBuffer;
}

const char *KmpGetErrorMessage(int nError)
{
    switch(nError) {
        case KMPERR_NOERROR :               return "OK";
        case KMPERR_INVALID_PARAM :         return "Invalid param";
        case KMPERR_INVALID_SESSION :       return "Invalid session";
        case KMPERR_INVALID_HANDLE :        return "Invalid handle";
        case KMPERR_INVALID_ADDRESS :       return "Invalid address";
        case KMPERR_INVALID_LENGTH :        return "Invalid length";
        case KMPERR_INVALID_BUFFER :        return "Invalid buffer";
        case KMPERR_CANNOT_CONNECT :        return "Cannot connect";
        case KMPERR_REPLY_TIMEOUT :         return "Reply timeout";
        case KMPERR_NOT_INITIALIZED :       return "Not initialized";
        case KMPERR_SYSTEM_ERROR :          return "System error";
        case KMPERR_MEMORY_ERROR :          return "Memory error";
    }
    return UNKNOWN_STR;
}

const char *GetCidString(BYTE cid)
{
    switch(cid) {
        case KMP_CID_GET_TYPE:  return "GetType";
        case KMP_CID_RESET:  return "Reset";
        case KMP_CID_SET_CLOCK:  return "SetClock";
        case KMP_CID_GET_REGISTER:  return "GetRegister";
        case KMP_CID_PUT_REGISTER:  return "PutRegister";
        case KMP_CID_PUT_REGISTER_EXTENDED:  return "PutRegisterExtended";
        case KMP_CID_RESET_NO_BACKUP:  return "ResetNoBackup";
        case KMP_CID_SET_DISP_YES:  return "SetDispYES";
        case KMP_CID_DEB_STOP:  return "DebStop";
        case KMP_CID_GET_POWER_LIMIT_STATUS:  return "GetPowerLImitStatus";
        case KMP_CID_SET_CUT_OFF_STATE:  return "SetCutOffState";
        case KMP_CID_GET_CUT_OFF_STATE:  return "GetCutOffState";
        case KMP_CID_READ_EVENT_STATUS:  return "ReadEventStatus";
        case KMP_CID_CLR_EVENT_STATUS:  return "ClrEventStatus";
        case KMP_CID_EVENT_STATUS_ACK:  return "EventStatusAck";    // omni
        case KMP_CID_GET_LOG_TIME_PRESENT:  return "GetLogTimePresent";
        case KMP_CID_GET_LOG_LAST_PRESENT:  return "GetLogLastPresent";
        case KMP_CID_GET_LOG_ID_PRESENT:  return "GetLogIDPresent";
        case KMP_CID_GET_LOG_TIME_PAST:  return "GetLogTimePast";
        case KMP_CID_SET_VOLT_QUAL_CONFIG:  return "SetVoltQualConfig";
        case KMP_CID_GET_VOLT_QUAL_CONFIG:  return "GetVoltQualConfig";
        case KMP_CID_SET_LOAD_PROFILE_CONFIG_REGISTER:  return "SetLoadProfileConfigRegister";
        case KMP_CID_GET_LOAD_PROFILE_CONFIG:  return "GetLoadProfileConfig";
        case KMP_CID_GET_COMPACT_LOG_TIME_PRESENT:  return "GetCompactLogTimePresent";
        case KMP_CID_GET_COMPACT_LOG_LAST_PRESENT:  return "GetCompactLogLastPresent";
        case KMP_CID_GET_COMPACT_LOG_ID_PRESENT:  return "GetCompactLogIdPresent";
        case KMP_CID_GET_COMPACT_LOG_TIME_PAST:  return "GetCompactLogTimePast";
        case KMP_CID_PUT_LOGGER_CONFIGURATION:  return "PutLoggerConfiguration";
        case KMP_CID_GET_LOGGER_CONFIGURATION:  return "GetLoggerConfiguration";
        case KMP_CID_PUT_BLOCK:  return "PutBlock"; // omni
        case KMP_CID_GET_BLOCK:  return "GetBlock"; // omni
        case KMP_CID_LOGGER_READ_OUT: return "LoggerReadOut";   // omni
    }
    return UNKNOWN_STR;
}

char * SwMajorVersion(BYTE major, char *szBuffer)
{
    memset(szBuffer, 0, 3);

    if(major > 0x1A)
    {
        szBuffer[0] = 'A';
        szBuffer[1] = major - 0x1B + 'A';
    }
    else if(major > 0x00)
    {
        szBuffer[0] = major - 0x01 + 'A';
    }

    return szBuffer;
}

char * SwMinorVersion(BYTE minor, char *szBuffer)
{
    sprintf(szBuffer, "%d", minor);
    return szBuffer;
}

const char * GetMeterType(BYTE mainType, BYTE subType)
{
    switch(mainType)
    {
        case 0x53:
            switch(subType)
            {
                case 0x00: return "K382B/C";
                case 0x01: return "K382Jx";
                case 0x03: return "K382Kx";
                default: return "K382?";
            }
            break;
        case 0x55:
            switch(subType)
            {
                case 0x00: return "K382D/E";
                case 0x01: return "K382Jx";
                case 0x02: return "K382 NTA";
                case 0x03: return "K382Kx";
                default: return "K382?";
            }
            break;
        case 0x56:
            switch(subType)
            {
                case 0x00: return "K162B/C";
                case 0x01: return "K162Jx";
                case 0x03: return "K162Kx";
                default: return "K162?";
            }
            break;
        case 0x57:
            switch(subType)
            {
                case 0x00: return "K162D/E";
                case 0x01: return "K162Jx";
                case 0x03: return "K162Kx";
                default: return "K162?";
            }
            break;
        case 0x58:
            switch(subType)
            {
                case 0x00: return "K282B/C";
                case 0x01: return "K282Jx";
                case 0x03: return "K282Kx";
                default: return "K282?";
            }
            break;
        case 0x59:
            switch(subType)
            {
                case 0x00: return "K282D/E";
                case 0x01: return "K282Jx";
                case 0x03: return "K282Kx";
                default: return "K382?";
            }
            break;
        case 0x5a:
            switch(subType)
            {
                case 0x00: return "OMNI";
                default: return "OMNI?";
            }
            break;
        case 0x60:
            switch(subType)
            {
                case 0x00: return "K351B";
                default: return "K351?";
            }
            break;
        case 0x61:
            switch(subType)
            {
                case 0x00: return "K351B Aron";
                default: return "K351?";
            }
            break;
        case 0x62:
        case 0x63:
            switch(subType)
            {
                case 0x00: return "K382Lx";
                default: return "K382L?";
            }
            break;
        case 0x64:
        case 0x65:
            switch(subType)
            {
                case 0x00: return "K162Lx";
                default: return "K162:?";
            }
            break;
        case 0x66:
        case 0x67:
            switch(subType)
            {
                case 0x00: return "K282Lx";
                default: return "K282L?";
            }
            break;
        case 0x68:
        case 0x69:
            switch(subType)
            {
                case 0x00: return "K382Mx";
                default: return "K382M?";
            }
            break;
        case 0x70:
        case 0x71:
            switch(subType)
            {
                case 0x00: return "K162Mx";
                default: return "K162M?";
            }
            break;
        case 0x72:
        case 0x73:
            switch(subType)
            {
                case 0x00: return "K282Mx";
                default: return "K282M?";
            }
            break;
        case 0x75:
            switch(subType)
            {
                case 0x00: return "K351C";
                default: return "K351?";
            }
            break;
        case 0x76:
            switch(subType)
            {
                case 0x00: return "K251C";
                default: return "K251?";
            }
            break;
        default:
            return "KAM-UNKNOWN";
    }
}

BOOL IsDlmsMeter(BYTE mainType, BYTE subType)
{
    return subType != 0x00 ? TRUE : FALSE;
}

const char * GetKamstrupUnit(BYTE unit)
{
    switch(unit) {
        case 1:         return "Wh";
        case 2:         return "kWh";
        case 3:         return "MWh";
        case 4:         return "GWh";
        case 13:        return "varh";
        case 14:        return "kvarh";
        case 15:        return "Mvarh";
        case 16:        return "Gvarh";
        case 17:        return "VAh";
        case 18:        return "kVAh";
        case 19:        return "MVAh";
        case 20:        return "GVAh";
        case 21:        return "W";
        case 22:        return "kW";
        case 23:        return "MW";
        case 24:        return "GW";
        case 25:        return "var";
        case 26:        return "kvar";
        case 27:        return "Mvar";
        case 28:        return "Gvar";
        case 29:        return "VA";
        case 30:        return "kVA";
        case 31:        return "MVA";
        case 32:        return "GVA";
        case 33:        return "V";
        case 34:        return "A";
        case 35:        return "kV";
        case 36:        return "kA";
        case 37:        return "C";
        case 38:        return "K";
        case 39:        return "l";
        case 40:        return "m3";
        case 46:        return "h";
        case 47:        return ""; // clock
        case 48:        return ""; // date
        case 51:        return ""; // number
        case 53:        return "RTC";
        case 54:        return ""; // aiscii
        case 55:        return "m3 x 10";
        case 56:        return "ton x 10";
        case 57:        return "GJ x 10";
        case 59:        return "Bit";
        case 60:        return "s"; // seconds
        case 61:        return "ms"; // milliseconds
        case 63:        return "RTC-Q";
        case 64:        return "";  // date time
        case 67:        return "Hz";
        case 68:        return "Degree";
        case 72:        return "";  // KamDateTime
    }
    return "";
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
BYTE GetRegValue(BYTE * szStream, double * nValue, char * valueStr, bool bDiff)
{
    BYTE    unit, nob, siex;
    char    kv[512], kvt[32];
#ifndef _WIN32
    long double  v = 0.0;
#else
    double  v = 0.0;
#endif
    double e = 1.0;
    WORD    _wv;
    UINT    _uiv;
    unsigned long long    _uliv;
    bool bNumber = false;
    int j;

    memset(kv, 0, sizeof(kv));
    if(nValue) *nValue = v;

    unit = szStream[0];
    nob = szStream[1];
    siex = szStream[2];

    /** KMP에서 잘못된 length를 주는 경우가 있어서 예외 처리를 한다.
      *
      * unit 47(Time), 48(Date)에 대해서 nob는 4로 고정한다.
      */
    switch(unit)
    {
        case 47: case 48: 
            nob = 4; break;
    }

    e = 1.0;
    switch(nob) {
        case 1: 
            bNumber = true;
            v = (long double)((BYTE) szStream[3]); 
            break;
        case 2: 
            bNumber = true;
            memcpy(&_wv, szStream + 3, sizeof(WORD));
            v = (long double)(BigToHostShort(_wv)); 
            break;
        case 4: 
            bNumber = true;
            memcpy(&_uiv, szStream + 3, sizeof(UINT));
            v = (long double)(BigToHostInt(_uiv)); 
            break;
        case 8: 
            bNumber = true;
            memcpy(&_uliv, szStream + 3, sizeof(unsigned long long));
            v = (long double)(BigToHostLong(_uliv)); 
        default:
            if(unit==51) // number
            {
                int k;
                memset(kvt, 0, sizeof(kvt));
                if(nob < 8)
                {
                    for(k=0, j=(8-nob); k<nob; k++, j++)
                    {
                        kvt[j] = szStream[3+k];
                    }
                }
                else
                {
                    // 8 byte 이상 처리할 수 없기 때문에 그 이상되는 부분은 제외한다.
                    for(k=(nob-8), j=0; k<nob; k++, j++)
                    {
                        kvt[j] = szStream[3+k];
                    }
                }
                memcpy(&_uliv, kvt, sizeof(unsigned long long));
                v = (long double)(BigToHostLong(_uliv)); 
                bNumber = true;
            }
            break;
    }

    if(nValue) *nValue = v;
            
    switch(unit) {
        case 47:
            sprintf(kv, "%02d:%02d:%02d", (UINT)(v / 10000), (UINT)(v / 100) % 100, (UINT)(v) % 100); 
        break;
        case 48:
            sprintf(kv, "%04d/%02d/%02d", (UINT)(v / 10000)+2000, (UINT)(v / 100) % 100, (UINT)(v) % 100); 
        break;
        case 49:
            sprintf(kv, "%04d/%02d/%02d", (UINT)(v / 10000), (UINT)(v / 100) % 100, (UINT)(v) % 100); 
        break;
        case 50:
            sprintf(kv, "%02d/%02d", (UINT)(v / 100) % 100, (UINT)(v) % 100); 
        break;
        case 53:
            sprintf(kv, "%02d/%02d/%02d %02d:%02d:%02d %s%s%s%s",
                    szStream[3+7], szStream[3+6], szStream[3+5],
                    szStream[3+4], szStream[3+3], szStream[3+2],
                    (szStream[3+1] > 0 && szStream[3+1] < 8) ? _week[szStream[3+1]-1] : "",
                    (szStream[3] & 0x01) ? "DST ": "",
                    (szStream[3] & 0x02) ? "": "INV ",
                    (szStream[3] & 0x04) ? "EEPROM": "RTC");
        break;
        case 54:
            for(j=0;j<nob;j++) {
                sprintf(kvt, "%c", (BYTE) szStream[3 + j]);
                strcat(kv, kvt);
            }
            strcat(kv, GetKamstrupUnit(unit));
        break;
        case 59:
            char c;
            for(j=0;j<nob;j++) {
                c = szStream[3 + j];
                sprintf(kvt, "%c%c%c%c%c%c%c%c", 
                        c & 0x80 ? 'T' : 'F', c & 0x40 ? 'T' : 'F',
                        c & 0x20 ? 'T' : 'F', c & 0x10 ? 'T' : 'F',
                        c & 0x08 ? 'T' : 'F', c & 0x04 ? 'T' : 'F',
                        c & 0x02 ? 'T' : 'F', c & 0x01 ? 'T' : 'F'
                );
                if(j > 0) strcat(kv, " ");
                strcat(kv, kvt);
            }
            strcat(kv, GetKamstrupUnit(unit));
        break;
        case 63:
            sprintf(kv, "%02d/%02d/%02d %02d:%02d:%02d %s%s%s",
                    szStream[3+7], szStream[3+6], szStream[3+5],
                    szStream[3+4], szStream[3+3], szStream[3+2],
                    (szStream[3+1] > 0 && szStream[3+1] < 8) ? _week[szStream[3+1]-1] : "",
                    (szStream[3] & 0x01) ? "DST ": "",
                    (szStream[3] & 0x02) ? "": "INV ");
        break;
        case 72:    // KamDateTime
        {
            if(nob >= 5)
            {
                UINT limit = 60*60*24*365;
                memcpy(&_uiv, szStream + 3 + 1, sizeof(UINT));
                if(bDiff && BigToHostInt(_uiv) <= limit) // 1year 보다 작을 때
                {
                    unsigned int buv = BigToHostInt(_uiv);
                    unsigned int uvv;
                    char daytimebuff[64];

                    uvv = buv / (60*60*24); buv = buv % (60*60*24);
                    if(uvv > 0)
                    {
                        sprintf(daytimebuff,"%u d ", uvv);
                        strcat(kv, daytimebuff);
                    }
                    uvv = buv / (60*60); buv = buv % (60*60);
                    if(uvv > 0)
                    {
                        sprintf(daytimebuff,"%u h ", uvv);
                        strcat(kv, daytimebuff);
                    }
                    uvv = buv / (60); buv = buv % (60);
                    if(uvv > 0)
                    {
                        sprintf(daytimebuff,"%u m ", uvv);
                        strcat(kv, daytimebuff);
                    }
                    if(buv > 0)
                    {
                        sprintf(daytimebuff,"%u s ", buv);
                        strcat(kv, daytimebuff);
                    }
                    //sprintf(kv, "%u s", BigToHostInt(_uiv));  // Diff 상태일 때는 초로 계산한다
                }
                else
                {
                    struct tm tmlocal;
                    time_t timev;
    
                    timev = (time_t)(BigToHostInt(_uiv)); 
                    timev += KMP_2000_TIME_T;                   // 2000/1/1 기준 local time_t를 더한다

                    memset(&tmlocal, 0, sizeof(struct tm));
                    localtime_r(&timev, &tmlocal);

                    sprintf(kv, "%04d/%02d/%02d %02d:%02d:%02d %s%s", 
                            tmlocal.tm_year + 1900, tmlocal.tm_mon + 1,
                            tmlocal.tm_mday, tmlocal.tm_hour,
                            tmlocal.tm_min, tmlocal.tm_sec,
                            (szStream[3] & 0x01) ? "DST ": "",
                            (szStream[3] & 0x02) ? "": "INV ");
                }
            }
        }
        break;
        default:
            if(bNumber) {
                if(siex & 0x40) {
                    if(siex & 0x3F) {
                        e /= pow(10,(siex & 0x3F));
                    }
                }else {
                    if(siex & 0x3F) {
                        e *= pow(10,(siex & 0x3F));
                    }
                }
                v *= e;
                if(siex & 0x80) v = -v;
    
                if(siex & 0x40) {
                    switch(siex & 0x3F) {
                        case 1: case 2:
#ifndef _WIN32
                            sprintf(kv, "%.2Lf %s", v, GetKamstrupUnit(unit));
#else
                            sprintf(kv, "%.2f %s", v, GetKamstrupUnit(unit));
#endif
                        break;
                        case 3: case 4:
#ifndef _WIN32
                            sprintf(kv, "%.4Lf %s", v, GetKamstrupUnit(unit));
#else
                            sprintf(kv, "%.4f %s", v, GetKamstrupUnit(unit));
#endif
                        break;
                        default:
#ifndef _WIN32
                            sprintf(kv, "%Lf %s", v, GetKamstrupUnit(unit));
#else
                            sprintf(kv, "%f %s", v, GetKamstrupUnit(unit));
#endif
                        break;
                    }
                }else {
                    sprintf(kv, "%ld %s", (signed long int)v, GetKamstrupUnit(unit));
                }

                if(nValue) *nValue = v;
            } else if(unit ==  51) {
                for(j=0;j<nob;j++) {
                    sprintf(kvt, "%02X", (BYTE) szStream[3 + j]);
                    strcat(kv, kvt);
                }
                strcat(kv,"h");
                strcat(kv, GetKamstrupUnit(unit));
            } else {
                for(j=0;j<nob;j++) {
                    sprintf(kvt, "%02X ", (BYTE) szStream[3 + j]);
                    strcat(kv, kvt);
                }
                if(!bDiff || nob > 0) strcat(kv, GetKamstrupUnit(unit));
            }
        break;
    }

    if(valueStr) sprintf(valueStr, "%s", kv);

    return nob;
}

/** 
  * @param regId Endian이 Host Endian으로 변환되어 와야 한다 
  */
const char * GetRegisterString(WORD regId)
{
    switch(regId) {
        case 0x0001:    return "Active energy A14";
        case 0x0002:    return "Active energy A23";
        case 0x0003:    return "Reactive energy R12";
        case 0x0004:    return "Reactive energy R34";
        case 0x0005:    return "Reactive energy R1";
        case 0x0006:    return "Reactive energy R4";
        case 0x0008:    return "Secondary active energy A23";
        case 0x0009:    return "Secondary reactive energy R12";
        case 0x000A:    return "Secondary reactive energy R34";
        case 0x000B:    return "Secondary reactive energy R1";
        case 0x000C:    return "Secondary reactive energy R4";
        case 0x000D:    return "Active energy A14";
        case 0x000E:    return "Active energy A23";
        case 0x000F:    return "Reactive energy R12";
        case 0x0010:    return "Reactive energy R34";
        case 0x0012:    return "Resetable counter A23";
        case 0x0013:    return "Active energy A14 Tariff 1";
        case 0x0014:    return "Active energy A23 Tariff 1";
        case 0x0015:    return "Reactive energy R12 Tariff 1";
        case 0x0016:    return "Reactive energy R34 Tariff 1";
        case 0x0017:    return "Active energy A14 Tariff 2";
        case 0x0018:    return "Active energy A23 Tariff 2";
        case 0x0019:    return "Reactive energy R12 Tariff 2";
        case 0x001A:    return "Reactive energy R34 Tariff 2";
        case 0x001B:    return "Active energy A14 Tariff 3";
        case 0x001C:    return "Active energy A23 Tariff 3";
        case 0x001D:    return "Reactive energy R12 Tariff 3";
        case 0x001E:    return "Reactive energy R34 Tariff 3";
        case 0x001F:    return "Active energy A14 Tariff 4";
        case 0x0020:    return "Active energy A23 Tariff 4";
        case 0x0021:    return "Reactive energy R12 Tariff 4";
        case 0x0022:    return "Reactive energy R34 Tariff 4";
        case 0x0027:    return "Max power P14";
        case 0x0028:    return "Max power P23";
        case 0x0029:    return "Max power Q12";
        case 0x002A:    return "Max power Q34";
        case 0x002B:    return "Accumulated max power P14";
        case 0x002C:    return "Accumulated max power P23";
        case 0x002D:    return "Accumulated max power Q12";
        case 0x002E:    return "Accumulated max power Q34";
        case 0x002F:    return "Number of debiting periods";
        case 0x0032:    return "Meter status";
        case 0x0033:    return "Meter number 1";
        case 0x0034:    return "Meter number 2";
        case 0x0035:    return "Meter number 3";
        case 0x0036:    return "Configurations number 1";
        case 0x0037:    return "Configurations number 2";
        case 0x0038:    return "Configurations number 3";
        case 0x0039:    return "Special Data 1";
        case 0x003A:    return "Pulse input";
        case 0x00C7:    return "Load profiel logintrval";
        case 0x00DE:    return "ConfigChangedEventCouont";
        case 0x00E7:    return "IncrementConfigChangedEvbentCount";
        case 0x03E9:    return "Serial number";
        case 0x03EA:    return "Clock";
        case 0x03EB:    return "Date";
        case 0x03EC:    return "Hour counter";
        case 0x03ED:    return "Software revision";
        case 0x03F2:    return "Total meter number";
        case 0x03FD:    return "Special Data 2";
        case 0x03FF:    return "Actual power P14";
        case 0x0400:    return "Actual power P23";
        case 0x0401:    return "Actual power Q12";
        case 0x0402:    return "Actual power Q34";
        case 0x0405:    return "Configurations number 4";
        case 0x0407:    return "Active energy A1234";
        case 0x0408:    return "Operation mode";
        case 0x0409:    return "Max power P14 Tariff 1";
        case 0x040C:    return "Max power P14 Tariff 2";
        case 0x040F:    return "Power threshold value";
        case 0x0410:    return "Power threshold counter";
        case 0x0415:    return "RTC status";
        case 0x0416:    return "VCOPCO status";
        case 0x0417:    return "RTC";
        case 0x0418:    return "RTC2";                          // Undocumented
        case 0x0419:    return "Max power P14 RTC";
        case 0x041A:    return "Max power P14 Tariff 1 RTC";
        case 0x041B:    return "Max power P14 Tariff 2 RTC";
        case 0x041E:    return "Voltage L1";
        case 0x041F:    return "Voltage L2";
        case 0x0420:    return "Voltage L3";
        case 0x0422:    return "Type number";
        case 0x0423:    return "Active energy A14 Tariff 5";
        case 0x0424:    return "Active energy A14 Tariff 6";
        case 0x0425:    return "Active energy A14 Tariff 7";
        case 0x0426:    return "Active energy A14 Tariff 8";
        case 0x0427:    return "Active energy A23 Tariff 5";
        case 0x0428:    return "Active energy A23 Tariff 6";
        case 0x0429:    return "Active energy A23 Tariff 7";
        case 0x042A:    return "Active energy A23 Tariff 8";
        case 0x042B:    return "Reactive energy R12 Tariff 5";
        case 0x042C:    return "Reactive energy R12 Tariff 6";
        case 0x042D:    return "Reactive energy R12 Tariff 7";
        case 0x042E:    return "Reactive energy R12 Tariff 8";
        case 0x042F:    return "Reactive energy R34 Tariff 5";
        case 0x0430:    return "Reactive energy R34 Tariff 6";
        case 0x0431:    return "Reactive energy R34 Tariff 7";
        case 0x0432:    return "Reactive energy R34 Tariff 8";
        case 0x0433:    return "Configurations number 5";
        case 0x0434:    return "Current L1";
        case 0x0435:    return "Current L2";
        case 0x0436:    return "Current L3";
        case 0x0438:    return "Actual power P14 L1";
        case 0x0439:    return "Actual power P14 L2";
        case 0x043A:    return "Actual power P14 L3";
        case 0x043B:    return "ROM checksum";
        case 0x043C:    return "Voltage extremity";
        case 0x043D:    return "Voltage event";
        case 0x043E:    return "Logger status";
        case 0x043F:    return "Connection status";
        case 0x0440:    return "Connection feedback";
        case 0x044E:    return "Module port I/O configuration";
        case 0x0454:    return "I shortcirtuit";
        case 0x0455:    return "K1";
        case 0x0456:    return "K2";
        case 0x0457:    return "K3";
        case 0x0458:    return "T1";
        case 0x0459:    return "T2";
        case 0x045A:    return "T3";
        case 0x045B:    return "Tconnectwait";
        case 0x045C:    return "Tprepayment low";
        case 0x045D:    return "Switching on";
        case 0x045E:    return "Cutoff basis";
        case 0x045F:    return "ThLow";
        case 0x0460:    return "ThHigh";
        case 0x0461:    return "A14prepayment";
        case 0x0462:    return "ThDisconnect";
        case 0x0466:    return "Complete type number";
        case 0x0467:    return "Max power Q14 RTC";
        case 0x046A:    return "Max power Q23 Tariff 1";
        case 0x046B:    return "Max power Q23 Tariff 1 RTC";
        case 0x046E:    return "Max power Q23 Tariff 2";
        case 0x046F:    return "Max power Q23 Tariff 2 RTC";
        case 0x0472:    return "Secondary active energy A14 Tariff 1";
        case 0x0473:    return "Secondary active energy A14 Tariff 2";
        case 0x0474:    return "Secondary active energy A14 Tariff 3";
        case 0x0475:    return "Secondary active energy A14 Tariff 4";
        case 0x0476:    return "Secondary active energy A14 Tariff 5";
        case 0x0477:    return "Secondary active energy A14 Tariff 6";
        case 0x0478:    return "Secondary active energy A14 Tariff 7";
        case 0x0479:    return "Secondary active energy A14 Tariff 8";
        case 0x047A:    return "Secondary active energy A23 Tariff 1";
        case 0x047B:    return "Secondary active energy A23 Tariff 2";
        case 0x047C:    return "Secondary active energy A23 Tariff 3";
        case 0x047D:    return "Secondary active energy A23 Tariff 4";
        case 0x047E:    return "Secondary active energy A23 Tariff 5";
        case 0x047F:    return "Secondary active energy A23 Tariff 6";
        case 0x0480:    return "Secondary active energy A23 Tariff 7";
        case 0x0481:    return "Secondary active energy A23 Tariff 8";
        case 0x0482:    return "Secondary reactive energy R12 Tariff 1";
        case 0x0483:    return "Secondary reactive energy R12 Tariff 2";
        case 0x0484:    return "Secondary reactive energy R12 Tariff 3";
        case 0x0485:    return "Secondary reactive energy R12 Tariff 4";
        case 0x0486:    return "Secondary reactive energy R12 Tariff 5";
        case 0x0487:    return "Secondary reactive energy R12 Tariff 6";
        case 0x0488:    return "Secondary reactive energy R12 Tariff 7";
        case 0x0489:    return "Secondary reactive energy R12 Tariff 8";
        case 0x048A:    return "Secondary reactive energy R34 Tariff 1";
        case 0x048B:    return "Secondary reactive energy R34 Tariff 2";
        case 0x048C:    return "Secondary reactive energy R34 Tariff 3";
        case 0x048D:    return "Secondary reactive energy R34 Tariff 4";
        case 0x048E:    return "Secondary reactive energy R34 Tariff 5";
        case 0x048F:    return "Secondary reactive energy R34 Tariff 6";
        case 0x0490:    return "Secondary reactive energy R34 Tariff 7";
        case 0x0491:    return "Secondary reactive energy R34 Tariff 8";
        case 0x0492:    return "Power factor L1";
        case 0x0493:    return "Power factor L2";
        case 0x0494:    return "Power factor L3";
        case 0x0495:    return "Total power factor";
        case 0x0497:    return "Debitinglogger2 loginterval";
        case 0x049C:    return "Password";
        case 0x04A5:    return "Config code 000";
        case 0x04A6:    return "P14 Maximum";
        case 0x04A7:    return "P14 Minimum";
        case 0x04A8:    return "LegalLoggerSize";
        case 0x04A9:    return "LegalLoggerDepth";
        case 0x04AA:    return "AnalysisLoggerDepth";
        case 0x04AB:    return "AnalysisLoggerInterval";
        case 0x04AC:    return "P14 Maximum time";
        case 0x04AD:    return "P14 Maximum Date";
        case 0x04AE:    return "P14 Maximum RTC";
        case 0x04AF:    return "P14 Minimum time";
        case 0x04B0:    return "P14 Minimum Date";
        case 0x04B1:    return "P14 Minimum RTC";
        case 0x04BA:    return "LoadProfileRegisterSetup";
        case 0x04BB:    return "LoadProfileLoggerSetup";
        case 0x04BC:    return "VQLogULow";
        case 0x04BD:    return "VQLogUHigh";
        case 0x04BE:    return "VQLogTeventMinDuration";
        case 0x04BF:    return "Avg Voltage L1";
        case 0x04C0:    return "Avg Voltage L2";
        case 0x04C1:    return "Avg Voltage L3";
        case 0x04C2:    return "Avg Current L1";
        case 0x04C3:    return "Avg Current L2";
        case 0x04C4:    return "Avg Current L3";
        case 0x04C6:    return "LoadProfile Event status";
        case 0x04C8:    return "Logger status 2";
        case 0x04CA:    return "Load1Active";
        case 0x04CB:    return "Load1Mode";
        case 0x04CC:    return "Load1ConvertTariffToPos";
        case 0x04CD:    return "Load2Active";
        case 0x04CE:    return "Load2Mode";
        case 0x04CF:    return "Load2ConvertTariffToPos";
        case 0x04D0:    return "LoadVariableDelay";
        case 0x04D1:    return "WorkingDaysSetup";
        case 0x04D2:    return "PluseInputLevel";
        case 0x04D3:    return "EventstatusA";
        case 0x04D4:    return "EventMaskA";
        case 0x04D5:    return "EventstatusB";
        case 0x04D6:    return "EventMaskB PosEdge";
        case 0x04D7:    return "EventMaskB NegEdge";
        case 0x04D8:    return "DayLightSavingConfig";
        case 0x04D9:    return "DataQualiyMask";
        case 0x04DA:    return "NeutralFaultLogEvent";
        case 0x04DB:    return "ModuleIndentity";
        case 0x04DC:    return "Load1VariableeDelayCnt";
        case 0x04DD:    return "Load2VariableeDelayCnt";
        case 0x04DE:    return "NeutralFaultVNth";
        case 0x04DF:    return "NeutralFaultVLTh";
        case 0x04E0:    return "NeutralFaultTime";
        case 0x04E1:    return "NeutralVoltage";
        case 0x04E2:    return "Displaytest";
        case 0x04E3:    return "DisplayuserForcedCall";
        case 0x04E4:    return "DisplayDisconnect";
        case 0x04E5:    return "DisplayDebitingLogger";
        case 0x04E6:    return "DisplayLoadProfileLogger";
        case 0x04EA:    return "PowerfaultTiemThreshold";
        case 0x04EB:    return "DisplayOptionsConfig";
        case 0x04ED:    return "EnergyA14IntervalValueDay";
        case 0x04EE:    return "EnergyA14IntervalValueWeek";
        case 0x04EF:    return "EnergyA14IntervalValueMonth";
        case 0x04F0:    return "EnergyA14IntervalValueYear";
        case 0x04F4:    return "CommAddrForMBusAndK1";
        case 0x04F7:    return "CommAddrKMP";
        case 0x04FA:    return "Anet";
        case 0x04FB:    return "Reactive energy R2";
        case 0x04FC:    return "Reactive energy R3";
        case 0x04FD:    return "Apparent energy E14";
        case 0x04FE:    return "Apparent energy E23";
        case 0x04FF:    return "Apparent energy E14 Tariff 1";
        case 0x0500:    return "Apparent energy E14 Tariff 2";
        case 0x0501:    return "Apparent energy E14 Tariff 3";
        case 0x0502:    return "Apparent energy E14 Tariff 4";
        case 0x0503:    return "Apparent energy E14 Tariff 5";
        case 0x0504:    return "Apparent energy E14 Tariff 6";
        case 0x0505:    return "Apparent energy E14 Tariff 7";
        case 0x0506:    return "Apparent energy E14 Tariff 8";
        case 0x0507:    return "Apparent energy E23 Tariff 1";
        case 0x0508:    return "Apparent energy E23 Tariff 2";
        case 0x0509:    return "Apparent energy E23 Tariff 3";
        case 0x050A:    return "Apparent energy E23 Tariff 4";
        case 0x050B:    return "Apparent energy E23 Tariff 5";
        case 0x050C:    return "Apparent energy E23 Tariff 6";
        case 0x050D:    return "Apparent energy E23 Tariff 7";
        case 0x050E:    return "Apparent energy E23 Tariff 8";
        case 0x050F:    return "Resetable counter R12";
        case 0x0510:    return "Resetable counter R34";
        case 0x0511:    return "Resetable counter E14";
        case 0x0512:    return "Resetable counter E23";
        case 0x0513:    return "Active positive energy A14 L1";
        case 0x0514:    return "Active positive energy A14 L2";
        case 0x0515:    return "Active positive energy A14 L3";
        case 0x0516:    return "Active positive energy A23 L1";
        case 0x0517:    return "Active positive energy A23 L2";
        case 0x0518:    return "Active positive energy A23 L3";
        case 0x0519:    return "Actual power S14";
        case 0x051A:    return "Actual power S23";
        case 0x051B:    return "Actual power S1234";
        case 0x051C:    return "Average power S14";
        case 0x051D:    return "Average power S23";
        case 0x051E:    return "Max power S23";
        case 0x051F:    return "Max power S23 RTC";
        case 0x0520:    return "Max power P14 Tariff 3";
        case 0x0521:    return "Max power P14 Tariff 3 RTC";
        case 0x0522:    return "Max power P14 Tariff 4";
        case 0x0523:    return "Max power P14 Tariff 4 RTC";
        case 0x0524:    return "Max power P14 Tariff 5";
        case 0x0525:    return "Max power P14 Tariff 5 RTC";
        case 0x0526:    return "Max power P14 Tariff 6";
        case 0x0527:    return "Max power P14 Tariff 6 RTC";
        case 0x0528:    return "Max power P14 Tariff 7";
        case 0x0529:    return "Max power P14 Tariff 7 RTC";
        case 0x052A:    return "Max power P14 Tariff 8";
        case 0x052B:    return "Max power P14 Tariff 8 RTC";
        case 0x052E:    return "Max power S14";
        case 0x052F:    return "Max power S14 RTC";
        case 0x0530:    return "Max power S14 Tariff 1";
        case 0x0531:    return "Max power S14 Tariff 1 RTC";
        case 0x0532:    return "Max power S14 Tariff 2";
        case 0x0533:    return "Max power S14 Tariff 2 RTC";
        case 0x0534:    return "Max power S14 Tariff 3";
        case 0x0535:    return "Max power S14 Tariff 3 RTC";
        case 0x0536:    return "Max power S14 Tariff 4";
        case 0x0537:    return "Max power S14 Tariff 4 RTC";
        case 0x0538:    return "Max power S14 Tariff 5";
        case 0x0539:    return "Max power S14 Tariff 5 RTC";
        case 0x053A:    return "Max power S14 Tariff 6";
        case 0x053B:    return "Max power S14 Tariff 6 RTC";
        case 0x053C:    return "Max power S14 Tariff 7";
        case 0x053D:    return "Max power S14 Tariff 7 RTC";
        case 0x053E:    return "Max power S14 Tariff 8";
        case 0x053F:    return "Max power S14 Tariff 8 RTC";
        case 0x0540:    return "Actual power P23 L1";
        case 0x0541:    return "Actual power P23 L2";
        case 0x0542:    return "Actual power P23 L3";
        case 0x0543:    return "Actual power Q12 L1";
        case 0x0544:    return "Actual power Q12 L2";
        case 0x0545:    return "Actual power Q12 L3";
        case 0x0546:    return "Actual power Q34 L1";
        case 0x0547:    return "Actual power Q34 L2";
        case 0x0548:    return "Actual power Q34 L3";
        case 0x0549:    return "Actual power S14 L1";
        case 0x054A:    return "Actual power S14 L2";
        case 0x054B:    return "Actual power S14 L3";
        case 0x054C:    return "Actual power S23 L1";
        case 0x054D:    return "Actual power S23 L2";
        case 0x054E:    return "Actual power S23 L3";
        case 0x054F:    return "Average power P12 L1";
        case 0x0550:    return "Average power P12 L2";
        case 0x0551:    return "Average power P12 L3";
        case 0x0552:    return "Average power P23 L1";
        case 0x0553:    return "Average power P23 L2";
        case 0x0554:    return "Average power P23 L3";
        case 0x0555:    return "Average power Q12 L1";
        case 0x0556:    return "Average power Q12 L2";
        case 0x0557:    return "Average power Q12 L3";
        case 0x0558:    return "Average power Q34 L1";
        case 0x0559:    return "Average power Q34 L2";
        case 0x055A:    return "Average power Q34 L3";
        case 0x055B:    return "Average power S14 L1";
        case 0x055C:    return "Average power S14 L2";
        case 0x055D:    return "Average power S14 L3";
        case 0x055E:    return "Average power S23 L1";
        case 0x055F:    return "Average power S23 L2";
        case 0x0560:    return "Average power S23 L3";
        case 0x0561:    return "Average power factor L1";
        case 0x0562:    return "Average power factor L2";
        case 0x0563:    return "Average power factor L3";
        case 0x0564:    return "Average power factor total";
        case 0x0569:    return "Average Voltage L1 - (10min)";
        case 0x056A:    return "Average Voltage L2 - (10min)";
        case 0x056B:    return "Average Voltage L3 - (10min)";
        case 0x056C:    return "Supply voltage unbalance";
        case 0x056D:    return "Supply voltage unbalance - mean";
        case 0x056E:    return "Frequency";
        case 0x056F:    return "Frequency - mean";
        case 0x0570:    return "Hour counter tariff 1";
        case 0x0571:    return "Hour counter tariff 2";
        case 0x0572:    return "Hour counter tariff 3";
        case 0x0573:    return "Hour counter tariff 4";
        case 0x0574:    return "Hour counter tariff 5";
        case 0x0575:    return "Hour counter tariff 6";
        case 0x0576:    return "Hour counter tariff 7";
        case 0x0577:    return "Hour counter tariff 8";
        case 0x0578:    return "Power threshold counter tariff 1";
        case 0x0579:    return "Power threshold counter tariff 2";
        case 0x057A:    return "Power threshold counter tariff 3";
        case 0x057B:    return "Power threshold counter tariff 4";
        case 0x057C:    return "Power threshold counter tariff 5";
        case 0x057D:    return "Power threshold counter tariff 6";
        case 0x057E:    return "Power threshold counter tariff 7";
        case 0x057F:    return "Power threshold counter tariff 8";
        case 0x0580:    return "A14pp credit";
        case 0x0583:    return "LastPayment value";
        case 0x0584:    return "LastPayment RTC";
        case 0x0585:    return "TRemaining";
        case 0x0586:    return "Counter 1";
        case 0x0587:    return "Counter 2";
        case 0x0588:    return "Counter 3";
        case 0x0589:    return "Counter 4";
        case 0x058A:    return "Counter 5";
        case 0x058B:    return "Counter 6";
        case 0x058C:    return "Counter 7";
        case 0x058D:    return "Counter 8";
        case 0x058E:    return "CounterVoltageVariationLow1";
        case 0x058F:    return "CounterVoltageVariationLow2";
        case 0x0590:    return "CounterVoltageVariationHigh";
        case 0x0591:    return "CounterRapidVoltageChanges";
        case 0x0592:    return "CounterVoltageUnvalance";
        case 0x0593:    return "CounterInteruptsLong";
        case 0x0594:    return "CounterInteruptsshort";
        case 0x0595:    return "CountTHDL1";
        case 0x0596:    return "CountTHDL2";
        case 0x0597:    return "CountTHDL3";
        case 0x0598:    return "CountDips";
        case 0x0599:    return "CountSwells";
        case 0x059A:    return "THDu - L1 instantaneous";
        case 0x059B:    return "THDu - L1 mean(10min)";
        case 0x059C:    return "THDu - L1 mean(analysis)";
        case 0x059D:    return "THDu - L2 instantaneous";
        case 0x059E:    return "THDu - L2 mean(10min)";
        case 0x059F:    return "THDu - L2 mean(analysis)";
        case 0x05A0:    return "THDu - L3 instantaneous";
        case 0x05A1:    return "THDu - L3 mean(10min)";
        case 0x05A2:    return "THDu - L3 mean(analysis)";
        case 0x05A3:    return "THDi - L1 instantaneous";
        case 0x05A4:    return "THDi - L1 mean(10min)";
        case 0x05A5:    return "THDi - L1 mean(analysis)";
        case 0x05A6:    return "THDi - L2 instantaneous";
        case 0x05A7:    return "THDi - L2 mean(10min)";
        case 0x05A8:    return "THDi - L2 mean(analysis)";
        case 0x05A9:    return "THDi - L3 instantaneous";
        case 0x05AA:    return "THDi - L3 mean(10min)";
        case 0x05AB:    return "THDi - L3 mean(analysis)";
        case 0x05AE:    return "cLogAttribStatus";
        case 0x05AF:    return "cLogAttribDataQuality";
        case 0x05B0:    return "cLogAttribLogsInUse";
        case 0x05B1:    return "cLogAttribLogDepth";
        case 0x05B2:    return "cLogAttribLogId";
        case 0x05B3:    return "cLogAttribNoBFlash";
        case 0x05B4:    return "VoltageQuality1Phase";
        case 0x05B5:    return "VoltageQuality1Event";
        case 0x05B6:    return "VoltageQuality1MeanValue";
        case 0x05B7:    return "VoltageQuality1MaxValue";
        case 0x05B8:    return "VoltageQuality1MinValue";
        case 0x05BE:    return "Phase SequenceDetaction";
        case 0x05BF:    return "Power factor angle L1";
        case 0x05C0:    return "Power factor angle L2";
        case 0x05C1:    return "Power factor angle L3";
        case 0x05C2:    return "Power factor angle total";
        case 0x05D1:    return "VQCounterTHDIL1";
        case 0x05D2:    return "VQCounterTHDIL2";
        case 0x05D3:    return "VQCounterTHDIL3";
        case 0x07DA:    return "Active tariff";
        case 0x07DB:    return "TariffMode";
        case 0x1772:    return "Format_KMP_Energy";
        case 0x1778:    return "Power threshold value tariff 1";
        case 0x1779:    return "Power threshold value tariff 2";
        case 0x177A:    return "Power threshold value tariff 3";
        case 0x177B:    return "Power threshold value tariff 4";
        case 0x177C:    return "Power threshold value tariff 5";
        case 0x177D:    return "Power threshold value tariff 6";
        case 0x177E:    return "Power threshold value tariff 7";
        case 0x177F:    return "Power threshold value tariff 8";
        case 0x178F:    return "SoftwareRevision";
        case 0x1799:    return "T PP exception start";
        case 0x179A:    return "T PP exception stop";
        case 0x179B:    return "PP AlarmLimit";
        case 0x179D:    return "Debit1StopTime";
        case 0x17B3:    return "DataQualityPowerOutageTimeThreshold";
        case 0x17B4:    return "EnableWelmecLogFill";
        case 0x1808:    return "SoftwareUploadInformationPart";
        case 0x180A:    return "SoftwareVariantRevision_Old";
        case 0x180C:    return "StartUploadInterface";
        case 0x1812:    return "IEC1107UserDataSelectPrimaryModule";
        case 0x1813:    return "IEC1107UserDataSelectSecondaryModule";
        case 0x1814:    return "IEC1107UserDataSelectIRModule";
        case 0x1815:    return "IEC1107ModeD0TimeOutPrimaryModule";
        case 0x1816:    return "IEC1107ModeD0TimeOutSecondaryModule";
        case 0x1817:    return "IEC1107ModeD0TimeOutIRModule";
        case 0x1818:    return "IEC1107ModeD0BaudRatePrimaryModule";
        case 0x1819:    return "IEC1107ModeD0BaudRateSecondaryModule";
        case 0x181A:    return "IEC1107ModeD0BaudRateIRModule";
        case 0x181B:    return "IEC1107ComModeSelectPrimaryModule";
        case 0x181C:    return "IEC1107ComModeSelectSecondaryModule";
        case 0x181D:    return "IEC1107ComModeSelectIRModule";
        case 0x181E:    return "IEC1107ProtocolTimerTa";
        case 0x181F:    return "IEC1107ProtocolTimerTr";
        case 0x1820:    return "IEC1107ProtocolTimerTt";
        case 0x1821:    return "IEC1107ProtocolTimerTi";
        case 0x1831:    return "DataQualityPowerOutageTimeThreshold";
        case 0x1834:    return "OriginatorID";
        case 0x1838:    return "DataQualityOverVoltageThreshold";
        case 0x1839:    return "DataQualityUnderVoltageThreshold";
    }
    return UNKNOWN_STR;
}

const char * GetCutOffState(BYTE cutOff)
{
    switch(cutOff)
    {
        case 0x01: return "Relays disconnected by command"; 
        case 0x04: return "Relays connected";
        case 0x05: return "Pre cutoff warning";
        case 0x06: return "Cutoff";
        case 0x07: return "Cutoff Prepayment";
        case 0x08: return "Relays released for reconnection";
        case 0x09: return "Cutoff Prepayment, Lowmax expired";
        case 0x0B: return "Relays disconnected by push button";
    }

    return UNKNOWN_STR;
}

const char *GetCutOffFeedback(BYTE cutOff, BYTE feedback, BYTE * szBuffer)
{
    BOOL bCutOff = FALSE;

    switch(cutOff)
    {
        case 0x01: case 0x05: case 0x06: case 0x07: case 0x09:
            bCutOff = TRUE;
            break;
    }

    sprintf((char *)szBuffer, "%s%s%s%s%s%s%s",
            (feedback & 0x01) ? "L1 " : "",
            (feedback & 0x02) ? "L2 " : "",
            (feedback & 0x04) ? "L3 " : "",
            (feedback & 0x08) ? "" : "INV ",
            (feedback & 0x10) ? "" : "ERROR ",
            bCutOff ? ((feedback & 0x20) ? "LOWCUT " : "HIGHCUT ") : "",
            (feedback & 0x40) ? "PENDING" : "");

    return (const char *) szBuffer;
}

/** EventStatus0 에 대한 출력 가능한 문자열을 구한다.
  *
  * @param status EventStatus0
  * @param mask Event Mask
  * 
  * @return status와 mask를 이용해서 해당 Event가 발생했을 경우 문자열을 리턴한다
  *     만약 발생하지 않았다면 NULL을 Return 한다.
  */
const char *GetEventStatus0(BYTE status, BYTE mask)
{
    if(status & mask)
    {
        switch(mask)
        {
            case 0x01: return "Meter reset";
            case 0x02: return "Error at EEPROM access/restore or backup";
            case 0x04: return "Critical magnetic field detected";
            case 0x08: return "RAM test error";
            case 0x10: return "ROM checksum error";
            case 0x20: return "Alarm input";
            case 0x40: return "Front cover open";
            case 0x80: return "Auxiliary power supply";
        }
    }

    return NULL;
}

/** EventStatus1 에 대한 출력 가능한 문자열을 구한다.
  *
  * @param status EventStatus1
  * @param mask Event Mask
  * 
  * @return status와 mask를 이용해서 해당 Event가 발생했을 경우 문자열을 리턴한다
  *     만약 발생하지 않았다면 NULL을 Return 한다.
  */
const char *GetEventStatus1(BYTE status, BYTE mask)
{
    if(status & mask)
    {
        switch(mask)
        {
            case 0x01: return "RF data ready";
            case 0x02: return "Forced call using DTC = 03h";
        }
    }

    return NULL;
}

/** EventStatus2 에 대한 출력 가능한 문자열을 구한다.
  *
  * @param status EventStatus2
  * @param mask Event Mask
  * 
  * @return status와 mask를 이용해서 해당 Event가 발생했을 경우 문자열을 리턴한다
  *     만약 발생하지 않았다면 NULL을 Return 한다.
  */
const char *GetEventStatus2(BYTE status, BYTE mask)
{
    if(status & mask)
    {
        switch(mask)
        {
            case 0x01: return "Debitering event logged";
            case 0x02: return "RTC event logged";
            case 0x04: return "Meter status event logged";
            case 0x08: return "Voltage quality event logged";
            case 0x10: return "Disconnect event logged";
        }
    }

    return NULL;
}

/** EventStatus3 에 대한 출력 가능한 문자열을 구한다.
  *
  * @param status EventStatus3
  * @param mask Event Mask
  * 
  * @return future use를 위해 reserved 되었다. 항상 NULL을 return 한다.
  */
const char *GetEventStatus3(BYTE status, BYTE mask)
{
    return NULL;
}

/** Log Type에 대한 출력 가능한 문자열을 구한다.
  *
  * @param logType log type
  *
  * @return Log Type 문자열
  */
const char *GetLogType(BYTE logType)
{
    switch(logType)
    {
        case 0x01: return "Debiting logger";
        case 0x02: return "Status event logger";
        case 0x03: return "RTC event logger";
        case 0x04: return "Voltage quality event logger";
        case 0x05: return "Disconnect event logger";
        case 0x06: return "Load profile logger";
        case 0x07: return "Debiting logger for 351BC";
        case 0x08: return "Debiting logger 2";
        case 0x10: return "Load profile vent logger";
        case 0x11: return "Analyze logger";
    }

    return UNKNOWN_STR;
}

/** Log Info에 대한 출력 가능한 문자열을 구한다.
  *
  * @param logInfo log info
  * @param mask Log Info Mask
  *
  * @return Log Info 문자열 만약 설정되지 않았다면 NULL을 Return 한다.
  */
const char *GetLogInfo(BYTE logInfo, BYTE mask)
{
    if(logInfo & mask)
    {
        switch(mask)
        {
            case 0x01: return "Logger is empty";
            case 0x04: return "LogID or time is not in logger range (not existing)";
            case 0x08: return "Length of the application layer is exceeded";
            case 0x10: return "The end of the begging of the logger is reached";
        }
    }

    return NULL;
}

/** Log Info에 대한 출력 가능한 문자열을 구한다. (OMNIPower)
  *
  * @param logInfo log info
  * @param mask Log Info Mask
  *
  * @return Log Info 문자열 만약 설정되지 않았다면 NULL을 Return 한다.
  */
const char *GetLogInfo(WORD logInfo, WORD mask)
{
    if(logInfo & mask)
    {
        switch(mask)
        {
            case 0x0001: return "Logger is empty";
            case 0x0002: return "LogID or time is not in logger range (not existing)";
            case 0x0004: return "Time interval in logger changed";
            case 0x0008: return "Not supported register";
            case 0x0010: return "Not all of the requested data is returned";
            case 0x0020: return "Formatting in logger changed";
            case 0x0040: return "Last existing logging is included int the returned data";
            case 0x0080: return "First existing logging is included int the returned data";
            case 0x0100: return "Memory error";
            case 0x0200: return "Invalid request";
            case 0x0400: return "Internal error";
        }
    }

    return NULL;
}

/** Voltage quality 에 대한 출력 가능한 문자열을 구한다.
  *
  * @param quality voltage quality
  * @param mask Mask
  * 
  * @return quality와 mask를 이용해서 해당 Event가 발생했을 경우 문자열을 리턴한다
  *     만약 발생하지 않았다면 NULL을 Return 한다.
  */
const char *GetVoltageQuality(BYTE quality, BYTE mask)
{
    if(quality & mask)
    {
        switch(mask)
        {
            case 0x01: return "System L1";
            case 0x02: return "System L2";
            case 0x04: return "System L3";
            case 0x08: return "Voltage exceeds cutoff voltage";
            case 0x10: return "Voltage below cutoff voltage";
            case 0x20: return "Voltage within limits of under and over voltage";
            case 0x40: return "Under voltage";
            case 0x80: return "Over voltage";
        }
    }

    return NULL;
}

/** RTC Event 에 대한 출력 가능한 문자열을 구한다.
  *
  * @param status RTC event status
  * @param mask Mask
  * 
  * @return status와 mask를 이용해서 해당 Event가 발생했을 경우 문자열을 리턴한다
  *     만약 발생하지 않았다면 NULL을 Return 한다.
  */
const char *GetRTCStatus(BYTE status, BYTE mask)
{
    if(status & mask)
    {
        switch(mask)
        {
            case 0x01: return "SetClock / PutRegister 1047";
            case 0x04: return "No change";
        }
    }

    return NULL;
}

