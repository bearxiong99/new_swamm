/** Concentrator Dummy Server Utils.cpp
  *
  */

#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "if4api.h"
#include "varapi.h"
#include "MemoryDebug.h"

void _DumpMibValue(MIBValue *pValue, int nCount);
void _Dump(const char *pszBuffer, int nLength);
BOOL _OidToString(OID3 *pOid, char *pszOid);

/** Dummy Server에서 Request에 대한 Dump를 수행한다.
  *
  */
void DumpRequest(const char * oid, const char * command, MIBValue *pValue, int nCount)
{
    fprintf(stdout, "REQUEST: %s %s\n", oid, command);
    _DumpMibValue(pValue, nCount);
    fprintf(stdout, "ENDREQ\n");
}

BOOL _OidToString(OID3 *pOid, char *pszOid)
{
	if (!pOid || !pszOid)
		return FALSE;

	if (pOid->id1 == 0)
		 sprintf(pszOid, "0.0.0");
	else if (pOid->id2 == 0)
		 sprintf(pszOid, "%0d", pOid->id1);
	else if (pOid->id3 == 0)
		 sprintf(pszOid, "%0d.%0d", pOid->id1, pOid->id2);
	else sprintf(pszOid, "%0d.%0d.%0d", pOid->id1, pOid->id2, pOid->id3);
	return TRUE;
}

void _DumpMibValue(MIBValue *pValue, int nCount)
{
	int			i, len;
	char		szOID[30];
	char		*pszValue;
	TIMESTAMP	*t;
	TIMEDATE	*d;
	MIBValue	*p;
	VAROBJECT	*pObject;
	BYTE		type;

    fprintf(stdout, "  PARAMCNT: %d\n", nCount);

	p = pValue;
	for(i=0; i<nCount && p; i++)
	{
		pObject = VARAPI_GetObject(&p->oid);
		type	= pObject ? pObject->var.type : p->type;
		_OidToString(&p->oid, szOID);

        fprintf(stdout, "  PARAM.%d: %s %s %d\n", 
                i+1,                                    /**< Parameter Index */
                VARAPI_GetTypeName(type),               /**< Type */
                szOID,                                  /**< OID String */
                p->len                                  /**< Value length */
            );
		if (p->len > 0)
		{
		    switch(type) {
 		      case VARSMI_BOOL : 	fprintf(stdout,"    %s\n", p->stream.u8 == 0 ? "FALSE" : "TRUE"); break;
  		      case VARSMI_BYTE :	fprintf(stdout,"    %0d\n", p->stream.u8); break;
  		      case VARSMI_WORD :	fprintf(stdout,"    %0d\n", p->stream.u16); break;
		      case VARSMI_UINT :	fprintf(stdout,"    %0d\n", p->stream.u32); break;
		      case VARSMI_CHAR :	fprintf(stdout,"    %0d\n", p->stream.s8); break;
 		      case VARSMI_SHORT :	fprintf(stdout,"    %0d\n", (signed short)p->stream.s16); break;
 		      case VARSMI_INT : 	fprintf(stdout,"    %0d\n", (signed int)p->stream.s32); break;
		      case VARSMI_OID : 	fprintf(stdout,"    %0d.%0d.%0d\n", p->stream.id.id1, p->stream.id.id2, p->stream.id.id3); break;
  		      case VARSMI_IPADDR : 	fprintf(stdout,"    %0d.%0d.%0d.%0d\n",
											p->stream.p[0], p->stream.p[1],
											p->stream.p[2], p->stream.p[3]);
				   					break;
		      case VARSMI_TIMESTAMP :
				   t = (TIMESTAMP *)p->stream.p;
				   fprintf(stdout,"    %04d/%02d/%02d %02d:%02d:%02d\n",
							t->year, t->mon, t->day,
							t->hour, t->min, t->sec);
				   break;
 		      case VARSMI_TIMEDATE :
				   d = (TIMEDATE *)p->stream.p;
				   fprintf(stdout,"    %04d/%02d/%02d\n", d->year, d->mon, d->day);
				   break;
  		      case VARSMI_STRING :
				   len = min(p->len, 45);
				   pszValue = (char *)MALLOC(len+1);
				   if (pszValue != NULL)
				   {
				   	   memcpy(pszValue, p->stream.p, len);
				   	   pszValue[len] = 0;
				   	   fprintf(stdout,"    '%s'\n", pszValue);
					   FREE(pszValue);
				   }
				   break;
 		      case VARSMI_UNKNOWN :
  		      case VARSMI_EUI64 :
		      case VARSMI_STREAM :
 		      case VARSMI_OPAQUE :
				   _Dump(p->stream.p, p->len);
				   break;
		    }
		}
		p = p->pNext;
	}
}

void _Dump(const char *pszBuffer, int nLength)
{
	char	*pszString;
    char    szChar[10];
	BYTE	c;
    int     i, len, n;

	if (!pszBuffer || !nLength)
		return;

	len = (nLength*4) + (nLength/25)*4 + 25; 
	pszString = (char *)malloc(len);
	if (pszString != NULL)
	{
		*pszString = '\0';
   		for(i=0,n=0; i<nLength; i++)
  	  	{
	        c = pszBuffer[i];
  	      	sprintf(szChar, "%02X ", c);
			n++;
        	strcat(pszString, szChar);
			if (i && ((i+1) % 25) == 0)
			{
				strcat(pszString, "\n    ");
				n = 0;
			}
    	}

	   	fprintf(stdout, "    %s\n", pszString);
		free(pszString);
	}
}
