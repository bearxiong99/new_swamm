#ifndef __KD_OADI_H__
#define __KD_OADI_H__

// Optical Application Device Interface (OADI)

#define OADI_IDENT						0xAA

// Frame Action

#define OADI_ACTION_WRITE				0x00
#define OADI_ACTION_READ				0x80

#define OADI_ACTION_REQUEST				0x00
#define OADI_ACTION_RESPONSE			0x40
	
#define OADI_ACTION_READ_REQUEST		(OADI_ACTION_READ | OADI_ACTION_REQUEST)
#define OADI_ACTION_READ_RESPONSE		(OADI_ACTION_READ | OADI_ACTION_RESPONSE)
#define OADI_ACTION_WRITE_REQUEST		(OADI_ACTION_WRITE | OADI_ACTION_REQUEST)
#define OADI_ACTION_WRITE_RESPONSE		(OADI_ACTION_WRITE | OADI_ACTION_RESPONSE)

// Frame Type

#define OADI_TYPE_EM250					0x00
#define OADI_TYPE_MSP430				0x01

#define OADI_KD_STATUS				0x00
#define OADI_KD_AMR					0x01
#define OADI_KD_VALVE				0x02
#define OADI_KD_VERSION				0x03

#define KD_ALARM_CASE_OPEN			0x01
#define KD_ALARM_TAMPER				0x02
#define KD_ALARM_VIBRATOR			0x04
#define KD_ALARM_UNSTABLE			0x08
#define KD_ALARM_OPEN_FAIL			0x10
#define KD_ALARM_CLOSE_FAIL			0x20

#ifdef _WIN32
#pragma pack(push, 1)
#endif

typedef struct	_tagOADI_HEADER
{
		BYTE			ident;					// Identification (0xAA)
		WORD			fctrl;					// Frame Control
		BYTE			len;					// Data length
}	__ATTRIBUTE_PACKED__ OADI_HEADER;

typedef struct	_tagOADI_TAIL
{
		WORD			crc;					// FCS
}	__ATTRIBUTE_PACKED__ OADI_TAIL;

typedef struct	_tagOADI_FRAME
{
		OADI_HEADER		hdr;
		BYTE			payload[256];
		OADI_TAIL		tail;	
}	__ATTRIBUTE_PACKED__ OADI_FRAME;

typedef struct	_tagOADI_KD_STATUS_PAYLOAD
{
		UINT			curPulse;				// Current Pulse
		char			meterSerial[12];		// Serial Number
		BYTE			alarmStatus;			// Alarm Status
		BYTE			meterStatus;			// Meter Status
}	__ATTRIBUTE_PACKED__ OADI_KD_STATUS_PAYLOAD;

typedef struct	_tagOADI_KD_AMR_PAYLOAD
{
		BYTE			writeType;				// 0x00:Include serial, 0x01:not include serial
		UINT			curPulse;				// Current Pulse
		char			meterSerial[12];		// Serial Number
}	__ATTRIBUTE_PACKED__ OADI_KD_AMR_PAYLOAD;

typedef struct	_tagOADI_KD_VALVE_PAYLOAD
{
		BYTE			valve;					// 0:On, 1:Off, 2:Standby
}	__ATTRIBUTE_PACKED__ OADI_KD_VALVE_PAYLOAD;

typedef struct	_tagOADI_KD_VERSION_PAYLOAD
{
		char			testDate[6];			// YYMMDD(ASCII) 2010/02/25 -> 100225
		char			testResult;				// A:Accept
		char			hwVersion[7];			// H/W Version, ex)1.01 -> H/W1.01
		char			swVersion[7];			// S/W Version, ex)1.01 -> S/W1.01
}	__ATTRIBUTE_PACKED__ OADI_KD_VERSION_PAYLOAD;

////////////////////////////////////////////////////////////////////
// End Device UZBEK keukdong Metering frame format 
//////////////////////////////////////////////////////////////////////
typedef struct  _tagKEUKDONG_METERING_PAYLOAD
{
		char					serial[12];
		BYTE					alarm;
		BYTE					status;
}   __ATTRIBUTE_PACKED__ KEUKDONG_METERING_PAYLOAD;


#ifdef _WIN32
#pragma pack(pop)
#endif

#endif	// __KD_OADI_H__
