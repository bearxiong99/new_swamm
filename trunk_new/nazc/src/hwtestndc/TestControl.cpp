
#include "typedef.h"
#include "gpio.h"
#include "GPIOAPI.h"

#include "CommonUtil.h"
#include "ShellCommand.h"
#include "TestControl.h"
#include "Utility.h"

static char errMsgBuff[256];

// Parameter
CONTROL_CHECK m_CtrlParam;

TEST_GROUP m_ControlTest[] =
{
// HW실 요청으로 Case Open 제외 (2014-12-16)
//    { "case open",        testCaseOpen,           { &m_CtrlParam.nWaitTime, NULL, NULL, NULL} },
    { "power fail",       testPowerFail,          { &m_CtrlParam.nWaitTime, NULL, NULL, NULL} },
    { "low battery",      testLowBattery,         { &m_CtrlParam.nWaitTime, NULL, NULL, NULL} },
    { NULL, NULL, { NULL, NULL, NULL, NULL } }
};

/** 60초 이내에 Case Open을 감지해야 한다.
  */
char* testCaseOpen(const void* waitTime, const void*, const void*, const void*)
{
    int nWaitTime = *((int*)waitTime) * 10;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) goto FAIL;
        return NULL;
    }

    for(int cnt=0; cnt<nWaitTime; cnt++)
    {
        if(GPIOAPI_ReadGpio(GP_CASE_OPEN_INPUT) == 0) return NULL;
        USLEEP(100000);
    }

FAIL:
    sprintf(errMsgBuff,"case-close detect fail");
    return errMsgBuff;
}

char* testPowerFail(const void* waitTime, const void*, const void*, const void*)
{
    int nWaitTime = *((int*)waitTime) * 10;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) goto FAIL;
        return NULL;
    }

    for(int cnt=0; cnt<nWaitTime; cnt++)
    {
        if(GPIOAPI_ReadGpio(GP_PWR_FAIL_INPUT) > 0) return NULL;
        USLEEP(100000);
    }

FAIL:
    sprintf(errMsgBuff,"power-fail detect fail");
    return errMsgBuff;
}

char* testLowBattery(const void* waitTime, const void*, const void*, const void*)
{
    int nWaitTime = *((int*)waitTime) * 10;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) goto FAIL;
        return NULL;
    }

    for(int cnt=0; cnt<nWaitTime; cnt++)
    {
        if(GPIOAPI_ReadGpio(GP_LOW_BATT_INPUT) == 0) return NULL;
        USLEEP(100000);
    }

FAIL:
    sprintf(errMsgBuff,"low-battery detect fail");
    return errMsgBuff;
}

