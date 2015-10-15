#ifndef __COMMON_UTIL_H__
#define __COMMON_UTIL_H__

#include "typedef.h"

void USLEEP(unsigned int msec);
char *MCU_STRDUP(const char *pszString);

BOOL IsNullValue(const void *pValue);
int IsExists(const char *pszFileName);
char *strnstr(char *src, const char *tag, int len);
char * strip(const char *pszString);
char * stripCpy(char *DST, const char *SRC);
char * stripSafeCpy(char *DST, int DSTLen, const char *SRC);
int GetFileValue(const char *pszPath, int *pValue);
int SetFileValue(const char *pszPath, int nValue);
int GetFileString(const char *pszPath, char *pszString, int nBuffSize, const char *pszDefault=NULL, BOOL bClear=FALSE);
int SetFileString(const char *pszPath, const char *pszString);
int GetVersionString(char *szVersionString);
cetime_t ceTime(cetime_t *timer);
int uptime();

BOOL EUI64ToStr(const EUI64 *id, char *pszStr);
BOOL StrToEUI64(const char *pszStr, EUI64 *id);

BOOL ReverseEUI64(EUI64 *pSource, EUI64 *pTarget);

/** Ordering Functions */
WORD BigToHostShort(unsigned short nValue);
UINT BigToHostInt(unsigned int nValue);
unsigned long long BigToHostLong(unsigned long long nValue);
void BigToHostWord24(WORD24 nValue, WORD24 *nRetValue);
//float ORDERBYFLOAT(float fValue);

#define HostToBigShort(x)      BigToHostShort(x)
#define HostToBigInt(x)        BigToHostInt(x)
#define HostToBigLong(x)       BigToHostLong(x)
#define HostToBigWord24(x,y)   BigToHostWord24(x,y)

WORD LittleToHostShort(unsigned short nValue);
UINT LittleToHostInt(unsigned int nValue);
unsigned long long LittleToHostLong(unsigned long long nValue);
void LittleToHostWord24(WORD24 nValue, WORD24 *nRetValue);

#define HostToLittleShort(x)      LittleToHostShort(x)
#define HostToLittleInt(x)        LittleToHostInt(x)
#define HostToLittleLong(x)       LittleToHostlong(x)
#define HostToLittleWord24(x,y)   LittleToHostWord24(x,y)

WORD CondToHostShort(ORDER order, unsigned short nValue);
UINT CondToHostInt(ORDER order, unsigned int nValue);
unsigned long long CondToHostLong(ORDER order, unsigned long long nValue);
void CondToHostWord24(ORDER order, WORD24 nValue, WORD24 *nRetValue);

#define HostToCondShort(o,x)      CondToHostShort(o,x)
#define HostToCondInt(o,x)        CondToHostInt(o,x)
#define HostToCondLong(o,x)       CondToHostlong(o,x)
#define HostToCondWord24(o,x,y)   CondToHostWord24(o,x,y)

TIMESTAMP * HostToBigTimestamp(TIMESTAMP *host, TIMESTAMP *big);
TIMESTAMP * HostToLittleTimestamp(TIMESTAMP *host, TIMESTAMP *little);
TIMESTAMP * BigToHostTimestamp(TIMESTAMP *big, TIMESTAMP *host);
TIMESTAMP * LittleToHostTimestamp(TIMESTAMP *little, TIMESTAMP *host);

BYTE* HostIntToBigStream(unsigned int nVal, BYTE *szStream);
BYTE* HostIntToLittleStream(unsigned int nVal, BYTE *szStream);

unsigned long long BigStreamToHostLong(BYTE *szStream, int nLength);
unsigned long long LittleStreamToHostLong(BYTE *szStream, int nLength);
unsigned long long CondStreamToHostLong(ORDER order, BYTE *szStream, int nLength);

int	CalcBaudrate(int nBaud);

TIMETICK *GetTimeTick(TIMETICK *tick);
int GetTimeInterval(TIMETICK *pPrev, TIMETICK *pCurrent);
int GetTimevalInterval(struct timeval *pPrev, struct timeval *pCurrent);

cetime_t MakeTime(TIMESTAMP *pStamp);
int GetTimestampInterval(TIMESTAMP *pPrev, TIMESTAMP *pCurrent);
void GetGmtTimestamp(GMTTIMESTAMP *pTime);
void TimeString(char *pszTimeString);
void TimeStringMilli(char *pszTimeString);
TIMESTAMP* GetTimestamp(TIMESTAMP *pStamp, cetime_t *pNow);
void MakeTimeString(char *pszTimeString, cetime_t *now, BOOL bPrintable=FALSE);

BOOL Timestamp5ToTimestamp(TIMESTAMP5 *p5Stamp, TIMESTAMP *pStamp);
BOOL TimestampToTimestamp5(TIMESTAMP *pStamp, TIMESTAMP5 *p5Stamp);
BOOL Timestamp6ToTimestamp(TIMESTAMP6 *p6Stamp, TIMESTAMP *pStamp);
BOOL TimestampToTimestamp6(TIMESTAMP *pStamp, TIMESTAMP6 *p6Stamp);

cetime_t* ConvHexaTimeStr2TimeT(const char *szHexaTimeString, cetime_t *pTimeT);
char* ConvTimeT2HexaTimeStr(cetime_t timeT, char *szHexaTimeString);

int ChangNdmIP(char *pszPath, char *pszString);
int StrReplace(char *text,char *src,char *rep, char *src2, int len);
UINT ReturnFileLength(char *pszfile);

float BigOctetToHostFloat(char *szOctet);

UINT Word24ToUint(WORD24 nValue);
void UintToWord24(UINT nValue, WORD24 *nRetValue);

BYTE* ReverseBcd(BYTE *szBcd, BYTE *szDest, int nLen);
unsigned long long int BcdToInt(BYTE *szBcd, WORD len);
unsigned long long int AsciiToInt(BYTE *szAscii, WORD len);

#ifndef _WIN32
char *GetSalt(char *pszSalt);
char *ExtractSalt(const char *pszEncryptedPasswd, char *pszSalt);
char *SetUserKey(const char *pszKey, const char *pszSalt);
#endif

int CopyFile(const char *source, const char *dest);
#endif	// __COMMON_UTIL_H__
