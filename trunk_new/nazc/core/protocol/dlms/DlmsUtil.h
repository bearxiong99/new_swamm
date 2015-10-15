//////////////////////////////////////////////////////////////////////
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@paran.com
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __DLMS_UTIL_H__
#define __DLMS_UTIL_H__

#include "dlmsType.h"

BYTE	DlmsGetTagSize(DLMS_TAG *pTag);
int 	DlmsGetTagCount(DLMS_TAG *pList, int nLength);

DLMSVARIABLE * DlmsAllocToken(int nLimit);
void	DlmsGetTagToken(DLMSVARIABLE *pToken, int *nToken, int nMaxToken, DLMS_TAG *pList, int nLength);
int     DlmsSetTagValue(DLMSTAGVALUE * tagBuf, BYTE nType, void *pValue, BYTE nLen);
void DlmsFreeToken(DLMSVARIABLE *pToken, int nCount);

void	DumpTagValue(DLMS_TAG *pList, int nLength);
void	DumpParamValue(DLMS_TAG *pList, int nLength);

void DlmsReqAddress(BYTE * header, HDLC_REQ_ADDR *addr, BYTE *src, UINT *dest, BYTE *ctrl);
void DlmsResAddress(BYTE * header, HDLC_RES_ADDR *addr, UINT *src, BYTE *dest, BYTE *ctrl);

BYTE * getResPayload(DLMS_FRAME * pFrame);
int getResAddressLength(HDLC_RES_ADDR *addr);

void freeToken(DLMSVARIABLE *pToken, int nCount);
#endif // __dlms_util_h__
