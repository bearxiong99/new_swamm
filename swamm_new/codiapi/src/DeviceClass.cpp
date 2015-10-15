////////////////////////////////////////////////////////////////////
//
//  codiDeviceClass.cpp: implementation of the CDeviceClass class.
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
#include "DeviceClass.h"
#include "DebugUtil.h"

////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeviceClass::CDeviceClass()
{
	m_selfCoordinator = NULL;
}

CDeviceClass::~CDeviceClass()
{
}

////////////////////////////////////////////////////////////////////
// Coordinator functions
//////////////////////////////////////////////////////////////////////

int CDeviceClass::GetFD(COORDINATOR *codi)
{
	return 0;
}

int CDeviceClass::StartDevice(COORDINATOR *codi, CODIDEVICE *pDevice)
{
	XDEBUG("CDeviceClass: StartDevice\r\n");
	m_selfCoordinator = codi;
	codi->pInterface = (void *)this;
	return CODIERR_NOERROR;	
}

int CDeviceClass::StopDevice(COORDINATOR *codi)
{
	XDEBUG("CDeviceClass: StopDevice\r\n");
	return CODIERR_NOERROR;	
}

void CDeviceClass::SetPassive(COORDINATOR *codi, BOOL bPassive)
{
}

BOOL CDeviceClass::EnableFlowControl(COORDINATOR *codi, BOOL bEnable)
{
	return TRUE;
}

int CDeviceClass::Send(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
	return nLength;
}

void CDeviceClass::OnPrevSend(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
}

void CDeviceClass::OnSend(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
}

void CDeviceClass::OnRecv(COORDINATOR *codi, BYTE *pBuffer, int nLength)
{
}

void CDeviceClass::OnError(COORDINATOR *codi, int nError)
{
}

