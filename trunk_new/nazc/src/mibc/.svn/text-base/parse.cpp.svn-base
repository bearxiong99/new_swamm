#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "typedef.h"
#include "varapi.h"
#include "oidmib.h"
#include "parse.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////

#define MAX_TOKEN				50

int		m_nLine	 = 0;
char	m_szStream[256];
char	m_szLine[256];
char	m_szBuffer[1024];

int		m_nToken = 0;
char	*m_pToken[MAX_TOKEN];

OBJECT	m_RootObject = 
{
	{0,0,0}, 0,	0, 0, "Root", 	"root", 0, 0, NULL, NULL, NULL 
};


//////////////////////////////////////////////////////////////////////
// Command Table
//////////////////////////////////////////////////////////////////////

int ParseObject(char *pszInput, char *pszOutput, int nOption)
{
	FILE	*src;
	char	*pszFileName;
	int		len, nType;
	OID3	oid;
	int		flag;

	src = fopen(pszInput, "rt");
	if (src == NULL)
		return -1;

	pszFileName = strrchr(pszInput, '/');
	pszFileName = pszFileName ? pszFileName+1 : pszInput;

	while(fgets(m_szStream, 255, src) > 0)
	{
		len = strlen(m_szStream);
		if (len >= 1)
		{
			if ((m_szStream[len-1] == 0xd) || (m_szStream[len-1] == 0xa))
				m_szStream[len-1] = '\0';
		}

		if (len >= 2)
		{
			if ((m_szStream[len-2] == 0xd) || (m_szStream[len-2] == 0xa))
				m_szStream[len-2] = '\0';
		}

		strcpy(m_szLine, m_szStream);
		m_nLine++;

		if (m_szStream[0] == 0)
			continue;

		if (m_szStream[0] == '#')
			continue;

		splite_token(m_szLine);

		if (m_nToken != 7)
		{
			printf("%s(%0d): Invalid Token Count.\n", pszFileName, m_nLine);
			printf("	'%s'\n", m_szStream);
			continue;
		}

		nType = GetValueTypeByName(m_pToken[2]);
		if (nType == VARSMI_UNKNOWN)
		{
			printf("%s(%0d): Invalid Variable Type.\n", pszFileName, m_nLine);
			printf("	'%s'\n", m_pToken[2]);
			continue;
		}

		flag = 0;
		if (m_pToken[3][0] == '*') flag |= ATTR_SUM_LENGTH;	
		if (strchr(m_pToken[4], 'R')) flag |= ATTR_READ;	
		if (strchr(m_pToken[4], 'W')) flag |= ATTR_WRITE;	
		if (strchr(m_pToken[4], 'L')) flag |= ATTR_LIST;	
		if (strchr(m_pToken[4], 'S')) flag |= ATTR_SMI;	
		if (strchr(m_pToken[4], 'C')) flag |= ATTR_COMMAND;	
		if (strchr(m_pToken[4], 'E')) flag |= ATTR_EVENT;	
		if (strchr(m_pToken[4], 'A')) flag |= ATTR_ALARM;	
		if (StringToOid(m_pToken[5], &oid)) flag |= ATTR_GROUP;

		AddObject(m_nLine,
					m_pToken[0],
					m_pToken[1],
					nType,
					atoi(m_pToken[3]),
					m_pToken[4],
					flag,
					&oid,
					m_pToken[6] ? m_pToken[6] : (char *)"");
	}

	fclose(src);
	return 0;
}

BOOL QueryObject(PFNQUERYCALLBACK pfnCallback, void *pParam)
{
	return QueryNode(&m_RootObject, pfnCallback, pParam);
}

//////////////////////////////////////////////////////////////////////
// Command Table
//////////////////////////////////////////////////////////////////////

int AddObject(int nLine, char *pszID, char *pszName, int nType, int nLength,
				char *attr, int flag, OID3 *group, char *pszDescr)
{
	OBJECT	*pParent, *pObject, *pTail;
	OID3	oid, gid;
	BYTE	id, oidlen;
	
	if (!StringToOid(pszID, &oid))
		return -1;

	memset(&gid, 0, sizeof(OID3));
	if (oid.id1 == 0)
		return -1;

	if (oid.id2 == 0)
	{
		id = oid.id1;
		oidlen = 1;
	}
	else if (oid.id3 == 0)
	{
		id = oid.id2;
		oidlen = 2;
	}
	else
	{
		id = oid.id3;
		oidlen = 3;
	}

	if (FindObject(&m_RootObject, &oid))
		return -2;

	if (oidlen > 1)
	{
		pParent = FindParent(&m_RootObject, &oid, oidlen-1);	
		if (pParent == NULL)
			return -3;
	}
	else
	{
		pParent = &m_RootObject;
	}

	pObject = (OBJECT *)malloc(sizeof(OBJECT));
	if (pObject == NULL)
		return -4;

	memset(pObject, 0, sizeof(OBJECT));
	memcpy(&pObject->oid, &oid, 3);
	memcpy(&pObject->group, &group, 3);

	pObject->pszName	= pszName && *pszName ? strdup(pszName) : NULL;
	pObject->pszDescr	= pszDescr && *pszDescr ? strdup(pszDescr) : NULL;
	pObject->type		= nType;
	pObject->len		= nLength;
	pObject->line		= nLine;
	pObject->id			= id;
	pObject->attr		= flag;

	if (pParent->pChild == NULL)
	{
		pParent->pChild = pObject;
	}
	else
	{
		pTail = pParent->pChild;
		while(pTail && pTail->pNext) pTail = pTail->pNext;
		pTail->pNext = pObject;
	}
	return 0;
}

OBJECT *FindObject(OBJECT *pParent, OID3 *oid)
{
	OBJECT	*pNext, *pFind;
	
	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		if (memcmp(&pNext->oid, oid, 3) == 0)
			return pNext;	
		if (pNext->pChild != NULL)
		{
			pFind = FindObject(pNext->pChild, oid);
			if (pFind != NULL)
				return pFind;
		}
	}
	return NULL;
}

OBJECT *FindParent(OBJECT *pParent, OID3 *oid, int nLength)
{
	OBJECT	*pNext, *pFind;

	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		if (memcmp(&pNext->oid, oid, nLength) == 0)
			return pNext;
	
		if (pNext->pChild != NULL)
		{
			pFind = FindParent(pNext, oid, nLength);
			if (pFind != NULL)
				return pFind;
		}
	}
	return NULL;
}

BOOL QueryNode(OBJECT *pParent, PFNQUERYCALLBACK pfnCallback, void *pParam)
{
	OBJECT	*pObject;

	// Parent Callback
	if (!pfnCallback(pParent, NULL, pParam))
		return FALSE;

	// Navigation Node
	for(pObject=pParent->pChild; pObject; pObject=pObject->pNext)
	{
		if (!pfnCallback(pParent, pObject, pParam))
			return FALSE;
	}

	for(pObject=pParent->pChild; pObject; pObject=pObject->pNext)
	{
		if (!QueryNode(pObject, pfnCallback, pParam))
			return FALSE;
	}
	return TRUE;
}

void splite_token(char *pszLine)
{
	int		i, nSize;
	char	*p;

	m_nToken = 0;
	for(i=0; i<MAX_TOKEN; i++)
		m_pToken[i] = NULL;

	p = pszLine;
	nSize = strlen(pszLine);
	
	for(i=0; i<nSize && p && *p; i++)
	{
		m_pToken[m_nToken] = p;
		m_nToken++;

		p = strchr(p, ',');
		if (p != NULL)
		{
			*p = '\0';
			p++;
		}

		// Skip White Space
		for(; p && *p; p++)
		{
			if ((*p != ' ') && (*p != 0x09))
				break;	
		}
	}	
}

