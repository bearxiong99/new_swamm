#ifndef __PRIVATE_PROFILE_H__
#define __PRIVATE_PROFILE_H__

#include "Locker.h"

typedef struct	_tagPPOBJECT
{
	char 	*section;
	char	*key;
	char	*remark;
	char	*value;
	struct	_tagPPOBJECT	*next;
}	PPOBJECT;

typedef void (*PFNENUMPROFILE)(PPOBJECT *pObject, void *pParam);

class CPrivateProfile
{
public:
	CPrivateProfile(char *pszFileName=NULL);
	virtual ~CPrivateProfile();

public:
	BOOL	Load(char *pszFileName=NULL);
	BOOL	Save();
	BOOL	SaveAs(char *pszFileName);
	BOOL	Flush();
	
	BOOL	SetValueBool(char *pszSection, char *pszKey, BOOL bValue);
	BOOL	SetValueString(char *pszSection, char *pszKey, char *pszValue);
	BOOL	SetValueInt(char *pszSection, char *pszKey, int nValue);
	BOOL	SetValueOpaque(char *pszSection, char *pszKey, char *pszValue, int nLength);

	BOOL	GetValueBool(char *pszSection, char *pszKey, BOOL *bValue, BOOL bDefault=FALSE);
	BOOL	GetValue(char *pszSection, char *pszKey, char *pszValue, char *pszDefauult="");
	BOOL	GetValueInt(char *pszSection, char *pszKey, int *nValue, int nDefault=0);
	BOOL	GetValueOpaque(char *pszSection, char *pszKey, char *pszValue, int *nLength);

	BOOL	EnumSection(PFNENUMPROFILE pfnCallback, char *pszSection, void *pParam);
	BOOL	EnumKey(PFNENUMPROFILE pfnCallback, void *pParam);

	BOOL	DeleteSection(char *pszSection);
	BOOL	DeleteKey(char *pszSection, char *pszKey);
	
protected:
	PPOBJECT *NewObject(char *pszSection, char *pszKey);
	void	DeleteObject(PPOBJECT *pObject, PPOBJECT *pPrev);
	void	FreeObject(PPOBJECT *pObject);
	void	SetObjectValue(PPOBJECT *pObject, char *pszValue);
	void 	FreeAllResource();
	PPOBJECT *Find(char *pszSection, char *pszKey, PPOBJECT **pPrev=NULL);

	BOOL	Sort();
	BOOL	LoadFromFile(char *pszFileName);
	BOOL	SaveToFile(char *pszFileName);

public:
	static	int SectionSort(const void *p1, const void *p2);

protected:
	BOOL		m_bLoading;
	int			m_nObjectCount;
	char		m_szFileName[64];
	PPOBJECT	*m_pFirstNode;
	PPOBJECT	*m_pLastNode;
	CLocker		m_Locker;
};

#endif	// __PRIVATE_PROFILE_H__
