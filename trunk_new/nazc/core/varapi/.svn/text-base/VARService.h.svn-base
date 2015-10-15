#ifndef __VAR_SERVICE_H__
#define __VAR_SERVICE_H__

#include "Locker.h"
#include "TimeoutThread.h"

class CVARService : public CTimeoutThread
{
public:
	CVARService();
	virtual ~CVARService();

public:
	int 	Initialize(const char *pszFileName, VAROBJECT *pRoot, BOOL bSave);
	int 	Destroy();

public:
	void 	SetModify(BOOL bModified=TRUE);
	int 	Load();
	int 	Save(BOOL bAll);

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

public:
	BOOL	EnumObject(PFNENUMVAROBJECT pfnCallback, void *pParam);
	VAROBJECT *GetObject(OID3 *oid);
	VAROBJECT *GetObjectByName(const char *pszName);
    BOOL    Serialize(char *pPath, MIBValue * pList);
    BOOL    Deserialize(char *pPath, MIBValue ** pList, int * nCount);

protected:
	void	InitObjectValue(VAROBJECT *pList);
	void	FreeObjectValue(VAROBJECT *pList);

	VAROBJECT *FindObjectByID(VAROBJECT *pList, BYTE *oid, int nIndex);
	VAROBJECT *FindObjectByName(VAROBJECT *pList, const char *pszName);
	void	NavigateObject(VAROBJECT *pList, PFNENUMVAROBJECT pfnCallback, void *pParam);

	void	LoadFromFile(FILE *fp, VAROBJECT *pList);
	void	SaveToFile(FILE *fp, VAROBJECT *pList, BOOL bAll);

protected:
	CLocker		m_IOLocker;
	BOOL		m_bSave;
	BOOL		m_bModified;
	VAROBJECT	*m_pRootObject;
	char		m_szFileName[64];
};

extern CVARService	*m_pVARService;

#endif // __VAR_SERVICE_H__
