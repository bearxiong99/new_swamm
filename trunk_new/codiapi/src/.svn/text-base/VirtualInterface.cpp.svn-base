////////////////////////////////////////////////////////////////////
//
//  VirtualInterface.cpp: implementation of the CVirtualInterface class.
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
#include "VirtualInterface.h"
#include "CoordinatorServer.h"
#include "DebugUtil.h"

////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVirtualInterface::CVirtualInterface()
{
}

CVirtualInterface::~CVirtualInterface()
{
}

////////////////////////////////////////////////////////////////////
// functions
//////////////////////////////////////////////////////////////////////

int CVirtualInterface::GetFD(COORDINATOR *codi)
{
	return 0;
}

int CVirtualInterface::StartDevice(COORDINATOR *codi, CODIDEVICE *pDevice)
{
	int		nError;

	XDEBUG("CVirtualInterface: StartDevice\r\n");
	nError = CDeviceClass::StartDevice(codi, pDevice);
	if (nError != CODIERR_NOERROR)
		return nError;

	CTimeoutThread::StartupThread(3);
	return CODIERR_NOERROR;	
}

int CVirtualInterface::StopDevice(COORDINATOR *codi)
{
	XDEBUG("CVirtualInterface: StopDevice\r\n");
	CTimeoutThread::ShutdownThread();
	CDeviceClass::StopDevice(codi);
	return CODIERR_NOERROR;	
}

int CVirtualInterface::Send(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	return CODIERR_NOERROR;	
}

void CVirtualInterface::OnSend(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	CCoordinatorServer	*pServer;

	pServer = (CCoordinatorServer *)codi->pService;
	pServer->OnSendFrame(codi, pBuffer, nLength);	
}

void CVirtualInterface::OnRecv(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	CCoordinatorServer	*pServer;

	pServer = (CCoordinatorServer *)codi->pService;
	pServer->OnReceiveFrame(codi, pBuffer, nLength);	
}

BYTE	*bootmsg	= (BYTE *)const_cast<char *>("\x87\x98\x0\x11\x10");

BOOL CVirtualInterface::OnActiveThread()
{
	OnRecv(m_selfCoordinator, (BYTE *)bootmsg, 10);
	return TRUE;
}

BOOL CVirtualInterface::OnTimeoutThread()
{
	OnActiveThread();
	return TRUE;
}
