//////////////////////////////////////////////////////////////////////
//
//	IF4Fiber.h: interface for the CIF4Fiber class.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2005 NURI Telecom Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __IF4_FIBER_H__
#define __IF4_FIBER_H__

#include "if4frame.h"

class CIF4Fiber
{ 
public:
	CIF4Fiber();
	virtual ~CIF4Fiber();

// Override Functions
public:
	virtual	BOOL Start(int nPort, int nMaxSession);
	virtual	void Stop();

public:
	virtual void OnDataFile(WORKSESSION *pSession, UINT nSourceId, BYTE nType, char *pszName, BYTE *pData, int nLength);
	virtual void OnData(WORKSESSION *pSession, UINT nSourceId, IF4_DATA_FRAME *pData, int nLength);
	virtual void OnTypeR(WORKSESSION *pSession, UINT nSourceId, IF4_TYPER_FRAME *pData, int nLength);
	virtual void OnEvent(WORKSESSION *pSession, UINT nSourceId, IF4_EVENT_FRAME *pEvent, int nLength);
	virtual void OnAlarm(WORKSESSION *pSession, UINT nSourceId, IF4_ALARM_FRAME *pAlarm, int nLength);
};

#endif	// __IF4_FIBER_H__
