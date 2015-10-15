#ifndef __AIMIR_VERSION_H__
#define __AIMIR_VERSION_H__

#include "buildver.h"

#define MCU_TYPE_URBAN			0
#define MCU_TYPE_RURAL			1
#define MCU_TYPE_SLAVE			2
#define MCU_TYPE_INDOOR			3
#define MCU_TYPE_OUTDOOR		4

#define MCU_TYPE_HEADER_INDOOR      "INDOOR"
#define MCU_TYPE_HEADER_OUTDOOR     "OUTDOOR"

/*-- SW_VERSION, SW_VERSION_FIX는 실제 NAZC의 버전이 증가함에 따라서 증가한다.
 *-- UPDATE_VERSION_FIX의 경우 DM Agent를 위해서 배포판의 revision이 증가할 때
 *-- 같이 증가한다
 --*/
#define UPDATE_VERSION_FIX		"3.6"

#if     defined(__SUPPORT_NZC1__)
#define HW_VERSION_FIX			"NZC"
#define SW_VERSION_FIX			"3.0"
#elif   defined(__SUPPORT_NZC2__)
#define HW_VERSION_FIX			"NZC"
#define SW_VERSION_FIX			"3.1"
#elif   defined(__TI_AM335X__)
#define HW_VERSION_FIX			"NDC"
#define SW_VERSION_FIX			"4.2"
#else
#include <assert.h>
#define SW_VERSION_FIX			assert(0)
#endif

#define SW_PREFIX_TYPE			"AMM"

/*-- DM Agent를 위해서 HW Modle 정보를 넣는다
 --*/
#define HW_MODEL_INDOOR         "I211"
#define HW_MODEL_OUTDOOR        "O312"

#if     defined(__SUPPORT_NZC1__)
#define HW_VERSION				0x12
#define SW_VERSION				0x30
#elif   defined(__SUPPORT_NZC2__)
#define HW_VERSION				0x20
#define SW_VERSION				0x31
#elif   defined(__TI_AM335X__)
#define HW_VERSION				0x07
#define SW_VERSION				0x42
#else
#include <assert.h>
#define HW_VERSION				assert(0)
#define SW_VERSION				assert(0)
#endif


#define MODEL_TYPE_UNKNOWN		0
#define MODEL_TYPE_HTAMM		1

#define VENDOR_UNKNOWN			0
#define VENDOR_NURITELECOM		1

#endif	// __AIMIR_VERSION_H__
