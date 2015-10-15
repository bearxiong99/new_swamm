#include "common.h"
#include "DlmsDataFrame.h"
#include "DlmsCrcCheck.h"
#include "CommonUtil.h"
#include "DlmsUtil.h"
#include "DebugUtil.h"
#include "dlmsError.h"
#include "dlmsUtils.h"

BYTE GetHdlcControl(BYTE rxControl, BOOL bReset)
{
	BYTE	nControl;
	BYTE	rrr, sss;

	rrr = (rxControl & 0xe0) >> 4;				// RRR (receive sequence number)
	sss = (rxControl & 0x0e) << 4;				// SSS (send sequence number)
	
	if (bReset) rrr = 0x00;
	nControl = (sss + 0x20) + 0x10 + rrr + bReset;

	return nControl;
}

int MakeHdlcHeader(HDLC_HEADER *pHeader, BOOL bSeg, BYTE src, UINT dest, BYTE ctrl, WORD payloadLen, WORD * headerLength)
{
    int offset = 5;
    BYTE *pszBuffer = (BYTE *)pHeader;

	memset(pHeader, 0, sizeof(HDLC_HEADER));
	pHeader->startFlag 	= HDLC_FLAG;

    if(dest < 0xFF) {         //  one byte dest addr
        pHeader->addr.d1.dest = (BYTE)(dest & 0xFF);
        pHeader->addr.d1.src = src;
        offset = 2;
    }else if(dest < 0xFFFF) {   //  two bytes dest addr
        pHeader->addr.d2.dest = dlmsOrderByShort((WORD)(dest & 0xFFFF));
        pHeader->addr.d2.src = src;
        offset = 3;
    }else if((dest & 0x00000001) == 0x00000001) {   //  four bytes dest addr
        pHeader->addr.d4.dest = dlmsOrderByInt(dest);
        pHeader->addr.d4.src = src;
        offset = 5;
    }else {
        return DLMSERR_INVALID_DEST_ADDR;
    }
	pHeader->typeLen	= dlmsOrderByShort((HDLC_FRAME_FORMAT << 8) | 
            (bSeg ? (HDLC_SEGMENTATION << 8) : 0x0000) | ((payloadLen+offset+3+2) & 0x07FF)); // p + h + t
    pszBuffer[3 + offset] = ctrl;       // control,pHeader->ctrl		= ctrl;

    if(headerLength) *headerLength = 4 + offset; // header + flag

    return DLMSERR_NOERROR;
}

void MakeHdlcTail(BYTE *pszBuffer, WORD len)
{
	HDLC_TAIL	*pTail;

	pTail = (HDLC_TAIL *)(pszBuffer + len);
	pTail->crc		= DlmsGetCrc(&pszBuffer[1], len-1);
	pTail->endFlag	= HDLC_FLAG;
}

int MakeHdlcFrame(BYTE *pszBuffer, BYTE src, UINT dest, BYTE ctrl, WORD *frameLength)
{
    WORD    hLen = 0;
    int     nError;

	// Header + Tail 
	if((nError=MakeHdlcHeader((HDLC_HEADER *)pszBuffer, FALSE, src, dest, ctrl, 0, &hLen))!=DLMSERR_NOERROR) {
        return nError;
    }

	MakeHdlcTail(pszBuffer, hLen);
    if(frameLength) *frameLength = hLen + sizeof(HDLC_TAIL);
    return nError;
}

int MakeDlmsHeader(DLMS_HEADER *pHeader, BOOL bSeg, BYTE src, UINT dest, BYTE ctrl, WORD payloadLen, WORD * headerLength)
{
    int nError;
    WORD hLen = 0;
    WORD hcs = 0x00;

    if((nError = MakeHdlcHeader((HDLC_HEADER *)pHeader, bSeg, src, dest, ctrl, payloadLen+2, &hLen)) != DLMSERR_NOERROR) {
        return nError;
    }
	hcs = DlmsGetCrc((BYTE *)&pHeader->typeLen, hLen - 1);
    memcpy(((BYTE *)pHeader) + hLen, (BYTE *)&hcs, sizeof(WORD)); 

    if(headerLength) *headerLength = hLen + 2;  // HDLC Header + hcs(2)
    return nError;
}

int MakeDlmsFrame(BYTE *pszBuffer, BYTE src, UINT dest, BYTE ctrl, WORD *frameLength)
{
	WORD		hLen=0;
    int         nError;

	if((nError=MakeDlmsHeader((DLMS_HEADER *)pszBuffer, FALSE, src, dest, ctrl, 0, &hLen)) != DLMSERR_NOERROR) {
        return nError;
    }
	MakeHdlcTail(pszBuffer, hLen);
    if(frameLength) *frameLength = hLen + sizeof(HDLC_TAIL);
	return nError;
}

int MakeDlmsFrame(BYTE *pszBuffer, BYTE src, UINT dest, BYTE ctrl, 
        BYTE *pszPayload, WORD nLength, BYTE *pszTag, WORD nTagLen, WORD *frameLength)
{
	WORD		hLen = 0;
    int         nError;
    int         payloadLength = (pszPayload == NULL ? 0 : nLength);
    int         tagLength = (pszTag == NULL ? 0 : nTagLen);
    int         nTotLen = payloadLength + tagLength;;
    BOOL        bSeg = FALSE;

	if((nError=MakeDlmsHeader((DLMS_HEADER *)pszBuffer, bSeg, src, dest, ctrl, nTotLen, &hLen)) != DLMSERR_NOERROR) {
        return nError;
    }

    if(payloadLength > 0) {
	    memcpy(&pszBuffer[hLen], pszPayload, payloadLength);
    }
    if(tagLength > 0) {
	    memcpy(&pszBuffer[hLen + payloadLength], pszTag, tagLength);
	}

	MakeHdlcTail(pszBuffer, hLen + nTotLen);
    if(frameLength) *frameLength = hLen + nTotLen + sizeof(HDLC_TAIL);

    return nError;
}

int MakeDlmsGenericFrame(BYTE *pszBuffer, BYTE src, UINT dest, BYTE ctrl, BYTE *pszPayload, WORD nLength, 
        BYTE *pszTag, WORD nTagLen, WORD *frameLength)
{
	// NO Imformations 
	if ((pszPayload == NULL) || (nLength == 0))
		return MakeHdlcFrame(pszBuffer, src, dest, ctrl, frameLength);

	// have informations : DLMS FRAME (AARQ, GET, SET)
	return MakeDlmsFrame(pszBuffer, src, dest, ctrl, pszPayload, nLength, pszTag, nTagLen, frameLength);
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
