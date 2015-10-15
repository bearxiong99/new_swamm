#ifndef __KMP_DATA_FRAME_H__
#define __KMP_DATA_FRAME_H__

#include "kmpType.h"
#include "kmpFrame.h"

int MakeKmpTail(BYTE *pszBuffer, BYTE len, KMP_TAIL *pTail);
int MakeKmpHeader(KMP_HEADER *pHeader, BYTE addr, BYTE cid);
int MakeKmpFrame(BYTE *pszBuffer, BYTE cid, BYTE *pszData, int nLength);
int StuffFrame(BYTE *pszBuffer, int nLength, BYTE *stuffedBuffer);
int StripFrame(BYTE *pszBuffer, int nLength, BYTE *stripBuffer);

#endif	// __KMP_DATA_FRAME_H__
