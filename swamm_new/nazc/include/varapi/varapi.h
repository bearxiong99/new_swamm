#ifndef __VAR_API_H__
#define __VAR_API_H__

#include "vartype.h"

#ifdef __cplusplus
extern "C" {
#endif

// Service Startup/Shutdown
int 	VARAPI_Initialize(const char *pszFileName, VAROBJECT *pRoot, BOOL bSave);
int 	VARAPI_Destroy();

void 	VARAPI_EnableModify(BOOL bEnable);
void 	VARAPI_SetModify();

int 	VARAPI_Load();
int 	VARAPI_Save(BOOL bAll);
BOOL    VARAPI_Serialize(char *pPath, MIBValue *pValues);
BOOL    VARAPI_Deserialize(char *pPath, MIBValue **pValues, int * nCount);

MIBValue *VARAPI_GetValue(OID3 *oid, MIBValue *pParam);
int 	VARAPI_SetValue(MIBValue *pValue, MIBValue *pParam);
int 	VARAPI_GetIndexValue(VAROBJECT *pObject, void *pValue, MIBValue *pParam);

MIBValue *VARAPI_DupValue(const MIBValue *pValue);
MIBValue *VARAPI_DupValueList(const MIBValue *pValue, int nCount);
int 	VARAPI_CopyValue(const VAROBJECT *pObject, MIBValue *pValue);
int 	VARAPI_ReplaceValue(VAROBJECT *pObject, const MIBValue *pValue);
int 	VARAPI_UpdateValue(const char *pszName, const char *pszValue, int nLength);

int 	VARAPI_FreeNode(MIBValue *pValue);
int 	VARAPI_FreeValue(MIBValue *pValue);
int 	VARAPI_FreeValueList(MIBValue *pList, int nCount);

BOOL 	VARAPI_EnumObject(PFNENUMVAROBJECT pfnCallback, void *pParam);
VAROBJECT *VARAPI_GetObject(OID3 *oid);
VAROBJECT *VARAPI_GetObjectByName(const char *pszName);
int 	VARAPI_OidCompare(OID3 *id, const char *pszOid);
const char 	*VARAPI_OidToName(OID3 *oid, char *pszName);
BOOL 	VARAPI_StringToOid(const char *pszOid, OID3 *id);
BOOL 	VARAPI_OidToString(OID3 *pOid, char *pszOid);
const char	*VARAPI_GetTypeName(int nType);

#ifdef __cplusplus
}
#endif

#endif	// __VAR_API_H__
