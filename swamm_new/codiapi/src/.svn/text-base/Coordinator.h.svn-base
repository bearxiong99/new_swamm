#ifndef __COORDINATOR_H__
#define __COORDINATOR_H__

#include "CoordinatorServer.h"

class CCoordinator : public CCoordinatorServer
{
public:
	CCoordinator();
	virtual ~CCoordinator();

public:
	// API Init/Exit functions
	int 	Initialize();
	int 	Exit();

	// Coordinator register/unregister functions
	int 	Register(COORDINATOR **codi, CODIDEVICE *pDevice);
	int 	Unregister(COORDINATOR *codi);

	// Coordinator service functions
	int 	Startup(COORDINATOR *codi);
	int 	Shutdown(COORDINATOR *codi);

	// Coordinator functions
	int 	GetProperty(COORDINATOR *codi, BYTE nProperty, BYTE *payload, int *nLength, int nTimeout);
	int 	SetProperty(COORDINATOR *codi, BYTE nProperty, BYTE *payload, int nLength, int nTimeout);
	int 	Multicast(COORDINATOR *codi, BYTE type, BYTE *payload, int nLength);
	int 	Sync(COORDINATOR *codi, int nInterval);
	int 	Reset(COORDINATOR *codi);
	int		Bootload(COORDINATOR *codi, int nType);
	int		Download(COORDINATOR *codi, int nModemType, const char *pszFileName, const char *pszExt);
	int 	Playback(COORDINATOR *codi, BYTE *frame, int nLength);

    // Coordinator Options
	int		SetOption(COORDINATOR *codi, int nOption);
	int		GetOption(COORDINATOR *codi, int *nOption);

	// End Device functions
	int 	EndiOpen(COORDINATOR *codi, ENDDEVICE **endi, EUI64 *id);
	int 	EndiClose(ENDDEVICE *endi);
    HANDLE  EndiGetDevice(EUI64 *id);
	int 	EndiConnect(ENDDEVICE *endi, WORD shortid, BYTE hops, WORD *path, int nTimeout, BOOL bExtenedTimeout, BOOL bDirect, BOOL bEndDevice);
	int 	EndiDisconnect(ENDDEVICE *endi);
	int 	EndiSend(ENDDEVICE *endi, BYTE rw, BYTE type, BYTE *payload, int nLength, int nTimeout);
	int		EndiSendRecv(ENDDEVICE *endi, BYTE read_write_req, BYTE type, BYTE *payload, int nLength,
						BYTE *data, int *datalen, int nTimeout);
	int		EndiGetOption(ENDDEVICE *endi, int nOption, int *nValue);
	int		EndiSetOption(ENDDEVICE *endi, int nOption, int nValue);

	// End Device utility functions
	int		EndiBypass(ENDDEVICE *endi, BYTE *payload, int nLength, int nTimeout);
	int		EndiBypass(COORDINATOR *codi, BYTE *payload, int nLength);
	int		EndiLeave(ENDDEVICE *endi, BYTE joinTime, BYTE channel, WORD panid, int nTimeout);
	int		EndiReadAmr(ENDDEVICE *endi, BYTE *data, int *nLength, int nTimeout);
	int		EndiWriteAmr(ENDDEVICE *endi, WORD mask, BYTE *data, int nLength, int nTimeout);
	int		EndiReadRom(ENDDEVICE *endi, WORD addr, int nLength, BYTE *data, int *nSize, int nTimeout);
	int		EndiReadLongRom(ENDDEVICE *endi, UINT addr, int nLength, BYTE *data, int *nSize, int nTimeout);
	int		EndiWriteRom(ENDDEVICE *endi, WORD addr, BYTE *data, int nLength, int nTimeout);
	int		EndiWriteRom(COORDINATOR *codi, WORD addr, BYTE *data, int nLength);
	int		EndiWriteLongRom(ENDDEVICE *endi, UINT addr, BYTE *data, int nLength, BYTE *replay, int *nSize, int nTimeout);
	int		EndiWriteLongRom(COORDINATOR *codi, UINT addr, BYTE *data, int nLength);
	int		EndiWriteMccb(ENDDEVICE *endi, BYTE value, BYTE *data, int nTimeout);
	int		EndiWriteCid(ENDDEVICE *endi, BYTE *value,int valuelen,  BYTE *data, int *datalen, int nTimeout);
	int		EndiCommandWrite(ENDDEVICE *endi, BYTE cmd, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout);
	int		EndiCommandRead(ENDDEVICE *endi, BYTE cmd, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout);

	int		EndiBind(COORDINATOR *codi, EUI64 *id, BYTE type, int nTimeout, BOOL bExtenedTimeout);
	int		EndiLink(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, int nTimeout);
	int		EndiWrite(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, BYTE *data, int nLength, int nTimeout);

	// Utility functions
	BOOL 	IsInitialized();
	BOOL 	IsStarted(COORDINATOR *codi);
	BOOL 	IsSuperParent(COORDINATOR *codi);
	BOOL 	IsConnected(ENDDEVICE *endi);
	BOOL 	IsError(ENDDEVICE *endi);
	int		GetState(COORDINATOR *codi);
	HANDLE	GetHandle(COORDINATOR *codi);
	int 	GetVersion(BYTE *nMajor, BYTE *nMinor, BYTE *nRevision);
	cetime_t GetLastCommunicationTime(COORDINATOR *codi);
	const char 	*ErrorMessage(int nError);

	BOOL    GetWatchdogState(COORDINATOR *codi);
	BOOL    SetWatchdogState(COORDINATOR *codi, BOOL bState);

};

extern CCoordinator	*m_ptheCoordinator;

#endif	// __COORDINATOR_H__
