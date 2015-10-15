#ifndef __CMD_UTIL_H__
#define __CMD_UTIL_H__

#include "Chunk.h"

int GetHexaList(char *szSource, char *szHexaList, int maxLen, int *nLength);

#ifdef __USE_STDOUT__
void PrintOutStream(BYTE *pszStream, int nLength);
void PrintOutMIBValue(MIBValue *pValue, int nCount);
void PrintOutSmiValue(char *pValue, int nCount);

void repeater_data_parsing(char *mdata, UINT length, BOOL bDetail);
void pulse_data_parsing(char * sensorType, char *mdata, UINT length, BOOL bDetail);
void kamstrup_data_parsing(char *mdata, UINT length, BOOL hasLp);
void aidon_data_parsing(char *mdata, UINT length, BOOL bDetail);
void sx_data_parsing(char *mdata, UINT length, BOOL bDetail);
void ge_data_parsing(char *mdata, UINT length, BOOL bDetail);
void mbus_data_parsing(char *mdata, UINT length, BOOL bDetail);
void dlms_data_parsing(char *mdata, UINT length, BOOL bDetail);
void ihd_data_parsing(char *mdata, UINT length, BOOL bDetail);
int DisplayCommandResult(IF4Invoke *pInvoke);
#else
void PrintOutStream(CLISESSION *pSession, BYTE *pszStream, int nLength);
void PrintOutMIBValue(CLISESSION *pSession, MIBValue *pValue, int nCount);
void PrintOutSmiValue(CLISESSION *pSession, char *pValue, int nCount);

void repeater_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail);
void pulse_data_parsing(CLISESSION *pSession, char * sensorType, char *mdata, UINT length, BOOL bDetail);
void kamstrup_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL hasLp);
void aidon_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail);
void sx_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail);
void ge_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail);
void mbus_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail);
void dlms_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail);
void ihd_data_parsing(CLISESSION *pSession, char *mdata, UINT length, BOOL bDetail);
int DisplayCommandResult(CLISESSION *pSession, IF4Invoke *pInvoke);
#endif

void PrintOutStreamToString(char * pszString, BYTE *pszStream, int nLength);
char *itoa_comma(char *buf, int nValue);
void SPrintMIBValue(char * pszString, BYTE nType, MIBValue *pValue, int nMaxLen);
void GETTIMESTAMP(TIMESTAMP *pStamp, time_t *pNow);
BOOL GetShellCommandResult(CChunk *pChunk, char *pszCommand);
void ResetSystem();
void getStream2DayString(char * data, char * buff);
void getStream2HourString(char * data, char * buff);

char * CollapseString(const char * origStr, char * collapsedStr, int nCollapsedStrLen, char collapseChar = '.');

BOOL NeedAsync(BYTE nAttr);
BOOL MakeAsyncParameter(CIF4Invoke * pInvoke, EUI64 * pId, const char * oidStr);

double GetSignedDouble(UINT value);
double GetUnsignedDouble(UINT value);

UINT getParserType(UINT nParserType, char *szModel);
const char * getVendorString(BYTE nVendor);

#endif	// __CMD_UTIL_H__
