//////////////////////////////////////////////////////////////////////
//
//	MobileServer.h: interface for the CMobileServer class.
//
//	This file is a part of the AIMIR-GCP.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "SerialServer.h"
#include "TelnetClient.h"
#include "Chunk.h"

#define MOBILE_METHOD_UNKNOWN		0
#define MOBILE_METHOD_TELNET		1
#define MOBILE_METHOD_GET			2
#define MOBILE_METHOD_PUT			3
#define MOBILE_METHOD_COMMAND		4
#define MOBILE_METHOD_LIST			5
#define MOBILE_METHOD_NACS			6

typedef	struct	_tagMTPHEADER
{
		int		nMethod;
		double	fVersion;
		char	szURL[128];
		char	szLocation[128];
		char	szUser[33];
		char	szPassword[33];
		BOOL	bInstall;
		int		nLength;
		int		nSeek;
		int		nState;
		int		nFrameSize;
		int		nErrorCount;
		BYTE	nSeq;
		FILE	*fp;
}	MTPHEADER, *PMTHHEADER;

class CMobileServer : public CSerialServer
{
public:
	CMobileServer();
	virtual	~CMobileServer();

public:
	BOOL	IsActive();
	time_t	GetLastActive();
	int		GetDCD();

public:
	BOOL	Initialize(BOOL bGsmMode, int speed, BOOL bAnswer);
	void	Destroy();

	int		WriteToModem(char *pszBuffer, int nLength=-1);
	int		WriteToModem(int c);
	int		ReadFromModem(char *pszBuffer, int nLength, int nTimeout);
	void	TerminateAlert();
	void	ResetSystem();
	void	FirmwareUpdate(char *pszFileName);
	BOOL	ExecuteCommand(LPSTR pszCommand);

protected:
	void	untilcpy(char *dest, char *src);
	void	SendResult(int nError);

protected:
	BOOL	GetHeader(MTPHEADER *pHeader, char *pszBuffer, int nLength);
	BOOL	OnReceiveSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength);
	void	OnSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength);
	void	OnPut(WORKSESSION *pSession, LPSTR pszBuffer, int nLength);

protected:
	BOOL			m_bLiveSession;
	BOOL			m_bAnswer;
	BOOL			m_bGsmMode;
	int				m_nState;
	time_t			m_tmLastReceived;
	CChunk			m_Chunk;
	MTPHEADER		m_theHeader;
	CTelnetClient	m_theClient;
};

extern	CMobileServer	*m_pMobileServer;
