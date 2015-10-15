//////////////////////////////////////////////////////////////////////
//
//	GPIOAPI.cpp: implementation of GPIOAPI
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
#include "GpioControl.h"
#include "GPIOAPI.h"

CGpioControl	*m_pGpioService = NULL;

//////////////////////////////////////////////////////////////////////
// CGPIOAPI Attribute
//////////////////////////////////////////////////////////////////////

BOOL GPIOAPI_IsEnableHooker()
{
	if (m_pGpioService == NULL)
		return FALSE;

	return m_pGpioService->IsEnableHooker();
}

BOOL GPIOAPI_EnableGpioHooker(BOOL bEnable)
{
	if (m_pGpioService == NULL)
		return FALSE;

	return m_pGpioService->EnableGpioHooker(bEnable);
}

int GPIOAPI_ReadGpio(int nPort)
{
	if (m_pGpioService == NULL)
		return 0;

	return m_pGpioService->ReadGpio(nPort);
}

BOOL GPIOAPI_WriteGpio(int nPort, int nValue)
{
	if (m_pGpioService == NULL)
		return FALSE;

	return m_pGpioService->WriteGpio(nPort, nValue);
}

//////////////////////////////////////////////////////////////////////
// CGPIOAPI Functions
//////////////////////////////////////////////////////////////////////

BOOL GPIOAPI_Initialize(int nMobileType)
{
	if (m_pGpioService != NULL)
		return FALSE;

	m_pGpioService = new CGpioControl;
	return m_pGpioService->Initialize(nMobileType);
}

BOOL GPIOAPI_Destroy()
{
	if (m_pGpioService == NULL)
		return FALSE;

	m_pGpioService->Destroy();
	delete m_pGpioService;
	m_pGpioService = NULL;
	return TRUE;
}

BOOL GPIOAPI_InstallHooker(PFNGPIOCALLBACK pfnCallback, void *pParam, void *pArgment)
{
	if (m_pGpioService == NULL)
		return FALSE;

	return m_pGpioService->InstallHooker(pfnCallback, pParam, pArgment);
}

BOOL GPIOAPI_UninstallHooker()
{
	if (m_pGpioService == NULL)
		return FALSE;

	return m_pGpioService->UninstallHooker();
}

