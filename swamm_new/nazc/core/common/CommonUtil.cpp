#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#ifndef  _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#else
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
#endif
#include <errno.h>
#ifdef _WIN32
  #include <winerror.h>
#endif

#include <ctype.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#if defined(__CYGWIN__)
#include <crypt.h>
#endif

#include "typedef.h"

#include "MemoryDebug.h"
#include "version.h"
#include "CommonUtil.h"

int StrReplace(char *text,const char *src,char *rep, const char *src2, int len);

void USLEEP(unsigned int msec)
{
#ifdef _WIN32
    Sleep(msec / 1000);
#else
    usleep(msec);
#endif
}

cetime_t ceTime(cetime_t *timer)
{
#ifdef _WIN32
	*timer = (GetTickCount() / 1000);
	return *timer;
#else
	return time(timer);
#endif
}

char *MCU_STRDUP(const char *pszString)
{
	char	*pszDest;
	int		len;

	if (!pszString || !*pszString)
		return NULL;

	len = strlen(pszString);
#ifdef __STATIC_LAUNCHER__
	pszDest = (char *)malloc(len+1);
#else
	pszDest = (char *)MALLOC(len+1);
#endif
	if (pszDest == NULL)
		return NULL;
	strcpy(pszDest, pszString);
	return pszDest;
}

/** Value가 NULL 또는 NULL Value 인지 검사
  */
BOOL IsNullValue(const void *pValue)
{
    BYTE *pByte;
    if(pValue == NULL) return TRUE;

    pByte = (BYTE *)pValue;
    if((pByte[0]) == 0x00) return TRUE;
    return FALSE;
}

/** pszFileName이 정규 파일일 때 1을 리턴한다.
  */
int IsExists(const char *pszFileName)
{
	struct  stat file_info;
    int res = 0;

	if(stat(pszFileName, &file_info)) return res;

    if(S_ISREG(file_info.st_mode)) res = 1;

	return res;
}

char *strnstr(char *src, const char *tag, int len)
{
	int		i, j, size;

	if((src == NULL) || (tag == NULL) || (len == 0))	// src, tag, len에 값이 없으면 NULL을 리턴한다.
		return NULL;

	size = strlen(tag);
	j = len;

	for(i=0; i<len; i++)
	{
		if (j<size)			// for문을 수행할 때마다 하나씩 감소하는 j(len의 길이)와  size의 길이를 비교하여
			return NULL;	// len의 길이가 size보다 작으면 NULL을 리턴해준다.

		if (strncmp(src+i, tag, size) == 0)
			return src+i;
		j--;				// j(len의 길이)를 하나씩 감소한다. 
	}
	return NULL;
}

/** 특정 문자열을 받아서 left, right의 white space를 제거해 준다.
  *
  * 원본으로 받은 문자열은 변경되지 않고 새로운 문자열을 malloc 해서 보내주게 되기
  * 때문에 strip을 사용한 코드는 반드시 return 된 pointer를 FREE 해줘야 한다.
  *
  * 만약 pszString이 NULL 이라면 NULL이 리턴된다.
  */
char * strip(const char *pszString)
{
    int i, len, leftIdx=-1, rightIdx=-1;    
    char *res = NULL;

    if(pszString == NULL) return NULL;

	len = strlen(pszString);
	for(i=len-1; i>=0; i--)
	{
		if (!isblank(pszString[i])) 
        {
            rightIdx = i;
            break;
        }
	}	

    for(i=0;i<rightIdx+1;i++) 
    {
        if(!isblank(pszString[i])) 
        {
            leftIdx = i;
            break;
        }
    }

    if(leftIdx < 0 || rightIdx < 0)
    {
        return NULL;
    }

    len = rightIdx - leftIdx + 1;

    if(len <= 0) return NULL;

#ifdef __STATIC_LAUNCHER__
    res = (char *) malloc(len + 1);
#else
    res = (char *) MALLOC(len + 1);
#endif
    memcpy(res, pszString + leftIdx, len);
    res[len] = 0x00;

    return res;
}

/** SRC문자열을 strip 한 후 DST로 copy 한다. 이때 DSTLen - 1 보다 작거나
  * 같은 길이만큼만 복사된다.
  */
char * stripSafeCpy(char *DST, int DSTLen, const char *SRC)
{
    char * stripedStr = strip(SRC);
    int len;

    if(stripedStr == NULL) return NULL;

    len = (int)((int)strlen(stripedStr) < DSTLen - 1 ? strlen(stripedStr) : DSTLen - 1);
    strncpy(DST, stripedStr, len);
    DST[len] = 0x00;

#ifdef __STATIC_LAUNCHER__
    free(stripedStr);
#else
    FREE(stripedStr);
#endif
    return DST;
}

/** SRC문자열을 strip 한 후 DST로 copy 한다. 
  */
char * stripCpy(char *DST, const char *SRC)
{
    return stripSafeCpy(DST, INT_MAX, SRC);
}


int GetFileValue(const char *pszPath, int *pValue)
{
	struct stat statbuf;
    FILE    *fp;
    char    szBuffer[128];

	*pValue = 0;
    fp = fopen(pszPath, "rb");
    if (fp == NULL)
        return 0;

    memset(szBuffer, 0, 128);
    fread(szBuffer, 100, 1, fp);
    fclose(fp);

    *pValue = strtol(szBuffer,(char **)NULL, 10);

	if (*pValue == 0)
		return 0;

	/* pszPath가 정규파일이면 참을 리턴하고 아니면 거짓을 리턴한다. */

	if (stat(pszPath, &statbuf) == -1) // false
		return 0;
	else
		return S_ISREG(statbuf.st_mode);
}

int SetFileValue(const char *pszPath, int nValue)
{
    FILE    *fp;
    char    szBuffer[128];

    fp = fopen(pszPath, "wb");
    if (fp == NULL)
        return 0;

	sprintf(szBuffer, "%0d", nValue);
    fwrite(szBuffer, strlen(szBuffer), 1, fp);
    fclose(fp);

	return 1;
}

int GetFileString(const char *pszPath, char *pszString, int nBuffSize, const char *pszDefault, BOOL bClear)
{
    FILE    *fp;
	int		n = 0, res=0;
	struct  stat file_info;

    /** NULL 까지 생각해서 Length가 2 이상 이어야 한다 */
    if(pszString == NULL || nBuffSize < 2) return 0;

    memset(pszString, 0, nBuffSize);

	if(stat(pszPath, &file_info)) return 0;

    fp = fopen(pszPath, "rb");
    if (fp == NULL)
	{
		if (pszDefault != NULL) {
			n = (int)MIN(nBuffSize - 1, (int)strlen(pszDefault));
			strncpy(pszString, pszDefault, n);
	}
	}
    else 
    {
        n = fread(pszString, 1, nBuffSize - 1, fp);
    fclose(fp);

        if(n > 0 && n == file_info.st_size) 
            res = 1;
    }

	for(;n > 1; n--)
	{
		if (bClear && ((pszString[n-1] == 0xd) || (pszString[n-1] == 0xa)))
			 pszString[n-1] = 0x00;
		else break;
	}

	return res;
}

int SetFileString(const char *pszPath, const char *pszString)
{
    FILE    *fp;

    if(pszString == NULL) return 0;

    fp = fopen(pszPath, "wb");
    if (fp == NULL)
        return 0;

    fwrite(pszString, strlen(pszString), 1, fp);
    fclose(fp);

	return 1;
}

int uptime()
{
	double	stime;
	double	idletime;
	char	buf[36];
	FILE	*fp;

	if ((fp = fopen("/proc/uptime", "r")) == NULL)
		return -1;

	fgets(buf, 36, fp);
	sscanf(buf, "%lf %lf", &stime, &idletime);
	fclose(fp);
	return (int)stime;
}

//////////////////////////////////////////////////////////////////////
// Common Functions
//////////////////////////////////////////////////////////////////////

BOOL EUI64ToStr(const EUI64 *id, char *pszStr)
{
	char	szIds[10];
	int		i, len;

	if (!pszStr || !id)
		return FALSE;

	len = (int)sizeof(EUI64);
	*pszStr = '\0';
	for(i=0; i<len; i++)
	{
        memset(szIds, 0, sizeof(szIds));
		sprintf(szIds, "%02X", (id->ids[i] & 0xFF));
		strcat(pszStr, szIds);
	}	
    return TRUE;
}

BOOL StrToEUI64(const char *pszStr, EUI64 *id)
{
    int     i;
    long    res;
    char    szBuffer[10];
    char    *endptr = NULL;
    char    *pszSp;

    if (!pszStr || !id)
        return FALSE;

    if(strlen(pszStr) < 16) 
        return FALSE;

    pszSp = const_cast<char *>(pszStr);
    for(i=0; i<8; i++, endptr=NULL)
    {
        memcpy(szBuffer, &pszSp[i*2], 2);
        szBuffer[2] = '\0';
        res = strtol(szBuffer, &endptr, 16);
        if((res == LONG_MAX || res == LONG_MIN) && errno == ERANGE) {
            return FALSE;
        }
        if(*endptr) return FALSE;
        id->ids[i] = (BYTE)res;
    }
    return TRUE;
}

BOOL ReverseEUI64(EUI64 *pSource, EUI64 *pTarget)
{
    int i;

    if (!pSource || !pTarget)
        return FALSE;

    for(i=0;i<8;i++)
    {
        pTarget->ids[i] = pSource->ids[7-i];
    }
    return TRUE;
}

/** WORD24 type을 UINT로 mapping 해 준다.
  */
UINT Word24ToUint(WORD24 nValue)
{
#ifdef  __BIG_ENDIAN_SYSTEM__
    return (nValue.v[0] << 16) | (nValue.v[1] << 8) | nValue.v[2];
#else
    return (nValue.v[2] << 16) | (nValue.v[1] << 8) | nValue.v[0];
#endif
}

void UintToWord24(UINT nValue, WORD24 *nRetValue)
{
    if(nRetValue == NULL) return;
    
#ifdef  __BIG_ENDIAN_SYSTEM__
    nRetValue->v[0] = (nValue >> 16) & 0xFF;
    nRetValue->v[1] = (nValue >> 8) & 0xFF;
    nRetValue->v[2] = (nValue) & 0xFF;
#else
    nRetValue->v[2] = (nValue >> 16) & 0xFF;
    nRetValue->v[1] = (nValue >> 8) & 0xFF;
    nRetValue->v[0] = (nValue) & 0xFF;
#endif
}

void BigToHostWord24(WORD24 nValue, WORD24 *nRetValue)
{
    if(nRetValue == NULL) return;
#ifdef  __BIG_ENDIAN_SYSTEM__
    nRetValue->v[0] = nValue.v[0];
    nRetValue->v[1] = nValue.v[1];
    nRetValue->v[2] = nValue.v[2];
#else
    nRetValue->v[0] = nValue.v[2];
    nRetValue->v[1] = nValue.v[1];
    nRetValue->v[2] = nValue.v[0];
#endif
}

unsigned short BigToHostShort(unsigned short nValue)
{
	return ntohs(nValue);
}

unsigned int BigToHostInt(unsigned int nValue)
{
	return ntohl(nValue);
}

#ifdef  __BIG_ENDIAN_SYSTEM__
#define ntohll(x) x
#else
#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )
#endif

#define htonll(x) ntohll(x)

unsigned long long BigToHostLong(unsigned long long nValue)
{
	return ntohll(nValue);
}

float BigToHostFloat(float fValue)
{
#ifdef  __BIG_ENDIAN_SYSTEM__
    return fValue;
#else
    int i,len = sizeof(float);
    char szFValue[len];
    char szFRes[len];
    float fRes;

    memcpy(szFValue, &fValue, len);
    for(i=0;i<len;i++)
    {
        szFRes[len-1-i] = szFValue[i];
    }
    memcpy(&fRes, szFRes, len);

    return fRes;
#endif
}

TIMESTAMP * HostToBigTimestamp(TIMESTAMP *host, TIMESTAMP *big)
{
    if(host == NULL || big == NULL) return NULL;
    memcpy(big, host, sizeof(TIMESTAMP));
    big->year = HostToBigShort(host->year);
    return big;
}

TIMESTAMP * HostToLittleTimestamp(TIMESTAMP *host, TIMESTAMP *little)
{
    if(host == NULL || little == NULL) return NULL;
    memcpy(little, host, sizeof(TIMESTAMP));
    little->year = HostToLittleShort(host->year);
    return little;
}

TIMESTAMP * BigToHostTimestamp(TIMESTAMP *big, TIMESTAMP *host)
{
    if(host == NULL || big == NULL) return NULL;
    memcpy(host, big, sizeof(TIMESTAMP));
    host->year = BigToHostShort(big->year);
    return host;
}

TIMESTAMP * LittleToHostTimestamp(TIMESTAMP *little, TIMESTAMP *host)
{
    if(host == NULL || little == NULL) return NULL;
    memcpy(host, little, sizeof(TIMESTAMP));
    host->year = LittleToHostShort(little->year);
    return host;
}

/** 임의의 Big Endian Data 열에서 Long value를 뽑아낸다.
 */
unsigned long long BigStreamToHostLong(BYTE *szStream, int nLength)
{
    unsigned long long value = 0;
#ifdef __LITTLE_ENDIAN_SYSTEM__
    int i;
    BYTE buff[8];
#endif

    if(szStream == NULL || nLength < 0 || nLength > 8) return 0;

#ifdef __BIG_ENDIAN_SYSTEM__
    memcpy(&value + (8-nLength), szStream, nLength);
#else
    memset(buff, 0, sizeof(buff));
    for(i=0;i<nLength;i++)
    {
        buff[i] = szStream[nLength-i-1];
    }
    memcpy(&value, buff, nLength);
#endif

    return value;
}

/** 임의의 Little Endian Data 열에서 Long value를 뽑아낸다.
 */
unsigned long long LittleStreamToHostLong(BYTE *szStream, int nLength)
{
    unsigned long long value = 0;
#ifdef __BIG_ENDIAN_SYSTEM__
    int i;
    BYTE buff[8];
#endif

    if(szStream == NULL || nLength < 0 || nLength > 8) return 0;

#ifdef __BIG_ENDIAN_SYSTEM__
    memset(buff, 0, sizeof(buff));
    for(i=0;i<nLength;i++)
    {
        buff[nLength-i-1] = szStream[i];
    }
    memcpy(&value + (8-nLength), szStream, nLength)
#else
    memcpy(&value, szStream, nLength);
#endif

    return value;
}

unsigned long long CondStreamToHostLong(ORDER order, BYTE *szStream, int nLength)
{
    if(order == ORDER_LSB) return LittleStreamToHostLong(szStream, nLength);
    else return BigStreamToHostLong(szStream, nLength);
}

BYTE* HostIntToBigStream(unsigned int nVal, BYTE *szStream)
{
    if(szStream == NULL) return NULL;

    szStream[0] = (nVal >> 24) & 0xFF;
    szStream[1] = (nVal >> 16) & 0xFF;
    szStream[2] = (nVal >> 8) & 0xFF;
    szStream[3] = nVal & 0xFF;

    return szStream;
}

BYTE* HostIntToLittleStream(unsigned int nVal, BYTE *szStream)
{
    if(szStream == NULL) return NULL;

    szStream[3] = (nVal >> 24) & 0xFF;
    szStream[2] = (nVal >> 16) & 0xFF;
    szStream[1] = (nVal >> 8) & 0xFF;
    szStream[0] = nVal & 0xFF;

    return szStream;
}

#if 0
unsigned int BYTE2UINT3(BYTE * intStr)
{
    unsigned int v = 0;

    if(!intStr) return v;

    v |= intStr[0]; v <<= 8;
    v |= intStr[1]; v <<= 8;
    v |= intStr[2]; v <<= 8;

    return v;
}

INT BYTE2INT3(BYTE * intStr)
{
    /** @todo 이 함수는 수정되어야 한다.
      * 음수일 경우 2의 보수를 구해야 한다
      */
    INT v = 0;
    int sign = 1;
    BYTE vStr[3];

    if(!intStr) return v;

    memcpy(vStr, intStr, 3);

    if(vStr[0] & 0x80)
    {
        sign = -1;
        vStr[0] &= 0x7F;
    }

    v |= vStr[0]; v <<= 8;
    v |= vStr[1]; v <<= 8;
    v |= vStr[2]; v <<= 8;

    return v * sign;
}
#endif

unsigned short LittleToHostShort(unsigned short nValue)
{
#ifdef  __BIG_ENDIAN_SYSTEM__
    return (nValue >> 8) | (nValue << 8);
#else
    return nValue;
#endif
}

unsigned int LittleToHostInt(unsigned int nValue)
{
#ifdef  __BIG_ENDIAN_SYSTEM__
    return (nValue >> 24) | ((nValue << 8) & 0x00FF0000) |
          ((nValue >> 8) & 0x0000FF00) | (nValue << 24);
#else
    return nValue;
#endif
}

unsigned long long LittleToHostLong(unsigned long long nValue)
{
#ifdef  __BIG_ENDIAN_SYSTEM__
    return (nValue >> 56) | ((nValue >> 40) & 0x000000000000FF00LLU) | 
        ((nValue >> 24) & 0x0000000000FF0000LLU) | ((nValue >> 8) & 0x00000000FF000000LLU) | 
        (nValue << 56) | ((nValue << 40) & 0x00FF000000000000LLU) | ((nValue << 24) & 0x0000FF0000000000LLU) | 
        ((nValue << 8) & 0x000000FF00000000LLU);
#else
    return nValue;
#endif
}

void LittleToHostWord24(WORD24 nValue, WORD24 *nRetValue)
{
    if(nRetValue == NULL) return;
#ifdef  __BIG_ENDIAN_SYSTEM__
    nRetValue->v[0] = nValue.v[2];
    nRetValue->v[1] = nValue.v[1];
    nRetValue->v[2] = nValue.v[0];
#else
    nRetValue->v[0] = nValue.v[0];
    nRetValue->v[1] = nValue.v[1];
    nRetValue->v[2] = nValue.v[2];
#endif
}

unsigned short CondToHostShort(ORDER order, unsigned short nValue)
{
    if(order == ORDER_LSB) return LittleToHostShort(nValue);
    else return BigToHostShort(nValue);
}

unsigned int CondToHostInt(ORDER order, unsigned int nValue)
{
    if(order == ORDER_LSB) return LittleToHostInt(nValue);
    else return BigToHostInt(nValue);
}

unsigned long long CondToHostLong(ORDER order, unsigned long long nValue)
{
    if(order == ORDER_LSB) return LittleToHostLong(nValue);
    else return BigToHostLong(nValue);
}

void CondToHostWord24(ORDER order, WORD24 nValue, WORD24 *nRetValue)
{
    if(nRetValue == NULL) return;
    if(order == ORDER_LSB) return LittleToHostWord24(nValue, nRetValue);
    else return BigToHostWord24(nValue, nRetValue);
}

/////////////////////////////////////////////////////////////////////////////////////////
// TIME Functions                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////
TIMETICK *GetTimeTick(TIMETICK *tick)
{
	if (tick == NULL)
		return NULL;

	memset(tick, 0, sizeof(TIMETICK));
#if defined(_WIN32) 
	tick->t = GetTickCount();
#else
    gettimeofday((struct timeval *)&tick->t, NULL);
#endif
	return tick;
}

/** TimeStamp를 이용해서 cetime_t 값을 얻어온다.
 */
cetime_t MakeTime(TIMESTAMP *pStamp)
{
    struct tm stm;
    if(pStamp == NULL) return 0;

    memset(&stm, 0, sizeof(struct tm));
    stm.tm_year = pStamp->year - 1900;
    stm.tm_mon = pStamp->mon - 1;
    stm.tm_mday = pStamp->day;
    stm.tm_hour = pStamp->hour;
    stm.tm_min = pStamp->min;
    stm.tm_sec = pStamp->sec;

    return (cetime_t)mktime(&stm);
}

/** TIMESTAMP를 이용한 초단위의 시간차 계산.
 */
int GetTimestampInterval(TIMESTAMP *pPrev, TIMESTAMP *pCurrent)
{
    cetime_t nPre, nCurr;

    nPre = MakeTime(pPrev);
    nCurr = MakeTime(pCurrent);
	
	return (int)(nCurr - nPre);
}

/** Millsec 단위의 시간차 계산.
 */
int GetTimeInterval(TIMETICK *pPrev, TIMETICK *pCurrent)
{
	int		nInterval;
#if defined(_WIN32)
    nInterval = pCurrent->t - pPrev->t;
#else
	double	fCurrent, fPrevious;

	fCurrent    = (pCurrent->t.tv_sec * 1000.0) + (pCurrent->t.tv_usec / 1000.0);
	fPrevious	= (pPrev->t.tv_sec * 1000.0) + (pPrev->t.tv_usec / 1000.0);
	nInterval	= (int)(fCurrent - fPrevious);
#endif
	
	return nInterval;
}

int GetTimevalInterval(struct timeval *pPrev, struct timeval *pCurrent)
{
    double  fCurrent, fPrevious;
    int     nInterval;

    fCurrent    = (pCurrent->tv_sec * 1000.0) + (pCurrent->tv_usec / 1000.0);
    fPrevious   = (pPrev->tv_sec * 1000.0) + (pPrev->tv_usec / 1000.0);
    nInterval   = (int)(fCurrent - fPrevious);

    return nInterval;
}

void GetGmtTimestamp(GMTTIMESTAMP *pTime)
{
	time_t		now;
	struct	tm	when;

	time(&now);
	when = *localtime(&now);

#ifdef  __USE_BSD
	pTime->tz	= (unsigned short)(when.tm_gmtoff/60);
#endif
	pTime->dst	= 0;
    pTime->year = when.tm_year + 1900;
    pTime->mon  = when.tm_mon + 1;
    pTime->day  = when.tm_mday;
    pTime->hour = when.tm_hour;
    pTime->min  = when.tm_min;
    pTime->sec  = when.tm_sec;
}

TIMESTAMP* GetTimestamp(TIMESTAMP *pStamp, cetime_t *pNow)
{
	time_t      	now;
	struct  tm		when;

	if (!pStamp)
		return NULL;

	if (pNow == NULL)
	{
		time(&now);
		pNow = (cetime_t *)&now;
	}
	else
	{
		if (*pNow == 0)
		{
			memset(pStamp, 0, sizeof(TIMESTAMP));
			return pStamp;
		}
	}
	when 			= *localtime(pNow);
	pStamp->year	= when.tm_year + 1900;
	pStamp->mon		= when.tm_mon + 1;
	pStamp->day		= when.tm_mday;
	pStamp->hour	= when.tm_hour;
	pStamp->min		= when.tm_min;
	pStamp->sec		= when.tm_sec;

    return pStamp;
}

BOOL Timestamp5ToTimestamp(TIMESTAMP5 *p5Stamp, TIMESTAMP *pStamp)
{
    if(pStamp == NULL || p5Stamp == NULL) return FALSE;

    pStamp->year = p5Stamp->year + 2000;
    pStamp->mon = p5Stamp->mon;
    pStamp->day = p5Stamp->day;
    pStamp->hour = p5Stamp->hour;
    pStamp->min = p5Stamp->min;
    pStamp->sec = 0;

    return TRUE;
}

BOOL TimestampToTimestamp5(TIMESTAMP *pStamp, TIMESTAMP5 *p5Stamp)
{
    if(pStamp == NULL || p5Stamp == NULL) return FALSE;

    p5Stamp->year = pStamp->year - 2000;
    p5Stamp->mon = pStamp->mon;
    p5Stamp->day = pStamp->day;
    p5Stamp->hour = pStamp->hour;
    p5Stamp->min = pStamp->min;

    return TRUE;
}

BOOL Timestamp6ToTimestamp(TIMESTAMP6 *p6Stamp, TIMESTAMP *pStamp)
{
    if(pStamp == NULL || p6Stamp == NULL) return FALSE;

    pStamp->year = p6Stamp->year + 2000;
    pStamp->mon = p6Stamp->mon;
    pStamp->day = p6Stamp->day;
    pStamp->hour = p6Stamp->hour;
    pStamp->min = p6Stamp->min;
    pStamp->sec = p6Stamp->sec;;

    return TRUE;
}

BOOL TimestampToTimestamp6(TIMESTAMP *pStamp, TIMESTAMP6 *p6Stamp)
{
    if(pStamp == NULL || p6Stamp == NULL) return FALSE;

    p6Stamp->year = pStamp->year - 2000;
    p6Stamp->mon = pStamp->mon;
    p6Stamp->day = pStamp->day;
    p6Stamp->hour = pStamp->hour;
    p6Stamp->min = pStamp->min;
    p6Stamp->sec = pStamp->sec;;

    return TRUE;
}

void MakeTimeString(char *pszTimeString, cetime_t *now, BOOL bPrintable)
{
    struct  tm  when;

    if (!pszTimeString)
        return;

    when = *localtime(now);
	if (bPrintable)
	{
		if (now && (*now == 0))
		{
			sprintf(pszTimeString, "0000/00/00 00:00:00");
			return;
		}

    	sprintf(pszTimeString, "%04d/%02d/%02d %02d:%02d:%02d",
            when.tm_year + 1900,
            when.tm_mon + 1,
            when.tm_mday,
            when.tm_hour,
            when.tm_min,
            when.tm_sec);
	}
	else
	{
		if (now && (*now == 0))
		{
			sprintf(pszTimeString, "00000000000000");
			return;
		}

    	sprintf(pszTimeString, "%04d%02d%02d%02d%02d%02d",
            when.tm_year + 1900,
            when.tm_mon + 1,
            when.tm_mday,
            when.tm_hour,
            when.tm_min,
            when.tm_sec);
	}	
}

void TimeString(char *pszTimeString)
{
#ifdef _WIN32
	SYSTEMTIME	now;

    if (!pszTimeString)
        return;

	GetLocalTime(&now);
   	sprintf(pszTimeString, "%02d:%02d:%02d",
			now.wHour, now.wMinute, now.wSecond);
#else
    struct  tm  when;
	time_t	now;	

    if (!pszTimeString)
        return;

	time(&now);
    when = *localtime(&now);
   	sprintf(pszTimeString, "%02d:%02d:%02d",
            when.tm_hour,
            when.tm_min,
            when.tm_sec);
#endif
}

void TimeStringMilli(char *pszTimeString)
{
#ifdef _WIN32
	SYSTEMTIME	now;

    if (!pszTimeString)
        return;

	GetLocalTime(&now);
   	sprintf(pszTimeString, "%02d:%02d:%02d.%03d",
			now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
#else
	struct	timeval	cur;
	struct	tm		when;

    if (!pszTimeString)
        return;

	gettimeofday(&cur, NULL);
	when = *localtime(&cur.tv_sec);
	
   	sprintf(pszTimeString, "%02d:%02d:%02d.%03d",
			when.tm_hour, when.tm_min,
			when.tm_sec, (unsigned int)cur.tv_usec/1000);
#endif
}

/** SX Meter 등에서 사용되는 HEXA-ASCII 형태의 Time String을 cetime_t 형으로 변환한다.
 *
 * @param szHexaTimeString 'YYMMDDHHmmss' 형태로 전달되어야 하며 Hexa format 이어야 한다
 * @param pTimeT cetime_t 형 pointer로 전달되어야 한다
 * @return 정상일 경우 parameter로 전달된 cetime_t pointer가 전달되고 비정상 상황일 경우
 *      NULL 이 전달된다.
 */
cetime_t* ConvHexaTimeStr2TimeT(const char *szHexaTimeString, cetime_t *pTimeT)
{
    int Y=0,M=0,D=0,h=0,m=0,s=0;
    struct tm stm;

    /** Error Check */
	if (szHexaTimeString == NULL) return NULL;
	if (pTimeT == NULL) return NULL;

    sscanf(szHexaTimeString,"%02X%02X%02X%02X%02X%02X",
            &Y, &M, &D, &h, &m, &s);

    memset(&stm, 0, sizeof(struct tm));
    stm.tm_year = Y + 100;
    stm.tm_mon = M - 1;
    stm.tm_mday = D;
    stm.tm_hour = h;
    stm.tm_min = m;
    stm.tm_sec = s;

    *pTimeT = (cetime_t)mktime(&stm);

    return pTimeT;
}

/** cetime_t형 Data를 SX Meter 등에서 사용되는 HEXA-ASCII 형태로 변환한다.
 *
 * @param timeT cetime_t 형 시간 정보
 * @param szHexaTimeString 'YYMMDDHHmmss' 형태로 시간 값이 전달되어야 하는 문자열.
 * @return 정상일 경우 parameter로 전달된 string pointer가 전달되고 비정상 상황일 경우
 *      NULL 이 전달된다.
 */
char* ConvTimeT2HexaTimeStr(cetime_t timeT, char *szHexaTimeString)
{
	struct tm when;

    /** Error Check */
	if (szHexaTimeString == NULL) return NULL;

	when = *localtime(&timeT);

    sprintf(szHexaTimeString, "%02X%02X%02X%02X%02X%02X",
        when.tm_year > 100 ? when.tm_year - 100 : 0,
        when.tm_mon + 1,
        when.tm_mday,
        when.tm_hour,
        when.tm_min,
        when.tm_sec);

    return szHexaTimeString;
}


/////////////////////////////////////////////////////////////////////////////////////////

int ChangNdmIP(char *pszPath, char *pszString)
{
	FILE *fp, *fa;
	struct  stat file_info;
	char *pszTemp=NULL;
//	int i, b;
	char *pTemp;

	fp = fopen(pszPath, "r+");
	if (fp == NULL) return -2;

	stat(pszPath, &file_info);
#ifdef __STATIC_LAUNCHER__
	pszTemp = (char *)malloc((int)file_info.st_size);
#else
	pszTemp = (char *)MALLOC((int)file_info.st_size);
#endif
	if (pszTemp == NULL) 
    {
        fclose(fp);
        return -1;
    }

#ifdef __STATIC_LAUNCHER__
	pTemp = (char *)malloc((int)strlen(pszString)+7);
#else
	pTemp = (char *)MALLOC((int)strlen(pszString)+7);
#endif
	if (pTemp == NULL) 
    {
        fclose(fp);
#ifdef __STATIC_LAUNCHER__
        free(pszTemp);
#else
        FREE(pszTemp);
#endif
        return -1;
    }
	sprintf(pTemp, "http://%s", pszString);

	fread(pszTemp, file_info.st_size, 1, fp);
	StrReplace(pszTemp, "http", pTemp, ":8085", file_info.st_size);

	fclose(fp);
	fa=fopen("temp", "w+");
	fwrite(pszTemp, strlen(pszTemp), 1, fa);
#ifdef __STATIC_LAUNCHER__
	if(pszTemp) free(pszTemp);
	if(pTemp) free(pTemp);
#else
	if(pszTemp) FREE(pszTemp);
	if(pTemp) FREE(pTemp);
#endif
	fclose(fa);
	system("mv temp ndm_tree.xml");
	return 1;
}

int StrReplace(char *text,const char *src,char *rep, const char *src2, int len)
{
  char *pFound=NULL, *pFound2=NULL;
  char *temp=NULL;
#ifdef __STATIC_LAUNCHER__
  temp = (char *)malloc((int)len);
#else
  temp = (char *)MALLOC((int)len);
#endif

  pFound2=strstr(text, src2);

  if ( (pFound = strstr(text, src)) == NULL )
  {
#ifdef __STATIC_LAUNCHER__
        free(temp);
#else
        FREE(temp);
#endif
        return 2;
  }

  strcpy(temp, text);
  temp[pFound-text] = '\0';
  strcat(temp, rep);
  strcat(temp, pFound2);
  strcpy(text, temp);
  return 0;
}

int GetVersionString(char *szVersion)
{
    char m_szMcuType[64] = {0,};

    GetFileString("/app/conf/mcusystype", m_szMcuType, sizeof(m_szMcuType), "OUTDOOR", TRUE);
#ifdef __ENABLE_SW_BUILD__
	sprintf(szVersion, "%s-%s-%s(%s)-%s",
            m_szMcuType, HW_VERSION_FIX, SW_VERSION_FIX, SW_BUILD, SW_PREFIX_TYPE);
#else
// sy 151013
//	sprintf(szVersion, "%s-%s-%s-%s",m_szMcuType, HW_VERSION_FIX, SW_VERSION_FIX, SW_PREFIX_TYPE);
#endif
    return 0;
}
UINT ReturnFileLength(char *pszfile)
{
    FILE *fp;
    UINT ret;

    fp = fopen( pszfile, "r" ) ; 
    fseek(fp , 0l , SEEK_END );  
    ret = ftell(fp);
    fclose(fp);
    return ret;
}

#ifndef _WIN32
// crypt에서 사용될 salt 값을 얻어온다.
char *GetSalt(char *pszSalt)
{
    const char *salt_set="./abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                     "0123456789";

    if(!pszSalt) return NULL;

    memset(pszSalt, 0, 3);

    pszSalt[0] = salt_set[random() % strlen(salt_set)];
    pszSalt[1] = salt_set[random() % strlen(salt_set)];

    return pszSalt;
}

char *ExtractSalt(const char *pszEncryptedPasswd, char *pszSalt)
{
    if(!pszSalt || !pszEncryptedPasswd) return NULL;

    memset(pszSalt, 0, 3);

    pszSalt[0] = pszEncryptedPasswd[0];
    pszSalt[1] = pszEncryptedPasswd[1];

    return pszSalt;
}

// Password는 crypt 함수로 암호화 하여 저장된다. 2009.11.04
char *SetUserKey(const char *pszKey, const char *pszSalt)
{
#ifdef __STATIC_LAUNCHER__
    return const_cast<char *>(pszKey);
#else
    return crypt(pszKey, pszSalt);
#endif
}
#endif

/** source에서 dest로 파일을 복사해준다.
  *
  * @return error code
  *     - 1 : null parameter
  *     - 2 : file open fail
  */
int CopyFile(const char *source, const char *dest)
{
    FILE *fs, *fd;
    int ch;

    if(!source || !dest) return 1;

    if((fs=fopen(source,"r"))==NULL)
    {
        return 2;
    }
    if((fd=fopen(dest,"w"))==NULL)
    {
        fclose(fs);
        return 2;
    }

    while(1)
    {
        if((ch=getc(fs)) == EOF)
        {
            break;
        }
        putc(ch,fd);
    }

    fclose(fd);
    fclose(fs);

    return 0;
}

float BigOctetToHostFloat(char *szOctet)
{
    char    szBuffer[4];
    float   *pFValue;

    if(!szOctet) return 0.0;

#ifndef  __BIG_ENDIAN_SYSTEM__
    szBuffer[0] = szOctet[3];
    szBuffer[1] = szOctet[2];
    szBuffer[2] = szOctet[1];
    szBuffer[3] = szOctet[0];
#else
    memcpy(szBuffer, szOctet, 4);
#endif
    pFValue = (float *)szBuffer;
    return *pFValue;
 
}

BYTE* ReverseBcd(BYTE *szBcd, BYTE *szDest, int nLen)
{
    int i;

    if(!szBcd || !szDest) return szDest;
    for(i=0;i<nLen;i++)
    {
        szDest[nLen-i-1] = szBcd[i];
    }
    return szDest;
}

/** source 의 Byte의 bit 값들을 reverse 한다.
 *
 */
BYTE ReverseByte(BYTE c)
{
    c = (c & 0x0F) << 4 | (c & 0xF0) >> 4;
    c = (c & 0x33) << 2 | (c & 0xCC) >> 2;
    c = (c & 0x55) << 1 | (c & 0xAA) >> 1;

    return c;
}

unsigned long long int BcdToInt(BYTE *szBcd, WORD len)
{
    unsigned long long int res = 0;
    BYTE v;
    int i;

    if(!szBcd) return res;
    for(i=0;i<len;i++)
    {
        v = szBcd[i];
        res *= 100;
        res += (((v >> 4) % 10) * 10) + ((v & 0x0F) % 10);
    }

    return res;
}

unsigned long long int AsciiToInt(BYTE *szAscii, WORD len)
{
    unsigned long long int res = 0;
    BYTE v;
    int i;

    if(!szAscii) return res;
    for(i=0;i<len;i++)
    {
        if(!isdigit(szAscii[i])) v = 0;
        else v = szAscii[i] - 0x30;
        res *= 10;
        res += v;
    }

    return res;
}



