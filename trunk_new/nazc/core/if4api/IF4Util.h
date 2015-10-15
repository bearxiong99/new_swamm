#ifndef __IF4_UTIL_H__
#define __IF4_UTIL_H__

BYTE BCD_BYTE(WORD value);
BYTE BCD2BYTE(BYTE c);
WORD BCD_WORD(WORD value);
void BCD_TIMESTAMP(TIMESTAMP *dest, TIMESTAMP *src);
void GET_TIMESTAMP(TIMESTAMP *pStamp, time_t *pNow);
BOOL StringToOid(const char *pszOid, OID3 *pOid);
BOOL OidToString(OID3 *pOid, char *pszOid);
void FreeLinkedValue(MIBValue *pStart);
const char *GetDataFileType(int nType);
void LogSmiValue(char *pValue, int nCount);
void SmiToHostValue(char *pValue, int nCount);
void SmiToLittleValue(char *pValue, int nCount);
void DumpSmiValue(char *pValue, int nCount);
void DumpMIBValue(MIBValue *pValue, int nCount);
const char *GetIF4ErrorMessage(int nError);
BYTE GetTID();

#endif	// __IF4_UTIL_H__
