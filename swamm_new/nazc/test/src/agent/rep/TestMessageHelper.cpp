
#include <UnitTest++.h>

#include "typedef.h"
#include "varapi.h"
#include "DebugUtil.h"
#include "rep/MessageHelper.h"

struct DbMessageHelperFixture
{
    DbMessageHelperFixture() : 
            dbFile("tTest.db"),
            targetId("0123456789ABCDEF0")
    {
    }
    ~DbMessageHelperFixture() 
    {
        remove(dbFile);
    }
    const char * dbFile;
    const char * targetId;
};

/** MessageHelper Unit Test.
 *
 * DCU를 통해 Device에 Message를 전송할 때 사용하는 MessageTbl에 대한
 * Helper 검사.
 *
 */
SUITE(MessageHelper)
{

/** 신규 메시지를 생성하고 삭제해 본다.
 *
 */
TEST_FIXTURE(DbMessageHelperFixture, MessageAddSelectDelete)
{
    CMessageHelper dh(dbFile);
    IF4Wrapper wrapper;

    /** 정상적으로 DB 초기화 */
    CHECK_EQUAL(dh.Initialize(), TRUE);
    CHECK_EQUAL(dh.CheckDB(), TRUE);

    /** 잘못된 Delete에 대해서 Delete 요청 시에도 정상적으로 성공해야 한다 */
    // Message가 없을 때 삭제 시도 시 성공
    CHECK_EQUAL(dh.Delete(targetId, MESSAGE_TYPE_ALL), TRUE);

    /** 새로운 Message 생성 */
    CHECK_EQUAL(
        dh.Add(
                targetId,   // Target ID
                1,          // Message ID
                MESSAGE_TYPE_PASSIVE,  // Message Type (Passive 3)
                60,         // Duration (60 sec)
                0,          // Error Handler
                0,          // Pre Handler
                0,          // Post Handler
                9,          // User Data
                2,          // Message Length
                (const BYTE *)"AB"        // Message
            )
        , TRUE);

    /** 1개 이상의 Message가 있어야 한다 */
    memset(&wrapper, 0, sizeof(IF4Wrapper));
    CHECK_EQUAL(dh.Select(targetId, MESSAGE_TYPE_PASSIVE, &wrapper), TRUE);
    CHECK(wrapper.nResult > 0);
    VARAPI_FreeNode(wrapper.pResult);

    /** Message 삭제 성공. Message가 0개 여야 한다 */
    memset(&wrapper, 0, sizeof(IF4Wrapper));
    CHECK_EQUAL(dh.Delete(targetId, MESSAGE_TYPE_PASSIVE), TRUE);
    CHECK_EQUAL(dh.Select(targetId, MESSAGE_TYPE_PASSIVE, &wrapper), TRUE);
    CHECK_EQUAL(0, wrapper.nResult);
    VARAPI_FreeNode(wrapper.pResult);

}

}

