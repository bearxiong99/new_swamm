#ifndef __DLMS_TYPE_H__
#define __DLMS_TYPE_H__

#include "typedef.h"
#include "dlmsDefine.h"
#include "dlmsObis.h"

#define MAX_DLMS_RESULT_SIZE					2048*8

// Clock status
// The status bits are defined as follows:
//   bit 7 (MSB): daylight saving active
//   bit 6: reserved
//   bit 5: reserved
//   bit 4: reserved
//   bit 3: invalid clock status
//   bit 2: different clock base 
//   bit 1: doubtful value
//   bit 0 (LSB): invalid value

#ifdef _WIN32
#pragma pack(push, 1)
#endif

typedef struct	_tagDLMSTIMEDATE
{
		WORD		year;						// Year
		BYTE		mon;						// Month
		BYTE		day;						// Day of month
		BYTE		week;						// Day of week (1:월요일 ~ 7:일요일)
		BYTE		hour;						// Hour
		BYTE		min;						// Min
		BYTE		sec;						// Second
		BYTE		hsec;						// hundredths of second (1/100초)
		signed short deviation;					// GMT와의 시간 오차 (-720 ~ 720), 단위 : 분
		BYTE		status;						// Clock status
}   __ATTRIBUTE_PACKED__ DLMSTIMEDATE;

typedef struct	_tagDLMSDATE
{
		WORD		year;						// Year
		BYTE		mon;						// Month
		BYTE		day;						// Day of month
		BYTE		week;						// Day of week (1:월요일 ~ 7:일요일)
}   __ATTRIBUTE_PACKED__ DLMSDATE;

typedef struct	_tagDLMSTIME
{
		BYTE		hour;						// Hour
		BYTE		min;						// Min
		BYTE		sec;						// Second
		BYTE		hsec;						// hundredths of second (1/100초)
}   __ATTRIBUTE_PACKED__ DLMSTIME;

typedef struct	_tagDLMSRESULT
{
        BYTE        headerSize;                         // Header size
		BYTE		dest;								// Destination
		UINT		src;								// Source
		BYTE		method;								// GET/SET/ACTION
		BYTE		resFlag;							// Normal/Datablock/List
		BYTE		nError;								// Result error code
		WORD		nLength;							// Data length
        BYTE        lastBlock;                          // last block
		BYTE		szBuffer[MAX_DLMS_RESULT_SIZE];		// Data buffer
}	__ATTRIBUTE_PACKED__ DLMSRESULT;

#ifdef _WIN32
#pragma pack(pop)
#endif

typedef struct  _tagDLMSTAGVALUE
{
    int nLength;
    BYTE pPoint[0];
} DLMSTAGVALUE;

typedef struct	_tagDLMSVARIABLE
{
		BYTE		type;
		BYTE		len;
		union
		{
			unsigned char			array;
			signed char				i8;
			unsigned char			u8;
			signed short			i16;
			unsigned short			u16;
			signed int				i32;
			unsigned int			u32;
			float 					f32;
			signed long long int	i64;
			unsigned long long int	u64;
			DLMSTIMEDATE			td;
			DLMSDATE				d;
			DLMSTIME				t;
			unsigned char			*p;
		} var;
}	DLMSVARIABLE;

typedef struct	_tagDLMSPARAM
{
		UINT		ident;
		UINT		alloc;
		UINT		count;
		DLMSVARIABLE *list;
}	DLMSPARAM;

#if 0
typedef struct	_tagDLMSOBJECT
{
		UINT		ident;
}	DLMSOBJECT;	

typedef struct	_tagDLMSDATA
{
		UINT		ident;
		UINT		count;
		DLMSVARIABLE *list;
}	DLMSDATA;
typedef struct	_tagDLMSACTION
{
		UINT		ident;
		UINT		count;
}	DLMSACTION;

typedef struct	_tagDLMSACCESS
{
		UINT		ident;
		UINT		count;
}	DLMSACCESS;

typedef struct	_tagDLMSCLIENT
{
		UINT		ident;								// Identifier
		char		addr[33];							// Target IP address
		WORD		port;								// Target Socket Port
		BYTE		src;								// Source address
		UINT		dest;								// Destination address
		int			timeout;							// Timeout

		BYTE		lastRxControl;
		BYTE		lastTxControl;
		UINT		maxInfoTransmitLen;					// Maximum information field length transmit
		UINT		maxInfoReceiveLen;					// Maximum information field length receive
		UINT		windowTransmit;						// Window size transmit
		UINT		windowReceive;						// Window size receive

		void		*session;							// TCP Session
}	DLMSCLIENT;

typedef struct	_tagDLMSSTATUS
{
		UINT		sendPacket;							// Total send packet
		UINT		recvPacket;							// Total recv packet
		UINT		sendBytes;							// Total send bytes
		UINT		recvBytes;							// Total recv bytes
}	DLMSSTATUS;

typedef void *		HSESSION;

typedef BOOL (*PFNDLMSTRAP)(HSESSION hSession, BYTE nType, BYTE *data, int nLength);
#endif

#endif	// __DLMS_TYPE_H__
