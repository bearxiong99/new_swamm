#ifndef __MCU_DEFINES_H__
#define __MCU_DEFINES_H__

#define ENDIFILE_PATH                   "/app/endi"
#define MEMBER_PATH                     "/app/member"
#define LOGFILE_PATH					"/app/log"
#define EVENT_PATH						"/app/event"
#define CONFIGURE_PATH					"/app/conf"
#define DATAFILE_PATH					"/app/data"
#define TMP_DATAFILE_PATH				"/app/data"
#define FIRMWARE_PATH					"/app/sw"
#define FIRMWARE_COMPRESS_FILE			"/tmp/aimir.tar.gz"
#define SETTING_FILENAME        		"/app/conf/mcu.conf"
#define VARCONF_FILENAME                "/app/conf/var.conf"
#define VARCONF_OLD_FILENAME            "/app/conf/var.conf.old"

#define LM75							"/proc/sys/dev/sensors/lm75-i2c-0-48/temp"

#define RTC1307							"/proc/driver/ds1307"
#define RTC1339							"/proc/driver/ds1339"
#define RTC1340							"/proc/driver/ds1340"

#define TEMP_DEVICE						LM75
#define RTC_DEVICE						RTC1340

#define MAX_SCUDATA_COUNT				8				// 최대 데이터 프레임 갯수
#define MAX_SINKFRAME_SIZE				128				// 최대 데이터 패킷 길이

#define MAX_INTERFACE_COUNT				6				// 최대 인터페이스 갯수
#define MAX_SINK_COUNT					2				// 최대 SINK 갯수

#define MAX_SCU_COUNT					512				// 최대 SCU COUNT
#define MAX_SCU_PORT					4				// SCU 포트 갯수

#define MAX_SINK_BIND_COUNT				32				// 최대 바인드 허용 범위
#define MAX_RECVQ_SIZE					1024			// 최대 수신 버터 크기

#define MAX_ROM_READ_BYTES              63              // ROM Read시 Frame당 최대 Byte
#define MAX_OTA_BYTES                   64              // OTA시 Frame당 최대 Byte

#define BIND_UNUSED						0
#define BIND_ACTIVE						1

#define SENSORMETHOD_WAKEUP				0
#define SENSORMETHOD_POLLING			1

#define SENSORCLOSE_REASON_NORMAL		0
#define SENSORCLOSE_REASON_TIMEOUT		1
#define SENSORCLOSE_REASON_REFUSE		2
#define SENSORCLOSE_REASON_ERROR		3

#define DISABLE_SERVICE					0x0001
#define DISABLE_LICENCE					0x0002

#define SINKSET_MASK_CHANNEL			0x80
#define SINKSET_MASK_PANID				0x40
#define SINKSET_MASK_TIME				0x20

#define GWMSG_SHUTDOWN					100				// POWER OFF MESSAGE
#define GWMSG_RESET						101				// RESET MESSAGE
#define GWMSG_FACTORYDEFAULT			102				// FACTORY DEFAULT MESSAGE
#define GWMSG_FIRMWARE_UPDATE			200				// FIRMWARE UPDATE MESSAGE

#define TIMESYNC_MCU					0				// MCU의 RealTimeClock 재설정
#define TIMESYNC_SCU					1				// SCU의 RealTimeClock 재설정

#define LED_OFF							0				// LED OFF VALUE
#define LED_ON							1				// LED ON VALUE

#define VDD_OFF							0				// VDD OFF VALUE
#define VDD_ON							1				// VDD ON VALUE

#define RESET_UNKNOWN					-1				// 알수 없음
#define RESET_SYSTEM					0				// 리셋 
#define RESET_FACTORY_DEFAULT			1				// 초기화
#define RESET_SINK1						10				// SINK#1 리셋
#define RESET_SINK2						11				// SINK#2 리셋
#define RESET_CDMA						12				// CDMA
#define RESET_ADSL						13				// ADSL

#define CODI_TYPE_UNKNOWN				0
#define CODI_TYPE_EMBER_STACK_331		1
#define CODI_TYPE_EMBER_STACK_333		2
#define CODI_TYPE_ZIGBEE_STACK_25X		10
#define CODI_TYPE_ZIGBEE_STACK_30X		11
#define CODI_TYPE_ZIGBEE_STACK_31X		12
#define CODI_TYPE_ZIGBEE_STACK_32X		13

#define SENSOR_TYPE_UNKNOWN				99				// 알수 없음
#define SENSOR_TYPE_ZRU					1				// 전자식 계량기
#define SENSOR_TYPE_ZMU					2				// 알람 UNIT
#define SENSOR_TYPE_ZEUPC				3				// PC
#define SENSOR_TYPE_ZEUPDA				4				// PDA
#define SENSOR_TYPE_ZEUPLS				5				// 펄스식 계량기
#define SENSOR_TYPE_ZEUEISS				6
#define SENSOR_TYPE_ZEUPQ				7
#define SENSOR_TYPE_ZEUIO				8
#define SENSOR_TYPE_MMIU				11              // 고압모뎀
#define SENSOR_TYPE_IEIU                13              // 집단에너지모뎀
#define SENSOR_TYPE_ZEUMBUS             14
#define SENSOR_TYPE_IHD                 15
#define SENSOR_TYPE_ACD                 16
#define SENSOR_TYPE_HMU                 17
#define SENSOR_TYPE_CONVERTOR           19              // Ethernet Converor
#define SENSOR_TYPE_REP				    98

//SVC 서비스 타입을 기술한다.(1:전기, 2:가스,3:수도,
//4:온수, 5:냉방, 6:열량, 7:보정기(대용량 가스)
#define SERVICE_TYPE_UNKNOWN			0
#define SERVICE_TYPE_ELEC				1				// 전기
#define SERVICE_TYPE_GAS				2				// 가스
#define SERVICE_TYPE_WATER				3				// 수도
#define SERVICE_TYPE_WARM               4               // 온수 
#define SERVICE_TYPE_COOLING            5               // 냉방   //집중기 안씀
#define SERVICE_TYPE_HEAT               6               // 열량      //집중기 안씀
#define SERVICE_TYPE_VCORR              7               // 보정기     //집중기 안씀
#define SERVICE_TYPE_MBUS               8               // MBUS
#define SERVICE_TYPE_SMOKEDETECTOR      9               // Smoke Detector
#define SERVICE_TYPE_INTERFACE          10              // Interface Device (IHD)


#define HOST_TYPE_UNKNOWN				0				// 알수 없음
#define HOST_TYPE_MCU					1				// MCU
#define HOST_TYPE_MCT					2				// MCT
#define HOST_TYPE_ZIGBEE2SERIAL			3				// Zigbee2Serial SINK
#define HOST_TYPE_OTHER					4				// 기타

#define STATE_LINK_DEFAULT				0x1000			// 기본 네트워크 장비
#define STATE_LINK_DOWN					0x0000			// 비활성화 
#define STATE_LINK_UP					0x0001			// 활성화

#define PWRSRC_UNKNOWN					0				// 알수 없음
#define PWRSRC_LINEPOWER				1				// LINE POWER NODE
#define PWRSRC_BATTERY					2				// BATTERY NODE

#define SORT_BY_GUID					0				// ID순 소트
#define SORT_BY_LINKTIME				1				// LINK순

// System Vendor
#define SYSTEM_VENDOR_UNKNOWN			0
#define SYSTEM_VENDOR_NURITELECOM		1

// CDMA Vendor
#define MOBILE_VENDOR_UNKNOW			0
#define MOBILE_VENDOR_GW4010K			100
#define MOBILE_VENDOR_GW5035C			101
#define MOBILE_VENDOR_DTSS800			102
#define MOBILE_VENDOR_DTS1800			103

// GSM/GPRS Vendor
#define MOBILE_VENDOR_MC55				200
#define MOBILE_VENDOR_WAVECOM			201

// Metering Vendor
#define METER_VENDOR_NOTSET             -1
#define METER_VENDOR_UNKNOWN            0x00
#define METER_VENDOR_KAMSTRUP           0x01
#define METER_VENDOR_GE                 0x02
#define METER_VENDOR_ELSTER             0x03
#define METER_VENDOR_LANDIS_GYR         0x04
#define METER_VENDOR_AIDON              0x05
#define METER_VENDOR_LSIS               0x06
#define METER_VENDOR_WIZIT              0x07
#define METER_VENDOR_ACTARIS            0x08
#define METER_VENDOR_GASMETER           0x09
#define METER_VENDOR_EDMI               0x0A
#define METER_VENDOR_KETI               0x0B
#define METER_VENDOR_NAMJUN             0x0C        	// 남전사
#define METER_VENDOR_TAIHAN             0x0D        	// 대한전선
#define METER_VENDOR_ABB                0x0E
#define METER_VENDOR_KHE                0x0F        	// 금호전기
#define METER_VENDOR_ILJINE             0x10        	// 일진전기
#define METER_VENDOR_TGE                0x11        	// 태광전기
#define METER_VENDOR_PSTEC              0x12        
#define METER_VENDOR_KT                 0x13
#define METER_VENDOR_SEOCHANG           0x14        	// 서창
#define METER_VENDOR_CHUNIL             0x15        	// 천일계전
#define METER_VENDOR_AMRTECH            0x16
#define METER_VENDOR_DMPOWER            0x17
#define METER_VENDOR_AMSTECH            0x18
#define METER_VENDOR_OMNI               0x19        	// 옴니시스템
#define METER_VENDOR_MICRONIC           0x1A        	// 마이크로닉스
#define METER_VENDOR_HYUPSIN            0x1B        	// 협신전기
#define METER_VENDOR_MSM                0x1C
#define METER_VENDOR_POWERPLUS          0x1D
#define METER_VENDOR_YPP                0x1E        
#define METER_VENDOR_PYUNGIL            0x1F        	// 평일
#define METER_VENDOR_AEG                0x20
#define METER_VENDOR_ANSI               0x21
#define METER_VENDOR_SENSUS             0x22
#define METER_VENDOR_ITRON              0x23
#define METER_VENDOR_KROMSCHRODER       0x24
#define METER_VENDOR_SIEMENS            0x25
#define METER_VENDOR_MITSUBISHI         0x26
#define METER_VENDOR_OSAKI              0x27
#define METER_VENDOR_KAMSTRUP_2         0x28            // Issue #977 : Kamstrup 신규 검침 Format을 사용하는 미터일 경우
#define METER_VENDOR_KAMSTRUP_3         0x29            // Issue #853 : Kamstrup Omnipower meter를 위한 신규 검침 Format을 사용하는 미터일 경우

#define METER_VENDOR_ETC                0xFE        	// 기타
#define METER_VENDOR_NURI               0xFF

// MBUS Model
#define MBUS_MODEL_COMPACT              1
#define MBUS_MODEL_401                  2

// Metering Strategy
#define METERING_STRATEGY_SERIAL        0
#define METERING_STRATEGY_DISTRIBUTE    1

// Scanning Strategy
#define SCANNING_STRATEGY_LAZY          0               // 최대한 늦게 수행
#define SCANNING_STRATEGY_IMMEDIATELY   1               // 즉시 수행

// Datafile name length
#define DATE_TIME_LEN                   10

// Operation Mode
#define OP_MODE_NORMAL                  0x00
#define OP_MODE_TEST                    0x01
#define OP_MODE_PULSE_MESH              0x02

// Meter Error Type
#define ERR_TYPE_CLOCK_ERROR            0x00
#define ERR_TYPE_LP_STOP_ERROR          0x01

// Meter Event Status Code
#define ERR_CLOSE                       0
#define ERR_OPEN                        1

#define ENDISTATE_NORMAL				0				// 정상
#define ENDISTATE_INIT					1				// 초기화 상태
#define ENDISTATE_CONNECTION_ERROR		10				// End Device에 접속이 안되는 경우
#define ENDISTATE_METER_ERROR			11				// 미터로 부터 수신이 없는 경우
#define ENDISTATE_DATA_RECEIVE          20              // Pulse Meter로 부터 Data 전송이 시작될 때
#define ENDISTATE_ERROR					100				// 에러

#define ENDDEVICE_ATTR_UNKNOWN          0x00000000
#define ENDDEVICE_ATTR_POWERNODE        0x00000001
#define ENDDEVICE_ATTR_BATTERYNODE      0x00000002
#define ENDDEVICE_ATTR_SOLARNODE        0x00000004
#define ENDDEVICE_ATTR_RFD              0x00000008  	// Reduced Function Device
#define ENDDEVICE_ATTR_MBUS_MASTER      0x00000010
#define ENDDEVICE_ATTR_MBUS_SLAVE       0x00000020
#define ENDDEVICE_ATTR_MBUS_ARM         0x00000040
#define ENDDEVICE_ATTR_REPEATER         0x00000080
#define ENDDEVICE_ATTR_ALL              0x000000ff

/**< 기존과 호환성을 유지하며 새로운 Attribute 정의 */
#define ENDDEVICE_ATTR_POLL             0x00000100    	// Polling
#define ENDDEVICE_ATTR_ASYNC            0x00000200    	// Async Device

#define MCU_ALERT_INVALID_MAGIC_NUMBER      0x01
#define MCU_ALERT_INVALID_ENDDEVICE_SIZE    0x02
#define MCU_ALERT_INVALID_EVTCONFIGURE_SIZE 0x03

/*-- Issue #2020 : TimeSync 조합이 많아지기 때문에 이제 STRATEGY를 정의해서 쓰도록 한다. --*/
#define TIMESYNC_STRATEGY_HIBRID_MESH           0x00
#define TIMESYNC_STRATEGY_HIBRID_SLEEPY_MESH    0x01
#define TIMESYNC_STRATEGY_SLEEPY_MESH           0x02

#define TIMESYNC_STRATEGY_LAST_VALUE            0x02

/*-- Issue #2033 : TimeSync Type 추가 --*/
#define TIMESYNC_TYPE_LONG                  0
#define TIMESYNC_TYPE_SHORT                 1

#define NETWORK_TYPE_STAR                   0x00
#define NETWORK_TYPE_MESH                   0x01

/** Issue #10: Scheduler Type */
#define SCHEDULER_TYPE_MASK                 0x00
#define SCHEDULER_TYPE_INTERVAL             0x01

/*-- Issue #1502 : Transaction Support --*/
#define TR_STATE_WAIT                       0x01
#define TR_STATE_QUEUE                      0x02
#define TR_STATE_RUN                        0x04
#define TR_STATE_TERMINATE                  0x08
#define TR_STATE_DELETE                     0x10

#define TR_SHOW_WAITING_TRANSACTION         TR_STATE_WAIT
#define TR_SHOW_QUEUEING_TRANSACTION        TR_STATE_QUEUE
#define TR_SHOW_RUNNING_TRANSACTION         TR_STATE_RUN
#define TR_SHOW_TERMINATE_TRANSACTION       TR_STATE_TERMINATE
#define TR_SHOW_ALL_TRANSACTION             0xFF

#define ASYNC_OPT_RETURN_CODE_EVT           0x01
#define ASYNC_OPT_RESULT_DATA_EVT           0x02
#define ASYNC_OPT_RETURN_CODE_SAVE          0x10
#define ASYNC_OPT_RESULT_DATA_SAVE          0x20

#define TR_GET_OPT_TRINFO                   0x01
#define TR_GET_OPT_REQUEST                  0x02
#define TR_GET_OPT_RESPONSE                 0x04
#define TR_GET_OPT_HISTORY                  0x08
#define TR_GET_OPT_ALL                      0xFF

/*-- Issue #1923 : Upgrade Type --*/
#define CONCENTRATOR_UPGRADE_GZIP           0x01
#define CONCENTRATOR_UPGRADE_DIFF           0x02
#define CONCENTRATOR_UPGRADE_TAR            0x03

/*-- State Loop Tag --*/
#define STATE_OPEN                  0
#define STATE_WAIT_CONNECT          1
#define STATE_SENSOR_INVENTORY      10
#define STATE_SENSOR_REPLY          11
#define STATE_INITIALIZE			12

#define STATE_METERING              20
#define STATE_SEND_COMMAND			21

#define STATE_READ_NODEINFO         31
#define STATE_READ_AMRINFO          32
#define STATE_READ_AMRDATA          33
#define STATE_READ_BATTPOINT        34
#define STATE_READ_BATTLOG          35
#define STATE_READ_LPSET			36
#define STATE_READ_LPDATA			37
#define STATE_READ_SOLARPOINT       38
#define STATE_READ_SOLARLOG         39
#define STATE_READ_CONSTANT         40
#define STATE_READ_SUBINFO          41
#define STATE_READ_METERCONSTANT    42
#define STATE_READ_METERSERIAL      43
#define STATE_READ_ENERGY_LEVEL     44
#define STATE_READ_VOLTAGESCALE     45
#define STATE_READ_METERTYPE        46  /** Issue #345 : 동일 Parser에서 다양한 분기를 해야 할 경우를 위한 단계 
                                          * STATE_READ_NODEINFO 이후에 실행되며 다음으로는 STATE_READ_METERSERIAL을 수행한다
                                          */
#define STATE_SYNC_METERTIME        47 /** Issue #619 : Poll Type Meter 중 시간 동기화 수행 */

#define STATE_SELECT_FIRMWARE       61
#define STATE_SEND_FIRMWARE         62
#define STATE_VERIFY                63
#define STATE_INSTALL               64
#define STATE_SCAN                  65

#define STATE_CLEAR_MTR_FAIL_CNT    81

#define STATE_WAIT_REPLY            100
#define STATE_RETRY                 200
#define STATE_OK                    300
#define STATE_DONE                  400
#define STATE_ERROR                 500

/** Aidon STATE */
#define STATE_AIDON_IDENT           1011
#define STATE_AIDON_DATA            1012
#define STATE_AIDON_DONE            1013

/** MBUS STATE */
#define STATE_MBUS_COMSET           1101
#define STATE_MBUS_GET_UD2          1102
#define STATE_MBUS_LPPOINT          1103
#define STATE_MBUS_LPDATA           1104
#define STATE_MBUS_DONE_UD2         1105
#define STATE_MBUS_WRITE_ADDR       1106

/** DLMS STATE */
#define STATE_DLMS_GET              2001
#define STATE_DLMS_SNRM             2002
#define STATE_DLMS_AARQ             2003

#define STATE_QUERY                 2004
#define STATE_QUERY_PARSE           2005
#define STATE_DISCONNECT            2006

#define STATE_SEGMENTATION          2007

/** KAMSTRUP STATE */
//#define STATE_KMP_QUERY             3001

/** ANSI STATE */
#define STATE_PRE_READ              4000
#define STATE_IDENTIFICATION        4001
#define STATE_NEGOTIATE             4002
#define STATE_LOGON                 4003
#define STATE_SECURITY              4004
#define STATE_LOGOFF                4005
#define STATE_READ_TABLE            4006
#define STATE_READ_NEXT             4008
#define STATE_TERMINATE             4010
#define STATE_LAST_ACK              4011

#define STATE_PROC_EXECUTE          4100
#define STATE_PROC_SNAPSHOT_DATA    4284


/** OTA Type definition */
#define OTA_TYPE_CONCENTRATOR		0
#define OTA_TYPE_SENSOR				1
#define OTA_TYPE_COORDINATOR		2

/** Core definition */
#define MAIN_CORE                   FALSE
#define SUB_CORE                    TRUE

/** NZC Power Type */
#define NZC_POWER_TYPE_LINE         0x0001
#define NZC_POWER_TYPE_BATTERY      0x0010
#define NZC_POWER_TYPE_SOLAR        0x0020

/** NZC Firmware Type */
#define FW_TYPE_CONCENTRATOR        0
#define FW_TYPE_COORDINATOR         1
#define FW_TYPE_POWERCONTROL        2

/** Parser Type */
#define PARSER_TYPE_UNKNOWN     	0
#define PARSER_TYPE_PULSE       	1
#define PARSER_TYPE_AIDON       	2
#define PARSER_TYPE_ANSI        	3
#define PARSER_TYPE_KAMST       	4
#define PARSER_TYPE_REPEATER    	5
#define PARSER_TYPE_MBUS        	6
#define PARSER_TYPE_ACD         	7
#define PARSER_TYPE_SMOKE       	8
#define PARSER_TYPE_DLMS        	9
#define PARSER_TYPE_IHD         	10
#define PARSER_TYPE_HMU         	11
#define PARSER_TYPE_FIREALARM		12
#define PARSER_TYPE_I210			13
#define PARSER_TYPE_SX              14
#define PARSER_TYPE_OSAKI           15
#define PARSER_TYPE_3PARTY          16
#define PARSER_TYPE_KAMST_NEW       17
#define PARSER_TYPE_KEUKDONG        18
#define PARSER_TYPE_NOT_SET     	0xFFFFFFFF


/** Energy Level */
#define ENERGY_LEVEL_UNDEFINED      0x00
#define ENERGY_LEVEL_MIN            0x01
#define ENERGY_LEVEL_MAX            0x0F

/** Unknown String */
#define UNKNOWN_STR     "UNKNOWN"


/** UNDEFINED PORT NUMBER */
#define UNDEFINED_PORT_NUMBER       0

/** MObile Module Configure (Issue #1264) */
#define MOBILE_MODULE_TYPE_UNKNOWN  ""
#define MOBILE_MODULE_TYPE_DTSS800  "dtss800"       // ANYDATA CDMA
#define MOBILE_MODULE_TYPE_AME5210  "ame5210"       // LTE
#define MOBILE_MODULE_TYPE_MC55     "mc55"          // GSM 2G
#define MOBILE_MODULE_TYPE_GE910    "ge910"         // GSM 2G
#define MOBILE_MODULE_TYPE_UE910    "ue910"         // GSM 3G

#define MOBILE_MODULE_FLAG_SMS_MONITOR_MASK 0x80000000
#define MOBILE_MODULE_FLAG_SMS_TYPE_MASK    0x40000000
#define MOBILE_MODULE_FLAG_SMS_TYPE_ETHER   0x40000000
#define MOBILE_MODULE_FLAG_SMS_TYPE_SERIAL  0x00000000
#define MOBILE_MODULE_FLAG_SMS_CTRL_MASK    0x20000000
#define MOBILE_MODULE_FLAG_SMS_IP_MASK      0x10000000
#define MOBILE_MODULE_FLAG_SMS_IP_UDP       0x10000000
#define MOBILE_MODULE_FLAG_SMS_IP_TCP       0x00000000

#define MOBILE_MODULE_FLAG_PPP_ACTIVATE_MASK 0x00008000
#define MOBILE_MODULE_FLAG_PPP_TYPE_MASK    0x00004000
#define MOBILE_MODULE_FLAG_PPP_TYPE_ETHER   0x00004000
#define MOBILE_MODULE_FLAG_PPP_TYPE_SERIAL  0x00000000
#define MOBILE_MODULE_FLAG_PPP_CTRL_MASK    0x00002000
#define MOBILE_MODULE_FLAG_PPP_IP_MASK      0x00001000
#define MOBILE_MODULE_FLAG_PPP_IP_UDP       0x00001000
#define MOBILE_MODULE_FLAG_PPP_IP_TCP       0x00000000

#endif	// __MCU_DEFINES_H__

