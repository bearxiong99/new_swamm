//////////////////////////////////////////////////////////////////////
//
//    GpioControl.h: interface for the CGpioControl class.
//
//    This file is a part of the AIMIR.
//    (c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//    This source code can only be used under the terms and conditions 
//    outlined in the accompanying license agreement.
//
//    casir@paran.com
//    http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __GPIO_CONTROL_H__
#define __GPIO_CONTROL_H__

#include <pthread.h>
#include "Locker.h"
#include "GPIOAPI.h"
#include "LinkedList.h"

typedef struct    _tagGPIOENTRY
{
    int             fd;
    unsigned int    gpio;
    UINT            mask;
    unsigned int    nValue;
}GPIOENTRY;

class CGpioControl
{
public:
    CGpioControl();
    virtual ~CGpioControl();

public:
    BOOL    IsEnableHooker();
    BOOL    EnableGpioHooker(BOOL bEnable);

    int        ReadGpio(int nPort);
    BOOL    WriteGpio(int nPort, int nValue);
    unsigned int ReadAdc(int nPort);
public:
    BOOL    Initialize(BOOL bInitialize, const char* pMobileType, BOOL bMonitoring);
    BOOL    Destroy();

    BOOL    InstallHooker(PFNGPIOCALLBACK pfnCallback, void *pParam, void *pArgment);
    BOOL    UninstallHooker();

protected:
    void    RunMonitoring();
#if defined(__TI_AM335X__)
    void    EpollAdd(unsigned nNum, unsigned int nValue , unsigned int nState);
#endif
    
public:
    static    void *gpioWatchThread(void *pParam);

protected:
    BOOL            m_bExitPending;
    BOOL            m_bEnableHooker;
    char            *m_pMobileType;
    int                m_nFD;
    int                m_nEpfd;
    CLinkedList<GPIOENTRY *>    m_InputList;
    void            *m_pParam;
    void            *m_pArgment;
    CLocker            m_Locker;
    CLocker            m_gpioLocker;
    PFNGPIOCALLBACK    m_pfnCallback;
    PFNUSERHOOKER    m_pfnUserHooker;
    pthread_t        m_gpioThreadID;
};

#endif // __GPIO_CONTROL_H__
