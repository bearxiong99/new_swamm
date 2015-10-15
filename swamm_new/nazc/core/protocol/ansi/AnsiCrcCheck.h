//////////////////////////////////////////////////////////////////////
//
//	AnsiCrcCheck.h: ANSI C12.18 Crc Check Functions
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2011 NURI Telecom Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __ANSI_CRC_CHECK_H__
#define __ANSI_CRC_CHECK_H__

WORD    AnsiGetCrc(BYTE *pszBuffer, int nLength);
BOOL    AnsiVerifyCrc(BYTE *pszBuffer, int nSize);

BYTE AnsiGetCheckSum(BYTE *pszBuffer, int nLength);

#endif	// __ANSI_CRC_CHECK_H__
