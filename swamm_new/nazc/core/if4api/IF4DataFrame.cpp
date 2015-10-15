#include "common.h"
#include "IF4DataFrame.h"
#include "IF4Service.h"
#include "IF4CrcCheck.h"
#include "IF4Util.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "zlib.h"

BOOL	m_bGlobalCompressFlag = TRUE;

//////////////////////////////////////////////////////////////////////
// IF4/IF5 Frame Utility
//////////////////////////////////////////////////////////////////////

void MakeIF4Header(IF4_HEADER *pHeader, BYTE nSeq, BYTE nAttr, int nLength, BYTE nService)
{
	pHeader->soh	= IF4_SOH;
	pHeader->seq	= nSeq;
	pHeader->attr	= nAttr;
	pHeader->len	= HostToLittleInt(nLength);
	pHeader->svc	= nService;
}

void MakeIF4Tail(IF4_HEADER *pHeader, int nLength)
{
	IF4_TAIL	*pTail;
	WORD		nCrc;

	pHeader->len = HostToLittleInt(nLength);
	pTail	= (IF4_TAIL *)((BYTE *)pHeader + nLength + sizeof(IF4_HEADER));
	nCrc   	= IF4GetCrc((BYTE *)pHeader, nLength+sizeof(IF4_HEADER), 0);
	pTail->crc = nCrc;
}

int MakeCompressFrame(IF4_HEADER *pHeader, int nLength, BOOL bCompress)
{
	BYTE	*pszBuffer, *pszData;
	BYTE	*pszCompress = NULL;
	uLong	nCompLen = 0;
	int 	nError;

	if (m_bGlobalCompressFlag && bCompress && (nLength > IF4_COMPRESS_MIN_SIZE))
	{
		pszBuffer = (BYTE *)pHeader;
		pszData	  = &pszBuffer[sizeof(IF4_HEADER)];
		pszCompress = (BYTE *)MALLOC(nLength);
		if (pszCompress != NULL)
		{   
			nCompLen = nLength;
			nError = compress((Bytef*)pszCompress, &nCompLen, (const Bytef*)pszData, nLength);
			XDEBUG("COMPRESS: SOURCE=%d byte(s), DEST=%d byte(s)\r\n", nLength, (int)nCompLen);
			if ((nError == Z_OK) && ((nCompLen > 0) && (nCompLen < (uLong)nLength)))
			{
				pHeader->attr |= IF4_ATTR_COMPRESS;
				nLength = nCompLen; 
				memcpy(pszData, pszCompress, nCompLen);
				MakeIF4Tail(pHeader, nLength);
			}
			FREE(pszCompress);
		}
	}
	return (nLength + sizeof(IF4_HEADER) + sizeof(IF4_TAIL));
}

void MakeIF4ServiceHeader(IF4_SVC_HEADER *pHeader)
{
	pHeader->mcuid = HostToLittleInt(m_pIF4Service->GetMcuID());
}

int MakeControlFrame(IF4_CTRL_FRAME *pFrame, BYTE nCode, char *arg, int argLength)
{
	BYTE	nAttr;
	WORD	nLength;

	// nLength = CODE(1) + ARG_LEN(2) + ARG(N)
	nLength		= argLength + sizeof(WORD) + 1;
	nAttr		= IF4_ATTR_CTRLFRAME;

	memset(pFrame, 0, sizeof(IF4_CTRL_FRAME));
	MakeIF4Header(&pFrame->hdr, 0, nAttr, nLength, IF4_SVC_CONTROL);
	pFrame->code	= nCode;
	pFrame->len		= HostToLittleShort(argLength);
	memcpy(pFrame->arg, arg, argLength);
	MakeIF4Tail(&pFrame->hdr, nLength);

	return MakeCompressFrame(&pFrame->hdr, nLength);
}

int MakeIF4MultipartFrame(IF4_MULTI_FRAME *pFrame, BYTE attr, BYTE seq, BYTE nCompress, UINT nSrcLength, BYTE *pszData, int nLength)
{
	IF4_PARTIAL_FRAME	*pPartial;
	BYTE	*p;
 
	pPartial = (IF4_PARTIAL_FRAME *)pFrame;
	MakeIF4Header(&pPartial->hdr, seq, attr, 0, IF4_SVC_PARTIAL);

	pPartial->chdr.compress		= nCompress;
	pPartial->chdr.source_size	= HostToLittleInt(nSrcLength);

	p = (BYTE *)&pPartial->data;
	memcpy(p, pszData, nLength);

	MakeIF4Tail(&pPartial->hdr, nLength+sizeof(IF4_COMPRESS_HEADER));
	return (nLength + sizeof(IF4_HEADER) + sizeof(IF4_TAIL) + sizeof(IF4_COMPRESS_HEADER));
}

int MakeIF4MultipartFrame(IF4_MULTI_FRAME *pFrame, BYTE attr, BYTE seq, BYTE *pszData, int nLength)
{
	BYTE	*p;
 
	MakeIF4Header(&pFrame->hdr, seq, attr, 0, IF4_SVC_PARTIAL);

	p = (BYTE *)&pFrame->data;
	memcpy(p, pszData, nLength);

	MakeIF4Tail(&pFrame->hdr, nLength);
	return (nLength + sizeof(IF4_HEADER) + sizeof(IF4_TAIL));
}

void MakeIF4CommandHeader(IF4_CMD_HEADER *pHeader, OID3 *oid, BYTE tid, BYTE attr, BYTE errCode, WORD cnt)
{
	memcpy(&pHeader->cmd, oid, sizeof(OID3));
	pHeader->attr		= attr;
	pHeader->tid		= tid;
	pHeader->errcode	= errCode;
	pHeader->cnt		= HostToLittleShort(cnt);
}

int MakeCommandFrame(IF4_CMD_FRAME *pFrame, OID3 *oid, BYTE tid, BYTE attr, BYTE errCode, WORD cnt, SMIValue *pValue, int *nSourceLength, BOOL bCompress)
{
	int 	nLength, i, len, n;
	BYTE	*p1, *p2;
 
	MakeIF4Header(&pFrame->hdr, 0, IF4_ATTR_START|IF4_ATTR_END, 0, IF4_SVC_COMMAND);
	MakeIF4ServiceHeader(&pFrame->sh);
	MakeIF4CommandHeader(&pFrame->ch, oid, tid, attr, errCode, cnt);

	p1	= (BYTE *)pValue;
	p2 	= (BYTE *)&pFrame->args;
	nLength = sizeof(IF4_SVC_HEADER) + sizeof(IF4_CMD_HEADER); 

	n = 0;
	for(i=0; i<cnt; i++)
	{
		len = ((SMIValue *)p1)->len + sizeof(WORD) + sizeof(OID3);
		nLength += len;
		p1 += len;
		n  += len;
	}
    SmiToLittleValue((char *)pValue, cnt);
	memcpy(p2, pValue, n);
	MakeIF4Tail(&pFrame->hdr, nLength);
	if (nSourceLength) *nSourceLength = sizeof(IF4_HEADER) + nLength + sizeof(IF4_TAIL);
	return MakeCompressFrame(&pFrame->hdr, nLength, bCompress);
}

void MakeIF4DataHeader(IF4_DATA_HEADER *pHeader, WORD cnt)
{
	pHeader->cnt = HostToLittleShort(cnt);
}

int MakeIF4DataFrame(IF4_DATA_FRAME *pFrame, WORD cnt, BYTE *pszBuffer, int nDataLen, int *nSourceLength)
{
	BYTE	*p;
	int 	nLength=0;

	MakeIF4Header(&pFrame->hdr, 0, IF4_ATTR_START|IF4_ATTR_END, 0, IF4_SVC_DATA);
	MakeIF4ServiceHeader(&pFrame->sh);
	MakeIF4DataHeader(&pFrame->dh, cnt);

	p = (BYTE *)&pFrame->args;
	nLength = sizeof(IF4_SVC_HEADER) + sizeof(IF4_DATA_HEADER) + nDataLen; 
	memcpy(p, pszBuffer, nDataLen);

	MakeIF4Tail(&pFrame->hdr, nLength);
	if (nSourceLength) *nSourceLength = sizeof(IF4_HEADER) + nLength + sizeof(IF4_TAIL);
	return MakeCompressFrame(&pFrame->hdr, nLength);
}

int MakeIF4TypeRFrame(IF4_TYPER_FRAME *pFrame, WORD cnt, BYTE *pszBuffer, int nDataLen, int *nSourceLength)
{
	BYTE	*p;
	int 	nLength=0;

	MakeIF4Header(&pFrame->hdr, 0, IF4_ATTR_START|IF4_ATTR_END, 0, IF4_SVC_NEWMETERDATA);
	MakeIF4ServiceHeader(&pFrame->sh);
	MakeIF4DataHeader(&pFrame->dh, cnt);

	p = (BYTE *)&pFrame->data;
	nLength = sizeof(IF4_SVC_HEADER) + sizeof(IF4_DATA_HEADER) + nDataLen; 
	memcpy(p, pszBuffer, nDataLen);

	MakeIF4Tail(&pFrame->hdr, nLength);
	if (nSourceLength) *nSourceLength = sizeof(IF4_HEADER) + nLength + sizeof(IF4_TAIL);
	return MakeCompressFrame(&pFrame->hdr, nLength);
}

void MakeIF4DataFileHeader(IF4_DATAFILE_HEADER *pHeader, BYTE nType, char *pszFileName, UINT nSize)
{
	memset(pHeader, 0, sizeof(IF4_DATAFILE_HEADER));
	pHeader->ftype		= nType;
	pHeader->fsize		= HostToLittleInt(nSize);
	strncpy(pHeader->fname, pszFileName, 32);
}

int MakeIF4DataFileFrame(IF4_DATAFILE_FRAME *pFrame, BYTE nType, char *pszFileName, BYTE *pszBuffer, int nDataLen, int *nSourceLength)
{
	BYTE	*p;
	int 	nLength=0;

	MakeIF4Header(&pFrame->hdr, 0, IF4_ATTR_START|IF4_ATTR_END, 0, IF4_SVC_DATAFILE);
	MakeIF4ServiceHeader(&pFrame->sh);
	MakeIF4DataFileHeader(&pFrame->fh, nType, pszFileName, nDataLen);

	p = (BYTE *)&pFrame->data;
	nLength = sizeof(IF4_SVC_HEADER) + sizeof(IF4_DATAFILE_HEADER) + nDataLen; 
	memcpy(p, pszBuffer, nDataLen);

	MakeIF4Tail(&pFrame->hdr, nLength);
	if (nSourceLength) *nSourceLength = sizeof(IF4_HEADER) + nLength + sizeof(IF4_TAIL);
	return MakeCompressFrame(&pFrame->hdr, nLength);
}

void MakeIF4MeteringDataHeader(IF4_DATA_HEADER *pHeader, WORD cnt)
{
	pHeader->cnt = HostToLittleShort(cnt);
}

int MakeIF4MeteringDataFrame(IF4_DATA_FRAME *pFrame, WORD cnt, BYTE *pszBuffer, int nDataLen, int *nSourceLength, BYTE nSeq)
{
	BYTE	*p;
	int 	nLength=0;

	MakeIF4Header(&pFrame->hdr, nSeq, IF4_ATTR_START|IF4_ATTR_END, 0, IF4_SVC_METERINGDATA);
	MakeIF4ServiceHeader(&pFrame->sh);
	MakeIF4DataHeader(&pFrame->dh, cnt);

	p = (BYTE *)&pFrame->args;
	nLength = sizeof(IF4_SVC_HEADER) + sizeof(IF4_DATA_HEADER) + nDataLen; 
	memcpy(p, pszBuffer, nDataLen);

	MakeIF4Tail(&pFrame->hdr, nLength);
	if (nSourceLength) *nSourceLength = sizeof(IF4_HEADER) + nLength + sizeof(IF4_TAIL);
	return MakeCompressFrame(&pFrame->hdr, nLength);
}

void MakeIF4AlarmHeader(IF4_ALARM_HEADER *pHeader, BYTE nType, EUI64 *id, TIMESTAMP *pTime, TIMESTAMP *pSysTime, BYTE idx)
{
	pHeader->srcType = nType;
	pHeader->idx	 = idx;
	memcpy(pHeader->srcID, id, sizeof(EUI64));

	if (pTime != NULL)
	{
		 memcpy(&pHeader->tmAlarm, pTime, sizeof(TIMESTAMP));
		 pHeader->tmAlarm.year = pTime->year;
	}
	else GET_TIMESTAMP(&pHeader->tmAlarm, NULL);

    pHeader->tmAlarm.year = HostToLittleShort(pHeader->tmAlarm.year);

	if (pSysTime != NULL)
		 memcpy(&pHeader->tmMcu, pSysTime, sizeof(TIMESTAMP));
	else GET_TIMESTAMP(&pHeader->tmMcu, NULL);

    pHeader->tmMcu.year = HostToLittleShort(pHeader->tmMcu.year);
}

int MakeIF4AlarmFrame(IF4_ALARM_FRAME *pFrame, BYTE nType, EUI64 *id, TIMESTAMP *pTime, BYTE *pMsg, int nDataLen, TIMESTAMP *pSysTime, BYTE idx, int *nSourceLength)
{
	IF4_DATA	*pData;
	int 		nLength=0;

	MakeIF4Header(&pFrame->hdr, 0, IF4_ATTR_START|IF4_ATTR_END, 0, IF4_SVC_ALARM);
	MakeIF4ServiceHeader(&pFrame->sh);
	MakeIF4AlarmHeader(&pFrame->ah, nType, id, pTime, pSysTime, idx);

	pData 		= (IF4_DATA *)&pFrame->args;
	pData->len	= HostToLittleShort(nDataLen);
	nLength 	= sizeof(IF4_SVC_HEADER) + sizeof(IF4_ALARM_HEADER) + nDataLen + 2; 
	memcpy(pData->data, pMsg, nDataLen); 

	MakeIF4Tail(&pFrame->hdr, nLength);
	if (nSourceLength) *nSourceLength = sizeof(IF4_HEADER) + nLength + sizeof(IF4_TAIL);
	return MakeCompressFrame(&pFrame->hdr, nLength);
}

void MakeIF4EventHeader(IF4_EVENT_HEADER *pHeader, OID3 *code, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime)
{
	memcpy(&pHeader->eventCode, code, sizeof(OID3));
	pHeader->srcType = srcType;
	if (srcID == NULL)
	     memset(&pHeader->srcID, 0, sizeof(EUI64));
	else memcpy(&pHeader->srcID, srcID, sizeof(EUI64));
	if (pTime != NULL)
		 memcpy(&pHeader->tmEvent, pTime, sizeof(TIMESTAMP));
	else GET_TIMESTAMP(&pHeader->tmEvent, NULL);

    pHeader->tmEvent.year = HostToLittleShort(pHeader->tmEvent.year);
}

int MakeIF4EventFrame(IF4_EVENT_FRAME *pFrame, OID3 *code, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime, WORD cnt, SMIValue *pValue, int *nSourceLength, BOOL bCompress)
{
	int 		i, len, n=0, nLength=0;
	SMIValue	*pParam;
	BYTE		*pSeek;

	MakeIF4Header(&pFrame->hdr, 0, IF4_ATTR_START|IF4_ATTR_END, 0, IF4_SVC_EVENT);
	MakeIF4ServiceHeader(&pFrame->sh);
	MakeIF4EventHeader(&pFrame->eh, code, srcType, srcID, pTime);

	pFrame->eh.cnt = HostToLittleShort(cnt);
	pSeek = (BYTE *)&pFrame->args;
	nLength	= sizeof(IF4_SVC_HEADER) + sizeof(IF4_EVENT_HEADER); 
	for(n=0, i=0; i<cnt; i++)
	{
		pParam = (SMIValue *)((BYTE *)pValue + n);
		len	= pParam->len + sizeof(OID3) + sizeof(WORD);
		n += len;
	}
    SmiToLittleValue((char *)pValue, cnt);
	memcpy(pSeek, pValue, n);
	nLength += n;

	MakeIF4Tail(&pFrame->hdr, nLength);

	// Check compress flag
	if (nSourceLength) *nSourceLength = sizeof(IF4_HEADER) + nLength + sizeof(IF4_TAIL);
	return bCompress ? MakeCompressFrame(&pFrame->hdr, nLength) : nLength;
}

//////////////////////////////////////////////////////////////////////
// IF4/IF5 Frame Utility
//////////////////////////////////////////////////////////////////////

int CopyMIB2SMIValue(SMIValue *pDest, MIBValue *pSrc)
{
	int		len = 0;
    int     srcLen = pSrc->len;

	memcpy(&pDest->oid, &pSrc->oid, sizeof(OID3));
	pDest->len 	= (WORD)srcLen;
	len	= (srcLen + 5);
	
	switch(pSrc->type) {
	  case VARSMI_CHAR :
		   pDest->stream.s8 = pSrc->stream.s8;
		   break;
	  case VARSMI_BOOL :
		   pDest->stream.u8	= pSrc->stream.u8;
		   break;
	  case VARSMI_BYTE :
		   pDest->stream.u8	= pSrc->stream.u8;
		   break;
	  case VARSMI_SHORT :
		   pDest->stream.s16 = pSrc->stream.s16;
		   break;
	  case VARSMI_WORD :
		   pDest->stream.u16 = pSrc->stream.u16;
		   break;
	  case VARSMI_INT :
		   pDest->stream.s32 = pSrc->stream.s32;
		   break;
	  case VARSMI_UINT :
		   pDest->stream.u32 = pSrc->stream.u32;
		   break;
	  case VARSMI_OID :
		   memcpy(&pDest->stream.id, &pSrc->stream.id, sizeof(OID3));
		   break;
	  case VARSMI_STRING :
	  case VARSMI_STREAM :	
	  case VARSMI_OPAQUE :	 
		   memcpy(pDest->stream.str, pSrc->stream.p, srcLen);
		   break;
	  case VARSMI_EUI64 :
		   memcpy(pDest->stream.str, pSrc->stream.p, MIN(srcLen,8));
		   break;
	  case VARSMI_IPADDR :
		   memcpy(pDest->stream.str, pSrc->stream.p, MIN(srcLen,6));
		   break;
	  case VARSMI_TIMESTAMP :
		   memcpy(pDest->stream.str, pSrc->stream.p, MIN(srcLen,7));
		   break;
	  case VARSMI_TIMEDATE :
		   memcpy(pDest->stream.str, pSrc->stream.p, MIN(srcLen,4));
		   break;
	}

	return len;
}

void CopySMI2MIBValue(MIBValue *pDest, SMIValue *pSrc, BYTE nType)
{
    UINT     srcLen = pSrc->len;

	memcpy(&pDest->oid, &pSrc->oid, sizeof(OID3));
	pDest->type	= nType;
	pDest->len 	= srcLen;
	pDest->stream.p	= NULL;
	
	switch(nType) {
	  case VARSMI_CHAR :
		   pDest->stream.s8	= pSrc->stream.s8;
		   break;
	  case VARSMI_BOOL :
		   pDest->stream.u8	= pSrc->stream.u8;
		   break;
	  case VARSMI_BYTE :
		   pDest->stream.u8	= pSrc->stream.u8;
		   break;
	  case VARSMI_SHORT :
		   pDest->stream.s16 = pSrc->stream.s16;
		   break;
	  case VARSMI_WORD :
		   pDest->stream.u16 = pSrc->stream.u16;
		   break;
	  case VARSMI_INT :
		   pDest->stream.s32 = pSrc->stream.s32;
		   break;
	  case VARSMI_UINT :
		   pDest->stream.u32 = pSrc->stream.u32;
		   break;
	  case VARSMI_OID :
		   memcpy(&pDest->stream.id, &pSrc->stream.id, sizeof(OID3));
		   break;
	  case VARSMI_STRING :
	  case VARSMI_STREAM :	
	  case VARSMI_OPAQUE :	 
		   pDest->stream.p = (char *)MALLOC(srcLen+1);
		   memcpy(pDest->stream.p, pSrc->stream.str, srcLen);
		   pDest->stream.p[srcLen] = '\0';
		   break;	 
	  case VARSMI_EUI64 :
		   pDest->stream.p = (char *)MALLOC(8);
		   memset(pDest->stream.p, 0, 8);
		   memcpy(pDest->stream.p, pSrc->stream.str, MIN(srcLen,8));
		   break;	 
	  case VARSMI_IPADDR :
		   pDest->stream.p = (char *)MALLOC(6);
		   memset(pDest->stream.p, 0, 6);
		   memcpy(pDest->stream.p, pSrc->stream.str, MIN(srcLen,6));
		   break;	 
	  case VARSMI_TIMESTAMP :
		   pDest->stream.p = (char *)MALLOC(sizeof(TIMESTAMP));
		   memset(pDest->stream.p, 0, sizeof(TIMESTAMP));
		   memcpy(pDest->stream.p, pSrc->stream.str, MIN(srcLen,sizeof(TIMESTAMP)));
		   break;	 
	  case VARSMI_TIMEDATE :
		   pDest->stream.p = (char *)MALLOC(sizeof(TIMEDATE));
		   memset(pDest->stream.p, 0, sizeof(TIMEDATE));
		   memcpy(pDest->stream.p, pSrc->stream.str, MIN(srcLen,sizeof(TIMEDATE)));
		   break;	 
	  default :
		   pDest->stream.p = NULL;
		   break;
	}
}

