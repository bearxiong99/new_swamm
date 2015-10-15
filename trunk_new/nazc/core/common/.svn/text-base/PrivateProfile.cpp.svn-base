#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "PrivateProfile.h"
#include "MemoryDebug.h"

////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////

CPrivateProfile::CPrivateProfile(char *pszFileName)
{
    m_bLoading 		= FALSE;
	m_pFirstNode	= NULL;
	m_pLastNode		= NULL;
	m_nObjectCount	= 0;
	*m_szFileName 	= '\0';

	if (pszFileName && *pszFileName)
		Load(pszFileName);	
}

CPrivateProfile::~CPrivateProfile()
{
	Flush();
}

////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////

BOOL CPrivateProfile::Load(char *pszFileName)
{
	BOOL	bResult;
	
	if (!pszFileName || !*pszFileName)
		return FALSE;

	m_Locker.Lock();
	FreeAllResource();
	m_nObjectCount = 0;

	strcpy(m_szFileName, pszFileName);
	bResult = LoadFromFile(pszFileName);
	m_Locker.Unlock();
	return bResult;
}

BOOL CPrivateProfile::Save()
{
	BOOL	bResult;
	
	if (!*m_szFileName)
		return FALSE;

	m_Locker.Lock();
	bResult = SaveToFile(m_szFileName);
	m_Locker.Unlock();
	return bResult;
}

BOOL CPrivateProfile::SaveAs(char *pszFileName)
{
	BOOL	bResult;
	
	if (!pszFileName || !*pszFileName)
		return FALSE;

	m_Locker.Lock();
	bResult = SaveToFile(pszFileName);
	m_Locker.Unlock();
	return bResult;
}

BOOL CPrivateProfile::Flush()
{
	m_Locker.Lock();
	FreeAllResource();
	m_Locker.Unlock();
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////

BOOL CPrivateProfile::SetValueBool(char *pszSection, char *pszKey, BOOL bValue)
{
	PPOBJECT	*pObject;
	char		szValue[20];

	if (!pszSection || !*pszSection)
		return FALSE;
	
	if (!pszKey || !*pszKey)
		return FALSE;

	m_Locker.Lock();

	pObject = Find(pszSection, pszKey);
	if (!pObject) pObject = NewObject(pszSection, pszKey);
	sprintf(szValue, "%s", bValue ? "yes" : "no");
	SetObjectValue(pObject, szValue);

	m_Locker.Unlock();
	return TRUE;
}

BOOL CPrivateProfile::SetValueString(char *pszSection, char *pszKey, char *pszValue)
{
	PPOBJECT	*pObject;

	if (!pszSection || !*pszSection)
		return FALSE;
	
	if (!pszKey || !*pszKey)
		return FALSE;

	if (!pszValue)
		return FALSE;
	
	m_Locker.Lock();

	pObject = Find(pszSection, pszKey);
	if (!pObject) pObject = NewObject(pszSection, pszKey);
	SetObjectValue(pObject, pszValue);
			
	m_Locker.Unlock();
	return TRUE;
}

BOOL CPrivateProfile::SetValueInt(char *pszSection, char *pszKey, int nValue)
{
	PPOBJECT	*pObject;
	char		szValue[32];

	if (!pszSection || !*pszSection)
		return FALSE;
	
	if (!pszKey || !*pszKey)
		return FALSE;

	m_Locker.Lock();

	pObject = Find(pszSection, pszKey);
	if (!pObject) pObject = NewObject(pszSection, pszKey);
	sprintf(szValue, "%d", nValue);
	SetObjectValue(pObject, szValue);
			
	m_Locker.Unlock();
	return TRUE;
}

BOOL CPrivateProfile::SetValueOpaque(char *pszSection, char *pszKey, char *pszValue, int nLength)
{
	PPOBJECT	*pObject;
	char		*pValue, szValue[10];
	int			i;
	
	if (!pszSection || !*pszSection)
		return FALSE;
	
	if (!pszKey || !*pszKey)
		return FALSE;

	if (!pszValue)
		return FALSE;
	
	m_Locker.Lock();

	pObject = Find(pszSection, pszKey);
	if (!pObject) pObject = NewObject(pszSection, pszKey);
	pValue = (char *)MALLOC((nLength*3)+1);
	*pValue = '\0';
	for(i=0; i<nLength; i++)
	{
		if (i == 0)
			 sprintf(szValue, "%02X", pszValue[i] & 0xff);
		else sprintf(szValue, " %02X", pszValue[i] & 0xff);
		strcat(pValue, szValue);
	}	
	SetObjectValue(pObject, pValue);
	FREE(pValue);

	m_Locker.Unlock();
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////

BOOL CPrivateProfile::GetValueBool(char *pszSection, char *pszKey, BOOL *bValue, BOOL bDefault)
{
	PPOBJECT	*pObject;
	BOOL		bResult = FALSE;

	if (!bValue)
		return FALSE;
	
	*bValue = bDefault;

	if (!pszSection || !*pszSection)
		return FALSE;
	
	if (!pszKey || !*pszKey)
		return FALSE;
	
	m_Locker.Lock();

	pObject = Find(pszSection, pszKey);
	if (pObject != NULL)
	{
		*bValue = ((strcmp(pObject->value, "yes") == 0) ||
				   (strcmp(pObject->value, "YES") == 0) ||
				   (strcmp(pObject->value, "true") == 0) ||
				   (strcmp(pObject->value, "TRUE") == 0)) ? TRUE : FALSE;
		bResult = TRUE;
	}

	m_Locker.Unlock();
	return bResult;
}

BOOL CPrivateProfile::GetValue(char *pszSection, char *pszKey, char *pszValue, char *pszDefault)
{
	PPOBJECT	*pObject;
	BOOL		bResult = FALSE;

	if (!pszValue)
		return FALSE;

	*pszValue = '\0';
	if (pszDefault) strcpy(pszValue, pszDefault);
	
	if (!pszSection || !*pszSection)
		return FALSE;
	
	if (!pszKey || !*pszKey)
		return FALSE;

	m_Locker.Lock();

	pObject = Find(pszSection, pszKey);
	if (pObject != NULL)
	{
		strcpy(pszValue, pObject->value);
		bResult = TRUE;
	}
	
	m_Locker.Unlock();
	return bResult;
}

BOOL CPrivateProfile::GetValueInt(char *pszSection, char *pszKey, int *nValue, int nDefault)
{
	PPOBJECT	*pObject;
	BOOL		bResult = FALSE;
	int			n;

	if (!nValue)
		return FALSE;
	
	*nValue = nDefault;

	if (!pszSection || !*pszSection)
		return FALSE;
	
	if (!pszKey || !*pszKey)
		return FALSE;
	
	m_Locker.Lock();

	pObject = Find(pszSection, pszKey);
	if (pObject != NULL)
	{	
		sscanf(pObject->value, "%d", &n);
		*nValue = n;
		bResult = TRUE;
	}

	m_Locker.Unlock();
	return bResult;
}

BOOL CPrivateProfile::GetValueOpaque(char *pszSection, char *pszKey, char *pszValue, int *nLength)	
{
	PPOBJECT	*pObject;
	BOOL		bResult = FALSE;
	char		*p;
	int			n, len = 0;

	if (!pszValue || !nLength)
		return FALSE;

	*nLength = 0;
	
	if (!pszSection || !*pszSection)
		return FALSE;
	
	if (!pszKey || !*pszKey)
		return FALSE;

	m_Locker.Lock();

	pObject = Find(pszSection, pszKey);
	if (pObject != NULL)
	{
		p = pObject->value;
		while(p && *p)
		{
			sscanf(p, "%x", &n);
			pszValue[len] = (BYTE)n;
			len++;

			p = strchr(p, ' ');
		}
		*nLength = len;
		bResult = TRUE;
	}

	m_Locker.Unlock();
	return bResult;
}

BOOL CPrivateProfile::EnumSection(PFNENUMPROFILE pfnCallback, char *pszSection, void *pParam)
{
	PPOBJECT	*pObject;

	if (!pfnCallback)
		return FALSE;

	if (!pszSection || !*pszSection)
		return FALSE;
	
	m_Locker.Lock();
	pObject = m_pFirstNode;
	while(pObject)
	{
		if (strcmp(pObject->section, pszSection) == 0)
			pfnCallback(pObject, pParam);

		pObject = pObject->next;
	}
	m_Locker.Unlock();
	return TRUE;
}

BOOL CPrivateProfile::EnumKey(PFNENUMPROFILE pfnCallback, void *pParam)
{
	PPOBJECT	*pObject;

	if (!pfnCallback)
		return FALSE;
	
	m_Locker.Lock();
	pObject = m_pFirstNode;
	while(pObject)
	{
		pfnCallback(pObject, pParam);
		pObject = pObject->next;
	}
	m_Locker.Unlock();
	return TRUE;
}

BOOL CPrivateProfile::DeleteSection(char *pszSection)
{
	PPOBJECT	**pList;
	PPOBJECT	*pObject;
	int			i, nSeek = 0;

	if (!pszSection || !*pszSection)
		return FALSE;
		
	if (m_nObjectCount == 0)
		return TRUE;
	
	m_Locker.Lock();
	pList = (PPOBJECT **)MALLOC(sizeof(PPOBJECT *)*m_nObjectCount);
	if (pList != NULL)
	{
		// Make object list	
		memset(pList, 0, sizeof(PPOBJECT *)*m_nObjectCount);
		pObject = m_pFirstNode;
		while(pObject)
		{
			pList[nSeek] = pObject;
			nSeek++;
			pObject = pObject->next;
		}

		// Sort object
		qsort(pList, m_nObjectCount, sizeof(PPOBJECT *), SectionSort);

		nSeek = 0;
		m_pFirstNode = NULL;
		m_pLastNode	 = NULL;

		for(i=0; i<m_nObjectCount; i++)
		{
			if (strcmp(pList[i]->section, pszSection) != 0)
			{
				if (m_pFirstNode == NULL)
				{
					m_pFirstNode = pList[i];
					m_pFirstNode->next = NULL;
					m_pLastNode  = pList[i];
				}
				else
				{
					m_pLastNode->next = pList[i];
					m_pLastNode = pList[i];
				}
				nSeek++;
			}
			else
			{
				// Free resource
				FreeObject(pList[i]);
			}
		}

		if (m_pLastNode != NULL)
			m_pLastNode->next = NULL;
		m_nObjectCount = nSeek;
		FREE(pList);
	}
	
	m_Locker.Unlock();
	return TRUE;
}

BOOL CPrivateProfile::DeleteKey(char *pszSection, char *pszKey)
{
	PPOBJECT	*pObject, *pPrev;
	BOOL		bResult = FALSE;

	if (!pszSection || !*pszSection)
		return FALSE;
	
	if (!pszKey || !*pszKey)
		return FALSE;

	m_Locker.Lock();

	pObject = Find(pszSection, pszKey, &pPrev);
	if (pObject != NULL)
	{
		DeleteObject(pObject, pPrev);
		m_nObjectCount--;
	}

	m_Locker.Unlock();
	return bResult;
}
	
////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////

PPOBJECT *CPrivateProfile::NewObject(char *pszSection, char *pszKey)
{
	PPOBJECT	*pObject;

	if (!pszSection || !*pszSection)
		return NULL;

	if (!pszKey || !*pszKey)
		return NULL;
	
	pObject = (PPOBJECT *)MALLOC(sizeof(PPOBJECT));
	if (pObject == NULL)
		return NULL;

	memset(pObject, 0, sizeof(PPOBJECT));
	pObject->section = strdup(pszSection);
	pObject->key = strdup(pszKey);
	
	if (m_pFirstNode == NULL)
	{
		m_pFirstNode = pObject;
		m_pLastNode = pObject;
	}
	else
	{
		m_pLastNode->next = pObject;
		m_pLastNode = pObject;
	}

	m_nObjectCount++;
	return pObject;
}

void CPrivateProfile::DeleteObject(PPOBJECT *pObject, PPOBJECT *pPrev)
{
	// Delete link
	if (pObject == m_pFirstNode)
		m_pFirstNode = pObject->next;

	if (pObject == m_pLastNode)
	{
		if (pPrev != NULL) pPrev->next = NULL;
		m_pLastNode = pPrev;
	}

	if (pPrev != NULL)
		pPrev->next = pObject->next;
	
	// Free resource
	FreeObject(pObject);
}
	
void CPrivateProfile::FreeObject(PPOBJECT *pObject)
{
	if (pObject->section) FREE(pObject->section);
	if (pObject->key) FREE(pObject->key);
	if (pObject->value) FREE(pObject->value);
	FREE(pObject);
}
	
void CPrivateProfile::SetObjectValue(PPOBJECT *pObject, char *pszValue)
{
	int		len;

	if (pObject->value != NULL)
		FREE(pObject->value);

	len = strlen(pszValue);
	pObject->value = (char *)MALLOC(len+1);
	strcpy(pObject->value, pszValue);
	pObject->value[len] = '\0';
}

void CPrivateProfile::FreeAllResource()
{
	PPOBJECT	*pObject, *pDelete;

	pObject = m_pFirstNode;
	while(pObject)
	{
		pDelete = pObject;
		pObject = pObject->next;

		FreeObject(pDelete);
	}

	m_pFirstNode 	= NULL;
	m_pLastNode 	= NULL;
	m_nObjectCount	= 0;
    m_bLoading 		= FALSE;
}

PPOBJECT *CPrivateProfile::Find(char *pszSection, char *pszKey, PPOBJECT **pPrev)
{
	PPOBJECT	*pObject;
	PPOBJECT	*pPrevious;

	if (pPrev)
		*pPrev = NULL;

	pPrevious = NULL;
	pObject = m_pFirstNode;
	while(pObject)
	{
		if (strcmp(pObject->section, pszSection) == 0)
		{
			if (strcmp(pObject->key, pszKey) == 0)
			{
				if (pPrev) *pPrev = pPrevious;
				break;
			}
		}

		pPrevious = pObject;
		pObject = pObject->next;
	}
	return pObject;
}

int CPrivateProfile::SectionSort(const void *p1, const void *p2)
{
	PPOBJECT	*pObject1;
	PPOBJECT	*pObject2;

	pObject1 = *(PPOBJECT * const *)p1;
	pObject2 = *(PPOBJECT * const *)p2;

	return strcmp(pObject1->section, pObject2->section);
}

BOOL CPrivateProfile::Sort()
{
	PPOBJECT	**pList;
	PPOBJECT	*pObject;
	int			i, nSeek = 0;

	if (m_nObjectCount == 0)
		return TRUE;
	
	pList = (PPOBJECT **)MALLOC(sizeof(PPOBJECT *)*m_nObjectCount);
	if (pList == NULL)
		return FALSE;

	// Make object list	
	memset(pList, 0, sizeof(PPOBJECT *)*m_nObjectCount);
	pObject = m_pFirstNode;
	while(pObject)
	{
		pList[nSeek] = pObject;
		nSeek++;
		pObject = pObject->next;
	}

	// Sort object
	qsort(pList, m_nObjectCount, sizeof(PPOBJECT *), SectionSort);

	m_pFirstNode = NULL;
	m_pLastNode	 = NULL;

	for(i=0; i<m_nObjectCount; i++)
	{
		if (m_pFirstNode == NULL)
		{
			m_pFirstNode = pList[i];
			m_pFirstNode->next = NULL;
			m_pLastNode  = pList[i];
		}
		else
		{
			m_pLastNode->next = pList[i];
			m_pLastNode = pList[i];
		}
	}

	if (m_pLastNode != NULL)
		m_pLastNode->next = NULL;
	
	FREE(pList);
	return TRUE;
}

BOOL CPrivateProfile::LoadFromFile(char *pszFileName)
{
	PPOBJECT	*pObject;
	char		szSection[100];
	char		*p, szBuffer[1024];
	char		*key;
	FILE		*fp;
	int			len;

	// Remove all resouce
	FreeAllResource();
	
	fp = fopen(pszFileName, "rt");
	if (fp == NULL)
		return FALSE;

	*szSection = '\0';
	while(fgets(szBuffer, 1024, fp))
	{
		len = strlen(szBuffer) - 1;
		szBuffer[len] = '\0';

		if (szBuffer[0] == '[')
		{
			if (strrchr(szBuffer, ']') == NULL)
				continue;

			strcpy(szSection, &szBuffer[1]);
			p = strrchr(szSection, ']');
			*p = '\0';
		}
		
		if (!*szSection)
			continue;
		
		key = szBuffer;
		p = strchr(szBuffer, '=');
		if (p != NULL)
		{
			*p = '\0';
			p++;

			pObject = NewObject(szSection, key);
			SetObjectValue(pObject, p);
		}
	}
	
	fclose(fp);
	m_bLoading = TRUE;
	return TRUE;
}

BOOL CPrivateProfile::SaveToFile(char *pszFileName)
{
	PPOBJECT	*pObject;
	char		szSection[64];
	char		szTempName[64];
	char		szBuffer[1024];
//	char		szValue[1024];
//	char		val[10];
	FILE		*fp;
//	int			i;

	Sort();

	sprintf(szTempName, "%s.tmp", pszFileName);
	fp = fopen(szTempName, "wb");
	if (fp == NULL)
		return FALSE;

	*szSection = '\0';
	pObject = m_pFirstNode;
	while(pObject)
	{
		if (strcmp(szSection, pObject->section) != 0)
		{
			if (!*szSection)
				 sprintf(szBuffer, "[%s]\n", pObject->section);	
			else sprintf(szBuffer, "\n[%s]\n", pObject->section);

			fwrite(szBuffer, strlen(szBuffer), 1, fp);
			strcpy(szSection, pObject->section);
		}
		
		sprintf(szBuffer, "%s=%s\n", pObject->key, pObject->value);	
		fwrite(szBuffer, strlen(szBuffer), 1, fp);
		
		pObject = pObject->next;
	}
	
	fflush(fp);
	fclose(fp);

	rename(szTempName, pszFileName);
	return TRUE;
}

