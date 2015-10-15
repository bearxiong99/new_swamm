
#include <UnitTest++.h>

#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"


/** 2. Version Test
 */


SUITE(_02_Version)
{

/** Board & F/W Version Check.
 *
 * sysHwVersion(2.1.8)을 얻어와서 sysSwVersrion(2.1.9)와 맞는 쌍인지 검사한다.
 *
 *      - sysHwVersion (2.1.8)
 *      - sysSwVersion (2.1.9)
 *      - cmdStdGetChild (199.4)
 */
TEST(BoardVersion)
{
    CIF4Invoke  invoke(gServerAddr, 8000, 3);
    int nError;
    float fHwVer, fSwVer;

    {
        UNITTEST_TIME_CONSTRAINT(1000);

        invoke.AddParam("2.1");

        nError = Call(invoke.GetHandle(), "199.4");

        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        CHECK(invoke.GetHandle()->nResultCount > 0);

        fHwVer = (invoke.ResultAtName("sysHwVersion")->stream.u8 >> 4)
            + (invoke.ResultAtName("sysHwVersion")->stream.u8 & 0x0f) * 0.1;
        fSwVer = (invoke.ResultAtName("sysSwVersion")->stream.u8 >> 4)
            + (invoke.ResultAtName("sysSwVersion")->stream.u8 & 0x0f) * 0.1;

        if(fHwVer == 1.2) {
            CHECK_EQUAL((float)3.0, fSwVer); 
        }else if(fHwVer == 2.0) {
            CHECK_EQUAL((float)3.1, fSwVer); 
        }else {
            CHECK(false);
        }
    }

}


} // End of Suite
