
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <UnitTest++.h>

#include "codeUtility.h"
#include "types/units.h"
#include "ansiUtils.h"

/** core/protocol/ansi
 */
SUITE(ProtocolAnsiUtil)
{

TEST(ansiUnit2AimirUnit)
{
    CHECK_EQUAL(UNIT_TYPE_WATT_HOUR, ansiUnit2AimirUnit(0));     // Active power
    CHECK_EQUAL(UNIT_TYPE_VOLTAMPEREREACTIVE, ansiUnit2AimirUnit(1));     // Reactive power
    CHECK_EQUAL(UNIT_TYPE_VOLTAMPERE, ansiUnit2AimirUnit(2));     // apparent power
    CHECK_EQUAL(UNIT_TYPE_VOLT, ansiUnit2AimirUnit(8));     // Volts
    CHECK_EQUAL(UNIT_TYPE_UNDEFINED, ansiUnit2AimirUnit(200));     // Undefined
}

TEST(ansiChannel2AimirChannel)
{
    /** SM110 Channel 정보가 AiMiR Channel 정보로 정확하게 Mapping 되는지 확인 */
    CHECK_EQUAL(1, ansiChannel2AimirChannel(0, 5, FALSE, TRUE, FALSE, FALSE, TRUE));
    CHECK_EQUAL(2, ansiChannel2AimirChannel(0, 5, FALSE, FALSE, TRUE, TRUE, FALSE));
    CHECK_EQUAL(3, ansiChannel2AimirChannel(1, 5, FALSE, TRUE, TRUE, FALSE, FALSE));
    CHECK_EQUAL(4, ansiChannel2AimirChannel(1, 5, FALSE, FALSE, FALSE, TRUE, TRUE));
    CHECK_EQUAL(15, ansiChannel2AimirChannel(0, 5, FALSE, TRUE, TRUE, TRUE, TRUE));
    CHECK_EQUAL(16, ansiChannel2AimirChannel(0, 5, TRUE, TRUE, TRUE, TRUE, TRUE));
    CHECK_EQUAL(32, ansiChannel2AimirChannel(8, 2, FALSE, FALSE, FALSE, FALSE, FALSE));

    /** 잘못된 정보에 대해서는 Undefined로 값 리턴 */
    CHECK_EQUAL(0, ansiChannel2AimirChannel(1, 5, FALSE, TRUE, FALSE, FALSE, TRUE));
    CHECK_EQUAL(0, ansiChannel2AimirChannel(0, 5, FALSE, TRUE, TRUE, FALSE, FALSE));
    CHECK_EQUAL(0, ansiChannel2AimirChannel(2, 5, FALSE, TRUE, TRUE, FALSE, FALSE));
    CHECK_EQUAL(0, ansiChannel2AimirChannel(8, 1, FALSE, FALSE, FALSE, FALSE, FALSE));
}


} // End of Suite
