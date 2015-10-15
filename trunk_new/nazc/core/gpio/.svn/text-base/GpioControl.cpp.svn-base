//////////////////////////////////////////////////////////////////////
//
//    GpioControl.cpp: implementation of the CGpioControl class.
//
//    This file is a part of the AIMIR-GCP.
//    (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//    This source code can only be used under the terms and conditions 
//    outlined in the accompanying license agreement.
//
//    casir@com.ne.kr
//    http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "GPIOAPI.h"
#include "GpioControl.h"
#include "DebugUtil.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "mcudef.h"
#include <sys/epoll.h>
#include <string.h>

#define INPUT_GPIO_MAX        7
//#define __TI_AM335X__  2

/** GPIO 모드는 2가지를 안고 간다. 
 * 1. 기존의 PXA255 에 쓰이는 GPIO Control ioctl 
 * 2. TI Board 에 쓰일 GPIO Contorl epoll 2가지 모드를 가지고 가는 이유는 
 * GPIO Control 하는 방식이 다르기 때문에 1가지 소스에서 define 에 따라 다르기 동작할수 있는 
 * 알고리즘을 넣어주고 Make 파일에서 GPIO Define을 설정함으로써 사용할수 있게 구현하였다. **/

#ifdef __TI_AM335X__
#include <stdio.h>         // puts()
#include <fcntl.h>         // O_WRONLY
#include <unistd.h>        // write(), close()
#include "gpio_am335x.h"
#endif 
//////////////////////////////////////////////////////////////////////
// CGpioControl Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGpioControl::CGpioControl()
{
    m_pMobileType    = NULL;
    m_nFD            = -1;
    m_pParam        = NULL;
    m_pArgment        = NULL;
    m_pfnCallback    = NULL;
    m_pfnUserHooker    = NULL;

    m_bExitPending    = FALSE;
    m_bEnableHooker    = TRUE;
}

CGpioControl::~CGpioControl()
{
    if(m_pMobileType != NULL) free(m_pMobileType);
}

//////////////////////////////////////////////////////////////////////
// CGpioControl Attribute
//////////////////////////////////////////////////////////////////////

BOOL CGpioControl::IsEnableHooker()
{
    return m_bEnableHooker;
}

int CGpioControl::ReadGpio(int nPort)
{
    int        nValue=0;

#ifndef __TI_AM335X__
    if (m_nFD == -1)
        return 0;
#endif

    m_gpioLocker.Lock();
#ifdef __TI_AM335X__
    gpio_get_value(nPort , (unsigned int*)&nValue ) ;
#else
    nValue = ioctl(m_nFD, GPIO_IOCTL_IN, nPort);
#endif

    m_gpioLocker.Unlock();
    return nValue;
}

BOOL CGpioControl::WriteGpio(int nPort, int nValue)
{ 
    int res=0;

#ifndef __TI_AM335X__
    if (m_nFD == -1)
        return FALSE;
#endif

    m_gpioLocker.Lock();

#ifdef __TI_AM335X__
    res = gpio_set_value(nPort , (nValue > 0) ? GPIO_HIGH : GPIO_LOW);
#else
    ioctl(m_nFD, GPIO_IOCTL_OUT, (nValue > 0) ? GPIOHIGH(nPort) : GPIOLOW(nPort));
#endif 

    m_gpioLocker.Unlock();
    return !res ? TRUE : FALSE;
}

unsigned int CGpioControl::ReadAdc(int nPort)
{
#ifdef __TI_AM335X__
    unsigned int  nValue=0;

    m_gpioLocker.Lock();
    adc_get_value(nPort , &nValue ) ;
    m_gpioLocker.Unlock();

    return nValue;
#else
    return 0;
#endif
}

//////////////////////////////////////////////////////////////////////
// CGpioControl initialize/destroy
//////////////////////////////////////////////////////////////////////

/** GPIO 초기화.
 *
 * @param bInitialize 초기화 여부 (다수의 Process에서 호출할 경우 최초 1회만 초기화 하면 된다)
 * @param nMobileType MOBILE_TYPE_CDMA, MOBILE_TYPE_GSM
 */
BOOL CGpioControl::Initialize(BOOL bInitialize, const char* pMobileType, BOOL bMonitoring)
{
    if(pMobileType != NULL && strlen(pMobileType)>0)
    {
        m_pMobileType = (char*)MALLOC(strlen(pMobileType)+1);
        strcpy(m_pMobileType, pMobileType);
    }

    /** 초기화기 필요 없으면 바로 Return 한다 */
    if(bInitialize == FALSE) return TRUE;

#ifdef __TI_AM335X__
/** GPIO Epoll 추가 **/
    m_nEpfd = epoll_create(INPUT_GPIO_MAX);

    /*---- INPUT ----*/
    XDEBUG("** GPIO Initialize Start\r\n");
    XDEBUG(" GP_GSM_SYNC_INPUT ................ %d\r\n", GP_GSM_SYNC_INPUT);
    gpio_export(GP_GSM_SYNC_INPUT);
    if(!gpio_dir_in(GP_GSM_SYNC_INPUT))
    {
        if(bMonitoring) EpollAdd(GP_GSM_SYNC_INPUT, 1, GPIO_ON); // GPIO 번호, 초기값, GPIO State( gpio, ADC )  
    }

    XDEBUG(" GP_LOW_BATT_INPUT ................ %d\r\n", GP_LOW_BATT_INPUT);
    gpio_export(GP_LOW_BATT_INPUT);
    gpio_dir_in(GP_LOW_BATT_INPUT);
    if(bMonitoring) EpollAdd(GP_LOW_BATT_INPUT,1,GPIO_ON);

    XDEBUG(" GP_CASE_OPEN_INPUT ............... %d\r\n", GP_CASE_OPEN_INPUT); 
    gpio_export(GP_CASE_OPEN_INPUT);
    gpio_dir_in(GP_CASE_OPEN_INPUT);
    if(bMonitoring) EpollAdd(GP_CASE_OPEN_INPUT,0,GPIO_ON);

    XDEBUG(" GP_PWR_FAIL_INPUT ................ %d\r\n", GP_PWR_FAIL_INPUT);
    gpio_export(GP_PWR_FAIL_INPUT);
    gpio_dir_in(GP_PWR_FAIL_INPUT);
    if(bMonitoring) EpollAdd(GP_PWR_FAIL_INPUT,0,GPIO_ON);

    XDEBUG(" GP_BATT_CHG_STATUS_INPUT ......... %d\r\n", GP_BATT_CHG_STATUS_INPUT); 
    gpio_export(GP_BATT_CHG_STATUS_INPUT);
    gpio_dir_in(GP_BATT_CHG_STATUS_INPUT);
    if(bMonitoring) EpollAdd(GP_BATT_CHG_STATUS_INPUT,0,GPIO_ON); 

    if(bMonitoring) EpollAdd(ADC_BATT_VOL_INPUT,4095,ADC_ON);  // GPIO 번호, 초기값, GPIO State( gpio, ADC )   
    if(bMonitoring) EpollAdd(ADC_MAIN_VOL_INPUT,4040,ADC_ON); 
    
    /*---- OUTPUT ----*/
#if 0
    /** GPIO 중 SW Reset은 사용하지 않는다 */
    XDEBUG(" GP_SW_RESET_OUT ....... HIGH ..... %d\r\n", GP_SW_RESET_OUT);
    gpio_export(GP_SW_RESET_OUT); 
    gpio_dir_out(GP_SW_RESET_OUT , GPIO_DIR_OUT_HIGH);
    gpio_set_value(GP_SW_RESET_OUT , GPIO_HIGH );
#endif

    XDEBUG(" GP_STA_GREEN_OUT ...... LOW  ..... %d\r\n", GP_STA_GREEN_OUT);
    gpio_export(GP_STA_GREEN_OUT);
    gpio_dir_out(GP_STA_GREEN_OUT , GPIO_DIR_OUT_LOW); //GPIO_DIR_OUT
    gpio_set_value(GP_STA_GREEN_OUT , GPIO_LOW );

    XDEBUG(" GP_CODI_PCTL_OUT ...... HIGH ..... %d\r\n", GP_CODI_PCTL_OUT);
    gpio_export(GP_CODI_PCTL_OUT);
    gpio_dir_out(GP_CODI_PCTL_OUT , GPIO_DIR_OUT_HIGH);
    gpio_set_value(GP_CODI_PCTL_OUT , GPIO_HIGH );

    XDEBUG(" GP_CODI_RST_OUT ....... HIGH ..... %d\r\n", GP_CODI_RST_OUT);
    gpio_export(GP_CODI_RST_OUT);
    gpio_dir_out(GP_CODI_RST_OUT , GPIO_DIR_OUT_HIGH);
    gpio_set_value(GP_CODI_RST_OUT , GPIO_HIGH );

    XDEBUG(" GP_NPLC_PCTL_OUT ...... HIGH ..... %d\r\n", GP_NPLC_PCTL_OUT);
    gpio_export(GP_NPLC_PCTL_OUT);
    gpio_dir_out(GP_NPLC_PCTL_OUT , GPIO_DIR_OUT_HIGH);
    gpio_set_value(GP_NPLC_PCTL_OUT , GPIO_HIGH );

    XDEBUG(" GP_NPLC_RST_OUT ....... HIGH ..... %d\r\n", GP_NPLC_RST_OUT);
    gpio_export(GP_NPLC_RST_OUT);
    gpio_dir_out(GP_NPLC_RST_OUT , GPIO_DIR_OUT_HIGH);
    gpio_set_value(GP_NPLC_RST_OUT , GPIO_HIGH );

    /** Mobile Module Power Setting.
     *
     * AMTelecom Module의 경우 GSM_PCTL_OUT 일 Low로 설정하여 전원을 Off 시킨 후
     * GSM_INIT_PCTL_OUT 값을 조정해 주어야 한다.
     */
    gpio_export(GP_GSM_PCTL_OUT);
    XDEBUG(" GP_GSM_PCTL_OUT ....... HIGH ..... %d\r\n", GP_GSM_PCTL_OUT);
    gpio_dir_out(GP_GSM_PCTL_OUT , GPIO_DIR_OUT_HIGH); //GPIO_DIR_OUT
    gpio_set_value(GP_GSM_PCTL_OUT, GPIO_HIGH);


    XDEBUG(" GP_GSM_RST_OUT ........ LOW  ..... %d\r\n", GP_GSM_RST_OUT);
    gpio_export(GP_GSM_RST_OUT);
    gpio_dir_out(GP_GSM_RST_OUT , GPIO_DIR_OUT_LOW); //GPIO_DIR_OUT
    gpio_set_value(GP_GSM_RST_OUT , GPIO_LOW );

    gpio_export(GP_GSM_INIT_PCTL_OUT);
    if(m_pMobileType != NULL)
    {
        /** TI AM335x는 DTSS800, MC55 를 지원하지 않는다 */
        if(!strcasecmp(MOBILE_MODULE_TYPE_AME5210, m_pMobileType))
        {
            // AM Telecom
            XDEBUG(" GP_GSM_INIT_PCTL_OUT .. LOW  ..... %d\r\n", GP_GSM_INIT_PCTL_OUT);
            gpio_dir_out(GP_GSM_INIT_PCTL_OUT , GPIO_DIR_OUT_LOW); //GPIO_DIR_OUT
            XDEBUG(" GP_GSM_PCTL_OUT ....... HIGH ..... %d\r\n", GP_GSM_PCTL_OUT);
            gpio_dir_out(GP_GSM_PCTL_OUT , GPIO_DIR_OUT_HIGH); //GPIO_DIR_OUT
#if 0
            gpio_set_value(GP_GSM_PCTL_OUT, GPIO_HIGH);
#endif
        }
        else if(!strcasecmp(MOBILE_MODULE_TYPE_GE910, m_pMobileType) ||
                !strcasecmp(MOBILE_MODULE_TYPE_UE910, m_pMobileType))
        {
            XDEBUG(" GP_GSM_INIT_PCTL_OUT .. LOW  ..... %d\r\n", GP_GSM_INIT_PCTL_OUT);
            gpio_dir_out(GP_GSM_INIT_PCTL_OUT , GPIO_DIR_OUT_LOW); //GPIO_DIR_OUT
#if 0
            XDEBUG(" GP_GSM_INIT_PCTL_OUT .. HIGH ..... %d\r\n", GP_GSM_INIT_PCTL_OUT);
            gpio_set_value(GP_GSM_INIT_PCTL_OUT, GPIO_HIGH);
            usleep(5000000);
            XDEBUG(" GP_GSM_INIT_PCTL_OUT .. LOW  ..... %d\r\n", GP_GSM_INIT_PCTL_OUT);
            gpio_set_value(GP_GSM_INIT_PCTL_OUT, GPIO_LOW);
#endif
        }
    }

    XDEBUG(" GP_RF_SIG_OUT ......... LOW  ..... %d\r\n", GP_RF_SIG_OUT);
    gpio_export(GP_RF_SIG_OUT);
    gpio_dir_out(GP_RF_SIG_OUT , GPIO_DIR_OUT_LOW); // //GPIO_DIR_OUT
    gpio_set_value(GP_RF_SIG_OUT , GPIO_LOW );

    XDEBUG(" GP_BATT_CHG_EN_OUT .... LOW  ..... %d\r\n", GP_BATT_CHG_EN_OUT); 
    gpio_export(GP_BATT_CHG_EN_OUT);
    gpio_dir_out(GP_BATT_CHG_EN_OUT , GPIO_DIR_OUT_LOW); //GPIO_DIR_OUT

    // Battery Charging Reset
    gpio_set_value(GP_BATT_CHG_EN_OUT , GPIO_HIGH ); USLEEP(500000);
    gpio_set_value(GP_BATT_CHG_EN_OUT , GPIO_LOW );

    
    XDEBUG(" GP_BATT_SW_OUT ........ HIGH ..... %d\r\n", GP_BATT_SW_OUT);
    gpio_export(GP_BATT_SW_OUT);
    gpio_dir_out(GP_BATT_SW_OUT , GPIO_DIR_OUT_HIGH); //
    gpio_set_value(GP_BATT_SW_OUT , GPIO_HIGH );
    
    XDEBUG(" GP_485_TX_EN_OUT ...... HIGH ..... %d\r\n", GP_485_TX_EN_OUT);
    gpio_export(GP_485_TX_EN_OUT);
    if(!gpio_dir_out(GP_485_TX_EN_OUT , GPIO_DIR_OUT_HIGH))
    {
        gpio_set_value(GP_485_TX_EN_OUT , GPIO_HIGH );
    }
    
    XDEBUG(" GP_DEBUG_LED1_OUT ..... LOW  ..... %d\r\n", GP_DEBUG_LED1_OUT);
    gpio_export(GP_DEBUG_LED1_OUT);
    gpio_dir_out(GP_DEBUG_LED1_OUT , GPIO_DIR_OUT_LOW); //
    gpio_set_value(GP_DEBUG_LED1_OUT , GPIO_LOW );
    
    XDEBUG(" GP_DEBUG_LED2_OUT ..... LOW  ..... %d\r\n", GP_DEBUG_LED2_OUT);
    gpio_export(GP_DEBUG_LED2_OUT);
    gpio_dir_out(GP_DEBUG_LED2_OUT , GPIO_DIR_OUT_LOW); //
    gpio_set_value(GP_DEBUG_LED2_OUT , GPIO_LOW );
    
    XDEBUG(" GP_DEBUG_LED3_OUT ..... LOW  ..... %d\r\n", GP_DEBUG_LED3_OUT);
    gpio_export(GP_DEBUG_LED3_OUT);
    gpio_dir_out(GP_DEBUG_LED3_OUT , GPIO_DIR_OUT_LOW); //
    gpio_set_value(GP_DEBUG_LED3_OUT , GPIO_LOW );
    
    XDEBUG("** GPIO Initialize End\r\n");
#else
    if (m_nFD != -1)
        return FALSE;

    m_nFD = open("/dev/gpio", O_RDWR | O_NDELAY);
    if (m_nFD < 0)
    {
           printf(" -------- GPIO Devive Open Fail -------\n");
        m_nFD = -1;
        return FALSE;
    }

    // GPIO 입력 다이렉션
    ioctl(m_nFD, GPIO_IOCTL_DIR, GP_PWR_FAIL_INPUT);                    // PWR FAIL
    ioctl(m_nFD, GPIO_IOCTL_DIR, GP_LOW_BATT_INPUT);                    // LOW BATTERY
    ioctl(m_nFD, GPIO_IOCTL_DIR, GP_GSM_DCD_INPUT);                        // MOBILE DCD
    ioctl(m_nFD, GPIO_IOCTL_DIR, GP_GSM_SYNC_INPUT);                    // MOBILE SYNC
    ioctl(m_nFD, GPIO_IOCTL_DIR, GP_GSM_RI_INPUT);                        // MOBILE RI
    ioctl(m_nFD, GPIO_IOCTL_DIR, GP_DOOR_OPEN_INPUT);                    // DOOR OPEN/CLOSE
    ioctl(m_nFD, GPIO_IOCTL_DIR, GP_TEMP_OS_INPUT);                        // HEATER ON/OFF
    ioctl(m_nFD, GPIO_IOCTL_DIR, GP_BATT_CHG_STATUS_INPUT);                // BATT CHG STS

    // GPIO 포트 초기값 설정
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_SW_RST_OUT));                // PWR RESET
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_CODI_RST_OUT));            // COORDINATOR RESET

    // GPIO Output Direction Setting
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_SW_RST_OUT));                // SW RESET
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_CODI_RST_OUT));                // COORDINATOR RESET
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_BATT_SW_OUT));                // PWR CTRL
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_BATT_CHG_EN_OUT));            // BATT CHG EN
    if(m_pMobileType != NULL)
    {
        /** PXA는 GE910, UE910 를 지원하지 않는다 */
        if(!strcasecmp(MOBILE_MODULE_TYPE_DTSS800, m_pMobileType))
        {
            ioctl(m_nFD, GPIO_IOCTL_DIR, GP_GSM_IGT_OUT);                   // MOBILE RESET (IGT)
        }
        else if(!strcasecmp(MOBILE_MODULE_TYPE_MC55, m_pMobileType))
        {
            ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_GSM_EMERGOFF_OUT));        // EMERGENCY OFF
            ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_GSM_IGT_OUT));            // MOBILE RESET (IGT)
        }
    }
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_GSM_PCTL_OUT));                // MOBILE POWER
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_GSM_DTR_OUT));                // MOBILE DTR

    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_LED_READY_OUT));            // LED READY
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_STA_GREEN_OUT));            // STATUS GREEN LED
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_STA_RED_OUT));                // STATUS RED LED
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_DEBUG_LED1_OUT));            // LED DEBUG1
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_DEBUG_LED2_OUT));            // LED DEBUG2
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_DEBUG_LED3_OUT));            // LED DEBUG3
    ioctl(m_nFD, GPIO_IOCTL_DIR, GPIOSET(GP_DEBUG_LED4_OUT));            // LED DEBUG4

    // 신규 보드는 HIGH, 영국, 스웨덴은 LOW, 구형은 HIGH
#if     defined(__SUPPORT_NZC1__)
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_BATT_SW_OUT));                // BATTERY ENABLE/DISABLE CONTROL
#elif   defined(__SUPPORT_NZC2__)
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_BATT_SW_OUT));                // BATTERY ENABLE/DISABLE CONTROL
#else
    assert(0);
#endif

    if(m_pMobileType != NULL && !strcasecmp(MOBILE_MODULE_TYPE_MC55, m_pMobileType))
    {
        ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_EMERGOFF_OUT));        // GSM RESET
        ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_IGT_OUT));            // GSM IGT 
    }

    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_LED_READY_OUT));            // READY LED OFF
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_STA_GREEN_OUT));            // GREEN LED OFF
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_STA_RED_OUT));                // RED LED OFF

    XDEBUG("** Mobile Power OFF\xd\xa");
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_BATT_CHG_EN_OUT));            // BATT CHARGE DISABLE
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_PCTL_OUT));                // GSM Power Off

    usleep(1000000);

    XDEBUG("** Mobile Power ON\xd\xa");
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_BATT_CHG_EN_OUT));            // BATT CHARGE ENABLE
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_GSM_PCTL_OUT));

    usleep(1000000);

    if(m_pMobileType != NULL && !strcasecmp(MOBILE_MODULE_TYPE_MC55, m_pMobileType))
    {
        XDEBUG("** Mobile IGT or Reset\xd\xa");
        ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_GSM_IGT_OUT));            // IGT HIGH
        usleep(1000000);        
        ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_IGT_OUT));            // IGT LOW
        usleep(1000000);        
        ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_GSM_IGT_OUT));            // IGT HIGH
    }

    XDEBUG("** Mobile DTR Control\xd\xa");
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOHIGH(GP_GSM_DTR_OUT));                // DTR HIGH
    usleep(1000000);        
    ioctl(m_nFD, GPIO_IOCTL_OUT, GPIOLOW(GP_GSM_DTR_OUT));                // DTR LOW

#endif

    /** Monitoring이 필요 없으면 바로 Return 한다 */
    if(bMonitoring == FALSE) return TRUE;
   
    // 상태 모니터링 쓰레드 생성
    m_bExitPending = FALSE;
    if (pthread_create(&m_gpioThreadID, NULL, gpioWatchThread, (void *)this) != 0)
        return FALSE;

    pthread_detach(m_gpioThreadID);
    return TRUE;
}

#ifdef __TI_AM335X__
void CGpioControl::EpollAdd(unsigned nNum, unsigned int nValue , unsigned int nState )
{
    struct epoll_event ev;
    int fd;
    GPIOENTRY     *pGpio;
#if 0
    char buff[64];
#endif

    fd = open_value_file(nNum, nState); // GPIO file value open 
    pGpio = (GPIOENTRY *)malloc(sizeof(GPIOENTRY));
    memset(pGpio, 0, sizeof(GPIOENTRY));
    pGpio->gpio = nNum;
    pGpio->fd = fd;
    pGpio->nValue = nValue;        
    
    if(nState == GPIO_ON)
    {
        switch(nNum)
        {
            case GP_GSM_SYNC_INPUT :
            break;
            case GP_LOW_BATT_INPUT :
                pGpio->mask = GPIONOTIFY_LOWBAT;
            break;
            case GP_CASE_OPEN_INPUT :
                pGpio->mask = GPIONOTIFY_DOOR;
            break;
            case GP_PWR_FAIL_INPUT :
                pGpio->mask = GPIONOTIFY_PWR;
            break;
            case GP_BATT_CHG_STATUS_INPUT :
                pGpio->mask = GPIONOTIFY_BATTERY_CHARG;
            break;
        }
    }
    else
    {
        switch(nNum)
        {
            case ADC_BATT_VOL_INPUT:
                pGpio->mask = GPIONOTIFY_BATT_ADC;
                break;
            case ADC_MAIN_VOL_INPUT:
                pGpio->mask = GPIONOTIFY_MAIN_ADC;
                break;
        }
    }

    memset(&ev, 0, sizeof(struct epoll_event)); 
    ev.events = EPOLLIN | EPOLLPRI;
    ev.data.fd = fd;
    ev.data.ptr = pGpio;
    epoll_ctl(m_nEpfd, EPOLL_CTL_ADD, fd, &ev);
}
    
#if 0
int    CGpioControl::ADCValueCheck(int ValCheckNum, int AdcNum, int AdcValue )
{
    int ResultNum;
    ResultNum = ValCheckNum;
    //printf( "ADCValue = %d  , ADC NUM = %d \n", AdcValue ,AdcNum); 
    switch( AdcNum )
    {
        case 0: // Battert AD
            if( AdcValue > 3938 ) ResultNum = 0; 
            else if ( (AdcValue > 3628) && (AdcValue < 3938) ) ResultNum = 1; 
            else if ( (AdcValue > 3248) && (AdcValue < 3628) ) ResultNum = 2; 
            else if ( (AdcValue > 2911) && (AdcValue < 3248) ) ResultNum = 3; 
            else if ( (AdcValue > 2634) && (AdcValue < 2911) ) ResultNum = 4; 
            else ResultNum = 5; 
            break;
        case 1: // Power fail 
            if( AdcValue > 3769 ) ResultNum = 0; 
        //    else if ( AdcValue < 3938 && AdcValue > 3628 ) ResultNum = 1; 
        //    else if ( AdcValue < 3628 && AdcValue > 3248 ) ResultNum = 2; 
        //    else if ( AdcValue < 3248 && AdcValue > 2911 ) ResultNum = 3; 
        //    else if ( AdcValue < 2911 && AdcValue > 2634 ) ResultNum = 4; 
            else ResultNum = 5; 
            break;
        case 2: // Low Battery 
            if( AdcValue > 3938 ) ResultNum = 0; 
            else ResultNum = 5;            
            break;
    }
    return ResultNum;
}
#endif

#endif

BOOL CGpioControl::Destroy()
{
    void *    nStatus;

    UninstallHooker();

#ifndef __TI_AM335X__
    m_bExitPending  = TRUE;
    close(m_nEpfd);
#else
    if (m_nFD == -1)
        return FALSE;

    m_bExitPending  = TRUE;
    close(m_nFD);
#endif 

    pthread_join(m_gpioThreadID, &nStatus);

    m_nFD = -1;
    m_nEpfd = -1 ;

    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CGpioControl functions
//////////////////////////////////////////////////////////////////////

BOOL CGpioControl::InstallHooker(PFNGPIOCALLBACK pfnCallback, void *pParam, void *pArgment)
{
    if (pfnCallback == NULL)
        return FALSE;

    m_Locker.Lock();
    m_pfnCallback    = pfnCallback;
    m_pParam        = pParam;
    m_pArgment        = pArgment;
    m_Locker.Unlock();
    return TRUE;
}

BOOL CGpioControl::UninstallHooker()
{
    if (m_pfnCallback == NULL)
        return FALSE;

    m_Locker.Lock();
    m_pfnCallback    = NULL;
    m_pParam        = NULL;
    m_pArgment        = NULL;
    m_Locker.Unlock();
    return TRUE;
}

BOOL CGpioControl::EnableGpioHooker(BOOL bEnable)
{
    m_Locker.Lock();
    m_bEnableHooker = bEnable;
    m_Locker.Unlock();
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CGpioControl monitoring service
//////////////////////////////////////////////////////////////////////

void CGpioControl::RunMonitoring()
{

#ifdef __TI_AM335X__
    int nEvent;
    int i = 0;
    unsigned int nValue = 0;
    struct epoll_event *events;
    char buf[64];
    int bufsize=sizeof(buf);
    GPIOENTRY     *pGpio;
    events = (struct epoll_event *)malloc(sizeof(*events) * INPUT_GPIO_MAX );

    for(;!m_bExitPending;)
    {
        nEvent = epoll_wait( m_nEpfd ,events , INPUT_GPIO_MAX , 3000000);
        for(i =0 ; i < nEvent ; i++)
        {
            pGpio = (GPIOENTRY*)events[i].data.ptr ;

            memset(buf, 0, bufsize);

            lseek(pGpio->fd, 0, SEEK_SET);
            if ( read(pGpio->fd, buf, bufsize) )
             {
                nValue = strtol(buf, (char**)NULL, 10);
                //if (nValue) nValue = 1;

                 if (pGpio->nValue != nValue)
                 {
                    if (m_bEnableHooker && (m_pfnCallback != NULL))
                    {
                        pGpio->nValue = nValue ;
                        m_Locker.Lock();
                        m_pfnCallback(pGpio->mask, nValue, m_pParam, m_pArgment);
                        m_Locker.Unlock();
                    }
                 }
             }        
        }
        USLEEP(3000000);    
    }

    nEvent = epoll_wait( m_nEpfd ,events , INPUT_GPIO_MAX  , 1000000);
    for(i =0 ; i < nEvent ; i++)
    {
        pGpio = (GPIOENTRY*)events[i].data.ptr ;
        epoll_ctl(m_nEpfd, EPOLL_CTL_DEL , pGpio->fd, events);
        free(events[i].data.ptr);
        close(pGpio->fd);
    }
    free(events);
#else
    struct    timeval    timeout;
    fd_set    rset;
    int        fd, n;
    int        dcd, low, pwr, ring;
    int        heater, door, charge;
    BYTE    oldDCD, oldPWR, oldBAT, oldRING;
    BYTE    oldHEATER, oldDOOR, oldCHARGE;

    fd = m_nFD;
    m_gpioLocker.Lock();
    oldDCD      = ioctl(fd, GPIO_IOCTL_IN, GP_GSM_DCD_INPUT);
    oldPWR      = ioctl(fd, GPIO_IOCTL_IN, GP_PWR_FAIL_INPUT);
    oldBAT      = ioctl(fd, GPIO_IOCTL_IN, GP_LOW_BATT_INPUT);
    oldRING     = ioctl(fd, GPIO_IOCTL_IN, GP_GSM_RI_INPUT);
    oldDOOR        = ioctl(fd, GPIO_IOCTL_IN, GP_DOOR_OPEN_INPUT);
    oldHEATER    = ioctl(fd, GPIO_IOCTL_IN, GP_TEMP_OS_INPUT);
    oldCHARGE    = ioctl(fd, GPIO_IOCTL_IN, GP_BATT_CHG_STATUS_INPUT);
    m_gpioLocker.Unlock();

    for(;!m_bExitPending;)
    {
        FD_ZERO(&rset);
        FD_SET(fd, &rset);

        timeout.tv_sec    = 3;
        timeout.tv_usec    = 0;

        n = select(fd+1, &rset, NULL, NULL, &timeout);
        if (n == -1)
            break;

        if (FD_ISSET(fd, &rset))
        {
            m_gpioLocker.Lock();
            dcd      = ioctl(fd, GPIO_IOCTL_IN, GP_GSM_DCD_INPUT);
            pwr      = ioctl(fd, GPIO_IOCTL_IN, GP_PWR_FAIL_INPUT);
            low      = ioctl(fd, GPIO_IOCTL_IN, GP_LOW_BATT_INPUT);
            ring     = ioctl(fd, GPIO_IOCTL_IN, GP_GSM_RI_INPUT);
            door    = ioctl(fd, GPIO_IOCTL_IN, GP_DOOR_OPEN_INPUT);
            heater    = ioctl(fd, GPIO_IOCTL_IN, GP_TEMP_OS_INPUT);
            charge    = ioctl(fd, GPIO_IOCTL_IN, GP_BATT_CHG_STATUS_INPUT);
            m_gpioLocker.Unlock();

            // DCD Check
            if (dcd != oldDCD)
            {
                XDEBUG("GPIO: ------ Mobile DCD %s ------\r\n", dcd == 0 ? "Active" : "Normal");
                m_Locker.Lock();
                if (m_bEnableHooker && (m_pfnCallback != NULL))
                    m_pfnCallback(GPIONOTIFY_DCD, dcd, m_pParam, m_pArgment);
                m_Locker.Unlock();
                oldDCD = dcd;
            }

            if (pwr != oldPWR)
            {
                // 100ms 후에도 Power Fail인지 검사한다.
                usleep(100000);
                pwr = ioctl(fd, GPIO_IOCTL_IN, GP_PWR_FAIL_INPUT);
                if (pwr != oldPWR)
                {
                    XDEBUG("GPIO: ------ Power %s signal ------\r\n", pwr == 0 ? "UP" : "DOWN");
                    m_Locker.Lock();
                    if (m_bEnableHooker && (m_pfnCallback != NULL))
                        m_pfnCallback(GPIONOTIFY_PWR, pwr, m_pParam, m_pArgment);
                    m_Locker.Unlock();
                    oldPWR = pwr;

                    // Power restore
                    if (pwr == 0)
                    {
                        // Battery Charging
                        XDEBUG("GPIO: ------ Toggle battery charge ------\r\n");
                        ioctl(fd, GPIO_IOCTL_OUT, GPIOLOW(GP_BATT_CHG_EN_OUT));        // BATT CHARGE DISABLE
                        usleep(1000000);
                        ioctl(fd, GPIO_IOCTL_OUT, GPIOHIGH(GP_BATT_CHG_EN_OUT));    // BATT CHARGE ENABLE
                    }
                }
            }

            if (low != oldBAT)
            {
                // 100ms 후에도 Low Battery인지 검사한다.
                usleep(100000);
                low  = ioctl(fd, GPIO_IOCTL_IN, GP_LOW_BATT_INPUT);
                if (low != oldBAT)
                {
                    XDEBUG("GPIO: ------ Low Battery %s signal ------\n", low == 0 ? "Low" : "Normal");
                    m_Locker.Lock();
                    if (m_bEnableHooker && (m_pfnCallback != NULL))
                        m_pfnCallback(GPIONOTIFY_LOWBAT, low, m_pParam, m_pArgment);
                    m_Locker.Unlock();
                    oldBAT = low;
                }
            }

            if (ring != oldRING)
            {
                XDEBUG("GPIO: ------ Mobile RING signal %s ------\n", ring == 0 ? "Active" : "Normal");
                m_Locker.Lock();
                if (m_bEnableHooker && (m_pfnCallback != NULL))
                    m_pfnCallback(GPIONOTIFY_RING, ring, m_pParam, m_pArgment);
                m_Locker.Unlock();
                oldRING = ring;
            }

            // Heater
            if (heater != oldHEATER)
            {
                XDEBUG("GPIO: ------ Heater %s ------\r\n", heater == 0 ? "OFF" : "ON");
                m_Locker.Lock();
                if (m_bEnableHooker && (m_pfnCallback != NULL))
                    m_pfnCallback(GPIONOTIFY_HEATER, heater, m_pParam, m_pArgment);
                m_Locker.Unlock();
                oldHEATER = heater;
            }

            // Door open
            if (door != oldDOOR)
            {
                XDEBUG("GPIO: ------ Door %s ------\r\n", door == 0 ? "OPEN" : "CLOSE");
                m_Locker.Lock();
                if (m_bEnableHooker && (m_pfnCallback != NULL))
                    m_pfnCallback(GPIONOTIFY_DOOR, door, m_pParam, m_pArgment);
                m_Locker.Unlock();
                oldDOOR = door;
            }

            // Battery charge
            if (charge != oldCHARGE)
            {
                XDEBUG("GPIO: ------ Battery charge %s ------\r\n", charge == 0 ? "OFF" : "CHARGE");
                m_Locker.Lock();
                if (m_bEnableHooker && (m_pfnCallback != NULL))
                    m_pfnCallback(GPIONOTIFY_BATTERY_CHARG, dcd, m_pParam, m_pArgment);
                m_Locker.Unlock();
                oldCHARGE = charge;
            }
        }
    }
#endif

}

//////////////////////////////////////////////////////////////////////
// CGpioControl Status Monitoring Thread
//////////////////////////////////////////////////////////////////////

void *CGpioControl::gpioWatchThread(void *pParam)
{
    CGpioControl    *pThis = (CGpioControl *)pParam;

    pThis->RunMonitoring();
    return 0;
}

