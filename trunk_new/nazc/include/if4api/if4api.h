#ifndef __IF4_API_H__
#define __IF4_API_H__

#include "if4def.h"
#include "if4type.h"
#include "if4cmderr.h"
#include "if4err.h"
#include "if4invoke.h"

#ifdef __cplusplus
extern "C" {
#endif

// Service Startup/Shutdown
int IF4API_Initialize(int nPort, IF4_COMMAND_TABLE *pTable);
int IF4API_Destroy();

// Inovke Functions
IF4Invoke *IF4API_NewInvoke(const char *pszAddress, int port, int nTimeout);
int IF4API_DeleteInvoke(IF4Invoke *pInvoke);
int IF4API_AddParamMIBValue(IF4Invoke *pInvoke, const MIBValue *pValue); 
int IF4API_AddParamValue(IF4Invoke *pInvoke, const char *pszOid); 
int IF4API_AddParamVoid(IF4Invoke *pInvoke, const char *pszOid); 
int IF4API_AddParamFormat(IF4Invoke *pInvoke, const char *pszOid, BYTE nType, const char *pszValue, WORD len); 
int IF4API_AddParamOpaque(IF4Invoke *pInvoke, const char *pszOid, const char *pszValue, WORD len); 
int IF4API_AddParamString(IF4Invoke *pInvoke, const char *pszOid, const char *pszValue); 
int IF4API_AddParamNumber(IF4Invoke *pInvoke, const char *pszOid, BYTE nType, int nValue); 

int IF4API_UpdateParamMIBValue(IF4Invoke *pInvoke, const MIBValue *pValue);
BOOL IF4API_IsParamIn(IF4Invoke *pInvoke, OID3 *oid);

MIBValue *IF4API_GetResult(IF4Invoke *pInvoke, const char *pszOid);
MIBValue *IF4API_GetResultByName(IF4Invoke *pInvoke, const char *pszName);

// Result Functions
int IF4API_AddResultFormat(IF4Wrapper *pWrapper, const char *pszOid, BYTE nType, const void *pValue, int nLength); 
int IF4API_AddResultOpaque(IF4Wrapper *pWrapper, const char *pszOid, const void *pValue, int nLength); 
int IF4API_AddResultString(IF4Wrapper *pWrapper, const char *pszOid, const char *pszString); 
int IF4API_AddResultNumber(IF4Wrapper *pWrapper, const char *pszOid, BYTE nType, int nValue); 
int IF4API_AddResultMIBValue(IF4Wrapper *pWrapper, MIBValue *pValue); 

// Service Command Functions
IF4Error IF4API_Cancel(IF4Invoke *pInvoke);
IF4Error IF4API_Command(IF4Invoke *pInvoke, const char *pszCommand, BYTE nAttr);
IF4Error IF4API_Alarm(IF4Invoke *pInvoke, BYTE srcType, EUI64 *id, TIMESTAMP *pTime, int nLength, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx);
IF4Error IF4API_AlarmFrame(char *pszBuffer, IF4Invoke *pInvoke, BYTE srcType, EUI64 *id, TIMESTAMP *pTime, int nLength, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx);
IF4Error IF4API_Event(IF4Invoke *pInvoke, char *pszEvent, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime);
IF4Error IF4API_EventFrame(char *pszBuffer, IF4Invoke *pInvoke, char *pszEvent, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime);
IF4Error IF4API_SendData(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int nLength);
IF4Error IF4API_SendDataFile(IF4Invoke *pInvoke, BYTE nType, char *pszFileName, BOOL bCompressed);
IF4Error IF4API_SendMeteringData(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int nLength);
IF4Error IF4API_SendFile(IF4Invoke *pInvoke, char *pszFileName, BYTE nChannel);
IF4Error IF4API_SendTypeR(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int nLength);

IF4Error IF4API_OpenSession(IF4Invoke *pInvoke);
IF4Error IF4API_SendSession(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int len, BYTE nSeq);
IF4Error IF4API_CloseSession(IF4Invoke *pInvoke);

// Utility Functions
void IF4API_EnableCompress(BOOL bEnable);
void IF4API_SetID(UINT nID);
UINT IF4API_GetID();
int IF4API_TIMESTAMPTOSTR(char *pszGUID, TIMESTAMP *pTime);
void IF4API_DumpMIBValue(MIBValue *pValue, int nCount);
void IF4API_FreeMIBValue(MIBValue *pValue);
const char *IF4API_GetErrorMessage(int nError);
MIBValue * IF4API_NewMIBValue(const char *pszOid, int nType, int nValue, const char *pszValue, int nLength);
void IF4API_SetMaxLogSize(int logSize);

// Data/Event/Alarm Callback function
typedef void (*PFNIF4ONDATAFILE)(char *pszAddress, UINT nSourceId, BYTE nType, char *pszName, BYTE *pData, int nLength);
typedef void (*PFNIF4ONDATA)(char *pszAddress, UINT nSourceId, BYTE *pData, int nLength);
typedef void (*PFNIF4ONTYPER)(char *pszAddress, UINT nSourceId, BYTE *pData, int nLength);
typedef void (*PFNIF4ONEVENT)(char *pszAddress, UINT nSourceId, BYTE *pEvent, int nLength);
typedef void (*PFNIF4ONALARM)(char *pszAddress, UINT nSourceId, BYTE *pAlarm, int nLength);

int IF4API_SetUserCallback(PFNIF4ONDATAFILE pfnOnDataFile, PFNIF4ONDATA pfnOnData, 
        PFNIF4ONEVENT pfnOnEvent, PFNIF4ONALARM pfnOnAlarm, PFNIF4ONTYPER pfnOnTypeR);

#ifdef __cplusplus
}
#endif

#endif	// __IF4_API_H__
