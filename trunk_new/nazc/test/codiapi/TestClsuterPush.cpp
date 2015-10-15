
#include <UnitTest++.h>

#include "endiDefine.h"

/** Issue #84
 *
 * Push Metering을 위해 새롭개 정의된 Cluster 0x1B에 대한 테스트
 *
 */
SUITE(ClusterPushMetering)
{

/** Push Metering이 정의되어 있는지 확인
 */
TEST(ClusterDefinition)
{
    CHECK_EQUAL(0x1B, ENDI_DATATYPE_PUSH);
}

}

