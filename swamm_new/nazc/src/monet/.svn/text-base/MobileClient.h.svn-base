//////////////////////////////////////////////////////////////////////
//
//	MobileClient.h: interface for the CMobileClient class.
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

#include "Queue.h"
#include "SerialServer.h"

class CMobileClient : public CSerialServer
{
public:
	CMobileClient();
	virtual	~CMobileClient();

public:
	void	SetDisplay(BOOL bDisplay);

public:
	BOOL	Initialize(char *pszDevice, int nSpeed);
	void	Destroy();

	int		WriteToModem(char *pszBuffer, int nLength=-1);
	int		WriteToModem(int c);
	int		ReadFromModem(char *pszBuffer, int nLength, int nTimeout);

protected:
	BOOL	OnReceiveSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength);
	void	OnSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength);

protected:
	BOOL	m_bDisplay;
	char	m_szLine[4096];
	int		m_nLength;
	CQueue	*m_pQueue;
};

extern	CMobileClient	*m_pMobileClient;
