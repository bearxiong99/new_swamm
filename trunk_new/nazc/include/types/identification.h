//////////////////////////////////////////////////////////////////////
//
//	identifications.h: AIMIR MCU  Identification Type Definitions.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2012 NURITELECOM Telecom Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __IDTYPES_H__
#define __IDTYPES_H__

#define ID_TYPE_UNDEFINED           0x00
#define ID_TYPE_IPV4                0x01
#define ID_TYPE_IPV6                0x02
#define ID_TYPE_MAC                 0x03
#define ID_TYPE_EUI64               0x04

#define ID_TYPE_SIGNED_NUMBER       0x11
#define ID_TYPE_UNSIGNED_NUMBER     0x12

#define ID_TYPE_PRINTABLE           0x21
#define ID_TYPE_BCD                 0x22
#define ID_TYPE_STREAM              0x23


// Object Type
#define OBJECT_TYPE_ELECTRICITY     0x01
#define OBJECT_TYPE_GAS             0x07
#define OBJECT_TYPE_WATER           0x08
#define OBJECT_TYPE_HOT_WATER       0x09

// Channel Value Type
#define VALUE_TYPE_DIFF             0x01
#define VALUE_TYPE_CURRENT          0x02

#endif 	// __IDTYPES_H__
