#ifndef __CODI_API_H__
#define __CODI_API_H__

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "codiTypedef.h"
#include "codiCallback.h"

/* Coordinator API Functions */

int codiInit();
int codiExit();
int codiRegister(HANDLE *codi, CODIDEVICE *pDevice);
int codiUnregister(HANDLE codi);
int codiStartup(HANDLE codi);
int codiShutdown(HANDLE codi);
int codiGetProperty(HANDLE codi, int cmd, BYTE *payload, int *nLength, int nTimeout);
int codiSetProperty(HANDLE codi, int cmd, BYTE *payload, int nLength, int nTimeout);
int codiMulticast(HANDLE codi, BYTE type, BYTE *payload, int nLength);
int codiSync(HANDLE codi, int nInterval);
int codiReset(HANDLE codi);
int	codiBootload(HANDLE codi, int nType);
int codiDownload(HANDLE codi, int nModemType, const char *pszFileName, const char *pszExt);
int codiPlayback(HANDLE codi, BYTE *frame, int nLength);

int codiSetOption(HANDLE codi, int nOption);
int codiGetOption(HANDLE codi, int *nOption);

/* End Device API Functions */

int endiOpen(HANDLE codi, HANDLE *endi, EUI64 *id);
int endiClose(HANDLE endi);
HANDLE endiGetHandle(EUI64 * id);
int endiConnect(HANDLE endi, int nTimeout, BOOL bEndDevice);
int endiConnectWithRouteRecord(HANDLE endi, WORD shortid, BYTE hops, WORD *path, int nTimeout, BOOL bEndDevice=FALSE);
int endiConnectDirect(HANDLE endi, WORD shortid, int nTimeout, BOOL bEndDevice=FALSE);
int endiDisconnect(HANDLE endi);
int endiDisconnectDirectConnection(HANDLE endi);
int endiSend(HANDLE endi, BYTE read_write_req, BYTE type, BYTE *payload, int nLength, int nTimeout);
int endiSendRecv(HANDLE endi, BYTE read_write_req, BYTE type, BYTE *payload, int nLength,
					BYTE *data, int *datalen, int nTimeout);
int endiGetOption(HANDLE endi, int nOption, int *nValue);
int endiSetOption(HANDLE endi, int nOption, int nValue);

/* End Device Utility Functions */

int endiBypass(HANDLE endi, BYTE *payload, int nLength, int nTimeout);
int multicastBypass(HANDLE codi, BYTE *payload, int nLength);
int endiLeave(HANDLE endi, BYTE joinTime, BYTE channel, WORD panid, int nTimeout);
int endiReadAmr(HANDLE endi, BYTE *data, int *nLength, int nTimeout);
int endiWriteAmr(HANDLE endi, WORD mask, BYTE *data, int nLength, int nTimeout);
int endiReadRom(HANDLE endi, WORD addr, int nLength, BYTE *data, int *nSize, int nTimeout);
int endiReadLongRom(HANDLE endi, UINT addr, int nLength, BYTE *data, int *nSize, int nTimeout);
int endiWriteRom(HANDLE endi, WORD addr, BYTE *data, int nLength, int nTimeout);
int multicastWriteRom(HANDLE codi, WORD addr, BYTE *data, int nLength);
int endiWriteLongRom(HANDLE endi, UINT addr, BYTE *data, int nLength, BYTE *replay, int *nSize, int nTimeout);
int multicastWriteLongRom(HANDLE codi, UINT addr, BYTE *data, int nLength);
int endiWriteMccb(HANDLE endi, BYTE value, BYTE *data, int nTimeout);
//int endiWriteCid(HANDLE endi, BYTE *value, int valuelen, BYTE *data, int *datalen, int nTimeout);
int endiCommandWrite(HANDLE endi, BYTE cmd, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout);
int endiCommandRead(HANDLE endi, BYTE cmd, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout);

int endiBind(HANDLE codi, EUI64 *id, BYTE type, int nTimeout, BOOL bExtenedTimeout=FALSE);
int endiLink(HANDLE codi, EUI64 *id, BYTE seq, BYTE type, int nTimeout);
int endiWrite(HANDLE codi, EUI64 *id, BYTE seq, BYTE type, BYTE *data, int nLength, int nTimeout);

/* Utility Functions */

BOOL codiIsInitialized(void);
BOOL codiIsStarted(HANDLE codi);
BOOL codiIsSuperParent(HANDLE codi);
BOOL endiIsConnected(HANDLE endi);
BOOL endiIsError(HANDLE endi);

BOOL codiGetWatchdogState(HANDLE codi);
BOOL codiSetWatchdogState(HANDLE codi, BOOL bState);

int codiGetState(HANDLE codi);
HANDLE codiGetHandle(HANDLE codi);
int codiGetVersion(BYTE *nMajor, BYTE *nMinor, BYTE *nRevision);
const char *codiErrorMessage(int nError);
cetime_t codiGetLastCommunicationTime(HANDLE codi);

int atoeui64(const char *pszString, EUI64 *id);
int eui64toa(const EUI64 *id, char *pszString);

WORD ntoshort(WORD val);
UINT ntoint(UINT val);

#ifdef __cplusplus
}
#endif

#endif	/* __CODI_API_H__ */

