
#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "DlmsDataFrame.h"
#include "DlmsUtil.h"

#include "dlmsDataStream.h"
#include "dlmsFunctions.h"
#include "dlmsUtils.h"

BYTE mDlmsOidLn[] = DLMS_OID_LN;
BYTE mDlmsOidSn[] = DLMS_OID_SN;
BYTE mDlmsDefaultSecurityMachanism[] = DLMS_DEFAULT_SECURITY_MACHANISM;

int dlmsInit(void)
{
    return 0;
}

int dlmsMakeSendFrame(BYTE nType, BYTE *pszParam, int nLength, BYTE src, UINT dest, BYTE **pszDlmsFrame, int *nFrameLength)
{
    DLMS_FRAME frame;
    WORD nSize=0;
    int nError;

    if(pszDlmsFrame == NULL || nFrameLength == NULL) return DLMSERR_INVALID_PARAM;

    if((nError=MakeDlmsGenericFrame((BYTE *)&frame, src, dest, nType, pszParam, nLength, NULL, 0, &nSize))!=DLMSERR_NOERROR)
    {
        return nError;    
    }

    if(nSize <= 0)
        return DLMSERR_SYSTEM_ERROR;

    (*pszDlmsFrame) = (BYTE *)MALLOC(nSize);
    if(*pszDlmsFrame == NULL) return DLMSERR_MEMORY_ERROR;

    memcpy(*pszDlmsFrame, (BYTE *)&frame, nSize);
    *nFrameLength = nSize;

    return nError;;
}

/** AARQ Frame.
  *
  * Examples.
  *
  * BYTE authParam[57] =
  * {
  *   0x00,         LLC quality
  *   0x60,         application 0 (0x60)
  *   0x36,         length (54)
  *     0xA1,       Application-Context-Name 
  *     0x09,       length (9)
  *       0x06,     Object Identifier (0x06)
  *       0x07,     length (7)
  *       0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01,  (LN 2.16.756.5.8.1.1)
  *     0x8A,       ACSE-requirements
  *     0x02,       length (2)
  *       0x07, 0x80, 
  *     0x8B,       mechanism-name
  *     0x07,       length (7)
  *       0x60, 0x85, 0x74, 0x05, 0x08, 0x02, 0x01,  (default-COSEM-low-lvel-security-mechanism-name 2.16.756.5.8.2.1)
  *     0xAC,       authentication value
  *     0x0A,       length (10)
  *       0x80,     GraphicString
  *       0x08,     length (8)
  *       0x31, 0x41, 0x32, 0x42, 0x33, 0x43, 0x34, 0x44,   ( "1A2B3C4D" )
  *     0xBE,       user information
  *     0x10,       length (16)
  *       0x04,     OCTET STRING
  *       0x0E,     length (14)
  *         0x01,                   DLMS APDU CHOICE (InitiateRequest)
  *         0x00,                   dedicated-key (FALSE)
  *         0x00,                   response-allowed (FALSE)
  *         0x00,                   proposed-quality-of-service (FALSE)
  *         0x06,                   proposed-dlms-version-number (value 6)
  *           0x5F,0x1F,            Conformance block (application 31) 
  *           0x04,                 length(4)
  *             0x00,               number of unused block
  *             0x00, 0x18, 0x19, 
  *         0xFF, 0xFF              client-max-receive-pdu-size 
  * };
  *
  * @param nAssociationType DLMS_ASSOCIATION_LN, DLMS_ASSOCIATION_SN
  * @param nAuthValueType DLMS_AUTH_VALUE_GRAPHICSTRING
  * @param pszAuthValue authentication value
  * @param nAuthLength length of pszAuthValue
  * @param nConformance proposed conformance
  * @param src
  * @param dest
  * @param pszDlmsFrame
  * @param nFrameLength
  *
  */
int dlmsMakeAarqFrame(BYTE nAssociationType, DLMS_AUTH_VALUE nAuthValueType, BYTE *pszAuthValue, int nAuthLength, 
        UINT nConformance, BYTE src, UINT dest, BYTE **pszDlmsFrame, int *nFrameLength)
{
    DLMS_FRAME frame;
    BYTE nType = HDLC_CTRL_I;
    BYTE szBuffer[256];
    int nLength = 0;
    WORD nSize = 0;
    int nError;

    if(pszDlmsFrame == NULL || nFrameLength == NULL) return DLMSERR_INVALID_PARAM;

    memset(szBuffer, 0, sizeof(szBuffer)); 

    szBuffer[nLength++] = LSAP_DEST_ADDRESS;                                    // LSAP Dest
    szBuffer[nLength++] = LSAP_SRC_ADDRESS;                                     // LSAP Src
    nLength ++;                                                                 // LLC quality
    szBuffer[nLength++] = ASN_CLASS_APPLICATION | ASN_CONSTRUCTED | 0x00;       // application 0
    szBuffer[nLength++] = 46 + nAuthLength;                                     //  length 
    szBuffer[nLength++] = ASN_CLASS_CONTEXT | ASN_CONSTRUCTED | 0x01;           //  context, application context-name(1)
    szBuffer[nLength++] = 0x09;                                                 //   length 
    szBuffer[nLength++] = ASN_CLASS_UNIVERSAL | ASN_TYPE_OID;                   //   object identifier
    szBuffer[nLength++] = 0x07;                                                 //    length
    if(nAssociationType == DLMS_ASSOCIATION_LN) {                               //    OID
        memcpy(&szBuffer[nLength], mDlmsOidLn, 0x07);                           //     LN
    } else {
        memcpy(&szBuffer[nLength], mDlmsOidSn, 0x07);                           //     SN
    }
    nLength += 0x07;                                                        
    szBuffer[nLength++] = ASN_CLASS_CONTEXT | 0x0A;                             //  context, ACSE-requirements(10)
    szBuffer[nLength++] = 0x02;                                                 //   length
    szBuffer[nLength++] = 0x07; szBuffer[nLength++] = 0x80;                     //   value
    szBuffer[nLength++] = ASN_CLASS_CONTEXT | 0x0B;                             //  context, mechanism-name(11)
    szBuffer[nLength++] = 0x07;                                                 //   length
    memcpy(&szBuffer[nLength], mDlmsDefaultSecurityMachanism, 0x07);            //   default-COSEM-low-level-security-mechanism-name
    nLength += 0x07;
    szBuffer[nLength++] = ASN_CLASS_CONTEXT | ASN_CONSTRUCTED | 0x0C;           //  context, authentication-value(12)
    szBuffer[nLength++] = nAuthLength + 2;                                      //   length
    szBuffer[nLength++] = nAuthValueType;                                       //   authentication value type
    szBuffer[nLength++] = nAuthLength;                                          //    length
    memcpy(&szBuffer[nLength], pszAuthValue, nAuthLength);                      //    security value
    nLength += nAuthLength;
    szBuffer[nLength++] = ASN_CLASS_CONTEXT | ASN_CONSTRUCTED | 0x1E;           //  context, user-information(30)
    szBuffer[nLength++] = 0x10;                                                 //   length
    szBuffer[nLength++] = ASN_CLASS_UNIVERSAL | ASN_TYPE_OCTET_STRING;          //   octet string
    szBuffer[nLength++] = 0x0E;                                                 //    length
    szBuffer[nLength++] = 0x01;                                                 //    DLMS APDU CHOICE (InitiateRequest)
    szBuffer[nLength++] = 0x00;                                                 //    dedicated-key (FALSE)
    szBuffer[nLength++] = 0x00;                                                 //    response-allowed (FALSE)
    szBuffer[nLength++] = 0x00;                                                 //    propsed-quality-of-service(FALSE)
    szBuffer[nLength++] = 0x06;                                                 //    proposed-dlms-version-number(value 6)
    szBuffer[nLength++] = 0x5F; szBuffer[nLength++] = 0x1F;                     //    Conformace block (application 31)
    szBuffer[nLength++] = 0x04;                                                 //     length
    szBuffer[nLength++] = 0x00;                                                 //     number of unused block
    szBuffer[nLength++] = (nConformance >> 16) & 0x000000FF;                    //     conformance 0
    szBuffer[nLength++] = (nConformance >> 8) & 0x000000FF;                     //     conformance 1
    szBuffer[nLength++] = (nConformance) & 0x000000FF;                          //     conformance 2
    szBuffer[nLength++] = 0xFF; szBuffer[nLength++] = 0xFF;                     //    client-max-receive-pdu-size

    if((nError=MakeDlmsGenericFrame((BYTE *)&frame, src, dest, nType, szBuffer, nLength, NULL, 0, &nSize))!=DLMSERR_NOERROR)
    {
        return nError;    
    }
    if(nSize <= 0)
        return DLMSERR_SYSTEM_ERROR;

    (*pszDlmsFrame) = (BYTE *)MALLOC(nSize);
    if(*pszDlmsFrame == NULL) return DLMSERR_MEMORY_ERROR;

    memcpy(*pszDlmsFrame, (BYTE *)&frame, nSize);
    *nFrameLength = nSize;

    return nError;;
}

int dlmsMakeGetFrame(DLMSOBIS *obis, DLMSTAGVALUE *pParam, BYTE lastRxControl, BYTE src, UINT dest, BYTE **pszDlmsFrame, int *nFrameLength)
{
	DLMS_FRAME	frame;
	DLMS_GETREQ_PAYLOAD	payload;
	WORD        nSize=0;
	BYTE		nControl;
    int         nError;
    BYTE        nNull = 0;

	memset(&payload, 0, sizeof(DLMS_GETREQ_PAYLOAD));
    payload.destLSAP    = LSAP_DEST_ADDRESS;
    payload.srcLSAP     = LSAP_SRC_ADDRESS;
	payload.method		= DLMS_GET | DLMS_REQUEST;
	payload.reqFlag		= DLMS_NORMAL;
	payload.idPriority	= DLMS_INVOKE_ID | DLMS_INVOKE_PRIORITY;
	payload.cls			= BigToHostShort(obis->cls);
	payload.attr		= obis->attr;
	//payload.logical		= 0;
	memcpy(&payload.obis, &obis->code, sizeof(OBISCODE));	
	
	nControl = GetHdlcControl(lastRxControl, FALSE);
	if((nError=MakeDlmsGenericFrame((BYTE *)&frame, src, dest, nControl,
								(BYTE *)&payload, sizeof(DLMS_GETREQ_PAYLOAD), 
                                pParam == NULL ? &nNull : pParam->pPoint,
                                pParam == NULL ? 1 : pParam->nLength, &nSize))!=DLMSERR_NOERROR) 
    {
        return nError;
    }
    if(nSize <= 0)
        return DLMSERR_SYSTEM_ERROR;

    (*pszDlmsFrame) = (BYTE *)MALLOC(nSize);
    if(*pszDlmsFrame == NULL) return DLMSERR_MEMORY_ERROR;

    memcpy(*pszDlmsFrame, (BYTE *)&frame, nSize);
    *nFrameLength = nSize;
	
	return nError;
}

int dlmsMakeGetNextFrame(UINT nBlockCount, BYTE lastRxControl, BYTE src, UINT dest, BYTE **pszDlmsFrame, int *nFrameLength)
{
	DLMS_FRAME	frame;
	DLMS_GETBLOCKREQ_PAYLOAD	payload;
	BYTE		nControl;
	WORD        nSize=0;
    int         nError;

	memset(&payload, 0, sizeof(DLMS_GETBLOCKREQ_PAYLOAD));
    payload.destLSAP    = LSAP_DEST_ADDRESS;
    payload.srcLSAP     = LSAP_SRC_ADDRESS;
	payload.method		= DLMS_GET | DLMS_REQUEST;
	payload.reqFlag		= DLMS_DATABLOCK;
	payload.idPriority	= DLMS_INVOKE_ID | DLMS_INVOKE_PRIORITY;
	payload.nBlockCount	= BigToHostInt(nBlockCount);
	
	nControl = GetHdlcControl(lastRxControl, FALSE);
	if((nError=MakeDlmsGenericFrame((BYTE *)&frame, src, dest, nControl,
								(BYTE *)&payload, sizeof(DLMS_GETBLOCKREQ_PAYLOAD), NULL, 0, &nSize))!=DLMSERR_NOERROR) 
    {
        return nError;
    }
    if(nSize <= 0)
        return DLMSERR_SYSTEM_ERROR;

    (*pszDlmsFrame) = (BYTE *)MALLOC(nSize);
    if(*pszDlmsFrame == NULL) return DLMSERR_MEMORY_ERROR;

    memcpy(*pszDlmsFrame, (BYTE *)&frame, nSize);
    *nFrameLength = nSize;

	return nError;
}

BYTE * dlmsGetResPayload(DLMS_FRAME * pFrame)
{
    if(pFrame == NULL) return NULL;

    return getResPayload(pFrame);
}

/*
BOOL dlmsParseStream(DATASTREAM *pStream, BYTE *pszStream, int nLength)
{
    return m_ptheDlmsDataStream->ParseStream(pStream, pszStream, nLength);
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int dlmsSetTagValue(DLMSTAGVALUE * tagBuf, BYTE nType, void *pValue, BYTE nLen)
{
    return DlmsSetTagValue(tagBuf, nType, pValue, nLen);
}

void dlmsReqAddress(DLMS_HEADER *header, HDLC_REQ_ADDR *addr, BYTE *src, UINT *dest, BYTE *ctrl)
{
    return DlmsReqAddress((BYTE *)header, addr, src, dest, ctrl);
}
void dlmsResAddress(DLMS_HEADER * header, HDLC_RES_ADDR *addr, UINT *src, BYTE *dest, BYTE *ctrl)
{
    return DlmsResAddress((BYTE *)header, addr, src, dest, ctrl);
}

DLMSVARIABLE * dlmsAllocToken(int nLimit)
{
    return DlmsAllocToken(nLimit);
}

int	dlmsToken(DLMSVARIABLE *pToken, int nPrevTokenSize, int *nTokenSize, int nMaxToken, BYTE *pszBuffer, int nLength)
{
	*nTokenSize = 0;
    freeToken(pToken, nPrevTokenSize);
	DlmsGetTagToken(pToken, nTokenSize, nMaxToken, (DLMS_TAG *)pszBuffer, nLength);
	return DLMSERR_NOERROR;
}

void dlmsFreeToken(DLMSVARIABLE *pToken, int nCount)
{
    if(!pToken) return;
    DlmsFreeToken(pToken, nCount);
}

BYTE dlmsGetTagSize(BYTE *pTag)
{
    return DlmsGetTagSize((DLMS_TAG *)pTag);
}


