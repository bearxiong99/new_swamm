//////////////////////////////////////////////////////////////////////
//
//	EventManager.cpp: implementation of the CEventManager class.
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
#include "logdef.h"
#include "EventManager.h"
#include "AgentService.h"
#include "SystemMonitor.h"
#include "MobileUtility.h"
#include "Utility.h"
#include "IcmpPing.h"
#include "CommonUtil.h"
#include "Variable.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "if4frame.h"
#include "varapi.h"
#include "SystemUtil.h"
#include "ShellCommand.h"

#include "AgentLog.h"
 
//////////////////////////////////////////////////////////////////////
// Global Variable Definitions
//////////////////////////////////////////////////////////////////////

#define EVENTTEMP	"/tmp/event.dat"

CEventManager	*m_pEventManager = NULL;

extern int  m_nSystemID;
extern char m_szFepServer[];
extern int  m_nFepAlarmPort;

extern BYTE m_nEventAlertLevel;
extern int	m_nEventLogSize;
extern int	m_nAlarmSaveDays;
extern int	m_nCmdHistSaveDay;
extern int	m_nEventLogSaveDay;
extern int	m_nCommLogSaveDay;
extern int	m_nMobileLogSaveDay;
extern int	m_nMeterLogSaveDay;
extern int	m_nUpgradeLogSaveDay;
extern int	m_nUploadLogSaveDay;
extern int	m_nTimesyncLogSaveDay;
extern MOBILECFGENTRY m_sMobileConfig;


EVTCFGENTRY EventConfigure[] = {
    { "200.1",  "McuInstall",           EVTCLASS_USER,      EVTCLASS_USER,      TRUE },
    { "200.2",  "McuUninstall",         EVTCLASS_USER,      EVTCLASS_USER,      TRUE },
    { "200.3",  "McuStartup",           EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "200.4",  "McuShutdown",          EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "200.10", "McuChange",            EVTCLASS_INFO,      EVTCLASS_INFO,      TRUE },
    { "200.11", "McuOamLogin",          EVTCLASS_INFO,      EVTCLASS_INFO,      TRUE },
    { "200.12", "McuOamLogout",         EVTCLASS_INFO,      EVTCLASS_INFO,      TRUE },
    { "200.13", "McuClockChange",       EVTCLASS_NORMAL,    EVTCLASS_NORMAL,    TRUE },
    { "200.14", "McuPowerFail",         EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "200.15", "McuPowerRestore",      EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "200.16", "McuLowBattery",        EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "200.17", "McuLowBatteryRestore", EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "200.18", "McuTempRange",         EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
    { "200.19", "McuTempRangeRestore",  EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
    { "200.20", "McuReset",             EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "200.21", "McuFactoryDefault",    EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "200.22", "McuFirmwareUpdate",    EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "200.23", "McuBatteryChargingStart", EVTCLASS_NORMAL, EVTCLASS_NORMAL,    TRUE },
    { "200.24", "McuBatteryChargingEnd",EVTCLASS_NORMAL,    EVTCLASS_NORMAL,    TRUE },
#if defined(__TI_AM335X__)
    { "200.25", "McuCoverOpen",         EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
    { "200.26", "McuCoverClose",        EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
#endif
    { "200.27", "McuHeaterOn",          EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "200.28", "McuHeaterOff",         EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "200.29", "McuAlert",             EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "200.30", "BatteryFail",          EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "200.105","McuDiagnosis",         EVTCLASS_INFO,      EVTCLASS_INFO,      TRUE },
    { "201.6",  "CoordiReset",          EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "202.4",  "MobileConnect",        EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "202.6",  "MobileKeepalive",      EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "203.1",  "SensorJoinNetwork",    EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "203.2",  "SensorLeaveNetwork",   EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
/** 현재 지원되지 않고 있다
    { "203.4",  "SensorConnect",        EVTCLASS_NORMAL,    EVTCLASS_NORMAL,    TRUE },
    { "203.5",  "SensorDisconnect",     EVTCLASS_NORMAL,    EVTCLASS_NORMAL,    TRUE },
    { "203.6",  "SensorTimeout",        EVTCLASS_NORMAL,    EVTCLASS_NORMAL,    TRUE },
    { "203.7",  "SensorBreaking",       EVTCLASS_NORMAL,    EVTCLASS_NORMAL,    TRUE },
    { "203.8",  "SensorUnknown",        EVTCLASS_NORMAL,    EVTCLASS_NORMAL,    TRUE },
    { "203.9",  "SensorReset",          EVTCLASS_NORMAL,    EVTCLASS_NORMAL,    TRUE },
**/
    { "203.10", "SensorAlarm",          EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "203.105","SensorInstall",        EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "203.106","SensorUninstall",      EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "203.107","SensorPowerOutage",    EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "203.109","EnergyLevelChange",    EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
    { "204.4",  "MalfDiskError",        EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
    { "204.5",  "MalfDiskErrorRestore", EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
    { "204.6",  "MalfMemoryError",      EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
    { "204.7",  "MalfMemoryErrorRestore",EVTCLASS_MAJOR,    EVTCLASS_MAJOR,     TRUE },
    { "204.8",  "MalfWatchdogReset",    EVTCLASS_MAJOR,     EVTCLASS_MAJOR,     TRUE },
    { "204.9",  "MalfGarbageCleaning",  EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "205.1",  "CommZRUError",         EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "205.2",  "CommZMUError",         EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "205.3",  "CommZEUError",         EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "212.1",  "MeterError",           EVTCLASS_MINOR,     EVTCLASS_MINOR,     TRUE },
    { "214.1",	"OtaDownload",			EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "214.2",	"OtaStart",				EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "214.3",	"OtaEnd",				EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "214.4",	"OtaResult",			EVTCLASS_MAJOR,  	EVTCLASS_MAJOR,  	TRUE },
    { "215.1",  "Transaction",          EVTCLASS_CRITICAL,  EVTCLASS_CRITICAL,  TRUE },
    { "",  "", 0xff, 0xff, FALSE }
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventManager::CEventManager() : CJobObject("EventManager", 60*60)
{
	m_pEventManager = this;
	m_bIdle			= TRUE;
	m_nLastReduce	= 0;
	m_pszEventBuffer = (char *)MALLOC(64*1024);
}

CEventManager::~CEventManager()
{
	if (m_pszEventBuffer != NULL)
		FREE(m_pszEventBuffer);
	m_pszEventBuffer = NULL;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CEventManager::Initialize()
{
    LoadEventConfigure("/app/evtcfg");

    for(m_nEventCount=0;EventConfigure[m_nEventCount].nClass!=0xFF;m_nEventCount++) ;

	if (!CTimeoutThread::StartupThread(3))
		return FALSE;

	return TRUE;
}

void CEventManager::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

BOOL CEventManager::IsIdle()
{
	int		nCount;

	m_Locker.Lock();
	nCount = m_List.GetCount();
	m_Locker.Unlock();

	if (nCount > 0)
		return FALSE;

	return m_bIdle;
}

//////////////////////////////////////////////////////////////////////
// COORDI Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CEventManager::AddEvent(int nClass, CIF4Invoke *pInvoke, const char *pszEvent, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime)
{
	EVENTITEM	*pItem;
    EVTCFGENTRY *pConfigure;

    pConfigure = SearchEventConfigure(pszEvent);
    if(!pConfigure) return FALSE;
    
    if(!pConfigure->bNotify) return FALSE;

	pItem = (EVENTITEM *)MALLOC(sizeof(EVENTITEM));
	if (pItem == NULL)
		return FALSE;

	memset(pItem, 0, sizeof(EVENTITEM));
	strcpy(pItem->szOid, pszEvent);
	if (srcID != NULL) memcpy(pItem->srcID, srcID, 8);
	if (pTime != NULL)
		 memcpy(&pItem->tmEvent, pTime, sizeof(TIMESTAMP));
	else GetTimestamp(&pItem->tmEvent, NULL);
	pItem->nClass	= pConfigure->nClass;
	pItem->nType	= srcType;
	pItem->pInvoke 	= pInvoke;
	time(&pItem->tmSave);

   	m_Locker.Lock();
   	pItem->nPosition = (int)m_List.AddTail(pItem);
	WriteEvent(pItem);
   	m_Locker.Unlock();

    ActiveThread();
	return TRUE;
}

void CEventManager::RemoveAll()
{
	EVENTITEM	*pItem, *pDelete;
	POSITION	pos;

	m_Locker.Lock();
	pItem = m_List.GetFirst(pos);
	while(pItem)
	{
		pDelete = pItem;
		pItem = m_List.GetNext(pos);
	
		if (pDelete->pInvoke != NULL)
			delete pDelete->pInvoke;
		FREE(pDelete);
	}
	m_List.RemoveAll();
	m_Locker.Unlock();
}

//////////////////////////////////////////////////////////////////////
// SINK Override Functions
//////////////////////////////////////////////////////////////////////

void CEventManager::ChangeLocalAddress(EVENTITEM *pItem)
{
	IF4Invoke	*pInvoke;
	MIBValue	*pNext;
	ETHENTRY	ethv;

	if (!pItem || !pItem->pInvoke)
		return;

	pInvoke = pItem->pInvoke->GetHandle();
	if (pInvoke == NULL)
		return;

	memset(&ethv, 0, sizeof(ETHENTRY));
	pNext = pInvoke->pParamNode;
	while(pNext)
	{
		if ((pNext->oid.id1 == 2) && (pNext->oid.id2 == 102) && (pNext->oid.id3 == 3))
		{
			GetEthernetInfo(ethv.ethName,
					(UINT *)pNext->stream.p,
					&ethv.ethSubnetMask,
					&ethv.ethGateway,
					(BYTE *)&ethv.ethPhyAddr);
			break;
		}
		pNext = pNext->pNext;
	}
}

BOOL CEventManager::SendEvent(EVENTITEM *pItem)
{
	char	*pszAddress;
	int		i, nPort, nError = 0;

	if (pItem->pInvoke == NULL)
		return TRUE;

	// Change Startup Event (Local address)
	if ((strcmp(pItem->szOid, "200.3") == 0) ||
		(strcmp(pItem->szOid, "202.6") == 0))
		ChangeLocalAddress(pItem);		

	for(i=0; i<3; i++)
	{
		pszAddress = pItem->pInvoke->GetAddress();
		nPort      = pItem->pInvoke->GetPort();

		XDEBUG("EVENT(%s) Send to %s:%d (try=%d).\xd\xa",
			 pItem->szOid, pszAddress ? pszAddress : "", nPort, i+1);

		try
		{
			nError = pItem->pInvoke->Event(pItem->szOid, pItem->nType, pItem->srcID, &pItem->tmEvent);
		}
		catch(...)
		{
			XDEBUG("***** Event(%s) Sending Exception.\xd\xa", pItem->szOid);
			nError = IF4ERR_SYSTEM_ERROR;
			break;
		}

		XDEBUG("EVENT(%s) Sending %s.\xd\xa", pItem->szOid, nError == IF4ERR_NOERROR ? "Done" : "Fail");
		if (nError == IF4ERR_NOERROR)
			break;
	}
    return (nError == IF4ERR_NOERROR) ? TRUE : FALSE;
}

BOOL CEventManager::AppendEventList(EVENTITEM *pItem)
{
	int		nLength;
	FILE	*fp;

	if (pItem->pInvoke == NULL)
		return TRUE;

	// Change Startup Event (Local address)
	if ((strcmp(pItem->szOid, "200.3") == 0) ||
		(strcmp(pItem->szOid, "202.6") == 0))
		ChangeLocalAddress(pItem);		

	nLength = IF4API_EventFrame(m_pszEventBuffer, pItem->pInvoke->GetHandle(), pItem->szOid,
						pItem->nType, pItem->srcID, &pItem->tmEvent);

	fp = fopen(EVENTTEMP, "a+b");
	if (fp == NULL)
		return FALSE;

	fwrite(m_pszEventBuffer, 1, nLength, fp);
	fclose(fp);
	return TRUE;
}

void CEventManager::SendEventByMobile()
{
	char	szCommand[256];
	BOOL	bFind = FALSE;

	// Circuit mode
	if ((m_pService->GetEthernetType() == ETHER_TYPE_PPP) &&
		(m_pService->GetMobileMode() == MOBILE_MODE_CSD))
	{
		MOBILE_Lock();
		for(;;)
		{
			if (FindProcess("mserver") > 0)
			{
				bFind = TRUE;
				XDEBUG("EVENT: BUSY: mserver active.\r\n");
				usleep(5000000);
				continue;
			}
			else
			{
				if (bFind) usleep(3000000);
				break;
			}
		}

        // TODO send를 새로운 Mobile에 맞게 수정해야 한다
		sprintf(szCommand, "/app/sw/send \"%s\" %d %d %s 5 0 %d",
				m_sMobileConfig.moduleType,
				m_nSystemID, m_nFepAlarmPort, EVENTTEMP, m_nDebugLevel == 0 ? 1 : 0);
		SystemExec(szCommand);

		unlink(EVENTTEMP); 
		MOBILE_Unlock();
		return;
	}

	// LAN/DHCP/AlwaysOn Mode
	UploadFile(EVENTTEMP);
	unlink(EVENTTEMP); 
}

BOOL CEventManager::WriteEvent(EVENTITEM *pItem)
{
    IF4_EVENT_FRAME *pEvent = NULL;
	char	szName[64];
	int		nLength;
	OID3	oid;

    if(pItem->pInvoke == NULL) return FALSE;

	nLength = IF4API_EventFrame(m_pszEventBuffer, pItem->pInvoke->GetHandle(), pItem->szOid,
									pItem->nType, pItem->srcID, &pItem->tmEvent);
	if (nLength <= 0)
		return FALSE;

    pEvent = (IF4_EVENT_FRAME *)m_pszEventBuffer;
    if (VARAPI_StringToOid(pItem->szOid, &oid))
        VARAPI_OidToName(&oid, szName);
    else strcpy(szName, "eventUnknown");

    return EVENT_LOG(szName, (char *)&pEvent->args, pEvent->eh.cnt, nLength);
}

void CEventManager::ReduceLog(const char *pszDirectory, const char *pszType, int nDays)
{
	DIR		*dir_fdesc;
	dirent	*dp;
	time_t	old;
	struct	tm	when;
	char	szPath[128];
	char	szDate[20];

	time(&old);
	old -= nDays * (24*(60*60));
	when = *localtime(&old);
	sprintf(szDate, "%04d%02d%02d",
			when.tm_year+1900,
			when.tm_mon+1,
			when.tm_mday);

	dir_fdesc = opendir(pszDirectory);
	if (dir_fdesc == NULL)
		return;

	for(; (dp=readdir(dir_fdesc));)
	{
#if     defined(_DIRENT_HAVE_D_TYPE)
		if (dp->d_type == DT_DIR)
			continue;
#endif

		if (dp->d_name[0] == '.')
			continue;

		if (strncmp(dp->d_name, pszType, 3) != 0)
			continue;

		if (strncmp(&dp->d_name[3], szDate, 8) >= 0)
			continue;

		XDEBUG("Reduce %s/%s (%s)...\xd\xa", pszDirectory, dp->d_name, szDate);
		sprintf(szPath, "%s/%s", pszDirectory, dp->d_name);
		unlink(szPath);
	}

	closedir(dir_fdesc);
	SystemExec("sync");
}

void CEventManager::ReduceEventAndAlarm()
{
	time_t	now;
	struct	tm	when;

	time(&now);
	if ((now-m_nLastReduce) <= (60*60))
		return;

	when = *localtime(&now);
	if (m_nLastReduce != 0)
	{
		if ((when.tm_hour != 0) || (when.tm_min < 50))
			return;
	}

	time(&m_nLastReduce);
	ReduceLog("/app/log", EVENT_LOG_FILE, m_nEventLogSaveDay);
	ReduceLog("/app/log", CMD_HISTORY_LOG_FILE, m_nCmdHistSaveDay);
	ReduceLog("/app/log", COMM_LOG_FILE, m_nCommLogSaveDay);
	ReduceLog("/app/log", MOBILE_LOG_FILE, m_nMobileLogSaveDay);
	ReduceLog("/app/log", UPLOAD_LOG_FILE, m_nUploadLogSaveDay);  // Upload log
	ReduceLog("/app/log", TIMESYNC_LOG_FILE, m_nTimesyncLogSaveDay);// Meter Timesync log
	ReduceLog("/app/log", METERING_LOG_FILE, m_nMeterLogSaveDay);   // Metering log
	ReduceLog("/app/log", UPGRADE_LOG_FILE, m_nUpgradeLogSaveDay); // Uplgrade log
	SystemExec("sync");
}

BOOL CEventManager::LoadEventConfigure(const char *pszPath)
{
	EVTCFGENTRY	member;
    DIR			*dir_fdesc;
    dirent		*dp;
	int			fd, n, sn;
	char		*p, szFileName[64];
    int         i;
    BOOL        bUpdate;

    dir_fdesc = opendir(pszPath);
    if (!dir_fdesc)
        return FALSE;

	sn = sizeof(EVTCFGENTRY);

    m_Locker.Lock();
    for(;(dp=readdir(dir_fdesc));)
    {
        p = strrchr(dp->d_name, '.');
        if ((p == NULL) || (strcmp(p, ".ecfg") != 0))
            continue;

        sprintf(szFileName, "%s/%s", pszPath, dp->d_name);
        fd = open(szFileName, O_RDONLY);
        if (fd > 0)
        {
			memset(&member, 0, sn);
			n = read(fd, &member, sn);
			close(fd);

			if (n == sn)
			{
                bUpdate = FALSE;
                for(i=0;EventConfigure[i].nClass!=0xFF;i++) {
                    if(!strcmp(member.szOid, EventConfigure[i].szOid)) {
                        memcpy(&EventConfigure[i], &member, sizeof(EVTCFGENTRY));
                        bUpdate = TRUE;
                        break;
                    }
                }
                if(!bUpdate) {
				    // Delete file
				    XDEBUG("Delete %s (not support event)\r\n", dp->d_name);
				    remove(szFileName);
                }
			}
			else
			{
				// Delete file
				XDEBUG("Delete %s (no match size)\r\n", dp->d_name);

                mcuAlert(MCU_ALERT_INVALID_EVTCONFIGURE_SIZE, (BYTE *)&n, sizeof(UINT), (BYTE *)&sn, sizeof(UINT));

				remove(szFileName);
			}
		}
	}
	closedir(dir_fdesc);
	m_Locker.Unlock();
	return TRUE;
}

BOOL CEventManager::SaveEventConfigure(EVTCFGENTRY *pConfigure)
{
	FILE	*fp;
	char	szFileName[64];

	sprintf(szFileName, "/app/evtcfg/%s.ecfg", pConfigure->szOid);

	fp = fopen(szFileName, "wb");
	if (fp == NULL)
		return FALSE;

	fwrite(pConfigure, sizeof(EVTCFGENTRY), 1, fp);

	fflush(fp);
	fclose(fp);
	return TRUE;
}

BOOL CEventManager::UpdateEventConfigure(const char * szOid, BOOL bUseDefaultClass, int nClass, int nNotify)
{
    EVTCFGENTRY * pConfigure;
    BOOL    bUpdate = FALSE;

    pConfigure = SearchEventConfigure(szOid);

    if(!pConfigure) return FALSE;

    /** Issue #833 */
    if(bUseDefaultClass) {
        pConfigure->nClass = pConfigure->nDefaultClass;
        pConfigure->bNotify = TRUE; // Default 값은 항상 TRUE 이다
        bUpdate = TRUE;
    }
    else
    {
        if(nClass >= EVTCLASS_CRITICAL && nClass <= EVTCLASS_INFO) {
            pConfigure->nClass = (BYTE) nClass;
            bUpdate = TRUE;
        }
        switch(nNotify) {
            case 0:
            case 1:
                pConfigure->bNotify = (BYTE) (nNotify?TRUE:FALSE);
                bUpdate = TRUE;
                break;
        }
    }
    if(bUpdate) {
        SaveEventConfigure(pConfigure);
    }
    return bUpdate;
}

int CEventManager::GetEventConfigure(EVTCFGENTRY ** ppEvent)
{
    if(m_nEventCount>0) {
        *ppEvent = (EVTCFGENTRY *)MALLOC(sizeof(EVTCFGENTRY)*m_nEventCount);
        memcpy(*ppEvent,EventConfigure,sizeof(EVTCFGENTRY)*m_nEventCount);
    }

    return m_nEventCount;
}

EVTCFGENTRY * CEventManager::SearchEventConfigure(const char * eventName)
{
    int i;

    for(i=0;i<m_nEventCount;i++) {
        if(!strcmp(EventConfigure[i].szOid, eventName) || !strcmp(EventConfigure[i].szEvent,eventName)) return &EventConfigure[i];
    }
    return NULL; 
}


//////////////////////////////////////////////////////////////////////
// SINK Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CEventManager::OnActiveThread()
{
	EVENTITEM	*pItem;
	time_t		now;
	int			nCount, nRetry;
	BOOL		bConnected = FALSE;
	BOOL		bAccept = TRUE;

	ReduceEventAndAlarm();

	m_Locker.Lock();
	nCount = m_List.GetCount();
	m_Locker.Unlock();

	if (nCount <= 0) {
		return TRUE;
    }

	// Make PPP Connection
	m_bIdle = FALSE;
	if ((m_pService->GetEthernetType() == ETHER_TYPE_PPP) &&
		(m_pService->GetMobileMode() == MOBILE_MODE_CSD))
	{
		nCount = 0;
		for(nRetry=0; nRetry<10; nRetry++)
		{
	   		for(;!IsThreadExitPending();)
	   		{
	   			m_Locker.Lock();
  	     		pItem = m_List.GetHead();
	       		if (pItem) m_List.RemoveAt((POSITION)pItem->nPosition);
  	  			m_Locker.Unlock();

	      		if (pItem == NULL)
 	          		break;

	            WatchdogClear();
				if (pItem->nClass <= (int)m_nEventAlertLevel)
				{
					time(&now);
					if ((now-pItem->tmSave) < (10*60))
					{
						bAccept = AppendEventList(pItem);
						nCount++;
					}
				}

				if (pItem->pInvoke != NULL)
					delete pItem->pInvoke;
   	    		FREE(pItem);
			}

			if (nCount > 0)
				SendEventByMobile();
		}
	}
	else
	{
	   	for(;!IsThreadExitPending();)
	   	{
	    	m_Locker.Lock();
  	     	pItem = m_List.GetHead();
	       	if (pItem) m_List.RemoveAt((POSITION)pItem->nPosition);
  		  	m_Locker.Unlock();

	      	if (pItem == NULL)
 	          	break;

	        WatchdogClear();
			if (pItem->nClass <= (int)m_nEventAlertLevel)
			{
				time(&now);
				if ((now-pItem->tmSave) < (10*60))
				{
					if(!bConnected) bConnected = m_pSystemMonitor->NewConnection();
					if (bConnected)
					{
						bAccept = SendEvent(pItem);
						if (!bAccept) {
		                    m_pSystemMonitor->DeleteConnection();
                            bConnected = FALSE;
                        }
					}
                    else
                    {
                        /* Connection에 성공 실패 여부와 상관없이 Connection Count는 증가하기 때문에
                         * 실패했을 경우에도 DeleteConnection을 불러야 한다.
                         */
		                m_pSystemMonitor->DeleteConnection();
                    }
				}
			}

			if (pItem->pInvoke != NULL)
            {
				delete pItem->pInvoke;
            }
   	    	FREE(pItem);
		}
		if(bConnected) 
        {
            m_pSystemMonitor->DeleteConnection();
        }
	}

	// Remove PPP Connection
	m_bIdle = TRUE;
	return TRUE;
}

BOOL CEventManager::OnTimeoutThread()
{
	WatchdogClear();
	return OnActiveThread();
}

