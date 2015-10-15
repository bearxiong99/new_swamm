#ifndef __CMD_MESSAGE_H__
#define __CMD_MESSAGE_H__

const char *MSGAPI_GetServiceType(int nType);
const char *MSGAPI_GetEtherType(int nType);
const char *MSGAPI_GetMobileType(int nType);
const char *MSGAPI_GetMobileMode(int nType);
const char *MSGAPI_GetMobileAPN(int nType);
const char *MSGAPI_GetResetReason(int nType);
const char *MSGAPI_GetSensorType(BYTE nType);
const char *MSGAPI_GetSensorVendor(BYTE nType, BYTE nVendor);
const char *MSGAPI_GetSensorActiveState(BYTE nFlag);
const char *MSGAPI_GetSinkType(BYTE nType);
const char *MSGAPI_GetCodiType(BYTE nType);
const char *MSGAPI_GetUserType(BYTE nType);
const char *MSGAPI_GetUploadCycleType(BYTE nType);
const char *MSGAPI_GetUploadCycle(char *pszBuffer, BYTE nType, unsigned int nCycle);
const char *MSGAPI_GetTxPowerMode(BYTE nType);
const char *MSGAPI_GetResetKind(BYTE nType);
const char *MSGAPI_GetSensorState(BYTE nType);
const char *MSGAPI_GetEventLevelString(int nLevel);
const char *MSGAPI_PingMethod(int nMethod);
const char *MSGAPI_ScanningStrategy(int nStrategy);
const char *MSGAPI_GetOpMode(int nState);
const char *MSGAPI_GetMeteringType(int nType, char* buffer);
const char *MSGAPI_GetPowerType(BYTE nType);
void  MSGAPI_GetAlarmMask(WORD alarmMask, char * buffer);
const char *MSGAPI_GetDlmsUnit(BYTE unit);
const char *MSGAPI_GetEventType(BYTE event);
const char *MSGAPI_TimeSyncStrategy(BYTE strategy);
const char *MSGAPI_NetworkType(BYTE networkType);
const char *MSGAPI_OpticalCommand(BYTE cmd);
const char *MSGAPI_DistributionType(BYTE distType);
const char *MSGAPI_FirmwareType(BYTE fwType);
void MSGAPI_SysStateMask(UINT mask, char * buffer);
const char *MSGAPI_GetEmberTableStatus(BYTE nStatus);
const char *MSGAPI_MSP430Status(BYTE nStatus);
const char *MSGAPI_GeMeterMode(BYTE nMode);

#endif	// __CMD_MESSAGE_H__
