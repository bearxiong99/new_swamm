////////////////////////////////////////////////////////////////////
//
//  Coordinator.cpp: implementation of the CCoordinator class.
//
//  This file is a part of the AIMIR.
//  (c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  http://www.nuritelecom.com
//  casir@paran.com
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "codiapi.h"
#include "Coordinator.h"
#include "CoordinatorUtility.h"
#include "DebugUtil.h"
#include "DeviceClass.h"

#include "CoordinatorServer.h"

////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordinator	m_theCoordinator;
CCoordinator	*m_ptheCoordinator = NULL;

////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordinator::CCoordinator()
{
	m_ptheCoordinator = this;
	m_bInitialized = FALSE;
}

CCoordinator::~CCoordinator()
{
}

////////////////////////////////////////////////////////////////////
// Coordinator functions
//////////////////////////////////////////////////////////////////////

int CCoordinator::Initialize()
{
	if (m_bInitialized)
		return CODIERR_ALREADY_INITIALIZED;

	StartupStreamWorker();
	m_bInitialized = TRUE;
	return CODIERR_NOERROR;	
}

int CCoordinator::Exit()
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;

	ShutdownStreamWorker();
    UnregisterCoordinatorAll();
    ShutdownCoordinatorAll();

	m_bInitialized = FALSE;
	return CODIERR_NOERROR;	
}

int CCoordinator::Register(COORDINATOR **codi, CODIDEVICE *pDevice)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (pDevice == NULL)
		return CODIERR_INVALID_DEVICE;

	return RegisterCoordinator(codi, pDevice);
}

int CCoordinator::Unregister(COORDINATOR *codi)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	return UnregisterCoordinator(codi);
}

int CCoordinator::Startup(COORDINATOR *codi)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	return StartupCoordinator(codi);	
}

int CCoordinator::Shutdown(COORDINATOR *codi)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	return ShutdownCoordinator(codi);	
}

int CCoordinator::GetProperty(COORDINATOR *codi, BYTE nProperty, BYTE *payload, int *nLength, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (payload == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength == NULL)
		return CODIERR_INVALID_LENGTH;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;

	return GetCoordinatorProperty(codi, nProperty, payload, nLength, nTimeout);	
}

int CCoordinator::SetProperty(COORDINATOR *codi, BYTE nProperty, BYTE *payload, int nLength, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (payload == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;

	return SetCoordinatorProperty(codi, nProperty, payload, nLength, nTimeout);	
}

int CCoordinator::Multicast(COORDINATOR *codi, BYTE type, BYTE *payload, int nLength)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (payload == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;

	return MulticastToEndDevice(codi, type, payload, nLength);	
}

int CCoordinator::Sync(COORDINATOR *codi, int nInterval)
{
	CODI_MODULE_PAYLOAD			*pModule;
	CODI_SERIAL_PAYLOAD			*pSerial;
	CODI_NETWORK_PAYLOAD		*pNetwork;
	CODI_SECURITY_PAYLOAD		*pSecurity;
	CODI_STACKMEMORY_PAYLOAD	*pStack;
	CODI_SCAN_PAYLOAD			*pScan;
	CODI_PERMIT_PAYLOAD			*pPermit;
	CODI_ROUTEDISCOVERY_PAYLOAD	*pDiscovery;
	CODI_MULTICASTHOP_PAYLOAD   *pHops;
	cetime_t	now;
	BYTE	szBuffer[CODI_MAX_PAYLOAD_SIZE];
	int		nError, nLength = 0;

	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	// 마지막 시간 동기화 이후 경과 시간을 비교한다.
	// nInterval == 0이면 무조건 동기화 한다.
	if (nInterval > 0)
	{
		ceTime(&now);
		if ((unsigned long)(now-codi->tmLastSync) <= (unsigned long)nInterval)
			return CODIERR_NOERROR;
	}

    memset(szBuffer, 0, sizeof(szBuffer));
	nError = GetCoordinatorProperty(codi, CODI_CMD_MODULE_PARAM, szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return nError;

	pModule = (CODI_MODULE_PAYLOAD *)szBuffer;
    codi->shortid	= BigToHostShort(pModule->shortid);

    memset(szBuffer, 0, sizeof(szBuffer));
	nError = GetCoordinatorProperty(codi, CODI_CMD_SERIAL_PARAM, szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return nError;

	pSerial = (CODI_SERIAL_PAYLOAD *)szBuffer;
	codi->baudrate	= pSerial->baudrate;
	codi->parity	= pSerial->parity;
	codi->databit	= pSerial->databit;
	codi->stopbit	= pSerial->stopbit;
	codi->rtscts	= pSerial->rtscts;
	
    memset(szBuffer, 0, sizeof(szBuffer));
	nError = GetCoordinatorProperty(codi, CODI_CMD_NETWORK_PARAM, szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return nError;

	pNetwork = (CODI_NETWORK_PAYLOAD *)szBuffer;
	codi->autoset	= pNetwork->autoset;
	codi->channel	= pNetwork->channel;
	codi->panid		= ntoshort(pNetwork->panid);
	codi->power		= pNetwork->power;
	codi->txpowermode = pNetwork->txpowermode;
	memcpy(codi->extpanid, pNetwork->extpanid, 8);

    memset(szBuffer, 0, sizeof(szBuffer));
	nError = GetCoordinatorProperty(codi, CODI_CMD_SECURITY_PARAM, szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return nError;

	pSecurity = (CODI_SECURITY_PAYLOAD *)szBuffer;
	codi->security	= pSecurity->security;
	memcpy(codi->link_key, pSecurity->link_key, 16);
	memcpy(codi->network_key, pSecurity->network_key, 16);

    memset(szBuffer, 0, sizeof(szBuffer));
	nError = GetCoordinatorProperty(codi, CODI_CMD_STACK_MEMORY, szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return nError;

	pStack = (CODI_STACKMEMORY_PAYLOAD *)szBuffer;
	codi->address_table_size			= pStack->ADDRESS_TABLE_SIZE;
	codi->whole_address_table_size	    = pStack->WHOLE_ADDRESS_TABLE_SIZE;
	codi->neighbor_table_size			= pStack->NEIGHBOR_TABLE_SIZE;
	codi->source_route_table_size	    = pStack->SOURCE_ROUTE_TABLE_SIZE;
	codi->route_table_size				= pStack->ROUTE_TABLE_SIZE;
	codi->max_hops						= pStack->MAX_HOPS;
	codi->packet_buffer_count			= pStack->PACKET_BUFFER_COUNT;
    if(nLength >= 8) // Issue #2076, #2079 : swVersion이 추가되지 않은 Coordinator를 위한 Magic Code
    {
        codi->swVersion                 = BigToHostShort(pStack->SOFTWARE_VERSION);
    }
    else
    {
        codi->swVersion                 = 0;
    }
    if(nLength >= 10) // Issue #2490 : Key Table Size가 추가되지 않은 Coordinator를 위한 Magic Code
    {
        codi->key_table_size            = pStack->KEY_TABLE_SIZE;
        codi->max_children              = pStack->MAX_CHILDREN;
    }
    else
    {
        codi->key_table_size            = 0;
        codi->max_children              = 0;
    }

    memset(szBuffer, 0, sizeof(szBuffer));
	nError = GetCoordinatorProperty(codi, CODI_CMD_SCAN_PARAM, szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return nError;

	pScan = (CODI_SCAN_PAYLOAD *)szBuffer;
	codi->scan_mask = pScan->scan_mask;
	codi->energy_scan_duration = pScan->energy_scan_duration;
	codi->active_scan_duration = pScan->active_scan_duration;

    memset(szBuffer, 0, sizeof(szBuffer));
	nError = GetCoordinatorProperty(codi, CODI_CMD_PERMIT, szBuffer, &nLength, 3000);
	if (nError != CODIERR_NOERROR)
		return nError;

	pPermit = (CODI_PERMIT_PAYLOAD *)szBuffer;
	codi->permit_time = pPermit->permit_time;

    /** Issue #1619 : Router Discovery
     ** Issue #1621 : Multicast Frame
     **
     ** 두가지의 추가적인 기능을 지원한다.
     ** 이 기능은 Coordinator v2.1 b12 이상에서 지원되기 때문에 버전 검사를 수행한다.
     */
    if((codi->fw == 0x21 && codi->build >= 0x12) || codi->fw > 0x21) {
        memset(szBuffer, 0, sizeof(szBuffer));
	    nError = GetCoordinatorProperty(codi, CODI_CMD_ROUTE_DISCOVERY, szBuffer, &nLength, 3000);
	    if (nError != CODIERR_NOERROR)
		    return nError;

	    pDiscovery = (CODI_ROUTEDISCOVERY_PAYLOAD *)szBuffer;
	    codi->discovery = pDiscovery->type;

	    nError = GetCoordinatorProperty(codi, CODI_CMD_MULTICAST_HOP, szBuffer, &nLength, 3000);
	    if (nError != CODIERR_NOERROR)
		    return nError;

	    pHops = (CODI_MULTICASTHOP_PAYLOAD *)szBuffer;
	    codi->mhops = pHops->hop;
    }

	ceTime(&codi->tmLastSync);
	return CODIERR_NOERROR;
}

int CCoordinator::Reset(COORDINATOR *codi)
{       
    if (!m_bInitialized)
        return CODIERR_NOT_INITIALIZED;
    
    if (codi == NULL)
        return CODIERR_INVALID_HANDLE;
    
    if (codi->nIdentifier != CODI_IDENTIFIER)
        return CODIERR_INVALID_HANDLE;

    if (!codi->bStarted)
        return CODIERR_NOT_STARTED;
    
    return CCoordinatorServer::ResetCoordinator(codi);
}

int CCoordinator::Bootload(COORDINATOR *codi, int nType)
{
    if (!m_bInitialized)
        return CODIERR_NOT_INITIALIZED;
    
    if (codi == NULL)
        return CODIERR_INVALID_HANDLE;
    
    if (codi->nIdentifier != CODI_IDENTIFIER)
        return CODIERR_INVALID_HANDLE;

    if (!codi->bStarted)
        return CODIERR_NOT_STARTED;
   
    return CCoordinatorServer::BootloadCoordinator(codi, nType);
}

int CCoordinator::Download(COORDINATOR *codi, int nModemType, const char *pszFileName, const char *pszExt)
{
	int		nError;

    if (!m_bInitialized)
        return CODIERR_NOT_INITIALIZED;
    
    if (codi == NULL)
        return CODIERR_INVALID_HANDLE;
    
    if (codi->nIdentifier != CODI_IDENTIFIER)
        return CODIERR_INVALID_HANDLE;

    if (!codi->bStarted)
        return CODIERR_NOT_STARTED;

	nError = CCoordinatorServer::DownloadCoordinator(codi, nModemType, pszFileName, pszExt);
	if (nError != CODIERR_NOERROR)
		return nError;

	return CODIERR_NOERROR;
}

int CCoordinator::Playback(COORDINATOR *codi, BYTE *frame, int nLength)
{
	if (frame == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;

    if (!m_bInitialized)
        return CODIERR_NOT_INITIALIZED;
    
    if (codi == NULL)
        return CODIERR_INVALID_HANDLE;
    
    if (codi->nIdentifier != CODI_IDENTIFIER)
        return CODIERR_INVALID_HANDLE;

    if (!codi->bStarted)
        return CODIERR_NOT_STARTED;
   
    return PlaybackCoordinator(codi, frame, nLength);
}

int CCoordinator::SetOption(COORDINATOR *codi, int nOption)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	return SetOptionCoordinator(codi, nOption);	
}

int CCoordinator::GetOption(COORDINATOR *codi, int *nOption)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (nOption == NULL)
		return CODIERR_INVALID_PARAM;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	return GetOptionCoordinator(codi, nOption);	
}


////////////////////////////////////////////////////////////////////
// End Device functions
//////////////////////////////////////////////////////////////////////

int CCoordinator::EndiOpen(COORDINATOR *codi, ENDDEVICE **endi, EUI64 *id)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (GetState(codi) != CODISTATE_NORMAL)
		return CODIERR_COORDINATOR_NOT_READY;

	return OpenEndDevice(codi, endi, id);
}

int CCoordinator::EndiClose(ENDDEVICE *endi)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;
	
	return CloseEndDevice(endi);
}

HANDLE CCoordinator::EndiGetDevice(EUI64 *id)
{
	if (!m_bInitialized)
		return NULL;
	
	if (id == NULL)
		return NULL;

	return GetEndDeviceByID(id);
}

int CCoordinator::EndiConnect(ENDDEVICE *endi, WORD shortid, BYTE hops, WORD *path, int nTimeout, BOOL bExtenedTimeout, BOOL bDirect, BOOL bEndDevice)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	if (GetState(endi->codi) != CODISTATE_NORMAL)
		return CODIERR_COORDINATOR_NOT_READY;

	if ((shortid == 0) && (hops == 0) && (path == NULL) && (bDirect == FALSE))
	{
		WORD	nShortID = 0;
		BYTE	nHops = 0;
		WORD	arPath[CODI_MAX_ROUTING_PATH];

		XDEBUG(" +++++++++++++++++++++++++++++debug sungyeung ((shortid == 0) && (hops == 0) && (path == NULL) && (bDirect == FALSE)) : %s : %d \n", __FILE__, __LINE__);

		codiOnQueryRouteRecord(endi->codi, &endi->id, &nShortID, &nHops, arPath);
		return ConnectEndDevice(endi, nShortID, nHops, arPath, nTimeout, bExtenedTimeout, bDirect, bEndDevice);		
	}

	XDEBUG(" +++++++++++++++++++++++++++++debug sungyeung CCoordinator::EndiConnect : %s : %d \n", __FILE__, __LINE__);

	return ConnectEndDevice(endi, shortid, hops, path, nTimeout, bExtenedTimeout, bDirect, bEndDevice);		
}

int CCoordinator::EndiDisconnect(ENDDEVICE *endi)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (!endi->bConnected)
		return CODIERR_NOT_CONNECTED;

	return DisconnectEndDevice(endi);		
}

int CCoordinator::EndiSend(ENDDEVICE *endi, BYTE rw, BYTE type, BYTE *payload, int nLength, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if ((rw != CODI_ACCESS_READ) && (rw != CODI_ACCESS_WRITE))
		return CODIERR_INVALID_METHOD;

	if (payload == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return SendToEndDevice(endi, rw, type, payload, nLength, nTimeout);		
}

int CCoordinator::EndiSendRecv(ENDDEVICE *endi, BYTE rw, BYTE type, BYTE *payload, int nLength, BYTE *data, int *datalen, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if ((rw != CODI_ACCESS_READ) && (rw != CODI_ACCESS_WRITE))
		return CODIERR_INVALID_METHOD;

	if (payload == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return SendRecvToEndDevice(endi, rw, type, payload, nLength, data, datalen, nTimeout);		
}

int CCoordinator::EndiGetOption(ENDDEVICE *endi, int nOption, int *nValue)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (nValue == NULL)
		return CODIERR_INVALID_PARAM;	

	return GetOptionEndDevice(endi, nOption, nValue);
}

int CCoordinator::EndiSetOption(ENDDEVICE *endi, int nOption, int nValue)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;
	
	return SetOptionEndDevice(endi, nOption, nValue);
}

////////////////////////////////////////////////////////////////////
// End Device Utility functions
//////////////////////////////////////////////////////////////////////

int CCoordinator::EndiBypass(ENDDEVICE *endi, BYTE *payload, int nLength, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (payload == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;
	
	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return BypassEndDevice(endi, payload, nLength, nTimeout);
}

int CCoordinator::EndiBypass(COORDINATOR *codi, BYTE *payload, int nLength)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (payload == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;
	
	return BypassEndDevice(codi, payload, nLength);
}


int CCoordinator::EndiLeave(ENDDEVICE *endi, BYTE joinTime, BYTE channel, WORD panid, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;
	
	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return LeaveEndDevice(endi, joinTime, channel, panid, nTimeout);
}

int CCoordinator::EndiReadAmr(ENDDEVICE *endi, BYTE *data, int *nLength, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (data == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength == NULL)
		return CODIERR_INVALID_LENGTH;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return ReadAmrEndDevice(endi, data, nLength, nTimeout);
}

int CCoordinator::EndiWriteAmr(ENDDEVICE *endi, WORD mask, BYTE *data, int nLength, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (data == NULL)
		return CODIERR_INVALID_PARAM;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;
	
	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return WriteAmrEndDevice(endi, mask, data, nLength, nTimeout);
}

int CCoordinator::EndiReadRom(ENDDEVICE *endi, WORD addr, int nLength, BYTE *data, int *nSize, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (data == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;

	if (nSize == NULL)
		return CODIERR_INVALID_PARAM;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return ReadRomEndDevice(endi, addr, nLength, data, nSize, nTimeout);
}

int CCoordinator::EndiWriteRom(ENDDEVICE *endi, WORD addr, BYTE *data, int nLength, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (data == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;
	
	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return WriteRomEndDevice(endi, addr, data, nLength, nTimeout);
}

int CCoordinator::EndiWriteRom(COORDINATOR *codi, WORD addr, BYTE *data, int nLength)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (data == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;
	
	return WriteRomEndDevice(codi, addr, data, nLength);
}

int CCoordinator::EndiReadLongRom(ENDDEVICE *endi, UINT addr, int nLength, BYTE *data, int *nSize, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (data == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;

	if (nSize == NULL)
		return CODIERR_INVALID_PARAM;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return ReadLongRomEndDevice(endi, addr, nLength, data, nSize, nTimeout);
}

int CCoordinator::EndiWriteLongRom(ENDDEVICE *endi, UINT addr, BYTE *data, int nLength, BYTE *replay, int *nSize, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (data == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;
	
	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	return WriteLongRomEndDevice(endi, addr, data, nLength, replay, nSize, nTimeout);
}

int CCoordinator::EndiWriteLongRom(COORDINATOR *codi, UINT addr, BYTE *data, int nLength)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (data == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nLength <= 0)
		return CODIERR_INVALID_LENGTH;
	
	return WriteLongRomEndDevice(codi, addr, data, nLength);
}

int CCoordinator::EndiWriteCid(ENDDEVICE *endi, BYTE *value,int valuelen,  BYTE *data, int *datalen, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;
	
	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (value == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	//XDEBUG("CCoordinator :: received %x (%d)\xd\xa", value[0], valuelen);
	return WriteCidEndDevice(endi, value, valuelen, data, datalen, nTimeout);
}

int CCoordinator::EndiWriteMccb(ENDDEVICE *endi, BYTE value, BYTE *data, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;
	
	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (data == NULL)
		return CODIERR_INVALID_BUFFER;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return WriteMccbEndDevice(endi, value, data, nTimeout);
}

int CCoordinator::EndiCommandWrite(ENDDEVICE *endi, BYTE cmd, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;
	
	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return CommandEndDevice(endi, cmd, ENDI_CMD_WRITE_REQUEST, data, nLength, result, nSize, nTimeout);
}

int CCoordinator::EndiCommandRead(ENDDEVICE *endi, BYTE cmd, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (endi == NULL)
		return CODIERR_INVALID_HANDLE;
	
	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;
	
	if (!endi->codi->bStarted)
		return CODIERR_NOT_STARTED;

	if (nTimeout < 0)
		return CODIERR_INVALID_TIMEOUT;
	
	return CommandEndDevice(endi, cmd, ENDI_CMD_READ_REQUEST, data, nLength, result, nSize, nTimeout);
}

int	CCoordinator::EndiBind(COORDINATOR *codi, EUI64 *id, BYTE type, int nTimeout, BOOL bExtendedTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	return BindEndDevice(codi, id, type, nTimeout, bExtendedTimeout);
}

int	CCoordinator::EndiLink(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	return LinkEndDevice(codi, id, seq, type, nTimeout);
}

int	CCoordinator::EndiWrite(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, BYTE *data, int nLength, int nTimeout)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	return WriteEndDevice(codi, id, seq, type, data, nLength, nTimeout);
}

////////////////////////////////////////////////////////////////////
// Utility functions
//////////////////////////////////////////////////////////////////////

BOOL CCoordinator::IsInitialized()
{
	return m_bInitialized;
}

BOOL CCoordinator::IsStarted(COORDINATOR *codi)
{
	if (!m_bInitialized)
		return FALSE;
	
	if (codi == NULL)
		return FALSE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return FALSE;

	return codi->bStarted;
}

BOOL CCoordinator::IsSuperParent(COORDINATOR *codi)
{
	if (!m_bInitialized)
		return FALSE;
	
	if (codi == NULL)
		return FALSE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return FALSE;

	return codi->bSuperParent;
}

BOOL CCoordinator::IsConnected(ENDDEVICE *endi)
{
	if (!m_bInitialized)
		return FALSE;
	
	if (endi == NULL)
		return FALSE;

	if (endi->nIdentifier != ENDI_IDENTIFIER)
		return FALSE;

	return endi->bConnected;
}

BOOL CCoordinator::IsError(ENDDEVICE *endi)
{
	if (!m_bInitialized)								// 초기화 되지 않음
		return TRUE;
	
	if (endi == NULL)									// 잘못된 핸들
		return TRUE;

	if (endi->nIdentifier != ENDI_IDENTIFIER)			// 잘못된 핸들
		return TRUE;

	if (!endi->bConnected)								// 연결되지 않은 상태
		return TRUE;

	if (endi->nSeqError >= 5)							// 연속 시퀀스 에러 발생
		return TRUE;

	return FALSE;										// NO ERROR
}

int CCoordinator::GetState(COORDINATOR *codi)
{
	if (!m_bInitialized)
		return FALSE;
	
	if (codi == NULL)
		return CODISTATE_ERROR;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODISTATE_ERROR;

	if (!codi->bStarted)
		return CODISTATE_NOT_STARTED;

	if (!*codi->szGUID)
		return CODISTATE_NOT_CONNECTED;

	if (codi->bNoResponse)
		return CODISTATE_NO_RESPONSE;

	switch(codi->nStackStatus) {
	  case 0 :						return CODISTATE_STACK_NOT_READY;
	  case CODI_STACK_UP :			return CODISTATE_NORMAL;
	  case CODI_STACK_PANID_CHANGE :return CODISTATE_NORMAL;
	  case CODI_STACK_DOWN :		return CODISTATE_STACK_DOWN;
	  case CODI_STACK_JOIN_FAILED :	return CODISTATE_JOIN_FAIL;
	  case CODI_STACK_NO_BEACONS :	return CODISTATE_NO_BEACONS;
	}
	return CODISTATE_ERROR;
}

BOOL CCoordinator::GetWatchdogState(COORDINATOR *codi)
{
	if (!m_bInitialized)
		return FALSE;
	
	if (codi == NULL)
		return FALSE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return FALSE;

    return codi->bWatchdog;
}

BOOL CCoordinator::SetWatchdogState(COORDINATOR *codi, BOOL bState)
{
	if (!m_bInitialized)
		return FALSE;
	
	if (codi == NULL)
		return FALSE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return FALSE;

    codi->bWatchdog = bState;

    return TRUE;
}

HANDLE CCoordinator::GetHandle(COORDINATOR *codi)
{
	CDeviceClass	*pDevice;

	if (!m_bInitialized)
		return NULL;
	
	if (codi == NULL)
		return NULL;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return NULL;

	pDevice = (CDeviceClass *)codi->pInterface;
	return (HANDLE)pDevice->GetFD(codi);
}

const char *CCoordinator::ErrorMessage(int nError)
{
	switch(nError) {
	  case CODIERR_NOERROR :					return "Success";
	  case CODIERR_INVALID_DEVICE :				return "Invalid device";
	  case CODIERR_DEVICE_BUSY :				return "Device busy";
	  case CODIERR_DEVICE_INUSE :				return "Device inuse";
	  case CODIERR_MEMORY_ERROR :				return "Out of memory";
	  case CODIERR_INVALID_HANDLE :				return "Invalid handle";
	  case CODIERR_INVALID_BUFFER :				return "Invalid buffer";
	  case CODIERR_INVALID_LENGTH :				return "Invalid length";
	  case CODIERR_INVALID_TIMEOUT :			return "Invalid timeout";
	  case CODIERR_ALREADY_INITIALIZED :		return "Already initialized";
	  case CODIERR_NOT_INITIALIZED :			return "Not initialized";
	  case CODIERR_INVALID_ID :					return "Invalid ID";
	  case CODIERR_NOT_REGISTERED :				return "Not registered";
	  case CODIERR_NOT_STARTED :				return "Not started";
	  case CODIERR_ALREADY_STARTED :			return "Already started";
	  case CODIERR_INVALID_INTERFACE :			return "Invalid interface";
	  case CODIERR_DEVICE_ERROR :				return "Device error";
	  case CODIERR_OUT_OF_BINDING :				return "Out of binding resouce";
	  case CODIERR_INUSE_ENDDEVICE :			return "Inuse end device";
	  case CODIERR_INVALID_KEY :				return "Invalid key";
	  case CODIERR_REPLY_TIMEOUT :				return "Reply timeout";
	  case CODIERR_INVALID_PARAM :				return "Invalid parameter";
	  case CODIERR_COMMAND_ERROR :				return "Command error";
	  case CODIERR_UNKNOWN_ERROR :				return "Unknown error";
	  case CODIERR_NOT_CONNECTED :				return "Not connected";
	  case CODIERR_COORDINATOR_NOT_READY :		return "Coordinator not ready";
	  case CODIERR_INVALID_OPTION :				return "Invalid option";
	  case CODIERR_INVALID_METHOD :				return "Invalid method";
	  case CODIERR_INVALID_PARENT :				return "Invalid super parent";	
	  case CODIERR_PARENT_BINDING_ERROR :		return "Parent binding error";
	  case CODIERR_SET_CHILD_ERROR :			return "Set child error";	
	  case CODIERR_PROGRESS :					return "Request progress";
	  case CODIERR_SERIAL_TIMEOUT :				return "Serial timeout";
	  case CODIERR_SERIAL_ERROR :				return "Serial error";
	  case CODIERR_FRAME_CONTROL_ERROR :		return "Frame control error";
	  case CODIERR_REQUEST_ERROR :				return "Request error";
	  case CODIERR_FRAME_TYPE_ERROR :			return "Frame type error";
	  case CODIERR_CRC16_ERROR :				return "CRC error";
	  case CODIERR_NOT_SUPPORTED :				return "Not supported";
	  case CODIERR_INVALID_DATA :				return "Invalid data";
	  case CODIERR_NO_NETWORK_ERROR :			return "No network error";
	  case CODIERR_FORM_NETWORK_ERROR :			return "Form(Join) network error";
	  case CODIERR_LEAVE_NETWORK_ERROR :		return "Leave network error";
	  case CODIERR_BIND_ADDRESS_DUPLICATION : 	return "Bind address duplication error";
	  case CODIERR_FULL_BINDING_INDEX :			return "Full binding index error";
	  case CODIERR_EUI64_FOUND_FAIL_ERROR : 	return "EUI64 found fail error";
	  case CODIERR_SET_BINDING_ERROR :			return "Set binding error";
	  case CODIERR_DELETE_BINDING_ERROR :		return "Delete binding error";
	  case CODIERR_BINDING_IS_ACTIVE_ERROR : 	return "Binding is active error";
	  case CODIERR_SEND_DATA_ERROR :			return "Send data error";
	  case CODIERR_PAYLOAD_LENGTH_ERROR :		return "Payload length error";
	  case CODIERR_DELIVERY_ERROR :				return "Delivery error";
	  case CODIERR_BINDING_ERROR :				return "Binding error";
	}
	return "Uknown error";
}

cetime_t CCoordinator::GetLastCommunicationTime(COORDINATOR *codi)
{
	if (!m_bInitialized)
		return CODIERR_NOT_INITIALIZED;
	
	if (codi == NULL)
		return CODIERR_INVALID_HANDLE;

	if (codi->nIdentifier != CODI_IDENTIFIER)
		return CODIERR_INVALID_HANDLE;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	return codi->tmLastComm;
}

int CCoordinator::GetVersion(BYTE *nMajor, BYTE *nMinor, BYTE *nRevision)
{
	if (!nMajor || !nMinor || !nRevision)
		return CODIERR_INVALID_BUFFER;

	*nMajor		= CODIAPI_MAJOR_VERSION;
	*nMinor		= CODIAPI_MINOR_VERSION;
	*nRevision	= CODIAPI_REVISION;
	return CODIERR_NOERROR;
}

