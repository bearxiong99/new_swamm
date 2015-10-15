////////////////////////////////////////////////////////////////////
//
//  SerialInterface.cpp: implementation of the CSerialInterface class.
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
#include "SerialInterface.h"
#include "CoordinatorServer.h"
#include "CoordinatorUtility.h"
#include "DebugUtil.h"

////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialInterface::CSerialInterface()
{
}

CSerialInterface::~CSerialInterface()
{
}

////////////////////////////////////////////////////////////////////
// functions
//////////////////////////////////////////////////////////////////////

int CSerialInterface::GetFD(COORDINATOR *codi)
{
	return CSerialServer::GetHandle();
}

int CSerialInterface::StartDevice(COORDINATOR *codi, CODIDEVICE *pDevice)
{
	int		nError;
	int		nOptions = 0;
	int		nSpeed = 0;

	XDEBUG("CSerialInterface: StartDevice\r\n");
	nError = CDeviceClass::StartDevice(codi, pDevice);
	if (nError != CODIERR_NOERROR)
		return nError;

#ifdef _WIN32
	switch(pDevice->nSpeed) {
	  case 115200 :	nSpeed = CBR_115200; break;
	  case 57600 :	nSpeed = CBR_57600; break;
	  case 38400 :	nSpeed = CBR_38400; break;
	  case 19200 :	nSpeed = CBR_19200; break;
	  case 9600 :	nSpeed = CBR_9600; break;
	  default : 	nSpeed = CBR_115200; break;
	}

	nOptions = (pDevice->nOptions & CODI_OPT_RTSCTS) ? TRUE : FALSE;
	if (!Connect(pDevice->szDevice, nSpeed, NOPARITY, 8, ONESTOPBIT, nOptions))
		return CODIERR_DEVICE_ERROR;
#else
	switch(pDevice->nSpeed) {
	  case 115200 :	nSpeed = B115200; break;
	  case 57600 :	nSpeed = B57600; break;
	  case 38400 :	nSpeed = B38400; break;
	  case 19200 :	nSpeed = B19200; break;
	  case 9600 :	nSpeed = B9600; break;
	  default : 	nSpeed = B115200; break;
	}

	nOptions = (pDevice->nOptions & CODI_OPT_RTSCTS) ? CRTSCTS : 0;
	CSerialServer::DisableSendFail(TRUE);
	if (!CSerialServer::Startup(pDevice->szDevice, 1, -1, nSpeed, NONE_PARITY, nOptions))
		return CODIERR_DEVICE_ERROR;
#endif


	return CODIERR_NOERROR;	
}

int CSerialInterface::StopDevice(COORDINATOR *codi)
{
#ifdef _WIN32
	Disconnect();
#else
	CSerialServer::DisableSendFail(FALSE);
	CSerialServer::Shutdown();
#endif

	CDeviceClass::StopDevice(codi);
	return CODIERR_NOERROR;	
}

void CSerialInterface::SetPassive(COORDINATOR *codi, BOOL bPassive)
{
	CSerialServer::SetPassiveMode(bPassive);
}

BOOL CSerialInterface::EnableFlowControl(COORDINATOR *codi, BOOL bEnable)
{
	return CSerialServer::EnableFlowControl(bEnable);
}

int CSerialInterface::Send(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	if (m_pSaveSession == 0)
		return 0;

	// Passive mode 또는 Bootloader 모드이면 전송을 무시한다.
	if (codi->bPassive || codi->bBootloader)
		return nLength;

#ifdef _WIN32
	CSerialWriter::AddSendStream((LPSTR)pBuffer, nLength);
#else
	CSerialServer::AddSendStream((WORKSESSION *)m_pSaveSession, (char *)pBuffer, nLength);
#endif
	return nLength;	
}

////////////////////////////////////////////////////////////////////
// Coordinator override functions
//////////////////////////////////////////////////////////////////////

void CSerialInterface::OnPrevSend(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	CCoordinatorServer	*pServer;

	pServer = (CCoordinatorServer *)codi->pService;
	pServer->OnPrevSendFrame(codi, pBuffer, nLength);	
}

void CSerialInterface::OnSend(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	CCoordinatorServer	*pServer;

	pServer = (CCoordinatorServer *)codi->pService;
	pServer->OnSendFrame(codi, pBuffer, nLength);	
}

void CSerialInterface::OnRecv(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	CCoordinatorServer	*pServer;

	pServer = (CCoordinatorServer *)codi->pService;
	pServer->OnReceiveFrame(codi, pBuffer, nLength);	
}

void CSerialInterface::OnError(COORDINATOR *codi, int nError)
{
	CCoordinatorServer	*pServer;

	pServer = (CCoordinatorServer *)codi->pService;
	pServer->OnError(codi, nError);	
}

////////////////////////////////////////////////////////////////////
// Serial override functions
//////////////////////////////////////////////////////////////////////

BOOL CSerialInterface::OnReceiveSession(WORKSESSION *pSession, char* pszBuffer, int nLength)
{
//	XDEBUG("OnReceiveSession(len=%d)\r\n", nLength);
//	XDUMP(pszBuffer, nLength, TRUE);

	OnRecv(m_selfCoordinator, (BYTE *)pszBuffer, nLength);
	return TRUE;
}

void CSerialInterface::OnPrevSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength)
{
//	XDEBUG("OnPrevSendSession(len=%d)\r\n", nLength);
//	XDUMP(pszBuffer, nLength, TRUE);

	OnPrevSend(m_selfCoordinator, (BYTE *)pszBuffer, nLength);	
}

void CSerialInterface::OnSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength)
{
//	XDEBUG("OnSendSession(len=%d)\r\n", nLength);
//	XDUMP(pszBuffer, nLength, TRUE);

	OnSend(m_selfCoordinator, (BYTE *)pszBuffer, nLength);	
}

void CSerialInterface::OnErrorSession(WORKSESSION *pSession, int nError)
{
//	XDEBUG("OnErrorSession(nError=%d)\r\n", nError);

	OnError(m_selfCoordinator, nError);	
}

