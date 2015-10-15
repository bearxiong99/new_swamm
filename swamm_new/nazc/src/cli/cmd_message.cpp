#include <math.h>
#include "common.h"
#include "cmd_message.h"
#include "endiDefine.h"
#include "gpio/GPIOAPI.h"

/////////////////////////////////////////////////////////////////////////////
// System Function
/////////////////////////////////////////////////////////////////////////////

const char *MSGAPI_GetServiceType(int nType)
{
	switch(nType) {
	  case 0 : return UNKNOWN_STR;
	  case 1 : return "POWER";
	  case 2 : return "GAS";
	  case 3 : return "WATER";
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_GetEtherType(int nType)
{
	switch(nType) {
	  case ETHER_TYPE_LAN : 	return "LAN";
	  case ETHER_TYPE_DHCP : 	return "DHCP";
	  case ETHER_TYPE_PPPOE : 	return "PPPoE";
	  case ETHER_TYPE_PPP : 	return "PPP";
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_GetMobileType(int nType)
{
	switch(nType) {
	  case MOBILE_TYPE_NONE : 	return "NONE";
	  case MOBILE_TYPE_GSM : 	return "GSM/GPRS";
	  case MOBILE_TYPE_CDMA : 	return "CDMA";
	  case MOBILE_TYPE_PSTN : 	return "PSTN";
	}
	return UNKNOWN_STR;
}
const char *MSGAPI_GetResetReason(int nType)
{
	switch(nType) {
      case RST_UNKNOWN    :     return UNKNOWN_STR;
	  case RST_COMMAND    :	    return "CMD RESET";
	  case RST_UPFIRM     : 	return "FIRM UP";
	  case RST_AUTORESET  : 	return "AUTORESET";
	  case RST_SYSMONITOR : 	return "SYSMONITOR";
	  case RST_LOWBATT    : 	return "LOW BATT";
	}
	return UNKNOWN_STR;
}
const char *MSGAPI_GetMobileMode(int nType)
{
	switch(nType) {
	  case MOBILE_MODE_CSD: 	return "CSD";
	  case MOBILE_MODE_PACKET : return "PACKET";
	  case MOBILE_MODE_ALWAYS : return "Always ON";
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_GetMobileAPN(int nType)
{
	return "Telenor";
}

const char *MSGAPI_GetSensorType(BYTE nType)
{
	switch(nType) {
	  case 0 : return "<INIT>";
	  case 1 : return "ZRU";
	  case 2 : return "ZMU";
	  case 3 : return "ZEU-PC";
	  case 4 : return "ZEU-PDA";
//	  case 5 : return "ZEU-PLS";
	  case 5 : return "NAPC-AM";
	  case 6 : return "ZEU-EISS";
	  case 7 : return "ZEU-PQ";
	  case 8 : return "ZEU-IO";
	  case 100 : return "NAPC-BM";
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_GetSensorVendor(BYTE nType, BYTE nVendor)
{
	switch(nType) {
	  case 1 : // ZRU
		   switch(nVendor) {
		     case 1 : return "Kamstrup";
			 case 2 : return "GE";
			 case 3 : return "Elster";
			 case 4 : return "Landis+Gyr";
			 case 5 : return "Aidon";
		   }
		   break;
	  case 2 : // ZMU
		   switch(nVendor) {
			 case 1 : return "Summit3208";
			 case 2 : return "MENIX";
		   }
		   break;
	  case 5 : // ZEU-PLS
		   switch(nVendor) {
		     case 0 :
		     case 1 :
		     case 2 :
		     case 3 :
		     case 4 :
		     case 5 : return "Kamstrup";
		   }
		   break;
	  case 3 : // ZEU-PC
		   return "NURITELECOM";
		   break;
	  case 0 : // CODI
	  case 4 : // ZEU-PDA
	  case 6 : // ZEU-EISS
	  case 7 : // ZEU-PQ
	  case 8 : // ZEU-IO
		   return UNKNOWN_STR;
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_GetSensorActiveState(BYTE nFlag)
{
	if (nFlag & 0x40) return "ERROR";
	if (nFlag & 0x20) return "TESTING";
	if (nFlag & 0x80) return "ACTIVE";
	return "NORMAL";
}

const char *MSGAPI_GetSinkType(BYTE nType)
{
	switch(nType) {
	  case 1 : return "PLUSEMETER";
	  case 2 : return "DIGITALMETER";
	  case 3 : return "RF3SERIAL";
	  case 4 : return "HTAMM";
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_GetCodiType(BYTE nType)
{
    switch(nType) {
        case CODI_TYPE_EMBER_STACK_331 :  return "Ember Stack 3.3.1";
        case CODI_TYPE_EMBER_STACK_333 :  return "Ember Stack 3.3.3";
        case CODI_TYPE_ZIGBEE_STACK_25X : return "Zigbee Stack 2.5.x";
        case CODI_TYPE_ZIGBEE_STACK_30X : return "Zigbee Stack 3.0.x";
        case CODI_TYPE_ZIGBEE_STACK_31X : return "Zigbee Stack 3.1.x";
        case CODI_TYPE_ZIGBEE_STACK_32X : return "Zigbee Stack 3.2.x";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_GetUserType(BYTE nType)
{
	switch(nType) {
	  case 0 : return "SUPER";
	  case 1 : return "ADMIN";
	  case 2 : return "USER";
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_GetUploadCycleType(BYTE nType)
{
	switch(nType) {
	  case 0 : return "Default (hourly)";
	  case 1 : return "Immediately";
	  case 2 : return "Daily";
      case 3 : return "Weekly";
      case 4 : return "Hourly";
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_GetUploadCycle(char *pszBuffer, BYTE nType, unsigned int nCycle)
{
	switch(nType) {
	  case 1 :
		   strcpy(pszBuffer, "Immediately");
		   break;
	  case 0 :
	  case 2 :
		   sprintf(pszBuffer, "0x%08X", nCycle);
		   break;
      case 3 :
		   switch(nCycle) {
			 case 0 : strcpy(pszBuffer, "Sunday"); break;
			 case 1 : strcpy(pszBuffer, "Monday"); break;
			 case 2 : strcpy(pszBuffer, "Tuesday"); break;
			 case 3 : strcpy(pszBuffer, "Wednesday"); break;
			 case 4 : strcpy(pszBuffer, "Thursday"); break;
			 case 5 : strcpy(pszBuffer, "Friday"); break;
			 case 6 : strcpy(pszBuffer, "Saturday"); break;
		   }
		   break;
      case 4 :
		   sprintf(pszBuffer, "0x%08X", nCycle);
		   break;
	  default :
		   strcpy(pszBuffer, UNKNOWN_STR);
		   break;
	}
	return pszBuffer;
}

const char *MSGAPI_GetTxPowerMode(BYTE nType)
{
    switch(nType) {
        case 0 : return "Default";
        case 1 : return "Boost";
        case 2 : return "Alternate";
        case 3 : return "Alternate & Boost";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_GetResetKind(BYTE nType)
{
    switch(nType) {
        case 0x00 : return "RESET_NONE";
        case 0x01 : return "RESET_EXTERNAL";
        case 0x02 : return "RESET_POWERON";
        case 0x03 : return "RESET_WATCHDOG";
        case 0x04 : return "RESET_BROWNOUT";
        case 0x05 : return "RESET_JTAG";
        case 0x06 : return "RESET_ASSERT";
        case 0x07 : return "RESET_RSTACK";
        case 0x08 : return "RESET_CSTACK";
        case 0x09 : return "RESET_BOOTLOADER";
        case 0x0A : return "RESET_PC_ROLLOVER";
        case 0x0B : return "RESET_SOFTWARE";
        case 0x0C : return "RESET_PROTFAULT";
        case 0x0D : return "RESET_FLASH_VERIFY_FAIL";
        case 0x0E : return "RESET_FLASH_WRITE_INHIBIT";
        case 0x0F : return "RESET_BOOTLOADER_IMG_BAD";
        case 0x14 : return "RESET_FACTORY_SETTING";
        case 0x15 : return "RESET_REED_SW";
        case 0x16 : return "RESET_FIXED_INTERVAL";
        case 0x17 : return "RESET_SOFT_REMOTE";
        case 0x18 : return "RESET_AMR_FAIL";
        case 0x19 : return "RESET_FAIL_CLOSE_CONNECTION";
        case 0x1A : return "RESET_DELETE_BIND_FAIL";
        case 0x1B : return "RESET_SCAN_FAIL";
        case 0x1C : return "RESET_NO_JOIN";
    }
    return "RESET_UNKNOWN";
}

const char *MSGAPI_GetSensorState(BYTE nType)
{
    switch(nType) {
        case ENDISTATE_NORMAL:              return "Normal";
        case ENDISTATE_INIT:                return "Init";
        case ENDISTATE_CONNECTION_ERROR:    return "Connection Error";
        case ENDISTATE_METER_ERROR:         return "Meter Error";
        case ENDISTATE_ERROR:               return "Error";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_GetEventLevelString(int nLevel)
{
	switch(nLevel) {
	  case 0 :	return "Critical";
	  case 1 :	return "Major";
	  case 2 :  return "Minor";
	  case 3 :	return "Normal";
	  case 4 :	return "Information";
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_PingMethod(int nMethod)
{
	switch(nMethod) {
	  case 0 : return "ICMP";
	  case 1 : return "SOCKET";
	  case 2 : return "ICMP+SOCKET";
	}
	return "ICMP";
}

const char *MSGAPI_ScanningStrategy(int nStrategy)
{
	switch(nStrategy) {
	  case 0 : return "Lazy";
	  case 1 : return "Immediately";
	}
	return "Lazy";
}

const char *MSGAPI_GetOpMode(int nType)
{
	switch(nType) {
	  case OP_MODE_NORMAL : 	return "Normal";
	  case OP_MODE_TEST : 	    return "Test";
	  case OP_MODE_PULSE_MESH : return "Pulse-Mesh";
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_GetMeteringType(int nType, char *buffer)
{
    if(buffer == NULL) return "";

	switch(nType) {
	  case 0 : 	sprintf(buffer,"%s", "Default"); break;
	  case 3 : 	sprintf(buffer,"%s", "Relay Status"); break;
	  case 4 :  sprintf(buffer,"%s", "Relay On"); break;
	  case 5 :  sprintf(buffer,"%s", "Relay Off"); break;
	  case 6 :  sprintf(buffer,"%s", "Activate On"); break;
	  case 7 :  sprintf(buffer,"%s", "Activate Off"); break;
	  case 8 :  sprintf(buffer,"%s", "Meter TimeSync"); break;
	  case 10 : sprintf(buffer,"%s", "Get Event Log"); break;
	  case 11 : sprintf(buffer,"%s", "Read Ident Only"); break;
      default: sprintf(buffer, "%d", nType); break;
	}
	return buffer;
}

const char *MSGAPI_GetPowerType(BYTE nType)
{
	switch(nType) {
	  case 0x01 : return "LINE";
	  case 0x02 :  return "BATTERY";
	  case 0x04 :  return "SOLAR";
	}
	return UNKNOWN_STR;
}

void MSGAPI_GetAlarmMask(WORD alarmMask, char * buffer)
{
    sprintf(buffer, "(");
    if(alarmMask & 0x0010) strcat(buffer, " Case_Open");
    if(alarmMask & 0x0020) strcat(buffer, " MR_Tamper");
    if(alarmMask & 0x0040) strcat(buffer, " MR_Detachment");
    if(alarmMask & 0x0080) strcat(buffer, " Low_Battery");
    if(alarmMask & 0x0100) strcat(buffer, " Pulse_Cut");
    strcat(buffer," )");
}

const char * MSGAPI_GetDlmsUnit(BYTE unit)
{
    switch(unit) {
        case 1:         return "a";
        case 2:         return "mo";
        case 3:         return "wk";
        case 4:         return "d";
        case 5:         return "h";
        case 6:         return "min";
        case 7:         return "s";
        case 8:         return "degree";
        case 9:         return "degree-celsius";
        case 10:        return "currency";
        case 11:        return "m";
        case 12:        return "m/s";
        case 13:        
        case 14:        return "m3";
        case 15:        
        case 16:        return "m3/h";
        case 17:        
        case 18:        return "m3/d";
        case 19:        return "l";
        case 20:        return "kg";
        case 21:        return "N";
        case 22:        return "Nm";
        case 23:        return "Pa";
        case 24:        return "par";
        case 25:        return "J";
        case 26:        return "J/h";
        case 27:        return "W";
        case 28:        return "VA";
        case 29:        return "var";
        case 30:        return "Wh";
        case 31:        return "VAh";
        case 32:        return "varh";
        case 33:        return "A";
        case 34:        return "C";
        case 35:        return "V";
        case 36:        return "V/m";
        case 37:        return "F";
        case 38:        return "ohm";
        case 39:        return "ohmm2/m";
        case 40:        return "Wb";
        case 41:        return "T";
        case 42:        return "A/m";
        case 43:        return "H";
        case 44:        return "Hz";
        case 45:        return "1/(Wh)";
        case 46:        return "1/(varh)";
        case 47:        return "1/(VAh)";
        case 48:        return "V2h";
        case 49:        return "A2h";
        case 50:        return "kg/s";
        case 51:        return "S,mho";
        case 52:        return "K";
        case 53:        return "1/(V2h)";
        case 54:        return "1/(A2h)";
        case 55:        return "1/m3";
        case 56:        return "%";
        case 57:        return "Ah";
        case 60:        return "Wh/m3";
        case 61:        return "J/m3";
        case 62:        return "Mol%";
        case 63:        return "g/m3";
        case 64:        return "Pa s";
        case 254:       return "other";
        case 255:       return "count";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_GetEventType(BYTE event)
{
    switch(event) {
        case ENDI_EVENT_TYPE_RESET:             return "RESET";
        case ENDI_EVENT_TYPE_POWER_OUTAGE:      return "POWER OUTAGE";
        case ENDI_EVENT_TYPE_NETWORK_JOIN:      return "NETWORK JOIN";
        case ENDI_EVENT_TYPE_NETWORK_LEAVE:     return "NETWORK LEAVE";
        case ENDI_EVENT_TYPE_FACTORY_SETTING:   return "FACTORY SETTING";
        case ENDI_EVENT_TYPE_LP_PERIOD_CHANGE:  return "LP PERIOD CHANGE";
        case ENDI_EVENT_TYPE_TIME_OFFSET:       return "TIME OFFSET";
        case ENDI_EVENT_TYPE_LX_MISSING:        return "LX MISSING";
        case ENDI_EVENT_TYPE_SCAN_FAIL:         return "SCAN FAIL";
        case ENDI_EVENT_TYPE_LOW_BATTERY:       return "LOW BATTERY";
        case ENDI_EVENT_TYPE_CASE_OPEN:         return "CASE OPEN";
        case ENDI_EVENT_TYPE_TAMPER:            return "TAMPER";
        case ENDI_EVENT_TYPE_RESERVED_SRAM_CRACK: return "RESERVED SRAM CRACK";
        case ENDI_EVENT_TYPE_KEEP_ALIVE_FAIL:   return "KEEP ALIVE FAIL";
        case ENDI_EVENT_TYPE_DETACHEMENT:       return "DETACHEMENT";
        case ENDI_EVENT_TYPE_PULSE_MO:          return "PULSE MO";
        case ENDI_EVENT_TYPE_WKAE_UP:           return "WAKE UP";
        case ENDI_EVENT_TYPE_NO_PARENT:         return "NO PARENT";
        case ENDI_EVENT_TYPE_BATT_DISC_EN:      return "BATTERY DISCHARGE ENABLE";
        case ENDI_EVENT_TYPE_NETWORK_TYPE:      return "NETWORK TYPE CHANGE";
        case ENDI_EVENT_TYPE_OPTICAL_CMD:       return "OPTICAL COMMAND";
        case ENDI_EVENT_TYPE_PULSE_BACK:        return "DETECT BACK PULSE";
        case ENDI_EVENT_TYPE_ATTACHMENT:        return "ATTACHMENT";
        case ENDI_EVENT_TYPE_STATE_ALARM:       return "STATE ALARM";
        case ENDI_EVENT_TYPE_MSP430_STATUS:     return "MSP340 STATUS";
        case ENDI_EVENT_TYPE_MSP430_RESET:      return "MSP340 RESET";
        case ENDI_EVENT_TYPE_NET_JOIN_LIMITED:  return "JOIN LIMIT";
        case ENDI_EVENT_TYPE_REPEATER_STATUS:   return "PREATER STATUS";
        case ENDI_EVENT_TYPE_NET_KEY_SWITCHING: return "KEY SWITCHING";
        case ENDI_EVENT_TYPE_INVALID_METERING:  return "INVALID MTR";
        case ENDI_EVENT_TYPE_BOOTUP:            return "BOOTUP";
        case ENDI_EVENT_TYPE_EEPROM_LP_FAIL:    return "EEPROM LP FAIL";
        case ENDI_EVENT_TYPE_CPULSE_ERROR:      return "CPULSE ERROR";
        case ENDI_EVENT_TYPE_LP_ROLLBACK:       return "LP ROLLBACK";
        case ENDI_EVENT_TYPE_METER_FW_UPGRADE_FAIL: return "FW UPGRADE FAIL";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_TimeSyncStrategy(BYTE strategy)
{
    switch(strategy) {
        case TIMESYNC_STRATEGY_HIBRID_MESH:  return "Hibrid / Mesh";
        case TIMESYNC_STRATEGY_HIBRID_SLEEPY_MESH:  return "Hibrid + Sleepy Mesh";
        case TIMESYNC_STRATEGY_SLEEPY_MESH:  return "Sleepy Mesh";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_NetworkType(BYTE networkType)
{
    switch(networkType) {
        case 0x00:  return "STAR";
        case 0x01:  return "MESH";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_OpticalCommand(BYTE cmd)
{
    switch(cmd) {
        case 0x00:  return "Boot load";
        case 0x01:  return "Modem Parameter";
        case 0x02:  return "Join";
        case 0x03:  return "Leave";
        case 0x04:  return "Reset";
        case 0x05:  return "Time";
        case 0x06:  return "Current Pulse";
        case 0x07:  return "Alarm Flag";
        case 0x08:  return "LP Period";
        case 0x09:  return "LP Choice";
        case 0x0A:  return "Operating Day";
        case 0x0B:  return "Active Min";
        case 0x0C:  return "Metering Day";
        case 0x0D:  return "Metering Hour";
        case 0x0E:  return "Active Keep Time";
        case 0x0F:  return "RF Power Set";
        case 0x10:  return "Permit Mode";
        case 0x11:  return "Permit State";
        case 0x12:  return "Alarm Mask";
        case 0x13:  return "Network Type";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_DistributionType(BYTE distType)
{
    switch(distType) {
        case 0x00:  return "Concentrator";
        case 0x01:  return "Sensor";
        case 0x02:  return "Coordinator";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_FirmwareType(BYTE fwType)
{
    switch(fwType) {
        case 0x00:  return "Concentrator";
        case 0x01:  return "Coordinator";
        case 0x02:  return "Power Controller";
    }
    return UNKNOWN_STR;
}

void MSGAPI_SysStateMask(UINT mask, char * buffer)
{
    sprintf(buffer, "%s%s%s%s%s",
            (mask & GPIONOTIFY_PWR) ? "P" : "",
            (mask & GPIONOTIFY_LOWBAT) ? "B" : "",
            (mask & GPIONOTIFY_COORDINATOR) ? "C" : "",
            (mask & GPIONOTIFY_MEMORY) ? "M" : "",
            (mask & GPIONOTIFY_FLASH) ? "S" : "");
}

const char *MSGAPI_GetEmberTableStatus(BYTE nStatus)
{
	switch(nStatus) {
        case 0x00:              return "      ";
        case 0xB0:              return "HWMISM";    // EMBER_STACK_AND_HARDWARE_MISMATCH(xB0) 
        case 0xB1:              return "INVKEY";    // EMBER_KEY_INVALID(xB2) 
        case 0xB2:              return "OUTRNG";    // EMBER_INDEX_OUT_OF_RANGE(xB1) 
        case 0xB3:              return "INVADD";    // EMBER_KEY_TABLE_INVALID_ADDRESS(xB3) 
        case 0xB4:              return "TBLFUL";    // EMBER_TABLE_FULL(xB4) 
        case 0xB5:              return "LIBERR";    // EMBER_LIBRARY_NOT_PRESENT(xB5) 
        case 0xB6:              return "ERASED";    // EMBER_TABLE_ENTRY_ERASED(xB6) 
        case 0xB7:              return "INVSEC";    // EMBER_SECURITY_CONFIGURATION_INVALID(xB7) 
        case 0xB8:              return "SWKERR";    // EMBER_TOO_SOON_FOR_SWITCH_KEY(xB8) 
        case 0xB9:              return "VRFERR";    // EMBER_SIGNATURE_VERIFY_FAILURE(xB9) 
        case 0xBA:              return "INPROG";    // EMBER_OPERATION_IN_PROGRESS(xBA)
	}
	return UNKNOWN_STR;
}

const char *MSGAPI_MSP430Status(BYTE nStatus)
{
    switch(nStatus) {
        case 0x00:  return "Status Info";
        case 0x01:  return "Current Pulse, Serial Number";
        case 0x02:  return "Valve On/Off";
        case 0x03:  return "Function Test Result, Version";
    }
    return UNKNOWN_STR;
}

const char *MSGAPI_GeMeterMode(BYTE nMode)
{
    switch(nMode) {
        case 0x00:  return "Demand-only";
        case 0x01:  return "Demand/LP";
        case 0x02:  return "TOU";
    }
    return UNKNOWN_STR;
}

