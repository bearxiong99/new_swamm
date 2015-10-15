//////////////////////////////////////////////////////////////////////
//
//	GpioControl.h: interface for the CGpioControl class.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@paran.com
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __GPIO_CONTROL_H__
#define __GPIO_CONTROL_H__

#include <pthread.h>
#include "Locker.h"
#include "GPIOAPI.h"

class CGpioControl
{
public:
	CGpioControl();
	virtual ~CGpioControl();

public:
	BOOL	IsEnableHooker();
	BOOL	EnableGpioHooker(BOOL bEnable);

	int		ReadGpio(int nPort);
	BOOL	WriteGpio(int nPort, int nValue);

public:
	BOOL	Initialize(int nMobileType);
	BOOL	Destroy();

	BOOL	InstallHooker(PFNGPIOCALLBACK pfnCallback, void *pParam, void *pArgment);
	BOOL	UninstallHooker();

protected:
	void	RunMonitoring();

public:
	static	void *gpioWatchThread(void *pParam);

protected:
	BOOL			m_bExitPending;
	BOOL			m_bEnableHooker;
	int				m_nMobileType;
	int				m_nFD;
	void			*m_pParam;
	void			*m_pArgment;
	CLocker			m_Locker;
	CLocker			m_gpioLocker;
	PFNGPIOCALLBACK	m_pfnCallback;
	PFNUSERHOOKER	m_pfnUserHooker;
    pthread_t		m_gpioThreadID;
};

#endif // __GPIO_CONTROL_H__
