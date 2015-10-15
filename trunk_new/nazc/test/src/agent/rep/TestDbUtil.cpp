
#include <UnitTest++.h>

#include "DebugUtil.h"
#include "rep/DbUtil.h"

/** Database Utility 검사.
 *
 *
 */
SUITE(DbUtil)
{

/** DB의 DATE Format을 TIMESTAMP로 변환
 *
 */
TEST(ConvTimeStamp)
{
    const char * v1 = "2012-03-12 23:15:17";
    const char * v2 = "2012-3-12 23:15:17";
    const char * iv1 = "2012--12 23:15:17";
    TIMESTAMP ts;

    //XDEBUG("+ Test:ConvTimeStamp\r\n");
    /** 정상적인 Format과 변환 */
    CHECK_EQUAL(TRUE, ConvTimeStamp(v1, &ts));
    CHECK_EQUAL(2012, ts.year); CHECK_EQUAL(3, ts.mon); CHECK_EQUAL(12, ts.day);
    CHECK_EQUAL(23, ts.hour); CHECK_EQUAL(15, ts.min); CHECK_EQUAL(17, ts.sec);
    CHECK_EQUAL(TRUE, ConvTimeStamp(v2, &ts));
    CHECK_EQUAL(2012, ts.year); CHECK_EQUAL(3, ts.mon); CHECK_EQUAL(12, ts.day);
    CHECK_EQUAL(23, ts.hour); CHECK_EQUAL(15, ts.min); CHECK_EQUAL(17, ts.sec);

    /** 비정상적 Format 변환 */
    CHECK_EQUAL(FALSE, ConvTimeStamp(iv1, &ts));
    CHECK_EQUAL(FALSE, ConvTimeStamp(NULL, &ts));
    CHECK_EQUAL(FALSE, ConvTimeStamp(v1, NULL));
    //XDEBUG("- Test:ConvTimeStamp\r\n");
}

}

