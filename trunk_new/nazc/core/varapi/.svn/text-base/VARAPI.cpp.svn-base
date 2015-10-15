#include "common.h"
#include "VARService.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

//////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////

#define OID_ACCEPT      "0123456789."

CVARService		m_theVARService;
BOOL			m_bGlobalEnableModify = TRUE;

//////////////////////////////////////////////////////////////////////
// Startup/Shutdown Functions
//////////////////////////////////////////////////////////////////////

int VARAPI_Initialize(const char *pszFileName, VAROBJECT *pRoot, BOOL bSave)
{
	return m_theVARService.Initialize(pszFileName, pRoot, bSave);
}

int VARAPI_Destroy()
{
	return m_theVARService.Destroy();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

void VARAPI_EnableModify(BOOL bEnable)
{
	m_bGlobalEnableModify = bEnable;
}

void VARAPI_SetModify()
{
	m_theVARService.SetModify(TRUE);
}

int VARAPI_Load()
{
	return m_theVARService.Load();
}

int VARAPI_Save(BOOL bAll)
{
	return m_theVARService.Save(bAll);
}

BOOL VARAPI_Serialize(char *pPath, MIBValue *pValues)
{
	return m_theVARService.Serialize(pPath, pValues);
}

BOOL VARAPI_Deserialize(char *pPath, MIBValue **pValues, int * nCount)
{
	return m_theVARService.Deserialize(pPath, pValues, nCount);
}

//////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////

BOOL VARAPI_EnumObject(PFNENUMVAROBJECT pfnCallback, void *pParam)
{
	return m_theVARService.EnumObject(pfnCallback, pParam);
}

VAROBJECT *VARAPI_GetObject(OID3 *oid)
{
	return m_theVARService.GetObject(oid);
}

VAROBJECT *VARAPI_GetObjectByName(const char *pszName)
{
	return m_theVARService.GetObjectByName(pszName);
}

MIBValue *VARAPI_GetValue(OID3 *oid, MIBValue *pParam)
{
	VAROBJECT	*pObject;
	MIBValue	*pValue;

	pObject = VARAPI_GetObject(oid);
	if (pObject == NULL)
		return NULL;

	if (pObject->pfnHandler != NULL)
	{
		pValue = (MIBValue *)MALLOC(sizeof(MIBValue));
		memset(pValue, 0, sizeof(MIBValue));
		memcpy(&pValue->oid, &pObject->var.oid, sizeof(OID3));
		pValue->type = pObject->var.type;
		pObject->pfnHandler(pObject, VARMETHOD_GET, pValue, NULL, NULL, pParam);
		pObject->pfnHandler(pObject, VARMETHOD_MOVE, pValue, NULL, NULL, pParam);
	}
	else
	{
		// Simple Get Method
		pValue = VARAPI_DupValue(&pObject->var);
	}

	return pValue;
}

int VARAPI_CopyValue(const VAROBJECT *pObject, MIBValue *pValue)
{
	int		len;

	pValue->len	= pObject->var.len;
	switch(pObject->var.type) {
	  case VARSMI_BOOL :
		   pValue->stream.u8 = pObject->var.stream.u8;
		   break;
	  case VARSMI_BYTE :
		   pValue->stream.u8 = pObject->var.stream.u8;
	 	   break;
	  case VARSMI_WORD :
		   pValue->stream.u16 = pObject->var.stream.u16;
		   break;
	  case VARSMI_UINT :
		   pValue->stream.u32 = pObject->var.stream.u32;
		   break;
	  case VARSMI_CHAR :
		   pValue->stream.s8 = pObject->var.stream.s8;
		   break;
	  case VARSMI_SHORT :
		   pValue->stream.s16 = pObject->var.stream.s16;
		   break;
	  case VARSMI_INT :
		   pValue->stream.s32 = pObject->var.stream.s32;
		   break;
	  case VARSMI_OID :
		   memcpy(&pValue->stream.id, &pObject->var.stream.id, sizeof(OID3));
		   break;
	  case VARSMI_STRING :
		   len = strlen(pObject->var.stream.p);
		   pValue->stream.p = (char *)MALLOC(len+1);
		   memcpy(pValue->stream.p, pObject->var.stream.p, len);
		   pValue->stream.p[len] = 0;
		   break;
	  case VARSMI_STREAM :
	  case VARSMI_OPAQUE :
		   pValue->stream.p = NULL;
		   if (pObject->var.len > 0)
		   {
		   	  pValue->stream.p = (char *)MALLOC(pValue->len);
		      memcpy(pValue->stream.p, pObject->var.stream.p, pValue->len);
		   }
		   break;
	  case VARSMI_EUI64 :
	  case VARSMI_IPADDR :
	  case VARSMI_TIMESTAMP :
	  case VARSMI_TIMEDATE :
		   pValue->stream.p = (char *)MALLOC(pValue->len);
		   memcpy(pValue->stream.p, pObject->var.stream.p, pValue->len);
		   break;
	}
	return VARERR_NOERROR;
}

MIBValue *VARAPI_DupValue(const MIBValue *pValue)
{
	MIBValue	*pCopy;
	int			len;

    if(!pValue) return NULL;

	pCopy = (MIBValue *)MALLOC(sizeof(MIBValue));
	if (pCopy == NULL) return NULL;

	memcpy(pCopy, pValue, sizeof(MIBValue));
	switch(pValue->type) {
	  case VARSMI_STRING :
           len = (pValue->len != 0) ? strlen(pValue->stream.p) : 0;
           pCopy->stream.p = (char *)MALLOC(len+1);
           memcpy(pCopy->stream.p, pValue->stream.p, len);
           pCopy->stream.p[len] = '\0';
           pCopy->len = len;
		   break;
	  case VARSMI_STREAM :
	  case VARSMI_OPAQUE :
	  case VARSMI_EUI64 :
	  case VARSMI_IPADDR :
	  case VARSMI_TIMESTAMP :
	  case VARSMI_TIMEDATE :	
		   if (pValue->len > 0)
		   {
			   pCopy->stream.p = (char *)MALLOC(pValue->len);
			   memcpy(pCopy->stream.p, pValue->stream.p, pValue->len);
		   }
		   break;
	}
	return pCopy;
}

MIBValue *VARAPI_DupValueList(const MIBValue *pValue, int nCount)
{
    int i;
    MIBValue *pCopy = NULL, *pHead = NULL;

    if(!pValue || nCount <= 0) return NULL;

    pHead = VARAPI_DupValue(pValue);
    pValue = pValue->pNext;
    pCopy = pHead;

    for(i=1;i<nCount && pValue;i++, pCopy=pCopy->pNext, pValue=pValue->pNext) {
        pCopy->pNext = VARAPI_DupValue(pValue);
    }

    return pHead;
}

int VARAPI_ReplaceValue(VAROBJECT *pObject, const MIBValue *pValue)
{
	if (pObject == NULL)
		return VARERR_INVALID_OID;

	if (pObject->var.len == 0)
	    return VARERR_INVALID_BUFFER;

	switch(pValue->type) {
	  case VARSMI_STRING :
	  case VARSMI_STREAM :
	  case VARSMI_OPAQUE :
	  case VARSMI_EUI64 :
	  case VARSMI_IPADDR :
	  case VARSMI_TIMESTAMP :
	  case VARSMI_TIMEDATE :	
		   memset(pObject->var.stream.p, 0, pObject->var.len);
		   memcpy(pObject->var.stream.p, pValue->stream.p, MIN(pValue->len, pObject->var.len));
		   break;
	  default :
		   pObject->var.stream.u32 = pValue->stream.u32;
		   break;
	}
	return VARERR_NOERROR;
}

int VARAPI_UpdateValue(const char *pszName, const char *pszValue, int nLength)
{
	VAROBJECT	*pObject;

	pObject = VARAPI_GetObjectByName(pszName);
	if (pObject == NULL)
		return VARERR_INVALID_OID;

	if (pObject->var.len == 0)
	    return VARERR_INVALID_BUFFER;

	switch(pObject->var.type) {
	  case VARSMI_STRING :
	  case VARSMI_STREAM :
	  case VARSMI_OPAQUE :
	  case VARSMI_EUI64 :
	  case VARSMI_IPADDR :
	  case VARSMI_TIMESTAMP :
	  case VARSMI_TIMEDATE :	
		   memset(pObject->var.stream.p, 0, pObject->var.len);
		   memcpy(pObject->var.stream.p, pszValue, MIN(nLength, pObject->var.len));
		   break;
	}
	return VARERR_NOERROR;
}

int VARAPI_SetValue(MIBValue *pValue, MIBValue *pParam)
{
	VAROBJECT	*pObject;
	int			nError;

	if (pValue == NULL)
		return VARERR_INVALID_OID;

	pObject = VARAPI_GetObject(&pValue->oid);
	if (pObject == NULL)
		return VARERR_INVALID_OID;
	
	if (pObject->pfnHandler != NULL)
	{
		nError = pObject->pfnHandler(pObject, VARMETHOD_SET, pValue, NULL, NULL, pParam);
		if (nError < 0) return (nError == -1) ? VARERR_INVALID_VALUE : VARERR_SET_ERROR;
	}
	else
	{
		if (pObject->var.len == 0)
		    return VARERR_INVALID_BUFFER;

		switch(pValue->type) {
		  case VARSMI_STRING :
		  case VARSMI_STREAM :
		  case VARSMI_OPAQUE :
		  case VARSMI_EUI64 :
		  case VARSMI_IPADDR :
		  case VARSMI_TIMESTAMP :
		  case VARSMI_TIMEDATE :	
			   memset(pObject->var.stream.p, 0, pObject->var.len);
			   memcpy(pObject->var.stream.p, pValue->stream.p, MIN(pValue->len, pObject->var.len));
			   break;
	 	 default :
			   pObject->var.stream.u32 = pValue->stream.u32;
			   break;
		}
	}
	return VARERR_NOERROR;
}

int VARAPI_GetIndexValue(VAROBJECT *pObject, void *pValue, MIBValue *pParam)
{
	MIBValue	value;

	memset(&value, 0, sizeof(MIBValue));
	memcpy(&value.oid, &pObject->var.oid, sizeof(OID3));
	value.type = pObject->var.type;

	switch(pObject->var.type) {
	  case VARSMI_STRING :
		   pObject->pfnHandler(pObject, VARMETHOD_GET, &value, NULL, NULL, pParam);
		   pObject->pfnHandler(pObject, VARMETHOD_MOVE, &value, NULL, NULL, pParam);
		   if (value.stream.p != NULL)
		   {
			   memcpy(pValue, value.stream.p, strlen(value.stream.p));
			   FREE(value.stream.p);
		   }
		   break;
	  case VARSMI_STREAM :
	  case VARSMI_OPAQUE :
	  case VARSMI_EUI64 :
	  case VARSMI_IPADDR :
	  case VARSMI_TIMESTAMP :
	  case VARSMI_TIMEDATE :
		   pObject->pfnHandler(pObject, VARMETHOD_GET, &value, NULL, NULL, pParam);
		   pObject->pfnHandler(pObject, VARMETHOD_MOVE, &value, NULL, NULL, pParam);
		   if (value.stream.p != NULL)
		   {
			   memcpy(pValue, value.stream.p, value.len);
			   FREE(value.stream.p);
		   }
		   break;
	  default :
		   pObject->pfnHandler(pObject, VARMETHOD_GET, &value, NULL, NULL, pParam);
		   pObject->pfnHandler(pObject, VARMETHOD_MOVE, &value, NULL, NULL, pParam);
		   memcpy(pValue, &value.stream.u32, value.len);
		   break;
	}
	return VARERR_NOERROR;
}

int VARAPI_GetValueList(OID3 *oid, MIBValue **pList, int *nCount)
{
	return VARERR_NOERROR;
}

int VARAPI_FreeValue(MIBValue *pValue)
{
	switch(pValue->type) {
	  case VARSMI_STRING :
	  case VARSMI_STREAM :
	  case VARSMI_OPAQUE :
	  case VARSMI_EUI64 :
	  case VARSMI_IPADDR :
	  case VARSMI_TIMESTAMP :
	  case VARSMI_TIMEDATE :	
		   if (pValue->stream.p != NULL)
			   FREE(pValue->stream.p);
		   break;
	}
	FREE(pValue);
	return VARERR_NOERROR;
}

int VARAPI_FreeNode(MIBValue *pNode)
{
	MIBValue	*pValue, *pFree;
	
	if (pNode == NULL)
		return VARERR_INVALID_VALUE;

	pValue = pNode;
	for(; pValue;)
	{
		pFree = pValue;
		pValue = pValue->pNext;
		VARAPI_FreeValue(pFree);	
	}
	return VARERR_NOERROR;
}

int VARAPI_FreeValueList(MIBValue *pList, int nCount)
{
	int		i;

	for(i=0; i<nCount; i++)
	{
		switch(pList[i].type) {
		  case VARSMI_STRING :
		  case VARSMI_STREAM :
		  case VARSMI_OPAQUE :
		  case VARSMI_EUI64 :
		  case VARSMI_IPADDR :
		  case VARSMI_TIMESTAMP :
		  case VARSMI_TIMEDATE :	
			   if (pList[i].stream.p != NULL)
				   FREE(pList[i].stream.p);
			   break;
		}
	}
	FREE(pList);
	return VARERR_NOERROR;
}

int VARAPI_OidCompare(OID3 *id, const char *pszOid)
{
	OID3	oid;

	memset(&oid, 0, sizeof(OID3));
	VARAPI_StringToOid(pszOid, &oid);
	return memcmp(id, &oid, sizeof(OID3));
}

const char *VARAPI_OidToName(OID3 *oid, char *pszName)
{
	VAROBJECT	*pObject;

	if (!oid || !pszName)
		return NULL;

	*pszName = '\0';
	pObject = VARAPI_GetObject(oid);
	if (pObject == NULL)
		return NULL;

	strcpy(pszName, pObject->pszName);
	return pObject->pszName;
}

BOOL VARAPI_StringToOid(const char *pszOid, OID3 *pOid)
{
	int		i, len, dot=0;
	char	*p[5];
	char	c;

	if (!pszOid || !pOid)
		return FALSE;

    char    *oid = const_cast<char *>(pszOid);

	p[0] = oid;
	p[1] = p[2] = NULL;
	len  = strlen(oid);
	for(i=0; i<len; i++)
	{
		c = oid[i];
		if (!strchr(OID_ACCEPT, c))
			return FALSE;
		if (c == '.')
		{
			dot++;
			if (dot > 2)
				return FALSE;

			p[dot] = &oid[i+1];
		}
	}

    pOid->id1   = p[0] ? atoi(p[0]) : 0;
    pOid->id2   = p[1] ? atoi(p[1]) : 0;
    pOid->id3   = p[2] ? atoi(p[2]) : 0;
	
	return TRUE;
}

BOOL VARAPI_OidToString(OID3 *pOid, char *pszOid)
{
    if (!pOid || !pszOid)
        return FALSE;

    if (pOid->id1 == 0)
         sprintf(pszOid, "0.0.0");
    else if (pOid->id2 == 0)
         sprintf(pszOid, "%0d", pOid->id1);
    else if (pOid->id3 == 0)
         sprintf(pszOid, "%0d.%0d", pOid->id1, pOid->id2);
    else sprintf(pszOid, "%0d.%0d.%0d", pOid->id1, pOid->id2, pOid->id3);
    return TRUE;
}

const char *VARAPI_GetTypeName(int nType)
{
    switch(nType) {
      case VARSMI_UNKNOWN :     return "Unknown";
      case VARSMI_BOOL :        return "BOOL";
      case VARSMI_BYTE :        return "BYTE";
      case VARSMI_WORD :        return "WORD";
      case VARSMI_UINT :        return "UINT";
      case VARSMI_CHAR :        return "CHAR";
      case VARSMI_SHORT :       return "SHORT";
      case VARSMI_INT :         return "INT";
      case VARSMI_OID :         return "OID";
      case VARSMI_STRING :      return "STRING";
      case VARSMI_STREAM :      return "STREAM";
      case VARSMI_OPAQUE :      return "OPAQUE";
      case VARSMI_EUI64 :       return "EUI64";
      case VARSMI_IPADDR :      return "IPADDR";
      case VARSMI_TIMESTAMP :   return "TIMESTAMP";
      case VARSMI_TIMEDATE :    return "TIMEDATE";
    } 
    return "Unknown";
}

