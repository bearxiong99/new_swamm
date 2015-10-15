#ifndef __GE_METER_H__
#define __GE_METER_H__

#include "ansi.h"

// ANSI
#define ANSI_STP            	0xEE
#define ANSI_ACK            	0x06
#define ANSI_NAK            	0x15
#define	ANSI_NO_ERROR			0x00

// ANSI meter_cmd
#define ANSI_IDENT      		0x20                            // identify
#define ANSI_NEGO       		0x61                            // negotiate
#define ANSI_LOGON      		0x50                            // log on
#define ANSI_SECURITY   		0x51                            // Security
#define ANSI_AUTH       		0x53                            // authenticate
#define ANSI_READ       		0x30                            // read
#define ANSI_PREAD      		0x3F                            // partial read offset
#define ANSI_WRITE      		0x40                            // write
#define ANSI_WAIT       		0x70                            // wait
#define ANSI_LOGOFF     		0x52                            // log off
#define ANSI_TERMI      		0x21                            // terminate
#define ANSI_DISCON     		0x22                            // disconnect
#define ANSI_RR         		0xFF                            // reading continue

// meter_read_class     
// Standard Table
#define ANSI_TABLE_ST_01        0x0001                          // Manufacturer Identification Table
#define ANSI_TABLE_ST_03        0x0003                          // End Device Mode & Status Table
#define ANSI_TABLE_ST_05        0x0005                          // Device Identification Table
#define ANSI_TABLE_ST_07		0x0007							// Procedure Initiate Table
#define ANSI_TABLE_ST_08		0x0008							// Procedure Response Table
#define ANSI_TABLE_ST_21        0x0015                          // Actual Register Table
#define ANSI_TABLE_ST_22        0x0016                          // Data Selection Table 
#define ANSI_TABLE_ST_23        0x0017                          // Present Data Table
#define ANSI_TABLE_ST_25        0x0019                          // Prev Demand Reset Data Table
#define ANSI_TABLE_ST_55        0x0037                          // Clock State Table
#define ANSI_TABLE_ST_61        0x003D                          // Actual Load Profile Table
#define ANSI_TABLE_ST_62        0x003E                          // Load Profile Control Table
#define ANSI_TABLE_ST_63        0x003F                          // Load Profile Status Table
#define ANSI_TABLE_ST_64        0x0040                          // Load Profile Data Set 1 Table
#define ANSI_TABLE_ST_71        0x0047                          // Actual Log Table
#define ANSI_TABLE_ST_72        0x0048                          // Events Identification Table
#define ANSI_TABLE_ST_75        0x004B                          // Event Log Control Table
#define ANSI_TABLE_ST_76        0x004C                          // Event Log Data Table
#define ANSI_TABLE_ST_130		0x0082							// Relay Switch Status Table
#define ANSI_TABLE_ST_131		0x0083							// Relay Operate Table
#define ANSI_TABLE_ST_132		0x0084							// Relay Switch Log Table

// Manufacturer Table
#define ANSI_TABLE_MT_66        0x0842                          //
#define ANSI_TABLE_MT_67        0x0843                          // Meter Program Constants - 2
#define ANSI_TABLE_MT_70        0x0846                          // Display Configuration
#define ANSI_TABLE_MT_75        0x084B                          // Scale Factors
#define ANSI_TABLE_MT_78        0x084E                          // Security Log Table
#define ANSI_TABLE_MT_113       0x0871                          // Power Quality Data Table
#define ANSI_TABLE_MT_110		0x086E							// Present Register Data Table
#define ANSI_TABLE_MT_72		0x0848							// Line-Side Diagnostics / Power Quality Data Table
#define ANSI_TABLE_MT_112		0x0870							// 

// Meter Basic Config
#define METER_RETRY         	5                               // meter reading retry count
#define METERBUF_SIZE       	9728                            // buffer size for packet data

// packet status definitions
#define P_STAT_OK               0x00

#define P_STAT_NOANSWER     	0x01							// error no answer
#define P_STAT_ERR_CRC      	0x02                            // error CRC
#define P_STAT_ERR_ID       	0x03                            // error identification
#define P_STAT_ERR_FRT      	0x04                            // error data format
#define P_STAT_ERR_LNT      	0x05                            // error length
#define	P_STAT_ERR_CODE			0x06							// error response code

#define	ANSI_FRAME_READ1		0
#define	ANSI_FRAME_READ2		1

#define READ_OPTION_FULL		0								// Full table read
#define READ_OPTION_CURRENT		1								// Current table read
#define READ_OPTION_PREVIOUS	2								// Previouse table read
#define READ_OPTION_RELAY		3								// Relay switch table read
#define WRITE_OPTION_RELAYON	4								// Relay switch on
#define WRITE_OPTION_RELAYOFF	5								// Relay switch off
#define WRITE_OPTION_ACTON		6								// Relay activation on
#define WRITE_OPTION_ACTOFF		7								// Relay activation off
#define WRITE_OPTION_TIMESYNC	8								// Timesync
#define READ_OPTION_TEST		9								// Rest read
#define READ_OPTION_EVENTLOG	10								// Event logs
#define READ_IDENT_ONLY			11
#define OPTION_MAX				READ_IDENT_ONLY

#define	RELAY_ACTIVATION_OFF	0
#define RELAY_ACTIVATION_ON		1
#define RELAY_SWITCH_ON			2
#define RELAY_SWITCH_OFF		3

// meter kind
#define GE_IEC_I210				0
#define GE_IEC_KV2C				1

// meter protocol common variable
typedef struct _ansiprotocol
{
    BYTE       	packet_cmd;
    WORD      	packet_table_id;
    BYTE       	packet_ctrl;
    WORD      	packet_length;
    UINT      	packet_pread_offset;
    WORD      	packet_pread_count;
    char       	packet_write_table_buf[256];
    WORD      	packet_write_table_count;
	BYTE		packet_retry;
	BYTE		packet_write_ok;
}__attribute__ ((packed)) ANSI_CONFIG;

typedef struct _meterconfig
{
    BYTE        meter_rr_max;
    BYTE        meter_rr_count;
	char		table_data[8092];
	WORD		table_length;
	char		meter_serial_num[17];
	char		meter_id[21];
	WORD		meter_lp_valid_blocks;
    WORD      	meter_lp_valid_int;
	WORD		meter_event_length;
	BYTE		meter_relay_activation;
	BYTE		meter_relay_switch;
	BYTE		meter_relay_result;
    BYTE        meter_rr_control;
	BYTE		meter_kind;
    BYTE        meter_rxd_control;
    BYTE        meter_wait_count;
    BYTE        meter_read_inst_class;
    BYTE        meter_read_inst_class_phase;
    BYTE        meter_password[10];
    WORD      	meter_lp_len;
    WORD      	meter_lp_valid_num;
}__attribute__ ((packed)) METER_CONFIG;

// METER buffer
typedef struct _meterbuf
{
    WORD      	len;
    BYTE       	buf[METERBUF_SIZE];
}__attribute__ ((packed)) METER_BUF;

typedef struct _tagANSI_HEADER
{
		BYTE		stp;
		BYTE		reserved;
		BYTE		ctrl;
		BYTE		seq;
		WORD		length;
}	__attribute__ ((packed)) ANSI_HEADER;

typedef struct _tagANSI_TAIL
{
		WORD		crc;
}	__attribute__ ((packed)) ANSI_TAIL;

typedef struct _tagANSI_FRAME
{
		ANSI_HEADER	hdr;
		BYTE		data[128];
		ANSI_TAIL	tail;
}	__attribute__ ((packed)) ANSI_FRAME;

typedef struct _tagPACKET_TABLE_HEADER
{
		char		kind;
		char		table[3];
		WORD		length;
}	__attribute__ ((packed)) PACKET_TABLE_HEADER;

typedef struct _tagPACKET_TABLE
{
		PACKET_TABLE_HEADER	hdr;
		char		data[0];
}	__attribute__ ((packed)) PACKET_TABLE;

typedef struct _tagPACKET_HEADER
{
		char		ident[3];
		int			length;
		BYTE		group;
		BYTE		member;
		EUI64		sensor_id;
		char		meter_id[18];
}	__attribute__ ((packed)) PACKET_HEADER;

typedef struct _tagPACKET_TAIL
{
		WORD		crc;
		char		eof;
}	__attribute__ ((packed)) PACKET_TAIL;

#endif	// __GE_METER_H__

