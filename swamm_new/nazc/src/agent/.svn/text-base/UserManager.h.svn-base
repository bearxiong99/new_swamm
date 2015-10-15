#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__

#include "LinkedList.h"
#include "Locker.h"

typedef void (*PFNENUMUSERCALLBACK)(USERINFO *pUser, void *pParam);

class CUserManager
{
public:
	CUserManager();
	virtual ~CUserManager();

public:
	BOOL	IsUser(char *pszUser);
	BOOL 	GetUser(char *pszUser, char *pszPassword, BYTE *pGroup);

public:
	BOOL	LoadUser();
	void	UnloadUser();
	void	EnumUser(PFNENUMUSERCALLBACK pfnCallback, void *pParam);
	BOOL	AddUser(char *pszUser, char *pszPassword, BYTE nGroup);
	BOOL	DeleteUser(char *pszUser);
	BOOL	ChangePassword(char *pszUser, char *pszPassword);
	BOOL	ChangeGroup(char *pszUser, BYTE nGroup);

protected:
	USERINFO *AllocateUser(const char *pszUser, const char *pszPassword, int nGroup);
	void 	FreeUser(USERINFO *pUser);
	USERINFO *FindUser(char *pszUser);
	void 	WriteDefaultToFile();
	void 	WriteUserToFile();

protected:
	CLocker		m_Locker;
	CLinkedList<USERINFO *>	m_UserList;
};

extern CUserManager	*m_pUserManager;

#endif	// __USER_MANAGER_H__
