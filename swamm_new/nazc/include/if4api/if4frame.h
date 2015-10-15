#ifndef __IF4_FRAME_TYPE_H__
#define __IF4_FRAME_TYPE_H__

#include "varapi.h"

// Control Code Definitions

#define IF4_SOH						0x5E		// 프레임 시작을 알리는 해더
#define IF4_ENQ						0x01		// 최초 연결시 전달되는 준비 상태
#define IF4_ACK						0x02		// 정상적인 경우
#define IF4_NAK						0x03		// 잘못된 경우
#define IF4_CAN						0x04		// 전송 프레임 취소
#define IF4_EOT						0x05		// 연결 종료
#define IF4_AUT						0x06		// 접속 인증
#define IF4_WCK						0x07		// 윈도우 검사 요청
#define IF4_REDIRECT				0x08		// Redirection (Address/Port)
#define IF4_RETRY					0x09		// Retry (Delay=n seconds)

// Header Attribute Definitions

#define IF4_ATTR_CTRLFRAME			0x80		// Control/Data Frame Flag
#define IF4_ATTR_RESERVED2			0x40		// Reserved 2
#define IF4_ATTR_DO_NOT_COMPRESS	0x20		// Response do not compress
#define IF4_ATTR_MUSTCONFIRM		0x10		// Confirm/Non-Confirm Flag
#define IF4_ATTR_CRYPT				0x08		// Crypt Flag
#define IF4_ATTR_COMPRESS			0x04		// Compress Flag
#define IF4_ATTR_START				0x02		// Multi Frame Start Flag
#define IF4_ATTR_END				0x01		// Multi Frame End Flag

// Service Definitions

#define IF4_SVC_CONTROL				0x00		// Control
#define IF4_SVC_COMMAND				'C'			// Command Service
#define IF4_SVC_DATA				'M'			// Measurement Metering Data Service (SAT1 Format)
#define IF4_SVC_METERINGDATA		'S'			// Measurement Metering Data Service (SAT2 Format)
#define IF4_SVC_NEWMETERDATA        'R'			// Measurement Metering Data Service (V4.0 2012 Relatime Metering)
#define IF4_SVC_DATAFILE			'D'			// Data File
#define IF4_SVC_ALARM				'A'			// Alarm Service
#define IF4_SVC_EVENT				'E'			// Event Service
#define IF4_SVC_TRANSFER			'F'			// File Transfer Service
#define IF4_SVC_TELNET				'T'			// Telnet Service
#define IF4_SVC_PARTIAL				'P'			// Partial frame

#ifdef _WIN32
#pragma pack(push, 1)
#endif

// Value Type Definitions

typedef struct	_tagSMIValue
{
		OID3			oid;					// OID
		WORD			len;					// Data Length
		union
		{
			signed char     s8;               	// 1 Byte Value
            BYTE            u8;                	// 1 Byte Value
            signed short    s16;               	// 2 Byte Value
            WORD            u16;               	// 2 Byte Value
            int             s32;              	// 4 Byte Value
            UINT            u32;              	// 4 Byte Value
            char            *p;               	// Pointer
			OID3			id;					// 3 Byte OID
			BYTE			ip[4];				// IPv4
			EUI64			eui64;				// EUI64 ID
			TIMESTAMP		time;				// Timestamp
			TIMEDATE		day;				// Timedate
			char			str[1024];			// String
		}	stream;		
}	__ATTRIBUTE_PACKED__ SMIValue, *PSMIValue;

typedef struct	_tagIF4_DATA
{
		WORD			len;					// Data Length
		char			data[256];				// Data Buffer
}	__ATTRIBUTE_PACKED__ IF4_DATA, *PIF4_DATA;

// Generial Header & Tail Frame

typedef struct	_tagIF4_HEADER
{
		BYTE			soh;					// Satr Of Header 
		BYTE			seq;					// Sequence Number
		BYTE			attr;					// Communication Attribute
		UINT			len;					// Data Length
		BYTE			svc;					// Service Type
}	__ATTRIBUTE_PACKED__ IF4_HEADER, *PIF4_HEADER;

typedef struct	_tagIF4_TAIL
{
		WORD			crc;					// CRC 16
}	__ATTRIBUTE_PACKED__ IF4_TAIL, *PIF4_TAIL;

typedef struct	_tagIF4_PACKET
{
		IF4_HEADER		hdr;
		char			arg[100];
}	__ATTRIBUTE_PACKED__ IF4_PACKET;

// Control Frame

typedef struct	_tagIF4_CTRL_FRAME
{
		IF4_HEADER		hdr;					// Header
		BYTE			code;					// Control Code
		WORD			len;					// Argment Length
		char			arg[100];				// Argment
}	__ATTRIBUTE_PACKED__ IF4_CTRL_FRAME, *PIF4_CTRL_FRAME;

typedef struct	_tagIF4_CTRL_ENQ
{
		BYTE			ver[2];					// Version
		WORD			window_size;			// Window Size
		BYTE			window_count;			// Window Count
}	__ATTRIBUTE_PACKED__ IF4_CTRL_ENQ, *PIF4_CTRL_ENQ;

typedef struct	_tagIF4_CTRL_REDIRECT
{
		BYTE			addr[16];				// IP Address
		UINT			port;					// Port Number
}	__ATTRIBUTE_PACKED__ IF4_CTRL_REDIRECT, *PIF4_CTRL_REDIRECT;

typedef struct	_tagIF4_CTRL_RETRY
{
		UINT			delay;					// Delay time (second)
}	__ATTRIBUTE_PACKED__ IF4_CTRL_RETRY, *PIF4_CTRL_RETRY;

// Version 2 data

typedef struct	_tagIF4_COMPRESS_HEADER
{
		BYTE			compress;				// Compress type (0:None, 1:zlib, 2:gzip)
		UINT			source_size;			// Source size
}	__ATTRIBUTE_PACKED__ IF4_COMPRESS_HEADER, *PIF4_COMPRESS_HEADER;

typedef struct	_tagIF4_GENERAL_DATA
{
		IF4_COMPRESS_HEADER chdr;				// Compress Header
		char			data[1024];				// Data
}	__ATTRIBUTE_PACKED__ IF4_GENERAL_DATA, *PIF4_GENERAL_DATA;

typedef struct	_tagIF4_MULTI_FRAME
{
		IF4_HEADER		hdr;					// Header
		char			data[1024];				// Data
		IF4_TAIL		tail;					// Tail
}	__ATTRIBUTE_PACKED__ IF4_MULTI_FRAME, *PIF4_MULTI_FRAME;

typedef struct	_tagIF4_PARTIAL_FRAME
{
		IF4_HEADER		hdr;					// Header
		IF4_COMPRESS_HEADER chdr;				// Compress Header
		char			data[1024];				// Data
		IF4_TAIL		tail;					// Tail
}	__ATTRIBUTE_PACKED__ IF4_PARTIAL_FRAME, *PIF4_PARTIAL_FRAME;

// Download frame definitions

#define MAX_DOWNLOAD_DATA_SIZE					60

#define DOWNLOAD_IDENTIFICATION					0x7E

#define DOWNLOAD_ATTR_HEADER					0x80
#define DOWNLOAD_ATTR_CONTROL					0x40
#define DOWNLOAD_ATTR_START						0x02
#define DOWNLOAD_ATTR_END						0x01

typedef struct  _tagDOWNLOAD_HEADER
{
		BYTE			ident;					// Frame identification
        BYTE        	seq;                    // Sequence number
        BYTE        	attr;                   // Attribute
		WORD			len;					// length
}   __ATTRIBUTE_PACKED__ DOWNLOAD_HEADER;

typedef struct  _tagDOWNLOAD_TAIL
{       
		WORD			crc;					// CRC Checksum
}   __ATTRIBUTE_PACKED__ DOWNLOAD_TAIL;

typedef struct  _tagDOWNLOAD_PACKET
{       
        DOWNLOAD_HEADER hdr;                    // Frame header
        BYTE            data[4096];             // Payload 
		DOWNLOAD_TAIL	tail;					// Frame Tail
}   __ATTRIBUTE_PACKED__ DOWNLOAD_PACKET;

typedef struct  _tagDOWNLOAD_FILE_FORMAT
{       
        BYTE            type;                   // Download type 
        BYTE            flow;                   // Software flow control
		char			name[64];				// File name
        unsigned long   nLength;                // File length
        char            key[32];                // MD5 Hash key
}   __ATTRIBUTE_PACKED__ DOWNLOAD_FILE_FORMAT;

// Command Service Frame

typedef struct	_tagIF4_SVC_HEADER
{
		UINT			mcuid;					// MCU ID
}	__ATTRIBUTE_PACKED__ IF4_SVC_HEADER, *PIF4_SVC_HEADER;

typedef struct	_tagIF4_CMD_HEADER
{
		OID3			cmd;					// Command OID
		BYTE			tid;					// TID
		BYTE			attr;					// Attribute
		BYTE			errcode;				// Error Code
		WORD			cnt;					// Parameter Count
}	__ATTRIBUTE_PACKED__ IF4_CMD_HEADER, *PIF4_CMD_HEADER;

typedef struct	_tagIF4_CMD_FRAME
{
		IF4_HEADER		hdr;					// Communication Header
		IF4_SVC_HEADER	sh;					    // Service Header
		IF4_CMD_HEADER	ch;					    // Command Service Header
		SMIValue		args;					// Command Parameter
}	__ATTRIBUTE_PACKED__ IF4_CMD_FRAME, *PIF4_CMD_FRAME;

// Measurement Metering Data Service Frame

typedef struct	_tagIF4_DATA_HEADER
{
		WORD			cnt;					// Sensor Count
}	__ATTRIBUTE_PACKED__ IF4_DATA_HEADER, *PIF4_DATA_HEADER;

typedef struct	_tagIF4_DATA_FRAME
{
		IF4_HEADER				hdr;			// Communication Header
		IF4_SVC_HEADER			sh;			    // Service Header
		IF4_DATA_HEADER			dh;			    // Metering Data Service Header
		char					args[2];		// Meter Item	
}	__ATTRIBUTE_PACKED__ IF4_DATA_FRAME, *PIF4_DATA_FRAME;

// Data File Service Frame

typedef struct	_tagIF4_DATAFILE_HEADER
{
		BYTE			ftype;					// File Type (IF4_DATAFILE_xxxx)
		char			fname[32];				// File name
		UINT			fsize;					// File Size
}	__ATTRIBUTE_PACKED__ IF4_DATAFILE_HEADER, *PIF4_DATAFILE_HEADER;

typedef struct	_tagIF4_DATAFILE_FRAME
{
		IF4_HEADER				hdr;			// Communication Header
		IF4_SVC_HEADER			sh;			    // Service Header
		IF4_DATAFILE_HEADER		fh;			    // Data File Service Header
		char					data[2];		// Data	
}	__ATTRIBUTE_PACKED__ IF4_DATAFILE_FRAME, *PIF4_DATAFILE_FRAME;

// Type R Metering (Push Metering)

typedef struct _tagIF4_TYPER_HEADER
{
        BYTE                    type;           // Frame Type (0x01:Inventory, 0x02: Meter Cfg, 0x03: Metering Data)
        WORD                    length;         // payload length (BIG Endian)
        char                    payload[0];     // payload
}   __ATTRIBUTE_PACKED__ IF4_TYPER_HEADER, *PIF4_TYPER_HEADER;

typedef struct	_tagIF4_TYPER_FRAME
{
		IF4_HEADER				hdr;			// Communication Header
		IF4_SVC_HEADER			sh;			    // Service Header
        IF4_DATA_HEADER         dh;             // Data Header
		char					data[0];        // Data	
}	__ATTRIBUTE_PACKED__ IF4_TYPER_FRAME, *PIF4_TYPER_FRAME;


// Alarm Service Frame

typedef struct	_tagIF4_ALARM_HEADER
{
		BYTE			srcType;				// Alarm Source Type	
		BYTE			srcID[8];				// Sensor ID (AU)
		TIMESTAMP		tmMcu;					// MCU Time
		TIMESTAMP		tmAlarm;				// Alarm Time
		BYTE			idx;					// Index
}	__ATTRIBUTE_PACKED__ IF4_ALARM_HEADER, *PIF4_ALARM_HEADER;

typedef struct	_tagIF4_ALARM_FRAME
{
		IF4_HEADER			hdr;				// Communication Header
		IF4_SVC_HEADER		sh;				    // Service Header
		IF4_ALARM_HEADER	ah;				    // Alarm Service Header
		IF4_DATA			args;				// Alarm Data
}	__ATTRIBUTE_PACKED__ IF4_ALARM_FRAME, *PIF4_ALARM_FRAME;

// Event Service Frame

typedef struct	_tagIF4_EVENT_HEADER
{
		OID3			eventCode;				// Event Code
		BYTE			srcType;				// Event Source Type	
		BYTE			srcID[8];				// Event Source ID
		TIMESTAMP		tmEvent;				// Event Time
		WORD			cnt;					// Param Count		
}	__ATTRIBUTE_PACKED__ IF4_EVENT_HEADER, *PIF4_EVENT_HEADER;

typedef struct	_tagIF4_EVENT_FRAME
{
		IF4_HEADER			hdr;				// Communication Header
		IF4_SVC_HEADER		sh;				    // Service Header
		IF4_EVENT_HEADER	eh;				    // Event Service Header
		SMIValue			args;				// Parameters
}	__ATTRIBUTE_PACKED__ IF4_EVENT_FRAME, *PIF4_EVENT_FRAME;

// File Transfer Service Frame

typedef struct	_tagIF4_TRANSFER_HEADER
{
		char			fileName[32];			// File Name
		UINT			fileLength;				// File Length
		BYTE			fileMode;				// File Access Mode
		TIMESTAMP		tmFile;					// File Time
		BYTE			fileChannel;			// Opreration Channel
}	__ATTRIBUTE_PACKED__ IF4_TRANSFER_HEADER, *PIF4_TRANSFER_HEADER;

typedef struct	_tagIF4_TRANSFER_FRAME
{
		IF4_HEADER				hdr;			// Communication Header
		IF4_SVC_HEADER			sh;			    // Service Header
		IF4_TRANSFER_HEADER		th;			    // Transfer Service Header
		char					args[256];		// File Data
}	__ATTRIBUTE_PACKED__ IF4_TRANSFER_FRAME, *PIF4_TRANSFER_FRAME;


//
// Frame Payload Headers
//
typedef struct    _tagDATA_MTR_HEADER
{
        EUI64       id;
        char        mid[20];
        BYTE        stype;
        BYTE        svc;
        BYTE        vendor;
        WORD        dataCnt;
        WORD        length;
        TIMESTAMP    timestamp;                    // 년월일시분초
        char        data[0];                    // 실제 데이터
}    __ATTRIBUTE_PACKED__  DATA_MTR_HEADER;

typedef struct    _tagTYPER_ID
{
        BYTE        idType;
        BYTE        idLen;
        char        id[0];
}    __ATTRIBUTE_PACKED__  TYPER_ID;

typedef struct    _tagTYPER_INVENTORY
{
        UINT        shortId;
        TYPER_ID    modemId;
    //  BYTE        portNum;
    //  TYPER_ID    meterId;
    //  UINT        parserType;
    //  BYTE        serviceType;
    //  BYTE        vendor;
    //  BYTE        modelLen;
    //  BYTE        model[0];
    //  WORD        hwVer;
    //  WORD        swVer;   
    //  BYTE        swBuild; 
    //  TIMESTAMP   installTime;   
    //  BYTE        lpPeriod;   
}    __ATTRIBUTE_PACKED__  TYPER_INVENTORY;

typedef struct    _tagTYPER_CHANNEL_CONFIGURE
{
        BYTE        index;
        BYTE        unit;
        BYTE        sigExp;
        BYTE        valueType;
        BYTE        objectType;
        BYTE        channelType;
}    __ATTRIBUTE_PACKED__  TYPER_CHANNEL_CONFIGURE;

typedef struct    _tagTYPER_CHANNEL
{
        UINT        shortId;
        TYPER_ID    modemId;
     // BYTE        chCnt;
     // TYPER_CHANNEL_CONFIGURE     configure[0];
}    __ATTRIBUTE_PACKED__  TYPER_CHANNEL;

typedef struct    _tagTYPER_LP_VALUE
{
        TIMESTAMP5  time;
        UINT        values[0];
}    __ATTRIBUTE_PACKED__  TYPER_LP_VALUE;

typedef struct    _tagTYPER_BASEPULSE_VALUE
{
        unsigned long long  lastValue;
        unsigned long long  baseValue;
}    __ATTRIBUTE_PACKED__  TYPER_BASEPULSE_VALUE;

typedef struct    _tagTYPER_BASEPULSE
{
        TIMESTAMP5 lastTime;
        TIMESTAMP5 baseTime;
        TYPER_BASEPULSE_VALUE   values[0];
}    __ATTRIBUTE_PACKED__  TYPER_BASEPULSE;

typedef struct    _tagTYPER_LOG_VALUE
{
        BYTE        category;
        BYTE        length;
        TIMESTAMP5  time;
        BYTE        data[0];
}    __ATTRIBUTE_PACKED__  TYPER_LOG_VALUE;

typedef struct    _tagTYPER_LOG
{
        BYTE        logCnt;
        TYPER_LOG_VALUE     values[0];
}    __ATTRIBUTE_PACKED__  TYPER_LOG;

typedef struct    _tagTYPER_METERING
{
        UINT        shortId;
        TYPER_ID    modemId;
     // BYTE        chCnt;
     // BYTE        bpCnt;
     // TYPER_BASEPULSE     bp[0];
}    __ATTRIBUTE_PACKED__  TYPER_METERING;



#ifdef _WIN32
#pragma pack(pop)
#endif

#endif	// __IF4_FRAME_TYPE_H__
