#ifndef __IF4_SERVICE_H__
#define __IF4_SERVICE_H__

#include "if4api.h"
#include "IF4TcpService.h"

class CIF4Service
{
public:
	CIF4Service();
	virtual ~CIF4Service();

public:
	void	SetMcuID(UINT nID);
	UINT	GetMcuID();
	void	EnableCompress(BOOL bEnable);

public:
	int 	Startup(int nPort, IF4_COMMAND_TABLE *pTable);
	int 	Shutdown();

public:
	IF4Invoke *NewInvoke(const char *pszAddress, int nPort, int nTimeout);
	int 	DeleteInvoke(IF4Invoke *pInvoke);
	int		AddParam(IF4Invoke *pInvoke, OID3 *pOid, int nType, int nValue, const char *pszValue, int nLength);
	int		AddResult(IF4Wrapper *pInvoke, OID3 *pOid, int nType, int nValue, const char *pszValue, int nLength);
	MIBValue *NewValue(OID3 *oid, int nType, int nValue, const char *pszValue, int nLength);
	BOOL	PrecreateInvoke(IF4Invoke *pInvoke, int nLength=IF4_DEFAULT_FRAME_SIZE);
	int		Cancel(IF4Invoke *pInvoke);

	int		SetUserCallback(PFNIF4ONDATAFILE pfnOnDataFile, PFNIF4ONDATA pfnOnData, 
                    PFNIF4ONEVENT pfnOnEvent, PFNIF4ONALARM pfnOnAlarm, PFNIF4ONTYPER pfnOnTypeR);

public:
	int 	AlarmFrame(IF4Invoke *pInvoke, char *pszBuffer, BYTE nType, EUI64 *id, TIMESTAMP *pTime, int nLength, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx);
	int 	EventFrame(IF4Invoke *pInvoke, char *pszBuffer, OID3 *id, BYTE nType, BYTE *srcId, TIMESTAMP *pTime);

	int 	Command(IF4Invoke *pInvoke, OID3 *cmd, BYTE nAttr);
	int 	Alarm(IF4Invoke *pInvoke, BYTE nType, EUI64 *id, TIMESTAMP *pTime, int nLength, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx);
	int 	Event(IF4Invoke *pInvoke, OID3 *id, BYTE nType, BYTE *srcId, TIMESTAMP *pTime);
	int 	SendData(IF4Invoke *pInvoke, WORD nCount, BYTE *pData, int nLength);
	int		SendDataFile(IF4Invoke *pInvoke, BYTE nType, char *pszFileName, BOOL bCompressed);
	int 	SendMeteringData(IF4Invoke *pInvoke, WORD nCount, BYTE *pData, int nLength);
	int 	SendTypeR(IF4Invoke *pInvoke, WORD nCount, BYTE *pData, int nLength);
	int		OpenSession(IF4Invoke *pInvoke);
	int	 	SendSession(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int len, BYTE nSeq);
	int		CloseSession(IF4Invoke *pInvoke);
	int 	SendFile(IF4Invoke *pInvoke, char *pszFileName, BYTE nChannel);

public:
	IF4_COMMAND_TABLE *FindCommand(OID3 *cmd);
	IF4_COMMAND_TABLE *FindCommandByName(char *pszName);

protected:
	CIF4TcpService		m_theTcpService;
	IF4_COMMAND_TABLE 	*m_pCommandTable;
	UINT				m_nServiceProviderID;

public:
	PFNIF4ONDATAFILE	m_pfnOnDataFile;
	PFNIF4ONDATA		m_pfnOnData;
	PFNIF4ONEVENT		m_pfnOnEvent;
	PFNIF4ONALARM		m_pfnOnAlarm;
	PFNIF4ONTYPER		m_pfnOnTypeR;
};

extern CIF4Service	*m_pIF4Service;

#endif // __IF4_SERVICE_H__
