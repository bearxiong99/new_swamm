#ifndef __DLMS_OBIS_H__
#define __DLMS_OBIS_H__

#include "typedef.h"

#ifdef _WIN32
#pragma pack(push, 1)
#endif

typedef struct	_tagOBISCODE
{
		BYTE		a;
		BYTE		b;
		BYTE		c;
		BYTE		d;
		BYTE		e;
		BYTE		f;
}	__ATTRIBUTE_PACKED__ OBISCODE;

#ifdef _WIN32
#pragma pack(pop)
#endif

typedef struct	_tagDLMSOBIS
{
		BYTE		cls;							// Class ID
		OBISCODE	code;							// OBIS code
		BYTE		attr;							// Attribute
}	DLMSOBIS;

#endif	// __DLMS_OBIS_H__
