#include <math.h>

#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "mbusDefine.h"
#include "MbusUtils.h"

#define UNKNOWN_STR     "UNKNOWN"

const char *GetMbusErrorMessage(int nError)
{
	switch(nError) {
	  case MBUSERR_NOERROR :				return "OK";
	  case MBUSERR_INVALID_PARAM :			return "Invalid param";
	  case MBUSERR_INVALID_SESSION :		return "Invalid session";
	  case MBUSERR_INVALID_HANDLE :			return "Invalid handle";
	  case MBUSERR_INVALID_ADDRESS :		return "Invalid address";
	  case MBUSERR_INVALID_PORT :			return "Invalid port";
	  case MBUSERR_INVALID_SOURCE_ADDR :	return "Invalid source address";	
	  case MBUSERR_INVALID_DEST_ADDR :		return "Invalid destination address";
	  case MBUSERR_INVALID_TIMEOUT :		return "Invalid timeout";
	  case MBUSERR_INVALID_ACCESS :			return "Invalid access";
	  case MBUSERR_INVALID_DATA :			return "Invalid data";
	  case MBUSERR_INVALID_ACTION :			return "Invalid action";
	  case MBUSERR_INVALID_OBISCODE :		return "Invalid obiscode";
	  case MBUSERR_INVALID_STRING :			return "Invalid string";
	  case MBUSERR_INVALID_LENGTH :			return "Invalid length";
	  case MBUSERR_INVALID_BUFFER :			return "Invalid buffer";
	  case MBUSERR_CANNOT_CONNECT :			return "Cannot connect";
	  case MBUSERR_REPLY_TIMEOUT :			return "Reply timeout";
	  case MBUSERR_NOT_INITIALIZED :		return "Not initialized";
	  case MBUSERR_SYSTEM_ERROR :			return "System error";
	  case MBUSERR_MEMORY_ERROR :			return "Memory error";
	}
	return "Unkown";
}

const char *GetMbusMedium(BYTE nType)
{
    switch(nType) {
        case 0x00:  return "Other";
        case 0x01:  return "Oil";
        case 0x02:  return "Electricity";
        case 0x03:  return "Gas";
        case 0x04:  return "Heat";
        case 0x05:  return "Steam";
        case 0x06:  return "Hot Water";
        case 0x07:  return "Water";
        case 0x08:  return "H.C.A";
        case 0x0A:  return "Gas Mode 2";
        case 0x0B:  return "Heat Mode 2";
        case 0x0C:  return "Hot Water Mode 2";
        case 0x0D:  return "Water Mode 2";
        case 0x0E:  return "H.C.A Mode 2";
    }
    return UNKNOWN_STR;
}

/** MBUS의 DIF, VIF 값을 이용해서 Message를 생성해 낸다.
  *
  * MBUS를 위해서 꼭 필요한 부분만 구현한다.
  *
  * @return Value의 Length
  */
WORD GetDIFVIFValue(BYTE dif, BYTE edif, BYTE vif, BYTE *value, UNIONVALUE *uv, BYTE *buffer)
{
    WORD len=0;
    ULONG   U = 0;
    int exp = 0;
    BYTE revBuffer[32];

    if(!value || !uv || !buffer) return len;

    switch(dif & 0x0F)
    {
        case 0x00: len = 0; break;  // No Data
        case 0x01: case 0x09: len = 1; break;
        case 0x02: case 0x0A: len = 2; break;
        case 0x03: case 0x0B: len = 3; break;
        case 0x04: case 0x0C: len = 4; break;
        case 0x05: len = 4; break;
        case 0x06: case 0x0E: len = 6; break;
        case 0x07: len = 8; break;
        default: return len;        // Not Support;
    }

    if(!(dif & 0x08)) memcpy(uv, value, len);

    switch(dif & 0x0F)
    {
        case 0x02: 
            uv->wv = LittleToHostShort(uv->wv);
            U = (ULONG) uv->wv;
            break;
        case 0x03: case 0x04: 
            uv->iv = LittleToHostInt(uv->iv);
            U = (ULONG) uv->iv;
            break;
        case 0x06: case 0x07: 
            uv->ulv = LittleToHostLong(uv->ulv);
            U = (ULONG) uv->ulv;
            break;
        case 0x09: case 0x0A: case 0x0B: 
        case 0x0C: case 0x0E: 
            memset(revBuffer, 0, sizeof(revBuffer));
            ReverseBcd(value, revBuffer, len);
            uv->ulv = BcdToInt(revBuffer, len);
            U = (ULONG) uv->ulv;
            break;
    }

    if(!(vif & 0x78)) {                 // E000 0nnn    Energy 10(nnn-3) Wh 
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f kWh", "Energy", U * pow(10, exp - 3)/1000.0); 
    }else if((vif & 0x78) == 0x08) {    // E000 1nnn    Energy 10(nnn) J
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f kJ", "Energy", U * pow(10, exp)/1000.0); 
    }else if((vif & 0x78) == 0x10) {    // E001 0nnn    Volume 10(nnn-6) m3
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f l", "Volume", U * pow(10, exp - 6) * 1000.0); 
    }else if((vif & 0x78) == 0x18) {    // E001 1nnn    Mass 10(nnn-3) kg
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f kg", "Mass", U * pow(10, exp - 3)); 
    }else if((vif & 0x7C) == 0x20) {    // E010 00nn    On Time (nn 00 sec, 01 min, 10 hour, 11 days)
        exp = vif & 0x03;
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu %s", "OnTime", U, exp==0?"sec":(exp==1?"min":(exp==2?"hour":"days"))); 
#else
        sprintf((char *)buffer, "%-10s %lu %s", "OnTime", U, exp==0?"sec":(exp==1?"min":(exp==2?"hour":"days"))); 
#endif
    }else if((vif & 0x7C) == 0x24) {    // E010 01nn    Operating Time (nn 00 sec, 01 min, 10 hour, 11 days)
        exp = vif & 0x03;
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu %s", "OpTime", U, exp==0?"sec":(exp==1?"min":(exp==2?"hour":"days"))); 
#else
        sprintf((char *)buffer, "%-10s %lu %s", "OpTime", U, exp==0?"sec":(exp==1?"min":(exp==2?"hour":"days"))); 
#endif
    }else if((vif & 0x78) == 0x28) {    // E010 1nnn    Power 10(nnn-3) W
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f W", "Power", U * pow(10, exp - 3)); 
    }else if((vif & 0x78) == 0x30) {    // E011 0nnn    Power 10(nnn) J/h
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f kJ/h", "Energy", U * pow(10, exp)/1000.0); 
    }else if((vif & 0x78) == 0x38) {    // E011 1nnn    Volume Flow 10(nnn-6) m3/h
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f l/h", "VolFlow", U * pow(10, exp - 6) * 1000.0); 
    }else if((vif & 0x78) == 0x40) {    // E100 0nnn    Volume Flow ext. 10(nnn-7) m3/min
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f l/min", "VolFlowExt", U * pow(10, exp - 7) * 1000.0); 
    }else if((vif & 0x78) == 0x48) {    // E100 1nnn    Volume Flow ext. 10(nnn-9) m3/s
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f ml/s", "VolFlowExt", U * pow(10, exp - 9) * 1000000.0); 
    }else if((vif & 0x78) == 0x50) {    // E101 0nnn    Mass flow 10(nnn-3) kg/h
        exp = vif & 0x07;
        sprintf((char *)buffer, "%-10s %.3f kg/h", "MassFlow", U * pow(10, exp - 3)); 
    }else if((vif & 0x7C) == 0x58) {    // E101 10nn    Flow Temperature 10(nn-3) C
        exp = vif & 0x03;
        sprintf((char *)buffer, "%-10s %.3f C", "FlowTemp", U * pow(10, exp - 3)); 
    }else if((vif & 0x7C) == 0x5C) {    // E101 11nn    Return Temperature 10(nn-3) C
        exp = vif & 0x03;
        sprintf((char *)buffer, "%-10s %.3f C", "ReturnTemp", U * pow(10, exp - 3)); 
    }else if((vif & 0x7C) == 0x60) {    // E110 00nn    Temperature Difference 10(nn-3) K
        exp = vif & 0x03;
        sprintf((char *)buffer, "%-10s %.3f mK", "TempDiff", U * pow(10, exp - 3) * 1000.0); 
    }else if((vif & 0x7C) == 0x64) {    // E110 01nn    External Temperature 10(nn-3) C
        exp = vif & 0x03;
        sprintf((char *)buffer, "%-10s %.3f C", "ExtTemp", U * pow(10, exp - 3)); 
    }else if((vif & 0x7C) == 0x68) {    // E110 10nn    Pressure 10(nn-3) var
        exp = vif & 0x03;
        sprintf((char *)buffer, "%-10s %.3f mbar", "Pressure", U * pow(10, exp - 3) * 1000.0); 
    }else if((vif & 0x7E) == 0x6C) {    // E110 110n    Time Point (n 0 date, 1 time & date)
        exp = vif & 0x01;
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu %s", "TimePoint", U, exp==0?"date":"date&time"); 
#else
        sprintf((char *)buffer, "%-10s %lu %s", "TimePoint", U, exp==0?"date":"date&time"); 
#endif
    }else if((vif & 0x7F) == 0x6E) {    // E110 1110    Units for H.C.A
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu", "H.C.A", U); 
#else
        sprintf((char *)buffer, "%-10s %lu", "H.C.A", U); 
#endif
    }else if((vif & 0x7C) == 0x70) {    // E111 00nn    Averaging Duration (nn 00 sec, 01 min, 10 hour, 11 days)
        exp = vif & 0x03;
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu %s", "AvgDur", U, exp==0?"sec":(exp==1?"min":(exp==2?"hour":"days"))); 
#else
        sprintf((char *)buffer, "%-10s %lu %s", "AvgDur", U, exp==0?"sec":(exp==1?"min":(exp==2?"hour":"days"))); 
#endif
    }else if((vif & 0x7C) == 0x74) {    // E111 01nn    Actuality Duration (nn 00 sec, 01 min, 10 hour, 11 days)
        exp = vif & 0x03;
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu %s", "ActDur", U, exp==0?"sec":(exp==1?"min":(exp==2?"hour":"days"))); 
#else
        sprintf((char *)buffer, "%-10s %lu %s", "ActDur", U, exp==0?"sec":(exp==1?"min":(exp==2?"hour":"days"))); 
#endif
    }else if((vif & 0x7F) == 0x78) {    // E111 1000    Fabrication No
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu", "Fabr.No", U); 
#else
        sprintf((char *)buffer, "%-10s %lu", "Fabr.No", U); 
#endif
    }else if((vif & 0x7F) == 0x79) {    // E111 1001    Identification
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu", "ID", U); 
#else
        sprintf((char *)buffer, "%-10s %lu", "ID", U); 
#endif
    }else if((vif & 0x7F) == 0x7A) {    // E111 1010    Bus Address
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu", "BusAddr", U); 
#else
        sprintf((char *)buffer, "%-10s %lu", "BusAddr", U); 
#endif
    }else {
#ifndef _WIN32
        sprintf((char *)buffer, "%-10s %llu", "", U); 
#else
        sprintf((char *)buffer, "%-10s %lu", "", U); 
#endif
    }
        /*
    switch(vif & 0x7F)
    {
           이 코드는 Variable Data Structure가 아니라 Fixed Data Structure이기
           때문에 일단 주석 처리 한다
        case 0x00: sprintf((char *)buffer, "%llu hms", U); break;
        case 0x01: sprintf((char *)buffer, "%llu DMY", U); break;
        case 0x02: sprintf((char *)buffer, "%llu Wh", U); break;
        case 0x03: sprintf((char *)buffer, "%llu Wh", U * 10); break;
        case 0x04: sprintf((char *)buffer, "%llu Wh", U * 100); break;
        case 0x05: sprintf((char *)buffer, "%llu kWh", U); break;
        case 0x06: sprintf((char *)buffer, "%llu kWh", U * 10); break;
        case 0x07: sprintf((char *)buffer, "%llu kWh", U * 100); break;
        case 0x08: sprintf((char *)buffer, "%llu MWh", U); break;
        case 0x09: sprintf((char *)buffer, "%llu MWh", U * 10); break;
        case 0x0A: sprintf((char *)buffer, "%llu MWh", U * 100); break;
        case 0x0B: sprintf((char *)buffer, "%llu kJ", U); break;
        case 0x0C: sprintf((char *)buffer, "%llu kJ", U * 10); break;
        case 0x0D: sprintf((char *)buffer, "%llu kJ", U * 100); break;
        case 0x0E: sprintf((char *)buffer, "%llu MJ", U); break;
        case 0x0F: sprintf((char *)buffer, "%llu MJ", U * 10); break;
        case 0x10: sprintf((char *)buffer, "%llu MJ", U * 100); break;
        case 0x11: sprintf((char *)buffer, "%llu GJ", U); break;
        case 0x12: sprintf((char *)buffer, "%llu GJ", U * 10); break;
        case 0x13: sprintf((char *)buffer, "%llu GJ", U * 100); break;
        case 0x14: sprintf((char *)buffer, "%llu W", U); break;
        case 0x15: sprintf((char *)buffer, "%llu W", U * 10); break;
        case 0x16: sprintf((char *)buffer, "%llu W", U * 100); break;
        case 0x17: sprintf((char *)buffer, "%llu kW", U); break;
        case 0x18: sprintf((char *)buffer, "%llu kW", U * 10); break;
        case 0x19: sprintf((char *)buffer, "%llu kW", U * 100); break;
        case 0x1A: sprintf((char *)buffer, "%llu MW", U); break;
        case 0x1B: sprintf((char *)buffer, "%llu MW", U * 10); break;
        case 0x1C: sprintf((char *)buffer, "%llu MW", U * 100); break;
        case 0x1D: sprintf((char *)buffer, "%llu kJ/h", U); break;
        case 0x1E: sprintf((char *)buffer, "%llu kJ/h", U * 10); break;
        case 0x1F: sprintf((char *)buffer, "%llu kJ/h", U * 100); break;
        case 0x20: sprintf((char *)buffer, "%llu MJ/h", U); break;
        case 0x21: sprintf((char *)buffer, "%llu MJ/h", U * 10); break;
        case 0x22: sprintf((char *)buffer, "%llu MJ/h", U * 100); break;
        case 0x23: sprintf((char *)buffer, "%llu GJ/h", U); break;
        case 0x24: sprintf((char *)buffer, "%llu GJ/h", U * 10); break;
        case 0x25: sprintf((char *)buffer, "%llu GJ/h", U * 100); break;
        case 0x26: sprintf((char *)buffer, "%llu ml", U); break;
        case 0x27: sprintf((char *)buffer, "%llu ml", U * 10); break;
        case 0x28: sprintf((char *)buffer, "%llu ml", U * 100); break;
        case 0x29: sprintf((char *)buffer, "%llu l", U); break;
        case 0x2A: sprintf((char *)buffer, "%llu l", U * 10); break;
        case 0x2B: sprintf((char *)buffer, "%llu l", U * 100); break;
        case 0x2C: sprintf((char *)buffer, "%llu m3", U); break;
        case 0x2D: sprintf((char *)buffer, "%llu m3", U * 10); break;
        case 0x2E: sprintf((char *)buffer, "%llu m3", U * 100); break;
        case 0x2F: sprintf((char *)buffer, "%llu ml/h", U); break;
        case 0x30: sprintf((char *)buffer, "%llu ml/h", U * 10); break;
        case 0x31: sprintf((char *)buffer, "%llu ml/h", U * 100); break;
        case 0x32: sprintf((char *)buffer, "%llu l/h", U); break;
        case 0x33: sprintf((char *)buffer, "%llu l/h", U * 10); break;
        case 0x34: sprintf((char *)buffer, "%llu l/h", U * 100); break;
        case 0x35: sprintf((char *)buffer, "%llu m3/h", U); break;
        case 0x36: sprintf((char *)buffer, "%llu m3/h", U * 10); break;
        case 0x37: sprintf((char *)buffer, "%llu m3/h", U * 100); break;
        case 0x38: sprintf((char *)buffer, "%.2f C", (double) (U / 100)); break;
        case 0x39: sprintf((char *)buffer, "%llu units", U); break;
        case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3E: 
        case 0x3F: sprintf((char *)buffer, "%llu", U); break;
    }
        */

    return len;
}

