
#include <UnitTest++.h>

#include "DebugUtil.h"
#include "rep/DatabaseHelper.h"


struct DbHelperFixture
{
    DbHelperFixture() : 
            dbFile("tTest.db"),
            dbClass("tDbClass")
    {
    }
    ~DbHelperFixture() 
    {
        remove(dbFile);
    }
    const char * dbFile;
    const char * dbClass;
};

/** DatabaseHelper Unit Test.
 *
 * SQLite3 DB를 이용할 수 있게 도와주는 DatabaseHelper를 시험한다.
 * DatabaseHelper를 이용해서 DB Table에 대한 버전 관리를 하며 필요할 경우
 * Upgrade를 위한 절차를 수행할 수 있다.
 *
 */
SUITE(DatabaseHelper)
{

/** CDatabaseHelper Instance가 생성되는지 확인한다.
 */
TEST_FIXTURE(DbHelperFixture, DbHelperInstantiation)
{
    CDatabaseHelper dh(dbFile, dbClass, 1);

    //XDEBUG("+ Test:DhHelperInstantiation\r\n");
    /** 정상적으로 DB 초기화 */
    CHECK_EQUAL(dh.Initialize(), TRUE);
    /** DB 상태 점검 시 정상 */
    CHECK_EQUAL(dh.CheckDB(), TRUE);
    /** Version 정보 확인 */
    CHECK_EQUAL(dh.GetVersion(), 1);
    //XDEBUG("- Test:DhHelperInstantiation\r\n");
}

}

