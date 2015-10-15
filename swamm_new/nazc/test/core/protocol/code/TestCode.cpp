
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <UnitTest++.h>

#include "codeUtility.h"
#include "types/units.h"

/** core/tostring module test.
 */
SUITE(ProtocolCode)
{

TEST(GetUnitStr)
{
    CHECK_EQUAL(0, strcmp(GetUnitStr(UNIT_TYPE_WATT), "W"));
    CHECK_EQUAL(0, strcmp(GetUnitStr(UNIT_TYPE_UNDEFINED), ""));
    CHECK_EQUAL(0, strcmp(GetUnitStr(UNIT_TYPE_WATT_HOUR), "Wh"));
}

TEST(GetChannelStr)
{
    CHECK_EQUAL(0, strcmp(GetChannelStr(1), "Sum Li Active power+ (QI+QIV)"));
}


} // End of Suite
