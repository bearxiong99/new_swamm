#ifndef __COORDINATOR_SERVER_H__
#define __COORDINATOR_SERVER_H__

#include "CoordinatorFrame.h"
#include "CoordinatorDataStream.h"
#include "CoordinatorWaitReply.h"
#include "EndDevicePool.h"
#include "TimeoutThread.h"
#include "LinkedList.h"
#include "Locker.h"

typedef struct	_tagCODIWORKSTREAM
{
		COORDINATOR		*codi;
		char			*pBuffer;
		int				nLength;
		int				nPosition;
}	CODIWORKSTREAM;

class CCoordinatorServer : public CCoordinatorFrame,
						   public CCoordinatorDataStream,
						   public CEndDevicePool,
						   public CCoordinatorWaitReply,
						   public CTimeoutThread
{
public:
	CCoordinatorServer();
	virtual ~CCoordinatorServer();

public:
	void	StartupStreamWorker();
	void	ShutdownStreamWorker();
	int 	RegisterCoordinator(COORDINATOR **codi, CODIDEVICE *pDevice);
	int 	UnregisterCoordinator(COORDINATOR *codi);
	int 	UnregisterCoordinatorAll();
	int 	StartupCoordinator(COORDINATOR *codi);
	int 	StartupCoordinatorAll();
	int 	ShutdownCoordinator(COORDINATOR *codi);
	int 	ShutdownCoordinatorAll();
	int 	GetCoordinatorProperty(COORDINATOR *codi, BYTE nProperty, BYTE *payload, int *nLength, int nTimeout);
	int 	SetCoordinatorProperty(COORDINATOR *codi, BYTE nProperty, BYTE *payload, int nLength, int nTimeout);
	int 	MulticastToEndDevice(COORDINATOR *codi, BYTE type, BYTE *payload, int nLength);

    // Coordinator Options
	int 	SetOptionCoordinator(COORDINATOR *codi, int nOption);
	int 	GetOptionCoordinator(COORDINATOR *codi, int *nOption);

public:
	int 	OpenEndDevice(COORDINATOR *codi, ENDDEVICE **endi, EUI64 *id);
	int 	CloseEndDevice(ENDDEVICE *endi);
    HANDLE  GetEndDeviceByID(EUI64 *id);

	int 	ConnectParentDevice(COORDINATOR *codi, EUI64 *id, WORD shortid, BYTE hops, WORD *path, int nTimeout);
	int 	SetChildDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, WORD shortid, EUI64 *child, int nTimeout);
	int 	DisconnectParentDevice(COORDINATOR *codi, EUI64 *id, WORD shortid, BYTE hops, WORD *path);

	int 	ConnectEndDevice(ENDDEVICE *endi, WORD shortid, BYTE hops, WORD *path, int nTimeout, BOOL bExtendedTimeout, BOOL bDirect, BOOL bEndDevice);
	int		LinkEndDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, int nTimeout);
	int		LinkEndDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, int nTimeout);
	int		LinkCloseEndDevice(ENDDEVICE *endi, int nTimeout);
	int		LinkCloseEndDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, int nTimeout);
	int 	DisconnectEndDevice(ENDDEVICE *endi);
	int 	SendToEndDevice(ENDDEVICE *endi, BYTE rw, BYTE type, BYTE *payload, int nLength, int nTimeout);
	int		SendRecvToEndDevice(ENDDEVICE *endi, BYTE read_write_req, BYTE type, BYTE *payload, int nLength,
						BYTE *data, int *datalen, int nTimeout);
	int 	GetOptionEndDevice(ENDDEVICE *endi, int nOption, int *nValue);
	int 	SetOptionEndDevice(ENDDEVICE *endi, int nOption, int nValue);
	int		ResetCoordinator(COORDINATOR *codi);
	int		BootloadCoordinator(COORDINATOR *codi, int nType);
	int		DownloadCoordinator(COORDINATOR *codi, int nType, const char *pszFileName, const char *pszExt);
	int		PlaybackCoordinator(COORDINATOR *codi, BYTE *frame, int nLength);

	int		BypassEndDevice(ENDDEVICE *endi, BYTE *payload, int nLength, int nTimeout);
	int		BypassEndDevice(COORDINATOR *codi, BYTE *payload, int nLength);
	int		LeaveEndDevice(ENDDEVICE *endi, BYTE joinTime, BYTE channel, WORD panid, int nTimeout);
	int		ReadAmrEndDevice(ENDDEVICE *endi, BYTE *data, int *nLength, int nTimeout);
	int		WriteAmrEndDevice(ENDDEVICE *endi, WORD mask, BYTE *data, int nLength, int nTimeout);
	int		ReadRomEndDevice(ENDDEVICE *endi, WORD addr, int nLength, BYTE *data, int *nSize, int nTimeout);
	int		ReadLongRomEndDevice(ENDDEVICE *endi, UINT addr, int nLength, BYTE *data, int *nSize, int nTimeout);
	int		WriteRomEndDevice(ENDDEVICE *endi, WORD addr, BYTE *data, int nLength, int nTimeout);
	int		WriteRomEndDevice(COORDINATOR *codi, WORD addr, BYTE *data, int nLength);
	int		WriteLongRomEndDevice(ENDDEVICE *endi, UINT addr, BYTE *data, int nLength, BYTE * replay, int *nSize, int nTimeout);
	int		WriteLongRomEndDevice(COORDINATOR *endi, UINT addr, BYTE *data, int nLength);
	int		WriteMccbEndDevice(ENDDEVICE *endi, BYTE value, BYTE *data, int nTimeout);
	int		WriteCidEndDevice(ENDDEVICE *endi, BYTE *value, int valuelen, BYTE *data, int *datalen, int nTimeout);
	int		CommandEndDevice(ENDDEVICE *endi, BYTE cmd, BYTE type, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout);

	int		BindEndDevice(COORDINATOR *codi, EUI64 *id, BYTE type, int nTimeout, BOOL bExtendedTimeout);
	int		WriteEndDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, BYTE *data, int nLength, int nTimeout);

public:
	int		SendToCoordinator(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	void	SetCoordinatorPassive(COORDINATOR *codi, BOOL bPassive);

	int 	WaitingForCommandReply(COORDINATOR *codi, BYTE method, BYTE type, BYTE seq,
						BYTE *pFrame, int len, BYTE *pBuffer, int *nLength, int nTimeout);
	int 	WaitingForDataReply(COORDINATOR *codi, BYTE method, BYTE type, BYTE seq, BYTE waitMask,
						BYTE *pFrame, int len, int nTimeout);

	void	OnPrevSendFrame(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	void	OnSendFrame(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	void	OnReceiveFrame(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	void	OnError(COORDINATOR *codi, int nError);

	void 	OnReceiveCoordinator(COORDINATOR *codi, CODI_GENERAL_FRAME *pFrame, int nLength);
	CODIWORKSTREAM *AddStreamQueue(COORDINATOR *codi, CODI_GENERAL_FRAME *pFrame, int nLength);
	CODIWORKSTREAM *AddStreamQueue2(COORDINATOR *codi, BYTE *pFrame, int nLength);

protected:
	int		Unbinding(ENDDEVICE *endi);
	int		CloseDirectConnection(ENDDEVICE *endi);
	void	ReceiveStreamWorker(COORDINATOR *codi, CODI_GENERAL_FRAME *pFrame, int nLength);
	void	CheckCoordinatorState();
	BOOL 	OnActiveThread();
	BOOL 	OnTimeoutThread();

private:
	void 	OnInformationFrame(COORDINATOR *codi, CODI_INFO_FRAME *pFrame, int nLength);
	void	OnCommandFrame(COORDINATOR *codi, CODI_COMMAND_FRAME *pFrame, int nLength);
	void	OnCommandReadFrame(COORDINATOR *codi, CODI_COMMAND_FRAME *pFrame, int nLength);
	void	OnCommandWriteFrame(COORDINATOR *codi, CODI_COMMAND_FRAME *pFrame, int nLength);
	void	OnDataFrame(COORDINATOR *codi, ENDI_DATA_FRAME *pFrame, int nLength);

private:
	COORDINATOR *AddCoordinator(CODIDEVICE *pDevice);
	BOOL 	RemoveCoordinator(COORDINATOR *codi);
	COORDINATOR *FindCoordinatorByName(char *pszName);
	COORDINATOR *FindCoordinatorByHandle(COORDINATOR *codi);
	int 	StartCoordinator(COORDINATOR *codi);
	int 	StopCoordinator(COORDINATOR *codi);
    int     SetExtendedTimeout(ENDDEVICE *endi, BOOL bExtendedTimeout);

protected:
	BOOL		m_bInitialized;
	cetime_t	m_tmLastStateCheck;
    int         m_nCoordinatorOption;
	CLocker		m_codiLocker;
	CLocker		m_streamLocker;
	CLinkedList<COORDINATOR *>	m_codiList;
	CLinkedList<CODIWORKSTREAM *> m_streamList;
};

#endif	// __COORDINATOR_SERVER_H__
