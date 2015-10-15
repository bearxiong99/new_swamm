/** Concentrator Dummy Server Utils.cpp
  *
  */

#include "common.h"
#include "if4api.h"
#include "MemoryDebug.h"
#include "codiapi.h"
#include "endiFrame.h"
#include "CommonUtil.h"

char flowBuffer[1024*10];
int  flowLen=0;
int  flowTotLen=0;
BOOL flowStart=FALSE;

void _DumpMibValue(MIBValue *pValue, int nCount);
void _Dump(const char *pszBuffer, int nLength);
BOOL _OidToString(OID3 *pOid, char *pszOid);
void _DumpKetiValue(MIBValue *pValue, int nCount);

#ifdef _WIN32
#pragma pack(push, 1)
#endif	/* _WIN32 */
typedef struct
{
    BYTE        scid[5];
    TIMESTAMP   sct;
    WORD        ct;
    WORD        ch;
    WORD        cb;
    BYTE        period;
    BYTE        tu;
    BYTE        hu;
    BYTE        bu;
}	__ATTRIBUTE_PACKED__ KETI_TEMP_ST;

typedef struct
{
    WORD        cx;
    WORD        cy;
    WORD        cz;
    WORD        cg;
}	__ATTRIBUTE_PACKED__ KETI_ACC_LP_ST;

typedef struct
{
    BYTE        scid[5];
    TIMESTAMP   sct;
    WORD        cx;
    WORD        cy;
    WORD        cz;
    WORD        cg;
    WORD        cb;
    BYTE        period;
    BYTE        bcnt;
    TIMESTAMP   lt;
    BYTE        lcnt;
    KETI_ACC_LP_ST lp[0];
}	__ATTRIBUTE_PACKED__ KETI_ACC_ST;


typedef struct
{
    BYTE        scid[5];
    TIMESTAMP   sct;
    UINT        ctv;
    UINT        ci;
    UINT        ca;
    UINT        cp;
    BYTE        period;
    BYTE        tvu;
    BYTE        iu;
    BYTE        au;
    BYTE        pu;
}	__ATTRIBUTE_PACKED__ KETI_FLOW_ST;
#ifdef _WIN32
#pragma pack(pop)
#endif	/* _WIN32 */

/** Dummy Server에서 Request에 대한 Dump를 수행한다.
  *
  */
void DumpRequest(const char * oid, const char * command, MIBValue *pValue, int nCount)
{
    fprintf(stdout, "REQUEST: %s %s\n", oid, command); fflush(stdout);
    _DumpMibValue(pValue, nCount);
    fprintf(stdout, "ENDREQ\n"); fflush(stdout);
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

    fprintf(stdout, "  PARAMCNT: %d\n", nCount); fflush(stdout);

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
            ); fflush(stdout);
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
				   len = MIN(p->len, 45);
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
        fflush(stdout);
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

	   	fprintf(stdout, "    %s\n", pszString); fflush(stdout);
		free(pszString);
	}
}

void DumpKetiRequest(const char * oid, const char * command, MIBValue *pValue, int nCount)
{
    //fprintf(stdout, "========== KETI PROJECT ==========\n"); fflush(stdout);
    _DumpKetiValue(pValue, nCount);
    //fprintf(stdout, "==================================\n"); fflush(stdout);
}

void _DumpKetiValue(MIBValue *pValue, int nCount)
{
#if 0
    int                     seq;
    ENDI_3PARTY_FRAME *     frame;
    APP_3PARTY_PAYLOAD *    payload;
    BYTE                    mid[32];
    
    
    if(nCount < 7) return;      // invalid param

    seq = pValue[2].stream.u8;
    
    //fprintf(stdout, "  seq: %d\n", seq); fflush(stdout);
    frame = (ENDI_3PARTY_FRAME *)pValue[4].stream.p;

    /** STX 검사 */
    if (frame->hdr.stx != 0x02)  return;

    if(frame->hdr.seq == 0x01)
    {
        payload = (APP_3PARTY_PAYLOAD *) frame->payload;
        if(payload->hdr.stx != 0x02) return;
        /*
        fprintf(stdout, "Add 3rd Party Sensor Vendor 0x%02X, Model 0x%02X\n", 
                    payload->hdr.vendor, payload->hdr.model); fflush(stdout);
        fprintf(stdout, "Frame Len = %d\r\n", frame->hdr.len);
        */
        switch(payload->hdr.vendor)
        {
            case 0x0B: // KETI
                switch(payload->hdr.model)
                {
                    case 0x01: // Temperature
                        KETI_TEMP_ST * tempSt = (KETI_TEMP_ST *) payload->payload;
                        memset(mid, 0, sizeof(mid));
                        strncpy((char *)mid, (char *)tempSt->scid, sizeof(tempSt->scid));
                        fprintf(stdout, "========== KETI TEMPERATURE ==========\n");
                        fprintf(stdout, " ID %s (%04d/%02d/%02d %02d:%02d:%02d)\n",
                                    mid, 
                                    BigToHostShort(tempSt->sct.year),
                                    tempSt->sct.mon,
                                    tempSt->sct.day,
                                    tempSt->sct.hour,
                                    tempSt->sct.min,
                                    tempSt->sct.sec);
                        fprintf(stdout, "  + CT %2d CH %2d CB %2d\n",
                                    BigToHostShort(tempSt->ct),
                                    BigToHostShort(tempSt->ch),
                                    BigToHostShort(tempSt->cb));
                        fprintf(stdout, "  + TU %2d HU %2d BU %2d\n",
                                    tempSt->tu,
                                    tempSt->hu,
                                    tempSt->bu);
                        fprintf(stdout, "--------------------------------------\n");
                        fflush(stdout);
                        break;
                    case 0x02: // Accelation
                        flowTotLen = BigToHostShort(payload->hdr.len);
                        if((UINT)flowTotLen > sizeof(flowBuffer))
                        {
                            flowTotLen = 0;
                            return;
                        }
                        //fprintf(stdout, "flowTotLen=%d\n", flowTotLen);
                        flowLen=frame->hdr.len;
                        memcpy(flowBuffer, payload, flowLen);
                        flowStart = TRUE;
                        break;
                    case 0x03: // Flow
                        KETI_FLOW_ST * flowSt = (KETI_FLOW_ST *) payload->payload;
                        memset(mid, 0, sizeof(mid));
                        strncpy((char *)mid, (char *)flowSt->scid, sizeof(flowSt->scid));
                        fprintf(stdout, "========== KETI FLOW METER ===========\n");
                        fprintf(stdout, " ID %s (%04d/%02d/%02d %02d:%02d:%02d)\n",
                                    mid, 
                                    BigToHostShort(flowSt->sct.year),
                                    flowSt->sct.mon,
                                    flowSt->sct.day,
                                    flowSt->sct.hour,
                                    flowSt->sct.min,
                                    flowSt->sct.sec);
                        fprintf(stdout, "  + CTV %2d CI %2d CA %2d CP %2d\n",
                                    BigToHostInt(flowSt->ctv),
                                    BigToHostInt(flowSt->ci),
                                    BigToHostInt(flowSt->ca),
                                    BigToHostInt(flowSt->cp));
                        fprintf(stdout, "  + TVU %2d IU %2d AU %2d PU %2d\n",
                                    flowSt->tvu,
                                    flowSt->iu,
                                    flowSt->au,
                                    flowSt->pu);
                        fprintf(stdout, "--------------------------------------\n");
                        fflush(stdout);
                        break;
                }
                break;
        }
    }
    else
    {
        int tlen = frame->hdr.len;
        int i,lcnt;
                        
        if(!flowStart) return;

        if((UINT)(tlen + flowLen) > sizeof(flowBuffer))
        {
            flowLen = 0;
            flowTotLen = 0;
            flowStart = FALSE;
            return;
        }
        memcpy(flowBuffer + flowLen, frame->payload, tlen);
        flowLen += tlen;

        /*
        fprintf(stdout,"seq %d flowLen %d totLen %d tlen %d\n",
            seq, flowLen, flowTotLen, tlen);
            */

        if(flowLen >= flowTotLen)
        {
            KETI_ACC_ST * accSt = (KETI_ACC_ST *) ((APP_3PARTY_PAYLOAD *)flowBuffer)->payload;

            flowLen = 0;
            flowTotLen = 0;
            flowStart = FALSE;

            memset(mid, 0, sizeof(mid));
            strncpy((char *)mid, (char *)accSt->scid, sizeof(accSt->scid));
            fprintf(stdout, "========== KETI ACC SENSOR ===========\n");
            fprintf(stdout, " ID %s (%04d/%02d/%02d %02d:%02d:%02d)\n",
                        mid, 
                        BigToHostShort(accSt->sct.year),
                        accSt->sct.mon,
                        accSt->sct.day,
                        accSt->sct.hour,
                        accSt->sct.min,
                        accSt->sct.sec);
            fprintf(stdout, "  + CX %02d CY %02d CZ %02d CG %02d CB %02d\n",
                        BigToHostShort(accSt->cx),
                        BigToHostShort(accSt->cy),
                        BigToHostShort(accSt->cz),
                        BigToHostShort(accSt->cg),
                        BigToHostShort(accSt->cb));
            fprintf(stdout, "  + LP (%04d/%02d/%02d %02d:%02d:%02d) Cnt %02d\n",
                        BigToHostShort(accSt->lt.year),
                        accSt->lt.mon,
                        accSt->lt.day,
                        accSt->lt.hour,
                        accSt->lt.min,
                        accSt->lt.sec,
                        accSt->lcnt);
            for(i=0,lcnt=accSt->lcnt;i<lcnt;i++)
            {
                fprintf(stdout, "     %02d CX %6d CY %6d CZ %6d CG %6d\n",
                            i+1,
                            BigToHostShort(accSt->lp[i].cx),
                            BigToHostShort(accSt->lp[i].cy),
                            BigToHostShort(accSt->lp[i].cz),
                            BigToHostShort(accSt->lp[i].cg));
            }
            fprintf(stdout, "--------------------------------------\n");
            fflush(stdout);
        }
    }
#endif

}

