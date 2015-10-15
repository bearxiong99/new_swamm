
#include <UnitTest++.h>

#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"
#include "codiDefine.h"


struct CoordinatorFixture
{
    CoordinatorFixture() : channel(13), panid(1234), rfpower(-9),
            txpowermode(CODI_TXPOWERMODE_BOOST), permit(200), encrypt(0),
            discovery(TRUE), multicasthops(5), mtortype(CODI_MANYTOONE_LOW_RAM)
    {
        CIF4Invoke  * invoke = NULL;
        int nError;
        {
            /*-- disable auto forming --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParam("3.3.12", (BYTE) 0);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(2000);

            /*-- route discovery --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("3.3.22", FALSE);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(2000);

            /*-- multicast hops --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("3.3.23", (BYTE)10);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(2000);

            /*-- mtor type --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("2.2.28", CODI_MANYTOONE_HIGH_RAM);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(2000);
        }
    }
    ~CoordinatorFixture() {}

    const BYTE  channel;
    const WORD  panid;
    const signed char  rfpower;
    const BYTE  txpowermode;
    const BYTE  permit;
    const BYTE  encrypt;
    const BOOL  discovery;
    const BYTE  multicasthops;
    const BYTE  mtortype;
};

/** Test Suite #5 Coordinator Test.
 */
SUITE(_05_Coordinator)
{

/** Test Reset Coordinator.
 *  먼저 Auto Forming을 Disable 시킨 후  Coordinator를 Reset 시킨다.
 *  이후 Reset Kind가 RESET_POWERON 인지 확인한다.
 *
 *      - codiAutoSetting (3.3.12)
 *      - codiEntry (3.3)
 *
 *      - cmdResetDevice (100.23)
 *      - cmdGetCodiList (101.13)
 *      - cmdStdSet (199.2)
 *
 *  Auto Forming이 설정되어 있을 경우 Forming 때문에 오랜 시간이 걸릴 수 있다.
 */
TEST_FIXTURE(CoordinatorFixture, ResetCoordinator)
{
    int nError;

    CIF4Invoke  * invoke = NULL;
    CODIENTRY   *pCoordinator = NULL;

    {
        UNITTEST_TIME_CONSTRAINT(3000);

        /*-- reset coordinator --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("1.4", (BYTE) 1);
        nError = Call(invoke->GetHandle(), "100.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;
    }

    UnitTest::TimeHelpers::SleepMs(3000);

    {
        UNITTEST_TIME_CONSTRAINT(3000);

        /*-- get coordinator reset kind --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        nError = Call(invoke->GetHandle(), "101.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        pCoordinator = (CODIENTRY *)invoke->GetHandle()->pResult[0]->stream.p;
        CHECK_EQUAL(0x02, pCoordinator->codiResetKind);     // Reset PowerOn
        delete invoke;
    }
}

/** Test Coordinator Setting.
 *  Coordinator의 Attribute 들이 정상인지 확인.
 *
 *      - codiEntry (3.3)
 *      - codiChannel (3.3.13)
 *      - codiPanID (3.3.14)
 *      - codiRfPower (3.3.16)
 *      - codiTxPowerMode (3.3.17)
 *      - codiPermit (3.3.18)
 *      - codiEnableEncrypt (3.3.19)
 *      - codiRouterDiscovery (3.3.22)
 *      - codiMulitcastHops (3.3.23)
 *      - varEnableHighRAM (2.2.28)
 *
 *      - cmdGetCodiList (101.13)
 *      - cmdStdSet (199.2)
 *      - cmdStdGet (199.1)
 *
 */
TEST_FIXTURE(CoordinatorFixture, CoodinatorAttribute)
{
    int nError;

    CIF4Invoke  * invoke = NULL;
    CODIENTRY   *pCoordinator = NULL;
    CODIENTRY   Coordi;
    BOOL        origMtorType = FALSE;

    memset(&Coordi, 0, sizeof(CODIENTRY));
    {
        UNITTEST_TIME_CONSTRAINT(3000);

        /*-- get coordinator entries --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        nError = Call(invoke->GetHandle(), "101.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        pCoordinator = (CODIENTRY *)invoke->GetHandle()->pResult[0]->stream.p;
        memcpy(&Coordi, pCoordinator, sizeof(CODIENTRY));
        delete invoke;

        /*-- mtor type --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("2.2.28");
        nError = Call(invoke->GetHandle(), "199.1");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        origMtorType = invoke->GetHandle()->pResult[0]->stream.u8;
        delete invoke;
    }

    {
        /*-- channel --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.13", channel);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        UnitTest::TimeHelpers::SleepMs(5000);
        delete invoke;

        /*-- panid --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.14", panid);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        UnitTest::TimeHelpers::SleepMs(5000);
        delete invoke;

        /*-- rfpower --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.16", rfpower);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        UnitTest::TimeHelpers::SleepMs(5000);
        delete invoke;

        /*-- txpowermode --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.17", txpowermode);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /*-- permit --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.18", permit);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /*-- encrypt --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.19", encrypt);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        UnitTest::TimeHelpers::SleepMs(5000);
        delete invoke;

        /*-- route discovery --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.22", discovery);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /*-- multicast hops --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.23", multicasthops);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /*-- mtor type --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("2.2.28", mtortype);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

    }

    {
        UNITTEST_TIME_CONSTRAINT(3000);

        /*-- get coordinator entries --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        nError = Call(invoke->GetHandle(), "101.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        pCoordinator = (CODIENTRY *)invoke->GetHandle()->pResult[0]->stream.p;

        CHECK_EQUAL(channel, pCoordinator->codiChannel);
        CHECK_EQUAL(panid, pCoordinator->codiPanID);
        CHECK_EQUAL(rfpower, pCoordinator->codiRfPower);
        CHECK_EQUAL(txpowermode, pCoordinator->codiTxPowerMode);
        CHECK_EQUAL(permit, pCoordinator->codiPermit);
        CHECK_EQUAL((BYTE)1, pCoordinator->codiEnableEncrypt); /** EnableEncrypt 값은 Disable 시킬 수 없다 */
        CHECK_EQUAL(discovery, pCoordinator->codiRouteDiscovery);
        CHECK_EQUAL(multicasthops, pCoordinator->codiMulticastHops);
        delete invoke;

        /*-- mtor type --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("2.2.28");
        nError = Call(invoke->GetHandle(), "199.1");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            CHECK_EQUAL(mtortype, invoke->GetHandle()->pResult[0]->stream.u8);
        }
        delete invoke;
    }

    /*-- 원상 복귀 시킨다 --*/
    {
        /*-- channel --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.13", Coordi.codiChannel);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        UnitTest::TimeHelpers::SleepMs(5000);
        delete invoke;

        /*-- panid --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.14", Coordi.codiPanID);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        UnitTest::TimeHelpers::SleepMs(5000);
        delete invoke;

        /*-- rfpower --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.16", Coordi.codiRfPower);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        UnitTest::TimeHelpers::SleepMs(5000);
        delete invoke;

        /*-- txpowermode --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.17", Coordi.codiTxPowerMode);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /*-- permit --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.18", Coordi.codiPermit);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /*-- encrypt --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.19", Coordi.codiEnableEncrypt);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        UnitTest::TimeHelpers::SleepMs(5000);
        delete invoke;

        /*-- route discovery --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.22", Coordi.codiRouteDiscovery);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /*-- multicast hops --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("3.3.23", Coordi.codiMulticastHops);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /*-- mtor type --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("2.2.28", origMtorType);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

    }

    {
        UNITTEST_TIME_CONSTRAINT(3000);

        /*-- get coordinator entries --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        nError = Call(invoke->GetHandle(), "101.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        pCoordinator = (CODIENTRY *)invoke->GetHandle()->pResult[0]->stream.p;

        CHECK_EQUAL(Coordi.codiChannel, pCoordinator->codiChannel);
        CHECK_EQUAL(Coordi.codiPanID, pCoordinator->codiPanID);
        CHECK_EQUAL(Coordi.codiRfPower, pCoordinator->codiRfPower);
        CHECK_EQUAL(Coordi.codiTxPowerMode, pCoordinator->codiTxPowerMode);
        CHECK_EQUAL(Coordi.codiPermit, pCoordinator->codiPermit);
        CHECK_EQUAL((BYTE)1, pCoordinator->codiEnableEncrypt);
        CHECK_EQUAL(Coordi.codiEnableEncrypt, pCoordinator->codiEnableEncrypt);
        CHECK_EQUAL(Coordi.codiRouteDiscovery, pCoordinator->codiRouteDiscovery);
        CHECK_EQUAL(Coordi.codiMulticastHops, pCoordinator->codiMulticastHops);
        delete invoke;

        /*-- mtor type --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
        invoke->AddParam("2.2.28");
        nError = Call(invoke->GetHandle(), "199.1");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            CHECK_EQUAL(origMtorType, invoke->GetHandle()->pResult[0]->stream.u8);
        }
        delete invoke;
    }
}


} // End of Suite
