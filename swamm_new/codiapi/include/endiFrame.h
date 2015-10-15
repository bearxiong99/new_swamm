#ifndef __ENDI_FRAME_H__
#define __ENDI_FRAME_H__

#include "endiDefine.h"

#ifdef _WIN32
#pragma pack(push, 1)
#endif	/* _WIN32 */

////////////////////////////////////////////////////////////////////
// End Device data frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_DATA_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		BYTE					payload[CODI_MAX_PAYLOAD_SIZE];	// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_DATA_FRAME;

////////////////////////////////////////////////////////////////////
// End Device Link frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_LINK_PAYLOAD							// Error frame payload
{
		BYTE					type;							// Link close type 
}	__ATTRIBUTE_PACKED__ ENDI_LINK_PAYLOAD;

typedef struct	_tagENDI_LINK_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_LINK_PAYLOAD		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_LINK_FRAME;

////////////////////////////////////////////////////////////////////
// End Device error frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_ERROR_PAYLOAD							// Error frame payload
{
		BYTE					code;							// Error code
}	__ATTRIBUTE_PACKED__ ENDI_ERROR_PAYLOAD;

typedef struct	_tagENDI_ERROR_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_ERROR_PAYLOAD		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_ERROR_FRAME;

////////////////////////////////////////////////////////////////////
// End Device bypass frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_BYPASS_PAYLOAD							// Bypass frame payload
{
		BYTE					data[CODI_MAX_PAYLOAD_SIZE];	// Bypass data
}	__ATTRIBUTE_PACKED__ ENDI_BYPASS_PAYLOAD;

typedef struct	_tagENDI_BYPASS_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_BYPASS_PAYLOAD		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_BYPASS_FRAME;

////////////////////////////////////////////////////////////////////
// End Device leave frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_LEAVE_PAYLOAD							// Leave frame payload
{
		BYTE					type;							// Leave type
		BYTE					join;							// Join time
#if     defined(__SUPPORT_ADVANCED_LEAVE__)
		BYTE					channel;						// Primary Cannel
		WORD					panid;						    // Primary Panid
#endif
}	__ATTRIBUTE_PACKED__ ENDI_LEAVE_PAYLOAD;

typedef struct	_tagENDI_LEAVE_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_LEAVE_PAYLOAD		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_LEAVE_FRAME;

////////////////////////////////////////////////////////////////////
// End Device AMR frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_AMR_DATA								// AMR Data
{
		GMTTIMESTAMP			gmttime;						// Time
		UINT					pulse;							// Current pulse
		BYTE					period;							// Meter period
		WORD					oprday;							// Operating day
		WORD					actmin;							// Active min
		BYTE					reset;							// Reset
		BYTE					meterread;						// Meter read second
		BYTE					daymask[4];						// Metering day
		BYTE					hourmask[12];					// Metering hour
		BYTE					rep_daymask[4];					// Repeating hour mask
		BYTE					rep_hourmask[12];				//
		WORD					rep_setuptime;					// Repeater setup time
		UINT					last_pulse;						// 
        BYTE                    choice;                         // LP Choice
        BYTE                    alarm;                          // Alarm Flag
}	__ATTRIBUTE_PACKED__ ENDI_AMR_DATA;

typedef struct	_tagENDI_AMRREAD_PAYLOAD						// AMR frame header
{
		BYTE					type;							// AMR type
}	__ATTRIBUTE_PACKED__ ENDI_AMRREAD_PAYLOAD;

typedef struct	_tagENDI_AMR_PAYLOAD							// AMR frame header
{
		BYTE					type;							// AMR type
		WORD					mask;							// AMR mask
		ENDI_AMR_DATA			data;							// AMR data
}	__ATTRIBUTE_PACKED__ ENDI_AMR_PAYLOAD;

typedef struct	_tagENDI_AMR_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_AMR_PAYLOAD		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_AMR_FRAME;

////////////////////////////////////////////////////////////////////
// End Device ROM Read/Write frame format 
//////////////////////////////////////////////////////////////////////

typedef struct _tagENDI_ROMREAD_PAYLOAD                         // ROM read payload
{
        BYTE                    typelen;                        // type & length
        WORD                    addr;                           // ROM Address
}   __ATTRIBUTE_PACKED__ ENDI_ROMREAD_PAYLOAD;

typedef struct	_tagENDI_ROM_PAYLOAD							// ROM read payload
{
		BYTE					typelen;						// type & length
		WORD					addr;							// ROM Address
		BYTE					data[CODI_MAX_PAYLOAD_SIZE];	// Frame payload
}	__ATTRIBUTE_PACKED__ ENDI_ROM_PAYLOAD;

typedef struct	_tagENDI_ROM_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_ROM_PAYLOAD		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_ROM_FRAME;

////////////////////////////////////////////////////////////////////
// End Device ROM Read/Write frame format (Long Address)
//////////////////////////////////////////////////////////////////////

typedef struct _tagENDI_LONG_ROMREAD_PAYLOAD                      // ROM read payload
{
		BYTE					type;						    // type
		BYTE					len;						    // length
		UINT					addr;							// ROM Address
}   __ATTRIBUTE_PACKED__ ENDI_LONG_ROMREAD_PAYLOAD;

typedef struct	_tagENDI_LONG_ROM_PAYLOAD							// ROM read payload
{
		BYTE					type;						    // type
		BYTE					len;						    // length
		UINT					addr;							// ROM Address
		BYTE					data[CODI_MAX_PAYLOAD_SIZE];	// Frame payload
}	__ATTRIBUTE_PACKED__ ENDI_LONG_ROM_PAYLOAD;

typedef struct	_tagENDI_LONG_ROM_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_LONG_ROM_PAYLOAD	payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_LONG_ROM_FRAME;

////////////////////////////////////////////////////////////////////
// End Device Status Frame
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_STATE_PAYLOAD							// State event payload
{
		BYTE					LQI;						    // LQI
		signed char				RSSI;						    // RSSI
        BYTE                    permit_mode;                    // 0 : Disable AutoPermitControl, !0 : Enable AutoPermitControl
        BYTE                    permit_state;                   // Permit Time (0 / 255)
        BYTE                    max_child_cnt;                  // Max Child Node Count
        BYTE                    child_cnt;                      // Current Child Node Count
        WORD                    solar_volt;                     // Main Volt.
        WORD                    solar_chg_bat_volt;             // Charge Battery Volt.
        WORD                    solar_board_volt;               // Board Volt.
        BYTE                    battery_chg_state;              // 0 : Charging, !0 : Complete
}	__ATTRIBUTE_PACKED__ ENDI_STATE_PAYLOAD;

typedef struct	_tagENDI_STATE_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_STATE_PAYLOAD		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_STATE_FRAME;

////////////////////////////////////////////////////////////////////
// End Device Super Router Frame
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_SUPER_PARENT_PAYLOAD					// Super parent payload
{
		BYTE					type;							// SP Type
		WORD					child_shortid;					// Child short id
		EUI64					child_id;						// Child EUI64 ID		
}	__ATTRIBUTE_PACKED__ ENDI_SUPER_PARENT_PAYLOAD;

typedef struct	_tagENDI_SUPER_PARENT_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_SUPER_PARENT_PAYLOAD payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_SUPER_PARENT_FRAME;

////////////////////////////////////////////////////////////////////
// Coordinator Frame
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_DISCOVERY_PAYLOAD						// Discovery request payload
{
		BYTE					cmd_type;						// command type
}	__ATTRIBUTE_PACKED__ ENDI_DISCOVERY_PAYLOAD;

typedef struct	_tagENDI_DISCOVERY_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_DISCOVERY_PAYLOAD	payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_DISCOVERY_FRAME;

////////////////////////////////////////////////////////////////////
// End Device Command Frame Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_COMMAND_PAYLOAD						// End Device Command Payload
{
		BYTE					cmd_type;						// command type
		BYTE					frame_type;						// frame type
		BYTE					data[CODI_MAX_PAYLOAD_SIZE];	// Frame payload
}	__ATTRIBUTE_PACKED__ ENDI_COMMAND_PAYLOAD;

typedef struct	_tagENDI_COMMAND_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_COMMAND_PAYLOAD	payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_COMMAND_FRAME;

////////////////////////////////////////////////////////////////////
// End Device MCCB frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_MCCB_PAYLOAD							// MCCB read payload
{
		BYTE					typevalue;						// type & value
}	__ATTRIBUTE_PACKED__ ENDI_MCCB_PAYLOAD;

typedef struct	_tagENDI_MCCB_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_MCCB_PAYLOAD		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_MCCB_FRAME;

////////////////////////////////////////////////////////////////////
// End Device Timesync frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_TIMESYNC_PAYLOAD						// Timesync payload
{
		GMTTIMESTAMP			gmt;							// GMT Time
		UINT					nazc;							// NAZC ID
        BYTE                    type;                           // Issue #2033 : Long(0), Short(1)
}	__ATTRIBUTE_PACKED__ ENDI_TIMESYNC_PAYLOAD;

typedef struct	_tagENDI_TIMESYNC_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_TIMESYNC_PAYLOAD	payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_TIMESYNC_FRAME;

////////////////////////////////////////////////////////////////////
// End Device Metering frame format 
//////////////////////////////////////////////////////////////////////

typedef struct  _tagENDI_METERING_INFO
{
		GMTTIMESTAMP			curTime;						// Current time
		WORD					oprDay;							// Operating day
		WORD					actMin;							// Active min
		WORD					batteryVolt;					// Battery volt
        WORD                    curConsum;                      // Current Consumption
        BYTE                    offset;                         // Vattery Offset
		UINT					curPulse;						// Current pulse
        BYTE                    lpChoice;                       // (0: Current LP, n: n days ago);
		BYTE					lpPeriod;						// LP period
		TIMEDATE				lpDate;							// LP Date (year/mon/day)
		UINT					basePulse;						// Base pulse
        BYTE                    fwVersion;                      //
        BYTE                    fwBuild;                        //
        BYTE                    hwVersion;                      //
        BYTE                    swVersion;                      //
		BYTE					LQI;						    // LQI
		signed char				RSSI;						    // RSSI
        BYTE                    sensorType;                     // Sensor type
        BYTE                    alarmFlag;                      // Alarm Flag (0:Off, 1~255: On)
        BYTE                    networkType;                    // Network Type (0:Start, 1: Mesh, 2~255: Reserved)
        BYTE                    energyLevel;                    // Device Energy Level (0:Undefined, 1~15)
/**
  * 초기 Unzbek 가스 작업을 위해 추가되었던 attribute.
  * 이후 삭제되었지만 Code에 남아 있었다.
  *
        BYTE                    GasData;                        // Gas Valve Status Value
*/
}   __ATTRIBUTE_PACKED__ ENDI_METERING_INFO;

typedef struct	_tagENDI_METERING_PAYLOAD
{
		BYTE					type;                           // Metering Data Type(0:Payload, n:LP Log)
		union
		{
            ENDI_METERING_INFO  info;                           // Information 
		    WORD				lpData[24];						// LP data
        } 	val;
}	__ATTRIBUTE_PACKED__ ENDI_METERING_PAYLOAD;

typedef struct	_tagENDI_METERING_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
        ENDI_METERING_PAYLOAD   payload;                        // Metering Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_METERING_FRAME;


////////////////////////////////////////////////////////////////////
// End Device M-bus Metering frame format 
//////////////////////////////////////////////////////////////////////
typedef struct  _tagMBUS_METERING_PAYLOAD
{
        BYTE        meterType;
        TIMEDATE    date;
        BYTE        data[0];
}   __ATTRIBUTE_PACKED__ MBUS_METERING_PAYLOAD;

typedef struct  _tagMBUS_DATA_FRAME
{
        BYTE    mbusSeq;
        BYTE    payload[ZIGBEE_MAX_PAYLOAD_SIZE];     //LEN
}   __ATTRIBUTE_PACKED__ MBUS_DATA_FRAME;


////////////////////////////////////////////////////////////////////
// End Device Push Metering frame format  (Issue #84)
//////////////////////////////////////////////////////////////////////

typedef struct  _tagENDI_PUSH_HEADER
{
        BYTE                    frameType;                      // Frame Type, Ordering
        WORD                    length;                         // Big Endian
        BYTE                    portNum;                        // Device Port Number
        BYTE                    meterStatus;                    // Meter Status (bit 7:MeterType, 6: RelayStatus)
        BYTE                    dataFormatLength;               // bit 7~4:Current Pulse length, 3~0:LP Length
        BYTE                    energyLevel;                    // Energy Level
        TIMESTAMP6              currentTime;                    // Current Time
        BYTE                    channelCount;                   // Channel Count
        BYTE                    currentPulse[0];                // Current Pulse
}   __ATTRIBUTE_PACKED__ ENDI_PUSH_HEADER;

typedef struct  _tagPUSH_EVENT
{
        BYTE                    eventType;                      // Event Type (Meter specific)
        BYTE                    eventLength;                    // Event Data Length
        BYTE                    eventCount;                     // Event Count
        BYTE                    data[0];                        // Event Data
}   __ATTRIBUTE_PACKED__ PUSH_EVENT;

typedef struct  _tagENDI_PUSH_EVT_HEADER
{
        BYTE                    frameType;                      // Frame Type, Ordering
        WORD                    length;                         // Big Endian
        BYTE                    portNum;                        // Device Port Number
        TIMESTAMP6              currentTime;                    // Meter Current Time
        BYTE                    categoryCount;                  // Event Category Count
        PUSH_EVENT              eventData[0];                   // Event Data
}   __ATTRIBUTE_PACKED__ ENDI_PUSH_EVT_HEADER;

typedef struct  _tagENDI_PUSH_LPINFO
{
        BYTE                    lpInterval;                     // Interval (minute)
        BYTE                    lpCount;                        // LP Count
        BYTE                    lpData[0];                      // LP Data
}   __ATTRIBUTE_PACKED__ ENDI_PUSH_LPINFO;

typedef struct  _tagENDI_PUSH_BASEPULSE
{
        TIMESTAMP5              baseTime;                       // Base Pulse Snapshot Time
        BYTE                    basePulse[0];                   // Base Pulse
}   __ATTRIBUTE_PACKED__ ENDI_PUSH_BASEPULSE;

typedef struct  _tagENDI_PUSH_LP
{
        TIMESTAMP5              lastTime;                       // Last Block Time
        BYTE                    lp[0];                          // Load Profile
}   __ATTRIBUTE_PACKED__ ENDI_PUSH_LP;

typedef struct	_tagENDI_PUSH_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		BYTE					data[CODI_MAX_PAYLOAD_SIZE];	// Frame payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_PUSH_FRAME;


////////////////////////////////////////////////////////////////////
// End Device Event frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_EVENT_STATUS
{
		union
		{
			BYTE				u8;
			WORD				u16;
			UINT				u32;
			BYTE				str[4];
		}	val;
}	__ATTRIBUTE_PACKED__ ENDI_EVENT_STATUS;

typedef struct	_tagENDI_EVENT_PAYLOAD							// Event frame payload
{
		GMTTIMESTAMP			gmt;							// Event time
		BYTE					type;							// Event type
		ENDI_EVENT_STATUS		status;							// Event status
}	__ATTRIBUTE_PACKED__ ENDI_EVENT_PAYLOAD;

typedef struct	_tagENDI_EVENT_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_EVENT_PAYLOAD 		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_EVENT_FRAME;

////////////////////////////////////////////////////////////////////
// End Device Node information Data Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_NODE_INFO
{
		char		NODE_KIND[20];							// 해당 모델 이름(ASCII)
		BYTE		RESERVED0[10];							//
		BYTE		FIRMWARE_VERSION;						//
		BYTE		FIRMWARE_BUILD;							//
		BYTE		SOFTWARE_VERSION;						// Version 1.0 => 0x10
		BYTE		HARDWARE_VERSION;						//
		BYTE		PROTOCOL_VERSION;						//
		BYTE		INTERFACE_VERSION;						//
		WORD		RESET_COUNT;							// 리셋 카운터
		BYTE		RESET_REASON;							// 마지막 리셋 코드
        BYTE        SUPER_PARENT;                           // Super Parent 여부
        WORD        SOLAR_AD_VOLT;                          // Solar Cell의 메인 전원 전압 (수집값의 2배를 해줘야 한다)
        WORD        SOLAR_CHG_BATT_VOLT;                    // Solar Cell의 충전용 배터리 전압
        WORD        SOLAR_B_DC_VOLT;                        // Solar 리피터로 들어오는 보드 VCC
        
}	__ATTRIBUTE_PACKED__ ENDI_NODE_INFO;

////////////////////////////////////////////////////////////////////
// End Device Network information Data Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_NETWORK_INFO
{
		BYTE		MANUAL_ENABLE;							// 0=자동 스캔으로 조인
		BYTE		CHANNEL;								// 지그비 네트워크 채널 (manual 1 일때)
		WORD		PANID;									// 지그비 네트워크 PAN ID (manual 1 일때)
		signed char	TXPOWER;								// RF 송신 출력 (manual 1 일때)
		BYTE		SECURITY_ENABLE;						// 0=AES-128 Key disable
		BYTE		LINK_KEY[16];							// Link key
		BYTE		NETWORK_KEY[16];						// Network key
		BYTE		EXT_PANID[8];							// Extended Pan ID
}	__ATTRIBUTE_PACKED__ ENDI_NETWORK_INFO;

////////////////////////////////////////////////////////////////////
// End Device AMR Data Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_AMR_INFO
{
		char		METER_SERIAL_NUMBER[20];				// 메타 시리얼 번호
		char		VENDOR[30];								// 메타 제조사 벤더
		char		CUSTOMER_NAME[20];						// 고객 이름
		char		CONSUMPTION_LOCATION[30];				// 설치 위치
		BYTE		FIXED_RESET;							// 0~23 : 하루에 주기적으로 리셋할 시각
		BYTE		TEST_FLAG;								// QC Test FLAG (1=ENABLE), DEFAULT OFF
		UINT		METERING_DAY;							// 검침 일자 MASK
		BYTE		METERING_HOUR[12];						// 검침 시간 MASK
		UINT		REPEATING_DAY;							//
		BYTE		REPEATING_HOUR[12];						//
		WORD		REPEATING_SETUP_SEC;					// 					
		UINT		NAZC_NUMBER;							// NAZC 번호
		BYTE		LP_CHOICE;								// 배터리 노드의 LP 데이터의 일자를 선택하는 값
        BYTE        ALARM_FLAG;                             // Alarm Flag
}	__ATTRIBUTE_PACKED__ ENDI_AMR_INFO;

////////////////////////////////////////////////////////////////////
// End Device Battry Log Data Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_BATTERYLOG_INFO
{
		BYTE		BATTERY_POINTER;						// 베터리 로그 포인터
}	__ATTRIBUTE_PACKED__ ENDI_BATTERYLOG_INFO;

typedef struct	_tagENDI_BATTERY_ENTRY
{
		WORD		BATTERY_VOLT;							// 배터리 전압
		WORD		CONSUMPTION_CURRENT;					// 현재까지 사용한 전류
		BYTE		OFFSET;									// 바로 직전 전압 데이터 차
}	__ATTRIBUTE_PACKED__ ENDI_BATTERY_ENTRY;

////////////////////////////////////////////////////////////////////
// End Device Event Log Data Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_EVENTLOG_INFO
{
		BYTE		EVENT_POINTER;							// 이벤트 포인터
}	__ATTRIBUTE_PACKED__ ENDI_EVENTLOG_INFO;

typedef struct	_tagENDI_EVENT_ENTRY
{
		GMTTIMESTAMP GMT_TIME;								// 이벤트 발생 시간
		BYTE		EVENT_TYPE;								// 이벤트 유형
		BYTE		EVENT_STATUS[4];						// 이벤트 상태
}	__ATTRIBUTE_PACKED__ ENDI_EVENT_ENTRY;

////////////////////////////////////////////////////////////////////
// End Device Load Profile Log Data Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_LPLOG_INFO
{
		BYTE		LPPERIOD;								// LP 주기
		BYTE		LPPOINTER;								// LP 포인터
}	__ATTRIBUTE_PACKED__ ENDI_LPLOG_INFO;

typedef struct	_tagENDI_LP_ENTRY
{
		TIMEDATE	LPDATE;									// LP 기록 날짜(년, 월, 일)
		UINT		BASE_PULSE;								// 0시때의 지침값
		BYTE		LP_DATA[192];							// LP Data는 2byte
}	__ATTRIBUTE_PACKED__ ENDI_LP_ENTRY;

////////////////////////////////////////////////////////////////////
// End Device Event Solar Volt Log Data Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_SOLARLOG_INFO
{
		BYTE		SOLAR_POINTER;							// Solar LOG 포인터
}	__ATTRIBUTE_PACKED__ ENDI_SOLARLOG_INFO;

typedef struct	_tagENDI_SOLARVOLT_ENTRY
{
        WORD        SOLAR_AD_VOLT;                          // Solar Cell의 메인 전원 전압 (수집값의 2배를 해줘야 한다)
        WORD        SOLAR_CHG_BATT_VOLT;                    // Solar Cell의 충전용 배터리 전압
        WORD        SOLAR_B_DC_VOLT;                        // Solar 리피터로 들어오는 보드 VCC
}	__ATTRIBUTE_PACKED__ ENDI_SOLARVOLT_ENTRY;

typedef struct	_tagENDI_SOLARLOG_ENTRY
{
		TIMEDATE	         SOLAR_DATE;					// LOG 기록 날짜(년, 월, 일)
        ENDI_SOLARVOLT_ENTRY SOLAR_VOLT[24 * 4];            // 전원 LOG 정보
}	__ATTRIBUTE_PACKED__ ENDI_SOLARLOG_ENTRY;

////////////////////////////////////////////////////////////////////
// End Device D I/O Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_DIO_PAYLOAD						// D I/O frame payload
{
        BYTE        type;                                   // Type
        BYTE        mask;                                   // Port Mask    (7~4 input mask, 3~0 output mask)
        BYTE        reserved;                               // reserved field
        BYTE        value;                                  // Value
}	__ATTRIBUTE_PACKED__ ENDI_DIO_PAYLOAD;

typedef struct	_tagENDI_DIO_FRAME
{
		CODI_HEADER				hdr;							// Header
		EUI64					id;								// Source address
		ENDI_DIO_PAYLOAD		payload;						// Payload
		CODI_TAIL				tail;							// Tail
}	__ATTRIBUTE_PACKED__ ENDI_DIO_FRAME;

////////////////////////////////////////////////////////////////////
// End Device NAN Protocol Format
//////////////////////////////////////////////////////////////////////

typedef struct	_tagENDI_NAN_PAYLOAD                            // NAN frame payload
{
        BYTE        trid;                                       // Transaction ID (0~255)
        BYTE        ctrl;                                       // 7bit(flow:req=0,res=1), 6bit(tail=1), 5~0bit sequence
		BYTE        data[ZIGBEE_MAX_PAYLOAD_SIZE];	            // NAN data
}	__ATTRIBUTE_PACKED__ ENDI_NAN_PAYLOAD;

typedef struct	_tagENDI_NAN_FRAME
{
		CODI_HEADER				hdr;                            // Header
		EUI64					id;                             // Source address
		ENDI_NAN_PAYLOAD        payload;                        // Payload
		CODI_TAIL				tail;                           // Tail
}	__ATTRIBUTE_PACKED__ ENDI_NAN_FRAME;


////////////////////////////////////////////////////////////////////
// KETI Protocol Format
//////////////////////////////////////////////////////////////////////

typedef struct  _tagENDI_3PARTY_HEADER
{
        BYTE        stx;                                    // 0x02
        BYTE        seq;                                    // Sequence
        BYTE        cmd;                                    // Command
        BYTE        param;                                  // Command Parameter
        BYTE        len;
}   __ATTRIBUTE_PACKED__ ENDI_3PARTY_HEADER;

typedef struct	_tagENDI_3PARTY_TAIL                        // 3rd Party Protocol tail
{
        WORD        fcs;                                    // CRC16
        BYTE        etx;                                    // 0x03
}	__ATTRIBUTE_PACKED__ ENDI_3PARTY_TAIL;

typedef struct	_tagENDI_3PARTY_FRAME                       // 3rd Party Protocol frame
{
        ENDI_3PARTY_HEADER  hdr;
        BYTE                payload[ZIGBEE_MAX_PAYLOAD_SIZE];
        ENDI_3PARTY_TAIL    tail;
}	__ATTRIBUTE_PACKED__ ENDI_3PARTY_FRAME;


typedef struct  _tagAPP_3PARTY_HEADER
{
        BYTE        stx;                                    // 0x02
        BYTE        vendor;                                 // Sequence
        BYTE        model;                                  // Command
        WORD        len;                                    // length
}   __ATTRIBUTE_PACKED__ APP_3PARTY_HEADER;

typedef struct __tagENDI_3PARTY_TAIL    APP_3PARTY_TAIL;

typedef struct	_tagAPP_3PARTY_PAYLOAD                      // 3rd Party Protocol frame
{
        APP_3PARTY_HEADER   hdr;
        BYTE                payload[0];
}	__ATTRIBUTE_PACKED__ APP_3PARTY_PAYLOAD;



#ifdef _WIN32
#pragma pack(pop)
#endif	/* _WIN32 */

#endif	/* __ENDI_FRAME_H__ */
