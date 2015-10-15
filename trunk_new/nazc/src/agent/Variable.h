#ifndef __VARIABLE_H__
#define __VARIABLE_H__

extern VAROBJECT	m_Root_node[];
extern VAROBJECT	m_smi_node[];
extern VAROBJECT	m_smiEntry_node[];
extern VAROBJECT	m_unknownEntry_node[];
extern VAROBJECT	m_boolEntry_node[];
extern VAROBJECT	m_byteEntry_node[];
extern VAROBJECT	m_wordEntry_node[];
extern VAROBJECT	m_uintEntry_node[];
extern VAROBJECT	m_charEntry_node[];
extern VAROBJECT	m_shortEntry_node[];
extern VAROBJECT	m_intEntry_node[];
extern VAROBJECT	m_oidEntry_node[];
extern VAROBJECT	m_oidDot1_node[];
extern VAROBJECT	m_oidDot2_node[];
extern VAROBJECT	m_oidDot3_node[];
extern VAROBJECT	m_stringEntry_node[];
extern VAROBJECT	m_streamEntry_node[];
extern VAROBJECT	m_opaqueEntry_node[];
extern VAROBJECT	m_eui64Entry_node[];
extern VAROBJECT	m_ipEntry_node[];
extern VAROBJECT	m_timeEntry_node[];
extern VAROBJECT	m_dayEntry_node[];
extern VAROBJECT	m_verEntry_node[];
extern VAROBJECT	m_smivarEntry_node[];
extern VAROBJECT	m_pageEntry_node[];
extern VAROBJECT	m_gmtEntry_node[];
extern VAROBJECT	m_mcu_node[];
extern VAROBJECT	m_sysEntry_node[];
extern VAROBJECT	m_varEntry_node[];
extern VAROBJECT	m_varValueEntry_node[];
extern VAROBJECT	m_varGeEntry_node[];
extern VAROBJECT	m_varEvtCfgEntry_node[];
extern VAROBJECT	m_varSubValueEntry_node[];
extern VAROBJECT	m_usrEntry_node[];
extern VAROBJECT	m_gpsEntry_node[];
extern VAROBJECT    m_mobileCfgEntry_node[];
extern VAROBJECT	m_memEntry_node[];
extern VAROBJECT	m_flashEntry_node[];
extern VAROBJECT	m_ethEntry_node[];
extern VAROBJECT	m_gpioEntry_node[];
extern VAROBJECT    m_gpioCtrlEntry_node[];
extern VAROBJECT	m_mobileEntry_node[];
extern VAROBJECT	m_pluginEntry_node[];
extern VAROBJECT	m_procEntry_node[];
extern VAROBJECT	m_pppEntry_node[];
extern VAROBJECT	m_envEntry_node[];
extern VAROBJECT	m_patchEntry_node[];
extern VAROBJECT	m_fwEntry_node[];
extern VAROBJECT	m_commLogEntry_node[];
extern VAROBJECT	m_debugLogEntry_node[];
extern VAROBJECT    m_commLogStruct_node[];
extern VAROBJECT	m_cmdHistEntry_node[];
extern VAROBJECT    m_cmdHistStruct_node[];
extern VAROBJECT	m_mcuEventEntry_node[];
extern VAROBJECT    m_mcuEventStruct_node[];
extern VAROBJECT	m_meterLogEntry_node[];
extern VAROBJECT	m_mobileLogEntry_node[];
extern VAROBJECT	m_mobileLogStruct_node[];

extern VAROBJECT	m_sink_node[];
extern VAROBJECT	m_sinkStaticEntry_node[];
extern VAROBJECT    m_codiEntry_node[];
extern VAROBJECT    m_codiDeviceEntry_node[];
extern VAROBJECT    m_codiBindingEntry_node[];
extern VAROBJECT    m_codiNeighborEntry_node[];
extern VAROBJECT    m_codiMemoryEntry_node[];
extern VAROBJECT    m_codiRegEntry_node[];

extern VAROBJECT	m_sensor_node[];
extern VAROBJECT	m_sensorConnectEntry_node[];
extern VAROBJECT	m_sensorInfoEntry_node[];
extern VAROBJECT	m_sensorInfoNewEntry_node[];
extern VAROBJECT	m_sensorPathEntry_node[];
extern VAROBJECT	m_sensorEventEntry_node[];
extern VAROBJECT	m_sensorBatteryEntry_node[];
extern VAROBJECT	m_sensorConstantEntry_node[];

extern VAROBJECT	m_network_node[];
extern VAROBJECT	m_ffdEntry_node[];

extern VAROBJECT	m_transaction_node[];
extern VAROBJECT	m_trInfoEntry_node[];
extern VAROBJECT	m_trHistoryEntry_node[];

//  [11/10/2010 DHKim]
extern VAROBJECT	m_group_node[];
extern VAROBJECT	m_groupInfoEntry_node[];
extern VAROBJECT m_commandInfoEntry_node[];

extern VAROBJECT	m_meter_node[];
extern VAROBJECT    m_meterLogStruct_node[];
extern VAROBJECT	m_meterDataEntry_node[];
extern VAROBJECT	m_meterEntry_node[];
extern VAROBJECT	m_meterLPEntry_node[];
extern VAROBJECT    m_meterEntry_mbus[];

extern VAROBJECT    m_sinkEntry_node[];

extern VAROBJECT	m_eventMcu_node[];
extern VAROBJECT	m_eventSink_node[];
extern VAROBJECT	m_eventMobile_node[];
extern VAROBJECT	m_eventSensor_node[];
extern VAROBJECT	m_eventMalf_node[];
extern VAROBJECT	m_eventComm_node[];
extern VAROBJECT	m_Alarm_node[];
extern VAROBJECT	m_eventMeter_node[];
extern VAROBJECT	m_eventTransaction_node[];
extern VAROBJECT	m_eventOta_node[];
extern VAROBJECT	m_eventManagement_node[]; 

typedef struct _tagSYSENTRY
{
	UINT        sysMask;
	UINT        sysID;
	BYTE        sysType;
	char        sysName[64];
	char        sysDescr[64];
	char        sysLocation[64];
	char        sysContact[64];
	BYTE        sysHwVersion;
	BYTE        sysSwVersion;
	char        sysPhoneNumber[16];
	BYTE        sysEtherType;
	BYTE        sysMobileType;
	BYTE        sysMobileMode;
	BYTE        sysMobileAPN;
	UINT        sysUpTime;
	TIMESTAMP   sysTime;
	int         sysCurTemp;
	int         sysMinTemp;
	int         sysMaxTemp;
	char        sysServer[256];
	UINT        sysServerPort;
	UINT        sysServerAlarmPort;
	UINT        sysLocalPort;
	BYTE        sysState;
	BYTE        sysVendor;
	BYTE        sysModel;
	BYTE		sysCircuitMode;					// Not use (SWAMM)
	UINT		sysMobileVendor;
	char		sysMobileAccessPointName[64];
	char        sysSwRevision[32];
	BYTE        sysResetReason;
    UINT        sysJoinNodeCount;
    short       sysTimeZone;
    BYTE        sysOpMode;
    UINT        sysPowerType;
    UINT        sysStateMask;
    BYTE        sysSecurityState;               // [9/13/2010 DHKim] SSL Security State
} __attribute__ ((packed)) SYSENTRY;

typedef struct _tagGPSENTRY
{
    BYTE        gpsFixPosition;
    BYTE        gpsCount;
    TIMESTAMP   gpsTime;
    char        gpsPos[32];
    int        	gpsTimezone;
} __attribute__ ((packed)) GPSENTRY;

typedef struct _tagVARENTRY
{
	UINT        varEnableMask;
	BOOL        varEnableRecovery;
	BOOL        varEnableCmdHistLog;
	BOOL        varEnableCommLog;
	BOOL        varEanbleAutoRegister;
	BOOL        varEnableAutoReset;
	BOOL        varEnableAutoTimeSync;
	BOOL        varEnableAutoSinkReset;
	BOOL        varEnableSubNetwork;
	BOOL        varEnableMobileStaticLog;
	BOOL        varEnableAutoUpload;
	BOOL        varEnableSecurity;
    BOOL        varEnableMonitoring;
    BOOL        varEnableKeepalive;
    BOOL        varEnableGpsTimesync;
    BOOL        varEnableTimeBroadcast;
    BOOL        varEnableStatusMonitoring;
    BOOL        varEnableMemoryCheck;
    BOOL        varEnableFlashCheck;
    BOOL		varEnableMeterTimesync;
    BOOL		varEnableMeterCheck;
	BOOL		varEnableHeater;
	BOOL		varEnableReadMeterEvent;
	BOOL		varEnableLowBattery;
	BOOL		varEnableTestMode;
	BOOL        varEnableDebugLog;
	BOOL        varEnableMeterRecovery;
	BOOL        varEnableHighRAM;
	BOOL        varEnableGarbageCleaning;
	BOOL			varEnableJoinControl;
} __attribute__ ((packed)) VARENTRY;

typedef struct _tagVARVALUEENTRY
{
	UINT        varValueMask;
	UINT        varAutoResetCheckInterval;
	UINT        varSysPowerOffDelay;
	UINT        varSysTempMonInterval;
	UINT        varAutoTimeSyncInterval;
	BYTE        varSysMeteringThreadCount;
	UINT        varSinkPollingInterval;
	UINT        varSinkResetInterval;
	UINT        varSinkResetDelay;
	UINT        varSinkLedTurnOffInterval;
	BYTE        varSinkAckWaitTime;
	BYTE        varSensorTimeout;
	BYTE        varSensorKeepalive;
	WORD        varSensorMeterSaveCount;
	UINT        varMeterDayMask;
	UINT        varMeterHourMask;
	BYTE        varMeterStartMin;
	UINT        varRecoveryDayMask;
	UINT        varRecoveryHourMask;
	BYTE        varRecoveryStartMin;
	BYTE        varCmdHistSaveDay;
	BYTE        varEventLogSaveDay;
	BYTE        varCommLogSaveDay;
	BYTE        varMeterLogSaveDay;
	TIMESTAMP   varAutoResetTime;
	BYTE        varMeterUploadCycleType;
	UINT        varMeterUploadCycle;
	BYTE        varMeterUploadStartHour;
	BYTE        varMeterUploadStartMin;
	BYTE        varMeterUploadRetry;
	TIMESTAMP   varMeterIssueDate;
	BYTE        varMemoryCriticalRate;
	BYTE        varFlashCriticalRate;
	BYTE		varNapcGroupInterval;
	BYTE		varNapcRetry;
	BYTE		varNapcRetryHour;
	BYTE		varNapcRetryStartSecond;
	UINT		varNapcRetryClear;
    UINT        varMaxEventLogSize;
    UINT        varMaxAlarmLogSize;
    UINT        varMaxCmdLogSize;
    UINT        varMaxCommLogSize;
    UINT        varMaxMobileLogSize;
    UINT        varKeepaliveInterval;
    BYTE        varAlarmLogSaveDay;
    UINT        varTimeBroadcastInterval;
    BYTE        varStatusMonitorTime;
	UINT		varUploadTryTime;
	UINT		varDataSaveDay;
	UINT		varMeteringPeriod;
	UINT		varRecoveryPeriod;
	UINT		varMeteringRetry;
	UINT		varRecoveryRetry;
	UINT		varCheckDayMask;
	UINT		varCheckHourMask;
	BYTE		varCheckStartMin;
    UINT		varCheckPeriod;
    UINT		varCheckRetry;
	UINT		varMeterTimesyncDayMask;
	UINT		varMeterTimesyncHourMask;
	UINT		varMeterReadingInterval;				// Not use
	int			varHeaterOnTemp;						// 히터 켜기 동작 온도
	int			varHeaterOffTemp;						// 히터 끄기 중지 온도
	int			varMobileLiveCheckInterval;				// 모바일 연결 상태 검사 주기 (ICMP Ping)
	UINT		varEventReadDayMask;					// GE 미터 이벤트 로그 읽기 일자 MASK
	UINT		varEventReadHourMask;					// GE 미터 이벤트 로그 읽기 시간 MASK
	UINT		varEventSendDelay;						// 대용량 이벤트 전송 딜레이
	BYTE		varEventAlertLevel;						// 서버로 이벤트를 전송하는 최소 등급
	int 		varSensorLimit;						    // MCU에서 관리할 수 있는 Sensor 갯수
    BYTE        varMeteringStrategy;                    // Metering Strategy
    int         varTimesyncThresholdLow;                // Timesync Threshold (Low)
	BYTE		varMobileLiveCheckMethod;				// 모바일 상태 검사 방법
	BYTE		varScanningStrategy;				    // Inventory Scanning Strategy
	BYTE		varMobileLogSaveDay;				    // 
	BYTE		varUpgradeLogSaveDay;				    // 
	BYTE		varUploadLogSaveDay;				    // 
	BYTE		varTimesyncLogSaveDay;				    // 
    UINT        varMaxMeterLogSize;
    UINT        varMaxUpgradeLogSize;
    UINT        varMaxUploadLogSize;
    UINT        varMaxTimesyncLogSize;
    BYTE        varDefaultMeteringOption;               // Parser에서 사용할 Default option
    BYTE        varSensorCleaningThreshold;
    BYTE        varTimeSyncStrategy;
    BYTE        varTransactionSaveDay;
    BYTE        varSchedulerType;
    int         varTimesyncThresholdHigh;               // Timesync Threshold (High)
} __attribute__ ((packed)) VARVALUEENTRY;

typedef struct _tagVARGEENTRY
{
	UINT		varGeMeteringInterval;
	BYTE		varGeUser[10];
	BYTE		varGeMeterSecurity[20];
	BYTE		varGeAllUploadHour;
} __attribute__ ((packed)) VARGEENTRY;

typedef struct	_tagEVTCFGENTRY
{
		char		szOid[16];
		char		szEvent[64];
		BYTE		nClass;				// Event Class
        BYTE        nDefaultClass;      // Event Default Class
        BYTE        bNotify;            // FEP로의 Notification 여부
} __attribute__ ((packed)) EVTCFGENTRY;

typedef struct _tagMOBILECFGENTRY
{
	char        moduleType[32];                 // Module type
    UINT        moduleFlag;                     // Module flags
    char        interface[64];                  // main interface path
    UINT        interfaceSpeed;                 // speed or port number
    char        sms[64];                        // short message lookup path
    UINT        smsSpeed;                       // sms lookup speed 
} __attribute__ ((packed)) MOBILECFGENTRY;

typedef struct _tagVARSUBVALUEENTRY
{
	UINT        varSubValueMask;
	BYTE        varSubChannel;
	WORD        varSubPanID;
} __attribute__ ((packed)) VARSUBVALUEENTRY;

typedef struct _tagUSRENTRY
{
	BYTE        usrMask;
	char        usrAccount[16];
	char        usrPassword[16];
	BYTE        usrGroup;
} __attribute__ ((packed)) USRENTRY;

typedef struct _tagMEMENTRY
{
	UINT        memTotalSize;
	UINT        memFreeSize;
	UINT        memUseSize;
	UINT        memCacheSize;
	UINT        memBufferSize;
} __attribute__ ((packed)) MEMENTRY;

typedef struct _tagFLASHENTRY
{
	UINT        flashTotalSize;
	UINT        flashFreeSize;
	UINT        flashUseSize;
} __attribute__ ((packed)) FLASHENTRY;

typedef struct _tagETHENTRY
{
	char        ethName[16];
	char        ethPhyAddr[8];
	UINT        ethIpAddr;
	UINT        ethSubnetMask;
	UINT        ethGateway;
} __attribute__ ((packed)) ETHENTRY;

typedef struct _tagGPIOENTRY
{
	UINT        gpioMask;
	BYTE        gpioPowerFail;
	BYTE        gpioLowBattery;
	BYTE        gpioBUSY;
	BYTE        gpioSTBY;
	BYTE        gpioDCD;
	BYTE        gpioRI;
	BYTE        gpioSink1State;
	BYTE        gpioSink2State;
	BYTE		gpioHeaterState;
	BYTE		gpioCoverState;
	BYTE		gpioCharginState;
} __attribute__ ((packed)) GPIOENTRY;

typedef struct _tagGPIOCTRLENTRY
{
	BYTE		gpioPowerCtrl;
	BYTE		gpioReset;
	BYTE		gpioMobilePower;
	BYTE		gpioMobileReset;
	BYTE		gpioMobileDtr;
	BYTE		gpioSink1Power;
	BYTE		gpioSink2Power;
	BYTE		gpioExternalPower;
	BYTE		gpioLedSink1;
	BYTE		gpioLedSink2;
	BYTE		gpioLedReady;
	BYTE		gpioLedDebug1;
	BYTE		gpioLedDebug2;
	BYTE		gpioLedDebug3;
	BYTE		gpioLedDebug4;
	BYTE		gpioEmergencyOff;
	BYTE		gpioCoordinatorReset;
} __attribute__ ((packed)) GPIOCTRLENTRY;

typedef struct _tagMOBILEENTRY
{
	char        mobileID[16];
	UINT        mobileIpaddr;
	int         mobileRxDbm;
	int         mobileTxDbm;
	UINT        mobilePacketLiveTime;
	UINT        mobileSendBytes;
	UINT        mobileRecvBytes;
	TIMESTAMP   mobileLastConnectTime;
	char		mobileSimCardId[20];
	UINT		mobileVolt;
} __attribute__ ((packed)) MOBILEENTRY;

typedef struct _tagPLUGINENTRY
{
	char        pluginName[16];
	char        pluginFileName[32];
	char        pluginDescr[64];
	BYTE        pluginType;
	BYTE        pluginState;
	char        pluginVersion[8];
} __attribute__ ((packed)) PLUGINENTRY;

typedef struct _tagPROCENTRY
{
	UINT        procID;
	char        procTTY[8];
	UINT        procSize;
	BYTE        procState;
	char        procCmd[50];
} __attribute__ ((packed)) PROCENTRY;

typedef struct _tagPPPENTRY
{
	char        pppProvider[64];
	char        pppUser[32];
	char        pppPassword[32];
} __attribute__ ((packed)) PPPENTRY;

typedef struct _tagENVENTRY
{
	char        envKernelVersion[64];
	char        envGccVersion[64];
	char        envCpuName[64];
	UINT        envCpuMips;
} __attribute__ ((packed)) ENVENTRY;

typedef struct _tagPATCHENTRY
{
	UINT        patchID;
	UINT        patchIssueNumber;
    BOOL        patchState;
	char        patchDescr[128];
} __attribute__ ((packed)) PATCHENTRY;

typedef struct _tagFWENTRY
{
    BYTE        fwType;
    char        fwCore[64];
	WORD        fwHardware;
	WORD        fwSoftware;
	WORD        fwBuild;
} __attribute__ ((packed)) FWENTRY;

typedef struct _tagCOMMLOGENTRY
{
	TIMESTAMP   commLogDate;
	char        commLogFileName[32];
	UINT        commLogFileSize;
} __attribute__ ((packed)) COMMLOGENTRY;

typedef struct _tagDEBUGLOGENTRY
{
	TIMESTAMP   debugLogDate;
	char        debugLogFileName[32];
	UINT        debugLogFileSize;
} __attribute__ ((packed)) DEBUGLOGENTRY;


typedef struct _tagCOMMLOGSTRUCT
{
	BYTE        commLogLength;
	BYTE        commLogMin;
	BYTE        commLogSec;
	char        commLogData[0];
} __attribute__ ((packed)) COMMLOGSTRUCT;

typedef struct _tagCMDHISTENTRY
{
	TIMESTAMP   cmdHistDate;
	char        cmdHistFileName[32];
	UINT        cmdHistFileSize;
} __attribute__ ((packed)) CMDHISTENTRY;

typedef struct _tagCMDHISTSTRUCT
{
	WORD        cmdHistLength;
	BYTE        cmdHistTimeHour;
	BYTE        cmdHistTimeMin;
	BYTE        cmdHistTimeSec;
	BYTE        cmdHistOwner;
	char        cmdHistUser[16];
	char        cmdHistData[0];
} __attribute__ ((packed)) CMDHISTSTRUCT;

typedef struct _tagMCUEVENTENTRY
{
	TIMESTAMP   mcuEvnetDate;
	char        mcuEventFileName[32];
	UINT        mcuEventFileSize;
} __attribute__ ((packed)) MCUEVENTENTRY;

typedef struct _tagMCUEVENTSTRUCT
{
	WORD        mcuEventLength;
	char        mcuEventData[0];
} __attribute__ ((packed)) MCUEVENTSTRUCT;

typedef struct _tagMETERLOGENTRY
{
	TIMESTAMP   meterLogDate;
	char        meterLogFileName[32];
	UINT        meterLogFileSize;
} __attribute__ ((packed)) METERLOGENTRY;

typedef struct _tagMETERLOGSTRUCT
{
	EUI64       meterLogID;
	BYTE        meterLogType;
	TIMESTAMP   meterLogTime;
	TIMESTAMP   meterLogStartTime;
	TIMESTAMP   meterLogEndTime;
	BYTE        meterLogResult;
	UINT        meterLogElapse;
	BYTE        meterLogTryCount;
} __attribute__ ((packed)) METERLOGSTRUCT;

typedef struct _tagMOBILELOGENTRY
{
	TIMEDATE    mobileLogDate;
	char        mobileLogFileName[32];
	UINT        mobileLogFileSize;
} __attribute__ ((packed)) MOBILELOGENTRY;

typedef struct _tagMOBILELOGSTRUCT
{
} __attribute__ ((packed)) MOBILELOGSTRUCT;

typedef struct _tagCODIENTRY
{
	WORD        codiMask;
	BYTE        codiIndex;
	EUI64       codiID;
	BYTE        codiType;
	WORD        codiShortID;
	BYTE        codiFwVer;
	BYTE        codiHwVer;
	BYTE        codiZAIfVer;
	BYTE        codiZZIfVer;
	BYTE        codiFwBuild;
	BYTE        codiResetKind;
	BYTE        codiAutoSetting;
	BYTE        codiChannel;
	WORD        codiPanID;
	char        codiExtPanID[8];
	signed char codiRfPower;
	BYTE        codiTxPowerMode;
	BYTE        codiPermit;
	BYTE		codiEnableEncrypt;
	char        codiLineKey[16];
	char        codiNetworkKey[16];
    BOOL        codiRouteDiscovery;
    BOOL        codiMulticastHops;
} __attribute__ ((packed)) CODIENTRY;

typedef struct _tagCODIDEVICEENTRY
{
	BYTE        codiIndex;
	EUI64       codiID;
	char        codiDevice[16];
	BYTE        codiBaudRate;
	BYTE        codiParityBit;
	BYTE        codiDataBit;
	BYTE        codiStopBit;
	BYTE        codiRtsCts;
} __attribute__ ((packed)) CODIDEVICEENTRY;

typedef struct _tagCODIBINDINGENTRY
{
	BYTE        codiBindIndex;
	BYTE        codiBindType;
	BYTE        codiBindLocal;
	BYTE        codiBindRemote;
	EUI64       codiBindID;
	WORD        codiLastHeard;
} __attribute__ ((packed)) CODIBINDINGENTRY;

typedef struct _tagCODINEIGHBORENTRY
{
	BYTE        codiNeighborIndex;
	WORD        codiNeighborShortID;
	BYTE        codiNeighborLqi;
	BYTE        codiNeighborInCost;
	BYTE        codiNeighborOutCost;
	BYTE        codiNeighborAge;
	EUI64       codiNeighborID;
} __attribute__ ((packed)) CODINEIGHBORENTRY;

typedef struct _tagCODIMEMORYENTRY
{
	BYTE        codiIndex;
	EUI64       codiID;
	BYTE        codiAddressTableSize;
	BYTE        codiWholeAddressTableSize;
	BYTE        codiNeighborTableSize;
	BYTE        codiSourceRouteTableSize;
	BYTE        codiRouteTableSize;
	BYTE        codiMaxHops;
	BYTE        codiPacketBufferCount;
	WORD        codiSoftwareVersion;
	BYTE        codiKeyTableSize;
	BYTE        codiMaxChildren;
} __attribute__ ((packed)) CODIMEMORYENTRY;

typedef struct _tagCODIREGENTRY
{
	BYTE        codiRegIndex;
    BYTE        codiRegStatus;      // 0x00(Valid), 0xB3(Invalid Address), 0xB7(Security Configuration Invalid)
	EUI64       codiRegID;
} __attribute__ ((packed)) CODIREGENTRY;

typedef struct _tagSENSORCONNECTENTRY
{
	EUI64       sensorID;
	BYTE        sensorType;
	TIMESTAMP   sensorLastConnect;
	TIMESTAMP   sensorLastDisconnect;
	TIMESTAMP   sensorInstallDate;
	BYTE        sensorPageCount;
	BYTE        sensorState;
	BYTE        sensorFlag;
	BYTE        sensorVendor;
	char        sensorSerialNumber[20];
	BYTE        sensorServiceType;
} __attribute__ ((packed)) SENSORCONNECTENTRY;

typedef struct _tagSENSORINFOENTRY
{
	BYTE        sensorGroup;
	BYTE        sensorMember;
	EUI64       sensorID;
	char        sensorSerialNumber[18];
	TIMESTAMP   sensorLastConnect;
	TIMESTAMP   sensorInstallDate;
	BYTE		sensorAttr;
	BYTE		sensorState;
} __attribute__ ((packed)) SENSORINFOENTRY;

typedef struct _tagSENSORINFOENTRYNEW
{
	EUI64       sensorID;
	char        sensorSerialNumber[20];
	TIMESTAMP   sensorLastConnect;
	TIMESTAMP   sensorInstallDate;
	BYTE		sensorAttr;
	BYTE		sensorState;
	char		sensorModel[18];
	WORD		sensorHwVersion;
	WORD		sensorFwVersion;
	BYTE		sensorOTAState;
	TIMESTAMP   sensorLastOTATime;
	WORD		sensorFwBuild;
    UINT        sensorParser;       // RF-2
} __attribute__ ((packed)) SENSORINFOENTRYNEW;

typedef struct _tagSENSORCONSTANTENTRY
{
    INT         powerVoltageScaleInt;
    UINT        powerVoltageScaleReal;
    INT         powerCurrentScaleInt;
    UINT        powerCurrentScaleReal;
} __attribute__ ((packed)) SENSORCONSTANTENTRY;

typedef struct _tagSENSORPATHENTRY
{
	EUI64		sensorPathID;
	char		sensorPathSerial[20];
	char		sensorPathModel[18];
	WORD		sensorPathShortID;
	BYTE		sensorPathHops;
	WORD		sensorPathNode[30];
    TIMESTAMP   sensorPathTime;
    BYTE        sensorPathLQI;
    signed char sensorPathRSSI;
} __attribute__ ((packed)) SENSORPATHENTRY;

typedef struct _tagSENSOREVENTENTRY
{
    TIMESTAMP   sevtTime;
    BYTE        sevtType;
    BYTE        sevtStatus[4];
} __attribute__ ((packed)) SENSOREVENTENTRY;

typedef struct _tagSENSORBATTERYENTRY
{
    WORD        battVolt;
    WORD        battCurrent;
    BYTE        battOffset;
} __attribute__ ((packed)) SENSORBATTERYENTRY;

typedef struct _tagFFDENTRY
{
	EUI64		ffdID;
	char		ffdModel[18];
    TIMESTAMP   ffdLastPoll;
    TIMESTAMP   ffdLastPush;
	WORD		ffdHwVersion;
	WORD		ffdFwVersion;
	WORD		ffdFwBuild;
	BYTE		ffdPowerType;
	BYTE		ffdPermitMode;
	BYTE		ffdPermitState;
	BYTE		ffdMaxChildTbl;
	BYTE		ffdCurrentChild;
	WORD		ffdAlarmMask;
	BYTE		ffdAlarmFlag;
	BYTE		ffdTestFlag;
	WORD		ffdSolarAdVolt;
	WORD		ffdSolarChgBattVolt;
	WORD		ffdSolarBoardVolt;
    BYTE        ffdSolarBattChgState;
} __attribute__ ((packed)) FFDENTRY;


typedef struct _tagTRINFOENTRY
{
	EUI64		triTargetID;
    OID3        triCommand;
    WORD        triID;
    BYTE        triOption;
    BYTE        triDay;
    signed char triInitNice;
    signed char triCurNice;
    BYTE        triInitTry;
    BYTE        triCurTry;
    BYTE        triQueue;
    TIMESTAMP   triCreateTime;
    TIMESTAMP   triLastTime;
    BYTE        triState;
    int         triError;
} __attribute__ ((packed)) TRINFOENTRY;

//  [11/10/2010 DHKim]
typedef struct	_tagGROUPINFOENTRY
{
	int		nTotCnt;
	int		nGroupkey;
	char	pGroupName[256];
	int		nMemberCnt;
	//char	pMemberInfo[256];
	char	pMemberID[20];
}	__attribute__ ((packed)) GROUPINFOENTRY;


typedef struct _tagTRHISTORYENTRY
{
    TIMESTAMP   trhTime;
    BYTE        trhState;
    int         trhError;
} __attribute__ ((packed)) TRHISTORYENTRY;


typedef struct _tagMETERDATAENTRY
{
	EUI64		mdSensorID;
	char		mdID[20];
	BYTE		mdType;
	BYTE		mdVendor;
	BYTE		mdServiceType;
	TIMESTAMP	mdTime;
	WORD		mdLength;
	char		mdData[0];
} __attribute__ ((packed)) METERDATAENTRY;

typedef struct _tagMETERENTRY
{
	BYTE		mtrGroup;
	BYTE		mtrMember;
	EUI64		mtrID;
	char		mtrSerial[18];
	TIMESTAMP	mtrTime;
	WORD		mtrLength;
	char		mtrData[0];
} __attribute__ ((packed)) METERENTRY;

typedef struct  _tagMETERLPENTRY
{
    EUI64      	mlpId; 
    char        mlpMid[20];
    BYTE        mlpType;
    BYTE        mlpServiceType;
    BYTE        mlpVendor;
    WORD        mlpDataCount;
    WORD        mlpLength;
    TIMESTAMP   mlpTime;
    char        mlpData[0];
}   __attribute__ ((packed)) METERLPENTRY;

typedef struct _tagMETERENTRYMBUS
{
        GEUI64    mmtrGid;                        // EUI ID
        BYTE      mmtrMasterBank;                //generation of the heat meter
        char      mmtrSerial[22];           // serial num
        char      mmtrMenufa[22];           // menufacture
        BYTE      mmtrGener;                //generation of the heat meter
        BYTE      mmtrMedia;
        BYTE      mmtrCntAccess;
        BYTE      mmtrStatus;                   // Status code 
        TIMESTAMP    mmtrTMMetering;               // 마지막 검침 성공 시간
} __attribute__ ((packed)) METERENTRYMBUS;
typedef struct  _tagDISTRIBUTIONENTRY
{
	char		szTriggerId[17];
	BYTE		nType;
	char		szModel[21];
	BYTE		nTransferType;
	BYTE		nInstallType;
	BYTE		nOtaStep;
	BYTE		nWriteCount;
	UINT		nRetry;
	BYTE		nThread;
	WORD		nOldHw;
	WORD		nOldSw;
	WORD		nOldBuild;
	WORD		nNewHw;
	WORD		nNewSw;
	WORD		nNewBuild;
	UINT		nCount;
	UINT		nElapse;
	BYTE		nState;
	time_t		tmStart;
}   __attribute__ ((packed)) DISTRIBUTIONENTRY;

typedef struct  _tagOTANODEENTRY
{
	EUI64		id;						// Sensor ID
	char		szModel[21];			// Model name
	WORD		hw;						// HW Version
	WORD		sw;						// SW Version
	WORD		build;					// BUILD Version
	BYTE		hops;					// HOP

	UINT		nOtaMask;				// 수행한 STEP MASK 값
	UINT		nTotal;					// 전체 전송 바이트
	UINT		nCurrent;				// 현재 전송 바이트
	UINT		nElapse;				// 사용 시간
	int			nError;					// 에러 코드
	BYTE		nState;					// 현재 상태

	time_t		tmStart;				// 시작 시간
	time_t		tmEnd;					// 종료 시간
}   __attribute__ ((packed)) OTANODEENTRY;

int oidEntryQuery(VAROBJECT *, int, MIBValue *, MIBValue **, int *, MIBValue *);
int eui64EntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int ipEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int timeEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int dayEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int verEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int smivarEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int pageEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int gmtEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int sysEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int varEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int varValueEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int varSubValueEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int varGeEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int usrEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int gpsEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int mobileCfgEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int memEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int flashEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int ethEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int gpioEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int gpioCtrlEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int mobileEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int pluginEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int procEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int pppEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int envEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int commLogEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int commLogStructQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int cmdHistEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int cmdHistStructQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int mcuEventEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int mcuEventStructQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int meterLogEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int meterLogStructQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int mobileLogEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int mobileLogStructQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int sinkEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int sinkStaticEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int codiEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int codiDeviceEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int codiBindingEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int codiNeighborEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int codiMemoryEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int codiRegisterEntryQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int sensorConnectEntryQuery(VAROBJECT *, int, MIBValue *, MIBValue **, int *, MIBValue *);
int meterDataEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int meterEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int meterLPEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int meterMBusEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
int debugLogEntryQuery(VAROBJECT *, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);
#endif	// __VARIABLE_H__
