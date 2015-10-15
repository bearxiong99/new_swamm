//-------------------------------------------------------------------------------
// 화   일 : gpio.h
// 설   명 : PXA255 의 단순한 gpio 입출력을 담당한다.
// 작   성 : freefrug@falinux.com
// 날   짜 : 2007-04-20 ~
//
//-------------------------------------------------------------------------------

#ifndef __GPIO_H__
#define __GPIO_H__

/* ioctl 함수의 command -------------------------------------------------------*/

#ifdef __TI_AM335X__
#define GPIO_DIR_IN             0x00
#define GPIO_DIR_OUT_LOW        0x01
#define GPIO_DIR_OUT_HIGH       0x02
#define GPIO_LOW                0x00
#define GPIO_HIGH               0x01
#define GPIO_ON                 0x00 
#define ADC_ON                  0x01

#else

/* 모듈관련 -------------------------------------------------------------------*/
#define GPIO_MAJOR_DEF        253

#if     defined(__SUPPORT_NZC2__)
#define GPIO_NAME             "NZC2.xgpio"
#elif   defined(__SUPPORT_NZC1__)
#define GPIO_NAME             "NZC1.xgpio"
#else       // 정의되지 않았을 때는 에러 발생
#include <assert.h>
#define GPIO_NAME               assert(0)
#endif

#define GPIO_VERSION            GPIO_NAME " Ver 1.1"

#define MAGIC_GPIO              'g'
#define GPIO_IOCTL_DIR          _IOW( MAGIC_GPIO, 80, long )
#define GPIO_IOCTL_IN           _IOR( MAGIC_GPIO, 81, long )
#define GPIO_IOCTL_OUT          _IOW( MAGIC_GPIO, 82, long )

#define GPIO_DIR_IN             0x0000
#define GPIO_DIR_OUT            0x8000
#define GPIO_LOW                0x0000
#define GPIO_HIGH               0x4000

#define GPIO_CTL_MASK           0xff00
#define GPIO_NUM_MASK           0x007f

#define get_gpio_num(x)         ((((x) & GPIO_NUM_MASK) <= 84 ) ? ((x) & GPIO_NUM_MASK) : -1)  
#define get_direction(x)        (((x) & GPIO_DIR_OUT) ? GPIO_OUT : GPIO_IN)
#define is_high_level(x)        ((x) & GPIO_HIGH )

#define GPIOSET(x)              (x | GPIO_DIR_OUT)
#define GPIOHIGH(x)             (x | GPIO_HIGH)
#define GPIOLOW(x)              x

/* GPIO Iutput Setting */
#define EXT_nRST            POWER_RESET    // Power reset by Watchdog, CDMA_RESET_OUT, RST SW  
#endif


typedef struct    _tagGPIOINTENTRY
{
        int        gpio_nr;
        int        edge_mask;
}    GPIOINTENTRY;

#endif  // __GPIO_H__

