#include <math.h>
#include "common.h"
#include "cmd_util.h"
#include "Variable.h"
#include "if4frame.h"
#include "varapi.h"
#ifndef __USE_STDOUT__
#include "CLIInterface.h"
#include "CLIUtility.h"
#else
#include "clitypes.h"
#endif
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "gpiomap.h"
#include "codiapi.h"

#include "vendor/ansi.h"
#include "vendor/aidondef.h"
#include "vendor/repdef.h"
#include "vendor/ihddef.h"
#include "vendor/kamstrup.h"

#include "cmd_message.h"
#include "codeUtility.h"

#include "dlmsFunctions.h"
#include "dlmsUtils.h"
#include "dlmsFrame.h"
#include "dlmsObisDefine.h"

#include "mbusFrame.h"
#include "mbusFunctions.h"

#include "kmpFrame.h"
#include "kmpDefine.h"
#include "kmpFunctions.h"

#include "kukdongFrame.h"

#define STATE_SD_READ_INFO          2
#define STATE_SD_READ_LP_HEADER     3
#define STATE_SD_LP_ENTRY           4
#define STATE_SD_OBIS               5
#define STATE_SD_READ_SOLAR_LOG     6
#define STATE_SD_READ_TYPE          7
#define STATE_SD_READ_BASE          8
#define STATE_SD_MD                 9
#define STATE_SD_CH_INFO            10
#define STATE_SD_EXTRA              11
#define STATE_SD_DONE               0

const char	*dayofweek[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char * _getKepcoVendorString(BYTE nVendor);
const char * _getKepcoAdditionalEquipment(BYTE nValue);


UINT m_nKmpReadIdx = 0;
UINT m_nKmpLastIdx = 0;

#ifdef __USE_STDOUT__
#define PRINTOUT(...)       fprintf(stdout, __VA_ARGS__)
#define DUMP(...)           FDUMP(stdout, __VA_ARGS__)
#else
#define PRINTOUT(...)       OUTTEXT(pSession, __VA_ARGS__)
#define DUMP(...)           OUTDUMP(pSession, __VA_ARGS__)
#endif


int GetHexaList(char *szSource, char *szHexaList, int maxLen, int *nLength)
{
    char * str;
    char * saveptr = NULL;
    char * token;
    int idx;
    UINT val;

    if(!szSource || !szHexaList || !nLength) return CLIERR_ERROR;

    for(idx=0, str=szSource; ; str = NULL)
    {
        token = strtok_r(str, " ", &saveptr);
        if(token == NULL) break;

        val = (UINT) strtol(token, (char **)NULL, 16);

        if(val > 0xFF) 
        {
            return CLIERR_INVALID_PARAM;
        }
        if(idx >= maxLen) return CLIERR_INVALID_PARAM;

        szHexaList[idx] = val; idx++;
    }

    *nLength = idx;

    return CLIERR_OK;
}

#ifdef __USE_STDOUT__
void PrintOutStream(BYTE *pszStream, int nLength)
#else
void PrintOutStream(CLISESSION *pSession, BYTE *pszStream, int nLength)
#endif
{
	char	*p, var[10];
	int		i;

	p = (char *)MALLOC(nLength*3+10);
	if (p == NULL)
		return;

	*p = '\0';
	for(i=0; i<nLength; i++)
	{
		sprintf(var, "%02X ", pszStream[i]);
		strcat(p, var);
		if (i && ((i % 25) == 0))
			strcat(p, "\r\n");
	}
	PRINTOUT(p);
	FREE(p);
}

void PrintOutStreamToString(char * pszString, BYTE *pszStream, int nLength)
{
	char	var[10];
	int		i;

	if (pszString == NULL)
		return;

	*pszString = '\0';
	for(i=0; i<nLength; i++)
	{
		sprintf(var, "%02X ", pszStream[i]);
		strcat(pszString, var);
	}
}

char *itoa_comma(char *buf, int nValue)
{
	char	temp[20];
	int		i, len, nSeek=0;

	sprintf(temp, "%0d", nValue);
	len = strlen(temp);

	for(i=0; i<len; i++)
	{
		if ((i!=0) && ((len-i)%3) == 0)
		{
			buf[nSeek] = ',';
			nSeek++;
		}
		buf[nSeek] = temp[i];
		nSeek++;
	}
	buf[nSeek] ='\0';
	return buf;
}

void SPrintMIBValue(char * pszString, BYTE nType, MIBValue *pValue, int nMaxLen)
{
	int			len;
	TIMESTAMP	*t;
	TIMEDATE	*d;
	MIBValue	*p;
	//VAROBJECT	*pObject;

	p = pValue;
	//pObject = VARAPI_GetObject(&p->oid);

	if (p->len > 0)
	{
	    switch(nType) {
 	      case VARSMI_BOOL : 	{
                                    BOOL b;
                                    memcpy(&b, p->stream.p, sizeof(BOOL));
                                    sprintf(pszString, "%s", b ? "TRUE" : "FALSE");
                                }
								break;
  	      case VARSMI_BYTE :	{
                                    BYTE bv;
                                    memcpy(&bv, p->stream.p, sizeof(BYTE));
                                    sprintf(pszString, "%0d", bv); 
                                }
                                break;
  	      case VARSMI_WORD :	{
                                    WORD wv;
                                    memcpy(&wv, p->stream.p, sizeof(WORD));
                                    sprintf(pszString, "%0d", wv);
                                }
                                break;
	      case VARSMI_UINT :	{
                                    UINT uv;
                                    memcpy(&uv, p->stream.p, sizeof(UINT));
                                    sprintf(pszString, "%0d", uv); 
                                }
                                break;
	      case VARSMI_CHAR :	{
                                    char cv;
                                    memcpy(&cv, p->stream.p, sizeof(char));
                                    sprintf(pszString, "0x%02X", cv); 
                                }
                                break;
	      case VARSMI_SHORT :	{
                                    short sv;
                                    memcpy(&sv, p->stream.p, sizeof(short));
                                    sprintf(pszString, "%0d", sv); 
                                }
                                break;
	      case VARSMI_INT :	    {
                                    INT iv;
                                    memcpy(&iv, p->stream.p, sizeof(INT));
                                    sprintf(pszString, "%0d", iv); 
                                }
                                break;
	      case VARSMI_OID :	    {
                                    OID3 ov;
                                    memcpy(&ov, p->stream.p, sizeof(OID3));
                                    VARAPI_OidToString(&ov, pszString);
                                }
                                break;
  	      case VARSMI_IPADDR : 	sprintf(pszString, "%0d.%0d.%0d.%0d",
										p->stream.p[0], p->stream.p[1],
										p->stream.p[2], p->stream.p[3]);
			   					break;
	      case VARSMI_TIMESTAMP :
			   t = (TIMESTAMP *)p->stream.p;
			   sprintf(pszString, "%04d/%02d/%02d %02d:%02d:%02d",
						t->year, t->mon, t->day,
						t->hour, t->min, t->sec);
			   break;
 	      case VARSMI_TIMEDATE :
			   d = (TIMEDATE *)p->stream.p;
			   sprintf(pszString, "%04d/%02d/%02d", d->year, d->mon, d->day);
			   break;
  	      case VARSMI_STRING :
			   len = MIN(p->len, nMaxLen);
               strncpy(pszString, p->stream.p, len);
			   pszString[len] = 0;
			   break;
  	      case VARSMI_EUI64 :
               {
                   EUI64 *id = (EUI64 *)p->stream.p;
                   EUI64ToStr(id, pszString);
               }
			   break;
 	      case VARSMI_UNKNOWN :
	      case VARSMI_STREAM :
 	      case VARSMI_OPAQUE :
			   PrintOutStreamToString(pszString, (BYTE *)p->stream.p, (int)(MIN(p->len, nMaxLen)/3));
			   break;
	    }
	}
}

#ifdef __USE_STDOUT__
void PrintOutMIBValue(MIBValue *pValue, int nCount)
#else
void PrintOutMIBValue(CLISESSION *pSession, MIBValue *pValue, int nCount)
#endif
{
	int			i, len;
	char		szOID[30];
	char		*pszValue;
	TIMESTAMP	*t;
	TIMEDATE	*d;
	MIBValue	*p;
	VAROBJECT	*pObject;
	BYTE		type;

	p = pValue;
	for(i=0; i<nCount; i++)
	{
		pObject = VARAPI_GetObject(&p->oid);
		type	= pObject ? pObject->var.type : 0;
		VARAPI_OidToString(&p->oid, szOID);

		if (p->len > 0)
		{
			PRINTOUT(" %5d %s(%s), %s(%0d), LEN=%0d, VALUE=", i+1,
				pObject ? pObject->pszName : "Unknown",
				szOID, VARAPI_GetTypeName(type), type, p->len);

		    switch(type) {
 		      case VARSMI_BOOL : 	PRINTOUT("%s(%0d)\xd\xa",
											p->stream.u8 == 0 ? "FALSE" : "TRUE", p->stream.u8);
									break;
  		      case VARSMI_BYTE :	PRINTOUT("%0d\xd\xa", p->stream.u8); break;
  		      case VARSMI_WORD :	PRINTOUT("%0d\xd\xa", p->stream.u16); break;
		      case VARSMI_UINT :	PRINTOUT("%0d\xd\xa", p->stream.u32); break;
		      case VARSMI_CHAR :	PRINTOUT("%0d\xd\xa", p->stream.s8); break;
 		      case VARSMI_SHORT :	PRINTOUT("%0d\xd\xa", p->stream.s16); break;
 		      case VARSMI_INT : 	PRINTOUT("%0d\xd\xa", p->stream.s32); break;
		      case VARSMI_OID : 	PRINTOUT("%0d.%0d.%0d\xd\xa",
											p->stream.id.id1, p->stream.id.id2, p->stream.id.id3);
									break;
  		      case VARSMI_IPADDR : 	PRINTOUT("%0d.%0d.%0d.%0d\xd\xa",
											p->stream.p[0], p->stream.p[1],
											p->stream.p[2], p->stream.p[3]);
				   					break;
		      case VARSMI_TIMESTAMP :
				   t = (TIMESTAMP *)p->stream.p;
				   PRINTOUT("%04d/%02d/%02d %02d:%02d:%02d\xd\xa",
							t->year, t->mon, t->day,
							t->hour, t->min, t->sec);
				   break;
 		      case VARSMI_TIMEDATE :
				   d = (TIMEDATE *)p->stream.p;
				   PRINTOUT("%04d/%02d/%02d\xd\xa", d->year, d->mon, d->day);
				   break;
  		      case VARSMI_STRING :
				   len = MIN(p->len, 45);
				   pszValue = (char *)MALLOC(len+1);
				   if (pszValue != NULL)
				   {
				   	   memcpy(pszValue, p->stream.p, len);
				   	   pszValue[len] = 0;
				   	   PRINTOUT("'%s'\xd\xa", pszValue);
					   FREE(pszValue);
				   }
				   break;
 		      case VARSMI_UNKNOWN :
  		      case VARSMI_EUI64 :
		      case VARSMI_STREAM :
 		      case VARSMI_OPAQUE :
#ifdef __USE_STDOUT__
				   PrintOutStream((BYTE *)p->stream.p, MIN(p->len, 15));
#else
				   PrintOutStream(pSession, (BYTE *)p->stream.p, MIN(p->len, 15));
#endif
				   PRINTOUT("\xd\xa");
				   break;
		    }
		}
		else
		{
			PRINTOUT(" %-5d %s(%s), %s(%0d), LEN=%0d\xd\xa", i+1,
				pObject ? pObject->pszName : "Unknown",
				szOID, VARAPI_GetTypeName(type), type, p->len);
		}
		p = p->pNext;
	}
}

#ifdef __USE_STDOUT__
void PrintOutSmiValue(char *pValue, int nCount)
#else
void PrintOutSmiValue(CLISESSION *pSession, char *pValue, int nCount)
#endif
{
	int			i, j, nSeek = 0, len;
	char		*pszValue;
	TIMESTAMP	*t;
	TIMEDATE	*d;
	SMIValue	*p;
	VAROBJECT	*pObject;
	char		szBuffer[80], szValue[10];
	BYTE		type;

	for(i=0; i<nCount; i++)
	{
		p 		= (SMIValue *)(pValue + nSeek);
		pObject = VARAPI_GetObject(&p->oid);
		type	= pObject ? pObject->var.type : 0;

		if (p->len > 0)
		{
			PRINTOUT("%4d: %s=", i+1, pObject ? pObject->pszName : "Unknown");
		    switch(type) {
 		      case VARSMI_BOOL :
				   PRINTOUT("%s(%0d)\xd\xa",
							p->stream.u8 == 0 ? "FALSE" : "TRUE", 
							p->stream.u8);
				   break;
  		      case VARSMI_BYTE :	PRINTOUT("%0d\xd\xa", p->stream.u8); break;
  		      case VARSMI_WORD :	PRINTOUT("%0d\xd\xa", p->stream.u16); break;
		      case VARSMI_UINT :	PRINTOUT("%0d\xd\xa", p->stream.u32); break;
		      case VARSMI_CHAR :	PRINTOUT("%0d\xd\xa", p->stream.s8); break;
 		      case VARSMI_SHORT :	PRINTOUT("%0d\xd\xa", p->stream.s16); break;
 		      case VARSMI_INT : 	PRINTOUT("%0d\xd\xa", p->stream.s32); break;
		      case VARSMI_OID :
				   PRINTOUT("%0d.%0d.%0d\xd\xa",
							(BYTE)p->stream.id.id1,
							(BYTE)p->stream.id.id2,
							(BYTE)p->stream.id.id3);
				   break;
  		      case VARSMI_IPADDR :
				   PRINTOUT("%0d.%0d.%0d.%0d\xd\xa",
							(BYTE)p->stream.str[0],
							(BYTE)p->stream.str[1],
							(BYTE)p->stream.str[2],
							(BYTE)p->stream.str[3]);
				   break;
		      case VARSMI_TIMESTAMP :
				   t = (TIMESTAMP *)p->stream.str;
				   PRINTOUT("%04d/%02d/%02d %02d:%02d:%02d\xd\xa",
							t->year, t->mon, t->day,
							t->hour, t->min, t->sec);
				   break;
 		      case VARSMI_TIMEDATE :
				   d = (TIMEDATE *)p->stream.str;
				   PRINTOUT("%04d/%02d/%02d\xd\xa", d->year, d->mon, d->day);
				   break;
  		      case VARSMI_STRING :
				   len = MIN(p->len, 45);
				   pszValue = (char *)MALLOC(len+1);
				   if (pszValue != NULL)
				   {
				   	   memcpy(pszValue, p->stream.str, len);
				   	   pszValue[len] = 0;
				   	   PRINTOUT("'%s'\xd\xa", pszValue);
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
				   PRINTOUT("%s\xd\xa", szBuffer);
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
				   PRINTOUT("%s\xd\xa", szBuffer);
				   break;
		    }
		}
		else
		{
			PRINTOUT("%4d: %s(%0d.%0d.%0d), %s(%0d), LEN=%0d\xd\xa", 
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

void ResetSystem()
{
	system("sync");
	usleep(1000000);
	system("sync");
	usleep(1000000);
	system("sync");
	usleep(1000000);
    
    system("reboot");
}

void getStream2DayString(char * data, char * buff)
{
    char metering;
    int i,j;
    int skip=1,start=-1,last=4;
    int conti = 0;
    int isFirst = 1;
    int idx=1;
    unsigned char uc;

    data += last-1;
    for(i=0; i< last; i++) {
        uc = (unsigned char) *data;
        if(skip) uc >>= 1;
        for(j=skip; j< 8; j++, idx++) {
            metering = uc & 0x01;
            uc >>= 1;
            if(metering) {
                if(conti) continue;
                conti = 1;
                start = idx;
            }else {
                if(!conti) continue;
                conti = 0;
                if(isFirst) isFirst = 0;
                else sprintf(buff,",");
                buff += strlen(buff);

                sprintf(buff, "%02d", start);
                buff += strlen(buff);
                if(start != idx-1) {
                    sprintf(buff, "~%02d", idx-1);
                    buff += strlen(buff);
                }
                start = -1;
            }
        }
        data--;
        skip = 0;
    }
    if(start>-1) {
        if(!isFirst) sprintf(buff,",");
        buff += strlen(buff);

        sprintf(buff, "%02d", start);
        buff += strlen(buff);
        if(start != idx-1) {
            sprintf(buff, "~%02d", idx-1);
            buff += strlen(buff);
        }
    }
}

void _getHourMin(int idx, int * hour, int * min)
{
    int m = idx * 15;
    *hour = (int)(m/60);
    *min = m % 60;
}

void getStream2HourString(char * data, char * buff)
{
    char metering;
    int i,j;
    int start=-1,last=12;
    int conti = 0;
    int isFirst = 1;
    int hour, min, idx=0;
    unsigned char uc;

    data += last-1;
    for(i=0; i< last; i++) {
        uc = (unsigned char) *data;
        for(j=0; j< 8; j++, idx++) {
            metering = uc & 0x01;
            uc >>= 1;
            if(metering) {
                if(conti) continue;
                conti = 1;
                start = idx;
            }else {
                if(!conti) continue;
                conti = 0;
                if(isFirst) isFirst = 0;
                else sprintf(buff,",");
                buff += strlen(buff);

                if(start != idx-1) {
                    _getHourMin(start, &hour, &min);
                    sprintf(buff, "%02d:%02d~", hour, min);
                }
                buff += strlen(buff);
                _getHourMin(idx-1, &hour, &min);
                sprintf(buff, "%02d:%02d", hour, min);
                buff += strlen(buff);
                start = -1;
            }
        }
        data--;
    }
    if(start>-1) {
        if(!isFirst) sprintf(buff,",");
        buff += strlen(buff);

        if(start != idx-1) {
            _getHourMin(start, &hour, &min);
            sprintf(buff, "%02d:%02d~", hour, min);
        }
        buff += strlen(buff);
        _getHourMin(idx-1, &hour, &min);
        sprintf(buff, "%02d:%02d", hour, min);
    }
}

char * CollapseString(const char * origStr, char * collapsedStr, int nCollapsedStrLen, char collapseChar)
{
    int len;

    if(!origStr) return collapsedStr;

    len = strlen(origStr);

    if(len >= nCollapsedStrLen - 1)
    {
        len = nCollapsedStrLen - 3;
        collapsedStr[nCollapsedStrLen - 3] = collapseChar;
        collapsedStr[nCollapsedStrLen - 2] = collapseChar;
    }

    memcpy(collapsedStr, origStr, len);

    return collapsedStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if     defined(_WIN32)
#pragma pack(push, 1)
#endif
typedef struct  _tagGETABLE
{
        char    table[4];
        WORD    len;
        BYTE    data[0];
}   __ATTRIBUTE_PACKED__ GETABLE;
#if     defined(_WIN32)
#pragma pack(pop)
#endif

#ifdef __USE_STDOUT__
void ge_data_parsing(char *mdata, UINT length, BOOL bDetail)
#else
void ge_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail)
#endif
{
    GETABLE *pHeader;
    UINT     pos, nSeek;
    char    tname[5], buffer[256];
	char	*p;
    int     tcnt=0,tsize=0, tnum, i, j, k, len;
    WORD    /*nbrBlksSet1 = 0,*/ nbrBlkIntsSet1 = 0;
    BYTE    nbrChnsSet1 = 2;
    //BYTE    maxIntTimeSet1 = 15;
    UINT    lpLen = 0;
    int     nbr_uom_entries=62;
    WORD    scalars_set1[2], divisors_set1[2];
	unsigned long long summation_ch1 = 0, summation_ch2 = 0;
    WORD    vSqrHrSf=0, vRmsSf=0, vAhSf=0;
    INT     iSqrHrSf=0, vASf=0;
    BOOL    bCont = TRUE;
    ORDER   nOrder = ORDER_LSB;
    BYTE    arrChannelSelected[64];
    BYTE    arrChannelUnits[64];
    BYTE    /*nMeterType = 0,*/ nMeterMode = 0;
    long double displayConstant = (long double)(1 / 400000);

    memset(tname, 0, sizeof(tname));
    memset(arrChannelSelected, 0, sizeof(arrChannelSelected));
    memset(arrChannelUnits, 0, sizeof(arrChannelUnits));

    /** TODO : SM110의 경우 Little Endian을 사용한다.
      *        ANSI Meter를 위해서는 Table 0 을 읽어서 LSB/MSB 여부를 판단한 후
      *        맞는 Ordering 함수를 사용해야 한다.
      */
    /** pre reading */
    p = (char *)mdata;
	for(pos=0, nSeek=0; pos<length && bCont; pos+=nSeek)
	{
		pHeader = (GETABLE *)p;
	    nSeek = sizeof(GETABLE);

	    if(bDetail) {
            strncpy(tname, pHeader->table, 4);
            tnum = (int)strtol(tname+1,(char **)NULL,10);
            switch(tname[0]) {
            case 'S':
                switch(tnum) {
                    case 0: {    
                        ANSI_ST_TABLE0 *ts = (ANSI_ST_TABLE0 *) (p + nSeek);
                        nOrder = ts->DATA_ORDER ? ORDER_MSB : ORDER_LSB;
                    } break;
                    // Channel Configuration lookup (ST 61, 62)
                    case 61: {
                        ANSI_ST_TABLE61 *ts = (ANSI_ST_TABLE61 *) (p + nSeek);
                        nbrChnsSet1 = ts->NBR_CHNS_SET1;
                    } break;
                    case 62: {  
                        LP_SEL_SET1 *pLpSelSet1;
                        j = MIN(nbrChnsSet1, sizeof(arrChannelSelected));
                        for(i=0;i<j;i++) {
                            pLpSelSet1 = (LP_SEL_SET1 *)(p + nSeek + (sizeof(LP_SEL_SET1)*i));
                            arrChannelSelected[i] = pLpSelSet1->LP_SOURCE_SELECT;
                        }
                    } break;
				}
                break;
            case 'M':
                switch(tnum) {
					case 0 : {
						ANSI_MT_TABLE0 *ts = (ANSI_MT_TABLE0 *) (p + nSeek);
                        //nMeterType = ts->METER_TYPE;
                        nMeterMode = ts->METER_MODE;
					} break;
					case 75 : {
						ANSI_MT_TABLE75 *ts = (ANSI_MT_TABLE75 *) (p + nSeek);

                        vSqrHrSf = CondToHostShort(nOrder, ts->V_SQR_HR_SF);
                        vRmsSf = CondToHostShort(nOrder, ts->V_RMS_SF);
                        iSqrHrSf = CondToHostInt(nOrder, Word24ToUint(ts->I_SQR_HR_SF));
                        vASf = CondToHostShort(nOrder, Word24ToUint(ts->VA_SF));
                        vAhSf = CondToHostShort(nOrder, ts->VAH_SF);

                        // Display 상수 일단 vASf 값으로 가정한다. 이 값이 일반적으로 1/400000 이다. 
                        // 원칙적으로는 vASf, vAhSf 값을 나누어서 처리해야 한다.
                        displayConstant = (long double) vASf *  pow(10, -9);

                        bCont = FALSE;
					} break;
				}
                break;
            }
        }
 	    nSeek += BigToHostShort(pHeader->len);
		p += nSeek;
	}

    /** reading */
    p = (char *)mdata;
	for(pos=0, nSeek=0; pos<length; pos+=nSeek)
	{
		pHeader = (GETABLE *)p;
	    nSeek = sizeof(GETABLE);

	    if(bDetail) {
            strncpy(tname, pHeader->table, 4);
            tnum = (int)strtol(tname+1,(char **)NULL,10);
            PRINTOUT("          %4s : %d\xd\xa", tname, BigToHostShort(pHeader->len));
            switch(tname[0]) {
            case 'A':
            case 'B':
            case 'S':
                switch(tnum) {
                    case 0: {    
                        ANSI_ST_TABLE0 *ts = (ANSI_ST_TABLE0 *) (p + nSeek);
                        PRINTOUT("                + ORDER(%s)\xd\xa", 
                                    ts->DATA_ORDER ? "MSB" : "LSB"); 
                    } break;
                    case 1: {    
                        ANSI_ST_TABLE1 *ts = (ANSI_ST_TABLE1 *) (p + nSeek);
                        PRINTOUT("                + HW v%d r%d\xd\xa", 
                                    ts->HW_VERSION_NUMBER, ts->HW_REVISION_NUMBER);
                        PRINTOUT("                + FW v%d r%d\xd\xa", 
                                    ts->FW_VERSION_NUMBER, ts->FW_REVISION_NUMBER);
                        strncpy(buffer, (char *)ts->MFG_SERIAL_NUMBER, sizeof(ts->MFG_SERIAL_NUMBER)); buffer[sizeof(ts->MFG_SERIAL_NUMBER)] = 0x00;
                        PRINTOUT("                + MFGID %s\xd\xa", buffer);
                        strncpy(buffer, (char *)ts->ED_MODEL, sizeof(ts->ED_MODEL)); buffer[sizeof(ts->ED_MODEL)] = 0x00;
                        PRINTOUT("                + MODEL %s\xd\xa", buffer);
                    } break;
                    case 3: {    
                        ANSI_ST_TABLE3 *ts = (ANSI_ST_TABLE3 *) (p + nSeek);
                        PRINTOUT("                + METERING(%c) TEST(%c) UNPROG(%c)\xd\xa", 
                                    ts->METERING_FLAG ? 'T' : 'F', 
                                    ts->TEST_MODE_FLAG ? 'T' : 'F', 
                                    ts->UNPROGRAMMED_FLAG ? 'T': 'F');
                        PRINTOUT("                + RAMF(%c) ROMF(%c) NONVMF(%c) CLOCKE(%c)\xd\xa", 
                                    ts->RAM_FAILURE_FLAG ? 'E' : 'N', 
                                    ts->ROM_FAILURE_FLAG ? 'E' : 'N', 
                                    ts->NONVOL_MEM_FAILURE_FLAG ? 'E' : 'N',
                                    ts->CLOCK_ERROR_FLAG ? 'E' : 'N');
                        PRINTOUT("                + LOWB(%c) LOWP(%c) DMDOV(%c)\xd\xa", 
                                    ts->LOW_BATTERY_FLAG ? 'E' : 'N', 
                                    ts->LOW_LOSS_POTENTIAL_FLAG ? 'T' : 'F', 
                                    ts->DEMAND_OVERLOAD_FLAG ? 'T' : 'F');
                        PRINTOUT("                + MTRE(%c) DCDET(%c) SYSE(%c)\xd\xa", 
                                    ts->METERING_ERROR ? 'E' : 'N', 
                                    ts->DC_DETECTED ? 'T' : 'F', 
                                    ts->SYSTEM_ERROR ? 'T' : 'F');
                        PRINTOUT("                + RECVKWH(%c) LEADKVARH(%c) LoPROG(%c)\xd\xa", 
                                    ts->RECEIVED_KWH ? 'T' : 'F', 
                                    ts->LEADING_KVARH ? 'T' : 'F', 
                                    ts->LOSS_OF_PROGRAM ? 'T' : 'F');
                    } break;
                    case 5: {    
                        ANSI_ST_TABLE5 *ts = (ANSI_ST_TABLE5 *) (p + nSeek);
                        strncpy(buffer, (char *)ts->METER_ID, sizeof(ts->METER_ID)); buffer[sizeof(ts->METER_ID)] = 0x00;
                        PRINTOUT("                + METERID %s\xd\xa", buffer);
                    } break;
                    case 10: 
                    case 11: {
                        ANSI_ST_TABLE11 *ts = (ANSI_ST_TABLE11 *) (p + nSeek);
                        PRINTOUT("                + PF_EXCLUDE(%c) RESET_EXCLUDE(%c) BLOCK_DEMAND(%c)\xd\xa", 
                                ts->PF_EXCLUDE_FLAG ? 'T' : 'F', ts->RESET_EXCLUDE_FLAG ? 'T' : 'F', ts->BLOCK_DEMAND_FLAG ? 'T' : 'F');
                        PRINTOUT("                + SLIDING_DEMAND(%c) THERMAL_DEMAND(%c) SET1(%c) SET2(%c)\xd\xa", 
                                ts->SLIDING_DEMAND_FLAG ? 'T' : 'F', ts->THERMAL_DEMAND_FLAG ? 'T' : 'F', 
                                ts->SET1_PRESENT_FLAG ? 'T' : 'F', ts->SET2_PRESENT_FLAG ? 'T' : 'F');
                        PRINTOUT("                + NBR_UOM(%d) NBR_DEMAND_CTRL(%d) DATA_CTRL_LEN(%d)\xd\xa", 
                                ts->NBR_UOM_ENTRIES, ts->NBR_DEMAND_CTRL_ENTRIES, ts->DATA_CTRL_LENGTH);
                        PRINTOUT("                + NBR_DATA_CTRL(%d) NBR_CONSTANTS(%d) CONST_SELECTOR(%d) NBR_SOURCES(%d)\xd\xa", 
                                ts->NBR_DATA_CTRL_ENTRIES, ts->NBR_CONSTANTS_ENTRIES, ts->CONSTANTS_SELECTOR, ts->NBR_SOURCES);
                        nbr_uom_entries = ts->NBR_UOM_ENTRIES;
                    } break;
                    case 12: {
                        ANSI_ST_TABLE12 *ts = (ANSI_ST_TABLE12 *) (p + nSeek);
                        j = MIN(62, nbr_uom_entries);
                        for(i=0,k=0;i<j;i++) {
                            if(arrChannelSelected[k] == i) {
                                PRINTOUT("                  %2d : ID(%02X) TB(%d) MUL(%d) Qn(%c%c%c%c) NF(%c) SEG(%d) HMN(%c) NFS(%c)\xd\xa", 
                                    i, ts[i].ID_CODE,
                                    ts[i].TIME_BASE, ts[i].MULTIPLIER,
                                    ts[i].Q1_ACCOUNTABILITY ? 'T' : 'F', ts[i].Q2_ACCOUNTABILITY ? 'T' : 'F',
                                    ts[i].Q3_ACCOUNTABILITY ? 'T' : 'F', ts[i].Q4_ACCOUNTABILITY ? 'T' : 'F',
                                    ts[i].NET_FLOW_ACCOUNTABILITY ? 'T' : 'F', ts[i].SEGMENTATION,
                                    ts[i].HARMONIC ? 'T' : 'F', ts[i].NFS ? 'T' : 'F');
                                arrChannelUnits[k] = ts[i].ID_CODE;
                                k++;
                            }
                        }
                    } break;
					case 21: {
						ANSI_ST_TABLE21 *ts = (ANSI_ST_TABLE21 *) (p + nSeek);
                       	PRINTOUT("                + NBR_SELF_READS %d NBR_SUMMATIONS %d NBR_DEMANDS %d\xd\xa",
                                ts->NBR_SELF_READS, ts->NBR_SUMMATIONS, ts->NBR_DEMANDS);
                       	PRINTOUT("                + NBR_COIN_VALUES %d NBR_OCCUR %d NBR_TIERS %d\xd\xa",
                                ts->NBR_COIN_VALUES, ts->NBR_OCCUR, ts->NBR_TIERS);
					} break;
					case 23: {
                        if(nMeterMode == 2) // TOU
                        {
						ANSI_ST_TABLE23_TOU *ts = (ANSI_ST_TABLE23_TOU *) (p + nSeek);
                        summation_ch1 = CondStreamToHostLong(nOrder, ts->TOT_DATA_BLOCK.SUMMATIONS, 6);
                        summation_ch2 = CondStreamToHostLong(nOrder, &ts->TOT_DATA_BLOCK.SUMMATIONS[6], 6);
                       	PRINTOUT("                + TOT_SUM_CH1 %lld (%.3Lf) TOT_SUM_CH2 %lld (%.3Lf)\xd\xa",
                       	    summation_ch1, (long double)summation_ch1 * displayConstant, 
                            summation_ch2, (long double)summation_ch2 * displayConstant);
                        }
                        else // Demand
                        {
						ANSI_ST_TABLE23_DEMAND *ts = (ANSI_ST_TABLE23_DEMAND *) (p + nSeek);
                        summation_ch1 = CondStreamToHostLong(nOrder, ts->TIER_DATA_BLOCK.SUMMATIONS, 6);
                        summation_ch2 = CondStreamToHostLong(nOrder, &ts->TIER_DATA_BLOCK.SUMMATIONS[6], 6);
                       	PRINTOUT("                + RTP_SUM_CH1 %lld (%.3Lf) RTP_SUM_CH2 %lld (%.3Lf)\xd\xa",
                       	    summation_ch1, (long double)summation_ch1 * displayConstant, 
                            summation_ch2, (long double)summation_ch2 * displayConstant);
                        }
					} break;
                    case 25: {    
                        if(nMeterMode == 2) // TOU
                        {
                        ANSI_ST_TABLE25_TOU *ts = (ANSI_ST_TABLE25_TOU *) (p + nSeek);
                        PRINTOUT("                + END_DATE_TIME %04d/%02d/%02d %02d:%02d\xd\xa", 
                            ts->INFO.END_DATE_TIME.YEAR + 2000, ts->INFO.END_DATE_TIME.MON, ts->INFO.END_DATE_TIME.DAY, 
                            ts->INFO.END_DATE_TIME.HOUR, ts->INFO.END_DATE_TIME.MIN);
                        PRINTOUT("                + SEASON %d\xd\xa", ts->INFO.SEASON);
                        summation_ch1 = CondStreamToHostLong(nOrder, ts->DATA.TOT_DATA_BLOCK.SUMMATIONS, 6);
                        summation_ch2 = CondStreamToHostLong(nOrder, &ts->DATA.TOT_DATA_BLOCK.SUMMATIONS[6], 6);
                       	PRINTOUT("                + TOT_SUM_CH1 %lld (%.3Lf) TOT_SUM_CH2 %lld (%.3Lf)\xd\xa",
                       	    summation_ch1, (long double)summation_ch1 * displayConstant, 
                            summation_ch2, (long double)summation_ch2 * displayConstant);
                        }
                        else // Demand
                        {
                        ANSI_ST_TABLE25_DEMAND *ts = (ANSI_ST_TABLE25_DEMAND *) (p + nSeek);
                        PRINTOUT("                + END_DATE_TIME %04d/%02d/%02d %02d:%02d\xd\xa", 
                            ts->INFO.END_DATE_TIME.YEAR + 2000, ts->INFO.END_DATE_TIME.MON, ts->INFO.END_DATE_TIME.DAY, 
                            ts->INFO.END_DATE_TIME.HOUR, ts->INFO.END_DATE_TIME.MIN);
                        PRINTOUT("                + SEASON %d\xd\xa", ts->INFO.SEASON);
                        summation_ch1 = CondStreamToHostLong(nOrder, ts->DATA.TIER_DATA_BLOCK.SUMMATIONS, 6);
                        summation_ch2 = CondStreamToHostLong(nOrder, &ts->DATA.TIER_DATA_BLOCK.SUMMATIONS[6], 6);
                       	PRINTOUT("                + RTP_SUM_CH1 %lld (%.3Lf) RTP_SUM_CH2 %lld (%.3Lf)\xd\xa",
                       	    summation_ch1, (long double)summation_ch1 * displayConstant, 
                            summation_ch2, (long double)summation_ch2 * displayConstant);
                        }
                    } break;
                    case 26: {    
                        if(nMeterMode == 2) // TOU
                        {
                        ANSI_ST_TABLE26_TOU *ts = (ANSI_ST_TABLE26_TOU *) (p + nSeek);
                        PRINTOUT("                + ORDER(%s) OVERFLOW(%c) LIST(%s) INH_OVERFLOW(%c)\xd\xa", 
                                ts->STATUS.ORDER_FLAG ? "DES" : "ASC",
                                ts->STATUS.OVERFLOW_FLAG ? 'T' : 'F',
                                ts->STATUS.LIST_TYPE_FLAG ? "LIST" : "FIFO",
                                ts->STATUS.INHIBIT_OVERFLOW_FLAG ? 'T' : 'F' );
                        PRINTOUT("                + NBR_VALID %d LAST_ELE %d LAST_SEQ %d NBR_UNREAD %d\xd\xa", 
                                ts->NBR_VALID_ENTRIES,
                                ts->LAST_ENTRY_ELEMENT,
                                ts->LAST_ENTRY_SEQ_NUMBER,
                                ts->NBR_UNREAD_ENTRIES);
 	                    len = BigToHostShort(pHeader->len) - 6;
                        for(i=0,j=0;i<len;i+=sizeof(ANSI_ST_TABLE25_TOU),j++) {
                            PRINTOUT("                + END_DATE_TIME %04d/%02d/%02d %02d:%02d SEASON %d\xd\xa", 
                                    ts->ENTRIES[j].INFO.END_DATE_TIME.YEAR + 2000, ts->ENTRIES[j].INFO.END_DATE_TIME.MON, 
                                    ts->ENTRIES[j].INFO.END_DATE_TIME.DAY, ts->ENTRIES[j].INFO.END_DATE_TIME.HOUR, 
                                    ts->ENTRIES[j].INFO.END_DATE_TIME.MIN, ts->ENTRIES[j].INFO.SEASON);
                            summation_ch1 = CondStreamToHostLong(nOrder, ts->ENTRIES[j].DATA.TOT_DATA_BLOCK.SUMMATIONS, 6);
                            summation_ch2 = CondStreamToHostLong(nOrder, &ts->ENTRIES[j].DATA.TOT_DATA_BLOCK.SUMMATIONS[6], 6);
                       	    PRINTOUT("                + TOT_SUM_CH1 %lld (%.3Lf) TOT_SUM_CH2 %lld (%.3Lf)\xd\xa",
                       		    summation_ch1, (long double)summation_ch1 * displayConstant, 
                                summation_ch2, (long double)summation_ch2 * displayConstant);
                        }
                        }
                        else
                        {
                        ANSI_ST_TABLE26_DEMAND *ts = (ANSI_ST_TABLE26_DEMAND *) (p + nSeek);
                        PRINTOUT("                + ORDER(%s) OVERFLOW(%c) LIST(%s) INH_OVERFLOW(%c)\xd\xa", 
                                ts->STATUS.ORDER_FLAG ? "DES" : "ASC",
                                ts->STATUS.OVERFLOW_FLAG ? 'T' : 'F',
                                ts->STATUS.LIST_TYPE_FLAG ? "LIST" : "FIFO",
                                ts->STATUS.INHIBIT_OVERFLOW_FLAG ? 'T' : 'F' );
                        PRINTOUT("                + NBR_VALID %d LAST_ELE %d LAST_SEQ %d NBR_UNREAD %d\xd\xa", 
                                ts->NBR_VALID_ENTRIES,
                                ts->LAST_ENTRY_ELEMENT,
                                ts->LAST_ENTRY_SEQ_NUMBER,
                                ts->NBR_UNREAD_ENTRIES);
 	                    len = BigToHostShort(pHeader->len) - 6;
                        for(i=0,j=0;i<len;i+=sizeof(ANSI_ST_TABLE25_DEMAND),j++) {
                            PRINTOUT("                + END_DATE_TIME %04d/%02d/%02d %02d:%02d SEASON %d\xd\xa", 
                                    ts->ENTRIES[j].INFO.END_DATE_TIME.YEAR + 2000, ts->ENTRIES[j].INFO.END_DATE_TIME.MON, 
                                    ts->ENTRIES[j].INFO.END_DATE_TIME.DAY, ts->ENTRIES[j].INFO.END_DATE_TIME.HOUR, 
                                    ts->ENTRIES[j].INFO.END_DATE_TIME.MIN, ts->ENTRIES[j].INFO.SEASON);
                            summation_ch1 = CondStreamToHostLong(nOrder, ts->ENTRIES[j].DATA.TIER_DATA_BLOCK.SUMMATIONS, 6);
                            summation_ch2 = CondStreamToHostLong(nOrder, &ts->ENTRIES[j].DATA.TIER_DATA_BLOCK.SUMMATIONS[6], 6);
                       	    PRINTOUT("                + RTP_SUM_CH1 %lld (%.3Lf) RTP_SUM_CH2 %lld (%.3Lf)\xd\xa",
                       		    summation_ch1, (long double)summation_ch1 * displayConstant, 
                                summation_ch2, (long double)summation_ch2 * displayConstant);
                        }
                        }
                    } break;
                    case 52: {    
                        ANSI_ST_TABLE52 *ts = (ANSI_ST_TABLE52 *) (p + nSeek);
                        PRINTOUT("                + %04d/%02d/%02d %02d:%02d:%02d\xd\xa", 
                            ts->YEAR + 2000, ts->MONTH, ts->DAY, ts->HOUR, ts->MINUTE, ts->SECOND);
                        PRINTOUT("                + DoW %d DST %d GMT %d TZF %d DSTF %d\xd\xa", 
                            ts->TIME_DATE_QUAL & 7,
                            (ts->TIME_DATE_QUAL & 0x08) >> 3,
                            (ts->TIME_DATE_QUAL & 0x10) >> 4,
                            (ts->TIME_DATE_QUAL & 0x20) >> 5,
                            (ts->TIME_DATE_QUAL & 0x40) >> 6);
                    } break;
                    case 55: {    
                        ANSI_ST_TABLE55 *ts = (ANSI_ST_TABLE55 *) (p + nSeek);
                        PRINTOUT("                + %04d/%02d/%02d %02d:%02d:%02d\xd\xa", 
                            ts->YEAR + 2000, ts->MONTH, ts->DAY, ts->HOUR, ts->MINUTE, ts->SECOND);
                        PRINTOUT("                + DoW %d DST %d GMT %d TZF %d DSTF %d\xd\xa", 
                            ts->TIME_DATE_QUAL & 7,
                            (ts->TIME_DATE_QUAL & 0x08) >> 3,
                            (ts->TIME_DATE_QUAL & 0x10) >> 4,
                            (ts->TIME_DATE_QUAL & 0x20) >> 5,
                            (ts->TIME_DATE_QUAL & 0x40) >> 6);
                    } break;
                    case 61: {
                        ANSI_ST_TABLE61 *ts = (ANSI_ST_TABLE61 *) (p + nSeek);
                        PRINTOUT("                + LP_MEMORY_LEN %d LP_FLAGS %04X LP_FORMATS %02X\xd\xa", 
                            ts->LP_MEMORY_LEN, ts->LP_FLAGS, ts->LP_FORMATS);
                        PRINTOUT("                + BLKS_SET1 %d BLK_INTS_SET1 %d\xd\xa", 
                            ts->NBR_BLKS_SET1, ts->NBR_BLK_INTS_SET1);
                        PRINTOUT("                + CHNS_SET1 %d MAX_INT_TIME_SET1 %d\xd\xa", 
                            ts->NBR_CHNS_SET1, ts->MAX_INT_TIME_SET1);
                        PRINTOUT("                + LP_DATA_SET1_LEN %d\xd\xa", 
                            (((ts->NBR_CHNS_SET1/2)+1)+(ts->NBR_CHNS_SET1*2))*ts->NBR_BLK_INTS_SET1+5+(ts->NBR_CHNS_SET1)*6);

                        //nbrBlksSet1 = ts->NBR_BLKS_SET1;
                        nbrBlkIntsSet1 = ts->NBR_BLK_INTS_SET1;
                        nbrChnsSet1 = ts->NBR_CHNS_SET1;
                        //maxIntTimeSet1 = ts->MAX_INT_TIME_SET1;
                        lpLen = (((ts->NBR_CHNS_SET1/2)+1)+(ts->NBR_CHNS_SET1*2))*ts->NBR_BLK_INTS_SET1+5+(ts->NBR_CHNS_SET1)*6;
                    } break;
                    case 62: {
                        LP_SEL_SET1 *pLpSelSet1;
                        WORD * pScalars = (WORD *)(p + nSeek + (nbrChnsSet1*sizeof(LP_SEL_SET1))+1); // skip LP_SEL_SET1 and INT_FMT_CDE1 
                        WORD * pDivisors = (WORD *)(p + nSeek + (nbrChnsSet1*sizeof(LP_SEL_SET1))+1 + (nbrChnsSet1*2));
                        for(i=0;i<nbrChnsSet1;i++) {
                            pLpSelSet1 = (LP_SEL_SET1 *)(p + nSeek + (sizeof(LP_SEL_SET1)*i));
                            scalars_set1[i] = CondToHostShort(nOrder, pScalars[i]);
                            divisors_set1[i] = CondToHostShort(nOrder, pDivisors[i]);
                            PRINTOUT("                + END_RDG_FLAG %c LP_SOURCE %2d END_BLK_RDG_SOURCE %2d\xd\xa", 
                                pLpSelSet1->END_RDG_FLAG ? 'T' : 'F', 
                                pLpSelSet1->LP_SOURCE_SELECT, pLpSelSet1->END_BLK_RDG_SOURCE_SELECT);
                            PRINTOUT("                + SCALARS %d %04X DIVISORS %d %04X\xd\xa", 
                                scalars_set1[i], pScalars[i], divisors_set1[i], pDivisors[i]);
                        }
                    } break;
                    case 63: {
                        ANSI_ST_TABLE63 *ts = (ANSI_ST_TABLE63 *) (p + nSeek);
                        PRINTOUT("                + ORDER(%s) OVERFLOW(%c) LIST(%s) INH_OVERFLOW(%c)\xd\xa", 
                                ts->STATUS.ORDER_FLAG ? "DES" : "ASC",
                                ts->STATUS.OVERFLOW_FLAG ? 'T' : 'F',
                                ts->STATUS.LIST_TYPE_FLAG ? "LIST" : "FIFO",
                                ts->STATUS.INHIBIT_OVERFLOW_FLAG ? 'T' : 'F' );
                        PRINTOUT("                + INT_ODR(%s) LP_ACTIVE(%c) TEST_MODE(%c)\xd\xa", 
                                ts->STATUS.INTERVAL_ORDER_FLAG ? "DES" : "ASC",
                                ts->STATUS.ACTIVE_MODE_FLAG ? 'T' : 'F',
                                ts->STATUS.TEST_MODE_FLAG ? 'T' : 'F' );
                        PRINTOUT("                + VLD_BLK %d LST_BLK_ELEMT %d LST_BLK_SEQ %d\xd\xa", 
                            ts->NBR_VALID_BLOCKS, ts->LAST_BLOCK_ELEMENT, ts->LAST_BLOCK_SEQ_NBR);
                        PRINTOUT("                + UNR_BLK %d VLD_INT %d\xd\xa", 
                            ts->NBR_UNREAD_BLOCKS, ts->NBR_VALID_INT);
                    } break;
                    case 64: {
                        WORD tlen = BigToHostShort(pHeader->len);
                        WORD tidx = 0;
                        WORD v;
                        long double dv;
                        int cellcnt = 4;

                        /** Issue #855 ~1960 한 Block의 instance 수를 극단적으로 줄여서 4보다 작게 했을 경우
                          */
                        if(cellcnt > nbrBlkIntsSet1)
                        {
                            cellcnt = nbrBlkIntsSet1;
                        }

                        for(i=0;i<tlen;i+=lpLen) {
                            ANSI_END_TIME *et = (ANSI_END_TIME *) (p + nSeek + tidx);
                            tidx += sizeof(ANSI_END_TIME);

                            PRINTOUT("                + BLK_END_TIME %04d/%02d/%02d %02d:%02d", 
                                et->YEAR + 2000, et->MON, et->DAY, et->HOUR, et->MIN);

                            /** I211의 경우 END_READINGS(READING_RCD type) 의 조건이 다음과 같다.
                              *
                              * ACT_LP_TBL (S0061 Actual Load Profile Table)
                              *
                              * if(ACT_LP_TBL.BLK_END_READ_FLAG) => true
                              *     BLOCK_END_READ : NI_FMAT1;
                              *
                              * if(ACT_LP_TBL.BLK_END_PULSE_FLAG) => false
                              *     BLOCK_END_PULSE : UINT32;
                              *
                              * NI_FMAT1은 S00 Table에서 10(INT48) 로 정의되어 있다 (6byte)
                              */
                            /** ANSI_END_READINGS.BLOCK_END_READ는 표기 안한다. 추후 표기 */
                            for(j=0;j<nbrChnsSet1;j++)
                            {
                                ANSI_END_READINGS *er = (ANSI_END_READINGS *) (p + nSeek + tidx);
                                unsigned long long lastv = CondStreamToHostLong(nOrder, er->BLOCK_END_READ, 6); 

                                tidx += sizeof(ANSI_END_READINGS);
                                PRINTOUT(" %lld (%.3Lf)", lastv, (long double)lastv * displayConstant);
                            }
                            PRINTOUT("\xd\xa");

                            /** LP */
                            for(j=0;j<nbrBlkIntsSet1;j++)
                            {
                                if(!(j%cellcnt)) PRINTOUT("                  "); 
                                tidx += (WORD)(nbrChnsSet1/2) + 1;      /** EXTENDED_INT_STATUS */
                                for(k=0;k<nbrChnsSet1;k++)
                                {
                                    /** Alignment 문제로 memcpy로 변경 
                                     * v = *((WORD *)(p + nSeek + tidx)); tidx += 2;
                                     */
                                    memcpy(&v, p + nSeek + tidx, 2); tidx += 2;
                                    if(v == 0xFFFF)
                                    {
                                        dv = -1.0;
                                    }else{
#if 0
                                        /** WORD형 value * Scalar Constant * 10e-9 * display * (divisor[ch] / scalar[ch]) * 60 / interval */
                                        dv = (long double)CondToHostShort(nOrder, v) * (long double)vAhSf * pow(10, -7 /* -9 + 2 */) *
                                            ((long double)divisors_set1[k]/(long double)scalars_set1[k])*60/(long double)maxIntTimeSet1;
                                        /** display factor pow(10,2) 에 대한 내용을 찾을 수 없다.
                                          * 60/maxIntTimeSet1 에 대한 내용을 찾을 수 없다.
                                          * 정확한 의미를 알 수 있을 때 까지 기존식을 사용한다 
                                          */
#else
                                        /** WORD형 value * Scalar Constant * 10e-9 * (divisor[ch] / scalar[ch]) */
                                        dv = (long double)CondToHostShort(nOrder, v) * displayConstant *
                                            ((long double)divisors_set1[k]/(long double)scalars_set1[k]);
#endif
                                    }
                                    if(arrChannelUnits[k] < 8)
                                    {
                                        PRINTOUT(" %9.4Lf", dv); // Power
                                    }
                                    else
                                    {
                                        PRINTOUT(" %04X", v);   // Voltage, Current
                                    }
                                }
                                if(!((j+1)%cellcnt)) PRINTOUT("\r\n");
                            }
                        }
                        
                        /** cell count로 instance의 수가 나누어 떨어지지 않을 꼉우 개행을 추가 한다 */
                        if(nbrBlkIntsSet1 % cellcnt)
                        {
                            PRINTOUT("\r\n");
                        }
                    } break;
                    case 76: {
                        ANSI_ST_TABLE76 *ts = (ANSI_ST_TABLE76 *) (p + nSeek);
                        int tlen = (int)((BigToHostShort(pHeader->len) - sizeof(ANSI_ST_TABLE76)) / sizeof(ANSI_EVENT_ENTRIES));

                        PRINTOUT("                + ORDER(%s) OVERFLOW(%c) LIST(%s) INH_OVERFLOW(%c)\xd\xa", 
                                ts->ORDER_FLAG ? "DES" : "ASC",
                                ts->OVERFLOW_FLAG ? 'T' : 'F',
                                ts->LIST_TYPE_FLAG ? "LIST" : "FIFO",
                                ts->INHIBIT_OVERFLOW_FLAG ? 'T' : 'F' );

                        PRINTOUT("                + VALID(%d) LAST(%d) LAST_SEQ(%d) UNREAD(%d)\xd\xa", 
                                ts->NBR_VALID_ENTRIES, 
                                ts->LAST_ENTRY_ELEMENT,
                                ts->LAST_ENTRY_SEQ_NBR,
                                ts->NBR_UNREAD_ENTRIES);
                        for(i=0;i<tlen;i++) {
                            PRINTOUT("                  %4d : %04d/%02d/%02d %02d:%02d:%02d %04X %c%03d %d\xd\xa", 
                                    i+1, ts->EVENT_ENTRIES[i].EVENT_TIME.YEAR + 2000, ts->EVENT_ENTRIES[i].EVENT_TIME.MON,
                                    ts->EVENT_ENTRIES[i].EVENT_TIME.DAY, ts->EVENT_ENTRIES[i].EVENT_TIME.HOUR,
                                    ts->EVENT_ENTRIES[i].EVENT_TIME.MIN, ts->EVENT_ENTRIES[i].EVENT_TIME.SEC,
                                    ts->EVENT_ENTRIES[i].USERID,
                                    ts->EVENT_ENTRIES[i].STD_VS_MFG_FLAG ? 'M' : 'S', ts->EVENT_ENTRIES[i].TBL_PROC_NBR, 
                                    ts->EVENT_ENTRIES[i].EVENT_ARGUMENT);
                        }
                    } break;
                    case 112: {
                        ANSI_ST_TABLE112 *ts = (ANSI_ST_TABLE112 *) (p + nSeek);
                        len = MIN(BigToHostShort(pHeader->len),sizeof(ANSI_ST_TABLE112)) / sizeof(ANSI_LOAD_CTRL_STATUS);
                        // Load Control Status 만큼 반복한다.
                        for(i=0; i<len; i++)
                        {
                            PRINTOUT("                + %s\xd\xa", ts->STATUS_ENTRIES[i].NAME);
                            PRINTOUT("                  REQ %d OUT %d SENSED %d CNTDOWN %02d:%02d:%02d\xd\xa", 
                                    ts->STATUS_ENTRIES[i].REQUESTED_LEVEL,
                                    ts->STATUS_ENTRIES[i].OUTPUT_LEVEL,
                                    ts->STATUS_ENTRIES[i].SENSED_LEVEL,
                                    ts->STATUS_ENTRIES[i].HOURS,
                                    ts->STATUS_ENTRIES[i].MINUTES,
                                    ts->STATUS_ENTRIES[i].SECONDS);
                        }
                    } break;
                    case 130: {
                        ANSI_ST_TABLE130 *ts = (ANSI_ST_TABLE130 *) (p + nSeek);
                        PRINTOUT("                + RELAY(%s) ACTIVE(%s)\xd\xa", 
                                ts->RUN_STATUS ? "ON" : "OFF",
                                ts->ACTIVE_STATUS ? "ON" : "OFF");
                    } break;
                    case 132: {
                        ANSI_ST_TABLE132 *ts = (ANSI_ST_TABLE132 *) (p + nSeek);
                        PRINTOUT("                + NBR_SWITCH_ON(%d) NBR_SWITCH_OFF(%d)\xd\xa", 
                                ts->NBR_SWITCH_ON, ts->NBR_SWITCH_OFF);
                        j = MIN(10, ts->NBR_SWITCH_ON);
                        if(j) PRINTOUT("                + SWITCH ON LOG\xd\xa"); 
                        for(i=0;i<j;i++) {
                            PRINTOUT("                  %2d : %04d/%02d/%02d %02d:%02d:%02d %04X (%s)\xd\xa", 
                                    i+1, ts->SWITCH_ON[i].YEAR + 2000, ts->SWITCH_ON[i].MONTH,
                                    ts->SWITCH_ON[i].DAY, ts->SWITCH_ON[i].HOUR,
                                    ts->SWITCH_ON[i].MIN, ts->SWITCH_ON[i].SEC,
                                    ts->SWITCH_ON[i].USERID,
                                    ts->SWITCH_ON[i].OPER_TYPE ? "CMD" : "USER");
                        }
                        j = MIN(10, ts->NBR_SWITCH_OFF);
                        if(j) PRINTOUT("                + SWITCH OFF LOG\xd\xa"); 
                        for(i=0;i<j;i++) {
                            PRINTOUT("                  %2d : %04d/%02d/%02d %02d:%02d:%02d %04X\xd\xa", 
                                    i+1, ts->SWITCH_OFF[i].YEAR + 2000, ts->SWITCH_OFF[i].MONTH,
                                    ts->SWITCH_OFF[i].DAY, ts->SWITCH_OFF[i].HOUR,
                                    ts->SWITCH_OFF[i].MIN, ts->SWITCH_OFF[i].SEC,
                                    ts->SWITCH_OFF[i].USERID);
                        }
                    } break;
                }
                break;
            case 'M':
                switch(tnum) {
					case 0 : {
						ANSI_MT_TABLE0 *ts = (ANSI_MT_TABLE0 *) (p + nSeek);
				  		PRINTOUT("                + MFG %d.%d FW %d.%d b%d TYPE %d MODE %s\xd\xa",
                                ts->MFG_VERSION_NBR, ts->MFG_REVISION_NBR,
                                ts->ROM_CONST_DATA_RCD.FW_VERSION, ts->ROM_CONST_DATA_RCD.FW_REVISION,
                                ts->ROM_CONST_DATA_RCD.FW_BUILD, ts->METER_TYPE,
                                MSGAPI_GeMeterMode(ts->METER_MODE));

					} break;
#if 0
					case 16 : {
						ANSI_MT_TABLE16 *ts = (ANSI_MT_TABLE16 *) (p + nSeek);
				  		PRINTOUT("                + %c %s%s%sMULTIPLIER %d%sMSGTYPE %02X TOT %d kWh\xd\xa",
										ts->START_CHR,
                                        ts->RESET_TAMPER ? "RESET_TAMPER " : "",
                                        ts->METERING_PROBLEM ? "MTR_ERR " : "",
                                        ts->REVERSE_ENERGY_FLOW ? "REVERSE " : "",
                                        ts->DISPLAY_MULTIPLIER ? 10 : 1,
                                        ts->DISPLAY_DIGITS ? "DIGITS 4 " : " ",
                                        ts->SPI_MESSAGE_TYPE,
										ts->TOTAL_KWH);
					} break;
#endif
					case 67 : {
						ANSI_MT_TABLE67 *ts = (ANSI_MT_TABLE67 *) (p + nSeek);
				  		PRINTOUT("                + CUR %d POT %d\xd\xa",
										ts->CUR_TRANS_RATIO,
										ts->POT_TRANS_RATIO);
					} break;
                    case 68: {    
                        ANSI_MT_TABLE68 *ts = (ANSI_MT_TABLE68 *) (p + nSeek);
                        PRINTOUT("                + EF MTR(%c) WCD(%c) RAM(%c) EOM(%c) EEPROM(%c) BAT(%c)\xd\xa", 
                                    ts->ERROR_FREEZE.METERING_ERROR ? 'T' : 'F', 
                                    ts->ERROR_FREEZE.WATCHDOG_TIMEOUT ? 'T' : 'F', 
                                    ts->ERROR_FREEZE.RAM ? 'T' : 'F',
                                    ts->ERROR_FREEZE.ROM ? 'T' : 'F', 
                                    ts->ERROR_FREEZE.EEPROM ? 'T' : 'F', 
                                    ts->ERROR_FREEZE.BATT_POT_FAIL ? 'T' : 'F');

                        PRINTOUT("                + CE LB(%c) LP(%c) DTO(%c) RKW(%c) LKV(%c) UP(%c) LoP(%c) TC(%c) BP(%c) DC(%c)\xd\xa", 
                                    ts->CAUTION_ENABLE.LOW_BATTERY ? 'T' : 'F', 
                                    ts->CAUTION_ENABLE.LOW_POTENTIAL ? 'T' : 'F', 
                                    ts->CAUTION_ENABLE.DMD_THRESH_OVERLOAD ? 'T' : 'F',
                                    ts->CAUTION_ENABLE.RECEIVED_KWH ? 'T' : 'F', 
                                    ts->CAUTION_ENABLE.LEADING_KVAH ? 'T' : 'F', 
                                    ts->CAUTION_ENABLE.UNPROGRAMMED ? 'T' : 'F',
                                    ts->CAUTION_ENABLE.LOSS_OF_PROGRAM ? 'T' : 'F',
                                    ts->CAUTION_ENABLE.TIME_CHANGED ? 'T' : 'F', 
                                    ts->CAUTION_ENABLE.BAD_PASSWORD ? 'T' : 'F', 
                                    ts->CAUTION_ENABLE.DC_DETECTED ? 'T' : 'F');

                        PRINTOUT("                + CD LB(%c) LP(%c) DTO(%c) RKW(%c) LKV(%c) UP(%c) LoP(%c) TC(%c) BP(%c) DC(%c)\xd\xa", 
                                    ts->CAUTION_DISPLAY.LOW_BATTERY ? 'T' : 'F', 
                                    ts->CAUTION_DISPLAY.LOW_POTENTIAL ? 'T' : 'F', 
                                    ts->CAUTION_DISPLAY.DMD_THRESH_OVERLOAD ? 'T' : 'F',
                                    ts->CAUTION_DISPLAY.RECEIVED_KWH ? 'T' : 'F', 
                                    ts->CAUTION_DISPLAY.LEADING_KVAH ? 'T' : 'F', 
                                    ts->CAUTION_DISPLAY.UNPROGRAMMED ? 'T' : 'F',
                                    ts->CAUTION_DISPLAY.LOSS_OF_PROGRAM ? 'T' : 'F',
                                    ts->CAUTION_DISPLAY.TIME_CHANGED ? 'T' : 'F', 
                                    ts->CAUTION_DISPLAY.BAD_PASSWORD ? 'T' : 'F', 
                                    ts->CAUTION_DISPLAY.DC_DETECTED ? 'T' : 'F');

                        PRINTOUT("                + CF LB(%c) LP(%c) DTO(%c) RKW(%c) LKV(%c) UP(%c) LoP(%c) TC(%c) BP(%c) DC(%c)\xd\xa", 
                                    ts->CAUTION_FREEZE.LOW_BATTERY ? 'T' : 'F', 
                                    ts->CAUTION_FREEZE.LOW_POTENTIAL ? 'T' : 'F', 
                                    ts->CAUTION_FREEZE.DMD_THRESH_OVERLOAD ? 'T' : 'F',
                                    ts->CAUTION_FREEZE.RECEIVED_KWH ? 'T' : 'F', 
                                    ts->CAUTION_FREEZE.LEADING_KVAH ? 'T' : 'F', 
                                    ts->CAUTION_FREEZE.UNPROGRAMMED ? 'T' : 'F',
                                    ts->CAUTION_FREEZE.LOSS_OF_PROGRAM ? 'T' : 'F',
                                    ts->CAUTION_FREEZE.TIME_CHANGED ? 'T' : 'F', 
                                    ts->CAUTION_FREEZE.BAD_PASSWORD ? 'T' : 'F', 
                                    ts->CAUTION_FREEZE.DC_DETECTED ? 'T' : 'F');

                        PRINTOUT("                + DOTH %04X PVTOL %d SED(%c) SMIN %d SMAX %d\xd\xa",
                                    ts->DMD_OVERLOAD_THRESH,
                                    ts->PHA_VOLT_TOLERANCE,
                                    ts->SERVICE_ERROR_DETECT_ENABLE ? 'T' : 'F', 
                                    ts->SERVICE_MIN_VOLTAGE, ts->SERVICE_MAX_VOLTAGE); 
                    } break;
					case 75 : {
                        PRINTOUT("                + VSQRHRSF %d VRMSSF %d ISQRHRSF %d VASF %d VAHSF %d\xd\xa",
                                    vSqrHrSf, vRmsSf, iSqrHrSf, vASf, vAhSf);
					} break;
					case 78 : {
						ANSI_MT_TABLE78 *ts = (ANSI_MT_TABLE78 *) (p + nSeek);
						PRINTOUT("                + SECS %d NBR %d LAST_TIME %04d/%02d/%02d %02d:%02d\xd\xa",
									   	ts->CUM_POWER_OUTAGE_SECS,
										ts->NBR_POWER_OUTAGES,
										ts->DT_LAST_POWER_OUTAGE[0] + 2000, ts->DT_LAST_POWER_OUTAGE[1],
										ts->DT_LAST_POWER_OUTAGE[2], ts->DT_LAST_POWER_OUTAGE[3], ts->DT_LAST_POWER_OUTAGE[4]);
					} break;
					case 113 : {
						ANSI_MT_TABLE113 *ts = (ANSI_MT_TABLE113 *) (p + nSeek);
						PRINTOUT("                + RMS_VOL_PHA %d RMS_VOL_PHC %d MOMENTARY_INTERVAL_PF %d\xd\xa",
									   	ts->RMS_VOLTAGE_PHA,
										ts->RMS_VOLTAGE_PHC,
										ts->MOMENTARY_INTERVAL_PF);
 	                    if(BigToHostShort(pHeader->len)>5)
                        {
						    PRINTOUT("                + TEMPERATURE %d\xd\xa", ts->TEMPERATURE);
                        }
					} break;
					case 115 : {
						ANSI_MT_TABLE115 *ts = (ANSI_MT_TABLE115 *) (p + nSeek);
						PRINTOUT("                + LC_STATUS_FLAG %s%s%s%s%s%s%s%s\xd\xa",
                                        ts->COMM_ERROR ? "COMM_ERR " : "",
                                        ts->SWITCH_CONTROLLER_ERROR ? "CONTROLLER_ERR " : "",
                                        ts->SWITCH_FAILED_TO_CLOSE ? "FAILED_TO_CLOSE " : "",
                                        ts->ALTERNATE_SOURCE ? "ALTERNATE_SOURCE " : "",
                                        ts->BYPASSED ? "BYPASSED " : "",
                                        ts->SWITCH_FAILED_TO_OPEN ? "FAILED_TO_OPEN " : "",
                                        ts->PPM_ALERT ? "PPM_ALERT " : "",
                                        ts->MANUAL_ARM_TIMED_OUT ? "ARM_TIMEOUT " : "");
						PRINTOUT("                + LC_STATUS %s%s%s%s%s%s\xd\xa",
                                        ts->ECP_STATE ? "ECP " : "",
                                        ts->DLP_STATE ? "DLP " : "",
                                        ts->PPM_STATE ? "PPM " : "",
                                        ts->ECP_DISCONNECT ? "ECP_DISCON " : "",
                                        ts->DLP_DISCONNECT ? "DLP_DISCON " : "",
                                        ts->PPM_DISCONNECT ? "PPM_DISCON " : "");
						PRINTOUT("                + LC_RECONNECT %d\xd\xa", ts->LC_RECONNECT_ATTEMPT_COUNT);
						PRINTOUT("                + LC_STATUS_HISTORY %04X\xd\xa", ts->LC_STATUS_HISTORY);
						PRINTOUT("                + RCDC_STATE %s%s%s%s\xd\xa",
                                        ts->OPEN_HOLD_FOR_COMMAND ? "OPEN_HOLD " : "",
                                        ts->WAITING_TO_ARM ? "WAITING_TO_ARM " : "",
                                        ts->ARMED_FOR_CLOSURE ? "ARMED_FOR_CLOSURE " : "",
                                        ts->OUTAGE_OPEN_IN_EFFECT ? "OUTAGE_OPEN " : "");
						PRINTOUT("                  ACTUAL %s DISRED %s LOCKOUT %s\xd\xa",
                                        ts->ACTUAL_SWITCH_STATE ? "Closed" : "Open",
                                        ts->DESIRED_SWITCH_STATE ? "Closed" : "Open",
                                        ts->LOCKOUT_IN_EFFECT ? "Frozen" : "Not frozen");
					} break;
                }
                break;
            case 'T':
                switch(tnum) {
                    case 1: {    
                        STAT_TABLE *ts = (STAT_TABLE *) (p + nSeek);
                        PRINTOUT("                + TR %d SEND %d RECV %d ERR %d\xd\xa", 
                                    ts->TRANSMIT_PKT_COUNT,
                                    ts->SEND_PKT_COUNT,
                                    ts->RECV_PKT_COUNT,
                                    ts->ERROR_PKT_COUNT);
                        PRINTOUT("                + SEND %d RECV %d\xd\xa", 
                                    ts->TOTAL_SEND_BYTES,
                                    ts->TOTAL_RECV_BYTES);
                        PRINTOUT("                + TOTAL %d CONN %d WAIT %d MIN %d MAX %d\xd\xa", 
                                    ts->TOTAL_USE_MSEC,
                                    ts->SENSOR_CONNECT_MSEC,
                                    ts->TOTAL_REPLY_WAIT_MSEC,
                                    ts->MIN_REPLY_MSEC,
                                    ts->MAX_REPLY_MSEC);
                    } break;
                }
                break;
            }
        }
 	    nSeek += BigToHostShort(pHeader->len);
 	    tsize += BigToHostShort(pHeader->len);
        tcnt ++; 
		p += nSeek;
	}

    PRINTOUT("   TABLE TOTAL : %d (%d)\xd\xa", tcnt, tsize);
}

#ifdef __USE_STDOUT__
void aidon_data_parsing(char *mdata, UINT length, BOOL bDetail)
#else
void aidon_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail)
#endif
{
    AIDON_METERING_DATA aidonHeader;
    AIDON_LPENTRY aidonLpEntry;
    char    *bp,data[256];
    int     lpDateCnt=0;
    int     lpLen=0, lpReadSize=0, state, len, j;
    UINT     idx = 0;

    memset(&aidonHeader, 0, sizeof(aidonHeader));

    state = STATE_SD_READ_LP_HEADER;
    while(state != STATE_SD_DONE) {
        switch(state) {
            case STATE_SD_READ_LP_HEADER:
                if(idx+sizeof(AIDON_METERING_DATA) > length) {
                    PRINTOUT("ERROR -- Read LPHEADER Fail\r\n");
                    state = STATE_SD_DONE;
                    break;
                }
                memcpy(&aidonHeader, mdata+idx, sizeof(AIDON_METERING_DATA));
                idx += sizeof(AIDON_METERING_DATA);
                
                state = STATE_SD_LP_ENTRY;
                switch(aidonHeader.lpPeriod) {
                    case 0: case 1: case 2: case 4:
                        lpLen = aidonHeader.lpPeriod * 48;
                        break;
                    default:
                        PRINTOUT("ERROR -- Invalid LPPERIOD=%d\r\n", aidonHeader.lpPeriod);
                        state = STATE_SD_DONE;
                }
                if(state == STATE_SD_DONE) break;
                lpLen += sizeof(TIMEDATE)+sizeof(UINT);
                lpDateCnt = aidonHeader.dateCount;
                lpReadSize = 0;
	            PRINTOUT(" CURRENT PULSE : %d\xd\xa", aidonHeader.curPulse);
	            PRINTOUT("     LP PERIOD : %d\xd\xa", aidonHeader.lpPeriod);
	            PRINTOUT("METER CONSTANT : %d\xd\xa", aidonHeader.meterConstant);
	            PRINTOUT("   LP DATE CNT : %d\xd\xa", lpDateCnt);
            break;
            case STATE_SD_LP_ENTRY:
                if(lpDateCnt <= 0) {
                    state = STATE_SD_OBIS;
                    break;
                }
                lpDateCnt --;
                if(idx+lpLen <= length) {
                    memcpy(&aidonLpEntry, mdata+idx, lpLen);
	                PRINTOUT("       LP DATE : %04d/%02d/%02d\xd\xa", 
                            BigToHostShort(aidonLpEntry.LPDATE.year),
                            aidonLpEntry.LPDATE.mon,
                            aidonLpEntry.LPDATE.day);
	                PRINTOUT("    BASE PULSE : %04d\xd\xa", 
                            BigToHostInt(aidonLpEntry.BASE_PULSE));
                    for(j=0;j<(aidonHeader.lpPeriod * 48);j+=2) {
                        if(!(j % 24)) PRINTOUT("  ");
                        PRINTOUT(" %02X%02X", aidonLpEntry.LP_DATA[j], aidonLpEntry.LP_DATA[j+1]); 
                        if(!((j+2) % 24)) PRINTOUT("\xd\xa");
                    }
                    idx += lpLen;
                    lpReadSize += lpLen;
                } else {
                    PRINTOUT("ERROR -- Read LPENTRY Fail\r\n");
                    state = STATE_SD_DONE;
                }
            break;
            case STATE_SD_OBIS:
                len = length - lpReadSize - sizeof(AIDON_METERING_DATA) - sizeof(TIMESTAMP);
                if(len > 0) {
                    if(idx+len <= length) {
                        if(bDetail) {
	                        PRINTOUT("     OBIS DATA :\xd\xa");
                            bp = mdata+idx;
                            while(sscanf(bp,"%s", data)>0) {
	                            PRINTOUT("                 %s\xd\xa", data);
                                bp += strlen(data)+2;
                            }
                        }
                        idx += len;
                    } else {
                        PRINTOUT("ERROR -- Read OBIS Fail\r\n");
                    }
                }
                state = STATE_SD_DONE;
            break;
        }
    }
}

#ifdef __USE_STDOUT__
void kamstrup_data_parsing(BYTE nVendorType, char *mdata, UINT length, BOOL hasLp)
#else
void kamstrup_data_parsing(CLISESSION *pSession, BYTE nVendorType, char *mdata, UINT length, BOOL hasLp)
#endif
{
    AIDON_METERING_DATA aidonHeader;
    AIDON_LPENTRY aidonLpEntry;
    KAM_METERING_INFO kam2Header;
    KAM_METERING_IDX_COUNT kam2IdxCount;
    KAM_METERING_OMNI_IDX_COUNT omniIdxCount;
    KMP_FRAME * kmpFrame;
    UINT    idx = 0;
    int     lpLen=0, lpReadSize=0, state;
    int     lpInsCount=0;
    WORD    regId, logId;
    UINT    logId4;
    UINT    logIdStart;
    UINT    logCnt;
    BYTE    kmpLen, payloadLen, j, k, mask;
    BYTE    logType, noReg;
    WORD    mask2, logInfo2;
    char    kv[512];
    BYTE    valueBuffer[255];
    int     formatBufferSize=255;
    BYTE    *formatBuffer[formatBufferSize];
    BYTE    *copyFormatBuffer;
    char    *pKv;
    char    buff0[32];
    char    buff1[32];
    char    *regStr;
    double  dValue;
    BYTE    bValue, bCutOffStatus = 0;
    WORD24  kmpLpFormat[4];
    WORD24  kmpLpTimeFormat;
    BYTE    kmpChannelMask=0;

    memset(&aidonHeader, 0, sizeof(aidonHeader));

    if(hasLp)
    {
        state = STATE_SD_READ_LP_HEADER;
    }
    else
    {
        state = STATE_SD_OBIS;
    }
    while(state != STATE_SD_DONE) {
        switch(state) {
            case STATE_SD_READ_LP_HEADER:
                switch(nVendorType)
                {
                    case METER_VENDOR_KAMSTRUP_2: // Kamstrup 신규 검침 Format
                        if(idx+sizeof(KAM_METERING_INFO) > length) {
                            PRINTOUT("ERROR -- Read LPHEADER Fail\r\n");
                            state = STATE_SD_DONE;
                            break;
                        }
                        memcpy(&kam2Header, mdata+idx, sizeof(KAM_METERING_INFO));
                        idx += sizeof(KAM_METERING_INFO);
                
                        switch(kam2Header.period) {
                            case 0: case 1: case 2: case 4: case 12:
                                break;
                            default:
                                PRINTOUT("ERROR -- Invalid LPPERIOD=%d\r\n", kam2Header.period);
                                state = STATE_SD_DONE;
                                break;
                        }
                        if(state == STATE_SD_DONE) break;

	                    PRINTOUT("     LP PERIOD : %d\xd\xa", kam2Header.period);

                        /** LP Time Fromat를 이용해서 LP Instance의 길이를 구한다 */
                        lpLen += kam2Header.timeFormat.v[1];
                        memcpy(&kmpLpTimeFormat, &kam2Header.timeFormat, sizeof(WORD24));

                        /** Channel Mask를 이용해서 LP Instance의 길이를 구한다 */
                        kmpChannelMask = kam2Header.channelMask;
                        memset(kmpLpFormat, 0, sizeof(kmpLpFormat));
                        for(j=0;j<4;j++)
                        {
                            if(kam2Header.channelMask & (0x01<<j))
                            {
                                memcpy(&kmpLpFormat[j], mdata+idx, sizeof(WORD24)); 
                                lpLen += kmpLpFormat[j].v[1];
	                            PRINTOUT("    CHANNEL[%d] : %s %s\xd\xa", j+1, 
                                        kmpGetRegisterString(j+1), 
                                        kmpGetKamstrupUnit(kmpLpFormat[j].v[2]));
                                idx += sizeof(WORD24);
                            }
                        }

                        memcpy(&kam2IdxCount, mdata+idx, sizeof(KAM_METERING_IDX_COUNT));
                        idx += sizeof(KAM_METERING_IDX_COUNT);
                
                        lpInsCount = BigToHostShort(kam2IdxCount.lpInsCount);
                        lpReadSize = 0;
	                    PRINTOUT("   LP LAST IDX : %d\xd\xa", BigToHostShort(kam2IdxCount.lastLpIndex));
	                    PRINTOUT("    LP INS CNT : %d\xd\xa", lpInsCount);
                        state = STATE_SD_LP_ENTRY;
                        break;
                    case METER_VENDOR_KAMSTRUP_3: // Kamstrup Omnipower 신규 검침 Format
                        if(idx+sizeof(KAM_METERING_INFO) > length) {
                            PRINTOUT("ERROR -- Read LPHEADER Fail\r\n");
                            state = STATE_SD_DONE;
                            break;
                        }
                        memcpy(&kam2Header, mdata+idx, sizeof(KAM_METERING_INFO));
                        idx += sizeof(KAM_METERING_INFO);
                
                        switch(kam2Header.period) {
                            case 0: case 1: case 2: case 4: case 12:
                                break;
                            default:
                                PRINTOUT("ERROR -- Invalid LPPERIOD=%d\r\n", kam2Header.period);
                                state = STATE_SD_DONE;
                                break;
                        }
                        if(state == STATE_SD_DONE) break;

	                    PRINTOUT("     LP PERIOD : %d\xd\xa", kam2Header.period);

                        /** LP Time Fromat를 이용해서 LP Instance의 길이를 구한다 */
                        lpLen += kam2Header.timeFormat.v[1];
                        memcpy(&kmpLpTimeFormat, &kam2Header.timeFormat, sizeof(WORD24));

                        /** Channel Mask를 이용해서 LP Instance의 길이를 구한다 */
                        kmpChannelMask = kam2Header.channelMask;
                        memset(kmpLpFormat, 0, sizeof(kmpLpFormat));
                        for(j=0;j<4;j++)
                        {
                            if(kam2Header.channelMask & (0x01<<j))
                            {
                                memcpy(&kmpLpFormat[j], mdata+idx, sizeof(WORD24)); 
                                lpLen += kmpLpFormat[j].v[1];
	                            PRINTOUT("    CHANNEL[%d] : %s %s\xd\xa", j+1, 
                                        kmpGetRegisterString(j+1), 
                                        kmpGetKamstrupUnit(kmpLpFormat[j].v[2]));
                                idx += sizeof(WORD24);
                            }
                        }

                        memcpy(&omniIdxCount, mdata+idx, sizeof(KAM_METERING_OMNI_IDX_COUNT));
                        idx += sizeof(KAM_METERING_OMNI_IDX_COUNT);
                
                        lpInsCount = BigToHostShort(omniIdxCount.lpInsCount);
                        lpReadSize = 0;
	                    PRINTOUT("   LP LAST IDX : %u\xd\xa", BigToHostInt(omniIdxCount.lastLpIndex));
	                    PRINTOUT("    LP INS CNT : %d\xd\xa", lpInsCount);
                        state = STATE_SD_LP_ENTRY;
                        break;
                    default: // 기존 Kamstrup Format
                        if(idx+sizeof(AIDON_METERING_DATA) > length) {
                            PRINTOUT("ERROR -- Read LPHEADER Fail\r\n");
                            state = STATE_SD_DONE;
                            break;
                        }
                        memcpy(&aidonHeader, mdata+idx, sizeof(AIDON_METERING_DATA));
                        idx += sizeof(AIDON_METERING_DATA);
                
                        state = STATE_SD_LP_ENTRY;
                        switch(aidonHeader.lpPeriod) {
                            case 0: case 1: case 2: case 4:
                                lpLen = aidonHeader.lpPeriod * 48;
                                break;
                            default:
                                PRINTOUT("ERROR -- Invalid LPPERIOD=%d\r\n", aidonHeader.lpPeriod);
                                state = STATE_SD_DONE;
                                break;
                        }
                        if(state == STATE_SD_DONE) break;
                        lpLen += sizeof(TIMEDATE)+sizeof(UINT);
                        lpInsCount = aidonHeader.dateCount;
                        lpReadSize = 0;
	                    PRINTOUT(" CURRENT PULSE : %d\xd\xa", aidonHeader.curPulse);
	                    PRINTOUT("     LP PERIOD : %d\xd\xa", aidonHeader.lpPeriod);
	                    PRINTOUT("METER CONSTANT : %d\xd\xa", aidonHeader.meterConstant);
	                    PRINTOUT("   LP DATE CNT : %d\xd\xa", lpInsCount);
                        break;
                }
                break;
            case STATE_SD_LP_ENTRY:
                if(lpInsCount <= 0) {
                    state = STATE_SD_OBIS;
	                PRINTOUT("      KMP DATA : \xd\xa");
                    break;
                }
                lpInsCount --;
                if(idx+lpLen <= length) {
                    switch(nVendorType)
                    {
                        case METER_VENDOR_KAMSTRUP_2:
                        case METER_VENDOR_KAMSTRUP_3:
                            k = 0;
                            memcpy(kv, &kmpLpTimeFormat, sizeof(WORD24));
                            memcpy(kv+sizeof(WORD24), mdata+idx+k, kmpLpTimeFormat.v[1]);
                            k += kmpGetRegValue((BYTE *)kv, NULL, (char *)valueBuffer);
	                        PRINTOUT("       %s  ", valueBuffer); 
                            for(j=0;j<4;j++)
                            {
                                if(kmpChannelMask & (0x01<<j))
                                {
                                    memcpy(kv, &kmpLpFormat[j], sizeof(WORD24));
                                    memcpy(kv+sizeof(WORD24), mdata+idx+k, kmpLpFormat[j].v[1]);
                                    k += kmpGetRegValue((BYTE *)kv, NULL, (char *)valueBuffer);
	                                PRINTOUT(" %12s", valueBuffer); 
                                }
                            }
                            PRINTOUT("\r\n");
                            break;
                        default:
                            memcpy(&aidonLpEntry, mdata+idx, lpLen);
	                        PRINTOUT("       LP DATE : %04d/%02d/%02d\xd\xa", 
                                    BigToHostShort(aidonLpEntry.LPDATE.year),
                                    aidonLpEntry.LPDATE.mon,
                                    aidonLpEntry.LPDATE.day);
	                        PRINTOUT("    BASE PULSE : %d\xd\xa", 
                                    BigToHostInt(aidonLpEntry.BASE_PULSE));
                            for(j=0;j<(aidonHeader.lpPeriod * 48);j+=2) {
                                if(!(j % 24)) PRINTOUT("  ");
                                PRINTOUT(" %02X%02X", aidonLpEntry.LP_DATA[j], aidonLpEntry.LP_DATA[j+1]); 
                                if(!((j+2) % 24)) PRINTOUT("\xd\xa");
                            }
                            break;
                    }
                    idx += lpLen;
                    lpReadSize += lpLen;
                } else {
                    PRINTOUT("ERROR -- Read LPENTRY Fail\r\n");
                    state = STATE_SD_DONE;
                }
            break;
            case STATE_SD_OBIS:
                if(idx+1 < length) {
                    kmpLen = mdata[idx];
                    payloadLen = kmpLen - 6;    // SOF, ADDR, CID, CRC(2), EOF
                    kmpFrame = (KMP_FRAME *) (mdata + idx +1);

                    switch(kmpFrame->hdr.cid)
                    {
                        case KMP_CID_GET_TYPE:
                            if(payloadLen > 3)
                            {
                                memset(buff0, 0, sizeof(buff0)); memset(buff1, 0, sizeof(buff1));
	                            PRINTOUT("                 + Meter Type %s %s%s\xd\xa", 
                                    kmpGetMeterType(kmpFrame->payload[0], kmpFrame->payload[1]),
                                    kmpSwMajorVersion(kmpFrame->payload[2], (char *)buff0),
                                    kmpSwMinorVersion(kmpFrame->payload[3], (char *)buff1));
                            }
                            break;
                        case KMP_CID_GET_REGISTER:
                            // kmpLen 에서 tail(crc+stop) + regformat(3) 을 뺀 것 보다 많이 남아 있어야 한다
                            for(j=4; j < kmpLen - 6; j+=5) 
                            {
                                memcpy(&regId, mdata + idx + j, sizeof(WORD));
                                regId = BigToHostShort(regId);
                                j += kmpGetRegValue((BYTE *)&mdata[idx+j+2], NULL, (char *)kv);
                                regStr = const_cast<char *>(kmpGetRegisterString(regId));
                                if(regStr)
                                {
	                                PRINTOUT("                 + %s %s\xd\xa", regStr, kv);
                                }
                                else
                                {
	                                PRINTOUT("                 + %04X %s\xd\xa", regId, kv);
                                }
                            }
                            break;
                        case KMP_CID_GET_POWER_LIMIT_STATUS:
                            if(payloadLen > 1)
                            {
	                            PRINTOUT("                 + Current limiter %s\xd\xa", 
                                        kmpFrame->payload[0] & 0x01 ? "on" : "off");
	                            PRINTOUT("                 + PA/module supply %s\xd\xa", 
                                        kmpFrame->payload[0] & 0x02 ? "on" : "off");
	                            PRINTOUT("                 + State nonumber %d\xd\xa", kmpFrame->payload[1]);
                            }
                            if(payloadLen > 3)
                            {
                                memcpy(&regId, &kmpFrame->payload[2], sizeof(WORD));
                                regId = BigToHostShort(regId);
	                            PRINTOUT("                 + State timer %d\xd\xa", regId);
                            }
                            break;
                        case KMP_CID_READ_EVENT_STATUS:
                            for(j=0;j<payloadLen;j++)
                            {
                                mask = 0x01;
                                for(k=0;k<8;k++, mask <<= 1)
                                {
                                    if((regStr=const_cast<char *>(kmpGetEventStatus(j, kmpFrame->payload[j],  mask))) != NULL)
                                    {
	                                    PRINTOUT("                 + %s\xd\xa", regStr);
                                    }
                                }
                            }
                            break;
                        case KMP_CID_SET_CUT_OFF_STATE:
                        case KMP_CID_GET_CUT_OFF_STATE:
                            if(payloadLen > 1)
                            {
	                            PRINTOUT("                 + CutOff State %s\xd\xa", 
                                            kmpGetCutOffState(kmpFrame->payload[0]));
	                            PRINTOUT("                 + CutOff Feedback %s\xd\xa", 
                                            kmpGetCutOffFeedback(kmpFrame->payload[0], kmpFrame->payload[1], (BYTE *)kv));
                            }
                            break;
                        case KMP_CID_GET_LOG_TIME_PRESENT:
                        case KMP_CID_GET_LOG_LAST_PRESENT:
                        case KMP_CID_GET_LOG_ID_PRESENT:
                        case KMP_CID_GET_LOG_TIME_PAST:
                            logType = kmpFrame->payload[0];
                            noReg = kmpFrame->payload[1];

	                        PRINTOUT("                 + %s\xd\xa", kmpGetLogType(logType));

                            mask = 0x01;
                            for(k=0;k<8;k++, mask <<= 1)
                            {
                                if((regStr=const_cast<char *>(kmpGetLogInfo(kmpFrame->payload[payloadLen-1],  mask))) != NULL)
                                {
	                                PRINTOUT("                  * %s\xd\xa", regStr);
                                }
                            }

                            memset(formatBuffer, 0, sizeof(formatBuffer));
                            for(j=2;j+5<payloadLen;)   // LatestReadLogId, NewLogId, info 제외
                            {
                                memcpy(&logId, &kmpFrame->payload[j], sizeof(WORD));
                                logId = BigToHostShort(logId); j += 2;

	                            PRINTOUT("                 + LogId %d\xd\xa", logId);
                                for(k=0; k < noReg; k++) 
                                {
                                    if(formatBuffer[k] == NULL)
                                    {
                                        formatBuffer[k] = &kmpFrame->payload[j];
                                        j+=5;
                                    }
                                    memcpy(valueBuffer, formatBuffer[k], 5);        // regid(2) + format(3)
                                    memcpy(valueBuffer+5, &kmpFrame->payload[j], valueBuffer[3]);   // format.length만큳 copy
                                    memcpy(&regId, valueBuffer, sizeof(WORD));
                                    regId = BigToHostShort(regId); dValue = 0.0;
                                    j += kmpGetRegValue((BYTE *)&valueBuffer[2], &dValue, (char *)kv);
                                    regStr = const_cast<char *>(kmpGetRegisterString(regId));

                                    switch(regId)
                                    {
                                        case 0x0032:    // Meter status
                                            mask = 0x01;
                                            bValue = (BYTE) dValue;
                                            for(k=0;k<8;k++, mask <<= 1)
                                            {
                                                if((regStr=const_cast<char *>(kmpGetEventStatus(0, bValue,  mask))) != NULL)
                                                {
	                                                PRINTOUT("                  * %s\xd\xa", regStr);
                                                }
                                            }
                                            break;
                                        case 0x0309:    // RTC status
                                            mask = 0x01;
                                            bValue = (BYTE) dValue;
                                            for(k=0;k<8;k++, mask <<= 1)
                                            {
                                                if((regStr=const_cast<char *>(kmpGetRTCStatus(bValue,  mask))) != NULL)
                                                {
	                                                PRINTOUT("                  * %s\xd\xa", regStr);
                                                }
                                            }
                                            break;
                                        case 0x043D:    // Voltage event
                                            mask = 0x01;
                                            bValue = (BYTE) dValue;
                                            for(k=0;k<8;k++, mask <<= 1)
                                            {
                                                if((regStr=const_cast<char *>(kmpGetVoltageQuality(bValue,  mask))) != NULL)
                                                {
	                                                PRINTOUT("                  * %s\xd\xa", regStr);
                                                }
                                            }
                                            break;
                                        case 0x043F:    // Cutoff status
                                            bCutOffStatus = bValue = (BYTE) dValue;
	                                        PRINTOUT("                  * %s\xd\xa", kmpGetCutOffState(bValue));
                                            break;
                                        case 0x0440:    // Cutoff feedback
                                            bValue = (BYTE) dValue;
	                                        pKv =  const_cast<char *>(kmpGetCutOffFeedback(bCutOffStatus, bValue, (BYTE *)kv));
                                            if(pKv && strlen(pKv)>0)
                                            {
	                                            PRINTOUT("                  * %s\xd\xa", pKv);
                                            }
                                            break;
                                        default:
                                            if(regStr)
                                            {
	                                            PRINTOUT("                  * %s %s\xd\xa", regStr, kv);
                                            }
                                            else
                                            {
	                                            PRINTOUT("                  * %04X %s\xd\xa", regId, kv);
                                            }
                                            break;
                                    }
                                }
                            }

                            if(j<=payloadLen-5)
                            {
                                memcpy(&logId, &kmpFrame->payload[j], sizeof(WORD));
                                logId = BigToHostShort(logId); j += 2;
                                memcpy(&regId, &kmpFrame->payload[j], sizeof(WORD));
                                regId = BigToHostShort(regId);
	                            PRINTOUT("                 + LatestReadLogId %d NewLogId %d\xd\xa", logId, regId);

                                m_nKmpReadIdx = logId;
                                m_nKmpLastIdx = regId;
                            }
                            break;
                        case KMP_CID_LOGGER_READ_OUT:       // Omnipower meter
                            {
                            bool bCompact = false;

#if 0
                                        PRINTOUT("================================\r\n");
			                            DUMP((char *)&kmpFrame->payload[0], payloadLen, FALSE);
                                        PRINTOUT("--------------------------------\r\n");
#endif

                            logType = kmpFrame->payload[1];
                            noReg = kmpFrame->payload[2];

	                        PRINTOUT("                 + %s\xd\xa", kmpGetLogType(logType));

                            if(payloadLen < (3+4)) break;   // 뒤에 더이상 데이터가 없을 때

                            mask2 = 0x0001;
                            memcpy(&logInfo2, &kmpFrame->payload[payloadLen-2], sizeof(WORD));
                            logInfo2 = BigToHostShort(logInfo2);
                            for(k=0;k<16;k++, mask2 <<= 1)
                            {
                                if((regStr=const_cast<char *>(kmpGetLogInfo(logInfo2,  mask2))) != NULL)
                                {
	                                PRINTOUT("                  * %s\xd\xa", regStr);
                                }
                            }

                            memcpy(&logIdStart, &kmpFrame->payload[3], sizeof(UINT));
                            logIdStart = BigToHostInt(logIdStart); 
	                        PRINTOUT("                 + LogId %u\xd\xa", logIdStart);

                            logCnt = 0;
                            memset(formatBuffer, 0, sizeof(formatBuffer));
                            for(j=7;noReg > 0 && j+6<payloadLen;logCnt++)   // NewLogId(4), info(2) 제외
                            {
                                BYTE tempFormatBuffer[5];

                                int compactPosition = j;
                                
                                if(bCompact)
                                {
                                    j+= (noReg-1)/2 + 1;
	                                PRINTOUT("                 + New Row\xd\xa");
                                }

                                for(k=0; k < noReg; k++) 
                                {
                                    if(formatBuffer[k] == NULL)
                                    {
                                        formatBuffer[k] = &kmpFrame->payload[j];
                                        j+=5;
                                    }
                                    if(bCompact) // Compact format
                                    {
                                        BYTE cFormat = kmpFrame->payload[compactPosition + (k/2)];
                                        if(!(k%2))
                                        {
                                            cFormat = cFormat >> 4;
                                        }

                                        BYTE sign = (cFormat & 0x08) ? 0x80 : 0x00;
                                        BYTE len = cFormat & 0x07;

                                        if(len >= 7)
                                        {
                                            // format buffer copy
                                            copyFormatBuffer = formatBuffer[k];
                                        }
                                        else 
                                        {
                                            memcpy(tempFormatBuffer, formatBuffer[k], sizeof(tempFormatBuffer));
                                            tempFormatBuffer[3] = len;
                                            tempFormatBuffer[4] = sign | (tempFormatBuffer[4]&0x7F);
                                            copyFormatBuffer = tempFormatBuffer;
                                        }
                                    }
                                    else
                                    {
                                            copyFormatBuffer = formatBuffer[k];
                                    }
                                    memcpy(valueBuffer, copyFormatBuffer, 5);        // regid(2) + format(3)
                                    memcpy(valueBuffer+5, &kmpFrame->payload[j], valueBuffer[3]);   // format.length만큳 copy
                                    memcpy(&regId, valueBuffer, sizeof(WORD));
                                    regId = BigToHostShort(regId); dValue = 0.0;
                                    j += kmpGetRegValue((BYTE *)&valueBuffer[2], &dValue, (char *)kv, bCompact);
                                    regStr = const_cast<char *>(kmpGetRegisterString(regId));

                                    switch(regId)
                                    {
                                        case 0x0032:    // Meter status
                                            mask = 0x01;
                                            bValue = (BYTE) dValue;
                                            for(k=0;k<8;k++, mask <<= 1)
                                            {
                                                if((regStr=const_cast<char *>(kmpGetEventStatus(0, bValue,  mask))) != NULL)
                                                {
	                                                PRINTOUT("                  * %s\xd\xa", regStr);
                                                }
                                            }
                                            break;
                                        case 0x0309:    // RTC status
                                            mask = 0x01;
                                            bValue = (BYTE) dValue;
                                            for(k=0;k<8;k++, mask <<= 1)
                                            {
                                                if((regStr=const_cast<char *>(kmpGetRTCStatus(bValue,  mask))) != NULL)
                                                {
	                                                PRINTOUT("                  * %s\xd\xa", regStr);
                                                }
                                            }
                                            break;
                                        case 0x043D:    // Voltage event
                                            mask = 0x01;
                                            bValue = (BYTE) dValue;
                                            for(k=0;k<8;k++, mask <<= 1)
                                            {
                                                if((regStr=const_cast<char *>(kmpGetVoltageQuality(bValue,  mask))) != NULL)
                                                {
	                                                PRINTOUT("                  * %s\xd\xa", regStr);
                                                }
                                            }
                                            break;
                                        case 0x043F:    // Cutoff status
                                            bCutOffStatus = bValue = (BYTE) dValue;
	                                        PRINTOUT("                  * %s\xd\xa", kmpGetCutOffState(bValue));
                                            break;
                                        case 0x0440:    // Cutoff feedback
                                            bValue = (BYTE) dValue;
	                                        pKv =  const_cast<char *>(kmpGetCutOffFeedback(bCutOffStatus, bValue, (BYTE *)kv));
                                            if(pKv && strlen(pKv)>0)
                                            {
	                                            PRINTOUT("                  * %s\xd\xa", pKv);
                                            }
                                            break;
                                        default:
                                            if(strlen(kv) > 0)
                                            {
                                                if(regStr)
                                                {
	                                                PRINTOUT("                  * %s %s\xd\xa", regStr, kv);
                                                }
                                                else
                                                {
	                                                PRINTOUT("                  * %04X %s\xd\xa", regId, kv);
                                                }
                                            }
                                            break;
                                    }
                                }
                                if(!bCompact)
                                {
                                    memcpy(&regId, &kmpFrame->payload[j], sizeof(WORD));
                                    regId = BigToHostShort(regId); 
                                    j+= 2;
                                    for(k=0; k < noReg; k++) 
                                    {
                                        if(formatBuffer[k] != NULL && 
                                                formatBuffer[k][0] == (BYTE)((regId>>8)&0xFF) &&
                                                formatBuffer[k][1] == (BYTE)(regId&0xFF))
                                        {
                                            memcpy(valueBuffer, formatBuffer[k], 5);        // regid(2) + format(3)
                                            memcpy(valueBuffer+5, &kmpFrame->payload[j], valueBuffer[3]);   // format.length만큳 copy
                                            dValue = 0.0;
                                            j += kmpGetRegValue((BYTE *)&valueBuffer[2], &dValue, (char *)kv, true);
                                            regStr = const_cast<char *>(kmpGetRegisterString(regId));
                                            if(regStr)
                                            {
	                                            PRINTOUT("                  * Default Interval %s %s\xd\xa", regStr, kv);
                                            }
                                            else
                                            {
	                                            PRINTOUT("                  * Default Interval %04X %s\xd\xa", regId, kv);
                                            }
                                            break;
                                        }
                                    }
                                }
                                bCompact = true;
                            }

                            if(j<=payloadLen-6)
                            {
                                memcpy(&logId4, &kmpFrame->payload[j], sizeof(UINT));
                                logId4 = BigToHostInt(logId4); j += 4;
	                            PRINTOUT("                 + NewLogId %u\xd\xa", logId4);

                                m_nKmpReadIdx = logIdStart + logCnt;
                                m_nKmpLastIdx = logId4;
                            }
                            }
                            break;
                        case KMP_CID_GET_LOAD_PROFILE_CONFIG:
                            if(payloadLen > 2)
                            {
                                if(kmpFrame->payload[0] & 0x01) PRINTOUT("                 + A14 logged\xd\xa"); 
                                if(kmpFrame->payload[0] & 0x02) PRINTOUT("                 + A23 logged\xd\xa"); 
                                if(kmpFrame->payload[0] & 0x04) PRINTOUT("                 + R12 logged\xd\xa"); 
                                if(kmpFrame->payload[0] & 0x08) PRINTOUT("                 + R34 logged\xd\xa"); 
                                switch(kmpFrame->payload[1])
                                {
                                    case 5: case 15: case 30: case 60:
                                        PRINTOUT("                 + log interval %d min\xd\xa", kmpFrame->payload[1]); 
                                        break;
                                    default:
                                        PRINTOUT("                 + invalid log interval %d\xd\xa", kmpFrame->payload[1]); 
                                        break;
                                }
                                PRINTOUT("                 + Logger %s\xd\xa", kmpFrame->payload[2] & 0x80 ? "on" : "off");
                                switch(kmpFrame->payload[2] & 0x03)
                                {
                                    case 0: 
                                        PRINTOUT("                 + small logger, 10kB\xd\xa"); 
                                        break;
                                    case 3: 
                                        PRINTOUT("                 + large logger, 105kB\xd\xa"); 
                                        break;
                                    default:
                                        PRINTOUT("                 + unknown logger size\xd\xa"); 
                                        break;
                                }
                            }
                            break;
                        case KMP_CID_GET_VOLT_QUAL_CONFIG:
                            if(payloadLen > 4)
                            {
                                memcpy(&logId, &kmpFrame->payload[0], sizeof(WORD));
                                logId = BigToHostShort(logId);
                                memcpy(&regId, &kmpFrame->payload[2], sizeof(WORD));
                                regId = BigToHostShort(regId);
                                PRINTOUT("                 + Voltage Quality %d V ~ %d V  %d sec\xd\xa",
                                        logId, regId, kmpFrame->payload[4] * 10); 
                            }
                            break;
                        default:
			                DUMP((char *)&mdata[idx+1], (int)kmpLen, FALSE);
                            break;
                    }

                    idx += kmpLen + 1;
                    break;
                } else state = STATE_SD_DONE;
            break;
        }
    }
}

#ifdef __USE_STDOUT__
void sx_data_parsing(char *mdata, UINT length, BOOL bDetail)
#else
void sx_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail)
#endif
{
    POLLBASED_MULT_CH_METERING_INFO mtrInfo;
    POLLBASED_MULT_CH_INFO chInfo;
    TIMEDATE    *timeDate;

    UINT    idx = 0;
    int     lpLen=0, lpReadSize=0, state;
    int     lpDateCnt=0;
    int     i, j, value;
    int     y=0,m=0,d=0;
    UINT    basePulse;
    BYTE    sxLen, sxId;
    char    strBuffer[64];

    memset(&mtrInfo, 0, sizeof(mtrInfo));

    state = STATE_SD_READ_LP_HEADER;
    while(state != STATE_SD_DONE) {
        switch(state) {
            case STATE_SD_READ_LP_HEADER:
                if(idx+sizeof(POLLBASED_MULT_CH_METERING_INFO) > length) {
                    PRINTOUT("ERROR -- Read LPHEADER Fail\r\n");
                    state = STATE_SD_DONE;
                    break;
                }
                memcpy(&mtrInfo, mdata+idx, sizeof(POLLBASED_MULT_CH_METERING_INFO));
                idx += sizeof(POLLBASED_MULT_CH_METERING_INFO);
                
                state = STATE_SD_CH_INFO;
                switch(mtrInfo.lpPeriod) {
                    case 0: case 1: case 2: case 4:
                        lpLen = mtrInfo.lpPeriod * 48 * 6;
                        break;
                    default:
                        PRINTOUT("ERROR -- Invalid LPPERIOD=%d\r\n", mtrInfo.lpPeriod);
                        state = STATE_SD_DONE;
                }
                if(state == STATE_SD_DONE) break;
                lpLen += sizeof(TIMEDATE)+(sizeof(UINT) * 6);
                lpDateCnt = mtrInfo.dateCount;
                lpReadSize = 0;
	            //PRINTOUT(" CURRENT PULSE : %d\xd\xa", mtrInfo.curPulse);
	            PRINTOUT("     LP PERIOD : %d\xd\xa", mtrInfo.lpPeriod);
	            //PRINTOUT("METER CONSTANT : %d\xd\xa", mtrInfo.meterConstant);
	            PRINTOUT("   LP DATE CNT : %d\xd\xa", lpDateCnt);
                break;
            case STATE_SD_CH_INFO:
	            PRINTOUT("       CHANNEL :\xd\xa");
                for(i=0; i<6; i++)
                {
                    memcpy(&chInfo, mdata+idx, sizeof(POLLBASED_MULT_CH_INFO));
	                PRINTOUT("                 + %02X %d %u\xd\xa", 
                            chInfo.channelType, chInfo.meterConstant, BigToHostInt(chInfo.curPulse));
                    idx += sizeof(POLLBASED_MULT_CH_INFO);
                }
                state = STATE_SD_LP_ENTRY;
                break;
            case STATE_SD_LP_ENTRY:
                if(lpDateCnt <= 0) {
                    state = STATE_SD_MD;
	                PRINTOUT("       SX DATA : \xd\xa");
                    break;
                }
                lpDateCnt --;
                if(idx+lpLen <= length) {
                    timeDate = (TIMEDATE *)(mdata+idx); idx += sizeof(TIMEDATE);
                    //memcpy(&lpEntry, mdata+idx, lpLen);
	                PRINTOUT("       LP DATE : %04d/%02d/%02d\xd\xa", 
                            BigToHostShort(timeDate->year), timeDate->mon, timeDate->day);

	                PRINTOUT("    BASE PULSE : ");
                    for(i=0; i<6; i++)
                    {
                        memcpy(&basePulse, mdata+idx, sizeof(UINT)); idx += sizeof(UINT);
	                    PRINTOUT(" %d", BigToHostInt(basePulse));
                    }
	                PRINTOUT("\r\n");
                    for(j=0;j<(mtrInfo.lpPeriod * 48 * 6);j+=(2*6)) {
                        if(!(j % 24)) PRINTOUT("  ");
                        PRINTOUT(" %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X  ", 
                                mdata[idx], mdata[idx+1], mdata[idx+2], mdata[idx+3], mdata[idx+4], mdata[idx+5],
                                mdata[idx+6], mdata[idx+7], mdata[idx+8], mdata[idx+9], mdata[idx+10], mdata[idx+11]);
                        idx += (2 * 6);
                        if(!((j+(2*6)) % 24)) PRINTOUT("\xd\xa");
                    }
                    lpReadSize += lpLen;
                } else {
                    PRINTOUT("ERROR -- Read LPENTRY Fail\r\n");
                    state = STATE_SD_DONE;
                }
            break;
            case STATE_SD_MD:
                if(idx+3 < length) {
                    memset(strBuffer, 0, sizeof(strBuffer));
                    sxLen = mdata[idx];
                    sxId = mdata[idx+1];
                    memcpy(strBuffer, mdata + idx + 2, sxLen - 1);
                    value = (int)strtol(strBuffer, (char **)NULL, 10);

                    switch(sxId)
                    {
                        case 0x00: // Meter identification number
	                        PRINTOUT("                 + SERIAL %s\xd\xa",  strBuffer);
                            break;
                        case 0x02: // Meter FW Issue Date
                            sscanf(strBuffer,"%02d%02d%02d", &d, &m, &y);
	                        PRINTOUT("                 + FW ISSUE %02d/%02d/%02d\xd\xa",  y, m, d);
                            break;
                        case 0xF5: // Meter FW Future Date
                            sscanf(strBuffer,"%02d%02d%02d", &d, &m, &y);
	                        PRINTOUT("                 + FW FUTURE %02d/%02d/%02d\xd\xa",  y, m, d);
                            break;
                        case 0x34: // Meter Time
                        {
                            cetime_t meterTimeT=0;
                            ConvHexaTimeStr2TimeT(strBuffer, &meterTimeT);
                            MakeTimeString(strBuffer, &meterTimeT, TRUE);
	                        PRINTOUT("                 + TIME %s\xd\xa",  strBuffer);
                        }
                            break;
                        case 0x35: // Battery Status
	                        PRINTOUT("                 + BATTERY %s\xd\xa", value ? "Normal" : "Low Battery");
                            break;
                        case 0xD0: // Voltage
	                        PRINTOUT("                 + VOLTAGE %d\xd\xa", value);
                            break;
                        case 0xD1: // Neutral Current
	                        PRINTOUT("                 + NEUTRAL CURRENT %d\xd\xa", value);
                            break;
                        case 0xD2: // Line Current
	                        PRINTOUT("                 + LINE CURRENT %d\xd\xa", value);
                            break;
                        case 0xD5: // Forward Energy
	                        PRINTOUT("                 + FORWARD ENERGY %d\xd\xa", value);
                            break;
                        case 0xD6: // Reverse Energy
	                        PRINTOUT("                 + REVERSE ENERGY %d\xd\xa", value);
                            break;
                        case 0xD7: // Display value
	                        PRINTOUT("                 + DISPLAY VALUE %u\xd\xa", value);
                            break;
                        case 0xE7: // Tamper Status
	                        PRINTOUT("                 + TAMPER STATUS %s\xd\xa", strBuffer);
                            break;
                        case 0xEC: // Power Factor
	                        PRINTOUT("                 + POWER FACTOR %s\xd\xa", strBuffer);
                            break;
                        case 0xF4:  // Relay Status
	                        PRINTOUT("                 + RELAY %s\xd\xa", value ? "Connected" : "Disconnected");
                            break;
                        default:
	                        PRINTOUT("                 + %02X %s\xd\xa",  sxId, strBuffer);
                            break;
                    }
                    idx += sxLen + 1;
                    break;
                } else state = STATE_SD_DONE;
            break;
        }
    }
}

#ifdef __USE_STDOUT__
void pulse_data_parsing(char * sensorType, char *mdata, UINT length, BOOL bDetail, EXTRA_DATA_TYPE etype)
#else
void pulse_data_parsing(CLISESSION *pSession, char * sensorType, char *mdata, UINT length, BOOL bDetail, EXTRA_DATA_TYPE etype)
#endif
{
    ENDI_METERING_INFO meteringInfo;
    int     lpLen=0, state;
    UINT    idx = 0;
    UINT    infoLen = sizeof(ENDI_METERING_INFO);
    int     lqi=0, j;
    char    amp[32]={0,};
    UINT    basepulse;
    TIMEDATE lpDate;

    memset(&meteringInfo, 0, infoLen);

    state = STATE_SD_READ_INFO;
    while(state != STATE_SD_DONE) {
        switch(state) {
            case STATE_SD_READ_INFO:
                if(idx+infoLen > length) {
                    PRINTOUT("ERROR -- Read METERINGINFO Fail\r\n");
                    state = STATE_SD_DONE;
                    break;
                }
                memcpy(&meteringInfo, mdata+idx, infoLen);
                idx += infoLen;
                
                switch(meteringInfo.lpPeriod) {
                    case 0: case 1: case 2: case 4:
                        lpLen = meteringInfo.lpPeriod * 48 + 8;
                        break;
                    default:
                        PRINTOUT("ERROR -- Invalid LPPERIOD=%d\r\n", meteringInfo.lpPeriod);
                        state = STATE_SD_DONE;
                }
                if(state == STATE_SD_DONE) break;

                if((int) meteringInfo.LQI < 80 ) {
                    lqi = (int) meteringInfo.LQI;
                }else {
                    lqi = (int) (80 + (meteringInfo.LQI - 80) * 0.125);
                }

                if(lqi>100) lqi = 100;

                /** Issue #1867 : 전류 표현 방식 변경 */
                if((meteringInfo.fwVersion == 0x22 && meteringInfo.fwBuild >= 0x20) || 
                        meteringInfo.fwVersion > 0x22) {
                    sprintf(amp,"%d", BigToHostShort(meteringInfo.curConsum));
                } else {
                    sprintf(amp,"%.04f", (double)BigToHostShort(meteringInfo.curConsum) / (double)10000);
                }

                PRINTOUT("   INFORMATION : %04d/%02d/%02d %02d:%02d:%02d, OPRDAY=%d, ACTMIN=%d\r\n"
                                  "                 TYPE=%s(%s), LQI=%d, RSSI=%d, ALARM=%d, LEVEL=%d\r\n"
                                  "                 BAT=%.04f, AMP=%s, OFF=%.04f\r\n"
                                  "                 LP [%04d/%02d/%02d] PERIOD=%d, CHOICE=%d, BP=%d, CP=%d\r\n"
                                  "                 FW VER=%.1f, FW BLD=0x%02X, HW VER=%.1f, SW VER=%.1f\r\n",
                    BigToHostShort(meteringInfo.curTime.year),
                    meteringInfo.curTime.mon ,
                    meteringInfo.curTime.day ,
                    meteringInfo.curTime.hour ,
                    meteringInfo.curTime.min ,
                    meteringInfo.curTime.sec ,
                    BigToHostShort(meteringInfo.oprDay),
                    BigToHostShort(meteringInfo.actMin),
                    sensorType ? sensorType : "PULSE-BATTERY",
                    MSGAPI_NetworkType(meteringInfo.networkType),
                    lqi,
                    meteringInfo.RSSI,
                    infoLen > 42 ? meteringInfo.alarmFlag : -1,     // Issue #1864 : 42bytes 보다 작을 경우 alarmFlag가 지원되지 않는다
                    meteringInfo.energyLevel,
                    (double)BigToHostShort(meteringInfo.batteryVolt) / (double)10000,
                    amp,                                            // Issue #1867 : 전류 표현 방식 변경
                    (double)meteringInfo.offset / (double)10000,
                    BigToHostShort(meteringInfo.lpDate.year),
                    meteringInfo.lpDate.mon,
                    meteringInfo.lpDate.day,
                    meteringInfo.lpPeriod,
                    meteringInfo.lpChoice,
                    BigToHostInt(meteringInfo.basePulse),
                    BigToHostInt(meteringInfo.curPulse),
                    (meteringInfo.fwVersion >> 4) + (meteringInfo.fwVersion & 0x0f) * 0.1,
                    meteringInfo.fwBuild,
                    (meteringInfo.hwVersion >> 4) + (meteringInfo.hwVersion & 0x0f) * 0.1,
                    (meteringInfo.swVersion >> 4) + (meteringInfo.swVersion & 0x0f) * 0.1);
                state = STATE_SD_LP_ENTRY;
            break;
            case STATE_SD_LP_ENTRY:
                if(idx+lpLen <= length) {
                    memcpy(&lpDate, mdata+idx, sizeof(TIMEDATE)); idx += sizeof(TIMEDATE);
                    memcpy(&basepulse, mdata+idx, sizeof(UINT)); idx += sizeof(UINT);
	                PRINTOUT("        LP LEN : LP LEN=%d, LP [%04d/%02d/%02d] BP=%d\xd\xa", 
                            lpLen - 8,
                            BigToHostShort(lpDate.year),
                            lpDate.mon,
                            lpDate.day,
                            BigToHostInt(basepulse));
                    for(j=0;j<(meteringInfo.lpPeriod * 48);j+=2) {
                        if(!(j % 24)) PRINTOUT("  ");
                        PRINTOUT(" %02X%02X", mdata[idx + j], mdata[idx + j + 1]); 
                        if(!((j+2) % 24)) PRINTOUT("\xd\xa");
                    }
                    idx += lpLen - sizeof(TIMEDATE) - sizeof(UINT);
                    state = STATE_SD_LP_ENTRY;
                } else {
                    state = STATE_SD_EXTRA;
                }
            break;
            case STATE_SD_EXTRA:
                switch(etype)
                {
#ifdef __SUPPORT_KUKDONG_METER__
                    case EXTRA_KEUKDONG: // 극동 미터
                        char    buffer[256];
                        int     year, month, day;
                        OADI_KD_STATUS_PAYLOAD *pStatusPayload;
                        OADI_KD_VERSION_PAYLOAD *pVersionPayload;

                        if(idx+sizeof(OADI_KD_STATUS_PAYLOAD)+sizeof(OADI_KD_VERSION_PAYLOAD) <= length) {
                            pStatusPayload = (OADI_KD_STATUS_PAYLOAD *)&mdata[idx]; idx += sizeof(OADI_KD_STATUS_PAYLOAD);
                            pVersionPayload = (OADI_KD_VERSION_PAYLOAD *)&mdata[idx]; idx += sizeof(OADI_KD_VERSION_PAYLOAD);
        
                            memset(buffer,0,sizeof(buffer)); memcpy(buffer, pStatusPayload->meterSerial, sizeof(pStatusPayload->meterSerial));
        	                PRINTOUT("  METER SERIAL : %s ",  buffer);
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->hwVersion,sizeof(pVersionPayload->hwVersion)); PRINTOUT("%s ",  buffer);
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->swVersion,sizeof(pVersionPayload->swVersion)); PRINTOUT("%s ",  buffer);
        	                PRINTOUT(" CURRENT PULSE : %d\xd\xa",  BigToHostInt(pStatusPayload->curPulse));
        
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->testDate,2); year = strtol(buffer,(char **)NULL, 10);
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->testDate+2,2); month = strtol(buffer,(char **)NULL, 10);
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->testDate+4,2); day = strtol(buffer,(char **)NULL, 10);
        	                PRINTOUT("    METER TEST : %04d/%02d/%02d %c\xd\xa",  year, month, day, 
                                    (pVersionPayload->testResult >= 'A' && pVersionPayload->testResult <= 'z') ? pVersionPayload->testResult : ' ');
        
                            memset(buffer,0,sizeof(buffer));
                            MSGAPI_GetKdAlarmStatus(pStatusPayload->alarmStatus, buffer); 
        	                PRINTOUT("  ALARM STATUS : %s\xd\xa",  buffer);
        	                PRINTOUT("  METER STATUS : %s\xd\xa",  MSGAPI_GetKdMeterStatus(pStatusPayload->meterStatus)); 
                        } else if(idx+sizeof(OADI_KD_VERSION_PAYLOAD) <= length) {
                            pVersionPayload = (OADI_KD_VERSION_PAYLOAD *)&mdata[idx]; idx += sizeof(OADI_KD_VERSION_PAYLOAD);
        
        	                PRINTOUT("       VERSION : ");
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->hwVersion,sizeof(pVersionPayload->hwVersion)); PRINTOUT("%s ",  buffer);
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->swVersion,sizeof(pVersionPayload->swVersion)); PRINTOUT("%s ",  buffer);
        	                PRINTOUT(" \xd\xa");
        
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->testDate,2); year = strtol(buffer,(char **)NULL, 10);
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->testDate+2,2); month = strtol(buffer,(char **)NULL, 10);
                            memset(buffer,0,sizeof(buffer));memcpy(buffer,pVersionPayload->testDate+4,2); day = strtol(buffer,(char **)NULL, 10);
        	                PRINTOUT("    METER TEST : %04d/%02d/%02d %c\xd\xa",  year, month, day, 
                                    (pVersionPayload->testResult >= 'A' && pVersionPayload->testResult <= 'z') ? pVersionPayload->testResult : ' ');
                        } else if(idx+sizeof(OADI_KD_STATUS_PAYLOAD) <= length) {
        
                            pStatusPayload = (OADI_KD_STATUS_PAYLOAD *)&mdata[idx]; idx += sizeof(OADI_KD_STATUS_PAYLOAD);
        
                            memset(buffer,0,sizeof(buffer)); memcpy(buffer, pStatusPayload->meterSerial, sizeof(pStatusPayload->meterSerial));
        	                PRINTOUT("  METER SERIAL : %s ",  buffer);
        	                PRINTOUT(" CURRENT PULSE : %d\xd\xa",  BigToHostInt(pStatusPayload->curPulse));
        
                            memset(buffer,0,sizeof(buffer));
                            MSGAPI_GetKdAlarmStatus(pStatusPayload->alarmStatus, buffer); 
        	                PRINTOUT("  ALARM STATUS : %s\xd\xa",  buffer);
        	                PRINTOUT("  METER STATUS : %s\xd\xa",  MSGAPI_GetKdMeterStatus(pStatusPayload->meterStatus)); 
        
                        } else if(idx+2 <= length) {
                            memset(buffer,0,sizeof(buffer));
        
                            MSGAPI_GetKdAlarmStatus(mdata[idx], buffer); idx++;
        
        	                PRINTOUT("  ALARM STATUS : %s\xd\xa",  buffer);
        	                PRINTOUT("  METER STATUS : %s\xd\xa",  MSGAPI_GetKdMeterStatus(mdata[idx])); idx++;
                        }
                        break;
#endif
                    default:
                        break;
                }
                state = STATE_SD_DONE;
            break;
        }
    }
}

#ifdef __USE_STDOUT__
void repeater_data_parsing(char *mdata, UINT length, BOOL bDetail)
#else
void repeater_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail)
#endif
{
    REPEATER_METERING_DATA repeaterData;
    SOLAR_TIME_DATE solarTimeDate;
    SOLAR_LOG_DATA solarLogData;
    int     state;
    UINT    idx = 0, i;
    char    buffer[256];
    int     count=0;

    state = STATE_SD_READ_INFO;
    while(state != STATE_SD_DONE) {
        switch(state) {
            case STATE_SD_READ_INFO:
                if(idx+sizeof(REPEATER_METERING_DATA) > length) {
                    PRINTOUT("ERROR -- Read REPEATER INFO Fail\r\n");
                    state = STATE_SD_DONE;
                    break;
                }
                memcpy(&repeaterData, mdata+idx, sizeof(REPEATER_METERING_DATA));
                idx += sizeof(REPEATER_METERING_DATA);
                
                MSGAPI_GetAlarmMask(BigToHostShort(repeaterData.alarmMask), buffer);
                PRINTOUT("   INFORMATION : %04d/%02d/%02d %02d:%02d:%02d (TZ:%03d, DST:%02d) OPRDAY=%d, ACTMIN=%d\r\n"
                                  "                 TYPE=%s, RK=%s, PERMIT MODE=%s, PERMIT TIME=%d\r\n"
                                  "                 TEST=%s, ALARM=%s %s\r\n"
                                  "                 BATT=%.4f, CURRENT=%d, OFFSET=%.4f\r\n"
                                  "                 SOLAR AD=%.4f, CHG BATT=%.4f, VCC=%.4f\r\n"
                                  "                 FW=%.1f, BLD=0x%02X, HW=%.1f, SW=%.1f\r\n",
                    BigToHostShort(repeaterData.time.year),
                    repeaterData.time.mon ,
                    repeaterData.time.day ,
                    repeaterData.time.hour ,
                    repeaterData.time.min ,
                    repeaterData.time.sec ,
                    BigToHostShort(repeaterData.time.tz),
                    BigToHostShort(repeaterData.time.dst),
                    BigToHostShort(repeaterData.operDay),
                    BigToHostShort(repeaterData.activeMin),
                    MSGAPI_GetPowerType(repeaterData.powerType),
                    MSGAPI_GetResetKind(repeaterData.resetReason),
                    repeaterData.permitMode ? "AUTO" : "MANUAL",
                    repeaterData.permitState ? 255 : 0,
                    repeaterData.testFlag ? "ON" : "OFF",
                    repeaterData.alarmFlag ? "ON" : "OFF",
                    buffer,
                    (double)(BigToHostShort(repeaterData.battVolt)) / 10000.0, 
                    BigToHostShort(repeaterData.current), 
                    (double)(repeaterData.offset) / 10000.0, 
                    (double)(BigToHostShort(repeaterData.solarAdVolt)) * 2 / 10000.0, 
                    (double)(BigToHostShort(repeaterData.solarChgBattVolt)) / 10000.0, 
                    (double)(BigToHostShort(repeaterData.solarBoardDcVolt)) / 10000.0, 
                    (repeaterData.fwVersion >> 4) + (repeaterData.fwVersion & 0x0f) * 0.1,
                    repeaterData.fwBuild,
                    (repeaterData.hwVersion >> 4) + (repeaterData.hwVersion & 0x0f) * 0.1,
                    (repeaterData.swVersion >> 4) + (repeaterData.swVersion & 0x0f) * 0.1);

                count = repeaterData.solarLogCount;
                state = STATE_SD_READ_SOLAR_LOG;
            break;
            case STATE_SD_READ_SOLAR_LOG :
                if(count <= 0) {
                    state = STATE_SD_DONE;
                    break;
                }

                if(idx+sizeof(SOLAR_TIME_DATE) > length) {
                    PRINTOUT("ERROR -- Read SOLAR LOG TIME DATE Fail\r\n");
                    state = STATE_SD_DONE;
                    break;
                }

                memcpy(&solarTimeDate, mdata+idx, sizeof(SOLAR_TIME_DATE));
                idx += sizeof(SOLAR_TIME_DATE);

                PRINTOUT("   SOLAR BATTERY LOG : %04d/%02d/%02d\r\n",
                    BigToHostShort(solarTimeDate.year),
                    solarTimeDate.month,
                    solarTimeDate.day);

                if(idx+(sizeof(SOLAR_LOG_DATA) * solarTimeDate.hourCount) > length) {
                    PRINTOUT("ERROR -- Read SOLAR LOG DATA Fail\r\n");
                    state = STATE_SD_DONE;
                    break;
                }

                for(i=0;i<solarTimeDate.hourCount;i++) {
                    memcpy(&solarLogData, mdata+idx, sizeof(SOLAR_LOG_DATA));
                    idx += sizeof(SOLAR_LOG_DATA);
                    PRINTOUT("                  + %02d:%02d SOLAR AD=%.4f, CHG BATT=%.4f, VCC=%.4f\r\n",
                        (int)(solarLogData.hour / 4), (solarLogData.hour % 4) * 15,
                        (double)(BigToHostShort(solarLogData.solarAdVolt)) * 2 / 10000.0, 
                        (double)(BigToHostShort(solarLogData.solarChgBattVolt)) / 10000.0, 
                        (double)(BigToHostShort(solarLogData.solarBoardDcVolt)) / 10000.0);
                }
                count --;
            break;
            default :
                state = STATE_SD_DONE;
                break;
        }
    }
}


#ifdef __USE_STDOUT__
void mbus_data_parsing(char *mdata, UINT length, BOOL bDetail)
#else
void mbus_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail)
#endif
{
    TIMEDATE    lpDate;
    BYTE        nType = 0;
    int         nLen = 0;
    int         nLpSize = 0;
    int         nBaseHeaderSize = 0;
    int         nLpCount = 0;
    int         count=0;
    int         state, i=0;
    WORD        nDataLen;
    UINT        idx = 0;
    UNIONVALUE  uv;
    BYTE        buffer[256];
    MBUS_FIXED_DATA_HEADER *dheader;
    MBUS_LONG_FRAME *longFrame;

    state = STATE_SD_READ_TYPE;
    while(state != STATE_SD_DONE) {
        switch(state) 
        {
            case STATE_SD_READ_TYPE:
                if(idx+1 > length) {
                    state = STATE_SD_DONE;
                    break;
                }
                nType = mdata[idx];
                idx ++;
                state = STATE_SD_READ_INFO;
            break;
            case STATE_SD_READ_INFO:
                if(idx+sizeof(TIMEDATE) > length) {
                    state = STATE_SD_DONE;
                    break;
                }
                memcpy(&lpDate, mdata+idx, sizeof(TIMEDATE));
                idx += sizeof(TIMEDATE);
                
                PRINTOUT("   INFORMATION : %04d/%02d/%02d ", BigToHostShort(lpDate.year), lpDate.mon, lpDate.day);
                switch(nType)
                {
                    case 0: 
                        PRINTOUT("%s\r\n", "Kamstrup 401/Compact"); 
                        state = STATE_SD_READ_BASE;
                        break;
                    case 1:
                        PRINTOUT("%s\r\n", "LandisGyr UH50 G2/ULTRAHEAT/G4"); 
                        state = STATE_SD_READ_BASE;
                        break;
                    default:
                        PRINTOUT("%s\r\n", "UNKNOWN"); 
                        state = STATE_SD_DONE;
                        break;
                }
            break;
            case STATE_SD_READ_BASE:
                switch(nType)
                {
                    case 0:
                        nLen = 136;
                        nLpSize = 64;
                        nLpCount = 24;
                        nBaseHeaderSize = nLpSize + 19;
                        break;
                    case 1:
                        nLen = 59;
                        nLpSize = 34;
                        nLpCount = 24;
                        nBaseHeaderSize = nLen;
                        break;
                }
                PRINTOUT("   BASE RECORD : %d bytes\r\n", nLen);
			    //DUMP((char *)(mdata+idx), nLen);

                i = 0;
                longFrame = (MBUS_LONG_FRAME *)(mdata+idx);
                PRINTOUT("                 + %-10s %d\r\n", "PORT", longFrame->hdr.addr);
                i += sizeof(MBUS_LONG_HEADER);

                dheader = (MBUS_FIXED_DATA_HEADER *)(mdata+idx+i);
                memset(buffer, 0, sizeof(buffer)); ReverseBcd(dheader->customer, buffer, sizeof(dheader->customer)); 
                PRINTOUT("                 + %-10s %llu\r\n", "ID", BcdToInt(buffer, sizeof(dheader->customer)));
                PRINTOUT("                 + %-10s %s %s\r\n", "Manufr.ID", 
                        ConvManufacturerCode((const char *)dheader->manufacturer, (char *)buffer), 
                        GetManufacturerFullString((char *)buffer));
                PRINTOUT("                 + %-10s %d\r\n", "Generation", dheader->generation);
                PRINTOUT("                 + %-10s %s\r\n", "Medium", mbusGetMedium(dheader->medium));
                PRINTOUT("                 + %-10s %02X\r\n", "Access", dheader->access);
                PRINTOUT("                 + %-10s %02X\r\n", "Status", dheader->status);

                i += sizeof(MBUS_FIXED_DATA_HEADER);

                for(nDataLen=0; i<nBaseHeaderSize ; i+= nDataLen+2)
                {
                    memset(&uv, 0, sizeof(UNIONVALUE));
                    memset(buffer, 0, sizeof(buffer));
                    nDataLen = mbusGetDIFVIFValue(mdata[idx+i], 0x00, mdata[idx+i+1], (BYTE *)&mdata[idx+i+2], &uv, buffer);
                    PRINTOUT("                 + %s\r\n", buffer);
                }
                idx += nLen;
                count = 0;
                state = STATE_SD_LP_ENTRY;
            break;
            case STATE_SD_LP_ENTRY:
                if(count >= nLpCount) 
                {
                    state = STATE_SD_READ_TYPE;
                    break;
                }
                if((idx+nLpSize) <= length) {
                    if((unsigned char)(mdata[idx]) != 0xFF) 
                    {
                        PRINTOUT("      LP COUNT : %d\r\n", count+1);

			            //DUMP((char *)(mdata+idx), nLpSize);
                        for(i=0, nDataLen=0; i < nLpSize; i+= nDataLen+2)
                        {
                            memset(&uv, 0, sizeof(UNIONVALUE));
                            memset(buffer, 0, sizeof(buffer));
                            nDataLen = mbusGetDIFVIFValue(mdata[idx+i], 0x00, mdata[idx+i+1], (BYTE *)&mdata[idx+i+2], &uv, buffer);
                            PRINTOUT("                 + %s\r\n", buffer);
                        }
                    }
                    idx += nLpSize;
                    count ++;
                } else {
                    state = STATE_SD_DONE;
                }
            break;
        }
    }
}

#if     defined(_WIN32)
#pragma pack(push, 1)
#endif
typedef struct  _tagDLMSMDATA
{
    OBISCODE    obis;
    BYTE        cls;
    BYTE        attr;
    WORD        len;
    BYTE        data[0];
}   __ATTRIBUTE_PACKED__ DLMSMDATA;
#if     defined(_WIN32)
#pragma pack(pop)
#endif

#ifdef __USE_STDOUT__
void dlms_dump_tag_value(const char *prefix, DLMS_TAG *pList, int nLength)
#else
void dlms_dump_tag_value(CLISESSION *pSession, const char *prefix, DLMS_TAG *pList, int nLength)
#endif
{
	DLMSTIMEDATE	*pTimeDate;
	DLMSDATE		*pDate;
	DLMSTIME		*pTime;
	DLMS_TAG		*pTag;
	DLMS_ARG		*pArg;
	BYTE			*pSeek;
	int				i, nSeek, n=0, len;

	pSeek = (BYTE *)pList;
	for(i=0, n=0, nSeek=0; i<nLength; i+=nSeek)
	{
		pTag = (DLMS_TAG *)(pSeek+i);
		pArg = (DLMS_ARG *)(pSeek+i);

	    switch(pTag->tag) {
		  case DLMS_DATATYPE_ARRAY :
               len = 0;
               if(pArg->var.array & 0x80)
               {
                   len = pArg->var.array & 0x7F;
               }
			   PRINTOUT("%s %02d: ARRAY(%d), length=1, value=%d\r\n", prefix,
							n+1, pTag->tag, *(&pArg->var.array + len));
			   break;
		  case DLMS_DATATYPE_COMPACT_ARRAY :
               len = 0;
               if(pArg->var.array & 0x80)
               {
                   len = pArg->var.array & 0x7F;
               }
			   PRINTOUT("%s %02d: COMPACT-ARRAY(%d), length=1, value=%d\r\n", prefix,
							n+1, pTag->tag, *(&pArg->var.array + len));
			   break;
		  case DLMS_DATATYPE_STRUCTURE :
			   PRINTOUT("%s %02d: STRUCTURE(%d), length=1, value=%d\r\n", prefix, n+1, pTag->tag, pArg->var.u8);
			   break;
		  case DLMS_DATATYPE_GROUP :
			   PRINTOUT("%s %02d: GROUP(%d), length=1, groupLen=%d\r\n", prefix, n+1, pTag->tag, pTag->len);
			   break;
	      case DLMS_DATATYPE_BOOLEAN :
			   PRINTOUT("%s %02d: BOOLEAN(%d), length=1, value=%s(0x%02X)\r\n", prefix, n+1, pTag->tag,
						pArg->var.u8 == 0 ? "FALSE" : "TRUE", pArg->var.u8);
			   break;
          case DLMS_DATATYPE_ENUM :
			   PRINTOUT("%s %02d: ENUM(%d), length=1, value=%d\r\n", prefix, n+1, pTag->tag, pArg->var.u8);
			   break;
	      case DLMS_DATATYPE_BIT_STRING :
			   PRINTOUT("%s %02d: BIT-STRING(%d), length=%d, value=", prefix, n+1, pTag->tag, pTag->len);
			   DUMP((char *)pTag->data, pTag->len, FALSE);
			   break;
	      case DLMS_DATATYPE_INT32 :
			   PRINTOUT("%s %02d: INT32(%d), length=4, value=%d\r\n", prefix, n+1, pTag->tag,
						(signed int)BigToHostInt(pArg->var.i32));
			   break;
	      case DLMS_DATATYPE_UINT32 :
			   PRINTOUT("%s %02d: UINT32(%d), length=4, value=%d\r\n", prefix, n+1, pTag->tag, 
						(unsigned int)BigToHostInt(pArg->var.u32));
			   break;
	      case DLMS_DATATYPE_FLOAT32 :
			   PRINTOUT("%s %02d: FLOAT32(%d), length=4, value=%f\r\n", prefix, n+1, pTag->tag, 
						(double)BigOctetToHostFloat((char *)&pArg->var.u32));
			   break;
	      case DLMS_DATATYPE_OCTET_STRING :
			   PRINTOUT("%s %02d: OCTET-STRING(%d), length=%d, value=", prefix, n+1, pTag->tag, pTag->len);
			   DUMP((char *)pTag->data, pTag->len, FALSE);
			   break;
	      case DLMS_DATATYPE_VISIBLE_STRING :
			   PRINTOUT("%s %02d: VISIBLE-STRING(%d), length=%d, value=", prefix, n+1, pTag->tag, pTag->len);
			   DUMP((char *)pTag->data, pTag->len, FALSE);
			   break;
	      case DLMS_DATATYPE_BCD :
			   PRINTOUT("%s %02d: BCD(%d), length=1, value=%d\r\n", prefix, n+1, pTag->tag,
						(((pArg->var.u8 & 0xf0) >> 4) * 10) + (pArg->var.u8 & 0x0f));
			   break;
	      case DLMS_DATATYPE_INT8 :
			   PRINTOUT("%s %02d: INT8(%d), length=1, value=%d\r\n", prefix, n+1, pTag->tag, (signed char)pArg->var.i8);
			   break;
	      case DLMS_DATATYPE_INT16 :
			   PRINTOUT("%s %02d: INT16(%d), length=2, value=%d\r\n", prefix, n+1, pTag->tag,
						(signed short)BigToHostShort(pArg->var.i16));
			   break;
	      case DLMS_DATATYPE_UINT8 :
			   PRINTOUT("%s %02d: UINT8(%d), length=1, value=%d\r\n", prefix, n+1, pTag->tag, pArg->var.u8);
			   break;
	      case DLMS_DATATYPE_UINT16 :
			   PRINTOUT("%s %02d: UINT16(%d), length=2, value=%d\r\n", prefix, n+1, pTag->tag,
						(unsigned short)BigToHostShort(pArg->var.u16));
			   break;
  	      case DLMS_DATATYPE_INT64 :
			   PRINTOUT("%s %02d: INT64(%d), length=8\r\n", prefix, n+1, pTag->tag);
			   break;
	      case DLMS_DATATYPE_UINT64 :
			   PRINTOUT("%s %02d: UINT64(%d), length=8\r\n", prefix, n+1, pTag->tag);
			   break;
	      case DLMS_DATATYPE_DATE_TIME :
			   pTimeDate = (DLMSTIMEDATE *)&pArg->var.array;
			   PRINTOUT("%s %02d: DATETIME(%d), length=%d, value=%04d/%02d/%02d %02d:%02d:%02d.%02d (%d)\r\n", prefix, 
						n+1, pTag->tag, sizeof(DLMSTIMEDATE),
						BigToHostShort(pTimeDate->year), pTimeDate->mon, pTimeDate->day,
						pTimeDate->hour, pTimeDate->min, pTimeDate->sec, pTimeDate->hsec,
						pTimeDate->week);
			   break;
		  case DLMS_DATATYPE_DATE :
			   pDate = (DLMSDATE *)&pArg->var.array;
			   PRINTOUT("%s %02d: DATE(%d), length=%d, value=%04d/%02d/%02d (%d)\r\n", prefix, 
						n+1, pTag->tag, sizeof(DLMSDATE),
						BigToHostShort(pDate->year), pDate->mon, pDate->day, pDate->week);
			   break;
		  case DLMS_DATATYPE_TIME :
			   pTime = (DLMSTIME *)&pArg->var.array;
			   PRINTOUT("%s %02d: TIME(%d), length=%d, value=%02d:%02d:%02d.%02d\r\n", prefix, 
						n+1, pTag->tag, sizeof(DLMSTIME),
						pTime->hour, pTime->min, pTime->sec, pTime->hsec);
			   break;
  	      case DLMS_DATATYPE_NULL :
			   PRINTOUT("%s %02d: NULL(%d), length=1\r\n", prefix, n+1, pTag->tag);
			   break;
		  default :
			   PRINTOUT("%s %02d: UNKNOWN(%d), length=%d\r\n", prefix, n+1, pTag->tag, pTag->len);
			   break;
	    }

		nSeek = dlmsGetTagSize((BYTE *)pTag);   
		n++;
	}
}

#define DLMS_GAP    "      "

OBISCODE _vz = OBIS_CODE_VZ;
OBISCODE _billing_peroid = OBIS_CODE_BILLING_PERIOD;
OBISCODE _device_info = OBIS_CODE_DEVICE_INFO;
OBISCODE _monthly_enery_profile = OBIS_CODE_MONTHLY_ENERY_PROFILE;
OBISCODE _monthly_demand_profile = OBIS_CODE_MONTHLY_DEMAND_PROFILE;
OBISCODE _kepco_meter_info = OBIS_CODE_KEPCO_METER_INFO;
OBISCODE _load_profile = OBIS_CODE_LOAD_PROFILE;
OBISCODE _meter_constant_active = OBIS_CODE_METER_CONSTANT_ACTIVE;
OBISCODE _meter_constant_reactive = OBIS_CODE_METER_CONSTANT_REACTIVE;
OBISCODE _current_max_demand = OBIS_CODE_KEPCO_CURRENT_MAX_DEMAND;
OBISCODE _previous_max_demand = OBIS_CODE_KEPCO_PREVIOUS_MAX_DEMAND;
OBISCODE _power_failure = OBIS_CODE_POWER_FAILURE;
OBISCODE _power_restore = OBIS_CODE_POWER_RESTORE;
OBISCODE _manufacturer_meter_id = OBIS_CODE_MANUFACTURER_METER_ID;
OBISCODE _customer_meter_id = OBIS_CODE_CUSTOMER_METER_ID;
OBISCODE _lcd_info = OBIS_CODE_LCD_INFO;
OBISCODE _load_control_status = OBIS_CODE_LOAD_CONTROL_STATUS;
OBISCODE _output_signal = OBIS_CODE_OUTPUT_SIGNAL;
OBISCODE _instantaneous = OBIS_CODE_INSTANTANEOUS;
OBISCODE _sag = OBIS_CODE_SAG;
OBISCODE _swell = OBIS_CODE_SWELL;

#define MAX_DLMS_TOKEN      1024

const char *dlms_result_total_tariff[] = {"Tot", "Tr1", "Tr2", "Tr3", "Tr4"};

BOOL _makeKepcoLpStatusString(BYTE *statusCode, int len, char * szBuffer)
{
    if(szBuffer == NULL) return FALSE;
    if(len > 2)
    {
        WORD tariff = statusCode[1] & 0x03;

        sprintf(szBuffer, "0x%02X%02X%02X t%d %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
            statusCode[0], statusCode[1], statusCode[2], tariff + 1,
            statusCode[0] & 0x80 ? "OC " : "",   // Over Current
            statusCode[0] & 0x10 ? "MCo " : "",   // Modem Cover
            statusCode[0] & 0x08 ? "TCo " : "",   // Terminal Cover
            statusCode[0] & 0x04 ? "LOn " :"",   // Latch ON
            statusCode[0] & 0x02 ? "LOff " :"",  // Latch OFF
            statusCode[0] & 0x01 ? "LE " : "",   // Latch Error
            statusCode[1] & 0x80 ? "SS " : "",   // Sag/Swell
            statusCode[1] & 0x40 ? "CE " : "",   // Connection Error (오결선)
            statusCode[1] & 0x20 ? "TM " : "",   // Temperature 
            statusCode[1] & 0x10 ? "DST " : "",  // Daylight Saving Time
            statusCode[1] & 0x08 ? "MD " : "",   // Magnetic Detect
            statusCode[1] & 0x04 ? "CLOff " : "",   // Current Limite Cut-off
            statusCode[2] & 0x80 ? "SR " : "",   // Self Read
            statusCode[2] & 0x40 ? "PF " : "",   // Power Failure
            statusCode[2] & 0x20 ? "TC " : "",   // Time Change
            statusCode[2] & 0x10 ? "MDR " : "",  // Manual DR
            statusCode[2] & 0x08 ? "DR " : "",   // DR
            statusCode[2] & 0x04 ? "NB " : "",   // No Battery
            statusCode[2] & 0x02 ? "LM " : "",   // Line Missing
            statusCode[2] & 0x01 ? "PC " : ""    // Program Change
        );
    }
    else
    {
        sprintf(szBuffer, "0x%02X %s%s%s%s%s%s%s%s", 
            statusCode[0], 
            statusCode[0] & 0x80 ? "SR " : "",   // Self Read
            statusCode[0] & 0x40 ? "PF " : "",   // Power Failure
            statusCode[0] & 0x20 ? "TC " : "",   // Time Change
            statusCode[0] & 0x10 ? "MDR " : "",  // Manual DR
            statusCode[0] & 0x08 ? "DR " : "",   // DR
            statusCode[0] & 0x04 ? "NB " : "",   // No Battery
            statusCode[0] & 0x02 ? "LM " : "",   // Line Missing
            statusCode[0] & 0x01 ? "PC " : ""    // Program Change
        );
    }
    return TRUE;
}


const char * _getDlmsMeterInfo(BYTE minfo)
{
    BYTE n = minfo >> 4;

    switch(n)
    {
        case 0x00: return "1-Phase 2-Wire";
        case 0x01: return "1-Phase 3-Wire";
        case 0x02: return "3-Phase 3-Wire";
        case 0x03: return "3-Phase 4-Wire";
    }
    return "INVLID";
}

#ifdef __USE_STDOUT__
BOOL dlms_class_parsing(DLMSVARIABLE *pToken, OBISCODE *obis, BYTE cls, BYTE attr, BYTE *data, WORD nLen)
#else
BOOL dlms_class_parsing(CLISESSION *pSession, DLMSVARIABLE *pToken, OBISCODE *obis, BYTE cls, BYTE attr, BYTE *data, WORD nLen)
#endif
{
    char        szBuffer[64+1];
    DLMSTIMEDATE   *pDTime, *pDTime2;
    TIMESTAMP   *pTime;
    int         nToken = 0;
    int         i,n,len;

    if(!pToken || !obis || !data) return FALSE;

    /** TODO : Block Data 일 경우 처리가 달라야 한다 */
    dlmsToken(pToken, nToken, &nToken, MAX_DLMS_TOKEN, data, nLen);
    if(nToken <= 0) return FALSE;

    switch(cls)
    {
        case DLMS_CLASS_DATA:
            switch(attr)
            {
                case 2: // value
                    if(!memcmp(obis, &_device_info, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        memset(szBuffer, 0, sizeof(szBuffer));
                        strncpy(szBuffer, (char *)pToken[0].var.p, MIN(pToken[0].len, sizeof(szBuffer)-1));
                        PRINTOUT(DLMS_GAP "DeviceName %s\r\n", szBuffer);
                        return TRUE;
                    }else if(!memcmp(obis, &_vz, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        PRINTOUT(DLMS_GAP "VZ %d\r\n", pToken[0].var.u8);
                        return TRUE;
                    }else if(!memcmp(obis, &_billing_peroid, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        PRINTOUT(DLMS_GAP "BillingPeroid %d\r\n", pToken[0].var.u8);
                        return TRUE;
                    }else if(!memcmp(obis, &_manufacturer_meter_id, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        memset(szBuffer, 0, sizeof(szBuffer));
                        strncpy(szBuffer, (char *)pToken[0].var.p, MIN(pToken[0].len, sizeof(szBuffer)-1));
                        PRINTOUT(DLMS_GAP "Manufacturer MeterId %s\r\n", szBuffer);
                        return TRUE;
                    }else if(!memcmp(obis, &_customer_meter_id, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        memset(szBuffer, 0, sizeof(szBuffer));
                        strncpy(szBuffer, (char *)pToken[0].var.p, MIN(pToken[0].len, sizeof(szBuffer)-1));
                        PRINTOUT(DLMS_GAP "Customer MeterId %s\r\n", szBuffer);
                        return TRUE;
                    }else if(!memcmp(obis, &_load_control_status, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        PRINTOUT(DLMS_GAP "Relay Status %s (%s)\r\n", 
                                pToken[0].var.u8 ? "On" : "Off", pToken[0].var.u8 ? "Close" : "Open");
                        return TRUE;
                    }else if(!memcmp(obis, &_output_signal, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        PRINTOUT(DLMS_GAP "%s\r\n", _getKepcoAdditionalEquipment(pToken[0].var.u8));
                        return TRUE;
                    }
                    break;
            }
            break;
        case DLMS_CLASS_REGISTER:
            switch(attr)
            {
                case 2: // value
                    if(!memcmp(obis, &_meter_constant_active, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        PRINTOUT(DLMS_GAP "ActiveC %6.3f\r\n", BigOctetToHostFloat((char *)&pToken[0].var.u32));
                        return TRUE;
                    } else if(!memcmp(obis, &_meter_constant_reactive, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        PRINTOUT(DLMS_GAP "ReactiveC %6.3f\r\n", BigOctetToHostFloat((char *)&pToken[0].var.u32));
                        return TRUE;
                    }
                    break;
                case 3: // scaler_unit
                    if(nToken < 3) break;
                    PRINTOUT(DLMS_GAP "Scaler %d Unit %s\r\n", pToken[1].var.i8, MSGAPI_GetDlmsUnit(pToken[2].var.u8));
                    return TRUE;
            }
            break;
        case DLMS_CLASS_PROFILE_GENERIC:
            switch(attr)
            {
                case 2: // buffer
                    if(!memcmp(obis, &_monthly_enery_profile, sizeof(OBISCODE)-1)) {        // VZ 부분은 검사 안함
                        if(nToken < 14) break;                                              // 4 * 3 + 2
                        /** Issue #930 : G-Type Meter 검침
                         * G-Type 미터와 표준미터와는 구성된 항목이 다르다.
                         * G-Type 미터의 경우 무효 전력양도 포함된다
                         */
                        if(pToken[2+2].type == DLMS_DATATYPE_FLOAT32) // 표준미터
                        {
                            for(i=2, n=0; i<nToken && n < 4; i+= 3, n++)
                            {
                                PRINTOUT(DLMS_GAP "%s Active %6u Lag-Reactive %6u AvgPFactor %6.3f\r\n", 
                                        dlms_result_total_tariff[n], 
                                        pToken[i].var.u32, pToken[i+1].var.u32, 
                                        pToken[i+2].var.f32);
                            }
                        }
                        else if(pToken[2+4].type == DLMS_DATATYPE_FLOAT32) // G-Type Meter
                        {
                            for(i=2, n=0; i<nToken && n < 5; i+= 5, n++)
                            {
                                //PRINTOUT(DLMS_GAP "%s Active %6u Apparent %6u Lag-Reactive %6u Lead-Reactive %6u AvgPFactor %6.3f\r\n", 
                                PRINTOUT(DLMS_GAP "%s Active %6u Apparent %6u Lag-Reactive %6u Lead-Reactive %6u AvgPFactor %6.3f\r\n", 
                                        dlms_result_total_tariff[n], 
                                        pToken[i].var.u32, pToken[i+1].var.u32, 
                                        pToken[i+2].var.u32, pToken[i+3].var.u32, 
                                        pToken[i+4].var.f32);
                            }
                        }
                        else    // Unknown Type
                        {
                            break;
                        }
                        return TRUE;
                    } else if(!memcmp(obis, &_monthly_demand_profile, sizeof(OBISCODE)-1)) {    // VZ 부분은 검사 안함
                        if(nToken < 26) break;                                              // 4 * 6 + 2
                        for(i=2, n=0; i<nToken && n < 4; i+= 6, n++)
                        {
                            pDTime = (DLMSTIMEDATE *)pToken[i+1].var.p;
                            pDTime2 = (DLMSTIMEDATE *)pToken[i+4].var.p;
                            PRINTOUT(DLMS_GAP "%s Active %6u Max %04d/%02d/%02d %02d:%02d:%02d Cum %6u\r\n" 
                                     DLMS_GAP "  Reactive %6u Max %04d/%02d/%02d %02d:%02d:%02d Cum %6u\r\n", 
                                    dlms_result_total_tariff[n], 
                                    pToken[i].var.u16,
                                    BigToHostShort(pDTime->year), pDTime->mon, pDTime->day,
                                    pDTime->hour, pDTime->min, pDTime->sec,
                                    pToken[i+2].var.u32,
                                    pToken[i+3].var.u16,
                                    BigToHostShort(pDTime2->year), pDTime2->mon, pDTime2->day,
                                    pDTime2->min, pDTime2->sec, pDTime->sec,
                                    pToken[i+5].var.u32);
                        }
                        return TRUE;
                    } else if(!memcmp(obis, &_current_max_demand, sizeof(OBISCODE)) ||
                                !memcmp(obis, &_previous_max_demand, sizeof(OBISCODE))) {
                        if(nToken < 17) break;                                              // 3 * 5 + 2
                        for(i=2, n=1; i<nToken && n < 4; i+= 5, n++)
                        {
                            pDTime = (DLMSTIMEDATE *)pToken[i+4].var.p;
                            PRINTOUT(DLMS_GAP "%s %04d/%02d/%02d %02d:%02d:%02d Active %6u Reactive %6u MaxDemand %6u CumDemand %6u\r\n", 
                                    dlms_result_total_tariff[n], 
                                    BigToHostShort(pDTime->year), pDTime->mon, pDTime->day,
                                    pDTime->hour, pDTime->min, pDTime->sec,
                                    pToken[i].var.u32, pToken[i+1].var.u32, pToken[i+2].var.u16, pToken[i+3].var.u32);
                        }
                        return TRUE;
                    } else if(!memcmp(obis, &_kepco_meter_info, sizeof(OBISCODE))) {
                        if(nToken < 15) break;
                        memset(szBuffer, 0, sizeof(szBuffer));
                        strncpy(szBuffer, pToken[4].var.p ? (char *)pToken[4].var.p : "", 8);
                        pTime = (TIMESTAMP *)pToken[5].var.p;
                        PRINTOUT(DLMS_GAP "MeterID %s %04d/%02d/%02d %02d:%02d:%02d %s\r\n", 
                                szBuffer,
                                BigToHostShort(pTime->year), pTime->mon, pTime->day,
                                pTime->hour, pTime->min, pTime->sec, 
                                (pToken[5].var.p[7] < 7 ? dayofweek[pToken[5].var.p[7]] : "INV")
                            );
                        PRINTOUT(DLMS_GAP "%s %d-Class %s\r\n", 
                                _getDlmsMeterInfo(pToken[2].var.p[0]), pToken[2].var.p[0] & 0x0F,
                                _getKepcoVendorString(pToken[3].var.p[0]));
                        PRINTOUT(DLMS_GAP "Error 0x%02X Caution 0x%02X %d pulses/kwh PTr %d CTr %d\r\n", 
                                pToken[6].var.p[0], pToken[7].var.p[0],
                                pToken[8].var.u16, pToken[9].var.u16, pToken[10].var.u16);
                        pTime = (TIMESTAMP *)pToken[13].var.p;
                        PRINTOUT(DLMS_GAP "Day %d Interval %d Last %04d/%02d/%02d %02d:%02d\r\n", 
                                pToken[11].var.u8, pToken[12].var.u8,
                                BigToHostShort(pTime->year), pTime->mon, pTime->day,
                                pTime->hour, pTime->min);
                        pTime = (TIMESTAMP *)&pToken[14].var.p[2];
                        PRINTOUT(DLMS_GAP "SelfRead 0x%02X %04d/%02d/%02d %02d:%02d:%02d %s\r\n", 
                                pToken[14].var.p[1] & 0xFF,
                                BigToHostShort(pTime->year), pTime->mon, pTime->day,
                                pTime->hour, pTime->min, pTime->sec, 
                                (pToken[14].var.p[9] < 7 ? dayofweek[pToken[14].var.p[9]] : "INV"));
                        return TRUE;
                    } else if(!memcmp(obis, &_load_profile, sizeof(OBISCODE))) {
                        for(n=0;n<nToken;n++)
                        {
                            if(pToken[n].type == DLMS_DATATYPE_STRUCTURE) break;
                        }

                        for(i=n-1,n=1; i<nToken; i+=n)
                        {
                            n = 1;
                            if(pToken[i].type != DLMS_DATATYPE_STRUCTURE)
                            {
                                /** 시작이 잘못된 것은 Skip */
                                continue;
                            }

                            /** 한전 일진 미터 2009의 경우
                              *
                              *  - 한전 구매 규격상 LP의 경우 6개의 Attribute가 와야 하지만 2개의 Attribute 만
                              *    전송해 준다. 따라서 이에 대한 예외 처리를 수행해 줘야 한다.
                              */

                            if(nToken > i + 6 &&
                               pToken[i+1].type == DLMS_DATATYPE_UINT16 &&
                               pToken[i+2].type == DLMS_DATATYPE_UINT16 &&
                               pToken[i+3].type == DLMS_DATATYPE_UINT16 &&
                               pToken[i+4].type == DLMS_DATATYPE_UINT16 &&
                               pToken[i+5].type == DLMS_DATATYPE_OCTET_STRING &&
                               pToken[i+6].type == DLMS_DATATYPE_BIT_STRING)
                            {
                                /** 한전 구매 규격 일치 미터 */
                                pDTime = (DLMSTIMEDATE *)pToken[i+5].var.p;
                                memset(szBuffer, 0, sizeof(szBuffer));
                                _makeKepcoLpStatusString(pToken[i+6].var.p, pToken[i+6].len, szBuffer);
                                PRINTOUT(DLMS_GAP "%04d/%02d/%02d %02d:%02d:%02d %4d %4d %4d %4d %s\r\n",
                                        BigToHostShort(pDTime->year), pDTime->mon, pDTime->day,
                                        pDTime->hour, pDTime->min, pDTime->sec, 
                                        pToken[i+1].var.u16, pToken[i+2].var.u16,
                                        pToken[i+3].var.u16, pToken[i+4].var.u16,
                                        szBuffer);
                                n = 6 + 1;
                            }else if(nToken > i+ 2 &&
                                pToken[i+1].type == DLMS_DATATYPE_UINT16 && pToken[i+2].type == DLMS_DATATYPE_UINT16)
                            {
                                /** 일진 미터 */
                                PRINTOUT(DLMS_GAP "%4d %4d\r\n", 
                                        pToken[i+1].var.u16, pToken[i+2].var.u16);
                                n = 2 + 1;
                            } else if(nToken > i + 10 && 
                               pToken[i+1].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+2].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+3].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+4].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+5].type == DLMS_DATATYPE_OCTET_STRING &&
                               pToken[i+6].type == DLMS_DATATYPE_BIT_STRING &&
                               pToken[i+7].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+8].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+9].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+10].type == DLMS_DATATYPE_UINT32)
                            {
                                /** Issue #930 
                                 * 한전 G-Type Meter의 경우 표준미터와 E-Type Meter과 같이 UINT16을 사용하는 것이
                                 * 아니라 UINT32를 사용한다.
                                 * 순방향/역방향 모두 수집하는 경우
                                 */
                                pDTime = (DLMSTIMEDATE *)pToken[i+5].var.p;
                                memset(szBuffer, 0, sizeof(szBuffer));
                                _makeKepcoLpStatusString(pToken[i+6].var.p, pToken[i+6].len, szBuffer);
                                PRINTOUT(DLMS_GAP "%04d/%02d/%02d %02d:%02d:%02d %4d %4d %4d %4d %4d %4d %4d %4d %s\r\n", 
                                        BigToHostShort(pDTime->year), pDTime->mon, pDTime->day,
                                        pDTime->hour, pDTime->min, pDTime->sec, 
                                        pToken[i+1].var.u32, pToken[i+2].var.u32,
                                        pToken[i+3].var.u32, pToken[i+4].var.u32,
                                        pToken[i+7].var.u32, pToken[i+8].var.u32,
                                        pToken[i+9].var.u32, pToken[i+10].var.u32,
                                        szBuffer);
                                n = 6 + 1;
                            } else if(nToken > i + 6 && 
                               pToken[i+1].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+2].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+3].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+4].type == DLMS_DATATYPE_UINT32 &&
                               pToken[i+5].type == DLMS_DATATYPE_OCTET_STRING &&
                               pToken[i+6].type == DLMS_DATATYPE_BIT_STRING)
                            {
                                /** Issue #930 
                                 * 한전 G-Type Meter의 경우 표준미터와 E-Type Meter과 같이 UINT16을 사용하는 것이
                                 * 아니라 UINT32를 사용한다.
                                 * 순방향만 수집하는 경우
                                 */
                                pDTime = (DLMSTIMEDATE *)pToken[i+5].var.p;
                                memset(szBuffer, 0, sizeof(szBuffer));
                                _makeKepcoLpStatusString(pToken[i+6].var.p, pToken[i+6].len, szBuffer);
                                PRINTOUT(DLMS_GAP "%04d/%02d/%02d %02d:%02d:%02d %4d %4d %4d %4d %s\r\n", 
                                        BigToHostShort(pDTime->year), pDTime->mon, pDTime->day,
                                        pDTime->hour, pDTime->min, pDTime->sec, 
                                        pToken[i+1].var.u32, pToken[i+2].var.u32,
                                        pToken[i+3].var.u32, pToken[i+4].var.u32,
                                        szBuffer);
                                n = 6 + 1;
                            } else
                            {
#ifdef __USE_STDOUT__
                                dlms_dump_tag_value(DLMS_GAP,  (DLMS_TAG *)data, nLen);
#else
                                dlms_dump_tag_value(pSession, DLMS_GAP,  (DLMS_TAG *)data, nLen);
#endif
                                return FALSE;
                            }
                        }
                        return TRUE;
                    } else if(!memcmp(obis, &_power_failure, sizeof(OBISCODE)) ||
                            !memcmp(obis, &_power_restore, sizeof(OBISCODE)) ||
                            !memcmp(obis, &_sag, sizeof(OBISCODE)) ||
                            !memcmp(obis, &_swell, sizeof(OBISCODE))) {
                        if(nToken < 1) break;
                        if(pToken[0].type != DLMS_DATATYPE_ARRAY) break;

                        len = pToken[0].var.array;

                        for(i=0, n=1; i<len && n < nToken; i++, n+=3)
                        {
                            pDTime = (DLMSTIMEDATE *)pToken[n+1].var.p;
                            PRINTOUT(DLMS_GAP "%04d/%02d/%02d %02d:%02d:%02d %6u\r\n", 
                                    BigToHostShort(pDTime->year), pDTime->mon, pDTime->day,
                                    pDTime->hour, pDTime->min, pDTime->sec,
                                    pToken[n+2].var.u16);
                        }
                        return TRUE;
                    } else if(!memcmp(obis, &_instantaneous, sizeof(OBISCODE))) {   // Issue #1049 Sag, Swell, THD 값 수집
                        if(nToken < 20) break;
                        PRINTOUT(DLMS_GAP "Instantaneous Values\r\n");
                        i=2; PRINTOUT(DLMS_GAP "  Current          A %7.3f B %7.3f C %7.3f\r\n", 
                                pToken[i].var.f32, pToken[i+5].var.f32, pToken[i+10].var.f32);
                        i=3; PRINTOUT(DLMS_GAP "  Voltage          A %7.3f B %7.3f C %7.3f\r\n", 
                                pToken[i].var.f32, pToken[i+5].var.f32, pToken[i+10].var.f32);
                        i=4; PRINTOUT(DLMS_GAP "  Voltage THD      A %7.3f B %7.3f C %7.3f\r\n", 
                                pToken[i].var.f32, pToken[i+5].var.f32, pToken[i+10].var.f32);
                        i=5; PRINTOUT(DLMS_GAP "  Power Factor     A %7.3f B %7.3f C %7.3f\r\n", 
                                pToken[i].var.f32, pToken[i+5].var.f32, pToken[i+10].var.f32);
                        i=6; PRINTOUT(DLMS_GAP "  Phase Diff(V-A)  A %7.3f B %7.3f C %7.3f\r\n", 
                                pToken[i].var.f32, pToken[i+5].var.f32, pToken[i+10].var.f32);
                        i=17;PRINTOUT(DLMS_GAP "  Phase Diff(V-V) AB %7.3f          AC %7.3f\r\n", 
                                pToken[i+11].var.f32, pToken[i+12].var.f32);
                             PRINTOUT(DLMS_GAP "  Temperature        %7.3f\r\n", pToken[19].var.f32);
                        return TRUE;
                    }
                    break;
                case 3: // capture object
                    for(n=0;n<nToken;n++)
                    {
                        if(pToken[n].type == DLMS_DATATYPE_STRUCTURE) break;
                    }

                    for(i=n-1,n=1; i<nToken; i+=n)
                    {
                        n = 1;
                        if(pToken[i].type != DLMS_DATATYPE_STRUCTURE)
                        {
                            /** 시작이 잘못된 것은 Skip */
                            continue;
                        }

                        if(nToken > i + 4 && 
                               pToken[i+1].type == DLMS_DATATYPE_UINT16 &&
                               pToken[i+2].type == DLMS_DATATYPE_OCTET_STRING && 
                               pToken[i+3].type == DLMS_DATATYPE_INT8 && 
                               pToken[i+4].type == DLMS_DATATYPE_UINT16)
                        {
                            if(pToken[i+2].len == 6)
                            {
                                // profile generic cpature object schema (obis code)
                                PRINTOUT(DLMS_GAP "OBIS %02X.%02X.%02X.%02X.%02X.%02X\r\n", 
                                        pToken[i+2].var.p[0],
                                        pToken[i+2].var.p[1],
                                        pToken[i+2].var.p[2],
                                        pToken[i+2].var.p[3],
                                        pToken[i+2].var.p[4],
                                        pToken[i+2].var.p[5]);
                            }
                            n = 4 + 1;
                        }
                    }
                    return TRUE;
                case 4: // interval
                    if(nToken < 1) break;

                    PRINTOUT(DLMS_GAP "Interval %d sec(s)\r\n", pToken[0].var.u32);
                    return TRUE;
                case 7: // entries_in_use
                    if(nToken < 1) break;

                    PRINTOUT(DLMS_GAP "Entry %d\r\n", pToken[0].var.u32);
                    return TRUE;
            }
            break;
        case DLMS_CLASS_CLOCK:
            switch(attr)
            {
                case 2: // time
                    if(nToken < 1) break;
                    pDTime = (DLMSTIMEDATE *)pToken[0].var.p;
                    PRINTOUT(DLMS_GAP "Meter Time %04d/%02d/%02d %02d:%02d:%02d\r\n", 
                                    BigToHostShort(pDTime->year), pDTime->mon, pDTime->day,
                                    pDTime->hour, pDTime->min, pDTime->sec);
			        //DUMP((char *)pToken[0].var.p, pToken[0].len, TRUE);
                    return TRUE;
            }
            break;
        case DLMS_CLASS_SCRIPT_TABLE:
            switch(attr)
            {
                case 1: // action response
                    if(nToken < 1) break;
                    PRINTOUT(DLMS_GAP "Method Parameter %d\r\n", pToken[0].var.u16);
                    return TRUE;
            }
            break;
    }

#ifdef __USE_STDOUT__
    dlms_dump_tag_value(          DLMS_GAP, (DLMS_TAG *)data, nLen);
#else
    dlms_dump_tag_value(pSession, DLMS_GAP,  (DLMS_TAG *)data, nLen);
#endif

    return FALSE;
}

#ifdef __USE_STDOUT__
void dlms_data_parsing(char *mdata, UINT length, BOOL bDetail)
#else
void dlms_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail)
#endif
{
    UINT            pos, nSeek;
	char	        *p;
    char            szObis[32];
    DLMSMDATA       *pData;
    WORD            nLen;
    BYTE            cls, attr;
    UINT            cnt=0;
    BYTE            prevCls=0;
    BYTE            prevAttr=0;
    DLMSVARIABLE    *pToken;
    int             nCount = 0, nSize = 0;
    OBISCODE        prevObis;

    memset(&prevObis, 0, sizeof(OBISCODE));
    pToken = (DLMSVARIABLE *)MALLOC(sizeof(DLMSVARIABLE)*MAX_DLMS_TOKEN);
    if(pToken==NULL)
    {
            PRINTOUT("INTERNAL ERROR : MALLOC FAIL\xd\xa");
            return;
    }
    memset(pToken, 0, sizeof(DLMSVARIABLE)*MAX_DLMS_TOKEN);

    /** reading */
    p = (char *)mdata;
	for(pos=0, nSeek=0; pos<length; pos+=nSeek, cnt++)
	{
		pData = (DLMSMDATA *)p;
	    nSeek = sizeof(DLMSMDATA);

        memset(szObis, 0, sizeof(szObis));
        dlmsObisToString(&pData->obis, szObis);
        cls = pData->cls;
        attr = pData->attr;
        nLen = LittleToHostShort(pData->len);

	    if(bDetail) {
            if(memcmp(&prevObis, &pData->obis, sizeof(OBISCODE)) ||
                prevCls != cls || prevAttr != attr)
            {
                if(prevCls != 0 && prevAttr != 0)
                {
                    PRINTOUT("      %2d frame(s) %6d byte(s) %6d byte/frame \xd\xa", 
                        nCount, nSize, nCount ? (int)(nSize/nCount) : 0);
                    nCount = 0; 
                    nSize = 0;
                }
                prevCls = cls; 
                prevAttr = attr;
            }
            if(memcmp(&prevObis, &pData->obis, sizeof(OBISCODE)))
            {
                PRINTOUT("  + %s (%2d) %-18s (%2d)\xd\xa", szObis, cls, dlmsGetInterfaceClass(cls), attr);
                memcpy(&prevObis, &pData->obis, sizeof(OBISCODE));
            }
            nCount++; nSize += nLen;
#ifdef __USE_STDOUT__
            if(!dlms_class_parsing(pToken, &pData->obis, cls, attr, pData->data, nLen))
#else
            if(!dlms_class_parsing(pSession, pToken, &pData->obis, cls, attr, pData->data, nLen))
#endif
            {
#if 0
#ifndef __USE_STDOUT__
                OUTDUMP(pSession, (char *)pData->data, nLen);
#endif
#endif
            }
        }

        nSeek += nLen;
		p += nSeek;
	}

    FREE(pToken);

    if(nSize && nCount)
    {
        if(prevCls != 0 && prevAttr != 0)
        {
            PRINTOUT("      %2d frame(s) %6d byte(s) %6d byte/frame \xd\xa", 
                nCount, nSize, nCount ? (int)(nSize/nCount) : 0);
        }
    }

    PRINTOUT("         TOTAL : %d (%d)\xd\xa", cnt, length);
}

#ifdef __USE_STDOUT__
void ihd_data_parsing(char *mdata, UINT length, BOOL bDetail)
#else
void ihd_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail)
#endif
{
    IHD_METERING_DATA *pIhdData;

    if(length < sizeof(IHD_METERING_DATA)) return;

    pIhdData = (IHD_METERING_DATA *)mdata;

    PRINTOUT("       UP TIME : %d sec\r\n", BigToHostInt(pIhdData->upTime));
    PRINTOUT("  CURRENT TIME : %04d/%02d/%02d %02d:%02d:%02d\r\n", 
            BigToHostShort(pIhdData->currentTime.year),
            pIhdData->currentTime.mon,
            pIhdData->currentTime.day,
            pIhdData->currentTime.hour,
            pIhdData->currentTime.min,
            pIhdData->currentTime.sec
        );
    PRINTOUT("        HW VER : %d.%d\r\n", 
            (pIhdData->hwVer >> 8) & 0xFF,
            pIhdData->hwVer & 0xFF
        );
    PRINTOUT("        SW VER : %d.%d (%d)\r\n", 
            (pIhdData->swVer >> 8) & 0xFF,
            pIhdData->swVer & 0xFF,
            BigToHostShort(pIhdData->swBuild)
        );
    PRINTOUT("  ENERGY LEVEL : %d\r\n", pIhdData->energyLevel);
    PRINTOUT("      FUNCTION : 0x%08X\r\n", pIhdData->functionMask);

}

BOOL NeedAsync(BYTE nAttr)
{
    BOOL bOk = FALSE;
    // RFD가 아니고 Battery Node 일 때 Async
    if (!(nAttr & ENDDEVICE_ATTR_RFD) && (nAttr & ENDDEVICE_ATTR_BATTERYNODE)) bOk = TRUE;
    return bOk;
}

BOOL MakeAsyncParameter(CIF4Invoke * pInvoke, EUI64 * pId, const char * oidStr)
{
    OID3    oid;
    
    VARAPI_StringToOid(oidStr, &oid);

	pInvoke->AddParamFormat("1.14", VARSMI_EUI64, (void *)pId, sizeof(EUI64));
    pInvoke->AddParam("1.10", &oid);
    pInvoke->AddParam("1.4", (BYTE) ASYNC_OPT_RESULT_DATA_SAVE);
    pInvoke->AddParam("1.4", (BYTE) 0);
    pInvoke->AddParam("1.7", (INT) 0);
    pInvoke->AddParam("1.4", (BYTE) 1);

    return TRUE;
}

double GetSignedDouble(UINT value)
{
    UINT v = value << 8;
    int sign, i;
    double f = 0.0;

    //printf("value %08X\r\n", value);
    sign = (v & 0x80000000) ? -1 : 1;
    if(sign<0)
    {
        v^=0xFFFFFF00;
        v+=0x0100;
    }
    for(i=0; (i< 24) && v; i++, v<<=1)
    {
        f +=  (v & 0x80000000) ? pow(2, -(i)) : 0.0;
        //printf("v %08X %c %f\r\n", v, (v & 0x80000000) ? 'T' : 'F' , pow(2, -(i+1)));
    }

    return f * sign;
}

double GetUnsignedDouble(UINT value)
{
    UINT v = value << 8;
    int i;
    double f = 0.0;

    for(i=0; (i< 24) && v; i++, v<<=1)
    {
        f +=  (v & 0x80000000) ? pow(2, -(i+1)) : 0.0;
    }
    return f;
}

#ifdef __USE_STDOUT__
int DisplayCommandResult(IF4Invoke *pInvoke)
#else
int DisplayCommandResult(CLISESSION *pSession, IF4Invoke *pInvoke)
#endif
{
    int     len, i;
    char    buffer[512];

    len =  (int) (pInvoke->pResult[0]->len < (int)sizeof(buffer) ? pInvoke->pResult[0]->len : sizeof(buffer));
    memcpy(buffer, pInvoke->pResult[0]->stream.p, len);

    PRINTOUT("Command Result=[");
    for(i=0;i<len;i++) {
        if(i) PRINTOUT(" ");
        PRINTOUT("%02X", buffer[i]);
    }
    PRINTOUT("]\xd\xa");

    return 0;
}

UINT getParserType(UINT nParserType, char *szModel)
{
    BYTE            parserType = PARSER_TYPE_UNKNOWN;

    if(nParserType != PARSER_TYPE_UNKNOWN) return nParserType;

    if(szModel && *szModel)
    {
        if(!strncasecmp("ADN",szModel,3)) {
            parserType = PARSER_TYPE_AIDON;
        } else if(strstr(szModel,"DETECTOR")) {
            parserType = PARSER_TYPE_SMOKE;
		} else if(strstr(szModel,"ALARM")) {			// Wooks
			parserType = PARSER_TYPE_FIREALARM;
		} else if(!strncasecmp("SM",szModel,2)) {
            parserType = PARSER_TYPE_ANSI;
        } else if(!strncasecmp("K1",szModel,2) || !strncasecmp("K2",szModel,2) 
            || !strncasecmp("K3",szModel,2)) {
            parserType = PARSER_TYPE_KAMST;
        } else if(!strncasecmp("REPEATER",szModel,8)) { 
            parserType = PARSER_TYPE_REPEATER;
        } else if(strstr(szModel,"PULSE") || strstr(szModel,"BATTERY")) {
            parserType = PARSER_TYPE_PULSE;
        } else if(strstr(szModel,"NAMR-H101MG") || strstr(szModel,"MBUS")) {
            parserType = PARSER_TYPE_MBUS;
        } else if(strstr(szModel,"ACD")) {
            parserType = PARSER_TYPE_ACD;
        } else if(strstr(szModel,"NAMR-P111SR") || strstr(szModel, "DLMS")) {
            parserType = PARSER_TYPE_DLMS;
        } else if(strstr(szModel,"IHD")) {
            parserType = PARSER_TYPE_IHD;
        } else if(strstr(szModel,"HMU")) {
            parserType = PARSER_TYPE_HMU;
        } else if(strstr(szModel,"NAMR-P10A") || strstr(szModel, "OSAKI")) {
            parserType = PARSER_TYPE_OSAKI;
        } else if(strstr(szModel,"NAMR-P10B") || strstr(szModel, "SX")) {
            parserType = PARSER_TYPE_SX;
        } else if(!strncasecmp("NAPC-G202SR", szModel, 11) || !strncasecmp("NAPC-G202SR", szModel, 11)) {
            parserType = PARSER_TYPE_KEUKDONG;
        }
    }
    return parserType;
}

const char * getVendorString(BYTE nVendor)
{
    switch (nVendor)
    {
        case METER_VENDOR_KAMSTRUP:     
        case METER_VENDOR_KAMSTRUP_2:       // Issue #977 Kamstrup 신규 검침 Format
        case METER_VENDOR_KAMSTRUP_3:       // Issue #853 Kamstrup Omnipower 신규 검침 Format
                                        return "KAMSTRUP";
        case METER_VENDOR_GE:           return "GE";
        case METER_VENDOR_ELSTER:       return "ELSTER";
        case METER_VENDOR_LANDIS_GYR:   return "LANDIS&GYR";
        case METER_VENDOR_AIDON:        return "AIDON";
        case METER_VENDOR_LSIS:         return "LSIS";
        case METER_VENDOR_WIZIT:        return "WIZIT";
        case METER_VENDOR_ACTARIS:      return "ACTARIS";
        case METER_VENDOR_GASMETER:     return "GAS";
        case METER_VENDOR_EDMI:         return "EDMI";
        case METER_VENDOR_KETI:         return "KETI";
        case METER_VENDOR_NAMJUN:       return "NAMJUN";
        case METER_VENDOR_TAIHAN:       return "TAIHAN";
        case METER_VENDOR_ABB:          return "ABB";
        case METER_VENDOR_KHE:          return "KHE";
        case METER_VENDOR_ILJINE:       return "ILJINE";
        case METER_VENDOR_TGE:          return "TGE";
        case METER_VENDOR_PSTEC:        return "PSTEC";
        case METER_VENDOR_KT:           return "KT";
        case METER_VENDOR_SEOCHANG:     return "SECHANG";
        case METER_VENDOR_CHUNIL:       return "CHUNIL";
        case METER_VENDOR_AMRTECH:      return "AMRTECH";
        case METER_VENDOR_DMPOWER:      return "DMPOWER";
        case METER_VENDOR_AMSTECH:      return "AMRTECH";
        case METER_VENDOR_OMNI:         return "OMNI";
        case METER_VENDOR_MICRONIC:     return "MICRONIC";
        case METER_VENDOR_HYUPSIN:      return "HYUPSIN";
        case METER_VENDOR_MSM:          return "MSM";
        case METER_VENDOR_POWERPLUS:    return "POWERPLUS";
        case METER_VENDOR_YPP:          return "YPP";
        case METER_VENDOR_PYUNGIL:      return "PYUNGIL";
        case METER_VENDOR_AEG:          return "AEG";
        case METER_VENDOR_ANSI:         return "ANSI";
        case METER_VENDOR_SENSUS:       return "SENSUS";
        case METER_VENDOR_ITRON:        return "ITRON";
        case METER_VENDOR_KROMSCHRODER: return "KROMSCHRODER";
        case METER_VENDOR_SIEMENS:      return "SIEMENS";
        case METER_VENDOR_MITSUBISHI:   return "MITSUBISHI";
        case METER_VENDOR_OSAKI:        return "OSAKI";

        case METER_VENDOR_ETC:          return "ETC";
        case METER_VENDOR_NURI:         return "NURI";
    }
    return UNKNOWN_STR;
}

const char * _getKepcoVendorString(BYTE nVendor)
{
    switch (nVendor)
    {
        case 0x01:          return "LSIS";
        case 0x02:          return "PSTEC";
        case 0x03:          return "KT";
        case 0x04:          return "TAIHAN";
        case 0x05:          return "SEOCHANG";
        case 0x06:          return "WIZIT";
        case 0x07:          return "CHUNIL";
        case 0x08:          return "NAMJUN";
        case 0x09:          return "ILJINE";
        case 0x15:          return "AMRTECH";
        case 0x17:          return "TGE";
        case 0x18:          return "DMPOWER";
        case 0x19:          return "AMSTECH";
        case 0x1A:          return "OMNI";
        case 0x1B:          return "MICRONIC";
        case 0x1C:          return "HYUPSIN";
        case 0x1D:          return "MSM";
        case 0x23:          return "POWERPLUS";
        case 0x24:          return "YPP";
        case 0x25:          return "PYUNGIL";
        case 0x2F:          return "NURI";
        case 0x33:          return "AEG";
        case 0x38:          return "KDN";
        case 0x46:          return "GE";
        case 0x4D:          return "DIGITAL(USA)";
        case 0x50:          return "LANDIS&GYR";
        case 0x5A:          return "ETC";

        case 0x0B:          return "LSIS*";
        case 0x0C:          return "PSTEC*";
        case 0x0D:          return "KT*";
        case 0x0E:          return "TAIHAN*";
        case 0x0F:          return "SEOCHANG*";
        case 0x10:          return "WIZIT*";
        case 0x11:          return "CHUNIL*";
        case 0x12:          return "NAMJUN*";
        case 0x13:          return "ILJINE*";
        case 0x16:          return "AMRTECH*";
        case 0x3D:          return "AEG*";
        case 0x4B:          return "GE*";
        case 0x55:          return "LANDIS&GYR*";
        case 0x5F:          return "ETC*";
    }
    return UNKNOWN_STR;
}

const char * _getKepcoAdditionalEquipment(BYTE nValue)
{
    switch (nValue)
    {
        case 0x00:  return "No Setting";
        case 0x04:  return "Remote Load Control";
        case 0x08:  return "Time Switch Control";
        case 0x10:  return "Ampare Limit Control";
    }
    return UNKNOWN_STR;
}

