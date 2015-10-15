#ifndef __SET_COMMAND_H__
#define __SET_COMMAND_H__

int cmdSetId(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetName(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetDescr(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLocation(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetContact(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetTime(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetTimezone(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOpMode(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetPowerType(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetTempRange(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetMobileType(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetMobileMode(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetMobileAPN(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetMobileNumber(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetServerAddress(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetServerPort(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetServerAlarmPort(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetServerSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler);          //  [9/17/2010 DHKim]
int cmdSetLocalPort(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLocalAddress(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetEthernetType(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetCoordinatorAutoSetting(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorChannel(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCooordinatorPanID(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCooordinatorExtPanID(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorRfPower(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorTxPowerMode(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorPermitTime(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorLinkKey(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorNetworkKey(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorDiscovery(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorHops(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetCoordinatorMtorType(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetSensorMeterSerial(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorVendor(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorLocation(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorNetworkType(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorCustomer(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorResetTime(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorTestMode(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorReset(CLISESSION *pSession, int argc, char **argv, void *pHandler);

// AMR Setup
#ifdef  __SUPPORT_ENDDEVICE__
int cmdSetSensorCurrentPulse(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorLastPulse(CLISESSION *pSession, int argc, char **argv, void *pHandler);
#endif
int cmdSetSensorLpPeriod(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorLpChoice(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorSetupSec(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorAlarmFlag(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorOperDay(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorMeteringDay(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorMeteringQuarter(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorActiveMin(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorActiveKeepTime(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorRfPower(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorPermitMode(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorPermitState(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorAlarmMask(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorMeteringFailCnt(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetSensorTime(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorVerify(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorInstall(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorValue(CLISESSION *pSession, int argc, char **argv, void *pHandler);

/** Issue #89 : SX Meter Factory Reset */
int cmdSetSxFactoryReset(CLISESSION *pSession, int argc, char **argv, void *pHandler);

/** 2011.03.21 : Added By Wooks for Smoke Detector and Alarm(Siren) Commands
*/
int cmdSetSensorSiren(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorTemperatureLevel(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int	cmdSetSensorTxPowerMode(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorHeartBeatDay(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorHeartBeatQuarter(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSensorPushMetering(CLISESSION *pSession, int argc, char **argv, void *pHandler); // Issue #495
////////////////////////////////////////////////////////////////////////////////////////////////////

int cmdEnableRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableCmdHist(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableCommLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableAutoRegister(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableAutoReset(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableAutoTimeSync(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableAutoCodiReset(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableSubNetwork(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableMobileLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableAutoUpload(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableKeepalive(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableGpsTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableRetry(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableMemoryCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableMeterFailRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableFlashCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableHighRam(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableTimeBroadcast(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableLowBatt(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableStatusMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableMeterPolling(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableMeterTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableDebugLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableTestMode(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdEnableGarbageCleaning(CLISESSION *pSession, int argc, char **argv, void *pHandler);
#if 0 // Issue #835 스웨덴향 집중기는 join control을 지원하지 않는다
int cmdEnableJoinControl(CLISESSION *pSession, int argc, char **argv, void *pHandler);		//  [4/26/2011 DHKim]
#endif

int cmdDisableRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableCmdHist(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableCommLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableAutoRegister(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableAutoReset(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableAutoTimeSync(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableAutoCodiReset(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableSubNetwork(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableMobileLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableAutoUpload(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableKeepalive(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableGpsTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableRetry(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableMemoryCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableMeterFailRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableFlashCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableHighRam(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableTimeBroadcast(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableStatusMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableMeterPolling(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableMeterTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableLowBatt(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableDebugLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableTestMode(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdDisableGarbageCleaning(CLISESSION *pSession, int argc, char **argv, void *pHandler);
#if 0 // Issue #835 스웨덴향 집중기는 join control을 지원하지 않는다
int cmdDisableJoinControl(CLISESSION *pSession, int argc, char **argv, void *pHandler);			//  [4/26/2011 DHKim]
#endif

int cmdSetOptionScanningStrategy(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionMeteringThread(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionRetry(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionAutoreset(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionKeepalive(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionSensorLimit(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionAutouploadWeekly(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionAutouploadDaily(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionAutouploadHourly(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionAutouploadImmediately(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionMeteringSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionRecoverySchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionEventReadSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionPollingSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionTimesyncSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionTimesyncThreshold(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionTimesyncStrategy(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionNetworkTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetOptionMemorycheck(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionFlashcheck(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionMobileLinkCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionPingMethod(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionTimebroadcast(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionStatusMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetOptionSchedulerType(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionMeteringSaveday(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionMeteringInterval(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionMeteringFixhour(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionMeterUser(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionMeterSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetOptionEventAlertLevel(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionEventNotify(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionEventSeverity(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionEventConfigurationDefault(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionEventSendDelay(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionMeteringType(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionSensorCleaningThreshold(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionTransactionThreshold(CLISESSION *pSession, int argc, char **argv, void *pHandler);
 
int cmdSetSavedayEventLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSavedayAlarmLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSavedayCmdLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSavedayCommLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSavedayMobileLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSavedayMeterLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSavedayUpgradeLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSavedayUploadLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSavedayTimesyncLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetSavedayDebugLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetLimitMobileLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLimitEventLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLimitCommLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLimitAlarmLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLimitCmdLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLimitMeterLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLimitUpgradeLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLimitUploadLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLimitTimesyncLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetLimitDebugLog(CLISESSION *pSession, int argc, char **argv, void *pHandler);

int cmdSetGpio(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetPhoneList(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetPatch(CLISESSION *pSession, int argc, char **argv, void *pHandler);
int cmdSetOptionDumpFile(CLISESSION *pSession, int argc, char **argv, void *pHandler);

// 실증단지 Group 
int cmdAddGroup(CLISESSION *pSession, int argc, char **argv, void *pHandler);

#endif	// __SET_COMMAND_H__
