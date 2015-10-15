#ifndef __DLMS_FRAME_H__
#define __DLMS_FRAME_H__

#include "typedef.h"

#ifdef _WIN32
#pragma pack(push, 1)
#endif

////////////////////////////////////////////////////////////////////////////////
// HDLC Frame
////////////////////////////////////////////////////////////////////////////////

typedef struct  _tagHDLC_REQ_ADDR1
{
        BYTE        dest;                               // Destination address
        BYTE        src;                                // Source address	
}   __ATTRIBUTE_PACKED__ HDLC_REQ_ADDR1;

typedef struct  _tagHDLC_REQ_ADDR2
{
        WORD        dest;								// Destination address
        BYTE        src;								// Source address	
}   __ATTRIBUTE_PACKED__ HDLC_REQ_ADDR2;

typedef struct  _tagHDLC_REQ_ADDR4
{
        UINT        dest;                               // Destination address
        BYTE        src;                                // Source address	
}   __ATTRIBUTE_PACKED__ HDLC_REQ_ADDR4;

typedef union  _tagHDLC_REQ_ADDR
{
    HDLC_REQ_ADDR1  d1;
    HDLC_REQ_ADDR2  d2;
    HDLC_REQ_ADDR4  d4;
}   __ATTRIBUTE_PACKED__ HDLC_REQ_ADDR;

typedef struct  _tagHDLC_RES_ADDR1
{
        BYTE        dest;                               // Destination address
        BYTE        src;                                // Source address	
}   __ATTRIBUTE_PACKED__ HDLC_RES_ADDR1;

typedef struct  _tagHDLC_RES_ADDR2
{
        BYTE        dest;								// Destination address	
        WORD        src;								// Source address
}   __ATTRIBUTE_PACKED__ HDLC_RES_ADDR2;

typedef struct  _tagHDLC_RES_ADDR4
{
        BYTE        dest;                               // Destination address
        UINT        src;                                // Source address	
}   __ATTRIBUTE_PACKED__ HDLC_RES_ADDR4;

typedef union  _tagHDLC_RES_ADDR
{
    HDLC_RES_ADDR1  s1;
    HDLC_RES_ADDR2  s2;
    HDLC_RES_ADDR4  s4;
}   __ATTRIBUTE_PACKED__ HDLC_RES_ADDR;

typedef struct  _tagHDLC_HEADER
{
        BYTE        startFlag;                          // 0x7E
        WORD        typeLen;                            // Type & Length (1010SLLLLLLLLLLL) 0x07FF
		HDLC_REQ_ADDR	addr;							// Request address
        BYTE        ctrl;								// Frame control 
}   __ATTRIBUTE_PACKED__ HDLC_HEADER;

typedef struct  _tagHDLC_TAIL
{
        WORD        crc;                                // CRC16
        BYTE        endFlag;                            // 0x7E
}   __ATTRIBUTE_PACKED__ HDLC_TAIL;

typedef struct  _tagHDLC_FRAME
{
        HDLC_HEADER     hdr;
        HDLC_TAIL       tail;
}   __ATTRIBUTE_PACKED__ HDLC_FRAME;

////////////////////////////////////////////////////////////////////////////////
// DLMS 
////////////////////////////////////////////////////////////////////////////////

typedef HDLC_REQ_ADDR		DLMS_REQ_ADDR;
typedef HDLC_RES_ADDR		DLMS_RES_ADDR;

typedef struct  _tagDLMS_ARG
{
		BYTE		tag;								// TAG type
		union
		{
			signed char		i8;
			unsigned char	u8;
			signed short	i16;
			unsigned short	u16;
			signed int		i32;
			unsigned int	u32;
            float           f32;
			signed long long int	i64;
			unsigned long long int	u64;
			unsigned char	array;
		} var;
}   __ATTRIBUTE_PACKED__ DLMS_ARG;

typedef struct  _tagDLMS_TAG
{
		BYTE		tag;								// TAG type
		BYTE		len;								// TAG data length
		BYTE		data[2];							// Data
}   __ATTRIBUTE_PACKED__ DLMS_TAG;

typedef struct  _tagDLMS_HEADER
{
        BYTE        startFlag;                          // 0x7E
        WORD        typeLen;                            // Type & Length (1010SLLLLLLLLLLL)
		DLMS_REQ_ADDR	addr;							// Request address
        BYTE        ctrl;								// Frame control 
		WORD		hcs;								// Header check sequence
}   __ATTRIBUTE_PACKED__ DLMS_HEADER;

typedef HDLC_TAIL	DLMS_TAIL;

// UA - Unnumbered acknowledge (SNRM, DISC response)

typedef struct  _tagDLMS_FRAME
{
		DLMS_HEADER		hdr;							// Header
		BYTE			payload[256];					// Payload
		DLMS_TAIL		tail;							// Tail
}   __ATTRIBUTE_PACKED__ DLMS_FRAME;

// UA (SNRM response)

#define UA_PARAM_MAX_INFO_TX_LENGTH			0x05		// Maximum information field length transmit
#define UA_PARAM_MAX_INFO_RX_LENGTH			0x06		// Maximum information field length receive
#define UA_PARAM_TX_WINDOW_SIZE				0x07		// Window size transmit
#define UA_PARAM_RX_WINDOW_SIZE				0x08		// Window size receive

typedef struct  _tagDLMS_UA_PAYLOAD
{
		BYTE			format;							// Format identifier (0x81)
		BYTE			group;							// Group identifier (0x80)
		BYTE			groupLen;						// Group length
		DLMS_TAG		param[1];						// param[0] ... param[n]
}   __ATTRIBUTE_PACKED__ DLMS_UA_PAYLOAD;

typedef struct  _tagDLMS_UA_FRAME
{
		DLMS_HEADER		hdr;							// Header
		DLMS_UA_PAYLOAD	payload;
		DLMS_TAIL		tail;							// Tail
}   __ATTRIBUTE_PACKED__ DLMS_UA_FRAME;

// AARQ - Application Association Request

typedef struct  _tagDLMS_AARQ_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		DLMS_TAG	param[0];							// param[0] ... param[n]
}   __ATTRIBUTE_PACKED__ DLMS_AARQ_PAYLOAD;

// AARE

typedef struct  _tagDLMS_AARE_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		DLMS_TAG	param[0];							// param[0] ... param[n]
}   __ATTRIBUTE_PACKED__ DLMS_AARE_PAYLOAD;

typedef struct  _tagDLMS_AARE_FRAME
{
		DLMS_HEADER		hdr;							// Header
		BYTE			destLSAP;						// LSAP Destination
		BYTE			srcLSAP;						// LSAP Source
		BYTE			result;							// Result
		BYTE			tag;							// AARE tag
		BYTE			tagLen;							// AARE len
		BYTE			payload[100];					// Payload
		DLMS_TAIL		tail;							// Tail
}   __ATTRIBUTE_PACKED__ DLMS_AARE_FRAME;

// GET (GET.requset.normal, GET.response.normal, GET.request.list, ...)

typedef struct  _tagDLMS_REQUEST_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		reqFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		WORD		cls;								// interface class
		OBISCODE	obis;								// logical name, OBIS code
		BYTE		attr;								// Attribute
		BYTE		logical;							// Logical name (0x00)
		BYTE		param[64];							// Parameters
}	__ATTRIBUTE_PACKED__ DLMS_REQUEST_PAYLOAD;

typedef struct  _tagDLMS_GETREQ_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		reqFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		WORD		cls;								// interface class
		OBISCODE	obis;								// logical name, OBIS code
		BYTE		attr;								// Attribute
//		BYTE		logical;							// Logical name (0x00)
}	__ATTRIBUTE_PACKED__ DLMS_GETREQ_PAYLOAD;

typedef struct  _tagDLMS_GETREQ_FRAME
{
		DLMS_HEADER	hdr;								// Header
		DLMS_GETREQ_PAYLOAD	payload;					// Payload
        DLMS_TAIL  	tail;								// Tail
}   __ATTRIBUTE_PACKED__ DLMS_GETREQ_FRAME;

typedef struct  _tagDLMS_GETBLOCKREQ_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		reqFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		UINT		nBlockCount;						// Block count
}	__ATTRIBUTE_PACKED__ DLMS_GETBLOCKREQ_PAYLOAD;

typedef struct  _tagDLMS_GETBLOCKREQ_FRAME
{
		DLMS_HEADER	hdr;								// Header
		DLMS_GETBLOCKREQ_PAYLOAD	payload;			// Payload
        DLMS_TAIL  	tail;								// Tail
}   __ATTRIBUTE_PACKED__ DLMS_GETBLOCKREQ_FRAME;

typedef struct  _tagDLMS_SELECTEDGETREQ_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		reqFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		WORD		cls;								// interface class
		OBISCODE	obis;								// logical name, OBIS code
		BYTE		attr;								// Attribute
#if defined(__SUPPORT_DLMS_SELECTOR_ATTRIBUTE__)
        /** 왜 selector가 빠져야 하는지 파악 필요 */
        BYTE        selector;                           // Selector (1/2)
#endif
		BYTE		typeArrTag;							// Array (1)
		BYTE		typeArrValue;						// 1
		BYTE		typeStructureTag;					// Structure (2)
		BYTE		typeStructureValue;					// 4
		BYTE		firstEntryTag;						// first entry to retrive
		UINT		firstEntry;
		BYTE		lastEntryTag;						// end entry to retrive
		UINT		lastEntry;
		BYTE		firstValueTag;						// index of first value to retrive
		WORD		firstValue;
		BYTE		lastValueTag;						// index of last value to retrive
		WORD		lastValue;
}	__ATTRIBUTE_PACKED__ DLMS_SELECTEDGETREQ_PAYLOAD;

typedef struct  _tagDLMS_SELECTEDGETREQ_FRAME
{
		DLMS_HEADER	hdr;								// Header
		DLMS_SELECTEDGETREQ_PAYLOAD	payload;			// Payload
        DLMS_TAIL  	tail;								// Tail
}   __ATTRIBUTE_PACKED__ DLMS_SELECTEDGETREQ_FRAME;

typedef struct  _tagDLMS_BLOCKRES_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		resFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		BYTE		last;								// Get-Data-Result choice data
        UINT        blockCount;                         // Block Count
		BYTE		payload[0];					        // Payload
}	__ATTRIBUTE_PACKED__ DLMS_BLOCKRES_PAYLOAD;

typedef struct  _tagDLMS_GETRES_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		resFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		BYTE		dataChoice;							// Get-Data-Result choice data
		DLMS_TAG	param[1];							// Tag data (param[0] .. param[n]
}	__ATTRIBUTE_PACKED__ DLMS_GETRES_PAYLOAD;

typedef struct  _tagDLMS_GETRES_FRAME
{
		DLMS_HEADER	hdr;								// Header
		DLMS_GETRES_PAYLOAD	payload;					// Payload
        DLMS_TAIL  	tail;								// Tail
}   __ATTRIBUTE_PACKED__ DLMS_GETRES_FRAME;

typedef struct  _tagDLMS_GETERR_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		resFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		BYTE		result;								// Get-Data-Result choice data
		BYTE		resCode;							// Result code
}	__ATTRIBUTE_PACKED__ DLMS_GETERR_PAYLOAD;

typedef struct  _tagDLMS_GETERR_FRAME
{
		DLMS_HEADER	hdr;								// Header
		DLMS_GETERR_PAYLOAD	payload;					// Payload
        DLMS_TAIL  	tail;								// Tail
}   __ATTRIBUTE_PACKED__ DLMS_GETERR_FRAME;

// ACTION (ACTION.requset.normal
typedef struct  _tagDLMS_ACTIONREQ_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		reqFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		WORD		cls;								// interface class
		OBISCODE	obis;								// logical name, OBIS code
		BYTE		actMethod;							// Action Method
}	__ATTRIBUTE_PACKED__ DLMS_ACTIONREQ_PAYLOAD;

typedef struct  _tagDLMS_ACTIONREQ_FRAME
{
		DLMS_HEADER	hdr;								// Header
		DLMS_ACTIONREQ_PAYLOAD	payload;				// Payload
        DLMS_TAIL  	tail;								// Tail
}   __ATTRIBUTE_PACKED__ DLMS_ACTIONREQ_FRAME;

typedef struct  _tagDLMS_ACTIONRES_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		resFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		BYTE		result;								// Action Result
		BYTE		dataChoice;							// Get-Data-Result choice data
		DLMS_TAG	param[1];							// Tag data (param[0] .. param[n]
}	__ATTRIBUTE_PACKED__ DLMS_ACTIONRES_PAYLOAD;
// SET

typedef struct  _tagDLMS_SETREQ_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		reqFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		WORD		cls;								// interface class
		OBISCODE	obis;								// logical name, OBIS code
		BYTE		attr;								// Attribute
		BYTE		logical;							// Logical name (0x00)
		DLMS_TAG	param;								// Set data value
}	__ATTRIBUTE_PACKED__ DLMS_SETREQ_PAYLOAD;

typedef struct  _tagDLMS_SETREQ_FRAME
{
		DLMS_HEADER	hdr;								// Header
		DLMS_SETREQ_PAYLOAD	payload;					// Payload
        DLMS_TAIL  	tail;								// Tail
}   __ATTRIBUTE_PACKED__ DLMS_SETREQ_FRAME;

typedef struct  _tagDLMS_SETRES_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		resFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		BYTE		result;								// Get-Data-Result choice data
}	__ATTRIBUTE_PACKED__ DLMS_SETRES_PAYLOAD;

typedef struct  _tagDLMS_SETRES_FRAME
{
		DLMS_HEADER	hdr;								// Header
		DLMS_SETRES_PAYLOAD	payload;					// Payload
        DLMS_TAIL  	tail;								// Tail
}   __ATTRIBUTE_PACKED__ DLMS_SETRES_FRAME;

// ConfirmedServiceError
typedef struct  _tagDLMS_SERVICEERR_PAYLOAD
{
        BYTE        destLSAP;						    // LSAP Destination
        BYTE        srcLSAP;						    // LSAP Source
		BYTE		quality;							// LLC Quality (0x00)
		BYTE		method;								// GET/SET/ACTION, Requst/Response
		BYTE		resFlag;							// Normal, datablock, list	
		BYTE		idPriority;							// invoke-id and priority (0x81)
		BYTE		confirmedServiceErr;                // ConfirmedServiceError Code
		BYTE		serviceErr;					        // ServiceError Code
        BYTE        detail;                             // Detail
}	__ATTRIBUTE_PACKED__ DLMS_SERVICEERR_PAYLOAD;

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif	// __DLMS_FRAME_H__
