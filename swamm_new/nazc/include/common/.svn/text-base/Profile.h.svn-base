#ifndef __PROFILE_H__
#define __PROFILE_H__

#include "typedef.h"

class CProfile
{
public:
	CProfile(char *pszFileName=NULL);
	virtual ~CProfile();

public:
	virtual BOOL IsModified() const;
	virtual char *GetFileName() const;
	virtual void SetFileName(char *pszFileName);

public:
	virtual BOOL GetProfile(char *pszKey, char *pszString, int nMaximum);
	virtual BOOL GetProfileInt(char *pszKey, int *nValue);
	virtual BOOL WriteProfile(char *pszKey, char *pszString);
	virtual BOOL WriteProfileInt(char *pszKey, int nValue);
	virtual BOOL SaveProfile();
	virtual BOOL LoadProfile();

private:
	BOOL	Open();
	void	Close();
	BOOL	Load();
	BOOL	Save();

private:
	int		m_nFD;
	BOOL	m_bModified;
	char	m_szFileName[128];
};

#endif	// __PROFILE_H__
