
#include "typedef.h"
#include "gpio.h"
#include "GPIOAPI.h"

#include "CommonUtil.h"
#include "ShellCommand.h"
#include "TestGpio.h"
#include "Utility.h"

const int nHigh = 1;
const int nLow = 0;

const int nGpio0_20 = 20;
const int nGpio0_29 = 29;
const int nGpio1_28 = 60;
const int nGpio1_31 = 63;
const int nGpio2_0 = 64;

static char errMsgBuff[256];

TEST_GROUP m_GpioTest[] =
{
//    { "GPIO GSM_SYN(0-20)/LOW",         testGpioRead,          { &nGpio0_20, &nLow, NULL, NULL} },
    { "GPIO LOW_BATT(0-29)/HIGH",       testGpioRead,          { &nGpio0_29, &nHigh, NULL, NULL} },
// HW실 요청으로 Case Open 제외 (2014-12-16)
//    { "GPIO CASE_OPEN(1-28)/HIGH",      testGpioRead,          { &nGpio1_28, &nHigh, NULL, NULL} },
    { "GPIO POWER_FAIL(1-31)/LOW",      testGpioRead,          { &nGpio1_31, &nLow, NULL, NULL} },
//    { "GPIO BATT_CHG_STATE(2-0)/HIGH",  testGpioRead,          { &nGpio2_0, &nHigh, NULL, NULL} },
    { "Mobile module power control",    testGpioMobilePwCtrl,  { NULL, NULL, NULL, NULL} },
    { NULL, NULL, { NULL, NULL, NULL, NULL } }
};

char* testGpioRead(const void* gpioNum, const void* expectedValue, const void*, const void*)
{
    int readVal;
    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            readVal = !(*((int*)expectedValue));
            goto FAIL;
        }
        return NULL;
    }

    readVal = GPIOAPI_ReadGpio(*((int*)gpioNum));
    if(readVal == *((int*)expectedValue)) return NULL;

FAIL:
    sprintf(errMsgBuff,"invalid gpio value %s", readVal ? "HIGH" : "LOW");
    return errMsgBuff;
}

char* testGpioMobilePwCtrl(const void*, const void*, const void*, const void*)
{
    int readVal;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            goto FAIL;
        }
        return NULL;
    }

    // Mobile Power Control
    GPIOAPI_WriteGpio(GP_GSM_PCTL_OUT, GPIO_HIGH);
    USLEEP(1000000);
    GPIOAPI_WriteGpio(GP_GSM_PCTL_OUT, GPIO_LOW);
    USLEEP(1000000);
    GPIOAPI_WriteGpio(GP_GSM_PCTL_OUT, GPIO_HIGH);
    USLEEP(1000000);

    // Initialial power on
    GPIOAPI_WriteGpio(GP_GSM_INIT_PCTL_OUT, GPIO_HIGH);
    USLEEP(5000000);
    GPIOAPI_WriteGpio(GP_GSM_INIT_PCTL_OUT, GPIO_LOW);
    USLEEP(1000000);

    readVal = GPIOAPI_ReadGpio(GP_GSM_SYNC_INPUT);
    if(readVal != 0) 
    {
FAIL:
        sprintf(errMsgBuff,"GSM Power control fail");
        return errMsgBuff;
    }

    return NULL;
}
