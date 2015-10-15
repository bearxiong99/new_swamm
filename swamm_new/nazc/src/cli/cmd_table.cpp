#include "common.h"
#include "cmd_syntax.h"
#include "cmd_table.h"
#include "cmd_show.h"
#include "cmd_set.h"
#include "cmd_ctrl.h"
#include "cmd_default.h"
#include "cmd_test.h"
#include "cmd_emergency.h"

//------------------------------------------------------------------------------
// 파라메터를 검사하는 함수들의 테이블
//------------------------------------------------------------------------------

CLIPARAMCHECKER	m_CliParameters[] =
{
    { "1sttime",    "1sttime",           
					"1st Time (hex)",             		check_hex },
	{ "acodiid",	"all|codi-id:eui64",	
					"All or CODI ID",		            check_allid	},
	{ "action",     "Action",					
					"Action (0:read, 1:write)",		    check_number },
	{ "adate",		"Date",					
					"all, yyyymmdd",					check_adate },
	{ "addr",		"addr",					
					"Address (Group.Member)",			check_address },
	{ "agroup",		"all|group",			
					"All or Group number",				check_agroup },
	{ "aif",		"all|ifname",			
					"All or Interface Name",			check_allifname },
	{ "aid",		"all|id:eui64",			
					"All or SENSOR ID",	                check_allid	},
	{ "aidparser",	"all|id:eui64|parser",			
					"All or SENSOR ID or Sensor Type(ge, aidon, kamst, mbus, repeater, battery ...)",  check_idparser	},
	{ "allparser",	"all|parser",			
					"All or Sensor Type(ge, aidon, kamst, mbus, repeater, battery ...)", check_string	},
    { "amask",      "alarmmask",           
					"Alarm Mask (0x0010:case open, 0020:mr tamper, 0040:mr detachment, "
                    "0080:low battery 0100:pulse cut)",     check_hex },
	{ "apn",		"apn",			
					"Access Point Name",				check_string },
	{ "apnname",	"name",			
					"APN short name",					check_string },
	{ "autoset",	"auto-setting",				
					"Network Auto Setting (0=off, 1=on)", check_onoff },
	{ "bind",		"bind",					
					"Bind Number",						check_number },
	{ "bnum",		"byte-number",				
					"Positive Number(0~255)",			check_byte },
	{ "build",		"build",					
					"Build Number",						check_number },
	{ "ch",			"channel",				
					"Channel Number",					check_channel },
	{ "cid",        "CID",				
					"KMP CID (hexa) or CID string\r\n"
                    "      CID String\r\n"
                    "       - Time   : Clock, Sync\r\n"
                    "       - Asset  : Type, Inventory, Serial, LpConfig\r\n"
                    "       - Status : Load, Voltage, Current, PowerLimit\r\n"
                    "       - CutOff : RelayStatus, Disconnect, Connect, Reconnect\r\n"
                    "       - Log    : GetEventStatus, GetStatusLog, GetRtcLog, GetLpLog\r\n"
                    "                  GetVQualityLog, GetCutOffLog\r\n"
                    "       - index  : next, prev, last, showindex, clearindex"
                                               ,        check_hex_or_string },
	{ "coretype",   "coretype",				
					"Core Type (0:modem, 1:core)",	    check_coretype },
	{ "cmd",		"COMMAND",				
					"Command Identifier",				check_oid },
	{ "com",		"company-code",			
					"Meter Mfg",						check_number },
	{ "command",	"command",				
					"Shell Command",					check_string },
	{ "codiid",		"codi-id:eui64",		
					"CODI ID",			                check_id },
	{ "concmd",		"apn-setting",			
					"APN setting ex) AT+CGDCONT=1,\"IP\",\"telstra.internet\"", check_string },
	{ "concmd1",	"dialcmd",			
					"Dial command ex) ATD*99***1#, ATD*99#", check_string },
    { "count",    	"count",           
					"Data count", 		                check_number },
	{ "csecurity",	"csecurity",				
					"Security (0=disable, 1=enable)", 	check_enable },
    { "customer",   "customer",				
					"Customer Name",			        check_string },
	{ "data",		"data",				
					"Data String",					    check_string },
    { "databit",    "data-bit",				
					"7=7Bit, 8=8Bit",					check_databit },
	{ "day",		"day",					
					"Day",								check_number },
    { "daymask",    "daymask",           
					"Day bit mask (hex), 0bit=1~30bit=31", check_hex },
	{ "@dd",		"day",					
					"Day",								check_number },
    { "devtype",   	"type",           
					"Device Type(0=mcu,1=sensor,2=codi)",	check_devtype },
	{ "dir",		"directory",			
					"Directory name",					check_string },
	{ "disttype",	"distributiontype",			
					"Distribution type (con[centrator],coordi[nator],sen[sor])",check_string },
    { "elevel", 	"energy level",			
					"Energy level (1~15)",				check_number },
    { "e-type", 	"event-type",			
					"Event type",						check_number },
	{ "emt",		"type",					
					"Meter Type(0=ge, 1=aidon, 2=kamstrup)",	check_elmetertype },
	{ "enable",		"enables",				
					"enable, disable",					check_enable },
    { "endidmask",  "daymask",           
					"Day bit mask (hex), 0x02=1, 0x04=2, 0x06=1,2 0x08=3 ...", check_hex },
    { "endiqmask",  "quartermask",           
					"Quarter bit mask (hex), 0x01=00:00, 0x02=00:15, 0x03=00:00,00:15 ... :", check_hex },
	{ "epwr",       "Effective RF-power",				
					"RF Power (-5~-9,-11,-12,-14,-17,-20,-26,-43)", check_epower },
	{ "ethernet",	"ethernet",				
					"Ethernet type (lan,dhcp,adsl,vdsl,cable,ppp)", check_ethernet },
	{ "ethtype",	"ethtype",				
					"Ethernet type (0:LAN,1:DHCP,2:PPPoE,3:PPP)", check_number },
    { "evd",    	"delay",           
					"Event delay time (second)",		check_number },
    { "evl",    	"severity",           
					"Event severity (0:Critical, 1:Major, 2:Minor, 3:Normal, 4:Information)", check_severity },
	{ "evt",		"id:oid|name",				
					"Event Identifier or Name",			check_string },
	{ "extpanid",	"extended-panid",				
					"Extended PAN ID", 					check_hex8 },
	{ "fepaddr",	"ip-address",			
					"FEP Server Address",				check_ip },
	{ "fepport",	"port",					
					"FEP Server Port",					check_number },
	{ "fid",		"eui64",				
					"Sensor ID (16 Digit)",				check_id64 },
	{ "filename",	"filename",				
					"Firmware file name",				check_string },
	{ "force",		"force",				
					"on/off, true/false, 1/0",			check_onoff },
	{ "fversion",	"fversion",				
					"Firmware Version",				    check_version },
    { "meteringoption",   "Metering Option",           
					"Metering option(default:0, relay-status:3, relay-on:4, relay-off:5, timesync:8, eventlog:10)", check_number },
    { "getmethod", 	"type",           
					"Download Method(0=zmodem,1=wget,2=mcu memory)",	check_getmethod },
	{ "gpnum",		"value",				
					"GPIO Value (0/1)",					check_number },
	{ "gpport",		"port",					
					"GPIO Port Number",					check_number },
	//  [10/20/2010 DHKim]
	{ "group",		"group",
					"Group Name",						check_string },
	{ "groupkey",		"groupKey",
					"Group Key Value",						check_number },
	{ "tablekind",		"tablekind",
					"Group Table kind",						check_number },
	//////////////////////////////////////////////////////////////////////////
	{ "gdeleteckmember",		"gdeleteckmember",
					"Group Delete Check Member(0=disable, 1=enable)",						check_enable },
	{ "groupnameinfo",		"groupnameinfo",
					"Group name info (0=group name info, 1=group member info)",						check_enable },
	{ "gtype",		"gtype",
					"Group type",						check_string },
	{ "gtypeid",	"gtype|id",
					"Group type or EUI64 id",			check_string },
	{ "gw",			"gw",					
					"Gateway",							check_ip },
	{ "haddr",		"address",			    
					"IP Address",						check_ip },
	{ "hexa",       "hexa",			    
					"Hexa Number ex) 03 AB DC 5 A",		check_hex },
	{ "hexadata",   "hexa-strings",			    
					"Hexa Data ex) 03 AB DC 5 A",		check_hex_list },
	{ "hh",			"time",					
					"Metering Time",					check_number },
	{ "@hh",		"hour",					
					"Hour",								check_number },
    { "@hh1",       "rhour",                
					"Repair time (hour)",               check_number },
	{ "hname",		"name",					
					"Host name",						check_string },
	{ "hour",		"hour",				
					"Hour (0~23)",						check_number },
    { "hourmask",   "hourmask",           
					"Hour bit mask (hex), 0bit=0~23bit=23 o'Clock", check_hex },
	{ "hpfrom",		"port (from)",			
					"Port Number (Start)",				check_number },
	{ "hpto",		"port (to)",			
					"Port Number (End)",				check_number },
	{ "hversion",	"hversion",				
					"Hardware Version",				    check_version },
	{ "id",			"id:eui64",				
					"SENSOR ID",						check_id },
	{ "id64",		"mcu-id",				
					"MCU ID (16 Digit)",				check_id64 },
	{ "idlist",		"id:eui64 list",				
					"SENSOR ID List",					check_id },
	{ "idparser",	"id:eui64|parser",			
					"SENSOR ID or Sensor Type(ge, aidon, kamst, mbus, battery ...)", check_idparser	},
	{ "if",			"ifname",				
					"Interface Name",					check_ifname },
	{ "intv",		"intv",					
					"Interval",							check_number },
	{ "intv-min",	"intv-min",					
					"Interval (min)",					check_number },
	{ "intv-sec",	"intv-sec",					
					"Interval (second)",				check_number },
	{ "ip",			"ip-address",			
					"IP Address",						check_ip },
	{ "ip2",		"ip-address",			
					"IP Address or 'FEP'",				check_pingip },
	{ "kb",			"size",				
					"Maximum size (k byte)",			check_number },
    { "keepalive",  "keepalive",			
					"Max alive time",					check_keepalive },
	{ "lbt",		"lbt",					
					"Delay (10~255 ms)",				check_lbt },
	{ "linkkey",	"link-key",				
					"Link key", 						check_hex16 },
    { "loc",        "location",				
					"Consumption Location",			    check_string },
	{ "lpmode",		"lpmode",					
					"LP data mode (0=Previous LP, 1=Current LP)", check_number },
	{ "lpperiod",	"lpperiod",				
					"LP Period (1:60min, 2:30min, 4:15min)", check_number },
	{ "mbuscmd",	"mbuscmd",				
					"MBUS Command\r\n"
                    "     Read (1: UD2)\r\n"
                    "    Write (1: Reset, 2: Address)",		
                                                        check_number },
	{ "mc",			"level",				
					"Meter Class",						check_number },
    { "mccb",    	"mccb-cmd",           
					"MCCB Command (1=Enable-Disconnected, 2=Enable-Automatically, 3=Enable-Connect Now, 4=Disable-Disconnect, 5=Device Status, 6=Phase Status, 7=Last Accepted Control Msg)",
					                                    check_number },
	{ "mdintv",		"interval",				
					"Read interval",					check_number },
	{ "mid",		"meter-id",				
					"Meter ID",							check_string },
    { "mlci", 		"interval",				
					"Check interval (min)",				check_number },
	{ "min",		"min",				
					"Minute (0~59)",					check_number },
	{ "@mm",		"mon",					
					"Month",							check_number },
    { "@mm1",       "interval",             
					"Group Interval (min)",             check_number },
    { "mmask",      "clear-mask",           
					"Clear mask (hex)",             	check_hex },
    { "mmday",     	"day",           
					"0:sun,1:mon,2:tue,3:wed,4:thu,5:fri,6:sat)", check_hex },
	{ "mobile",		"mobile",				
					"Mobile vendor (growell, anydata)",	check_mobile },
	{ "mobmode",	"mobmode",				
					"Mobile mode (0:CSD,1:PACKET,2:Always)", check_number },
	{ "mobtype",	"mobtype",				
					"Mobile type (0:NONE,1:GSM/GPRS,2:CDMA,3:PSTN)", check_number },
	{ "model",		"model",				
					"Sensor model",					    check_string },
	{ "msecurity",	"security",				
					"Meter security",					check_string },
	{ "mstype",		"format",				
					"Data format (A=ASCII, H=HEX)",		check_format },
	{ "mt",			"type",					
					"Meter Type",						check_metertype },
	{ "mtmax",		"mtmax",					
					"Timesync Threshold secs(max)",     check_number },
	{ "mtmin",		"mtmin",					
					"Timesync Threshold secs(min)",     check_number },
	{ "mtortype",	"mtor-type",				
					"MTOR type (0:lowram, 1:highram)",	check_mtortype },
	{ "muser",		"user",				
					"Meter user name",					check_string },
	{ "netkey",		"network-key",				
					"Network key ex) 56 7B ..",			check_hex16 },
	{ "netmask",	"netmask",				
					"Subnet Mask", 						check_ip },
	{ "@nn",		"min",					
					"Minute",							check_number },
	{ "num",		"value",				
					"Number",							check_number },
	{ "ntype",		"network-type",				
					"Network Type (0:Star, 1:Mesh)",	check_networktype },
    { "offset",    	"offset",           
					"Meter Data Reading offset", 		check_number },
	{ "oid",		"OID3",					
					"Object Identifier",				check_oid },
	{ "onoff",		"on-off",				
					"on/off, true/false, high/low, 1/0",	check_onoff },
	{ "opcode",		"opcode",					
					"Operation code (0=clear, 1=active, 2=on, 3=off)",	check_opcode },
	{ "opmode",		"opmode",					
					"Operation Mode (0=normal, 1=test)",check_opmode },
    { "option",    	"option",           
					"Metering option(default:0, relay statue:3, timesync: 8, event:10 )", 		            
                                                        check_number },
	{ "options",	"options ...",			
					"PPP Options (-noauth, novj, novjccomp, nodeflate, nobsdcomp, ...)", check_string },
	{ "otatype",	"ota-type",				
					"OTA Type (0=auto, 1=multicast, 2=unicast)", check_otatype },
	{ "cputype",	"cpu-type",				
					"CPU Type (0= EM250, 1=ARM711, 2=default)", check_cputype },
	{ "panid",		"panid",				
					"PAN-ID",							check_number },
    { "parity",     "parity",				
					"none, odd, even",					check_parity },
	{ "parser",	    "parser",			
					"Sensor Type(ge, aidon, kamst, mbus, mbus-arm, repeater, battery ...)", check_idparser	},
    { "pathviewtype","View Type",				
					"View (linear, tree)",	            check_string },
	{ "period",		"period",				
					"Metering or Recovery period",		check_number },
    { "perm",       "group",                
					"User Group (0:Super User, 1:Admin, 2:User)", check_number },
	{ "permit",		"permit",				
					"Permit Time (0 ~ 255)", 			check_number },
	{ "pgroup",		"group",				
					"Group(admin,user,guest)",			check_group },
	{ "pid",		"pid",					
					"Process ID",						check_number },
	{ "pingmethod",	"method",				
					"Ping method (0:ICMP, 1:SOCKET, 2:ICMP+SOCKET)", check_number },
    { "pmask",      "port mask",           
					"Digital port mask (hex 01 ~ FF)",  check_hex },
	{ "pnum",		"pnum",				
					"Positive Number",					check_unsigned },
	{ "port",		"port",					
					"Port Number",						check_number },
	{ "power",		"power",					
					"Power Type (17:Normal, 48:Solar)",	check_number },
	{ "ppass",		"password",				
					"password",							check_string },
	{ "ps16",		"password",				
					"password",							check_string },
	{ "pt",			"port",					
					"Port Number",						check_port },
	{ "pulse",	    "pulse",				
					"Pulse value",				        check_unsigned },
	{ "puser",		"user",					
					"User ID",							check_string },
    { "pvalue",     "port value",           
					"Digital port value (hex 00 ~ FF)", check_hex },
	{ "pwr",		"RF-power",				
					"RF Power (3~-9,-11,-12,-14,-17,-20,-26,-43)", check_power },
	{ "q",			"phonenum",			
					"Mobile Phone Number",				check_string },
    { "rate",       "rate",            
					"Critical rate (1~100)", 			check_number },
	{ "retry",		"retry",				
					"Retry count (1~n)",				check_number },
    { "routing",    "routing",				
					"Mesh routing (disable, enable)",	check_routing },
	{ "s",			"string",				
					"string",							check_string },
	{ "s-type",		"type",					
					"Type",								check_number },
	{ "saveday",	"saveday",					
					"Data save day",					check_number },
	{ "schedulertype",  "scheduler type",				
					"Scheduler type (0:Mask,1:Interval)",check_schedulertype },
	{ "sec",		"second",				
					"Second (0~255)",					check_number },
	{ "sec-name",	"security name",				
					"Parser name or Node Kind or ID",	check_string },
	{ "sec-type",	"security type",				
					"PARSER/NODE/ID",	                check_string },
	{ "sensorcmd",	"sensorcmd",				
					"sensor command (digit) ", check_number },
	{ "service",	"service",				
					"Service type (1:ELECT,2:GAS,3:WATER)",	check_number },
	{ "sn",			"serial",				
					"Meter Serial Number",				check_number },
    { "speed",      "speed",				
					"Baudrate (2400,4800, 9600, ...)",	check_speed },
	{ "@ss",		"sec",					
					"Second",							check_number },
    { "@ss1",       "start-sec",            
					"Start second",   					check_number },
    { "sstrategy",   "strategy",           
					"Inventory Scanning strategy (0:lazy, 1:immediately)", check_sstrategy },
    { "stopbit",    "stop-bit",				
					"1=1 Stop Bit, 2=2 Stop Bit",		check_stopbit },
	{ "str",		"string",				
					"string",							check_string },
    { "summaryviewtype","View Type",				
					"View (detail, summary)",	        check_string },
	{ "table",		"table",					
					"Table index (0=all, 1=current, 2=previous, 3=relay, 10=event)", check_table },
    { "thread",    	"count",           
					"Metering thread count (1~5)", 		check_thread },
    { "timeout",    "timeout",				
					"Timeout",							check_timeout },
	{ "timezone",	"timezone",			
					"minutes westof Greenwich (720 ~ -660) ",check_number },
	{ "tmax",		"tmax",					
					"Temperature(max)",					check_float },
	{ "tmin",		"tmin",					
					"Temperature(min)",					check_float },
	{ "tnum",		"tnum",				
					"Transaction Number",				check_unsigned },
	{ "to1",		"timeout",				
					"Timeout (sec)",					check_number },
	{ "to2",		"keepalive",			
					"Keepalive time (sec)",				check_number },
	{ "tp",			"topology",				
					"Meter Topology",					check_number },
    { "trigger",	"TriggerID",				
					"Trigger ID",	            		check_string },
	{ "tropt",	    "troption",			
					"TR State(0:Default(7), 1:Wait, 2:Queue, 4:Run, 7:W+Q+R, 8:Terminate, 255:All)",	check_troption },
	{ "tstrategy",	"timesync",				
					"Timesync Strategy (0: Hybrid/Mesh, 1: Hybrid + Sleepy Mesh, 2: Sleepy Mesh)",
                                                        check_number },
	{ "txmod",		"tx-pwrmode",				
					"TX Power Mode (0=Default, 1=Boost, 2=Alternate, 3=Alternate & Boost)",
                                                        check_number },
	{ "type",		"type",					
					"Sensor type (1:ZRU,2:ZMU,5:ZEUPLS)",check_number },
	{ "tzhour",	    "timezone",			
					"hours west of Greenwich (12 ~ -11) ",check_number },
    { "user",       "user",                 
					"User ID",                          check_string },
    { "vendor",     "vendor",				
					"Vendor Information",			    check_string },
	{ "while",		"trytime",					
					"Upload retry time (min)",			check_number },
    { "wm",     	"mask",           
					"Wakeup min bit mask (hex)",   		check_hex },
	{ "@yy",		"year",					
					"Year",								check_number },

	{ NULL,	NULL, NULL,	NULL }
};

//------------------------------------------------------------------------------
// 명령어 테이블 (링크로 서로 연결되어 진다.)
//------------------------------------------------------------------------------

CLIHANDLER  m_tblEnable[] =
{
    { "autologout",		cmdEnableAutoLogout,			NULL,				NULL,		
						"Enable auto logout", 			CLIGROUP_GUEST, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "autoregister",	cmdEnableAutoRegister,			NULL,				NULL,		
						"Enable auto register",			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "autoreset",		cmdEnableAutoReset,				NULL,				NULL,		
						"Enable self reset", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "autoupload",     cmdEnableAutoUpload,            NULL,               NULL,
                        "Enable auto upload",           CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
/** MCU는 항상 자동으로 시간을 동기화 해야 하기 때문에 기능을 삭제한다.
	{ "autotimesync",	cmdEnableAutoTimeSync,			NULL,				NULL,		
						"Enable auto timesync", 		CLIGROUP_USER, 		CLI_LOGGING ,   CLI_NORMAL_CMD},
*/
    { "cleaning",		cmdEnableGarbageCleaning,		NULL,				NULL,		
						"Enable garbage cleaning",		CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "cmdhist",		cmdEnableCmdHist,				NULL,				NULL,		
						"Enable command histroy",		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
/** Issue #1127: Comm Log를 CLI에서 제거
    { "commlog",		cmdEnableCommLog,				NULL,				NULL,		
						"Enable communication log",		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
*/
/** Issue #1426 : */
    { "debuglog",		cmdEnableDebugLog,				NULL,				NULL,		
						"Enable debug log",     		CLIGROUP_USER, 		CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "failrecovery",	cmdEnableMeterFailRecovery,		NULL,				NULL,		
						"Enable meter fail recovery", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "flashcheck",		cmdEnableFlashCheck,			NULL,				NULL,		
						"Enable flash memory check", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
/** GPS 기능을 더이상 지원하지 않는다.
    { "gpstimesync",	cmdEnableGpsTimesync,			NULL,				NULL,		
						"Enable GPS timesync",			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
*/
    { "highram",		cmdEnableHighRam,			    NULL,				NULL,		
						"Enable high RAM coordinator", 	CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "keepalive",		cmdEnableKeepalive,				NULL,				NULL,		
						"Enable keepalive event",		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "memorycheck",	cmdEnableMemoryCheck,			NULL,				NULL,		
						"Enable memory check", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meterping",		cmdEnableMeterPolling,			NULL,				NULL,		
						"Enable meter ping", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "metertimesync",	cmdEnableMeterTimesync,			NULL,				NULL,		
						"Enable meter timesync", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mobilelog",		cmdEnableMobileLog,				NULL,				NULL,		
						"Enable mobile log", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "monitoring",		cmdEnableMonitoring,			NULL,				NULL,		
						"Enable monitoring", 			CLIGROUP_GUEST, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "recovery",       cmdEnableRecovery,              NULL,               NULL,
                        "Enable metring retry",         CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "sensortimesync",	cmdEnableTimeBroadcast,			NULL,				NULL,		
						"Enable time broadcast", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
#if 0 // Issue #835 스웨덴향 집중기는 join control을 지원하지 않는다
	{ "joinctrl",	cmdEnableJoinControl,			NULL,				NULL,		
						"Enable Join device All", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
#endif
#if 0
    { "checklowbatt",	cmdEnableLowBatt,			NULL,				NULL,		
						"Enable checking low-battery", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
#endif
  { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblDisable[] =
{
    { "autologout",		cmdDisableAutoLogout,			NULL,				NULL,		
						"Disable auto logout", 			CLIGROUP_GUEST, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "autoregister",	cmdDisableAutoRegister,			NULL,				NULL,		
						"Disable auto register", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "autoreset",		cmdDisableAutoReset,			NULL,				NULL,		
						"Disable auto reset", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
/** MCU는 항상 자동으로 시간을 동기화 해야 하기 때문에 기능을 삭제한다.
	{ "autotimesync",	cmdDisableAutoTimeSync,			NULL,				NULL,		
*/
    { "autoupload",     cmdDisableAutoUpload,           NULL,               NULL,
                        "Disable auto upload",          CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "cleaning",		cmdDisableGarbageCleaning,		NULL,				NULL,		
						"Disable garbage cleaning",		CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "cmdhist",		cmdDisableCmdHist,				NULL,				NULL,		
						"Disable command history", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
/** Issue #1127: Comm Log를 CLI에서 제거
    { "commlog",		cmdDisableCommLog,				NULL,				NULL,		
						"Disable communication log", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
*/  
/** Issue #1426 : */
    { "debuglog",		cmdDisableDebugLog,				NULL,				NULL,		
						"Disable debug log", 	        CLIGROUP_USER, 		CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "failrecovery",	cmdDisableMeterFailRecovery,	NULL,				NULL,		
						"Disable meter fail recovery", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "flashcheck",		cmdDisableFlashCheck,			NULL,				NULL,		
						"Disable flash memory check", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
/** GPS 기능을 더이상 지원하지 않는다.
    { "gpstimesync",	cmdDisableGpsTimesync,			NULL,				NULL,		
						"Disable GPS timesync", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
*/
    { "highram",		cmdDisableHighRam,			    NULL,				NULL,		
						"Disable high RAM coordinator", CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "keepalive",		cmdDisableKeepalive,			NULL,				NULL,		
						"Disable keepalive event",		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "memorycheck",	cmdDisableMemoryCheck,			NULL,				NULL,		
						"Disable memory check", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meterping",		cmdDisableMeterPolling,			NULL,				NULL,		
						"Disable meter ping", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "metertimesync",	cmdDisableMeterTimesync,		NULL,				NULL,		
						"Disable meter timesync", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mobilelog",		cmdDisableMobileLog,			NULL,				NULL,		
						"Disable mobile log", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "monitoring",		cmdDisableMonitoring,			NULL,				NULL,		
						"Disable monitoring",			CLIGROUP_GUEST, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "recovery",       cmdDisableRecovery,             NULL,               NULL,
                        "Disable metering recovery",    CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "sensortimesync",	cmdDisableTimeBroadcast,		NULL,				NULL,		
						"Disable time broadcast", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
#if 0 // Issue #835 스웨덴향 집중기는 join control을 지원하지 않는다
	{ "joinctrl",	cmdDisableJoinControl,			NULL,				NULL,		
						"Disable Join device All", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
#endif
#if 0
    { "checklowbatt",	cmdDisableLowBatt,			NULL,				NULL,		
						"Disable checking low-battery", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },   
#endif
  { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblReset[] =
{
    { "system",			cmdReset,      					NULL,				NULL,		
						"Reset MCU", 					CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "coordinator",	cmdResetCodi,					NULL,				NULL,		
						"Reset CODI", 					CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mobile",			cmdResetMobile,					NULL,				NULL,		
						"Reset mobile", 				CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "sensor",			cmdResetSensor,					NULL,				"%id*%sec",		
						"Reset sensor", 				CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "factorydef",		cmdFactoryDefault,				NULL,				NULL,		
						"Factory default setting", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
	{ NULL, NULL, NULL,	NULL, NULL,	0, FALSE, CLI_NORMAL_CMD }
};

CLIHANDLER  m_tblSetLimit[] =
{
#if 0
    // Issue #838 : Agent에서 Alarm Log를 기록하지 않고 있다. 따라서 관련 기능을 제거한다
    { "alarmlog",		cmdSetLimitAlarmLog,			NULL,				"%kb",	
						"Set alarm log limit", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
#endif
    { "cmdlog",			cmdSetLimitCmdLog,				NULL,				"%kb",	
						"Set command log limit", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
/** Issue #1127: Comm Log를 CLI에서 제거
    { "commlog",		cmdSetLimitCommLog,				NULL,				"%kb",	
						"Set communication log limit", 	CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
*/
    { "eventlog",		cmdSetLimitEventLog,			NULL,				"%kb",	
						"Set event log limit", 			CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meteringlog",	cmdSetLimitMeterLog,			NULL,				"%kb",	
						"Set metering log limit", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mobilelog",		cmdSetLimitMobileLog,			NULL,				"%kb",	
						"Set mobile log limit", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "timesynclog",	cmdSetLimitTimesyncLog,			NULL,				"%kb",	
						"Set meter timesync log limit", CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "upgradelog",	    cmdSetLimitUpgradeLog,			NULL,				"%kb",	
						"Set upgrade log limit", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "uploadlog",	    cmdSetLimitUploadLog,			NULL,				"%kb",	
						"Set upload log limit", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
/** Issue #1426 : 아직 미구현
    { "debuglog",		cmdSetLimitDebugLog,			NULL,				"%kb",	
						"Set Debug log limit",      	CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
*/
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetDRAsset[] =
{
    { "level",			cmdSetEnergyLevel,				NULL,				"%id%elevel", 		
						"Set energy level", 	        CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblScan[] =
{
    { "sensor",	        cmdScanSensor,	                NULL,				"%idparser*%fversion%build",		
						"Sensor Inventory Scanning",    CLIGROUP_ADMIN,	CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowMeter[] =
{
    { "list",           cmdShowMeterList,               NULL,               "*%allparser",
                        "Show meter list",              CLIGROUP_GUEST,     CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "pq",    	        cmdShowMeterPQ,			        NULL,   			"%id", 
						"Show power quality", 			CLIGROUP_USER, 		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "security",    	cmdShowMeterSecurity,			NULL,   			"*%sec-type%sec-name", 
						"Show meter security", 			CLIGROUP_ADMIN, 	CLI_SKIPLOG,    CLI_HIDDEN_CMD },
    { NULL, NULL, NULL, NULL, NULL, 0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowDRAsset[] =
{
    { "list",			cmdShowDRAssetInfo,				NULL,				"*%aidparser",		
						"Show DR asset list", 			CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "level",			cmdShowEnergyLevel,				NULL,				"%id", 		
						"Show energy level", 	        CLIGROUP_USER,      CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};


CLIHANDLER  m_tblShowSensor[] =
{
#ifdef  __SUPPORT_ENDDEVICE__
    { "amr",		    cmdShowSensorAmrData,			NULL,				"%id",		
						"Show sensor AMR data.",		CLIGROUP_USER,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
#endif
    { "battery",		cmdShowSensorBattery,		    NULL,				"%id*%count",		
						"Show sensor battery log.",     CLIGROUP_USER,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "event",		    cmdShowSensorEvent,			    NULL,				"%id*%count",		
						"Show sensor event log.",		CLIGROUP_USER,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "info",			cmdShowSensorInfo,				NULL,				"%id",		
						"Show sensor info.",			CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "last",           cmdShowSensorLastValue,	        NULL,				"*%aidparser", 		
						"Show sensor last values",      CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "list",			cmdShowSensorList,				NULL,				"*%allparser",		
						"Show sensor list", 			CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "path",			cmdShowSensorPath,				NULL,				"*%aidparser%pathviewtype", 		
						"Show sensor routing path", 	CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "port",    	    cmdShowSensorPort,			    NULL,   			"%id%pmask", 
						"Show sensor digital port", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "rominfo",		cmdShowSensorRomInfo,			NULL,				"%id",		
						"Show sensor rom info.",		CLIGROUP_USER,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "summary",		cmdShowSensorSummary,			NULL,				"*%allparser%summaryviewtype", 		
						"Show sensor summary", 	        CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "value",  	    cmdShowSensorValue,		        NULL,  				"%id%sensorcmd",
						"Show sensor value", 		    CLIGROUP_USER, 	    CLI_SKIPLOG,    CLI_HIDDEN_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowFullFunctionDevice[] =
{
    { "list",			cmdShowFFDList,				    NULL,				"*%allparser%fversion%build", 		
						"Show Full Function Device list", 	CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowFw[] =
{
    { "build",			cmdShowFirmwareBuild,           NULL,               NULL, 		
						"Show Firmware build information",CLIGROUP_USER,	CLI_SKIPLOG,    CLI_HIDDEN_CMD },
    { "list",			cmdShowFirmwareList,            NULL,               NULL, 		
						"Show Firmware list", 	        CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowMetering[] =
{
    { "summary",        cmdShowMeteringList,            NULL,               "*%@yy%@mm%@dd",
                        "Show metering summary",        CLIGROUP_GUEST,     CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "lp",             cmdShowLoadProfile,             NULL,               "%id*%@dd%@mm%@yy",
                        "Show load profile",            CLIGROUP_GUEST,     CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "info",           cmdShowMeteringInfo,            NULL,               "%id*%@yy%@mm%@dd%pnum",
                        "Show metering information",    CLIGROUP_GUEST,     CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "log",    	  	cmdShowMeteringLog,            	NULL,               "*%@yy%@mm%@dd",
                        "Show metering log",    		CLIGROUP_GUEST,     CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowMobile[] =
{
    { "info",			cmdShowMobile,					NULL,				NULL,		
						"Show mobile properties",		CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "log",			cmdShowMobileLog,				NULL,				"*%@yy%@mm%@dd", 
						"Show mobile log", 				CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowApn[] =
{
    { "list",			cmdShowApnList,					NULL,				NULL,		
						"Show apn list",				CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "info",			cmdShowApnInfo,					NULL,				"%apn", 
						"Show apn information",			CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowPpp[] =
{
    { "info",			cmdShowPpp,						NULL,				NULL, 		
						"Show PPP configurations", 		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "log",			cmdShowPppLog,					NULL,				NULL, 		
						"Show connection log", 			CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowEvent[] =
{
    { "configure",		cmdShowEventConfigure,			NULL,				NULL,
						"Show event configure", 		CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "log",			cmdShowEvent,					NULL,				"*%@yy%@mm%@dd",
						"Show event log", 			    CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowPhone[] =
{
    { "list",           cmdShowPhoneList,               NULL,               NULL,
                        "Show telephone list",          CLIGROUP_GUEST,     CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetPhone[] =
{
    { "list",           cmdSetPhoneList,                NULL,               "*%q%q%q%q%q%q%q%q%q%q%q%q",
                        "Set Phone List",               CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowTransaction[] =
{
    { "info",			cmdShowTransactionInfo,			NULL,				"%tnum",		
						"Show transaction info.",		CLIGROUP_USER,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "list",			cmdShowTransactionList,			NULL,				"*%tropt%aidparser%fversion%build",		
						"Show transaction list", 		CLIGROUP_USER,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "result",			cmdShowTransactionResult,		NULL,				"%tnum",		
						"Show transaction result", 		CLIGROUP_USER,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowUpgrade[] =
{
	{ "info", 			cmdShowUpgradeInfo,  		 	NULL,   			"%trigger", 
						"Show upgrade information",		CLIGROUP_USER,  	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "list", 			cmdShowUpgradeList,  		 	NULL,   			NULL, 
						"Show upgrade list", 			CLIGROUP_USER,  	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "pkginfo",		cmdShowPackageInfo,				NULL,				"*%disttype",	
						"Show upgrade package info.",	CLIGROUP_USER, 	    CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "status", 		cmdShowUpgradeStatus,  		 	NULL,   			"%trigger", 
						"Show upgrade status", 			CLIGROUP_USER,  	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "log",			cmdShowUpgradeLog,				NULL,				"*%@yy%@mm%@dd",	
						"Show upgrade history log",		CLIGROUP_USER,      CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShowCoordinator[] =
{
    { "list",	        cmdShowCodiList,		   		NULL,               NULL,		
						"Show coordinator list",        CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "register", 		cmdShowCodiRegister,            NULL,   			NULL, 
						"Show coordinator register list",CLIGROUP_USER,     CLI_SKIPLOG,    CLI_HIDDEN_CMD },
	{ "stack", 			cmdShowCodiStackParam,         NULL,   			NULL, 
						"Show coordinator stack parameter",	CLIGROUP_USER,  CLI_SKIPLOG,    CLI_HIDDEN_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblShow[] =
{
#if 0
    // Issue #838 : Agent에서 Alarm Log를 기록하지 않고 있다. 따라서 관련 기능을 제거한다
	{ "alarmlog",		cmdShowAlarmLog,				NULL,				"*%@yy%@mm%@dd",	
						"Show alarm history log",		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
#endif
    { "apn",			NULL,							m_tblShowApn,		NULL,		
						"Show APN information",			CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "cmdlog",		    cmdShowCommandLog,				NULL,				"*%@yy%@mm%@dd",	
						"Show command history log",		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "coordinator",	NULL,		   		            m_tblShowCoordinator,NULL,		
						"Show coordinator information", CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
/** Issue #1426 : 아직 미구현
	{ "debuglog",		cmdShowDebugLog,				NULL,				"*%@yy%@mm%@dd", 
						"Show Debug Log",        		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
*/
    { "dr",			    NULL,		 					m_tblShowDRAsset,	NULL,		
						"Show DR asset information",	CLIGROUP_USER, 	    CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "ethernet",		cmdShowEthernet,				NULL,				NULL,		
						"Show local ip setting",		CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "event",			NULL,					        m_tblShowEvent,		"NULL",
						"Show Event",					CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "ffd",		    NULL,		 					m_tblShowFullFunctionDevice,	NULL,		
						"Show full function device information",		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "flash",			cmdShowFlash,					NULL,				NULL,		
						"Show flash information",		CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "fw",		        NULL,		 					m_tblShowFw,        NULL,		
						"Show firmware information",	CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "gpio",			cmdShowGpio,					NULL,				NULL,		
						"Show hardware in/out pin state", CLIGROUP_GUEST,	CLI_SKIPLOG,    CLI_HIDDEN_CMD },
    { "group",          cmdShowGroup,                   NULL,               "*%gtypeid%group%id",		
                        "Show group entry",             CLIGROUP_USER,      CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "interface",		cmdShowInterface,				NULL,				NULL,		
						"Show Interface", 				CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "memory",			cmdShowMemory,					NULL,				NULL,		
						"Show memory information",		CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "meter",          NULL,                           m_tblShowMeter,     NULL,
                        "Show meter information",       CLIGROUP_GUEST,     CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "metering",       NULL,                           m_tblShowMetering,  NULL,
                        "Show metering information",    CLIGROUP_GUEST,     CLI_SKIPLOG,   CLI_NORMAL_CMD },
    { "mobile",       	NULL,                           m_tblShowMobile,    NULL,
                        "Show mobile information",		CLIGROUP_GUEST,     CLI_SKIPLOG,   CLI_NORMAL_CMD },
	{ "option",			cmdShowEnvironment,				NULL,				NULL,		
						"Show options",					CLIGROUP_GUEST,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "patch",		    cmdShowPatch,					NULL,				NULL,		
						"Show Patch Info.",			    CLIGROUP_ADMIN,		CLI_SKIPLOG,    CLI_HIDDEN_CMD },
/** EDF를 위해서 잠시 막는다 
    { "phone",          NULL,                           m_tblShowPhone,     NULL,     
                        "Show phone number list",       CLIGROUP_GUEST,     CLI_SKIPLOG,    CLI_NORMAL_CMD },
*/
    { "ppp",			NULL,							m_tblShowPpp,		NULL,		
						"Show ppp information",			CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "sensor",			NULL,		 					m_tblShowSensor,	NULL,		
						"Show sensor information",		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "system",			cmdShowSystem,					NULL,				NULL,		
						"Show system information", 		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "timesynclog",	cmdShowTimesyncLog,				NULL,				"*%@yy%@mm%@dd", 
						"Show meter timesync log", 		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "transaction",	NULL,							m_tblShowTransaction,	NULL,		
						"Show transaction information",	CLIGROUP_USER,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "upgrade",		NULL,					        m_tblShowUpgrade,	"NULL",
						"Show Upgrade",					CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
#if 0
	{ "upgradelog",		cmdShowUpgradeLog,				NULL,				"*%@yy%@mm%@dd",	
						"Show upgrade history log",		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
#endif
	{ "uploadlog",		cmdShowUploadLog,				NULL,				"*%@yy%@mm%@dd", 
						"Show auto upload log", 		CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "user",			cmdShowUser,					NULL,				NULL,		
						"Show system user",				CLIGROUP_USER,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { "version",		cmdShowVersion,					NULL,				NULL,		
						"Show Version Info.",			CLIGROUP_GUEST,		CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetSensor[] =
{
    { "activemin",  	cmdSetSensorActiveMin,		    NULL,  				"%id%pnum",
						"Set sensor active minute",	    CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "alarm",  	    cmdSetSensorAlarmFlag,		    NULL,  				"%aid%onoff",
						"Set sensor alarm flag",	    CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "alarmmask",  	cmdSetSensorAlarmMask,		    NULL,  				"%aid%amask",
						"Set sensor alarm mask",	    CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "choice",  	    cmdSetSensorLpChoice,		    NULL,  				"%aid%pnum",
						"Set sensor LP choice(0:today, 1:yesterday, ...)",	CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
#ifdef  __SUPPORT_ENDDEVICE__
    { "currentpulse",  	cmdSetSensorCurrentPulse,		NULL,  				"%id%pulse",
						"Set sensor current pulse",	    CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
#endif
    { "customer",    	cmdSetSensorCustomer,			NULL,   			"%id%customer", 
						"Set customer name", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "failcount",  	cmdSetSensorMeteringFailCnt,	NULL,  				"%aid%bnum",
						"Set sensor metering fail count", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "fixedreset",  	cmdSetSensorResetTime,			NULL,  				"%aid%hour",
						"Set sensor fixed reset time", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "hday",           cmdSetSensorHeartBeatDay,       NULL,               "%aid%endidmask",
						"Set sensor heart beat day",    CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "hquarter",       cmdSetSensorHeartBeatQuarter,   NULL,               "%aid%endiqmask",
						"Set sensor heart beat quarter",CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "install",  	    cmdSetSensorInstall,		    NULL,  				"%id%coretype",
						"Set sensor install command", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "keeptime",  	    cmdSetSensorActiveKeepTime,		NULL,  				"%aid%sec",
						"Set sensor active keep time",	CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
#ifdef  __SUPPORT_ENDDEVICE__
    { "lastpulse",  	cmdSetSensorLastPulse,		    NULL,  				"%id%pulse",
						"Set sensor last pulse",	    CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
#endif
    { "location",   	cmdSetSensorLocation, 			NULL,   			"%id%loc",
						"Set location", 			    CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mday",           cmdSetSensorMeteringDay,        NULL,               "%aid%endidmask",
                        "Set sensor metering day",      CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meterserial",  	cmdSetSensorMeterSerial,		NULL,  				"%id%mid",
						"Set sensor meter serial", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mquarter",       cmdSetSensorMeteringQuarter,    NULL,               "%aid%endiqmask",
                        "Set sensor metering quarter",  CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "networktype",    cmdSetSensorNetworkType,        NULL,               "%aid%ntype",
                        "Set sensor network type",      CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "operday",  	    cmdSetSensorOperDay,		    NULL,  				"%id%pnum",
						"Set sensor operating day",	    CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "period",  	    cmdSetSensorLpPeriod,		    NULL,  				"%aid%lpperiod",
						"Set sensor LP period",	        CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "permitmode",  	cmdSetSensorPermitMode,		    NULL,  				"%aid%onoff",
						"Set sensor permit mode",	    CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "permitstate",  	cmdSetSensorPermitState,		NULL,  				"%aid%onoff",
						"Set sensor permit state",	    CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "port",    	    cmdSetSensorPort,			    NULL,   			"%id%pmask%pvalue", 
						"Set digital port", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "push",           cmdSetSensorPushMetering,		NULL,  				"%aid%onoff",
						"Set sensor push metering flag",CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "reset",  		cmdSetSensorReset,			    NULL,  				"%aid*%num",
						"Set sensor reset",	            CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "rfpower",  	    cmdSetSensorRfPower,		    NULL,  				"%aid%epwr",
						"Set sensor RF power",	        CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
/** Repeater는 아직 정의되지 않았다
    { "setupsec",  	    cmdSetSensorSetupSec,		    NULL,  				"%id%pnum",
						"Set sensor repeating setup sec",CLIGROUP_USER,      CLI_LOGGING,   CLI_NORMAL_CMD },
*/
	/** 2011.03.21 : Added By Wooks for Smoke Detector and Alarm(Siren) Commands
	*/
	{ "siren",			cmdSetSensorSiren,				NULL,				"%id%onoff",
						"Set sensor start siren",		CLIGROUP_USER,		CLI_LOGGING,	CLI_NORMAL_CMD },
	{ "sxfactory",		cmdSetSxFactoryReset,           NULL,				"%id",
						"SX meter factory reset",		CLIGROUP_ADMIN,		CLI_LOGGING,	CLI_HIDDEN_CMD },
	{ "temperature",	cmdSetSensorTemperatureLevel,	NULL,				"%id%num",
						"Set sensor temperature level",	CLIGROUP_USER,		CLI_LOGGING,	CLI_NORMAL_CMD },	
    { "testmode",  		cmdSetSensorTestMode,			NULL,  				"%aid%onoff",
						"Set sensor test mode",	        CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "time",			cmdSetSensorTime,				NULL,				"%aid*%@yy%@mm%@dd%@hh%@nn%@ss%timezone",	
						"Set sensor time",				CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "txpowermode",	cmdSetSensorTxPowerMode,		NULL,				"%id%onoff",
						"Set sensor TX Power Mode",		CLIGROUP_USER,		CLI_LOGGING,	CLI_NORMAL_CMD },
    /** Issue #2575: 삭제되었던 "set sensor command" 명령어를 "set sensor value"로 부활
      */
    { "value",  	    cmdSetSensorValue,		        NULL,  				"%aid%sensorcmd*%hexadata",
						"Set sensor value", 		    CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "vendor", 		cmdSetSensorVendor,			    NULL,   			"%id%vendor",	
						"Set vendor information", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD }, 
    { "verify",  	    cmdSetSensorVerify,		        NULL,  				"%id%filename",
						"Set sensor verify command", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_HIDDEN_CMD },


	////////////////////////////////////////////////////////////////////////////////////////////////////

    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetMobile[] =
{
    { "type",			cmdSetMobileType,				NULL,				"%mobtype",	
						"Set Mobile Type", 				CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mode",			cmdSetMobileMode,				NULL,				"%mobmode",	
						"Set Mobile Mode", 				CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "apn",			cmdSetMobileAPN,				NULL,				"%apn",		
						"Set Mobile APN", 				CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "number",			cmdSetMobileNumber,				NULL,				"%str",		
						"Set Mobile Number", 			CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetTemp[] =
{
    { "range",			cmdSetTempRange,				NULL,				"%tmin%tmax", 
						"Set Temperature Range", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetServer[] =
{
    { "address",		cmdSetServerAddress,			NULL,				"%str",		
						"Set Server Address", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "port",			cmdSetServerPort,				NULL,				"%num",		
						"Set Server Port", 				CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "alarmport",		cmdSetServerAlarmPort,			NULL,				"%num",		
						"Set Server Alarm Port", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetLocal[] =
{
    { "address",		cmdSetLocalAddress,				NULL,				"%ip%netmask%gw", 
						"Set Local Server Port", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "port",			cmdSetLocalPort,				NULL,				"%num",		
						"Set Local Server Port", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetCodi[] =
{
    { "autosetting",	cmdSetCoordinatorAutoSetting,	NULL,				"%autoset",		
						"Set Coordinator Auto Setting", CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "channel",		cmdSetCoordinatorChannel,		NULL,				"%ch",		
						"Set Coordinator Channel",		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "discovery",		cmdSetCoordinatorDiscovery,	    NULL,				"%onoff",		
						"Set Coordinator Route Discovery Option",CLIGROUP_ADMIN,CLI_LOGGING,CLI_HIDDEN_CMD },
/** 현재 사용되지 않기 때문에 뺀다
    { "extpanid",		cmdSetCooordinatorExtPanID,		NULL,				"%extpanid",		
						"Set Coordinator Extended PAN-ID", CLIGROUP_USER, 	CLI_LOGGING,    CLI_NORMAL_CMD },
 */
    { "hop",			cmdSetCoordinatorHops,	        NULL,				"%num",		
						"Set Coordinator Multicast HOPs",CLIGROUP_ADMIN,    CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "linkkey",		cmdSetCoordinatorLinkKey,		NULL,				"%linkkey",		
						"Set Coordinator Link Key", 	CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "mtortype",		cmdSetCoordinatorMtorType,	    NULL,				"%mtortype",		
						"Set Coordinator MTOR Type", 	CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "networkkey",		cmdSetCoordinatorNetworkKey,	NULL,				"%netkey",		
						"Set Coordinator Network Key", 	CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "panid",			cmdSetCooordinatorPanID,		NULL,				"%panid",		
						"Set Coordinator PAN-ID", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "permit",			cmdSetCoordinatorPermitTime,	NULL,				"%permit",		
						"Set Coordinator Permit Time", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "power",			cmdSetCoordinatorRfPower,		NULL,				"%pwr",		
						"Set Coordinator RF Power", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "security",		cmdSetCoordinatorSecurity,		NULL,				"%csecurity",		
						"Set Coordinator Security", 	CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "txpowermode",	cmdSetCoordinatorTxPowerMode,	NULL,				"%txmod",		
						"Set Coordinator TX Power Mode", CLIGROUP_USER, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetSystem[] =
{
    { "id",				cmdSetId,						NULL,				"%num",		
						"Set MCU ID", 					CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "name",			cmdSetName,						NULL,				"%str",		
						"Set MCU Name", 				CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "descr",			cmdSetDescr,					NULL,				"%str",		
						"Set MCU Description", 			CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "location",		cmdSetLocation,					NULL,				"%str",		
						"Set MCU Location", 			CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "contact",		cmdSetContact,					NULL,				"%str",		
						"Set Contact Info.", 			CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "time",			cmdSetTime,						NULL,				"%@yy%@mm%@dd%@hh%@nn%@ss",	
						"Set MCU Time",					CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mode",			cmdSetOpMode,					NULL,				"%opmode",	
						"Set MCU Operation mode",		CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "tzone",			cmdSetTimezone,					NULL,				"%tzhour",	
						"Set MCU Time Zone",			CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "power",			cmdSetPowerType,				NULL,				"%power",	
						"Set MCU Power Type",			CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_HIDDEN_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetEthernet[] =
{
    { "type",			cmdSetEthernetType,				NULL,				"%ethtype", 
						"Set Ethernet Type", 			CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetSaveday[] =
{
#if 0
    // Issue #838 : Agent에서 Alarm Log를 기록하지 않고 있다. 따라서 관련 기능을 제거한다
    { "alarmlog",		cmdSetSavedayAlarmLog,			NULL,				"%saveday", 
						"Set alarm log saveday", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
#endif
    { "cmdlog",			cmdSetSavedayCmdLog,			NULL,				"%saveday", 
						"Set command log saveday", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
/** Issue #1127: Comm Log를 CLI에서 제거
    { "commlog",		cmdSetSavedayCommLog,			NULL,				"%saveday", 
						"Set communication log saveday", CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
*/
/** Issue #1426 : 아직 미구현
    { "debuglog",		cmdSetSavedayDebugLog,			NULL,				"%saveday", 
						"Set debug log saveday",        CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
*/
    { "eventlog",		cmdSetSavedayEventLog,			NULL,				"%saveday", 
						"Set event log saveday", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meteringlog",	cmdSetSavedayMeterLog,		    NULL,				"%saveday", 
						"Set metering log saveday", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mobilelog",		cmdSetSavedayMobileLog,			NULL,				"%saveday", 
						"Set mobile log saveday", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "timesynclog",	cmdSetSavedayTimesyncLog,		NULL,				"%saveday", 
						"Set meter timesync log saveday",CLIGROUP_USER, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "uploadlog",		cmdSetSavedayUploadLog,			NULL,				"%saveday", 
						"Set upload log saveday", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "upgradelog",		cmdSetSavedayUpgradeLog,		NULL,				"%saveday", 
						"Set upgrade log saveday", 		CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetOptionAutoupload[] =
{
    { "weekly",         cmdSetOptionAutouploadWeekly,   NULL,               "%mmday%hour%min%while",
                        "Setup weekly upload",          CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "daily",          cmdSetOptionAutouploadDaily,    NULL,               "%daymask%hour%min%while",
                        "Setup daily upload",           CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "hourly",         cmdSetOptionAutouploadHourly,   NULL,               "%hourmask%min%while",
                        "Setup hourly upload",          CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "immediately",    cmdSetOptionAutouploadImmediately, NULL,            "%while",
                        "Setup immediately upload",     CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSetOption[] =
{
    { "autoreset",		cmdSetOptionAutoreset,			NULL,				"%day%hour%min",		
						"Set auto reset schedule", 		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "autoupload",     NULL,                           m_tblSetOptionAutoupload, NULL,
                        "Setup auto upload schedule",   CLIGROUP_USER,      CLI_LOGGING,     CLI_NORMAL_CMD },
    { "memorycheck",	cmdSetOptionMemorycheck,		NULL,				"%rate",		
						"Set memory critical rate",	    CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "flashcheck",		cmdSetOptionFlashcheck,			NULL,				"%rate",		
						"Set flash critical rate", 	    CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "cleaning",		cmdSetOptionSensorCleaningThreshold,NULL,			"%day",		
						"Set sensor cleaning threshold",CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "dumpfile",		cmdSetOptionDumpFile,           NULL,		        "%filename",		
						"Set debuging dump file",       CLIGROUP_USER, 	    CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "eventdelay", 	cmdSetOptionEventSendDelay,		NULL,				"%evd",		
						"Set event send delay", 		CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "eventfilter", 	cmdSetOptionEventAlertLevel,	NULL,				"%evl",		
						"Set event filtering level", 	CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "eventnotify", 	cmdSetOptionEventNotify,	    NULL,				"%evt%onoff",		
						"Set event notifiy configure",  CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "eventreadschedule", cmdSetOptionEventReadSchedule,	NULL,			"%daymask%hourmask",		
						"Set meter event log read schedule",CLIGROUP_USER,	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "eventrevert", 	cmdSetOptionEventConfigurationDefault,	    NULL,	"%evt",		
						"Set event configure default",  CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "eventseverity", 	cmdSetOptionEventSeverity,	    NULL,				"%evt%evl",		
						"Set event severity configure", CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "keepalive",		cmdSetOptionKeepalive,			NULL,				"%intv-min",		
						"Set keepalive event", 		    CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meteringinterval",cmdSetOptionMeteringInterval,	NULL,				"%intv-min", 
						"Set metering inteval(min)", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meteringsaveday", cmdSetOptionMeteringSaveday,	NULL,				"%saveday", 
						"Set metering data saveday", 	CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meteringschedule", cmdSetOptionMeteringSchedule,	NULL,				"%daymask%hourmask%min%retry%period",		
						"Set metering schedule",		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meteringthread",	cmdSetOptionMeteringThread,		NULL,				"%thread",		
						"Set metering thread count",	CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meteringtype",   cmdSetOptionMeteringType,	    NULL,				"%meteringoption",		
						"Set metering default option",CLIGROUP_USER,	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "metersecurity",	cmdSetOptionMeterSecurity,		NULL,				"%mstype%msecurity",		
						"Set meter security", 			CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meteruser",		cmdSetOptionMeterUser,			NULL,				"%muser",		
						"Set meter user id", 			CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mobilelinkcheck", cmdSetOptionMobileLinkCheck,	NULL,				"%intv-min",		
						"Set mobile link check interval", CLIGROUP_USER, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "nazctimesync",   cmdSetOptionNetworkTimesync,	NULL,				"%intv-min",		
						"Set NAZC timesync interval",   CLIGROUP_USER,	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "recoveryschedule", cmdSetOptionRecoverySchedule,	NULL,				"%daymask%hourmask%min%retry%period",		
						"Set recovery schedule",		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "pingmethod", 	cmdSetOptionPingMethod,			NULL,				"%pingmethod",		
						"Set mobile link check method (ICMP/SOCKET)", CLIGROUP_USER, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "pingschedule", 	cmdSetOptionPollingSchedule,	NULL,				"%daymask%hourmask%min%retry%period",		
						"Set meter ping schedule",		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "scheduler",      cmdSetOptionSchedulerType,	    NULL,				"%schedulertype",		
						"Set scheduler type",           CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "scanningstrategy",cmdSetOptionScanningStrategy,	NULL,				"%sstrategy",		
						"Set inventory scanning strategy", CLIGROUP_USER,	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "sensorlimit",	cmdSetOptionSensorLimit,		NULL,				"%num",		
						"Set sensor limit", 		    CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "timesyncschedule", cmdSetOptionTimesyncSchedule,	NULL,				"%daymask%hourmask",		
						"Set meter timesync schedule",	CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "timesyncstrategy", cmdSetOptionTimesyncStrategy,	NULL,				"%tstrategy",		
						"Set meter timesync strategy",	CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "timesyncthreshold",cmdSetOptionTimesyncThreshold,NULL,				"%mtmin%mtmax",		
						"Set meter timesync threshold(High/Low)",CLIGROUP_USER,CLI_LOGGING, CLI_NORMAL_CMD },
    { "transaction",    cmdSetOptionTransactionThreshold,NULL,				"%day",		
						"Set event transaction threshold",CLIGROUP_USER,	CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblSet[] =
{
    { "system",			NULL,		  					m_tblSetSystem,		NULL,		
						"System Setting", 				CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "coordinator",	NULL,		    				m_tblSetCodi,		NULL,		
						"CODI Setting", 				CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mobile",			NULL,		  					m_tblSetMobile,		NULL,		
						"Mobile Setting", 				CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
/** EDF를 위해 잠시 막는다
    { "phone",          NULL,                           m_tblSetPhone,      NULL,  
                        "Phone List Setting",           CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
*/
    { "ethernet",		NULL,	    					m_tblSetEthernet,	NULL,		
						"Ethernet Setting", 			CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "server",			NULL,		  					m_tblSetServer,		NULL,		
						"Server Setting", 				CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "local",			NULL,		   					m_tblSetLocal,		NULL,		
						"Local Server Setting",			CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "temp",			NULL,		    				m_tblSetTemp,		NULL,		
						"Temperature Setting", 			CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "dr",			    NULL,	 	  					m_tblSetDRAsset,    NULL,		
						"DR Setting",                   CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "sensor",			NULL,	 	  					m_tblSetSensor,	    NULL,		
						"Sensor Setting",				CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "option",			NULL,							m_tblSetOption,		NULL, 
						"Setup option", 				CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "saveday",		NULL,							m_tblSetSaveday,	NULL, 
						"Saveday setting", 				CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "limit",			NULL,							m_tblSetLimit,		NULL, 
						"Limit Setting", 				CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "password",		cmdSetPassword,					NULL,				"%ps16", 
						"Change password", 				CLIGROUP_USER, 		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "gpio",			cmdSetGpio,	     				NULL,				"%num%num",	
						"GPIO Value Setting", 			CLIGROUP_USER,		CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "patch",			cmdSetPatch,	     			NULL,				"%num%onoff",	
						"Set Patch State", 			    CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_HIDDEN_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblMcu[] =
{
    { "install",		cmdInstall,						NULL,				NULL,		
						"MCU Install", 					CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "uninstall",		cmdUninstall,					NULL,				NULL,		
						"MCU Uninstall", 				CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "diagnosis",		cmdDiagnosis,					NULL,				NULL,		
						"Diagnosis", 					CLIGROUP_USER, 	    CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "cleaning",		cmdGarbageCleaning,				NULL,				NULL,		
						"Garbage cleaning", 			CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
	{ "backup",			cmdBackupAll,					NULL,               NULL,		
						"Backup data & configuration", 	CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
	{ "restore",		cmdRestore,						NULL,				NULL,		
						"Restore backup file",			CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblPing[] =
{
	{ "server",		    cmdPing,						NULL,				"%ip",		
						"Ping test",					CLIGROUP_GUEST, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "meter",		    cmdCheckMeter,					NULL,				"%id",		
						"Meter ping test (single)",		CLIGROUP_GUEST, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "start",          cmdMeterPingStart,         		NULL,   			NULL, 	
						"Start ping test (all)", 		CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "stop",           cmdMeterPingStop,         		NULL,   			NULL, 	
						"Stop ping test (all)", 		CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblAddMeter[] =
{
    { "security",	    cmdAddMeterSecurity,		    NULL,				"%sec-type%sec-name%mstype%msecurity",		
						"Add meter security", 			CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};
CLIHANDLER  m_tblAdd[] =
{
    { "apn",            cmdAddApn,          	   		NULL, "%apnname%user%ppass%concmd%concmd1*%options", 	
						"Add mobile apn", 				CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "group",          cmdAddGroup,                    NULL,               "%gtype%group%idlist",		
                        "Add group entry",              CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "register",       cmdAddCoordiRegister,           NULL,               "%id",		
                        "Add coordinator register",     CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "meter",	        NULL,		                    m_tblAddMeter,		NULL,		
						"Add meter property", 			CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "sensor",         cmdAddSensor,          	   		NULL,   			"%id", 	
						"Add sensor", 					CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "user",           cmdAddUser,             		NULL,   			"%user%ppass%perm", 	
						"Add user", 					CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "table",		cmdSetIHDTable,					NULL,				"%id", 		
						"Set IHD TABLE", 	        	CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },

    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblDeleteMeter[] =
{
    { "instance",       cmdDeleteMeter,                 NULL,               "%aid%num",
                        "Delete Meter Instance",        CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "security",       cmdDeleteMeterSecurity,         NULL,               "%sec-type%sec-name",
                        "Delete Meter Instance",        CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_tblDelete[] =
{
    { "apn",            cmdDeleteApn,    	      		NULL,   			"%apn",    
						"Delete APN",          			CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "group",          cmdDeleteGroup,                 NULL,               "*%gtypeid%group%id",		
                        "Delete group entry",           CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "meter",          NULL,                           m_tblDeleteMeter,   NULL,
                        "Delete Meter command",         CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { "register",       cmdDeleteCoordiRegister,        NULL,               "%id",		
                        "Delete coordinator register",  CLIGROUP_USER,      CLI_LOGGING,    CLI_HIDDEN_CMD },
#if     defined(__SUPPORT_ADVANCED_LEAVE__)
    { "sensor",			cmdDeleteSensor,				NULL,				"%aid*%ch%panid",	
#else
    { "sensor",			cmdDeleteSensor,				NULL,				"%aid",	
#endif
						"Delete Sensor", 				CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "transaction",    cmdDeleteTransaction,           NULL,               "%pnum",		
                        "Delete transaction",           CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "upgrade", 		cmdUpgradeCancel,  		 		NULL,   			"%trigger", 
						"Delete Upgrade request", 		CLIGROUP_ADMIN,  	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "user",           cmdDeleteUser,          		NULL,   			"%user",    
						"Delete User",          		CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "table",			cmdDelIHDTable,					NULL,				"%id", 		
						"Del IHD TABLE", 	        	CLIGROUP_USER,      CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};


CLIHANDLER  m_tblOndemand[] =
{
    { "metering",		cmdOndemand,				    NULL,				"%id*%option%offset%count",		
						"Ondemand metering", 			CLIGROUP_GUEST,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "bypass",		    cmdOndemandBypass,			    NULL,				"%id%mstype%data",		
						"Ondemand bypass", 			    CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "relay",		    cmdOndemandRelay,			    NULL,				"%id%opcode",		
						"Ondemand relay operation",		CLIGROUP_USER,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mccb",		    cmdOndemandMccb,			    NULL,				"%id%mccb",		
						"Ondemand aidon mccb operation", CLIGROUP_USER,	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "kmp",		    cmdOndemandCid, 			    NULL,				"%id%cid*%hexadata",		
						"Ondemand KMP operation",       CLIGROUP_USER,	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { "mbus",		    cmdOndemandMbus, 			    NULL,				"%id%port%action%mbuscmd*%hexadata",		
						"Ondemand mbus operation",      CLIGROUP_USER,	    CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "timesync",		cmdOndemandTimesync,			NULL,				"%id",		
						"Ondemand meter timesync",		CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "recovery",		cmdOndemandMeterFailRecovery,	NULL,				"%id",		
						"Ondemand meter fail recovery",	CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER	m_tblRecovery[] =
{
	{ "metering",		cmdRecovery,		            NULL,	            NULL,
                        "Retry failed metering schedule",CLIGROUP_USER, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "meter",		    cmdGetMeterSchedule,		    NULL,	            "%id*%option%offset%count",
                        "Recovery metering data"        ,CLIGROUP_USER, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER	m_tblTimesync[] =
{
	{ "sensor",			cmdMulticastTime,	    		NULL,	            NULL,
                        "Time synchronization of sensor",CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "meter",  	    cmdMeterTimesync,  		 		NULL,   			NULL, 
						"Time synchronization of meter", CLIGROUP_ADMIN,  	CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER	m_tblDistribution[] =
{
	{ "agent",	        cmdFirmwareDownload,	    		NULL,	            NULL,
                        "Upgrade agent firmware",	        CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "coordinator",	cmdCoordinatorFirmwareDownload,		NULL,				"%filename",		
						"Coordinator Firmware Upgrade", 	CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
    { "powercontrol",	cmdPowerControlFirmwareDownload,	NULL,				"%filename",
						"Sensor Firmware Upgrade",          CLIGROUP_ADMIN,	    CLI_LOGGING,    CLI_HIDDEN_CMD },
    { "sensor",	        cmdSensorFirmwareDownload,	        NULL,				"%parser%hversion%fversion%build%filename*%id",
						"Sensor Firmware Upgrade",          CLIGROUP_ADMIN,	    CLI_LOGGING,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

//////////////////////////////////////////////////////////////////////////
//  [10/20/2010 DHKim]
//  그룹 CLI Table
//////////////////////////////////////////////////////////////////////////
CLIHANDLER m_tblGroup[] =
{
	{ "command",			cmdGroupAsyncCall,	    		NULL,	            "%groupkey%cmd%tropt%saveday",
						"Group Command",							CLIGROUP_USER, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "add",			cmdGroupAdd,	    		NULL,	            "%group",
						"Group Add",				CLIGROUP_USER, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "delete",  	    cmdGroupDelete,  		 		NULL,   			"%groupkey", 
						"Group Delete",					CLIGROUP_USER,  	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "addmember",			cmdGroupAddMember,	    		NULL,	            "%groupkey%id",
						"Group Add Member",							CLIGROUP_USER, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "deletemember",  	    cmdGroupDeleteMember,  		 		NULL,   			"%groupkey%id", 
						"Group Delete Member",						CLIGROUP_USER,  	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "info",  	    cmdGroupInfo,  		 		NULL,   			"*%group%groupnameinfo", 
						"Group Information",		CLIGROUP_USER,  	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "showcommand",  	    cmdGroupShowCommand,  		 		NULL,   			"*%groupkey", 
						"Show Group Command",							CLIGROUP_USER,  	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "tabledelete",  	    cmdGroupTableDelete,  		 		NULL,   			"%tablekind", 
						"Group Table Delete",							CLIGROUP_USER,  	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

//////////////////////////////////////////////////////////////////////////
//  [4/21/2011 DHKim]
//  Join Table CLI
//////////////////////////////////////////////////////////////////////////
#if 0 // Issue #835 스웨덴향 집중기는 join control을 지원하지 않는다
CLIHANDLER m_tblJoin[] =
{
	{ "show",			cmdJointableShow,	    		NULL,	            NULL,
						"Jointable Show",							CLIGROUP_USER, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "add",			cmdJointableAdd,	    		NULL,	            "*%id",
						"Jointable Add",							CLIGROUP_USER, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "delete",			cmdJointableDelete,	    		NULL,	            "*%id",
						"Jointable Delete",							CLIGROUP_USER, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};
#endif
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	Main-Command Handler Tables
//------------------------------------------------------------------------------

CLIHANDLER	m_CliCommandHandlers[] =
{
	{ "add",			NULL,		     				m_tblAdd,			NULL,		
						"Add commands",     			CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "delete",			NULL,		     				m_tblDelete,		NULL,		
						"Delete commands", 	    		CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "show",			NULL,		   	   				m_tblShow,			NULL,		
						"Show information",				CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "set",			NULL,			    			m_tblSet,			NULL,		
						"Setting commands", 			CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "enable",			NULL,		     				m_tblEnable,		NULL,		
						"Disable functions",			CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "disable",		NULL,		    				m_tblDisable,		NULL,		
						"Enable functions", 			CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "reset",			NULL,			  				m_tblReset,			NULL,		
						"Reset", 						CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "ondemand",		NULL,							m_tblOndemand,		NULL,	
						"Ondemand commands",			CLIGROUP_GUEST, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "metering",   	 cmdQuery,  	      		 	NULL,   			"*%option%offset%count", 
						"Metering All", 				CLIGROUP_GUEST,  	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "recovery",       NULL,      	                    m_tblRecovery,      "*%@yy%@mm%@dd%@hh",
                        "Recover metering data",        CLIGROUP_GUEST,     CLI_LOGGING,    CLI_NORMAL_CMD },
    { "upload",       	cmdMeterUpload,                 NULL,               "*%@yy%@mm%@dd%@hh",
                        "Upload metering data",         CLIGROUP_GUEST,     CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "timesync",  	    NULL,  		 		            m_tblTimesync,   	NULL, 
						"Time synchronization",         CLIGROUP_ADMIN,  	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "ping",			NULL,							m_tblPing,			NULL,	
						"Meter ping test",		        CLIGROUP_GUEST, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "upgrade",		NULL,							m_tblDistribution,	NULL,		
						"Firmware upgrade", 			CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "scan",		    NULL,		   					m_tblScan,		NULL,		
						"Scanning",				        CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "store",			cmdStore,						NULL,				NULL,	
						"Download data file", 			CLIGROUP_USER, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "mcu",			NULL,							m_tblMcu,			NULL,		
						"MCU management operations", 	CLIGROUP_USER, 	    CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "logout",			cmdLogout,						NULL,				NULL,		
						"Logout", 						CLIGROUP_GUEST, 	CLI_LOGGING,    CLI_NORMAL_CMD },
	{ "shutdown",		cmdShutdown,		  			NULL,			    NULL,		
						"System shutdown", 				CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_NORMAL_CMD },
    { "ls",				cmdShowFileList,				NULL,				"*%dir",	
						"Show file list", 				CLIGROUP_USER, 		CLI_SKIPLOG,    CLI_HIDDEN_CMD },
    { "ps",				cmdShowProcess,					NULL,				NULL,		
						"Show process", 				CLIGROUP_USER, 		CLI_SKIPLOG,    CLI_HIDDEN_CMD },
    { "kill",			cmdKillProcess,					NULL,				"%pid",		
						"Kill process",					CLIGROUP_ADMIN,		CLI_LOGGING,    CLI_HIDDEN_CMD },
	{ "shell",			cmdShell,						NULL,				"%command",	
						"Shell command",				CLIGROUP_ADMIN, 	CLI_LOGGING,    CLI_HIDDEN_CMD },
	{ "history",		cmdHistory,						NULL,				NULL,		
						"Command history", 				CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "help",			cmdHelp,						NULL,				NULL,		
						"Help", 						CLIGROUP_GUEST, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	//  [10/20/2010 DHKim]
	{ "group",		NULL,						m_tblGroup,			NULL,
						"MCU group operations", 	CLIGROUP_USER, 	    CLI_SKIPLOG,    CLI_NORMAL_CMD },
#if 0 // Issue #835 스웨덴향 집중기는 join control을 지원하지 않는다
	{ "jointable",		NULL,						m_tblJoin,			"*%id",
						"MCU join operations", 	CLIGROUP_USER, 	    CLI_SKIPLOG,    CLI_NORMAL_CMD },
#endif
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER	m_CliEmergencyHandlers[] =
{
	{ "ping",			cmdEmergencyPing,		NULL,	"%ip",		"Ping", 				0, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "shell",			cmdEmergencyShell,		NULL,	"%command",	"Shell Command", 		0, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "download",		cmdEmergencyFirmwareDownload,	NULL, NULL,	"Download", 			0, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
	{ "reset",			cmdEmergencyReset,		NULL,	NULL,		"Reset", 				0, 	CLI_SKIPLOG,    CLI_NORMAL_CMD },
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER  m_CliDebugCommandHandlers[] =
{
    { NULL, NULL, NULL,	NULL, NULL,	0, FALSE, FALSE }
};

CLIHANDLER	*m_pCommandHandlers = m_CliCommandHandlers;

