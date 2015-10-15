//////////////////////////////////////////////////////////////////////
//
//	SystemMonitor.cpp: implementation of the CSystemMonitor class.
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
#include "SystemMonitor.h"
#include "AgentService.h"
#include "StatusMonitor.h"
#include "Utility.h"
#include "IcmpPing.h"
#include "PingClient.h"
#include "CommonUtil.h"
#include "Utility.h"
#include "SystemUtil.h"
#include "DebugUtil.h"
#include "gpiomap.h"
#include "version.h"
#include "ShellCommand.h"

#include "AgentLog.h"
 
//////////////////////////////////////////////////////////////////////
// Global Variable Definitions
//////////////////////////////////////////////////////////////////////

#define STATE_INIT				0
#define STATE_RUN_PPP			1
#define STATE_CONNECT_WAIT		2
#define STATE_RUNNING			3	
#define STATE_TERM_WAIT			4

#define STATE_RESET				0
#define STATE_RUN_PPP			1
#define STATE_FIND_PPP			2
#define STATE_CONNECT			3
#define STATE_CHECK				4
#define STATE_PING				5
//#define STATE_RECONNECT			6
#define STATE_GSM_MODE			8
#define STATE_CHECK_GPRS		9

CSystemMonitor	*m_pSystemMonitor = NULL;

extern int		m_nEthernetType;
extern int		m_nMobileType;
extern int		m_nMobileMode;

extern int		m_nSystemType;
extern char		m_szFepServer[];
extern int		m_nFepPort;
extern BOOL		m_bEnableMobileLog;

extern int		m_nMobileLiveCheckMethod;
extern int		m_nMobileLiveCheckInterval;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemMonitor::CSystemMonitor() : CJobObject("SystemMonitor", 60 * 10)
{
	m_pSystemMonitor = this;
	m_bConnected 	 = FALSE;
	m_nAliveCount	 = 0;
	m_bGsmMode		 = FALSE;
	m_bCheck	 	 = FALSE;
}

CSystemMonitor::~CSystemMonitor()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CSystemMonitor::IsConnected()
{
	return m_bConnected;
}

BOOL CSystemMonitor::IsGsmMode()
{
	return m_bGsmMode;
}

int CSystemMonitor::GetCount()
{
	return m_nAliveCount;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CSystemMonitor::Initialize()
{
	if (m_nSystemType == MCU_TYPE_SLAVE)
		m_bGsmMode = TRUE;

	if (!CTimeoutThread::StartupThread(2))
		return FALSE;
	return TRUE;
}

void CSystemMonitor::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

BOOL CSystemMonitor::NewConnection()
{
	int		i;

	if (IsConnected())
	{
		m_Locker.Lock();
		m_nAliveCount++;
		m_Locker.Unlock();
		return TRUE;
	}

	m_Locker.Lock();
	m_nAliveCount++;
	m_Locker.Unlock();

	ActiveThread();

	for(i=0; i<180; i++)
	{
		USLEEP(1000000);

		if (IsConnected())
			return TRUE;
	}
	return FALSE;
}

void CSystemMonitor::DeleteConnection()
{
	m_Locker.Lock();
	m_nAliveCount--;
	if (m_nAliveCount < 0)
		m_nAliveCount = 0;
	m_Locker.Unlock();
}

void CSystemMonitor::CheckConnection()
{
	m_bCheck = TRUE;
}

//////////////////////////////////////////////////////////////////////
// Override Functions
//////////////////////////////////////////////////////////////////////

void CSystemMonitor::PowerResetModem(BOOL bWait)
{   
    XDEBUG("#Mobile Power Reset.\xd\xa");
    GPIOAPI_WriteGpio(GP_GSM_PCTL_OUT, GPIO_LOW);
    USLEEP(1000000);
    GPIOAPI_WriteGpio(GP_GSM_PCTL_OUT, GPIO_HIGH);
    USLEEP(500000);
}

void CSystemMonitor::ResetModem()
{
	XDEBUG("#Mobile Module Type : %s\r\n", MOBILE_MODULE_TYPE_GE910);

	XDEBUG("#Mobile Module Reset.\xd\xa");
	GPIOAPI_WriteGpio(GP_GSM_RST_OUT, GPIO_HIGH);
	USLEEP(1000000);
	GPIOAPI_WriteGpio(GP_GSM_RST_OUT, GPIO_LOW);
	USLEEP(1000000);

	/** TI AM335x는 DTSS800, MC55 를 지원하지 않는다 */
	if (!strcasecmp(MOBILE_MODULE_TYPE_AME5210, "MOBILE_TYPE_GSM")) {
		// AM Telecom
		GPIOAPI_WriteGpio(GP_GSM_PCTL_OUT, GPIO_HIGH);
	}
	// 2014.11.14 : HW실 김재훈차장과 통화후 제거
	// 이미 전원이 들어가 있는 상태에서는 GSM Reset만 하면 된다고 함
	// 2015.04.28  시험 결과 GSM_RESET 시 Power control이 같이 이루어 지고
	// 있기 때문에 GE910/UE910에서는 GP_GSM_RST_OUT을 수행하지 않는다
	else if (!strcasecmp(MOBILE_MODULE_TYPE_GE910, MOBILE_MODULE_TYPE_GE910)
			|| !strcasecmp(MOBILE_MODULE_TYPE_UE910,
					MOBILE_MODULE_TYPE_GE910)) {
		XDEBUG("#Mobile Module Software Reset HIGH\r\n");
		GPIOAPI_WriteGpio(GP_GSM_INIT_PCTL_OUT, GPIO_HIGH);
		USLEEP(5000000);
		XDEBUG("#Mobile Module Software Reset LOW\r\n");
		GPIOAPI_WriteGpio(GP_GSM_INIT_PCTL_OUT, GPIO_LOW);
	}
}   

void CSystemMonitor::ResetSystem()
{
    SystemExec("sync");
    SystemExec("sync");
    SystemExec("sync");
    SystemExec("reboot");
}

BOOL CSystemMonitor::Ping(char *pszAddress)
{
	switch(m_nMobileLiveCheckMethod) {
	  case 1 :
		   return IF4_Ping(m_szFepServer, m_nFepPort, 5, 3);
	  case 2 :
		   if (ICMP_Ping(m_szFepServer, 5, 5))
			   return TRUE;
		   return IF4_Ping(m_szFepServer, m_nFepPort, 5, 3);
	  case 0 :
	  default :
		   return ICMP_Ping(m_szFepServer, 5, 5);
	}
	return FALSE;
}

BOOL CSystemMonitor::IF4_Ping(char *pszAddress, int nPort, int nTimeout, int nRetry)
{
	CPingClient	ping;
	BOOL		bResult;

    XDEBUG("IF4PING: IF4 Pinging %s:%d\xd\xa", pszAddress, nPort);
	bResult = ping.Ping(pszAddress, nPort, nTimeout, nRetry);
    XDEBUG("IF4PING: %s %s:%d\xd\xa", bResult ? "Reply from" : "Request timed out.",
					pszAddress, nPort);
	return bResult;
}

BOOL CSystemMonitor::ICMP_Ping(char *pszAddress, int nTimeout, int nRetry)
{        
    CIcmpPing   ping;
    BOOL        bLive = FALSE;
    int         i, ret;
    struct      timeval atime;
    struct      sockaddr_in addr;
        
    XDEBUG("ICMP: Pinging %s\xd\xa", pszAddress);
    ping.IcmpInit();
    for(i=0; (i<nRetry) && !bLive; i++)
    {
        ret = ping.CheckIcmp(pszAddress, &atime, &addr, nTimeout*100);
        if (ret != -1)
        {
            XDEBUG("ICMP: Reply from %s: %d sec %d msc.\xd\xa",
                    pszAddress, atime.tv_sec, atime.tv_usec);
            bLive = TRUE;
            break;
        }   
        
        XDEBUG("ICMP: Request timed out. (%s)\xd\xa", pszAddress);
    }
    ping.IcmpClose();
    return bLive;
}

int CSystemMonitor::GetCSQ()
{
	FILE	*fp;
	char	szBuffer[128];
	char	szCSQ[16] = "";
	int		nCSQ = 0;

	fp = fopen("/app/sw/mobileinfo", "rt");
	if (fp != NULL)
	{
		while(fgets(szBuffer, 127, fp))
		{
			szBuffer[strlen(szBuffer)-1] = '\0';
			if (strncmp(szBuffer, "+CSQ: ", 6) == 0)
			{
				strcpy(szCSQ, &szBuffer[6]);
				if (atoi(szCSQ) <= 31)
					 nCSQ = -(113-(atoi(szCSQ)*2));
				nCSQ = MIN(0, nCSQ);
			}
		}
		fclose(fp);
	}
	return nCSQ;
}

void CSystemMonitor::GetMobileInfo(char *id, BOOL *auth, int *nCSQ)
{
	FILE	*fp;
	char	szBuffer[128];
	char	szCSQ[16] = "";
	int		n = 0;

	strcpy(id, "");
	*auth = FALSE;
	*nCSQ = 0;

	fp = fopen("/app/sw/mobileinfo", "rt");
	if (fp != NULL)
	{
		while(fgets(szBuffer, 127, fp))
		{
			szBuffer[strlen(szBuffer)-1] = '\0';
            if (strncmp(szBuffer, "^SCID: ", 7) == 0)
                strcpy(id, &szBuffer[7]);
            else if (strncmp(szBuffer, "+CPIN: ", 7) == 0)
			{
				if (strcmp(&szBuffer[7], "READY") == 0)
				     *auth = TRUE;
			    else *auth = FALSE;
			}
			else if (strncmp(szBuffer, "+CSQ: ", 6) == 0)
			{
				strcpy(szCSQ, &szBuffer[6]);
				if (atoi(szCSQ) <= 31)
					 n = -(113-(atoi(szCSQ)*2));
				*nCSQ = MIN(0, n);
			}
		}
		fclose(fp);
	}
}

const char *CSystemMonitor::GetMobileError(int nError)
{
	switch(nError) {
	  case MOBERR_NOERROR :			return "No error.";
	  case MOBERR_PPP_NOT_FOUND :	return "PPPD is not active.";
	  case MOBERR_DCD_OFF :			return "DCD off";
	  case MOBERR_PING_ERROR :		return "Ping error";
	}
	return "Unknown";
}

void CSystemMonitor::AddMobileLog(int nType, int nError)
{
	UINT		addr, subnet, gateway;
	BYTE		phyAddr[16];
	BYTE		*p = (BYTE *)&addr;
	BYTE		*p1 = (BYTE *)&subnet;
	BYTE		*p2 = (BYTE *)&gateway;
	char		szID[32];
	char		szBuffer[128];
	BOOL		bAuth;
	int			nCSQ;

	if (!m_bEnableMobileLog)
		return;

	switch(nType) {
	  case 0 :
		   GetMobileInfo(szID, &bAuth, &nCSQ);
		   GetEthernetInfo(szBuffer, &addr, &subnet, &gateway, phyAddr);
  		   MOBILE_LOG("PPP Connected. (FEP=%s)\n" 
		              "  SIMID=%s, AUTH=%s, CSQ=%d\n"
		              "  ip=%d.%d.%d.%d, subnet=%d.%d.%d.%d, gateway=%d.%d.%d.%d\n",
                        m_szFepServer,
					    szID,
					    bAuth ? "READY" : "ERROR",
					    nCSQ,
					    p[0] & 0xff, p[1] & 0xff, p[2] & 0xff, p[3] & 0xff,
					    p1[0] & 0xff, p1[1] & 0xff, p1[2] & 0xff, p1[3] & 0xff,
					    p2[0] & 0xff, p2[1] & 0xff, p2[2] & 0xff, p2[3] & 0xff);
		   break;

 	  case 1 :
		   MOBILE_LOG("PPP Disconnected (FEP=%s).\n" 
                   "  Reason=%s (%d)\n",
                        m_szFepServer,
                        GetMobileError(nError), nError);			
		   break;

	  case 2 :
		   GetMobileInfo(szID, &bAuth, &nCSQ);
  		   MOBILE_LOG("Power Reset (FEP=%s)\n" 
                   "  SIMID=%s, AUTH=%s, CSQ=%d\n",
                        m_szFepServer,
					    szID, bAuth ? "READY" : "ERROR", nCSQ);
		   break;

	  case 3 :
		   GetMobileInfo(szID, &bAuth, &nCSQ);
  		   MOBILE_LOG("GSM Mode (FEP=%s)\n"
		            "  SIMID=%s, AUTH=%s, CSQ=%d\n",
					    m_szFepServer,
					    szID, bAuth ? "READY" : "ERROR", nCSQ);
		   break;
	}
}

void CSystemMonitor::KeepConnection()
{
	char	szName[16];
	int		nState, nRetry, pid=0;
	UINT	addr, subnet, gateway;
	BYTE	phyAddr[16], *p = (BYTE *)&addr;
	time_t	last, now, tmConnect;
	int		nError = 0;
	int		nPowerReset = 0;

	time(&last);
	time(&tmConnect);

	nState = STATE_RUN_PPP;
	nRetry = 0;

	for(;;)
	{
		WatchdogClear();

		// GPRS Mode
		switch(nState) {
		  case STATE_RESET :
			   XDEBUG("PPP(%0d) - STATE_RESET - Power Reset.\r\n", nState);
               MOBILE_LOG("Mobile Power Reset\r\n");
			   USLEEP(1000000 * 60 * 3);
			   PowerResetModem(TRUE);
			   USLEEP(1000000 * 10);
			   nState = STATE_RUN_PPP;
			   break;

		  case STATE_RUN_PPP :
			   XDEBUG("PPP(%0d) - STATE_RUN_PPP\r\n", nState);
               MOBILE_LOG("Execute PPP daemon\r\n");
			   unlink("/var/log/messages");
			   ResetModem();
			   USLEEP(1000000);
			   ExecutePPPD();
			   pid = 0;
			   time(&last);
			   nState = STATE_FIND_PPP;
			   break;

		  case STATE_FIND_PPP :
			   XDEBUG("PPP(%0d) - STATE_FIND_PPP\r\n", nState);
			   USLEEP(2000000);
			   pid = FindProcess("pppd");
			   if (pid <= 0)
			   {
				   XDEBUG("PPP is not active. (pid=%0d)\r\n", pid);
				   MOBILE_LOG("PPP is not active. (pid=%0d)\r\n", pid);
				   nState = STATE_RETRY;
			  	   break;
			   }
			   nState = STATE_CONNECT;
			   break;

		  case STATE_CONNECT :
			   XDEBUG("PPP(%0d) - STATE_CONNECT (pid=%0d)\r\n", nState, pid);
			   time(&now);
			   if ((now-last) > 60)
			   {
			   	   XDEBUG("PPP(%0d) - STATE_CONNECT (time=%0d)\r\n", nState, now-last);
			   	   MOBILE_LOG("Waiting timeout (time=%0d)\r\n", nState, now-last);
				   nState = STATE_RETRY;
				   break;
			   }

			   if (!IsPid(pid))
			   {
			   	   XDEBUG("PPP(%0d) - STATE_CONNECT - pid not found. (pid=%0d)\r\n", nState, pid);
			   	   MOBILE_LOG("Pid not found. (pid=%0d)\r\n", pid);
				   nState = STATE_RETRY;
				   break;
			   }

			   GetEthernetInfo(szName, &addr, &subnet, &gateway, phyAddr);
		   	   if ((strncmp(szName, "ppp", 3) != 0) || (addr == 0))
			   {
				   USLEEP(5000000);
				   break;
			   }

			   XDEBUG("\r\nPPP Connected.\r\n", szName);
			   XDEBUG("Name=%s, IP=%d.%d.%d.%d\r\n",
					  szName, p[0] & 0xff, p[1] & 0xff, p[2] & 0xff, p[3] & 0xff);
			   m_bConnected = TRUE;
			   time(&tmConnect);
			   mobileConnectEvent();
			   AddMobileLog(0);
			   time(&last);
			   nRetry = 0;
			   nPowerReset = 0;
			   nError = 0;
			   m_bCheck = FALSE;
			   nState = STATE_CHECK;
			   break;

		  case STATE_CHECK :
//			   XDEBUG("PPP(%0d) - STATE_CHECK\r\n", nState);
			   if (m_bCheck)
			   {
				   XDEBUG("PPP(%0d) - STATE_CHECK - Reconnect flag set.\r\n");
				   m_bCheck = FALSE;
			       nState = STATE_PING;
				   break;
			   }

			   if ((pid == 0) || (!IsPid(pid)))
			   {
				   pid = FindProcess("pppd");
				   if (pid <= 0)
				   {
					   XDEBUG("PPP(%0d) - STATE_CHECK - pppd(%0d) not found.\r\n", nState, pid);
					   nError = MOBERR_PPP_NOT_FOUND;
					   nState = STATE_RETRY;
					   break;
				   }
			   }

			   USLEEP(3000000);

			   // Check interval
			   time(&now);
			   if ((unsigned long)(now-last) > (unsigned long)(m_nMobileLiveCheckInterval*60))
			      nState = STATE_PING;
			   break;

		  case STATE_PING :
			   XDEBUG("PPP(%0d) - STATE_PING - Server=%s\r\n", nState, m_szFepServer);
			   if (!Ping(m_szFepServer))
			   {
			   	   XDEBUG("PPP(%0d) - STATE_PING - Ping Error.\r\n", nState);
				   nError = MOBERR_PING_ERROR;
				   nState = STATE_RETRY;
				   break;
			   }
			   time(&last);
			   nState = STATE_CHECK;
			   break;
	
		  case STATE_RETRY :
			   m_bConnected = FALSE;
			   nRetry++;
			   XDEBUG("PPP(%0d) - STATE_RETRY (Retry=%0d)\r\n", nState, nRetry);
			   KillProcess("pppd");
			   USLEEP(1000000 * 60);
			   if (nRetry <= 3)
			   {
			   	   nState = STATE_RUN_PPP;
				   break;
			   }

			   time(&last);
			   nPowerReset++;
			   if (nPowerReset > 3)
			   {
			   	   // GSM MODE
			   	   AddMobileLog(3);
				   nState = STATE_GSM_MODE;
				   break;
			   }

			   XDEBUG("PPP: Power Reset mode.\r\n");
			   nRetry = 0;
			   AddMobileLog(2);
			   nState = STATE_RESET;
			   break;

		  case STATE_GSM_MODE :
#ifdef __OLD_GSM_MODE__
			   time(&now);
			   XDEBUG("PPP: IDLE Mode. (%d)\r\n", (unsigned long)(now-last));
			   if ((unsigned long)(now-last) <= (unsigned long)(30*60))
			   {
				   USLEEP(5000000);
				   break;
			   }
			   /* 기존 GSM 모드로 있다가 복귀되는 루틴 */
			   if (FindProcess("mserver") > 0)
			   {
				   USLEEP(10000000);
				   break;
			   }
			   if (FindProcess("send") > 0)
			   {
				   USLEEP(10000000);
				   break;
			   }
			   nRetry = 0;
			   nPowerReset = 0;
			   m_bGsmMode = FALSE;
			   nState = STATE_RESET;
#else
			   /* EDF와 같이 문제를 복구하기 위하여 리부팅한다. */
			   /* 계속 리셋을 방지하기 위하여 리셋한다.  */
			   /* 이 루틴은 최악의 경우 복구를 위해서 존재한다. */
			   USLEEP(1000000 * 10);
				 m_pService->SetResetState(RST_SYSMONITOR);
			   ResetSystem();
#endif // __OLD_GSM_MODE__
			   break;
		}
	}
}

BOOL CSystemMonitor::OnActiveThread()
{
	char	szName[64];
	UINT	addr, subnet, gateway;
	BYTE 	phyAddr[10], *p = (BYTE *)&addr;
	int		pid = 0, state;
	int		n, nLast = 0;
    int     nRetry = 0;

	if (m_nEthernetType != ETHER_TYPE_PPP)
	{
		if(!m_bConnected) m_bConnected = TRUE;
		return TRUE;
	}

	if (m_nAliveCount <= 0)
		return TRUE;
	
	switch(m_nMobileMode) {
	  case MOBILE_MODE_CSD :
		   m_bConnected = TRUE;
		   return TRUE;

	  case MOBILE_MODE_ALWAYS :
		   KeepConnection();
		   return TRUE;
	}

	for(state=STATE_INIT; state!=STATE_DONE;)
	{	
		WatchdogClear();

		switch(state) {
		  case STATE_INIT :	
			   state = STATE_RUN_PPP;
			   break;

		  case STATE_RUN_PPP :
			   nRetry++;
               if(nRetry > 3) {
			   	    m_bConnected = FALSE;	
                    state = STATE_DONE;
                    break;
               }
			   pid = FindProcess("pppd");
		  	   if (pid <= 0)
			   {
				   unlink("/var/log/messages");
				   ResetModem();
				   ExecutePPPD();
				   USLEEP(3000000);
				   break;
			   }
			   state = STATE_CONNECT_WAIT;
			   break;

		  case STATE_CONNECT_WAIT :
			   if (!IsPid(pid))
			   {
				   USLEEP(3000000);
				   state = STATE_RUN_PPP;
				   break;
			   }
			   GetEthernetInfo(szName, &addr, &subnet, &gateway, phyAddr);
			   if ((strncmp(szName, "ppp", 3) != 0) || (addr == 0))
			   {
				   USLEEP(3000000);
				   break;
			   }
			   XDEBUG("\r\nPPP Connected.\r\n", szName);
			   XDEBUG("Name=%s, IP=%d.%d.%d.%d\r\n",
						szName, p[0] & 0xff, p[1] & 0xff, p[2] & 0xff, p[3] & 0xff);
			   m_bConnected = TRUE;	
			   AddMobileLog(0);
			   state = STATE_RUNNING;
			   break;

		  case STATE_RUNNING :	
			   if (!IsPid(pid))
			   {
			   	   m_bConnected = FALSE;	
			       AddMobileLog(1, MOBERR_PPP_NOT_FOUND);
				   USLEEP(1000000);
				   state = STATE_RUN_PPP;
				   break;
			   }
			   else if (m_nAliveCount > 0)
			   {
				   USLEEP(3000000);
				   break;
			   }
			   nLast = uptime();
			   state = STATE_TERM_WAIT;
			   break;

		  case STATE_TERM_WAIT :
			   USLEEP(1000000);
			   if (m_nAliveCount > 0)
			   {
				   state = STATE_RUNNING;
				   break;
			   }

			   n = uptime() - nLast;
			   if (n < 10)
				   break;
			   AddMobileLog(1);
			   state = STATE_DONE;
    		   m_bConnected = FALSE;	
			   break;
		}
	}

	KillProcess("pppd");
	USLEEP(1000000);
	ResetModem();
  	m_bConnected = FALSE;
	return TRUE;
}

BOOL CSystemMonitor::OnTimeoutThread()
{
	WatchdogClear();
	return OnActiveThread();
}
