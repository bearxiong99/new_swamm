//////////////////////////////////////////////////////////////////////
//
//	IF4CrcCheck.h: IF4/IF5 Crc Check Functions
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2005 NURI Telecom Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	Cylcic Redundancy Check (CRC).
//	1 + x + x^5 + x^12 + x^16 is irreducible polynomial.
//
//	casir@com.ne.kr
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __IF4_CRC_CHECK_H__
#define __IF4_CRC_CHECK_H__

WORD	IF4GetCrc(BYTE *pszBuffer, int nLength, int nOffset=0);
BOOL	IF4VerifyCrc(BYTE *pszBuffer, int nLength, int nOffset=0);

#endif	// __IF4_CRC_CHECK_H__
