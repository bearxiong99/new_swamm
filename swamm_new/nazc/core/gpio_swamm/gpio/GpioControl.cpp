//////////////////////////////////////////////////////////////////////
//
//	GpioControl.cpp: implementation of the CGpioControl class.
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

#include "common.h"
#include "GPIOAPI.h"
#include "GpioControl.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CGpioControl Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGpioControl::CGpioControl()
{
	m_nMobileType	= MOBILE_TYPE_GSM;
	m_nFD			= -1;
	m_pParam		= NULL;
	m_pArgment		= NULL;
	m_pfnCallback	= NULL;
	m_pfnUserHooker	= NULL;

	m_bExitPending	= FALSE;
	m_bEnableHooker	= TRUE;
}

CGpioControl::~CGpioControl()
{
}

//////////////////////////////////////////////////////////////////////
// CGpioControl Attribute
//////////////////////////////////////////////////////////////////////

BOOL CGpioControl::IsEnableHooker()
{
	return m_bEnableHooker;
}

int CGpioControl::ReadGpio(int nPort)
{
	int		nValue=0;

	if (m_nFD == -1)
		return 0;

    m_gpioLocker.Lock();
	nValue = ioctl(m_nFD, GPIO_IOCTL_IN, nPort);
    m_gpioLocker.Unlock();
	return nValue;
}

BOOL CGpioControl::WriteGpio(int nPort, int nValue)
{
	if (m_nFD == -1)
		return FALSE;

    m_gpioLocker.Lock();
	ioctl(m_nFD, GPIO_IOCTL_OUT, (nValue > 0) ? GPIOHIGH(nPort) : GPIOLOW(nPort));
    m_gpioLocker.Unlock();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CGpioControl initialize/destroy
//////////////////////////////////////////////////////////////////////

BOOL CGpioControl::Initialize(int nMobileType)
{
	m_nMobileType = nMobileType;

	if (m_nFD != -1)
		return FALSE;

    m_nFD = open("/dev/gpio", O_RDWR | O_NDELAY);
    if (m_nFD < 0)
    {
       	printf(" -------- GPIO Devive Open Fail -------\n");
		m_nFD = -1;
        return FALSE;
    }

    // GPIO 입력 다이렉션
	ioctl(m_nFD, GPIO_IOCTL_DIR, GP_PWR_FAIL_INPUT);					// PWR FAIL
	ioctl(m_nFD, GPIO_IOCTL_DIR, GP_LOW_BATT_INPUT);					// LOW BATTERY
	ioctl(m_nFD, GPIO_IOCTL_DIR, GP_GSM_DCD_INPUT);						// MOBILE DCD
	ioctl(m_nFD, GPIO_IOCTL_DIR, GP_GSM_SYNC_INPUT);					// MOBILE SYNC
	ioctl(m_nFD, GPIO_IOCTL_DIR, GP_GSM_RI_INPUT);						// MOBILE RI
	ioctl(m_nFD, GPIO_IOCTL_DIR, GP_DOOR_OPEN_INPUT);					// DOOR OPEN/CLOSE
	ioctl(m_nFD, GPIO_IOCTL_DIR, GP_TEMP_OS_INPUT);						// HEATER ON/OFF
	ioctl(m_nFD, GPIO_IOCTL_DIR, GP_BATT_CHG_STATUS_INPUT);				// BATT CHG STS
	ioctl(m_nFD, GPIO_IOCTL_DIR, GP_CODI_RST_OUT);				        // COORDINATOR RESET

    // GPIO 포트 초기값 설정
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_SW_RST_OUT));				// PWR RESET
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_CODI_RST_OUT));			// COORDINATOR RESET

    // GPIO Output Direction Setting
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_SW_RST_OUT));				// SW RESET
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_BATT_SW_OUT));				// PWR CTRL
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_BATT_CHG_EN_OUT));			// BATT CHG EN
	if (m_nMobileType != MOBILE_TYPE_CDMA)
    {
		ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_GSM_EMERGOFF_OUT));		// EMERGENCY OFF
	    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_GSM_IGT_OUT));			// MOBILE RESET (IGT)
    }else {
        ioctl(m_nFD, GPIO_IOCTL_DIR, GP_GSM_IGT_OUT);                   // MOBILE RESET (IGT)
    }
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_GSM_PCTL_OUT));				// MOBILE POWER
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_GSM_DTR_OUT));				// MOBILE DTR

	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_LED_READY_OUT));			// LED READY
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_STA_GREEN_OUT));			// STATUS GREEN LED
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_STA_RED_OUT));				// STATUS RED LED
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_DEBUG_LED0_OUT));			// LED DEBUG1
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_DEBUG_LED1_OUT));			// LED DEBUG2
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_DEBUG_LED2_OUT));			// LED DEBUG3
	ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_DEBUG_LED3_OUT));			// LED DEBUG4

    // GPIO 포트 초기값 설정
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_SW_RST_OUT));				// PWR RESET

	// 신규 보드는 HIGH, 영국, 스웨덴은 LOW, 구형은 HIGH
#if     defined(__SUPPORT_NZC1__)
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_BATT_SW_OUT));				// BATTERY ENABLE/DISABLE CONTROL
#elif   defined(__SUPPORT_NZC2__)
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_BATT_SW_OUT));				// BATTERY ENABLE/DISABLE CONTROL
#else
    assert(0);
#endif

	if (m_nMobileType != MOBILE_TYPE_CDMA){
		ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_EMERGOFF_OUT));		// GSM RESET
	    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_IGT_OUT));			// GSM IGT 
    }

	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_LED_READY_OUT));			// READY LED OFF
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_STA_GREEN_OUT));			// GREEN LED OFF
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_STA_RED_OUT));				// RED LED OFF

	XDEBUG("** Mobile Power OFF\xd\xa");
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_BATT_CHG_EN_OUT));			// BATT CHARGE DISABLE
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_PCTL_OUT));				// GSM Power Off

	usleep(1000000);

	XDEBUG("** Mobile Power ON\xd\xa");
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_BATT_CHG_EN_OUT));			// BATT CHARGE ENABLE
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_GSM_PCTL_OUT));

	usleep(1000000);

/*
	if (m_nMobileType != MOBILE_TYPE_CDMA)
	{
		XDEBUG("** Mobile Emergency OFF\xd\xa");
		ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_GSM_EMERGOFF_OUT));	// GSM EMERGENCY HIGH
		usleep(4000000);												// 4 seconds
		ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_EMERGOFF_OUT));		// GSM EMERGENCY LOW
	}
*/

	if (m_nMobileType != MOBILE_TYPE_CDMA)
	{
	    XDEBUG("** Mobile IGT or Reset\xd\xa");
		ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_GSM_IGT_OUT));			// IGT HIGH
		usleep(1000000);		
		ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_IGT_OUT));			// IGT LOW
		usleep(1000000);		
		ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_GSM_IGT_OUT));			// IGT HIGH
	}

	XDEBUG("** Mobile DTR Control\xd\xa");
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_GSM_DTR_OUT));				// DTR HIGH
	usleep(1000000);		
	ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_DTR_OUT));				// DTR LOW

	// 상태 모니터링 쓰레드 생성
	m_bExitPending = FALSE;
    if (pthread_create(&m_gpioThreadID, NULL, gpioWatchThread, (void *)this) != 0)
        return FALSE;

	pthread_detach(m_gpioThreadID);
	return TRUE;
}

BOOL CGpioControl::Destroy()
{
	void *	nStatus;

	UninstallHooker();
	if (m_nFD == -1)
		return FALSE;

    m_bExitPending  = TRUE;
	close(m_nFD);

	pthread_join(m_gpioThreadID, &nStatus);
	m_nFD = -1;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CGpioControl functions
//////////////////////////////////////////////////////////////////////

BOOL CGpioControl::InstallHooker(PFNGPIOCALLBACK pfnCallback, void *pParam, void *pArgment)
{
	if (pfnCallback == NULL)
		return FALSE;

	m_Locker.Lock();
	m_pfnCallback	= pfnCallback;
	m_pParam		= pParam;
	m_pArgment		= pArgment;
	m_Locker.Unlock();
	return TRUE;
}

BOOL CGpioControl::UninstallHooker()
{
	if (m_pfnCallback == NULL)
		return FALSE;

	m_Locker.Lock();
	m_pfnCallback	= NULL;
	m_pParam		= NULL;
	m_pArgment		= NULL;
	m_Locker.Unlock();
	return TRUE;
}

BOOL CGpioControl::EnableGpioHooker(BOOL bEnable)
{
	m_Locker.Lock();
	m_bEnableHooker = bEnable;
	m_Locker.Unlock();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CGpioControl monitoring service
//////////////////////////////////////////////////////////////////////

void CGpioControl::RunMonitoring()
{
	struct	timeval	timeout;
	fd_set	rset;
	int		fd, n;
	int		dcd, low, pwr, ring;
	int		heater, door, charge;
	BYTE	oldDCD, oldPWR, oldBAT, oldRING;
	BYTE	oldHEATER, oldDOOR, oldCHARGE;

	fd = m_nFD;
	m_gpioLocker.Lock();
	oldDCD  	= ioctl(fd, GPIO_IOCTL_IN, GP_GSM_DCD_INPUT);
	oldPWR  	= ioctl(fd, GPIO_IOCTL_IN, GP_PWR_FAIL_INPUT);
	oldBAT  	= ioctl(fd, GPIO_IOCTL_IN, GP_LOW_BATT_INPUT);
	oldRING 	= ioctl(fd, GPIO_IOCTL_IN, GP_GSM_RI_INPUT);
	oldDOOR		= ioctl(fd, GPIO_IOCTL_IN, GP_DOOR_OPEN_INPUT);
	oldHEATER	= ioctl(fd, GPIO_IOCTL_IN, GP_TEMP_OS_INPUT);
	oldCHARGE	= ioctl(fd, GPIO_IOCTL_IN, GP_BATT_CHG_STATUS_INPUT);
	m_gpioLocker.Unlock();

	for(;!m_bExitPending;)
	{
		FD_ZERO(&rset);
		FD_SET(fd, &rset);

		timeout.tv_sec	= 3;
		timeout.tv_usec	= 0;

		n = select(fd+1, &rset, NULL, NULL, &timeout);
		if (n == -1)
			break;

		if (FD_ISSET(fd, &rset))
		{
			m_gpioLocker.Lock();
			dcd  	= ioctl(fd, GPIO_IOCTL_IN, GP_GSM_DCD_INPUT);
			pwr  	= ioctl(fd, GPIO_IOCTL_IN, GP_PWR_FAIL_INPUT);
			low  	= ioctl(fd, GPIO_IOCTL_IN, GP_LOW_BATT_INPUT);
			ring 	= ioctl(fd, GPIO_IOCTL_IN, GP_GSM_RI_INPUT);
			door	= ioctl(fd, GPIO_IOCTL_IN, GP_DOOR_OPEN_INPUT);
			heater	= ioctl(fd, GPIO_IOCTL_IN, GP_TEMP_OS_INPUT);
			charge	= ioctl(fd, GPIO_IOCTL_IN, GP_BATT_CHG_STATUS_INPUT);
			m_gpioLocker.Unlock();

			// DCD Check
			if (dcd != oldDCD)
			{
				XDEBUG("GPIO: ------ Mobile DCD %s ------\r\n", dcd == 0 ? "Active" : "Normal");
				m_Locker.Lock();
				if (m_bEnableHooker && (m_pfnCallback != NULL))
					m_pfnCallback(GPIONOTIFY_DCD, dcd, m_pParam, m_pArgment);
				m_Locker.Unlock();
				oldDCD = dcd;
			}

			if (pwr != oldPWR)
			{
				// 100ms 후에도 Power Fail인지 검사한다.
				usleep(100000);
				pwr = ioctl(fd, GPIO_IOCTL_IN, GP_PWR_FAIL_INPUT);
		        if (pwr != oldPWR)
				{
					XDEBUG("GPIO: ------ Power %s signal ------\r\n", pwr == 0 ? "UP" : "DOWN");
					m_Locker.Lock();
					if (m_bEnableHooker && (m_pfnCallback != NULL))
						m_pfnCallback(GPIONOTIFY_PWR, pwr, m_pParam, m_pArgment);
					m_Locker.Unlock();
					oldPWR = pwr;

					// Power restore
					if (pwr == 0)
				    {
						// Battery Charging
						XDEBUG("GPIO: ------ Toggle battery charge ------\r\n");
						ioctl(fd, GPIO_IOCTL_OUT, GPIOLOW(GP_BATT_CHG_EN_OUT));		// BATT CHARGE DISABLE
						usleep(1000000);
						ioctl(fd, GPIO_IOCTL_OUT, GPIOHIGH(GP_BATT_CHG_EN_OUT));	// BATT CHARGE ENABLE
					}
				}
			}

			if (low != oldBAT)
			{
				// 100ms 후에도 Low Battery인지 검사한다.
				usleep(100000);
				low  = ioctl(fd, GPIO_IOCTL_IN, GP_LOW_BATT_INPUT);
			    if (low != oldBAT)
				{
					XDEBUG("GPIO: ------ Low Battery %s signal ------\n", low == 0 ? "Low" : "Normal");
					m_Locker.Lock();
					if (m_bEnableHooker && (m_pfnCallback != NULL))
						m_pfnCallback(GPIONOTIFY_LOWBAT, low, m_pParam, m_pArgment);
					m_Locker.Unlock();
					oldBAT = low;
				}
			}

			if (ring != oldRING)
			{
				XDEBUG("GPIO: ------ Mobile RING signal %s ------\n", ring == 0 ? "Active" : "Normal");
				m_Locker.Lock();
				if (m_bEnableHooker && (m_pfnCallback != NULL))
					m_pfnCallback(GPIONOTIFY_RING, ring, m_pParam, m_pArgment);
				m_Locker.Unlock();
				oldRING = ring;
			}

			// Heater
			if (heater != oldHEATER)
			{
				XDEBUG("GPIO: ------ Heater %s ------\r\n", heater == 0 ? "OFF" : "ON");
				m_Locker.Lock();
				if (m_bEnableHooker && (m_pfnCallback != NULL))
					m_pfnCallback(GPIONOTIFY_HEATER, heater, m_pParam, m_pArgment);
				m_Locker.Unlock();
				oldHEATER = heater;
			}

			// Door open
			if (door != oldDOOR)
			{
				XDEBUG("GPIO: ------ Door %s ------\r\n", door == 0 ? "OPEN" : "CLOSE");
				m_Locker.Lock();
				if (m_bEnableHooker && (m_pfnCallback != NULL))
					m_pfnCallback(GPIONOTIFY_DOOR, door, m_pParam, m_pArgment);
				m_Locker.Unlock();
				oldDOOR = door;
			}

			// Battery charge
			if (charge != oldCHARGE)
			{
				XDEBUG("GPIO: ------ Battery charge %s ------\r\n", charge == 0 ? "OFF" : "CHARGE");
				m_Locker.Lock();
				if (m_bEnableHooker && (m_pfnCallback != NULL))
					m_pfnCallback(GPIONOTIFY_BATTERY, dcd, m_pParam, m_pArgment);
				m_Locker.Unlock();
				oldCHARGE = charge;
			}
		}
	}	
}

//////////////////////////////////////////////////////////////////////
// CGpioControl Status Monitoring Thread
//////////////////////////////////////////////////////////////////////

void *CGpioControl::gpioWatchThread(void *pParam)
{
	CGpioControl	*pThis = (CGpioControl *)pParam;

	pThis->RunMonitoring();
	return 0;
}

