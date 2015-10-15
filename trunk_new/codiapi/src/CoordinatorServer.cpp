///////////////////////////////////////////////////////////////////
//
//  CoordinatorServer.cpp: implementation of the CCoordinatorServerServer class.
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
#include "CoordinatorServer.h"
#include "CoordinatorCrcCheck.h"
#include "CoordinatorMessage.h"
#include "CoordinatorUtility.h"
#include "SerialInterface.h"
#include "VirtualInterface.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#ifndef _WIN32
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>

#include "ShellCommand.h"
#endif

////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordinatorServer::CCoordinatorServer()
{
    m_nCoordinatorOption = CODI_OPTION_AIMIR | CODI_OPTION_LINK;
}

CCoordinatorServer::~CCoordinatorServer()
{
}

////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

void CCoordinatorServer::StartupStreamWorker()
{

	ceTime(&m_tmLastStateCheck);
	CTimeoutThread::StartupThread(2);
}

void CCoordinatorServer::ShutdownStreamWorker()
{
	
	CTimeoutThread::ShutdownThread();
}

int CCoordinatorServer::RegisterCoordinator(COORDINATOR **pReturn, CODIDEVICE *pDevice)
{
	COORDINATOR	*codi;
	int			nError;
	
	if (!*pDevice->szDevice)
		return CODIERR_INVALID_DEVICE;

	m_codiLocker.Lock();
	codi = FindCoordinatorByName(pDevice->szDevice);
	if (codi == NULL)
	{
		codi = AddCoordinator(pDevice);
		if (codi != NULL)
			 nError = CODIERR_NOERROR;
		else nError = CODIERR_MEMORY_ERROR;
		*pReturn = codi;
	}
	else nError = CODIERR_DEVICE_INUSE;
	m_codiLocker.Unlock();
	return nError;
}

int CCoordinatorServer::UnregisterCoordinator(COORDINATOR *codi)
{
	
	m_codiLocker.Lock();
	RemoveCoordinator(codi);
	m_codiLocker.Unlock();
	return CODIERR_NOERROR;
}

int CCoordinatorServer::UnregisterCoordinatorAll()
{
	COORDINATOR	*codi, *del;
	POSITION	pos;

	m_codiLocker.Lock();
	codi = m_codiList.GetFirst(pos);
	while(codi)
	{
		del = codi;
		codi = m_codiList.GetNext(pos);
		RemoveCoordinator(del);
	}
	m_codiList.RemoveAll();
	m_codiLocker.Unlock();
	return CODIERR_NOERROR;
}

int CCoordinatorServer::StartupCoordinator(COORDINATOR *codi)
{
	COORDINATOR	*pFind;
	int			nError;
	
	m_codiLocker.Lock();
	pFind = FindCoordinatorByHandle(codi);
	if (pFind)
		 nError = StartCoordinator(pFind);
	else nError = CODIERR_NOT_REGISTERED;
	m_codiLocker.Unlock();
	return nError;
}

int CCoordinatorServer::StartupCoordinatorAll()
{
	COORDINATOR	*codi;
	POSITION	pos;
	int			nError = CODIERR_NOERROR;

	m_codiLocker.Lock();
	codi = m_codiList.GetFirst(pos);
	while(codi)
	{
		nError = StartCoordinator(codi);
		if (nError != CODIERR_NOERROR)
			break;
		codi = m_codiList.GetNext(pos);
	}
	m_codiLocker.Unlock();
	return nError;
}

int CCoordinatorServer::ShutdownCoordinator(COORDINATOR *codi)
{
	COORDINATOR	*pFind;
	int			nError;

	m_codiLocker.Lock();
	pFind = FindCoordinatorByHandle(codi);
	if (pFind)
		 nError = StopCoordinator(pFind);	
	else nError = CODIERR_NOT_REGISTERED;
	m_codiLocker.Unlock();
	return nError;
}

int CCoordinatorServer::ShutdownCoordinatorAll()
{
	COORDINATOR	*codi, *target;
	POSITION	pos;

	m_codiLocker.Lock();
	codi = m_codiList.GetFirst(pos);
	while(codi)
	{
		target = codi;
		codi = m_codiList.GetNext(pos);

		// Stop시에는 에러를 무시한다.
		StopCoordinator(target);
	}
	m_codiLocker.Unlock();
	return CODIERR_NOERROR;
}

int CCoordinatorServer::GetCoordinatorProperty(COORDINATOR *codi, BYTE nProperty, BYTE *payload, int *nLength, int nTimeout)
{
	COORDINATOR	*pFind;
	CODI_COMMAND_FRAME	frame;
	int			len;

	m_codiLocker.Lock();
	pFind = FindCoordinatorByHandle(codi);
	m_codiLocker.Unlock();

	if (pFind == NULL)
		return CODIERR_NOT_REGISTERED;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	len = MakeCommandFrame(&frame, CODI_ACCESS_READ, nProperty, NULL, 0);
	if (nTimeout == 0)
	{
		SendToCoordinator(codi, (BYTE *)&frame, len);
		return CODIERR_PROGRESS;
	}

	return WaitingForCommandReply(codi, CODI_ACCESS_READ, nProperty, frame.hdr.seq,
						(BYTE *)&frame, len, payload, nLength, nTimeout);
}

int CCoordinatorServer::SetCoordinatorProperty(COORDINATOR *codi, BYTE nProperty, BYTE *payload, int nLength, int nTimeout)
{
	COORDINATOR	*pFind;
	CODI_COMMAND_FRAME	frame;
	int			len;

	m_codiLocker.Lock();
	pFind = FindCoordinatorByHandle(codi);
	m_codiLocker.Unlock();

	if (pFind == NULL)
		return CODIERR_NOT_REGISTERED;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	len = MakeCommandFrame(&frame, CODI_ACCESS_WRITE, nProperty, payload, nLength);
	if (nTimeout == 0)
	{
		SendToCoordinator(codi, (BYTE *)&frame, len);
		return CODIERR_PROGRESS;
	}

	return WaitingForCommandReply(codi, CODI_ACCESS_WRITE, nProperty, frame.hdr.seq,
						(BYTE *)&frame, len, NULL, NULL, nTimeout);
}

int CCoordinatorServer::SetOptionCoordinator(COORDINATOR *codi, int nOption)
{
	COORDINATOR	*pFind;
	int			nError = CODIERR_NOERROR;

	m_codiLocker.Lock();
	pFind = FindCoordinatorByHandle(codi);
	if (pFind)
    {
        m_nCoordinatorOption = nOption;
    }
	else
    {
        nError = CODIERR_NOT_REGISTERED;
    }
	m_codiLocker.Unlock();
	return nError;
}

int CCoordinatorServer::GetOptionCoordinator(COORDINATOR *codi, int *nOption)
{
	COORDINATOR	*pFind;
	int			nError = CODIERR_NOERROR;

	m_codiLocker.Lock();
	pFind = FindCoordinatorByHandle(codi);
	if (pFind)
    {
        *nOption = m_nCoordinatorOption;
    }
	else
    {
        nError = CODIERR_NOT_REGISTERED;
    }
	m_codiLocker.Unlock();
	return nError;
}


int CCoordinatorServer::MulticastToEndDevice(COORDINATOR *codi, BYTE type, BYTE *payload, int nLength)
{
	COORDINATOR	*pFind;
	ENDI_DATA_FRAME		frame;
	int			len;

	m_codiLocker.Lock();
	pFind = FindCoordinatorByName(codi->device.szDevice);
	m_codiLocker.Unlock();

	if (pFind == NULL)
		return CODIERR_NOT_REGISTERED;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	len = MakeMulticastFrame(&frame, 0xFF, type, payload, nLength);
	SendToCoordinator(codi, (BYTE *)&frame, len);
	return CODIERR_NOERROR;
}

HANDLE CCoordinatorServer::GetEndDeviceByID(EUI64 *id)
{
	return (HANDLE)CEndDevicePool::GetEndDeviceByID(id);	
}

int CCoordinatorServer::OpenEndDevice(COORDINATOR *codi, ENDDEVICE **endi, EUI64 *id)
{
	return CEndDevicePool::NewEndDevice(codi, endi, id);	
}

int CCoordinatorServer::CloseEndDevice(ENDDEVICE *endi)
{
	int		nError = CODIERR_NOERROR;;
	EUI64	parent;
	WORD	parentid=0;
	BYTE	parenthop=0;
	WORD	parentpath[CODI_MAX_ROUTING_PATH];
    int     i;

    // Binding이 삭제 안되고 Close 될 경우
    if (endi->bBinding) 
	    endi->bDirect ? CloseDirectConnection(endi) : Unbinding(endi);

	// Disconnect를 안하고 Close를 한 경우, Timeout을 대기하지 않는다.
	if (endi->bConnected)
		DisconnectEndDevice(endi);

	if (codiIsSuperParent(endi->codi))
	{
		// 부모가 바인딩 된 경우
		if (endi->bParentBinding)
		{
			// 부모가 존재하는 경우에만 적용된다.
			if (codiOnQuerySuperParent(&endi->id, &parent, &parentid, &parenthop, parentpath))
			{
				nError = ConnectParentDevice(endi->codi, &parent, parentid, parenthop, parentpath, 3000);
			    if (nError == CODIERR_NOERROR)
				{
					WORD	shortid = 0;
					BYTE	hops = 0;
					WORD	path[CODI_MAX_ROUTING_PATH];
                    BYTE     seq = 0;

#ifndef __NETWORK_NODE__
		            // Issue #1151 : Bind 후 Link를 전송할 때 까지 100ms의 delay를 둔다
                    USLEEP(100000);
                    nError = LinkEndDevice(endi->codi, &parent, seq, 3000); seq++;
                    if( nError == CODIERR_NOERROR) 
                    {
#endif
		    		    codiOnQueryRouteRecord(endi->codi, &endi->id, &shortid, &hops, path);
					    SetChildDevice(endi->codi, &parent, seq, ENDI_SPTYPE_DEL_REQUEST, shortid, &endi->id, 3000); seq++;
#ifndef __NETWORK_NODE__
                    }

                    for(i=0;i<2;i++)
                    {
	                    nError = LinkCloseEndDevice(endi->codi, &parent, seq, 1000);
                        if(nError == CODIERR_NOERROR || nError == CODIERR_PROGRESS) break;
                    }
#endif
					nError = DisconnectParentDevice(endi->codi, &parent, parentid, parenthop, parentpath);
				}
			}
		}
	}	 

	CEndDevicePool::DeleteEndDevice(endi);	
	return nError;
}

int CCoordinatorServer::ConnectParentDevice(COORDINATOR *codi, EUI64 *id, WORD shortid, BYTE hops, WORD *path, int nTimeout)
{
	CODI_COMMAND_FRAME	frame;
	CODI_BIND_PAYLOAD	payload;	
	int		nError=CODIERR_NOERROR, len;
	int		i, nSize;

    // Binding을 수행한다.
    memset(&payload, 0, sizeof(CODI_BIND_PAYLOAD));
    ReverseEUI64(id, &payload.id);
    payload.type	= CODI_BIND_SET_BIND;
    payload.shortid = BigToHostShort(shortid);
    payload.hops 	= hops;
    for(i=0; i<hops; i++)
	    payload.routingPath[i] = BigToHostShort(path[i]);		  

    nSize = sizeof(CODI_BIND_PAYLOAD) - (sizeof(WORD) * (CODI_MAX_ROUTING_PATH-hops));
    len = MakeCommandFrame(&frame, CODI_ACCESS_WRITE, CODI_CMD_BIND, (BYTE *)&payload, nSize);
    if (nTimeout == 0)
    {
	    SendToCoordinator(codi, (BYTE *)&frame, len);
	    return CODIERR_PROGRESS;
    }

	nError = WaitingForCommandReply(codi, CODI_ACCESS_WRITE, CODI_CMD_BIND, frame.hdr.seq,
						(BYTE *)&frame, len, NULL, NULL, nTimeout);

	return nError;
}

int	CCoordinatorServer::SetChildDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, WORD shortid, EUI64 *child, int nTimeout)
{
	ENDI_DATA_FRAME     frame;
	ENDI_SUPER_PARENT_PAYLOAD payload;
	int		len;

	memset(&payload, 0, sizeof(ENDI_SUPER_PARENT_PAYLOAD));
	payload.type = type;
	payload.child_shortid = BigToHostShort(shortid);
	ReverseEUI64(child, &payload.child_id);
	len = MakeDataFrameByID(id, &frame, CODI_ACCESS_WRITE, seq, ENDI_DATATYPE_SUPER_PARENT,
						(BYTE *)&payload, sizeof(ENDI_SUPER_PARENT_PAYLOAD));
	if (nTimeout == 0)
	{
		SendToCoordinator(codi, (BYTE *)&frame, len);
		return CODIERR_PROGRESS;
	}

	return WaitingForDataReply(codi, CODI_ACCESS_WRITE, ENDI_DATATYPE_SUPER_PARENT, frame.hdr.seq, WAIT_WRITE_RESPONSE | WAIT_READ_RESPONSE,
							(BYTE *)&frame, len, nTimeout);
}

int CCoordinatorServer::DisconnectParentDevice(COORDINATOR *codi, EUI64 *id, WORD shortid, BYTE hops, WORD *path)
{
	CODI_COMMAND_FRAME	frame;
	CODI_BIND_PAYLOAD	payload;	
	int		i, nRetry, len, nError, nSize;

	memset(&payload, 0, sizeof(CODI_BIND_PAYLOAD));
	ReverseEUI64(id, &payload.id);
	payload.type 	= CODI_BIND_DELETE_BIND;
	payload.shortid = BigToHostShort(shortid);
	payload.hops 	= hops;
	for(i=0; i<hops; i++)
		payload.routingPath[i] = BigToHostShort(path[i]);		  

	nSize = sizeof(CODI_BIND_PAYLOAD) - (sizeof(WORD) * (CODI_MAX_ROUTING_PATH-hops));
	len = MakeCommandFrame(&frame, CODI_ACCESS_WRITE, CODI_CMD_BIND,
						(BYTE *)&payload, nSize);

    // 지우기전에 1000ms wait
	USLEEP(1000000);

	for(nRetry=0;;)
	{ 
		nError = WaitingForCommandReply(codi, CODI_ACCESS_WRITE, CODI_CMD_BIND, frame.hdr.seq,
						(BYTE *)&frame, len, NULL, NULL, 2000);

		// 바인딩 삭제 오류 인경우만 재시도 한다.
		if ((nError != CODIERR_DELETE_BINDING_ERROR) && (nError != CODIERR_BINDING_IS_ACTIVE_ERROR))
			break;

		nRetry++;
		if (nRetry >= 10)
		{
			// 10회를 다시 시도하여도 바인딩을 삭제할 수 없는 경우
			if (nError ==CODIERR_DELETE_BINDING_ERROR)
		    {
				XDEBUG("\033[1;40;31m----------- DELETE BINDING ERROR -----------\033[0m\r\n");
				ResetCoordinator(codi);
			}
			break;
		}

		// 1000ms 이후에 재시도 한다.
		USLEEP(1000000);
	}
	return nError;
}

int CCoordinatorServer::ConnectEndDevice(ENDDEVICE *endi, WORD nShortID, BYTE nHops, WORD *pRoutePath, int nTimeout, 
            BOOL bExtendedTimeout, BOOL bDirect, BOOL bEndDevice)
{
	CODI_COMMAND_FRAME	frame;
	CODI_BIND_PAYLOAD	bindPayload;	
	CODI_DIRECTCOMM_PAYLOAD directPayload;	
	char	szGUID[17];
	WORD	shortid = 0;
	BYTE	hops = 0;
	WORD	path[CODI_MAX_ROUTING_PATH];
	int		nError=CODIERR_NOERROR, len;
	int		i, nSize;

	EUI64	parent;
	WORD	parentid=0;
	BYTE	parenthop=0;
	WORD	parentpath[CODI_MAX_ROUTING_PATH];

    if(!endi->bBinding)
	{
	    // Short ID가 지정되지 않은 경우
	    if (nShortID == 0)
	    {
		    // Route record를 구한다.
		    memset(path, 0, sizeof(WORD)*CODI_MAX_ROUTING_PATH);
		    codiOnQueryRouteRecord(endi->codi, &endi->id, &shortid, &hops, path);
	    }
	    else
	    {
		    // Short ID가 지정된 경우 해당 정보를 사용한다.
		    shortid	= nShortID;
		    hops	= nHops;
            if(hops > 0) {
		        memcpy(path, pRoutePath, sizeof(WORD)*hops);
            }
	    }

        if(hops > CODI_MAX_ROUTING_PATH) hops = CODI_MAX_ROUTING_PATH;

        endi->bDirect = bDirect;
        if(shortid) endi->nShortId = shortid;

        /** Parent Node가 존재한다고 모두 End Device 가 되는건 아니다.
         *  따라서 End Device 여부를 검사할 수 있게 해야 한다.
         */
		if (codiIsSuperParent(endi->codi) && bEndDevice )
		{
            BYTE seq=0;

			// 부모가 존재하는 경우에만 적용된다.
			if (!codiOnQuerySuperParent(&endi->id, &parent, &parentid, &parenthop, parentpath))
				return CODIERR_INVALID_PARENT;

			nError = ConnectParentDevice(endi->codi, &parent, parentid, parenthop, parentpath, 3000);
		    if (nError != CODIERR_NOERROR)
				return CODIERR_PARENT_BINDING_ERROR;
					
#ifndef __NETWORK_NODE__
		    // Issue #1151 : Bind 후 Link를 전송할 때 까지 100ms의 delay를 둔다
            USLEEP(100000);
            nError = LinkEndDevice(endi->codi, &parent, seq, 3000); seq++;
            if( nError != CODIERR_NOERROR) 
            {
				DisconnectParentDevice(endi->codi, &parent, parentid, parenthop, parentpath);
				return nError;
            }
#endif

			nError = SetChildDevice(endi->codi, &parent, seq, ENDI_SPTYPE_SET_REQUEST, shortid, &endi->id, 3000); seq++;
			if (nError != CODIERR_NOERROR)
			{
				DisconnectParentDevice(endi->codi, &parent, parentid, parenthop, parentpath);
				return CODIERR_SET_CHILD_ERROR;
			}

#ifndef __NETWORK_NODE__
	        // Link Close를 전달한다.
            /** Error가 나도 Disconnect 절차를 지켜야 한다 */
            for(i=0;i<2;i++)
            {
	            nError = LinkCloseEndDevice(endi->codi, &parent, seq, 1000);
                if(nError == CODIERR_NOERROR || nError == CODIERR_PROGRESS) break;
            }
#endif

			DisconnectParentDevice(endi->codi, &parent, parentid, parenthop, parentpath);
			endi->bParentBinding = TRUE;

			// 접속 이전에 대기한다.	
        	USLEEP(100000);
		}	 

	    // Binding을 수행한다.
        if(!bDirect) {
	        memset(&bindPayload, 0, sizeof(CODI_BIND_PAYLOAD));
	        ReverseEUI64(&endi->id, &bindPayload.id);
	        bindPayload.type	= CODI_BIND_SET_BIND;
	        bindPayload.shortid = BigToHostShort(shortid);
	        bindPayload.hops 	= hops;
	        for(i=0; i<hops; i++)
		        bindPayload.routingPath[i] = BigToHostShort(path[i]);		  

	        nSize = sizeof(CODI_BIND_PAYLOAD) - (sizeof(WORD) * (CODI_MAX_ROUTING_PATH-hops));
	        len = MakeCommandFrame(&frame, CODI_ACCESS_WRITE, CODI_CMD_BIND, (BYTE *)&bindPayload, nSize);
	        if (nTimeout == 0)
	        {
		        SendToCoordinator(endi->codi, (BYTE *)&frame, len);
		        return CODIERR_PROGRESS;
	        }
    
	        nError = WaitingForCommandReply(endi->codi, CODI_ACCESS_WRITE, CODI_CMD_BIND, frame.hdr.seq,
						(BYTE *)&frame, len, NULL, NULL, nTimeout);
        }else {
	        memset(&directPayload, 0, sizeof(CODI_DIRECTCOMM_PAYLOAD));
	        ReverseEUI64(&endi->id, &directPayload.id);
	        directPayload.type	= CODI_BIND_SET_BIND;
	        directPayload.shortid = BigToHostShort(shortid);	  

            nSize = sizeof(CODI_DIRECTCOMM_PAYLOAD);
	        len = MakeCommandFrame(&frame, CODI_ACCESS_WRITE, CODI_CMD_DIRECT_COMM, (BYTE *)&directPayload, nSize);
	        if (nTimeout == 0)
	        {
		        SendToCoordinator(endi->codi, (BYTE *)&frame, len);
		        return CODIERR_PROGRESS;
	        }
    
	        nError = WaitingForCommandReply(endi->codi, CODI_ACCESS_WRITE, CODI_CMD_DIRECT_COMM, frame.hdr.seq,
						(BYTE *)&frame, len, NULL, NULL, nTimeout);
        }

	    if ((nError != CODIERR_NOERROR) && (nError != CODIERR_PROGRESS)) {
            endi->nLastError = nError;
            return nError;
        }

        // Binding 정보를 표시
        endi->bBinding = TRUE;

		EUI64ToStr(&endi->id, szGUID);

#ifndef __NETWORK_NODE__
		XDEBUG("EXTENDED TIMEOUT SET TRY(%s:%s)\r\n", szGUID, bExtendedTimeout ? "T" : "F");
        nError = SetExtendedTimeout(endi, bExtendedTimeout);
        if(nError != CODIERR_NOERROR) {
			XDEBUG(" EXTENDED TIMEOUT SET FAIL(%s:%s) = %s(%d)\r\n", 
                szGUID, bExtendedTimeout ? "T" : "F", codiErrorMessage(nError), nError);
            if(nError != CODIERR_NOT_SUPPORTED) {
                /** Issue #1622: Coordinator v2.1 b11 이전일 경우 Not Support Error가 발생한다.
                  *     Not Support를 에러 처리할 경우 이전 버전은 통신이 되지 않는다.
                  *     따라서 Not Support는 무시한다.
                  */
                endi->nLastError = nError;
                return nError;
            }
        }
#endif
    }

	if ((nError == CODIERR_NOERROR) || (nError == CODIERR_PROGRESS))
	{
		// Link를 재전송 할 수 있으므로 SEQUENCE를 0으로 환원한다.
		endi->seq = 0;
		endi->nLastRecvSeq = -1;
		endi->nSeqError = 0;

#ifndef __NETWORK_NODE__
		// Issue #1151 : Bind 후 Link를 전송할 때 까지 100ms의 delay를 둔다
        USLEEP(100000);
        nError = LinkEndDevice(endi->codi, &endi->id, GetEndDeviceSequence(endi), nTimeout);

		if (nError == CODIERR_NOERROR || nError == CODIERR_PROGRESS)
		{
#endif
			// 정상 접속된 경우
			ceTime(&endi->tmLastConnect);
			endi->bConnected = TRUE;
			endiOnConnect((HANDLE)endi->codi, (HANDLE)endi);
#ifndef __NETWORK_NODE__
		}
#endif
	}

    endi->nLastError = nError;
	return nError;
}

int CCoordinatorServer::ResetCoordinator(COORDINATOR *codi)
{
	CODI_RESET_PAYLOAD	payload;
	int		i, nError;

	// 1. Software 리셋을 성공할 때까지 3회 재시도한다.
	XDEBUG(" \033[1;40;31m -- Software Coordinator reset. --\033[0m\r\n");
	for(i=0; i<3; i++)
	{
		payload.reset = CODI_RESET_SET;
		nError = SetCoordinatorProperty(codi, CODI_CMD_RESET, (BYTE *)&payload, 
							sizeof(CODI_RESET_PAYLOAD), 3000);
		if (nError == CODIERR_NOERROR)
			return CODIERR_NOERROR;

		USLEEP(100000);
	}

	// 2. Hardware 리셋을 수행한다.
	XDEBUG(" \033[1;40;31m -- Hardware Coordinator reset. --\033[0m\r\n");
	codiOnNoResponse((HANDLE)codi, TRUE);	
	return CODIERR_NOERROR;
}

int	CCoordinatorServer::BootloadCoordinator(COORDINATOR *codi, int nType)
{
	CODI_BOOTLOAD_PAYLOAD	payload;
	CDeviceClass   	*pClass;

	if (nType == CODI_BOOTLOAD_RUN)
	{
		// Passive mode와 Bootloader 모드를 해제한다. 
		codi->bPassive = FALSE;
		codi->bBootloader = FALSE;
		SetCoordinatorPassive(codi, FALSE);

		// Coordinator를 부트 모드에서 시작한다.
		SendToCoordinator(codi, (BYTE *)const_cast<char *>("2\r"), 2);
		USLEEP(2000000);
		
		// Flow control을 다시 켠다.
		if (codi->device.nOptions & CODI_OPT_RTSCTS)
		{
	    	pClass = (CDeviceClass *)codi->pInterface;
			pClass->EnableFlowControl(codi, TRUE);
		}
		return CODIERR_NOERROR;
	}

	payload.type = CODI_BOOTLOAD_SET;
	payload.key[0] = 0x57;
	payload.key[1] = 0x48;

	SetCoordinatorProperty(codi, CODI_CMD_BOOTLOAD, (BYTE *)&payload,
                            sizeof(CODI_BOOTLOAD_PAYLOAD), 3000);

	// 이미 부트로더에 있는 경우를 위하여 부트로더로 전환이 실패해도 무시한다.
	//if (nError != CODIERR_NOERROR)
	//	return nError;

	// Setup passive mode
	codi->bPassive = TRUE;
	codi->bBootloader = TRUE;
	SetCoordinatorPassive(codi, TRUE);

	if (codi->device.nOptions & CODI_OPT_RTSCTS)
	{
	    pClass = (CDeviceClass *)codi->pInterface;
		pClass->EnableFlowControl(codi, FALSE);
	}

	USLEEP(2000000);
	return CODIERR_NOERROR;
}

int	CCoordinatorServer::DownloadCoordinator(COORDINATOR *codi, int nType, const char *pszFileName, const char *pszExt)
{
	CDeviceClass	*pClass;

	int				nError;
#ifndef _WIN32
	char	tmp[256];
    int    	fd, pid, child, status;
#endif

	pClass = (CDeviceClass *)codi->pInterface;
	if (pClass == NULL)
		return CODIERR_INVALID_HANDLE;

	nError = CCoordinatorServer::BootloadCoordinator(codi, CODI_BOOTLOAD_BOOTLOADER);

#ifdef _WIN32
	
#else
	fd = pClass->GetFD(codi);

    if(nError == CODIERR_NOERROR) {
	    read(fd, tmp, sizeof(tmp));
    }
    write(fd, "1", 1);
	usleep(1000000);
	read(fd, tmp, 32);

    pid = Spawn(child, 1, "/app/sw");
    if (pid > 0)
    {
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);

		// STDERR은 dup하지 않는다.
		// 결과가 출력되므로
		//dup2(fd, STDERR_FILENO);

		switch(nType) {
		  case CODI_MODEMTYPE_XMODEM :
#if defined(__TI_AM335X__)
        	   execl("/usr/bin/lsz", "lsz", "-b", "-X", pszFileName, NULL);
#else
        	   execl("/usr/bin/sz", "sz", "-b", "-X", pszFileName, NULL);
#endif
			   break;
		  case CODI_MODEMTYPE_YMODEM :
#if defined(__TI_AM335X__)
        	   execl("/usr/bin/lsz", "lsz", "-b", "-Y", pszFileName, NULL);
#else
        	   execl("/usr/bin/sz", "sz", "-b", "-Y", pszFileName, NULL);
#endif
			   break;
		  case CODI_MODEMTYPE_ZMODEM :
#if defined(__TI_AM335X__)
        	   execl("/usr/bin/lsz", "lsz", "-b", "-Z", pszFileName, NULL);
#else
        	   execl("/usr/bin/sz", "sz", "-b", "-Z", pszFileName, NULL);
#endif
			   break;
		}
        exit(0);
    }

    while(waitpid(child, &status, WNOHANG) == 0)
        usleep(1000000);
#endif

	USLEEP(1000000);
	CCoordinatorServer::BootloadCoordinator(codi, CODI_BOOTLOAD_RUN);
	return CODIERR_NOERROR;
}

int	CCoordinatorServer::PlaybackCoordinator(COORDINATOR *codi, BYTE *frame, int nLength)
{
	
	AddStreamQueue2(codi, frame, nLength);
	return CODIERR_NOERROR;
}

int CCoordinatorServer::Unbinding(ENDDEVICE *endi)
{
	CODI_COMMAND_FRAME	frame;
	CODI_BIND_PAYLOAD	payload;	
	WORD	shortid = 0;
	BYTE	hops = 0;
	WORD	path[CODI_MAX_ROUTING_PATH];
	int		i, nRetry, len, nError, nSize;

	memset(&payload, 0, sizeof(CODI_BIND_PAYLOAD));
	payload.type = CODI_BIND_DELETE_BIND;
	ReverseEUI64(&endi->id, &payload.id);

	codiOnQueryRouteRecord(endi->codi, &endi->id, &shortid, &hops, path);
	payload.shortid = BigToHostShort(shortid);
	payload.hops 	= hops;
	for(i=0; i<hops; i++)
		payload.routingPath[i] = BigToHostShort(path[i]);		  

	nSize = sizeof(CODI_BIND_PAYLOAD) - (sizeof(WORD) * (CODI_MAX_ROUTING_PATH-hops));
	len = MakeCommandFrame(&frame, CODI_ACCESS_WRITE, CODI_CMD_BIND,
						(BYTE *)&payload, nSize);

    // 지우기전에 100ms wait
	USLEEP(100000);

    /*-- 지동민씨의 요청으로 retry 간격을 2초로 조정한다.
     *-- 대신 retry 횟수를 7회로 줄여서 전체 시간을 조정한다.
     --*/
	for(nRetry=0;;)
	{ 
		nError = WaitingForCommandReply(endi->codi, CODI_ACCESS_WRITE, CODI_CMD_BIND, frame.hdr.seq,
						(BYTE *)&frame, len, NULL, NULL, 2000);

		// 바인딩 삭제 오류 인경우만 재시도 한다.
		if ((nError != CODIERR_DELETE_BINDING_ERROR) && (nError != CODIERR_BINDING_IS_ACTIVE_ERROR))
			break;

		nRetry++;
		if (nRetry >= 10)
		{
			// 10회를 다시 시도하여도 바인딩을 삭제할 수 없는 경우
			if (nError ==CODIERR_DELETE_BINDING_ERROR)
		    {
				XDEBUG("\033[1;40;31m----------- DELETE BINDING ERROR -----------\033[0m\r\n");
				ResetCoordinator(endi->codi);
			}
			break;
		}

		// 1000ms 이후에 재시도 한다.
		USLEEP(1000000);
	}

    endi->bBinding = FALSE;

    endi->nLastError = nError;
	return nError;
}

int CCoordinatorServer::CloseDirectConnection(ENDDEVICE *endi)
{
	CODI_COMMAND_FRAME	frame;
	CODI_DIRECTCOMM_PAYLOAD directPayload;	
	WORD	shortid = 0;
	int		nError=CODIERR_NOERROR, len;
	int		nSize, nRetry;
		
    if(endi->bBinding && endi->bDirect) {
        shortid = endi->nShortId;

	    memset(&directPayload, 0, sizeof(CODI_DIRECTCOMM_PAYLOAD));
	    ReverseEUI64(&endi->id, &directPayload.id);
	    directPayload.type	= CODI_BIND_DELETE_BIND;
	    directPayload.shortid = BigToHostShort(shortid);

	    nSize = sizeof(CODI_DIRECTCOMM_PAYLOAD);
	    len = MakeCommandFrame(&frame, CODI_ACCESS_WRITE, CODI_CMD_DIRECT_COMM, (BYTE *)&directPayload, nSize);
    
	    for(nRetry=0;;) {
	        nError = WaitingForCommandReply(endi->codi, CODI_ACCESS_WRITE, CODI_CMD_DIRECT_COMM, frame.hdr.seq,
		        (BYTE *)&frame, len, NULL, NULL, 2000);

	        if (nError == CODIERR_NOERROR) break;

		    nRetry++;
		    if (nRetry >= 10)
		    {
				XDEBUG("\033[1;40;31m----------- DELETE DIRECT CONNECTION ERROR -----------\033[0m\r\n");
				ResetCoordinator(endi->codi);
			    break;
		    }
    
		    // 1000ms 이후에 재시도 한다.
		    USLEEP(1000000);
        }

        // Binding 정보를 표시
        endi->bBinding = FALSE;
    }

	return nError;
}


int CCoordinatorServer::DisconnectEndDevice(ENDDEVICE *endi)
{
	int		nError = CODIERR_NOERROR;
    int     i;
	
#ifndef __NETWORK_NODE__
	// Link Close를 전달한다.
    if(endi->bBinding && endi->bConnected) {
        for(i=0;i<2;i++)
        {
	        nError = LinkCloseEndDevice(endi, 1000);
            if(nError == CODIERR_NOERROR || nError == CODIERR_PROGRESS) break;
        }
    }
#endif

	// Binding을 삭제한다.
    if(endi->bBinding)
	    nError = endi->bDirect ? CloseDirectConnection(endi) : Unbinding(endi);

	endiOnDisconnect((HANDLE)endi->codi, (HANDLE)endi);
	endi->bConnected = FALSE;
    endi->nLastError = nError;
	return nError;
}

int	CCoordinatorServer::LinkEndDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, int nTimeout)
{
    BYTE                type;
	
	type = ENDI_LINKOPEN_REQUEST;
#ifdef WINCE
	type |= ENDI_LINKSRC_PDA;
#endif
    return LinkEndDevice(codi, id, seq, type, nTimeout);
}

int	CCoordinatorServer::LinkEndDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, int nTimeout)
{
	ENDI_DATA_FRAME     frame;
	ENDI_LINK_PAYLOAD   payload;
	int                 len;
	
    /** Issue #2567 Link 절차를 사용하지 않을 경우 */
    if(!(m_nCoordinatorOption & CODI_OPTION_LINK))
    {
        return CODIERR_NOERROR;
    }

	memset(&payload, 0, sizeof(ENDI_LINK_PAYLOAD));
	payload.type = type;
	len = MakeDataFrameByID(id, &frame, CODI_ACCESS_WRITE, seq, ENDI_DATATYPE_LINK,
						(BYTE *)&payload, sizeof(ENDI_LINK_PAYLOAD));
	if (nTimeout == 0)
	{
		SendToCoordinator(codi, (BYTE *)&frame, len);
		return CODIERR_PROGRESS;
	}

	return WaitingForDataReply(codi, CODI_ACCESS_WRITE, ENDI_DATATYPE_LINK, frame.hdr.seq, WAIT_WRITE_RESPONSE | WAIT_READ_RESPONSE,
							(BYTE *)&frame, len, nTimeout);
}

int CCoordinatorServer::LinkCloseEndDevice(ENDDEVICE *endi, int nTimeout)
{
    if(endi->nLastError == CODIERR_DELIVERY_ERROR) {
        endi->nLastError = CODIERR_NOERROR;
        return CODIERR_NOERROR;
    }
	
    endi->nLastError = LinkCloseEndDevice(endi->codi, &endi->id, GetEndDeviceSequence(endi), nTimeout);

    return endi->nLastError;
}

int CCoordinatorServer::LinkCloseEndDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, int nTimeout)
{
	ENDI_DATA_FRAME		frame;
	ENDI_LINK_PAYLOAD	payload;
	int					len;
	
    /** Issue #2567 Link 절차를 사용하지 않을 경우 */
    if(!(m_nCoordinatorOption & CODI_OPTION_LINK))
    {
        return CODIERR_NOERROR;
    }

	memset(&payload, 0, sizeof(ENDI_LINK_PAYLOAD));
	payload.type = ENDI_LINKCLOSE_REQUEST;
#ifdef WINCE
	payload.type |= ENDI_LINKSRC_PDA;
#endif
	len = MakeDataFrameByID(id, &frame, CODI_ACCESS_WRITE, seq, ENDI_DATATYPE_LINK,
						(BYTE *)&payload, sizeof(ENDI_LINK_PAYLOAD));
	if (nTimeout == 0)
	{
		SendToCoordinator(codi, (BYTE *)&frame, len);
		return CODIERR_PROGRESS;
	}

	return WaitingForDataReply(codi, CODI_ACCESS_WRITE, ENDI_DATATYPE_LINK, frame.hdr.seq, WAIT_WRITE_RESPONSE,
						(BYTE *)&frame, len, nTimeout);
}


int CCoordinatorServer::SendToEndDevice(ENDDEVICE *endi, BYTE rw, BYTE type, BYTE *payload, int nLength, int nTimeout)
{
	ENDI_DATA_FRAME		frame;
	int					len;

	len = MakeDataFrame(endi, &frame, rw, type, payload, nLength);
	
	endi->nTotalSend += len;
	if (type == ENDI_DATATYPE_BYPASS)
		endi->nTotalDataSend += len;
    else if (type == ENDI_DATATYPE_NAN)
		endi->nTotalNanSend += len;

	if (nTimeout == 0)
	{
		SendToCoordinator(endi->codi, (BYTE *)&frame, len);
		return CODIERR_PROGRESS;
	}

	endi->nLastError = WaitingForDataReply(endi->codi, rw, type, frame.hdr.seq, WAIT_WRITE_RESPONSE | WAIT_READ_RESPONSE,
						(BYTE *)&frame, len, nTimeout);
    return endi->nLastError;
}

int CCoordinatorServer::SendRecvToEndDevice(ENDDEVICE *endi, BYTE rw, BYTE type, BYTE *payload, int nLength, BYTE *data, int *datalen, int nTimeout)
{
	CODIWAITREPLY	*pReply;
	EUI64			id;
	int				nError;
		
	*datalen = 0;
	ReverseEUI64(&endi->id, &id);
	pReply = AddWaitReply(endi->codi, CODI_ACCESS_READ, CODI_FRAMETYPE_DATA, type, 0, WAIT_WRITE_RESPONSE | WAIT_READ_RESPONSE, &id, data);
	if (pReply == NULL)
		return CODIERR_MEMORY_ERROR;

	// Send Data Frame
	nError = SendToEndDevice(endi, rw, type, payload, nLength, (type != ENDI_DATATYPE_LINK) ? nTimeout : 0);
	if ((nError != CODIERR_NOERROR) && (nError != CODIERR_PROGRESS))
	{
		DeleteWaitReply(pReply);
        endi->nLastError = nError;
		return nError;
	}

	nError = WaitForReply(pReply, nTimeout);
	if ((nError == CODIERR_NOERROR) || pReply->replied)
	{
        if (pReply->result != CODI_SUCCESS)
            nError = GetCommandError(pReply->result);
        else
		    nError = CODIERR_NOERROR;
		if (datalen != NULL)
			*datalen = pReply->nLength;
	}

	DeleteWaitReply(pReply);
    endi->nLastError = nError;
	return nError;
}

int CCoordinatorServer::GetOptionEndDevice(ENDDEVICE *endi, int nOption, int *nValue)
{
		
	switch(nOption) {
	  case ENDIOPT_SEQUENCE :
		   *nValue = ((endi->nOptions & ENDIOPT_SEQUENCE) > 0) ? 1 : 0;
		   return CODIERR_NOERROR;

	  case ENDIOPT_TIMEOUT :
		   *nValue = endi->nTimeout;
		   return CODIERR_NOERROR;
	}
	return CODIERR_INVALID_OPTION;
}

int CCoordinatorServer::SetOptionEndDevice(ENDDEVICE *endi, int nOption, int nValue)
{
			
	switch(nOption) {
	  case ENDIOPT_SEQUENCE :
		   if (nValue == 1)
			    endi->nOptions |= ENDIOPT_SEQUENCE; 
		   else endi->nOptions &= (~ENDIOPT_SEQUENCE);
		   return CODIERR_NOERROR;

	  case ENDIOPT_TIMEOUT :
		   endi->nTimeout = nValue;
		   return CODIERR_NOERROR;
	}
	return CODIERR_INVALID_OPTION;
}

////////////////////////////////////////////////////////////////////
// End Device Utility functions
//////////////////////////////////////////////////////////////////////

int CCoordinatorServer::BypassEndDevice(ENDDEVICE *endi, BYTE *payload, int nLength, int nTimeout)
{
					
	return SendToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_BYPASS, payload, nLength, nTimeout);
}

int CCoordinatorServer::BypassEndDevice(COORDINATOR *codi, BYTE *payload, int nLength)
{
					
	return MulticastToEndDevice(codi, ENDI_DATATYPE_BYPASS, payload, nLength);
}


int CCoordinatorServer::LeaveEndDevice(ENDDEVICE *endi, BYTE joinTime, BYTE channel, WORD panid, int nTimeout)
{
	ENDI_LEAVE_PAYLOAD	*payload;
	char	szBuffer[100];
	char	szReply[100];
	int		nError, len=0;
						
	payload = (ENDI_LEAVE_PAYLOAD *)szBuffer;
	payload->type = ENDI_LEAVE_NETWORK_REQUEST;
	payload->join = joinTime;
#if     defined(__SUPPORT_ADVANCED_LEAVE__)
    payload->channel = channel;
    payload->panid = panid;
#endif

	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_NETWORK_LEAVE,
				(BYTE *)payload, sizeof(ENDI_LEAVE_PAYLOAD),
				(BYTE *)szReply, &len, nTimeout);
	if (nError == CODIERR_NOERROR)
	{
		XDEBUG(" Leave Write Reply = %d\r\n", len);
		XDUMP((char *)szReply, len, TRUE);
	}
	return nError;
}

int CCoordinatorServer::ReadAmrEndDevice(ENDDEVICE *endi, BYTE *data, int *nLength, int nTimeout)
{
	ENDI_AMRREAD_PAYLOAD	amrread;
	ENDI_AMR_PAYLOAD		*payload;
	char	szBuffer[100];
	int		nError, len;
						
	*nLength = len = 0;
	amrread.type = ENDI_AMR_READ_REQUEST;

	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_AMR,
						(BYTE *)&amrread, sizeof(ENDI_AMRREAD_PAYLOAD),
						(BYTE *)szBuffer, &len, nTimeout);
	if (nError == CODIERR_NOERROR)
	{
		// 앞에 해더를 제외한 데이터만 전송한다.
		payload = (ENDI_AMR_PAYLOAD *)szBuffer;
		if ((len-3) > 0)
		{
			*nLength = len - 3;
			memcpy(data, &payload->data, len-3);
		}	
	}
	return nError;
}

int CCoordinatorServer::WriteAmrEndDevice(ENDDEVICE *endi, WORD mask, BYTE *data, int nLength, int nTimeout)
{
	ENDI_AMR_PAYLOAD	*payload;
	char	szBuffer[100];
	char	szReply[100];
	int		nError, len;
						
	payload = (ENDI_AMR_PAYLOAD *)szBuffer;
	payload->type = ENDI_AMR_WRITE_REQUEST;
	payload->mask = BigToHostShort(mask);
	memcpy(&payload->data, data, nLength);

	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_AMR,
				(BYTE *)payload, nLength+3,
				(BYTE *)szReply, &len, nTimeout);
	if (nError == CODIERR_NOERROR)
	{
		payload = (ENDI_AMR_PAYLOAD *)szReply;
		if ((len-3) > 0)
		{
			XDEBUG(" AMR Write Reply = %d\r\n", len-3);
			XDUMP((char *)&payload->data, len-3, TRUE);
		}
	}
	return nError;
}

int CCoordinatorServer::ReadRomEndDevice(ENDDEVICE *endi, WORD addr, int nLength, BYTE *data, int *nSize, int nTimeout)
{
	ENDI_ROMREAD_PAYLOAD	romread;
	ENDI_ROM_PAYLOAD		*payload;
	char	szBuffer[100];
	int		nError, len;
							
	*nSize = len = 0;
	romread.typelen = ENDI_ROM_READ_REQUEST | (BYTE)nLength;
	romread.addr    = BigToHostShort(addr);

	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_ROM_READ,
						(BYTE *)&romread, sizeof(ENDI_ROMREAD_PAYLOAD),
						(BYTE *)szBuffer, &len, nTimeout);
	if (nError == CODIERR_NOERROR)
	{
		// 읽은 길이가 요청한 길이와 다를때 오류를 발생한다.
		if ((len-3) != nLength)
		{
			XDEBUG(" \033[1;40;31mERROR: Read request size=%d, ROM read size=%d\033[0m\r\n", nLength, (len-3));
			return CODIERR_INVALID_LENGTH;
		}

		// 앞에 해더를 제외한 데이터만 전송한다.
		payload = (ENDI_ROM_PAYLOAD *)szBuffer;
		if ((len-3) > 0)
		{
			// 길이가 0보다 크면 복사한다.
            /*
			XDEBUG(" ROM Read Reply = %d\r\n", len-3);
			XDUMP((char *)&payload->data, len-3, TRUE);
            */

			*nSize = len - 3;
			memcpy(data, &payload->data, len-3);
		}	
	}
	return nError;
}

int CCoordinatorServer::WriteRomEndDevice(ENDDEVICE *endi, WORD addr, BYTE *data, int nLength, int nTimeout)
{
	ENDI_ROM_PAYLOAD	*payload;
	char	szBuffer[100];
	char	szReply[100];
	int		nError, len;
					
	payload = (ENDI_ROM_PAYLOAD *)szBuffer;
	payload->typelen = ENDI_ROM_WRITE_REQUEST | (BYTE)nLength;
	payload->addr 	 = BigToHostShort(addr);
	memcpy(&payload->data, data, nLength);

	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_ROM_READ,
				(BYTE *)payload, nLength+3,
				(BYTE *)szReply, &len, nTimeout);

	if (nError == CODIERR_NOERROR)
	{
		payload = (ENDI_ROM_PAYLOAD *)szReply;
        /*
		if ((len-3) > 0)
		{
		    XDEBUG(" ROM Write Reply = %d\r\n", len-3);
			XDUMP((char *)&payload->data, len-3, TRUE);
		}
        */
	}
	return nError;
}

int CCoordinatorServer::WriteRomEndDevice(COORDINATOR *codi, WORD addr, BYTE *data, int nLength)
{
	ENDI_ROM_PAYLOAD	*payload;
	char	szBuffer[100];
						
	payload = (ENDI_ROM_PAYLOAD *)szBuffer;
	payload->typelen = ENDI_ROM_WRITE_REQUEST | (BYTE)nLength;
	payload->addr 	 = BigToHostShort(addr);
	memcpy(&payload->data, data, nLength);

    return MulticastToEndDevice(codi, ENDI_DATATYPE_ROM_READ, (BYTE *)payload, nLength+3);
}

int CCoordinatorServer::ReadLongRomEndDevice(ENDDEVICE *endi, UINT addr, int nLength, BYTE *data, int *nSize, int nTimeout)
{
	ENDI_LONG_ROMREAD_PAYLOAD	romread;
	ENDI_LONG_ROM_PAYLOAD		*payload;
	char	szBuffer[100];
	int		nError, len;

	*nSize = len = 0;
	romread.type = ENDI_ROM_READ_REQUEST;
	romread.len  = (BYTE)nLength;
	romread.addr = BigToHostInt(addr);

	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_LONG_ROM_READ,
						(BYTE *)&romread, sizeof(ENDI_LONG_ROMREAD_PAYLOAD),
						(BYTE *)szBuffer, &len, nTimeout);
	if (nError == CODIERR_NOERROR)
	{
		// 읽은 길이가 요청한 길이와 다를때 오류를 발생한다.
		if ((len-6) != nLength)
		{
			XDEBUG(" \033[1;40;31mERROR: Read request size=%d, ROM read size=%d\033[0m\r\n", nLength, (len-6));
			return CODIERR_INVALID_LENGTH;
		}

		// 앞에 해더를 제외한 데이터만 전송한다.
		payload = (ENDI_LONG_ROM_PAYLOAD *)szBuffer;
		if ((len-6) > 0)
		{
			// 길이가 0보다 크면 복사한다.
            /*
			XDEBUG(" ROM Read Reply = %d\r\n", len-6);
			XDUMP((char *)&payload->data, len-6, TRUE);
            */

			*nSize = len - 6;
			memcpy(data, &payload->data, len-6);
		}	
	}
	return nError;
}

int CCoordinatorServer::WriteLongRomEndDevice(ENDDEVICE *endi, UINT addr, BYTE *data, int nLength, BYTE * replay, int *nSize, int nTimeout)
{
	ENDI_LONG_ROM_PAYLOAD	*payload;
	char	szBuffer[100];
	char	szReply[100];
	int		nError, len;

	payload = (ENDI_LONG_ROM_PAYLOAD *)szBuffer;
	payload->type = ENDI_ROM_WRITE_REQUEST;
	payload->len  = (BYTE)nLength;
	payload->addr = BigToHostInt(addr);
	memcpy(&payload->data, data, nLength);
	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_LONG_ROM_READ,
				(BYTE *)payload, nLength+6,
				(BYTE *)szReply, &len, nTimeout);

	if (nError == CODIERR_NOERROR)
	{
		payload = (ENDI_LONG_ROM_PAYLOAD *)szReply;
		if ((len-6) > 0)
		{
        
	//		XDEBUG(" ROM Write Reply = %d\r\n", len-6);
	//		XDUMP((char *)&payload->data, len-6, TRUE);
        
            *nSize = len - 6;
            memcpy(replay, &payload->data, len-6);
		}
	}

    return nError;
}

int CCoordinatorServer::WriteLongRomEndDevice(COORDINATOR *codi, UINT addr, BYTE *data, int nLength)
{
	ENDI_LONG_ROM_PAYLOAD	*payload;
	char	szBuffer[100];

	payload = (ENDI_LONG_ROM_PAYLOAD *)szBuffer;
	payload->type = ENDI_ROM_WRITE_REQUEST;
	payload->len  = (BYTE)nLength;
	payload->addr = BigToHostInt(addr);
	memcpy(&payload->data, data, nLength);

    return MulticastToEndDevice(codi, ENDI_DATATYPE_LONG_ROM_READ, (BYTE *)payload, nLength+6);
}

int CCoordinatorServer::WriteCidEndDevice(ENDDEVICE *endi, BYTE *value, int valuelen, BYTE *data, int *datalen, int nTimeout)
{
    
	int		nError;

	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_BYPASS,
				(BYTE *)value, valuelen,
				(BYTE *)data, datalen, nTimeout);
            XDEBUG("LEN : %d\xd\xa", *datalen);
	return nError;
}

int CCoordinatorServer::WriteMccbEndDevice(ENDDEVICE *endi, BYTE value, BYTE *data, int nTimeout)
{
	ENDI_MCCB_PAYLOAD	*payload;
	char	szBuffer[100];
	BYTE	szReply[100];
	int		nError, len;

	payload = (ENDI_MCCB_PAYLOAD *)szBuffer;
	payload->typevalue = value;

	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_MCCB,
				(BYTE *)payload, sizeof(ENDI_MCCB_PAYLOAD),
				(BYTE *)szReply, &len, nTimeout);

	if (nError == CODIERR_NOERROR)
	{
		if (len > 0)
		{
			szReply[0] = (szReply[0] & 0x7F);
			XDEBUG(" MCCB Write Reply = %d\r\n", len);
			XDUMP((char *)szReply, len, TRUE);
			memcpy(data, szReply, 1);
		}
	}
	return nError;
}

int CCoordinatorServer::CommandEndDevice(ENDDEVICE *endi, BYTE cmd, BYTE type, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout)
{
	ENDI_COMMAND_PAYLOAD	*payload;
	char	szBuffer[100];
	char	szReply[100];
	int		nError, len;

    *nSize = 0;
	payload = (ENDI_COMMAND_PAYLOAD *)szBuffer;
	payload->cmd_type = cmd;
    payload->frame_type = type;
	memcpy(&payload->data, data, nLength);

	nError = SendRecvToEndDevice(endi, CODI_ACCESS_WRITE, ENDI_DATATYPE_COMMAND,
				(BYTE *)payload, nLength+2,
				(BYTE *)szReply, &len, nTimeout);
	if (nError == CODIERR_NOERROR)
	{
		payload = (ENDI_COMMAND_PAYLOAD *)szReply;
		if ((len-2) > 0)
		{
            *nSize = len-2;
            memcpy(result, &payload->data, *nSize);
			XDEBUG(" COMMAND Reply = %d\r\n", *nSize);
			XDUMP((char *)result, *nSize, TRUE);
		}
	}
	return nError;
}

int	CCoordinatorServer::BindEndDevice(COORDINATOR *codi, EUI64 *id, BYTE type, int nTimeout, BOOL bExtendedTimeout)
{
    CODI_COMMAND_FRAME  frame;
    CODI_BIND_PAYLOAD   payload;
	WORD	shortid = 0;
	BYTE	hops = 0;
	WORD	path[CODI_MAX_ROUTING_PATH];
    int     i, nError, len, nSize;

	memset(path, 0, sizeof(WORD)*CODI_MAX_ROUTING_PATH);
	codiOnQueryRouteRecord(codi, id, &shortid, &hops, path);

	memset(&payload, 0, sizeof(CODI_BIND_PAYLOAD));
	ReverseEUI64(id, &payload.id);
	payload.type 	= type;
	payload.shortid = BigToHostShort(shortid);
	payload.hops 	= hops;
	for(i=0; i<hops; i++)
		payload.routingPath[i] = BigToHostShort(path[i]);		  
 
	nSize = sizeof(CODI_BIND_PAYLOAD) - (sizeof(WORD) * (CODI_MAX_ROUTING_PATH-hops));
	len = MakeCommandFrame(&frame, CODI_ACCESS_WRITE, CODI_CMD_BIND,
						(BYTE *)&payload, nSize);
	if (nTimeout == 0)
	{
		SendToCoordinator(codi, (BYTE *)&frame, len);
		return CODIERR_PROGRESS;
	}

    nError = WaitingForCommandReply(codi, CODI_ACCESS_WRITE, CODI_CMD_BIND, frame.hdr.seq,
                        (BYTE *)&frame, len, NULL, NULL, nTimeout);
	return nError;
}

int	CCoordinatorServer::WriteEndDevice(COORDINATOR *codi, EUI64 *id, BYTE seq, BYTE type, BYTE *data, int nLength, int nTimeout)
{
    ENDI_DATA_FRAME     frame;
    int                 len;

    len = MakeDataFrameByID(id, &frame, CODI_ACCESS_WRITE, seq, type, data, nLength);
	if (nTimeout == 0)
	{
    	SendToCoordinator(codi, (BYTE *)&frame, len);
		return CODIERR_PROGRESS;
	}

	return WaitingForDataReply(codi, CODI_ACCESS_WRITE, type, frame.hdr.seq, WAIT_WRITE_RESPONSE | WAIT_READ_RESPONSE,
							(BYTE *)&frame, len, nTimeout);
}


////////////////////////////////////////////////////////////////////
// Member functions
//////////////////////////////////////////////////////////////////////

COORDINATOR *CCoordinatorServer::AddCoordinator(CODIDEVICE *pDevice)
{
	COORDINATOR	*codi;

	codi = (COORDINATOR *)MALLOC(sizeof(COORDINATOR));
	if (codi == NULL)
		return NULL;

	memset(codi, 0, sizeof(COORDINATOR));
	codi->nIdentifier 	= CODI_IDENTIFIER;
	codi->pService		= (void *)this;
	codi->pStream		= NewStream((UINT_PTR)codi);
    codi->bWatchdog     = TRUE;
	memcpy(&codi->device, pDevice, sizeof(CODIDEVICE));
	codi->pos = (int)m_codiList.AddTail(codi);

	codiOnRegister((HANDLE)codi, pDevice);
	return codi;
}

BOOL CCoordinatorServer::RemoveCoordinator(COORDINATOR *codi)
{
    if (codi == NULL) return TRUE;
	if (codi->nIdentifier == 0)
		return TRUE;

	m_codiList.RemoveAt((POSITION)codi->pos);

	codiOnUnregister((HANDLE)codi, &codi->device);

	codi->nIdentifier = 0;
	if (codi->pStream != NULL)
		DeleteStream(codi->pStream);
	
	FREE(codi);
	return TRUE;
}

int CCoordinatorServer::StartCoordinator(COORDINATOR *codi)
{
	CDeviceClass	*pInterface = NULL;
	int				nError;

	if (codi->bStarted)
		return CODIERR_ALREADY_STARTED;

	switch(codi->device.nType) {
	  case CODI_COMTYPE_RS232 :
		   pInterface = new CSerialInterface();
		   break;
	  case CODI_COMTYPE_VIRTUAL :
		   pInterface = new CVirtualInterface();
		   break;
	  case CODI_COMTYPE_SPI :
	  default :
		   break;
	}

	if (pInterface == NULL)
	   return CODIERR_INVALID_INTERFACE;

	nError = pInterface->StartDevice(codi, &codi->device);
	if (nError != CODIERR_NOERROR)
	{
		delete pInterface;
		return nError;
	}

	codi->bStarted = TRUE;
	codiOnStartup((HANDLE)codi);
	return CODIERR_NOERROR;
}

int CCoordinatorServer::StopCoordinator(COORDINATOR *codi)
{
	CDeviceClass	*pInterface = NULL;
	int				nError;

	if (!codi->bStarted)
		return CODIERR_NOT_STARTED;

	pInterface = (CDeviceClass *)codi->pInterface;
	if (pInterface == NULL)
	   return CODIERR_INVALID_INTERFACE;

	codiOnShutdown((HANDLE)codi);

	nError = pInterface->StopDevice(codi);
	if (nError != CODIERR_NOERROR)
		return nError;

	delete pInterface;	
	codi->bStarted = FALSE;
	return CODIERR_NOERROR;
}

COORDINATOR *CCoordinatorServer::FindCoordinatorByName(char *pszName)
{
	COORDINATOR	*codi;
	POSITION	pos=0;

	codi = m_codiList.GetFirst(pos);
	while(codi)
	{
		if (strcmp(codi->device.szDevice, pszName) == 0)
			break;
		codi = m_codiList.GetNext(pos);
	}	
	return codi;
}

COORDINATOR *CCoordinatorServer::FindCoordinatorByHandle(COORDINATOR *pCoordinator)
{
	COORDINATOR	*codi;
	POSITION	pos=0;

	codi = m_codiList.GetFirst(pos);
	while(codi)
	{
		if (codi == pCoordinator)
			break;
		codi = m_codiList.GetNext(pos);
	}	
	return codi;
}

int CCoordinatorServer::SendToCoordinator(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	CDeviceClass	*pInterface;

	codi->nTotalSend += nLength;

	pInterface = (CDeviceClass *)codi->pInterface;
	return pInterface->Send(codi, pBuffer, nLength);
}

void CCoordinatorServer::SetCoordinatorPassive(COORDINATOR *codi, BOOL bPassive)
{
	CDeviceClass	*pInterface;

	pInterface = (CDeviceClass *)codi->pInterface;
	pInterface->SetPassive(codi, bPassive);
}

int CCoordinatorServer::WaitingForCommandReply(COORDINATOR *codi, BYTE type, BYTE cmd, BYTE seq, BYTE *pFrame, int len, BYTE *pBuffer, int *nLength, int nTimeout)
{
	CODIWAITREPLY	*pReply;
	int				nError;

	pReply = AddWaitReply(codi, type, CODI_FRAMETYPE_COMMAND, cmd, seq, WAIT_WRITE_RESPONSE | WAIT_READ_RESPONSE, NULL, pBuffer);
	if (pReply == NULL)
		return CODIERR_MEMORY_ERROR;

	SendToCoordinator(codi, pFrame, len);

	nError = WaitForReply(pReply, nTimeout);
	if (nError == CODIERR_NOERROR)
	{
		if (nLength != NULL)
			*nLength = pReply->nLength;
		if (type == CODI_ACCESS_WRITE)
		{
			if (pReply->result != CODI_SUCCESS)
				nError = GetCommandError(pReply->result);
		}
	}

	DeleteWaitReply(pReply);
	return nError;
}

int CCoordinatorServer::WaitingForDataReply(COORDINATOR *codi, BYTE type, BYTE cmd, BYTE seq, BYTE waitMask, BYTE *pFrame, int len, int nTimeout)
{
	CODIWAITREPLY	*pReply;
	int				nError;

	pReply = AddWaitReply(codi, type, CODI_FRAMETYPE_DATA, cmd, seq, waitMask, &((ENDI_DATA_FRAME *)pFrame)->id, NULL);
	if (pReply == NULL)
		return CODIERR_MEMORY_ERROR;

	SendToCoordinator(codi, pFrame, len);

	nError = WaitForReply(pReply, nTimeout);
	if (nError == CODIERR_NOERROR)
	{
		if (type == CODI_ACCESS_WRITE)
		{
			if (pReply->result != CODI_SUCCESS)
				nError = GetCommandError(pReply->result);
		}
	}

	DeleteWaitReply(pReply);
	return nError;
}

//////////////////////////////////////////////////////////////////////
// Frame work functions
//////////////////////////////////////////////////////////////////////

CODIWORKSTREAM *CCoordinatorServer::AddStreamQueue(COORDINATOR *codi, CODI_GENERAL_FRAME *pFrame, int nLength)
{
	CODIWORKSTREAM	*pStream;

	ceTime(&codi->tmLastRecv);
	codi->bNoResponse = FALSE;
	codi->nNoResCount = 0;
	codi->nTotalRecv += nLength;
	
	pStream = (CODIWORKSTREAM *)MALLOC(sizeof(CODIWORKSTREAM));
	if (pStream == NULL)
		return NULL;

	// Buffer는 DataStream에서 넘겨받은 버퍼를 Detach받아서 사용한다.
	// 메모리를 다시 할당하지 않코 재사용하고 free한다.

	pStream->pBuffer = (char *)pFrame;
	pStream->codi	 = codi;
	pStream->nLength = nLength;

	m_streamLocker.Lock();
	pStream->nPosition = (int)m_streamList.AddTail(pStream);
	m_streamLocker.Unlock();
	CTimeoutThread::ActiveThread();

	return pStream;
}

CODIWORKSTREAM *CCoordinatorServer::AddStreamQueue2(COORDINATOR *codi, BYTE *pFrame, int nLength)
{
	CODIWORKSTREAM	*pStream;
	
	pStream = (CODIWORKSTREAM *)MALLOC(sizeof(CODIWORKSTREAM));
	if (pStream == NULL)
		return NULL;

	pStream->pBuffer = (char *)MALLOC(nLength);
	if (pStream->pBuffer == NULL)
	{
		FREE(pStream);
		return NULL;
	}

	memcpy(pStream->pBuffer, pFrame, nLength);
	pStream->codi	 = codi;
	pStream->nLength = nLength;

	m_streamLocker.Lock();
	pStream->nPosition = (int)m_streamList.AddTail(pStream);
	m_streamLocker.Unlock();
	CTimeoutThread::ActiveThread();

	return pStream;
}

void CCoordinatorServer::ReceiveStreamWorker(COORDINATOR *codi, CODI_GENERAL_FRAME *pFrame, int nLength)
{
	char	szTime[20];
	BYTE	type, len;
	
	
	type = pFrame->hdr.ctrl & CODI_FRAMETYPE_MASK;
	TimeStringMilli(szTime);

	if (type == CODI_FRAMETYPE_DATA)
	{
		ENDI_DATA_FRAME		*pData;
		CODI_RADIO_STATUS	*pStatus;
		char	szGUID[17];
		EUI64	id;

		pData = (ENDI_DATA_FRAME *)pFrame;
		ReverseEUI64(&pData->id, &id);
		EUI64ToStr(&id, szGUID);

	    if ((pData->hdr.ctrl & CODI_FRAMEFLOW_MASK) != CODI_FRAMEFLOW_WRITERESPONSE)
		{
		
			len = nLength - (sizeof(CODI_HEADER)+sizeof(EUI64)+sizeof(CODI_TAIL)+sizeof(CODI_RADIO_STATUS));

			// Check zero size or older version
			if (len <= 0)
			{
				XDEBUG("----------- FRAME LENGTH ERROR (%d) -----------\r\n", len);
				XDEBUG("\033[1;40;32m%s MCU<-ENDI(%s): SEQ=%d, Receive %d byte(s)\033[0m\r\n",
						szTime, szGUID, pData->hdr.seq, nLength);
				XDUMP((char *)pFrame, nLength, TRUE);
				return;
			}

			pStatus = (CODI_RADIO_STATUS *)&pData->payload[len];
			if (pData->hdr.type == ENDI_DATATYPE_BYPASS)
			{
				
				XDEBUG("\033[1;40;32m%s MCU<-ENDI(%s): SEQ=%d, LQI=%d, RSSI=%d, Receive %d byte(s)\033[0m\r\n",
						szTime, szGUID, pData->hdr.seq, pStatus->LQI, pStatus->RSSI, len);
				XDUMP((char *)pData+sizeof(CODI_HEADER)+sizeof(EUI64), len, TRUE);
			}
			else
			{
				
				XDEBUG("\033[1;40;32m%s MCU<-ENDI(%s): SEQ=%d, LQI=%d, RSSI=%d, Receive %d byte(s)\033[0m\r\n",
						szTime, szGUID, pData->hdr.seq, pStatus->LQI, pStatus->RSSI, nLength);
				XDUMP((char *)pFrame, nLength, TRUE);
				PrintDataFrameHeader((ENDI_DATA_FRAME *)pFrame, nLength);
			}
		}
		else
		{
            /** Data Write Rsponse 도 다 찍는다 */
			//if (pData->hdr.type == ENDI_DATATYPE_LINK)
			//{
	
				XDEBUG("\033[1;40;32m%s MCU<-ENDI(%s): SEQ=%d, Receive %d byte(s)\033[0m\r\n",
						szTime, szGUID, pData->hdr.seq, nLength);
				XDUMP((char *)pFrame, nLength, TRUE);
				PrintDataFrameHeader((ENDI_DATA_FRAME *)pFrame, nLength);
			//}
		}
	}
	else if ((type == CODI_FRAMETYPE_INFORMATION) && (pFrame->hdr.type == CODI_INFO_ROUTE_RECORD))
	{
			
		XDEBUG("\033[1;40;36m%s MCU<-CODI(%s): SEQ=%d, Receive %d byte(s)\033[0m\r\n", 
					szTime, codi->szGUID, pFrame->hdr.seq, nLength);
#ifdef __CODIAPI_DEBUG__
	
		XDUMP((char *)pFrame, nLength, TRUE);
		PrintInformationFrameHeader((CODI_INFO_FRAME *)pFrame, nLength);
#endif
	
		OnInformationFrame(codi, (CODI_INFO_FRAME *)pFrame, nLength);
		return;
	}
	else
	{
#ifdef __CODIAPI_DEBUG__
		if ((type == CODI_FRAMETYPE_COMMAND) && (pFrame->hdr.type == CODI_CMD_PINGPONG))
		{
		
			// Live check message
			// Debugging을 찍지 않는다.
		}
		else
		{
				
			XDEBUG("\033[1;40;32m%s MCU<-CODI(%s): SEQ=%d, Receive %d byte(s)\033[0m\r\n",
					szTime, codi->szGUID, pFrame->hdr.seq, nLength);
			XDUMP((char *)pFrame, nLength, TRUE);
		}
#endif
	}


	switch(type) {
	  case CODI_FRAMETYPE_INFORMATION :
			
#ifdef __CODIAPI_DEBUG__
		   PrintInformationFrameHeader((CODI_INFO_FRAME *)pFrame, nLength);
#endif
		   OnInformationFrame(codi, (CODI_INFO_FRAME *)pFrame, nLength);
		   break;

	  case CODI_FRAMETYPE_COMMAND :
		 
		   if (pFrame->hdr.type != CODI_CMD_PINGPONG)
		   {
#ifdef __CODIAPI_DEBUG__
		   	   PrintCommandFrameHeader((CODI_COMMAND_FRAME *)pFrame, nLength);
			
#endif
		   	   OnCommandFrame(codi, (CODI_COMMAND_FRAME *)pFrame, nLength);
		
		   }
		   break;

	  case CODI_FRAMETYPE_DATA :
//		   if (pFrame->hdr.type != ENDI_DATATYPE_BYPASS)
//		   	   PrintDataFrameHeader((ENDI_DATA_FRAME *)pFrame, nLength);

           ceTime(&codi->tmLastComm);

		   OnDataFrame(codi, (ENDI_DATA_FRAME *)pFrame, nLength);
		   break;
	}
}

BOOL CCoordinatorServer::OnActiveThread()
{
	CODIWORKSTREAM	*pStream;

	
	for(;!IsThreadExitPending();)
	{
		m_streamLocker.Lock();
		pStream = m_streamList.GetHead();
		if (pStream) m_streamList.RemoveAt((POSITION)pStream->nPosition);
		m_streamLocker.Unlock();

		if (pStream == NULL)
			break;
	
		ReceiveStreamWorker(pStream->codi, (CODI_GENERAL_FRAME *)pStream->pBuffer, pStream->nLength);

		if(pStream->pBuffer) FREE(pStream->pBuffer);
		FREE(pStream);
	}

	return TRUE;		
}

void CCoordinatorServer::CheckCoordinatorState()
{
	CODI_COMMAND_FRAME	frame;
	cetime_t	now;
	char		szTime[25];
	COORDINATOR	*codi;
	POSITION	pos;
	int			len;

	ceTime(&now);
	if ((unsigned long)(now-m_tmLastStateCheck) < (unsigned long)10)
		return;
	
	m_codiLocker.Lock();

	codi = m_codiList.GetFirst(pos);
	while(codi)
	{
		if (codi->bStarted && !codi->bPassive && !codi->bBootloader && codi->bWatchdog)
		{
			// 마지막 수신 이후 10초가 경과한 경우
			ceTime(&now);
			if ((unsigned long)(now-codi->tmLastRecv) >= (unsigned long)10)
			{
				len = MakeCommandFrame(&frame, CODI_ACCESS_READ, CODI_CMD_PINGPONG, NULL, 0);
				SendToCoordinator(codi, (BYTE *)&frame, len);
			}

			if ((unsigned long)(now-codi->tmLastRecv) >= (unsigned long)30)
			{
				// 30초간 아무런 통신이 없음
				TimeStringMilli(szTime);
				XDEBUG("\033[1;40;31m%s: ---------- NO RESPONSE -----------\033[0m\r\n", szTime);
				codi->nNoResCount++;
				codi->bNoResponse = TRUE;

				// 16회이상 연속적이고 180초(3분)을 초과하면 즉시 리셋 요청
				if ((((codi->nNoResCount+2)%18) == 0) && ((unsigned long)(now-codi->tmLastRecv) >= (unsigned long)180)) 
					codiOnNoResponse(codi, TRUE);
				else  
                    codiOnNoResponse(codi, FALSE); 
			}
#ifndef _WIN32
			if (!codi->rk || codi->nStackStatus != CODI_STACK_UP) {
                XDEBUG("ResetKind 0x%02X StackStatus 0x%02X\r\n", codi->rk, codi->nStackStatus);
                // 180초 이상 상태가 Invalid 할 때
				TimeStringMilli(szTime);
				XDEBUG("\033[1;40;31m%s: ---------- INVALID CODI STATE -----------\033[0m\r\n", szTime);

                codi->nInvalidStateCount++;
				if ((((codi->nInvalidStateCount+1)%19) == 0))
					codiOnNoResponse(codi, TRUE);
                else
                    codiOnNoResponse(codi, FALSE); 
            }else {
                codi->nInvalidStateCount = 0;
            }
#endif
		}
		codi = m_codiList.GetNext(pos);
	}	
	ceTime(&m_tmLastStateCheck);
	m_codiLocker.Unlock();
}

BOOL CCoordinatorServer::OnTimeoutThread()
{
	
	CheckCoordinatorState();
	return OnActiveThread();
}

//////////////////////////////////////////////////////////////////////
// Send/Receive callback functions
//////////////////////////////////////////////////////////////////////

void CCoordinatorServer::OnPrevSendFrame(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
/*
	CODI_GENERAL_FRAME	*pFrame;
	BYTE	mode;

	// Coordinator의 정보를 읽는 동작을 빼고
	// 모두 Delay를 먹는다.
	pFrame = (CODI_GENERAL_FRAME *)pBuffer;
	mode = GET_FRAME_MODE(pFrame->hdr.ctrl);

	if (((pFrame->hdr.ctrl & CODI_FRAMETYPE_MASK) == CODI_FRAMETYPE_COMMAND) &&
		(mode == CODI_ACCESS_READ))
		return;

	// 200ms를 쉬고 쏜다.
	USLEEP(200000);
*/
}

void CCoordinatorServer::OnSendFrame(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	CODI_GENERAL_FRAME	*pFrame;
	CODI_INFO_FRAME		*pInfo;
	CODI_COMMAND_FRAME	*pCommand;
	ENDI_DATA_FRAME		*pData;
	char				szGUID[17];
	char				szTime[20];
	EUI64				id;
	BYTE				type, len;
	ceTime(&codi->tmLastSend);
	codi->nTotalSend += nLength;

	pFrame = (CODI_GENERAL_FRAME *)pBuffer;
	type = pFrame->hdr.ctrl & CODI_FRAMETYPE_MASK;

	TimeStringMilli(szTime);
	if (type != CODI_FRAMETYPE_DATA)
	{
		if ((type == CODI_FRAMETYPE_COMMAND) && (pFrame->hdr.type == CODI_CMD_PINGPONG))
		{
			// Live check message
			// Debugging을 찍지 않는다.
		}
		else
		{
#ifdef __CODIAPI_DEBUG__
			XDEBUG("\033[1;40;33m%s MCU->CODI(%s): SEQ=%d, Send %d byte(s)\033[0m\r\n",
					szTime, codi->szGUID, pFrame->hdr.seq, nLength);
			XDUMP((char *)pBuffer, nLength, TRUE);
#endif
			codiOnSend(codi, pFrame->hdr.ctrl, pFrame->hdr.type, pFrame->payload, pFrame->hdr.len);
		}
	}
	else
	{
		pData = (ENDI_DATA_FRAME *)pBuffer;
		ReverseEUI64(&pData->id, &id);
		eui64toa(&id, szGUID);		

		if (pData->hdr.type == ENDI_DATATYPE_BYPASS)
		{
			len = nLength - (sizeof(CODI_HEADER)+sizeof(EUI64)+sizeof(CODI_TAIL));
			XDEBUG("\033[1;40;33m%s MCU->ENDI(%s): SEQ=%d, Send %d byte(s)\033[0m\r\n", 
					szTime, szGUID, pData->hdr.seq, len);
			XDUMP((char *)(pBuffer+sizeof(CODI_HEADER)+sizeof(EUI64)), len, TRUE);
		}
		else
		{
			XDEBUG("\033[1;40;33m%s MCU->ENDI(%s): SEQ=%d, Send %d byte(s)\033[0m\r\n", 
					szTime, szGUID, pFrame->hdr.seq, nLength);
			XDUMP((char *)pBuffer, nLength, TRUE);
		}
		
		// Callback을 호출한다.
		EndDeviceSendCallback(codi, pData, nLength);
	}

	switch(type) {
	  case CODI_FRAMETYPE_INFORMATION :
		   pInfo = (CODI_INFO_FRAME *)pBuffer;
#ifdef __CODIAPI_DEBUG__
		   XDEBUG(" INFORMATION (ACCESS=%s, FLOW=%s, TYPE=%s[0x%02X], LENGTH=%d)\r\n",
					(pInfo->hdr.ctrl & CODI_FRAMEFLOW_READ) > 0 ? "Read" : "Write",
					(pInfo->hdr.ctrl & CODI_FRAMEFLOW_RESPONSE) > 0 ? "Response" : "Request",
					GetInformationTypeName(pInfo->hdr.type),
					pInfo->hdr.type, pInfo->hdr.len);
#endif
		   break;

	  case CODI_FRAMETYPE_COMMAND :
		   pCommand = (CODI_COMMAND_FRAME *)pBuffer;
		   if (pCommand->hdr.type != CODI_CMD_PINGPONG)
		   {
#ifdef __CODIAPI_DEBUG__   
		   	   XDEBUG(" COMMAND (ACCESS=%s, FLOW=%s, TYPE=%s[0x%02X], LENGTH=%d)\r\n",
					(pCommand->hdr.ctrl & CODI_FRAMEFLOW_READ) > 0 ? "Read" : "Write",
					(pCommand->hdr.ctrl & CODI_FRAMEFLOW_RESPONSE) > 0 ? "Response" : "Request",
					GetCommandTypeName(pCommand->hdr.type),
					pCommand->hdr.type, pCommand->hdr.len);
#endif
		   }
		   break;
	  case CODI_FRAMETYPE_DATA :
		   pData = (ENDI_DATA_FRAME *)pBuffer;
		   if (pData->hdr.type != ENDI_DATATYPE_BYPASS)
		   {
			   XDEBUG(" DATA (ACCESS=%s, FLOW=%s, TYPE=%s[0x%02X], LENGTH=%d)\r\n",
					(pData->hdr.ctrl & CODI_FRAMEFLOW_READ) > 0 ? "Read" : "Write",
					(pData->hdr.ctrl & CODI_FRAMEFLOW_RESPONSE) > 0 ? "Response" : "Request",
					GetDataTypeName(pData->hdr.type),
					pData->hdr.type, pData->hdr.len-sizeof(EUI64));
		   }
		   break;
	}
}

void CCoordinatorServer::OnReceiveFrame(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{		
	AddStream(codi->pStream, (char *)pBuffer, nLength);
}

void CCoordinatorServer::OnError(COORDINATOR *codi, int nError)
{			
	codiOnError(codi, CODI_DEVICE_ERROR);
}

//////////////////////////////////////////////////////////////////////
// Frame parsing function
//////////////////////////////////////////////////////////////////////

void CCoordinatorServer::OnReceiveCoordinator(COORDINATOR *codi, CODI_GENERAL_FRAME *pFrame, int nLength)
{			
	codi->nTotalRecv += nLength;
	AddStreamQueue(codi, pFrame, nLength);
}

//////////////////////////////////////////////////////////////////////
// Information frame
//////////////////////////////////////////////////////////////////////

void CCoordinatorServer::OnInformationFrame(COORDINATOR *codi, CODI_INFO_FRAME *pFrame, int nLength)
{
	CODI_INFO_BOOT_FRAME			*pBoot;
	CODI_INFO_STACK_FRAME			*pStack;
	CODI_INFO_JOIN_FRAME			*pJoin;
	CODI_INFO_ENERGYSCAN_FRAME		*pEnergy;
	CODI_INFO_ACTIVESCAN_FRAME		*pActive;
	CODI_INFO_SCANCOMPLETE_FRAME	*pScan;
	CODI_INFO_ERROR_FRAME			*pError;
	CODI_INFO_ROUTERECORD_FRAME		*pRoute;
	WORD					path[CODI_MAX_ROUTING_PATH];
	char					szBuffer[64];
	char					szGUID[17];
	EUI64					id;
	int						i;
	//BOOL					bJoinCtrl = FALSE;			
	switch(pFrame->hdr.type) {
	  case CODI_INFO_BOOT :
		   pBoot = (CODI_INFO_BOOT_FRAME *)pFrame;
		   ReverseEUI64(&pBoot->payload.id, &id);
		   EUI64ToStr(&id, codi->szGUID);		   	

		   memcpy(&codi->eui64, &id, sizeof(EUI64));
		   codi->shortid = BigToHostShort(pBoot->payload.shortid);
		   codi->fw		 = pBoot->payload.fw;
		   codi->hw		 = pBoot->payload.hw;
		   codi->zaif	 = pBoot->payload.zaif;
		   codi->zzif	 = pBoot->payload.zzif;
		   codi->build	 = pBoot->payload.build;
		   codi->rk		 = pBoot->payload.rk;

           /*-- Coordinator F/W 버전 중 Minor 버전 정보가 짝수이면 Super Parent Stack,
            *-- 홀수이면 Standard Stack을 나타낸다
            --*/
           codi->bSuperParent = ((pBoot->payload.fw & 0x0F) % 2) ? FALSE : TRUE;

		   XDEBUG("--------------------------------------------------------------\r\n");
		   XDEBUG("    EUI64 ID                     = %s\r\n", codi->szGUID);
		   XDEBUG("    Firmware version             = %d.%d\r\n", codi->fw >> 4, codi->fw & 0xf);
		   XDEBUG("    Hardware version             = %d.%d\r\n", codi->hw >> 4, codi->hw & 0xf);
		   XDEBUG("    Application Device Interface = %d.%d\r\n", codi->zaif >> 4, codi->zaif & 0xf);
		   XDEBUG("    ZigBee Interface             = %d.%d\r\n", codi->zzif >> 4, codi->zzif & 0xf);
		   XDEBUG("    Build number                 = %d\r\n", (codi->build >> 4) * 10 + (codi->build & 0xf));
		   XDEBUG("    Reset kind                   = 0x%02X\r\n", codi->rk);
		   XDEBUG("    Super Parent                 = %s\r\n", codi->bSuperParent ? "TRUE" : "FALSE");
		   XDEBUG("--------------------------------------------------------------\r\n");

		   // 리부팅되면 Stack의 상태를 Down 상태로 변경한다.
		   codi->nStackStatus = CODI_STACK_DOWN;

		   // Callback을 호출한다.
		   codiOnBoot((HANDLE)codi, &codi->eui64, codi->shortid,
				    codi->fw, codi->hw, codi->zaif, codi->zzif, codi->build, codi->rk); 
		   break;

	  case CODI_INFO_STACK_STATUS :
		   pStack = (CODI_INFO_STACK_FRAME *)pFrame;
		   codi->nStackStatus = pStack->payload.status;

		   XDEBUG("--------------------------------------------------------------\r\n");
		   XDEBUG("    Stack Status (status=0x%02X)\r\n", codi->nStackStatus);
		   XDEBUG("--------------------------------------------------------------\r\n");
		   EndDeviceNotifyStackStatus(pStack->payload.status);
		   codiOnStackStatus((HANDLE)codi, codi->nStackStatus);	
		   break;

	  case CODI_INFO_JOIN :
		   pJoin = (CODI_INFO_JOIN_FRAME *)pFrame;
		   ReverseEUI64(&pJoin->payload.id, &id);
		   EUI64ToStr(&id, szGUID);

		   XDEBUG("--------------------------------------------------------------\r\n");
		   XDEBUG("    ID=%s, ShortID=0x%04X, Parent=0x%04X, Status=0x%02X\r\n",
					szGUID,
					BigToHostShort(pJoin->payload.short_id),
					BigToHostShort(pJoin->payload.parent_short_id),
					pJoin->payload.status); 
		   XDEBUG("--------------------------------------------------------------\r\n");

		   codiOnDeviceJoin((HANDLE)codi, &id, BigToHostShort(pJoin->payload.short_id),
			   BigToHostShort(pJoin->payload.parent_short_id),
			   pJoin->payload.status);
		   break;

	  case CODI_INFO_ENERGY_SCAN :
		   pEnergy = (CODI_INFO_ENERGYSCAN_FRAME *)pFrame;

		   XDEBUG("--------------------------------------------------------------\r\n");
		   XDEBUG("    Channel=%d, RSSI=%d\r\n", pEnergy->payload.channel, pEnergy->payload.rssi);
		   XDEBUG("--------------------------------------------------------------\r\n");

		   codiOnEnergyScan((HANDLE)codi, pEnergy->payload.channel, pEnergy->payload.rssi);
		   break;

	  case CODI_INFO_ACTIVE_SCAN :
		   pActive = (CODI_INFO_ACTIVESCAN_FRAME *)pFrame;

		   BINTOSTR(pActive->payload.extpanid, szBuffer, 16);
		   XDEBUG("--------------------------------------------------------------\r\n");
		   XDEBUG("    Channel=%d, PANID=%d, EXTPANID=%s, EXPJOIN=%d, PROFILE=%d\r\n",
					pActive->payload.channel,
					BigToHostShort(pActive->payload.panid),
					szBuffer,
					pActive->payload.expecting_join,
					pActive->payload.stack_profile);
		   XDEBUG("--------------------------------------------------------------\r\n");

		   codiOnActiveScan((HANDLE)codi, pActive->payload.channel,
					BigToHostShort(pActive->payload.panid),
					pActive->payload.extpanid,
					pActive->payload.expecting_join,
					pActive->payload.stack_profile);
		   break;

	  case CODI_INFO_SCAN_COMPLETE :
		   pScan = (CODI_INFO_SCANCOMPLETE_FRAME *)pFrame;

		   XDEBUG("--------------------------------------------------------------\r\n");
		   XDEBUG("    Channel=%d, Status=0x%02X\r\n", pScan->payload.channel, pScan->payload.status);
		   XDEBUG("--------------------------------------------------------------\r\n");

		   codiOnScanComplete((HANDLE)codi, pScan->payload.channel, pScan->payload.status);
		   break;

	  case CODI_INFO_ROUTE_RECORD :
		   pRoute = (CODI_INFO_ROUTERECORD_FRAME *)pFrame;
		   ReverseEUI64(&pRoute->payload.id, &id);
		   	
		   for(i=0; i<pRoute->payload.hops && i<CODI_MAX_ROUTING_PATH; i++)
			   path[i] = BigToHostShort(pRoute->payload.routePath[i]);

		   EUI64ToStr(&id, szGUID);
		   XDEBUG(" ID=%s, ShortID=0x%04X, Hops=%d\r\n", szGUID,
							BigToHostShort(pRoute->payload.shortid), pRoute->payload.hops);

		   codiOnRouteRecord((HANDLE)codi, &id, BigToHostShort(pRoute->payload.shortid),
							pRoute->payload.hops, path);
		   break;

	  case CODI_INFO_ERROR :
		   pError = (CODI_INFO_ERROR_FRAME *)pFrame;
		   codi->nLastError = pError->payload.code;

#ifdef __CODIAPI_DEBUG__
		   if (pError->payload.code != 0)
		   {
			   XDEBUG("    \033[1;40;31mError Code = %s (0x%02X)\033[0m\r\n",
					GetErrorTypeName(pError->payload.code), pError->payload.code); 
		   }
		   else
		   {
			   XDEBUG("    Error Code = %s (0x%02X)\r\n",
					GetErrorTypeName(pError->payload.code), pError->payload.code); 
		   }
#endif
		   codiOnError((HANDLE)codi, pError->payload.code);
		   break;
	   	 	
	}

    codiOnInformation((HANDLE)codi, pFrame->hdr.ctrl, pFrame->hdr.type, (BYTE *)pFrame->payload, pFrame->hdr.len);
}

//////////////////////////////////////////////////////////////////////
// Command frame
//////////////////////////////////////////////////////////////////////

void CCoordinatorServer::OnCommandFrame(COORDINATOR *codi, CODI_COMMAND_FRAME *pFrame, int nLength)
{				
	if ((pFrame->hdr.ctrl & CODI_FRAMEFLOW_READ) > 0)
		 OnCommandReadFrame(codi, pFrame, nLength);
	else OnCommandWriteFrame(codi, pFrame, nLength);
}

void CCoordinatorServer::OnCommandReadFrame(COORDINATOR *codi, CODI_COMMAND_FRAME *pFrame, int nLength)
{
	CODI_MODULE_FRAME			*pModule;
	CODI_SERIAL_FRAME			*pSerial;
	CODI_NETWORK_FRAME			*pNetwork;
	CODI_SECURITY_FRAME			*pSecurity;
	CODI_SCAN_FRAME				*pScan;
	CODI_BINDINGTABLE_FRAME		*pBinding;
	CODI_NEIGHBORTABLE_FRAME	*pNeighbor;
	CODI_STACKMEMORY_FRAME		*pStack;
	CODI_PERMIT_FRAME			*pPermit;
	CODI_ROUTEDISCOVERY_FRAME	*pDiscovery;
	CODI_MULTICASTHOP_FRAME		*pHops;
//	CODI_FORMNETWORK_FRAME		*pNetwork;
//	CODI_RESET_FRAME			*pReset;
	char						szKey[64];
	char						szString[17];
	EUI64						id;
	int							i, cnt;					
	switch(pFrame->hdr.type) {
	  case CODI_CMD_MODULE_PARAM :
		   pModule = (CODI_MODULE_FRAME *)pFrame;
		   ReverseEUI64(&pModule->payload.id, &id);
		   EUI64ToStr(&id, codi->szGUID);		   	

		   ceTime(&codi->tmBoot);
		   memcpy(&codi->eui64, &id, sizeof(EUI64));
		   codi->fw		= pModule->payload.fw;
		   codi->hw		= pModule->payload.hw;
		   codi->zaif	= pModule->payload.zaif;
		   codi->zzif	= pModule->payload.zzif;
		   codi->build	= pModule->payload.build;
		   codi->rk		= pModule->payload.rk;

#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    EUI64 ID                     = %s\r\n", codi->szGUID);
		   XDEBUG("    Firmware version             = %d.%d\r\n", codi->fw >> 4, codi->fw & 0xf);
		   XDEBUG("    Hardware version             = %d.%d\r\n", codi->hw >> 4, codi->hw & 0xf);
		   XDEBUG("    Application Device Interface = %d.%d\r\n", codi->zaif >> 4, codi->zaif & 0xf);
		   XDEBUG("    ZigBee Interface             = %d.%d\r\n", codi->zzif >> 4, codi->zzif & 0xf);
		   XDEBUG("    Build number                 = %d.%d\r\n", codi->build >> 4, codi->build & 0xf);
		   XDEBUG("    Reset kind                   = 0x%02X\r\n", codi->rk);
#endif
		   break;

	  case CODI_CMD_SERIAL_PARAM :
		   pSerial = (CODI_SERIAL_FRAME *)pFrame;
#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    Baud Rate = %d\r\n", pSerial->payload.baudrate);
		   XDEBUG("    Parity    = %d\r\n", pSerial->payload.parity);
		   XDEBUG("    Data Bit  = %d\r\n", pSerial->payload.databit);
		   XDEBUG("    Stop Bit  = %d\r\n", pSerial->payload.stopbit);
		   XDEBUG("    RTS/CTS   = %d\r\n", pSerial->payload.rtscts);
#endif
		   break;

	  case CODI_CMD_NETWORK_PARAM :
		   pNetwork = (CODI_NETWORK_FRAME *)pFrame;
		   KEYTOSTR(pNetwork->payload.extpanid, szKey, 8);
		   BINTOSTR(pNetwork->payload.extpanid, szString, 8);
#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    Auto Setting   = %d\r\n", pNetwork->payload.autoset);
		   XDEBUG("    Channel        = %d\r\n", pNetwork->payload.channel);
		   XDEBUG("    PANID          = %d\r\n", BigToHostShort(pNetwork->payload.panid));
		   XDEBUG("    Extended PANID = '%s' %s\r\n", szKey, szString);
		   XDEBUG("    RF Power       = %d\r\n", pNetwork->payload.power);
		   XDEBUG("    TX Power Mode  = %d\r\n", pNetwork->payload.txpowermode);
#endif
		   break;

	  case CODI_CMD_SECURITY_PARAM :	
		   pSecurity = (CODI_SECURITY_FRAME *)pFrame;
#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    Security     = %d\r\n", pSecurity->payload.security);
#endif
		   KEYTOSTR(pSecurity->payload.link_key, szKey, 16);
		   BINTOSTR(pSecurity->payload.link_key, szString, 16);
#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    Link Key     = '%s' %s\r\n", szKey, szString);
#endif
		   KEYTOSTR(pSecurity->payload.network_key, szKey, 16);
		   BINTOSTR(pSecurity->payload.network_key, szString, 16);
#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    Network Key  = '%s' %s\r\n", szKey, szString);
#endif
		   break;

	  case CODI_CMD_SCAN_PARAM :
		   pScan = (CODI_SCAN_FRAME *)pFrame;
#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    Channel Scan Mask    = 0x%08X\r\n", BigToHostInt(pScan->payload.scan_mask));
		   XDEBUG("    Energy Scan Duration = %d\r\n", pScan->payload.energy_scan_duration);
		   XDEBUG("    Active Scan Duration = %d\r\n", pScan->payload.active_scan_duration);
#endif
		   break;

	  case CODI_CMD_BINDING_TABLE :
		   pBinding = (CODI_BINDINGTABLE_FRAME *)pFrame;
		   cnt = pBinding->hdr.len / sizeof(CODI_BINDING_ENTRY);
		   for(i=0; i<cnt; i++)
		   {
		   	   ReverseEUI64(&pBinding->payload.list[i].id, &id);
		   	   EUI64ToStr(&id, szKey);		   	
#ifdef __CODIAPI_DEBUG__
			   XDEBUG("    INDEX(%d) : ID=%s, TYPE=%d, LOCAL=%d, REMOTE=%d, LAST HEARD TICK=%d\r\n",
					pBinding->payload.list[i].index,
					szKey,
					pBinding->payload.list[i].type,
					pBinding->payload.list[i].local,
					pBinding->payload.list[i].remote,
					BigToHostShort(pBinding->payload.list[i].lastheard));
#endif
		   }
		   break;

	  case CODI_CMD_NEIGHBOR_TABLE :
		   pNeighbor = (CODI_NEIGHBORTABLE_FRAME *)pFrame;
		   cnt = pNeighbor->hdr.len / sizeof(CODI_NEIGHBOR_ENTRY);
		   for(i=0; i<cnt; i++)
		   {
		   	   ReverseEUI64(&pNeighbor->payload.list[i].id, &id);
		   	   EUI64ToStr(&id, szKey);		   	
#ifdef __CODIAPI_DEBUG__
			   XDEBUG("    %d : ID=%s, NETWORKID=%d, LQI=%d, INCOST=%d, OUTCOST=%d, AGE=%d\r\n",
					i+1,
					szKey,
					BigToHostShort(pNeighbor->payload.list[i].networkid),
					pNeighbor->payload.list[i].lqi,
					pNeighbor->payload.list[i].in_cost,
					pNeighbor->payload.list[i].out_cost,
					pNeighbor->payload.list[i].age);
#endif
		   }
		   break;

	  case CODI_CMD_STACK_MEMORY :
		   pStack = (CODI_STACKMEMORY_FRAME *)pFrame;
#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    ADDRESS_TABLE_SIZE        = %d\r\n", pStack->payload.ADDRESS_TABLE_SIZE);
		   XDEBUG("    WHOLE_ADDRESS_TABLE_SIZE  = %d\r\n", pStack->payload.WHOLE_ADDRESS_TABLE_SIZE);
		   XDEBUG("    NEIGHBOR_TABLE_SIZE       = %d\r\n", pStack->payload.NEIGHBOR_TABLE_SIZE);
		   XDEBUG("    SOURCE_ROUTE_TABLE_SIZE   = %d\r\n", pStack->payload.SOURCE_ROUTE_TABLE_SIZE);
		   XDEBUG("    ROUTE_TABLE_SIZE          = %d\r\n", pStack->payload.ROUTE_TABLE_SIZE);
		   XDEBUG("    MAX_HOPS                  = %d\r\n", pStack->payload.MAX_HOPS);
		   XDEBUG("    PACKET_BUFFER_COUNT       = %d\r\n", pStack->payload.PACKET_BUFFER_COUNT);
           if(nLength >= (int)sizeof(CODI_STACKMEMORY_FRAME)) {
		       XDEBUG("    SOFTWARE_VERSION          = 0x%04X\r\n", pStack->payload.SOFTWARE_VERSION);
           }
#endif
		   break;

	  case CODI_CMD_PERMIT :
		   pPermit = (CODI_PERMIT_FRAME *)pFrame;
#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    Permit Time = %d\r\n", pPermit->payload.permit_time);
#endif
		   break;

	  case CODI_CMD_ROUTE_DISCOVERY :
		   pDiscovery = (CODI_ROUTEDISCOVERY_FRAME *)pFrame;

#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    ROUTE_DISCOVERY           = %s\r\n", pDiscovery->payload.type ? "ON" : "OFF");
#endif
		   codi->discovery	= pDiscovery->payload.type;
           break;

	  case CODI_CMD_MULTICAST_HOP :
		   pHops = (CODI_MULTICASTHOP_FRAME *)pFrame;

#ifdef __CODIAPI_DEBUG__
		   XDEBUG("    MULTICAST_HOPS            = %d\r\n", pHops->payload.hop);
#endif
		   codi->mhops	= pHops->payload.hop;
           break;

	  case CODI_CMD_BIND :
	  case CODI_CMD_FORM_NETWORK :
	  case CODI_CMD_RESET :
	  case CODI_CMD_PINGPONG :
	  case CODI_CMD_BOOTLOAD :
	  case CODI_CMD_EXTENDED_TIMEOUT :
	  case CODI_CMD_DIRECT_COMM :
		   break;
	}

	// CALLBACK
	codiOnCommand((HANDLE)codi, pFrame->hdr.ctrl, pFrame->hdr.type, (BYTE *)pFrame->payload, pFrame->hdr.len);
    
	// 대기중인 결과가 있으면 응답해 준다.
	OnCommandReplyMessage(codi, pFrame, nLength);

}

void CCoordinatorServer::OnCommandWriteFrame(COORDINATOR *codi, CODI_COMMAND_FRAME *pFrame, int nLength)
{
	CODI_WRITE_FRAME	*pWrite;				
	pWrite = (CODI_WRITE_FRAME *)pFrame;
	if (pWrite->payload.code != 0)
	{
		XDEBUG("    \033[1;40;31mWrite Result = 0x%02X %s\033[0m\r\n",
			pWrite->payload.code, GetErrorTypeName(pWrite->payload.code));
	}

	// Save last error code
	codi->nLastError = pWrite->payload.code;

	// CALLBACK
	codiOnCommandError((HANDLE)codi, pFrame->hdr.ctrl, pWrite->hdr.type, pWrite->payload.code);

	// 대기중인 결과가 있으면 응답해 준다.
	OnCommandReplyMessage(codi, pFrame, nLength);
}

//////////////////////////////////////////////////////////////////////
// Data frame
//////////////////////////////////////////////////////////////////////

void CCoordinatorServer::OnDataFrame(COORDINATOR *codi, ENDI_DATA_FRAME *pFrame, int nLength)
{
	ENDI_METERING_FRAME	*pMetering;
	ENDI_EVENT_FRAME	*pEvent;
	ENDI_STATE_FRAME	*pState;
	ENDI_DISCOVERY_FRAME*pDiscovery;
#if defined(__SUPPORT_DEVICE_NODE__)
	ENDI_TIMESYNC_FRAME *pTimesync;
#endif
	CODI_RADIO_STATUS	*pStatus;
    MBUS_DATA_FRAME     *pMbus = NULL;
    FRAMEDATA           *pData = NULL;
	EUI64				id;
	BYTE				flow, mode;
	BYTE				len;
    WORD                shortid = 0x00;
				
    mode = GET_FRAME_MODE(pFrame->hdr.ctrl); 
    flow = GET_FRAME_FLOW(pFrame->hdr.ctrl);

	if ((mode == CODI_ACCESS_READ) && (flow == CODI_FLOW_RESPONSE))
	{
		len = pFrame->hdr.len - sizeof(EUI64) - sizeof(CODI_RADIO_STATUS);
		// Check zero size or older version
		if (len <= 0) return;
		pStatus = (CODI_RADIO_STATUS *)&pFrame->payload[len]; 
	}
	else
	{
		len = pFrame->hdr.len - sizeof(EUI64);
		pStatus = NULL;
	}
    
	switch(pFrame->hdr.type) {
	  case ENDI_DATATYPE_METERING :
		   pMetering = (ENDI_METERING_FRAME *)pFrame;
		   ReverseEUI64(&pMetering->id, &id);
		   endiOnMetering((HANDLE)codi, &id, pMetering->hdr.ctrl,
						&pMetering->payload, len,
						pStatus ? pStatus->LQI : 0,
						pStatus ? pStatus->RSSI : 0);
		   break;

	  case ENDI_DATATYPE_EVENT :
		   pEvent = (ENDI_EVENT_FRAME *)pFrame;
		   ReverseEUI64(&pEvent->id, &id);
		   endiOnEvent((HANDLE)codi, &id, pEvent->hdr.ctrl,
						&pEvent->payload, len,
						pStatus ? pStatus->LQI : 0,
						pStatus ? pStatus->RSSI : 0);
		   break;

      case ENDI_DATATYPE_STATE :
           pState = (ENDI_STATE_FRAME *)pFrame;
		   ReverseEUI64(&pState->id, &id);

           if(len > 2) {
                len = len - 2;
                memcpy(&shortid, &pFrame->payload[len], sizeof(WORD));
           }
		   endiOnState((HANDLE)codi, &id, pState->hdr.ctrl,
						&pState->payload, len,
						pStatus ? pStatus->LQI : 0,
						pStatus ? pStatus->RSSI : 0, 
                        shortid);
		   break;

      case ENDI_DATATYPE_DISCOVERY :
           pDiscovery = (ENDI_DISCOVERY_FRAME *)pFrame;
		   ReverseEUI64(&pDiscovery->id, &id);
		   endiOnDiscovery((HANDLE)codi, &id, pDiscovery->hdr.ctrl,
						&pDiscovery->payload, len,
						pStatus ? pStatus->LQI : 0,
						pStatus ? pStatus->RSSI : 0);
		   break;

      case ENDI_DATATYPE_TIMESYNC :
#if defined(__SUPPORT_DEVICE_NODE__)
           pTimesync = (ENDI_TIMESYNC_FRAME *)pFrame;
		   ReverseEUI64(&pTimesync->id, &id);
		   endiOnTimesync((HANDLE)codi, &id, pTimesync->hdr.ctrl,
						&pTimesync->payload, len,
						pStatus ? pStatus->LQI : 0,
						pStatus ? pStatus->RSSI : 0);
			 
#endif
		   break;

	  case ENDI_DATATYPE_BYPASS :
	  case ENDI_DATATYPE_NETWORK_LEAVE :
	  case ENDI_DATATYPE_AMR :
	  case ENDI_DATATYPE_ROM_READ :
	  case ENDI_DATATYPE_MCCB :
	  case ENDI_DATATYPE_LINK :
	  case ENDI_DATATYPE_LONG_ROM_READ :
	  case ENDI_DATATYPE_COMMAND :
	  case ENDI_DATATYPE_SUPER_PARENT :
  	  case ENDI_DATATYPE_DIO :
  	  case ENDI_DATATYPE_NAN :
		   ReverseEUI64(&pFrame->id, &id);
		   if(EndDeviceRecvCallback(codi, pFrame, len, nLength,
						pStatus ? pStatus->LQI : 0,
						pStatus ? pStatus->RSSI : 0)) {
	            OnDataReplyMessage(codi, pFrame);

                while((pData=GetNextData(&id))!=NULL)
                {
                    if(EndDeviceRecvCallback(codi, pData->data, pData->nLength, pData->nTLength, 
                                pData->LQI, pData->RSSI)) {

	                    XDEBUG(" ------------------ Cached Frame SEQ=%3d ------------------ \r\n", pData->seq);	
                        XDUMP((char *)pData->data, pData->nTLength, TRUE);

                        OnDataReplyMessage(codi, pData->data);
                    } else break;
                    if(pData->data) FREE(pData->data);
                    FREE(pData);
                }
                if(pData) {
                    if(pData->data) FREE(pData->data);
                    FREE(pData);
                }
           }
		   break;

       case ENDI_DATATYPE_MBUS :
           ReverseEUI64(&pFrame->id, &id);
           /** Specification 문서와 다르게 MPUS Payload에 mbus_seq가 들어 있지 않다.
             * 따라서 zigbee header의 seq 값을 사용한다.
             */
           endiOnMbusData((HANDLE)codi, &id, pFrame->hdr.ctrl, pFrame->hdr.seq,
                        pFrame->hdr.seq - 1, 0, pFrame->payload, len,
                        pStatus ? pStatus->LQI : 0,
                        pStatus ? pStatus->RSSI : 0);
            break;

       case ENDI_DATATYPE_MBUS_NEW :
           ReverseEUI64(&pFrame->id, &id);
           pMbus = (MBUS_DATA_FRAME *) pFrame->payload;
           endiOnMbusData((HANDLE)codi, &id, pFrame->hdr.ctrl,pFrame->hdr.seq,
                        pMbus->mbusSeq, 1, pMbus->payload, len - 1,
                        pStatus ? pStatus->LQI : 0,
                        pStatus ? pStatus->RSSI : 0);
            break;

#ifdef __SUPPORT_KUKDONG_METER__
       case ENDI_DATATYPE_KEUKDONG :
           ReverseEUI64(&pFrame->id, &id);
		   endiOnKeukdongData((HANDLE)codi, &id, pFrame->hdr.ctrl, 
						pFrame->payload, len,
						pStatus ? pStatus->LQI : 0,
						pStatus ? pStatus->RSSI : 0);
            break;
#endif

       case ENDI_DATATYPE_PUSH :
          /** Issue #84 : Push Metering 
           *    Push는 Coordinator 입장에서는 항상 Read Response이고 
           *    Node 입장에서는 항상 Write Request이다.
           */
           ReverseEUI64(&pFrame->id, &id);
		   endiOnPush((HANDLE)codi, &id, pFrame->hdr.ctrl, pFrame->hdr.seq,
		                (BYTE *)&pFrame->payload, len, 
                        pStatus ? pStatus->LQI : 0,
                        pStatus ? pStatus->RSSI : 0);
            break;

	  default :
		   ReverseEUI64(&pFrame->id, &id);
		   endiOnUserDefine((HANDLE)codi, &id, pFrame->hdr.ctrl, pFrame->hdr.seq, pFrame->hdr.type,
						(BYTE *)&pFrame->payload, len,
						pStatus ? pStatus->LQI : 0,
						pStatus ? pStatus->RSSI : 0);
		   break;
	}
 

}

/** Issue #1622: Sleepy End Device를 위해 Delivery Timeout을 조정하는 기능 추가
  */
int CCoordinatorServer::SetExtendedTimeout(ENDDEVICE *endi, BOOL bExtendedTimeout)
{					
#ifndef __NETWORK_NODE__
	CODI_COMMAND_FRAME	frame;
	CODI_EXTENDEDTIMEOUT_PAYLOAD	payload;	
    int len;

	// Extened Timeout을 설정한다.
	memset(&payload, 0, sizeof(CODI_EXTENDEDTIMEOUT_PAYLOAD));
	ReverseEUI64(&endi->id, &payload.id);
	payload.type	= bExtendedTimeout ? CODI_EXTENDED_TIMEOUT_EXTENDED : CODI_EXTENDED_TIMEOUT_NORMAL;

	len = MakeCommandFrame(&frame, CODI_ACCESS_WRITE, CODI_CMD_EXTENDED_TIMEOUT, (BYTE *)&payload, 
        sizeof(CODI_EXTENDEDTIMEOUT_PAYLOAD));

	return WaitingForCommandReply(endi->codi, CODI_ACCESS_WRITE, CODI_CMD_EXTENDED_TIMEOUT, frame.hdr.seq,
						(BYTE *)&frame, len, NULL, NULL, 3000);
#else
    return CODIERR_NOERROR;
#endif
}
