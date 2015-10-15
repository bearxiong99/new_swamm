//////////////////////////////////////////////////////////////////////
//
//	GPIOAPI.h: interface for GPIO.
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

#ifndef __GPIO_API_H__
#define __GPIO_API_H__

#include "gpiomap.h"

#define GPIONOTIFY_PWR					0x00000001
#define GPIONOTIFY_LOWBAT				0x00000002
#define GPIONOTIFY_HEATER				0x00000004
#define GPIONOTIFY_DOOR					0x00000008
#define GPIONOTIFY_BATTERY_CHARG		0x00000010
#define GPIONOTIFY_DCD					0x00000020
#define GPIONOTIFY_RING					0x00000040
#define GPIONOTIFY_BATTERY_FAIL			0x00000010
#define GPIONOTIFY_MEMORY				0x00001000
#define GPIONOTIFY_FLASH				0x00002000
#define GPIONOTIFY_MOBILE				0x00004000
#define GPIONOTIFY_RUNNING				0x00008000
#define GPIONOTIFY_TEMP					0x00000800
#define GPIONOTIFY_COORDINATOR			0x00000400

#define GPIONOTIFY_BATT_ADC             0x80000000
#define GPIONOTIFY_MAIN_ADC             0x40000000

#define GPIOERR_MASK					(GPIONOTIFY_PWR | GPIONOTIFY_LOWBAT | GPIONOTIFY_COORDINATOR |	\
										 GPIONOTIFY_DOOR | GPIONOTIFY_BATTERY_FAIL | \
                                         GPIONOTIFY_MEMORY | GPIONOTIFY_FLASH)

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*PFNUSERHOOKER)(int nType, int nValue);
typedef void (*PFNGPIOCALLBACK)(int nNotify, int nState, void *pParam, void *pArgment);

BOOL GPIOAPI_Initialize(BOOL bInitialize, const char* nMobileType, BOOL bMonitoring);
BOOL GPIOAPI_Destroy();

BOOL GPIOAPI_InstallHooker(PFNGPIOCALLBACK pfnCallback, void *pParam, void *pArgment);
BOOL GPIOAPI_UninstallHooker();

BOOL GPIOAPI_IsEnableHooker();
BOOL GPIOAPI_EnableGpioHooker(BOOL bEnable);

int	GPIOAPI_ReadGpio(int nPort);
BOOL GPIOAPI_WriteGpio(int nPort, int nValue);
unsigned int	GPIOAPI_ReadAdc(int nPort);

#ifdef __cplusplus
}
#endif

#endif // __GPIO_API_H__
