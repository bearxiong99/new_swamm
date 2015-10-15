
#include <UnitTest++.h>

#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"


/** Test Suite #3 Setting Test.
 */
SUITE(_03_Setting)
{

/** MCU ID 설정/조회.
 *      - sysID (2.1.2)
 *      - cmdStdGet (199.1)
 *      - cmdStdSet (199.2)
 */
TEST(McuID)
{
    int nError;
    CIF4Invoke  *invoke = NULL;
    UINT sysId = 0;

    {
        UNITTEST_TIME_CONSTRAINT(3000);

        {// Mcu ID 조회
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);

            invoke->AddParam("2.1.2");

            nError = Call(invoke->GetHandle(), "199.1");

            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            CHECK(invoke->GetHandle()->nResultCount > 0);

            sysId = LittleToHostInt(invoke->ResultAtName("sysID")->stream.u32);

            delete invoke;
        }
        {// Mcu ID 설정
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);

            invoke->AddParam("2.1.2", HostToLittleInt(sysId + 1));

            nError = Call(invoke->GetHandle(), "199.2");

            CHECK_EQUAL(IF4ERR_NOERROR, nError);

            delete invoke;
        }
        {// Mcu ID 조회
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);

            invoke->AddParam("2.1.2");

            nError = Call(invoke->GetHandle(), "199.1");

            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            CHECK(invoke->GetHandle()->nResultCount > 0);

            CHECK_EQUAL(sysId + 1, LittleToHostInt(invoke->ResultAtName("sysID")->stream.u32));

            delete invoke;
        }
        {// Mcu ID 설정
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);

            invoke->AddParam("2.1.2", HostToLittleInt(sysId));

            nError = Call(invoke->GetHandle(), "199.2");

            CHECK_EQUAL(IF4ERR_NOERROR, nError);

            delete invoke;
        }
    }
}

/** Server Address 설정/조회.
 *      - sysServer (2.1.20)
 *      - cmdStdGet (199.1)
 *      - cmdStdSet (199.2)
 */
TEST(ServerAddress)
{
    int nError;
    CIF4Invoke  *invoke = NULL;
    char sysAddr[64] = {0,};

    {
        UNITTEST_TIME_CONSTRAINT(3000);

        {// 조회
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);

            invoke->AddParam("2.1.20");

            nError = Call(invoke->GetHandle(), "199.1");

            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            CHECK(invoke->GetHandle()->nResultCount > 0);
            CHECK(invoke->GetHandle()->pResult[0]->len > 0);

            strncpy(sysAddr, invoke->GetHandle()->pResult[0]->stream.p, invoke->GetHandle()->pResult[0]->len);

            delete invoke;
        }
        {// 설정
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);

            invoke->AddParam("2.1.20", "127.0.0.1");

            nError = Call(invoke->GetHandle(), "199.2");

            CHECK_EQUAL(IF4ERR_NOERROR, nError);

            delete invoke;
        }
        {// 설정값 조회
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);

            invoke->AddParam("2.1.20");

            nError = Call(invoke->GetHandle(), "199.1");

            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            CHECK(invoke->GetHandle()->nResultCount > 0);
            CHECK(invoke->GetHandle()->pResult[0]->len > 0);

            CHECK_EQUAL(0, strcmp("127.0.0.1", invoke->GetHandle()->pResult[0]->stream.p));

            delete invoke;
        }
        {// 원래값 복구
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);

            invoke->AddParam("2.1.20", sysAddr);

            nError = Call(invoke->GetHandle(), "199.2");

            CHECK_EQUAL(IF4ERR_NOERROR, nError);
        }
    }
}

/** Test System.
 *
 *  System 상태를 설정하고 조회후 정상적으로 설정된건지 확인한다.
 *
 *      - cmdStdGetChild (199.4)
 *      - cmdStdSet (199.2)
 *
 *      - sysID (2.1.2)
 *      - sysTime (2.1.16)
 *      - sysName (2.1.4)
 *      - sysDescr (2.1.5)
 *      - sysLocation (2.1.6)
 *      - sysContact (2.1.7)
 *
 * 각 Attribute 현재 값을 읽어와서 새로운 값으로 변경 후 
 * 맞게 변경되었는지 확인해 보고 다시 이전 정보로 복구 시킨다.
 *
 */
TEST(SetSystem)
{
    int nError;
    char name[64]={0,};
    char descr[64]={0,};
    char location[64]={0,};
    char contact[64]={0,};
    TIMESTAMP  ptime;
    TIMESTAMP newtime;
    int  id = 0;

    CIF4Invoke  * invoke = NULL;

    {
    UNITTEST_TIME_CONSTRAINT(3000 * 3);

    invoke = new CIF4Invoke(gServerAddr, 8000, 3);

    invoke->AddParam("2.1");
    nError = Call(invoke->GetHandle(), "199.4");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);

	id = (int) invoke->ResultAtName("sysID")->stream.u32;
    memcpy(&ptime,invoke->ResultAtName("sysTime")->stream.p, sizeof(TIMESTAMP));
	strncpy(name, (char *) invoke->ResultAtName("sysName")->stream.p, invoke->ResultAtName("sysName")->len);
	strncpy(descr, (char *) invoke->ResultAtName("sysDescr")->stream.p, invoke->ResultAtName("sysDescr")->len);
	strncpy(location, (char *) invoke->ResultAtName("sysLocation")->stream.p, invoke->ResultAtName("sysLocation")->len);
	strncpy(contact, (char *) invoke->ResultAtName("sysContact")->stream.p, invoke->ResultAtName("sysContact")->len);
    memcpy(&newtime,&ptime,sizeof(TIMESTAMP));
    delete invoke;

    newtime.year --;
    {
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.2", (UINT) (id + 1));
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.4", "UnitTestName");
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.5", "UnitTestDescr");
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.6", "UnitTestLoc");
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.7", "UnitTestContact");
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParamFormat("2.1.16", VARSMI_TIMESTAMP, &newtime, sizeof(TIMESTAMP));
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;
    }

    invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    invoke->AddParam("2.1");
    nError = Call(invoke->GetHandle(), "199.4");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);

	CHECK_EQUAL((UINT)(id+1), invoke->ResultAtName("sysID")->stream.u32);
    CHECK_EQUAL((int)newtime.year,(int)((TIMESTAMP *)invoke->ResultAtName("sysTime")->stream.p)->year);
    CHECK_EQUAL((int)newtime.mon,(int)((TIMESTAMP *)invoke->ResultAtName("sysTime")->stream.p)->mon);
    CHECK_EQUAL((int)newtime.day,(int)((TIMESTAMP *)invoke->ResultAtName("sysTime")->stream.p)->day);
    CHECK_EQUAL((int)newtime.hour,(int)((TIMESTAMP *)invoke->ResultAtName("sysTime")->stream.p)->hour);
	CHECK_EQUAL(0, strncmp("UnitTestName", (char *) invoke->ResultAtName("sysName")->stream.p, invoke->ResultAtName("sysName")->len));
	CHECK_EQUAL(0, strncmp("UnitTestDescr", (char *) invoke->ResultAtName("sysDescr")->stream.p, invoke->ResultAtName("sysDescr")->len));
	CHECK_EQUAL(0, strncmp("UnitTestLoc", (char *) invoke->ResultAtName("sysLocation")->stream.p, invoke->ResultAtName("sysLocation")->len));
	CHECK_EQUAL(0, strncmp("UnitTestContact", (char *) invoke->ResultAtName("sysContact")->stream.p, invoke->ResultAtName("sysContact")->len));

    delete invoke;

    {
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.2", id);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.4", name);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.5", descr);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.6", location);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.7", contact);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParamFormat("2.1.16", VARSMI_TIMESTAMP, &ptime, sizeof(TIMESTAMP));
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;
    }

    invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    invoke->AddParam("2.1");
    nError = Call(invoke->GetHandle(), "199.4");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);

	CHECK_EQUAL((UINT) id, invoke->ResultAtName("sysID")->stream.u32);
    CHECK_EQUAL((int)ptime.year,(int)((TIMESTAMP *)invoke->ResultAtName("sysTime")->stream.p)->year);
    CHECK_EQUAL((int)ptime.mon,(int)((TIMESTAMP *)invoke->ResultAtName("sysTime")->stream.p)->mon);
    CHECK_EQUAL((int)ptime.day,(int)((TIMESTAMP *)invoke->ResultAtName("sysTime")->stream.p)->day);
    CHECK_EQUAL((int)ptime.hour,(int)((TIMESTAMP *)invoke->ResultAtName("sysTime")->stream.p)->hour);
	CHECK_EQUAL(0, strncmp(name, (char *) invoke->ResultAtName("sysName")->stream.p, invoke->ResultAtName("sysName")->len));
	CHECK_EQUAL(0, strncmp(descr, (char *) invoke->ResultAtName("sysDescr")->stream.p, invoke->ResultAtName("sysDescr")->len));
	CHECK_EQUAL(0, strncmp(location, (char *) invoke->ResultAtName("sysLocation")->stream.p, invoke->ResultAtName("sysLocation")->len));
	CHECK_EQUAL(0, strncmp(contact, (char *) invoke->ResultAtName("sysContact")->stream.p, invoke->ResultAtName("sysContact")->len));

    delete invoke;

    }
}

/** Test Mobile
 *  Mobile 정보를 조회하고 특정한 값으로 설정 후 정상적으로 설정 됐는지 
 *  확인한다. 그 후 원래 값으로 복귀 시키고 그 값 설정도 확인한다.
 *
 *      - cmdStdGetChild (199.4)
 *      - cmdStdSet (199.2)
 *
 *      - sysMobileType (2.1.12)
 *      - sysMobileMode (2.1.13)
 *      - sysPhoneNumber (2.1.10)
 */
TEST(SetMobile)
{
    int nError;
    BYTE mobileType, mobileMode;
    char phoneNumber[64]={0,};

    CIF4Invoke  * invoke = NULL;

    {
        UNITTEST_TIME_CONSTRAINT(3000);

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1");
        nError = Call(invoke->GetHandle(), "199.4");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);

	    mobileType = invoke->ResultAtName("sysMobileType")->stream.u8;
	    mobileMode = invoke->ResultAtName("sysMobileMode")->stream.u8;
	    strncpy(phoneNumber, (char *) invoke->ResultAtName("sysPhoneNumber")->stream.p, invoke->ResultAtName("sysPhoneNumber")->len);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.12", MOBILE_TYPE_PSTN);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.13", MOBILE_MODE_CSD);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.10", "1234-5678");
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1");
        nError = Call(invoke->GetHandle(), "199.4");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) 
        {
            delete invoke;
            return;
        }

        CHECK_EQUAL(MOBILE_TYPE_PSTN, (BYTE)invoke->ResultAtName("sysMobileType")->stream.u8);
        CHECK_EQUAL(MOBILE_MODE_CSD, (BYTE)invoke->ResultAtName("sysMobileMode")->stream.u8);
        CHECK_EQUAL(0, strncmp("1234-5678", (char *)invoke->ResultAtName("sysPhoneNumber")->stream.p, 
                invoke->ResultAtName("sysPhoneNumber")->len));
        delete invoke;

        /*---------------------------------------------------------------------------------*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.12", mobileType);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.13", mobileMode);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.10", phoneNumber);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1");
        nError = Call(invoke->GetHandle(), "199.4");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) 
        {
            delete invoke;
            return;
        }

        CHECK_EQUAL(mobileType, (BYTE)invoke->ResultAtName("sysMobileType")->stream.u8);
        CHECK_EQUAL(mobileMode, (BYTE)invoke->ResultAtName("sysMobileMode")->stream.u8);
        CHECK_EQUAL(0, strncmp(phoneNumber, (char *)invoke->ResultAtName("sysPhoneNumber")->stream.p, 
                invoke->ResultAtName("sysPhoneNumber")->len));
        delete invoke;

    }

}

/** Test Server.
 *  Server 정보를 조회하고 특정한 값으로 설정 후 정상적으로 설정 됐는지 
 *    확인한다. 그 후 원래 값으로 복귀 시키고 그 값 설정도 확인한다.
 *
 *      - cmdStdGetChild (199.4)
 *      - cmdStdSet (199.2)
 *
 *      - sysServer (2.1.20)
 *      - sysServerPort (2.1.21)
 *      - sysServerAlarmPort (2.1.22)
 */
TEST(SetServer)
{
    int nError;
    int serverPort=0, alarmPort=0;
    char addr[64] = {0,};

    CIF4Invoke  * invoke = NULL;

    {
        UNITTEST_TIME_CONSTRAINT(3000);

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1");
        nError = Call(invoke->GetHandle(), "199.4");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);

	    strncpy(addr, invoke->ResultAtName("sysServer")->stream.p, invoke->ResultAtName("sysServer")->len) ;
	    serverPort = invoke->ResultAtName("sysServerPort")->stream.u32;
	    alarmPort = invoke->ResultAtName("sysServerAlarmPort")->stream.u32;
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.20", "127.0.0.1");
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.21", 8765);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.22", 8766);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1");
        nError = Call(invoke->GetHandle(), "199.4");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);

        CHECK_EQUAL(0, strncmp("127.0.0.1", invoke->ResultAtName("sysServer")->stream.p, invoke->ResultAtName("sysServer")->len));
        CHECK_EQUAL((UINT)8765, (UINT)invoke->ResultAtName("sysServerPort")->stream.u32);
        CHECK_EQUAL((UINT)8766, (UINT)invoke->ResultAtName("sysServerAlarmPort")->stream.u32);
        delete invoke;

        /*---------------------------------------------------------------------------------*/

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.20", addr);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.21", serverPort);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1.22", alarmPort);
        nError = Call(invoke->GetHandle(), "199.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        invoke->AddParam("2.1");
        nError = Call(invoke->GetHandle(), "199.4");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) 
        {
            delete invoke;
            return;
        }

        CHECK_EQUAL(0, strncmp(addr, invoke->ResultAtName("sysServer")->stream.p, invoke->ResultAtName("sysServer")->len));
        CHECK_EQUAL((UINT)serverPort, (UINT)invoke->ResultAtName("sysServerPort")->stream.u32);
        CHECK_EQUAL((UINT)alarmPort, (UINT)invoke->ResultAtName("sysServerAlarmPort")->stream.u32);
        delete invoke;
    }
}

/** Test User Add/Delete.
 *   사용자를 추가하고 삭제한다
 *
 *      - cmdAddUser (106.1)
 *      - cmdDeleteUser (106.2)
 *      - cmdUserList (106.5)
 *
 *      - usrAccount (2.10.2)
 *      - usrPassword (2.10.3)
 *      - usrGroup (2.10.4)
 */
TEST(UserAddDelete)
{
    int nError;
    int nPrevUserCnt;

    CIF4Invoke * invoke = NULL;

    {
        UNITTEST_TIME_CONSTRAINT(3000);

        /*-- 현재 사용자 목록을 얻어온다. 반드시 1명 이상이 있어야 한다 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        nError = Call(invoke->GetHandle(), "106.5");

        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        CHECK(invoke->GetHandle()->nResultCount > 0);

        nPrevUserCnt = invoke->GetHandle()->nResultCount;
        delete invoke;
    
        /*-- 사용자를 추가한다 --*/
    
        /*-- 너무 짧은 비밀번호 일때 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);

        invoke->AddParam("2.10.2", "UnitTestUser_0");
        invoke->AddParam("2.10.3", "s"); // 너무 짧은 비밀번호
        invoke->AddParam("2.10.4", 2);
        nError = Call(invoke->GetHandle(), "106.1");
        CHECK_EQUAL(IF4ERR_INVALID_PASSWORD, nError);
        delete invoke;

        /*-- 정의되지 않은 그룹 번호 일때 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    
        invoke->AddParam("2.10.2", "UnitTestUser_1");
        invoke->AddParam("2.10.3", "ValidPassword");
        invoke->AddParam("2.10.4", 10);     // 정의되지 않은 그룹 번호
        nError = Call(invoke->GetHandle(), "106.1");
        CHECK_EQUAL(IF4ERR_INVALID_GROUP, nError);
        delete invoke;

        /*-- 정상적인 사용자 추가 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    
        invoke->AddParam("2.10.2", "UnitTestUser_2");
        invoke->AddParam("2.10.3", "ValidPassword");
        invoke->AddParam("2.10.4", 2); 
        nError = Call(invoke->GetHandle(), "106.1");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;
    
        /*-- 중복된 사용자일 때 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    
        invoke->AddParam("2.10.2", "UnitTestUser_2");
        invoke->AddParam("2.10.3", "ValidPassword");
        invoke->AddParam("2.10.4", 3);     // 정의되지 않은 그룹 번호
        nError = Call(invoke->GetHandle(), "106.1");
        CHECK_EQUAL(IF4ERR_INVALID_GROUP, nError);
        delete invoke;

        /*-- 사용자를 삭제한다 --*/

        /*-- 잘못된 사용자 명 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    
        invoke->AddParam("2.10.2", "UnitTestUser_X");
        nError = Call(invoke->GetHandle(), "106.2");
        CHECK_EQUAL(IF4ERR_CANNOT_DELETE, nError);
        delete invoke;
    
        /*-- aimir 계정 삭제 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    
        invoke->AddParam("2.10.2", "aimir");
        nError = Call(invoke->GetHandle(), "106.2");
        CHECK_EQUAL(IF4ERR_CANNOT_DELETE, nError);
        delete invoke;
    
        /*-- 정상적인 삭제 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    
        invoke->AddParam("2.10.2", "UnitTestUser_2");
        nError = Call(invoke->GetHandle(), "106.2");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /*--입력되면 안되는 사용자 명 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    
        invoke->AddParam("2.10.2", "UnitTestUser_0");
        nError = Call(invoke->GetHandle(), "106.2");
        CHECK_EQUAL(IF4ERR_CANNOT_DELETE, nError);
        delete invoke;

        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
    
        invoke->AddParam("2.10.2", "UnitTestUser_1");
        nError = Call(invoke->GetHandle(), "106.2");
        CHECK_EQUAL(IF4ERR_CANNOT_DELETE, nError);
        delete invoke;
    
        /*-- 사용자 목록을 다시 가지고 와서 테스트전과 숫자를 비교 --*/
        invoke = new CIF4Invoke(gServerAddr, 8000, 3);
        nError = Call(invoke->GetHandle(), "106.5");
    
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        CHECK_EQUAL(nPrevUserCnt , invoke->GetHandle()->nResultCount);
        delete invoke;

    }
}


} // End of Suite
