#ifndef __COMMAND_HANDLER_H__
#define __COMMAND_HANDLER_H__
#define __GROUP_SUPPORT_MODE__

extern	IF4_COMMAND_TABLE	m_CommandHandler[];

// deprecated function
int cmdDeprecatedFunction(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// MCU Control Command
int cmdMcuDiagnosis(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuReset(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuShutdown(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuFactoryDefault(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuGetTime(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuSetTime(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuGetState(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuLoopback(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuClearStatic(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuGetSystemInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuSetSystemInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuGetEnvironment(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuSetEnvironment(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuGetGpio(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuSetGpio(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdResetDevice(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetFwInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

int cmdMcuInstall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuUninstall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdOamLogin(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdOamLogout(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// Coordinator Control Command
int cmdGetCodiList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetCodiInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetCodiInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetCodiDevice(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetCodiDevice(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetCodiBinding(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetCodiNeighbor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetCodiMemory(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetCodiPermit(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetCodiFormNetwork(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetCodiReset(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdUpdateCoordinator(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetCodiRegister(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetCodiRegister(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDeleteCodiRegister(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// SENSOR Control Command
int cmdAddSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDeleteSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDeleteSensorAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdPingSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorCount(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetSensorTime(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorMultiROM(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetSensorROM(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetMbusROM(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorAllNew(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorInfoNew(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorAmrData(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetSensorAmrData(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdResetSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdUpdateSensorFirmware(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdScanSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdCommandSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdExecuteCommand(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorPath(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetFFDList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdAsynchronousCall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdShowTransactionList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdShowTransactionInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdModifyTransaction(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorEvent(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorBattery(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetMeterList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDigitalInOut(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorConstants(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorValue(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSendMessage(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdPollMessage(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

#ifdef	__GROUP_SUPPORT_MODE__
int cmdGroupAsyncCall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);						//  [10/13/2010 DHKim]
int cmdGroupAddMember(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);					//  [10/13/2010 DHKim]
int cmdGroupDeleteMember(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);				//  [10/13/2010 DHKim]
int cmdGroupAdd(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);							//  [10/22/2010 DHKim]
int cmdGroupDelete(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);							//  [10/22/2010 DHKim]
int cmdGroupInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);							//  [10/22/2010 DHKim]
int cmdGroupShowCommand(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);				//  [10/22/2010 DHKim]
int cmdGroupModify(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);							//  [11/1/2010 DHKim]
int cmdGroupMemberModify(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);				//  [11/1/2010 DHKim]
int cmdGroupTableDelete(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);					//  [4/27/2011 DHKim]
#endif

int cmdUpdateGroup(IF4Wrapper *pWrapper,  MIBValue *pValue, int cnt);               // 실증단지 Group 정보 갱신
int cmdDeleteGroup(IF4Wrapper *pWrapper,  MIBValue *pValue, int cnt);               // 실증단지 Group 정보 삭제
int cmdGetGroup(IF4Wrapper *pWrapper,  MIBValue *pValue, int cnt);                  // 실증단지 Group 정보 조회

// Log Command
int cmdClearLog(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetHistList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetMeterLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetEventLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetCommLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetMobileLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetAllLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// Metering Command
int cmdMetering(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMeteringAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdRecovery(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetMeter(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetMeterAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdOnDemandMeter(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdOnDemandMeterAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdOnDemand(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdOndemandGe(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetMeterSchedule(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdAidonMCCB(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdKamstrupCID(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdLiveCheck(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdOnDemandMbus(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetLoadProfile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetMeterSecurity(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetMeterSecurity(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// GE Only
int cmdReadTable(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdWriteTable(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdReadTableAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdCheckMeter(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

int cmdSyncMeterTime(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSyncMeterTimeAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// Session & Message
int cmdBypassSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// System User Command
int cmdAddUser(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDeleteUser(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetUser(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetUser(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdUserList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// Phone List
int cmdGetPhoneList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetPhoneList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSCID(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// APN Command
int cmdAddApn(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDeleteApn(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetApnList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetApnOption(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
BOOL IsOptionEnabled(const char *szOption, MIBValue *pValue, int cnt, int *nValueCnt = NULL);

// Firmware Command
int cmdFirmwareUpdate(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdShellCommand(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdCheck(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSensorTimesync(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// File Transfer Command
int cmdGetFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdPutFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdInstallFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetFirmware(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDistributeFirmware(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdPackageDistribution(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMergeAndInstall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// New Package Distribution (Concentrator, Coordinator, Sensor 통합)
int cmdDistribution(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDistributionState(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDistributionCancel(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDistributionList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDistributionInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDistributionSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// Common Control Command
int cmdStdGet(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdStdSet(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdStdList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdStdGetChild(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMeterUpload(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

int cmdSetTestMode(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetPingTestMode(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

int cmdGetConfiguration(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetConfiguration(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

int cmdGetEventConfigure(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetEventConfigure(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

int cmdMcuGetPatch(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuSetPatch(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMcuCleaning(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

//mbus
int cmdDeleteMeter(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// Energy Level Management
int cmdGetDRAssetInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetEnergyLevel(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetEnergyLevel(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdSetIHDTable(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdDelIHDTable(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
#endif	// __COMMAND_HANDLER_H__
