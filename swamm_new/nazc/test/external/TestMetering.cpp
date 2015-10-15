
#include <UnitTest++.h>

#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"
#include "GeMeter.h"
#include "AidonMeter.h"


/** Test Suite #09 Ondeamdn.
 */
SUITE(_08_Metering)
{
/** Test Metering All
 *   . 전체 Sensor 에 대한 Metering을 수행 한다.
 *
 *      - metering (197.102)
 *
 */
TEST(MeteringAll)
{
    int nError=0;

    CIF4Invoke  invoke(gServerAddr, 8000, 30);

    invoke.AddParam("1.9",0);
    nError = Call(invoke.GetHandle(), "197.102");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);

    UnitTest::TimeHelpers::SleepMs(10 * 60 * 1000);
}

/** Test Recovery Current
 *   . 실패 검침 테스트 (현재 검침 목록 중에서 실패한 검침을 다시 시도한다)
 *
 *      - recovery current (104.3)
 */
TEST(RecoveryMetering)
{
    int nError = 0;
    TIMESTAMP   tmRecovery;

    CIF4Invoke  invoke(gServerAddr, 8000, 30);

    GetTimestamp(&tmRecovery, NULL);
    invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmRecovery, sizeof(TIMESTAMP));
    nError = Call(invoke.GetHandle(), "104.3");

    CHECK_EQUAL(IF4ERR_NOERROR, nError);

    UnitTest::TimeHelpers::SleepMs(2 * 60 * 1000);
}

/** Test Recovery Meter
 *   . 지정된 Meter의 지정된 Meter 정보를 읽어온다.
 *
 *      - recovery previous (104.13)
 */
TEST(ScheduledMetering)
{
    int i=0;
    int nError, nCount=0;

    CIF4Invoke  invoke(gServerAddr, 8000, 30);
    CIF4Invoke  *invoke2 = NULL;
    IF4Invoke * pInvoke = NULL;

    SENSORINFOENTRYNEW  *pSensor=NULL;

    nError = Call(invoke.GetHandle(), "102.26");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);

    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nCount = pInvoke->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;

        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);

        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.9", 0);
        invoke2->AddParam("1.9", 96);
        invoke2->AddParam("1.9", 48);

        nError = Call(invoke2->GetHandle(), "104.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);

        delete invoke2;
    }

    UnitTest::TimeHelpers::SleepMs(2 * 60 * 1000);
}

}
