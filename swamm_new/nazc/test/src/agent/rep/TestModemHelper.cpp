
#include <UnitTest++.h>

#include "types/identification.h"
#include "mcudef.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"
#include "rep/ModemHelper.h"
#include "rep/MeterHelper.h"
#include "rep/ChannelCfgHelper.h"
#include "rep/MeteringHelper.h"
#include "rep/LogHelper.h"

struct DbDeviceFixture
{
    DbDeviceFixture() : 
            dbFile("tTest.db"),
            targetId("0123456789ABCDEF"),
            model("NZR-I121"),
            hwVer(1.0),
            swVer(1.1),
            build(2)
    {
    }
    ~DbDeviceFixture() 
    {
        remove(dbFile);
    }
    const char * dbFile;
    const char * targetId;
    const char * model;
    const double hwVer;
    const double swVer;
    const unsigned char build;
};

/** ModemHelper Unit Test.
 *
 * Modem을 추가/갱신하기 위한 Helper 시험.
 *
 */
SUITE(ModemHelper)
{

/** Modem 객체의 생성/갱신
 *
 */
TEST_FIXTURE(DbDeviceFixture, ModemInventoryUpdate)
{
    EUI64 id;
    UINT _oldId = 0;
    DBFetchData fetchData;
    MODEMTBLINS *instance;

    CModemHelper dh(dbFile);

    //XDEBUG("+ Test:ModemInventoryUpdate\r\n");
    memset(&fetchData, 0, sizeof(DBFetchData));

    /** 정상적으로 DB 초기화 */
    CHECK_EQUAL(dh.Initialize(), TRUE);
    CHECK_EQUAL(dh.CheckDB(), TRUE);

    StrToEUI64(targetId, &id);

    //XDEBUG("  * Check Delete Row\r\n");
    /** 일단 삭제부터 한다.  */
    CHECK_EQUAL(TRUE, dh.Delete(targetId));

    //XDEBUG("  * Check Select Row\r\n");
    /** DB Select 시 대상이 없어야 한다 */
    CHECK_EQUAL(TRUE, dh.Select(&id, &fetchData));
    CHECK_EQUAL(0, fetchData.nCount);
    dh.FreeResult(&fetchData);

    //XDEBUG("  * Check Create Row\r\n");
    /** 새로운 Modem 생성
     */
    CHECK_EQUAL(TRUE, dh.Update(targetId, model, hwVer, swVer, build));

    //XDEBUG("  * Check Select Row\r\n");
    /** DB Select 시 대상이 1개 있어야 한다 */
    CHECK_EQUAL(TRUE, dh.Select(&id, &fetchData));
    CHECK_EQUAL(1, fetchData.nCount);
    if((instance = dh.GetData(dh.GetHeadRow(&fetchData))) != NULL) 
    {
        _oldId = instance->_id;
    }
    dh.FreeResult(&fetchData);

    //XDEBUG("  * Check Update Row\r\n");
    /** 기존  Modem 갱신 
     */
    CHECK_EQUAL(TRUE, 
        dh.Update(targetId, model, hwVer, swVer, build + 1));

    //XDEBUG("  * Check Select Row\r\n");
    /** DB Select 시 대상이 1개 있어야 한다 */
    CHECK_EQUAL(TRUE, dh.Select(targetId, &fetchData));
    CHECK_EQUAL(1, fetchData.nCount);

    //XDEBUG("  * Check Select Row\r\n");
    /** ID가 변경되면 안된다 */
    if((instance = dh.GetData(dh.GetHeadRow(&fetchData))) != NULL) 
    {
        CHECK_EQUAL(_oldId, instance->_id);
    }
    dh.FreeResult(&fetchData);

    //XDEBUG("  * Check Delete Row\r\n");
    /** Modem을 삭제한다 */
    CHECK_EQUAL(TRUE, dh.Delete(&id));

    //XDEBUG("  * Check Select Row\r\n");
    /** DB Select 시 대상이 없어야 한다 */
    CHECK_EQUAL(TRUE, dh.Select(targetId, &fetchData));
    CHECK_EQUAL(0, fetchData.nCount);
    dh.FreeResult(&fetchData);

    //XDEBUG("- Test:ModemInventoryUpdate\r\n");
}

}


/** MeterHelper Unit Test.
 *
 * Modem과 연결되는 MeterTlb 정보는 Modem Instance가 필요하기 때문에
 * 같은 곳에서 Test 한다.
 *
 */
SUITE(MeterHelper)
{

/** Modem/Meter가 동시에 Open 초기화 되어도 문제가 없는지 검사.
 *
 */
TEST_FIXTURE(DbDeviceFixture, MeterCreate)
{
    EUI64 id;
    DBFetchData fetchData;
    //METERTBLINS *instance;

    //XDEBUG("+ Test:MeterCreate\r\n");
    memset(&fetchData, 0, sizeof(DBFetchData));

    CModemHelper modh(dbFile);
    CMeterHelper medh(dbFile);

    /** 정상적으로 DB 초기화 */
    CHECK_EQUAL(modh.Initialize(), TRUE);
    CHECK_EQUAL(medh.Initialize(), TRUE);
    CHECK_EQUAL(modh.CheckDB(), TRUE);
    CHECK_EQUAL(medh.CheckDB(), TRUE);

    StrToEUI64(targetId, &id);

    /** 새로운 Modem 생성
     */
    CHECK_EQUAL(TRUE, modh.Update(&id, model, hwVer, swVer, build));

    /** 새로운 Meter 생성
     */
    CHECK_EQUAL(TRUE, medh.Update(targetId, 0,
                "AB", PARSER_TYPE_REPEATER, SERVICE_TYPE_ELEC, METER_VENDOR_NURI, hwVer, swVer, 4));

    /** Select 해서 1이 나와야 함 */
    CHECK_EQUAL(TRUE, medh.Select(targetId, 0, &fetchData));
    CHECK_EQUAL(1, fetchData.nCount);
    medh.FreeResult(&fetchData);

    /** 추가 Meter
     */
    CHECK_EQUAL(TRUE, medh.Update(targetId, 1,
                "CD", PARSER_TYPE_REPEATER, SERVICE_TYPE_ELEC, METER_VENDOR_NURI, hwVer, swVer, 4));
    /** Select 해서 2이 나와야 함 */
    CHECK_EQUAL(TRUE, medh.Select(targetId, -1, &fetchData));
    CHECK_EQUAL(2, fetchData.nCount);
    medh.FreeResult(&fetchData);

    /** Delete */
    CHECK_EQUAL(TRUE, medh.Delete(&id, 0));
    /** Select 해서 1이 나와야 함 */
    CHECK_EQUAL(TRUE, medh.Select(targetId, -1, &fetchData));
    CHECK_EQUAL(1, fetchData.nCount);
    medh.FreeResult(&fetchData);

    /** Modem 삭제 시 Meter도 같이 삭제 되어야 함 */
    CHECK_EQUAL(TRUE, modh.Delete(targetId));
    /** Select 해서 0이 나와야 함 */
    CHECK_EQUAL(TRUE, medh.Select(targetId, -1, &fetchData));
    CHECK_EQUAL(0, fetchData.nCount);
    medh.FreeResult(&fetchData);

    //XDEBUG("- Test:MeterCreate\r\n");
}

}

/** ChannelCfgHelper Unit Test.
 *
 * Channel 정보는 Meter와 Modem을 필요로 하기 때문에 
 * 같은 곳에서 Test 한다.
 *
 */
SUITE(ChannelCfgHelper)
{

/** ChannelCfg 검사
 *
 */
TEST_FIXTURE(DbDeviceFixture, ChannelCfgCreate)
{
    EUI64 id;
    DBFetchData fetchData;
    BYTE nChIdx = 1;

    //XDEBUG("+ Test:ChannelCfgCreate\r\n");
    memset(&fetchData, 0, sizeof(DBFetchData));

    CModemHelper modh(dbFile);
    CMeterHelper medh(dbFile);
    CChannelCfgHelper ccdh(dbFile);

    /** 정상적으로 DB 초기화 */
    CHECK_EQUAL(modh.Initialize(), TRUE);
    CHECK_EQUAL(medh.Initialize(), TRUE);
    CHECK_EQUAL(ccdh.Initialize(), TRUE);
    CHECK_EQUAL(modh.CheckDB(), TRUE);
    CHECK_EQUAL(medh.CheckDB(), TRUE);
    CHECK_EQUAL(ccdh.CheckDB(), TRUE);

    StrToEUI64(targetId, &id);

    /** 새로운 Modem 생성
     */
    CHECK_EQUAL(TRUE, modh.Update(&id, model, hwVer, swVer, build));

    /** 새로운 Meter 생성
     */
    CHECK_EQUAL(TRUE, medh.Update(targetId, 0,
                "AB", PARSER_TYPE_REPEATER, SERVICE_TYPE_ELEC, METER_VENDOR_NURI, hwVer, swVer, 4));

    /** 새로운 Channel 생성
     */
    CHECK_EQUAL(TRUE, ccdh.Update(targetId, 0, 
                nChIdx,  // chidx
                30, // Unit Wh
                0,  // sigExp
                1,  // Diff
                1,  // ObjType Electricity
                1   // ChType
            ));

    /** Select 해서 1이 나와야 함 */
    CHECK_EQUAL(TRUE, ccdh.Select(targetId, 0, nChIdx, TRUE, &fetchData));
    CHECK_EQUAL(1, fetchData.nCount);
    ccdh.FreeResult(&fetchData);

    /** 기존 Channle 갱신
     *  Channel Configuration이 없어지는 것이 아니라 새로운것을 등록한다.
     */
    CHECK_EQUAL(TRUE, ccdh.Update(targetId, 0, 
                nChIdx,  // chidx
                30, // Unit Wh
                0,  // sigExp
                1,  // Diff
                1,  // ObjType Electricity
                2   // ChType
            ));

    /** Select 해서 1이 나와야 함  : 최신 Configuration */
    CHECK_EQUAL(TRUE, ccdh.Select(targetId, 0, nChIdx, TRUE, &fetchData));
    CHECK_EQUAL(1, fetchData.nCount);
    ccdh.FreeResult(&fetchData);
    /** Select 해서 2이 나와야 함  : 모든 Configuration */
    CHECK_EQUAL(TRUE, ccdh.Select(targetId, 0, nChIdx, FALSE, &fetchData));
    CHECK_EQUAL(2, fetchData.nCount);
    ccdh.FreeResult(&fetchData);

    /** 추가 Channel
     */
    CHECK_EQUAL(TRUE, ccdh.Update(targetId, 0, 
                nChIdx+1,  // chidx
                30, // Unit Wh
                0,  // sigExp
                1,  // Diff
                1,  // ObjType Electricity
                2   // ChType
            ));

    /** Select 해서 2이 나와야 함 */
    CHECK_EQUAL(TRUE, ccdh.Select(targetId, 0, TRUE, &fetchData));
    CHECK_EQUAL(2, fetchData.nCount);
    ccdh.FreeResult(&fetchData);

    /** Delete */
    CHECK_EQUAL(TRUE, ccdh.Delete(&id, 0, nChIdx));
    /** Select 해서 1이 나와야 함 */
    CHECK_EQUAL(TRUE, ccdh.Select(targetId, 0, TRUE, &fetchData));
    CHECK_EQUAL(1, fetchData.nCount);
    ccdh.FreeResult(&fetchData);

    /** Modem 삭제 시 Meter, Channel도 같이 삭제 되어야 함 */
    CHECK_EQUAL(TRUE, modh.Delete(targetId));
    /** Select 해서 0이 나와야 함 */
    CHECK_EQUAL(TRUE, ccdh.Select(targetId, 0, TRUE, &fetchData));
    CHECK_EQUAL(0, fetchData.nCount);
    ccdh.FreeResult(&fetchData);

    //XDEBUG("- Test:ChannelCfgCreate\r\n");
}

}

/** MeteringHelper Unit Test.
 *
 * 테스트의 효율을 위해 이곳에 코드를 넣는다.
 *
 * MeteringTbl, LoadProfileTbl, ChannelTbl, BasePulseTbl 관련 내용 테스트
 *
 */
SUITE(MeteringHelper)
{

TEST_FIXTURE(DbDeviceFixture, MeteringSave)
{
    EUI64 id;
    DBFetchData fetchData;
    BYTE nChIdx = 0;
    TIMESTAMP meterTime, baseTime;
    BASEPULSEINS basePulseIns[2];
    LOADPROFILEINS loadProfileIns[2];
    //METERINGDATAINS *pSelected;

    //XDEBUG("+ Test:MeteringSave\r\n");
    memset(&fetchData, 0, sizeof(DBFetchData));

    CModemHelper modh(dbFile);
    CMeterHelper medh(dbFile);
    CChannelCfgHelper ccdh(dbFile);
    CMeteringHelper mdh(dbFile);

    /** 정상적으로 DB 초기화 */
    CHECK_EQUAL(modh.Initialize(), TRUE);
    CHECK_EQUAL(medh.Initialize(), TRUE);
    CHECK_EQUAL(ccdh.Initialize(), TRUE);
    CHECK_EQUAL(mdh.Initialize(), TRUE);
    CHECK_EQUAL(modh.CheckDB(), TRUE);
    CHECK_EQUAL(medh.CheckDB(), TRUE);
    CHECK_EQUAL(ccdh.CheckDB(), TRUE);
    CHECK_EQUAL(mdh.CheckDB(), TRUE);

    StrToEUI64(targetId, &id);

    /** 새로운 Modem 생성
     */
    CHECK_EQUAL(TRUE, modh.Update(&id, model, hwVer, swVer, build));

    /** 새로운 Meter 생성
     */
    CHECK_EQUAL(TRUE, medh.Update(targetId, 0,
                "AB", PARSER_TYPE_REPEATER, SERVICE_TYPE_ELEC, METER_VENDOR_NURI, hwVer, swVer, 4));

    /** 새로운 Channel 생성
     */
    CHECK_EQUAL(TRUE, ccdh.Update(targetId, 0, 
                nChIdx,  // chidx
                30, // Unit Wh
                0,  // sigExp
                1,  // Diff
                1,  // ObjType Electricity
                1   // ChType
            ));

    /** 추가 Channel
     */
    CHECK_EQUAL(TRUE, ccdh.Update(targetId, 0, 
                nChIdx+1,  // chidx
                30, // Unit Wh
                0,  // sigExp
                1,  // Diff
                1,  // ObjType Electricity
                2   // ChType
            ));

    /** Metering Data Insert
    */
    GetTimestamp(&meterTime, NULL);
    GetTimestamp(&baseTime, NULL);
    memset(basePulseIns, 0, sizeof(BASEPULSEINS)*2);
    memset(loadProfileIns, 0, sizeof(LOADPROFILEINS)*2);

    // 두개의 Channel을 구분하기 위해 값 지정
    basePulseIns[1].nChIndex = nChIdx+1;
    loadProfileIns[1].nChIndex = nChIdx+1;

    CHECK_EQUAL(TRUE, mdh.Update(targetId, 0,
                &meterTime,     // Meter Time
                0,              // Meter Time Offset
                &baseTime,      // Base pulse Time
                2,              // Channel Count
                basePulseIns,
                2,
                loadProfileIns
            ));

    /** Select 해서 1이 나와야 함 */
    CHECK_EQUAL(TRUE, mdh.Select(NULL, 0, 
                NULL, NULL,
                &fetchData));
    CHECK_EQUAL(1, fetchData.nCount);
    CHECK(mdh.GetData(mdh.GetHeadRow(&fetchData)) != NULL);
    CHECK_EQUAL(2, mdh.GetData(mdh.GetHeadRow(&fetchData))->nChannelCnt);
    CHECK_EQUAL(2, mdh.GetData(mdh.GetHeadRow(&fetchData))->nTotLpCnt);
    mdh.DumpResult(&fetchData);
    ccdh.FreeResult(&fetchData);

    /** Modem 삭제 시 Meter, Channel, MeteringData도 같이 삭제 되어야 함 */
    CHECK_EQUAL(TRUE, modh.Delete(targetId));

    /** Select 해서 0이 나와야 함 */
    CHECK_EQUAL(TRUE, mdh.Select(targetId, 0, &fetchData));
    CHECK_EQUAL(0, fetchData.nCount);
    ccdh.FreeResult(&fetchData);

    //XDEBUG("- Test:MeteringSave\r\n");
}

}

/** LogHelper Unit Test.
 *
 * 테스트의 효율을 위해 이곳에 코드를 넣는다.
 *
 * LogTbl 관련 내용 테스트
 *
 */
SUITE(LogHelper)
{

TEST_FIXTURE(DbDeviceFixture, LogSave)
{
    EUI64 id;
    DBFetchData fetchData;
    LOGENTRYINS logEntryIns[2];

    CModemHelper modh(dbFile);
    CMeterHelper medh(dbFile);
    CLogHelper ldh(dbFile);

    memset(&fetchData, 0, sizeof(DBFetchData));

    /** 정상적으로 DB 초기화 */
    CHECK_EQUAL(modh.Initialize(), TRUE);
    CHECK_EQUAL(medh.Initialize(), TRUE);
    CHECK_EQUAL(ldh.Initialize(), TRUE);
    CHECK_EQUAL(modh.CheckDB(), TRUE);
    CHECK_EQUAL(medh.CheckDB(), TRUE);
    CHECK_EQUAL(ldh.CheckDB(), TRUE);

    StrToEUI64(targetId, &id);

    /** 새로운 Modem 생성
     */
    CHECK_EQUAL(TRUE, modh.Update(&id, model, hwVer, swVer, build));

    /** 새로운 Meter 생성
     */
    CHECK_EQUAL(TRUE, medh.Update(targetId, 0,
                "AB", PARSER_TYPE_REPEATER, SERVICE_TYPE_ELEC, METER_VENDOR_NURI, hwVer, swVer, 4));

    /** Log Data Insert
    */
    memset(logEntryIns, 0, sizeof(LOGENTRYINS)*2);

    GetTimestamp(&logEntryIns[0].logTime, NULL);
    GetTimestamp(&logEntryIns[1].logTime, NULL);

    logEntryIns[0].nLogSize = 1;
    logEntryIns[1].nLogSize = 1;
    logEntryIns[0].szLog = (BYTE *)MALLOC(1); memset(logEntryIns[0].szLog, 0, 1);
    logEntryIns[1].szLog = (BYTE *)MALLOC(1); memset(logEntryIns[1].szLog, 0, 1);

    CHECK_EQUAL(TRUE, ldh.Update(targetId, 0,
                2,              // Log Count
                logEntryIns
            ));

    FREE(logEntryIns[0].szLog);
    FREE(logEntryIns[1].szLog);

    /** Select 해서 1이 나와야 함(Device count) */
    CHECK_EQUAL(TRUE, ldh.Select(&fetchData));
    CHECK_EQUAL(1, fetchData.nCount);
    CHECK(ldh.GetData(ldh.GetHeadRow(&fetchData)) != NULL);
    /** Log 수는 2가 나와야 함 */
    CHECK_EQUAL(2, ldh.GetData(ldh.GetHeadRow(&fetchData))->nLogCnt);
    ldh.DumpResult(&fetchData);
    ldh.FreeResult(&fetchData);

}

}

