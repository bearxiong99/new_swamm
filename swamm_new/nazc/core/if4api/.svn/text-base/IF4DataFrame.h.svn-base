#ifndef __IF4_DATA_FRAME_H__
#define __IF4_DATA_FRAME_H__

#include "if4frame.h"

void MakeIF4Header(IF4_HEADER *pHeader, BYTE nSeq, BYTE nAttr, int nLength, BYTE nService);
void MakeIF4Tail(IF4_HEADER *pHeader, int nLength);
void MakeIF4ServiceHeader(IF4_SVC_HEADER *pHeader);

int MakeCompressFrame(IF4_HEADER *pHeader, int nLength, BOOL bCompress=TRUE);
int MakeControlFrame(IF4_CTRL_FRAME *pFrame, BYTE nCode, char *arg, int argLength);

int MakeIF4MultipartFrame(IF4_MULTI_FRAME *pFrame, BYTE attr, BYTE seq, BYTE nCompress, UINT nSrcLength, BYTE *pszData, int nLength);
int MakeIF4MultipartFrame(IF4_MULTI_FRAME *pFrame, BYTE attr, BYTE seq, BYTE *pszData, int nLength);

void MakeIF4CommandHeader(IF4_CMD_HEADER *pHeader, OID3 *oid3, BYTE tid, BYTE attr, BYTE errCode, WORD cnt);
int MakeCommandFrame(IF4_CMD_FRAME *pFrame, OID3 *oid3, BYTE tid, BYTE attr, BYTE errCode, WORD cnt, SMIValue *pValue, int *nSourceLength=NULL, BOOL bCompress=TRUE);

void MakeIF4DataHeader(IF4_DATA_HEADER *pHeader, WORD cnt);
int MakeIF4DataFrame(IF4_DATA_FRAME *pFrame, WORD cnt, BYTE *pszData, int nLength, int *nSourceLength=NULL);

void MakeIF4DataFileHeader(IF4_DATAFILE_HEADER *pHeader, BYTE nType, char *pszFileName, UINT nSize);
int MakeIF4DataFileFrame(IF4_DATAFILE_FRAME *pFrame, BYTE nType, char *pszFileName, BYTE *pszData, int nLength, int *nSourceLength=NULL);

void MakeIF4MeteringDataHeader(IF4_DATA_HEADER *pHeader, WORD cnt);
int MakeIF4MeteringDataFrame(IF4_DATA_FRAME *pFrame, WORD cnt, BYTE *pszData, int nLength, int *nSourceLength=NULL, BYTE nSeq=0);

void MakeIF4AlarmHeader(IF4_ALARM_HEADER *pHeader, BYTE srcTypd, EUI64 *id, TIMESTAMP *pAlarm, TIMESTAMP *pSysTime, BYTE idx);
int MakeIF4AlarmFrame(IF4_ALARM_FRAME *pFrame, BYTE nType, EUI64 *id, TIMESTAMP *pTime, BYTE *pMsg, int nDataLen, TIMESTAMP *pSysTime, BYTE idx, int *nSourceLength=NULL);

void MakeIF4EventHeader(IF4_EVENT_HEADER *pHeader, OID3 *code, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime);
int MakeIF4EventFrame(IF4_EVENT_FRAME *pFrame, OID3 *code, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime, WORD cnt, SMIValue *pValue, int *nSourceLength=NULL, BOOL bCompress=TRUE);

int MakeIF4TypeRFrame(IF4_TYPER_FRAME *pFrame, WORD cnt, BYTE *pszBuffer, int nDataLen, int *nSourceLength);

int CopyMIB2SMIValue(SMIValue *pDest, MIBValue *pSrc);
void CopySMI2MIBValue(MIBValue *pDest, SMIValue *pSrc, BYTE nType);

#endif	// __IF4_DATA_FRAME_H__
