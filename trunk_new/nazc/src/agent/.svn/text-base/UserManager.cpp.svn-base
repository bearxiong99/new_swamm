//////////////////////////////////////////////////////////////////////
//
//  UserManager.cpp: implementation of the CUserManager class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
//  http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "UserManager.h"
#include "MemoryDebug.h"
#include "CommonUtil.h"
#include "CommandHandler.h"

//////////////////////////////////////////////////////////////////////
// CUserManager Class
//////////////////////////////////////////////////////////////////////

#define	USER_FILENAME		"/app/conf/passwd"

CUserManager	*m_pUserManager = NULL;

//////////////////////////////////////////////////////////////////////
// CUserManager Class
//////////////////////////////////////////////////////////////////////

CUserManager::CUserManager()
{
	m_pUserManager = this;
}

CUserManager::~CUserManager()
{
}

//////////////////////////////////////////////////////////////////////
// CUserManager Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CUserManager::LoadUser()
{
	char	*p, *pszBuffer;
	char	*pUser, *pPassword, *pGroup;
	FILE	*fp;
	int		i, nLength;

	fp = fopen(USER_FILENAME, "rt");
	if (fp == NULL)
	{
		WriteDefaultToFile();
	    fp = fopen(USER_FILENAME, "rt");

        if (fp == NULL)
        {
            return FALSE;
        }
	}

	pszBuffer = (char *)MALLOC(2048);
	if (pszBuffer == NULL)
	{
		fclose(fp);
		return FALSE;
	}

	while(fgets(pszBuffer, 2048, fp))
	{
		nLength = strlen(pszBuffer);
		pszBuffer[nLength] = '\0';

		pUser = pPassword = pGroup = NULL;
		p = strtok(pszBuffer, ",");
		for(i=0; p && (i<3); i++)
		{
			switch(i) {
			  case 0 : pUser = p; break;
			  case 1 : pPassword = p; break;
			  case 2 : pGroup = p; break;
			}
			p = strtok(NULL, ",");
		}
		
		if (pUser && pPassword && pGroup)
		{
			m_Locker.Lock();
			AllocateUser(pUser, pPassword, atoi(pGroup));
			m_Locker.Unlock();
		}
	}

	FREE(pszBuffer);
	fclose(fp);
	return TRUE;
}

void CUserManager::UnloadUser()
{
	USERINFO	*pUser, *pDelete;
	POSITION	pos;

	m_Locker.Lock();
	pUser = m_UserList.GetFirst(pos);
	while(pUser)
	{
		pDelete = pUser;
		pUser = m_UserList.GetNext(pos);
		FreeUser(pDelete);
	}
	m_UserList.RemoveAll();
	m_Locker.Unlock();
}

BOOL CUserManager::IsUser(char *pszUser)
{
	USERINFO	*pUser;

	m_Locker.Lock();
	pUser = FindUser(pszUser);
	m_Locker.Unlock();
	return pUser ? TRUE : FALSE;
}

BOOL CUserManager::GetUser(char *pszUser, char *pszPassword, BYTE *pGroup)
{
	USERINFO	*pUser;

	*pszPassword = '\0';
	*pGroup		 = 0;

	m_Locker.Lock();
	pUser = FindUser(pszUser);
	if (pUser != NULL)
	{
		strcpy(pszPassword, pUser->szPassword);
		*pGroup = pUser->nGroup;
	}
	m_Locker.Unlock();
	return pUser ? TRUE : FALSE;
}

void CUserManager::EnumUser(PFNENUMUSERCALLBACK pfnCallback, void *pParam)
{
	USERINFO	*pUser;
	POSITION	pos;

	m_Locker.Lock();
	pUser = m_UserList.GetFirst(pos);
	while(pUser)
	{
		pfnCallback(pUser, pParam);
		pUser = m_UserList.GetNext(pos);
	}
	m_Locker.Unlock();
}

BOOL CUserManager::AddUser(char *pszUser, char *pszPassword, BYTE nGroup)
{
	USERINFO	*pUser;
	BOOL		bResult=FALSE;
    char        *pszResult;
    char        szSalt[3];

    GetSalt(szSalt);
    pszResult = SetUserKey(pszPassword, szSalt);

	m_Locker.Lock();
	pUser = FindUser(pszUser);
	if (pUser == NULL)
	{
		AllocateUser(pszUser, pszResult, nGroup);
		WriteUserToFile();
		bResult = TRUE;
	}
	m_Locker.Unlock();
	return bResult;
}

BOOL CUserManager::DeleteUser(char *pszUser)
{
	USERINFO	*pUser;
	BOOL		bResult=FALSE;

	if (!pszUser || !*pszUser)
		return FALSE;

	if (strcmp(pszUser, "aimir") == 0)
		return FALSE;

	m_Locker.Lock();
	pUser = FindUser(pszUser);
	if (pUser != NULL)
	{
		m_UserList.RemoveAt((POSITION)pUser->nPosition);
		FreeUser(pUser);
		WriteUserToFile();	
		bResult = TRUE;
	}
	m_Locker.Unlock();
	return bResult;
}

BOOL CUserManager::ChangePassword(char *pszUser, char *pszPassword)
{
	USERINFO	*pUser;
	BOOL		bResult=FALSE;
    char        *pszResult;
    char        szSalt[3];

    GetSalt(szSalt);

	m_Locker.Lock();
	pUser = FindUser(pszUser);
	if (pUser != NULL)
	{
        pszResult = SetUserKey(pszPassword, szSalt);
		strcpy(pUser->szPassword, pszResult);
		WriteUserToFile();	
		bResult = TRUE;
	}
	m_Locker.Unlock();
	return bResult;
}

BOOL CUserManager::ChangeGroup(char *pszUser, BYTE nGroup)
{
	USERINFO	*pUser;
	BOOL		bResult=FALSE;

	m_Locker.Lock();
	pUser = FindUser(pszUser);
	if (pUser != NULL)
	{
		pUser->nGroup = nGroup;
		WriteUserToFile();	
		bResult = TRUE;
	}
	m_Locker.Unlock();
	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CUserManager Override Functions
//////////////////////////////////////////////////////////////////////

USERINFO *CUserManager::AllocateUser(const char *pszUser, const char *pszPassword, int nGroup)
{
	USERINFO	*pUser;

	pUser = (USERINFO *)MALLOC(sizeof(USERINFO));
	if (pUser == NULL)
		return NULL;

	memset(pUser, 0, sizeof(USERINFO));
	memcpy(pUser->szUser, pszUser, 16); pUser->szUser[16] = '\0';
	memcpy(pUser->szPassword, pszPassword, 16); pUser->szPassword[16] = '\0';
	pUser->nGroup = nGroup;

	pUser->nPosition = (int)m_UserList.AddTail(pUser);
	return pUser;	
}

void CUserManager::FreeUser(USERINFO *pUser)
{
	if (pUser == NULL)
		return;
	FREE(pUser);
}

USERINFO *CUserManager::FindUser(char *pszUser)
{
	USERINFO	*pUser;
	POSITION	pos=INITIAL_POSITION;

	pUser = m_UserList.GetFirst(pos);
	while(pUser)
	{
		if (strcmp(pUser->szUser, pszUser) == 0)
			break;
		pUser = m_UserList.GetNext(pos);
	}
	return pUser;	
}

// Password는 SetUserKey로 암호화하여 저장한다.
void CUserManager::WriteDefaultToFile()
{
	FILE		*fp;
	char		szBuffer[256], *pszResult;
    char        szSalt[3];

	fp = fopen(USER_FILENAME, "wb");
	if (fp == NULL)
		return;

    GetSalt(szSalt);
    pszResult = SetUserKey("aimir", szSalt);
	sprintf(szBuffer, "aimir,%s,0", pszResult);
	fwrite(szBuffer, strlen(szBuffer), 1, fp);
	fclose(fp);
}

void CUserManager::WriteUserToFile()
{
	USERINFO	*pUser;
	POSITION	pos;
	FILE		*fp;
	char		szBuffer[256];

	fp = fopen(USER_FILENAME, "wb");
	if (fp == NULL)
		return;

	pUser = m_UserList.GetFirst(pos);
	while(pUser)
	{
		sprintf(szBuffer, "%s,%s,%0d\n", 
				pUser->szUser,
				pUser->szPassword,
				pUser->nGroup);
		fwrite(szBuffer, strlen(szBuffer), 1, fp);
		pUser = m_UserList.GetNext(pos);
	}

	fclose(fp);
}

