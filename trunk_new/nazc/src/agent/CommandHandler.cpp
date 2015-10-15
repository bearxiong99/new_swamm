#include "common.h"
#include <limits.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include "mcudef.h"
#include "logdef.h"
#include "CommandHandler.h"
#include "AgentService.h"
#include "EndDeviceList.h"
#include "Ondemander.h"
#include "StatusMonitor.h"
#include "UserManager.h"
#include "BatchJob.h"
#include "EventInterface.h"
#include "MeterUploader.h"
#include "MeterParserFactory.h"
#include "Metering.h"
#include "Polling.h"
#include "Variable.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "Chunk.h"
#include "Utility.h"
#include "IcmpPing.h"
#include "gpiomap.h"
#include "SystemMonitor.h"
#include "revision.h"
#include "EventInterface.h"
#include "Distributor.h"
#include "Patch.h"
#include "TransactionManager.h"
#include "ShellCommand.h"

#include "rep/MessageHelper.h"

//  [10/13/2010 DHKim]
//#include "gdbm/gdbm.h"
//  [3/10/2011 DHKim]
#include "DbGroup.h"
#include <vector>
using namespace std;

#undef __SINGLE_THREAD_ONLY__

#define SAVE_FILENAME			"/tmp/config.tar.gz"

#if     defined(__PATCH_11_65__)
/**< Issue #65 JEJU SP */
extern BOOL    *m_pbPatch_11_65;
#endif

void CoordinatorPowerControl();
int GetTimeInterval(struct timeval *pPrev, struct timeval *pCurrent);
int GetTimeInterval(struct timeval *pCurrent, long tv_sec, long tv_usec);

typedef struct _tagParserVersionBuild {
    char parser[256];
    BOOL bAll;
    BOOL bParser;
    WORD version;
    WORD build;
    BYTE nOption;
} ParserVersionBuild;

extern int m_nSystemType;
extern UINT m_nNzcPowerType;

//GDBM_FILE Recoverydbf = NULL, Recoverydbf_mem = NULL;

//////////////////////////////////////////////////////////////////////
// Command Table
//////////////////////////////////////////////////////////////////////

IF4_COMMAND_TABLE	m_CommandHandler[] =
{
	// MCU Control Command
	{ {100,1,0},	"cmdMcuDiagnosis",			cmdMcuDiagnosis, TRUE },
	{ {100,2,0},	"cmdMcuReset",				cmdMcuReset, TRUE },
	{ {100,3,0},	"cmdMcuShutdown",			cmdMcuShutdown, TRUE },
	{ {100,4,0},	"cmdMcuFactoryDefault",		cmdMcuFactoryDefault, TRUE },
	{ {100,5,0},	"cmdMcuGetTime",			cmdMcuGetTime, FALSE },
	{ {100,6,0},	"cmdMcuSetTime",			cmdMcuSetTime, TRUE },
	{ {100,7,0},	"cmdMcuGetState",			cmdMcuGetState, FALSE },
	{ {100,8,0},	"cmdMcuLoopback",			cmdMcuLoopback, TRUE },
	{ {100,9,0},	"cmdMcuClearStatic",		cmdMcuClearStatic, TRUE },
	{ {100,10,0},	"cmdMcuGetSystemInfo",		cmdMcuGetSystemInfo, FALSE },
	{ {100,12,0},	"cmdMcuGetEnvironment",		cmdMcuGetEnvironment, FALSE },
	{ {100,14,0},	"cmdMcuGetGpio",			cmdMcuGetGpio, FALSE },
	{ {100,15,0},	"cmdMcuSetGpio",			cmdMcuSetGpio, TRUE },
	{ {100,23,0},	"cmdResetDevice",			cmdResetDevice, TRUE },

	{ {100,26,0},	"cmdGetConfiguration",		cmdGetConfiguration, TRUE },
	{ {100,27,0},	"cmdSetConfiguration",		cmdSetConfiguration, TRUE },

	{ {100,28,0},	"cmdGetEventConfigure",		cmdGetEventConfigure, FALSE },
	{ {100,29,0},	"cmdSetEventConfigure",		cmdSetEventConfigure, TRUE },
	{ {100,30,0},	"cmdMcuGetPatch",		    cmdMcuGetPatch, FALSE },
	{ {100,31,0},	"cmdMcuSetPatch",		    cmdMcuSetPatch, TRUE },
	{ {100,32,0},	"cmdMcuCleaning",		    cmdMcuCleaning, TRUE },
	{ {100,33,0},	"cmdGetFwInfo",		        cmdGetFwInfo, FALSE },

	{ {100,200,0},	"cmdMcuInstall",			cmdMcuInstall, TRUE },
	{ {100,201,0},	"cmdMcuUninstall",			cmdMcuUninstall, TRUE },
	{ {100,202,0},	"cmdOamLogin",				cmdOamLogin, TRUE },
	{ {100,203,0},	"cmdOamLogout",				cmdOamLogout, TRUE },

	// SINK Control Command
	{ {101,1,0},	"cmdGetSinkList",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,2,0},	"cmdGetSink",				cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,3,0},	"cmdSetSink",				cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,4,0},	"cmdSetSinkTime",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,5,0},	"cmdGetSinkStatic",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,6,0},	"cmdGetSinkState",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,7,0},	"cmdSetAutoSetting",		cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,8,0},	"cmdSetChannel",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,9,0},	"cmdSetPanID",				cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,10,0},	"cmdSetRFPower",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,11,0},	"cmdSetTXMode",				cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {101,12,0},	"cmdGetCordiInfo",			cmdDeprecatedFunction, FALSE },     // Deprecated

	// Coordinator Control Command
	{ {101,13,0},	"cmdGetCodiList",			cmdGetCodiList, FALSE },
	{ {101,14,0},	"cmdGetCodiInfo",			cmdGetCodiInfo, FALSE },
	{ {101,15,0},	"cmdSetCodiInfo",			cmdSetCodiInfo, TRUE },
	{ {101,16,0},	"cmdGetCodiDevice",			cmdGetCodiDevice, FALSE },
	{ {101,17,0},	"cmdSetCodiDevice",			cmdSetCodiDevice, TRUE },
	{ {101,18,0},	"cmdGetCodiBinding",		cmdGetCodiBinding, FALSE },
	{ {101,19,0},	"cmdGetCodiNeighbor",		cmdGetCodiNeighbor, FALSE },
	{ {101,20,0},	"cmdGetCodiMemory",			cmdGetCodiMemory, FALSE },
	{ {101,21,0},	"cmdSetCodiPermit",			cmdSetCodiPermit, TRUE },
	{ {101,22,0},	"cmdSetCodiFormNetwork",	cmdSetCodiFormNetwork, TRUE },
	{ {101,23,0},	"cmdSetCodiReset",			cmdSetCodiReset, TRUE },
	{ {101,24,0},	"cmdUpdateCoordinator",		cmdUpdateCoordinator, TRUE },
	{ {101,25,0},	"cmdGetCodiRegister",		cmdGetCodiRegister, FALSE },
	{ {101,26,0},	"cmdSetCodiRegister",		cmdSetCodiRegister, TRUE },
	{ {101,27,0},	"cmdDeleteCodiRegister",	cmdDeleteCodiRegister, TRUE },

	// SENSOR Control Command
	{ {102,1,0},	"cmdAddSensor",				cmdAddSensor, TRUE },
	{ {102,2,0},	"cmdDeleteSensor",			cmdDeleteSensor, TRUE },
	{ {102,3,0},	"cmdDeleteSensorAll",		cmdDeleteSensorAll, TRUE },
	{ {102,4,0},	"cmdGetSensorList",			cmdDeprecatedFunction, FALSE },
	{ {102,5,0},	"cmdGetSensor",				cmdDeprecatedFunction, FALSE },
	{ {102,6,0},	"cmdGetSensorPage",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {102,7,0},	"cmdSetSensorPage",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {102,8,0},	"cmdPingSensor",			cmdPingSensor, TRUE },
	{ {102,9,0},	"cmdGetSensorStatic",		cmdDeprecatedFunction, FALSE },
	{ {102,10,0},	"cmdSetSensor",				cmdDeprecatedFunction, TRUE },
	{ {102,11,0},	"cmdGetSensorInfo",			cmdDeprecatedFunction, FALSE },
	{ {102,12,0},	"cmdGetSensorAll",			cmdDeprecatedFunction, FALSE },
	{ {102,13,0},	"cmdGetSensorAllEx",		cmdDeprecatedFunction, FALSE },
	{ {102,14,0},	"cmdGetSensorCount",		cmdGetSensorCount, FALSE },
	{ {102,15,0},	"cmdGetSensorSchedule",		cmdDeprecatedFunction,  FALSE },    // Deprecated
	{ {102,16,0},	"cmdCancelSensorSchedule",	cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {102,17,0},	"cmdCancelSensorScheduleAll", cmdDeprecatedFunction, FALSE },   // Deprecated
	{ {102,18,0},	"cmdGetSensorPageAll", 		cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {102,20,0},	"cmdDeleteMember",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {102,21,0},	"cmdDeleteMemberAll",		cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {102,22,0},	"cmdSetSensorTime",			cmdDeprecatedFunction, TRUE },
	{ {102,23,0},	"cmdGetSensorROM",			cmdGetSensorMultiROM, TRUE },
	{ {102,24,0},	"cmdSetSensorROM",			cmdSetSensorROM, TRUE },
	{ {102,26,0},	"cmdGetSensorAllNew",		cmdGetSensorAllNew, FALSE },
	{ {102,27,0},	"cmdGetSensorInfoNew",		cmdGetSensorInfoNew, FALSE },
	{ {102,28,0},	"cmdGetSensorAmrData",		cmdGetSensorAmrData, FALSE },
	{ {102,29,0},	"cmdSetSensorAmrData",		cmdSetSensorAmrData, TRUE },
	{ {102,30,0},	"cmdResetSensor",		    cmdResetSensor, TRUE },
//  { {102,31,0},   "cmdCorrectSensorPulse",   국내가스 (센서의 펄스를 보정)
	{ {102,32,0},	"cmdUpdateSensorFirmware",	cmdUpdateSensorFirmware, TRUE },
	{ {102,33,0},	"cmdGetSensorPath",			cmdGetSensorPath, FALSE },          // Path 정보
	{ {102,34,0},	"cmdCommandSensor",	        cmdCommandSensor, TRUE },
	{ {102,35,0},	"cmdScanSensor",	        cmdScanSensor, TRUE },
	{ {102,36,0},	"cmdDistributeFirmware",	cmdDistributeFirmware, TRUE },
	{ {102,37,0},	"cmdBypassSensor",	        cmdBypassSensor, FALSE },
	{ {102,38,0},	"cmdGetFFDList",	        cmdGetFFDList, FALSE },
//	{ {102,39,0},	"cmdOtaStatusList",	        아직 구현되지 않았다
	{ {102,40,0},	"cmdAsynchronousCall",	    cmdAsynchronousCall, TRUE },
	{ {102,41,0},	"cmdShowTransationList",    cmdShowTransactionList, FALSE },
	{ {102,42,0},	"cmdShowTransationInfo",    cmdShowTransactionInfo, FALSE },
	{ {102,43,0},	"cmdModifyTransaction",     cmdModifyTransaction, TRUE },
	{ {102,44,0},	"cmdGetSensorEvent",        cmdGetSensorEvent, FALSE },
	{ {102,45,0},	"cmdGetSensorBattery",      cmdGetSensorBattery, FALSE },
    { {102,46,0},   "cmdGetMeterList",          cmdGetMeterList, FALSE },
    { {102,47,0},   "cmdDeleteMeter",           cmdDeleteMeter, FALSE },
//  { {102,48,0},   "cmdSetMbusROM",            아직 구현되지 않았다
    { {102,49,0},   "cmdDigitalInOut",          cmdDigitalInOut, TRUE },
	{ {102,50,0},	"cmdExecuteCommand",	    cmdExecuteCommand, TRUE },          // cmdCommandSensor와 유사하지만 Read/Write를 선택할 수 있다.
	{ {102,51,0},	"cmdGetSensorConstants",	cmdGetSensorConstants, FALSE }, 
	{ {102,52,0},	"cmdGetSensorValue",	    cmdGetSensorValue, FALSE }, 
#ifdef	__GROUP_SUPPORT_MODE__
	{ {102,53,0},	"cmdGroupAsyncCall",		cmdGroupAsyncCall, FALSE },							//  [10/13/2010 DHKim]
	{ {102,54,0},	"cmdGroupAdd",		        cmdGroupAdd, FALSE },									//  [10/13/2010 DHKim]
	{ {102,55,0},	"cmdGroupDelete",		    cmdGroupDelete, FALSE },									//  [10/13/2010 DHKim]
	{ {102,56,0},	"cmdGroupAddMember",		cmdGroupAddMember, FALSE },					//  [10/22/2010 DHKim]
	{ {102,57,0},	"cmdGroupDeleteMember",		cmdGroupDeleteMember, FALSE },			//  [10/22/2010 DHKim]
	{ {102,58,0},	"cmdGroupInfo",		        cmdGroupInfo, FALSE },									//  [10/22/2010 DHKim]
	{ {102,59,0},	"cmdGroupShowCommand",		cmdGroupShowCommand, FALSE },			//  [11/19/2010 DHKim]
	{ {102,60,0},	"cmdGroupTableDelete",		cmdGroupTableDelete, FALSE },					//  [3/29/2011 DHKim]
	{ {102,61,0},	"cmdJointableShow",		    cmdJointableShow, FALSE },							//  [4/21/2011 DHKim]
	{ {102,62,0},	"cmdJointableAdd",		    cmdJointableAdd, FALSE },									//  [4/21/2011 DHKim]
	{ {102,63,0},	"cmdJointableDelete",		cmdJointableDelete, FALSE },							//  [4/21/2011 DHKim]
	{ {102,64,0},	"cmdJointableAddFile",		cmdJointableAddFile, FALSE },							//  [4/21/2011 DHKim]
#endif	
    { {102,70,0},   "cmdUpdateGroup",           cmdUpdateGroup, TRUE },             // 실증단지 Group 정보 갱신
    { {102,71,0},   "cmdDeleteGroup",           cmdDeleteGroup, TRUE },             // 실증단지 Group 정보 삭제 
    { {102,72,0},   "cmdGetGroup",              cmdGetGroup, FALSE },               // 실증단지 Group 정보 조회 
	{ {102,73,0},    "cmdSetIHDTable",        	cmdSetIHDTable, TRUE },           // NorthAfrica IHD add [jiwook]
	{ {102,74,0},    "cmdDelIHDTable",        	cmdDelIHDTable, TRUE },           // NorthAfrica IHD add [jiwook]
	// Log Command
	{ {103,1,0},	"cmdClearLog",				cmdClearLog, TRUE },
	{ {103,2,0},	"cmdGetCmdHistList",		cmdGetHistList, FALSE },
	{ {103,3,0},	"cmdGetMeterLogList",		cmdGetMeterLogList, FALSE },
	{ {103,4,0},	"cmdGetEventLogList",		cmdGetEventLogList, FALSE },
	{ {103,5,0},	"cmdGetCommLogList",		cmdGetCommLogList, FALSE },
	{ {103,6,0},	"cmdGetMobileLogList",		cmdGetMobileLogList, FALSE },
	{ {103,100,0},	"cmdGetAllLogList",			cmdGetAllLogList, FALSE },

	// Metering Command
	{ {104,1,0},	"cmdSingleMetering",		cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {104,2,0},	"cmdMeteringAll",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {104,3,0},	"cmdRecovery",				cmdRecovery, TRUE },
	{ {104,4,0},	"cmdGetMeter",				cmdGetMeter, FALSE },
	{ {104,5,0},	"cmdGetMeterAll",			cmdGetMeterAll, FALSE },
	{ {104,6,0},	"cmdOnDemandMeter",			cmdOnDemandMeter, TRUE },
	{ {104,7,0},	"cmdOnDemandMeterAll",		cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {104,11,0},	"cmdGetGroupMemberAll",		cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {104,12,0},	"cmdOnDemand",				cmdDeprecatedFunction, FALSE },     // Deprecated     
	{ {104,13,0},	"cmdGetMeterSchedule",		cmdGetMeterSchedule, TRUE },
	{ {104,14,0},	"cmdGetDRAssetInfo",		cmdGetDRAssetInfo, FALSE },
	{ {104,15,0},	"cmdGetEnergyLevel",		cmdGetEnergyLevel, FALSE },
	{ {104,16,0},	"cmdSetEnergyLevel",		cmdSetEnergyLevel, TRUE },
    { {104,17,0},   "cmdGetLoadProfile",        cmdGetLoadProfile, FALSE },
	{ {104,18,0},	"cmdGetMeteringOption",		cmdGetMeteringOption, FALSE },
	{ {104,19,0},	"cmdSetMeteringOption",		cmdSetMeteringOption, TRUE },
    // Issue #930 
	{ {104,20,0},	"cmdGetMeterProperty",		cmdGetMeterProperty, FALSE },
	{ {104,21,0},	"cmdSetMeterProperty",		cmdSetMeterProperty, TRUE },
	{ {104,22,0},	"cmdGetMeterPropertyById",	cmdGetMeterPropertyById, FALSE },

	{ {104,30,0},	"cmdCheckMeter",			cmdCheckMeter, TRUE },
    { {104,42,0},   "cmdOnDemandMbus",          cmdOnDemandMbus, TRUE },


	// Meter Command
	{ {105,5,0},	"cmdSyncMeterTime",			cmdSyncMeterTime, TRUE },
	{ {105,6,0},	"cmdSyncMeterTimeAll",		cmdSyncMeterTimeAll, TRUE },
	{ {105,7,0},	"cmdAidonMCCB",				cmdAidonMCCB, TRUE },
	{ {105,8,0},	"cmdKamstrupCID",			cmdKamstrupCID, TRUE },

	{ {105,9,0},	"cmdGetMeterSecurity",		cmdGetMeterSecurity, FALSE },
	{ {105,10,0},	"cmdSetMeterSecurity",		cmdSetMeterSecurity, TRUE },

#ifdef __SUPPORT_KUKDONG_METER__
	{ {105,11,0},	"cmdValveControl",			cmdValveControl, TRUE },
	{ {105,12,0},	"cmdGetMeterStatus",		cmdGetMeterStatus, FALSE },
	{ {105,13,0},	"cmdGetMeterVersion",		cmdGetMeterVersion, FALSE },
	{ {105,14,0},	"cmdSetMeterConfig",		cmdSetMeterConfig, TRUE },
#endif

	// System User Command
	{ {106,1,0},	"cmdAddUser",				cmdAddUser, TRUE },
	{ {106,2,0},	"cmdDeleteUser",			cmdDeleteUser, TRUE },
	{ {106,3,0},	"cmdGetUser",				cmdGetUser, FALSE },
	{ {106,4,0},	"cmdSetUser",				cmdSetUser, TRUE },
	{ {106,5,0},	"cmdUserList",				cmdUserList, FALSE },

	{ {107,1,0},	"cmdGetPhoneList",			cmdGetPhoneList, FALSE },
	{ {107,2,0},	"cmdSetPhoneList",			cmdSetPhoneList, TRUE },
	{ {107,3,0},	"cmdGetSCID",				cmdGetSCID, FALSE },

    // Message & Session
	//{ {111,1,0},	"cmdBypassData",            cmdDeprecatedFunction, FALSE },   집중기는 지원하지 않음 (FEP)  
	{ {111,4,0},	"cmdSendMessage",           cmdSendMessage, TRUE }, 
	{ {111,5,0},	"cmdPollMessage",           cmdPollMessage, FALSE }, 

	// APN Command
	{ {120,1,0},	"cmdAddApn",				cmdAddApn, TRUE },
	{ {120,2,0},	"cmdDeleteApn",				cmdDeleteApn, TRUE },
	{ {120,3,0},	"cmdGetApnList",			cmdGetApnList, FALSE },
	{ {120,4,0},	"cmdGetApnOption",			cmdGetApnOption, FALSE },

	// IPC Command
	{ {197,1,0},	"cmdFirmwareUpdate",		cmdFirmwareUpdate, TRUE },
//	{ {197,2,0},	"cmdEventTest",				cmdEventTest, FALSE },
	{ {197,3,0},	"cmdCheck",					cmdCheck, TRUE },
	{ {197,4,0},	"cmdShellCommand",			cmdShellCommand, FALSE },
	{ {197,5,0},	"cmdSensorTimesync",		cmdSensorTimesync, TRUE },
	{ {197,6,0},	"cmdMeterUpload",			cmdMeterUpload, TRUE },
	{ {197,10,0},	"cmdSetTestMode",			cmdSetTestMode, TRUE },
	{ {197,11,0},	"cmdPingTestMode",			cmdSetPingTestMode, TRUE },
	{ {197,12,0},	"cmdLiveCheck",				cmdLiveCheck, FALSE },

	// GE Only
	{ {197,100,0},	"cmdReadTable",				cmdReadTable, TRUE },
	{ {197,101,0},	"cmdWriteTable",			cmdDeprecatedFunction, FALSE },     // deprecated 2012.07.25 
	{ {197,102,0},	"cmdReadTableAll",			cmdReadTableAll, TRUE },
	{ {197,103,0},	"cmdWriteTableAll",			cmdDeprecatedFunction, FALSE },     // Deprecated

	// File Transfer Command
	{ {198,1,0},	"cmdGetFile",				cmdGetFile, FALSE },
	{ {198,2,0},	"cmdPutFile",				cmdPutFile, TRUE },
	{ {198,3,0},	"cmdInstallFile",			cmdDeprecatedFunction, TRUE },
	{ {198,10,0},	"cmdGetFirmware",			cmdGetFirmware, TRUE },             //unuse
	{ {198,11,0},	"cmdPackageDistribution",	cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {198,12,0},	"cmdMergeAndInstall",	    cmdMergeAndInstall, TRUE },          

	// New Package Distribution
	{ {198,13,0},	"cmdDistribution",	  	  	cmdDistribution, TRUE },          
	{ {198,14,0},	"cmdDistributionState",	    cmdDistributionState, FALSE },          
	{ {198,15,0},	"cmdDistributionCancel", 	cmdDistributionCancel, TRUE },          
	{ {198,16,0},	"cmdDistributionList", 		cmdDistributionList, FALSE },          
	{ {198,17,0},	"cmdDistributionInfo", 		cmdDistributionInfo, FALSE },          
	{ {198,18,0},	"cmdDistributionSensor", 	cmdDistributionSensor, TRUE },          

	// Common Control Command
	{ {199,1,0},	"cmdStdGet",				cmdStdGet, FALSE },
	{ {199,2,0},	"cmdStdSet",				cmdStdSet, TRUE },
	{ {199,4,0},	"cmdStdGetChild",			cmdStdGetChild, FALSE },
	{ {0,0,0},		NULL,						NULL }
};

//////////////////////////////////////////////////////////////////////////
//  [11/15/2010 DHKim]
//  Group Operation Command Table
//////////////////////////////////////////////////////////////////////////
#ifdef __GROUP_SUPPORT_MODE__
IF4_COMMAND_TABLE	m_GroupCommandHandler[] =
{
	// SENSOR Control Command
	{ {102,5,0},	"cmdGetSensor",				cmdDeprecatedFunction, FALSE },
	{ {102,6,0},	"cmdGetSensorPage",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {102,7,0},	"cmdSetSensorPage",			cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {102,9,0},	"cmdGetSensorStatic",		cmdDeprecatedFunction, FALSE },
	{ {102,10,0},	"cmdSetSensor",				cmdDeprecatedFunction, TRUE },
	{ {102,11,0},	"cmdGetSensorInfo",			cmdDeprecatedFunction, FALSE },
	{ {102,15,0},	"cmdGetSensorSchedule",		cmdDeprecatedFunction,  FALSE },    // Deprecated
	{ {102,16,0},	"cmdCancelSensorSchedule",	cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {102,22,0},	"cmdSetSensorTime",			cmdDeprecatedFunction, TRUE },
	{ {102,23,0},	"cmdGetSensorROM",			cmdGetSensorMultiROM, TRUE },
	{ {102,24,0},	"cmdSetSensorROM",			cmdSetSensorROM, TRUE },
	{ {102,27,0},	"cmdGetSensorInfoNew",		cmdGetSensorInfoNew, FALSE },
	{ {102,28,0},	"cmdGetSensorAmrData",		cmdGetSensorAmrData, FALSE },
	{ {102,29,0},	"cmdSetSensorAmrData",		cmdSetSensorAmrData, TRUE },
	{ {102,30,0},	"cmdResetSensor",		    cmdResetSensor, TRUE },
	{ {102,34,0},	"cmdCommandSensor",	        cmdCommandSensor, TRUE },
	{ {102,35,0},	"cmdScanSensor",	        cmdScanSensor, TRUE },
	{ {102,44,0},	"cmdGetSensorEvent",        cmdGetSensorEvent, FALSE },
	{ {102,45,0},	"cmdGetSensorBattery",      cmdGetSensorBattery, FALSE },
	{ {102,49,0},   "cmdDigitalInOut",          cmdDigitalInOut, TRUE },
	{ {102,50,0},	"cmdExecuteCommand",	    cmdExecuteCommand, TRUE },          // cmdCommandSensor와 유사하지만 Read/Write를 선택할 수 있다.
	{ {102,51,0},	"cmdGetSensorConstants",	cmdGetSensorConstants, FALSE }, 
	{ {102,52,0},	"cmdGetSensorValue",	    cmdGetSensorValue, FALSE }, 

	// Metering Command
	{ {104,1,0},	"cmdSingleMetering",		cmdDeprecatedFunction, FALSE },     // Deprecated
	{ {104,3,0},	"cmdRecovery",				cmdRecovery, TRUE },
	{ {104,4,0},	"cmdGetMeter",				cmdGetMeter, FALSE },
	{ {104,13,0},	"cmdGetMeterSchedule",		cmdGetMeterSchedule, TRUE },
	{ {104,30,0},	"cmdCheckMeter",			cmdCheckMeter, TRUE },

	// Meter Command
	{ {105,5,0},	"cmdSyncMeterTime",			cmdSyncMeterTime, TRUE },
	{ {105,7,0},	"cmdAidonMCCB",				cmdAidonMCCB, TRUE },
	{ {105,8,0},	"cmdKamstrupCID",			cmdKamstrupCID, TRUE },

	// GE Only
	{ {197,100,0},	"cmdReadTable",				cmdReadTable, TRUE },
	{ {197,101,0},	"cmdWriteTable",			cmdDeprecatedFunction, FALSE },     // Deprecated 2012.07.25

	// Common Control Command
	{ {199,1,0},	"cmdStdGet",				cmdStdGet, FALSE },
	{ {199,2,0},	"cmdStdSet",				cmdStdSet, TRUE },
	{ {199,4,0},	"cmdStdGetChild",			cmdStdGetChild, FALSE },
	{ {0,0,0},		NULL,						NULL }
};
#endif

extern char		m_szFepServer[];
extern int		m_nNeighborNode;
extern EUI64	m_arNeighborList[];

extern BOOL		m_bGprsPerfTestMode;
extern BOOL		m_bPingTestMode;
extern BOOL		m_bEnableAutoMetering;
extern MOBILECFGENTRY m_sMobileConfig;

//////////////////////////////////////////////////////////////////////
// Deprecated Function
//////////////////////////////////////////////////////////////////////

int cmdDeprecatedFunction(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    return IF4ERR_DEPRECATED_FUNCTION;
}

//////////////////////////////////////////////////////////////////////
// MCU Control Command
//////////////////////////////////////////////////////////////////////

int cmdMcuDiagnosis(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CIcmpPing   ping;
    struct      timeval atime;
    struct      sockaddr_in addr;
    int      	i, ret;
	BYTE		val[10];
    FILE    	*fp;
    char    	szBuffer[256];
    char    	szVersion[16] = "";
    char    	szSCID[32] = "";
    char    	szAuth[16] = "";
    char    	szCSQ[16] = "";
    int     	nCSQ = 99;
	BOOL		bOk = TRUE;

    fp = fopen("/app/conf/mobileinfo", "rt");
    if (fp != NULL)
    {
        while(fgets(szBuffer, sizeof(szBuffer), fp))
        {
            szBuffer[strlen(szBuffer)-1] = '\0';
            if (strncmp(szBuffer, "REV ", 4) == 0)
                strcpy(szVersion, &szBuffer[4]);
            else if (strncmp(szBuffer, "SIM ", 4) == 0)
                strcpy(szSCID, &szBuffer[4]);
            else if (strncmp(szBuffer, "PIN ", 4) == 0)
                strcpy(szAuth, &szBuffer[4]);
            else if (strncmp(szBuffer, "CSQ ", 4) == 0)
            {   
                strcpy(szCSQ, &szBuffer[4]);
                nCSQ = strtol(szCSQ, (char**)NULL, 10);
            }
        }
        fclose(fp);
    }

	val[0]	= m_pService->GetState() == 1 ? 0 : 1;									// MCU State
	val[1]	= codiGetState(GetCoordinator()) == CODISTATE_NORMAL ? 0 : 1;		    // SINK#1 State
	val[2]	= 0;																	// SINK#2 Not support
#if     defined(__INCLUDE_GPIO__)
	val[3]	= (m_nNzcPowerType & NZC_POWER_TYPE_SOLAR) ? 0 :
                (m_pStatusMonitor->GetErrorMask() & GPIONOTIFY_PWR ? 1 : 0);		// Power Fail
	val[4]	= m_pStatusMonitor->GetErrorMask() & GPIONOTIFY_LOWBAT ? 1 : 0;			// Low Battery
	val[5]	= m_pStatusMonitor->GetErrorMask() & GPIONOTIFY_TEMP ? 1 : 0;			// Temperature
	val[6]	= m_pStatusMonitor->GetErrorMask() & GPIONOTIFY_MEMORY ? 1 : 0;			// Memory
	val[7]	= m_pStatusMonitor->GetErrorMask() & GPIONOTIFY_FLASH ? 1 : 0;			// Flash Memory
#else
	val[3]	= 0;																	// Power Fail
	val[4]	= 0;																	// Low Battery
	val[5]	= 0;																	// Temperature
	val[6]	= 0;																	// Memory
	val[7]	= 0;																	// Falsh Memory
#endif
	val[8]	= 0;																	// Mobile

	if (*szVersion == '\0')
		val[8] = 1;
	else if (*szSCID == '\0')
		val[8] = 2;
	else if (strncmp(szAuth, "READY", 5) != 0)
		val[8] = 3;
	else if (nCSQ >= 51)
		val[8] = 4;

    ping.IcmpInit();
    ret     = ping.CheckIcmp(m_szFepServer, &atime, &addr, 300);
	val[9]	= (ret != -1) ? 0 : 1;													// Ethernet State
    ping.IcmpClose();

	for(i=0; i<10; i++)
	{
		if (val[i] != 0)
		{
			bOk = FALSE;
			break;
		}
	}
	
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, val, 10);
	if (!bOk) mcuMcuDiagnosisEvent((BYTE *)val, 10);
	return IF4ERR_NOERROR;
}

int cmdMcuReset(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    if(pWrapper != NULL) {
	    mcuResetEvent(GetSessionType(pWrapper->pSession), pWrapper->pSession->szAddress);
    }else {
	    mcuResetEvent(4, "127.0.0.1");
    }
	m_pService->SetResetState(RST_COMMAND);
	m_pService->PostMessage(GWMSG_RESET, NULL);
	return IF4ERR_NOERROR;
}

int cmdMcuShutdown(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	mcuShutdownEvent(GetSessionType(pWrapper->pSession), pWrapper->pSession->szAddress);
	m_pService->SetResetState(RST_COMMAND);
	m_pService->PostMessage(GWMSG_SHUTDOWN, NULL);

	return IF4ERR_NOERROR;
}

int cmdMcuFactoryDefault(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	mcuFactoryDefaultEvent(GetSessionType(pWrapper->pSession), pWrapper->pSession->szAddress);

    DestroyDirectories();
    InitializeDirectories();

	m_pService->PostMessage(GWMSG_FACTORYDEFAULT, NULL);
	return IF4ERR_NOERROR;
}

int cmdMcuGetTime(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	TIMESTAMP	tmNow;
	time_t		now;
	struct	tm	when;
	
	time(&now);
	when = *localtime(&now);

    tmNow.year    = when.tm_year + 1900;
    tmNow.mon     = when.tm_mon + 1;
    tmNow.day     = when.tm_mday;
    tmNow.hour    = when.tm_hour;
    tmNow.min     = when.tm_min;
    tmNow.sec     = when.tm_sec;

	IF4API_AddResultFormat(pWrapper, "2.1.16", VARSMI_TIMESTAMP, &tmNow, sizeof(TIMESTAMP));
	return IF4ERR_NOERROR;
}

int cmdMcuSetTime(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	TIMESTAMP	*pTime;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (pValue->type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	pTime = (TIMESTAMP *)pValue->stream.p;
	if ((pTime->year <= 2000) ||
		(pTime->mon == 0) || (pTime->mon > 12) ||
		(pTime->day == 0) || (pTime->day > 31) ||
		(pTime->hour > 23) ||
		(pTime->min >= 60) ||
		(pTime->sec >= 60))
		return IF4ERR_INVALID_PARAM;

	m_pRealTimeClock->SetTime(pTime);
	return IF4ERR_NOERROR;
}

int cmdMcuGetState(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	IF4API_AddResultNumber(pWrapper, "2.1.24", VARSMI_BYTE, m_pService->GetState());
	return IF4ERR_NOERROR;
}

int cmdMcuLoopback(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	int		i;

	for(i=0; i<cnt; i++)
		IF4API_AddResultMIBValue(pWrapper, &pValue[i]);
	return IF4ERR_NOERROR;
}

int cmdMcuClearStatic(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	return IF4ERR_NOERROR;
}

int cmdMcuGetSystemInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	MIBValue	*pGet;
	OID3		oid;

	VARAPI_StringToOid("2.1", &oid);	
	pGet = VARAPI_GetValue(&oid, pValue);
	if (pGet == NULL)
		return IF4ERR_INVALID_OID;

	IF4API_AddResultMIBValue(pWrapper, pGet);
	VARAPI_FreeValue(pGet);
	return IF4ERR_NOERROR;
}

int cmdMcuGetEnvironment(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	MIBValue	*pGet;
	OID3		oid;

	VARAPI_StringToOid("2.2", &oid);	
	pGet = VARAPI_GetValue(&oid, pValue);
	if (pGet != NULL)
	{
		IF4API_AddResultMIBValue(pWrapper, pGet);
		VARAPI_FreeValue(pGet);
	}

	VARAPI_StringToOid("2.3", &oid);	
	pGet = VARAPI_GetValue(&oid, pValue);
	if (pGet != NULL)
	{
		IF4API_AddResultMIBValue(pWrapper, pGet);
		VARAPI_FreeValue(pGet);
	}

	return IF4ERR_NOERROR;
}

int cmdMcuGetGpio(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	MIBValue	*pGet;
	OID3		oid;

	VARAPI_StringToOid("2.103", &oid);	
	pGet = VARAPI_GetValue(&oid, pValue);
	if (pGet != NULL)
	{
		IF4API_AddResultMIBValue(pWrapper, pGet);
		VARAPI_FreeValue(pGet);
	}
	return IF4ERR_NOERROR;
}

int cmdMcuSetGpio(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
#if     defined(__INCLUDE_GPIO__)
	BYTE	nPort, nValue;

	nPort  = pValue[0].stream.u8;
	nValue = pValue[1].stream.u8;

	GPIOAPI_WriteGpio(nPort, nValue);
#endif
	return IF4ERR_NOERROR;
}

int cmdResetDevice(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;
	
	switch(pValue->stream.u8) {
	  case 0 :	// Mobile
		   // Power OFF/ON
           m_pSystemMonitor->PowerResetModem(FALSE);
		   return IF4ERR_NOERROR;

	  case 1 :	// SINK #1
		   CoordinatorPowerControl();
		   return IF4ERR_NOERROR;

	  case 2 :	// SINK #2
	  case 3 :	// xDSL
           return IF4ERR_DO_NOT_SUPPORT;
	}
	return IF4ERR_DO_NOT_SUPPORT;
}

int cmdMcuInstall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	mcuInstallEvent();
	return IF4ERR_NOERROR;
}

int cmdMcuUninstall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	mcuUninstallEvent();
	return IF4ERR_NOERROR;
}

int cmdOamLogin(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	*pszAddress;
	char	*pszUser;

	if (cnt != 2)
		return IF4ERR_INVALID_PARAM;

	if ((pValue[0].type != VARSMI_STRING) ||
		(pValue[1].type != VARSMI_STRING))
		return IF4ERR_INVALID_PARAM;

	pszAddress = pValue[0].stream.p;
	pszUser	   = pValue[1].stream.p;
	mcuOamLogin(pszAddress, pszUser);
	return IF4ERR_NOERROR;
}

int cmdOamLogout(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	*pszAddress;
	char	*pszUser;

	if (cnt != 2)
		return IF4ERR_INVALID_PARAM;

	if ((pValue[0].type != VARSMI_STRING) ||
		(pValue[1].type != VARSMI_STRING))
		return IF4ERR_INVALID_PARAM;

	pszAddress = pValue[0].stream.p;
	pszUser	   = pValue[1].stream.p;
	mcuOamLogout(pszAddress, pszUser);
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// SINK Control Command
//////////////////////////////////////////////////////////////////////

int cmdGetCodiList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	COORDINATOR	*pCoordinator;
	CODIENTRY	codi;
	int			nError;

	nError = codiSync(GetCoordinator(), 0);
	if (nError != CODIERR_NOERROR)
		return IF4ERR_CANNOT_GETLIST;

	pCoordinator = (COORDINATOR *)GetCoordinator();
	if (pCoordinator == NULL)
		return IF4ERR_INVALID_HANDLE;

	memset(&codi, 0, sizeof(CODIENTRY));
	memcpy(&codi.codiID, &pCoordinator->eui64, sizeof(EUI64));
	memcpy(codi.codiExtPanID, pCoordinator->extpanid, 8);
	memcpy(codi.codiLineKey, pCoordinator->link_key, 16);
	memcpy(codi.codiNetworkKey, pCoordinator->network_key, 16);

	codi.codiMask			= 0;
	codi.codiIndex			= 0;
    /*-- Issue #2079 : swVersion이 지원될 경우 이 값을 사용하고
     *-- 지원되지 않을 경우 fw 값을 사용한다. 
     *-- CLI에서 CODI_TYPE_ZIGBEE_STACK_32X 보다 큰 값이 codiType으로 
     *-- 오면 swVersion이 사용된 것으로 판단한다.
     --*/
    if(pCoordinator->swVersion) {
        codi.codiType = (pCoordinator->swVersion >> 8) & 0xFF;
    } else {
        switch(pCoordinator->fw >> 4) {
            case 1: codi.codiType = CODI_TYPE_ZIGBEE_STACK_30X; break;
            case 2: codi.codiType = CODI_TYPE_ZIGBEE_STACK_31X; break;
            case 3: codi.codiType = CODI_TYPE_ZIGBEE_STACK_32X; break;
            default: codi.codiType = CODI_TYPE_UNKNOWN; break;
        }
    }
	codi.codiShortID		= pCoordinator->shortid;
	codi.codiFwVer			= pCoordinator->fw;
	codi.codiHwVer			= pCoordinator->hw;
	codi.codiZAIfVer		= pCoordinator->zaif;
	codi.codiZZIfVer		= pCoordinator->zzif;
	codi.codiFwBuild		= pCoordinator->build;
	codi.codiResetKind		= pCoordinator->rk;
	codi.codiAutoSetting	= pCoordinator->autoset;
	codi.codiChannel		= pCoordinator->channel;
	codi.codiPanID			= pCoordinator->panid;
	codi.codiRfPower		= pCoordinator->power;
	codi.codiTxPowerMode	= pCoordinator->txpowermode;
	codi.codiPermit			= pCoordinator->permit_time;
	codi.codiEnableEncrypt= pCoordinator->security;
	codi.codiRouteDiscovery = pCoordinator->discovery;
	codi.codiMulticastHops  = pCoordinator->mhops;

	IF4API_AddResultFormat(pWrapper, "3.3", VARSMI_OPAQUE, &codi, sizeof(CODIENTRY));
	return IF4ERR_NOERROR;
}

int cmdGetCodiInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	COORDINATOR	*pCoordinator;
	CODIENTRY	codi;
	int			nError;

	if (cnt != 1)
		return IF4ERR_NEED_MORE_VALUE;
		
	if (VARAPI_OidCompare(&pValue[0].oid, "3.3.2") != 0)
		return IF4ERR_INVALID_PARAM;

	nError = codiSync(GetCoordinator(), 0);
	if (nError != CODIERR_NOERROR)
		return IF4ERR_CANNOT_GET;

	pCoordinator = (COORDINATOR *)GetCoordinator();
	if (pCoordinator == NULL)
		return IF4ERR_INVALID_HANDLE;

	memset(&codi, 0, sizeof(CODIENTRY));
	memcpy(&codi.codiID, &pCoordinator->eui64, sizeof(EUI64));
	memcpy(codi.codiExtPanID, pCoordinator->extpanid, 8);
	memcpy(codi.codiLineKey, pCoordinator->link_key, 16);
	memcpy(codi.codiNetworkKey, pCoordinator->network_key, 16);

	codi.codiMask			= 0;
	codi.codiIndex			= 0;
    if((pCoordinator->fw >> 4)&0x01) codi.codiType			= CODI_TYPE_ZIGBEE_STACK_30X;
    else if((pCoordinator->fw >> 4)&0x02) codi.codiType		= CODI_TYPE_ZIGBEE_STACK_31X;
	codi.codiShortID		= pCoordinator->shortid;
	codi.codiFwVer			= pCoordinator->fw;
	codi.codiHwVer			= pCoordinator->hw;
	codi.codiZAIfVer		= pCoordinator->zaif;
	codi.codiZZIfVer		= pCoordinator->zzif;
	codi.codiFwBuild		= pCoordinator->build;
	codi.codiResetKind		= pCoordinator->rk;
	codi.codiAutoSetting	= pCoordinator->autoset;
	codi.codiChannel		= pCoordinator->channel;
	codi.codiPanID			= pCoordinator->panid;
	codi.codiRfPower		= pCoordinator->power;
	codi.codiTxPowerMode	= pCoordinator->txpowermode;
	codi.codiPermit			= pCoordinator->permit_time;
	codi.codiEnableEncrypt	= pCoordinator->security;

	IF4API_AddResultFormat(pWrapper, "3.3", VARSMI_OPAQUE, &codi, sizeof(CODIENTRY));
	return IF4ERR_NOERROR;
}

int cmdSetCodiInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	// 복수 설정시 리셋 문제로 이 기능을 지원하지 않는다.
	return IF4ERR_DO_NOT_SUPPORT;
}

int cmdGetCodiDevice(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CODIDEVICEENTRY	device;
	COORDINATOR		*pCoordinator;
	HANDLE	codi;

	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

	pCoordinator = (COORDINATOR *)codi;
	memset(&device, 0, sizeof(CODIDEVICEENTRY));
    device.codiIndex = 0;
    memcpy(&device.codiID, &pCoordinator->eui64, sizeof(EUI64));
	memcpy(&device.codiDevice, pCoordinator->device.szDevice, 16);
	device.codiBaudRate  = pCoordinator->device.nSpeed;
	device.codiParityBit = pCoordinator->device.nParity;
	device.codiDataBit   = pCoordinator->device.nDataBit;
	device.codiStopBit   = pCoordinator->device.nStopBit;
	device.codiRtsCts    = pCoordinator->device.nOptions & CODI_OPT_RTSCTS ? 1 : 0;

	IF4API_AddResultFormat(pWrapper, "3.4", VARSMI_OPAQUE, &device, sizeof(CODIDEVICEENTRY));
	return IF4ERR_NOERROR;
}

int cmdSetCodiDevice(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	// Device 정보를 수정하면 안된다.
	return IF4ERR_DO_NOT_SUPPORT;
}

int cmdGetCodiBinding(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CODI_BINDING_PAYLOAD	*pBinding;	
	CODIBINDINGENTRY		binding;
	char	szBuffer[256];
	HANDLE	codi;
	int		nError, nLength=0;
	int		i, nCount;

	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

	nError = codiGetProperty(codi, CODI_CMD_BINDING_TABLE, (BYTE *)szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return IF4ERR_CANNOT_GET;
	
	pBinding = (CODI_BINDING_PAYLOAD *)szBuffer;
	nCount = nLength / sizeof(CODI_BINDING_ENTRY);

	for(i=0; i<nCount; i++)
	{
		memset(&binding, 0, sizeof(CODIBINDINGENTRY));
		ReverseEUI64(&pBinding->list[i].id, &binding.codiBindID);
		binding.codiBindIndex	= pBinding->list[i].index;
		binding.codiBindType	= pBinding->list[i].type;
		binding.codiBindLocal	= pBinding->list[i].local;
		binding.codiBindRemote	= pBinding->list[i].remote;
        binding.codiLastHeard 	= ntoshort(pBinding->list[i].lastheard);

		IF4API_AddResultFormat(pWrapper, "3.5", VARSMI_OPAQUE, &binding, sizeof(CODIDEVICEENTRY));
	}
	return IF4ERR_NOERROR;
}

int cmdGetCodiNeighbor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CODI_NEIGHBOR_PAYLOAD	*pNode;	
	CODINEIGHBORENTRY		node;
	char	szBuffer[256];
	HANDLE	codi;
	int		nError, nLength=0;
	int		i, nCount;

	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

	nError = codiGetProperty(codi, CODI_CMD_NEIGHBOR_TABLE, (BYTE *)szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return IF4ERR_CANNOT_GET;
	
	pNode = (CODI_NEIGHBOR_PAYLOAD *)szBuffer;
	nCount = nLength / sizeof(CODINEIGHBORENTRY);

	for(i=0; i<nCount; i++)
	{
		memset(&node, 0, sizeof(CODINEIGHBORENTRY));
		ReverseEUI64(&pNode->list[i].id, &node.codiNeighborID);
		node.codiNeighborIndex		= i;
		node.codiNeighborShortID	= ntoshort(pNode->list[i].networkid);
		node.codiNeighborLqi		= pNode->list[i].lqi;
		node.codiNeighborInCost		= pNode->list[i].in_cost;
		node.codiNeighborOutCost	= pNode->list[i].out_cost;
		node.codiNeighborAge		= pNode->list[i].age;

		IF4API_AddResultFormat(pWrapper, "3.6", VARSMI_OPAQUE, &node, sizeof(CODINEIGHBORENTRY));
	}
	return IF4ERR_NOERROR;
}

int cmdGetCodiMemory(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CODI_STACKMEMORY_PAYLOAD	*pStack;	
	CODIMEMORYENTRY				mem;
    BYTE*                       pMem;
	char	szBuffer[256];
	HANDLE	codi;
	int		nError, nLength=0, nLen;

	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

	nError = codiGetProperty(codi, CODI_CMD_STACK_MEMORY, (BYTE *)szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return IF4ERR_CANNOT_GET;
	
	pStack = (CODI_STACKMEMORY_PAYLOAD *)szBuffer;
	memset(&mem, 0, sizeof(CODIMEMORYENTRY));

    nLen = MIN(nLength, (int) sizeof(CODIMEMORYENTRY)-9); // index(1) + id (8)
    /** CODIMEMORYENTRY의 경우 CODI_STACKMEMORY_PAYLOAD에 prefix로 codiIndex 및 codiID를 추가한 
      * 형태를 띄고 있기 때문에 길이 계산을 해서 memcpy를 한다
      */
    pMem = (BYTE*)&mem;
    memcpy(pMem + 9, pStack, nLen);
    mem.codiIndex = 0;
    memcpy(&mem.codiID, &((COORDINATOR *)codi)->eui64, sizeof(EUI64));

	IF4API_AddResultFormat(pWrapper, "3.7", VARSMI_OPAQUE, &mem, sizeof(CODIMEMORYENTRY));

	return IF4ERR_NOERROR;
}

int cmdGetCodiRegister(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CODI_KEY_TABLE_PAYLOAD	*pNode;	
	CODIREGENTRY		node;
	HANDLE	codi;
	char	szBuffer[256+32];
	int		nError, nLength=0;
	int		i, nCount;

	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

	nError = codiGetProperty(codi, CODI_CMD_KEY_TABLE_ENTRY, (BYTE *)szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return IF4ERR_CANNOT_GET;
	
	pNode = (CODI_KEY_TABLE_PAYLOAD *)szBuffer;
	nCount = nLength / sizeof(CODIREGENTRY);

    XDEBUG("cmdGetCodiRegister nCount=%d nLength=%d\r\n", nCount, nLength);

	for(i=0; i<nCount; i++)
	{
		memset(&node, 0, sizeof(CODIREGENTRY));
        node.codiRegIndex = i;
        node.codiRegStatus = pNode->list[i].status;
		ReverseEUI64(&pNode->list[i].id, &node.codiRegID);

		IF4API_AddResultFormat(pWrapper, "3.8", VARSMI_OPAQUE, &node, sizeof(CODIREGENTRY));
	}
	return IF4ERR_NOERROR;
}

int cmdSetCodiRegister(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CODI_KEY_PAYLOAD	pPayload;	
	HANDLE	codi;
	int		nError;

    if(cnt < 1 || !IsValidEUI64(&pValue[0].oid))
    {
        return IF4ERR_INVALID_PARAM;
    }

	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

    memset(&pPayload, 0, sizeof(CODI_KEY_PAYLOAD));

    pPayload.type = 0x01; // Set key table
    ReverseEUI64((EUI64 *)pValue[0].stream.p, &pPayload.id);

	nError = codiSetProperty(codi, CODI_CMD_KEY_TABLE, (BYTE *)&pPayload, sizeof(CODI_KEY_PAYLOAD),  3000);

	if (nError != CODIERR_NOERROR)
		return IF4ERR_CANNOT_GET;
	
	return IF4ERR_NOERROR;
}

int cmdDeleteCodiRegister(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CODI_KEY_PAYLOAD	pPayload;	
	HANDLE	codi;
	int		nError;

    if(cnt < 1 || !IsValidEUI64(&pValue[0].oid))
    {
        return IF4ERR_INVALID_PARAM;
    }

	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

    memset(&pPayload, 0, sizeof(CODI_KEY_PAYLOAD));

    pPayload.type = 0x02; // Delete key table
    ReverseEUI64((EUI64 *)pValue[0].stream.p, &pPayload.id);

	nError = codiSetProperty(codi, CODI_CMD_KEY_TABLE, (BYTE *)&pPayload, sizeof(CODI_KEY_PAYLOAD),  3000);

	if (nError != CODIERR_NOERROR)
		return IF4ERR_CANNOT_GET;
	
	return IF4ERR_NOERROR;
}

int cmdSetCodiPermit(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CODI_PERMIT_PAYLOAD     permit;
	HANDLE	codi;
	int		nError;

	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

	permit.permit_time = pValue->stream.u8;
	nError = codiSetProperty(codi, CODI_CMD_PERMIT, (BYTE *)&permit, 
								sizeof(CODI_PERMIT_PAYLOAD), 3000);
	if (nError == CODIERR_NOERROR)
		return IF4ERR_NOERROR;

	return IF4ERR_CANNOT_CHANGE;
}

int cmdSetCodiFormNetwork(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CODI_FORMNETWORK_PAYLOAD	form;
	HANDLE	codi;
	int		nError;

	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

	form.type = pValue->stream.u8;
	nError = codiSetProperty(codi, CODI_CMD_FORM_NETWORK, (BYTE *)&form,
								sizeof(CODI_FORMNETWORK_PAYLOAD), 3000);
	if (nError == CODIERR_NOERROR)
		return IF4ERR_NOERROR;

	return IF4ERR_CANNOT_CHANGE;
}

int cmdSetCodiReset(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    CoordinatorPowerControl();
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// SENSOR Control Command
//////////////////////////////////////////////////////////////////////

int cmdAddSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
#if 0
	BYTE		nType, nVendor, nService;
	BYTE		nFlags;
#endif
	char		szSerial[32];
	int			i, nSeek/*, nPort*/;
    EUI64       id;

	memset(&id, 0, sizeof(EUI64));
	*szSerial = '\0';
	//nVendor   = 0;
	//nService  = 0;
	//nFlags	  = 0;
    //nPort     = 0;

	for(i=0, nSeek=0; i<cnt; i++)
	{
		if (IsValidEUI64(&pValue[i].oid))
		{
			if (nSeek != 0)
			{
				// 수동의 경우 Inventory scanning 되야 등록된다.
	            //m_pEndDeviceList->SetEndDeviceByID(&id, "", szSerial, 0);
	            if (m_pEndDeviceList->GetEndDeviceByID(&id) == NULL)
					m_pInventoryScanner->Add(&id);
				else return IF4ERR_DUPLICATE;

				memset(&id, 0, sizeof(EUI64));
				*szSerial = '\0';
#if 0
				nType 	  = 0;
				nVendor   = 0;
				nService  = 0;
				nFlags	  = 0;
#endif
				nSeek 	  = 0;
			}
			memcpy(&id, pValue[i].stream.p, sizeof(EUI64));
			nSeek++;
		}
		else if (VARAPI_OidCompare(&pValue[i].oid, "4.1.2") == 0)
		{
			//nType = pValue[i].stream.u8;
			nSeek++;
		}
		else if (VARAPI_OidCompare(&pValue[i].oid, "4.1.8") == 0)
		{
			//nFlags = (pValue[i].stream.u8 & SENSOR_FLAG_NO_METERING);
			nSeek++;
		}
		else if (VARAPI_OidCompare(&pValue[i].oid, "4.1.9") == 0)
		{
			//nVendor = pValue[i].stream.u8;
			nSeek++;
		}
		else if (VARAPI_OidCompare(&pValue[i].oid, "4.1.10") == 0)
		{
			strcpy(szSerial, pValue[i].stream.p);
			nSeek++;
		}
		else if (VARAPI_OidCompare(&pValue[i].oid, "4.1.11") == 0)
		{
			//nService = pValue[i].stream.u8;
			nSeek++;
		}
	}

	if (nSeek != 0)
	{
		// 수동의 경우 Inventory scanning 되야 등록된다.
	    //m_pEndDeviceList->SetEndDeviceByID(&id, "", szSerial, 0);
	    if (m_pEndDeviceList->GetEndDeviceByID(&id) == NULL)
			m_pInventoryScanner->Add(&id);
		else return IF4ERR_DUPLICATE;
	}
		
	return IF4ERR_NOERROR;
}

BOOL LeaveSensor(HANDLE codi, ENDIENTRY *pEndDevice, BYTE channel, WORD panid)
{
	char	szGUID[17];

	// Leave End Device	
	eui64toa(&pEndDevice->id, szGUID);
	XDEBUG("LEAVE NETWOKR (ID=%s, PARSER=%s, CH=%d, PID=%d)\r\n", 
           szGUID, pEndDevice->szParser, channel, panid);
	if (m_pOndemander->Leave(codi, &pEndDevice->id, 12, channel, panid) == IF4ERR_NOERROR)
		sensorLeaveEvent(&pEndDevice->id);

	return TRUE;
}

int cmdDeleteSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY	*pEndDevice;
	HANDLE		codi;
	EUI64		*id;
	int			i, idcnt=0;
    BYTE        channel = 0;
    WORD        panid = 0;
    BOOL        bEUI64 = TRUE;

	for(i=0; i<cnt; i++)
	{
        if (pValue[i].type == VARSMI_BYTE) {
            bEUI64 = FALSE;
            channel = pValue[i].stream.u8;
        } else if ((pValue[i].type == VARSMI_WORD) && !bEUI64) {
            panid = pValue[i].stream.u16;
        } else if ((pValue[i].type == VARSMI_EUI64) && (pValue[i].len == sizeof(EUI64)) && bEUI64) {
            idcnt ++;
        } else {
			return IF4ERR_INVALID_PARAM;
        }
	}

	codi = GetCoordinator();
	for(i=0; i<idcnt; i++)
	{
		id = (EUI64 *)pValue[i].stream.p;
		pEndDevice = m_pEndDeviceList->GetEndDeviceByID(id);
		if (pEndDevice != NULL)
		{
			// Leave End Device
			LeaveSensor(codi, pEndDevice, channel, panid);

			// Delete End Device form list
			if (m_pEndDeviceList->DeleteEndDeviceByID(&pEndDevice->id))
			    sensorUninstallEvent(id);
		}
		else return IF4ERR_INVALID_ID;
	}
	return IF4ERR_NOERROR;
}

int cmdDeleteSensorAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	HANDLE		codi;

    ENDI_LEAVE_PAYLOAD  *payload;
    BYTE        szBuffer[100];
    int         nError, i;

#if     defined(__SUPPORT_ADVANCED_LEAVE__)
    BYTE        channel = 0;
    WORD        panid = 0;

    if(cnt > 0) {
        channel = pValue[0].stream.u8;
    }
    if(cnt > 1) {
        panid = pValue[1].stream.u16;
    }
#endif

	codi = GetCoordinator();

    payload = (ENDI_LEAVE_PAYLOAD *) &szBuffer;
    payload->type = ENDI_LEAVE_NETWORK_REQUEST;
    payload->join = 12;
#if     defined(__SUPPORT_ADVANCED_LEAVE__)
    payload->channel = channel;
    payload->panid = panid;
#endif

    for(i=0;i<2;i++) {
	    nError = codiMulticast(codi, ENDI_DATATYPE_NETWORK_LEAVE, (BYTE *)&szBuffer, sizeof(ENDI_LEAVE_PAYLOAD));

	    if (nError != CODIERR_NOERROR)
	    {
		    XDEBUG("---------------------------------------------\r\n");
		    XDEBUG("DELETE-SENSORALL-ERROR: %s(%d)\r\n", codiErrorMessage(nError), nError);
		    XDEBUG("---------------------------------------------\r\n");
	    }
        usleep(2000000);
    }

	m_pEndDeviceList->DeleteEndDeviceAll();

	return IF4ERR_NOERROR;
}

int cmdGetSensorCount(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	IF4API_AddResultNumber(pWrapper, "1.9", VARSMI_INT, m_pEndDeviceList->GetCount());
	return IF4ERR_NOERROR;
}

void AddSensorPathToResult(IF4Wrapper *pWrapper, ENDIENTRY *pEndDevice, SENSORPATHENTRY *pEntry)
{
    WORD    shortid=0;
    BYTE    hops=0;
    WORD    path[CODI_MAX_ROUTING_PATH];

    if (pEndDevice == NULL)
		return;

	memset(pEntry, 0, sizeof(SENSORPATHENTRY));
    memset(path,0,sizeof(path));

	memcpy(&pEntry->sensorPathID, &pEndDevice->id, sizeof(EUI64));
    if(!(pEndDevice->nAttribute & ENDDEVICE_ATTR_MBUS_MASTER))
	    strncpy(pEntry->sensorPathSerial, pEndDevice->szSerial, 18);
	strncpy(pEntry->sensorPathModel, pEndDevice->szModel, 18);
    pEntry->sensorPathLQI = pEndDevice->LQI;
    pEntry->sensorPathRSSI = pEndDevice->RSSI;

    if(m_pEndDeviceList->QueryRouteRecord(pEndDevice, &shortid, &hops, path, TRUE)) {

        pEntry->sensorPathShortID = shortid;
        pEntry->sensorPathHops = hops;
        memcpy(pEntry->sensorPathNode, path, sizeof(WORD) * CODI_MAX_ROUTING_PATH);

	    GetTimestamp(&pEntry->sensorPathTime, &pEndDevice->tmRouteRecord);

	    IF4API_AddResultFormat(pWrapper, "4.4", VARSMI_OPAQUE, pEntry, sizeof(SENSORPATHENTRY));
    }
}

BOOL EnumSensorPathCallback(ENDIENTRY *pEndDevice, void *pWrapper, void *pszBuffer, void *filter)
{
	SENSORPATHENTRY	*pEntry = (SENSORPATHENTRY *)pszBuffer;
    char * parser = (char *) filter;

	if (pEntry == NULL)
		return FALSE;

    if(!parser || !strncasecmp(parser,pEndDevice->szParser,strlen(parser))) {
	    AddSensorPathToResult((IF4Wrapper*)pWrapper, pEndDevice, pEntry);
	    return TRUE;
    }

	return TRUE;
}

BOOL EnumSelectShortIdCallback(ENDIENTRY *pEndDevice, void *pWrapper, void *pszBuffer, void *pShortId)
{
	SENSORPATHENTRY	*pEntry = (SENSORPATHENTRY *)pszBuffer;
    WORD *pSId = (WORD *) pShortId;
    int i;

	if (pEntry == NULL || pSId == NULL)
		return FALSE;

    if(pEndDevice->hops > 0) {
        if(pEndDevice->parentid == (*pSId)) {
	        AddSensorPathToResult((IF4Wrapper*)pWrapper, pEndDevice, pEntry);
	        return TRUE;
        }
    }

    if(pEndDevice->nAttribute & ENDDEVICE_ATTR_RFD) {
	    pEndDevice = m_pEndDeviceList->FindEndDeviceByShortID(pEndDevice->parentid);
    }

    for(i=0; pEndDevice && i<pEndDevice->hops; i++) {
        if(pEndDevice->path[i] == (*pSId)) {
	        AddSensorPathToResult((IF4Wrapper*)pWrapper, pEndDevice, pEntry);
	        return TRUE;
        }
    }

	return TRUE;
}

int cmdGetSensorPath(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    char    parser[256];
	int		nCount, i;
    SENSORPATHENTRY entry;
    ENDIENTRY * pEndDevice;
    EUI64   id;
    WORD    shortid=0;
    BYTE    hops=0;
    WORD    path[CODI_MAX_ROUTING_PATH];

    memset(parser,0,sizeof(parser));
    if(cnt > 0) {
	    if (IsValidEUI64(&pValue[0].oid))
        {
	        memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
            if((pEndDevice = m_pEndDeviceList->GetEndDeviceByID(&id))) {
                AddSensorPathToResult(pWrapper, pEndDevice, &entry);
                /** 상향 Path 정보 수집 */
                if(m_pEndDeviceList->QueryRouteRecord(pEndDevice, &shortid, &hops, path, FALSE)) {
                    for(i=0;i < hops; i++) {
                        if((pEndDevice = m_pEndDeviceList->GetEndDeviceByShortID(path[i]))) {
                            AddSensorPathToResult(pWrapper, pEndDevice, &entry);
                        }
                    }
                }
                /** 자기 Path 정보에 해당 ID의 Short ID가 있는 모든 Device 정보 수집 */
                m_pEndDeviceList->EnumEndDevice(EnumSelectShortIdCallback, (void *)pWrapper, (char *)&entry, (void *)&shortid);
                return IF4ERR_NOERROR;
            } else {
                return IF4ERR_NO_ENTRY;
            }
        }else {
            memcpy(parser,pValue->stream.p, pValue->len > (int) sizeof(parser) - 1 ? (int) sizeof(parser) - 1 : pValue->len);
        }
    }

    nCount = m_pEndDeviceList->EnumEndDevice(EnumSensorPathCallback, (void *)pWrapper, (char *)&entry, parser[0] ? parser : NULL);

	return (nCount <= 0) ? IF4ERR_NO_ENTRY : IF4ERR_NOERROR;
}

void AddSensorToResultNew(IF4Wrapper *pWrapper, ENDIENTRY *pEndi, SENSORINFOENTRYNEW *pEntry)
{
	METERDEVICENODE	*pMeter = NULL;

    if (pEndi == NULL)
		return;

	memset(pEntry, 0, sizeof(SENSORINFOENTRYNEW));
	memcpy(&pEntry->sensorID, &pEndi->id, sizeof(EUI64));
	GetTimestamp(&pEntry->sensorLastConnect, &pEndi->tmMetering);
	GetTimestamp(&pEntry->sensorInstallDate, &pEndi->tmInstall);
    if(!(pEndi->nAttribute & ENDDEVICE_ATTR_MBUS_MASTER))
	    strncpy(pEntry->sensorSerialNumber, pEndi->szSerial, 18);
	strncpy(pEntry->sensorModel, pEndi->szModel, 18);
	pEntry->sensorHwVersion = pEndi->hw;
	pEntry->sensorFwVersion = pEndi->fw;
	pEntry->sensorState 	= pEndi->nState;
	pEntry->sensorAttr		= pEndi->nAttribute;
	pEntry->sensorOTAState		= pEndi->installState;
	pEntry->sensorFwBuild		= pEndi->build;
	pEntry->sensorParser		= pEndi->nParser;
	GetTimestamp(&pEntry->sensorLastOTATime, &pEndi->tmLastFwUpdateTime);

	IF4API_AddResultFormat(pWrapper, "4.3", VARSMI_OPAQUE, pEntry, sizeof(SENSORINFOENTRYNEW));
	if(pEndi->nAttribute & ENDDEVICE_ATTR_MBUS_MASTER)
    {
        memset(pEntry, 0, sizeof(SENSORINFOENTRYNEW));
        memcpy(&pEntry->sensorID, &pEndi->id, sizeof(EUI64));
        GetTimestamp(&pEntry->sensorLastConnect, &pEndi->tmMetering);
        GetTimestamp(&pEntry->sensorInstallDate, &pEndi->tmInstall);
        strncpy(pEntry->sensorModel, pEndi->szSubModel, 18);
        pEntry->sensorHwVersion = pEndi->hw;
        pEntry->sensorFwVersion = pEndi->subfw;
        pEntry->sensorState 	= pEndi->nState;
	    pEntry->sensorAttr		= ENDDEVICE_ATTR_MBUS_ARM;
        pEntry->sensorOTAState		= pEndi->installState;
        pEntry->sensorFwBuild		= pEndi->subbuild;
        pEntry->sensorParser		= pEndi->nParser;
        GetTimestamp(&pEntry->sensorLastOTATime, &pEndi->tmLastFwUpdateTime);
        IF4API_AddResultFormat(pWrapper, "4.3", VARSMI_OPAQUE, pEntry, sizeof(SENSORINFOENTRYNEW));

        while((pMeter = m_pMeterDeviceList->GetMeter(&pEndi->id, pMeter)) != NULL)
        {
            memset(pEntry, 0, sizeof(SENSORINFOENTRYNEW));
            memcpy(&pEntry->sensorID, &pEndi->id, sizeof(EUI64));
            GetTimestamp(&pEntry->sensorLastConnect, &pMeter->tmMetering);
            GetTimestamp(&pEntry->sensorInstallDate, &pEndi->tmInstall);
            strncpy(pEntry->sensorSerialNumber, pMeter->szSerial, 18);
            strncpy(pEntry->sensorModel, pMeter->szMenufa, MIN(sizeof(pMeter->szMenufa), sizeof(pEntry->sensorModel)));
            pEntry->sensorHwVersion = pMeter->szMedia;              // HwVersion을 Media 값 전달에 사용한다.
            pEntry->sensorFwVersion = pEndi->fw;
            pEntry->sensorState 	= pMeter->szstatus;
            pEntry->sensorAttr		= ENDDEVICE_ATTR_MBUS_SLAVE;
            pEntry->sensorOTAState		= pEndi->installState;
	        pEntry->sensorFwBuild		= pMeter->gid.gport;        // Build를 Port값 전달에 사용한다.
            pEntry->sensorParser		= pEndi->nParser;
            GetTimestamp(&pEntry->sensorLastOTATime, &pEndi->tmLastFwUpdateTime);
            IF4API_AddResultFormat(pWrapper, "4.3", VARSMI_OPAQUE, pEntry, sizeof(SENSORINFOENTRYNEW));
        }
    }
}

void AddSensorToResultMetermbus(IF4Wrapper *pWrapper, METERDEVICENODE *pMeter, METERENTRYMBUS *pEntry)
{
    if (pMeter == NULL)
        return;

    memset(pEntry, 0, sizeof(METERENTRYMBUS));
    memcpy(&pEntry->mmtrGid, &pMeter->gid, sizeof(GEUI64));
    strncpy(pEntry->mmtrSerial, pMeter->szSerial, 22);
    strncpy(pEntry->mmtrMenufa, pMeter->szMenufa, 22);
    pEntry->mmtrGener       = pMeter->szGener;
    pEntry->mmtrMedia       = pMeter->szMedia;
    pEntry->mmtrCntAccess   = pMeter->szCntAccess;
    pEntry->mmtrStatus      = pMeter->szstatus;
    GetTimestamp(&pEntry->mmtrTMMetering, &pMeter->tmMetering);

    IF4API_AddResultFormat(pWrapper, "10.4", VARSMI_OPAQUE, pEntry, sizeof(METERENTRYMBUS));
}

BOOL EnumMeterCallback(METERDEVICENODE *pMeter, void *pWrapper, void *pszBuffer, void *filter)
{
    METERENTRYMBUS  *pEntry = (METERENTRYMBUS *)pszBuffer;
    //char * parser = (char *) filter;
    if (pEntry == NULL)
        return FALSE;
//필터는 없다. 
//    if(!parser || !strncasecmp(parser,pMeter->szParser,strlen(parser))) {
        AddSensorToResultMetermbus((IF4Wrapper*)pWrapper, pMeter, pEntry);
        return TRUE;
}

BOOL EnumSensorCallbackNew(ENDIENTRY *pEndi, void *pWrapper, void *pszBuffer, void *filter)
{
	SENSORINFOENTRYNEW	*pEntry = (SENSORINFOENTRYNEW *)pszBuffer;
    char * parserName = (char *) filter;
    CMeterParser * pParser = NULL;

	if (pEntry == NULL)
		return FALSE;

    if(!parserName) {
	    AddSensorToResultNew((IF4Wrapper*)pWrapper, pEndi, pEntry);
	    return TRUE;
    }
    else {
        pParser = m_pMeterParserFactory->SelectParser(pEndi->szParser);
        if(pParser && m_pMeterParserFactory->IsMatchedParser(*pParser, parserName)) {
	        AddSensorToResultNew((IF4Wrapper*)pWrapper, pEndi, pEntry);
	        return TRUE;
        }
    }
    return FALSE;
}

int cmdGetSensorAllNew(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    char            parser[256];
	char			*pszBuffer;
	int				nCount;

    memset(parser,0,sizeof(parser));
    if(cnt > 0) {
        memcpy(parser,pValue->stream.p, pValue->len > (int) sizeof(parser) - 1 ? (int) sizeof(parser) - 1 : pValue->len);
    }

	pszBuffer = (char *)MALLOC(sizeof(SENSORINFOENTRYNEW));
	if (pszBuffer == NULL)
		return IF4ERR_MEMORY_ERROR;

    nCount = m_pEndDeviceList->EnumEndDevice(EnumSensorCallbackNew, (void *)pWrapper, pszBuffer, parser[0] ? parser : NULL);
	FREE(pszBuffer);

	return (nCount <= 0) ? IF4ERR_NO_ENTRY : IF4ERR_NOERROR;
}

BOOL EnumSensorCallbackValue(ENDIENTRY *pEndi, void *pWrapper, void *parserFilter, void *idFilter)
{
    EUI64 *id = (EUI64 *) idFilter;
    char *parser = (char *) parserFilter;
    MIBValue *pMibValue = NULL;
    MIBValue *pmib = NULL;
    int nCount = 0;
    TIMESTAMP stamp;

    if(pEndi == NULL) return FALSE;
    if(id != NULL && memcmp(id, &pEndi->id, sizeof(EUI64))) return FALSE;
    if(parser != NULL && strncasecmp(parser, pEndi->szParser, strlen(parser))) return FALSE;

    memset(&stamp, 0, sizeof(TIMESTAMP));
    GetTimestamp(&stamp, &pEndi->tmValueChange);

	IF4API_AddResultFormat((IF4Wrapper *)pWrapper, "1.14", VARSMI_EUI64, &pEndi->id, sizeof(EUI64));
	IF4API_AddResultFormat((IF4Wrapper *)pWrapper, "1.11", VARSMI_STRING, pEndi->szModel, 
            strlen(pEndi->szModel) < sizeof(pEndi->szModel) ? strlen(pEndi->szModel) : sizeof(pEndi->szModel) );
	IF4API_AddResultNumber((IF4Wrapper *)pWrapper, "1.4", VARSMI_BYTE, pEndi->nAttribute);
	IF4API_AddResultFormat((IF4Wrapper *)pWrapper, "1.11", VARSMI_STRING, pEndi->szSerial, 
            strlen(pEndi->szSerial) < sizeof(pEndi->szSerial) ? strlen(pEndi->szSerial) : sizeof(pEndi->szSerial) );
	IF4API_AddResultFormat((IF4Wrapper *)pWrapper, "1.16", VARSMI_TIMESTAMP, &stamp, sizeof(TIMESTAMP));

    {
        char szPath[256]={0,};
        char szGUID[17]={0,};
	    eui64toa(&pEndi->id, szGUID);
        sprintf(szPath, "%s/%s.value", ENDI_VALUE_PATH, szGUID);
        VARAPI_Deserialize(szPath, &pMibValue, &nCount);
        IF4API_AddResultNumber((IF4Wrapper *)pWrapper, "1.5", VARSMI_WORD, (WORD)(nCount / 4));

        pmib = pMibValue;
        while(pmib != NULL)
        {
            IF4API_AddResultMIBValue((IF4Wrapper *)pWrapper, pmib);
            pmib = pmib->pNext;
        }
    }

    return TRUE;
}

int cmdGetSensorValue(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64           id;
    int             nCount=0;
    char            parser[256];
    BOOL            bId = FALSE;

    memset(parser,0,sizeof(parser));
    if(cnt > 0) {
	    if (IsValidEUI64(&pValue[0].oid))
        {
            bId = TRUE;
	        memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
        }else {
            memcpy(parser,pValue->stream.p, pValue->len > (int) sizeof(parser) - 1 ? (int) sizeof(parser) - 1 : pValue->len);
        }
    }

    nCount = m_pEndDeviceList->EnumEndDevice(EnumSensorCallbackValue, (void *)pWrapper, parser[0] ? parser : NULL, bId ? &id : NULL, 
            CEndDeviceList::compAttrModelId);

	return (nCount <= 0) ? IF4ERR_NO_ENTRY : IF4ERR_NOERROR;
}


int cmdGetMeterList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    char            parser[256];
    char            *pszBuffer;
    int             nCount;

    memset(parser,0,sizeof(parser));
    if(cnt > 0) {
        memcpy(parser,pValue->stream.p, pValue->len > (int) sizeof(parser) - 1 ? (int) sizeof(parser) - 1 : pValue->len);
    }

    pszBuffer = (char *)MALLOC(sizeof(METERENTRYMBUS));
    if (pszBuffer == NULL)
        return IF4ERR_MEMORY_ERROR;
    nCount = m_pMeterDeviceList->EnumMeterDevice(EnumMeterCallback, (void *)pWrapper, pszBuffer, parser[0] ? parser : NULL);
    FREE(pszBuffer);

    return (nCount <= 0) ? IF4ERR_NO_ENTRY : IF4ERR_NOERROR;
}


int cmdGetSensorInfoNew(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY			*pEndi;
	SENSORINFOENTRYNEW	*pEntry;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (pValue->len != 8)
		return IF4ERR_INVALID_ID;

	pEndi = m_pEndDeviceList->GetEndDeviceByID((EUI64 *)pValue->stream.p);
	if (pEndi == NULL)
		return IF4ERR_INVALID_ID;

	pEntry = (SENSORINFOENTRYNEW *)MALLOC(sizeof(SENSORINFOENTRYNEW));
	if (pEntry != NULL)
	{
		AddSensorToResultNew(pWrapper, pEndi, pEntry);
		FREE(pEntry);
	}
	return IF4ERR_NOERROR;
}

int cmdPingSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	return IF4ERR_DEPRECATED_FUNCTION;
}

int cmdSetSensorTime(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	return IF4ERR_DEPRECATED_FUNCTION;
}

int cmdGetSensorMultiROM(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 	id;
    int 	nError = IF4ERR_NOERROR, i;
    char 	szId[32];
    ParamAddr *param = NULL;
    ParamAddr *tmp = NULL;
    ReturnVal *rval = NULL;
    ReturnVal *prev;

    if(cnt < 3 || (cnt % 2 == 0))
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
    eui64toa(&id, szId);

    for(i=1; i<cnt;i+=2) {
	    if ((VARAPI_OidCompare(&pValue[i].oid, "1.5") != 0) &&
	        (VARAPI_OidCompare(&pValue[i+1].oid, "1.5") != 0)) {
		    nError = IF4ERR_INVALID_PARAM;
            break;
        }

        if(param) {
            tmp->next = (ParamAddr *)MALLOC(sizeof(ParamAddr));
	        if (tmp->next == NULL) {
		        nError = IF4ERR_MEMORY_ERROR;
			    XDEBUG("ERRROR: multi rom read fail: Malloc fail\r\n");
                break;
            }
            tmp = tmp->next;
        } else {
            param = (ParamAddr *)MALLOC(sizeof(ParamAddr));
	        if (param == NULL) {
		        nError = IF4ERR_MEMORY_ERROR;
			    XDEBUG("ERRROR: multi rom read fail: Malloc fail\r\n");
                break;
            }
            tmp = param;
        }
        memset(tmp, 0, sizeof(ParamAddr));
        
        tmp->addr = pValue[i].stream.u16;
        tmp->len = pValue[i+1].stream.u16;
    }

    if(nError == IF4ERR_NOERROR) {
	    nError = m_pOndemander->RomRead((HANDLE)GetCoordinator(), &id, param, &rval);
    }

    if (nError == IF4ERR_NOERROR) 
    {
        prev = rval;
        tmp = param;
        while (tmp) {
            if(prev == NULL) {
                XDEBUG("GetSensorMultiROM Fail: id=%s, NULL return value\r\n", szId);
                nError = IF4ERR_CANNOT_GET;
                break;
            }else {
                if (prev->len != tmp->len)
                {
                    XDEBUG("GetSensorMultiROM Fail: id=%s, reqlen=%d, reslen=%d\r\n", szId, tmp->len, prev->len);
                    nError = IF4ERR_CANNOT_GET;
                }else {
	                IF4API_AddResultOpaque(pWrapper, "1.12", prev->val.pVal, prev->len);
                }
            }
            tmp = tmp->next;
            prev = prev->next;
        } 
    }
    else 
    {
        XDEBUG("GetSensorMultiROM Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
    } 

    while(param) {
        tmp = param;
        param = param->next;
        FREE(tmp);
    }
    while(rval) {
        prev = rval;
        rval = rval->next;
        if(prev->val.pVal) {
            FREE(prev->val.pVal);
        }
        FREE(prev);
    }
    return nError;
}

int cmdSetSensorROM(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 id;
    WORD addr;
    int nError, len;
    BYTE *buffer;
    char szId[32]={0,};
    BOOL bAll = FALSE;

    if(cnt != 3)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid)) {
        if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0 || strncasecmp(pValue[0].stream.p,"all",3)) 
		    return IF4ERR_INVALID_ID;
        else {
            bAll = TRUE;
        }
    }

	if (VARAPI_OidCompare(&pValue[1].oid, "1.5") != 0 &&
	        VARAPI_OidCompare(&pValue[2].oid, "1.12") != 0)
		return IF4ERR_INVALID_PARAM;

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

    if(!bAll) {
	    memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
    }
    addr = pValue[1].stream.u16;
    len = pValue[2].len;
    buffer = (BYTE *) pValue[2].stream.p;
	   

       
    if(bAll) {
	    nError = m_pOndemander->RomWrite(GetCoordinator(), NULL, addr, buffer, len);
    }else {
	    nError = m_pOndemander->RomWrite(GetCoordinator(), &id, addr, buffer, len);
    }
	if (nError != IF4ERR_NOERROR) {
        eui64toa(&id, szId);
        XDEBUG("SetSensorROM Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
    }

    return nError;
}

int cmdGetSensorEvent(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 	id;
    int 	nError = IF4ERR_NOERROR;
    BYTE    nCount=0;
    char 	szId[32];
    ParamAddr param[2];
    ParamAddr *pParam;
    ParamAddr *tmp;
    ReturnVal *info = NULL;
    ReturnVal *evt = NULL;
    ReturnVal *prev;
    ENDI_EVENTLOG_INFO   *eventPointer;

    if(cnt < 2)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
    eui64toa(&id, szId);

    nCount = pValue[1].stream.u8; 

    memset(&param, 0, sizeof(ParamAddr) * 2);
    param[0].addr = (WORD) ENDI_ROMAP_EVENT_POINTER;
    param[0].len = (WORD) sizeof(ENDI_EVENTLOG_INFO);

    pParam = (ParamAddr *)&param[0];

	if((nError = m_pOndemander->RomRead((HANDLE)GetCoordinator(), &id, pParam, &info, FALSE, FALSE, TRUE)) == IF4ERR_NOERROR) 
    {
        eventPointer = (ENDI_EVENTLOG_INFO *)info->val.pVal;

        if(eventPointer->EVENT_POINTER >= nCount) {
            param[0].addr = ENDI_ROMAP_EVENT_LOGDATA  + (((eventPointer->EVENT_POINTER - nCount + 1)+250)%250)*sizeof(ENDI_EVENT_ENTRY);
            param[0].len = sizeof(ENDI_EVENT_ENTRY) * nCount;
        }else {
            param[0].addr = ENDI_ROMAP_EVENT_LOGDATA;
            param[0].len = sizeof(ENDI_EVENT_ENTRY) * (eventPointer->EVENT_POINTER + 1);

            param[1].addr = ENDI_ROMAP_EVENT_LOGDATA + ((250 - nCount + eventPointer->EVENT_POINTER + 1)%250) * sizeof(ENDI_EVENT_ENTRY);
            param[1].len = sizeof(ENDI_EVENT_ENTRY) * (nCount - eventPointer->EVENT_POINTER - 1);
            param[0].next = &param[1];
        }

	    if((nError = m_pOndemander->RomRead((HANDLE)GetCoordinator(), &id, pParam, &evt, TRUE, TRUE, FALSE)) == IF4ERR_NOERROR) 
        {
            prev = evt;
            tmp = pParam;
            while (tmp) {
                if(prev == NULL) {
                    XDEBUG("GetSensorEvent Fail: id=%s, NULL return value\r\n", szId);
                    nError = IF4ERR_CANNOT_GET;
                    break;
                }else {
                    if (prev->len != tmp->len)
                    {
                        XDEBUG("GetSensorEvent Fail: id=%s, reqlen=%d, reslen=%d\r\n", szId, tmp->len, prev->len);
                        nError = IF4ERR_CANNOT_GET;
                    }else {
	                    IF4API_AddResultOpaque(pWrapper, "1.12", prev->val.pVal, prev->len);
                    }
                }
                tmp = tmp->next;
                prev = prev->next;
            } 
        }
        else 
        {
            XDEBUG("GetSensorEvent Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
        } 

        if(evt && evt->val.pVal) FREE(evt->val.pVal);
        FREE(evt);
        if(info && info->val.pVal) FREE(info->val.pVal);
        FREE(info);
    }
    else 
    {
        XDEBUG("GetSensorEvent Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
    } 

    return nError;
}

int cmdGetSensorBattery(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 	id;
    int 	nError = IF4ERR_NOERROR;
    BYTE    nCount=0;
    char 	szId[32];
    ParamAddr param[2];
    ParamAddr *pParam;
    ParamAddr *tmp;
    ReturnVal *info = NULL;
    ReturnVal *batt = NULL;
    ReturnVal *prev;
    ENDI_BATTERYLOG_INFO   *battPointer;

    if(cnt < 2)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
    eui64toa(&id, szId);

    nCount = pValue[1].stream.u8; 

    memset(&param, 0, sizeof(ParamAddr) * 2);
    param[0].addr = (WORD) ENDI_ROMAP_BATTERY_POINTER;
    param[0].len = (WORD) sizeof(ENDI_BATTERYLOG_INFO);

    pParam = (ParamAddr *)&param[0];

	if((nError = m_pOndemander->RomRead((HANDLE)GetCoordinator(), &id, pParam, &info, FALSE, FALSE, TRUE)) == IF4ERR_NOERROR) 
    {
        battPointer = (ENDI_BATTERYLOG_INFO *)info->val.pVal;

        if(battPointer->BATTERY_POINTER >= nCount) {
            param[0].addr = ENDI_ROMAP_BATTERY_LOGDATA  + (((battPointer->BATTERY_POINTER - nCount + 1)+50)%50)*sizeof(ENDI_BATTERY_ENTRY);
            param[0].len = sizeof(ENDI_BATTERY_ENTRY) * nCount;
        }else {
            param[0].addr = ENDI_ROMAP_BATTERY_LOGDATA;
            param[0].len = sizeof(ENDI_BATTERY_ENTRY) * (battPointer->BATTERY_POINTER + 1);

            param[1].addr = ENDI_ROMAP_BATTERY_LOGDATA + ((50 - nCount + battPointer->BATTERY_POINTER + 1)%50) * sizeof(ENDI_BATTERY_ENTRY);
            param[1].len = sizeof(ENDI_BATTERY_ENTRY) * (nCount - battPointer->BATTERY_POINTER - 1);
            param[0].next = &param[1];
        }

	    if((nError = m_pOndemander->RomRead((HANDLE)GetCoordinator(), &id, pParam, &batt, TRUE, TRUE, FALSE)) == IF4ERR_NOERROR) 
        {
            prev = batt;
            tmp = pParam;
            while (tmp) {
                if(prev == NULL) {
                    XDEBUG("GetSensorBattery Fail: id=%s, NULL return value\r\n", szId);
                    nError = IF4ERR_CANNOT_GET;
                    break;
                }else {
                    if (prev->len != tmp->len)
                    {
                        XDEBUG("GetSensorBattery Fail: id=%s, reqlen=%d, reslen=%d\r\n", szId, tmp->len, prev->len);
                        nError = IF4ERR_CANNOT_GET;
                    }else {
	                    IF4API_AddResultOpaque(pWrapper, "1.12", prev->val.pVal, prev->len);
                    }
                }
                tmp = tmp->next;
                prev = prev->next;
            } 
        }
        else 
        {
            XDEBUG("GetSensorBattery Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
        } 

        if(batt && batt->val.pVal) FREE(batt->val.pVal);
        FREE(batt);
        if(info && info->val.pVal) FREE(info->val.pVal);
        FREE(info);
    }
    else 
    {
        XDEBUG("GetSensorBattery Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
    } 

    return nError;
}

int cmdGetSensorAmrData(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 id;
    int nError, len;
    char szId[32]={0,};
    ENDI_AMR_DATA amrdata;

    if(cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
    memset(&amrdata, 0, sizeof(ENDI_AMR_DATA));

	nError = m_pOndemander->AmrRead(GetCoordinator(), &id, (BYTE *)&amrdata, &len);
	if (nError != IF4ERR_NOERROR) {
        eui64toa(&id, szId);
        XDEBUG("GetSensoriAmrData Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
#if 0
    /*-- 전기 Sensor에서 AMR Data를 전송할 때 지정된 Size 보다 작게 전송하고 있다. 따라서 size 검사는 하지 않는다 --*/
    }else if(len != sizeof(ENDI_AMR_DATA)) {
        eui64toa(&id, szId);
        XDEBUG("GetSensoriAmrData Fail: id=%s, invalid data length=[%d:%d]\r\n", szId, 
                sizeof(ENDI_AMR_DATA), len);
        nError = IF4ERR_UNKNOWN_ERROR;
#endif
    }else {
		IF4API_AddResultOpaque(pWrapper, "1.12", &amrdata, len);
    }

    return nError;
}

int cmdSetSensorAmrData(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 id;
    WORD mask;
    int nError;
    char szId[32]={0,};
    ENDI_AMR_DATA amrdata;

    if(cnt != 3)
		return IF4ERR_INVALID_PARAM;

	if (IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

    if(VARAPI_OidCompare(&pValue[1].oid, "1.5") != 0) 
        return IF4ERR_INVALID_PARAM;
    if(VARAPI_OidCompare(&pValue[2].oid, "1.12") != 0) 
        return IF4ERR_INVALID_PARAM;
    if(pValue[2].len != sizeof(ENDI_AMR_DATA)) 
        return IF4ERR_UNKNOWN_ERROR;

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
    mask = pValue[1].stream.u16;
    memcpy(&amrdata, pValue[2].stream.p, pValue[2].len);

    /*-- Issue #1212 : AMR Frame의 Size 문제 때문에 Reset Field 까지만 전송한다 --*/
	nError = m_pOndemander->AmrWrite(GetCoordinator(), &id, mask, (BYTE *)&amrdata, 21);
	if (nError != IF4ERR_NOERROR) {
        eui64toa(&id, szId);
        XDEBUG("SetSensoriAmrData Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
    }

    return nError;
}

int cmdResetSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 id;
    WORD mask = 0;
    BYTE sec = 3;
    int nError;
    char szId[32]={0,};
    ENDI_AMR_DATA amrdata;

    if(cnt < 1)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

    if(cnt > 1) {
	    if (VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0)
		return IF4ERR_INVALID_ID;
        sec = pValue[1].stream.u8;
    }

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));

    memset(&amrdata, 0, sizeof(ENDI_AMR_DATA));
    mask  |= AMR_MASK_RESET;
    amrdata.reset = sec;

    /*-- Issue #1212 : AMR Frame의 Size 문제 때문에 Reset Field 까지만 전송한다 --*/
	nError = m_pOndemander->AmrWrite(GetCoordinator(), &id, mask, (BYTE *)&amrdata, 21);
	if (nError != IF4ERR_NOERROR) {
        eui64toa(&id, szId);
        XDEBUG("ResetSensor Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
    }

    return nError;
}

int cmdUpdateSensorFirmware(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	return IF4ERR_DEPRECATED_FUNCTION;
}

int cmdScanSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 id;
	OTANODE *pList;
    char parser[256];
    char * idxPtr = NULL;
    int nError=IF4ERR_NOERROR, i, nCount=0;
    BOOL bAll = FALSE;
    WORD firmwareVersion = 0x00;
    WORD build = 0x00;

    if(cnt < 1)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid)) {
        if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0 ) {
		    return IF4ERR_INVALID_ID;
        } else  {
            bAll = TRUE;
            memset(parser, 0, sizeof(parser));
            memcpy(parser, pValue[0].stream.p, pValue[0].len);

            if((idxPtr=strchr(parser,'$'))) {
                *idxPtr = 0x00;
            }
        }
    }

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

    if(!bAll) {
	    memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	    m_pInventoryScanner->Add(&id);
    }else {
        if(cnt > 1) {
            firmwareVersion = pValue[1].stream.u16;
        }
        if(cnt > 2) {
            build = pValue[2].stream.u16;
        }
	    pList = m_pEndDeviceList->GetOtaListByModel(parser, &nCount, 0, firmwareVersion, build);
        for(i=0;i<nCount;i++) {
	        m_pInventoryScanner->Add(&pList[i].id);
        }
        FREE(pList);
    }

    return nError;
}

int cmdCommandSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 id;
    BYTE buffer[256],result[256];
    char parser[256];
    char szId[32]={0,};
    char * idxPtr = NULL;
    int cmd=-1;
    int nError=IF4ERR_NOERROR, bufferLen = 0, resultLen = 0;
    BOOL bAll = FALSE;
    //BOOL bParser = FALSE;
    //WORD firmwareVersion = 0x00;
    //WORD build = 0x00;
    //BOOL force = FALSE;
    ENDI_COMMAND_PAYLOAD  *payload;

    if(cnt < 2)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid)) {
        if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0 ) {
		    return IF4ERR_INVALID_ID;
        } else  {
            bAll = TRUE;
            if(strncasecmp(pValue[0].stream.p,"all",3)) { 
                //bParser = TRUE;
                memset(parser, 0, sizeof(parser));
                memcpy(parser, pValue[0].stream.p, pValue[0].len);

                if((idxPtr=strchr(parser,'$'))) {
                    *idxPtr = 0x00;
                }
            }
        }
    }

	if (VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0)
		return IF4ERR_INVALID_PARAM;

#if 0
    if(cnt > 2) {
        firmwareVersion = pValue[2].stream.u16;
    }
    if(cnt > 3) {
        build = pValue[3].stream.u16;
    }
    if(cnt > 4) {
        force = pValue[4].stream.u8 ? TRUE : FALSE;
    }
#endif
    if(cnt > 5) {
        if((unsigned int)pValue[5].len > sizeof(buffer) - 1) 
            return IF4ERR_INVALID_PARAM;
    }

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

    cmd = pValue[1].stream.u8;

    if(!bAll) {
        memset(buffer, 0, sizeof(buffer));
	    memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
        if(cnt > 5) {
            bufferLen = pValue[5].len;
            memcpy(buffer, pValue[5].stream.p,  bufferLen);
        }

	    nError = m_pOndemander->EndiCommandWrite(GetCoordinator(), &id, cmd, buffer, bufferLen, result, &resultLen);
	    if (nError != IF4ERR_NOERROR) {
            eui64toa(&id, szId);
            XDEBUG("Sensor Command Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
        }else {
		    IF4API_AddResultOpaque(pWrapper, "1.12", &result, resultLen);
        }
    }
	else
	{
        switch(cmd) {
            case 0 : // Verify
            case 1 : // Install
                 // Unicast로만 동작되어야 한다.
                break;
            default:
                payload = (ENDI_COMMAND_PAYLOAD *) &buffer;
                payload->cmd_type = (BYTE) cmd;
                payload->frame_type = 0x00;
                if(cnt > 5) {
                    bufferLen = pValue[5].len;
                    memcpy(payload->data, pValue[5].stream.p,  bufferLen);
                }

	            nError = codiMulticast(GetCoordinator(), ENDI_DATATYPE_COMMAND, (BYTE *)&buffer, 
                            sizeof(payload->cmd_type) + sizeof(payload->frame_type) + bufferLen);

	            if (nError != CODIERR_NOERROR)
	            {
		            XDEBUG("---------------------------------------------\r\n");
		            XDEBUG("COMMAND-SENSORALL-ERROR: %s(%d)\r\n", codiErrorMessage(nError), nError);
		            XDEBUG("---------------------------------------------\r\n");
	            }
                break;
        }
        memset(result, 0, sizeof(result));
		IF4API_AddResultOpaque(pWrapper, "1.12", &result, 0);
    }

    return nError;
}

int cmdExecuteCommand(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 id;
    BYTE buffer[512],result[512];
    char szId[32]={0,};
    int cmd=-1;
    BYTE rw;
    int nError=IF4ERR_NOERROR, bufferLen = 0, resultLen = 0;
    BOOL bAll = FALSE;
    ENDI_COMMAND_PAYLOAD  *payload;

    if(cnt < 3)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid)) {
        if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0 ) {
		    return IF4ERR_INVALID_ID;
        } else  
        {
            if(!strncasecmp(pValue[0].stream.p,"all",3)) 
            { 
                bAll = TRUE;
            }
            else
            {
		        return IF4ERR_INVALID_PARAM;
            }
        }
    }

	if (VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0)      /** Command */
		return IF4ERR_INVALID_PARAM;
	if (VARAPI_OidCompare(&pValue[2].oid, "1.4") != 0)      /** Read/Write */
		return IF4ERR_INVALID_PARAM;

    if(cnt > 3) {
        if((unsigned int)pValue[3].len > sizeof(buffer) - 1) 
            return IF4ERR_INVALID_PARAM;
    }

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

    cmd = pValue[1].stream.u8;
    rw = pValue[2].stream.u8;       // write 0, read 1

    memset(buffer, 0, sizeof(buffer));
	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));

    if(bAll)
    {
        if(rw)  /** Multicast Read는 허용하지 않는다 */
        {
		    return IF4ERR_INVALID_PARAM;
        }

        payload = (ENDI_COMMAND_PAYLOAD *) &buffer;
        payload->cmd_type = (BYTE) cmd;
        payload->frame_type = 0x00;
        if(cnt > 3) {
            bufferLen = pValue[3].len;
            memcpy(payload->data, pValue[3].stream.p,  bufferLen);
        }

	    nError = codiMulticast(GetCoordinator(), ENDI_DATATYPE_COMMAND, (BYTE *)&buffer, 
            sizeof(payload->cmd_type) + sizeof(payload->frame_type) + bufferLen);

	    if (nError != CODIERR_NOERROR)
	    {
		    XDEBUG("---------------------------------------------\r\n");
		    XDEBUG("COMMAND-SENSORALL-ERROR: %s(%d)\r\n", codiErrorMessage(nError), nError);
		    XDEBUG("---------------------------------------------\r\n");
	    }
        else
        {
	        IF4API_AddResultOpaque(pWrapper, "1.12", &result, resultLen);
        }
    }
    else
    {
        if(cnt > 3) {
            bufferLen = pValue[3].len;
            memcpy(buffer, pValue[3].stream.p,  bufferLen);
        }

        if(rw)
	        nError = m_pOndemander->EndiCommandRead(GetCoordinator(), &id, cmd, buffer, bufferLen, result, &resultLen);
        else
	        nError = m_pOndemander->EndiCommandWrite(GetCoordinator(), &id, cmd, buffer, bufferLen, result, &resultLen);

	    if (nError != IF4ERR_NOERROR) {
            eui64toa(&id, szId);
            XDEBUG("Sensor Command Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
        }else {
	        IF4API_AddResultOpaque(pWrapper, "1.12", &result, resultLen);
        }
    }

    return nError;
}

//////////////////////////////////////////////////////////////////////
// Log Command
//////////////////////////////////////////////////////////////////////

int GetLogList(IF4Wrapper *pWrapper, TIMESTAMP *pFrom, TIMESTAMP *pTo,
				const char *pszDirectory, const char *pszHeader, const char *pszOid)
{
	COMMLOGENTRY	log;
	struct 		stat file_info;
    DIR     	*dir_fdesc;
    dirent  	*dp;
    char    	szFileName[128];
    char    	year[10], mon[10], day[10], hour[10];
	TIMESTAMP	tmNow;
	int			nCount = 0;

	dir_fdesc = opendir(pszDirectory);
	if (dir_fdesc == NULL)
		return IF4ERR_SYSTEM_ERROR;

	for(; (dp=readdir(dir_fdesc));)
	{
		if (strncmp(dp->d_name, pszHeader, 3) != 0)
			continue;

		sprintf(szFileName, "%s/%s", LOG_DIR, dp->d_name);
		memcpy(year, &dp->d_name[3], 4); year[4] = '\0';
		memcpy(mon, &dp->d_name[7], 2); mon[2] = '\0';
		memcpy(day, &dp->d_name[9], 2); day[2] = '\0';
		memcpy(hour, &dp->d_name[12], 2); hour[2] = '\0';

		memset(&tmNow, 0, sizeof(TIMESTAMP));
		tmNow.year	= atoi(year);
		tmNow.mon	= atoi(mon);
		tmNow.day	= atoi(day);
		tmNow.hour	= atoi(hour);

		if ((memcmp(&tmNow, pFrom, sizeof(TIMESTAMP)) < 0) ||
			(memcmp(&tmNow, pTo, sizeof(TIMESTAMP)) > 0))
			continue;

		memset(&log, 0, sizeof(COMMLOGENTRY));
		memcpy(&log.commLogDate, &tmNow, sizeof(TIMESTAMP));
		strcpy(log.commLogFileName, szFileName);

		stat(szFileName, &file_info);
		log.commLogFileSize = file_info.st_size;

		IF4API_AddResultFormat(pWrapper, pszOid, VARSMI_OPAQUE, &log, sizeof(COMMLOGENTRY));
		nCount++;
    }
    closedir(dir_fdesc);

	return nCount ? IF4ERR_NOERROR : IF4ERR_NO_ENTRY;
}

int GetAllLogList(IF4Wrapper *pWrapper, TIMESTAMP *pFrom, TIMESTAMP *pTo)
{
	COMMLOGENTRY	log;
	struct 		stat file_info;
    DIR     	*dir_fdesc;
    dirent  	*dp;
    char    	*p, szFileName[128];
    char    	year[10], mon[10], day[10], hour[10];
	TIMESTAMP	tmNow;
	int			nCount = 0, nType;

	dir_fdesc = opendir(LOG_DIR);
	if (dir_fdesc == NULL)
		return IF4ERR_SYSTEM_ERROR;

	for(; (dp=readdir(dir_fdesc));)
	{
		nType = -1;
        /** LOG 관련 TODO : */
		if (strncmp(dp->d_name, EVENT_LOG_FILE, 3) == 0) nType = 0;      //103.4, 2.204
		if (strncmp(dp->d_name, CMD_HISTORY_LOG_FILE, 3) == 0) nType = 1;      //103.2, 2.202
		if (strncmp(dp->d_name, METERING_LOG_FILE, 3) == 0) nType = 2;      //103.3, 2.206
		if (strncmp(dp->d_name, COMM_LOG_FILE, 3) == 0) nType = 3;      //103.5, 2.200
		if (strncmp(dp->d_name, MOBILE_LOG_FILE, 3) == 0) nType = 4;      //103.6, 2.208

		if (nType == -1)
			continue;

		p = strrchr(dp->d_name, '.');
		if ((p == NULL) || (strcmp(p, ".log") != 0))
			continue;

		sprintf(szFileName, "%s/%s", LOG_DIR, dp->d_name);
		memcpy(year, &dp->d_name[3], 4); year[4] = '\0';
		memcpy(mon, &dp->d_name[7], 2); mon[2] = '\0';
		memcpy(day, &dp->d_name[9], 2); day[2] = '\0';
		memcpy(hour, &dp->d_name[12], 2); hour[2] = '\0';

		memset(&tmNow, 0, sizeof(TIMESTAMP));
		tmNow.year	= atoi(year);
		tmNow.mon	= atoi(mon);
		tmNow.day	= atoi(day);
		tmNow.hour	= atoi(hour);

		if ((memcmp(&tmNow, pFrom, sizeof(TIMESTAMP)) < 0) ||
			(memcmp(&tmNow, pTo, sizeof(TIMESTAMP)) > 0))
			continue;

		memset(&log, 0, sizeof(COMMLOGENTRY));
		memcpy(&log.commLogDate, &tmNow, sizeof(TIMESTAMP));
		strcpy(log.commLogFileName, szFileName);

		stat(szFileName, &file_info);
		log.commLogFileSize = file_info.st_size;

		switch(nType) {
		  case 0 : IF4API_AddResultFormat(pWrapper, "2.204", VARSMI_OPAQUE, &log, sizeof(MCUEVENTENTRY)); break;	//event
		  case 1 : IF4API_AddResultFormat(pWrapper, "2.202", VARSMI_OPAQUE, &log, sizeof(CMDHISTENTRY)); break;	//cmd hist
		  case 2 : IF4API_AddResultFormat(pWrapper, "2.206", VARSMI_OPAQUE, &log, sizeof(METERLOGENTRY)); break;	//mtr
		  case 3 : IF4API_AddResultFormat(pWrapper, "2.200", VARSMI_OPAQUE, &log, sizeof(COMMLOGENTRY)); break;	//commlog
		  case 4 : IF4API_AddResultFormat(pWrapper, "2.208", VARSMI_OPAQUE, &log, sizeof(MOBILELOGENTRY)); break;	//mobilelog
	    }
		nCount++;
    }
    closedir(dir_fdesc);

	return nCount ? IF4ERR_NOERROR : IF4ERR_NO_ENTRY;
}

int cmdClearLog(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	SystemExec("rm -f " LOG_DIR);
	mkdir(LOG_DIR, 0777);
	return IF4ERR_NOERROR;
}

int cmdGetHistList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	TIMESTAMP	*pFrom, *pTo;

	if (cnt != 2)
		return IF4ERR_INVALID_PARAM;

	if (pValue[0].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	if (pValue[1].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	pFrom = (TIMESTAMP *)pValue[0].stream.p;
	pTo   = (TIMESTAMP *)pValue[1].stream.p;

	return GetLogList(pWrapper, pFrom, pTo, LOG_DIR, CMD_HISTORY_LOG_FILE, "2.202");
}
//cmdCmdMeterLogList
int cmdGetMeterLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	TIMESTAMP	*pFrom, *pTo;

	if (cnt != 2)
		return IF4ERR_INVALID_PARAM;

	if (pValue[0].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	if (pValue[1].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	pFrom = (TIMESTAMP *)pValue[0].stream.p;
	pTo   = (TIMESTAMP *)pValue[1].stream.p;

	return GetLogList(pWrapper, pFrom, pTo, LOG_DIR, METERING_LOG_FILE, "2.206");
}

int cmdGetEventLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	TIMESTAMP	*pFrom, *pTo;

	if (cnt != 2)
		return IF4ERR_INVALID_PARAM;

	if (pValue[0].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	if (pValue[1].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	pFrom = (TIMESTAMP *)pValue[0].stream.p;
	pTo   = (TIMESTAMP *)pValue[1].stream.p;

	return GetLogList(pWrapper, pFrom, pTo, LOG_DIR, EVENT_LOG_FILE, "2.204");
}

int cmdGetCommLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	TIMESTAMP	*pFrom, *pTo;

	if (cnt != 2)
		return IF4ERR_INVALID_PARAM;

	if (pValue[0].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	if (pValue[1].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	pFrom = (TIMESTAMP *)pValue[0].stream.p;
	pTo   = (TIMESTAMP *)pValue[1].stream.p;

	return GetLogList(pWrapper, pFrom, pTo, LOG_DIR, COMM_LOG_FILE, "2.200");
}

int cmdGetMobileLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	TIMESTAMP	*pFrom, *pTo;

	if (cnt != 2)
		return IF4ERR_INVALID_PARAM;

	if (pValue[0].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	if (pValue[1].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	pFrom = (TIMESTAMP *)pValue[0].stream.p;
	pTo   = (TIMESTAMP *)pValue[1].stream.p;

	return GetLogList(pWrapper, pFrom, pTo, LOG_DIR, MOBILE_LOG_FILE, "2.208");
}

int cmdGetAllLogList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	TIMESTAMP	*pFrom, *pTo;

	if (cnt != 2)
		return IF4ERR_INVALID_PARAM;

	if (pValue[0].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	if (pValue[1].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	pFrom = (TIMESTAMP *)pValue[0].stream.p;
	pTo   = (TIMESTAMP *)pValue[1].stream.p;

	return GetAllLogList(pWrapper, pFrom, pTo);
}

//////////////////////////////////////////////////////////////////////
// System User Command
//////////////////////////////////////////////////////////////////////

int cmdAddUser(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	if (cnt != 3)
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[0].oid, "2.10.2") != 0)
		return IF4ERR_INVALID_ACCOUNT;

	if (pValue[0].len < 5)
		return IF4ERR_INVALID_ACCOUNT;

	if (VARAPI_OidCompare(&pValue[1].oid, "2.10.3") != 0)
		return IF4ERR_INVALID_PASSWORD;

	if (pValue[1].len < 5)
		return IF4ERR_INVALID_PASSWORD;

	if (VARAPI_OidCompare(&pValue[2].oid, "2.10.4") != 0)
		return IF4ERR_INVALID_GROUP;	

	if ((pValue[2].stream.u8 != USER_GROUP_ADMIN) &&
		(pValue[2].stream.u8 != USER_GROUP_USER) &&
		(pValue[2].stream.u8 != USER_GROUP_GUEST))
		return IF4ERR_INVALID_GROUP;	
	
	if (!m_pUserManager->AddUser(pValue[0].stream.p, pValue[1].stream.p, pValue[2].stream.u8))
		return IF4ERR_INVALID_ACCOUNT;

	return IF4ERR_NOERROR;
}

int cmdDeleteUser(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[0].oid, "2.10.2") != 0)
		return IF4ERR_INVALID_ACCOUNT;

	if (!m_pUserManager->DeleteUser(pValue[0].stream.p))
		return IF4ERR_CANNOT_DELETE;

	return IF4ERR_NOERROR;
}

int cmdGetUser(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	USRENTRY	user;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[0].oid, "2.10.2") != 0)
		return IF4ERR_INVALID_ACCOUNT;

	memset(&user, 0, sizeof(USRENTRY));
	if (!m_pUserManager->GetUser((char *)pValue[0].stream.p, (char *)user.usrPassword, &user.usrGroup))
    {
		return IF4ERR_INVALID_ACCOUNT;
    }

	strcpy(user.usrAccount, pValue[0].stream.p);
	IF4API_AddResultFormat(pWrapper, "2.10", VARSMI_OPAQUE, &user, sizeof(USRENTRY));
	return IF4ERR_NOERROR;
}

int cmdSetUser(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	if (cnt != 3)
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[0].oid, "2.10.2") != 0)
		return IF4ERR_INVALID_ACCOUNT;

	if (VARAPI_OidCompare(&pValue[1].oid, "2.10.3") != 0)
		return IF4ERR_INVALID_PASSWORD;

	if (VARAPI_OidCompare(&pValue[2].oid, "2.10.4") != 0)
		return IF4ERR_INVALID_GROUP;	

	if (!m_pUserManager->ChangePassword(pValue[0].stream.p, pValue[1].stream.p))
		return IF4ERR_INVALID_ACCOUNT;

	if (!m_pUserManager->ChangeGroup(pValue[0].stream.p, pValue[2].stream.u8))
		return IF4ERR_INVALID_GROUP;

	return IF4ERR_NOERROR;
}

void UserCallback(USERINFO *pUser, void *pParam)
{
	IF4Wrapper *pWrapper = (IF4Wrapper *)pParam;
	USRENTRY	user;

	memset(&user, 0, sizeof(USRENTRY));
	strcpy(user.usrAccount, pUser->szUser);
	strcpy(user.usrPassword, pUser->szPassword);
	user.usrGroup = pUser->nGroup;

	IF4API_AddResultFormat(pWrapper, "2.10", VARSMI_OPAQUE, &user, sizeof(USRENTRY));
}

int cmdUserList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	m_pUserManager->EnumUser(UserCallback, (void *)pWrapper);
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Phone List Command
//////////////////////////////////////////////////////////////////////

int cmdGetPhoneList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	FILE	*fp;
	char	szBuffer[64];
	int		nCount = 0;

	if (cnt > 0)
		return IF4ERR_INVALID_PARAM;

	fp = fopen("/app/sw/phonelist", "rb");
	if (fp != NULL)
	{
		while(fgets(szBuffer, 63, fp))
		{
			szBuffer[strlen(szBuffer)-1] = '\0';
			if (strlen(szBuffer) == 0)
				continue;
			IF4API_AddResultString(pWrapper, "1.11", (char *)szBuffer);
			nCount++;
		}
		fclose(fp);
	}
	return (nCount > 0) ? IF4ERR_NOERROR : IF4ERR_NO_ENTRY;
}

int cmdSetPhoneList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	int		i;
	FILE	*fp;
	char	szBuffer[64];

	if (cnt > 10)
		return IF4ERR_INVALID_COUNT;
	
	for(i=0; i<cnt; i++)
		if ((pValue[i].type != VARSMI_STRING) && (pValue[i].type != VARSMI_STREAM))
			return IF4ERR_INVALID_PARAM;

	fp = fopen("/app/sw/phonelist", "wb");
	for(i=0; i<cnt; i++)
	{
		sprintf(szBuffer, "%s\n", pValue[i].stream.p);
		fwrite(szBuffer, 1, strlen(szBuffer), fp);
	}
	fclose(fp);
	return IF4ERR_NOERROR;
}

int cmdGetSCID(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	*pszBuffer;
	char	szSCID[32] = "";
	FILE	*fp;

	fp = fopen("/app/conf/mobileinfo", "rt");
	if (fp != NULL)
	{
		pszBuffer = (char *)MALLOC(128);
		while(fgets(pszBuffer, 127, fp))
		{	
			pszBuffer[strlen(pszBuffer)-1] = '\0';
			if (strncmp(pszBuffer, "SIM ", 4) == 0)
			{
				strcpy(szSCID, &pszBuffer[4]);
				break;
			}
		}
		FREE(pszBuffer);
		fclose(fp);
	}
 
	IF4API_AddResultString(pWrapper, "1.11", szSCID);
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// APN Command
//////////////////////////////////////////////////////////////////////

BOOL IsOptionEnabled(const char *szOption, MIBValue *pValue, int cnt, int *nValueCnt) 
{
	int		i, len;

	len = strlen(szOption);
	for(i=4; i<cnt; i++)
	{
		if (pValue[i].stream.p[0] == '-')
		{
			if (strncmp(&pValue[i].stream.p[1], szOption, len) == 0)
			{
				/* '-' 옵션을 줄 경우 뒤에 붙는 파라미터가 있는지 판단,
				 * 마지막일 경우 무시한다.
				 */
				if (nValueCnt != NULL && i+1 < cnt)
				{
					/* '-' 옵션 뒤에 다시 '-' 옵션이 붙을 경우 무시한다. */
					if (pValue[i+1].stream.p[0] != '-')	
						*nValueCnt = i+1;
				}
				return FALSE;
			}
		}
	}
	return TRUE;
}

int cmdAddApn(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	szName[65];
	char	szUser[65];
	char	szPassword[65];
	char	szCommand[65];
	char	szDial[65];
	char	szOption[65];
	char	szFileName[256];
	char	szMessage[256];
	FILE	*fp;
	int		i;

	if (cnt < 5)
		return IF4ERR_NEED_MORE_VALUE;

	memset(szName, 0, 65);
	memset(szUser, 0, 65);
	memset(szPassword, 0, 65);
	memset(szCommand, 0, 65);
	memset(szDial, 0, 65);

	for(i=0; i<cnt; i++)
	{
		if ((pValue[i].type != VARSMI_STRING) && (pValue[i].type != VARSMI_STREAM))
			return IF4ERR_INVALID_PARAM;

		if (pValue[i].len > 64)
			return IF4ERR_INVALID_LENGTH;

		switch(i) {
		  case 0 : strncpy(szName, pValue[i].stream.p, MIN(pValue[i].len, 64)); break;			
		  case 1 : strncpy(szUser, pValue[i].stream.p, MIN(pValue[i].len, 64)); break;			
		  case 2 : strncpy(szPassword, pValue[i].stream.p, MIN(pValue[i].len, 64)); break;			
		  case 3 : strncpy(szCommand, pValue[i].stream.p, MIN(pValue[i].len, 64)); break;
		  case 4 : strncpy(szDial, pValue[i].stream.p, MIN(pValue[i].len, 64)); break;
		}			
	}

	sprintf(szFileName, "/app/sw/ppp/peers/%s", szName);
	if (IsExists(szFileName))
		return IF4ERR_DUPLICATE;

	// peers에 해당 APN 파일을 생성한다.
	fp = fopen(szFileName, "w+b");
	if (fp != NULL)
	{
		sprintf(szMessage, "# File: /etc/ppp/peers/%s\n#\n\n", szName);
		fwrite(szMessage, strlen(szMessage), 1, fp);

		if (IsOptionEnabled("user", pValue, cnt))
		{
			sprintf(szMessage, "user %s	# username (no data acceleration)\n", szUser);
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}

		if (IsOptionEnabled("password", pValue, cnt))
		{
			sprintf(szMessage, "password %s	# a common GPRS/EDGE password\n\n", szPassword);
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}

		sprintf(szMessage, "\n");
		fwrite(szMessage, strlen(szMessage), 1, fp);

        if(strlen(m_sMobileConfig.interface) > 0)
        {
		    sprintf(szMessage, "%s		# modem\n", m_sMobileConfig.interface);
        }
        else
        {
#if !defined(__TI_AM335X__)
		    sprintf(szMessage, "/dev/ttyS0		# modem\n");
#else
		    sprintf(szMessage, "/dev/ttyO1		# modem\n");
#endif
        }

		fwrite(szMessage, strlen(szMessage), 1, fp);

		sprintf(szMessage, "%d			# speed\n", m_sMobileConfig.interfaceSpeed);
		fwrite(szMessage, strlen(szMessage), 1, fp);
        SetFileValue("/app/conf/mobilespeed", m_sMobileConfig.interfaceSpeed);

		if (IsOptionEnabled("crtscts", pValue, cnt))
		{
			sprintf(szMessage, "crtscts			# hardware flow control\n");
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}

		if (IsOptionEnabled("defaultroute", pValue, cnt))
		{	
			sprintf(szMessage, "defaultroute		# use the cellular network for the default route\n");
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}

		if (IsOptionEnabled("noipdefault", pValue, cnt))
		{
			sprintf(szMessage, "noipdefault		# remote server will allocate us an IP address\n");
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}

		if (IsOptionEnabled("usepeerdns", pValue, cnt))
		{
			sprintf(szMessage, "usepeerdns		# ask the peer for up to 2 DNS server addresses\n");
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}

		if (IsOptionEnabled("lock", pValue, cnt))
		{
			sprintf(szMessage, "lock			# lock the serial port\n");
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}

#if !defined(__TI_AM335X__)
		if (IsOptionEnabled("modem", pValue, cnt))
		{
			sprintf(szMessage, "modem			# use modem signal\n");
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}
        else
        {
		    sprintf(szMessage, "local			# don't use Carrier Detect or Data Terminal Ready\n\n");
		    fwrite(szMessage, strlen(szMessage), 1, fp);
        }
#else
        // TI_AM335X에서는 modem control signal을 사용하지 않는다
		if (IsOptionEnabled("local", pValue, cnt))
		{
		    sprintf(szMessage, "local			# don't use Carrier Detect or Data Terminal Ready\n\n");
		    fwrite(szMessage, strlen(szMessage), 1, fp);
		}
        else
        {
			sprintf(szMessage, "modem			# use modem signal\n");
			fwrite(szMessage, strlen(szMessage), 1, fp);
        }
#endif

		if (IsOptionEnabled("debug", pValue, cnt))
		{
			sprintf(szMessage, "debug			# provides verbose output to stderr\n");
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}

		if (IsOptionEnabled("noauth", pValue, cnt))
		{
			sprintf(szMessage, "noauth			# remote machine should not authenticate itself\n");
			fwrite(szMessage, strlen(szMessage), 1, fp);
		}

		for(i=5; i<cnt; i++)
		{
			memset(szOption, 0, 65);
		  	strncpy(szOption, pValue[i].stream.p, MIN(pValue[i].len, 64));
			if (szOption[0] != '-')
			{
				sprintf(szMessage, "%s\n", szOption);
				fwrite(szMessage, strlen(szMessage), 1, fp);
			}
		}

		sprintf(szMessage, "\n");
		fwrite(szMessage, strlen(szMessage), 1, fp);

		sprintf(szMessage, "connect			\"/etc/ppp/chat -v -f /etc/ppp/chatscript/%s-connect\"\n", szName);
		fwrite(szMessage, strlen(szMessage), 1, fp);
		sprintf(szMessage, "disconnect		\"/etc/ppp/chat -v -f /etc/ppp/chatscript/%s-disconnect\"\n", szName);
		fwrite(szMessage, strlen(szMessage), 1, fp);
		fclose(fp);

		// 파일의 권한을 변경한다.
		sprintf(szMessage, "chown root:root %s\n", szFileName);
		SystemExec(szMessage);
		sprintf(szMessage, "chmod 777 %s\n", szFileName);
		SystemExec(szMessage);
	}

	// Chat script를 만든다.
	sprintf(szFileName, "/app/sw/ppp/chatscript/%s-connect", szName);
	fp = fopen(szFileName, "w+b");
	if (fp != NULL)
	{
		sprintf(szMessage, "'ABORT'     'BUSY'\n");
		fwrite(szMessage, strlen(szMessage), 1, fp);
		sprintf(szMessage, "'ABORT'     'ERROR'\n");
		fwrite(szMessage, strlen(szMessage), 1, fp);
		sprintf(szMessage, "'ABORT'     'NO CARRIER'\n");
		fwrite(szMessage, strlen(szMessage), 1, fp);
		sprintf(szMessage, "'ABORT'     'NO DIALTONE'\n");
		fwrite(szMessage, strlen(szMessage), 1, fp);
		sprintf(szMessage, "'ABORT'     'Invalid Login'\n");
		fwrite(szMessage, strlen(szMessage), 1, fp);
		sprintf(szMessage, "''          '%s'\n", szCommand);
		fwrite(szMessage, strlen(szMessage), 1, fp);
		sprintf(szMessage, "'OK'        '%s'\n", szDial);
		fwrite(szMessage, strlen(szMessage), 1, fp);
		sprintf(szMessage, "'CONNECT'   ''\n");
		fwrite(szMessage, strlen(szMessage), 1, fp);
		fclose(fp);

		// 파일의 속성을 변경한다.
		sprintf(szMessage, "chown root:root %s\n", szFileName);
		SystemExec(szMessage);
		sprintf(szMessage, "chmod 777 %s\n", szFileName);
		SystemExec(szMessage);
	}

	return IF4ERR_NOERROR;
}

int cmdDeleteApn(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	szFileName[256];
	char	szName[128];

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if ((pValue[0].type != VARSMI_STRING) && (pValue[0].type != VARSMI_STREAM))
		return IF4ERR_INVALID_PARAM;
	
	memset(szName, 0, 127);
	strncpy(szName, pValue[0].stream.p, MIN(pValue[0].len, 127));
	
	XDEBUG("APN: Delete '%s'\r\n", szName);
	sprintf(szFileName, "/app/sw/ppp/peers/%s", szName);
	if (!IsExists(szFileName))
		return IF4ERR_INVALID_APN;

	unlink(szFileName);
	sprintf(szFileName, "/app/sw/ppp/chatscript/%s-connect", szName);
	unlink(szFileName);
	sprintf(szFileName, "/app/sw/ppp/chatscript/%s-disconnect", szName);
	unlink(szFileName);

	return IF4ERR_NOERROR;
}

int cmdGetApnList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    DIR			*dir_fdesc;
    dirent		*dp;

    dir_fdesc = opendir("/app/sw/ppp/peers");
    if (!dir_fdesc)
        return IF4ERR_NO_ENTRY;

    for(;(dp=readdir(dir_fdesc));)
    {
        if (dp->d_name[0] == '.')
			continue;

		IF4API_AddResultString(pWrapper, "1.11", dp->d_name);				// stringEntry
	}
	closedir(dir_fdesc);
	return IF4ERR_NOERROR;
}

#define MAX_APN_FILESIZE	10000

int cmdGetApnOption(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	szFileName[256];
	char	szName[128];
	char	*pszBuffer;
	FILE	*fp;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if ((pValue[0].type != VARSMI_STRING) && (pValue[0].type != VARSMI_STREAM))
		return IF4ERR_INVALID_PARAM;
	
	memset(szName, 0, 127);
	strncpy(szName, pValue[0].stream.p, MIN(pValue[0].len, 127));
	
	sprintf(szFileName, "/app/sw/ppp/peers/%s", szName);
	if (!IsExists(szFileName))
		return IF4ERR_INVALID_APN;

	pszBuffer = (char *)MALLOC(MAX_APN_FILESIZE+1);
	if (pszBuffer == NULL)
		return IF4ERR_MEMORY_ERROR;

	memset(pszBuffer, 0, MAX_APN_FILESIZE+1);
	fp = fopen(szFileName, "rb");
	if (fp != NULL)
	{
		fread(pszBuffer, 1, MAX_APN_FILESIZE, fp);
		fclose(fp);
		IF4API_AddResultString(pWrapper, "1.11", pszBuffer);				// stringEntry
	}
	FREE(pszBuffer);

	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Firmware Command
//////////////////////////////////////////////////////////////////////

int cmdFirmwareUpdate(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	szAddress[65] = "";
	char	szPath[_MAX_PATH+1] = "";
	BYTE	nType = 0;

	if (cnt < 2)
		return IF4ERR_INVALID_PARAM;

    printf("\r\nWARNING: Firmware Install.\r\n");

	memcpy(szAddress, pValue[0].stream.p, pValue[0].len);
	memcpy(szPath, pValue[1].stream.p, pValue[1].len);

	szAddress[pValue[0].len] = '\0';
	szPath[pValue[1].len] = '\0';

	nType = (cnt > 2) ? pValue[2].stream.u8 : 0;
	
    XDEBUG("    Address = %s\r\n", szAddress);
    XDEBUG("  File name = %s\r\n", szPath);
    XDEBUG("       Type = %d\r\n", nType);
    XDEBUG("\r\n");

    /** Upgrade Type
     *    0: DM Agent를 이용한 Upgrade
     *    1: 표준 Upgrade(Full Package)
     *    2: Coordinator Upgrade(Old Style : 현재 cmdUpdateCoordinator(101.24) 사용)
     *    3: BSDiff Style Upgrade(diff) -> MCU
     *    4: BSDiff Style Upgrade(diff) -> Coordinator
     *
     *     0는 더 이상 지원하지 않고 3,4는 아직 지원하지 않는다.
    */
    switch(nType) {
        case 0 : // FEP Request
            /** 원래 DM Agent를 위해 존재하던 코드. 더이상 DM Agent를 지원하지 않기 때문에 
              * 이 기능은 IF4ERR_DO_NOT_SUPPORT 를 리턴한다.
              */
            return IF4ERR_DO_NOT_SUPPORT;
        case 1 : // 표준적인 Upgrade (Upgrade Directory 이용)
            UpgradeConcentratorSystem(pWrapper, CONCENTRATOR_UPGRADE_GZIP, szAddress, FIRMWARE_COMPRESS_FILE);
            break;

        case 2 : // Coordinator Upgrade
            /** 현재는 cmdUpdateCoordinator(101.24) 를 이용하고 있다
              */
		    codiDownload(GetCoordinator(), CODI_MODEMTYPE_XMODEM, szPath, ".ebl");
            break;
        case 3 : // BSDiff Style Upgrade(diff) -> MCU
            UpgradeConcentratorSystem(pWrapper, CONCENTRATOR_UPGRADE_DIFF, szAddress, szPath);
            break;
        default:
            return IF4ERR_DO_NOT_SUPPORT;
    }

    XDEBUG("\r\nEnd of Firmware update event\r\n");
	return IF4ERR_NOERROR;
}

BOOL CheckSensor(ENDIENTRY *pEndi, void * param1, void * param2, void * param3)
{
/** NAZC : 현재 nType이 없기 때문에 모두 Unknown으로 리턴
 */
	const char	*pszName = "Unknown";
	char	szGUID[20];

	eui64toa(&pEndi->id, szGUID);
	XDEBUG("Wakeup %s %s.\xd\xa", pszName, szGUID);

	return TRUE;
}

int cmdCheck(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    m_pEndDeviceList->EnumEndDevice(CheckSensor, NULL, NULL, NULL);
	return IF4ERR_NOERROR;
}

int cmdLiveCheck(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	IF4API_AddResultNumber(pWrapper, "2.1.24", VARSMI_BYTE, m_pService->GetState());
	return IF4ERR_NOERROR;
}

int cmdShellCommand(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	CChunk	chunk(1024);
	char	szCommand[128];
	
	if (pValue->len >= 127)
		return IF4ERR_INVALID_PARAM;

	memset(szCommand, 0, 128);
	strncpy(szCommand, pValue->stream.p, 127);
	if (GetShellCommandResult(&chunk, szCommand))
		return IF4ERR_INVALID_COMMAND;

	IF4API_AddResultString(pWrapper, "1.11", chunk.GetBuffer());
	return IF4ERR_NOERROR;
}

int cmdSensorTimesync(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    BYTE    nTimeSyncType = TIMESYNC_TYPE_LONG;

    if (cnt > 0) nTimeSyncType = pValue->stream.u8;

	m_pSensorTimesync->Timesync(nTimeSyncType);
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// File Transfer Command
//////////////////////////////////////////////////////////////////////

int cmdGetFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char		*pszFileName;
	struct 		stat file_info;
	TIMESTAMP	tmFile;
	char		*pszBuffer;
	struct		tm when;	
	int			fd, len, flen=0;

	if ((cnt != 1) || (pValue->type != VARSMI_STRING))
		return IF4ERR_INVALID_PARAM;

	pszFileName = (char *)pValue->stream.p;
	if (stat(pszFileName, &file_info) == -1)
		return IF4ERR_INVALID_FILENAME;

	XDEBUG("Length = %0d\xd\xa", file_info.st_size);
	XDEBUG("Mode   = %0d\xd\xa", file_info.st_mode);

	fd = open(pszFileName, O_RDONLY);
	if (fd <= 0)
		return IF4ERR_INVALID_FILENAME;

	when 	    = *localtime(&file_info.st_mtime);
	tmFile.year	= when.tm_year + 1900;
	tmFile.mon	= when.tm_mon + 1,
	tmFile.day  = when.tm_mday;
	tmFile.hour = when.tm_hour;
	tmFile.min	= when.tm_min;
	tmFile.sec	= when.tm_sec;

	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, (UINT)file_info.st_size);
	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, (UINT)file_info.st_mode);
	IF4API_AddResultFormat(pWrapper, "1.16", VARSMI_TIMESTAMP, &tmFile, sizeof(TIMESTAMP));

	flen = MIN(50000, file_info.st_size);
	pszBuffer = (char *)MALLOC(flen);
	if (pszBuffer != NULL)
	{
		while((len=read(fd, pszBuffer, flen)) > 0)
			IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, pszBuffer, len);
		FREE(pszBuffer);
	}

	close(fd);
	return IF4ERR_NOERROR;
}

int cmdPutFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char		*p, *pszFileName;
	char		szPath[128];
	//TIMESTAMP	*pTime;
	int			i, fd, nMode;
    unsigned int  nSum=0, fsize;

	if (cnt < 5)
		return IF4ERR_INVALID_PARAM;

	if ((pValue[0].type != VARSMI_STRING) ||
		(pValue[1].type != VARSMI_UINT) ||
		(pValue[2].type != VARSMI_UINT) ||
		(pValue[3].type != VARSMI_TIMESTAMP) ||
		(pValue[4].type != VARSMI_STREAM))
		return IF4ERR_INVALID_PARAM;

	nSum = 0;
	for(i=4; i<cnt; i++)
		nSum += pValue[i].len;

    fsize = (unsigned int) pValue[1].stream.u32;
	if (fsize != nSum)
		return IF4ERR_INVALID_LENGTH;	

	pszFileName = (char *)pValue[0].stream.p;
	//pTime = (TIMESTAMP *)pValue[3].stream.p;
	nMode = (int)pValue[2].stream.u32;

	p = strrchr(pszFileName, '/');
	if (p != NULL)
	{
		strcpy(szPath, pszFileName);
		p = strrchr(szPath, '/');
		*p = '\0';
		mkdir(szPath, 0755);
	}

	fd = open(pszFileName, O_CREAT|O_WRONLY, nMode);
	if (fd <= 0)
		return IF4ERR_INVALID_FILENAME;

	for(i=4; i<cnt; i++)
		write(fd, pValue[i].stream.p, pValue[i].len);

	close(fd);
	return IF4ERR_NOERROR;
}

int cmdInstallFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	return IF4ERR_NOERROR;
}

int cmdGetFirmware(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	return IF4ERR_NOERROR;
}

int cmdDistributeFirmware(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    return IF4ERR_DEPRECATED_FUNCTION;
}

int cmdPackageDistribution(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    return IF4ERR_DEPRECATED_FUNCTION;
}

int cmdUpdateCoordinator(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	struct	stat file_info;
	char	szFileName[128];
    int         nError;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (pValue[0].type != VARSMI_STRING)
		return IF4ERR_INVALID_PARAM;

	memset(szFileName, 0, 128);
	strncpy(szFileName, pValue[0].stream.p, 127);

	if (!IsExists(szFileName))
		return IF4ERR_INVALID_FILENAME;
                     
	stat(szFileName, &file_info);

	XDEBUG("------ Coordinator firmware update -----\xd\xa");	
	XDEBUG("Filename = %s\xd\xa", szFileName);
	XDEBUG("    Size = %.2f K\xd\xa", (double)file_info.st_size / (double)1024);

	nError = codiDownload(GetCoordinator(), CODI_MODEMTYPE_XMODEM, szFileName, "ebl");
	if (nError != CODIERR_NOERROR)
	{
		XDEBUG("------ Coordinator firmware update fail -----\xd\xa");	
		return nError;
    }

	XDEBUG("------ Coordinator firmware update done -----\xd\xa");	
	return IF4ERR_NOERROR;
}

int cmdMergeAndInstall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	szDiff[512];
	char	szOrig[512];
	char	szNew[512];
	char	szInstall[512];
	char	szCommand[2048];
    BOOL    bReset = FALSE;
	struct	stat diff_info, orig_info;

	if (cnt < 3)
		return IF4ERR_INVALID_PARAM;

	memset(szDiff, 0, sizeof(szDiff));
	strncpy(szDiff, pValue[0].stream.p, pValue[0].len > (int) sizeof(szDiff) ? sizeof(szDiff) - 1 : pValue[0].len);
	memset(szOrig, 0, sizeof(szOrig));
	strncpy(szOrig, pValue[1].stream.p, pValue[1].len > (int) sizeof(szOrig) ? sizeof(szOrig) - 1 : pValue[1].len);
	memset(szNew, 0, sizeof(szNew));
	strncpy(szNew, pValue[2].stream.p, pValue[2].len > (int) sizeof(szNew) ? sizeof(szDiff) - 1 : pValue[2].len);

	memset(szInstall, 0, sizeof(szInstall));
    if (cnt > 3) {
	    strncpy(szInstall, pValue[3].stream.p, pValue[3].len > (int) sizeof(szInstall) ? sizeof(szDiff) - 1 : pValue[3].len);
    }
    if (cnt > 4) {
        bReset = pValue[4].stream.u32 ? TRUE : FALSE;
    }

	if (!IsExists(szDiff) || !IsExists(szOrig) || (cnt > 3 && szInstall[0] && !IsExists(szInstall)))
		return IF4ERR_INVALID_FILENAME;
   
	stat(szDiff, &diff_info);
	stat(szOrig, &orig_info);

	XDEBUG("------ Merge And Install -----\xd\xa");	
	XDEBUG("     Base File = %s\xd\xa", szOrig);
	XDEBUG("          Size = %.2f K\xd\xa", (double)orig_info.st_size / (double)1024);
	XDEBUG("     Diff File = %s\xd\xa", szOrig);
	XDEBUG("          Size = %.2f K\xd\xa", (double)diff_info.st_size / (double)1024);
	XDEBUG("      New File = %s\xd\xa", szNew);
	XDEBUG("Install Script = %s\xd\xa", szInstall);
	XDEBUG("  System Reset = %s\xd\xa", bReset ? "TRUE" : "FALSE");

    sprintf(szCommand,"/app/sw/bspatch %s %s %s", szOrig, szNew, szDiff);
    if(SystemExec(szCommand)) {
	    XDEBUG(" Merger FAIL : %s\xd\xa", szCommand);
        return IF4ERR_EXCPTION;
    }

    if(IsExists(szInstall)) {
        XDEBUG(" Merge And Install : Execute Script\xd\xa");
        SystemExec(szInstall);
    }

    if(bReset) {
        XDEBUG(" Merge And Install : Reset System\xd\xa");
	    cmdMcuReset(pWrapper, NULL, 0);
    }

	XDEBUG("------ Merge And Install done -----\xd\xa");	
	return IF4ERR_NOERROR;
}
                     
int cmdDistribution(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	EUI64	*pList = NULL;
	int		i, nError, nCount=0;
    int     nParamCnt = 19;

	if (cnt < nParamCnt) return IF4ERR_INVALID_PARAM;
	if (VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0) 	return IF4ERR_INVALID_TRIGGERID;            // TriggerId
	if (VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0) 		return IF4ERR_INVALID_OTA_TYPE;             // Type
	if (VARAPI_OidCompare(&pValue[2].oid, "1.11") != 0) 	return IF4ERR_INVALID_TRANSFER_TYPE;        // Model
	if (VARAPI_OidCompare(&pValue[3].oid, "1.4") != 0) 		return IF4ERR_INVALID_OTA_STEP;             // Transfer Type
	if (VARAPI_OidCompare(&pValue[4].oid, "1.6") != 0) 		return IF4ERR_INVALID_PARAM;                // OTA Step
	if (VARAPI_OidCompare(&pValue[5].oid, "1.4") != 0) 		return IF4ERR_INVALID_PARAM;                // Multicast Write count
	if (VARAPI_OidCompare(&pValue[6].oid, "1.6") != 0) 		return IF4ERR_INVALID_PARAM;                // Max Retry Count
	if (VARAPI_OidCompare(&pValue[7].oid, "1.4") != 0) 		return IF4ERR_INVALID_PARAM;                // OTA Thread count
	if (VARAPI_OidCompare(&pValue[8].oid, "1.4") != 0) 		return IF4ERR_INVALID_PARAM;                // Install Type
	if (VARAPI_OidCompare(&pValue[9].oid, "1.5") != 0) 		return IF4ERR_INVALID_VERSION;              // Old H/W Version
	if (VARAPI_OidCompare(&pValue[10].oid, "1.5") != 0) 	return IF4ERR_INVALID_VERSION;              // Old S/W Version
	if (VARAPI_OidCompare(&pValue[11].oid, "1.5") != 0) 	return IF4ERR_INVALID_VERSION;              // Old Build Version
	if (VARAPI_OidCompare(&pValue[12].oid, "1.5") != 0) 	return IF4ERR_INVALID_VERSION;              // New H/W Version
	if (VARAPI_OidCompare(&pValue[13].oid, "1.5") != 0) 	return IF4ERR_INVALID_VERSION;              // New S/W Version
	if (VARAPI_OidCompare(&pValue[14].oid, "1.5") != 0) 	return IF4ERR_INVALID_VERSION;              // New Build Version
	if (VARAPI_OidCompare(&pValue[15].oid, "1.11") != 0) 	return IF4ERR_INVALID_BIN_URL;              // Binary File URL
	if (VARAPI_OidCompare(&pValue[16].oid, "1.11") != 0) 	return IF4ERR_INVALID_BIN_CHECKSUM;         // Binary File Checksum
	if (VARAPI_OidCompare(&pValue[17].oid, "1.11") != 0) 	return IF4ERR_INVALID_DIFF_URL;             // Diff File URL
	if (VARAPI_OidCompare(&pValue[18].oid, "1.11") != 0)	return IF4ERR_INVALID_DIFF_CHECKSUM;        // Diff File Checksum

	nCount = cnt - nParamCnt;
	if (nCount > 0)
	{	
		// EUI64 ID 검사
		for(i=0; i<nCount; i++)
		{
	        if (!IsValidEUI64(&pValue[i+nParamCnt].oid))
				return IF4ERR_INVALID_ID;
		}
 
		// 리스트를 할당해서 넘긴다.
		pList = (EUI64 *)MALLOC(sizeof(EUI64)*nCount);
		if (pList == NULL) return IF4ERR_MEMORY_ERROR;
		for(i=0; i<nCount; i++)
			memcpy(&pList[i], pValue[i+nParamCnt].stream.p, sizeof(EUI64));
    }

	nError = m_pDistributor->Add(pValue[0].stream.p,			// TriggerId [STRING]
							pValue[1].stream.u8,				// Type [BYTE]
							pValue[2].stream.p,					// Model [STRING]
							pValue[3].stream.u8,				// Transfer Type [BYTE]
							pValue[4].stream.u32,				// OTA Step [UINT]
							pValue[5].stream.u8,				// Multicast Write Count [BYTE]
							pValue[6].stream.u32,				// Max Retry Count [UINT]
							pValue[7].stream.u8,				// OTA Thread Count [BYTE]
							pValue[8].stream.u8,				// Install Type [BYTE]
							pValue[9].stream.u16,				// Old H/W Version [WORD]
							pValue[10].stream.u16,				// Old S/W Version [WORD]
							pValue[11].stream.u16,				// Old Build Version [WORD]
							pValue[12].stream.u16,				// New H/W Version [WORD]
							pValue[13].stream.u16,				// New S/W Version [WORD]
							pValue[14].stream.u16,				// New Build Version [WORD]
							pValue[15].stream.p,				// Binary File URL [STRING]
							pValue[16].stream.p,				// Binary MD5 Checksum [STRING]
							pValue[17].stream.p,				// DIFF File URL [STRING]
							pValue[18].stream.p,				// DIFF MD5 Checksum [STRING]
							pList,								// Sensor EUI64 ID List
							nCount,                             // List Count
                            ENABLE_DOWNLOAD);                  // Download Flag
	if (nError != IF4ERR_NOERROR)
	{
		// 에러인 경우 List를 free한다. 정상이면 Distributor가 free한다.
		if (pList != NULL) FREE(pList);
		return nError;
	}
	return IF4ERR_NOERROR;
}

int cmdDistributionState(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	OTASTATE	state;
	int		nError;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)
		return IF4ERR_INVALID_TRIGGERID;

	memset(&state, 0, sizeof(OTASTATE));
	nError = m_pDistributor->GetState(pValue[0].stream.p, &state);
	if (nError != IF4ERR_NOERROR)
		return nError;

	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STRING, pValue[0].stream.p, pValue[0].len);
	IF4API_AddResultFormat(pWrapper, "1.21", VARSMI_STREAM, &state.tmStart, sizeof(GMTTIMESTAMP));
	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, state.nElapse);
	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, state.nTotal);
	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, state.nComplete);
	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, state.nFail);
	IF4API_AddResultNumber(pWrapper, "1.4", VARSMI_BYTE, state.nState);
	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, state.nOtaStep);
	return IF4ERR_NOERROR;
}

int cmdDistributionCancel(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)
		return IF4ERR_INVALID_TRIGGERID;

	return m_pDistributor->Cancel(pValue[0].stream.p);
    }

int cmdDistributionList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	DISTRIBUTIONENTRY	*pList = NULL;
	int		i, nCount=0, nError;

	nError = m_pDistributor->GetList(&pList, &nCount);
	if (nError != IF4ERR_NOERROR)
		return nError;

	for(i=0; i<nCount; i++)
		IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, &pList[i], sizeof(DISTRIBUTIONENTRY));
	
	if (pList) FREE(pList);
	return IF4ERR_NOERROR;
    }

int cmdDistributionInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	OTANODEENTRY	*pList = NULL;
	int		i, nCount=0, nError;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)
		return IF4ERR_INVALID_TRIGGERID;

	nError = m_pDistributor->GetOtaNodes(pValue[0].stream.p, &pList, &nCount);
	if (nError != IF4ERR_NOERROR)
		return nError;

	for(i=0; i<nCount; i++)
		IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, &pList[i], sizeof(OTANODEENTRY));

	if (pList) FREE(pList);
	return IF4ERR_NOERROR;
}
                     
int cmdDistributionSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	EUI64	*pList = NULL;
	int		i, nError, nCount=0;
    int     nParamCnt = 12;

	if (cnt < nParamCnt) return IF4ERR_INVALID_PARAM;
	if (VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0) 	return IF4ERR_INVALID_TRIGGERID;            // TriggerId
	if (VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0) 	return IF4ERR_INVALID_TRANSFER_TYPE;        // Model
	if (VARAPI_OidCompare(&pValue[2].oid, "1.4") != 0) 		return IF4ERR_INVALID_OTA_STEP;             // Transfer Type
	if (VARAPI_OidCompare(&pValue[3].oid, "1.6") != 0) 		return IF4ERR_INVALID_PARAM;                // OTA Step
	if (VARAPI_OidCompare(&pValue[4].oid, "1.4") != 0) 		return IF4ERR_INVALID_PARAM;                // Multicast Write count
	if (VARAPI_OidCompare(&pValue[5].oid, "1.6") != 0) 		return IF4ERR_INVALID_PARAM;                // Max Retry Count
	if (VARAPI_OidCompare(&pValue[6].oid, "1.4") != 0) 		return IF4ERR_INVALID_PARAM;                // OTA Thread count
	if (VARAPI_OidCompare(&pValue[7].oid, "1.4") != 0) 		return IF4ERR_INVALID_PARAM;                // Install Type
	if (VARAPI_OidCompare(&pValue[8].oid, "1.5") != 0) 		return IF4ERR_INVALID_VERSION;              // Old H/W Version
	if (VARAPI_OidCompare(&pValue[9].oid, "1.5") != 0) 	    return IF4ERR_INVALID_VERSION;              // Old S/W Version
	if (VARAPI_OidCompare(&pValue[10].oid, "1.5") != 0) 	return IF4ERR_INVALID_VERSION;              // Old Build Version
	if (VARAPI_OidCompare(&pValue[11].oid, "1.11") != 0) 	return IF4ERR_INVALID_BIN_URL;              // Binary File URL

	nCount = cnt - nParamCnt;
	if (nCount > 0)
	{	
		// EUI64 ID 검사
		for(i=0; i<nCount; i++)
		{
	        if (!IsValidEUI64(&pValue[i+nParamCnt].oid))
				return IF4ERR_INVALID_ID;
		}
 
		// 리스트를 할당해서 넘긴다.
		pList = (EUI64 *)MALLOC(sizeof(EUI64)*nCount);
		if (pList == NULL) return IF4ERR_MEMORY_ERROR;
		for(i=0; i<nCount; i++)
			memcpy(&pList[i], pValue[i+nParamCnt].stream.p, sizeof(EUI64));
    }

	nError = m_pDistributor->Add(pValue[0].stream.p,			// TriggerId [STRING]
							OTA_TYPE_SENSOR,				    // Type [BYTE]
							pValue[1].stream.p,					// Model [STRING]
							pValue[2].stream.u8,				// Transfer Type [BYTE]
							pValue[3].stream.u32,				// OTA Step [UINT]
							pValue[4].stream.u8,				// Multicast Write Count [BYTE]
							pValue[5].stream.u32,				// Max Retry Count [UINT]
							pValue[6].stream.u8,				// OTA Thread Count [BYTE]
							pValue[7].stream.u8,				// Install Type [BYTE]
							pValue[8].stream.u16,				// Old H/W Version [WORD]
							pValue[9].stream.u16,				// Old S/W Version [WORD]
							pValue[10].stream.u16,				// Old Build Version [WORD]
							0x01,                                  // New H/W Version [WORD]
							0x01,                                  // New S/W Version [WORD]
							1,                                  // New Build Version [WORD]
							pValue[11].stream.p,				// Binary File URL [STRING]
							NULL,                               // Binary MD5 Checksum [STRING]
							NULL,                               // DIFF File URL [STRING]
							NULL,                               // DIFF MD5 Checksum [STRING]
							pList,								// Sensor EUI64 ID List
							nCount,                             // List Count
                            DISABLE_DOWNLOAD);                  // Download Flag
	if (nError != IF4ERR_NOERROR)
	{
		// 에러인 경우 List를 free한다. 정상이면 Distributor가 free한다.
		if (pList != NULL) FREE(pList);
		return nError;
	}
	return IF4ERR_NOERROR;
}


//////////////////////////////////////////////////////////////////////
// Common Control Command
//////////////////////////////////////////////////////////////////////

/** Issue #2036: cmdStdGet, cmdStdSet, cmdStdGetChild에서 주고받는
  *  Data들의 Endian을 확실하게 정의해야 한다.
  *  현재는 Littlen Endian으로 VAR Variable이 저장된다고 가정하고 있는데
  *  이 부분을 자체 Endian을 사용하고 IF4로 날라 다닐 때만
  *  Little Endian을 사용하도록 해야 할 것 같다.
  **/
int cmdStdGet(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	MIBValue	*pGet;
	BOOL		bError=FALSE;
	int			i;

	if (cnt == 0)
		return IF4ERR_NEED_MORE_VALUE;

	for(i=0; i<cnt; i++)
	{
		pGet = VARAPI_GetValue(&pValue[i].oid, pValue);
		if (pGet == NULL)
		{
			bError = TRUE;
			continue;
		}

		IF4API_AddResultMIBValue(pWrapper, pGet);
		VARAPI_FreeValue(pGet);
	}
	return bError ? IF4ERR_CANNOT_GET : IF4ERR_NOERROR;
}

int cmdStdSet(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	BOOL	*pResult;
	BOOL	bError=FALSE;
	int		i;

	if (cnt == 0)
		return IF4ERR_NEED_MORE_VALUE;

	for(i=0; i<cnt; i++)
	{
		switch(pValue[i].type) {
		  case VARSMI_BOOL :
	  	  case VARSMI_BYTE :
		  case VARSMI_WORD :
		  case VARSMI_UINT :
		  case VARSMI_CHAR :
		  case VARSMI_SHORT :
		  case VARSMI_INT :
		  case VARSMI_OID :
		  case VARSMI_EUI64 :
		  case VARSMI_IPADDR :
		  case VARSMI_TIMESTAMP :
		  case VARSMI_TIMEDATE :
		  case VARSMI_OPAQUE :
			   if (pValue[i].len <= 0)
			      return IF4ERR_INVALID_PARAM;
		}
	}

	pResult = (BOOL *)MALLOC(sizeof(BOOL)*cnt);
	if (pResult == NULL)
		return IF4ERR_MEMORY_ERROR;

	memset(pResult, 1, sizeof(BOOL)*cnt);
	for(i=0; i<cnt; i++)
	{
		if (VARAPI_SetValue(&pValue[i], &pValue[i]) != VARERR_NOERROR)
		{
			pResult[i] = 0;
			bError = TRUE;
			continue;
		}
	}

	mcuChangeEvent(pValue, cnt, pResult);
	FREE(pResult);
	return bError ? IF4ERR_CANNOT_SET : IF4ERR_NOERROR;
}

int cmdStdGetChild(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	VAROBJECT 	*pObject;
	MIBValue	*pGet;
	int			i;

	if (cnt == 0)
		return IF4ERR_NEED_MORE_VALUE;

	pObject = VARAPI_GetObject(&pValue->oid);
	if (pObject == NULL)
		return IF4ERR_INVALID_OID;

	if (pObject->pChild == NULL)
		return IF4ERR_NO_ENTRY;

	pObject = pObject->pChild;
	for(i=0; pObject[i].pszName; i++)
	{
		pGet = VARAPI_GetValue(&pObject[i].var.oid, pValue);
		if (pGet != NULL)
		{
			IF4API_AddResultMIBValue(pWrapper, pGet);
			VARAPI_FreeValue(pGet);
		}
	}
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Metering Command
//////////////////////////////////////////////////////////////////////

int cmdMetering(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	// 삭제됨
	return IF4ERR_DEPRECATED_FUNCTION;
}

int cmdMeteringAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	// 삭제됨
	return IF4ERR_DEPRECATED_FUNCTION;
}

int cmdRecovery(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	//TIMESTAMP	*pTime;

	if (pValue[0].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	//pTime = (TIMESTAMP *)pValue[0].stream.p;
	m_pRecovery->Recovery();	
	return IF4ERR_NOERROR;
}

int AddMeteringInfo(IF4Wrapper *pWrapper, ENDIENTRY * pEndi, char *pszBuffer, TIMESTAMP *pStart, TIMESTAMP *pEnd, WORD nLimit)
{
	struct	stat 	file_info;
	METERLPENTRY 	*pEntry;
    EUI64           id;
	char			szSerial[20];
	char			szStart[32], szEnd[32];
	char			szFileName[128], *pszData;
	int				fd, cnt, len, preCnt=0;
	int				nLength/*, nSeek*/;
    struct dirent **namelist=NULL;
    int i,n;

	cnt = 0;
	sprintf(szStart, "%04d%02d%02d%02d", pStart->year, pStart->mon, pStart->day, pStart->hour);
	sprintf(szEnd, "%04d%02d%02d%02d", pEnd->year, pEnd->mon, pEnd->day, pEnd->hour);

	memset(&id, 0, sizeof(EUI64));
	szSerial[0] = '\0';

	memcpy(&id, &pEndi->id, sizeof(EUI64));
	strcpy(szSerial, pEndi->szSerial);

    n = scandir("/app/data", &namelist, 0, alphasort);

    if(n<=0) return 0;

	pszData = (char *)MALLOC(100*1024);
    if(pszData == NULL) return 0;

    if(nLimit != 0) {
	    for(i=0; i<n; i++)
	    {
#if     defined(_DIRENT_HAVE_D_TYPE)
		    if (namelist[i]->d_type == DT_DIR)
			    continue;
#endif
		    if (strncmp(namelist[i]->d_name, szStart, DATE_TIME_LEN) < 0)
			    continue;

		    if (strncmp(namelist[i]->d_name, szEnd, DATE_TIME_LEN) > 0)
			    continue;

		    sprintf(szFileName, "/app/data/%s", namelist[i]->d_name);
		    stat(szFileName, &file_info);
	        fd = open(szFileName, O_RDONLY);
	        if (fd > 0)
	        {
		        for(;;)
		        {
			        nLength = read(fd, pszData, sizeof(METERLPENTRY));
			        if (nLength != (int)sizeof(METERLPENTRY))
				        break;

			        pEntry = (METERLPENTRY *)pszData;

			        /*nSeek = */lseek(fd, LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP), SEEK_CUR);
                    if (memcmp(&id, &pEntry->mlpId, sizeof(EUI64)) != 0)
				        continue;
			        preCnt++;
		        }
		        close(fd);
		    }
	    }
    }
	for(i=0; i<n; i++)
	{
#if     defined(_DIRENT_HAVE_D_TYPE)
		if (namelist[i]->d_type == DT_DIR)
			continue;
#endif
		if (strncmp(namelist[i]->d_name, szStart, DATE_TIME_LEN) < 0)
			continue;

		if (strncmp(namelist[i]->d_name, szEnd, DATE_TIME_LEN) > 0)
			continue;

		sprintf(szFileName, "/app/data/%s", namelist[i]->d_name);
		stat(szFileName, &file_info);
	    fd = open(szFileName, O_RDONLY);
	    if (fd > 0)
	    {
		    for(;;)
		    {
			    nLength = read(fd, pszData, sizeof(METERLPENTRY));
			    if (nLength != (int)sizeof(METERLPENTRY))
				    break;

			    pEntry = (METERLPENTRY *)pszData;

                /** FEP의 요청으로 pData->lenght에 실제 data Length에 TIMESTAMP 길이까지
                 * 포함해서 보낸다. 따라서 실제 Data 길이는 TIMESTAMP 길이를 빼야 한다
                 */

                if (memcmp(&id, &pEntry->mlpId, sizeof(EUI64)) != 0)
                {
			        /*nSeek = */lseek(fd, LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP), SEEK_CUR);
				    continue;
                }
			    /*nSeek = */read(fd, pszData+sizeof(METERLPENTRY), LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP));

                if(!nLimit || (cnt >= (preCnt - nLimit))) 
                {
			        len = sizeof(METERLPENTRY) + LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP);
			        IF4API_AddResultFormat(pWrapper, "10.3", VARSMI_STREAM, pEntry, len);
                }
			    cnt++;
		    }
		    close(fd);
		}
	}
	FREE(pszData);
    free(namelist);
	return cnt;
}

int cmdGetMeter(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    char		*pszBuffer;
	ENDIENTRY *pEndi;
	TIMESTAMP	tmStart, tmEnd;
    EUI64       id;
    WORD        nLimit = 0;

	if (cnt < 3)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
	    return IF4ERR_INVALID_ID;

	if (pValue[1].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	if (pValue[2].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);
	if (pEndi == NULL)
	    return IF4ERR_INVALID_ID;

	memcpy(&tmStart, pValue[1].stream.p, sizeof(TIMESTAMP));
	memcpy(&tmEnd, pValue[2].stream.p, sizeof(TIMESTAMP));

    if (cnt > 3)
    {
        nLimit = pValue[3].stream.u16;
    }

    pszBuffer = (char *)MALLOC(100*1024);
    if (pszBuffer == NULL)
        return IF4ERR_MEMORY_ERROR;

    AddMeteringInfo(pWrapper, pEndi, (char *)pszBuffer, &tmStart, &tmEnd, nLimit);

    FREE(pszBuffer);
	return IF4ERR_NOERROR;
}

int GetMeteringDataToResult(IF4Wrapper *pWrapper, char *pszPath, int nLength, BOOL bSkipData)
{
	METERLPENTRY 	*pEntry;
	int				i, fd, len;
	int				nSeek, nCount = 0;
	char			*pszData;
	
	fd = open(pszPath, O_RDONLY);
	if (fd <= 0)
		return 0;

	pszData = (char *)MALLOC(nLength);
	if (pszData == NULL)
	{
		close(fd);
		return 0;
	}

	read(fd, pszData, nLength);
	close(fd);

	for(i=0, nSeek=1; i<nLength; i+=nSeek)
	{
		nSeek = 1;
		pEntry = (METERLPENTRY *)&pszData[i];

        /** FEP의 요청으로 pData->lenght에 실제 data Length에 TIMESTAMP 길이까지
         * 포함해서 보낸다. 따라서 실제 Data 길이는 TIMESTAMP 길이를 빼야 한다
         */
		nSeek = sizeof(METERLPENTRY) + LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP);
		len = sizeof(METERLPENTRY);
        if(!bSkipData) len += LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP);

		IF4API_AddResultFormat(pWrapper, "10.3", VARSMI_STREAM, pEntry, len);
		nCount++;
	}

	FREE(pszData);
	return nCount;
}

int GetMeteringData(IF4Wrapper *pWrapper, TIMESTAMP *pStart, TIMESTAMP *pEnd, BOOL bSkipData)
{
	DIR			*dir_fdesc;
	dirent		*dp;
	struct	stat file_info;
	char		szPath[128];
	char		szStart[20], szEnd[20];
	int			nTotal=0, nCount;

	sprintf(szStart, "%04d%02d%02d%02d", pStart->year, pStart->mon, pStart->day, pStart->hour);
	sprintf(szEnd, "%04d%02d%02d%02d", pEnd->year, pEnd->mon, pEnd->day, pEnd->hour);

	dir_fdesc = opendir("/app/data");
	if (dir_fdesc == NULL)
		return 0;

	for(; (dp=readdir(dir_fdesc));)
	{
#if     defined(_DIRENT_HAVE_D_TYPE)
		if (dp->d_type == DT_DIR)
			continue;
#endif

		if (dp->d_name[0] == '.')
			continue;

		if ((strncmp(dp->d_name, szStart, DATE_TIME_LEN) < 0) || (strncmp(dp->d_name, szEnd, DATE_TIME_LEN) > 0))
			continue;

		sprintf(szPath, "/app/data/%s", dp->d_name);
		stat(szPath, &file_info);
		nCount = GetMeteringDataToResult(pWrapper, szPath, (int)file_info.st_size, bSkipData);
		nTotal += nCount;
	}

	closedir(dir_fdesc);
	return nTotal;
}

int cmdGetMeterAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int             nCount;

	if (pValue[0].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	if (pValue[1].type != VARSMI_TIMESTAMP)
		return IF4ERR_INVALID_PARAM;

	nCount = GetMeteringData(pWrapper, (TIMESTAMP *)pValue[0].stream.p, (TIMESTAMP *)pValue[1].stream.p, 
        cnt > 2 ? (BOOL) pValue[2].stream.u8 : FALSE);

    return (nCount <= 0) ? IF4ERR_NO_ENTRY : IF4ERR_NOERROR;
}

int _OnDemandMeter(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt, 
        EUI64 *pId, char * szGUID, int nOption, int nOffset, int nCount)
{
	ENDIENTRY		*pEndi;
	METERLPENTRY	*pData;
	METERLPENTRY	header;
	METER_STAT	stat;
	CMDPARAM	param;
	CChunk		chunk(1024);
	char		szSerial[32];
	int			nError;

	pEndi = m_pEndDeviceList->GetEndDeviceByID(pId);
	if (pEndi == NULL)
	{
		XDEBUG("cmdOnDemand: Invalid ID: %s\xd\xa", szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memset(szSerial, 0, sizeof(szSerial));
	strncpy(szSerial, pEndi->szSerial, strlen(pEndi->szSerial));

    XDEBUG("\r\nOndemand(%s): Serial='%s', Option=%d, Offset=%d, Count=%d\r\n",
			szGUID, szSerial, nOption, nOffset, nCount);

	memset(&header, 0, sizeof(METERLPENTRY));
	memcpy(&header.mlpId, pId, sizeof(EUI64));
	strncpy(header.mlpMid, szSerial, strlen(szSerial));
	GetTimestamp(&header.mlpTime, NULL);
    header.mlpVendor = pEndi->nVendor;

	// 해더를 추가하고 Rollback을 위하여 Commit 위치를 준다.
	chunk.Add((char *)&header, sizeof(METERLPENTRY));
	chunk.Commit();

	memset(&param, 0, sizeof(CMDPARAM));
	memcpy(&param.id, pId, sizeof(EUI64));

    param.codi      = GetCoordinator();
	param.pWrapper	= pWrapper;
	param.argv		= pValue;
	param.argc		= cnt;
	param.nMeteringType = METERING_TYPE_SPECIAL;
	param.nOption	= nOption;
	param.nOffset   = nOffset;
    param.nCount    = nCount;
	param.pChunk	= &chunk;

	memset(&stat, 0, sizeof(METER_STAT));
	nError = m_pOndemander->Command(pEndi->szParser, &param, &stat);
	if (nError == ONDEMAND_ERROR_OK)
	{
        /** FEP의 요청으로 pData->lenght에 실제 data Length에 TIMESTAMP 길이까지
         * 포함해서 보낸다. 따라서 실제 Data 길이는 TIMESTAMP 길이를 빼야 한다
         */
		pData = (METERLPENTRY *)chunk.GetBuffer();
        pData->mlpDataCount = 1;

		// 검침 데이터에 METERLPENTRY가 포함되어 길이를 빼야 한다.
		pData->mlpLength  = HostToLittleShort(chunk.GetSize() - sizeof(METERLPENTRY) + sizeof(TIMESTAMP));

		IF4API_AddResultFormat(pWrapper, "10.3", VARSMI_STREAM, chunk.GetBuffer(), chunk.GetSize());
	}

    XDEBUG("\r\nOndemand(%s): Done (%d).\r\n\r\n", szGUID, nError);
	if (nError != ONDEMAND_ERROR_OK)
	{
	 	if (nError == ONDEMAND_ERROR_BUSY)
			 return IF4ERR_BUSY;
		else if (nError == ONDEMAND_ERROR_NOT_CONNECT)
			 return IF4ERR_CANNOT_CONNECT;
		else return IF4ERR_CANNOT_GET;
	}
	return IF4ERR_NOERROR;
}

int cmdOnDemandMeter(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char		szGUID[20];
    int         nOption=-1, nOffset=0, nCount=0;
	EUI64		id;

	if (cnt < 1)
		return IF4ERR_NEED_MORE_VALUE;
		
	if (IsValidEUI64(&pValue[0].oid))
	{   
		memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
		EUI64ToStr(&id, szGUID); 
	}   
	else return IF4ERR_INVALID_ID;

	if (cnt > 1)
	{
		if (VARAPI_OidCompare(&pValue[1].oid, "1.9") != 0)
			return IF4ERR_INVALID_PARAM;
		nOption = pValue[1].stream.u32;
	}
	
	if (cnt > 2)
	{
		if (VARAPI_OidCompare(&pValue[2].oid, "1.9") != 0)
			return IF4ERR_INVALID_PARAM;
		nOffset = pValue[2].stream.u32;
	}

	if (cnt > 3)
	{
		if (VARAPI_OidCompare(&pValue[3].oid, "1.9") != 0)
			return IF4ERR_INVALID_PARAM;
		nCount = pValue[3].stream.u32;
	}

    return _OnDemandMeter(pWrapper, pValue, cnt, &id, szGUID, nOption, nOffset, nCount);
}

int cmdOnDemandMeterAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	// 삭제됨(지원하지 않음)
	return IF4ERR_DEPRECATED_FUNCTION;
}

int cmdMeterUpload(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	TIMESTAMP	*pStart, *pEnd;

	if (cnt != 0)
	{
		if (cnt != 2)
			return IF4ERR_INVALID_PARAM;

		if ((pValue[0].type != VARSMI_TIMESTAMP) || (pValue[1].type != VARSMI_TIMESTAMP))
			return IF4ERR_INVALID_PARAM;
	
		pStart = (TIMESTAMP *)pValue[0].stream.p;
		pEnd   = (TIMESTAMP *)pValue[1].stream.p;
		m_pMeterUploader->Upload(pStart, pEnd);
		return IF4ERR_NOERROR;
	}

	m_pMeterUploader->Upload();
	return IF4ERR_NOERROR;
}

int cmdOnDemand(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    return IF4ERR_DEPRECATED_FUNCTION;
}

int cmdGetMeterSchedule(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	int			nOption=0,nOffset=0,nCount=0;
	int			idx=0;
	EUI64		id;

	if (cnt < 1)
		return IF4ERR_NEED_MORE_VALUE;
		
	if (IsValidEUI64(&pValue[0].oid))
	{   
		memcpy(&id, pValue[idx++].stream.p, sizeof(EUI64)); 
	}   
	else return IF4ERR_INVALID_ID;

	if (cnt > idx)
	{
		if (VARAPI_OidCompare(&pValue[idx].oid, "1.9") != 0)
			return IF4ERR_INVALID_PARAM;
		nOption = pValue[idx++].stream.u32;
	}
	
	if (cnt > idx)
	{
		if (VARAPI_OidCompare(&pValue[idx].oid, "1.9") != 0)
			return IF4ERR_INVALID_PARAM;
		nOffset = pValue[idx++].stream.u32;
	}

	if (cnt > idx)
	{
		if (VARAPI_OidCompare(&pValue[idx].oid, "1.9") != 0)
			return IF4ERR_INVALID_PARAM;
		nCount = pValue[idx++].stream.u32;
	}

	return m_pMeterReader->Metering(&id, nOption, nOffset, nCount);
}

int cmdOnDemandMbus(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char		szGUID[20];
	EUI64		id;
	ENDIENTRY   *pEndi;
	METER_STAT	stat;
	CMDPARAM	param;
	CChunk		chunk(1024);
	int			nError;

	if (cnt < 4)
		return IF4ERR_NEED_MORE_VALUE;
		
	if (IsValidEUI64(&pValue[0].oid))
	{   
		memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
		EUI64ToStr(&id, szGUID); 
	}   
	else return IF4ERR_INVALID_ID;

    if (VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0)
            return IF4ERR_INVALID_PARAM;
    if (VARAPI_OidCompare(&pValue[2].oid, "1.4") != 0)
            return IF4ERR_INVALID_PARAM;
    if (VARAPI_OidCompare(&pValue[3].oid, "1.4") != 0)
            return IF4ERR_INVALID_PARAM;

	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);
	if (pEndi == NULL)
	{
		XDEBUG("cmdOnDemand: Invalid ID: %s\xd\xa", szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memset(&param, 0, sizeof(CMDPARAM));
	memcpy(&param.id, &id, sizeof(EUI64));

    param.codi      = GetCoordinator();
	param.pWrapper	= pWrapper;
	param.argv		= pValue;
	param.argc		= cnt;
	param.nMeteringType = METERING_TYPE_BYPASS;
	param.pChunk	= &chunk;

	memset(&stat, 0, sizeof(METER_STAT));
	nError = m_pOndemander->Command(pEndi->szParser, &param, &stat);
	if (nError == ONDEMAND_ERROR_OK)
	{
		IF4API_AddResultFormat(pWrapper, "10.3", VARSMI_STREAM, chunk.GetBuffer(), chunk.GetSize());
	}

    XDEBUG("\r\nOndemand(%s): Done (%d).\r\n\r\n", szGUID, nError);
	if (nError != ONDEMAND_ERROR_OK)
	{
	 	if (nError == ONDEMAND_ERROR_BUSY)
			 return IF4ERR_BUSY;
		else if (nError == ONDEMAND_ERROR_NOT_CONNECT)
			 return IF4ERR_CANNOT_CONNECT;
		else return IF4ERR_CANNOT_GET;
	}
    return nError;
}

int cmdReadTable(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	if (cnt < 2)
		return IF4ERR_INVALID_PARAM;

	return cmdOnDemandMeter(pWrapper, pValue, cnt);
}

#if 0
/** ANSI Parser의 수정으로 이 방식은 더이상 지원하지 않는다.
  *
  * Relay control을 위해서는 ondemand option 4,5,6,7 을 사용하던지
  * cmdSetEnergyLevel을 사용해야 한다.
  */
int cmdWriteTable(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY	*pEndi;
	CMDPARAM	param;
	METER_STAT	stat;
	CChunk		chunk(1024);
    EUI64       id;
	char		szGUID[20], szSerial[20];
	int			nTable;
	int			nError;
	
	if (cnt != 3)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
	    return IF4ERR_INVALID_ID;

	if (VARAPI_OidCompare(&pValue[1].oid, "1.9") != 0)
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[2].oid, "1.12") != 0)
		return IF4ERR_INVALID_PARAM;

	nTable = pValue[1].stream.u32;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	eui64toa(&id, szGUID); 

	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);
	if (pEndi == NULL)
	{
		XDEBUG("cmdWriteTable: Invalid EUID: %s\xd\xa", szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memset(szSerial, 0, 20);
	strncpy(szSerial, pEndi->szSerial, 18);

    XDEBUG("\r\nWriteTable(%s): Serial='%s', Table=%d\r\n", szGUID, szSerial, nTable);

	memset(&param, 0, sizeof(CMDPARAM));
    param.codi      = GetCoordinator();
	param.pWrapper	= pWrapper;
	param.argv		= pValue;
	param.argc		= cnt;

	memcpy(&param.id, &pEndi->id, sizeof(EUI64));
	param.nOption	= nTable;
	param.pszParam	= pValue[2].stream.p;
	param.nLength	= pValue[2].len;
	param.pChunk	= &chunk;

	memset(&stat, 0, sizeof(METER_STAT));
	nError = m_pOndemander->Command(pEndi->szParser, &param, &stat);

	XDEBUG("WriteTable(%s): Length=%d\r\n", szGUID, chunk.GetSize());
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, chunk.GetBuffer(), chunk.GetSize());

    XDEBUG("\r\nWriteTable(%s): Done.\r\n\r\n", szGUID);
	if (nError != ONDEMAND_ERROR_OK)
	{
	 	if (nError == ONDEMAND_ERROR_BUSY)
			 return IF4ERR_BUSY;
		else if (nError == ONDEMAND_ERROR_NOT_CONNECT)
			 return IF4ERR_CANNOT_CONNECT;
		else if (nError == ONDEMAND_ERROR_PARAM)
			 return IF4ERR_INVALID_PARAM;
		else return IF4ERR_CANNOT_GET;
	}
	return IF4ERR_NOERROR;
}
#endif

int cmdReadTableAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	int		nResult;
    int     nOption = -1;
    int     nOffset = 96;
    int     nCount = 96;

    if(cnt > 0) {
	    if (VARAPI_OidCompare(&pValue[0].oid, "1.9") != 0)
		    return IF4ERR_INVALID_PARAM;
	    nOption = pValue[0].stream.u32;
    }

    if(cnt > 1) {
	    if (VARAPI_OidCompare(&pValue[1].oid, "1.9") != 0)
		    return IF4ERR_INVALID_PARAM;
	    nOffset = pValue[1].stream.u32;
    }

    if(cnt > 2) {
	    if (VARAPI_OidCompare(&pValue[1].oid, "1.9") != 0)
		    return IF4ERR_INVALID_PARAM;
	    nCount = pValue[2].stream.u32;
    }

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

    XDEBUG("\r\nReadTableAll: Start (Table=%d)\r\n", nOption);
	nResult = m_pMetering->Query(nOption, nOffset, nCount);
    XDEBUG("\r\nReadTableAll: Done. (nResult=%d)\r\n", nResult);
	return nResult;
}

int cmdCheckMeter(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY	*pEndi;
	CMDPARAM	param;
	METER_STAT	stat;
	CChunk		chunk(1024);
	char		szGUID[20], szSerial[20];
	int			nError;
    EUI64       id;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
	    return IF4ERR_INVALID_ID;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);

	if (pEndi == NULL)
	{
	    eui64toa(&pEndi->id, szGUID);
		XDEBUG("Invalid EUI ID: %s\xd\xa",szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memset(szSerial, 0, 20);
	strncpy(szSerial, pEndi->szSerial, 18);

    XDEBUG("\r\nCheck(%s): Serial='%s'\r\n", szGUID, szSerial);

	memset(&stat, 0, sizeof(METER_STAT));
	memset(&param, 0, sizeof(CMDPARAM));
    param.codi      = GetCoordinator();
	param.pWrapper	= pWrapper;
	param.argv		= pValue;
	param.argc		= cnt;

	memcpy(&param.id, &pEndi->id, sizeof(EUI64));
    param.nMeteringType = METERING_TYPE_SPECIAL;
	param.nOption	= ONDEMAND_OPTION_READ_IDENT_ONLY;
	param.pChunk	= &chunk;

	nError = m_pOndemander->Command(pEndi->szParser, &param, &stat);
	if (nError == ONDEMAND_ERROR_OK)
	{
		IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, chunk.GetBuffer(), chunk.GetSize());
		IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, &stat, sizeof(METER_STAT));
	}

    XDEBUG("\r\nCheck(%s): Done.\r\n\r\n", szGUID);
	if (nError != ONDEMAND_ERROR_OK)
	{
	 	if (nError == ONDEMAND_ERROR_BUSY)
			 return IF4ERR_BUSY;
		else if (nError == ONDEMAND_ERROR_NOT_CONNECT)
			 return IF4ERR_CANNOT_CONNECT;
		else if (nError == ONDEMAND_ERROR_PARAM)
			 return IF4ERR_INVALID_PARAM;
		else return IF4ERR_CANNOT_GET;
	}
	return IF4ERR_NOERROR;
}

int cmdSyncMeterTime(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY	*pEndi;
	CChunk		chunk(1024);
    EUI64       id;
	char		szGUID[20], szSerial[20];
	
	if (!IsValidEUI64(&pValue[0].oid))
	    return IF4ERR_INVALID_ID;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);

	if (pEndi == NULL)
	{
	    eui64toa(&id, szGUID);
		XDEBUG("cmdSyncMeterTime: Invalid EUI ID: %s\r\n", szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memset(szSerial, 0, 20);
	eui64toa(&pEndi->id, szGUID);
	strncpy(szSerial, pEndi->szSerial, 18);
    XDEBUG("\r\ncmdSyncMeterTime(%s): Serial='%s'\r\n", szGUID, szSerial);

    return _OnDemandMeter(pWrapper, pValue, cnt, &id, szGUID, ONDEMAND_OPTION_TIME_SYNC, 0, 1);
}

int cmdSyncMeterTimeAll(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	return m_pPolling->Polling(POLLING_TYPE_TIMESYNC);
}

int cmdAidonMCCB(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	HANDLE	codi;
	char	szGUID[17];
	EUI64	id;
	BYTE	value, reply = 0;
	int		nReturn;

	if (!IsValidEUI64(&pValue[0].oid))
	    return IF4ERR_INVALID_ID;

	if (cnt != 2)
		return IF4ERR_INVALID_PARAM;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
    eui64toa(&id, szGUID);
	value = pValue[1].stream.u8;

	codi = GetCoordinator();
	nReturn = m_pOndemander->MccbWrite(codi, &id, value, &reply);
	if (nReturn == IF4ERR_NOERROR)
		IF4API_AddResultNumber(pWrapper, "1.4", VARSMI_BYTE, reply);
	return nReturn;
}

int cmdKamstrupCID(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char		szGUID[20];
	EUI64		id;
	ENDIENTRY   *pEndi;
	METER_STAT	stat;
	CMDPARAM	param;
	CChunk		chunk(1024);
	int			nError;

	if (!IsValidEUI64(&pValue[0].oid))
	    return IF4ERR_INVALID_ID;

    if (VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0)
        return IF4ERR_INVALID_PARAM;

	if (cnt > 2)
    {
        if (VARAPI_OidCompare(&pValue[2].oid, "1.12") != 0)
            return IF4ERR_INVALID_PARAM;
    }

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	eui64toa(&id, szGUID);

	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);
	if (pEndi == NULL)
	{
		XDEBUG("cmdOnDemand: Invalid ID: %s\xd\xa", szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	memset(&param, 0, sizeof(CMDPARAM));
	memcpy(&param.id, &id, sizeof(EUI64));

    param.codi      = GetCoordinator();
	param.pWrapper	= pWrapper;
	param.argv		= pValue;
	param.argc		= cnt;
	param.nMeteringType = METERING_TYPE_BYPASS;
	param.pChunk	= &chunk;

	memset(&stat, 0, sizeof(METER_STAT));
	nError = m_pOndemander->Command(pEndi->szParser, &param, &stat);
	if (nError == ONDEMAND_ERROR_OK)
	{
		IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, chunk.GetBuffer(), chunk.GetSize());
	}

    XDEBUG("\r\nOndemand(%s): Done (%d).\r\n\r\n", szGUID, nError);
	if (nError != ONDEMAND_ERROR_OK)
	{
	 	if (nError == ONDEMAND_ERROR_BUSY)
			 return IF4ERR_BUSY;
		else if (nError == ONDEMAND_ERROR_NOT_CONNECT)
			 return IF4ERR_CANNOT_CONNECT;
		else return IF4ERR_CANNOT_GET;
	}
	return nError;
}

int cmdSetTestMode(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	m_bGprsPerfTestMode = pValue[0].stream.u8 > 0 ? TRUE : FALSE;
	m_bEnableAutoMetering = m_bGprsPerfTestMode ? FALSE : TRUE;
		
	SetFileValue("/app/conf/testmode", (int)m_bGprsPerfTestMode);
	return IF4ERR_NOERROR;
}

int cmdSetPingTestMode(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	BOOL	bPing;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	bPing = pValue[0].stream.u8 > 0 ? TRUE : FALSE;

    /*-- 현재 상태와 같으면 바로 리턴한다. --*/
    if(m_bPingTestMode == bPing) return IF4ERR_NOERROR;

#ifdef __SINGLE_THREAD_ONLY__
    if (bPing && IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	m_bPingTestMode = bPing;
	m_bEnableAutoMetering = m_bPingTestMode ? FALSE : TRUE;

	if (m_bPingTestMode) m_pPolling->Polling(POLLING_TYPE_CHECK);
	return IF4ERR_NOERROR;
}

int cmdGetConfiguration(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	szCommand[256], szName[64], *p;
	struct	stat file_info;

	strcpy(szName, SAVE_FILENAME);
	p = strstr(szName, ".gz");
	if (p != NULL) *p = '\0';
	
	if (IsExists(szName))
		unlink(szName);

	if (IsExists(SAVE_FILENAME))
		unlink(SAVE_FILENAME);

	printf("\r\n");
	printf("Save configuration.\r\n");
	VARAPI_Save(TRUE);

	usleep(1000000);
	SystemExec("sync");
	usleep(1000000);
	SystemExec("sync");

	printf("Make compress file.\r\n");

	chdir("/app");
	sprintf(szCommand, "tar cf %s conf member sw/ppp/chatscript sw/ppp/peers", szName);
	if (IsExists("/app/sw/dm_agent.cfg")) strcat(szCommand, " sw/dm_agent.cfg");
	if (IsExists("/app/sw/ndm_tree.xml")) strcat(szCommand, " sw/ndm_tree.xml");
	if (IsExists("/app/sw/phonelist")) strcat(szCommand, " sw/phonelist");
	strcat(szCommand, " > /tmp/result");
	SystemExec(szCommand);

	sprintf(szCommand, "gzip %s > /tmp/result", szName); 
	SystemExec(szCommand);
	chdir("/app/sw");

	printf("Send configrations.\r\n");

	stat(SAVE_FILENAME, &file_info);
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, SAVE_FILENAME, strlen(SAVE_FILENAME));
	IF4API_AddResultNumber(pWrapper, "1.9", VARSMI_INT, (int)file_info.st_size);
	return IF4ERR_NOERROR;
}

int cmdSetConfiguration(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	szCommand[128];

	if (!IsExists(SAVE_FILENAME))
	{
		printf("\r\n**** Configration file not found: %s ****\r\n", SAVE_FILENAME);
		return IF4ERR_INVALID_FILENAME;
	}

	VARAPI_EnableModify(FALSE);

	printf("\r\n");
	printf("Remove old configuration.\r\n");

	SystemExec("rm -rf /app/conf");
	SystemExec("rm -rf /app/member");
	SystemExec("rm -rf /app/data");
	SystemExec("rm -rf /app/event");
	SystemExec("rm -rf " LOG_DIR);
	SystemExec("rm -f /app/sw/timezone");
	SystemExec("rm -f /app/sw/phonelist");

	printf("Make new configuration.\r\n");
	chdir("/app");
	sprintf(szCommand, "tar zxf %s > /tmp/result", SAVE_FILENAME);
	SystemExec(szCommand);
	SystemExec("/bin/chown -R root:root /app/conf");
	SystemExec("/bin/chown -R root:root /app/member");
	SystemExec("/bin/chown -R root:root /app/sw");
	SystemExec("/bin/chown -R root:root /app/data");
	SystemExec("/bin/chown -R root:root /app/event");
	SystemExec("/bin/chown -R root:root " LOG_DIR);
	unlink(SAVE_FILENAME);
	chdir("/app/sw");

	printf("Sync ...\r\n");
	SystemExec("sync");
	usleep(1000000);
	SystemExec("sync");
	usleep(1000000);
	SystemExec("sync");

	printf("Reboot.\r\n");
	mcuResetEvent(GetSessionType(pWrapper->pSession), pWrapper->pSession->szAddress);
	m_pService->PostMessage(GWMSG_RESET, NULL);
	return IF4ERR_NOERROR;
}

BOOL cmdInitialDM()
{
  char  szTemp[32];
  int   i, m_szDMBDevID;
  FILE  *fp;
  double  /*fSwVer,*/ fHwVer;

  fp = fopen("/app/conf/dmagent.conf", "wb");
  if (fp == NULL)
    return FALSE;

  for(i = 0; i < 14 ; i++)
  {

      memset(szTemp, '\0', 32);
      switch(i)
      {
          case  0:
                GetFileValue("/app/conf/system.id", &m_szDMBDevID);
                sprintf(szTemp, "DevID:%d\xd\xa", m_szDMBDevID);
                break;
          case  1:
                strcpy(szTemp, "MAN:NURI\xd\xa");
                break;
          case  2:
                strcpy(szTemp, "LANG:ENG\xd\xa");
                break;
          case  3:
                sprintf(szTemp, "MOD:%s\xd\xa", SW_PREFIX_TYPE);
                break;
          case  4:
              sprintf(szTemp, "DevType:%s\xd\xa", HW_VERSION_FIX);
                break;
          case  5: 
                strcpy(szTemp, "FW Ver:1.0\xd\xa");
                break;
                //fSwVer = (SW_VERSION >> 4) + (SW_VERSION & 0x0f) * 0.1;
	            sprintf(szTemp, "SW Ver:%s\xd\xa", UPDATE_VERSION_FIX);
                break;
          case  7:     
                fHwVer = (HW_VERSION >> 4)  + (HW_VERSION & 0x0f) * 0.1;
                sprintf(szTemp, "HW Ver:%2.1f\xd\xa", fHwVer);
                break;
          case  8:
                strcpy(szTemp, "OEM:NURI\xd\xa");
                break;
          case  9:
                strcpy(szTemp, "DateOfMan:2007.01.01\xd\xa");
                break;
          case  10:
                sprintf(szTemp, "PName:%s_%s-v%s-%s.tar\xd\xa", 
				    HW_VERSION_FIX, 
				    (m_nSystemType==MCU_TYPE_OUTDOOR ? HW_MODEL_OUTDOOR:HW_MODEL_INDOOR),
				    UPDATE_VERSION_FIX, 
                    SW_PREFIX_TYPE);
                  break;
          default :
                  break;
         }
					usleep(1000);
         fwrite(szTemp, 32, 1, fp);
					usleep(1000);
       }
       fclose(fp);
			return TRUE;
}

int cmdGetEventConfigure(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EVTCFGENTRY * pEntry = NULL;
    int i,ret = 0;

    ret = m_pEventManager->GetEventConfigure(&pEntry);

    for(i=0;i<ret;i++) {
	    IF4API_AddResultFormat(pWrapper, "4.3", VARSMI_OPAQUE, &pEntry[i], sizeof(EVTCFGENTRY));
    }

    if(pEntry) FREE(pEntry);

	return IF4ERR_NOERROR;
}

int cmdSetEventConfigure(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    char eventName[64];
    BOOL bUseDefault = FALSE;
    BYTE nSeverity = 0xFF;
    int  nNotify = -1;

	if (cnt < 2) return IF4ERR_INVALID_PARAM;

	if ((VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)) return IF4ERR_INVALID_PARAM;
	if ((VARAPI_OidCompare(&pValue[1].oid, "1.3") != 0)) return IF4ERR_INVALID_PARAM;

    memset(eventName,0,sizeof(eventName));
    memcpy(eventName, pValue[0].stream.p, pValue[0].len);
    bUseDefault = pValue[1].stream.u8 ? TRUE : FALSE;

    if(cnt > 2) {
        nSeverity = pValue[2].stream.u8;
    }
    if(cnt > 3) {
        nNotify = (int) pValue[3].stream.u8;
    }

    m_pEventManager->UpdateEventConfigure(eventName, bUseDefault, nSeverity, nNotify);

	return IF4ERR_NOERROR;
}

int cmdMcuGetPatch(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int i;

    for(i=0;m_oPatchInformation[i].patchID;i++) {
        if(m_oPatchInformation[i].patchID != UINT_MAX) {
	        IF4API_AddResultFormat(pWrapper, "2.110", VARSMI_OPAQUE, &m_oPatchInformation[i], sizeof(PATCHENTRY));
        }
    }

	return IF4ERR_NOERROR;
}

int cmdMcuSetPatch(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    BOOL bApply = FALSE;
    UINT id = 0;
    int i;

	if (cnt < 2) return IF4ERR_INVALID_PARAM;

	if ((VARAPI_OidCompare(&pValue[0].oid, "1.6") != 0)) return IF4ERR_INVALID_PARAM;
	if ((VARAPI_OidCompare(&pValue[1].oid, "1.3") != 0)) return IF4ERR_INVALID_PARAM;

    id = (UINT) pValue[0].stream.u32;
    bApply = pValue[1].stream.u8 ? TRUE : FALSE;

    XDEBUG("cmdMcuSetPatch : id=[%d], bApply=[%s]\r\n", id, bApply ? "TRUE" : "FALSE");
    for(i=0;m_oPatchInformation[i].patchID;i++) {
        if(m_oPatchInformation[i].patchID == id) {
            m_oPatchInformation[i].patchState = bApply;
            if(m_oPatchHandler[i]) {
                m_oPatchHandler[i](i, m_oPatchInformation[i].patchID,
                    m_oPatchInformation[i].patchIssueNumber,
                    m_oPatchInformation[i].patchState,
                    m_oPatchInformation[i].patchDescr);
            }
            break;
        }
    }

    SavePatchList();

	return IF4ERR_NOERROR;
}

int cmdMcuCleaning(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    XDEBUG("cmdMcuCleaning : Activate");
    m_pMalfMonitor->Check();
	return IF4ERR_NOERROR;
}

int cmdGetFwInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    FWENTRY fw;
    char revision[64], buffer[64];
	BYTE szBuffer[CODI_MAX_PAYLOAD_SIZE];
	CODI_MODULE_PAYLOAD			*pModule;
    int i, len;
    int nLength;
    int nError;

    /** Concentrator Version Information */
    memset(&fw, 0, sizeof(FWENTRY));
    memset(revision, 0, sizeof(revision));

    fw.fwType = FW_TYPE_CONCENTRATOR;
    strcpy(fw.fwCore, "ARM");
    fw.fwHardware = ((HW_VERSION & 0xF0) << 4) | (HW_VERSION & 0x0F) ;
    fw.fwSoftware = ((SW_VERSION & 0xF0) << 4) | (SW_VERSION & 0x0F) ;
    strcpy(buffer, SW_REVISION_FIX);
    for(i=0,len=strlen(buffer); i < len; i++)
    {
        if(isdigit(buffer[i])) revision[i] = buffer[i];
        else break;
    }
    fw.fwBuild = strtol(revision, (char **)NULL, 10);

	IF4API_AddResultFormat(pWrapper, "2.111", VARSMI_OPAQUE, &fw, sizeof(FWENTRY));

    /** Coordinator Version Information */
    memset(&fw, 0, sizeof(FWENTRY));

    fw.fwType = FW_TYPE_COORDINATOR;
    strcpy(fw.fwCore, "EMBER");

    nError = codiGetProperty(GetCoordinator(), CODI_CMD_MODULE_PARAM, szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR) return IF4ERR_NOERROR;

	pModule = (CODI_MODULE_PAYLOAD *)szBuffer;

    fw.fwHardware = ((pModule->hw & 0xF0) << 4) | (pModule->hw & 0x0F) ;
    fw.fwSoftware = ((pModule->fw & 0xF0) << 4) | (pModule->fw & 0x0F) ;
    fw.fwBuild = ((pModule->build >> 4) * 10) + (pModule->build & 0x0F);

	IF4API_AddResultFormat(pWrapper, "2.111", VARSMI_OPAQUE, &fw, sizeof(FWENTRY));

	return IF4ERR_NOERROR;
}

void AddSensorFFDToResult(IF4Wrapper *pWrapper, ENDIENTRY *pEndDevice, FFDENTRY *pEntry)
{
    if (pEndDevice == NULL)
		return;

	memset(pEntry, 0, sizeof(FFDENTRY));
	memcpy(&pEntry->ffdID, &pEndDevice->id, sizeof(EUI64));
	strncpy(pEntry->ffdModel, pEndDevice->szModel, 18);
	GetTimestamp(&pEntry->ffdLastPoll, &pEndDevice->tmMetering);
	GetTimestamp(&pEntry->ffdLastPush, &pEndDevice->tmRouteDevice);
	pEntry->ffdHwVersion = pEndDevice->hw;
	pEntry->ffdFwVersion = pEndDevice->fw;
	pEntry->ffdFwBuild = pEndDevice->build;
    pEntry->ffdPowerType = pEndDevice->nAttribute & ENDDEVICE_ATTR_POWERNODE ? 0x01 : (
                pEndDevice->nAttribute & ENDDEVICE_ATTR_SOLARNODE ? 0x04 : (
                    pEndDevice->nAttribute & ENDDEVICE_ATTR_BATTERYNODE ? 0x02 : 0x00
                )
            );
    pEntry->ffdPermitMode = (BYTE) pEndDevice->bPermitMode;
    pEntry->ffdPermitState =  pEndDevice->bPermitState ? 0xff : 0x00;
    pEntry->ffdMaxChildTbl =  pEndDevice->nMaxChildTbl;
    pEntry->ffdCurrentChild =  pEndDevice->nChildTbl;
    pEntry->ffdAlarmMask =  pEndDevice->nAlarmMask;
    pEntry->ffdAlarmFlag =  pEndDevice->nAlarmFlag < 0 ? 0 : pEndDevice->nAlarmFlag;
    pEntry->ffdTestFlag =  pEndDevice->bTestFlag;
    pEntry->ffdSolarAdVolt =  pEndDevice->nSolarAdVolt;
    pEntry->ffdSolarChgBattVolt =  pEndDevice->nSolarChgBattVolt;
    pEntry->ffdSolarBoardVolt =  pEndDevice->nSolarBoardVolt;
    pEntry->ffdSolarBattChgState =  pEndDevice->nSolarChgState;

	IF4API_AddResultFormat(pWrapper, "5.1", VARSMI_OPAQUE, pEntry, sizeof(FFDENTRY));
}

BOOL EnumFFDCallback(ENDIENTRY *pEndDevice, void *pWrapper, void *pszBuffer, void *filter)
{
	FFDENTRY	*pEntry = (FFDENTRY *)pszBuffer;
    ParserVersionBuild * pvb = (ParserVersionBuild *) filter;

	if (pEntry == NULL)
		return FALSE;

    if( !(pEndDevice->nAttribute & ENDDEVICE_ATTR_RFD) 
        && (!pvb->bParser || (
        (pvb->bAll || (
            !strncasecmp(pvb->parser,pEndDevice->szParser,strlen(pvb->parser))
        )) && (
            !pvb->version || (pvb->version == pEndDevice->fw) 
        )  && (
            !pvb->build || (pvb->build == pEndDevice->build) 
        ))
    )) {
	    AddSensorFFDToResult((IF4Wrapper*)pWrapper, pEndDevice, pEntry);
	    return TRUE;
    }

	return TRUE;
}

int cmdGetFFDList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    ParserVersionBuild pvb;
	char			*pszBuffer;
	int				nCount;

    memset(&pvb,0,sizeof(pvb));
    if(cnt > 0) {
        memcpy(pvb.parser,pValue[0].stream.p, pValue[0].len > (int) sizeof(pvb.parser) - 1 ? (int) sizeof(pvb.parser) - 1 : pValue[0].len);
        if(!strcasecmp(pvb.parser, "all")) pvb.bAll = TRUE;
        pvb.bParser = TRUE;
    }
    if(cnt > 1) {
        pvb.version = pValue[1].stream.u16;
    }
    if(cnt > 2) {
        pvb.build = pValue[2].stream.u16;
    }

	pszBuffer = (char *)MALLOC(sizeof(FFDENTRY));
	if (pszBuffer == NULL)
		return IF4ERR_MEMORY_ERROR;

    nCount = m_pEndDeviceList->EnumEndDevice(EnumFFDCallback, (void *)pWrapper, pszBuffer, &pvb);
	FREE(pszBuffer);

	return (nCount <= 0) ? IF4ERR_NO_ENTRY : IF4ERR_NOERROR;
}

/** 비동기 방식의 Call을 수행한다.
 *
 * @param miuID         EUI64ID(1.14) : 대상이 되는 MIU ID
 *          - ID가 EndDeviceList에 없을 경우 INVALID_ID Error
 *          - ID가 BatteryNode 또는 SolarNode가 아닐 경우 INVALID_TARGET Error
 * @param commandOID    OID(1.10) : Async Call의 대상이 되는 Command OID
            - MIU와 직접 통신을 수행하는 명령이 아닐 경우 INVALID_COMMAND Error
 * @param nOption       BYTE(1.4) : Async Call Option
 *          - 0x01 : ASYNC_OPT_RETURN_CODE_EVT (결과 성공/실패 여부만 Event로 전송)
 *          - 0x02 : ASYNC_OPT_RESULT_DATA_EVT (전체 Return Value를 Event로 전송)
 *          - 0x10 : ASYNC_OPT_RETURN_CODE_SAVE (성공/실패 결과만 보관)
 *          - 0x20 : ASYNC_OPT_RESULT_DATA_SAVE (전체 Return Value 보관)
 *
 * @param nDays         BYTE(1.4) : Save Option 사용 시 보관 일 수(1 ~ 255). 만약 0이면 집중기 기본 설정값(정의 필요) 사용
 * @param nNice         char(1.7) : Request 우선 순위 (-2 ~ 3. Default 0)
 * @param nTry          BYTE(1.4) : 시도 횟수 (기본 1 : 최대 3). 시도를 할 때 마다 Nice 값이 증가한다.
 * @param ....          commandOID에 필요한 Parameter들 
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *          - IF4ERR_INVALID_TARGET
 *          - IF4ERR_INVALID_COMMAND
 *          - IF4ERR_OUT_OF_RANGE
 *          - IF4ERR_TRANSACTION_CREATE_FAIL
 * @return tranactionID UINT(1.6) : error 발생시는 0
 *
 */
int cmdAsynchronousCall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError = IF4ERR_NOERROR;
    WORD        nTrID=0;
    EUI64       id;
    ENDIENTRY   *endi;

	if (cnt < 6) return IF4ERR_INVALID_PARAM;

	if ((VARAPI_OidCompare(&pValue[0].oid, "1.14") != 0)) return IF4ERR_INVALID_PARAM;
	if ((VARAPI_OidCompare(&pValue[1].oid, "1.10") != 0)) return IF4ERR_INVALID_PARAM;
	if ((VARAPI_OidCompare(&pValue[2].oid, "1.4") != 0)) return IF4ERR_INVALID_PARAM;
	if ((VARAPI_OidCompare(&pValue[3].oid, "1.4") != 0)) return IF4ERR_INVALID_PARAM;
	if ((VARAPI_OidCompare(&pValue[4].oid, "1.7") != 0)) return IF4ERR_INVALID_PARAM;
	if ((VARAPI_OidCompare(&pValue[5].oid, "1.4") != 0)) return IF4ERR_INVALID_PARAM;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));

    /*-- nNice, nTry 값을 검사해서 유효값 범위내에 있는지 확인 --*/
    if (pValue[4].stream.s8  < -2 || pValue[4].stream.s8 > 3) return IF4ERR_INVALID_PARAM;
    if (pValue[5].stream.u8  > 3) return IF4ERR_INVALID_PARAM;

    /*-- 대상 장비가 없을 때 : Invalid ID --*/
    if(!(endi = m_pEndDeviceList->GetEndDeviceByID(&id))) {
        return IF4ERR_INVALID_ID;
    }

    /*-- 장비가 BatteryNode 또는 SolarNode가 아닐 경우 : Invalid Target
     *
    if(!(endi->nAttribute & ENDDEVICE_ATTR_BATTERYNODE || 
            endi->nAttribute & ENDDEVICE_ATTR_SOLARNODE)) {
        return IF4ERR_INVALID_TARGET;
    }
     */

    if(!m_pTransactionManager->Add(pValue, cnt, &nTrID)) {
        nError = IF4ERR_TRANSACTION_CREATE_FAIL;
    }

	IF4API_AddResultNumber(pWrapper, "1.5", VARSMI_WORD, nTrID);

	return nError;
}

/*////////////////////////////////////////////////////////////////////////
//  [10/13/2010 DHKim]
//  Group AsyncCall			(102.53)
 @param Group Name     STRING(1.11) : 대상이 되는 MIU ID
 @param commandOID    OID(1.10) : Async Call의 대상이 되는 Command OID
 @param nOption       BYTE(1.4) : Async Call Option
          - 0x01 : ASYNC_OPT_RETURN_CODE_EVT (결과 성공/실패 여부만 Event로 전송)
          - 0x02 : ASYNC_OPT_RESULT_DATA_EVT (전체 Return Value를 Event로 전송)
          - 0x10 : ASYNC_OPT_RETURN_CODE_SAVE (성공/실패 결과만 보관)
          - 0x20 : ASYNC_OPT_RESULT_DATA_SAVE (전체 Return Value 보관)
 @param nDays         BYTE(1.4) : Save Option 사용 시 보관 일 수(1 ~ 255). 만약 0이면 집중기 기본 설정값(정의 필요) 사용
 @param nNice         char(1.7) : Request 우선 순위 (-2 ~ 3. Default 0)
 @param nTry          BYTE(1.4) : 시도 횟수 (기본 1 : 최대 3). 시도를 할 때 마다 Nice 값이 증가한다.
 @param ....          commandOID에 필요한 Parameter들 
 
 * DB는 자동적으로 포인트를 반환하지 않으므로 Free가 필요함.
 * DB Close가 안 될경우 DB접근이 불가능하는 현상이 있으므로 주의!!
////////////////////////////////////////////////////////////////////////*/
#ifdef __GROUP_SUPPORT_MODE__
int cmdGroupAsyncCall(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/HEMSGroup.db";
	//char szGroupNames[255]= {0, };
	OID3	cmd;
	char szCmd[12]={0,};
	char szID[20]={0,};
	WORD nTrID = 0;
	EUI64 eEUI64;
	int nGroupKey = -1;
	BOOL bFindCommand = FALSE;

	MIBValue		*pHead;
	MIBValue		*pLast;
	MIBValue		*pNewValue = NULL;

	if( cnt < 6 || (VARAPI_OidCompare(&pValue[0].oid, "1.9") != 0) || (VARAPI_OidCompare(&pValue[1].oid, "1.10") != 0))
		return IF4ERR_INVALID_PARAM;			//GroupKey: INT(1.9)	//Command ID: OID(1.10)

	if ((VARAPI_OidCompare(&pValue[2].oid, "1.4") != 0) /*Option*/ || (VARAPI_OidCompare(&pValue[3].oid, "1.4") != 0) /*nDays(보관 일수)*/
		|| (VARAPI_OidCompare(&pValue[4].oid, "1.7") != 0) /*우선 순위*/ || (VARAPI_OidCompare(&pValue[5].oid, "1.4") != 0) /*시도 횟수*/)
		return IF4ERR_INVALID_PARAM;

	nGroupKey = pValue[0].stream.s32;
	memcpy( &cmd, &pValue[1].stream.id, sizeof(OID3) );

	VARAPI_OidToString(&cmd, szCmd);
	XDEBUG("cmd: %s\r\n", szCmd);

	// 사용 가능 명령어 확인
	for(int i=0; m_GroupCommandHandler[i].pszName; i++)
		if (memcmp(&m_GroupCommandHandler[i].oid, &cmd, 3) == 0)
			bFindCommand = TRUE;
	if( !bFindCommand )
		return IF4ERR_INVALID_COMMAND;

	// 그룹에 해당하는 멤버를 찾음
	if ( (m_pDbGroup->db_Table_Check(dbfile)) == SQLITE_OK )
	{
		m_pDbGroup->bufMember.clear();
		if( (m_pDbGroup->GroupCommand(dbfile, nGroupKey, m_pDbGroup->bufMember, pValue, cnt)) == SQLITE_OK )
		{
			XDEBUG("bufMember Size: %d\r\n", m_pDbGroup->bufMember.size());
			for(unsigned int i=0; i< m_pDbGroup->bufMember.size(); i++)
			{
				//XDEBUG("Member ID: %s\r\n", m_pDbGroup->bufMember[i]);
				/* 장치 목록이 실제로 존재하는지 확인
				if(!(endi = m_pEndDeviceList->GetEndDeviceByID(&(pMember[i].pID))))
					bNoMember = TRUE;
				*/
				StrToEUI64(m_pDbGroup->bufMember[i], &eEUI64);
				XDEBUG("EUI63ID: %s\r\n", m_pDbGroup->bufMember[i]);
				memcpy(szID, m_pDbGroup->bufMember[i], strlen(szID));

				pHead = IF4API_NewMIBValue("1.14", VARSMI_EUI64, 0, (char *)&eEUI64, sizeof(EUI64));
				pLast = pHead;
				if(pLast != NULL)
				{
					pLast->pNext = IF4API_NewMIBValue("1.10", VARSMI_OID, 0, (char*)&pValue[1].stream.id, sizeof(OID3));
					pLast = pLast->pNext;

					for (int j=2; j < cnt; j++)
					{
						if( VARAPI_OidCompare( &(pValue[j].oid), "1.3") == 0 )
							pLast->pNext = IF4API_NewMIBValue("1.3", VARSMI_BOOL, pValue[j].stream.u8, NULL, sizeof(BOOL));
						else if( VARAPI_OidCompare( &(pValue[j].oid), "1.4") == 0)
							pLast->pNext = IF4API_NewMIBValue("1.4", VARSMI_BYTE, pValue[j].stream.u8, NULL, sizeof(BYTE));
						else if ( VARAPI_OidCompare( &(pValue[j].oid), "1.5") == 0 )
							pLast->pNext = IF4API_NewMIBValue("1.5", VARSMI_WORD, pValue[j].stream.u16, NULL, sizeof(WORD));
						else if ( VARAPI_OidCompare( &(pValue[j].oid), "1.6") == 0 )
							pLast->pNext = IF4API_NewMIBValue("1.6", VARSMI_UINT, pValue[j].stream.u32, NULL, sizeof(UINT));
						else if ( VARAPI_OidCompare( &(pValue[j].oid), "1.7") == 0 )
							pLast->pNext = IF4API_NewMIBValue("1.7", VARSMI_CHAR, pValue[j].stream.s8, NULL, sizeof(char));
						else if ( VARAPI_OidCompare( &(pValue[j].oid), "1.8") == 0 )
							pLast->pNext = IF4API_NewMIBValue("1.8", VARSMI_SHORT, pValue[j].stream.s16, NULL, sizeof(WORD));
						else if ( VARAPI_OidCompare( &(pValue[j].oid), "1.9") == 0 )
							pLast->pNext = IF4API_NewMIBValue("1.9", VARSMI_INT,  pValue[j].stream.s32, NULL, sizeof(INT));
						else
							pLast->pNext = IF4API_NewMIBValue("1.11", VARSMI_STRING, 0,  pValue[j].stream.p, strlen(pValue[j].stream.p));

						pLast = pLast->pNext;
					}
					pLast->pNext = NULL;
					pNewValue = pHead;
				}
				if (!m_pTransactionManager->Add(pNewValue, cnt, &nTrID))
				{
					return IF4ERR_TRANSACTION_CREATE_FAIL;
				}
				IF4API_AddResultFormat(pWrapper,"4.1.1", VARSMI_EUI64, &eEUI64, sizeof(EUI64));
				IF4API_AddResultNumber(pWrapper, "1.5", VARSMI_WORD, nTrID);

			}
			XDEBUG("Success: GroupCommand!!\r\n");
		}
		else
			XDEBUG("Failure: Groupcommand!!\r\n");
	}
	
	return IF4ERR_NOERROR;

}
//////////////////////////////////////////////////////////////////////////
//  [10/22/2010 DHKim]
//  Group Add CommandHandler(102.54) 
//////////////////////////////////////////////////////////////////////////
int cmdGroupAdd(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/HEMSGroup.db";
	char szGroupNames[255]= {0, };
	int nGroupKey = -1;

	memcpy(szGroupNames, pValue[0].stream.p, pValue[0].len);

	if ( (m_pDbGroup->db_Table_Check(dbfile)) == SQLITE_OK )
	{
		nGroupKey = m_pDbGroup->GroupAdd(dbfile, szGroupNames);
		if(nGroupKey != -1)
		{
			XDEBUG("Success - GroupKey: %d, GroupName: %s\r\n", nGroupKey, szGroupNames);
			IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, nGroupKey);
		}
		else
			XDEBUG("Failure - Group Add!!\r\n");
	}

	XDEBUG("Return GroupKey : %d\r\n", nGroupKey);

	return IF4ERR_NOERROR;
}
//////////////////////////////////////////////////////////////////////////
//  [10/22/2010 DHKim]
//  Group Delete CommandHandler(102.55)
//////////////////////////////////////////////////////////////////////////
int cmdGroupDelete(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/HEMSGroup.db";
	int nGroupKey = pValue[0].stream.s32;

	if ( (m_pDbGroup->db_Table_Check(dbfile)) == SQLITE_OK )
	{
		if( m_pDbGroup->GroupDelete(dbfile, nGroupKey) == SQLITE_OK)
			XDEBUG("Success - GroupKey: %d Deleted\r\n", nGroupKey);
		else
			XDEBUG("Failure - Group Add!!\r\n");
	}

	XDEBUG("Return GroupKey : %d\r\n", nGroupKey);

	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////////
//  [10/18/2010 DHKim]
//  GROUP ADD MEMBER		(102.56)
//////////////////////////////////////////////////////////////////////////
int cmdGroupAddMember(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{

	const char* dbfile = "/app/sqlite/HEMSGroup.db";

	int nGroupKey = -1;
	EUI64	id;

	char szMemberId[8] = {0, };
	nGroupKey = pValue[0].stream.s32;
	memcpy(&id, pValue[1].stream.p, sizeof(EUI64));
	EUI64ToStr(&id, szMemberId);
	XDEBUG("GroupKey: %d, ID: %s, size: %d\r\n", nGroupKey, szMemberId, sizeof(szMemberId));

	if ( (m_pDbGroup->db_Table_Check(dbfile)) == SQLITE_OK )
	{
		if( m_pDbGroup->GroupAddMember(dbfile, nGroupKey, szMemberId) == SQLITE_OK )
			XDEBUG("Success: Group Member Add!!\r\n");
		else
			XDEBUG("Failure: Group Member Add!!\r\n");
	}

	XDEBUG("Return GroupKey : %d\r\n", nGroupKey);

	return IF4ERR_NOERROR;

}

//////////////////////////////////////////////////////////////////////////
//  [10/19/2010 DHKim]
//  Group Delete Member			(102.57)
//////////////////////////////////////////////////////////////////////////
int cmdGroupDeleteMember(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/HEMSGroup.db";

	int nGroupKey = -1;
	EUI64	id;

	char szMemberId[8] = {0, };
	nGroupKey = pValue[0].stream.s32;
	memcpy(&id, pValue[1].stream.p, sizeof(EUI64));
	EUI64ToStr(&id, szMemberId);
	XDEBUG("GroupKey: %d, ID: %s, size: %d\r\n", nGroupKey, szMemberId, sizeof(szMemberId));

	if ( (m_pDbGroup->db_Table_Check(dbfile)) == SQLITE_OK )
	{
		if( m_pDbGroup->GroupDeleteMember(dbfile, nGroupKey, szMemberId) == SQLITE_OK )
			XDEBUG("Success: Group Member Delete!!\r\n");
		else
			XDEBUG("Failure: Group Member Delete!!\r\n");
	}

	XDEBUG("Return GroupKey : %d\r\n", nGroupKey);

	return IF4ERR_NOERROR;

}

//////////////////////////////////////////////////////////////////////////
//  [10/22/2010 DHKim]
//  Group Information (102.58)
//////////////////////////////////////////////////////////////////////////
int cmdGroupInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/HEMSGroup.db";
	BOOL	bMemberSendDone = FALSE;
    int     nTempCnt = 0, nTempMemCnt = 0;

	m_pDbGroup->bufGroup = (GROUPINFO *)MALLOC(sizeof(GROUPINFO));

	if( m_pDbGroup->db_Table_Check(dbfile) == SQLITE_OK )
	{
		if (m_pDbGroup->GroupInfo(dbfile, m_pDbGroup->bufGroup) == SQLITE_OK)
			XDEBUG("Success: Group Info\r\n");
		else
			XDEBUG("Failure: Group Info\r\n");

		XDEBUG("Group.nTotalCnt: %d\r\n", m_pDbGroup->bufGroup->nTotalCnt );
		for(int i=0; i < m_pDbGroup->bufGroup->nTotalCnt; i++)
		{
			XDEBUG("Group.nMemberCnt: %d\r\n", m_pDbGroup->bufGroup->nMemberCnt[i] );
			if(!bMemberSendDone)
			{
				IF4API_AddResultNumber(pWrapper, "7.1.1", VARSMI_INT, m_pDbGroup->bufGroup->nTotalGroupCnt);
				IF4API_AddResultNumber(pWrapper, "7.1.2", VARSMI_INT, m_pDbGroup->bufGroup->nGroupKey[i]);
				IF4API_AddResultFormat(pWrapper, "7.1.3", VARSMI_STRING, &(m_pDbGroup->bufGroup->pGroupName[i*64]), 
                        strlen(&(m_pDbGroup->bufGroup->pGroupName[i*64])));
				IF4API_AddResultNumber(pWrapper, "7.1.4", VARSMI_INT, m_pDbGroup->bufGroup->nMemberCnt[i]);
				if( m_pDbGroup->bufGroup->nMemberCnt[i] != 0 )
					bMemberSendDone = TRUE;
                nTempCnt = m_pDbGroup->bufGroup->nMemberCnt[i] + nTempCnt;
			}
			if(m_pDbGroup->bufGroup->nMemberCnt[i] > 0 && i == nTempCnt-1)
			{
				IF4API_AddResultFormat(pWrapper, "7.1.5", VARSMI_STREAM, &(m_pDbGroup->bufGroup->pMemberID[nTempMemCnt]), 
                        8*m_pDbGroup->bufGroup->nMemberCnt[i]);
				bMemberSendDone = FALSE;
                nTempMemCnt += m_pDbGroup->bufGroup->nMemberCnt[i];
			}
		}
	}

    if(m_pDbGroup->bufGroup != NULL)	FREE(m_pDbGroup->bufGroup);

	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////////
//  [11/19/2010 DHKim]
//  Command Table 정보 조회
//////////////////////////////////////////////////////////////////////////
int cmdGroupShowCommand(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/HEMSGroup.db";
	m_pDbGroup->bufGroupCommand = (GROUPCOMMANDINFO *)MALLOC(sizeof(GROUPCOMMANDINFO));

	int nGroupKey = -1;

	if(0 < cnt)
		nGroupKey = pValue[0].stream.s32;

	if( m_pDbGroup->db_Table_Check(dbfile) == SQLITE_OK )
	{
		if (m_pDbGroup->GroupShowCommand(dbfile, nGroupKey, m_pDbGroup->bufGroupCommand) == SQLITE_OK)
			XDEBUG("Success: Group Show Command\r\n");
		else
			XDEBUG("Failure: Group Show Command\r\n");
	}

	FREE(m_pDbGroup->bufGroupCommand);

	return IF4ERR_NOERROR;
}

int cmdGroupTableDelete(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/HEMSGroup.db";

	char sztablekind[20] = {0, };

	memcpy(sztablekind, pValue[0].stream.p, sizeof(sztablekind));

	if( m_pDbGroup->db_Table_Check(dbfile) == SQLITE_OK )
	{
		if ( m_pDbGroup->GroupTableDelete(dbfile, sztablekind) == SQLITE_OK )
		{
			XDEBUG("Success: Delete Table Command Table Delete!!\r\n");
		}
		else
		{
			XDEBUG("Failure: Delete Table Command Table Delete!!\n");
		}
	}

	return IF4ERR_NOERROR;
}


int cmdJointableShow(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/EndiJoinTable.db";
	
	if ( m_pDbGroup->JoinTableShow(dbfile) == SQLITE_OK )
	{
		XDEBUG("Success: Join Table Show List!!\r\n");
	}
	else
	{
		XDEBUG("Failure: Join Table Show List!!\n");
	}

	return IF4ERR_NOERROR;

}

int cmdJointableAdd(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/EndiJoinTable.db";
	char szID[17] = {0, };

	for(int i=0; i < cnt; i++)
	{
		memcpy(szID, pValue[i].stream.p, sizeof(szID));

		if ( m_pDbGroup->JoinTableAdd(dbfile, szID) == SQLITE_OK )
		{
			XDEBUG("Success: Join Table Add!!\r\n");
		}
		else
		{
			XDEBUG("Failure: Join Table Add!!\n");
		}
	}

	return IF4ERR_NOERROR;
}

int cmdJointableDelete(IF4Wrapper *pWrapper,  MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/EndiJoinTable.db";
	char szID[17] = {0, };

	if( cnt == 0 )
	{
		if ( m_pDbGroup->JoinTableDelete(dbfile) == SQLITE_OK )
		{
			XDEBUG("Success: Join Table Add!!\r\n");
		}
		else
		{
			XDEBUG("Failure: Join Table Add!!\n");
		}		
	}

	for(int i=0; i < cnt; i++)
	{
		memcpy(szID, pValue[i].stream.p, sizeof(szID));

		if ( m_pDbGroup->JoinTableDelete(dbfile, szID) == SQLITE_OK )
		{
			XDEBUG("Success: Join Table Add!!\r\n");
		}
		else
		{
			XDEBUG("Failure: Join Table Add!!\n");
		}
	}
	return IF4ERR_NOERROR;
}

int cmdJointableAddFile(IF4Wrapper *pWrapper,  MIBValue *pValue, int cnt)
{
	const char* dbfile = "/app/sqlite/EndiJoinTable.db";

	char szfilename[128] = {0, };

	sprintf(szfilename, "/app/sw/%s", pValue[0].stream.p );
	XDEBUG("Filename: %s\r\n", szfilename);

	if( m_pDbGroup->JoinTableFileAdd(dbfile, szfilename) == SQLITE_OK)
		XDEBUG("Success: Join Table Add from File!!\r\n");
	else
		XDEBUG("Failure: Join Table Add from File!!\r\n");

	return IF4ERR_NOERROR;
}

#endif

BOOL EnumTransactionCallback(ENDIENTRY *pEndDevice, void *pWrapper, void *pszBuffer, void *filter)
{
    ParserVersionBuild * pvb = (ParserVersionBuild *) filter;

	if (pEndDevice == NULL)
		return FALSE;

    if( !pvb->bParser || ((pvb->bAll || 
            !strncasecmp(pvb->parser,pEndDevice->szParser,strlen(pvb->parser))
        ) && (
            !pvb->version || (pvb->version == pEndDevice->fw) 
        )  && (
            !pvb->build || (pvb->build == pEndDevice->build) 
        ))
    ) {
        int i, nCount=0;
        TRINFOENTRY * trInfo = NULL;

        if(m_pTransactionManager->LoadTransactionInfo(&pEndDevice->id, pvb->nOption, &trInfo, &nCount)) {
            for(i=0;i<nCount;i++) {
                IF4API_AddResultFormat((IF4Wrapper *)pWrapper, "6.1", VARSMI_OPAQUE, &trInfo[i], sizeof(TRINFOENTRY));
            }
        }
        if(trInfo) FREE(trInfo);
    }

	return TRUE;
}

/** Transaction List 조회.
 *
 * @param nOption       BYTE(1.4) : Show Option
 *          - 0x01 : TR_SHOW_WAITING_TRANSACTION (작업 대기중인 Transaction)
 *          - 0x02 : TR_SHOW_QUEUEING_TRANSACTION (현재 Queue에 대기중인 Transaction)
 *          - 0x04 : TR_SHOW_RUNNING_TRANSACTION (현재 Running 중인 Transaction)
 *          - 0x08 : TR_SHOW_TERMINATE_TRANSACTION (완료된 Transaction)
 *
 * @param miuID         EUI64ID(1.14) : 대상이 되는 MIU ID
 * @param all           STRING(1.11) : 전체
 *
 *     miuID/all 은 둘 중 하나만 첫번째 인자로 올 수 있다.
 *
 * @param version       WORD(1.5) 
 * @param build         WORD(1.5)
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *
 * @return trInfoList TrInfoEntry(6.1)  
 *
 */
int cmdShowTransactionList(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    ParserVersionBuild pvb;
    EUI64       id;
    int         nError = IF4ERR_NOERROR;
    BYTE        nOption = TR_SHOW_WAITING_TRANSACTION | TR_SHOW_QUEUEING_TRANSACTION | TR_SHOW_RUNNING_TRANSACTION;
	int			nCount=0;

    if(cnt > 0) {
        nOption = pValue[0].stream.u8;
    }

    memset(&pvb,0,sizeof(pvb));
    pvb.bAll = TRUE;
    if(cnt > 1) {
	    if (!IsValidEUI64(&pValue[0].oid)) {

            if(VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0 ) {
		        return IF4ERR_INVALID_ID;
            } else  {
                memcpy(pvb.parser,pValue[1].stream.p, pValue[1].len > (int) sizeof(pvb.parser) - 1 ? (int) sizeof(pvb.parser) - 1 : pValue[1].len);
                if(strcasecmp(pvb.parser, "all")) pvb.bAll = FALSE;
                pvb.bParser = TRUE;
            }
        } else {
	        memcpy(&id, pValue[1].stream.p, sizeof(EUI64));
            pvb.bAll = FALSE;
        }
    }

    if(cnt > 2) {
        pvb.version = pValue[2].stream.u16;
    }
    if(cnt > 3) {
        pvb.build = pValue[3].stream.u16;
    }

    if(pvb.bAll) {
        pvb.nOption = nOption;

        nCount = m_pEndDeviceList->EnumEndDevice(EnumTransactionCallback, (void *)pWrapper, NULL, &pvb);
	    nError = (nCount <= 0) ? IF4ERR_NO_ENTRY : IF4ERR_NOERROR;
    }else {
        int i;
        TRINFOENTRY * trInfo = NULL;

        if(m_pTransactionManager->LoadTransactionInfo(&id, nOption, &trInfo, &nCount)) {
            for(i=0;i<nCount;i++) {
                IF4API_AddResultFormat(pWrapper, "6.1", VARSMI_OPAQUE, &trInfo[i], sizeof(TRINFOENTRY));
            }
        }
        if(trInfo) FREE(trInfo);
	    nError = (nCount <= 0) ? IF4ERR_NO_ENTRY : IF4ERR_NOERROR;
    }
    return nError;
}

/** Transaction 정보 조회.
 *
 * @param trID       WORD(1.5) : Transaction ID
 * @param nOption    BYTE(1.4) : 조회 Option
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *          - IF4ERR_NO_ENTRY
 *
 * @return trInfoList TrInfoEntry(6.1)  
 * @return nParamCnt MIBValue Count(1.5)  
 * @return params nMibCnt 수 만큼의 MIBValue가 반복된다. (1.12)
 * @return nResultCnt MIBValue Count(1.5)  
 * @return results nMibCnt 수 만큼의 MIBValue가 반복된다. (1.12)
 * @return nHistCnt TRHISTORYENTRY Count(1.5)  
 * @return trHistories nHistCnt 수 만큼의 TRHISTORYENTRY가 반복된다. (6.2)
 *
 */
int cmdShowTransactionInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    TRINFOENTRY     trInfo;
    MIBValue        *params=NULL, *results=NULL;
    TRHISTORYENTRY  *trHistory=NULL;
    WORD            trId;
    int             nParamCnt=0, nResCnt=0, nHistCnt=0;
    int             nError = IF4ERR_NO_ENTRY;
    int             i;
    BYTE            nOption = TR_GET_OPT_ALL;

    if(cnt < 1) return IF4ERR_INVALID_PARAM;

    trId = pValue[0].stream.u16;

    if(cnt > 1) {
        nOption = pValue[1].stream.u8;
    }

    memset(&trInfo,0,sizeof(TRINFOENTRY));

    if(m_pTransactionManager->LoadTransactionInfo(trId, nOption, &trInfo, &params, &nParamCnt,
                &results, &nResCnt,
                &trHistory, &nHistCnt)) {

        IF4API_AddResultFormat(pWrapper, "6.1", VARSMI_OPAQUE, &trInfo, sizeof(TRINFOENTRY));
	    IF4API_AddResultNumber(pWrapper, "1.5", VARSMI_WORD, nParamCnt);
        for(i=0;i<nParamCnt && params;i++, params=params->pNext) {
            IF4API_AddResultMIBValue(pWrapper, params);
        }
	    IF4API_AddResultNumber(pWrapper, "1.5", VARSMI_WORD, nResCnt);
        for(i=0;i<nResCnt && results;i++, results=results->pNext) {
            IF4API_AddResultMIBValue(pWrapper, results);
        }
	    IF4API_AddResultNumber(pWrapper, "1.5", VARSMI_WORD, nHistCnt);
        for(i=0;i<nHistCnt;i++) {
            IF4API_AddResultFormat(pWrapper, "6.2", VARSMI_OPAQUE, &trHistory[i], sizeof(TRHISTORYENTRY));
        }

        nError = IF4ERR_NOERROR;
    }
    IF4API_FreeMIBValue(params);
    IF4API_FreeMIBValue(results);
    FREE(trHistory);
    
    return nError;
}

/** Transaction 상태를 변경한다.
 *
 * @param trID       WORD(1.5) : Transaction ID
 * @param nState     BYTE(1.4) : 변경 시킬 상태
 *        + TR_STATE_TERMINATE  0x08
 *        + TR_STATE_DELETE     0x10
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *          - IF4ERR_OUT_OF_RANGE
 *          - IF4ERR_INVALID_TRANSACTION_ID
 *          - IF4ERR_TRANSACTION_UPDATE_FAIL
 *
 */
int cmdModifyTransaction(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    WORD            trId;
    int             i, nError = IF4ERR_NO_ENTRY;
    BYTE            nState;

    if(cnt < 2) return IF4ERR_INVALID_PARAM;
    if(cnt % 2) return IF4ERR_INVALID_PARAM;

    for(i=0;i<cnt;i+=2) {
        trId = pValue[i].stream.u16;
        nState = pValue[i+1].stream.u8;

        if(nState != TR_STATE_TERMINATE && nState != TR_STATE_DELETE)  {
            nError = IF4ERR_OUT_OF_RANGE;
            break;
        }

		if (nState == TR_STATE_TERMINATE || nState == TR_STATE_TERMINATE)
		{
			nError = IF4ERR_TRANSACTION_UPDATE_FAIL;
			XDEBUG("Update GDBM Operation State Error!!\r\n");
		}

        nError = m_pTransactionManager->UpdateTransaction(trId, nState);
        if(nError != IF4ERR_NOERROR) break;
    }
    
    return nError;
}
int cmdDeleteMeter(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{


    EUI64       *id;
    BYTE        port;
    BOOL        bSuccess=TRUE;
    XDEBUG("CNT : %d\xd\xa", cnt);
	if (!IsValidEUI64(&pValue[0].oid))
        return IF4ERR_INVALID_ID;
    
    id = (EUI64 *)pValue[0].stream.p;
    port=pValue[1].stream.u8;
    bSuccess=m_pMeterDeviceList->Remove(id, port);
    if(!bSuccess) return IF4ERR_INVALID_ID;
    return IF4ERR_NOERROR;
}

/** Sensor의 Digital Port 값을 In/Out 한다.
 *
 * @param id         EUI64(1.14) : Sensor ID
 * @param direction  BYTE(1.4) : out(0), in(1)
 * @param mask       BYTE(1.4) : Port Mask
 * @param value      BYTE(1.4) : Digital Value
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdDigitalInOut(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 	id;
    int 	nError = IF4ERR_NOERROR;
    ENDI_DIO_PAYLOAD   *payload;
    BYTE        szBuffer[100];
    BYTE        szResBuffer[100];
    char szId[32]={0,};
    CMeterParser * pParser = NULL;
    ENDIENTRY *pEndi;
    int     nResLen=0;

    if(cnt < 4)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

    if((VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0) ||
       (VARAPI_OidCompare(&pValue[2].oid, "1.4") != 0) ||
       (VARAPI_OidCompare(&pValue[3].oid, "1.4") != 0))
        return IF4ERR_INVALID_PARAM;

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

    memset(szBuffer, 0, sizeof(szBuffer));
    payload = (ENDI_DIO_PAYLOAD *) &szBuffer;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
    payload->type = pValue[1].stream.u8;
    payload->mask = pValue[2].stream.u8;
    payload->value = pValue[3].stream.u8;

	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);
    if(pEndi == NULL) 
    {
        return IF4ERR_INVALID_ID;
    }

    pParser = m_pMeterParserFactory->SelectParser(pEndi->szParser);

	if((nError = m_pOndemander->EndiSend((HANDLE)GetCoordinator(), &id, CODI_ACCESS_WRITE, ENDI_DATATYPE_DIO, 
                    szBuffer, sizeof(ENDI_DIO_PAYLOAD), szResBuffer, &nResLen, 
                    pParser->GetConnectionTimeout(pEndi->nAttribute))) == IF4ERR_NOERROR) 
    {
	    IF4API_AddResultOpaque(pWrapper, "1.12", szResBuffer, nResLen);
    }
    else 
    {
        eui64toa(&id, szId);
        XDEBUG("DigitalInOut Fail: id=%s, %s\r\n", szId, IF4API_GetErrorMessage(nError));
    } 

    return nError;
}

/** Sensor의 상수 값을 조회한다.
 *
 * @param id         EUI64(1.14) : Sensor ID
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdGetSensorConstants(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 	id;
    int 	nError = IF4ERR_NOERROR;
    ENDIENTRY *pEndi;
    SENSORCONSTANTENTRY con;
    double fV, fC;

    if(cnt < 1)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

    memset(&con, 0, sizeof(SENSORCONSTANTENTRY));
	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));

	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);
    if(pEndi == NULL) 
    {
        return IF4ERR_INVALID_ID;
    }

    fV = pEndi->fVoltageScale;
    fC = pEndi->fCurrentScale;

    con.powerVoltageScaleInt = (INT)fV;
    con.powerVoltageScaleReal = (UINT)((fV - con.powerVoltageScaleInt) * 1000000);
    con.powerCurrentScaleInt = (INT)fC;
    con.powerCurrentScaleReal = (UINT)((fC - con.powerCurrentScaleInt) * 1000000);

	IF4API_AddResultOpaque(pWrapper, "1.12", &con, sizeof(SENSORCONSTANTENTRY));

    return nError;
}

/** Sensor에 임의의 데이터를 전송한다.
 *
 * @param id         EUI64(1.14) : Sensor ID
 * @param isLinkSkip BOOL(1.3) : Link Frame 전송 여부
 * @param data       Stream(1.12) : Data stream
 *
 *
 * Issue #1667 http://tauran.nuritelecom.com/rep0/mcu/branches/issue/1667
 *  - Issue #1667에서 구현되었던 기능을 merge 한다.
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdBypassSensor(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64 id;
	ENDIENTRY	*pEndi;
    //BOOL    bSkipLink = FALSE;
    int nError=IF4ERR_NOERROR;
	char	szGUID[17];

    if(cnt < 2)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);

	if (pEndi == NULL)
	{
	    eui64toa(&id, szGUID);
		XDEBUG("cmdBypassSensor: Invalid EUI ID: %s\r\n", szGUID);
		return IF4ERR_INVALID_ID;
	}

    if(VARAPI_OidCompare(&pValue[1].oid, "1.3") != 0) {
		return IF4ERR_INVALID_PARAM;
    }
    if(VARAPI_OidCompare(&pValue[2].oid, "1.12") != 0) {
		return IF4ERR_INVALID_PARAM;
    }

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

    /** Skip Link는 원래 기능에는 있었지만 workspace에 merge 하면서 기능을 삭제한다 */
    // bSkipLink = pValue[1].stream.u8 ? TRUE : FALSE;
	nError = m_pOndemander->BypassWrite(GetCoordinator(), &id, (BYTE *)pValue[2].stream.p, pValue[2].len);

    return nError;
}

/** Energy Level 리턴 값 설정.
 */
void AddEnergyLevelInfo(IF4Wrapper *pWrapper, ENDIENTRY *pEndi, CMeterParser *pParser, BOOL force)
{
    int         nLevelCount;
    BYTE        bLevelCount;
    BYTE        aLevelArray[255];
    TIMESTAMP   stamp;

    if(pEndi == NULL || pParser == NULL) return;

    if(force == FALSE && pParser->IsDRAsset(&pEndi->id) == FALSE) 
    {
        return;
    }

    memset(aLevelArray, 0, sizeof(aLevelArray));
    nLevelCount = pParser->GetValidEnergyLevel(&pEndi->id, aLevelArray);
    if (nLevelCount < 0) bLevelCount = 0;
    else bLevelCount = (BYTE)(nLevelCount & 0xFF);

    GetTimestamp(&stamp, &pEndi->tmLevelChange);

	IF4API_AddResultFormat(pWrapper, "1.14", VARSMI_EUI64, &pEndi->id, sizeof(EUI64));
    IF4API_AddResultNumber(pWrapper, "1.3", VARSMI_BOOL, pParser->IsDRAsset(&pEndi->id));
#if     defined(__PATCH_11_65__)
    if(*m_pbPatch_11_65) {
        IF4API_AddResultNumber(pWrapper, "1.4", VARSMI_BYTE, Patch_11_65_Nuri2Jeju(pEndi->nEnergyLevel));
    } else 
#endif
    {
        IF4API_AddResultNumber(pWrapper, "1.4", VARSMI_BYTE, pEndi->nEnergyLevel);
    }
	IF4API_AddResultFormat(pWrapper, "1.16", VARSMI_TIMESTAMP, &stamp, sizeof(TIMESTAMP));
    IF4API_AddResultNumber(pWrapper, "1.4", VARSMI_BYTE, bLevelCount);
    if (bLevelCount > 0) 
    {
#if     defined(__PATCH_11_65__)
        if(*m_pbPatch_11_65) {
            int i=0;
            for(;i<bLevelCount;i++) {
                aLevelArray[i] = Patch_11_65_Nuri2Jeju(aLevelArray[i]);
            }
        }  
#endif
        IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, aLevelArray, bLevelCount);
    }
}

BOOL EnumSensorEnergyLevel(ENDIENTRY *pEndi, void *pWrapper, void *pszBuffer, void *filter)
{
    char * parserCondi = (char *) pszBuffer;
    char * parserName = (char *) filter;
    CMeterParser * pParser = NULL;

    if(pEndi == NULL) return FALSE;
    pParser = m_pMeterParserFactory->SelectParser(pEndi->szParser);

    if(!parserCondi) {
	    AddEnergyLevelInfo((IF4Wrapper*)pWrapper, pEndi, pParser, TRUE);
	    return TRUE;
    }
    else {
        if(pParser && (!parserName || m_pMeterParserFactory->IsMatchedParser(*pParser, parserName))) {
	        AddEnergyLevelInfo((IF4Wrapper*)pWrapper, pEndi, pParser, FALSE);
	        return TRUE;
        }
    }
    return FALSE;
}

/** Device가 Energy Level을 지원하는지 여부 확인.
 *
 * @param id         EUI64(1.14) : Sensor ID or STRING(1.11) : ParserName
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *          - IF4ERR_INVALID_ID
 *
 * @return list [
 *              bDRAsset        DR Asset 여부(1.3)  
 *              nCurrentLevel   Current Energy Level(1.4)
 *              lastUpdate      last update time (1.16)
 *              nLevelCount     Device에서 지원하는 Level Count(1.4)
 *              aLevelArray     Level Array (1.12) 
 *                              nLevelCount길이로 반환됨. nLevelCount가 0일 경우 반환되지 않음.
 *          ]
 *
 */
int cmdGetDRAssetInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64       id;
	ENDIENTRY	*pEndi;
	char	    szGUID[17];
	char	    szParser[64];
    int         nCount=0;
    char        * idxPtr = NULL;

    if(cnt < 1) {
        nCount = m_pEndDeviceList->EnumEndDevice(EnumSensorEnergyLevel, (void *)pWrapper, const_cast<char *>("all"), NULL);
    }
    else  {
	    if (!IsValidEUI64(&pValue[0].oid)) {
            if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0 ) {
		        return IF4ERR_INVALID_PARAM;
            } else  {
                memset(szParser, 0, sizeof(szParser));
                if(strncasecmp(pValue[0].stream.p,"all",3)) { 
                    memcpy(szParser, pValue[0].stream.p, pValue[0].len);

                    if((idxPtr=strchr(szParser,'$'))) {
                        *idxPtr = 0x00;
                    }
                }
            }
            nCount = m_pEndDeviceList->EnumEndDevice(EnumSensorEnergyLevel, (void *)pWrapper, 
                    pValue[0].stream.p, szParser[0] ? szParser : NULL);
        }
        else if (IsValidEUI64(&pValue[0].oid))
        {
	        memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	        pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);

	        if (pEndi == NULL)
	        {
	            eui64toa(&id, szGUID);
		        XDEBUG("cmdGetDRAssetInfo: Invalid EUI ID: %s\r\n", szGUID);
		        return IF4ERR_INVALID_ID;
	        }
    
            if(!EnumSensorEnergyLevel(pEndi, (void *)pWrapper, NULL, NULL))
            {
                return IF4ERR_NO_ENTRY;
            }
        }
        else
        {
		    return IF4ERR_INVALID_PARAM;
        }
    }

	return (nCount <= 0) ? IF4ERR_NO_ENTRY : IF4ERR_NOERROR;
}

/** Device의 Energy Level 조회.
 *
 * @param id         EUI64(1.14) : Sensor ID
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *          - IF4ERR_DO_NOT_SUPPORT
 *          - IF4ERR_CANNOT_CONNECT
 *
 * @return nEnergyLevel Energy Level
 *
 */
int cmdGetEnergyLevel(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64       id;
	ENDIENTRY	*pEndi;
    int         nError=IF4ERR_NOERROR;
    CMeterParser *pParser = NULL;
	char	    szGUID[17];
    BYTE        nEnergyLevel = 0;

    if(cnt < 1)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);

	eui64toa(&id, szGUID);
	if (pEndi == NULL)
	{
		XDEBUG("cmdGetEnergyLevel: Invalid EUI ID: %s\r\n", szGUID);
		return IF4ERR_INVALID_ID;
	}

    pParser = m_pMeterParserFactory->SelectParser(pEndi->szParser);
    if (pParser == NULL)
    {
		XDEBUG("cmdGetEnergyLevel: Parser not found: %s\r\n", szGUID);
        return IF4ERR_UNKNOWN_ERROR;
    }

    if(pParser->IsDRAsset(&id) == FALSE) 
    {
		XDEBUG("cmdGetEnergyLevel: IsDRAsset: %s FALSE\r\n", szGUID);
        return IF4ERR_DO_NOT_SUPPORT;
    }

    nError = pParser->GetEnergyLevel(&id, &nEnergyLevel);
    if(nError != IF4ERR_NOERROR)  
    {
		XDEBUG("cmdGetEnergyLevel: %s %s\r\n", szGUID, IF4API_GetErrorMessage(nError));
        return nError;
    }

    m_pEndDeviceList->VerifyEndDevice(&id);

    IF4API_AddResultNumber(pWrapper, "1.4", VARSMI_BYTE, nEnergyLevel);

    return nError;
}

/** Device의 Energy Level 설정.
 *
 * TODO : 현재 Get/Set을 두번의 Connection으로 처리하고 있다. 이 부분을 Session을 지원할 때
 *        단일 Connection으로 가능하도록 하는 기능을 지원해야 한다.
 *
 *
 * @param id            EUI64(1.14) : Sensor ID
 * @param nEnergyLevel  BYTE(1.4) : Energy Level
 * @param pszUserData   STREAM(1.12) : Callback Data
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *          - IF4ERR_DO_NOT_SUPPORT
 *          - IF4ERR_CANNOT_CONNECT
 *
 */
int cmdSetEnergyLevel(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64       id;
	ENDIENTRY	*pEndi;
    int         nError=IF4ERR_NOERROR;
    CMeterParser *pParser = NULL;
	char	    szGUID[17];
    BYTE        nEnergyLevel = 0;
    BYTE        nTargetLevel = 0;
    BYTE        *pszUserData = NULL;
    int         nUserDataLen = 0;

    if(cnt < 2)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

    if (VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0)
        return IF4ERR_INVALID_PARAM;

    if(cnt > 2)
    {
        if (VARAPI_OidCompare(&pValue[2].oid, "1.12") != 0)
		    return IF4ERR_INVALID_PARAM;
    }

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);

    nTargetLevel = (BYTE) pValue[1].stream.u8;
	
	eui64toa(&id, szGUID);
	if (pEndi == NULL)
	{
		XDEBUG("cmdSetEnergyLevel: Invalid EUI ID: %s\r\n", szGUID);
		return IF4ERR_INVALID_ID;
	}

    pParser = m_pMeterParserFactory->SelectParser(pEndi->szParser);
    if (pParser == NULL)
    {
		XDEBUG("cmdSetEnergyLevel: Parser not found: %s\r\n", szGUID);
        return IF4ERR_UNKNOWN_ERROR;
    }

    if(pParser->IsDRAsset(&id) == FALSE) 
    {
		XDEBUG("cmdSetEnergyLevel: IsDRAsset: %s FALSE\r\n", szGUID);
        return IF4ERR_DO_NOT_SUPPORT;
    }

    nError = pParser->GetEnergyLevel(&id, &nEnergyLevel);
    if(nError != IF4ERR_NOERROR)  
    {
		XDEBUG("cmdSetEnergyLevel: %s %s\r\n", szGUID, IF4API_GetErrorMessage(nError));
        return nError;
    }

    if(nEnergyLevel == nTargetLevel) 
    {
        m_pEndDeviceList->VerifyEndDevice(&id);
        return IF4ERR_NOERROR;
    }

    if(cnt > 2)
    {
        nUserDataLen = pValue[2].len;
        pszUserData = (BYTE *)MALLOC(nUserDataLen);
        memcpy(pszUserData, pValue[2].stream.p, nUserDataLen);
    }

    nError = pParser->SetEnergyLevel(&id, nTargetLevel, pszUserData, nUserDataLen);
    if(nError != IF4ERR_NOERROR)  
    {
		XDEBUG("cmdSetEnergyLevel: %s %s\r\n", szGUID, IF4API_GetErrorMessage(nError));
        if(pszUserData != NULL) FREE(pszUserData);
        return nError;
    }

    m_pEndDeviceList->VerifyEndDevice(&id);

    if(pszUserData != NULL) FREE(pszUserData);
    return nError;
}
int cmdSetIHDTable(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	EUI64	id;	
	HANDLE	codi;
	int		nError;
	int 	nCount; 
	nCount = 0; 
    if(cnt < 1 || !IsValidEUI64(&pValue[0].oid))
    {
        return IF4ERR_INVALID_PARAM;
    }
	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;

    memset(&id, 0, sizeof(EUI64));
  
    ReverseEUI64((EUI64 *)pValue[0].stream.p, &id);
	nError = codiSetProperty(codi, CODI_CMD_ADD_EUI64, (BYTE *)&id, sizeof(EUI64),  3000);
	while(nError != CODIERR_NOERROR && nCount < 3 )   
	{
		nCount ++; 	
		usleep(500000);
		nError = codiSetProperty(codi, CODI_CMD_ADD_EUI64, (BYTE *)&id, sizeof(EUI64),  3000);
	}
	if (nError != CODIERR_NOERROR)	return IF4ERR_CANNOT_GET;
		
	return IF4ERR_NOERROR;
	
}   
int cmdDelIHDTable(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{

	EUI64	id;	
	HANDLE	codi;
	int		nError;
	int 	nCount; 
	nCount = 0;
    if(cnt < 1 || !IsValidEUI64(&pValue[0].oid))
    {
        return IF4ERR_INVALID_PARAM;
    }
	codi = GetCoordinator();
	if (codi == NULL)
		return IF4ERR_NO_ENTRY;
    memset(&id, 0, sizeof(EUI64));
   
    ReverseEUI64((EUI64 *)pValue[0].stream.p, &id);
	nError = codiSetProperty(codi, CODI_CMD_DELETE_EUI64, (BYTE *)&id, sizeof(EUI64),  3000);
	while(nError != CODIERR_NOERROR && nCount < 3 )   
	{
		nCount ++; 	
		usleep(500000);
		nError = codiSetProperty(codi, CODI_CMD_DELETE_EUI64, (BYTE *)&id, sizeof(EUI64),  3000);
	}
		
	if (nError != CODIERR_NOERROR) return IF4ERR_CANNOT_GET;

	return IF4ERR_NOERROR;
	
}

/** Target에 Message 전송.
 *
 * OID : 111.4
 *
 * @param id            EUI64(1.14) : Sensor ID
 * @param nMessageId    UINT(1.6) : 사용자 정의 Message ID
 * @param nMessageType  BYTE(1.4) : Message Type (0x01 Immediately, 0x02 Lazy, 0x03 Passive)
 * @param nDuration     UINT(1.6) : Lazy, Passive 일 경우 유지 시간
 * @param nErrorHandler UINT(1.6) : Error Handler
 * @param nPreHandler   UINT(1.6) : Pre-Action Handler
 * @param nPostHandler  UINT(1.6) : Post-Action Handler
 * @param nUserData     UINT(1.6) : User Data
 * @param pszData       STREAM(1.12) : Message
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_BUSY
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *          - IF4ERR_CANNOT_CONNECT
 *          - IF4ERR_DO_NOT_SUPPORT
 *
 */
int cmdSendMessage(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY	*pEndi;
    int         nError=IF4ERR_NOERROR;
    EUI64       id;
	char	    szGUID[17];

    if(cnt < 9)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid) || 
        VARAPI_OidCompare(&pValue[1].oid, "1.6") != 0 ||
        VARAPI_OidCompare(&pValue[2].oid, "1.4") != 0 ||
        VARAPI_OidCompare(&pValue[3].oid, "1.6") != 0 ||
        VARAPI_OidCompare(&pValue[4].oid, "1.6") != 0 ||
        VARAPI_OidCompare(&pValue[5].oid, "1.6") != 0 ||
        VARAPI_OidCompare(&pValue[6].oid, "1.6") != 0 ||
        VARAPI_OidCompare(&pValue[7].oid, "1.6") != 0 ||
        VARAPI_OidCompare(&pValue[8].oid, "1.12") != 0 
    ) { return IF4ERR_INVALID_PARAM; }

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	EUI64ToStr(&id, szGUID);

	pEndi = m_pEndDeviceList->GetEndDeviceByID(&id);
	if (pEndi == NULL)
	{
		XDEBUG("cmdSendMessage: Invalid EUI ID: %s\r\n", szGUID);
		return IF4ERR_INVALID_ID;
	}

    switch(pValue[2].stream.u8) {
        case MESSAGE_TYPE_IMMEDIATELY: // Immediately
#ifdef __SINGLE_THREAD_ONLY__
            if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif
	        nError = m_pOndemander->BypassWrite(GetCoordinator(), &id, 
                    (BYTE *)pValue[8].stream.p, pValue[8].len);
            break;
        case MESSAGE_TYPE_PASSIVE: // Passive
            m_pMessageHelper->Add(szGUID, pValue[1].stream.u32, pValue[2].stream.u8,
                pValue[3].stream.u32, pValue[4].stream.u32, pValue[5].stream.u32, pValue[6].stream.u32,
                pValue[7].stream.u32, pValue[8].len, (BYTE *)pValue[8].stream.p);
            break;
        case MESSAGE_TYPE_LAZY: // Lazy
        default:
            return IF4ERR_DO_NOT_SUPPORT;
    }
    return nError;
}

/** cmdSendMessage에 의해 저장된 Message를 조회.
 *
 * OID : 111.5
 *
 * Passive Message만 조회가 되고 한번 조회된 Message는 
 * DCU에서 자동으로 삭제된다.
 *
 * @param id            EUI64(1.14)     : Sensor ID
 *
 * @result nMessageCnt  UINT(1.6)       : Message count
 *       [ 
 *         nMessageId   UINT(1.6)       : Message id
 *         issueTime    TIMESTAMP(1.16) : Message 전송 시간
 *         nUserData    UINT(1.6)       : User data
 *         pszData      STREAM(1.12)    : Message
 *       ]
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdPollMessage(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    EUI64       id;
	char	    szGUID[17];
    int         nError=IF4ERR_NOERROR;
    //CppSQLite3Query result;

    if(cnt < 1)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid)) {
        return IF4ERR_INVALID_PARAM; 
    }

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	EUI64ToStr(&id, szGUID);

    m_pMessageHelper->Select(szGUID, MESSAGE_TYPE_PASSIVE, pWrapper);
    m_pMessageHelper->Delete(szGUID, MESSAGE_TYPE_PASSIVE);

    return nError;
}

/** Device의 Load Profile 조회
 *
 * @param id            EUI64(1.14) : Sensor ID
 * @param timestamp     TIMESTAMP(1.16) : 조회 대상 일
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdGetLoadProfile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
#if 0
    EUI64       id;
    // TODO
	char	    szGUID[17];
    TIMESTAMP   tFrom, tTo;
    BOOL        res;
    int         i,j;
    DBFetchData result;
    METERINGDATAINS *pIns;
    BYTE        *pResValue;

    if(cnt < 2)
		return IF4ERR_INVALID_PARAM;

	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_ID;

    if (VARAPI_OidCompare(&pValue[1].oid, "1.16") != 0)
        return IF4ERR_INVALID_PARAM;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
    EUI64ToStr(&id, szGUID);
    memcpy(&tFrom, pValue[1].stream.p, sizeof(TIMESTAMP));
    memcpy(&tTo, pValue[1].stream.p, sizeof(TIMESTAMP));
    tFrom.hour = 0; tFrom.min = 0; tFrom.sec = 0;
    tTo.hour = 23; tTo.min = 59; tTo.sec = 59;

    memset(&result, 0, sizeof(DBFetchData));
    res = m_pMeteringHelper->Select((char *)szGUID, 0, &tFrom, &tTo, &result);

    if(res && result.nCount > 0)
    {
        pIns = m_pMeteringHelper->GetData(m_pMeteringHelper->GetHeadRow(&result));
        if(pIns && pIns->nMeteringCnt > 0)
        {
            IF4API_AddResultNumber(pWrapper, "1.4", VARSMI_BYTE, pIns->nChannelCnt);
            pResValue = (BYTE *) MALLOC(8 * pIns->nChannelCnt);
            for(i=0; i<pIns->nChannelCnt; i++)
            {
                memcpy(pResValue + (i*8), &pIns->pMetering[0]->pBasePulse[i]->nBaseValue, 8);
            }
            IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, pResValue, 8 * pIns->nChannelCnt);

            IF4API_AddResultNumber(pWrapper, "1.5", VARSMI_WORD, pIns->pMetering[0]->nLpCnt);
            for(i=0; i<pIns->pMetering[0]->nLpCnt; )
            {
                IF4API_AddResultFormat(pWrapper, "1.16", VARSMI_TIMESTAMP, 
                        &pIns->pMetering[0]->pLoadProfile[i]->_snapShotTime, sizeof(TIMESTAMP));
                for(j=0; j<pIns->nChannelCnt; j++, i++)
                {
                    memcpy(pResValue + (j*8), &pIns->pMetering[0]->pLoadProfile[i]->nValue, 8);
                }
                IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, pResValue, 8 * pIns->nChannelCnt);
            }
            FREE(pResValue);
        }
    }
    //m_pMeteringHelper->DumpResult(&result);
    m_pMeteringHelper->FreeResult(&result);
#endif

    return nError;
}

/** Group 정보 갱신
 *
 * @param type          STRING(1.11): Type
 * @param name          STRING(1.11): Name
 * @param id            EUI64(1.14) : Sensor ID
 * ....
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdUpdateGroup(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
    int         i;

    if(cnt < 3)
		return IF4ERR_INVALID_PARAM;

    if (VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)
        return IF4ERR_INVALID_PARAM;

    if (VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0)
        return IF4ERR_INVALID_PARAM;

    for(i=2; i<cnt; i++)
    {
	    if (!IsValidEUI64(&pValue[i].oid))
		    return IF4ERR_INVALID_ID;
    }

    for(i=2; i<cnt; i++)
    {
        m_pGroupHelper->Update(
                (const char *)pValue[0].stream.p,
                (const char *)pValue[1].stream.p,
                (EUI64*)pValue[i].stream.p);
    }

    return nError;
}

/** Group 정보 삭제 
 *
 * @param type          STRING(1.11): Type
 * @param name          STRING(1.11): Name
 * @param id            EUI64(1.14) : Sensor ID
 * ....
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdDeleteGroup(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
	char	    szGUID[17];
    EUI64       id;
    /*BOOL        res = FALSE;*/
    char * szType = NULL;
    char * szName = NULL;
    char * szId = NULL;

    if(cnt > 0)
    {
	    if (IsValidEUI64(&pValue[0].oid))
        {
	        memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
            EUI64ToStr(&id, szGUID);
            szId = szGUID;
        }
        else
        {
            szType = (char *)pValue[0].stream.p;
        }
    }
    if(cnt > 1)
    {
	    if (IsValidEUI64(&pValue[1].oid))
        {
	        memcpy(&id, pValue[1].stream.p, sizeof(EUI64));
            EUI64ToStr(&id, szGUID);
            szId = szGUID;
        }
        else
        {
            szName = (char *)pValue[1].stream.p;
        }
    }
    if(cnt > 2)
    {
	    if (IsValidEUI64(&pValue[2].oid))
        {
	        memcpy(&id, pValue[2].stream.p, sizeof(EUI64));
            EUI64ToStr(&id, szGUID);
            szId = szGUID;
        }
        else
        {
            return IF4ERR_INVALID_ID;
        }

    }
    /*res = */m_pGroupHelper->Delete(szType, szName, szId);

    return nError;
}

/** Group 정보 조회 
 *
 * @param type          STRING(1.11): Type
 * @param name          STRING(1.11): Name
 * @param id            EUI64(1.14) : Sensor ID
 *
 * 첫 Parameter가 EUI64 일 경우 해당 ID의 모든 Group 정보
 * 첫 Parameter가 STRING 일 경우 type, name, id를 입력 받고
 *    name, id는 생략될 수 있다
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_ID
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdGetGroup(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
    DBFetchData result;
    GROUPDATAINS *pIns;
    GROUPENTRYINS *pEntry;
    Row         *pRow;
	char	    szGUID[17];
    EUI64       id;
    BOOL        res = FALSE;
    int         i,j;
    char * szType = NULL;
    char * szName = NULL;
    char * szId = NULL;

    memset(&result, 0, sizeof(DBFetchData));
        
    if(cnt > 0)
    {
	    if (IsValidEUI64(&pValue[0].oid))
        {
	        memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
            EUI64ToStr(&id, szGUID);
            szId = szGUID;
        }
        else
        {
            szType = (char *)pValue[0].stream.p;
        }
    }
    if(cnt > 1)
    {
	    if (IsValidEUI64(&pValue[1].oid))
        {
	        memcpy(&id, pValue[1].stream.p, sizeof(EUI64));
            EUI64ToStr(&id, szGUID);
            szId = szGUID;
        }
        else
        {
            szName = (char *)pValue[1].stream.p;
        }
    }
    if(cnt > 2)
    {
	    if (IsValidEUI64(&pValue[2].oid))
        {
	        memcpy(&id, pValue[2].stream.p, sizeof(EUI64));
            EUI64ToStr(&id, szGUID);
            szId = szGUID;
        }
        else
        {
            return IF4ERR_INVALID_ID;
        }

    }
    res = m_pGroupHelper->Select(szType, szName, szId, &result);

    //XDEBUG("result.nCount %d\r\n", result.nCount);
    if(res && result.nCount > 0)
    {
        pRow = m_pGroupHelper->GetHeadRow(&result);
        for(i=0; i<result.nCount && pRow; i++)
        {
            pIns = m_pGroupHelper->GetData(pRow);
            if(pIns)
            {
                for(j=0; j<pIns->nEntryCnt; j++)
                {
                    pEntry = pIns->pEntry[j];
                    IF4API_AddResultString(pWrapper, "1.11", pIns->szType);
                    IF4API_AddResultString(pWrapper, "1.11", pIns->szName);
                    IF4API_AddResultString(pWrapper, "1.11", pEntry->szTargetId);
                }
            }
            pRow = m_pGroupHelper->GetNextRow(pRow);
        }
    }
    //m_pGroupHelper->DumpResult(&result);
    m_pGroupHelper->FreeResult(&result);

    return nError;
}

/** Meter Security 정보 조회.
 *
 * @param type          STRING(1.11): Type (PARSER, NODE, ID)
 * @param name          STRING(1.11): Name
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdGetMeterSecurity(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
    DBFetchData result;
    METERPROPERTYDATAINS *pIns;
    Row         *pRow;
    BOOL        res = FALSE;
    int         i;
    char * szType = NULL;
    char * szName = NULL;

    memset(&result, 0, sizeof(DBFetchData));
        
    if(cnt > 0)
    {
        if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szType = (char *)pValue[0].stream.p;
    }
    if(cnt > 1)
    {
        if(VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szName = (char *)pValue[1].stream.p;
    }

    res = m_pMeterPropertyHelper->Select(PROPERTY_GRP_SECURITY, PROPERTY_NAME_SECURITY, szType, szName, &result);

    //XDEBUG("result.nCount %d\r\n", result.nCount);
    if(res && result.nCount > 0)
    {
        pRow = m_pMeterPropertyHelper->GetHeadRow(&result);
        for(i=0; i<result.nCount && pRow; i++)
        {
            pIns = m_pMeterPropertyHelper->GetData(pRow);
            if(pIns && pIns->pValue != NULL && pIns->nValueLen > 0)
            {
                IF4API_AddResultString(pWrapper, "1.11", pIns->szType);
                IF4API_AddResultString(pWrapper, "1.11", pIns->szKey);
                IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, pIns->pValue, pIns->nValueLen);
            }
            pRow = m_pMeterPropertyHelper->GetNextRow(pRow);
        }
    }
    m_pMeterPropertyHelper->FreeResult(&result);

    return nError;
}

/** Meter Security 정보 설정.
 *
 * @param type          STRING(1.11): Type (PARSER, NODE, ID)
 * @param name          STRING(1.11): Name
 * @param key           STREAM(1.12): Key
 *
 * key를 생략할 경우 delete로 동작. key가 전달될 경우 update/insert로 동작.
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdSetMeterSecurity(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
    char * szType = NULL;
    char * szName = NULL;

    if(cnt < 2)
    {
            return IF4ERR_INVALID_PARAM;
    }
    if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0 ||
       VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0)
    {
            return IF4ERR_INVALID_PARAM;
    }

    szType = (char *)pValue[0].stream.p;
    szName = (char *)pValue[1].stream.p;

    if(cnt > 2)
    {
        m_pMeterPropertyHelper->Update(PROPERTY_GRP_SECURITY, PROPERTY_NAME_SECURITY,
                szType, szName, (BYTE *)pValue[2].stream.p, pValue[2].len);
    }
    else
    {
        m_pMeterPropertyHelper->Delete(PROPERTY_GRP_SECURITY, PROPERTY_NAME_SECURITY,
                szType, szName);
    }

    return nError;
}

/** Metering Option 정보 조회.
 *
 * @param type          STRING(1.11): Type (PARSER, NODE, ID)
 * @param name          STRING(1.11): Name
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdGetMeteringOption(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
    DBFetchData result;
    METERPROPERTYDATAINS *pIns;
    Row         *pRow;
    BOOL        res = FALSE;
    int         i;
    char * szType = NULL;
    char * szName = NULL;

    memset(&result, 0, sizeof(DBFetchData));
        
    if(cnt > 0)
    {
        if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szType = (char *)pValue[0].stream.p;
    }
    if(cnt > 1)
    {
        if(VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szName = (char *)pValue[1].stream.p;
    }

    res = m_pMeterPropertyHelper->Select(PROPERTY_GRP_METERING_PARAMETER, NULL, szType, szName, &result);

    if(res && result.nCount > 0)
    {
        /** Issue #930 MeteringOptionTbl 의 정보를 MeterPropertyTbl 로 이전하면서 Data를 조합하는 코드를
         * 추가하게 된다.
         */
        int nOption = -1;
        int nOffset = -1;
        int nCount = -1;
        int nParamCnt = 0;
        char szType[128], szKey[128];
        BOOL bNew = TRUE;

        memset(szType, 0, sizeof(szType));
        memset(szKey, 0, sizeof(szKey));

        pRow = m_pMeterPropertyHelper->GetHeadRow(&result);
        for(i=0; i<result.nCount && pRow; i++)
        {
            pIns = m_pMeterPropertyHelper->GetData(pRow);
            if(pIns)
            {
                if(szType[0])
                {
                    if(strcmp(szType, pIns->szType)!=0 || strcmp(szKey, pIns->szKey) != 0)
                    {
                        IF4API_AddResultString(pWrapper, "1.11", szType);
                        IF4API_AddResultString(pWrapper, "1.11", szKey);
                        IF4API_AddResultNumber(pWrapper, "1.9", VARSMI_INT, nOption);
                        IF4API_AddResultNumber(pWrapper, "1.9", VARSMI_INT, nOffset);
                        IF4API_AddResultNumber(pWrapper, "1.9", VARSMI_INT, nCount);

                        bNew = TRUE;
                        nParamCnt = 0;
                        nOption = -1; nOffset = -1; nCount = -1;
                    }
                }
                if(strcmp(PROPERTY_NAME_OPTION, pIns->szProperty)==0)
                {
                    if(pIns->pValue != NULL && pIns->nValueLen > 0)
                    {
                        nOption = (int)strtol((const char *)pIns->pValue, (char **)NULL, 10);
                        nParamCnt ++;
                    }
                }
                if(strcmp(PROPERTY_NAME_OFFSET, pIns->szProperty)==0)
                {
                    if(pIns->pValue != NULL && pIns->nValueLen > 0)
                    {
                        nOffset = (int)strtol((const char *)pIns->pValue, (char **)NULL, 10);
                        nParamCnt ++;
                    }
                }
                if(strcmp(PROPERTY_NAME_COUNT, pIns->szProperty)==0)
                {
                    if(pIns->pValue != NULL && pIns->nValueLen > 0)
                    {
                        nCount = (int)strtol((const char *)pIns->pValue, (char **)NULL, 10);
                        nParamCnt ++;
                    }
                }

                if(bNew)
                {
                    bNew = FALSE;
                    strcpy(szType, pIns->szType);
                    strcpy(szKey, pIns->szKey);
                }

            }
            pRow = m_pMeterPropertyHelper->GetNextRow(pRow);
        }

        if(nParamCnt > 0)
        {
            IF4API_AddResultString(pWrapper, "1.11", szType);
            IF4API_AddResultString(pWrapper, "1.11", szKey);
            IF4API_AddResultNumber(pWrapper, "1.9", VARSMI_INT, nOption);
            IF4API_AddResultNumber(pWrapper, "1.9", VARSMI_INT, nOffset);
            IF4API_AddResultNumber(pWrapper, "1.9", VARSMI_INT, nCount);
        }
    }
    m_pMeterPropertyHelper->FreeResult(&result);

    return nError;
}

/** Metering Option 정보 설정.
 *
 * @param type          STRING(1.11): Type (PARSER, NODE, ID)
 * @param name          STRING(1.11): Name
 * @param option        INT(1.9)
 * @param offset        INT(1.9)
 * @param count         INT(1.9)
 *
 * option, offset, count를 생략할 경우 delete로 동작. 
 * option, offset, count가 전달될 경우 update/insert로 동작.
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdSetMeteringOption(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int    nError=IF4ERR_NOERROR;
    char * szType = NULL;
    char * szName = NULL;
    char   szBuffer[64];
    int    nOption = -1;
    int    nOffset = -1;
    int    nCount = -1;

    if(cnt < 2)
    {
            return IF4ERR_INVALID_PARAM;
    }
    if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0 ||
       VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0)
    {
            return IF4ERR_INVALID_PARAM;
    }

    szType = (char *)pValue[0].stream.p;
    szName = (char *)pValue[1].stream.p;

    if(cnt > 2)
    {
        nOption = pValue[2].stream.s32;
        sprintf(szBuffer, "%d", nOption);
        m_pMeterPropertyHelper->Update(PROPERTY_GRP_METERING_PARAMETER, PROPERTY_NAME_OPTION,
                szType, szName, (BYTE *)szBuffer, strlen(szBuffer));
        if(cnt > 3) 
        {
            nOffset = pValue[3].stream.s32;
        }
        sprintf(szBuffer, "%d", nOffset);
        m_pMeterPropertyHelper->Update(PROPERTY_GRP_METERING_PARAMETER, PROPERTY_NAME_OFFSET,
            szType, szName, (BYTE *)szBuffer, strlen(szBuffer));
        if(cnt > 4) 
        {
            nCount = pValue[4].stream.s32;
        }
        sprintf(szBuffer, "%d", nCount);
        m_pMeterPropertyHelper->Update(PROPERTY_GRP_METERING_PARAMETER, PROPERTY_NAME_COUNT,
            szType, szName, (BYTE *)szBuffer, strlen(szBuffer));
    }
    else
    {
        m_pMeterPropertyHelper->Delete(PROPERTY_GRP_METERING_PARAMETER, NULL, szType, szName);
    }

    return nError;
}

/** Meter Property 정보 조회.
 *
 * @param group         STRING(1.11): Property Group
 * @param property      STRING(1.11): Property Name
 * @param type          STRING(1.11): Type (PARSER, NODE, ID)
 * @param key           STRING(1.11): Property Key
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdGetMeterProperty(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
    DBFetchData result;
    METERPROPERTYDATAINS *pIns;
    Row         *pRow;
    BOOL        res = FALSE;
    int         i;
    char * szGroup = NULL;
    char * szProperty = NULL;
    char * szType = NULL;
    char * szKey = NULL;

    memset(&result, 0, sizeof(DBFetchData));
        
    if(cnt > 0)
    {
        if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szGroup = (char *)pValue[0].stream.p;
    }
    if(cnt > 1)
    {
        if(VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szProperty = (char *)pValue[1].stream.p;
    }
    if(cnt > 2)
    {
        if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szType = (char *)pValue[0].stream.p;
    }
    if(cnt > 3)
    {
        if(VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szKey = (char *)pValue[1].stream.p;
    }

    res = m_pMeterPropertyHelper->Select(szGroup, szProperty, szType, szKey, &result);

    if(res && result.nCount > 0)
    {
        pRow = m_pMeterPropertyHelper->GetHeadRow(&result);
        for(i=0; i<result.nCount && pRow; i++)
        {
            pIns = m_pMeterPropertyHelper->GetData(pRow);
            if(pIns && pIns->pValue != NULL && pIns->nValueLen > 0)
            {
                IF4API_AddResultString(pWrapper, "1.11", pIns->szGroup);
                IF4API_AddResultString(pWrapper, "1.11", pIns->szProperty);
                IF4API_AddResultString(pWrapper, "1.11", pIns->szType);
                IF4API_AddResultString(pWrapper, "1.11", pIns->szKey);
                IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, pIns->pValue, pIns->nValueLen);
            }
            pRow = m_pMeterPropertyHelper->GetNextRow(pRow);
        }
    }
    m_pMeterPropertyHelper->FreeResult(&result);

    return nError;
}

/** 개발 Sensor와 연결된 Meter Property 정보 조회.
 *
 * @param id            EUI64(1.14): Sensor ID
 * @param group         STRING(1.11): Property Group
 * @param property      STRING(1.11): Property Name
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdGetMeterPropertyById(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
    DBFetchData result;
    METERPROPERTYDATAINS *pIns;
    Row         *pRow;
    BOOL        res = FALSE;
    int         i;
    char * szGroup = NULL;
    char * szProperty = NULL;
    EUI64 id;
    char szGUID[17];
    ENDIENTRY *endi;

    if(cnt < 1)
    {
        return IF4ERR_INVALID_PARAM;
    }

    memset(&result, 0, sizeof(DBFetchData));
    memset(szGUID, 0, sizeof(szGUID));

    if (IsValidEUI64(&pValue[0].oid))
    {
        memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
        EUI64ToStr(&id, szGUID);
    }
    else
    {
        return IF4ERR_INVALID_PARAM;
    }

    if(cnt > 1)
    {
        if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szGroup = (char *)pValue[0].stream.p;
    }
    if(cnt > 1)
    {
        if(VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0)
        {
            return IF4ERR_INVALID_PARAM;
        }
        szProperty = (char *)pValue[1].stream.p;
    }

    endi = m_pEndDeviceList->GetEndDeviceByID(&id);
    res = m_pMeterPropertyHelper->SelectAll(szGroup, szProperty,  endi->szParser, endi->szNodeKind, &id, &result);

    if(res && result.nCount > 0)
    {
        pRow = m_pMeterPropertyHelper->GetHeadRow(&result);
        for(i=0; i<result.nCount && pRow; i++)
        {
            pIns = m_pMeterPropertyHelper->GetData(pRow);
            if(pIns && pIns->pValue != NULL && pIns->nValueLen > 0)
            {
                IF4API_AddResultString(pWrapper, "1.11", pIns->szGroup);
                IF4API_AddResultString(pWrapper, "1.11", pIns->szProperty);
                IF4API_AddResultString(pWrapper, "1.11", pIns->szType);
                IF4API_AddResultString(pWrapper, "1.11", pIns->szKey);
                IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, pIns->pValue, pIns->nValueLen);
            }
            pRow = m_pMeterPropertyHelper->GetNextRow(pRow);
        }
    }
    m_pMeterPropertyHelper->FreeResult(&result);

    return nError;
}

/** Meter Security 정보 설정.
 *
 * @param group         STRING(1.11): Property Group
 * @param property      STRING(1.11): Property Name
 * @param type          STRING(1.11): Type (PARSER, NODE, ID)
 * @param key           STRING(1.11): Property Key
 * @param value         STREAM(1.12): Property Value
 *
 * value를 생략할 경우 delete로 동작. value가 전달될 경우 update/insert로 동작.
 *
 * @return nErrorCode 
 *          - IF4ERR_NOERROR
 *          - IF4ERR_INVALID_PARAM
 *
 */
int cmdSetMeterProperty(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    int         nError=IF4ERR_NOERROR;
    char * szGroup = NULL;
    char * szProperty = NULL;
    char * szType = NULL;
    char * szKey = NULL;

    if(cnt < 4)
    {
            return IF4ERR_INVALID_PARAM;
    }
    if(VARAPI_OidCompare(&pValue[0].oid, "1.11") != 0 ||
       VARAPI_OidCompare(&pValue[1].oid, "1.11") != 0 ||
       VARAPI_OidCompare(&pValue[2].oid, "1.11") != 0 ||
       VARAPI_OidCompare(&pValue[3].oid, "1.11") != 0)
    {
            return IF4ERR_INVALID_PARAM;
    }

    szGroup = (char *)pValue[0].stream.p;
    szProperty = (char *)pValue[1].stream.p;
    szType = (char *)pValue[2].stream.p;
    szKey = (char *)pValue[3].stream.p;

    if(cnt > 4)
    {
        m_pMeterPropertyHelper->Update(szGroup, szProperty,
                szType, szKey, (BYTE *)pValue[4].stream.p, pValue[4].len);
    }
    else
    {
        m_pMeterPropertyHelper->Delete(szGroup, szProperty,
                szType, szKey);
    }

    return nError;
}

#ifdef __SUPPORT_KUKDONG_METER__
int cmdValveControl(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY	*pEndDevice;
	CMeterParser	*parser;
	METER_STAT	stat;
	CMDPARAM	param;
	CChunk		chunk(1024);
	char		szGUID[20];
    BYTE 		opcode=0;
	EUI64		id;
	int			nError;

	if (cnt < 2)
		return IF4ERR_NEED_MORE_VALUE;
		
	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0)
		return IF4ERR_INVALID_PARAM;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	EUI64ToStr(&id, szGUID); 
	opcode = pValue[1].stream.u8;

	pEndDevice = m_pEndDeviceList->GetEndDeviceByID(&id);
	if (pEndDevice == NULL)
	{
		XDEBUG("cmdOndemandValve: Invalid ID: %s\xd\xa", szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	parser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
	if ((parser == NULL) || (parser->GetModel() == NULL) || (strstr(parser->GetModel(), "KEUKDONG") == NULL))
		return IF4ERR_INVALID_TYPE;

	memset(&stat, 0, sizeof(METER_STAT));
	memset(&param, 0, sizeof(CMDPARAM));
    param.codi      = GetCoordinator();
	param.pWrapper	= pWrapper;
	param.argv		= pValue;
	param.argc		= cnt;

	memcpy(&param.id, &id, sizeof(EUI64));
    param.nMeteringType = METERING_TYPE_BYPASS;
	param.pChunk	= &chunk;

    switch(opcode)
    {
        case 0:
	        param.nOption	= WRITE_OPTION_KD_VALVE_OPEN;
            break;
        case 1:
	        param.nOption	= WRITE_OPTION_KD_VALVE_CLOSE;
            break;
        case 2:
	        param.nOption	= WRITE_OPTION_KD_VALVE_STANDBY;
            break;
        default:
		    return IF4ERR_INVALID_PARAM;
    }

	XDEBUG("\r\ncmdValveControl: ID=%s, opcode=%d\r\n", szGUID, opcode);
	nError = m_pOndemander->Command(pEndDevice->szParser, &param, &stat);
    XDEBUG("\r\ncmdValveControl: ID=%s, nError=%d\r\n\r\n", szGUID, nError);

	if (nError != ONDEMAND_ERROR_OK)
	{
	 	if (nError == ONDEMAND_ERROR_BUSY)
			 return IF4ERR_BUSY;
		else if (nError == ONDEMAND_ERROR_NOT_CONNECT)
			 return IF4ERR_CANNOT_CONNECT;
		else if (nError == ONDEMAND_ERROR_PARAM)
			 return IF4ERR_INVALID_PARAM;
		else return IF4ERR_CANNOT_GET;
	}
    return nError;
}

int cmdGetMeterStatus(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY	*pEndDevice;
	METER_STAT	stat;
	CMeterParser	*parser;
	OADI_KD_STATUS_PAYLOAD *pStatus;
	CMDPARAM	param;
	CChunk		chunk(1024);
	char		szGUID[20];
	char		szSerial[21];
	EUI64		id;
	int			nError, nOption = READ_OPTION_KD_METER_STATUS;

	if (cnt < 1)
		return IF4ERR_NEED_MORE_VALUE;
		
	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_PARAM;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	EUI64ToStr(&id, szGUID);

	pEndDevice = m_pEndDeviceList->GetEndDeviceByID(&id);
	if (pEndDevice == NULL)
	{
		XDEBUG("cmdGetMeterStatus: Invalid ID: %s\xd\xa", szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	parser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
	if ((parser == NULL) || (parser->GetModel() == NULL) || (strstr(parser->GetModel(), "KEUKDONG") == NULL))
		return IF4ERR_INVALID_TYPE;

	memset(&stat, 0, sizeof(METER_STAT));
	memset(&param, 0, sizeof(CMDPARAM));
    param.codi      = GetCoordinator();
	param.pWrapper	= pWrapper;
	param.argv		= pValue;
	param.argc		= cnt;

	memcpy(&param.id, &id, sizeof(EUI64));
    param.nMeteringType = METERING_TYPE_BYPASS;
	param.nOption	= nOption;
	param.pChunk	= &chunk;

    XDEBUG("\r\ncmdGetMeterStatus: ID=%s, Option=%d\r\n", szGUID, nOption);
	nError = m_pOndemander->Command(pEndDevice->szParser, &param, &stat);
    XDEBUG("\r\ncmdGetMeterStatus: ID=%s Done.\r\n\r\n", szGUID);

	if (nError != ONDEMAND_ERROR_OK)
	{
	 	if (nError == ONDEMAND_ERROR_BUSY)
			 return IF4ERR_BUSY;
		else if (nError == ONDEMAND_ERROR_NOT_CONNECT)
			 return IF4ERR_CANNOT_CONNECT;
		else if (nError == ONDEMAND_ERROR_PARAM)
			 return IF4ERR_INVALID_PARAM;
		else return IF4ERR_CANNOT_GET;
	}

	pStatus = (OADI_KD_STATUS_PAYLOAD *)chunk.GetBuffer();
	memset(szSerial, 0, sizeof(szSerial));
	strncpy(szSerial, pStatus->meterSerial, 12);

	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, BigToHostInt(pStatus->curPulse));		// Current pulse
	IF4API_AddResultString(pWrapper, "1.11", szSerial);									// Meter serial
	IF4API_AddResultNumber(pWrapper, "1.4", VARSMI_BYTE, pStatus->alarmStatus);			// Alaram status
	IF4API_AddResultNumber(pWrapper, "1.4", VARSMI_BYTE, pStatus->meterStatus);			// Meter status
    return nError;
}

int cmdGetMeterVersion(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY	*pEndDevice;
	CMeterParser	*parser;
	METER_STAT	stat;
	OADI_KD_VERSION_PAYLOAD *pVersion;
	CMDPARAM	param;
	CChunk		chunk(1024);
	char		szGUID[20];
	char		szDateAccept[11];
	char		szHwVersion[11];
	char		szSwVersion[11];
	EUI64		id;
	int			nError, nOption = READ_OPTION_KD_METER_VERSION;

	if (cnt < 1)
		return IF4ERR_NEED_MORE_VALUE;
		
	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_PARAM;

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	EUI64ToStr(&id, szGUID);

	pEndDevice = m_pEndDeviceList->GetEndDeviceByID(&id);
	if (pEndDevice == NULL)
	{
		XDEBUG("cmdGetMeterVersion: Invalid ID: %s\xd\xa", szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	parser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
	if ((parser == NULL) || (parser->GetModel() == NULL) || (strstr(parser->GetModel(), "KEUKDONG") == NULL))
		return IF4ERR_INVALID_TYPE;

	memset(&stat, 0, sizeof(METER_STAT));
	memset(&param, 0, sizeof(CMDPARAM));
    param.codi      = GetCoordinator();
	param.pWrapper	= pWrapper;
	param.argv		= pValue;
	param.argc		= cnt;

	memcpy(&param.id, &id, sizeof(EUI64));
    param.nMeteringType = METERING_TYPE_BYPASS;
	param.nOption	= nOption;
	param.pChunk	= &chunk;

    XDEBUG("\r\ncmdGetMeterVersion: ID=%s, Option=%d\r\n", szGUID, nOption);
	nError = m_pOndemander->Command(pEndDevice->szParser, &param, &stat);
    XDEBUG("\r\ncmdGetMeterVersion: ID=%s Done.\r\n\r\n", szGUID);

	if (nError != ONDEMAND_ERROR_OK)
	{
	 	if (nError == ONDEMAND_ERROR_BUSY)
			 return IF4ERR_BUSY;
		else if (nError == ONDEMAND_ERROR_NOT_CONNECT)
			 return IF4ERR_CANNOT_CONNECT;
		else if (nError == ONDEMAND_ERROR_PARAM)
			 return IF4ERR_INVALID_PARAM;
		else return IF4ERR_CANNOT_GET;
	}

	pVersion = (OADI_KD_VERSION_PAYLOAD *)chunk.GetBuffer();
	memset(szDateAccept, 0, sizeof(szDateAccept));
	memset(szHwVersion, 0, sizeof(szHwVersion));
	memset(szSwVersion, 0, sizeof(szSwVersion));

	memcpy(szDateAccept, pVersion->testDate, 6);
	szDateAccept[6] = pVersion->testResult;
	strncpy(szHwVersion, pVersion->hwVersion, 7);
	strncpy(szSwVersion, pVersion->swVersion, 7);

	IF4API_AddResultString(pWrapper, "1.11", szDateAccept);	// Test date & accept
	IF4API_AddResultString(pWrapper, "1.11", szHwVersion);	// H/W Version
	IF4API_AddResultString(pWrapper, "1.11", szSwVersion);	// S/W Version
    return nError;
}

int cmdSetMeterConfig(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	ENDIENTRY	*pEndDevice;
	CMeterParser	*parser;
	METER_STAT	stat;
	CMDPARAM	param;
	CChunk		chunk(1024);
	char		szGUID[20];
	EUI64		id;
	int			nError;

	if (cnt < 3)
		return IF4ERR_NEED_MORE_VALUE;
		
	if (!IsValidEUI64(&pValue[0].oid))
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[1].oid, "1.4") != 0)
		return IF4ERR_INVALID_PARAM;

	if (VARAPI_OidCompare(&pValue[2].oid, "1.6") != 0)
		return IF4ERR_INVALID_PARAM;

    if( pValue[1].stream.u8 != 0x00)
    {
	    if (VARAPI_OidCompare(&pValue[3].oid, "1.11") != 0)
		    return IF4ERR_INVALID_PARAM;
    }

	memcpy(&id, pValue[0].stream.p, sizeof(EUI64));
	EUI64ToStr(&id, szGUID); 
	
	pEndDevice = m_pEndDeviceList->GetEndDeviceByID(&id);
	if (pEndDevice == NULL)
	{
		XDEBUG("cmdSetMeterConfig: Invalid ID: %s\xd\xa", szGUID);
		return IF4ERR_INVALID_ID;
	}

#ifdef __SINGLE_THREAD_ONLY__
    if (IsBusyCoreService()) return IF4ERR_BUSY;
#endif

	parser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
	if ((parser == NULL) || (parser->GetModel() == NULL) || (strstr(parser->GetModel(), "KEUKDONG") == NULL))
		return IF4ERR_INVALID_TYPE;

	memset(&stat, 0, sizeof(METER_STAT));
	memset(&param, 0, sizeof(CMDPARAM));
    param.codi      = GetCoordinator();
	param.pWrapper	= pWrapper;
	param.argv		= pValue;
	param.argc		= cnt;

	memcpy(&param.id, &id, sizeof(EUI64));
	param.nOption	= WRITE_OPTION_KD_CONFIG;
    param.nMeteringType = METERING_TYPE_BYPASS;
	param.pChunk	= &chunk;
	
	XDEBUG("\r\ncmdSetMeterConfig: ID=%s, Mask=0x%02X, Pulse=%d, Serial=%s\r\n",
			szGUID, pValue[1].stream.u8, pValue[2].stream.u32, pValue[3].stream.p);
	nError = m_pOndemander->Command(pEndDevice->szParser, &param, &stat);
    XDEBUG("\r\ncmdSetMeterConfig: ID=%s Done.\r\n\r\n", szGUID);

	if (nError != ONDEMAND_ERROR_OK)
	{
	 	if (nError == ONDEMAND_ERROR_BUSY)
			 return IF4ERR_BUSY;
		else if (nError == ONDEMAND_ERROR_NOT_CONNECT)
			 return IF4ERR_CANNOT_CONNECT;
		else if (nError == ONDEMAND_ERROR_PARAM)
			 return IF4ERR_INVALID_PARAM;
		else return IF4ERR_CANNOT_GET;
	}
    return nError;
}
#endif




