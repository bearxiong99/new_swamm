#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "typedef.h"
#include "varapi.h"
#include "oidmib.h"

//////////////////////////////////////////////////////////////////////
// Definition Table
//////////////////////////////////////////////////////////////////////

#define OID_ACCEPT				"0123456789."

typedef struct	_tagVARTYPE
{
		char	*pszName;
		BYTE	nType;
		char	*pszTag;
}	VARTYPE;

VARTYPE	m_VarTypes[] =
{
	{ "BOOL",		VARSMI_BOOL,		"VARSMI_BOOL" },
	{ "BYTE",		VARSMI_BYTE,		"VARSMI_BYTE" },
	{ "WORD",		VARSMI_WORD,		"VARSMI_WORD" },
	{ "UINT",		VARSMI_UINT,		"VARSMI_UINT" },
	{ "CHAR",		VARSMI_CHAR,		"VARSMI_CHAR" },
	{ "SHORT",		VARSMI_SHORT,		"VARSMI_SHORT" },
	{ "INT",		VARSMI_INT,			"VARSMI_INT" },
	{ "STRING",		VARSMI_STRING,		"VARSMI_STRING" },
	{ "STREAM",		VARSMI_STREAM,		"VARSMI_STREAM" },
	{ "OPAQUE",		VARSMI_OPAQUE,		"VARSMI_OPAQUE" },
	{ "OID",		VARSMI_OID,			"VARSMI_OID" },
	{ "TIMESTAMP",	VARSMI_TIMESTAMP,	"VARSMI_TIMESTAMP" },
	{ "TIMEDATE",	VARSMI_TIMEDATE,	"VARSMI_TIMEDATE" },
	{ "IPADDR",		VARSMI_IPADDR,		"VARSMI_IPADDR" },
	{ "EUI64",		VARSMI_EUI64,		"VARSMI_EUI64" },
	{ "UNKNOWN",	VARSMI_UNKNOWN,		"VARSMI_UNKNOWN" },
	{ NULL,			0,					NULL }
};

//////////////////////////////////////////////////////////////////////
// Utility Functions
//////////////////////////////////////////////////////////////////////

int GetValueTypeByName(char *pszType)
{
	int		i;

	for(i=0; m_VarTypes[i].pszName; i++)
	{
		if (strcmp(m_VarTypes[i].pszName, pszType) == 0)
			return m_VarTypes[i].nType;
	}
	return VARSMI_UNKNOWN;
}

char *GetTypeName(int nType)
{
	switch(nType) {
	  case VARSMI_UNKNOWN :		return "VARSMI_UNKNOWN";
	  case VARSMI_BOOL :		return "VARSMI_BOOL";
	  case VARSMI_BYTE :		return "VARSMI_BYTE";
	  case VARSMI_WORD :		return "VARSMI_WORD";
	  case VARSMI_UINT :		return "VARSMI_UINT";
	  case VARSMI_CHAR :		return "VARSMI_CHAR";
	  case VARSMI_SHORT :		return "VARSMI_SHORT";
	  case VARSMI_INT :			return "VARSMI_INT";
	  case VARSMI_OID :			return "VARSMI_OID";
	  case VARSMI_STRING :		return "VARSMI_STRING";
	  case VARSMI_STREAM :		return "VARSMI_STREAM";
	  case VARSMI_OPAQUE :		return "VARSMI_OPAQUE";
	  case VARSMI_EUI64 :       return "VARSMI_EUI64";
	  case VARSMI_IPADDR :      return "VARSMI_IPADDR";
	  case VARSMI_TIMESTAMP :   return "VARSMI_TIMESTAMP";
	  case VARSMI_TIMEDATE :    return "VARSMI_TIMEDATE";
	}
	return "VARSMI_UNKNOWN";
}

char *GetTypeNameVar(int nType)
{
	switch(nType) {
	  case VARSMI_UNKNOWN :		return "UNKNOWN";
	  case VARSMI_BOOL :		return "BOOL";
	  case VARSMI_BYTE :		return "BYTE";
	  case VARSMI_WORD :		return "WORD";
	  case VARSMI_UINT :		return "UINT";
	  case VARSMI_CHAR :		return "signed char";
	  case VARSMI_SHORT :		return "short";
	  case VARSMI_INT :			return "int";
	  case VARSMI_OID :			return "OID3";
	  case VARSMI_STRING :		return "char";
	  case VARSMI_STREAM :		return "char";
	  case VARSMI_OPAQUE :		return "char";
	  case VARSMI_EUI64 :       return "EUI64";
	  case VARSMI_IPADDR :      return "BYTE";
	  case VARSMI_TIMESTAMP :   return "TIMESTAMP";
	  case VARSMI_TIMEDATE :    return "TIMEDATE";
	}
	return "VARSMI_UNKNOWN";
}

BOOL StringToOid(char *pszOid, OID3 *pOid)
{   
    int     i, len, dot=0;
    char    *p[5];
    char    c;
    
    if (!pszOid || !pOid)
        return FALSE;
    
    p[0] = pszOid;
	p[1] = p[2] = NULL;
    len  = strlen(pszOid);
    for(i=0; i<len; i++)
    {   
        c = pszOid[i];
        if (!strchr(OID_ACCEPT, c))
            return FALSE;
        if (c == '.')
        {   
            dot++;
            if (dot > 2)
                return FALSE;
            
            p[dot] = &pszOid[i+1];
        }
    }
    
    pOid->id1   = p[0] ? atoi(p[0]) : 0;
    pOid->id2   = p[1] ? atoi(p[1]) : 0;
    pOid->id3   = p[2] ? atoi(p[2]) : 0;
    
    return TRUE;
}

void OidToString(OID3 *oid, char *pszOid)
{
	if (oid->id1 == 0)
	   sprintf(pszOid, "0");
	if (oid->id2 == 0)
	   sprintf(pszOid, "%0d", oid->id1);
	else if (oid->id3 == 0)
	   sprintf(pszOid, "%0d.%0d", oid->id1, oid->id2);
	else sprintf(pszOid, "%0d.%0d.%0d", oid->id1, oid->id2, oid->id3);
}

void strupr(char *pszString)
{
	int		i, len;
	BYTE	c;

	len = strlen(pszString);
	for(i=0; i<len; i++)
	{
		c = pszString[i];
		if ((c >= 'a') && (c <= 'z'))
			pszString[i] = c - 32;
	}
}

