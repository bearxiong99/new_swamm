//////////////////////////////////////////////////////////////////////
//
//	CoordinatorCrcCheck.cpp: implementation of the Coordinator crc functions.
//
//	This file is a part of the AIMIR-GCP.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "codiapi.h"
#include "CoordinatorCrcCheck.h"
#include "CRC16.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Crc Check Functions
//////////////////////////////////////////////////////////////////////

BOOL VerifyCrcCheck(unsigned char *pszStream, int nLength)
{
	CODI_GENERAL_FRAME	*pFrame;
	WORD nCrc, nGenCrc;
	int		ptr, len;

	pFrame  = (CODI_GENERAL_FRAME *)pszStream;
	ptr 	= pFrame->hdr.len + sizeof(CODI_HEADER);
	nCrc 	= (pszStream[ptr+1] & 0xff) << 8;
	nCrc   += (pszStream[ptr] & 0xff); 

	len		= pFrame->hdr.len + sizeof(CODI_HEADER) - sizeof(CODI_IDENT);
	nGenCrc	= GetCrc16(pszStream+sizeof(CODI_IDENT), len);	

	if (nCrc != nGenCrc)
	{
		XDEBUG("CRC: Length=%d, CrcLength=%d, GenCrc=0x%04X, Crc=0x%04X\xd\xa",
				nLength, len,
				nGenCrc, nCrc);
	}

	return (nCrc == nGenCrc) ? TRUE : FALSE;
}

void SetFrameCrc(unsigned char *pszStream)
{
	CODI_GENERAL_FRAME	*pFrame;
	unsigned int	nCrc;
	unsigned char	nLength;

	pFrame = (CODI_GENERAL_FRAME *)pszStream;
	nLength	= pFrame->hdr.len + sizeof(CODI_HEADER) - sizeof(CODI_IDENT);
	nCrc 	= GetCrc16(pszStream+sizeof(CODI_IDENT), nLength);

	nLength += sizeof(CODI_IDENT);
	pszStream[nLength] 		= (nCrc & 0xff);
	pszStream[nLength+1] 	= (nCrc >> 8);
}

