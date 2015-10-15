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

#ifndef __MOBILE_CLIENT_H__
#define __MOBILE_CLIENT_H__

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
	BOOL	Initialize();
	void	Destroy();

	int		WriteToModem(const char *pszBuffer, int nLength=-1);
	int		WriteToModem(int c);
	int		ReadFromModem(char *pszBuffer, int nLength, int nTimeout);
	int		ReadLineFromModem(char *pszBuffer, int nLength, int nTimeout);
	void	Flush();

protected:
	BOOL	OnReceiveSession(WORKSESSION *pSession, char* pszBuffer, int nLength);
	void	OnSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength);

protected:
	BOOL	m_bSerialInitialize;
	BOOL	m_bDisplay;
	char	m_szLine[4096];
	int		m_nLength;
	CQueue	*m_pQueue;
};

#if defined(__TI_AM335X__)
/** PXA에서 PPP를 사용할 경우 전역 MobileClient를 사용할 경우 
 * PPP가 정상적으로 serial에 access 할 수 없다
 */
//extern  CMobileClient   *m_pMobileClient;
#endif

#endif  // __MOBILE_CLIENT_H__
