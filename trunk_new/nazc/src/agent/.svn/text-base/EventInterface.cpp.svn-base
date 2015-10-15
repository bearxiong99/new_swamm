//////////////////////////////////////////////////////////////////////
//
//	EventInterface.cpp: implementation of the CEventInterface class.
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

#include "common.h"
#include "Variable.h"
#include "AgentService.h"
#include "EventInterface.h"
#include "EventManager.h"
#include "CommonUtil.h"
#include "ShellCommand.h"
#include "codiapi.h"
#include "Utility.h"
#include "DebugUtil.h"
 
//////////////////////////////////////////////////////////////////////
// Event Definitions
//////////////////////////////////////////////////////////////////////

#define DEFAULT_TIMEOUT		10

typedef struct	_tagPAGESTREAM
{
		EUI64			id;
		BYTE			nPage;
		BYTE			nLength;
		char			szPage[128];
}	__attribute__ ((packed)) PAGESTREAM;

extern char		m_szFepServer[];
extern int		m_nFepPort;
extern int		m_nFepAlarmPort;
extern int		m_nLocalPort;

//////////////////////////////////////////////////////////////////////
// PDA Event
//////////////////////////////////////////////////////////////////////

BOOL SendOamEvent(BYTE nAction, BYTE nEvent, EUI64 *id, char *pszMessage, BYTE nLength)
{
	return TRUE;
}

BOOL SendPdaEvent(BYTE nAction, BYTE nEvent, BYTE nGroup, BYTE nMember, EUI64 *id, char *pszSerial)
{
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// MCU Event
//////////////////////////////////////////////////////////////////////

char *getFepServerAddress()
{
	return m_szFepServer;
}

void setFepServerAddress(char *pszAddress)
{
	strcpy(m_szFepServer, pszAddress);
}

void setFepServerPort(int nPort)
{
    if(nPort > 0)
	    m_nFepPort = nPort;
    else
        m_nFepPort = IF4_DEFAULT_PORT;
}

void setFepServerAlarmPort(int nPort)
{
    if(nPort > 0) 
        m_nFepAlarmPort = nPort;
    else
        m_nFepPort = IF4_DEFAULT_ALARM_PORT;
}

void setLocalPort(int nPort)
{
	m_nLocalPort = nPort;
}

int getLocalPort()
{
	return m_nLocalPort;
}

//////////////////////////////////////////////////////////////////////
// MCU Event
//////////////////////////////////////////////////////////////////////

void mcuInstallEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamValue("2.1.10");				// sysPhoneNumber
    pInvoke->AddParamValue("2.102.3");				// ethIpAddr
    pInvoke->AddParamValue("2.1.23");				// sysLocalPort
    pInvoke->AddParamValue("2.1.3");				// sysType
    pInvoke->AddParamValue("2.1.11");				// sysEtherType
    pInvoke->AddParamValue("2.1.12");				// sysMobileType
    pInvoke->AddParamValue("2.1.13");				// sysMobileMode
    m_pEventManager->AddEvent(EVTCLASS_USER, pInvoke, "200.1", IF4_SRC_MCU, NULL, NULL);
}

void mcuUninstallEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_USER, pInvoke, "200.2", IF4_SRC_MCU, NULL, NULL);
}

void mcuStartupEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamValue("2.1.10");				// sysPhoneNumber
    pInvoke->AddParamValue("2.102.3");				// ethIpAddr
    pInvoke->AddParamValue("2.1.23");				// sysLocalPort
    pInvoke->AddParamValue("2.1.3");				// sysType
    pInvoke->AddParamValue("2.1.12");				// sysMobileType
    pInvoke->AddParamValue("2.1.13");				// sysMobileMode
    pInvoke->AddParamValue("2.1.8");				// sysHwVersion
    pInvoke->AddParamValue("2.1.9");				// sysSwVersion
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "200.3", IF4_SRC_MCU, NULL, NULL);
}

void mcuShutdownEvent(BYTE nType, char *pszAddress)
{
	long		addr;

	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
	addr = inet_addr(pszAddress);
    pInvoke->AddParam("1.4", (BYTE)nType);
    pInvoke->AddParamFormat("1.15", VARSMI_IPADDR, &addr, sizeof(IPADDR));
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "200.4", IF4_SRC_MCU, NULL, NULL);
}

void mcuChangeEvent(MIBValue *pValue, int nCount, BOOL *pResult)
{
	int			i;

	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
	for(i=0; i<nCount; i++)
	{
		if (pResult[i])
			pInvoke->AddParamMIBValue(&pValue[i]);
	}
    m_pEventManager->AddEvent(EVTCLASS_INFO, pInvoke, "200.10", IF4_SRC_MCU, NULL, NULL);
}

void mcuOamLogin(char *pszAddress, char *pszUser)
{
	long		addr;

	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
	addr = inet_addr(pszAddress);
    pInvoke->AddParamFormat("1.15", VARSMI_IPADDR, &addr, sizeof(IPADDR));
    pInvoke->AddParam("2.10.2", pszUser);
    m_pEventManager->AddEvent(EVTCLASS_INFO, pInvoke, "200.11", IF4_SRC_MCU, NULL, NULL);
}

void mcuOamLogout(char *pszAddress, char *pszUser)
{
	long		addr;

	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
	addr = inet_addr(pszAddress);
    pInvoke->AddParamFormat("1.15", VARSMI_IPADDR, &addr, sizeof(IPADDR));
    pInvoke->AddParam("2.10.2", pszUser);
    m_pEventManager->AddEvent(EVTCLASS_INFO, pInvoke, "200.12", IF4_SRC_MCU, NULL, NULL);
}

void mcuClockChangeEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
	pInvoke->AddParamValue("2.1.16");
    m_pEventManager->AddEvent(EVTCLASS_NORMAL, pInvoke, "200.13", IF4_SRC_MCU, NULL, NULL);
}

void mcuPowerFailEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "200.14", IF4_SRC_MCU, NULL, NULL);
}

void mcuPowerRestoreEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "200.15", IF4_SRC_MCU, NULL, NULL);
}

void mcuLowBatteryEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "200.16", IF4_SRC_MCU, NULL, NULL);
}

void mcuLowBatteryRestoreEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "200.17", IF4_SRC_MCU, NULL, NULL);
}

void mcuBatteryFailEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "200.30", IF4_SRC_MCU, NULL, NULL);
}

void mcuTempRangeEvent(double fCurrent, double fMin, double fMax)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("2.1.17", (int)(fCurrent*(double)10));
    pInvoke->AddParam("2.1.18", (int)(fMin*(double)10));
    pInvoke->AddParam("2.1.19", (int)(fMax*(double)10));
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "200.18", IF4_SRC_MCU, NULL, NULL);
}

void mcuTempRangeRestoreEvent(double fCurrent, double fMin, double fMax)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("2.1.17", (int)(fCurrent*(double)10));
    pInvoke->AddParam("2.1.18", (int)(fMin*(double)10));
    pInvoke->AddParam("2.1.19", (int)(fMax*(double)10));
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "200.19", IF4_SRC_MCU, NULL, NULL);
}

void mcuResetEvent(BYTE nType, const char *pszAddress)
{
	long		addr;
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);

	addr = inet_addr(pszAddress);
    pInvoke->AddParam("1.4", (BYTE)nType);
    pInvoke->AddParamFormat("1.15", VARSMI_IPADDR, &addr, sizeof(IPADDR));
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "200.20", IF4_SRC_MCU, NULL, NULL);
}

void mcuFactoryDefaultEvent(BYTE nType, char *pszAddress)
{
	long		addr;
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);

	addr = inet_addr(pszAddress);
    pInvoke->AddParam("1.4", (BYTE)nType);
    pInvoke->AddParamFormat("1.15", VARSMI_IPADDR, &addr, sizeof(IPADDR));
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "200.21", IF4_SRC_MCU, NULL, NULL);
}

void mcuFirmwareUpdateEvent(BYTE nType, const char *pszAddress, const char *pszFileName)
{
	long		addr;
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);

	addr = inet_addr(pszAddress);
    pInvoke->AddParam("1.4", (BYTE)nType);
    pInvoke->AddParamFormat("1.15", VARSMI_IPADDR, &addr, sizeof(IPADDR));
    pInvoke->AddParam("1.11", pszFileName);
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "200.22", IF4_SRC_MCU, NULL, NULL);
}

void mcuBatteryChargingStartEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_NORMAL, pInvoke, "200.23", IF4_SRC_MCU, NULL, NULL);
}

void mcuBatteryChargingEndEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_NORMAL, pInvoke, "200.24", IF4_SRC_MCU, NULL, NULL);
}

void mcuCoverOpenEvent()
{
#if 0
    // Case Open을 지원하지 않는다 (정상적으로 감지 하지 못한다
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "200.25", IF4_SRC_MCU, NULL, NULL);
#endif
}

void mcuCoverCloseEvent()
{
#if 0
    // Case Open을 지원하지 않는다 (정상적으로 감지 하지 못한다
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "200.26", IF4_SRC_MCU, NULL, NULL);
#endif
}

void mcuHeaterOnEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamValue("2.3.62");				// varHeaterOnTemp
    pInvoke->AddParamValue("2.1.17");				// sysCurTemp
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "200.27", IF4_SRC_MCU, NULL, NULL);
}

void mcuHeaterOffEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamValue("2.3.63");				// varHeaterOffTemp
    pInvoke->AddParamValue("2.1.17");				// sysCurTemp
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "200.28", IF4_SRC_MCU, NULL, NULL);
}

void mcuMcuDiagnosisEvent(BYTE *pszData, int nLength)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("1.12", VARSMI_STREAM, pszData, nLength);
    m_pEventManager->AddEvent(EVTCLASS_INFO, pInvoke, "200.105", IF4_SRC_MCU, NULL, NULL);
}

void mcuAlert(BYTE alertType, BYTE * previous, int previousLen, BYTE * current, int currentLen )
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("1.4", alertType);			// varHeaterOffTemp
    pInvoke->AddParamFormat("1.12", VARSMI_STREAM, previous, previousLen);
    pInvoke->AddParamFormat("1.12", VARSMI_STREAM, current, currentLen);
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "200.29", IF4_SRC_MCU, NULL, NULL);
}


//////////////////////////////////////////////////////////////////////
// SINK Event
//////////////////////////////////////////////////////////////////////

void sinkResetEvent(EUI64 *id, BYTE nReason)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
	if (id == NULL)
    	 pInvoke->AddParamValue("3.1.3");
    else pInvoke->AddParamFormat("3.1.3", VARSMI_EUI64, id, sizeof(EUI64));
    pInvoke->AddParam("3.1.14", (BYTE)nReason);
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "201.6", IF4_SRC_MCU, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////
// MOBILE Event
//////////////////////////////////////////////////////////////////////

void mobileConnectEvent()
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamValue("2.102.3");				// ethIpAddr
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "202.4", IF4_SRC_MCU, NULL, NULL);
}

void mobileKeepaliveEvent()
{
	COORDINATOR	*pCoordinator;
	HANDLE	codi;

	codi = GetCoordinator();
	pCoordinator = (COORDINATOR *)codi;

	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamValue("2.102.3");
    pInvoke->AddParamValue("2.104.1");
	pInvoke->AddParamFormat("3.1.3", VARSMI_EUI64, &pCoordinator->eui64, sizeof(EUI64));
	pInvoke->AddParam("3.1.12", (BYTE)codiGetState(codi) == CODISTATE_NORMAL ? 1 : 0);
	pInvoke->AddParam("3.1.9", (BYTE)0);
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "202.6", IF4_SRC_MCU, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////
// SENSOR Event
//////////////////////////////////////////////////////////////////////

void sensorJoinEvent(EUI64 *id)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "203.1", IF4_SRC_MCU, NULL, NULL);
}

void sensorLeaveEvent(EUI64 *id)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "203.2", IF4_SRC_MCU, NULL, NULL);
}

void sensorConnectEvent(EUI64 *id, BYTE nConnectType)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    pInvoke->AddParam("1.4", (BYTE)nConnectType);
    m_pEventManager->AddEvent(EVTCLASS_NORMAL, pInvoke, "203.4", IF4_SRC_MCU, NULL, NULL);
}

void sensorDisconnectEvent(EUI64 *id)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    m_pEventManager->AddEvent(EVTCLASS_NORMAL, pInvoke, "203.5", IF4_SRC_MCU, NULL, NULL);
}

void sensorTimeoutEvent(EUI64 *id)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    m_pEventManager->AddEvent(EVTCLASS_NORMAL, pInvoke, "203.6", IF4_SRC_MCU, NULL, NULL);
}

void sensorBreakingEvent(EUI64 *id)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    m_pEventManager->AddEvent(EVTCLASS_NORMAL, pInvoke, "203.7", IF4_SRC_MCU, NULL, NULL);
}

void sensorUnknownEvent(EUI64 *id, int nType)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    pInvoke->AddParam("4.1.2", (BYTE)nType);
    m_pEventManager->AddEvent(EVTCLASS_NORMAL, pInvoke, "203.8", IF4_SRC_MCU, NULL, NULL);
}

void sensorResetEvent(EUI64 *id)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    m_pEventManager->AddEvent(EVTCLASS_NORMAL, pInvoke, "203.9", IF4_SRC_MCU, NULL, NULL);
}

void sensorAlarmEvent(EUI64 *id, GMTTIMESTAMP *pTime, BYTE nEvent, BYTE *pStatus)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    ENDI_EVENT_STATUS * est = (ENDI_EVENT_STATUS *) pStatus;

    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    pInvoke->AddParamFormat("1.21", VARSMI_STREAM, (char *)pTime, sizeof(GMTTIMESTAMP));
    pInvoke->AddParam("1.4", nEvent);
    pInvoke->AddParam("1.6", est->val.u32);
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "203.10", IF4_SRC_MCU, NULL, NULL);
}

void sensorPowerOutage(GMTTIMESTAMP *pTime, EUI64 *ids, int nCount, BYTE nEvent)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("1.4", nEvent);
    pInvoke->AddParamFormat("1.21", VARSMI_STREAM, (char *)pTime, sizeof(GMTTIMESTAMP));
    pInvoke->AddParamFormat("4.1.1", VARSMI_STREAM, ids, sizeof(EUI64)*nCount);
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "203.107", IF4_SRC_MCU, NULL, NULL);
}

void sensorInstallEvent(EUI64 *id, int nType, int nService, char *meterid, char *vendor, char *model, BYTE networkType)
{
    CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, (void *)id, sizeof(EUI64));
    pInvoke->AddParam("4.1.2", nType);
    pInvoke->AddParam("4.1.11", nService);
    pInvoke->AddParamFormat("10.3.2", VARSMI_STRING, (void *)meterid, strlen(meterid));
    pInvoke->AddParamFormat("1.11", VARSMI_STRING, (void *)vendor, strlen(vendor));
    pInvoke->AddParamFormat("1.11", VARSMI_STRING, (void *)model, strlen(model));
    pInvoke->AddParam("4.3.5", networkType);
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "203.105", IF4_SRC_MCU, NULL, NULL);
}

void sensorUninstallEvent(EUI64 *id)
{
    CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, (void *)id, sizeof(EUI64));
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "203.106", IF4_SRC_MCU, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////
// malFunction Event
//////////////////////////////////////////////////////////////////////

void malfDiskErrorEvent(UINT nTotal, UINT nFree, UINT nUse)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("2.101.1", (UINT)nTotal);
    pInvoke->AddParam("2.101.2", (UINT)nFree);
    pInvoke->AddParam("2.101.3", (UINT)nUse);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "204.4", IF4_SRC_MCU, NULL, NULL);
}

void malfDiskErrorRestoreEvent(UINT nTotal, UINT nFree, UINT nUse)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("2.101.1", (UINT)nTotal);
    pInvoke->AddParam("2.101.2", (UINT)nFree);
    pInvoke->AddParam("2.101.3", (UINT)nUse);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "204.5", IF4_SRC_MCU, NULL, NULL);
}

void malfMemoryErrorEvent(UINT nTotal, UINT nFree, UINT nUse, UINT nCache, UINT nBuffer)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("2.100.1", (UINT)nTotal);
    pInvoke->AddParam("2.100.2", (UINT)nFree);
    pInvoke->AddParam("2.100.3", (UINT)nUse);
    pInvoke->AddParam("2.100.4", (UINT)nCache);
    pInvoke->AddParam("2.100.5", (UINT)nBuffer);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "204.6", IF4_SRC_MCU, NULL, NULL);
}

void malfMemoryErrorRestoreEvent(UINT nTotal, UINT nFree, UINT nUse, UINT nCache, UINT nBuffer)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("2.100.1", (UINT)nTotal);
    pInvoke->AddParam("2.100.2", (UINT)nFree);
    pInvoke->AddParam("2.100.3", (UINT)nUse);
    pInvoke->AddParam("2.100.4", (UINT)nCache);
    pInvoke->AddParam("2.100.5", (UINT)nBuffer);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "204.7", IF4_SRC_MCU, NULL, NULL);
}

void malfWatchdogResetEvent(char *pszName)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("1.11", pszName);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "204.8", IF4_SRC_MCU, NULL, NULL);
}

void malfGarbageCleaningEvent(BYTE eventType, BYTE *eventData, int dataLength)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParam("1.4", eventType);
    if(eventData) pInvoke->AddParamFormat("1.12", VARSMI_STREAM, eventData, dataLength);
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "204.9", IF4_SRC_MCU, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////
// Communication Event
//////////////////////////////////////////////////////////////////////

void commZruErrorEvent(EUI64 *id, BYTE nState)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, (void *)id, sizeof(EUI64));
    pInvoke->AddParam("1.4", nState);
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "205.1", IF4_SRC_MCU, NULL, NULL);
}

void commZmuErrorEvent(EUI64 *id, BYTE nState)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, (void *)id, sizeof(EUI64));
    pInvoke->AddParam("1.4", nState);
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "205.2", IF4_SRC_MCU, NULL, NULL);
}

void commZeuErrorEvent(EUI64 *id, BYTE nState)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, (void *)id, sizeof(EUI64));
    pInvoke->AddParam("1.4", nState);
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "205.3", IF4_SRC_MCU, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////
// Meter Event
//////////////////////////////////////////////////////////////////////

void meterErrorEvent(EUI64 *id, char *meterId, UINT errorCode, UINT errorStatus, BYTE * errorData, UINT errorDataLen)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    char dummy[0];

    pInvoke->AddParamFormat("4.1.1", VARSMI_EUI64, id, sizeof(EUI64));
    pInvoke->AddParamFormat("1.11", VARSMI_STRING, meterId ? meterId : dummy, meterId ? strlen(meterId) : 0);
    pInvoke->AddParam("1.6", errorCode);
    pInvoke->AddParam("1.6", errorStatus);
    if(errorData && errorDataLen) pInvoke->AddParamFormat("1.12", VARSMI_STREAM, errorData, errorDataLen);
    m_pEventManager->AddEvent(EVTCLASS_MINOR, pInvoke, "212.1", IF4_SRC_MCU, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////
// Transaction Event
//////////////////////////////////////////////////////////////////////

void transactionEvent(WORD nTrID, BYTE nEvtType, BYTE nOption, cetime_t nCTime, cetime_t nLTime, INT nErrCode, UINT nResultCnt, MIBValue * pResult)
{
	CIF4Invoke      *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    MIBValue        *pMib = pResult;
    TIMESTAMP       cTime, lTime;
    unsigned int    i;

    pInvoke->AddParam("1.5", nTrID);
    pInvoke->AddParam("1.4", nEvtType);
    pInvoke->AddParam("1.4", nOption);
    GetTimestamp(&cTime, &nCTime); pInvoke->AddParamFormat("1.16", VARSMI_TIMESTAMP, &cTime, sizeof(TIMESTAMP));
    GetTimestamp(&lTime, &nLTime); pInvoke->AddParamFormat("1.16", VARSMI_TIMESTAMP, &lTime, sizeof(TIMESTAMP));
    pInvoke->AddParam("1.9", nErrCode);
    pInvoke->AddParam("1.6", nResultCnt);
    for(i=0; i<nResultCnt && pMib; i++, pMib=pMib->pNext) {
        pInvoke->AddParam("1.10", &pMib->oid);
        pInvoke->AddParam("1.9", pMib->len);
        pInvoke->AddParamFormat("1.12", VARSMI_STREAM, pMib->stream.p, pMib->len);
    }
    m_pEventManager->AddEvent(EVTCLASS_CRITICAL, pInvoke, "215.1", IF4_SRC_MCU, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////
// OTA Event
//////////////////////////////////////////////////////////////////////

void otaDownloadEvent(char *pszTriggerId, BYTE errorCode)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("1.11", VARSMI_STRING, (void *)pszTriggerId, strlen(pszTriggerId));
    pInvoke->AddParam("1.4", (BYTE)errorCode);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "214.1", IF4_SRC_MCU, NULL, NULL);
}

void otaStartEvent(char *pszTriggerId)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("1.11", VARSMI_STRING, (void *)pszTriggerId, strlen(pszTriggerId));
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "214.2", IF4_SRC_MCU, NULL, NULL);
}

void otaEndEvent(char *pszTriggerId, BYTE reason, int nTotal, int nSuccess, int nError)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("1.11", VARSMI_STRING, (void *)pszTriggerId, strlen(pszTriggerId));
    pInvoke->AddParam("1.4", (BYTE)reason);
    pInvoke->AddParam("1.6", (UINT)nTotal);
    pInvoke->AddParam("1.6", (UINT)nSuccess);
    pInvoke->AddParam("1.6", (UINT)nError);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "214.3", IF4_SRC_MCU, NULL, NULL);
}

void otaResultEvent(char *pszTriggerId, EUI64 *id, BYTE nType, BYTE nOtaStep, BYTE errorCode)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    pInvoke->AddParamFormat("1.11", VARSMI_STRING, (void *)pszTriggerId, strlen(pszTriggerId));
    pInvoke->AddParamFormat("1.14", VARSMI_EUI64, (void *)id, sizeof(EUI64));
    pInvoke->AddParam("1.4", (BYTE)nType);
    pInvoke->AddParam("1.4", (BYTE)nOtaStep);
    pInvoke->AddParam("1.4", (BYTE)errorCode);
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "214.4", IF4_SRC_MCU, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////
// Energy Level Event
//////////////////////////////////////////////////////////////////////

/** Device의 Energy Level이 변경되었을 경우 불려진다.
  */
void energyLevelChangeEvent(const EUI64 *pId, cetime_t nIssue, int nErrorCode, BYTE nPreviousLevel, BYTE nCurrentLevel, 
        BYTE *pszUserData, int nUserDataLen)
{
	CIF4Invoke  *pInvoke = new CIF4Invoke(m_szFepServer, m_nFepAlarmPort, DEFAULT_TIMEOUT);
    TIMESTAMP       issueTime;

    pInvoke->AddParamFormat("1.14", VARSMI_EUI64, (void *)pId, sizeof(EUI64));
    GetTimestamp(&issueTime, &nIssue); pInvoke->AddParamFormat("1.16", VARSMI_TIMESTAMP, &issueTime, sizeof(TIMESTAMP));
    pInvoke->AddParam("1.9", nErrorCode);
    pInvoke->AddParam("1.4", nPreviousLevel);
    pInvoke->AddParam("1.4", nCurrentLevel);
    if(pszUserData != NULL &&  nUserDataLen > 0) {
        pInvoke->AddParamFormat("1.12", VARSMI_STREAM, pszUserData, nUserDataLen);
    }
    m_pEventManager->AddEvent(EVTCLASS_MAJOR, pInvoke, "203.109", IF4_SRC_MCU, NULL, NULL);
}

