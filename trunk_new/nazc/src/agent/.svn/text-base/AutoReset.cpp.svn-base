//////////////////////////////////////////////////////////////////////
//
//	AutoReset.cpp: implementation of the CAutoReset class.
//
//	This file is a part of the AIMIR.
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
#include "AutoReset.h"
#include "AgentService.h"
#include "EventInterface.h"
#include "Distributor.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "gpio.h"
#include "ShellCommand.h"
 
//////////////////////////////////////////////////////////////////////
// Global Variable Definitions
//////////////////////////////////////////////////////////////////////

CAutoReset	*m_pAutoReset = NULL;

extern BOOL			m_bGprsPerfTestMode;
extern BOOL			m_bPingTestMode;

extern BOOL			m_bEnableAutoReset;				// Auto reset enable/disable
extern TIMESTAMP	m_tmAutoResetTime;				// Auto reset time
extern int			m_nAutoResetInterval;			// Auto reset interval

extern BOOL			m_bEnableKeepaliveEvent;		// Keepalive event enable/disable
extern int			m_nKeepaliveInvterval;			// Keepalive send interval (min)

#if     defined(__PATCH_14_1286__)
extern BOOL    *m_pbPatch_14_1286;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoReset::CAutoReset() : CJobObject("AutoReset", 60*60)
{
	m_pAutoReset = this;
	m_nLastCheck = 0;
	m_nLastReset = 0;
}

CAutoReset::~CAutoReset()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CAutoReset::Initialize()
{
	m_nLastCheck = uptime();
	if (!CTimeoutThread::StartupThread(11))
		return FALSE;

	return TRUE;
}

void CAutoReset::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

//////////////////////////////////////////////////////////////////////
// Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CAutoReset::OnActiveThread()
{
	time_t	now;
	struct	tm	when;
	char	szTime[32];
	int		i, current;
	unsigned long lElapse;
#if     defined(__PATCH_14_1286__)
    int nPowerFail = 0;
#endif

    // OTA가 동작 중이면 수행하지 않는다.
	if (m_pDistributor->IsBusy())
	{
//      XDEBUG("AutoReset: BUSY!!!!!! (Active Distributor)\r\n");
        return TRUE;
	}

	// Auto reset
#if     defined(__PATCH_14_1286__)
    /** Patch 14-1285가 enable 되어 있을 경우에는
     *  PowerFail 상태가 아닐 때만 fixed reset을 수행한다.
     */
	nPowerFail  = GPIOAPI_ReadGpio(GP_PWR_FAIL_INPUT);
	if (m_bEnableAutoReset && (!(*m_pbPatch_14_1286) || !nPowerFail) )
#else
	if (m_bEnableAutoReset)
#endif
	{
		lElapse = (unsigned long)((m_nAutoResetInterval-1) * (60*60*24));
		if ((unsigned long)uptime() > lElapse)
		{
			// 해당 시간대가 되었는지 확인한다.
			time(&now);
			when = *localtime(&now);

			if ((when.tm_hour == m_tmAutoResetTime.hour) && (when.tm_min == m_tmAutoResetTime.min))
			{
				XDEBUG("******* Auto Reset *******\r\n");
				usleep(1000000*60);
				m_pService->SetResetState(RST_AUTORESET);
				m_pService->PostMessage(GWMSG_RESET, NULL);

				// Reset message 전송후 1분동안 죽지 않으면 강제 리셋
				usleep(1000000*60);
				SystemExec("sync");
				usleep(1000000*3);
				for(i=0; i<10; i++)
				{
#if     defined(__INCLUDE_GPIO__)
			   		GPIOAPI_WriteGpio(GP_SW_RST_OUT, 0);
			   		GPIOAPI_WriteGpio(GP_BATT_SW_OUT, 0);
#endif
					usleep(1000000);
				}
				return TRUE;
			}
		}
	}

	// Keepalive Event
	if (!m_bEnableKeepaliveEvent)
		return TRUE;

	current = uptime();
	if ((current-m_nLastCheck) < m_nKeepaliveInvterval*60)
		return TRUE;

	MakeTimeString(szTime, &now, TRUE);
	XDEBUG("--------- KEEPALIVE Event. (TIME=%s) --------\r\n", szTime);
	mobileKeepaliveEvent();

	m_nLastCheck = current;
	return TRUE;
}

BOOL CAutoReset::OnTimeoutThread()
{
	WatchdogClear();

	if (m_bPingTestMode || m_bGprsPerfTestMode)
		return TRUE;

	return OnActiveThread();
}

