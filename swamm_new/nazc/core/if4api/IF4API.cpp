#include "common.h"
#include "IF4Service.h"
#include "IF4Util.h"

#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////

CIF4Service		m_theIF4Service;
int     m_nIF4CmdLogSize = 10000;

//////////////////////////////////////////////////////////////////////
// Startup/Shutdown Functions
//////////////////////////////////////////////////////////////////////

int IF4API_Initialize(int nPort, IF4_COMMAND_TABLE *pTable)
{
	XDEBUG(" #######################################debug sungyeung IF4API_Initialize : %s : %d \n", __FILE__, __LINE__);
	return m_theIF4Service.Startup(nPort, pTable);
}

int IF4API_Destroy()
{
	return m_theIF4Service.Shutdown();
}

//////////////////////////////////////////////////////////////////////
// Command Invoke Functions
//////////////////////////////////////////////////////////////////////

IF4Invoke *IF4API_NewInvoke(const char *pszAddress, int nPort, int nTimeout)
{
	return m_theIF4Service.NewInvoke(pszAddress, nPort, nTimeout);
}

int IF4API_DeleteInvoke(IF4Invoke *pInvoke)
{
	return m_theIF4Service.DeleteInvoke(pInvoke);
}

int IF4API_AddParamMIBValue(IF4Invoke *pInvoke, const MIBValue *pValue)
{
	MIBValue	*pCopy;

	pCopy = VARAPI_DupValue(pValue);

	if (pInvoke->pParamNode == NULL)
	{
		pInvoke->pParamNode = pCopy;
		pInvoke->pLastParam = pCopy;
	}
	else
	{
		pInvoke->pLastParam->pNext = pCopy;
		pInvoke->pLastParam = pCopy;
	}
	pInvoke->nParamCount++;
	return IF4ERR_NOERROR;
}

int IF4API_UpdateParamMIBValue(IF4Invoke *pInvoke, const MIBValue *pValue)
{
	MIBValue	*pCopy;
	MIBValue	*pHead, *pPrev;

    if(pInvoke == NULL) 
		return IF4ERR_INVALID_INVOKE;

    if(pValue == NULL) 
		return IF4ERR_NOERROR;

	pCopy = VARAPI_DupValue(pValue);
    pHead = pInvoke->pParamNode;

	for(pPrev=NULL; pHead; pPrev=pHead, pHead=pHead->pNext)
	{
        if(!memcmp(&pHead->oid, &pCopy->oid, sizeof(OID3)))
        {
            if(pPrev)   pPrev->pNext = pCopy;
            else        pInvoke->pParamNode = pCopy;
            if(pHead->pNext == NULL) pInvoke->pLastParam = pCopy;
            pCopy->pNext = pHead->pNext;
            VARAPI_FreeValue(pHead);
            return IF4ERR_NOERROR;
        }
	}
    /** Update될게 없다면 추가 한다 */
    pInvoke->pLastParam->pNext = pCopy;
    pInvoke->pLastParam = pCopy;

	return IF4ERR_NOERROR;
}

BOOL IF4API_IsParamIn(IF4Invoke *pInvoke, OID3 *oid)
{
	MIBValue	*pHead;

    if(pInvoke == NULL) 
		return FALSE;

    if(oid == NULL) 
		return FALSE;

    pHead = pInvoke->pParamNode;

	for(; pHead; pHead=pHead->pNext)
	{
        if(!memcmp(&pHead->oid, oid, sizeof(OID3)))
        {
            return TRUE;
        }
	}

	return FALSE;
}

int IF4API_AddParamVoid(IF4Invoke *pInvoke, const char *pszOid)
{
	OID3	oid;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	return m_theIF4Service.AddParam(pInvoke, &oid, VARSMI_UNKNOWN, 0, 0, 0);
}

int IF4API_AddParamValue(IF4Invoke *pInvoke, const char *pszOid)
{
	OID3		oid;
	MIBValue	*pValue;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	pValue = VARAPI_GetValue(&oid, NULL);
	if (pValue != NULL)
	{
		if (pValue->type == VARSMI_STRING)
			pValue->len = pValue->stream.p ? strlen(pValue->stream.p) : 0;

		if (pInvoke->pParamNode == NULL)
		{
			pInvoke->pParamNode = pValue;
			pInvoke->pLastParam = pValue;
		}
		else
		{
			pInvoke->pLastParam->pNext = pValue;
			pInvoke->pLastParam = pValue;
		}
		pInvoke->nParamCount++;
		return IF4ERR_NOERROR;
	}
	return IF4ERR_INVALID_OID;
}

int IF4API_AddParamFormat(IF4Invoke *pInvoke, const char *pszOid, BYTE nType, const char *pszValue, WORD len)
{
	OID3	oid;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	return m_theIF4Service.AddParam(pInvoke, &oid, nType, 0, pszValue, len);
}

int IF4API_AddParamOpaque(IF4Invoke *pInvoke, const char *pszOid, const char *pszValue, WORD len)
{
	OID3	oid;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	return m_theIF4Service.AddParam(pInvoke, &oid, VARSMI_OPAQUE, 0, pszValue, len);
}

int IF4API_AddParamString(IF4Invoke *pInvoke, const char *pszOid, const char *pszValue)
{
	OID3	oid;
	int		nLength;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	nLength = pszValue ? strlen(pszValue) : 0;
	return m_theIF4Service.AddParam(pInvoke, &oid, VARSMI_STRING, 0, pszValue, nLength);
}

int IF4API_AddParamNumber(IF4Invoke *pInvoke, const char *pszOid, BYTE nType, int nValue)
{
	OID3	oid;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	return m_theIF4Service.AddParam(pInvoke, &oid, nType, nValue, NULL, 0);
}

MIBValue *IF4API_GetResult(IF4Invoke *pInvoke, const char *pszOid)
{
	MIBValue	*pValue;
	OID3		oid;

	if (pInvoke == NULL)
		return NULL;

	if (!StringToOid(pszOid, &oid))
		return NULL;

	pValue = pInvoke->pResultNode;
	for(; pValue; pValue=pValue->pNext)
	{
		if (memcmp(&pValue->oid, &oid, sizeof(OID3)) == 0)
			return pValue;
	}
	return NULL;
}

MIBValue *IF4API_GetResultByName(IF4Invoke *pInvoke, const char *pszName)
{
	VAROBJECT 	*pObject;
	MIBValue	*pValue;

	pObject = VARAPI_GetObjectByName(pszName);
	if (pObject == NULL)
		return NULL;

	pValue = pInvoke->pResultNode;
	for(; pValue; pValue=pValue->pNext)
	{
		if (memcmp(&pValue->oid, &pObject->var.oid, sizeof(OID3)) == 0)
			return pValue;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// Command Result Functions
//////////////////////////////////////////////////////////////////////

int IF4API_AddResultFormat(IF4Wrapper *pWrapper, const char *pszOid, BYTE nType, const void *pValue, int nLength)
{
	OID3	oid;

	if (!pszOid || !*pszOid)
		return IF4ERR_INVALID_OID;
				
	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	return m_theIF4Service.AddResult(pWrapper, &oid, nType, 0, (const char *)pValue, nLength);
}

int IF4API_AddResultOpaque(IF4Wrapper *pWrapper, const char *pszOid, const void *pValue, int nLength)
{
	OID3	oid;

	if (!pszOid || !*pszOid)
		return IF4ERR_INVALID_OID;
				
	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	return m_theIF4Service.AddResult(pWrapper, &oid, VARSMI_OPAQUE, 0, (const char *)pValue, nLength);
}

int IF4API_AddResultString(IF4Wrapper *pWrapper, const char *pszOid, const char *pszString)     
{
	OID3	oid;
	WORD	nLength;

	if (!pszOid || !*pszOid)
		return IF4ERR_INVALID_OID;
				
	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	nLength = pszString && *pszString ? strlen(pszString) : 0;
	return m_theIF4Service.AddResult(pWrapper, &oid, VARSMI_STRING, 0, pszString, nLength);
}

int IF4API_AddResultNumber(IF4Wrapper *pWrapper, const char *pszOid, BYTE nType, int nValue)
{
	OID3	oid;
	int		len = 1;

	if (!pszOid || !*pszOid)
		return IF4ERR_INVALID_OID;
				
	if (!StringToOid(pszOid, &oid))
		return IF4ERR_INVALID_OID;

	switch(nType) {
	  case VARSMI_CHAR :
	  case VARSMI_BYTE :
	  case VARSMI_BOOL :
		   len = 1;
		   break;
	  case VARSMI_SHORT :
	  case VARSMI_WORD :
		   len = 2;
		   break;
	  case VARSMI_INT :
	  case VARSMI_UINT :
		   len = 4;
		   break;
	}

	return m_theIF4Service.AddResult(pWrapper, &oid, nType, nValue, NULL, len);
}

int IF4API_AddResultMIBValue(IF4Wrapper *pWrapper, MIBValue *pValue)
{
	MIBValue	*pCopy;

	pCopy = VARAPI_DupValue(pValue);
	if (pWrapper->pResult == NULL)
		 pWrapper->pResult = pCopy;
	else pWrapper->pLast->pNext = pCopy;

	pWrapper->pLast = pCopy;
	pWrapper->nResult++;
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Service Functions
//////////////////////////////////////////////////////////////////////

int IF4API_Cancel(IF4Invoke *pInvoke)
{
	return m_theIF4Service.Cancel(pInvoke);
}

int IF4API_Command(IF4Invoke *pInvoke, const char *pszCommand, BYTE nAttr)
{
	OID3	oid;

	if (!pszCommand || !*pszCommand)
		return IF4ERR_INVALID_OID;

	if (!StringToOid(pszCommand, &oid))
		return IF4ERR_INVALID_COMMAND;

	return m_theIF4Service.Command(pInvoke, &oid, nAttr);
}

int IF4API_AlarmFrame(char *pszBuffer, IF4Invoke *pInvoke, BYTE srcType, EUI64 *id, TIMESTAMP *pTime, int nLength, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx)
{
	return m_theIF4Service.AlarmFrame(pInvoke, pszBuffer, srcType, id, pTime, nLength, pMessage, pSysTime, idx);
}

int IF4API_Alarm(IF4Invoke *pInvoke, BYTE srcType, EUI64 *id, TIMESTAMP *pTime, int nLength, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx)
{
	return m_theIF4Service.Alarm(pInvoke, srcType, id, pTime, nLength, pMessage, pSysTime, idx);
}

int IF4API_EventFrame(char *pszBuffer, IF4Invoke *pInvoke, char *pszEvent, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime)
{
	OID3	oid;

	if (!StringToOid(pszEvent, &oid))
		return 0;

	return m_theIF4Service.EventFrame(pInvoke, pszBuffer, &oid, srcType, srcID, pTime);
}

int IF4API_Event(IF4Invoke *pInvoke, char *pszEvent, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime)
{
	OID3	oid;

	if (!pszEvent || !*pszEvent)
		return IF4ERR_INVALID_OID;

	if (!StringToOid(pszEvent, &oid))
		return IF4ERR_INVALID_EVENT;

	return m_theIF4Service.Event(pInvoke, &oid, srcType, srcID, pTime);
}

int IF4API_SendData(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int nLength)
{
	return m_theIF4Service.SendData(pInvoke, nCount, pszData, nLength);
}

int IF4API_SendDataFile(IF4Invoke *pInvoke, BYTE nType, char *pszFileName, BOOL bCompressed)
{
	return m_theIF4Service.SendDataFile(pInvoke, nType, pszFileName, bCompressed);
}

int IF4API_SendMeteringData(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int nLength)
{
	return m_theIF4Service.SendMeteringData(pInvoke, nCount, pszData, nLength);
}

int IF4API_SendFile(IF4Invoke *pInvoke, char *pszFileName, BYTE nChannel)
{
	return m_theIF4Service.SendFile(pInvoke, pszFileName, nChannel);
}

/** 신규 Metering Type R 을 전송 한다.
  */
int IF4API_SendTypeR(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int nLength)
{
	return m_theIF4Service.SendTypeR(pInvoke, nCount, pszData, nLength);
}

int IF4API_OpenSession(IF4Invoke *pInvoke)
{
	return m_theIF4Service.OpenSession(pInvoke);
}

int IF4API_SendSession(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int len, BYTE nSeq)
{
	return m_theIF4Service.SendSession(pInvoke, nCount, pszData, len, nSeq);
}

int IF4API_CloseSession(IF4Invoke *pInvoke)
{
	return m_theIF4Service.CloseSession(pInvoke);
}

//////////////////////////////////////////////////////////////////////
// Utility Functions
//////////////////////////////////////////////////////////////////////

void IF4API_EnableCompress(BOOL bEnable)
{
	m_theIF4Service.EnableCompress(bEnable);
}

void IF4API_SetID(UINT nID)
{
	m_theIF4Service.SetMcuID(nID);
}

UINT IF4API_GetID()
{
	return m_theIF4Service.GetMcuID();
}

int IF4API_TIMESTAMPTOSTR(char *pszTime, TIMESTAMP *pTime)
{
	TIMESTAMP	tmNow;
	time_t		now;
	struct	tm	when;  
 
    if (!pszTime)
        return IF4ERR_INVALID_BUFFER;

	if (pTime == NULL)
	{
		time(&now);
		when = *localtime(&now);
		tmNow.year	= when.tm_year + 1900;
		tmNow.mon	= when.tm_mon + 1;
		tmNow.day	= when.tm_mday;
		tmNow.hour	= when.tm_hour;
		tmNow.min	= when.tm_min;
		tmNow.sec	= when.tm_sec;
		pTime = &tmNow;
	}   
 
    sprintf(pszTime, "%04d/%02d/%02d %02d:%02d:%02d",
			pTime->year, pTime->mon, pTime->day,
			pTime->hour, pTime->min, pTime->sec);
	return IF4ERR_NOERROR;
}

void IF4API_DumpMIBValue(MIBValue *pValue, int nCount)
{
	if (!pValue || !nCount)
		return;

	DumpMIBValue(pValue, nCount);
}

void IF4API_FreeMIBValue(MIBValue *pValue)
{
	if (!pValue)
		return;

	FreeLinkedValue(pValue);
}

const char *IF4API_GetErrorMessage(int nError)
{
	return GetIF4ErrorMessage(nError);
}

MIBValue * IF4API_NewMIBValue(const char *pszOid, int nType, int nValue, const char *pszValue, int nLength)
{
    OID3 oid;
    StringToOid(pszOid, &oid);
    return m_theIF4Service.NewValue(&oid, nType, nValue, pszValue, nLength);
}

/** Command History Log에 사용될 LogSize Max 값을 지정한다.
  *
  * @warning Agent에서 m_nCmdLogSize 값을 변경할 때 마다 호출되어야 한다.
  *
  * @param logSize Max Log Size(kb)
  */
void IF4API_SetMaxLogSize(int logSize)
{
    m_nIF4CmdLogSize = logSize;
}

//////////////////////////////////////////////////////////////////////
// Callback Functions
//////////////////////////////////////////////////////////////////////

int IF4API_SetUserCallback(PFNIF4ONDATAFILE pfnOnDataFile, PFNIF4ONDATA pfnOnData, 
        PFNIF4ONEVENT pfnOnEvent, PFNIF4ONALARM pfnOnAlarm, PFNIF4ONTYPER pfnOnTypeR)
{
	return m_theIF4Service.SetUserCallback(pfnOnDataFile, pfnOnData, pfnOnEvent, pfnOnAlarm, pfnOnTypeR);
}
	
