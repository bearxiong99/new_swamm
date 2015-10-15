#ifndef __UTIL_H__
#define __UTIL_H__

int  GetValueTypeByName(char *pType);
char *GetTypeName(int nType);
char *GetTypeNameVar(int nType);
BOOL StringToOid(char *pszOid, OID3 *pOid);
void OidToString(OID3 *oid, char *pszOid);
void strupr(char *pszString);

#endif	// __UTIL_H__

