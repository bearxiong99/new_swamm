////////////////////////////////////////////////////////////////////////////////
//
// DLMS/COSEM Client Library
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __DLMS_FUNCTIONS_H__
#define __DLMS_FUNCTIONS_H__

#define PROTOCOL_DLMS_INIT      dlmsInit()

#ifdef __cplusplus
extern "C" {
#endif

#include "dlmsType.h"
#include "dlmsFrame.h"

// Initialize
int dlmsInit(void);

// DLMS functions
int dlmsMakeSendFrame(BYTE nType, BYTE *pszParam, int nLength, BYTE src, UINT dest, BYTE **pszDlmsFrame, int *nFrameLength);
int dlmsMakeGetFrame(DLMSOBIS *obis, DLMSTAGVALUE *pParam, BYTE lastRxControl, BYTE src, UINT dest, BYTE **pszDlmsFrame, int *nFrameLength);
int dlmsMakeGetNextFrame(UINT nBlockCount, BYTE lastRxControl, BYTE src, UINT dest, BYTE **pszDlmsFrame, int *nFrameLength);
int dlmsMakeAarqFrame(BYTE nSecurityType, DLMS_AUTH_VALUE nAuthValueType, BYTE *pszAuthValue, int nAuthLength, 
        UINT nConformance, BYTE src, UINT dest, BYTE **pszDlmsFrame, int *nFrameLength);

int dlmsSetTagValue(DLMSTAGVALUE * tagBuf, BYTE nType, void *pValue, BYTE nLen);
BYTE * dlmsGetResPayload(DLMS_FRAME * pFrame);

// Token functions
DLMSVARIABLE * dlmsAllocToken(int nLimit);
int	dlmsToken(DLMSVARIABLE *pToken, int nPrevTokenSize, int *nTokenSize, int nMaxToken, BYTE *pszBuffer, int nLength);
void dlmsFreeToken(DLMSVARIABLE *pToken, int nCount);

// Utility functions
void dlmsReqAddress(DLMS_HEADER *header, HDLC_REQ_ADDR *addr, BYTE *src, UINT *dest, BYTE *ctrl);
void dlmsResAddress(DLMS_HEADER * header, HDLC_RES_ADDR *addr, UINT *src, BYTE *dest, BYTE *ctrl);

#ifdef __cplusplus
}
#endif

#endif	// __DLMS_FUNTIONS_H__
