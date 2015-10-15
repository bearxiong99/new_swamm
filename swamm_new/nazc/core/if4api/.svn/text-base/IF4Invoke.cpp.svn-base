#include "common.h"
#include "if4invoke.h"
#include "IF4Util.h"
#include "MemoryDebug.h"

//////////////////////////////////////////////////////////////////////
// CIF4Invoke Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIF4Invoke::CIF4Invoke(const char *pszAddress, int nPort, int nTimeout)
{
	m_pInvoke = IF4API_NewInvoke(pszAddress, nPort, nTimeout);
}

CIF4Invoke::~CIF4Invoke()
{
	if (m_pInvoke != NULL)
	{
		IF4API_DeleteInvoke(m_pInvoke);
		m_pInvoke = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// CIF4Invoke Attribute
//////////////////////////////////////////////////////////////////////

IF4Invoke *CIF4Invoke::GetHandle() const
{
	return m_pInvoke;
}

char *CIF4Invoke::GetAddress()
{
	if (!m_pInvoke)
		return NULL;

	return (char *)m_pInvoke->szAddress;
}

int CIF4Invoke::GetPort()
{
	if (!m_pInvoke)
		return 0;

	return m_pInvoke->port;
}

int CIF4Invoke::GetError() const
{
	return m_pInvoke->nError;
}

MIBValue *CIF4Invoke::ResultAt(const char *pszOid)
{
	return IF4API_GetResult(m_pInvoke, pszOid);
}

MIBValue *CIF4Invoke::ResultAtName(const char *pszName)
{
	return IF4API_GetResultByName(m_pInvoke, pszName);
}

//////////////////////////////////////////////////////////////////////
// CIF4Invoke Operations
//////////////////////////////////////////////////////////////////////

int CIF4Invoke::Command(const char *pszOid, BYTE nAttribute)
{
	return IF4API_Command(m_pInvoke, pszOid, nAttribute);
}

int CIF4Invoke::Alarm(BYTE srcType, EUI64 *id, TIMESTAMP *pTime, WORD nLength, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx)
{
	return IF4API_Alarm(m_pInvoke, srcType, id, pTime, nLength, pMessage, pSysTime, idx);
}

int CIF4Invoke::Event(char *pszEvent, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime)
{
	return IF4API_Event(m_pInvoke, pszEvent, srcType, srcID, pTime);
}

int CIF4Invoke::SendData(WORD nCount, char *pszData, int nLength)
{
	return IF4API_SendData(m_pInvoke, nCount, (BYTE *)pszData, nLength);
}

int CIF4Invoke::SendMeteringData(WORD nCount, char *pszData, int nLength)
{
	return IF4API_SendMeteringData(m_pInvoke, nCount, (BYTE *)pszData, nLength);
}

int CIF4Invoke::SendFile(char *pszFileName, BYTE nChannel)
{
	return IF4API_SendFile(m_pInvoke, pszFileName, nChannel);
}

int	CIF4Invoke::OpenSession()
{
	return IF4API_OpenSession(m_pInvoke);
}

int	CIF4Invoke::SendSession(WORD nCount, BYTE *pszData, int len, BYTE nSeq)
{
	return IF4API_SendSession(m_pInvoke, nCount, pszData, len, nSeq);
}

int	CIF4Invoke::CloseSession()
{
	return IF4API_CloseSession(m_pInvoke);
}

//////////////////////////////////////////////////////////////////////
// CIF4Invoke Operations
//////////////////////////////////////////////////////////////////////

BOOL CIF4Invoke::AddParam(const char *pszOid)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_OID, 0);
	ChainLink(pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParamMIBValue(const MIBValue *pValue)
{
	IF4API_AddParamMIBValue(m_pInvoke, pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParamValue(const char *pszOid)
{
	IF4API_AddParamValue(m_pInvoke, pszOid);
	return TRUE;
}

BOOL CIF4Invoke::AddParamFormat(const char *pszOid, BYTE nType, const void *p, int nLength)
{
	MIBValue	*pValue;
	int			len = nLength;

	if (len == -1)
	{
		switch(nType) {
		  case VARSMI_EUI64 :		len = sizeof(EUI64); break;
		  case VARSMI_IPADDR :		len = sizeof(UINT); break;
		  case VARSMI_TIMESTAMP :	len = sizeof(TIMESTAMP); break;
		  case VARSMI_TIMEDATE :	len = sizeof(TIMEDATE); break;
          case VARSMI_BOOL :        len = sizeof(BOOL); break;
		}
	}

	pValue = AllocateValue(pszOid, nType, len);
	pValue->stream.p = (char *)MALLOC(len);
    memcpy(pValue->stream.p, p, len);
	ChainLink(pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParam(const char *pszOid, const void *p, int nLength)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_OPAQUE, nLength);
	pValue->stream.p = (char *)MALLOC(nLength);
    memcpy(pValue->stream.p, p, nLength);
	ChainLink(pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParam(const char *pszOid, const BYTE *p)
{
	MIBValue	*pValue;
	WORD		len;

	len = strlen((const char *)p);
	pValue = AllocateValue(pszOid, VARSMI_STRING, len);
	pValue->stream.p = (char *)MALLOC(len+1);
    memcpy(pValue->stream.p, p, len);
	pValue->stream.p[len] = '\0';
	ChainLink(pValue);
	return pValue ? TRUE : FALSE;
}

BOOL CIF4Invoke::AddParam(const char *pszOid, const char *pszValue)
{
	return AddParam(pszOid, (const BYTE *)pszValue);
}

BOOL CIF4Invoke::AddParam(const char *pszOid, const OID3 *oid)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_OID, sizeof(OID3));
	memcpy(&pValue->stream.id, oid, sizeof(OID3));
	ChainLink(pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParam(const char *pszOid, UINT nValue)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_UINT, sizeof(UINT));
	pValue->stream.u32 = nValue;
	ChainLink(pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParam(const char *pszOid, signed int nValue)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_INT, sizeof(int));
	pValue->stream.s32 = nValue;
	ChainLink(pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParam(const char *pszOid, WORD nValue)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_WORD, sizeof(WORD));
	pValue->stream.u16 = nValue;
	ChainLink(pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParam(const char *pszOid, signed short nValue)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_SHORT, sizeof(signed short));
	pValue->stream.s16 = nValue;
	ChainLink(pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParam(const char *pszOid, BYTE nValue)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_BYTE, sizeof(BYTE));
	pValue->stream.u8 = nValue;
	ChainLink(pValue);
	return TRUE;
}

BOOL CIF4Invoke::AddParam(const char *pszOid, signed char nValue)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_CHAR, sizeof(signed char));
	pValue->stream.s8 = nValue;
	ChainLink(pValue);
	return TRUE;
}

#ifndef _WIN32
BOOL CIF4Invoke::AddParam(const char *pszOid, BOOL bValue)
{
	MIBValue	*pValue;

	pValue = AllocateValue(pszOid, VARSMI_BOOL, sizeof(BYTE));
	pValue->stream.u8 = (BYTE)bValue;
	ChainLink(pValue);
	return TRUE;
}
#endif

//////////////////////////////////////////////////////////////////////
// CIF4Invoke Memeber Function
//////////////////////////////////////////////////////////////////////

MIBValue *CIF4Invoke::AllocateValue(const char *pszOid, BYTE nType, WORD len)
{
	MIBValue	*pValue;

	pValue = (MIBValue *)MALLOC(sizeof(MIBValue));
	if (pValue == NULL)
		return NULL;

	memset(pValue, 0, sizeof(MIBValue));
	StringToOid(pszOid, &pValue->oid);
	pValue->type = nType;
	pValue->len	 = len;
	return pValue;
}

void CIF4Invoke::ChainLink(MIBValue *pValue)
{
	if (m_pInvoke->pParamNode == NULL)
	{
		m_pInvoke->pParamNode = pValue;
		m_pInvoke->pLastParam = pValue;
	}
	else
	{
		m_pInvoke->pLastParam->pNext = pValue;
		m_pInvoke->pLastParam = pValue;
	}
	m_pInvoke->nParamCount++;
}
