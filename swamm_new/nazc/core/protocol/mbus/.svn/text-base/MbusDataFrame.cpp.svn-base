#include "common.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "mbusFrame.h"
#include "mbusDefine.h"

#include "MbusChecksum.h"
#include "MbusDataFrame.h"

int MakeMbusTail(BYTE *pszBuffer, BYTE len)
{
	MBUS_TAIL   *pTail;

	pTail = (MBUS_TAIL *)(pszBuffer + len);
	pTail->checksum = GetMbusChecksum(pszBuffer, len);
	pTail->stop     = MBUS_FRAME_STOP;

    return sizeof(MBUS_TAIL);
}

int MakeMbusShortHeader(MBUS_SHORT_HEADER *pHeader, BYTE ctrl, BYTE addr)
{
	memset(pHeader, 0, sizeof(MBUS_SHORT_HEADER));
	pHeader->start      = MBUS_FRAME_SHORT;
    pHeader->ctrl       = ctrl | MBUS_CTRL_MASK_REQUEST;
    pHeader->addr       = addr;

    return sizeof(MBUS_SHORT_HEADER);
}

int MakeMbusShortFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr)
{
    MBUS_SHORT_FRAME *frame = (MBUS_SHORT_FRAME *)pszBuffer;
    int len = 0;

    len += MakeMbusShortHeader(&frame->hdr, ctrl, addr);
    len += MakeMbusTail(pszBuffer + 1, sizeof(MBUS_SHORT_HEADER) - 1);

    return len;
}

int MakeMbusLongHeader(MBUS_LONG_HEADER *pHeader, BYTE ctrl, BYTE addr, BYTE ci, BYTE dataLen)
{
	memset(pHeader, 0, sizeof(MBUS_LONG_HEADER));
	pHeader->start = pHeader->start2    = MBUS_FRAME_LONG;
    pHeader->len = pHeader->len2        = 3 + dataLen;
    pHeader->ctrl       = ctrl | MBUS_CTRL_MASK_REQUEST;
    pHeader->addr       = addr;
    pHeader->ci         = ci;

    return sizeof(MBUS_LONG_HEADER);
}

int MakeMbusLongFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE ci, BYTE *userData, BYTE dataLen)
{
    MBUS_LONG_FRAME *frame = (MBUS_LONG_FRAME *)pszBuffer;
    int len = 0;
    int dlen = userData ? dataLen : 0;

    len += MakeMbusLongHeader(&frame->hdr, ctrl, addr, ci, dlen);
    if(userData && dataLen > 0)
    {
        memcpy(frame->data, userData, dataLen);
    }
    len += MakeMbusTail(pszBuffer + 4, 3 + dlen);

    return len + dlen;
}

int MakeMbusControlFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE ci)
{
    return MakeMbusLongFrame(pszBuffer, ctrl, addr, ci, NULL, 0);
}

#if 0
int MakeHdlcFrame(BYTE *pszBuffer, BYTE src, WORD dest, BYTE ctrl)
{
	WORD	len;

	// FLAG(1) + HEADER(6) + CRC(2) + FLAG(1) => 10 - 2 = 8
	len = sizeof(HDLC_FRAME) - 2;
	MakeHdlcHeader((HDLC_HEADER *)pszBuffer, src, dest, ctrl, len);
	MakeHdlcTail(pszBuffer, len);
	return sizeof(HDLC_FRAME);
}

void MakeDlmsHeader(DLMS_HEADER *pHeader, BYTE src, WORD dest, BYTE ctrl, WORD len)
{
	memset(pHeader, 0, sizeof(HDLC_HEADER));
	pHeader->startFlag		= HDLC_FLAG;
	pHeader->typeLen		= dlmsOrderByShort((HDLC_FRAME_FORMAT << 8) | (len & 0x07FF));
	pHeader->addr.dest		= dlmsOrderByShort(dest);
	pHeader->addr.src		= src;
	pHeader->ctrl			= ctrl;
//	pHeader->hcs			= dlmsOrderByShort(DlmsGetCrc((BYTE *)&pHeader->typeLen, sizeof(DLMS_HEADER)-3));
	pHeader->hcs			= DlmsGetCrc((BYTE *)&pHeader->typeLen, sizeof(DLMS_HEADER)-3);
}

int MakeDlmsFrame(BYTE *pszBuffer, BYTE src, WORD dest, BYTE ctrl)
{
	WORD		len;

	// FLAG(1) + DLMS_HEADER(8) + CRC(2) + FLAG(1)
	len = sizeof(DLMS_HEADER) + sizeof(DLMS_TAIL) - 2;
	MakeDlmsHeader((DLMS_HEADER *)pszBuffer, src, dest, ctrl, len);
	MakeHdlcTail(pszBuffer, len);
	return len + 2;
}

int MakeDlmsFrame(BYTE *pszBuffer, BYTE src, WORD dest, BYTE ctrl, BYTE *pszParam, WORD nLength)
{
	DLMS_FRAME	*pFrame;
	WORD		len;

	// FLAG(1) + DLMS_HEADER(8) + LSAP(2) + PARAM + CRC(2) + FLAG(1) - FLAG 2바이트 제외
	len = sizeof(DLMS_HEADER) + nLength + sizeof(DLMS_TAIL);
	MakeDlmsHeader((DLMS_HEADER *)pszBuffer, src, dest, ctrl, len);

	pFrame = (DLMS_FRAME *)pszBuffer;
	pFrame->destLSAP = LSAP_DEST_ADDRESS;
	pFrame->srcLSAP	 = LSAP_SRC_ADDRESS;
	memcpy(&pFrame->payload, pszParam, nLength);

	MakeHdlcTail(pszBuffer, len);
	return len + 2;
}

int MakeDlmsGenericFrame(BYTE *pszBuffer, BYTE src, WORD dest, BYTE ctrl, BYTE *pszParam, WORD nLength)
{
	// HDLC FRAME (SNRM, DISC)
	if ((pszParam == NULL) || (nLength == 0))
		return MakeHdlcFrame(pszBuffer, src, dest, ctrl);

	// DLMS FRAME (AARQ, GET, SET)
	return MakeDlmsFrame(pszBuffer, src, dest, ctrl, pszParam, nLength);
}

int MakeDlmsParamFrame(BYTE *pszBuffer, DLMSPARAM *pParam)
{
	DLMSVARIABLE	*pValue;
	DLMS_TAG	*pTag;
	DLMS_ARG	*pArg;
	BYTE		*pSeek;
	int			i, len, nLength=0;
	
	if ((pParam == NULL) || (pParam->ident != DLMS_PARAM_IDENTIFIER) || (pParam->count <= 0))
		return 0;

	pSeek = pszBuffer;
	for(i=0, len=0; i<(int)pParam->count; i++)
	{
		len = 1;
		pValue = &pParam->list[i];
		pTag   = (DLMS_TAG *)pSeek;
	    pArg   = (DLMS_ARG *)pSeek;
		
		pTag->tag = pValue->type;
		switch(pValue->type) {
		  case DLMS_DATATYPE_NULL :
			   break;

		  case DLMS_DATATYPE_ARRAY :
		  case DLMS_DATATYPE_COMPACT_ARRAY :
		  case DLMS_DATATYPE_STRUCTURE :
			   len = 2;
			   pArg->var.array = pValue->var.array;
			   break;

		  case DLMS_DATATYPE_BIT_STRING :
	 	  case DLMS_DATATYPE_OCTET_STRING :
	 	  case DLMS_DATATYPE_VISIBLE_STRING :
			   len = pValue->len + 2;
			   pTag->len = pValue->len;
			   memcpy(pTag->data, pValue->var.p, pValue->len);
			   break;

	 	  case DLMS_DATATYPE_BCD :
	 	  case DLMS_DATATYPE_BOOLEAN :
	 	  case DLMS_DATATYPE_INT8 :
	 	  case DLMS_DATATYPE_UINT8 :
			   len = sizeof(BYTE) + 1;
			   pArg->var.u8 = pValue->var.u8;
			   break;

	 	  case DLMS_DATATYPE_INT16 :
		  case DLMS_DATATYPE_UINT16 :
			   len = sizeof(WORD) + 1;
			   pArg->var.u16 = dlmsOrderByShort(pValue->var.u16);
			   break;

		  case DLMS_DATATYPE_INT32 :
		  case DLMS_DATATYPE_UINT32 :
			   len = sizeof(UINT) + 1;
			   pArg->var.u32 = dlmsOrderByShort(pValue->var.u32);
			   break;

	 	  case DLMS_DATATYPE_INT64 :
	 	  case DLMS_DATATYPE_UINT64 :
		  case DLMS_DATATYPE_FLOAT32 :
			   len = sizeof(long long int) + 1;
			   pArg->var.u64 = dlmsOrderByInt64(pValue->var.u64);
			   break;

		  case DLMS_DATATYPE_DATE_TIME :
			   len = sizeof(DLMSTIMEDATE) + 1;
			   memcpy(&pArg->var.array, &pValue->var.td, sizeof(DLMSTIMEDATE));
			   break;

		  case DLMS_DATATYPE_DATE :
			   len = sizeof(DLMSDATE) + 1;
			   memcpy(&pArg->var.array, &pValue->var.d, sizeof(DLMSDATE));
			   break;

		  case DLMS_DATATYPE_TIME :
			   len = sizeof(DLMSTIME) + 1;
			   memcpy(&pArg->var.array, &pValue->var.t, sizeof(DLMSTIME));
			   break;

		  default :
	    	   len = 0;
			   break;
		}

		nLength += len;
		pSeek	+= len;
	}
	return nLength;
}

const char *GetHdlcControlTypeString(BYTE nCtrl)
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

const char *GetDlmsDataTypeString(BYTE nType)
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

const char *GetDlmsMethodTypeString(BYTE nType)
{
	switch(nType) {
	  case DLMS_GET :		return "GET";
	  case DLMS_SET :		return "SET";
	  case DLMS_ACTION :	return "ACTION";
	}
	return "UNKNOWN";
}

const char *GetDlmsFlowTypeString(BYTE nFlow)
{
	switch(nFlow) {
	  case DLMS_REQUEST :	return "request";
	  case DLMS_RESPONSE :	return "response";
	}
	return "unknown";
}

const char *GetDlmsResTypeString(BYTE nType)
{
	switch(nType) {
	  case DLMS_NORMAL :	return "normal";
	  case DLMS_DATABLOCK :	return "datablock";
	  case DLMS_LIST :		return "list";
	}
	return "unknown";
}

#endif
