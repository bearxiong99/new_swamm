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
        pHeader->addr.d1.dest = (BYTE)(((dest << 1) & 0xFF) | 0x01);
        pHeader->addr.d1.src = (BYTE)((src << 1) | 0x01);
        offset = 2;
    }else if(dest < 0xFFFF) {   //  two bytes dest addr
        pHeader->addr.d2.dest = dlmsOrderByShort((WORD)(((dest << 1) & 0xFFFF) | 0x01));
        pHeader->addr.d2.src = (BYTE)((src << 1) | 0x01);
        offset = 3;
    }else if(dest < 0xFFFFFFFF) {   //  four bytes dest addr
        pHeader->addr.d4.dest = dlmsOrderByInt((dest << 1) | 0x01);
        pHeader->addr.d4.src = (BYTE)((src << 1) | 0x01);
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

