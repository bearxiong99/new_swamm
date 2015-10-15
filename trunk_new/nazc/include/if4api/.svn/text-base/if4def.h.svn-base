#ifndef __IF4_DEF_H__
#define __IF4_DEF_H__

#include "if4ver.h"
#include "vardef.h"

#define IF4_DEFAULT_PORT			8000			// Default Port Number
#define IF4_DEFAULT_ALARM_PORT		8001			// Default Port Number
#define IF4_DEFAULT_FRAME_SIZE		1024			

#define IF4_MIN_MULTI_PART_SIZE		4096
#define IF4_COMPRESS_MIN_SIZE		100
#define IF4_WINDOW_SIZE				1024
#define IF4_WINDOW_COUNT			8
#define IF4_WINDOW_GROW_SIZE		((IF4_WINDOW_SIZE * IF4_WINDOW_COUNT)*2)

// Command Service Attribute Definitions

#define IF4_CMDATTR_REQUEST			0x80			// Request/Respone Select (Set=Request)
#define IF4_CMDATTR_MCUIPC			0x40			// MCU IPC Communicatin Only
#define IF4_CMDATTR_RESPONSE		0x01			// Want Response Bit (Set=Need Respone, *defualt)

// Source Type Definitions

#define IF4_SRC_UNKNOWN				0				// Unknown Type
#define IF4_SRC_FEP					1				// FEP Server
#define IF4_SRC_MCU					2				// MCU
#define IF4_SRC_OAMPC				3				// PC
#define IF4_SRC_OAMPDA				4				// PDA
#define IF4_SRC_MOBILE				5				// CDMA/GSM/GPRS
#define IF4_SRC_SINK				6				// Zigbee Mask Module
#define IF4_SRC_SENSOR				7				// Zigbee Sensor
#define IF4_SRC_UNIT				8				// Entery Meter, AU, Expansion Unit

// Query Method

#define IF4_METHOD_GET				0				// GET Method
#define IF4_METHOD_SET				1				// SET Method
#define IF4_METHOD_LIST				2				// LIST Method

// Invoke Flag

#define IF4_FLAG_REPLY				0x01
#define IF4_FLAG_ACK				0x02

// Control Code

#define IF4_CTRLCODE_ACK			0
#define IF4_CTRLCODE_NAK			1
#define IF4_CTRLCODE_CAN			2

// Compress Type

#define IF4_COMP_NONE				0
#define IF4_COMP_ZLIB				1
#define IF4_COMP_GZIP				2

// Data File Type

// Measurement Metering Data Service Frame

#define IF4_DATAFILE_GENERIC		0
#define IF4_DATAFILE_MEASUREMENT	1
#define IF4_DATAFILE_SYSTEM			2
#define IF4_DATAFILE_APPLICATION	3
#define IF4_DATAFILE_CONFIG			4
#define IF4_DATAFILE_UNKNOWN		255

// Variable Hash
#define IF4_VARHASH_MCUID			0


// R DATA Type
#define IF4_TYPER_INVENTORY         0x01
#define IF4_TYPER_CONFIGURE         0x02
#define IF4_TYPER_METERING          0x03


#endif	// __IF4_DEF_H__
