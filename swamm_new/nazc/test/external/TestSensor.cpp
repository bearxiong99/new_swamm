
#include <UnitTest++.h>

#include "codiapi.h"
#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"


/** Test Sensor Operation
 */


/** Test Read/Write Sensor ROM
 *   . Sensor의 특정 Rom 주소의 정보를 읽고 쓴다.
 *   . Sensor의 Read/Write 시 connection의 정리 시간이 필요해서 500ms 씩 sleep 한다
 *
 *      - show sensor list (102.26)
 *      - set sensor meterserial (102.23)
 *      - set sensor fixedreset (102.23)
 *      - set sensor nzc (102.23)
 *      - set sensor vendor (102.23)
 *      - set sensor customer (102.23)
 *      - set sensor location (102.23)
 *      - set sensor mday (102.23)
 *      - set sensor mhour (102.23)
 *      - set sensor rday (102.23)
 *      - set sensor rhourmin (102.23)
 */
TEST(ReadWriteROM)
{
    int     i=0;
    int     nError;
    int     nCount=0;
    SENSORINFOENTRYNEW * pSensor = NULL;
#if 0
    WORD     ONE_BYTE=1, TWO_BYTES=2;

    char    resStr[64] = {0,};
    char    orgStr[64] = {0,};
    char    tmpStr[64] = {0,};
    int     orgValue = 0;
#endif
    int     resValue = 0;
    int     tmpValue = 0;
#ifdef  __DEBUG__    
    char    szId[32] = {0,};
#endif
    long    sleep_time = 500000;

    CIF4Invoke  invoke(gServerAddr, 8000, 30);
    CIF4Invoke *invoke2 = NULL;

    SKIP_TEST

    nError = Call(invoke.GetHandle(), "102.26");
    CHECK_EQUAL(IF4ERR_NOERROR, nError);
    if(nError != IF4ERR_NOERROR) return;

    nCount = invoke.GetHandle()->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)invoke.GetHandle()->pResult[i]->stream.p;

#ifdef  __DEBUG__
        EUI64ToStr(&pSensor->sensorID, szId);
#endif

/*-------------------------------------------------------------------------------------------------------*/
#if 0
        /*-- manual enable --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_MANUAL_ENABLE);
        invoke2->AddParam("1.5", (WORD) ONE_BYTE);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
#ifdef  __DEBUG__
        if(nError == IF4ERR_NOERROR) {
            resValue = *invoke2->GetHandle()->pResult[0]->stream.p & 0xff;
            fprintf(stdout,"[%s]      MANUAL_ENABLE: %s(%d)\n", szId,
                resValue ? "Enable" : "Disable", resValue);
        }
#endif
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- channel --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CHANNEL);
        invoke2->AddParam("1.5", (WORD) ONE_BYTE);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
#ifdef  __DEBUG__
        if(nError == IF4ERR_NOERROR) {
            resValue = *invoke2->GetHandle()->pResult[0]->stream.p & 0xff;
            fprintf(stdout,"[%s]            CHANNEL: (%d)\n", szId, resValue);
        }
#endif
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- panid --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_PANID);
        invoke2->AddParam("1.5", (WORD) TWO_BYTES);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
#ifdef  __DEBUG__
        if(nError == IF4ERR_NOERROR) {
            resValue = BigToHostShort(invoke2->GetHandle()->pResult[0]->stream.u16);
            fprintf(stdout,"[%s]              PANID: (%d)\n", szId, resValue);
        }
#endif
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- txpower --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_TXPOWER);
        invoke2->AddParam("1.5", (WORD) ONE_BYTE);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
#ifdef  __DEBUG__
        if(nError == IF4ERR_NOERROR) {
            resValue = *invoke2->GetHandle()->pResult[0]->stream.p & 0xff;
            fprintf(stdout,"[%s]            TXPOWER: (%d)\n", szId, resValue);
        }
#endif
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- security enable --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_SECURITY_ENABLE);
        invoke2->AddParam("1.5", (WORD) ONE_BYTE);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
#ifdef  __DEBUG__
        if(nError == IF4ERR_NOERROR) {
            resValue = *invoke2->GetHandle()->pResult[0]->stream.p & 0xff;
            fprintf(stdout,"[%s]    SECURITY ENABLE: %s(%d)\n", szId, 
                resValue ? "Enable" : "Disable", resValue);
        }
#endif
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- security key --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_SECURITY_KEY);
        invoke2->AddParam("1.5", (WORD) 16);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
#ifdef  __DEBUG__
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 16);
            fprintf(stdout,"[%s]                KEY: (%s)\n", szId, resStr);
        }
#endif
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- node kind --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_NODE_KIND);
        invoke2->AddParam("1.5", (WORD) 20);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
#ifdef  __DEBUG__
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 20);
            fprintf(stdout,"[%s]          NODE KIND: (%s)\n", szId, resStr);
        }
#endif
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- meter serial --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_METER_SERIAL_NUMBER);
        invoke2->AddParam("1.5", (WORD) 20);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(orgStr,0,sizeof(orgStr));
            memcpy(orgStr, invoke2->GetHandle()->pResult[0]->stream.p, 20);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]       METER SERIAL: (%s)\n", szId, orgStr);
#endif
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- meter serial set --*/
        sprintf(tmpStr,"%-20s","testMeterSerial");
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_METER_SERIAL_NUMBER);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, tmpStr, 20);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);

        /*-- meter serial read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_METER_SERIAL_NUMBER);
        invoke2->AddParam("1.5", (WORD) 20);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 20);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]    (T)METER SERIAL: (%s)\n", szId, resStr);
#endif
            CHECK_EQUAL(0, strncmp(tmpStr,resStr,20));
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- meter serial set (orig)--*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_METER_SERIAL_NUMBER);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, orgStr, 20);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);

        /*-- meter serial read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_METER_SERIAL_NUMBER);
        invoke2->AddParam("1.5", (WORD) 20);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 20);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]       METER SERIAL: (%s)\n", szId, resStr);
#endif
            CHECK_EQUAL(0, strncmp(orgStr,resStr,20));
        }
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- vendor --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_VENDOR);
        invoke2->AddParam("1.5", (WORD) 20);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(orgStr,0,sizeof(orgStr));
            memcpy(orgStr, invoke2->GetHandle()->pResult[0]->stream.p, 20);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]             VENDOR: (%s)\n", szId, orgStr);
#endif
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- vendor set --*/
        sprintf(tmpStr,"%-20s","testVendor");
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_VENDOR);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, tmpStr, 20);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);

        /*-- vendor read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_VENDOR);
        invoke2->AddParam("1.5", (WORD) 20);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 20);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]          (T)VENDOR: (%s)\n", szId, resStr);
#endif
            CHECK_EQUAL(0, strncmp(tmpStr,resStr,20));
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- vendor set (orig)--*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_VENDOR);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, orgStr, 20);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);

        /*-- vendor read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_VENDOR);
        invoke2->AddParam("1.5", (WORD) 20);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 20);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]             VENDOR: (%s)\n", szId, resStr);
#endif
            CHECK_EQUAL(0, strncmp(orgStr,resStr,20));
        }
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- customer --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CUSTOMER_NAME);
        invoke2->AddParam("1.5", (WORD) 30);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(orgStr,0,sizeof(orgStr));
            memcpy(orgStr, invoke2->GetHandle()->pResult[0]->stream.p, 30);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]      CUSTOMER NAME: (%s)\n", szId, orgStr);
#endif
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- customer set --*/
        sprintf(tmpStr,"%-30s","testCustomer");
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CUSTOMER_NAME);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, tmpStr, 30);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);

        /*-- customer read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CUSTOMER_NAME);
        invoke2->AddParam("1.5", (WORD) 30);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 30);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]   (T)CUSTOMER NAME: (%s)\n", szId, resStr);
#endif
            CHECK_EQUAL(0, strncmp(tmpStr,resStr,30));
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- customer set (orig)--*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CUSTOMER_NAME);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, orgStr, 30);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);

        /*-- customer read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CUSTOMER_NAME);
        invoke2->AddParam("1.5", (WORD) 30);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 30);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]      CUSTOMER NAME: (%s)\n", szId, resStr);
#endif
            CHECK_EQUAL(0, strncmp(orgStr,resStr,30));
        }
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- location --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CONSUMPTION_LOCATION);
        invoke2->AddParam("1.5", (WORD) 30);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(orgStr,0,sizeof(orgStr));
            memcpy(orgStr, invoke2->GetHandle()->pResult[0]->stream.p, 30);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]           LOCATION: (%s)\n", szId, orgStr);
#endif
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- location set --*/
        sprintf(tmpStr,"%-30s","testLocation");
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CONSUMPTION_LOCATION);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, tmpStr, 30);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);

        /*-- location read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CONSUMPTION_LOCATION);
        invoke2->AddParam("1.5", (WORD) 30);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 30);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]        (T)LOCATION: (%s)\n", szId, resStr);
#endif
            CHECK_EQUAL(0, strncmp(tmpStr,resStr,30));
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- location set (orig)--*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CONSUMPTION_LOCATION);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, orgStr, 30);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);
        /*-- location read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_CONSUMPTION_LOCATION);
        invoke2->AddParam("1.5", (WORD) 30);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            memset(resStr,0,sizeof(resStr));
            memcpy(resStr, invoke2->GetHandle()->pResult[0]->stream.p, 30);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]           LOCATION: (%s)\n", szId, resStr);
#endif
            CHECK_EQUAL(0, strncmp(orgStr,resStr,30));
        }
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
        /*-- fixed reset --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_FIXED_RESET);
        invoke2->AddParam("1.5", (WORD) ONE_BYTE);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            orgValue = *invoke2->GetHandle()->pResult[0]->stream.p & 0xff;
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]        FIXED RESET: (%d)\n", szId, orgValue);
#endif
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- fixed reset set --*/
        tmpValue = 2;
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_FIXED_RESET);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, &tmpValue, ONE_BYTE);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);

        /*-- fixed reset read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_FIXED_RESET);
        invoke2->AddParam("1.5", (WORD) ONE_BYTE);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            resValue = *invoke2->GetHandle()->pResult[0]->stream.p & 0xff;
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]     (T)FIXED RESET: (%d)\n", szId, resValue);
#endif
            CHECK_EQUAL(tmpValue, resValue);
        }
        delete invoke2;
        usleep(sleep_time);

        /*-- fixed reset set (orig)--*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_FIXED_RESET);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, &orgValue, ONE_BYTE);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        usleep(sleep_time);
        /*-- fixed reset read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_FIXED_RESET);
        invoke2->AddParam("1.5", (WORD) ONE_BYTE);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            resValue = *invoke2->GetHandle()->pResult[0]->stream.p & 0xff;
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]        FIXED RESET: (%d)\n", szId, resValue);
#endif
            CHECK_EQUAL(orgValue, resValue);
        }
        delete invoke2;
        usleep(sleep_time);

/*-------------------------------------------------------------------------------------------------------*/
#endif
        /*-- nazc number set --*/
        tmpValue = BigToHostInt(i+1);
        printf("i+1=%d, tmpValue=%d, oi(t)=%d, oi(i+1)=%d\n", i+1, tmpValue, BigToHostInt(tmpValue), BigToHostInt(i+1));
        CHECK_EQUAL(tmpValue, (int)BigToHostInt(i+1));
        CHECK_EQUAL(i+1, (int)BigToHostInt(tmpValue));
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_NAZC_NUMBER);
        invoke2->AddParamFormat("1.12", VARSMI_STREAM, &tmpValue, 4);
        nError = Call(invoke2->GetHandle(), "102.24");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke2;
        USLEEP(sleep_time);

        /*-- nazc number read --*/
        invoke2 = new CIF4Invoke(gServerAddr, 8000, 30);
        invoke2->AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&pSensor->sensorID, sizeof(EUI64));
        invoke2->AddParam("1.5", (WORD) ENDI_ROMAP_NAZC_NUMBER);
        invoke2->AddParam("1.5", (WORD) 4);
        nError = Call(invoke2->GetHandle(), "102.23");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError == IF4ERR_NOERROR) {
            resValue = BigToHostInt(invoke2->GetHandle()->pResult[0]->stream.u32);
#ifdef  __DEBUG__
            fprintf(stdout,"[%s]     (T)NAZC NUMBER: (%d)\n", szId, resValue);
#endif
            CHECK_EQUAL((int)BigToHostInt(tmpValue), resValue);
        }
        delete invoke2;
        USLEEP(sleep_time);

/*
#define ENDI_ROMAP_TEST_FLAG				0x01E5
#define ENDI_ROMAP_METERING_DAY				0x01E6
#define ENDI_ROMAP_METERING_HOUR			0x01EA
#define ENDI_ROMAP_REPEATING_DAY			0x01F6
#define ENDI_ROMAP_REPEATING_HOUR			0x01FA
#define ENDI_ROMAP_REPEATING_MIN			0x01FD
#define ENDI_ROMAP_NAZC_NUMBER				0x0206

#define ENDI_ROMAP_BATTERY_POINTER			0x0500
#define ENDI_ROMAP_BATTERY_LOGDATA         	0x0501

#define ENDI_ROMAP_EVENT_POINTER            0x1000
#define ENDI_ROMAP_EVENT_LOGDATA           	0x1001

#define ENDI_ROMAP_METER_LPPERIOD			0x2000
#define ENDI_ROMAP_METER_LPPOINTER         	0x2001
#define ENDI_ROMAP_METER_LPLOGDATA			0x2002
*/
    }

}
