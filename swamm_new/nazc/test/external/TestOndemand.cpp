
#include <UnitTest++.h>

#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"
#include "GeMeter.h"
#include "AidonMeter.h"
#include "MemoryDebug.h"
#include "codiapi.h"
#include "endiFrame.h"
#include "vendor/ansi.h"
#include "vendor/repdef.h"
#include "vendor/mccb.h"

/** Test Suite #09 Ondeamdn.
 */
SUITE(_09_Ondemand)
{

/** Test OnDemand Metering 
 *   . show sensor list를 이용해서 sensor 목록을 가지고 오고
 *     이 목록을 이용해서 sensor에 대한 ondemand metering를 수행한다
 *      - show sensor list (102.26)
 *      - ondemand metering (104.6)
 */
TEST(OnDemandMetering_GE)
{
    CIF4Invoke *invoke = NULL, *invoke2 = NULL;
    IF4Invoke * pInvoke = NULL, *pInvoke2 = NULL;
    SENSORINFOENTRYNEW  *pSensor=NULL;
    METERLPENTRY * pEntry = NULL;
    GETABLE * pTable = NULL;
    int nError = IF4ERR_NOERROR;
    int nCount=0, nCount2=0;
    int nLength=0, nSeek=0, len=0;
    int i=0,j=0,k=0;

    invoke = new CIF4Invoke(gServerAddr, 8000, 6);
    invoke->AddParam("1.11", "GE");
    nError = Call(invoke->GetHandle(), "102.26");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);

    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke->GetHandle();
    nCount = pInvoke->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;
        if(pSensor->sensorState != ENDISTATE_NORMAL) continue;

        invoke2 = new CIF4Invoke(gServerAddr, 8000, 120);
        invoke2->AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));

        nError = Call(invoke2->GetHandle(), "104.6");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);

        pInvoke2 = (IF4Invoke *)invoke2->GetHandle();

        nCount2 = pInvoke2->nResultCount;
        for(j=0; j<nCount2; j++)
        {
            pEntry = (METERLPENTRY *)invoke2->GetHandle()->pResult[j]->stream.p;
            CHECK_EQUAL(0, memcmp(&pSensor->sensorID,&pEntry->mlpId,sizeof(EUI64)));

            nLength = pEntry->mlpLength - sizeof(TIMESTAMP);
            for(k=0, nSeek=0; k<nLength; k+=nSeek) {
                pTable = (GETABLE *)(pEntry->mlpData + k);
                nSeek = sizeof(GETABLE);
                len   = BigToHostShort(pTable->len);
                nSeek += len;
		        if (strncmp(pTable->table, "S001", 4) == 0) {
			        S001 *s001 = (S001 *)&pTable->data;
                    char * res = NULL;
                    
                    res = strip((char *)s001->MFG_SERIAL_NUMBER);
                    CHECK_EQUAL(0, strncmp(pSensor->sensorSerialNumber, res,
                        strlen(pSensor->sensorSerialNumber)));
                    FREE(res);
		        } else if (strncmp(pTable->table, "S064", 4) == 0) {
                    CHECK(len > 0);
                }
            }
        }
        delete invoke2;
    }

    delete invoke;
}

#define STATE_SD_READ_INFO          2
#define STATE_SD_READ_LP_HEADER     3
#define STATE_SD_LP_ENTRY           4
#define STATE_SD_OBIS               5
#define STATE_SD_READ_SOLAR_LOG     6
#define STATE_SD_DONE               0

void checkAidonTypeMeterData(IF4Invoke *pInvoke)
{
    CIF4Invoke *invoke2 = NULL;
    IF4Invoke *pInvoke2;
    SENSORINFOENTRYNEW  *pSensor=NULL;
    METERLPENTRY * pEntry = NULL;
    char * mdata = NULL;
    int nError;
    int nCount=0, nCount2=0;
    int i,j=0,state=0,len=0;
    UINT idx=0, length=0;
    int lpLen=0, lpDateCnt=0, lpReadSize=0;
    BOOL bLpPeriod;

    AIDON_METERING_DATA *aidonHeader = NULL;

    nCount = pInvoke->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;
        if(pSensor->sensorState != ENDISTATE_NORMAL) continue;

        invoke2 = new CIF4Invoke(gServerAddr, 8000, 120);
        invoke2->AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));

        nError = Call(invoke2->GetHandle(), "104.6");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);

        pInvoke2 = (IF4Invoke *)invoke2->GetHandle();
        nCount2 = pInvoke2->nResultCount;
        for(j=0; j<nCount2; j++)
        {
            pEntry = (METERLPENTRY *)invoke2->GetHandle()->pResult[j]->stream.p;
            CHECK_EQUAL(0, memcmp(&pSensor->sensorID,&pEntry->mlpId,sizeof(EUI64)));

            mdata = (char *)pEntry->mlpData;
            length =  LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP);
            state = STATE_SD_READ_LP_HEADER;
            idx=0;
            while(state != STATE_SD_DONE) {
                switch(state) {
                    case STATE_SD_READ_LP_HEADER:
                        if(idx+sizeof(AIDON_METERING_DATA) > length) {
                            state = STATE_SD_DONE;
                            CHECK(FALSE);
                            break;
                        }
                        aidonHeader = (AIDON_METERING_DATA *)mdata+idx;
                        idx += sizeof(AIDON_METERING_DATA);
                
                        state = STATE_SD_LP_ENTRY;
                        switch(aidonHeader->lpPeriod) {
                            case 0: case 1: case 2: case 4:
                                lpLen = aidonHeader->lpPeriod * 48;
                                bLpPeriod = TRUE;
                                break;
                            default:
                                bLpPeriod = FALSE;
                                state = STATE_SD_DONE;
                                break;
                        }
                        CHECK(bLpPeriod);
                        lpLen += sizeof(TIMEDATE)+sizeof(UINT);
                        lpDateCnt = aidonHeader->dateCount;
                        lpReadSize = 0;

                        CHECK(lpDateCnt > 0);
                    break;
                    case STATE_SD_LP_ENTRY:
                        if(lpDateCnt <= 0) {
                            state = STATE_SD_OBIS;
                            break;
                        }
                        lpDateCnt --;
                        if(idx+lpLen <= length) {
                            idx += lpLen;
                            lpReadSize += lpLen;
                        } else {
                            state = STATE_SD_DONE;
                            CHECK(FALSE);
                        }
                    break;
                    case STATE_SD_OBIS:
                        len = length - lpReadSize - sizeof(AIDON_METERING_DATA) - sizeof(TIMESTAMP);
                        if(len > 0) {
                            if(idx+len <= length) {
                                idx += len;
                            } else {
                                CHECK(FALSE);
                            }
                        }
                        state = STATE_SD_DONE;
                    break;
                }
            }
        }
        delete invoke2;
    }
}

void checkPulseTypeMeterData(IF4Invoke *pInvoke)
{
    CIF4Invoke *invoke2 = NULL;
    IF4Invoke *pInvoke2;
    SENSORINFOENTRYNEW  *pSensor=NULL;
    METERLPENTRY * pEntry = NULL;
    UINT    infoLen = sizeof(ENDI_METERING_INFO);
    char * mdata = NULL;
    int nError;
    int nCount=0, nCount2=0;
    int i,j=0,state=0;
    UINT idx=0, length=0;
    int lpLen=0;
    BOOL bLpPeriod;

    ENDI_METERING_INFO *meteringInfo;

    nCount = pInvoke->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;
        if(pSensor->sensorState != ENDISTATE_NORMAL) continue;
        if(!(pSensor->sensorAttr & ENDDEVICE_ATTR_RFD)) continue;

        invoke2 = new CIF4Invoke(gServerAddr, 8000, 120);
        invoke2->AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));

        nError = Call(invoke2->GetHandle(), "104.6");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);

        pInvoke2 = (IF4Invoke *)invoke2->GetHandle();
        nCount2 = pInvoke2->nResultCount;
        for(j=0; j<nCount2; j++)
        {
            pEntry = (METERLPENTRY *)invoke2->GetHandle()->pResult[j]->stream.p;
            CHECK_EQUAL(0, memcmp(&pSensor->sensorID,&pEntry->mlpId,sizeof(EUI64)));

            mdata = (char *)pEntry->mlpData;
            length =  LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP);
            idx=0;
            state = STATE_SD_READ_INFO;
            while(state != STATE_SD_DONE) {
                switch(state) {
                    case STATE_SD_READ_INFO:
                        if(idx+infoLen > length) {
                            state = STATE_SD_DONE;
                            CHECK(FALSE);
                            break;
                        }
                        meteringInfo = (ENDI_METERING_INFO *) mdata+idx;
                        idx += infoLen;
                
                        switch(meteringInfo->lpPeriod) {
                            case 0: case 1: case 2: case 4:
                                lpLen = meteringInfo->lpPeriod * 48 + 8;
                                bLpPeriod = TRUE;
                                break;
                            default:
                                bLpPeriod = FALSE;
                                state = STATE_SD_DONE;
                                break;
                        }
                        CHECK(bLpPeriod);
                        state = STATE_SD_LP_ENTRY;
                    break;
                    case STATE_SD_LP_ENTRY:
                        if(idx+lpLen <= length) {
                            idx += lpLen - sizeof(TIMEDATE) - sizeof(UINT);
                            state = STATE_SD_LP_ENTRY;
                        } else {
                            state = STATE_SD_DONE;
                        }
                    break;
                }
            }
        }
        delete invoke2;
    }
}

void checkRepeaterTypeMeterData(IF4Invoke *pInvoke)
{
    CIF4Invoke *invoke2 = NULL;
    IF4Invoke *pInvoke2 = NULL;
    SENSORINFOENTRYNEW  *pSensor=NULL;
    METERLPENTRY * pEntry = NULL;
    char * mdata = NULL;
    int nError;
    int nCount=0, nCount2=0;
    int i,j=0,state;
    UINT idx=0, length=0;
    int count=0;

    REPEATER_METERING_DATA *repeaterData;
    SOLAR_TIME_DATE *solarTimeDate;
    SOLAR_LOG_DATA *solarLogData;

    nCount = pInvoke->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;
        if(pSensor->sensorState != ENDISTATE_NORMAL) continue;
        if(!(pSensor->sensorAttr & ENDDEVICE_ATTR_RFD)) continue;

        invoke2 = new CIF4Invoke(gServerAddr, 8000, 120);
        invoke2->AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));

        nError = Call(invoke2->GetHandle(), "104.6");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);

        pInvoke2 = (IF4Invoke *)invoke2->GetHandle();
        nCount2 = pInvoke2->nResultCount;
        for(j=0; j<nCount2; j++)
        {
            pEntry = (METERLPENTRY *)invoke2->GetHandle()->pResult[j]->stream.p;
            CHECK_EQUAL(0, memcmp(&pSensor->sensorID,&pEntry->mlpId,sizeof(EUI64)));

            mdata = (char *)pEntry->mlpData;
            length =  LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP);
            idx=0;
            state = STATE_SD_READ_INFO;
            while(state != STATE_SD_DONE) {
                switch(state) {
                    case STATE_SD_READ_INFO:
                        if(idx+sizeof(REPEATER_METERING_DATA) > length) {
                            CHECK(FALSE); // ERROR -- Read REPEATER INFO Fail
                            state = STATE_SD_DONE;
                            break;
                        }
                        repeaterData = (REPEATER_METERING_DATA *) mdata + idx;
                        idx += sizeof(REPEATER_METERING_DATA);
                
                        count = repeaterData->solarLogCount;
                        state = STATE_SD_READ_SOLAR_LOG;
                    break;
                    case STATE_SD_READ_SOLAR_LOG :
                        if(count <= 0) {
                            state = STATE_SD_DONE;
                            break;
                        }

                        if(idx+sizeof(SOLAR_TIME_DATE) > length) {
                            CHECK(FALSE); // ERROR -- Read SOLAR LOG TIME DATE Fail
                            state = STATE_SD_DONE;
                            break;
                        }

                        solarTimeDate = (SOLAR_TIME_DATE *) mdata+idx;
                        idx += sizeof(SOLAR_TIME_DATE);

                        if(idx+(sizeof(SOLAR_LOG_DATA) * solarTimeDate->hourCount) > length) {
                            CHECK(FALSE); // ERROR -- Read SOLAR LOG DATA Fail
                            state = STATE_SD_DONE;
                            break;
                        }

                        for(i=0;i<solarTimeDate->hourCount;i++) {
                            solarLogData = (SOLAR_LOG_DATA *) mdata+idx;
                            idx += sizeof(SOLAR_LOG_DATA);
                        }
                        count --;
                    break;
                    default :
                        state = STATE_SD_DONE;
                        break;
                }
            }
        }
        delete invoke2;
    }
}

TEST(OnDemandMetering_AIDON)
{
    CIF4Invoke *invoke = NULL;
    IF4Invoke * pInvoke;
    int nError = IF4ERR_NOERROR;

    invoke = new CIF4Invoke(gServerAddr, 8000, 6);
    invoke->AddParam("1.11", "AIDON");
    nError = Call(invoke->GetHandle(), "102.26");
    CHECK(nError == IF4ERR_NOERROR || nError == IF4ERR_NO_ENTRY);

    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke->GetHandle();
    checkAidonTypeMeterData(pInvoke);
    delete invoke;
}

TEST(OnDemandMetering_KAMSTRUP)
{
    CIF4Invoke *invoke = NULL;
    IF4Invoke * pInvoke;
    int nError = IF4ERR_NOERROR;

    invoke = new CIF4Invoke(gServerAddr, 8000, 6);
    invoke->AddParam("1.11", "KAMSTRUP");
    nError = Call(invoke->GetHandle(), "102.26");
    CHECK(nError == IF4ERR_NOERROR || nError == IF4ERR_NO_ENTRY);

    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke->GetHandle();
    checkAidonTypeMeterData(pInvoke);
    delete invoke;
}

TEST(OnDemandMetering_PULSE)
{
    CIF4Invoke *invoke = NULL;
    IF4Invoke * pInvoke;
    int nError = IF4ERR_NOERROR;

    invoke = new CIF4Invoke(gServerAddr, 8000, 6);
    invoke->AddParam("1.11", "PULSE");
    nError = Call(invoke->GetHandle(), "102.26");
    CHECK(nError == IF4ERR_NOERROR || nError == IF4ERR_NO_ENTRY);

    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke->GetHandle();
    checkPulseTypeMeterData(pInvoke);
    delete invoke;
}

TEST(OnDemandMetering_REPEATER)
{
    CIF4Invoke *invoke = NULL;
    IF4Invoke * pInvoke;
    int nError = IF4ERR_NOERROR;

    invoke = new CIF4Invoke(gServerAddr, 8000, 6);
    invoke->AddParam("1.11", "REPEATER");
    nError = Call(invoke->GetHandle(), "102.26");
    CHECK(nError == IF4ERR_NOERROR || nError == IF4ERR_NO_ENTRY);

    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke->GetHandle();
    checkRepeaterTypeMeterData(pInvoke);
    delete invoke;
}

TEST(OnDemandMetering_MBUS)
{
    /**  어떻게 테스트해야 하는지 아직 모른다 */
    CHECK(FALSE);
}

/** Test OnDemand Relay
 *   . show sensor list를 이용해서 sensor 목록을 가지고 오고
 *     이 목록을 이용해서 sensor에 대한 ondemand relay를 수행한다
 *      - show sensor list (102.26)
 *      - ondemand relay (197.101)
 *      - ondemand relay (197.100)
 *
 *  . Relay State Table
 *
 *   +-----------+------------+------------+
 *   |           | Active On  | Active Off |
 *   +-----------+------------+------------+
 *   | Relay On  |            | Relay Off  |
 *   +-----------+------------+------------+
 *   | Relay Off | Active Off | Active On  |
 *   |           | Relay On   |            |
 *   +-----------+------------+------------+
 *
 *      _ Relay On/Active Off 일 때 Relay Off 가능
 *      _ Relay Off/Active Off 일 때 Active On 가능
 *      _ Relay Off/Active On 일 때 Acitive Off, Relay On 가능
 *      _ Relay On/Active On 상태는 불가능
 *
 */
TEST(OnDemandRelay)
{
    int nError;
    int nCount=0, testCount=0;
    int i=0,j,nSeek,nLength,len;
    int nextCommand = 0;
    BYTE active=0, relay=0;
    BYTE requireActive=0xff, requireRelay=0xff;
    GETABLE *pTable = NULL;
    S130    *s130 = NULL;
    char data[1] = {0};
    BOOL    callSucc = FALSE;

    CIF4Invoke *invoke = NULL;
    CIF4Invoke  *invoke2 = NULL;
    IF4Invoke * pInvoke = NULL;

    SENSORINFOENTRYNEW  *pSensor=NULL;
    METERLPENTRY * pEntry;
    unsigned long sleepTime = 5 * 1000000;

    invoke = new CIF4Invoke(gServerAddr, 8000, 30);
    invoke->AddParam("1.11", "GE");
    nError = Call(invoke->GetHandle(), "102.26");
    CHECK(nError == IF4ERR_NOERROR || nError == IF4ERR_NO_ENTRY);

    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke->GetHandle();
    nCount = pInvoke->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;
        if(pSensor->sensorState != ENDISTATE_NORMAL) continue;

        testCount = 0;
        requireActive = requireRelay = 0xff;
        while(TRUE) {
            // 1) cmdReadTable(197.100) - 현재 상태 확인
            invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
            invoke2->AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
            invoke2->AddParam("1.9", 3 /* READ_OPTION_RELAY */);

            nError = Call(invoke2->GetHandle(), "197.100");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) {
                delete invoke2;
                CHECK(callSucc);
                break;
            }

            pEntry = (METERLPENTRY *)invoke2->GetHandle()->pResult[0]->stream.p;

            nLength = pEntry->mlpLength - sizeof(TIMESTAMP);

            active = relay = 0xff;
            for(j=0, nSeek=0; j<nLength; j+=nSeek) {
                pTable = (GETABLE *)(pEntry->mlpData + j);
                nSeek = sizeof(GETABLE);
                len   = BigToHostShort(pTable->len);
                nSeek += len;
                if (strncmp(pTable->table, "S130", 4) == 0) {
                    s130 = (S130 *)&pTable->data;
                    active = s130->activeState;
                    relay = s130->relayState;
                    break;
                }
            }
            delete invoke2;

            // 값이 읽혀 졌는지 검사
            CHECK(active != 0xff);
            CHECK(relay != 0xff);

            // Active On/Relay On 인지 검사 (이상태는 나올 수 없다)
            CHECK(active != 1 || relay != 1);

            // 이전 operation으로 정상적인 상태로 전이 된건지 검사
            if(requireActive != 0xff && requireRelay != 0xff) {
                CHECK_EQUAL((BYTE)requireActive, (BYTE)active);
                CHECK_EQUAL((BYTE)requireRelay, (BYTE)relay);
                testCount ++;
            }

            if(testCount > 2) break;
            
            if(relay) {
                if(active) break;   // 이 상태는 발생할 수 없다
                else {
                    nextCommand = 3;   // Relay off    
                    requireActive = 0;
                    requireRelay = 0;
                }
            }else {
                if(active) {
                    nextCommand = 2;   // Relay on
                    requireActive = 0;
                    requireRelay = 1;
                } else {
                    nextCommand = 1;   // Active on
                    requireActive = 1;
                    requireRelay = 0;
                }
            }

            // 2) cmdWriteTable(197.101) 
            invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
            invoke2->AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
            invoke2->AddParam("1.9", 131 /* Relay Table */);
            data[0] = nextCommand; /* Command */
            invoke2->AddParamFormat("1.12", VARSMI_STREAM, data, 1);
            nError = Call(invoke2->GetHandle(), "197.101");

            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) {
                delete invoke2;
                CHECK(callSucc);
                break;
            }

            pTable = (GETABLE *)invoke2->GetHandle()->pResult[0]->stream.p;

            CHECK_EQUAL(0, strncmp(pTable->table, "S130", 4));
            if (strncmp(pTable->table, "S130", 4) == 0) {
                s130 = (S130 *)&pTable->data;
                CHECK_EQUAL((BYTE)requireActive, (BYTE)s130->activeState);
                CHECK_EQUAL((BYTE)requireRelay, (BYTE)s130->relayState);
            }

            delete invoke2;
            
            switch(nextCommand) {
                case 2: // Relay On
                case 3: // Relay Off
                    USLEEP(sleepTime);
            }
        }
    }

    delete invoke;
}

/** Test OnDemand MCCB
 *   . show sensor list를 이용해서 sensor 목록을 가지고 오고
 *     이 목록을 이용해서 sensor에 대한 ondemand mccb를 수행한다
 *      - show sensor list (102.26)
 *      - ondemand mccb (105.7)
 *
 *  . MCCB Control
 *
 *      - Off : Disable-Disconnect(4) 
 *      - On  : Enable-Connect Now(3) 
 *
 * @warning MCCB Control 중 Return 값이 잘못오는 경우가 있는데 이 경우는
 *      F/W 적인 문제가 있어서 그렇다. F/W 버전을 최신 버전으로 변경해야 한다.
 *
 */
TEST(OnDemandMccb)
{
    int nError;
    int i=0, nCount=0;
    BYTE reply;

    CIF4Invoke *invoke = NULL;
    CIF4Invoke  *invoke2 = NULL;

    SENSORINFOENTRYNEW  *pSensor=NULL;

    invoke = new CIF4Invoke(gServerAddr, 8000, 30);
    invoke->AddParam("1.11", "AIDON");
    nError = Call(invoke->GetHandle(), "102.26");
    CHECK(nError == IF4ERR_NOERROR || nError == IF4ERR_NO_ENTRY);

    if(nError != IF4ERR_NOERROR) return;

    nCount = invoke->GetHandle()->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)invoke->GetHandle()->pResult[i]->stream.p;
        if(pSensor->sensorState != ENDISTATE_NORMAL) continue;

        // Relay Off
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 60);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.4", (BYTE) MCCB_SET_DISABLE_USE_DISCONNECT);
        nError = Call(invoke2->GetHandle(), "105.7");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) {
            delete invoke2;
            CHECK(FALSE);
            break;
        }
        reply = invoke2->GetHandle()->pResult[0]->stream.u8;
        CHECK(MCCB_ACK_OK == reply);
        delete invoke2;

        // Relay On
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 60);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.4", (BYTE) MCCB_SET_ENABLE_USE_CONNECT_NOW);
        nError = Call(invoke2->GetHandle(), "105.7");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) {
            delete invoke2;
            CHECK(FALSE);
            break;
        }
        reply = invoke2->GetHandle()->pResult[0]->stream.u8;
        CHECK(MCCB_ACK_OK == reply);
        delete invoke2;
    }

    delete invoke;
}

#define CID_STATE_GET 1
#define CID_STATE_OFF 2
#define CID_STATE_ON  3
#define CID_STATE_DONE  4

/** Test OnDemand CID Control
 *   . show sensor list를 이용해서 sensor 목록을 가지고 오고
 *     이 목록을 이용해서 sensor에 대한 ondemand cid를 수행한다
 *      - show sensor list (102.26)
 *      - ondemand cid (105.8)
 *
 *  . CID Control
 *
 *      - Off : "97 01"
 *      - On  : "97 02"
 *
 */
TEST(OnDemandCid)
{
    int nError;
    int i=0, nCount=0, resLen;
    BYTE *reply=NULL;
    char szS[32];
    BYTE v1=0, v2=0;
    int len=0, state=0;

    CIF4Invoke *invoke = NULL;
    CIF4Invoke  *invoke2 = NULL;

    SENSORINFOENTRYNEW  *pSensor=NULL;

    invoke = new CIF4Invoke(gServerAddr, 8000, 30);
    invoke->AddParam("1.11", "KAMST");
    nError = Call(invoke->GetHandle(), "102.26");
    CHECK(nError == IF4ERR_NOERROR || nError == IF4ERR_NO_ENTRY);

    if(nError != IF4ERR_NOERROR) return;

    nCount = invoke->GetHandle()->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)invoke->GetHandle()->pResult[i]->stream.p;
        if(pSensor->sensorState != ENDISTATE_NORMAL) continue;

        state = CID_STATE_GET;
        v1 = 0x98; v2 = 0x00; len = 1; // Get Status

        while(state != CID_STATE_DONE)
        {
            memset(szS, 0, sizeof(szS)); szS[0] = v1; szS[1] = v2;
            invoke2 = new CIF4Invoke(gServerAddr, 8000, 60);
            invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
            invoke2->AddParamFormat("1.12", VARSMI_STREAM, szS, len);
            nError = Call(invoke2->GetHandle(), "105.8");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) {
                delete invoke2;
                CHECK(FALSE);
                state = CID_STATE_DONE;
                break;
            }
            reply = (BYTE *)invoke2->GetHandle()->pResult[0]->stream.p;
            resLen = invoke2->GetHandle()->pResult[0]->len;
            CHECK_EQUAL(3, resLen);
            switch(state) {
                case CID_STATE_GET:
                    switch(reply[1]) {
                        case 0x01:
                            state = CID_STATE_ON;
                            v1 = 0x97; v2 = 0x02; len = 2;
                            break;
                        case 0x08:
                            state = CID_STATE_ON;
                            v1 = 0x97; v2 = 0x03; len = 2;
                            break;
                        default:
                            state = CID_STATE_OFF;
                            v1 = 0x97; v2 = 0x01; len = 2;
                    }
                    break;
                case CID_STATE_OFF:
                    CHECK(reply[1] == 0x01 || reply[1] == 0x08);
                    state = CID_STATE_DONE;
                    break;
                default:
                    CHECK(reply[1] == 0x04);
                    state = CID_STATE_DONE;
                    break;
            }
            delete invoke2;
        }
    }

    delete invoke;
}

/** Test Ondemand timesync
 *   . show sensor list를 이용해서 sensor 목록을 가지고 오고
 *     이 목록을 이용해서 sensor에 대한 ondemand timesync를 수행한다
 *
 *   . 정확한 Time Sync 알고리즘을 사용하는게 아니기 때문에 설정 시간에
 *     오차가 생길 수 있다. 이 오차를 최대 1초 이내로 가정한다.
 *     (오차는 Meter 시간 설정의 Network time 오차 -설정/응답소모시간-
 *      가 포함된다)
 *
 *      - show sensor list (102.26)
 *      - ondemand timesync (105.5)
 *
 */
TEST(OnDemandTimeSync)
{
    int i=0;
    int nError, nCount=0;

    CIF4Invoke  invoke(gServerAddr, 8000, 30);
    CIF4Invoke  *invoke2 = NULL;
    IF4Invoke * pInvoke = NULL;

    SENSORINFOENTRYNEW  *pSensor=NULL;
    ANSI_ST_TABLE55     *pPrevious=NULL, *pCurrent=NULL;
    struct tm   tm_cur;
    time_t      cur, local;
    BOOL        callSucc = FALSE;

    invoke.AddParam("1.11", "GE");
    nError = Call(invoke.GetHandle(), "102.26");
    CHECK(nError == IF4ERR_NOERROR || nError == IF4ERR_NO_ENTRY);

    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nCount = pInvoke->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;
        if(pSensor->sensorState != ENDISTATE_NORMAL) continue;

        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));

        time(&local);

        nError = Call(invoke2->GetHandle(), "105.5");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) {
            delete invoke2;
            CHECK(callSucc);
            break;
        }

        CHECK(invoke2->GetHandle()->nResultCount > 0);
        if(invoke2->GetHandle()->nResultCount <= 0) continue;

        pPrevious = (ANSI_ST_TABLE55 *)invoke2->GetHandle()->pResult[0]->stream.p + 4 + 2;
        pCurrent = (ANSI_ST_TABLE55 *)(invoke2->GetHandle()->pResult[0]->stream.p + 4 + 2 + 15);

        memset(&tm_cur, 0, sizeof(struct tm));
        tm_cur.tm_year = pCurrent->YEAR + 100;
        tm_cur.tm_mon  = pCurrent->MONTH - 1;
        tm_cur.tm_mday = pCurrent->DAY;
        tm_cur.tm_hour = pCurrent->HOUR;
        tm_cur.tm_min  = pCurrent->MINUTE;
        tm_cur.tm_sec  = pCurrent->SECOND;
        cur = mktime(&tm_cur);
        
        // 설정 시간과 실제 값과 3초 이내의 오차이어야 한다.
        CHECK_CLOSE((float)local, (float)cur, (float) 3.0);
    }
}

}
