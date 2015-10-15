#include <math.h>

#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "mcudef.h"

#include "kmpDefine.h"

#include "KmpUtils.h"

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
    }
    return "";
}

/** Get Register값을 Parsing 해 준다.
  * 
  * @param szStream RegFormat+RegValue 이 전달되어야 한다 (RID 제외)
  * @param nValue NULL이 아닐 경우 double형 value 전달
  * @param valuestr NULL이 아닐 경우 출력 가능한 포멧으로 전달
  *
  * @return Value의 Length
  */
BYTE GetRegValue(BYTE * szStream, double * nValue, char * valueStr)
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
            v = (long double)((BYTE) szStream[3]); 
            break;
        case 2: 
            memcpy(&_wv, szStream + 3, sizeof(WORD));
            v = (long double)(BigToHostShort(_wv)); 
            break;
        case 4: 
            memcpy(&_uiv, szStream + 3, sizeof(UINT));
            v = (long double)(BigToHostInt(_uiv)); 
            break;
        case 8: 
            memcpy(&_uliv, szStream + 3, sizeof(unsigned long long));
            v = (long double)(BigToHostLong(_uliv)); 
        default:
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
        case 53:
        case 63:
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
        default:
            if(nob == 1 || nob == 2 || nob == 4 || nob == 8) {
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
                strcat(kv, GetKamstrupUnit(unit));
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
        case 0x04A6:    return "P14 Maximum";
        case 0x04A7:    return "P14 Minimum";
        case 0x04A9:    return "LegalLoggerDepth";
        case 0x04AA:    return "AnalysisLoggerDepth";
        case 0x04AC:    return "P14 Maximum time";
        case 0x04AD:    return "P14 Maximum Date";
        case 0x04AE:    return "P14 Maximum RTC";
        case 0x04AF:    return "P14 Minimum time";
        case 0x04B0:    return "P14 Minimum Date";
        case 0x04B1:    return "P14 Minimum RTC";
        case 0x04BF:    return "Avg Voltage L1";
        case 0x04C0:    return "Avg Voltage L2";
        case 0x04C1:    return "Avg Voltage L3";
        case 0x04C2:    return "Avg Current L1";
        case 0x04C3:    return "Avg Current L2";
        case 0x04C4:    return "Avg Current L3";
        case 0x04C6:    return "LoadProfile Event status";
        case 0x04C8:    return "Logger status 2";
        case 0x07DA:    return "Active tariff";
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

