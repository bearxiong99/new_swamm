#include "typedef.h"
#include "varapi.h"

#include "Variable.h"

VAROBJECT	m_Root_node[] =
{
	{   1,	"smi", { {1,0,0}, VARSMI_OID, 3 },                  VARATTR_RW, m_smi_node, NULL, NULL },
	{   2,	"mcu", { {2,0,0}, VARSMI_OID, 3 },                  VARATTR_READ, m_mcu_node, NULL, NULL },
	{   3,	"sink", { {3,0,0}, VARSMI_OID, 0 },                 VARATTR_READ, m_sink_node, NULL, NULL },
	{   4,	"sensor", { {4,0,0}, VARSMI_OID, 0 },               VARATTR_READ, m_sensor_node, NULL, NULL },
	{   5,	"network", { {5,0,0}, VARSMI_OID, 0 },              VARATTR_READ, m_network_node, NULL, NULL },
	{  10,	"meter", { {10,0,0}, VARSMI_OID, 3 },               VARATTR_READ, m_meter_node, NULL, NULL },
	{ 200,	"eventMcu", { {200,0,0}, VARSMI_OID, 0 },           VARATTR_READ, m_eventMcu_node, NULL, NULL },
	{ 201,	"eventSink", { {201,0,0}, VARSMI_OID, 0 },          VARATTR_READ, m_eventSink_node, NULL, NULL },
	{ 202,	"eventMobile", { {202,0,0}, VARSMI_OID, 0 },        VARATTR_READ, m_eventMobile_node, NULL, NULL },
	{ 203,	"eventSensor", { {203,0,0}, VARSMI_OID, 0 },        VARATTR_READ, m_eventSensor_node, NULL, NULL },
	{ 204,	"eventMalf", { {204,0,0}, VARSMI_OID, 0 },          VARATTR_READ, m_eventMalf_node, NULL, NULL },
	{ 205,	"eventComm", { {205,0,0}, VARSMI_OID, 0 },          VARATTR_READ, m_eventComm_node, NULL, NULL },
	{ 206,	"Alarm", { {206,0,0}, VARSMI_OID, 0 },              VARATTR_READ, m_Alarm_node, NULL, NULL },
	{ 212,	"eventMeter", { {212,0,0}, VARSMI_OID, 0 },         VARATTR_READ, m_eventMeter_node, NULL, NULL },
    { 214,	"eventOta", { {214,0,0}, VARSMI_OID, 0 },           VARATTR_READ, m_eventOta_node, NULL, NULL },
    { 215,  "eventTransaction", { {215,0,0}, VARSMI_OID, 3 },   VARATTR_READ, m_eventTransaction_node, NULL, NULL },
	{ 255,  "eventManagement", { {255,0,0}, VARSMI_OID, 0 },    VARATTR_READ, m_eventManagement_node, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_smi_node[] =
{
	{   1,	"smiEntry", { {1,1,0}, VARSMI_OID, 3 },             VARATTR_RW, NULL, NULL, NULL },
	{   2,	"unknownEntry", { {1,2,0}, VARSMI_BYTE, 0 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"boolEntry", { {1,3,0}, VARSMI_BOOL, 1 },           VARATTR_RW, NULL, NULL, NULL },
	{   4,	"byteEntry", { {1,4,0}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, NULL },
	{   5,	"wordEntry", { {1,5,0}, VARSMI_WORD, 2 },           VARATTR_RW, NULL, NULL, NULL },
	{   6,	"uintEntry", { {1,6,0}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
	{   7,	"charEntry", { {1,7,0}, VARSMI_CHAR, 1 },           VARATTR_RW, NULL, NULL, NULL },
	{   8,	"shortEntry", { {1,8,0}, VARSMI_SHORT, 2 },         VARATTR_RW, NULL, NULL, NULL },
	{   9,	"intEntry", { {1,9,0}, VARSMI_INT, 4 },             VARATTR_RW, NULL, NULL, NULL },
	{  10,	"oidEntry", { {1,10,0}, VARSMI_OID, 3 },            VARATTR_RW, m_oidEntry_node, NULL, NULL },
	{  11,	"stringEntry", { {1,11,0}, VARSMI_STRING, 0 },      VARATTR_RW, NULL, NULL, NULL },
	{  12,	"streamEntry", { {1,12,0}, VARSMI_STREAM, 0 },      VARATTR_RW, NULL, NULL, NULL },
	{  13,	"opaqueEntry", { {1,13,0}, VARSMI_OPAQUE, 0 },      VARATTR_RW, NULL, NULL, NULL },
	{  14,	"eui64Entry", { {1,14,0}, VARSMI_EUI64, 8 },        VARATTR_RW, m_eui64Entry_node, NULL, NULL },
	{  15,	"ipEntry", { {1,15,0}, VARSMI_IPADDR, 4 },          VARATTR_RW, m_ipEntry_node, NULL, NULL },
	{  16,	"timeEntry", { {1,16,0}, VARSMI_TIMESTAMP, 7 },     VARATTR_RW, m_timeEntry_node, NULL, NULL },
	{  17,	"dayEntry", { {1,17,0}, VARSMI_TIMEDATE, 4 },       VARATTR_RW, m_dayEntry_node, NULL, NULL },
	{  18,	"verEntry", { {1,18,0}, VARSMI_STRING, 16 },        VARATTR_RW, m_verEntry_node, NULL, NULL },
	{  19,	"smivarEntry", { {1,19,0}, VARSMI_STREAM, 0 },      VARATTR_RW, m_smivarEntry_node, NULL, NULL },
	{  20,	"pageEntry", { {1,20,0}, VARSMI_STREAM, 0 },        VARATTR_RW, m_pageEntry_node, NULL, NULL },
	{  21,	"gmtEntry", { {1,21,0}, VARSMI_STREAM, 11 },        VARATTR_RW, m_gmtEntry_node, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_oidEntry_node[] =
{
	{   1,	"oidDot1", { {1,10,1}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{   2,	"oidDot2", { {1,10,2}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{   3,	"oidDot3", { {1,10,3}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_eui64Entry_node[] =
{
	{   1,	"eui64High", { {1,14,1}, VARSMI_UINT, 4 },          VARATTR_RW, NULL, NULL, NULL },
	{   2,	"eui64Low", { {1,14,2}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_ipEntry_node[] =
{
	{   1,	"ipDot1", { {1,15,1}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   2,	"ipDot2", { {1,15,2}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   3,	"ipDot3", { {1,15,3}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   4,	"ipDot4", { {1,15,4}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_timeEntry_node[] =
{
	{   1,	"timeYear", { {1,16,1}, VARSMI_WORD, 2 },           VARATTR_RW, NULL, NULL, NULL },
	{   2,	"timeMon", { {1,16,2}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{   3,	"timeDay", { {1,16,3}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{   4,	"timeHour", { {1,16,4}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, NULL },
	{   5,	"timeMin", { {1,16,5}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{   6,	"timeSec", { {1,16,6}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_dayEntry_node[] =
{
	{   1,	"dayYear", { {1,17,1}, VARSMI_WORD, 2 },            VARATTR_RW, NULL, NULL, NULL },
	{   2,	"dayMon", { {1,17,2}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   3,	"dayDay", { {1,17,3}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_verEntry_node[] =
{
	{   1,	"verPrefix", { {1,18,1}, VARSMI_STRING, 3 },        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"verFixed1", { {1,18,2}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
	{   3,	"verHwVersion", { {1,18,3}, VARSMI_STRING, 2 },     VARATTR_RW, NULL, NULL, NULL },
	{   4,	"verFixed2", { {1,18,4}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
	{   5,	"verSwVersion", { {1,18,5}, VARSMI_STRING, 4 },     VARATTR_RW, NULL, NULL, NULL },
	{   6,	"verFixed3", { {1,18,6}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
	{   7,	"verService", { {1,18,7}, VARSMI_STRING, 4 },       VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_smivarEntry_node[] =
{
	{   1,	"smivarOid", { {1,19,1}, VARSMI_OID, 3 },           VARATTR_RW, NULL, NULL, NULL },
	{   2,	"smivarLength", { {1,19,2}, VARSMI_WORD, 2 },       VARATTR_RW, NULL, NULL, NULL },
	{   3,	"smivarData", { {1,19,3}, VARSMI_STREAM, 0 },       VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_pageEntry_node[] =
{
	{   1,	"pageNumber", { {1,20,1}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, NULL },
	{   2,	"pageLength", { {1,20,2}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, NULL },
	{   3,	"pageData", { {1,20,3}, VARSMI_STREAM, 0 },         VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_gmtEntry_node[] =
{
	{   1,	"gmtTimezone", { {1,21,1}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"gmtDstValue", { {1,21,2}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{   3,	"gmtYear", { {1,21,3}, VARSMI_WORD, 2 },            VARATTR_RW, NULL, NULL, NULL },
	{   4,	"gmtMon", { {1,21,4}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   5,	"gmtDay", { {1,21,5}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   6,	"gmtHour", { {1,21,6}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{   7,	"gmtMin", { {1,21,7}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   8,	"gmtSec", { {1,21,8}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_mcu_node[] =
{
	{   1,	"sysEntry", 		{ {2,1,0}, VARSMI_OPAQUE, sizeof(SYSENTRY) },
								VARATTR_RW, m_sysEntry_node, NULL, NULL },
	{   2,	"varEntry", 		{ {2,2,0}, VARSMI_OPAQUE, sizeof(VARENTRY) },
								VARATTR_RW, m_varEntry_node, NULL, NULL },
	{   3,	"varValueEntry", 	{ {2,3,0}, VARSMI_OPAQUE, sizeof(VARVALUEENTRY) },
								VARATTR_RW, m_varValueEntry_node, NULL, NULL },
	{   4,	"varSubValueEntry", { {2,4,0}, VARSMI_OPAQUE, sizeof(VARSUBVALUEENTRY) },
								VARATTR_RW, m_varSubValueEntry_node, NULL, NULL },
	{   5,	"varGeEntry", 		{ {2,5,0}, VARSMI_OPAQUE, sizeof(VARGEENTRY) },
								VARATTR_RW, m_varGeEntry_node, NULL, NULL },
	{   6,	"varEvtCfgEntry", 	{ {2,6,0}, VARSMI_OPAQUE, sizeof(EVTCFGENTRY) },
								VARATTR_RW, m_varEvtCfgEntry_node, NULL, NULL },
	{  10,	"usrEntry", 		{ {2,10,0}, VARSMI_OPAQUE, sizeof(USRENTRY) },
								VARATTR_RW | VARATTR_LIST, m_usrEntry_node, NULL, NULL },
	{  11,	"gpsEntry", 		{ {2,11,0}, VARSMI_OPAQUE, sizeof(GPSENTRY) },
								VARATTR_RW | VARATTR_LIST, m_gpsEntry_node, NULL, NULL },
	{ 100,	"memEntry", 		{ {2,100,0}, VARSMI_OPAQUE, sizeof(MEMENTRY) },
								VARATTR_READ, m_memEntry_node, NULL, NULL },
	{ 101,	"flashEntry", 		{ {2,101,0}, VARSMI_OPAQUE, sizeof(FLASHENTRY) },
								VARATTR_READ, m_flashEntry_node, NULL, NULL },
	{ 102,	"ethEntry", 		{ {2,102,0}, VARSMI_OPAQUE, sizeof(ETHENTRY) },
								VARATTR_READ, m_ethEntry_node, NULL, NULL },
	{ 103,	"gpioEntry", 		{ {2,103,0}, VARSMI_OPAQUE, sizeof(GPIOENTRY) },
								VARATTR_READ, m_gpioEntry_node, NULL, NULL },
	{ 104,	"mobileEntry", 		{ {2,104,0}, VARSMI_OPAQUE, sizeof(MOBILEENTRY) },
								VARATTR_READ, m_mobileEntry_node, NULL, NULL },
	{ 105,	"pluginEntry", 		{ {2,105,0}, VARSMI_OPAQUE, sizeof(PLUGINENTRY) },
								VARATTR_READ | VARATTR_LIST, m_pluginEntry_node, NULL, NULL },
	{ 106,	"procEntry", 		{ {2,106,0}, VARSMI_OPAQUE, sizeof(PROCENTRY) },
								VARATTR_READ | VARATTR_LIST, m_procEntry_node, NULL, NULL },
	{ 107,	"pppEntry", 		{ {2,107,0}, VARSMI_OPAQUE, sizeof(PPPENTRY) },
								VARATTR_READ | VARATTR_LIST, m_pppEntry_node, NULL, NULL },
	{ 108,	"envEntry", 		{ {2,108,0}, VARSMI_OPAQUE, sizeof(ENVENTRY) },
								VARATTR_READ, m_envEntry_node, NULL, NULL },
	{ 109,	"gpioCtrlEntry", 	{ {2,109,0}, VARSMI_OPAQUE, sizeof(GPIOCTRLENTRY) },
								VARATTR_READ, m_gpioCtrlEntry_node, NULL, NULL },
	{ 110,	"patchEntry", 	    { {2,110,0}, VARSMI_OPAQUE, sizeof(PATCHENTRY) },
								VARATTR_READ | VARATTR_LIST, m_patchEntry_node, NULL, NULL },
    { 111,  "fwEntry",          { {2,111,0}, VARSMI_OPAQUE, sizeof(FWENTRY) },
                                VARATTR_READ | VARATTR_LIST, m_fwEntry_node, NULL, NULL },
	{ 200,	"commLogEntry", 	{ {2,200,0}, VARSMI_OPAQUE, sizeof(COMMLOGENTRY) },
								VARATTR_READ | VARATTR_LIST, m_commLogEntry_node, NULL, NULL },
	{ 201,	"commLogStruct", 	{ {2,201,0}, VARSMI_OPAQUE, sizeof(COMMLOGSTRUCT) },
								VARATTR_READ, m_commLogStruct_node, NULL, NULL },
	{ 202,	"cmdHistEntry", 	{ {2,202,0}, VARSMI_OPAQUE, sizeof(CMDHISTENTRY) },
								VARATTR_READ | VARATTR_LIST, m_cmdHistEntry_node, NULL, NULL },
	{ 203,	"cmdHistStruct", 	{ {2,203,0}, VARSMI_OPAQUE, sizeof(CMDHISTSTRUCT) },
								VARATTR_READ, m_cmdHistStruct_node, NULL, NULL },
	{ 204,	"mcuEventEntry", 	{ {2,204,0}, VARSMI_OPAQUE, sizeof(MCUEVENTENTRY) },
								VARATTR_READ | VARATTR_LIST, m_mcuEventEntry_node, NULL, NULL },
	{ 205,	"mcuEventStruct", 	{ {2,205,0}, VARSMI_OPAQUE, sizeof(MCUEVENTSTRUCT) },
								VARATTR_READ, m_mcuEventStruct_node, NULL, NULL },
	{ 206,	"meterLogEntry", 	{ {2,206,0}, VARSMI_OPAQUE, sizeof(METERLOGENTRY) },
								VARATTR_READ | VARATTR_LIST, m_meterLogEntry_node, NULL, NULL },
	{ 207,	"meterLogStruct", 	{ {2,207,0}, VARSMI_OPAQUE, sizeof(METERLOGSTRUCT) },
								VARATTR_READ, m_meterLogStruct_node, NULL, NULL },
	{ 208,	"mobileLogEntry", 	{ {2,208,0}, VARSMI_OPAQUE, sizeof(MOBILELOGENTRY) },
								VARATTR_READ | VARATTR_LIST, m_mobileLogEntry_node, NULL, NULL },
	{ 209,	"mobileLogStruct", 	{ {2,209,0}, VARSMI_OPAQUE, sizeof(MOBILELOGSTRUCT) },
								VARATTR_READ, m_mobileLogStruct_node, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_sysEntry_node[] =
{
	{   1,	"sysMask", { {2,1,1}, VARSMI_UINT, 4 },                 VARATTR_RW, NULL, NULL, NULL },
	{   2,	"sysID", { {2,1,2}, VARSMI_UINT, 4 },                   VARATTR_RW, NULL, NULL, NULL },
	{   3,	"sysType", { {2,1,3}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
	{   4,	"sysName", { {2,1,4}, VARSMI_STRING, 64 },              VARATTR_RW, NULL, NULL, NULL },
	{   5,	"sysDescr", { {2,1,5}, VARSMI_STRING, 64 },             VARATTR_RW, NULL, NULL, NULL },
	{   6,	"sysLocation", { {2,1,6}, VARSMI_STRING, 64 },          VARATTR_RW, NULL, NULL, NULL },
	{   7,	"sysContact", { {2,1,7}, VARSMI_STRING, 64 },           VARATTR_RW, NULL, NULL, NULL },
	{   8,	"sysHwVersion", { {2,1,8}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{   9,	"sysSwVersion", { {2,1,9}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{  10,	"sysPhoneNumber", { {2,1,10}, VARSMI_STRING, 16 },      VARATTR_RW, NULL, NULL, NULL },
	{  11,	"sysEtherType", { {2,1,11}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, NULL },
	{  12,	"sysMobileType", { {2,1,12}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
	{  13,	"sysMobileMode", { {2,1,13}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
	{  14,	"sysMobileAPN", { {2,1,14}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, NULL },
	{  15,	"sysUpTime", { {2,1,15}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, NULL },
	{  16,	"sysTime", { {2,1,16}, VARSMI_TIMESTAMP, 7 },           VARATTR_RW, NULL, NULL, NULL },
	{  17,	"sysCurTemp", { {2,1,17}, VARSMI_INT, 4 },              VARATTR_READ, NULL, NULL, NULL },
	{  18,	"sysMinTemp", { {2,1,18}, VARSMI_INT, 4 },              VARATTR_RW, NULL, NULL, NULL },
	{  19,	"sysMaxTemp", { {2,1,19}, VARSMI_INT, 4 },              VARATTR_RW, NULL, NULL, NULL },
	{  20,	"sysServer", { {2,1,20}, VARSMI_STRING, 256 },          VARATTR_RW, NULL, NULL, NULL },
	{  21,	"sysServerPort", { {2,1,21}, VARSMI_UINT, 4 },          VARATTR_RW, NULL, NULL, NULL },
	{  22,	"sysServerAlarmPort", { {2,1,22}, VARSMI_UINT, 4 },     VARATTR_RW, NULL, NULL, NULL },
	{  23,	"sysLocalPort", { {2,1,23}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
	{  24,	"sysState", { {2,1,24}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, NULL },
	{  25,	"sysVendor", { {2,1,25}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, NULL },
	{  26,	"sysModel", { {2,1,26}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, NULL },
	{  27,	"sysCircuitMode", { {2,1,27}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, NULL },
	{  28,	"sysMobileVendor", { {2,1,28}, VARSMI_UINT, 4 },        VARATTR_RW, NULL, NULL, NULL },
	{  29,	"sysMobileAccessPointName", { {2,1,29}, VARSMI_STRING, 64 },VARATTR_RW, NULL, NULL, NULL },
	{  30,	"sysSwRevision", { {2,1,30}, VARSMI_STRING, 32 },       VARATTR_READ, NULL, NULL, NULL },
	{  31,	"sysResetReason", { {2,1,31}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, NULL },
	{  32,	"sysJoinNodeCount", { {2,1,32}, VARSMI_UINT, 4 },       VARATTR_READ, NULL, NULL, NULL },
	{  33,	"sysTimeZone", { {2,1,33}, VARSMI_SHORT, 2 },           VARATTR_RW, NULL, NULL, NULL },
	{  34,	"sysOpMode", { {2,1,34}, VARSMI_BYTE, 1 },              VARATTR_RW, NULL, NULL, NULL },
	{  35,	"sysPowerType", { {2,1,35}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
	{  36,	"sysStateMask", { {2,1,36}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_varEntry_node[] =
{
	{   1,	"varEnableMask", { {2,2,1}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
	{   2,	"varEnableRecovery", { {2,2,2}, VARSMI_BOOL, 1 },       VARATTR_RW, NULL, NULL, NULL },
	{   3,	"varEnableCmdHistLog", { {2,2,3}, VARSMI_BOOL, 1 },     VARATTR_RW, NULL, NULL, NULL },
	{   4,	"varEnableCommLog", { {2,2,4}, VARSMI_BOOL, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{   5,	"varEanbleAutoRegister", { {2,2,5}, VARSMI_BOOL, 1 },   VARATTR_RW, NULL, NULL, NULL },
	{   6,	"varEnableAutoReset", { {2,2,6}, VARSMI_BOOL, 1 },      VARATTR_RW, NULL, NULL, NULL },
	{   7,	"varEnableAutoTimeSync", { {2,2,7}, VARSMI_BOOL, 1 },   VARATTR_RW, NULL, NULL, NULL },
	{   8,	"varEnableAutoSinkReset", { {2,2,8}, VARSMI_BOOL, 1 },  VARATTR_RW, NULL, NULL, NULL },
	{   9,	"varEnableSubNetwork", { {2,2,9}, VARSMI_BOOL, 1 },     VARATTR_RW, NULL, NULL, NULL },
	{  10,	"varEnableMobileStaticLog", { {2,2,10}, VARSMI_BOOL, 1 },VARATTR_RW, NULL, NULL, NULL },
	{  11,	"varEnableAutoUpload", { {2,2,11}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, NULL },
	{  12,	"varEnableSecurity", { {2,2,12}, VARSMI_BOOL, 1 },      VARATTR_RW, NULL, NULL, NULL },
	{  13,	"varEnableMonitoring", { {2,2,13}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, NULL },
	{  14,  "varEnableKeepalive", { {2,2,14}, VARSMI_BOOL, 1 },     VARATTR_RW, NULL, NULL, NULL },
    {  15,  "varEnableGpsTimesync", { {2,2,15}, VARSMI_BOOL, 1 },   VARATTR_RW, NULL, NULL, NULL },
    {  16,  "varEnableTimeBroadcast", { {2,2,16}, VARSMI_BOOL, 1 }, VARATTR_RW, NULL, NULL, NULL },
    {  17,  "varEnableStatusMonitoring", { {2,2,17}, VARSMI_BOOL, 1 },VARATTR_RW, NULL, NULL, NULL },
    {  18,  "varEnableMemoryCheck", { {2,2,18}, VARSMI_BOOL, 1 },   VARATTR_RW, NULL, NULL, NULL },
    {  19,  "varEnableFlashCheck", { {2,2,19}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, NULL },
    {  20,  "varEnableMeterTimesync", { {2,2,20}, VARSMI_BOOL, 1 }, VARATTR_RW, NULL, NULL, NULL },
    {  21,  "varEnableMeterCheck", { {2,2,21}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, NULL },
    {  22,  "varEnableHeater", { {2,2,22}, VARSMI_BOOL, 1 },        VARATTR_RW, NULL, NULL, NULL },
    {  23,  "varEnableReadMeterEvent", { {2,2,23}, VARSMI_BOOL, 1 },VARATTR_RW, NULL, NULL, NULL },
    {  24,  "varEnableLowBattery", { {2,2,24}, VARSMI_BOOL, 1 },    VARATTR_RW, NULL, NULL, NULL },
    {  25,  "varEnableTestMode", { {2,2,25}, VARSMI_BOOL, 1 },      VARATTR_RW, NULL, NULL, NULL },
    {  26,  "varEnableDebugLog", { {2,2,26}, VARSMI_BOOL, 1 },      VARATTR_RW, NULL, NULL, NULL },
    {  27,  "varEnableMeterRecovery", { {2,2,27}, VARSMI_BOOL, 1 }, VARATTR_RW, NULL, NULL, NULL },
    {  28,  "varEnableHighRAM", { {2,2,28}, VARSMI_BOOL, 1 },       VARATTR_RW, NULL, NULL, NULL },
    {  29,  "varEnableGarbageCleaning",{ {2,2,29}, VARSMI_BOOL, 1 },VARATTR_RW, NULL, NULL, NULL},
	{   0,	NULL }
};

VAROBJECT	m_varValueEntry_node[] =
{
	{   1,	"varValueMask", { {2,3,1}, VARSMI_UINT, 4 },                    VARATTR_RW, NULL, NULL, NULL },
	{   2,	"varAutoResetCheckInterval", { {2,3,2}, VARSMI_UINT, 4 },       VARATTR_RW, NULL, NULL, NULL },
	{   3,	"varSysPowerOffDelay", { {2,3,3}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, NULL },
	{   4,	"varSysTempMonInterval", { {2,3,4}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
	{   5,	"varAutoTimeSyncInterval", { {2,3,5}, VARSMI_UINT, 4 },         VARATTR_RW, NULL, NULL, NULL },
	{   6,	"varSysMeteringThreadCount", { {2,3,6}, VARSMI_BYTE, 1 },       VARATTR_RW, NULL, NULL, NULL },
	{   7,	"varSinkPollingInterval", { {2,3,7}, VARSMI_UINT, 4 },          VARATTR_RW, NULL, NULL, NULL },
	{   8,	"varSinkResetInterval", { {2,3,8}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, NULL },
	{   9,	"varSinkResetDelay", { {2,3,9}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, NULL },
	{  10,	"varSinkLedTurnOffInterval", { {2,3,10}, VARSMI_UINT, 4 },      VARATTR_RW, NULL, NULL, NULL },
	{  11,	"varSinkAckWaitTime", { {2,3,11}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{  12,	"varSensorTimeout", { {2,3,12}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, NULL },
	{  13,	"varSensorKeepalive", { {2,3,13}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{  14,	"varSensorMeterSaveCount", { {2,3,14}, VARSMI_WORD, 2 },        VARATTR_RW, NULL, NULL, NULL },
	{  15,	"varMeterDayMask", { {2,3,15}, VARSMI_UINT, 4 },                VARATTR_RW, NULL, NULL, NULL },
	{  16,	"varMeterHourMask", { {2,3,16}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, NULL },
	{  17,	"varMeterStartMin", { {2,3,17}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, NULL },
	{  18,	"varRecoveryDayMask", { {2,3,18}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, NULL },
	{  19,	"varRecoveryHourMask", { {2,3,19}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, NULL },
	{  20,	"varRecoveryStartMin", { {2,3,20}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{  21,	"varCmdHistSaveDay", { {2,3,21}, VARSMI_BYTE, 1 },              VARATTR_RW, NULL, NULL, NULL },
	{  22,	"varEventLogSaveDay", { {2,3,22}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{  23,	"varCommLogSaveDay", { {2,3,23}, VARSMI_BYTE, 1 },              VARATTR_RW, NULL, NULL, NULL },
	{  24,	"varMeterLogSaveDay", { {2,3,24}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{  25,	"varAutoResetTime", { {2,3,25}, VARSMI_TIMESTAMP, 7 },          VARATTR_RW, NULL, NULL, NULL },
	{  26,	"varMeterUploadCycleType", { {2,3,26}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{  27,	"varMeterUploadCycle", { {2,3,27}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, NULL },
	{  28,	"varMeterUploadStartHour", { {2,3,28}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{  29,	"varMeterUploadStartMin", { {2,3,29}, VARSMI_BYTE, 1 },         VARATTR_RW, NULL, NULL, NULL },
	{  30,	"varMeterUploadRetry", { {2,3,30}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{  31,	"varMeterIssueDate", { {2,3,31}, VARSMI_TIMESTAMP, 7 },         VARATTR_RW, NULL, NULL, NULL },
	{  32,	"varMemoryCriticalRate", { {2,3,32}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
	{  33,	"varFlashCriticalRate", { {2,3,33}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, NULL },
	{  34,	"varNapcGroupInterval", { {2,3,34}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, NULL },
	{  35,	"varNapcRetry", { {2,3,35}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, NULL },
	{  36,	"varNapcRetryHour", { {2,3,36}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, NULL },
	{  37,	"varNapcRetryStartSecond", { {2,3,37}, VARSMI_BYTE, 1 },        VARATTR_RW, NULL, NULL, NULL },
	{  38,	"varNapcRetryClear", { {2,3,38}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, NULL },
    {  39,  "varMaxEventLogSize", { {2,3,39}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, NULL },
    {  40,  "varMaxAlarmLogSize", { {2,3,40}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, NULL },
    {  41,  "varMaxCmdLogSize", { {2,3,41}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, NULL },
    {  42,  "varMaxCommLogSize", { {2,3,42}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, NULL },
    {  43,  "varMaxMobileLogSize", { {2,3,43}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, NULL },
    {  44,  "varKeepaliveInterval", { {2,3,44}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
    {  45,  "varAlarmLogSaveDay", { {2,3,45}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
    {  46,  "varTimeBroadcastInterval", { {2,3,46}, VARSMI_UINT, 4 },       VARATTR_RW, NULL, NULL, NULL },
    {  47,  "varStatusMonitorTime", { {2,3,47}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, NULL },
    {  48,  "varUploadTryTime", { {2,3,48}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, NULL },
    {  49,  "varDataSaveDay", { {2,3,49}, VARSMI_UINT, 4 },                 VARATTR_RW, NULL, NULL, NULL },
    {  50,  "varMeteringPeriod", { {2,3,50}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, NULL },
    {  51,  "varRecoveryPeriod", { {2,3,51}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, NULL },
    {  52,  "varMeteringRetry", { {2,3,52}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, NULL },
    {  53,  "varRecoveryRetry", { {2,3,53}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, NULL },
	{  54,	"varCheckDayMask", { {2,3,54}, VARSMI_UINT, 4 },                VARATTR_RW, NULL, NULL, NULL },
	{  55,	"varCheckHourMask", { {2,3,55}, VARSMI_UINT, 4 },               VARATTR_RW, NULL, NULL, NULL },
	{  56,	"varCheckStartMin", { {2,3,56}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, NULL },
    {  57,  "varCheckPeriod", { {2,3,57}, VARSMI_UINT, 4 },                 VARATTR_RW, NULL, NULL, NULL },
    {  58,  "varCheckRetry", { {2,3,58}, VARSMI_UINT, 4 },                  VARATTR_RW, NULL, NULL, NULL },
	{  59,	"varMeterTimesyncDayMask", { {2,3,59}, VARSMI_UINT, 4 },        VARATTR_RW, NULL, NULL, NULL },
	{  60,	"varMeterTimesyncHourMask", { {2,3,60}, VARSMI_UINT, 4 },       VARATTR_RW, NULL, NULL, NULL },
	{  61,	"varMeterReadingInterval", { {2,3,61}, VARSMI_UINT, 4 },        VARATTR_RW, NULL, NULL, NULL },
	{  62,	"varHeaterOnTemp", { {2,3,62}, VARSMI_INT, 4 },                 VARATTR_RW, NULL, NULL, NULL },
	{  63,	"varHeaterOffTemp", { {2,3,63}, VARSMI_INT, 4 },                VARATTR_RW, NULL, NULL, NULL },
	{  64,	"varMobileLiveCheckInterval", { {2,3,64}, VARSMI_INT, 4 },      VARATTR_RW, NULL, NULL, NULL },
	{  65,	"varEventReadDayMask", { {2,3,65}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, NULL },
	{  66,	"varEventReadHourMask", { {2,3,66}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
	{  67,	"varEventSendDelay", { {2,3,67}, VARSMI_UINT, 4 },              VARATTR_RW, NULL, NULL, NULL },
	{  68,	"varEventAlertLevel", { {2,3,68}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{  69,	"varSensorLimit", { {2,3,69}, VARSMI_INT, 4 },                  VARATTR_RW, NULL, NULL, NULL },
	{  70,	"varMeteringStrategy", { {2,3,70}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{  71,	"varTimesyncThreshold", { {2,3,71}, VARSMI_INT, 4 },            VARATTR_RW, NULL, NULL, NULL },
	{  72,	"varMobileLiveCheckMethod", { {2,3,72}, VARSMI_BYTE, 1 },       VARATTR_RW, NULL, NULL, NULL },
	{  73,	"varScanningStrategy", { {2,3,73}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{  74,	"varMobileLogSaveDay", { {2,3,74}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{  75,	"varUpgradeLogSaveDay", { {2,3,75}, VARSMI_BYTE, 1 },           VARATTR_RW, NULL, NULL, NULL },
	{  76,	"varUploadLogSaveDay", { {2,3,76}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
	{  77,	"varTimesyncLogSaveDay", { {2,3,77}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
    {  78,  "varMaxMeterLogSize", { {2,3,78}, VARSMI_UINT, 4 },             VARATTR_RW, NULL, NULL, NULL },
    {  79,  "varMaxUpgradeLogSize", { {2,3,79}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
    {  80,  "varMaxUploadLogSize", { {2,3,80}, VARSMI_UINT, 4 },            VARATTR_RW, NULL, NULL, NULL },
    {  81,  "varMaxTimesyncLogSize", { {2,3,81}, VARSMI_UINT, 4 },          VARATTR_RW, NULL, NULL, NULL },
    {  82,  "varDefaultGeMeteringOption", { {2,3,82}, VARSMI_BYTE, 1 },     VARATTR_RW, NULL, NULL, NULL },
    {  83,  "varSensorCleaningThreshold", { {2,3,83}, VARSMI_BYTE, 1 },     VARATTR_RW, NULL, NULL, NULL },
    {  84,  "varTimeSyncStrategy", { {2,3,84}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
    {  85,  "varTransactionSaveDay", { {2,3,85}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_varSubValueEntry_node[] =
{
	{   1,	"varSubValueMask", { {2,4,1}, VARSMI_UINT, 4 },                 VARATTR_RW, NULL, NULL, NULL },
	{   2,	"varSubChannel", { {2,4,2}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, NULL },
	{   3,	"varSubPanID", { {2,4,3}, VARSMI_WORD, 2 },                     VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_varGeEntry_node[] =
{
	{   1,	"varGeMeteringInterval", { {2,5,1}, VARSMI_UINT, 4 },           VARATTR_RW, NULL, NULL, NULL },
	{   2,	"varGeUser", { {2,5,2}, VARSMI_STREAM, 10 },                    VARATTR_RW, NULL, NULL, NULL },
	{   3,	"varGeMeterSecurity", { {2,5,3}, VARSMI_STREAM, 20 },           VARATTR_RW, NULL, NULL, NULL },
	{   4,	"varGeAllUploadHour", { {2,5,4}, VARSMI_BYTE, 1 },              VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_varEvtCfgEntry_node[] =
{
	{   1,	"varEvtCfgOid", { {2,6,1}, VARSMI_STREAM, 16 },                 VARATTR_READ, NULL, NULL, NULL },
	{   2,	"varEvtCfgEvent", { {2,6,2}, VARSMI_STREAM, 64 },               VARATTR_READ, NULL, NULL, NULL },
	{   3,	"varEvtCfgSeverity", { {2,6,3}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, NULL },
	{   4,	"varEvtCfgDefaultSeverity", { {2,6,4}, VARSMI_BYTE, 1 },        VARATTR_READ, NULL, NULL, NULL },
	{   5,	"varEvtCfgNotify", { {2,6,4}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_usrEntry_node[] =
{
	{   1,	"usrMask", { {2,10,1}, VARSMI_BYTE, 1 },                        VARATTR_RW, NULL, NULL, NULL },
	{   2,	"usrAccount", { {2,10,2}, VARSMI_STRING, 16 },                  VARATTR_RW, NULL, NULL, NULL },
	{   3,	"usrPassword", { {2,10,3}, VARSMI_STRING, 16 },                 VARATTR_RW, NULL, NULL, NULL },
	{   4,	"usrGroup", { {2,10,4}, VARSMI_BYTE, 1 },                       VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_gpsEntry_node[] =
{
	{   1,	"gpsFixPosition", { {2,11,1}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, NULL },
	{   2,	"gpsCount", { {2,11,2}, VARSMI_BYTE, 1 },                       VARATTR_RW, NULL, NULL, NULL },
	{   3,	"gpsTime", { {2,11,3}, VARSMI_TIMESTAMP, 7 },                   VARATTR_RW, NULL, NULL, NULL },
	{   4,	"gpsPos", { {2,11,4}, VARSMI_OPAQUE, 32 },                      VARATTR_RW, NULL, NULL, NULL },
	{   5,	"gpsTimezone", { {2,11,5}, VARSMI_INT, 4 },                     VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_memEntry_node[] =
{
	{   1,	"memTotalSize", { {2,100,1}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, NULL },
	{   2,	"memFreeSize", { {2,100,2}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, NULL },
	{   3,	"memUseSize", { {2,100,3}, VARSMI_UINT, 4 },                VARATTR_READ, NULL, NULL, NULL },
	{   4,	"memCacheSize", { {2,100,4}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, NULL },
	{   5,	"memBufferSize", { {2,100,5}, VARSMI_UINT, 4 },             VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_flashEntry_node[] =
{
	{   1,	"flashTotalSize", { {2,101,1}, VARSMI_UINT, 4 },            VARATTR_READ, NULL, NULL, NULL },
	{   2,	"flashFreeSize", { {2,101,2}, VARSMI_UINT, 4 },             VARATTR_READ, NULL, NULL, NULL },
	{   3,	"flashUseSize", { {2,101,3}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_ethEntry_node[] =
{
	{   1,	"ethName", { {2,102,1}, VARSMI_STRING, 16 },                VARATTR_READ, NULL, NULL, NULL },
	{   2,	"ethPhyAddr", { {2,102,2}, VARSMI_STREAM, 6 },              VARATTR_READ, NULL, NULL, NULL },
	{   3,	"ethIpAddr", { {2,102,3}, VARSMI_IPADDR, 4 },               VARATTR_RW, NULL, NULL, NULL },
	{   4,	"ethSubnetMask", { {2,102,4}, VARSMI_IPADDR, 4 },           VARATTR_RW, NULL, NULL, NULL },
	{   5,	"ethGateway", { {2,102,5}, VARSMI_IPADDR, 4 },              VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_gpioEntry_node[] =
{
	{   1,	"gpioMask", { {2,103,1}, VARSMI_UINT, 4 },                  VARATTR_READ, NULL, NULL, NULL },
	{   2,	"gpioPowerFail", { {2,103,2}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{   3,	"gpioLowBattery", { {2,103,3}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{   4,	"gpioBUSY", { {2,103,4}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
	{   5,	"gpioSTBY", { {2,103,5}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
	{   6,	"gpioDCD", { {2,103,6}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, NULL },
	{   7,	"gpioRI", { {2,103,7}, VARSMI_BYTE, 1 },                    VARATTR_READ, NULL, NULL, NULL },
	{   8,	"gpioSink1State", { {2,103,8}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{   9,	"gpioSink2State", { {2,103,9}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{  10,	"gpioHeaterState", { {2,103,10}, VARSMI_BYTE, 1 },          VARATTR_READ, NULL, NULL, NULL },
	{  11,	"gpioCoverState", { {2,103,11}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, NULL },
	{  12,	"gpioCharginState", { {2,103,12}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_gpioCtrlEntry_node[] =
{
	{   1,	"gpioPowerCtrl", { {2,109,1}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{   2,	"gpioReset", 	 { {2,109,2}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{   3,	"gpioMobilePower", { {2,109,3}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, NULL },
	{   4,	"gpioMobileReset", { {2,109,4}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, NULL },
	{   5,	"gpioMobileDtr", { {2,109,5}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{   6,	"gpioSink1Power", { {2,109,6}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{   7,	"gpioSink2Power", { {2,109,7}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{   8,	"gpioExternalPower", { {2,109,8}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, NULL },
	{   9,	"gpioLedSink1", { {2,109,9}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, NULL },
	{  10,	"gpioLedSink2", { {2,109,10}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{  11,	"gpioLedReady", { {2,109,11}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{  12,	"gpioLedDebug1", { {2,109,12}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{  13,	"gpioLedDebug2", { {2,109,13}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{  14,	"gpioLedDebug3", { {2,109,14}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{  15,	"gpioLedDebug4", { {2,109,15}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{  16,	"gpioEmergencyOff", { {2,109,16}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, NULL },
	{  17,	"gpioCoordinatorReset", { {2,109,17}, VARSMI_BYTE, 1 },     VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_mobileEntry_node[] =
{
	{   1,	"mobileID", { {2,104,1}, VARSMI_STRING, 16 },               VARATTR_READ, NULL, NULL, NULL },
	{   2,	"mobileIpaddr", { {2,104,2}, VARSMI_IPADDR, 4 },            VARATTR_READ, NULL, NULL, NULL },
	{   3,	"mobileRxDbm", { {2,104,3}, VARSMI_INT, 4 },                VARATTR_READ, NULL, NULL, NULL },
	{   4,	"mobileTxDbm", { {2,104,4}, VARSMI_INT, 4 },                VARATTR_READ, NULL, NULL, NULL },
	{   5,	"mobilePacketLiveTime", { {2,104,5}, VARSMI_UINT, 4 },      VARATTR_READ, NULL, NULL, NULL },
	{   6,	"mobileSendBytes", { {2,104,6}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, NULL },
	{   7,	"mobileRecvBytes", { {2,104,7}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, NULL },
	{   8,	"mobileLastConnectTime", { {2,104,8}, VARSMI_TIMESTAMP, 7 },VARATTR_READ, NULL, NULL, NULL },
	{   9,	"mobileSimCardId", { {2,104,9}, VARSMI_STRING, 7 },         VARATTR_READ, NULL, NULL, NULL },
	{  10,	"mobileVolt", { {2,104,10}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_pluginEntry_node[] =
{
	{   1,	"pluginName", { {2,105,1}, VARSMI_STRING, 16 },             VARATTR_READ, NULL, NULL, NULL },
	{   2,	"pluginFileName", { {2,105,2}, VARSMI_STRING, 32 },         VARATTR_READ, NULL, NULL, NULL },
	{   3,	"pluginDescr", { {2,105,3}, VARSMI_STRING, 64 },            VARATTR_READ, NULL, NULL, NULL },
	{   4,	"pluginType", { {2,105,4}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, NULL },
	{   5,	"pluginState", { {2,105,5}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, NULL },
	{   6,	"pluginVersion", { {2,105,6}, VARSMI_STRING, 8 },           VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_procEntry_node[] =
{
	{   1,	"procID", { {2,106,1}, VARSMI_UINT, 4 },                    VARATTR_READ, NULL, NULL, NULL },
	{   2,	"procTTY", { {2,106,2}, VARSMI_STRING, 8 },                 VARATTR_READ, NULL, NULL, NULL },
	{   3,	"procSize", { {2,106,3}, VARSMI_UINT, 4 },                  VARATTR_READ, NULL, NULL, NULL },
	{   4,	"procState", { {2,106,4}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
	{   5,	"procCmd", { {2,106,5}, VARSMI_STRING, 50 },                VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_pppEntry_node[] =
{
	{   1,	"pppProvider", { {2,107,1}, VARSMI_STRING, 64 },            VARATTR_READ, NULL, NULL, NULL },
	{   2,	"pppUser", { {2,107,2}, VARSMI_STRING, 32 },                VARATTR_RW, NULL, NULL, NULL },
	{   3,	"pppPassword", { {2,107,3}, VARSMI_STRING, 32 },            VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_envEntry_node[] =
{
	{   1,	"envKernelVersion", { {2,108,1}, VARSMI_STRING, 64 },       VARATTR_READ, NULL, NULL, NULL },
	{   2,	"envGccVersion", { {2,108,2}, VARSMI_STRING, 64 },          VARATTR_READ, NULL, NULL, NULL },
	{   3,	"envCpuName", { {2,108,3}, VARSMI_STRING, 64 },             VARATTR_READ, NULL, NULL, NULL },
	{   4,	"envCpuMips", { {2,108,4}, VARSMI_UINT, 4 },                VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_patchEntry_node[] =
{
	{   1,	"patchID", { {2,110,1}, VARSMI_UINT, 4 },                   VARATTR_READ, NULL, NULL, NULL },
	{   2,	"patchIssueNumber", { {2,110,2}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, NULL },
	{   3,	"patchState", { {2,110,3}, VARSMI_BOOL, 1 },                VARATTR_RW,   NULL, NULL, NULL },
	{   4,	"patchDescr", { {2,110,4}, VARSMI_STRING, 128 },            VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT    m_fwEntry_node[] =
{
    {   1,    "fwType", { {2,111,1}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
    {   2,    "fwCore", { {2,111,2}, VARSMI_STRING, 64 },               VARATTR_READ, NULL, NULL, NULL },
    {   3,    "fwHardware", { {2,111,3}, VARSMI_WORD, 2 },              VARATTR_READ, NULL, NULL, NULL },
    {   4,    "fwSoftware", { {2,111,4}, VARSMI_WORD, 2 },              VARATTR_READ, NULL, NULL, NULL },
    {   5,    "fwBuild", { {2,111,5}, VARSMI_WORD, 2 },                 VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT	m_commLogEntry_node[] =
{
	{   1,	"commLogDate", { {2,200,1}, VARSMI_TIMESTAMP, 7 },          VARATTR_READ, NULL, NULL, NULL },
	{   2,	"commLogFileName", { {2,200,2}, VARSMI_STRING, 32 },        VARATTR_READ, NULL, NULL, NULL },
	{   3,	"commLogFileSize", { {2,200,3}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_commLogStruct_node[] =
{
	{   1,	"commLogLength", { {2,201,1}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{   2,	"commLogMin", { {2,201,2}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, NULL },
	{   3,	"commLogSec", { {2,201,3}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, NULL },
	{   4,	"commLogData", { {2,201,4}, VARSMI_STREAM, 0 },             VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_cmdHistEntry_node[] =
{
	{   1,	"cmdHistDate", { {2,202,1}, VARSMI_TIMESTAMP, 7 },          VARATTR_READ, NULL, NULL, NULL },
	{   2,	"cmdHistFileName", { {2,202,2}, VARSMI_STRING, 32 },        VARATTR_READ, NULL, NULL, NULL },
	{   3,	"cmdHistFileSize", { {2,202,3}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_cmdHistStruct_node[] =
{
	{   1,	"cmdHistLength", { {2,203,1}, VARSMI_WORD, 2 },             VARATTR_READ, NULL, NULL, NULL },
	{   2,	"cmdHistTimeHour", { {2,203,2}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, NULL },
	{   3,	"cmdHistTimeMin", { {2,203,3}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{   4,	"cmdHistTimeSec", { {2,203,4}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{   5,	"cmdHistOwner", { {2,203,5}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, NULL },
	{   6,	"cmdHistUser", { {2,203,6}, VARSMI_STRING, 16 },            VARATTR_READ, NULL, NULL, NULL },
	{   7,	"cmdHistData", { {2,203,7}, VARSMI_STREAM, 0 },             VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_mcuEventEntry_node[] =
{
	{   1,	"mcuEvnetDate", { {2,204,1}, VARSMI_TIMESTAMP, 7 },         VARATTR_READ, NULL, NULL, NULL },
	{   2,	"mcuEventFileName", { {2,204,2}, VARSMI_STRING, 32 },       VARATTR_READ, NULL, NULL, NULL },
	{   3,	"mcuEventFileSize", { {2,204,3}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_mcuEventStruct_node[] =
{
	{   1,	"mcuEventLength", { {2,205,1}, VARSMI_WORD, 2 },            VARATTR_READ, NULL, NULL, NULL },
	{   2,	"mcuEventData", { {2,205,2}, VARSMI_STREAM, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_meterLogEntry_node[] =
{
	{   1,	"meterLogDate", { {2,206,1}, VARSMI_TIMESTAMP, 7 },         VARATTR_READ, NULL, NULL, NULL },
	{   2,	"meterLogFileName", { {2,206,2}, VARSMI_STRING, 32 },       VARATTR_READ, NULL, NULL, NULL },
	{   3,	"meterLogFileSize", { {2,206,3}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_meterLogStruct_node[] =
{
	{   1,	"meterLogID", { {2,207,1}, VARSMI_EUI64, 8 },               VARATTR_READ, NULL, NULL, NULL },
	{   2,	"meterLogType", { {2,207,2}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, NULL },
	{   3,	"meterLogTime", { {2,207,3}, VARSMI_TIMESTAMP, 7 },         VARATTR_READ, NULL, NULL, NULL },
	{   4,	"meterLogStartTime", { {2,207,4}, VARSMI_TIMESTAMP, 7 },    VARATTR_READ, NULL, NULL, NULL },
	{   5,	"meterLogEndTime", { {2,207,5}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
	{   6,	"meterLogResult", { {2,207,6}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{   7,	"meterLogElapse", { {2,207,7}, VARSMI_UINT, 4 },            VARATTR_READ, NULL, NULL, NULL },
	{   8,	"meterLogTryCount", { {2,207,8}, VARSMI_BYTE, 1 },          VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_mobileLogEntry_node[] =
{
	{   1,	"mobileLogDate", { {2,208,1}, VARSMI_TIMEDATE, 4 },         VARATTR_READ, NULL, NULL, NULL },
	{   2,	"mobileLogFileName", { {2,208,2}, VARSMI_STRING, 32 },      VARATTR_READ, NULL, NULL, NULL },
	{   3,	"mobileLogFileSize", { {2,208,3}, VARSMI_UINT, 4 },         VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_mobileLogStruct_node[] =
{
	{   0,	NULL }
};

VAROBJECT	m_sink_node[] =
{
	{   1,	"sinkEntry", { {3,1,0}, VARSMI_OPAQUE, 63 }, 
                        VARATTR_RW | VARATTR_LIST, m_sinkEntry_node, NULL, NULL },
	{   2,	"sinkStaticEntry", { {3,2,0}, VARSMI_OPAQUE, 39 },          
                        VARATTR_READ, m_sinkStaticEntry_node, NULL, NULL },
    {   3,  "codiEntry", { {3,3,0}, VARSMI_OPAQUE, sizeof(CODIENTRY) }, 
                        VARATTR_RW | VARATTR_LIST, m_codiEntry_node, NULL, NULL },
    {   4,  "codiDeviceEntry", { {3,4,0}, VARSMI_OPAQUE, sizeof(CODIDEVICEENTRY) }, 
                        VARATTR_RW | VARATTR_LIST, m_codiDeviceEntry_node, NULL, NULL },
    {   5,  "codiBindingEntry", { {3,5,0}, VARSMI_OPAQUE, sizeof(CODIBINDINGENTRY) },         
                        VARATTR_READ | VARATTR_LIST, m_codiBindingEntry_node, NULL, NULL },
    {   6,  "codiNeighborEntry", { {3,6,0}, VARSMI_OPAQUE, sizeof(CODINEIGHBORENTRY) },        
                        VARATTR_READ | VARATTR_LIST, m_codiNeighborEntry_node, NULL, NULL },
    {   7,  "codiMemoryEntry", { {3,7,0}, VARSMI_OPAQUE, sizeof(CODIMEMORYENTRY) },           
                        VARATTR_RW | VARATTR_LIST, m_codiMemoryEntry_node, NULL, NULL },
    {   8,  "codiRegisterEntry", { {3,8,0}, VARSMI_OPAQUE, sizeof(CODIREGENTRY) },           
                        VARATTR_RW | VARATTR_LIST, m_codiRegEntry_node, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_sinkEntry_node[] =
{
	{   1,	"sinkMask", { {3,1,1}, VARSMI_BYTE, 1 },                    VARATTR_RW, NULL, NULL, NULL },
	{   2,	"sinkIndex", { {3,1,2}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, NULL },
	{   3,	"sinkID", { {3,1,3}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, NULL },
	{   4,	"sinkDevice", { {3,1,4}, VARSMI_STRING, 16 },               VARATTR_READ, NULL, NULL, NULL },
	{   5,	"sinkType", { {3,1,5}, VARSMI_BYTE, 1 },                    VARATTR_READ, NULL, NULL, NULL },
	{   6,	"sinkMaxBind", { {3,1,6}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
	{   7,	"sinkChannel", { {3,1,7}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, NULL },
	{   8,	"sinkPanID", { {3,1,8}, VARSMI_WORD, 2 },                   VARATTR_RW, NULL, NULL, NULL },
	{   9,	"sinkNeighborNode", { {3,1,9}, VARSMI_BYTE, 1 },            VARATTR_READ, NULL, NULL, NULL },
	{  10,	"sinkRfPower", { {3,1,10}, VARSMI_BYTE, 1 },                VARATTR_RW, NULL, NULL, NULL },
	{  11,	"sinkBaudrate", { {3,1,11}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, NULL },
	{  12,	"sinkState", { {3,1,12}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
	{  13,	"sinkMaxActiveCount", { {3,1,13}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, NULL },
	{  14,	"sinkResetKind", { {3,1,14}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, NULL },
	{  15,	"sinkPC", { {3,1,15}, VARSMI_WORD, 2 },                     VARATTR_READ, NULL, NULL, NULL },
	{  16,	"sinkLastConnect", { {3,1,16}, VARSMI_TIMESTAMP, 7 },       VARATTR_READ, NULL, NULL, NULL },
	{  17,	"sinkLastDisconnect", { {3,1,17}, VARSMI_TIMESTAMP, 7 },    VARATTR_READ, NULL, NULL, NULL },
	{  18,	"sinkLastReset", { {3,1,18}, VARSMI_TIMESTAMP, 7 },         VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_sinkStaticEntry_node[] =
{
	{   1,	"sinkStaticIndex", { {3,2,1}, VARSMI_BYTE, 1 },             VARATTR_RW, NULL, NULL, NULL },
	{   2,	"sinkRecvBytes", { {3,2,2}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, NULL },
	{   3,	"sinkSendBytes", { {3,2,3}, VARSMI_UINT, 4 },               VARATTR_READ, NULL, NULL, NULL },
	{   4,	"sinkErrorBytes", { {3,2,4}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, NULL },
	{   5,	"sinkRecvPacket", { {3,2,5}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, NULL },
	{   6,	"sinkSendPacket", { {3,2,6}, VARSMI_UINT, 4 },              VARATTR_READ, NULL, NULL, NULL },
	{   7,	"sinkTimeoutPacket", { {3,2,7}, VARSMI_UINT, 4 },           VARATTR_READ, NULL, NULL, NULL },
	{   8,	"sinkSeqErrorPacket", { {3,2,8}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, NULL },
	{   9,	"sinkCrcErrorPacket", { {3,2,9}, VARSMI_UINT, 4 },          VARATTR_READ, NULL, NULL, NULL },
	{  10,	"sinkSendDropPacket", { {3,2,10}, VARSMI_UINT, 4 },         VARATTR_READ, NULL, NULL, NULL },
	{  11,	"sinkResetCount", { {3,2,11}, VARSMI_WORD, 2 },             VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT   m_codiEntry_node[] =
{
    {   1,  "codiMask", { {3,3,1}, VARSMI_WORD, 2 },                    VARATTR_RW, NULL, NULL, NULL },
    {   2,  "codiIndex", { {3,3,2}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, NULL },
    {   3,  "codiID", { {3,3,3}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, NULL },
    {   4,  "codiType", { {3,3,4}, VARSMI_BYTE, 1 },                    VARATTR_READ, NULL, NULL, NULL },
    {   5,  "codiShortID", { {3,3,5}, VARSMI_WORD, 2 },                 VARATTR_READ, NULL, NULL, NULL },
    {   6,  "codiFwVer", { {3,3,6}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, NULL },
    {   7,  "codiHwVer", { {3,3,7}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, NULL },
    {   8,  "codiZAIfVer", { {3,3,8}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
    {   9,  "codiZZIfVer", { {3,3,9}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
    {  10,  "codiFwBuild", { {3,3,10}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, NULL },
    {  11,  "codiResetKind", { {3,3,11}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, NULL },
    {  12,  "codiAutoSetting", { {3,3,12}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
    {  13,  "codiChannel", { {3,3,13}, VARSMI_BYTE, 1 },                VARATTR_RW, NULL, NULL, NULL },
    {  14,  "codiPanID", { {3,3,14}, VARSMI_WORD, 2 },                  VARATTR_RW, NULL, NULL, NULL },
    {  15,  "codiExtPanID", { {3,3,15}, VARSMI_STREAM, 8 },             VARATTR_RW, NULL, NULL, NULL },
    {  16,  "codiRfPower", { {3,3,16}, VARSMI_CHAR, 1 },                VARATTR_RW, NULL, NULL, NULL },
    {  17,  "codiTxPowerMode", { {3,3,17}, VARSMI_BYTE, 1 },            VARATTR_RW, NULL, NULL, NULL },
    {  18,  "codiPermit", { {3,3,18}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, NULL },
    {  19,  "codiEnableEncrypt", { {3,3,19}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
    {  20,  "codiLineKey", { {3,3,20}, VARSMI_STREAM, 16 },             VARATTR_RW, NULL, NULL, NULL },
    {  21,  "codiNetworkKey", { {3,3,21}, VARSMI_STREAM, 16 },          VARATTR_RW, NULL, NULL, NULL },
    {  22,  "codiRouteDiscovery", { {3,3,22}, VARSMI_BOOL, 1 },         VARATTR_RW, NULL, NULL, NULL },
    {  23,  "codiMulticastHops", { {3,3,23}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
    {   0,  NULL }
};

VAROBJECT   m_codiDeviceEntry_node[] =
{
    {   1,  "codiIndex", { {3,4,1}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, NULL },
    {   2,  "codiID", { {3,4,2}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, NULL },
    {   3,  "codiDevice", { {3,4,3}, VARSMI_STRING, 16 },               VARATTR_READ, NULL, NULL, NULL },
    {   4,  "codiBaudRate", { {3,4,4}, VARSMI_BYTE, 1 },                VARATTR_RW, NULL, NULL, NULL },
    {   5,  "codiParityBit", { {3,4,5}, VARSMI_BYTE, 1 },               VARATTR_RW, NULL, NULL, NULL },
    {   6,  "codiDataBit", { {3,4,6}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, NULL },
    {   7,  "codiStopBit", { {3,4,7}, VARSMI_BYTE, 1 },                 VARATTR_RW, NULL, NULL, NULL },
    {   8,  "codiRtsCts", { {3,4,8}, VARSMI_BYTE, 1 },                  VARATTR_RW, NULL, NULL, NULL },
    {   0,  NULL }
};

VAROBJECT   m_codiBindingEntry_node[] =
{
    {   1,  "codiBindIndex", { {3,5,1}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, NULL },
    {   2,  "codiBindType", { {3,5,2}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, NULL },
    {   3,  "codiBindLocal", { {3,5,3}, VARSMI_BYTE, 1 },               VARATTR_READ, NULL, NULL, NULL },
    {   4,  "codiBindRemote", { {3,5,4}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, NULL },
    {   5,  "codiBindID", { {3,5,5}, VARSMI_EUI64, 8 },                 VARATTR_READ, NULL, NULL, NULL },
    {   6,  "codiLastHeard", { {3,5,6}, VARSMI_WORD, 2 },               VARATTR_READ, NULL, NULL, NULL },
    {   0,  NULL }
};

VAROBJECT   m_codiNeighborEntry_node[] =
{
    {   1,  "codiNeighborIndex", { {3,6,1}, VARSMI_BYTE, 1 },           VARATTR_READ, NULL, NULL, NULL },
    {   2,  "codiNeighborShortID", { {3,6,2}, VARSMI_WORD, 2 },         VARATTR_READ, NULL, NULL, NULL },
    {   3,  "codiNeighborLqi", { {3,6,3}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
    {   4,  "codiNeighborInCost", { {3,6,4}, VARSMI_BYTE, 1 },          VARATTR_READ, NULL, NULL, NULL },
    {   5,  "codiNeighborOutCost", { {3,6,5}, VARSMI_BYTE, 1 },         VARATTR_READ, NULL, NULL, NULL },
    {   6,  "codiNeighborAge", { {3,6,6}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
    {   7,  "codiNeighborID", { {3,6,7}, VARSMI_EUI64, 8 },             VARATTR_READ, NULL, NULL, NULL },
    {   0,  NULL }
};

VAROBJECT   m_codiMemoryEntry_node[] =
{
    {   1,  "codiIndex", { {3,7,1}, VARSMI_BYTE, 1 },                   VARATTR_RW, NULL, NULL, NULL },
    {   2,  "codiID", { {3,7,2}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, NULL },
    {   3,  "codiAddressTableSize", { {3,7,3}, VARSMI_BYTE, 1 },        VARATTR_READ, NULL, NULL, NULL },
    {   4,  "codiWholeAddressTableSize", { {3,7,4}, VARSMI_BYTE, 1 },   VARATTR_READ, NULL, NULL, NULL },
    {   5,  "codiNeighborTableSize", { {3,7,5}, VARSMI_BYTE, 1 },       VARATTR_READ, NULL, NULL, NULL },
    {   6,  "codiSourceRouteTableSize", { {3,7,6}, VARSMI_BYTE, 1 },    VARATTR_READ, NULL, NULL, NULL },
    {   7,  "codiRouteTableSize", { {3,7,7}, VARSMI_BYTE, 1 },          VARATTR_READ, NULL, NULL, NULL },
    {   8,  "codiMaxHops", { {3,7,8}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
    {   9,  "codiPacketBufferCount", { {3,7,9}, VARSMI_BYTE, 1 },       VARATTR_READ, NULL, NULL, NULL },
    {  10,  "codiSoftwareVersion", { {3,7,10}, VARSMI_WORD, 2 },       VARATTR_READ, NULL, NULL, NULL },
    {  11,  "codiKeyTableSize", { {3,7,11}, VARSMI_BYTE, 1 },       VARATTR_READ, NULL, NULL, NULL },
    {  12,  "codiMaxChildren", { {3,7,12}, VARSMI_BYTE, 1 },       VARATTR_READ, NULL, NULL, NULL },
    {   0,  NULL }
};

VAROBJECT   m_codiRegEntry_node[] =
{
    {   1,  "codiRegIndex", { {3,8,1}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, NULL },
    {   2,  "codiRegStats", { {3,8,2}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, NULL },
    {   3,  "codiID", { {3,8,3}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, NULL },
    {   0,  NULL }
};

VAROBJECT	m_sensor_node[] =
{
	{   1,	"sensorConnectEntry", { {4,1,0}, VARSMI_OPAQUE, 54 },       
                            VARATTR_READ | VARATTR_LIST, m_sensorConnectEntry_node, NULL, NULL },
	{   2,	"sensorInfoEntry", { {4,2,0}, VARSMI_OPAQUE, 42 },          
                            VARATTR_READ | VARATTR_LIST, m_sensorInfoEntry_node, NULL, NULL },
	{   3,	"sensorInfoNewEntry", { {4,3,0}, VARSMI_OPAQUE, sizeof(SENSORINFOENTRYNEW) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorInfoNewEntry_node, NULL, NULL },
	{   4,	"sensorPathEntry", { {4,4,0}, VARSMI_OPAQUE, sizeof(SENSORPATHENTRY) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorPathEntry_node, NULL, NULL },
    {   6,  "sensorEventEntry", { {4,6,0}, VARSMI_OPAQUE, sizeof(SENSOREVENTENTRY) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorEventEntry_node, NULL, NULL },
    {   7,  "sensorBatteryEntry", { {4,7,0}, VARSMI_OPAQUE, sizeof(SENSORBATTERYENTRY) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorBatteryEntry_node, NULL, NULL },
    {   8,  "sensorConstantEntry", { {4,8,0}, VARSMI_OPAQUE, sizeof(SENSORCONSTANTENTRY) }, 
                            VARATTR_READ | VARATTR_LIST, m_sensorConstantEntry_node, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_sensorConnectEntry_node[] =
{
	{   1,	"sensorID", { {4,1,1}, VARSMI_EUI64, 8 },                   VARATTR_READ, NULL, NULL, NULL },
	{   2,	"sensorType", { {4,1,2}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
	{   3,	"sensorLastConnect", { {4,1,3}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
	{   4,	"sensorLastDisconnect", { {4,1,4}, VARSMI_TIMESTAMP, 7 },   VARATTR_READ, NULL, NULL, NULL },
	{   5,	"sensorInstallDate", { {4,1,5}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
	{   6,	"sensorPageCount", { {4,1,6}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{   7,	"sensorState", { {4,1,7}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
	{   8,	"sensorFlag", { {4,1,8}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
	{   9,	"sensorVendor", { {4,1,9}, VARSMI_BYTE, 1 },                VARATTR_RW, NULL, NULL, NULL },
	{  10,	"sensorSerialNumber", { {4,1,10}, VARSMI_STRING, 20 },      VARATTR_RW, NULL, NULL, NULL },
	{  11,  "sensorServiceType", { {4,1,11}, VARSMI_BYTE, 1 },          VARATTR_RW, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_sensorInfoEntry_node[] =
{
	{   1,	"sensorGroup", { {4,2,1}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
	{   2,	"sensorMember", { {4,2,2}, VARSMI_BYTE, 1 },                VARATTR_READ, NULL, NULL, NULL },
	{   3,	"sensorID", { {4,2,3}, VARSMI_EUI64, 8 },                   VARATTR_READ, NULL, NULL, NULL },
	{   4,	"sensorSerialNumber", { {4,2,4}, VARSMI_STRING, 18 },       VARATTR_READ, NULL, NULL, NULL },
	{   5,	"sensorLastConnect", { {4,2,5}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
	{   6,	"sensorInstallDate", { {4,2,6}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
	{   7,	"sensorAttr", { {4,2,7}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_sensorInfoNewEntry_node[] =
{
	{   1,	"sensorID", { {4,3,1}, VARSMI_EUI64, 8 },                   VARATTR_READ, NULL, NULL, NULL },
	{   2,	"sensorSerialNumber", { {4,3,2}, VARSMI_STREAM, 20 },       VARATTR_READ, NULL, NULL, NULL },
	{   3,	"sensorLastConnect", { {4,3,3}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
	{   4,	"sensorInstallDate", { {4,3,4}, VARSMI_TIMESTAMP, 7 },      VARATTR_READ, NULL, NULL, NULL },
	{   5,	"sensorAttr", { {4,3,5}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
	{   6,	"sensorState", { {4,3,6}, VARSMI_BYTE, 1 },                 VARATTR_READ, NULL, NULL, NULL },
	{   7,	"sensorModel", { {4,3,7}, VARSMI_STREAM, 18 },              VARATTR_READ, NULL, NULL, NULL },
	{   8,	"sensorHwVersion", { {4,3,8}, VARSMI_WORD, 2 },             VARATTR_READ, NULL, NULL, NULL },
	{   9,	"sensorFwVersion", { {4,3,9}, VARSMI_WORD, 2 },             VARATTR_READ, NULL, NULL, NULL },
	{  10,	"sensorOTAState", { {4,3,10}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{  11,	"sensorLastOTATime", { {4,3,11}, VARSMI_TIMESTAMP, 7 },     VARATTR_READ, NULL, NULL, NULL },
	{  12,	"sensorFwBuild", { {4,3,12}, VARSMI_WORD, 2 },              VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_sensorPathEntry_node[] =
{
	{   1,	"sensorPathID", 	 { {4,4,1}, VARSMI_EUI64, 	   8 },     VARATTR_READ, NULL, NULL, NULL },
	{   2,	"sensorPathSerial",  { {4,4,2}, VARSMI_STREAM,    20 },     VARATTR_READ, NULL, NULL, NULL },
	{   3,	"sensorPathModel", 	 { {4,4,3}, VARSMI_STREAM,    18 },     VARATTR_READ, NULL, NULL, NULL },
	{   4,	"sensorPathShortID", { {4,4,4}, VARSMI_WORD, 	   2 },     VARATTR_READ, NULL, NULL, NULL },
	{   5,	"sensorPathHops", 	 { {4,4,5}, VARSMI_BYTE, 	   1 },     VARATTR_READ, NULL, NULL, NULL },
	{   6,	"sensorPathNode", 	 { {4,4,6}, VARSMI_STREAM, 	  60 },     VARATTR_READ, NULL, NULL, NULL },
	{   7,	"sensorPathTime", 	 { {4,4,7}, VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
	{   8,	"sensorPathLQI", 	 { {4,4,8}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
	{   9,	"sensorPathRSSI", 	 { {4,4,9}, VARSMI_CHAR,       1 },     VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT    m_sensorEventEntry_node[] =
{
    {   1,    "sevtTime",           { {4,6,1}, VARSMI_TIMESTAMP, 7 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "sevtType",           { {4,6,2}, VARSMI_BYTE,      1 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "sevtStatus",         { {4,6,3}, VARSMI_STREAM,    4 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_sensorBatteryEntry_node[] =
{
    {   1,    "battVolt",           { {4,7,1}, VARSMI_WORD,     2 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "battCurrent",        { {4,7,2}, VARSMI_WORD,     2 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "battOffset",         { {4,7,3}, VARSMI_BYTE,     1 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_sensorConstantEntry_node[] =
{
    {   1,    "powerVoltageScaleInt", { {4,8,1}, VARSMI_INT,    4 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "powerVoltageScaleReal",{ {4,8,2}, VARSMI_UINT,   4 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "powerCurrentScaleInt", { {4,8,3}, VARSMI_INT,    4 },     VARATTR_READ, NULL, NULL, NULL },
    {   4,    "powerCurrentScaleReal",{ {4,8,4}, VARSMI_UINT,   4 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT	m_network_node[] =
{
	{   1,	"ffdEntry", { {5,1,0}, VARSMI_OPAQUE, 62 },       
                            VARATTR_READ | VARATTR_LIST, m_ffdEntry_node, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_ffdEntry_node[] =
{
	{   1,	"ffdID", 	            { {5,1,1},  VARSMI_EUI64, 	   8 },     VARATTR_READ, NULL, NULL, NULL },
	{   2,	"ffdModel", 	        { {5,1,2},  VARSMI_STREAM,    18 },     VARATTR_READ, NULL, NULL, NULL },
	{   3,	"ffdLastPoll",          { {5,1,3},  VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
	{   4,	"ffdLastPush",          { {5,1,4},  VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
	{   5,	"ffdHwVersion",         { {5,1,5},  VARSMI_WORD,       3 },     VARATTR_READ, NULL, NULL, NULL },
	{   6,	"ffdFwVersion",         { {5,1,6},  VARSMI_WORD, 	   3 },     VARATTR_READ, NULL, NULL, NULL },
	{   7,	"ffdFwBuild", 	        { {5,1,7},  VARSMI_WORD, 	   3 },     VARATTR_READ, NULL, NULL, NULL },
	{   8,	"ffdPowerType", 	    { {5,1,8},  VARSMI_BYTE, 	   1 },     VARATTR_READ, NULL, NULL, NULL },
	{   9,	"ffdPermitMode", 	    { {5,1,9},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
	{  10,	"ffdPermitState", 	    { {5,1,10}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
	{  11,	"ffdMaxChildTbl", 	    { {5,1,11}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
	{  12,	"ffdCurrentChild", 	    { {5,1,12}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
	{  13,	"ffdAlarmMask", 	    { {5,1,13}, VARSMI_WORD,       2 },     VARATTR_READ, NULL, NULL, NULL },
	{  14,	"ffdAlarmFlag", 	    { {5,1,14}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
	{  15,	"ffdTestFlag", 	        { {5,1,15}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
	{  16,	"ffdSolarAdVolt", 	    { {5,1,16}, VARSMI_WORD,       2 },     VARATTR_READ, NULL, NULL, NULL },
	{  17,	"ffdSolarChgBattVolt",  { {5,1,17}, VARSMI_WORD,       2 },     VARATTR_READ, NULL, NULL, NULL },
	{  18,	"ffdSolarBoardVolt",    { {5,1,18}, VARSMI_WORD,       2 },     VARATTR_READ, NULL, NULL, NULL },
	{  19,	"ffdSolarBattChgState", { {5,1,19}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT    m_transaction_node[] =
{
    {   1,    "trInfoEntry", { {6,1,0}, VARSMI_OPAQUE, 39 },       
                            VARATTR_READ | VARATTR_LIST, m_trInfoEntry_node, NULL, NULL },
    {   2,    "trHistoryEntry", { {6,2,0}, VARSMI_OPAQUE, 12 },       
                            VARATTR_READ | VARATTR_LIST, m_trHistoryEntry_node, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_trInfoEntry_node[] =
{
    {   1,    "triTargetID",          { {6,1,1},  VARSMI_EUI64,      8 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "triCommand",           { {6,1,2},  VARSMI_OID,        3 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "triID",                { {6,1,3},  VARSMI_WORD,       2 },     VARATTR_READ, NULL, NULL, NULL },
    {   4,    "triOption",            { {6,1,4},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   5,    "triDay",               { {6,1,5},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   6,    "triInitNice",          { {6,1,6},  VARSMI_CHAR,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   7,    "triCurNice",           { {6,1,7},  VARSMI_CHAR,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   8,    "triInitTry",           { {6,1,8},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   9,    "triCurTry",            { {6,1,9},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {  10,    "triQueue",             { {6,1,10}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {  11,    "triCreateTime",        { {6,1,11}, VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
    {  12,    "triLastTime",          { {6,1,12}, VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
    {  13,    "triState",             { {6,1,13}, VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {  14,    "triError",             { {6,1,14}, VARSMI_INT,        4 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT    m_trHistoryEntry_node[] =
{
    {   1,    "trhTime",              { {6,2,1},  VARSMI_TIMESTAMP,  7 },     VARATTR_READ, NULL, NULL, NULL },
    {   2,    "trhState",             { {6,2,2},  VARSMI_BYTE,       1 },     VARATTR_READ, NULL, NULL, NULL },
    {   3,    "trhError",             { {6,2,3},  VARSMI_INT,        4 },     VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};


VAROBJECT	m_meter_node[] =
{
	{   1,	"meterDataEntry", { {10,1,0}, VARSMI_OPAQUE, 268 },         
                        VARATTR_READ | VARATTR_LIST, m_meterDataEntry_node, NULL, NULL },
	{   2,	"meterEntry", { {10,2,0}, VARSMI_OPAQUE, sizeof(METERENTRY) },      
                        VARATTR_READ | VARATTR_LIST, m_meterEntry_node, NULL, NULL },
	{   3,	"meterLPEntry", { {10,3,0}, VARSMI_OPAQUE, sizeof(METERLPENTRY) },      
                        VARATTR_READ | VARATTR_LIST, m_meterLPEntry_node, NULL, NULL },
    {   4,  "meterEntryMbus", { {10,4,0}, VARSMI_OPAQUE, sizeof(METERENTRYMBUS) }, 
                        VARATTR_READ | VARATTR_LIST, m_meterEntry_mbus, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_meterDataEntry_node[] =
{
	{	1,	"mdID", { {10,1,1}, VARSMI_EUI64, 8 },                      VARATTR_READ, NULL, NULL, NULL },
	{	2,	"mdType", { {10,1,2}, VARSMI_BYTE, 1 },                     VARATTR_READ, NULL, NULL, NULL },
	{	3,	"mdVendor", { {10,1,3}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, NULL },
	{	4,	"mdServiceType", { {10,1,4}, VARSMI_BYTE, 1 },              VARATTR_READ, NULL, NULL, NULL },
	{	5,	"mdTime", { {10,1,5}, VARSMI_TIMESTAMP, 7 },                VARATTR_READ, NULL, NULL, NULL },
	{	6,	"mdLength", { {10,1,6}, VARSMI_WORD, 2 },                   VARATTR_READ, NULL, NULL, NULL },
	{	7,	"mdData", { {10,1,7}, VARSMI_STREAM, 0 },                   VARATTR_READ, NULL, NULL, NULL },
	{	0,	NULL }
};

VAROBJECT	m_meterEntry_node[] =
{
	{	1,	"mtrGroup", { {10,2,1}, VARSMI_BYTE, 1 },                   VARATTR_READ, NULL, NULL, NULL },
	{	2,	"mtrMember", { {10,2,2}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
	{	3,	"mtrID", { {10,2,3}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, NULL },
	{	4,	"mtrSerial", { {10,2,4}, VARSMI_STREAM, 18 },               VARATTR_READ, NULL, NULL, NULL },
	{	5,	"mtrTime", { {10,2,5}, VARSMI_TIMESTAMP, 7 },               VARATTR_READ, NULL, NULL, NULL },
	{	6,	"mtrLength", { {10,2,6}, VARSMI_WORD, 2 },                  VARATTR_READ, NULL, NULL, NULL },
	{	7,	"mtrData", { {10,2,7}, VARSMI_STREAM, 0 },                  VARATTR_READ, NULL, NULL, NULL },
	{	0,	NULL }
};

VAROBJECT	m_meterLPEntry_node[] =
{
	{	1,	"mlpId", { {10,3,1}, VARSMI_EUI64, 8 },                     VARATTR_READ, NULL, NULL, NULL },
	{	2,	"mlpMid", { {10,3,2}, VARSMI_STREAM, 20 },                  VARATTR_READ, NULL, NULL, NULL },
	{	3,	"mlpType", { {10,3,3}, VARSMI_BYTE, 1 },                    VARATTR_READ, NULL, NULL, NULL },
	{	4,	"mlpServiceType", { {10,3,4}, VARSMI_BYTE, 1 },             VARATTR_READ, NULL, NULL, NULL },
	{	5,	"mlpVendor", { {10,3,5}, VARSMI_BYTE, 1 },                  VARATTR_READ, NULL, NULL, NULL },
	{	6,	"mlpDataCount", { {10,3,6}, VARSMI_WORD, 2 },               VARATTR_READ, NULL, NULL, NULL },
	{	7,	"mlpLength", { {10,3,7}, VARSMI_WORD, 2 },                  VARATTR_READ, NULL, NULL, NULL },
	{	8,	"mlpTime", { {10,3,8}, VARSMI_TIMESTAMP, 7 },               VARATTR_READ, NULL, NULL, NULL },
	{	9,	"mlpData", { {10,3,9}, VARSMI_STREAM, 0 },                  VARATTR_READ, NULL, NULL, NULL },
	{	0,	NULL }
};

VAROBJECT   m_meterEntry_mbus[] =
{
    {   1,  "mmtrID", { {10,4,1}, VARSMI_EUI64, 8 }, VARATTR_READ, NULL, NULL, NULL },
    {   2,  "mmtrMasterBank", { {10,4,2}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   3,  "mmtrSerialNumber", { {10,4,3}, VARSMI_STREAM, 22 },VARATTR_READ, NULL, NULL, NULL },
    {   4,  "mmtrMenufact", { {10,4,4}, VARSMI_STREAM, 22 }, VARATTR_READ, NULL, NULL, NULL },
    {   5,  "mmtrGener", { {10,4,5}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   6,  "mmtrMedia", { {10,4,6}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   7,  "mmtrCntAccess", { {10,4,7}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   8,  "mmtrStatus", { {10,4,8}, VARSMI_BYTE, 1 }, VARATTR_READ, NULL, NULL, NULL },
    {   9,  "mmtrLastMetering", { {10,4,9}, VARSMI_TIMESTAMP, 7 },VARATTR_READ, NULL, NULL, NULL },
    {   0,  NULL } 
}; 

VAROBJECT	m_eventMcu_node[] =
{
	{   1,	"eventMcuInstall", { {200,1,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   2,	"eventMcuUninstall", { {200,2,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
	{   3,	"eventMcuStartup", { {200,3,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   4,	"eventMcuShutdown", { {200,4,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
	{  10,	"eventMcuChange", { {200,10,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{  11,	"eventMcuOamLogin", { {200,11,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
	{  12,	"eventMcuOamLogout", { {200,12,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
	{  13,	"eventMcuClockChange", { {200,13,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
	{  14,	"eventMcuPowerFail", { {200,14,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
	{  15,	"eventMcuPowerRestore", { {200,15,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
	{  16,	"eventMcuLowBattery", { {200,16,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
	{  17,	"eventMcuLowBatteryRestore", { {200,17,0}, VARSMI_OID, 0 }, VARATTR_READ, NULL, NULL, NULL },
	{  18,	"eventMcuTempRange", { {200,18,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
	{  19,	"eventMcuTempRangeResetore", { {200,19,0}, VARSMI_OID, 0 }, VARATTR_READ, NULL, NULL, NULL },
	{  20,	"eventMcuReset", { {200,20,0}, VARSMI_OID, 0 },             VARATTR_READ, NULL, NULL, NULL },
	{  21,	"eventMcuFactoryDefault", { {200,21,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
	{  22,	"eventMcuFirmwareUpdate", { {200,22,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
    {  23,  "eventMcuBatteryChargingStart", { {200,23,0}, VARSMI_OID, 0 },VARATTR_READ, NULL, NULL, NULL },
    {  24,  "eventMcuBatteryCharginEnd", { {200,24,0}, VARSMI_OID, 0 }, VARATTR_READ, NULL, NULL, NULL },
    {  25,  "eventMcuCoverOpen", { {200,25,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {  26,  "eventMcuCoverClose", { {200,26,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
    {  27,  "eventMcuHeaterOn", { {200,27,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {  28,  "eventMcuHeaterOff", { {200,28,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {  29,  "eventMcuAlert", { {200,29,0}, VARSMI_OID, 0 },             VARATTR_READ, NULL, NULL, NULL },
	{ 100,	"eventMcuMeteringStart", { {200,100,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
	{ 101,	"eventMcuMeteringEnd", { {200,101,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
	{ 102,	"eventMcuRecoveryStart", { {200,102,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
	{ 103,	"eventMcuRecoveryEnd", { {200,103,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
	{ 104,  "eventMcuMeteringFail", { {200,104,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
	{ 105,  "eventMcuDiagnosis", { {200,105,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_eventSink_node[] =
{
	{   1,	"eventSinkInstall", { {201,1,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
	{   2,	"eventSinkUninstall", { {201,2,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
	{   3,	"eventSinkConnect", { {201,3,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
	{   4,	"eventSinkDisconnect", { {201,4,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
	{   5,	"eventSinkChange", { {201,5,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   6,	"eventSinkReset", { {201,6,0}, VARSMI_OID, 0 },             VARATTR_READ, NULL, NULL, NULL },
	{   7,	"eventSinkError", { {201,7,0}, VARSMI_OID, 0 },             VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_eventMobile_node[] =
{
	{   1,	"eventMobileInstall", { {202,1,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
	{   2,	"eventMobileChange", { {202,2,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
	{   3,	"eventMobileReset", { {202,3,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
	{   4,	"eventMobileConnect", { {202,4,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
	{   5,	"eventMobileDisconnect", { {202,5,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
	{   6,	"eventMobileKeepalive", { {202,6,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_eventSensor_node[] =
{
	{   1,	"eventSensorJoinNetwork", { {203,1,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
	{   2,	"eventSensorLeaveNetwork", { {203,2,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
	{   3,	"eventSensorChange", { {203,3,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
	{   4,	"eventSensorConnect", { {203,4,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
	{   5,	"eventSensorDisconnect", { {203,5,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
	{   6,	"eventSensorTimeout", { {203,6,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
	{   7,	"eventSensorBreaking", { {203,7,0}, VARSMI_OID, 0 },        VARATTR_READ, NULL, NULL, NULL },
    {   8,  "eventSensorUnknown", { {203,8,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
    {   9,  "eventSensorReset", { {203,9,0}, VARSMI_OID, 0 },           VARATTR_READ, NULL, NULL, NULL },
    {  10,  "eventSensorAlarm", { {203,10,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
    {  11,  "eventSensorPageChange", { {203,11,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
    {  12,  "eventSensorPageRead", { {203,12,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    {  13,  "eventSensorPageReadAll", { {203,13,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
    {  14,  "eventSensorRecovery", { {203,14,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    { 100,  "eventSensorLogPage", { {203,100,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    { 105,  "eventSensorInstall", { {203,105,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
    { 106,  "eventSensorUninstall", { {203,106,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
    { 107,  "eventSensorPowerOutage", { {203,107,0}, VARSMI_OID, 0 },   VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_eventMalf_node[] =
{
	{   1,	"eventMalfCommandError", { {204,1,0}, VARSMI_OID, 0 },      VARATTR_READ, NULL, NULL, NULL },
	{   2,	"eventMalfMeterReadError", { {204,2,0}, VARSMI_OID, 0 },    VARATTR_READ, NULL, NULL, NULL },
	{   3,	"eventMalfUploadError", { {204,3,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
	{   4,	"eventMalfDiskError", { {204,4,0}, VARSMI_OID, 0 },         VARATTR_READ, NULL, NULL, NULL },
	{   5,	"eventMalfDiskErrorRestore", { {204,5,0}, VARSMI_OID, 0 },  VARATTR_READ, NULL, NULL, NULL },
	{   6,	"eventMalfMemoryError", { {204,6,0}, VARSMI_OID, 0 },       VARATTR_READ, NULL, NULL, NULL },
	{   7,	"eventMalfMemoryErrorRestore", { {204,7,0}, VARSMI_OID, 0 },VARATTR_READ, NULL, NULL, NULL },
	{   8,	"eventMalfWatchdogReset", { {204,8,0}, VARSMI_OID, 0 },     VARATTR_READ, NULL, NULL, NULL },
	{   9,	"eventMalfGarbageCleaning", { {204,9,0}, VARSMI_OID, 0 },   VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_eventComm_node[] =
{
	{   1,	"eventCommZRUError", { {205,1,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
	{   2,	"eventCommZMUError", { {205,2,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
	{   3,	"eventCommZEUError", { {205,3,0}, VARSMI_OID, 0 },          VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_Alarm_node[] =
{
	{   1,	"alarmMessage", { {206,1,0}, VARSMI_OID, 0 },               VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_eventMeter_node[] =
{
	{   1,	"eventMeterError", { {212,1,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT	m_eventOta_node[] =
{
	{   1,	"eventOtaDownload", { {214,1,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   2,	"eventOtaStart", 	{ {214,2,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   3,	"eventOtaEnd", 		{ {214,3,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   4,	"eventOtaResult", 	{ {214,4,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
	{   0,	NULL }
};

VAROBJECT    m_eventTransaction_node[] =
{
    {   1,    "eventTransaction", { {215,1,0}, VARSMI_OID, 0 },            VARATTR_READ, NULL, NULL, NULL },
    {   0,    NULL }
};

VAROBJECT   m_eventManagement_node[] =
{
	{	1,	"eventManEcho", { {255,1,0}, VARSMI_OID, 0 },               VARATTR_READ, NULL, NULL, NULL },
	{	0,	NULL }
};

