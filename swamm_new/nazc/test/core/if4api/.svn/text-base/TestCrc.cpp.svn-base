
#include <UnitTest++.h>

#include "typedef.h"
#include "IF4CrcCheck.h"
#include "if4frame.h"
#include "MemoryDebug.h"

static BYTE testStr[] = { 
        0x5E, 0x00, 0x80, 0x04, 0x00, 
        0x00, 0x00, 0x00, 0x05, 0x01, 
        0x00, 0x00, 0xD1, 0x8D };

static WORD origCrc = 0x8DD1;


/** core/if4api module test.
 */
SUITE(IF4CrcCheck)
{

/** CRC Check Test.
 *
 */
TEST(IF4GetCrc)
{
    WORD genCrc;
    
    /** Start Flag를 제외하고 CRC 전까지 */
    genCrc = IF4GetCrc(testStr, sizeof(testStr) - sizeof(IF4_TAIL), 0);
    CHECK_EQUAL(origCrc, genCrc);

    genCrc = IF4GetCrc(testStr, sizeof(testStr) - sizeof(IF4_TAIL) - 1, 0);
    CHECK(origCrc != genCrc);
}

TEST(IF4VerifyCrc)
{
    CHECK_EQUAL(TRUE,IF4VerifyCrc(testStr, sizeof(testStr) - 2, 0));
}

} // End of Suite
