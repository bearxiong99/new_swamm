#ifndef __GPIOMAP_H__
#define __GPIOMAP_H__

#include "gpio.h"

#ifdef __TI_AM335X__
/* ADC */ 
#define ADC_BATT_VOL_INPUT		    0		// Battery Voltage ADC Signal
#define ADC_MAIN_VOL_INPUT		    1		// Main voltage ADC Signal

/* input signal */
#define GP_GSM_SYNC_INPUT			20		// MC55 모듈의 SYNC 신호 h -> l
#define GP_LOW_BATT_INPUT			29		// Low Battery 신호
#define GP_CASE_OPEN_INPUT          60		// Case Open (Low : Close, High Open)
#define GP_PWR_FAIL_INPUT			63		// Power Fail 신호
#define GP_BATT_CHG_STATUS_INPUT	64		// 배터리의 충전 상태 신호 (L:충전중)

/* output signal */
#define GP_STA_GREEN_OUT			65		// TOP 케이스의 STA LED (GREEN)
#define GP_CODI_PCTL_OUT			86		// CORDI 모듈의 POWER ON/OFF
#define GP_NPLC_PCTL_OUT			87		// NPLC's Power On/Off - new add
#define GP_GSM_PCTL_OUT				88		// GSM 모듈의 POWER ON/OFF
#define GP_NPLC_RST_OUT 			89		// NPLC RESET - new add
#define GP_SW_RST_OUT				96		// 소프트웨어 RESET 신호 (사용하지 않는다 )
#define GP_DEBUG_LED3_OUT           97		// Debug LED #3
#define GP_CODI_RST_OUT				98		// Coordinator 모듈의 RESET
#define GP_GSM_RST_OUT				110 	// GSM's RESET 신호 - new add
#define GP_RF_SIG_OUT				111		// RF - subGiga for reserved  - new add

#define GP_BATT_SW_OUT				112		// 배터리의 메인 소스 입력 릴레이 ON/OFF
#define GP_GSM_INIT_PCTL_OUT		113		// GSM init Start On ( 30ms ) , then it's Off - new add
#define GP_485_TX_EN_OUT			114		// 485 transceiver enable - new add
#define GP_BATT_CHG_EN_OUT			115		// 배터리 충전 Enable
#define GP_DEBUG_LED1_OUT			116		// DEBUG LED1 신호
#define GP_DEBUG_LED2_OUT			117		// DEBUG LED2 신호

#else // PXA 255 GPIO 
/* GPIO Input */

#define GP_GSM_INT_INPUT			0		// GSM Interupt 신호
#define GP_ZB_INT_INPUT				1		// ZigBee Interupt 신호
#define GP_UART_INT0_INPUT			7		// 16C2550의 UART0 IRQ신호
#define GP_TEMP_OS_INPUT			14		// 히터 동작 상태 신호
#define GP_DOOR_OPEN_INPUT			17		// DOOR Open 신호
#define GP_PWR_FAIL_INPUT			19		// Power Fail 신호
#define GP_INT_ETH_INPUT			21		// CS8900의 IRQ 신호 
#define GP_UART_INT1_INPUT			22		// 16C2550의 UART1 IRQ신호 (미사용)
#define GP_SSP_RXD_INPUT			26		// SPI 기능 (미사용)
#define GP_SSP_EXTCLK_INPUT			27		// SPI 기능 (미사용)
#define GP_LOW_BATT_INPUT			31		// Low Battery 신호
#define GP_GSM_SYNC_INPUT			32		// MC55 모듈의 SYNC 신호
#define GP_GSM_TXD_INPUT			34		// MC55 모듈의 TXD 신호
#define GP_GSM_RTS_INPUT			35		// MC55 모듈의 RTS 신호
#define GP_GSM_DCD_INPUT			36		// MC55 모듈의 DCD 신호
#define GP_GSM_DSR_INPUT			37		// MC55 모듈의 DSR 신호
#define GP_GSM_RI_INPUT				38		// MC55 모듈의 RI 신호
#define GP_CODI_TXD_INPUT			42		// Coordinator 모듈의 TXD 신호
#define GP_CODI_RTS_INPUT			44		// Coordinator 모듈의 RTS 신호
#define GP_CONSOLE_TXD_INPUT		46		// CONSOLE용 TXD 신호
#define GP_GPS_TXD_INPUT			49		// GPS의 TXD 신호
#define GP_EXT_RTS_INPUT			50		// EXTERNAL의 RTS 신호
#define GP_BATT_CHG_STATUS_INPUT	74		// 배터리의 충전 상태 신호 (L:충전중)

/* GPIO Output */

#define GP_DEBUG_LED1_OUT			2		// DEBUG LED1 신호
#define GP_DEBUG_LED2_OUT			3		// DEBUG LED2 신호
#define GP_DEBUG_LED3_OUT			4		// DEBUG LED3 신호
#define GP_DEBUG_LED4_OUT			5		// DEBUG LED4 신호

#if     defined(__SUPPORT_NZC1__)
#define GP_CODI_PCTL_OUT			6		// CORDI 모듈의 POWER ON/OFF
#endif

#if     defined(__SUPPORT_NZC2__)
#define GP_BATT_SW_OUT				62		// 배터리의 메인 소스 입력 릴레이 ON/OFF
#elif   defined(__SUPPORT_NZC1__)  
#define GP_BATT_SW_OUT				8		// 배터리의 메인 소스 입력 릴레이 ON/OFF
#endif

#define GP_GSM_PCTL_OUT				11		// GSM 모듈의 POWER ON/OFF
#define GP_STA_RED_OUT				12		// TOP 케이스의 STA LED (RED)
#define GP_STA_GREEN_OUT			13		// TOP 케이스의 STA LED (GREEN)
#define GP_WDI_OUT					16		// Watch-Dog IC Clear 신호
#define GP_SSP_SCLK_OUT				23		// SPI 기능 (미사용)
#define GP_SSP_SFRM_OUT				24		// SPI 기능 (미사용)
#define GP_SSP_TXD_OUT				25		// SPI 기능 (미사용)
#define GP_LED_READY_OUT			29		// LED READY 신호 (미사용)
#define GP_GSM_IGT_OUT				30		// MC55 모듈의 IGT 신호 
#define GP_GSM_RXD_OUT				39		// MC55 모듈의 RXD 신호
#define GP_GSM_DTR_OUT				40		// MC55 모듈의 DTR 신호
#define GP_GSM_CTS_OUT				41		// MC55 모듈의 CTS 신호
#define GP_CODI_RXD_OUT				43		// CODI 모듈의 RXD 신호
#define GP_CODI_CTS_OUT				45		// CODI 모듈의 CTS 신호
#define GP_CONSOLE_RXD_OUT			47		// CONSOLE용 RXD 신호
#define GP_GPS_RXD_OUT				48		// GPS의 RXD 신호
#define GP_EXT_CTS_OUT				51		// EXTERNAL의 CTS 신호
#define GP_CODI_RST_OUT				56		// Coordinator 모듈의 RESET
#define GP_BATT_CHG_EN_OUT			75		// 배터리 충전 Enable
#define GP_SW_RST_OUT				76		// 소프트웨어 RESET 신호
#define GP_NAND_CE_OUT				81		// NAND FLASH Chip Enable
#define GP_NAND_CLE_OUT				82		// NAND FLASH Command 신호
#define GP_NAND_ALE_OUT				83		// NAND FLASH Address 신호
#define GP_GSM_EMERGOFF_OUT			84		// MC55 모듈의 EMERGOFF 신호 (Active > 3.2s)
#endif
#endif	// __GPIOMAP_H__
