#ifndef __ANSI_H__
#define __ANSI_H__

// Table 1: General Manufacturer Identification Table
typedef struct	_tagANSI_TABLE1
{
		BYTE	MANUFACTURER[4];
		BYTE	ED_MODEL[8];
		BYTE	HW_VERSION_NUMBER;
		BYTE	HW_REVISION_NUMBER;
		BYTE	FW_VERSION_NUMBER;
		BYTE	FW_REVISION_NUMBER;
		BYTE	MFG_SERIAL_NUMBER[16];
}	__attribute__ ((packed)) ANSI_TABLE1;

// Table 5: Standard Device Identification Table
typedef struct	_tagANSI_TABLE5
{
		BYTE	METER_ID[20];
}	__attribute__ ((packed)) ANSI_TABLE5;

// Table 7: Procedure Initiate Table
typedef struct	_tagANSI_TABLE7
{
		WORD	TBL_PROC;
		BYTE	SEQ_NUM;
		BYTE	PARAM[8];
}	__attribute__ ((packed)) ANSI_TABLE7;

// Table 8: Procedure Response Table
typedef struct	_tagANSI_TABLE8
{
		WORD	TBL_PROC;
		BYTE	SEQ_NBR;
		BYTE	RESULT_CODE;
		BYTE	RESP_DATA_RCD[8];
}	__attribute__ ((packed)) ANSI_TABLE8;

// Table 61: Actual Load Profile Table
typedef struct	_tagANSI_TABLE61
{
		UINT	LP_MEMORY_LEN;
		WORD	LP_FLAGS;
		BYTE	LP_FORMATS;
		WORD	NBR_BLKS_SET1;
		WORD	NBR_BLK_INTS_SET1;
		BYTE	NBR_CHNS_SET1;
		BYTE	MAX_INT_TIME_SET1;
}	__attribute__ ((packed)) ANSI_TABLE61;

// Table 63: Load Profile Status Table 
typedef struct	_tagANSI_TABLE63
{
		BYTE	LP_SET_STATUS_FLAGS;
		WORD	NBR_VALID_BLOCKS;
		WORD	LAST_BLOCK_ELEMENT;
		UINT	LAST_BLOCK_SEQ_NBR;
		WORD	NBR_UNREAD_BLOCKS;
		WORD	NBR_VALID_INT;
}	__attribute__ ((packed)) ANSI_TABLE63;

// Table 113: Power Quality Data Table 
typedef struct	_tagANSI_TABLE113
{
		WORD	RMS_VOLTAGE_PHA;
		WORD	RMS_VOLTAGE_PHC;
		BYTE	MOMENTARY_INTERVAL_PF;
}	__attribute__ ((packed)) ANSI_TABLE113;

typedef struct  _tagSTAT_TABLE
{
		UINT	TRANSMIT_PKT_COUNT;
		UINT	SEND_PKT_COUNT;
		UINT	RECV_PKT_COUNT;
		UINT	ERROR_PKT_COUNT;

		UINT	TOTAL_SEND_BYTES;
		UINT	TOTAL_RECV_BYTES;
		UINT	TOTAL_USE_MSEC;
		UINT	SENSOR_CONNECT_MSEC;
		UINT	TOTAL_REPLY_WAIT_MSEC;
		UINT	MIN_REPLY_MSEC;
		UINT	MAX_REPLY_MSEC;
}	__attribute__ ((packed)) STAT_TABLE;

#endif	// __ANSI_H__
