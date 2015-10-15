#include "common.h"

#include "typedef.h"
#include "varapi.h"
#include "if4frame.h"

#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "logdef.h"

#include "AgentLog.h"


extern int m_nEventLogSize;

void WriteSmiValue(FILE *fp, char *pValue, int nCount, int nLength)
{
    int         i, j, nSeek = 0, len;
    char        *pszValue;
    TIMESTAMP   *t;
    TIMEDATE    *d;
    SMIValue    *p;
    VAROBJECT   *pObject;
    char        szBuffer[80], szValue[10];
    BYTE        type;

    if(fp == NULL) return;

    for(i=0; i<nCount; i++)
    {
        p       = (SMIValue *)(pValue + nSeek);
        pObject = VARAPI_GetObject(&p->oid);
        type    = pObject ? pObject->var.type : VARSMI_UNKNOWN;

        if ((pObject != NULL) && (p->len > 0))
        {
            fprintf(fp, "%4d: %s=", i+1, pObject ? pObject->pszName : "Unknown");
            switch(type) {
              case VARSMI_BOOL :
				   fprintf(fp, "%s(%0d)\xd\xa",
				   p->stream.u8 == 0 ? "FALSE" : "TRUE", p->stream.u8);
				   break;
              case VARSMI_BYTE :
				   fprintf(fp, "%0d\xd\xa", p->stream.u8);
				   break;
              case VARSMI_WORD :
				   fprintf(fp, "%0d\xd\xa", LittleToHostShort(p->stream.u16));
				   break;
              case VARSMI_UINT :
				   fprintf(fp, "%0d\xd\xa", LittleToHostInt(p->stream.u32));
				   break;
              case VARSMI_CHAR :
				   fprintf(fp, "%0d\xd\xa", p->stream.s8);
				   break;
              case VARSMI_SHORT :
				   fprintf(fp, "%0d\xd\xa", (signed short)LittleToHostShort(p->stream.s16));
				   break;
              case VARSMI_INT :
				   fprintf(fp, "%0d\xd\xa", (signed int)LittleToHostInt(p->stream.s32));
				   break;
              case VARSMI_OID :
                   fprintf(fp, "%0d.%0d.%0d\xd\xa",
                            (BYTE)p->stream.id.id1,
                            (BYTE)p->stream.id.id2,
                            (BYTE)p->stream.id.id3);
                   break;
              case VARSMI_IPADDR :
                   fprintf(fp, "%0d.%0d.%0d.%0d\xd\xa",
                            (BYTE)p->stream.str[0],
                            (BYTE)p->stream.str[1],
                            (BYTE)p->stream.str[2],
                            (BYTE)p->stream.str[3]);
                   break;
              case VARSMI_TIMESTAMP :
                   t = (TIMESTAMP *)p->stream.str;
                   fprintf(fp, "%04d/%02d/%02d %02d:%02d:%02d\xd\xa",
                            t->year, t->mon, t->day,
                            t->hour, t->min, t->sec);
                   break;
              case VARSMI_TIMEDATE :
                   d = (TIMEDATE *)p->stream.str;
                   fprintf(fp, "%04d/%02d/%02d\xd\xa", d->year, d->mon, d->day);
                   break;
              case VARSMI_STRING :
                   len = MIN(p->len, 45);
                   pszValue = (char *)MALLOC(len+1);
                   if (pszValue != NULL)
                   {
                       memcpy(pszValue, p->stream.str, len);
                       pszValue[len] = 0;
                       fprintf(fp, "'%s'\xd\xa", pszValue);
                       FREE(pszValue);
                   }
                   break;
              case VARSMI_EUI64 :
                   *szBuffer = '\0';
                   len = MIN(15, p->len);
                   for(j=0; j<len; j++)
                   {
                       sprintf(szValue, "%02X", p->stream.str[j]);
                       strcat(szBuffer, szValue);
                   }
                   fprintf(fp, "%s\xd\xa", szBuffer);
                   break;

              case VARSMI_UNKNOWN :
              case VARSMI_STREAM :
              case VARSMI_OPAQUE :
                   *szBuffer = '\0';
                   len = MIN(15, p->len);
                   for(j=0; j<len; j++)
                   {
                       sprintf(szValue, "%02X ", p->stream.str[j]);
                       strcat(szBuffer, szValue);
                   }
                   fprintf(fp, "%s\xd\xa", szBuffer);
                   break;
            }
        }
        else
        {
            fprintf(fp, "%4d: %s(%0d.%0d.%0d), %s(%0d), LEN=%0d\xd\xa",
                i+1,
                pObject ? pObject->pszName : "Unknown",
                p->oid.id1, p->oid.id2, p->oid.id3,
                VARAPI_GetTypeName(type),
                type,
                p->len);
        }
        nSeek += (p->len + 5);
    }
}

BOOL EVENT_LOG(char * szEventName, char *szSmiValue, int nSmiCount, int nLength)
{
	char	szPath[64];
	TIMESTAMP tmNow;
	FILE	*fp;

	GetTimestamp(&tmNow, NULL);
	sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, EVENT_LOG_FILE,
            tmNow.year, tmNow.mon, tmNow.day);
	fp = fopen(szPath, "a+b");
	if (fp != NULL)
	{
    	fprintf(fp, "%02d/%02d %02d:%02d:%02d %-30s\r\n",
                tmNow.mon, tmNow.day,
				tmNow.hour, tmNow.min, tmNow.sec,
                szEventName);
        if(szSmiValue != NULL && nSmiCount > 0 && nLength > 0) {
            WriteSmiValue(fp, szSmiValue, nSmiCount, nLength);
        }
		fclose(fp);
	}

	CheckLogLimit(szPath, m_nEventLogSize);
	return TRUE;
}

