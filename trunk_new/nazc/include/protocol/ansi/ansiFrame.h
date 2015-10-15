#ifndef __ANSI_FRAME_H__
#define __ANSI_FRAME_H__

#include "typedef.h"

typedef struct _tagANSI_HEADER
{
		BYTE		stp;                            // 0xEE
		BYTE		reserved;
		BYTE		ctrl;                           // Control field
		BYTE		seq;
		WORD		length;
}	__ATTRIBUTE_PACKED__ ANSI_HEADER;

typedef struct _tagANSI_TAIL
{
		WORD		crc;
}	__ATTRIBUTE_PACKED__ ANSI_TAIL;

typedef struct _tagANSI_FRAME
{
        ANSI_HEADER	    hdr;
        BYTE            payload[256];
        ANSI_TAIL       tail;
}   __ATTRIBUTE_PACKED__ ANSI_FRAME;

/////////////////////////////////////////////////////

typedef struct _tagANSI_CODE_PAYLOAD
{
        BYTE            code;
}   __ATTRIBUTE_PACKED__ ANSI_CODE_PAYLOAD;

typedef ANSI_CODE_PAYLOAD   ANSI_IDENTIFICATION_PAYLOAD;

typedef struct _tagANSI_NEGOTIATE_PAYLOAD
{
        BYTE            code;
        WORD            packetSize;
        BYTE            nbrPacket;
        BYTE            baudRate;
}   __ATTRIBUTE_PACKED__ ANSI_NEGOTIATE_PAYLOAD;

typedef struct _tagANSI_LOGON_PAYLOAD
{
        BYTE            code;
        WORD            userId;
        BYTE            user[10];
}   __ATTRIBUTE_PACKED__ ANSI_LOGON_PAYLOAD;

typedef struct _tagANSI_SECURITY_PAYLOAD
{
        BYTE            code;
        BYTE            passwd[20];
}   __ATTRIBUTE_PACKED__ ANSI_SECURITY_PAYLOAD;

typedef ANSI_CODE_PAYLOAD   ANSI_LOGOFF_PAYLOAD;
typedef ANSI_CODE_PAYLOAD   ANSI_TERMINATE_PAYLOAD;

typedef struct _tagANSI_FULLREAD_PAYLOAD
{
        BYTE            code;
        WORD            tableId;
}   __ATTRIBUTE_PACKED__ ANSI_FULLREAD_PAYLOAD;

typedef struct _tagANSI_PREAD_INDEX_PAYLOAD
{
        BYTE            code;       // 0x31
        WORD            tableId;
        WORD            index;
        WORD            count;
}   __ATTRIBUTE_PACKED__ ANSI_PREAD_INDEX_PAYLOAD;

typedef struct _tagANSI_PREAD_OFFSET_PAYLOAD
{
        BYTE            code;       // 0x3F
        WORD            tableId;
        WORD24          offset;
        WORD            count;
}   __ATTRIBUTE_PACKED__ ANSI_PREAD_OFFSET_PAYLOAD;

typedef struct _tagANSI_FULLWRITE_PAYLOAD
{
        BYTE            code;
        WORD            tableId;
        WORD            count;
        BYTE            data[256];
}   __ATTRIBUTE_PACKED__ ANSI_FULLWRITE_PAYLOAD;

typedef struct _tagANSI_PWRITE_INDEX_PAYLOAD
{
        BYTE            code;      // 0x41
        WORD            tableId;
        WORD            index;
        WORD            count;
        BYTE            data[256];
}   __ATTRIBUTE_PACKED__ ANSI_PWRITE_INDEX_PAYLOAD;

typedef struct _tagANSI_PWRITE_OFFSET_PAYLOAD
{
        BYTE            code;      // 0x4F
        WORD            tableId;
        WORD24          offset;
        WORD            count;
        BYTE            data[256];
}   __ATTRIBUTE_PACKED__ ANSI_PWRITE_OFFSET_PAYLOAD;

typedef struct _tagANSI_TABLE_PAYLOAD
{
        BYTE            status;
        WORD            count;
        BYTE            data[0];
}   __ATTRIBUTE_PACKED__ ANSI_TABLE_PAYLOAD;

/////////////////////////////////////////////////////

typedef struct _tagANSI_RESULT_CODE
{
        BYTE            status;
}   __ATTRIBUTE_PACKED__ ANSI_RESULT_CODE;

typedef struct _tagANSI_TABLE_HEADER
{
		char		kind;
		char		table[3];
		WORD		length;
}	__ATTRIBUTE_PACKED__ ANSI_TABLE_HEADER;

typedef struct _tagANSI_TABLE
{
		ANSI_TABLE_HEADER	hdr;
		char		data[0];
}	__ATTRIBUTE_PACKED__ ANSI_TABLE;


#endif	// __ANSI_FRAME_H__

