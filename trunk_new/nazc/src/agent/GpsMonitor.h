//////////////////////////////////////////////////////////////////////
//
//	GpsMonitor.h: interface for the CGpsMonitor class.
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

#ifndef __GPS_MONITOR_H__
#define __GPS_MONITOR_H__

#include "SerialServer.h"
#include "Chunk.h"
#include "Locker.h"

#define GPS_GPRMC			0
#define GPS_GPGGA			1
#define GPS_GPGSV			2
#define GPS_GPGSA			3

class CGpsMonitor : public CSerialServer
{
// Construction/Destruction
public:
	CGpsMonitor();
	virtual	~CGpsMonitor();

public:
	BYTE	getFixPosition();
	BYTE	getSatCount();
	TIMESTAMP	*getTime();
	char	*getPosition();
	int		getTimezone();
	void	setTimezone(int nTimezone);

public:
	BOOL	Initialize();
	BYTE	GetChecksum(BYTE *pszBuffer, int nLength);

// Message Handlers
protected:
	void	SetGpsLine(int nIndex, char *pszBuffer);
	BOOL 	OnReceiveSession(WORKSESSION *pSession, char* pszBuffer, int nLength);
	void 	OnSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength);

protected:
	CLocker		m_Locker;
	char		m_szGpsLine[5][128];
	TIMESTAMP	m_tmGpsTime;
	int			m_nFixPosition;
	BOOL		m_bStatus;
	int			m_nUseCount;
	int			m_nCount;
	char		m_szLatitude[32];
	char		m_szLongitude[32];
	char		m_szGpsPosition[32];
	int			m_nTimeZone;
	UINT		m_nLastCheck;
	time_t		m_nLastUpdate;
	int			m_nState;
	CChunk		m_Chunk;
};

extern CGpsMonitor	*m_pGpsMonitor;

#endif	// __GPS_MONITOR_H__

