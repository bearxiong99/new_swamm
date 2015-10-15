
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <UnitTest++.h>

#include "typedef.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"


struct FileFixture
{
    FileFixture() : 
            validFile("validFile"),
            invalidFile("/invalid/path/invalidFile"),
            validDirectory("."),
            invalidDirectory("/invalid/path")
    {
        FILE * fp = NULL;
        
        fp = fopen(validFile, "wb");
        CHECK(fp != NULL);
        if(fp != NULL) fclose(fp);
    }
    ~FileFixture() 
    {
        remove(validFile);
    }
    const char * validFile;
    const char * invalidFile;
    const char * validDirectory;
    const char * invalidDirectory;
};

struct EUI64Fixture
{
    EUI64Fixture() : idstr("FFA0004C125D820A")
    {
        eui64id.ids[0] = eui64id_r.ids[7] = 0xFF;
        eui64id.ids[1] = eui64id_r.ids[6] = 0xA0;
        eui64id.ids[2] = eui64id_r.ids[5] = 0x00;
        eui64id.ids[3] = eui64id_r.ids[4] = 0x4C;
        eui64id.ids[4] = eui64id_r.ids[3] = 0x12;
        eui64id.ids[5] = eui64id_r.ids[2] = 0x5D;
        eui64id.ids[6] = eui64id_r.ids[1] = 0x82;
        eui64id.ids[7] = eui64id_r.ids[0] = 0x0A;
    }
    ~EUI64Fixture() {}

    EUI64 eui64id;
    EUI64 eui64id_r;
    const char * idstr;
};

/** core/common module test.
 */
SUITE(CommonUtil)
{

/** MCU_STRDUP Test.
 *
 */
TEST(MCU_STRDUP)
{
    const char *testStr = "01234567890 abc";
    char *resStr = NULL;

    resStr = MCU_STRDUP(testStr);

    CHECK(resStr != NULL);
    CHECK(strlen(testStr) == strlen(resStr));
    CHECK_EQUAL(0, strcmp(testStr, resStr));

    FREE(resStr);

    resStr = MCU_STRDUP(NULL);
    CHECK(resStr == NULL);
}

/** IsExists 가 File/Direcotry에 대해 정상 동작하는지 검사.
  *
  */
TEST_FIXTURE(FileFixture, IsExists)
{
    CHECK_EQUAL(0, IsExists(validDirectory));
    CHECK_EQUAL(1, IsExists(validFile));
    CHECK_EQUAL(0, IsExists(invalidDirectory));
    CHECK_EQUAL(0, IsExists(invalidFile));
}

TEST(strnstr)
{
    char *source = const_cast<char *>("0123456789");
    const char *tag = "567";
    const char *long_tag = "01234567890123";
    char *res = NULL;

    res = strnstr(source, tag, strlen(source));
    CHECK_EQUAL((source + 5), res);

    res = strnstr(source, long_tag, strlen(source));
    CHECK(res == NULL);

    res = strnstr(source, tag, 6);
    CHECK(res == NULL);

    res = strnstr(NULL, tag, 0);
    CHECK(res == NULL);

    res = strnstr(source, NULL, strlen(source));
    CHECK(res == NULL);
}

/** GetTimeInterval 함수 테스트.
  *
  * 여기서 GetTimeInterval 함수의 prev, curr 값이 바껴서 들어갔을 때
  * 음수값이 나오는 것이 맞는것인지 절대값(차이값)이 나오는 것이 맞는
  * 것인지 고려해 봐야 한다. 일단 테스트에서는 음수값이 나오는것이
  * 맞다고 생각했다.(현재 구현과 일치한다)
  */
TEST(GetTimeInterval)
{
    TIMETICK prev;
    TIMETICK curr;
    int res;
    int gap = 500;

    gettimeofday(&prev.t,NULL);
    UnitTest::TimeHelpers::SleepMs(gap);
    gettimeofday(&curr.t,NULL);

    res = GetTimeInterval(&prev, &curr);
    CHECK_CLOSE(res, gap, 10);

    res = GetTimeInterval(&curr, &prev);
    CHECK_CLOSE(res, -gap, 10);
}

TEST_FIXTURE(FileFixture, GetFileValue_SetFileValue)
{
    int v1 = 11;
    int v2 = INT_MAX;
    int v3 = INT_MIN;
    int res = 0;

    /*-- 정상적인 데이터 저장 --*/
    CHECK_EQUAL(1, SetFileValue(validFile, v1));
    CHECK_EQUAL(1, GetFileValue(validFile, &res));
    CHECK_EQUAL(v1, res);

    /*-- 잘못된 파일에 저장 안되야 함 --*/
    CHECK_EQUAL(0, SetFileValue(invalidFile, v1));
    CHECK_EQUAL(0, GetFileValue(invalidFile, &res));

    /*-- 디렉토리에서 읽기가 실패해야 함  --*/
    CHECK_EQUAL(0, SetFileValue(validDirectory, v1));
    CHECK_EQUAL(0, GetFileValue(validDirectory, &res));

    CHECK_EQUAL(1, SetFileValue(validFile, v2));
    CHECK_EQUAL(1, GetFileValue(validFile, &res));
    CHECK_EQUAL(v2, res);

    CHECK_EQUAL(1, SetFileValue(validFile, v3));
    CHECK_EQUAL(1, GetFileValue(validFile, &res));
    CHECK_EQUAL(v3, res);
}

TEST_FIXTURE(FileFixture, GetFileString_SetFileString)
{
    const char *size10 = "0123456789";
    const char *size10cr = "0123456789\r\n";
    const char *size120 = 
        "0123456789012345678901234567890123456789" 
        "0123456789012345678901234567890123456789"
        "0123456789012345678901234567890123456789";
    char buff[64];

    /** 정상적인 데이터 저장 */
    remove(validFile);
    CHECK_EQUAL(1, SetFileString(validFile, size10));
    CHECK_EQUAL(1, GetFileString(validFile, buff, sizeof(buff), size10, TRUE));
    CHECK_EQUAL(0, strcmp(size10, buff));

    /** 잘못된 파일에 저장 안되야 함 */
    CHECK_EQUAL(0, SetFileString(invalidFile, size10));
    CHECK_EQUAL(0, GetFileString(invalidFile, buff, sizeof(buff), size10, TRUE));

    /** 디렉토리에서 읽기가 실패해야 함  */
    CHECK_EQUAL(0, SetFileString(validDirectory, size10));
    CHECK_EQUAL(0, GetFileString(validDirectory, buff, sizeof(buff), size10, TRUE));

    /** \r, \n 이 있을 때 bClear가 TRUE 면 제거해줘야 한다 */
    remove(validFile);
    CHECK_EQUAL(1, SetFileString(validFile, size10cr));
    CHECK_EQUAL(1, GetFileString(validFile, buff, sizeof(buff), size10cr, TRUE));
    CHECK_EQUAL(0, strcmp(size10, buff));

    /** 너무 큰 데이터는 읽지 못해야 한다.
      * 큰 데이터를 쓸 수는 있지만 Buffer size 가 데이터 보다 작을 경우
      * 읽을 수 있는 만큼 읽고 0을 리턴해야 한다. */
    remove(validFile);
    CHECK_EQUAL(1, SetFileString(validFile, size120));
    CHECK_EQUAL(0, GetFileString(validFile, buff, sizeof(buff), NULL, TRUE));
    CHECK_EQUAL(0, strncmp(size120, buff, sizeof(buff) - 1));
}

/** uptime 함수를 검사한다.
  */
TEST(uptime)
{
    int up1, up2;
    int gap = 1000;

    up1 = uptime();
    UnitTest::TimeHelpers::SleepMs(gap);
    up2 = uptime();

    /** Uptime을 호출 후 1sec 후에 호출 했을 때
      * 정확하게 일치해야 한다.
      */
    CHECK_EQUAL(1, up2 - up1);
}

/** EUI64ToStr 함수 검사.
  */
TEST_FIXTURE(EUI64Fixture, EUI64ToStr)
{
    char buff[16] = {0,};
    char ebuff[16] = {0,};

    {
        /** EUI64 id 가 NULL 이면 Buffer 그대로 리턴되어야 한다. */
        memset(buff,0,sizeof(buff));
        memcpy(ebuff,buff,sizeof(buff));
        CHECK_EQUAL(FALSE, EUI64ToStr(NULL,buff));
        CHECK_EQUAL(0, memcmp(buff,ebuff,sizeof(buff)));
    }

    {
        /** Buffer가 NULL 이어도 실패해야 한다 */
        CHECK_EQUAL(FALSE, EUI64ToStr(&eui64id, NULL));
    }

    {
        /** 정상값을 넘기면 맞게 String으로 만들어 줘야 한다 */
        memset(buff,0,sizeof(buff));
        CHECK_EQUAL(TRUE, EUI64ToStr(&eui64id, buff));
        CHECK(strncmp(idstr, buff, strlen(idstr)) == 0);
    }
}

/** StrToEUI64 함수 검사.
  */
TEST_FIXTURE(EUI64Fixture, StrToEUI64)
{
    const char *outOfRangeId = "ABCDEFGHI0123456";
    const char *tooShortId = "ABCDEF012";
    EUI64 reui64;

    {
        /** 한쪽이라도 NULL 이면 실패 */
        CHECK_EQUAL(FALSE, StrToEUI64(NULL, &reui64));
        CHECK_EQUAL(FALSE, StrToEUI64(idstr, NULL));
    }

    {
        /** 잘못된 ID String이 전달되면 실패해야 한다 */
        CHECK_EQUAL(FALSE, StrToEUI64(outOfRangeId, &reui64));
        CHECK_EQUAL(FALSE, StrToEUI64(tooShortId, &reui64));
    }

    { 
        /** 정상적인 ID에 대한 검사는 정상적으로 되어야 한다 */
        memset(&reui64, 0, sizeof(EUI64));
        CHECK_EQUAL(TRUE, StrToEUI64(idstr, &reui64));
        CHECK(memcmp(&reui64, &eui64id, sizeof(EUI64)) == 0);
    }
}

TEST(BigToHostShort_BigToHostInt)
{
    /** SHORT */
    CHECK_EQUAL(0x5000, BigToHostShort(0x0050));

    /** INT */
    CHECK_EQUAL(0xAF074022, BigToHostInt(0x224007AF));
}

TEST(LittleToHostShort_LittleToHostInt)
{
#ifdef  __BIG_ENDIAN_SYSTEM__
    /** SHORT */
    CHECK_EQUAL(0x5000, LittleToHostShort(0x0050));

    /** INT */
    CHECK_EQUAL(0xAF074022, LittleToHostInt(0x224007AF));
#else
    /** SHORT */
    CHECK_EQUAL(0x0050, LittleToHostShort(0x0050));

    /** INT */
    CHECK_EQUAL((UINT)0x224007AF, LittleToHostInt((UINT)0x224007AF));
#endif
}

TEST(HostToLittleShort_HostToLittleInt)
{
#ifdef  __BIG_ENDIAN_SYSTEM__
    /** SHORT */
    CHECK_EQUAL(0x5000, HostToLittleShort(0x0050));

    /** INT */
    CHECK_EQUAL(0xAF074022, HostToLittleInt(0x224007AF));
#else
    /** SHORT */
    CHECK_EQUAL(0x0050, HostToLittleShort(0x0050));

    /** INT */
    CHECK_EQUAL((UINT)0x224007AF, HostToLittleInt((UINT)0x224007AF));
#endif
}

/** ReverseEUI64
  */
TEST_FIXTURE(EUI64Fixture, ReverseEUI64)
{
    EUI64 source, target;

    {
        /** NULL이 전달될 경우 실패 해야 한다 */
        CHECK_EQUAL(FALSE, ReverseEUI64(NULL,&target));
        CHECK_EQUAL(FALSE, ReverseEUI64(&source, NULL));
        CHECK_EQUAL(FALSE, ReverseEUI64(NULL, NULL));
    }

    {
        /** Source는 Target으로 반전되어야 하며 Source 자체 값은 변경이 없어야 한다 */
        CHECK(memcpy(&source, &eui64id, sizeof(EUI64)) == &source);
        CHECK_EQUAL(TRUE, ReverseEUI64(&source, &target));
        CHECK(memcmp(&eui64id_r, &target, sizeof(EUI64)) == 0);
        CHECK(memcmp(&eui64id, &source, sizeof(EUI64)) == 0);
    }
}

/** strip
  */
TEST(strip)
{
    char * res;

    {
        /** 정상적인 strip 확인 */
        res = strip(" ab cd "); CHECK(strcmp("ab cd", res) == 0); FREE(res);
        res = strip(NULL); CHECK(res == NULL);
        res = strip(""); CHECK(res != NULL); CHECK(strlen(res) == 0); FREE(res);
    }
}

/** stripCpy
  */
TEST(stripCpy)
{
    char dst[8];

    {
        stripCpy(dst, " ab cd "); CHECK(strcmp("ab cd", dst) == 0);
        stripCpy(dst, ""); CHECK(strlen(dst) == 0);
        memset(dst, 0, sizeof(dst)); stripCpy(dst, NULL); CHECK(strlen(dst) == 0);
    }
}

/** stripSafeCpy
  */
TEST(stripTestCpy)
{
    char dst[8];
    {
        stripSafeCpy(dst, sizeof(dst), " ab cd "); CHECK(strcmp("ab cd", dst) == 0);
        stripSafeCpy(dst, sizeof(dst), ""); CHECK(strlen(dst) == 0);
        memset(dst, 0, sizeof(dst)); stripSafeCpy(dst, sizeof(dst), NULL); CHECK(strlen(dst) == 0);
        stripSafeCpy(dst, sizeof(dst), " ab cd 1234567 "); CHECK(strcmp("ab cd 1", dst) == 0);
    }
}

/** MakeTime & GetTimestamp
  */
TEST(MakeTime_GetTimestamp)
{
    TIMESTAMP timeStamp;
    cetime_t nCeTime=0;

    /** 현재 시간을 얻어온 후
      * GetTimestamp로 설정하고 MakeTime으로 다시 역변했을 때
      * 같은 값이 나와야 한다.
      */
    ceTime(&nCeTime);
    memset(&timeStamp, 0, sizeof(TIMESTAMP));
    GetTimestamp(&timeStamp,&nCeTime);
    CHECK_EQUAL(nCeTime, MakeTime(&timeStamp));
}


/** BigStreamToHostLong, LittleStreamToHostLong
 */
TEST(BigStreamToHostLong)
{
    BYTE stream8[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    CHECK_EQUAL((unsigned long long)0x01,               BigStreamToHostLong(stream8, 1));
    CHECK_EQUAL((unsigned long long)0x0102,             BigStreamToHostLong(stream8, 2));
    CHECK_EQUAL((unsigned long long)0x010203,           BigStreamToHostLong(stream8, 3));
    CHECK_EQUAL((unsigned long long)0x01020304,         BigStreamToHostLong(stream8, 4));
    CHECK_EQUAL((unsigned long long)0x0102030405,       BigStreamToHostLong(stream8, 5));
    CHECK_EQUAL((unsigned long long)0x010203040506,     BigStreamToHostLong(stream8, 6));
    CHECK_EQUAL((unsigned long long)0x01020304050607,   BigStreamToHostLong(stream8, 7));
    CHECK_EQUAL((unsigned long long)0x0102030405060708, BigStreamToHostLong(stream8, 8));

    CHECK_EQUAL((unsigned long long)0x01,               LittleStreamToHostLong(stream8, 1));
    CHECK_EQUAL((unsigned long long)0x0201,             LittleStreamToHostLong(stream8, 2));
    CHECK_EQUAL((unsigned long long)0x030201,           LittleStreamToHostLong(stream8, 3));
    CHECK_EQUAL((unsigned long long)0x04030201,         LittleStreamToHostLong(stream8, 4));
    CHECK_EQUAL((unsigned long long)0x0504030201,       LittleStreamToHostLong(stream8, 5));
    CHECK_EQUAL((unsigned long long)0x060504030201,     LittleStreamToHostLong(stream8, 6));
    CHECK_EQUAL((unsigned long long)0x07060504030201,   LittleStreamToHostLong(stream8, 7));
    CHECK_EQUAL((unsigned long long)0x0807060504030201, LittleStreamToHostLong(stream8, 8));
}

/** HostIntToBigStream, HostIntToLittleStream
  */
TEST(HostIntToStream)
{
    UINT intValue = 0x302010;
    BYTE stream[32];
    BYTE big[4] = {0x00, 0x30, 0x20, 0x10};
    BYTE little[4] = {0x10, 0x20, 0x30, 0x00};

    memset(stream, 0, sizeof(stream));
    CHECK(HostIntToBigStream(intValue, stream) != NULL);
    CHECK_EQUAL(0, memcmp(stream, big, sizeof(big)));

    memset(stream, 0, sizeof(stream));
    CHECK(HostIntToLittleStream(intValue, stream) != NULL);
    CHECK_EQUAL(0, memcmp(stream, little, sizeof(little)));
}

} // End of Suite
