//////////////////////////////////////////////////////////////////////
//
//	DlmsTcpClient.cpp: implementation of the CDlmsTcpClient class.
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

#include <string.h>
#include "common.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DlmsUtil.h"
#include "DebugUtil.h"

#include "codeUtility.h"
#include "dlmsUtils.h"

#define USE_TAG_MALLOC
//////////////////////////////////////////////////////////////////////
// CDlmsTcpClient Construction/Destruction
//////////////////////////////////////////////////////////////////////

BYTE DlmsGetTagSize(DLMS_TAG *pTag)
{
    switch(pTag->tag) {
        /** NULL 다음에 Length가 오는 것으로 수정 */
	  case DLMS_DATATYPE_NULL :
	  case DLMS_DATATYPE_ARRAY :
	  case DLMS_DATATYPE_COMPACT_ARRAY :
           if(pTag->len & 0x80)
           {
               /** MSB가 1이면 0x7F mask 한 만큼의 data들이 length를 나타낸다
                 */
               return 2 + (pTag->len & 0x7F);
           }

		   return 2;

	  case DLMS_DATATYPE_STRUCTURE :
      case DLMS_DATATYPE_ENUM :
	  case DLMS_DATATYPE_GROUP :
		   return 2;

      case DLMS_DATATYPE_BIT_STRING :
           return (int)((pTag->len+7)/8) + 2;

      case DLMS_DATATYPE_OCTET_STRING :
      case DLMS_DATATYPE_VISIBLE_STRING :
		   return pTag->len + 2;

      case DLMS_DATATYPE_BCD :
      case DLMS_DATATYPE_BOOLEAN :
      case DLMS_DATATYPE_INT8 :
      case DLMS_DATATYPE_UINT8 :
		   return 2;

      case DLMS_DATATYPE_INT16 :
      case DLMS_DATATYPE_UINT16 :
		   return 3;

      case DLMS_DATATYPE_INT32 :
      case DLMS_DATATYPE_UINT32 :
      case DLMS_DATATYPE_FLOAT32 :
		   return 5;

      case DLMS_DATATYPE_INT64 :
      case DLMS_DATATYPE_UINT64 :
		   return 9;

	  case DLMS_DATATYPE_DATE_TIME :
		   return 13;
	
	  case DLMS_DATATYPE_DATE :
		   return 6;
	 
	  case DLMS_DATATYPE_TIME :
		   return 5;
	}
	return 1;
}

int DlmsGetTagCount(DLMS_TAG *pList, int nLength)
{
	DLMS_TAG	*pTag;
	BYTE		*pSeek;
	int			i, nSeek, n=0;

	pSeek = (BYTE *)pList;
	for(i=0, n=0, nSeek=0; i<nLength; i+=nSeek)
	{
		pTag = (DLMS_TAG *)(pSeek+i);
	    switch(pTag->tag) {
		  case DLMS_DATATYPE_ARRAY :
		  case DLMS_DATATYPE_COMPACT_ARRAY :
		  case DLMS_DATATYPE_STRUCTURE :
	      case DLMS_DATATYPE_BOOLEAN :
	      case DLMS_DATATYPE_BIT_STRING :
	      case DLMS_DATATYPE_INT32 :
	      case DLMS_DATATYPE_UINT32 :
	      case DLMS_DATATYPE_OCTET_STRING :
	      case DLMS_DATATYPE_VISIBLE_STRING :
	      case DLMS_DATATYPE_BCD :
	      case DLMS_DATATYPE_INT8 :
	      case DLMS_DATATYPE_INT16 :
	      case DLMS_DATATYPE_UINT8 :
	      case DLMS_DATATYPE_UINT16 :
  	      case DLMS_DATATYPE_INT64 :
	      case DLMS_DATATYPE_UINT64 :
		  case DLMS_DATATYPE_FLOAT32 :
		  case DLMS_DATATYPE_DATE_TIME :
		  case DLMS_DATATYPE_DATE :
		  case DLMS_DATATYPE_TIME :
  	      case DLMS_DATATYPE_NULL :
		  case DLMS_DATATYPE_GROUP :
          case DLMS_DATATYPE_ENUM :
			   n++;
			   break;
		  default :
			   break;
	    }

		nSeek = DlmsGetTagSize(pTag);   
	}
	return n;
}

int DlmsSetTagValue(DLMSTAGVALUE * tagBuf, BYTE nType, void *pValue, BYTE nLen)
{
    WORD wv;
    UINT uv;
    unsigned long long int uuv;

    if(tagBuf == NULL) return DLMSERR_INVALID_PARAM;

    tagBuf->pPoint[tagBuf->nLength++] = nType;
	switch(nType) {
	  	  case DLMS_DATATYPE_NULL :
               break;
		  case DLMS_DATATYPE_ARRAY :
		  case DLMS_DATATYPE_COMPACT_ARRAY :
		  case DLMS_DATATYPE_STRUCTURE :
		  case DLMS_DATATYPE_GROUP :
          case DLMS_DATATYPE_ENUM :
          case DLMS_DATATYPE_BCD :
          case DLMS_DATATYPE_BOOLEAN :
          case DLMS_DATATYPE_INT8 :
          case DLMS_DATATYPE_UINT8 :
               if(pValue == NULL) return DLMSERR_INVALID_PARAM;
               tagBuf->pPoint[tagBuf->nLength++] = *(BYTE *)pValue;
			   break;
	      case DLMS_DATATYPE_BIT_STRING :
               tagBuf->pPoint[tagBuf->nLength++] = nLen;
               if(nLen > 0) 
               {
                   if(pValue == NULL) return DLMSERR_INVALID_PARAM;
                   memcpy(&tagBuf->pPoint[tagBuf->nLength], pValue, (int)((nLen+7)/8));
                   tagBuf->nLength += (int)((nLen+7)/8);
               }
               break;
	      case DLMS_DATATYPE_OCTET_STRING :
	      case DLMS_DATATYPE_VISIBLE_STRING :
               tagBuf->pPoint[tagBuf->nLength++] = nLen;
               if(nLen > 0) 
               {
                   if(pValue == NULL) return DLMSERR_INVALID_PARAM;
                   memcpy(&tagBuf->pPoint[tagBuf->nLength], pValue, nLen);
                   tagBuf->nLength += nLen;
               }
			   break;
		  case DLMS_DATATYPE_DATE_TIME :
               if(nLen != sizeof(DLMSTIMEDATE)) return DLMSERR_INVALID_PARAM;
               memcpy(&tagBuf->pPoint[tagBuf->nLength], pValue, nLen);
               tagBuf->nLength += nLen;
			   break;
		  case DLMS_DATATYPE_DATE :
               if(nLen != sizeof(DLMSDATE)) return DLMSERR_INVALID_PARAM;
               memcpy(&tagBuf->pPoint[tagBuf->nLength], pValue, nLen);
               tagBuf->nLength += nLen;
			   break;
		  case DLMS_DATATYPE_TIME :
               if(nLen != sizeof(DLMSTIME)) return DLMSERR_INVALID_PARAM;
               memcpy(&tagBuf->pPoint[tagBuf->nLength], pValue, nLen);
               tagBuf->nLength += nLen;
			   break;
	      case DLMS_DATATYPE_INT16 :
	      case DLMS_DATATYPE_UINT16 :
               if(nLen != sizeof(WORD)) return DLMSERR_INVALID_PARAM;
               wv = dlmsOrderByShort(*(WORD *)pValue);
               memcpy(&tagBuf->pPoint[tagBuf->nLength], (BYTE *)&wv, nLen);
               tagBuf->nLength += nLen;
			   break;
	      case DLMS_DATATYPE_INT32 :
	      case DLMS_DATATYPE_UINT32 :
	      case DLMS_DATATYPE_FLOAT32 :
               if(nLen != sizeof(UINT)) return DLMSERR_INVALID_PARAM;
               uv = dlmsOrderByInt(*(UINT *)pValue);
               memcpy(&tagBuf->pPoint[tagBuf->nLength], (BYTE *)&uv, nLen);
               tagBuf->nLength += nLen;
			   break;
  	      case DLMS_DATATYPE_INT64 :
	      case DLMS_DATATYPE_UINT64 :
               if(nLen != 8) return DLMSERR_INVALID_PARAM;
               uuv = dlmsOrderByInt64(*(unsigned long long int *)pValue);
               memcpy(&tagBuf->pPoint[tagBuf->nLength], (BYTE *)&uv, nLen);
               tagBuf->nLength += nLen;
			   break;
		  default :
               XDEBUG(" \033[1;40;31mUNKNOWN DLMS TOKEN 0x%02X\033[0m\r\n", nType);
               return DLMSERR_INVALID_PARAM;
	    }
  
    return DLMSERR_NOERROR;
}

void DlmsGetTagToken(DLMSVARIABLE *pToken, int *nToken, int nMaxToken, DLMS_TAG *pList, int nLength)
{
	DLMSVARIABLE *pValue;
	DLMS_TAG	*pTag;
	DLMS_ARG	*pArg;
	BYTE		*pSeek;
    BYTE        *pPoint;
	int			i, n, nSeek, len;

	if ((pList == NULL) || (nLength <= 0))
		return;

	if ((pToken == NULL) || (nToken == NULL))
		return;

	*nToken = 0;
	pSeek = (BYTE *)pList;
    pPoint = pSeek;
	for(i=0, n=0, nSeek=0; i<nLength; i+=nSeek)
	{
        pPoint = pSeek + i;
		pTag   = (DLMS_TAG *)(pPoint);
		pArg   = (DLMS_ARG *)(pPoint);

		pValue = &pToken[n];
		pValue->type = pTag->tag;

	    switch(pTag->tag) {
	  	  case DLMS_DATATYPE_NULL :
              /*
			   pValue->len 			= 0;
               break;
               */
		  case DLMS_DATATYPE_ARRAY :
		  case DLMS_DATATYPE_COMPACT_ARRAY :
               len = 0;
               if(pArg->var.array & 0x80)
               {
                   len = pArg->var.array & 0x7F;
               }

			   pValue->len 			= 1;
			   pValue->var.array	= *(&pArg->var.array + len);
               //XDEBUG(" DLMS Token [%02d] : ARRAY %d\r\n", n, pValue->var.array);
			   break;

		  case DLMS_DATATYPE_STRUCTURE :
		  case DLMS_DATATYPE_GROUP :
          case DLMS_DATATYPE_ENUM :
			   pValue->len			= 1;
			   pValue->var.u8		= pArg->var.u8;
               //XDEBUG(" DLMS Token [%02d][%d] : STRUCTURE\r\n", n, pArg->var.u8);
			   break;

	      case DLMS_DATATYPE_BIT_STRING :
			   pValue->len			= pTag->len;
#if defined(USE_TAG_MALLOC)
               if(pTag->len > 0)
               {
                   pValue->var.p = (BYTE *) MALLOC((int)((pTag->len+7)/8));
                   memcpy(pValue->var.p, pTag->data, (int)((pTag->len+7)/8)); 
               }
               else
               {
                   pValue->var.p = NULL;
               }
#else
               pValue->var.p        = (BYTE *)&pTag->data;
#endif
               //XDEBUG(" DLMS Token [%02d] : BIT-STRING\r\n", n);
               break;

	      case DLMS_DATATYPE_OCTET_STRING :
	      case DLMS_DATATYPE_VISIBLE_STRING :
			   pValue->len			= pTag->len;
#if defined(USE_TAG_MALLOC)
               if(pTag->len > 0)
               {
                   pValue->var.p = (BYTE *) MALLOC(pTag->len);
                   memcpy(pValue->var.p, pTag->data, pTag->len); 
               }
               else
               {
                   pValue->var.p = NULL;
               }
#else
			   pValue->var.p		= (BYTE *)&pTag->data;
#endif
               //XDEBUG(" DLMS Token [%02d] : STRING\r\n", n);
			   break;

		  case DLMS_DATATYPE_DATE_TIME :
			   pValue->len			= sizeof(DLMSTIMEDATE);
			   memcpy(&pValue->var.td, (BYTE *)&pArg->var.array, pValue->len);
               //XDEBUG(" DLMS Token [%02d] : DATE TIME\r\n", n);
			   break;

		  case DLMS_DATATYPE_DATE :
			   pValue->len			= sizeof(DLMSDATE);
			   memcpy(&pValue->var.d, (BYTE *)&pArg->var.array, pValue->len);
               //XDEBUG(" DLMS Token [%02d] : DATE\r\n", n);
			   break;

		  case DLMS_DATATYPE_TIME :
			   pValue->len			= sizeof(DLMSTIME);
			   memcpy(&pValue->var.t, (BYTE *)&pArg->var.array, pValue->len);
               //XDEBUG(" DLMS Token [%02d] : TIME\r\n", n);
			   break;

	      case DLMS_DATATYPE_BCD :
			   pValue->len			= 1;
			   pValue->var.u8		= (((pArg->var.u8 & 0xf0) >> 4) * 10) + (pArg->var.u8 & 0x0f);
               //XDEBUG(" DLMS Token [%02d] : BCD\r\n", n);
			   break;

	      case DLMS_DATATYPE_BOOLEAN :
	      case DLMS_DATATYPE_INT8 :
               pValue->len          = 1;
               pValue->var.i8       = pArg->var.i8;
               //XDEBUG(" DLMS Token [%02d] : INT8\r\n", n);
               break;

	      case DLMS_DATATYPE_UINT8 :
			   pValue->len			= 1;
			   pValue->var.u8		= pArg->var.u8;
               //XDEBUG(" DLMS Token [%02d] : UINT8\r\n", n);
			   break;

	      case DLMS_DATATYPE_INT16 :
	      case DLMS_DATATYPE_UINT16 :
			   pValue->len			= 2;
			   pValue->var.u16		= (unsigned short)dlmsOrderByShort(pArg->var.u16);
               //XDEBUG(" DLMS Token [%02d] : UINT16\r\n", n);
			   break;

	      case DLMS_DATATYPE_INT32 :
	      case DLMS_DATATYPE_UINT32 :
	      case DLMS_DATATYPE_FLOAT32 :
			   pValue->len			= 4;
			   pValue->var.u32		= (unsigned int)dlmsOrderByInt(pArg->var.u32);
               //XDEBUG(" DLMS Token [%02d] : UINT32\r\n", n);
			   break;

  	      case DLMS_DATATYPE_INT64 :
	      case DLMS_DATATYPE_UINT64 :
			   pValue->len			= 8;
			   pValue->var.u64		= (unsigned long long int)dlmsOrderByInt64(pArg->var.u64);
               //XDEBUG(" DLMS Token [%02d] : UINT64\r\n", n);
			   break;

		  default :
               XDEBUG(" \033[1;40;31mUNKNOWN DLMS TOKEN 0x%02X\033[0m\r\n", pTag->tag);
			   //*nToken = n;
               n--;
			   break;
	    }
		n++;
		if (n >= nMaxToken)
			break;
  
		nSeek = DlmsGetTagSize(pTag);   
	}

	*nToken = n;
}

void DumpTagValue(DLMS_TAG *pList, int nLength)
{
	DLMSTIMEDATE	*pTimeDate;
	DLMSDATE		*pDate;
	DLMSTIME		*pTime;
	DLMS_TAG		*pTag;
	DLMS_ARG		*pArg;
	BYTE			*pSeek;
	int				i, nSeek, n=0, len;

    /** 성능 향상을 위해 Debug Mode가 Enable 되어 있지 않으면 바로 Return 한다 */
    if(!IS_DEBUG()) return;

	pSeek = (BYTE *)pList;
	for(i=0, n=0, nSeek=0; i<nLength; i+=nSeek)
	{
		pTag = (DLMS_TAG *)(pSeek+i);
		pArg = (DLMS_ARG *)(pSeek+i);

	    switch(pTag->tag) {
		  case DLMS_DATATYPE_ARRAY :
               len = 0;
               if(pArg->var.array & 0x80)
               {
                   len = pArg->var.array & 0x7F;
               }
			   XDEBUG(" %02d: ARRAY(%d), length=1, value=%d\r\n",
							n+1, pTag->tag, *(&pArg->var.array + len));
			   break;
		  case DLMS_DATATYPE_COMPACT_ARRAY :
               len = 0;
               if(pArg->var.array & 0x80)
               {
                   len = pArg->var.array & 0x7F;
               }
			   XDEBUG(" %02d: COMPACT-ARRAY(%d), length=1, value=%d\r\n",
							n+1, pTag->tag, *(&pArg->var.array + len));
			   break;
		  case DLMS_DATATYPE_STRUCTURE :
			   XDEBUG(" %02d: STRUCTURE(%d), length=1, value=%d\r\n", n+1, pTag->tag, pArg->var.u8);
			   break;
		  case DLMS_DATATYPE_GROUP :
			   XDEBUG(" %02d: GROUP(%d), length=1, groupLen=%d\r\n", n+1, pTag->tag, pTag->len);
			   break;
	      case DLMS_DATATYPE_BOOLEAN :
			   XDEBUG(" %02d: BOOLEAN(%d), length=1, value=%s(0x%02X)\r\n", n+1, pTag->tag,
						pArg->var.u8 == 0 ? "FALSE" : "TRUE", pArg->var.u8);
			   break;
          case DLMS_DATATYPE_ENUM :
			   XDEBUG(" %02d: ENUM(%d), length=1, value=%d\r\n", n+1, pTag->tag, pArg->var.u8);
			   break;
	      case DLMS_DATATYPE_BIT_STRING :
			   XDEBUG(" %02d: BIT-STRING(%d), length=%d, value=", n+1, pTag->tag, pTag->len);
			   XDUMP((char *)pTag->data, (int)((pTag->len+7)/8), FALSE);
			   break;
	      case DLMS_DATATYPE_INT32 :
			   XDEBUG(" %02d: INT32(%d), length=4, value=%d\r\n", n+1, pTag->tag,
						(signed int)dlmsOrderByInt(pArg->var.i32));
			   break;
	      case DLMS_DATATYPE_UINT32 :
			   XDEBUG(" %02d: UINT32(%d), length=4, value=%d\r\n", n+1, pTag->tag, 
						(unsigned int)dlmsOrderByInt(pArg->var.u32));
			   break;
	      case DLMS_DATATYPE_FLOAT32 :
			   XDEBUG(" %02d: FLOAT32(%d), length=4, value=%f\r\n", n+1, pTag->tag, 
						(double)dlmsOrderByInt(pArg->var.u32));
			   break;
	      case DLMS_DATATYPE_OCTET_STRING :
			   XDEBUG(" %02d: OCTET-STRING(%d), length=%d, value=", n+1, pTag->tag, pTag->len);
			   XDUMP((char *)pTag->data, pTag->len, FALSE);
			   break;
	      case DLMS_DATATYPE_VISIBLE_STRING :
			   XDEBUG(" %02d: VISIBLE-STRING(%d), length=%d, value=\r\n", n+1, pTag->tag, pTag->len);
			   XDUMP((char *)pTag->data, pTag->len, TRUE);
			   break;
	      case DLMS_DATATYPE_BCD :
			   XDEBUG(" %02d: BCD(%d), length=1, value=%d\r\n", n+1, pTag->tag,
						(((pArg->var.u8 & 0xf0) >> 4) * 10) + (pArg->var.u8 & 0x0f));
			   break;
	      case DLMS_DATATYPE_INT8 :
			   XDEBUG(" %02d: INT8(%d), length=1, value=%d\r\n", n+1, pTag->tag, (signed char)pArg->var.i8);
			   break;
	      case DLMS_DATATYPE_INT16 :
			   XDEBUG(" %02d: INT16(%d), length=2, value=%d\r\n", n+1, pTag->tag,
						(signed short)dlmsOrderByShort(pArg->var.i16));
			   break;
	      case DLMS_DATATYPE_UINT8 :
			   XDEBUG(" %02d: UINT8(%d), length=1, value=%d\r\n", n+1, pTag->tag, pArg->var.u8);
			   break;
	      case DLMS_DATATYPE_UINT16 :
			   XDEBUG(" %02d: UINT16(%d), length=2, value=%d\r\n", n+1, pTag->tag,
						(unsigned short)dlmsOrderByShort(pArg->var.u16));
			   break;
  	      case DLMS_DATATYPE_INT64 :
			   XDEBUG(" %02d: INT64(%d), length=8\r\n", n+1, pTag->tag);
			   break;
	      case DLMS_DATATYPE_UINT64 :
			   XDEBUG(" %02d: UINT64(%d), length=8\r\n", n+1, pTag->tag);
			   break;
	      case DLMS_DATATYPE_DATE_TIME :
			   pTimeDate = (DLMSTIMEDATE *)&pArg->var.array;
			   XDEBUG(" %02d: DATETIME(%d), length=%d, value=%04d/%02d/%02d %02d:%02d:%02d.%02d (%d)\r\n", 
						n+1, pTag->tag, sizeof(DLMSTIMEDATE),
						dlmsOrderByShort(pTimeDate->year), pTimeDate->mon, pTimeDate->day,
						pTimeDate->hour, pTimeDate->min, pTimeDate->sec, pTimeDate->hsec,
						pTimeDate->week);
			   break;
		  case DLMS_DATATYPE_DATE :
			   pDate = (DLMSDATE *)&pArg->var.array;
			   XDEBUG(" %02d: DATE(%d), length=%d, value=%04d/%02d/%02d (%d)\r\n", 
						n+1, pTag->tag, sizeof(DLMSDATE),
						dlmsOrderByShort(pDate->year), pDate->mon, pDate->day, pDate->week);
			   break;
		  case DLMS_DATATYPE_TIME :
			   pTime = (DLMSTIME *)&pArg->var.array;
			   XDEBUG(" %02d: TIME(%d), length=%d, value=%02d:%02d:%02d.%02d\r\n",
						n+1, pTag->tag, sizeof(DLMSTIME),
						pTime->hour, pTime->min, pTime->sec, pTime->hsec);
			   break;
  	      case DLMS_DATATYPE_NULL :
			   XDEBUG(" %02d: NULL(%d), length=0\r\n", n+1, pTag->tag);
			   break;
		  default :
			   XDEBUG(" %02d: UNKNOWN(%d)\r\n", n+1, pTag->tag);
			   break;
	    }

		nSeek = DlmsGetTagSize(pTag);   
		n++;
	}
}

void DumpParamValue(DLMS_TAG *pList, int nLength)
{
	DLMS_TAG	*pTag;
	BYTE		*pSeek;
	int			i, nSeek, n=0;

    /** 성능 향상을 위해 Debug Mode가 Enable 되어 있지 않으면 바로 Return 한다 */
    if(!IS_DEBUG()) return;

	pSeek = (BYTE *)pList;
	for(i=0, n=0, nSeek=0; i<nLength; i+=nSeek)
	{
		pTag = (DLMS_TAG *)(pSeek+i);
	    XDEBUG(" %02d: TYPE=0x%02X, length=%d, value=", n+1, pTag->tag, pTag->len);
	    XDUMP((char *)pTag->data, pTag->len, FALSE);
		nSeek = pTag->len + 2;   
		n++;
	}
}

/** Conformance Value dump.
 *
 * @param securityType DLMS_ASSOCIATION_LN, DLMS_ASSOCIATION_SN
 * @param conformance Conformance value
 */
void dlmsDumpConformanceValue(BYTE associationType, UINT conformance) 
{
    if(associationType == DLMS_ASSOCIATION_LN) {
        XDEBUG(" LN : ");
        if(conformance & CONF_LN_ATTR0_SUPPORTED_SET) XDEBUG("attribute0-supported-with-set ");
        if(conformance & CONF_LN_PRORITY_MGMT_SUPPORTED) XDEBUG("prority-mgmt-supported ");
        if(conformance & CONF_LN_ATTR0_SUPPORTED_GET) XDEBUG("attribute0-supported-with-get ");
        if(conformance & CONF_LN_BLOCK_TRANSFER_GET) XDEBUG("block-transfer-with-get ");
        if(conformance & CONF_LN_BLOCK_TRANSFER_SET) XDEBUG("block-transfer-with-set ");
        if(conformance & CONF_LN_BLOCK_TRANSFER_ACTION) XDEBUG("block-transfer-with-action ");
        if(conformance & CONF_LN_MULTIPLE_REFERENCES) XDEBUG("multiple-references ");
        if(conformance & CONF_LN_GET) XDEBUG("get ");
        if(conformance & CONF_LN_SET) XDEBUG("set ");
        if(conformance & CONF_LN_SELECTIVE_ACCESS) XDEBUG("selective-access ");
        if(conformance & CONF_LN_EVENT_NOTIFICATION) XDEBUG("event-notification ");
        if(conformance & CONF_LN_ACTION) XDEBUG("action ");
    } else {
        XDEBUG(" SN : ");
        if(conformance & CONF_SN_READ) XDEBUG("read ");
        if(conformance & CONF_SN_WRITE) XDEBUG("write ");
        if(conformance & CONF_SN_UNCONFIRMED_WRITE) XDEBUG("unconfirmed-write ");
        if(conformance & CONF_SN_MULTIPLE_REFERENCES) XDEBUG("multiple-references ");
        if(conformance & CONF_SN_INFORMATION_REPORT) XDEBUG("information-report ");
        if(conformance & CONF_SN_PARAMETERIZED_ACCESS) XDEBUG("parameterized-access ");
    }
    XDEBUG("\r\n");
}

void DlmsReqAddress(BYTE * header, HDLC_REQ_ADDR *addr, BYTE *src, UINT *dest, BYTE *ctrl) 
{
    if(!addr || !header) return;

    if((addr->d1.dest & 0x01) == 0x01) {
        if(dest) *dest = (UINT)(addr->d1.dest & 0xFF);
        if(src) *src = addr->d1.src;
        if(ctrl) *ctrl = header[3 + 2];
    }else if((dlmsOrderByShort(addr->d2.dest) & 0x0001) == 0x0001) {
        if(dest) *dest = (UINT)(dlmsOrderByShort(addr->d2.dest) & 0xFFFF);
        if(src) *src = addr->d2.src;
        if(ctrl) *ctrl = header[3 + 3];
    } else {
        if(dest) *dest = (UINT)(dlmsOrderByInt(addr->d4.dest) & 0xFFFFFFFF);
        if(src) *src = addr->d4.src;
        if(ctrl) *ctrl = header[3 + 5];
    }
}

void DlmsResAddress(BYTE * header, HDLC_RES_ADDR *addr, UINT *src, BYTE *dest, BYTE *ctrl) 
{
    if(!addr || !header) return;

    if((addr->s1.src & 0x01) == 0x01) {
        if(src) *src = (UINT)(addr->s1.src & 0xFF);
        if(dest) *dest = addr->s1.dest;
        if(ctrl) *ctrl = header[3 + 2];
    } else if((dlmsOrderByShort(addr->s2.src) & 0x0001) == 0x0001) {
        if(src) *src = (UINT)(dlmsOrderByShort(addr->s2.src) & 0xFFFF);
        if(dest) *dest = addr->s2.dest;
        if(ctrl) *ctrl = header[3 + 3];
    } else  {
        if(src) *src = (UINT)(dlmsOrderByInt(addr->s4.src) & 0xFFFFFFFF);
        if(dest) *dest = addr->s4.dest;
        if(ctrl) *ctrl = header[3 + 5];
    }
}

int getResAddressLength(HDLC_RES_ADDR *addr) 
{
    if(!addr) return 0;

    if((addr->s1.src & 0x01) == 0x01) {
        return 2;
    }
    if((dlmsOrderByShort(addr->s2.src) & 0x0001) == 0x0001) {
        return 3;
    }
    return 5;
}

BYTE * getResPayload(DLMS_FRAME * pFrame) 
{
    BYTE * pszBuffer = (BYTE *)pFrame;
    int len = sizeof(DLMS_HEADER); // header

    //XDEBUG("getResAddressLength=%d\r\n", getResAddressLength((HDLC_RES_ADDR *)&(pFrame->hdr.addr)));
    len -= 5 - getResAddressLength((HDLC_RES_ADDR *)&(pFrame->hdr.addr));
    return pszBuffer + len;
}

unsigned short dlmsOrderByShort(unsigned short value)
{
	return BigToHostShort(value);
}

unsigned int dlmsOrderByInt(unsigned int value)
{
	return BigToHostInt(value);
}

unsigned long long int dlmsOrderByInt64(unsigned long long int value)
{
	BYTE	*p;
	unsigned int			*high, *low;
	unsigned long long int	v;

	p 	 = (BYTE *)&value;
	high = (unsigned int *)p;
	low  = (unsigned int *)&p[4];

	v = (BigToHostInt(*low) << 16) + BigToHostInt(*high);
	return v;
}

int dlmsGetTokenPointer(DLMSVARIABLE *pToken, BYTE dlmsType, int nStart, int nMax)
{
    int i;

    for(i=nStart; i < nMax; i++) 
    {
        if(pToken[i].type == dlmsType)
        {
            return i;
        }
    }

    return nMax;
}

DLMSVARIABLE * DlmsAllocToken(int nLimit)
{
    return (DLMSVARIABLE *)MALLOC(sizeof(DLMSVARIABLE)* nLimit);
}

void freeToken(DLMSVARIABLE *pToken, int nCount)
{
#if defined(USE_TAG_MALLOC)
    int i;

    for(i=0; i < nCount; i++) 
    {
        switch(pToken[i].type)
        {
	        case DLMS_DATATYPE_BIT_STRING :
	        case DLMS_DATATYPE_OCTET_STRING :
	        case DLMS_DATATYPE_VISIBLE_STRING :
                if(pToken[i].var.p) 
                {
                    FREE(pToken[i].var.p);
                    pToken[i].var.p = NULL;
                }
                break;
        }
    }
#endif
}

void DlmsFreeToken(DLMSVARIABLE *pToken, int nCount)
{
#if defined(USE_TAG_MALLOC)
    freeToken(pToken, nCount);
#endif
    FREE(pToken);
}

//////////////////////////////////////////////////////////////////////////////////


const char *dlmsGetHdlcControlType(BYTE nCtrl)
{
	switch(nCtrl) {
	  case HDLC_CTRL_I :		return "I";
	  case HDLC_CTRL_RR :		return "RR";
	  case HDLC_CTRL_UI :		return "UI";
	  case HDLC_CTRL_RNR :		return "RNR";
	  case HDLC_CTRL_DM :		return "DM";
	  case HDLC_CTRL_AARE :		return "AARE";
	  case HDLC_CTRL_SCRIPT :	return "SCRIPT";
	  case HDLC_CTRL_DISC :		return "DISC";
	  case HDLC_CTRL_UA :		return "UA";
	  case HDLC_CTRL_SNRM :		return "SNRM";
	  case HDLC_CTRL_FRMR :		return "FRMR";
	}
	return "UNKNOWN";
}

const char *dlmsGetDataType(BYTE nType)
{
	switch(nType) {
	  case DLMS_DATATYPE_NULL :				return "NULL";
	  case DLMS_DATATYPE_ARRAY :			return "ARRAY";
	  case DLMS_DATATYPE_STRUCTURE :		return "STRUCTURE";
 	  case DLMS_DATATYPE_BOOLEAN :			return "BOOLEAN";
	  case DLMS_DATATYPE_BIT_STRING :		return "BIT-STRING";
	  case DLMS_DATATYPE_INT32 :			return "INT32";
	  case DLMS_DATATYPE_UINT32 :			return "UINT32";
 	  case DLMS_DATATYPE_OCTET_STRING :		return "OCTET-STRING";
 	  case DLMS_DATATYPE_VISIBLE_STRING :   return "VISIBLE-STRING";
 	  case DLMS_DATATYPE_BCD :				return "BCD";
 	  case DLMS_DATATYPE_INT8 :				return "INT8";
 	  case DLMS_DATATYPE_INT16 :			return "INT16";
 	  case DLMS_DATATYPE_UINT8 :			return "UINT8";
	  case DLMS_DATATYPE_UINT16 :			return "UINT16";
 	  case DLMS_DATATYPE_INT64 :			return "INT64";
 	  case DLMS_DATATYPE_UINT64 :			return "UINT64";
	  case DLMS_DATATYPE_COMPACT_ARRAY :	return "COMPACT-ARRAY";
	  case DLMS_DATATYPE_FLOAT32 :			return "FLOAT32";
	  case DLMS_DATATYPE_DATE_TIME :		return "DATE-TIME";
	  case DLMS_DATATYPE_DATE :				return "DATE";
	  case DLMS_DATATYPE_TIME :				return "TIME";
	}
	return "UNKNOWN";
}

const char *dlmsGetMethodType(BYTE nType)
{
	switch(nType) {
	  case DLMS_GET :		return "GET";
	  case DLMS_SET :		return "SET";
	  case DLMS_ACTION :	return "ACTION";
	}
	return "UNKNOWN";
}

const char *dlmsGetFlowType(BYTE nFlow)
{
	switch(nFlow) {
	  case DLMS_REQUEST :	return "request";
	  case DLMS_RESPONSE :	return "response";
	}
	return "unknown";
}

const char *dlmsGetResType(BYTE nType)
{
	switch(nType) {
	  case DLMS_NORMAL :	return "normal";
	  case DLMS_DATABLOCK :	return "datablock";
	  case DLMS_LIST :		return "list";
	}
	return "unknown";
}

const char * dlmsGetInterfaceClass(BYTE cls)
{
    switch(cls)
    {
        case  1: return "Data";
        case  3: return "Register";
        case  4: return "Extended register";
        case  5: return "Demand register";
        case  6: return "Register activation";
        case  7: return "Profile generic";
        case  8: return "Clock";
        case  9: return "Script table";
        case 10: return "Schedule";
        case 11: return "Special days table";
        case 20: return "Activity calendar";
        case 15: return "Association LN";
        case 12: return "Association SN";
        case 17: return "SAP assignment";
        case 21: return "Register monitor";
        case 26: return "Utility tables";
        case 22: return "Single action schedule";
        case 61: return "Register table";
        case 63: return "Status mapping";
        case 19: return "IEC local port setup";
        case 27: return "Modem configuration";
        case 28: return "Auto answer";
        case 29: return "Auto connect";
        case 23: return "IEC HDLC setup";
        case 24: return "IEC twisted pair setup";
        case 25: return "M-Bus port stup";
        case 41: return "TCP-UDP setup";
        case 42: return "IPv4 setup";
        case 43: return "Ethernet setup";
        case 44: return "PPP setup";
        case 45: return "GPRS modem setup";
        case 46: return "SMTP setup";
    }
    return "UNKNOWN";
}

const char * dlmsGetResponseCode(BYTE code) 
{
    switch(code) {
        case DLMS_RES_SUCCESS: return "Success";
        case DLMS_RES_HARDWARE_FAULT: return "Hardware Fault";
        case DLMS_RES_TEMPORARY_FAILURE: return "Temporary Failure";
        case DLMS_RES_READ_WRITE_DENIED: return "Read Write Denied";
        case DLMS_RES_OBJECT_UNDEFINED: return "Object Undefined";
        case DLMS_RES_OBJECT_CLASS_INCONSISTENT: return "Class Inconsistent";
        case DLMS_RES_OBJECT_UNAVAILABLE: return "Object Unavailable";
        case DLMS_RES_TYPE_UNMATCHED: return "Type Unmatched";
        case DLMS_RES_SCOPE_OF_ACCESS_VIOLATED: return "Scope of Access Violated";
        case DLMS_RES_DATA_BLOCK_UNAVAILABLE: return "Data Block Unavailable";
        case DLMS_RES_LONG_GET_ABORTED: return "Long Get Aborted";
        case DLMS_RES_NO_LONG_GET_IN_PROGRESS: return "No Long Get In Progress";
        case DLMS_RES_LONG_SET_ABORTED: return "Long Set Aborted";
        case DLMS_RES_NO_LONG_SET_IN_PROGRESS: return "No Long Set In Progress";
        case DLMS_RES_OTHER_REASON: return "Other Reason";
    }
    return "UNKNOWN";
}

const char * dlmsGetUnit(BYTE unit)
{
    return GetUnitStr(unit);
}

const char *dlmsGetErrorMessage(int nError)
{
	switch(nError) {
	  case DLMSERR_NOERROR :				return "OK";
	  case DLMSERR_INVALID_PARAM :			return "Invalid param";
	  case DLMSERR_INVALID_SESSION :		return "Invalid session";
	  case DLMSERR_INVALID_HANDLE :			return "Invalid handle";
	  case DLMSERR_INVALID_ADDRESS :		return "Invalid address";
	  case DLMSERR_INVALID_PORT :			return "Invalid port";
	  case DLMSERR_INVALID_SOURCE_ADDR :	return "Invalid source address";	
	  case DLMSERR_INVALID_DEST_ADDR :		return "Invalid destination address";
	  case DLMSERR_INVALID_TIMEOUT :		return "Invalid timeout";
	  case DLMSERR_INVALID_ACCESS :			return "Invalid access";
	  case DLMSERR_INVALID_DATA :			return "Invalid data";
	  case DLMSERR_INVALID_ACTION :			return "Invalid action";
	  case DLMSERR_INVALID_OBISCODE :		return "Invalid obiscode";
	  case DLMSERR_INVALID_STRING :			return "Invalid string";
	  case DLMSERR_INVALID_LENGTH :			return "Invalid length";
	  case DLMSERR_INVALID_BUFFER :			return "Invalid buffer";
	  case DLMSERR_CANNOT_CONNECT :			return "Cannot connect";
	  case DLMSERR_REPLY_TIMEOUT :			return "Reply timeout";
	  case DLMSERR_NOT_INITIALIZED :		return "Not initialized";
	  case DLMSERR_SYSTEM_ERROR :			return "System error";
	  case DLMSERR_MEMORY_ERROR :			return "Memory error";
	}
	return "Unkown";
}

const char *dlmsGetFrameType(BYTE nType)
{
	switch(nType) {
	  case 1 :	return "NORMAL";
	  case 2 :  return "DATABLOCK";
	  case 3 :	return "LIST";
	}
	return "UNKNOWN";
}

int dlmsObisToString(OBISCODE *obis, char *pszString)
{
	if (obis == NULL)
		return DLMSERR_INVALID_OBISCODE;

	if (pszString == NULL)
		return DLMSERR_INVALID_STRING;

	sprintf(pszString, "%02X.%02X.%02X.%02X.%02X.%02X",
			obis->a, obis->b, obis->c, obis->d, obis->e, obis->f);
	return DLMSERR_NOERROR;
}

int dlmsStringToObis(const char *pszString, OBISCODE *obis)
{
	BYTE	*pIndex;
	char	*p;
#ifndef _WIN32
    char    *last=NULL;
#endif
	int		index;

	if (obis == NULL)
		return DLMSERR_INVALID_OBISCODE;

	if (pszString == NULL)
		return DLMSERR_INVALID_STRING;

	memset(obis, 0, sizeof(OBISCODE));
	pIndex = (BYTE *)obis;

	index = 0;
#ifndef _WIN32
	p = strtok_r(const_cast<char *>(pszString), ".", &last);
#else
	p = strtok(const_cast<char *>(pszString), ".");
#endif
	while(p && *p)
	{
		pIndex[index] = (BYTE)strtol(p, (char **)NULL, 10);
		index++; 
		if (index >= (int)sizeof(OBISCODE))
			break;
#ifndef _WIN32
		p = strtok_r(NULL, ".", &last);
#else
		p = strtok(NULL, ".");
#endif
	}
	return DLMSERR_NOERROR;
}

void dlmsDumpValue(BYTE *pszData, int nLength)
{
    DumpTagValue((DLMS_TAG *)pszData, nLength);
}




