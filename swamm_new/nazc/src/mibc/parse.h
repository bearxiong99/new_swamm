#ifndef __PARSE_H__
#define __PARSE_H__

extern	OBJECT	m_RootObject;
 
typedef BOOL (*PFNQUERYCALLBACK)(OBJECT *pParent, OBJECT *pObject, void *pParam);

int 	ParseObject(char *pszInput, char *pszOutput, int nOption);
BOOL 	QueryObject(PFNQUERYCALLBACK pfnCallback, void *pParam);

int		AddObject(int nLine, char *pszID, char *pszName, int nType,
				  int nLength, char *attr, int flag, OID3 *group, char *pszDescr);
OBJECT *FindObject(OBJECT *pParent, OID3 *oid);
OBJECT *FindParent(OBJECT *pParent, OID3 *oid, int nLength);
BOOL	QueryNode(OBJECT *pParent, PFNQUERYCALLBACK pfnCallback, void *pParam);
void 	splite_token(char *pszLine);

#endif	// __PARSE_H__
