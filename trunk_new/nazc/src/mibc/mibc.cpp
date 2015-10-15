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

#define MODE_NORMAL				0
#define MODE_HEADER				1
#define MODE_TREE				2

BOOL	m_bNoHandler = FALSE;
BOOL	m_bStruct = FALSE;
BOOL	m_bHeader = FALSE;
int		m_nMode = MODE_NORMAL;

//////////////////////////////////////////////////////////////////////
// Callback Function Definitions
//////////////////////////////////////////////////////////////////////

BOOL DisplayNodeDefine(OBJECT *pParent, OBJECT *pObject, void *pParam);
void DisplayStructure(OBJECT *pParent);
void DisplayNode(OBJECT *pParent);
void SummaryNode(OBJECT *pParent);
void DisplayGroupNode(OBJECT *pParent);
void DisplayGroupTitleNode(OBJECT *pParent, BOOL bPrint);
void DisplayTree(OBJECT *pParent, int nLevel, BOOL bDoor, BOOL bEnd);
void FindDoor(OBJECT *pParent, int nLevel);

//////////////////////////////////////////////////////////////////////
// Main Program
//////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	char	szSource[256] = "";
	char	szTarget[256] = "";
	char	szTarget1[256] = "";
	char	*p;
	int		i;

	for(i=1; i<argc; i++)
	{
		if (argv[i][0] != '-')
		{
			strcpy(szSource, argv[i]);
		}
		else if (strcmp(argv[i], "--nohandler") == 0)
		{
			m_bNoHandler = TRUE;
		}
		else if (strcmp(argv[i], "--tree") == 0)
		{
			m_nMode = MODE_TREE;	
		}
		else if (strcmp(argv[i], "--struct") == 0)
		{
			m_bStruct = TRUE;	
		}
		else if (strcmp(argv[i], "--header") == 0)
		{
			m_nMode = MODE_HEADER;	
		}
	}

	sprintf(szTarget, "gen_%s", szSource);
	p = strrchr(szTarget, '.');
	if (p != NULL)
	{
		*p = '\0';
		strcpy(szTarget1, szTarget);
		strcat(szTarget, ".cpp");
		strcat(szTarget1, ".h");
	}

	ParseObject(szSource, szTarget, 0);

	switch(m_nMode) {
	  case MODE_NORMAL :
	  	   printf("#include \"common.h\"\n");
		   printf("#include \"varapi.h\"\n");
		   printf("#include \"Variable.h\"\n");
		   printf("\n");

//		   QueryObject(DisplayNodeDefine, NULL);
		   if (!m_bNoHandler && m_bStruct)
		   {
			   printf("\n");
			   DisplayStructure(&m_RootObject);
		   }
//		   printf("\n");
		   DisplayGroupTitleNode(&m_RootObject, FALSE);

		   SummaryNode(&m_RootObject);
		   DisplayNode(&m_RootObject);
		   if (!m_bNoHandler)
			  DisplayGroupNode(&m_RootObject);
		   break;

	  case MODE_HEADER :
	  	   printf("#ifndef __VARIABLE_H__\n");
		   printf("#define __VARIABLE_H__\n");
		   printf("\n");

		   QueryObject(DisplayNodeDefine, NULL);
		   DisplayStructure(&m_RootObject);
		   printf("\n");
		   DisplayGroupTitleNode(&m_RootObject, TRUE);

		   printf("\n");
		   printf("#endif	// __VARIABLE_H__\n");
		   break;

	  case MODE_TREE :
		   printf("root\n");
		   FindDoor(&m_RootObject, 0);
		   DisplayTree(&m_RootObject, 0, FALSE, FALSE);
		   break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Callback Routine
//////////////////////////////////////////////////////////////////////

BOOL DisplayNodeDefine(OBJECT *pParent, OBJECT *pObject, void *pParam)
{
	if (pObject != NULL)
		return TRUE;

	printf("extern VAROBJECT	m_%s_node[];\n", pParent->pszName);
	return TRUE;
}

void DisplayStructure(OBJECT *pParent)
{
	OBJECT	*pNext;
	char	szBuffer[100];

	if (pParent->type == VARSMI_OPAQUE)
	{
		strcpy(szBuffer, pParent->pszName);
		strupr(szBuffer);
		printf("\ntypedef struct _tag%s\n", szBuffer);
		printf("{\n");
		for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
		{
			switch(pNext->type) {
			  case VARSMI_CHAR :
			  case VARSMI_BYTE :
				   if (pNext->len == 1)
				   	    printf("	%-12s%s;\n", GetTypeNameVar(pNext->type), pNext->pszName);
				   else printf("	%-12s%s[%0d];\n", 
								GetTypeNameVar(pNext->type),
								pNext->pszName,
								pNext->len);
				   break;
	  		  case VARSMI_STRING :
	  		  case VARSMI_STREAM :
	 	      case VARSMI_OPAQUE :
	  	      case VARSMI_IPADDR :
				   printf("	%-12s%s[%0d];\n",
								GetTypeNameVar(pNext->type),
								pNext->pszName,
								pNext->len);
				   break;
			  default :
				   printf("	%-12s%s;\n", GetTypeNameVar(pNext->type), pNext->pszName);
				   break;
			}
		}
		printf("} __attribute__ ((packed)) %s;\n", szBuffer);
	}
	
	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		if (pNext->pChild != NULL)
			DisplayStructure(pNext);
	}
}

void DisplayNode(OBJECT *pParent)
{
	OBJECT	*pNext;
	char	szChild[64];
	char	szName[64];
	char	szHandler[100];

	printf("\nVAROBJECT	m_%s_node[] =\n", pParent->pszName);
	printf("{\n");
	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		sprintf(szName, "\"%s\",", pNext->pszName);
		sprintf(szChild, "m_%s_node", pNext->pszName ? pNext->pszName : "xxx");
		sprintf(szHandler, "%sQuery", pNext->attr & ATTR_GROUP && pNext->pGroup ? pNext->pGroup->pszName : "xxx");
		
		printf("	{ %3d,	%s { {%0d,%0d,%0d}, %s, %0d }, %0d, %s, %s, %s },\n",
				pNext->id,
				szName,
				pNext->oid.id1,
				pNext->oid.id2,
				pNext->oid.id3,
				GetTypeName(pNext->type),
				pNext->len,
				pNext->attr & 0xff,
				pNext->pChild == NULL ? "NULL" : szChild,
				"NULL",
				(!m_bNoHandler && (pNext->attr & ATTR_GROUP)) ? szHandler : "NULL");
	}
	printf("	{   0,	NULL }\n");
	printf("};\n");
	
	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		if (pNext->pChild != NULL)
			DisplayNode(pNext);
	}
}

void SummaryNode(OBJECT *pParent)
{
	OBJECT	*pNext;
	int		nLength = 0;

	if (pParent->attr & ATTR_SUM_LENGTH)
	{
		for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
		{
			nLength += pNext->len;
		}
		pParent->len = nLength;
	}

	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		if (pNext->pChild != NULL)
			SummaryNode(pNext);
	}
}

void DisplayGroupNode(OBJECT *pParent)
{
	OBJECT	*pNext;
	char	szAttr[50], szName[64];
	int		i, idlen;

	if (pParent->attr & ATTR_GROUP)
	{
		idlen = 0;
		if (pParent->oid.id2 == 0)
			idlen = 1;
		else if (pParent->oid.id3 == 0)
			idlen = 2;
		else idlen = 3;

		printf("\n");
		printf("int %sQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam)\n",
					pParent->pszName);
		printf("{\n");

		switch(idlen) {
		  case 1 : printf("	switch(pObject->var.oid.id2) {\n"); break;
		  case 2 : printf("	switch(pObject->var.oid.id3) {\n"); break;
		  case 3 : printf("	switch(pObject->var.oid.id3) {\n"); break;
		}

		if ((pParent->attr & ATTR_READ) && (pParent->attr & ATTR_WRITE))
			 strcpy(szAttr, "Read/Write");
		else if (pParent->attr & ATTR_READ)
			 strcpy(szAttr, "Read Only");
		else if (pParent->attr & ATTR_WRITE)
			 strcpy(szAttr, "Write Only");
		else strcpy(szAttr, "None");

	if (!(pParent->attr & (ATTR_SMI | ATTR_COMMAND | ATTR_EVENT | ATTR_ALARM)))
	{
		printf("	  case 0 :\n");
		printf("		   // %s(%0d.%0d.%0d) %s(%0d) %s\n",
									pParent->pszName,
									pParent->oid.id1,
									pParent->oid.id2,
									pParent->oid.id3,
									GetTypeName(pParent->type)+7,
									pParent->len, szAttr);

		if (pParent->attr & ATTR_READ)
		{
			printf("		   if (nMethod == VARMETHOD_GET)\n");
			printf("			   VARAPI_CopyValue(pObject, pValue);\n");
		}

		strcpy(szName, pParent->pszName);
		strupr(szName);

		printf("		   else if (nMethod == VARMETHOD_MOVE)\n");
		printf("		   {\n");
		printf("		  	   %s	*pEntry;\n", szName);
		printf("		  	   VARAPI_CopyValue(pObject, pValue);\n");
		printf("		  	   pEntry = (%s *)pValue->stream.p;\n", szName);
		printf("		  	   if (pEntry != NULL)\n");
		printf("		  	   {\n");
		printf("			   	   memset(pEntry, 0, sizeof(%s));\n", szName);
		for(i=0, pNext=pParent->pChild; pNext; i++, pNext=pNext->pNext)
		{
			if (memcmp(&pParent->group, &pNext->group, 3) == 0)
			{
				printf("			   	   VARAPI_GetIndexValue(&pObject->pChild[%0d], (void *)&pEntry->%s, pParam);\n",
										i, pNext->pszName);
			}
		}
		printf("		  	   }\n");
		printf("		   }\n");
		printf("		   return 1;\n");
	}

		for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
		{
			if (memcmp(&pParent->group, &pNext->group, 3) == 0)
			{
				if ((pNext->attr & ATTR_READ) && (pNext->attr & ATTR_WRITE))
					 strcpy(szAttr, "Read/Write");
				else if (pNext->attr & ATTR_READ)
					 strcpy(szAttr, "Read Only");
				else if (pNext->attr & ATTR_WRITE)
					 strcpy(szAttr, "Write Only");
				else strcpy(szAttr, "None");	
				printf("	  case %0d :\n", pNext->id);
				printf("		   // %s(%0d.%0d.%0d) %s(%0d) %s\n",
									pNext->pszName,
									pNext->oid.id1,
									pNext->oid.id2,
									pNext->oid.id3,
									GetTypeName(pNext->type)+7,
									pNext->len, szAttr);


				if (!(pParent->attr & (ATTR_SMI | ATTR_COMMAND | ATTR_EVENT | ATTR_ALARM)))
				{
					printf("		   if (nMethod == VARMETHOD_DEFAULT)\n");
					printf("		   {\n");
					printf("		  	   // Default Value\n");
					printf("		   }\n");
				}

				if (pNext->attr & ATTR_READ)
				{
					if (!(pParent->attr & (ATTR_SMI | ATTR_COMMAND | ATTR_EVENT | ATTR_ALARM)))
						 printf("		   else if (nMethod == VARMETHOD_GET)\n");
					else printf("		   if (nMethod == VARMETHOD_GET)\n");
					printf("			   VARAPI_CopyValue(pObject, pValue);\n");
				}

				if (pNext->attr & ATTR_WRITE)
				{
					printf("		   else if (nMethod == VARMETHOD_SET)\n");
					printf("			   VARAPI_ReplaceValue(pObject, pValue);\n");
				}

				if (!(pParent->attr & (ATTR_SMI | ATTR_COMMAND | ATTR_EVENT | ATTR_ALARM)))
				{
					printf("		   else if (nMethod == VARMETHOD_MOVE)\n");
					printf("		   {\n");
					printf("		  	   // User Operation\n");
					printf("		   }\n");
				}
				printf("		   return 1;\n");
			}
		}

		printf("	}\n");
		printf("	return 0;\n");
		printf("}\n");
	}

	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		if (pNext->pChild != NULL)
			DisplayGroupNode(pNext);
	}
}

void DisplayGroupTitleNode(OBJECT *pParent, BOOL bPrint)
{
	OBJECT	*pNext;

	if (pParent->attr & ATTR_GROUP)
	{
		pParent->pGroup = pParent;
	
		if (!m_bNoHandler && bPrint)
			printf("int %sQuery(VAROBJECT *pObject, int nMethod, MIBValue *pValue, MIBValue **pList, int *nCount, MIBValue *pParam);\n", pParent->pszName);

		for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
		{
			if (pNext->attr & ATTR_GROUP)
				pNext->pGroup = pParent;
			else pNext->pGroup = NULL;
		}
	}

	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		if (pNext->pChild != NULL)
			DisplayGroupTitleNode(pNext, bPrint);
	}
}

void DisplayTree(OBJECT *pParent, int nLevel, BOOL bDoor, BOOL bEnd)
{
	OBJECT	*pNext;
	char	szLine[50], szOid[64];
	int		i, n;

	bDoor = (pParent->attr & ATTR_END) || (pParent->attr & ATTR_DOOR);
	memset(szLine, 0, 50);
	if (bEnd)
	{
		for(i=0; i<=nLevel; i++)
		{
			n = (6*i);
			if (i != nLevel)
			     memcpy(&szLine[n], "      ", 6);
		}
	}
	else if (bDoor)
	{
		for(i=0; i<=nLevel; i++)
		{
			n = (6*i);
			if (i != nLevel)
			     memcpy(&szLine[n], " |    ", 6);
		}
	}
	else
	{
		for(i=0; i<=nLevel; i++)
		{
			n = (6*i);
			if (i != nLevel)
			   memcpy(&szLine[n], " |    ", 6);
		}
	}

	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		OidToString(&pNext->oid, szOid);
		bDoor = bDoor || (pNext->attr & ATTR_DOOR);
		bEnd  = bEnd || (pNext->attr & ATTR_END);
		if (bDoor)
		{
			if (bEnd)
				 memset(szLine, ' ', nLevel*6);
			else memset(&szLine[2], ' ', nLevel*6-2);
		}
		if (bEnd)
		{
			if ((nLevel-1) != 0)
				szLine[(nLevel-1)*6+1] = '|';
		}
		
		if (pNext == pParent->pChild)
		{
			printf("%s |\n", szLine);
		}

		printf("%s +--- %s (%s)\n", szLine, pNext->pszName, szOid);

		if (pNext->pNext == NULL)
			printf("%s\n", szLine);

		if (pNext->pChild != NULL)
			DisplayTree(pNext, nLevel+1, bDoor, bEnd);
	}
}

void FindDoor(OBJECT *pParent, int nLevel)
{
	OBJECT	*pNext;

	for(pNext=pParent->pChild; pNext; pNext=pNext->pNext)
	{
		if (pNext->pChild != NULL)
			FindDoor(pNext, nLevel+1);

		if ((pNext->pNext == NULL) && (nLevel == 0))
			pNext->attr |= ATTR_END;
		else if ((pNext->pNext == NULL) && (nLevel < 2))
			pNext->attr |= ATTR_DOOR;
	}
}

