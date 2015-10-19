//////////////////////////////////////////////////////////////////////
//
//	SensorTimesync.cpp: implementation of the CSensorTimesync class.
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
#include "codiapi.h"
#include "SensorTimesync.h"
#include "AgentService.h"
#include "Distributor.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "gpiomap.h"
 
//////////////////////////////////////////////////////////////////////
// Global Variable Definitions
//////////////////////////////////////////////////////////////////////

#define     RENEW_TIME      23

CSensorTimesync	*m_pSensorTimesync = NULL;

extern BOOL		m_bGprsPerfTestMode;
extern BOOL		m_bPingTestMode;
extern BOOL     m_bEnableHighRAM;

extern int		m_nSystemID;
extern int		m_nSystemType;
extern BOOL		m_bActiveRetry;
extern int		m_nGroupInterval;
extern int		m_nRetryStartSecond;

extern BOOL		m_bEnableTimeBroadcast;
extern int     	m_nTimeBroadcastInterval;                                          
extern BOOL		m_bEnableLowBattery;
#if     defined(__PATCH_1_1540__)
extern BOOL    *m_pbPatch_1_1540;
#endif
extern BYTE     m_nCoordinatorFwVersion;
extern BYTE     m_nCoordinatorFwBuild;
extern BYTE     m_nTimeSyncStrategy;
extern BYTE     m_nOpMode;
extern short    m_nTimeZone;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSensorTimesync::CSensorTimesync()
{
	m_pSensorTimesync = this;
	m_bDirect 		  = FALSE;
	m_bMtorReq 		  = FALSE;
    m_bTrActive       = FALSE;
    m_bDiscovery      = FALSE;
    m_nNeedBroadcastTime = 0;
}

CSensorTimesync::~CSensorTimesync()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CSensorTimesync::Initialize()
{
	time(&m_tmLastTimesync);
	time(&m_tmLastMtor);
	if (!CTimeoutThread::StartupThread(5))
		return FALSE;

	return TRUE;
}

void CSensorTimesync::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

void CSensorTimesync::SyncDiscovery(void)
{
	CODI_ROUTEDISCOVERY_PAYLOAD	discovery;
    int nError, nLength;

    m_TimeLocker.Lock();
    {
        nError = codiGetProperty(GetCoordinator(), CODI_CMD_ROUTE_DISCOVERY, (BYTE *)&discovery, &nLength, 3000);
        if (nError == CODIERR_NOERROR)
        {
            m_bDiscovery = discovery.type ? TRUE : FALSE;
        }
    }
    m_TimeLocker.Unlock();
}

void CSensorTimesync::Timesync(BYTE nTimeSyncType)
{
    m_TimeLocker.Lock();
    {
	    m_bDirect = TRUE;
    }
    m_TimeLocker.Unlock();

	m_nTimeSyncType = nTimeSyncType;
	CTimeoutThread::ActiveThread();
}

void CSensorTimesync::TimesyncSensor(HANDLE codi, BOOL bDirect, BYTE nTimeSyncType)
{
	ENDI_TIMESYNC_PAYLOAD *pTimesync;
	BYTE	        payload[256];
	char	        szTime[30];
	int		        nError;
    time_t          now;
	struct	tm		when;

	time(&now);
	when = *localtime(&now);

    /** #1182 : 2006년 이전이면 timesync를 하지 않도록 한다 
     *          이때 만약 사용자 요청에 의한 timesync라면 시스템 시간에 상관없이 
     *          무조건 timesync를 수행한다. **/
    if(!bDirect && when.tm_year + 1900 < 2006) return;

	MakeTimeString(szTime, &now, TRUE);
	XDEBUG("SENSOR-TIMESYNC: Multicast time (TIME=%s) (%s) (%c)\r\n", szTime, 
            nTimeSyncType ? "SHORT" : "LONG", bDirect ? 'M' : 'A');

	pTimesync = (ENDI_TIMESYNC_PAYLOAD *)&payload;
#ifdef  __USE_BSD
	pTimesync->gmt.tz	= ntoshort((int)(when.tm_gmtoff/60));
#else
	pTimesync->gmt.tz	= ntoshort(m_nTimeZone);
#endif
	pTimesync->gmt.dst	= ntoshort(0);
	pTimesync->gmt.year	= ntoshort(when.tm_year + 1900);
	pTimesync->gmt.mon	= (BYTE)when.tm_mon + 1;
	pTimesync->gmt.day	= (BYTE)when.tm_mday;
	pTimesync->gmt.hour	= (BYTE)when.tm_hour;
	pTimesync->gmt.min	= (BYTE)when.tm_min;
	pTimesync->gmt.sec	= (BYTE)when.tm_sec;
	pTimesync->nazc     = ntoint(m_nSystemID);		// NAZC 번호
    pTimesync->type     = nTimeSyncType;

	nError = codiMulticast(codi, ENDI_DATATYPE_TIMESYNC, (BYTE *)&payload, sizeof(ENDI_TIMESYNC_PAYLOAD));
	if (nError != CODIERR_NOERROR)
	{
		XDEBUG("---------------------------------------------\r\n");
		XDEBUG("SENSOR-TIMESYNC-ERROR: %s(%d)\r\n", codiErrorMessage(nError), nError);
		XDEBUG("---------------------------------------------\r\n");
	}else if(!bDirect) {
        m_tmLastTimesync = now;
    } 
}

void CSensorTimesync::MtorSensor(HANDLE codi)
{
	CODI_MANYTOONE_PAYLOAD *pManyToOne;
	BYTE	        payload[256];
	int		        nError;
    BYTE            mtorType = CODI_MANYTOONE_REQUEST;

    /** Issue #1965: HIGH/LOW RAM Coordinator 지원
      *
      * Coordinator v2.1 b16, v2.2 b9 이상에서만 동작한다
      */
    if((m_nCoordinatorFwVersion == 0x21 && m_nCoordinatorFwBuild > 0x15) ||
            (m_nCoordinatorFwVersion == 0x22 && m_nCoordinatorFwBuild > 0x08) ||
             m_nCoordinatorFwVersion > 0x22) {
        if(m_bEnableHighRAM) mtorType = CODI_MANYTOONE_HIGH_RAM;
        else mtorType = CODI_MANYTOONE_LOW_RAM;
    }

	XDEBUG("SENSOR-TIMESYNC: Many to one network (%s)\r\n",
        mtorType == CODI_MANYTOONE_HIGH_RAM ? "HIGH RAM" : "LOW RAM");

	pManyToOne 		   = (CODI_MANYTOONE_PAYLOAD *)&payload;
	pManyToOne->type   = mtorType;
	pManyToOne->radius = ((COORDINATOR *)codi)->max_hops;		// max hops

	nError = codiSetProperty(codi, CODI_CMD_MANY_TO_ONE_NETWORK,
	        (BYTE *)&payload, sizeof(CODI_MANYTOONE_PAYLOAD), 3000);
	if (nError != CODIERR_NOERROR)
	{
	    XDEBUG("---------------------------------------------\r\n");
	    XDEBUG("SENSOR-MANY-TO-ONE-ERROR: %s(%d)\r\n", codiErrorMessage(nError), nError);
	    XDEBUG("---------------------------------------------\r\n");
	}

}

BOOL wait_time(struct tm * when, int atSec)
{
	time_t			now;

    while(when->tm_sec < atSec) {
        usleep(500000);
	    time(&now);
	    when = localtime(&now);
    }

    return when->tm_sec > atSec ? FALSE : TRUE;
}

int CSensorTimesync::RenewNetwork(BYTE nSec)
{
    BYTE            buffer[256];
    ENDI_COMMAND_PAYLOAD  *payload;

    payload = (ENDI_COMMAND_PAYLOAD *) &buffer;
    payload->cmd_type = (BYTE) ENDI_CMD_SET_ACTIVE_KEEP_TIME;
    payload->frame_type = 0x00;
    payload->data[0] = nSec;

    return codiMulticast(GetCoordinator(), ENDI_DATATYPE_COMMAND, (BYTE *)&buffer, 
                sizeof(payload->cmd_type) + sizeof(payload->frame_type) + sizeof(BYTE));
}

void CSensorTimesync::BroadcastTime(HANDLE codi)
{
    XDEBUG("BROADCAST-TIME: Network Time Synchronize\r\n");
    m_TimeLocker.Lock();
    {
        m_bDirect = FALSE;
    }
    m_TimeLocker.Unlock();
    TimesyncSensor(codi, FALSE, TIMESYNC_TYPE_SHORT);
}

void CSensorTimesync::NeedBroadcastTime(void)
{
    m_TimeLocker.Lock();
    {
        if(!m_nNeedBroadcastTime) time(&m_nNeedBroadcastTime);
    }
    m_TimeLocker.Unlock();
}

BOOL CSensorTimesync::IsNeedBroadcastTime(void)
{
    BOOL        bOk = FALSE;
    cetime_t    nCurrent;

    time(&nCurrent);
    m_TimeLocker.Lock();
    {
        if(m_nNeedBroadcastTime && (nCurrent - m_nNeedBroadcastTime) > 9) {
            bOk = TRUE;
            m_nNeedBroadcastTime = 0;
        }
    }
    m_TimeLocker.Unlock();
    
    return bOk;
}

BOOL CSensorTimesync::IsUserRequest(void)
{
    BOOL        bOk = FALSE;

    m_TimeLocker.Lock();
    {
        if(m_bDirect) bOk = TRUE;
    }
    m_TimeLocker.Unlock();
    
    return bOk;
}


//////////////////////////////////////////////////////////////////////
// Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CSensorTimesync::OnActiveThread()
{
	HANDLE			codi;
	time_t			now;
	struct	tm		when;
	int				nState;
    BOOL            bDirect = m_bDirect;

    m_TimeLocker.Lock();
    {
        m_bDirect = FALSE;
    }
    m_TimeLocker.Unlock();
    m_bMtorReq = FALSE;

	codi = GetCoordinator();
	if (codi == NULL)
		return TRUE;

	nState = codiGetState(codi);
	if (nState != CODISTATE_NORMAL)
	{
		XDEBUG("SENSOR-TIMESYNC: Coordinator not ready. (nState=%d)\r\n", nState);
		return TRUE;
	}

    if(bDirect) {
        MtorSensor(codi);
        switch(m_nTimeSyncStrategy) {
            case TIMESYNC_STRATEGY_HIBRID_SLEEPY_MESH:
            case TIMESYNC_STRATEGY_SLEEPY_MESH:
                usleep(2000000);
                break;
            case TIMESYNC_STRATEGY_HIBRID_MESH:
            default:
                switch(m_nOpMode) {
                    default:
                        usleep(8000000);
                    case OP_MODE_TEST:
                        usleep(2000000);
                        break;
                }
                break;
        }
        TimesyncSensor(codi, bDirect, m_nTimeSyncType);
        return TRUE;
    }

    /** 배포 작업중일 때는 Schedule에 의한 Timesync를 보내지 않는다 */
    if(m_pDistributor->IsBusy()) return TRUE;

	time(&now);
	when = *localtime(&now);

    switch(m_nTimeSyncStrategy) {
        case TIMESYNC_STRATEGY_HIBRID_SLEEPY_MESH:
        	XDEBUG(" +++++++++++++++++++++++++++++TIMESYNC_STRATEGY_HIBRID_SLEEPY_MESH : %s : %d \n", __FILE__, __LINE__);
            switch(when.tm_min % 15) {
                case 0:
                case 2:
                    m_bTrActive = FALSE;
                    if(!m_bDiscovery) 
                    {
                        RouteDiscoveryControl(TRUE);
                        SyncDiscovery();
                    }
                    if(wait_time(&when, 5)) MtorSensor(codi);
                    if(wait_time(&when, 7)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_SHORT);
                    if(wait_time(&when, 10) && IsUserRequest()) break;
                    if(wait_time(&when, 15)) MtorSensor(codi);
                    if(wait_time(&when, 17)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_SHORT);
                    if(wait_time(&when, 20) && IsUserRequest()) break;
                    if(wait_time(&when, 25)) MtorSensor(codi);
                    if(wait_time(&when, 27)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_SHORT);
                    if(wait_time(&when, 30) && IsUserRequest()) break;

                    /** Issue #2411 : 35초에 RenewNetwork을 검사하던것을 37초까지 Ondemand을 하는 것으로 수정 */
                    if(wait_time(&when, 37)) {
                            m_pTransactionManager->ResolveTimeoutTransaction();
                            MtorSensor(codi);
                            if(wait_time(&when, 39)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_SHORT);
                    }

                    break;
                case 4:
                    if(m_bDiscovery) 
                    {
                        RouteDiscoveryControl(FALSE);
                        SyncDiscovery();
                    }
                    if(wait_time(&when, 2)) MtorSensor(codi);
                    if(m_nOpMode == OP_MODE_TEST) {
                        if(wait_time(&when, 4)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_SHORT);
                        if(wait_time(&when, 12) && IsNeedBroadcastTime()) BroadcastTime(codi); else if(IsUserRequest()) break;
                    }else {
                        if(wait_time(&when, 12)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_LONG);
                    }
                    if(wait_time(&when, 22) && IsNeedBroadcastTime()) BroadcastTime(codi); else if(IsUserRequest()) break;
                    if(wait_time(&when, 32) && IsNeedBroadcastTime()) BroadcastTime(codi); else if(IsUserRequest()) break;
                    if(wait_time(&when, 42) && IsNeedBroadcastTime()) BroadcastTime(codi); else if(IsUserRequest()) break;
                    if(wait_time(&when, 52) && IsNeedBroadcastTime()) BroadcastTime(codi); else if(IsUserRequest()) break;
                    break;
                case 14:
                    if(m_bDiscovery) 
                    {
                        RouteDiscoveryControl(FALSE);
                        SyncDiscovery();
                    }
                    if(wait_time(&when, 5) && IsNeedBroadcastTime()) BroadcastTime(codi); else if(IsUserRequest()) break;
                    if(wait_time(&when, 15) && IsNeedBroadcastTime()) BroadcastTime(codi); else if(IsUserRequest()) break;
                    if(wait_time(&when, 25) && IsNeedBroadcastTime()) BroadcastTime(codi); else if(IsUserRequest()) break;
                    if(wait_time(&when, 35) && IsNeedBroadcastTime()) BroadcastTime(codi); else if(IsUserRequest()) break;
                    if(wait_time(&when, 45)) {
                        m_pBatchJob->RemoveAllTransaction();
                        m_pTransactionManager->ResolveTimeoutTransaction();
                    }
                    break;
                default:
                    if(m_bDiscovery) 
                    {
                        RouteDiscoveryControl(FALSE);
                        SyncDiscovery();
                    }
                    if(IsNeedBroadcastTime()) BroadcastTime(codi); 
                    break;
            }
            break;
        case TIMESYNC_STRATEGY_SLEEPY_MESH:
            if(!(when.tm_min % 2)) {
                m_bTrActive = FALSE;
                if(!m_bDiscovery) 
                {
                    RouteDiscoveryControl(TRUE);
                    SyncDiscovery();
                }
                if(wait_time(&when, 5)) MtorSensor(codi);
                if(wait_time(&when, 7)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_SHORT);
                if(wait_time(&when, 10) && IsUserRequest()) break;
                if(wait_time(&when, 15)) MtorSensor(codi);
                if(wait_time(&when, 17)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_SHORT);
                if(wait_time(&when, 20) && IsUserRequest()) break;
                if(wait_time(&when, 25)) MtorSensor(codi);
                if(wait_time(&when, 27)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_SHORT);
                /** Issue #2411 : 35초에 RenewNetwork을 검사하던것을 37초까지 Ondemand을 하는 것으로 수정 */
                if(wait_time(&when, 37)) {
                    m_pTransactionManager->ResolveTimeoutTransaction();
                    MtorSensor(codi);
                    if(wait_time(&when, 39)) TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_SHORT);
                }

            }
            break;
        case TIMESYNC_STRATEGY_HIBRID_MESH:
        default:
            switch(when.tm_min) {
                case 0:
                case 15:
                case 30:
                    if(m_nOpMode == OP_MODE_TEST && wait_time(&when, 4))
                    {
                    	XDEBUG(" +++++++++++++++++++++++++++++m_nOpMode == OP_MODE_TEST && wait_time(&when, 4) : %s : %d \n",__FILE__, __LINE__);
                        TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_LONG);
                    }
                    break;
		case 45:
			if (wait_time(&when, 2))
			{
			XDEBUG(" +++++++++++++++++++++++++++++(wait_time(&when, 2)) : %s : %d \n",__FILE__, __LINE__);
			MtorSensor(codi);
			}
			if (m_nOpMode == OP_MODE_TEST)
			{
				if (wait_time(&when, 4))
				{
					XDEBUG(" +++++++++++++++++++++++++++++(m_nOpMode == OP_MODE_TEST) : %s : %d \n",__FILE__, __LINE__);
					TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_LONG);
				}
			}
			else
			{
				if (wait_time(&when, 12))
				{
					XDEBUG(" +++++++++++++++++++++++++++++(wait_time(&when, 12)) : %s : %d \n",__FILE__, __LINE__);
					TimesyncSensor(codi, bDirect, TIMESYNC_TYPE_LONG);
				}
			}
			break;
            }
            break;
    }

	return TRUE;
}

BOOL CSensorTimesync::OnTimeoutThread()
{
    /*-- Load가 있을 때도 반드시 timesync를 수행하도록 한다.
	if (m_bGprsPerfTestMode || m_bPingTestMode
	    || m_pMetering->IsBusy() || m_pRecovery->IsBusy() || m_pMeterReader->IsBusy())
		return TRUE;
    --*/

	return OnActiveThread();
}

