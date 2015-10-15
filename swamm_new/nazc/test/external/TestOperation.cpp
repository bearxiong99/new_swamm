
#include <UnitTest++.h>

#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"
#include "GeMeter.h"


/** Test Operations
 */


/** Test TimeSync All
 *   . 전체 Meter에 대한 Time Sync 수행
 *      - timesync (105.6)
 *
 */
TEST(TimeSyncAll)
{
    int nError;

    CIF4Invoke  invoke(gServerAddr, 8000, 30);

    SKIP_TEST

    nError = Call(invoke.GetHandle(), "105.6");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);

    CHECK_EQUAL(0, invoke.GetHandle()->nResultCount);

    /*-- 작업이 끝나는 시간까지 기다려야 하기 때문에 10초간 기다려 본다 --*/
    USLEEP(10*100000);
}


/** Test Meter Uploading
 *   . Metering 정보를 Upload 하는것을 검사한다.
 *      1) Default 값으로 Upload 수행
 *      2) 현재 시간 -1 시간에 대한 Upload 수행
 *
 *      - upload (197.6)
 */
TEST(MeterUpload)
{
    int nError;
    time_t currentTime;
    TIMESTAMP cMinusOneHour, endTimeStamp;

    CIF4Invoke  invoke(gServerAddr, 8000, 30);

    SKIP_TEST

    /*-- default call --*/
    nError = Call(invoke.GetHandle(), "197.6");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);

    /*-- 현재 시간 -1시간의 검침 데이터 upload --*/
    time(&currentTime);
    currentTime -= 60 * 60; // 1시간 빼기 
    GetTimestamp(&cMinusOneHour,&currentTime);

    cMinusOneHour.min = 0;
    cMinusOneHour.sec = 0;

    memcpy(&endTimeStamp, &cMinusOneHour, sizeof(TIMESTAMP));

    invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &cMinusOneHour, sizeof(TIMESTAMP));
    invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &endTimeStamp, sizeof(TIMESTAMP));

    nError = Call(invoke.GetHandle(), "197.6");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);
}

/** Test Ping Meter
 *   . Meter에 대한 Ping Test 수행
 *   . 일단 GE Meter만 True가 될 것이다. aidon Meter에 대해서는 궁리해 봐야 한다
 *
 *      - show sensor list (102.26)
 *      - upload (104.30)
 */
TEST(PingMeter)
{
    int nError, nCount=0, i=0;
    GETABLE     *pList;
    //METER_STAT  *pStat;
    BOOL        isValidPingResult = FALSE;
    SENSORINFOENTRYNEW  *pSensor=NULL;

    CIF4Invoke  invoke(gServerAddr, 8000, 30);
    CIF4Invoke *invoke2 = NULL;

    SKIP_TEST

    nError = Call(invoke.GetHandle(), "102.26");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);
    if(nError != IF4ERR_NOERROR) return;

    nCount = invoke.GetHandle()->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)invoke.GetHandle()->pResult[i]->stream.p;

        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        nError = Call(invoke2->GetHandle(), "104.30");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);

        if(nError != IF4ERR_NOERROR) {
            delete invoke2;
            return;
        }

        CHECK_EQUAL(2, invoke2->GetHandle()->nResultCount);

        if(invoke2->GetHandle()->nResultCount != 2) 
        {
            delete invoke2;
            return;
        }

	    pList = (GETABLE *)invoke2->GetHandle()->pResult[0]->stream.p;
	    //pStat = (METER_STAT *)invoke2->GetHandle()->pResult[1]->stream.p;

	    if (strncmp(pList->table, "S001", 4) == 0)  isValidPingResult = TRUE;
	    else isValidPingResult = FALSE;

        CHECK(isValidPingResult);

        delete invoke2;
    }
}

/** Test Ping Start/Stop
 *   . 전체 Meter에 대한 Ping Schedule을 Start/Stop 한다.
 *
 *      - ping start (197.11)
 *      - ping stop (197.11)
 */
TEST(PingStartStop)
{
    int nError;

    CIF4Invoke  invoke(gServerAddr, 8000, 30);
    CIF4Invoke  invoke2(gServerAddr, 8000, 30);

    SKIP_TEST

    /*-- Ping Start --*/
    invoke.AddParam("1.4", (BYTE) 1);
    nError = Call(invoke.GetHandle(), "197.11");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);

    /*-- ping이 수행되는 동안 잠시 기다린다 --*/
    USLEEP(5*100000);

    /*-- Ping Stop --*/
    invoke2.AddParam("1.4", (BYTE) 0);
    nError = Call(invoke2.GetHandle(), "197.11");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);
}

/** Test Diagnosis
 *   . Diagnosis 정보를 조회한다
 *
 *      - diagnosis (100.1)
 */
TEST(Diagnosis)
{
    int     nError;
    BYTE    *pState = NULL;
    BYTE    Normal=0;

    CIF4Invoke invoke(gServerAddr, 8000, 30);

    SKIP_TEST

    nError = Call(invoke.GetHandle(), "100.1");

    CHECK_EQUAL(IF4ERR_NOERROR, nError);

    if(nError != IF4ERR_NOERROR) return;

    pState = (BYTE *)invoke.GetHandle()->pResultNode[0].stream.p;

    CHECK_EQUAL(Normal, pState[0]);         // Mcu
    CHECK_EQUAL(Normal, pState[1]);         // Sink 1
    CHECK_EQUAL(Normal, pState[2]);         // Sink 2
    CHECK_EQUAL(Normal, pState[3]);         // Power
    CHECK_EQUAL(Normal, pState[4]);         // Battery
    CHECK_EQUAL(Normal, pState[5]);         // Temp
    CHECK_EQUAL(Normal, pState[6]);         // Memory
    CHECK_EQUAL(Normal, pState[7]);         // Flash
    CHECK_EQUAL(Normal, pState[8]);         // GSM
    CHECK_EQUAL(Normal, pState[9]);         // Ethernet

}

