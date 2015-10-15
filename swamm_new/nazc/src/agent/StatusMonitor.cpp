#include "common.h"
#include <sys/wait.h>
#include "AgentService.h"
#include "StatusMonitor.h"
#include "SystemMonitor.h"
#include "EventManager.h"
#include "MobileUtility.h"
#include "ShellCommand.h"
#include "SystemUtil.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "gpiomap.h"

#define __TI_AM335X__ //sungyeung
#define __INCLUDE_GPIO__ //sungyeung

CStatusMonitor   *m_pStatusMonitor = NULL;

extern UINT m_nNzcPowerType;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusMonitor::CStatusMonitor()
{
	m_pStatusMonitor 	= this;
	m_nLastLowBattery	= 0;
	m_nLastTime	     	= 0;
	m_nErrorMask	 	= 0;
#if     defined(__SUPPORT_NZC1__) || defined(__SUPPORT_NZC2__)   // Issue #959
	m_nDcdState 	    = 1;
#endif
#if     defined(__TI_AM335X__)
    m_nCaseOpen         = 0;    // Case Close
#endif
    m_nLastPowerFail    = 0;
    m_nLastPowerRestore = 0;
    m_bInitialize = FALSE;
    m_nAdcBattVolt = 0;
    m_nAdcMainVolt = 0;
}

CStatusMonitor::~CStatusMonitor()
{
}

//////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////

#if     defined(__SUPPORT_NZC1__) || defined(__SUPPORT_NZC2__)   // Issue #959
int	CStatusMonitor::GetDcdState()
{
	return m_nDcdState;
}
#endif

int	CStatusMonitor::GetState()
{
	return ((int)(m_nErrorMask & GPIOERR_MASK) > 0) ? 1 : 0;
}
#if     !defined(__EXCLUDE_LOW_BATTERY__)
int	CStatusMonitor::GetLowBat()
{
#if     defined(__INCLUDE_GPIO__)
	return GPIOAPI_ReadGpio(GP_LOW_BATT_INPUT);
#else
    return 0;
#endif
}
#endif

UINT CStatusMonitor::GetErrorMask()
{
	return m_nErrorMask;
}

void CStatusMonitor::SetState(BOOL bSet, int nState)
{
	m_Locker.Lock();
	if (bSet)
		 m_nErrorMask |= nState;
	else m_nErrorMask &= ~nState;	
	m_Locker.Unlock();
	ActiveThread();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CStatusMonitor::Initialize()
{
#if     defined(__INCLUDE_GPIO__)
	int		nPowerFail;
#if     !defined(__EXCLUDE_LOW_BATTERY__)
	int		nLowBattery;
#endif
#if     defined(__TI_AM335X__)
    unsigned int     nBattAdc = 0;
#endif

	time(&m_nLastTime);
	time(&m_nLastLowBattery);
	nPowerFail  = GPIOAPI_ReadGpio(GP_PWR_FAIL_INPUT);
#if     !defined(__EXCLUDE_LOW_BATTERY__)
	nLowBattery = GPIOAPI_ReadGpio(GP_LOW_BATT_INPUT);
#endif
#if     defined(__SUPPORT_NZC1__) || defined(__SUPPORT_NZC2__)   // Issue #959
	m_nDcdState = GPIOAPI_ReadGpio(GP_GSM_DCD_INPUT);
#endif
#if     defined(__TI_AM335X__)
    // Battery Charging resgiter off
    GPIOAPI_WriteGpio(GP_BATT_CHG_EN_OUT, 1); USLEEP(500000);
    // Read Battery ADC Value
    nBattAdc = GPIOAPI_ReadAdc(ADC_BATT_VOL_INPUT);
    // Battery Charging resgiter on
    GPIOAPI_WriteGpio(GP_BATT_CHG_EN_OUT, 0);
    XDEBUG("Check Battery ADC Value %d\r\n", nBattAdc);
    if(nBattAdc < 2000)
    {
        XDEBUG(ANSI_COLOR_RED "Battery Status : Abnormal\r\n" ANSI_NORMAL);
        // Battery Fail
        mcuBatteryFailEvent();
        m_nErrorMask |= GPIONOTIFY_BATTERY_FAIL;
    }
#endif

	m_nErrorMask |= (!(m_nNzcPowerType & NZC_POWER_TYPE_SOLAR) && nPowerFail)  ? GPIONOTIFY_PWR : 0;
#if     !defined(__EXCLUDE_LOW_BATTERY__)
	m_nErrorMask |= !nLowBattery ? GPIONOTIFY_LOWBAT : 0;
#endif

	GPIOAPI_WriteGpio(GP_STA_GREEN_OUT, 0);
#if     defined(__SUPPORT_NZC1__) || defined(__SUPPORT_NZC2__)   // Issue #959
	GPIOAPI_WriteGpio(GP_STA_RED_OUT, 1);
#endif

#endif
    if(codiGetState(GetCoordinator()) == CODISTATE_NORMAL)
    {
		 m_nErrorMask &= ~GPIONOTIFY_COORDINATOR;
    }
    else
    {
		 m_nErrorMask |= GPIONOTIFY_COORDINATOR;
    }

    if(m_nErrorMask)
    {
	    GPIOAPI_WriteGpio(GP_STA_GREEN_OUT, 1); // Abnormal State
    }

	if (!CTimeoutThread::StartupThread(2))
        return FALSE;

#if     defined(__INCLUDE_GPIO__)
	GPIOAPI_InstallHooker(GpioNotify, (void *)this, (void *)NULL);
#endif

    m_bInitialize = TRUE;
	return TRUE;
}

void CStatusMonitor::Destroy()
{
    m_bInitialize = FALSE;
#if     defined(__INCLUDE_GPIO__)
	GPIOAPI_UninstallHooker();
#endif
	CTimeoutThread::ShutdownThread();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CStatusMonitor::Monitoring()
{
	time_t	now;
#if     !defined(__EXCLUDE_LOW_BATTERY__)
	int		nTime;
#endif
    //XDEBUG(" Status Monitor Monitoring \r\n"); 
    //printf(" m_nErrorMask HEX = %x , DEC = %d \n ",  m_nErrorMask, m_nErrorMask); 
    if(codiGetState(GetCoordinator()) == CODISTATE_NORMAL)
    {
		 m_nErrorMask &= ~GPIONOTIFY_COORDINATOR;
    }
    else
    {
		 m_nErrorMask |= GPIONOTIFY_COORDINATOR;
    }

#if     defined(__INCLUDE_GPIO__)
	if (m_nErrorMask != 0)
	{
		GPIOAPI_WriteGpio(GP_STA_GREEN_OUT, 1);
	}
#endif

	    time(&now);
#if     defined(__INCLUDE_GPIO__)
#if     !defined(__EXCLUDE_LOW_BATTERY__)
		//printf(" ErrorMask HEX = %x , ErrorMask DEC = %d \n" , m_nErrorMask, m_nErrorMask);
		nTime = now - m_nLastLowBattery;
		if (m_nErrorMask & GPIONOTIFY_LOWBAT && m_nErrorMask & GPIONOTIFY_PWR)
		{
			printf("\xd\xaTurn off after %0d seconds.\xd\xa", 10 - nTime);

			if (nTime > 10)
			{
				printf("\xd\xa");
                printf("---- Low Battery Detected --\xd\xa");
				printf("---- POWER OFF ----\xd\xa");
				printf("\xd\xa");
				printf("Power off.\xd\xa");
				m_nErrorMask = 0;
				usleep(1000000);
				m_pService->SetResetState(RST_LOWBATT);
				m_pService->PostMessage(GWMSG_SHUTDOWN, 0);
			}
		    usleep(500000);	
		}
#endif
#endif

#if     defined(__INCLUDE_GPIO__)
            LazyPowerMonitoring();
#endif
            if(codiGetState(GetCoordinator()) == CODISTATE_NORMAL)
            {
		        m_nErrorMask &= ~GPIONOTIFY_COORDINATOR;
            }
            else
            {
		        m_nErrorMask |= GPIONOTIFY_COORDINATOR;
            }
	
#if     defined(__INCLUDE_GPIO__)
	        if (m_nErrorMask != 0)
            {
	            GPIOAPI_WriteGpio(GP_STA_GREEN_OUT, 1);
            }
            else
            {
	            GPIOAPI_WriteGpio(GP_STA_GREEN_OUT, 0);
            }
#endif

	return TRUE;
}

void CStatusMonitor::LazyPowerMonitoring()
{
	time_t	now;
    BOOL    bPowerFail = FALSE;
    BOOL    bPowerRestore = FALSE;
    time_t  interval = 3;

    time(&now);
	m_Locker.Lock();
    if(m_nLastPowerFail > 0 && now - m_nLastPowerFail > interval) bPowerFail = TRUE;
    if(m_nLastPowerRestore > 0 && now - m_nLastPowerRestore > interval) bPowerRestore = TRUE;
	m_Locker.Unlock();

    if(bPowerFail) {
	    m_Locker.Lock();
        m_nLastPowerFail = 0;
	    m_Locker.Unlock();
        mcuPowerFailEvent();
    }
    if(bPowerRestore) {
	    m_Locker.Lock();
        m_nLastPowerRestore = 0;
	    m_Locker.Unlock();
		mcuPowerRestoreEvent();
    }
}

void CStatusMonitor::GpioNotify(int nNotify, int nState, void *pParam, void *pArgment)
{
	CStatusMonitor	*pThis;
#if     defined(__SUPPORT_NZC1__) || defined(__SUPPORT_NZC2__)   // Issue #959
#if     defined(__INCLUDE_GPIO__)
	int		nType, dcd;
	int		pid;
#if     !defined(__EXCLUDE_LOW_BATTERY__)
	int		batt;
#endif
#else
	int		nType;
	int		pid;
#endif
#endif // __TI_AM335X__

	pThis = (CStatusMonitor *)pParam;
	switch(nNotify) {
	  case GPIONOTIFY_PWR :
           if(!(m_nNzcPowerType & NZC_POWER_TYPE_SOLAR)) {
		       if (nState == 1)
		       {
			       pThis->m_Locker.Lock();
	               time(&pThis->m_nLastPowerFail);
		   	       pThis->m_nErrorMask |= nNotify;
			       pThis->m_Locker.Unlock();
		       }
		       else
		       {
			       pThis->m_Locker.Lock();
                   if(pThis->m_nLastPowerFail) {
                        pThis->m_nLastPowerFail = 0;
                        pThis->m_nLastPowerRestore = 0;
                   }
	               else time(&pThis->m_nLastPowerRestore);
			       pThis->m_nErrorMask &= ~nNotify;
			       pThis->m_Locker.Unlock();
		       }
		       pThis->ActiveThread();
           }
		   break;
#if     defined(__TI_AM335X__)
      case GPIONOTIFY_DOOR:
           if(pThis->m_nCaseOpen != nState)
           {
                if(nState) // Case Open
                {
                    pThis->m_nErrorMask |= GPIONOTIFY_DOOR;
                    mcuCoverOpenEvent();
                }
                else
                {
                    pThis->m_nErrorMask &= ~GPIONOTIFY_DOOR;
                    mcuCoverCloseEvent();
                }
                pThis->m_nCaseOpen = nState;
           }
           break;
#endif

#if     !defined(__EXCLUDE_LOW_BATTERY__)
	  case GPIONOTIFY_LOWBAT :
		   if (nState == 0)
		   {
			   pThis->m_Locker.Lock();
			   time(&pThis->m_nLastLowBattery);
		   	   pThis->m_nErrorMask |= nNotify;
			   pThis->m_Locker.Unlock();

			   mcuLowBatteryEvent();
		   }
		   else
		   {
               /*-- LOW Battery가 Restore 되어도 특별한 GPIO Handling을 하지 않느다.
                *-- 이미 Shutdown 절차를 수행중이라면 Shutdown이 되게 되고
                *-- 그렇지 않다면 GPIO를 변경하지 않았기 때문에 변경할 부분이 없다. --*/
			   pThis->m_Locker.Lock();
			   pThis->m_nErrorMask &= ~nNotify;
			   pThis->m_Locker.Unlock();

			   mcuLowBatteryRestoreEvent();
		   }
		   pThis->ActiveThread();
		   break;
#endif
      case GPIONOTIFY_BATTERY_CHARG:
		   if (nState == 0)
           {
		   	   pThis->m_nErrorMask |= nNotify;
           }
           else
           {
			   pThis->m_nErrorMask &= ~nNotify;
           }
           break;

#if     defined(__SUPPORT_NZC1__) || defined(__SUPPORT_NZC2__)   // Issue #959
           /** TODO : RING을 Catch 하지 못할 경우 Packet Mode 및 CSD Mode를 사용할 수 없다.
             * TI + AMTelecom 사용시 어떻게 처리할지 여부를 고민해야 한다
             */
	  case GPIONOTIFY_RING :
#if     defined(__INCLUDE_GPIO__)
		   dcd = GPIOAPI_ReadGpio(GP_GSM_DCD_INPUT);
#if     !defined(__EXCLUDE_LOW_BATTERY__)
		   batt = GPIOAPI_ReadGpio(GP_LOW_BATT_INPUT);
#endif

		   XDEBUG("MOBILE: -------- RING(RI) --------\r\n");
#if     !defined(__EXCLUDE_LOW_BATTERY__)
		   XDEBUG("GPIO: BATT=%d, RING=%0d, DCD=%d (%s)\r\n", batt, nState, dcd, dcd == 1 ? "OFF" : "ACTIVE");
#else
		   XDEBUG("GPIO: RING=%0d, DCD=%d (%s)\r\n", nState, dcd, dcd == 1 ? "OFF" : "ACTIVE");
#endif

		   // RING OFF
		   if (nState == 1)
			   break;

           /** Issue #1991: 사용중인 CDMA 모듈이 데이터 통신 이외의 용도로는 사용할 수 없어서
             *     RING만 울리고 DCD는 셋팅되지 않기 때문에 CDMA RING은 무시한다 
             *     구현을 DCD가 Off 가 되는 상태이기 때문에 그 값을 검사하도록 수정한다.
             */
           if(dcd) break;

		   // Send data active
		   if (m_pSystemMonitor->GetCount() != 0)
		   {
			   XDEBUG("MOBILE: ------ Busy (Count) ------\r\n");
			   break;
		   }

		   if (MOBILE_IsBusy())
		   {
			   XDEBUG("MOBILE: ------ Mobile Busy ------\r\n");
			   break;
		   }

		   if (FindProcess("pppd") > 0)
		   {
			   XDEBUG("MOBILE: ------ Busy (ppp) ------\r\n");
			   break;
		   }

		   if (FindProcess("mserver") > 0)
		   {
			   XDEBUG("MOBILE: mserver active. (DCD=%s)\r\n", dcd == 1 ? "OFF" : "ACTIVE");

			   if (dcd == 1)
			   {
			   	   XDEBUG("------ Kill mserver ------\r\n");
			   	   KillProcess("mserver");
				   usleep(3000000);
			   }
			   else
			   {
			   	   XDEBUG("MOBILE: ------ Busy (mserver) ------\r\n");
			   	   break;
			   }
		   }

		   if (FindProcess("send") > 0)
		   {
			   XDEBUG("MOBILE: ------ Busy (send) ------\r\n");
			   break;
		   }

		   XDEBUG("\r\nOAM-MOBILE: ----- circuit call -----\r\n");
		   pid = fork();
		   if (pid < 0)
		   {
			   XDEBUG("fork: fork error!!\r\n");
			   break;
		   }
   	 	   else if (pid == 0)
    	   {
               setsid();
		       GetFileValue("/app/conf/mobile.type", &nType);
			   execl("/app/sw/mserver", "mserver", (nType == MOBILE_TYPE_CDMA) ? "-cdma" : "-gsm", NULL);
        	   exit(0);
		   }
           waitpid(pid,NULL,0);
		   break;
#endif      // Include GPIO

	  case GPIONOTIFY_DCD :
		   pThis->m_Locker.Lock();
		   pThis->m_nDcdState = nState;
		   pThis->m_Locker.Unlock();

		   XDEBUG("MOBILE: ------ Carrier DCD=%0d (%s) ------\r\n", nState, nState == 1 ? "OFF" : "ACTIVE");

		   // DCD Off
		   if (nState == 1)
			   break;

		   // CDMA를 사용중인 경우
		   if (MOBILE_IsBusy())
			   break;

		   // CDMA인 경우에만 DCD 시그널을 이용한다.
		   // CDMA는 RI 시그널이 오지 않으므로 DCD 시그널만으로 동작한다. (M2M의 경우)
		   GetFileValue("/app/conf/mobile.type", &nType);
		   if (nType != MOBILE_TYPE_CDMA)
				break;

		   // Send data active
		   if (m_pSystemMonitor->GetCount() != 0)
		   {
			   XDEBUG("MOBILE: ------ Busy (Count) ------\r\n");
			   break;
		   }

		   // PPP is active
		   if (FindProcess("send") > 0)
		   {
			   XDEBUG("MOBILE: ------ SEND Active ------\r\n");
			   break;
		   }

		   // PPP is active
		   if (FindProcess("pppd") > 0)
		   {
			   XDEBUG("MOBILE: ------ PPP Active ------\r\n");
			   break;
		   }

		   XDEBUG("\r\nOAM-MOBILE: Answer a circuit call.\r\n");
		   pid = fork();
		   if (pid < 0)
		   {
			   XDEBUG("fork: fork error!!\r\n");
			   break;
		   }
   	 	   else if (pid == 0)
    	   {
               setsid();
			   execl("/app/sw/mserver", "mserver", (nType == MOBILE_TYPE_CDMA) ? "-cdma" : "-gsm", NULL);
        	   exit(0);
		   }
           waitpid(pid,NULL,0);
		   break;
#else       // __TI_AM335X__
      case GPIONOTIFY_BATT_ADC:
           pThis->m_nAdcBattVolt = nState;
           break;
      case GPIONOTIFY_MAIN_ADC:
           pThis->m_nAdcMainVolt = nState;
           break;
#endif      // End of Core
	}
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CStatusMonitor::OnActiveThread()
{
	return Monitoring();
}

BOOL CStatusMonitor::OnTimeoutThread()
{
    if(!m_bInitialize) return TRUE;

    LazyPowerMonitoring();

	if (m_nErrorMask != 0) 
        OnActiveThread();

	return TRUE;
}

