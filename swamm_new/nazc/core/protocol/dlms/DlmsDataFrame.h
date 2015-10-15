#ifndef __DLMS_DATA_FRAME_H__
#define __DLMS_DATA_FRAME_H__

#include "dlmsType.h"

BYTE GetHdlcControl(BYTE rxControl, BOOL bReset);
int MakeHdlcHeader(HDLC_HEADER *pHeader, BOOL bSeg, BYTE src, UINT dest, BYTE ctrl, WORD payloadLen, WORD * headerLength);
void MakeHdlcTail(BYTE *pszBuffer, WORD len);
int MakeHdlcFrame(BYTE *pszBuffer, BYTE src, UINT dest, BYTE ctrl, WORD *frameLength);
int MakeDlmsHeader(DLMS_HEADER *pHeader, BOOL bSeg, BYTE src, UINT dest, BYTE ctrl, WORD payloadLen, WORD * headerLength);
int MakeDlmsFrame(BYTE *pszBuffer, BYTE src, UINT dest, BYTE ctrl, WORD *frameLength);
int MakeDlmsFrame(BYTE *pszBuffer, BYTE src, UINT dest, BYTE ctrl, 
        BYTE *pszPayload, WORD nLength, BYTE *pszTag, WORD nTagLen, WORD *frameLength);
int MakeDlmsGenericFrame(BYTE *pszBuffer, BYTE src, UINT dest, BYTE ctrl, 
        BYTE *pszPayload, WORD nLength, BYTE *pszTag, WORD nTagLen, WORD *frameLength);

//int MakeDlmsParamFrame(BYTE *pszBuffer, DLMSPARAM *pParam);
const char *GetHdlcControlTypeString(BYTE nCtrl);
const char *GetDlmsDataTypeString(BYTE nType);

const char *GetDlmsMethodTypeString(BYTE nType);
const char *GetDlmsFlowTypeString(BYTE nFlow);
const char *GetDlmsResTypeString(BYTE nType);

#endif	// __DLMS_DATA_FRAME_H__
