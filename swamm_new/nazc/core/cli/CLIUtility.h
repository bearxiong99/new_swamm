#ifndef __CLI_UTILITY_H__
#define __CLI_UTILITY_H__

void MKPRINTDATE(char *pszTarget, char *pszSource);
void MKTIMESTR(char *pszTimeString);
void MKPRINTTIMESTR(char *pszTimeString, time_t *now);
void TIMESTAMP2STR(char *pszString, TIMESTAMP *pTime, BOOL bPrintable);
void OUTTEXT(CLISESSION *pSession, const char *fmt, ...);
void OUTDUMP(CLISESSION *pSession, char *pszBuffer, int nLength, BOOL bPrintAscii=FALSE);
void PUT(CLISESSION *pSession, char *pszBuffer, int nLength);
void PRINT(CLISESSION *pSession, char *pszBuffer);

void do_not_support(CLISESSION *pSession);
BOOL GenericInvoke(CLISESSION *pSession, CIF4Invoke *pInvoke);
void display_help(CLISESSION *pSession, CLIHANDLER *pList, int nDepts, BOOL bFirstOnly=FALSE);
void ReportError(CLISESSION *pSession, int nError, int nResult=0);
BOOL Confirm(CLISESSION *pSession, const char *pszMessage);
int More(CLISESSION *pSession, int nCount, int nCurrent);

BOOL GetFirmwareFileName(const char *pszPath, char *pszFileName);
void GetFileData(char *pszBuffer, char *pszFileName, int nLength);
void GetFileDataEx(char *pszBuffer, char *pszFileName, int nLength);
void SetFileData(char *pszFileName, char *pszBuffer, int nLength);
void AddString(char *pszString, char *szAdd);

int SIGNED(unsigned char n);
void SENSOREUI64STR(char *pszGUID);

unsigned short ORDERBYSHORT(unsigned short nValue);
unsigned int ORDERBYINT(unsigned int nValue);

#endif	// __CLI_UTILITY_H__
