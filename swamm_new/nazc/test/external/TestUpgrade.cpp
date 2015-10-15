
#include <UnitTest++.h>

#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"
#include "../../src/agent/OtaWorker.h"
#include "if4frame.h"
#include "MemoryDebug.h"
#include "varapi.h"
#include "../../core/if4api/IF4DataFrame.h"
#include "../../src/agent/revision.h"

typedef struct _distSt
{
    WORD  nSourceBuild;
    WORD  nTargetBuild;
    char  pBinFile[256];
    char  pBinFileCheckSum[32+1];
    char  pDiffFile[256];
    char  pDiffFileCheckSum[32+1];
} DistSt;

BYTE gDownloadStatus = 0xFF;
char gStartTriger[256];
void UpgradeTestEventHandler(char *pszAddress, UINT nSourceId, BYTE *pEvent, int nLength);

struct CoordiFixture_Upgrade
{
    CoordiFixture_Upgrade() : pCoordinator(NULL)
    {

        CIF4Invoke  *invoke = NULL;
        int nError;
        char hostname[256];
        char base[64];
        char ipaddr[64];
        char cmd[256];
        gethostname(hostname, sizeof(hostname));
        hostent *hos = gethostbyname(hostname);
        char *addr = *(hos->h_addr_list);
        int port=29975;

        /** WEB Server를 기동해야 한다 */
        sprintf(cmd,"./nweb.sh -d fixture -p %d start", port);
        system(cmd);

        IF4API_SetUserCallback(NULL, NULL, UpgradeTestEventHandler, NULL);

        bTearDown = TRUE;

        sprintf(ipaddr,"%d.%d.%d.%d", (unsigned char)addr[0], (unsigned char)addr[1], (unsigned char)addr[2], (unsigned char)addr[3]);
        sprintf(base,"http://%s:%d/fwImages/", ipaddr, port);

        memset(&CoordiB06toB05, 0, sizeof(DistSt));
        memset(&CoordiB05toB06, 0, sizeof(DistSt));

        CoordiB06toB05.nSourceBuild = 0x06;
        CoordiB06toB05.nTargetBuild = 0x05;
        sprintf(CoordiB06toB05.pBinFile , "%s%s", base, "Coordinator/NZM108_Coordi_SW_V23_B05.ebl.gz");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Coordinator/NZM108_Coordi_SW_V23_B05.ebl.gz.sum", CoordiB06toB05.pBinFileCheckSum, 32));
        sprintf(CoordiB06toB05.pDiffFile , "%s%s", base, "Coordinator/NZM108_V23-06_V23-05.diff");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Coordinator/NZM108_V23-06_V23-05.diff.sum", CoordiB06toB05.pDiffFileCheckSum, 32));

        CoordiB05toB06.nSourceBuild = 0x05;
        CoordiB05toB06.nTargetBuild = 0x06;
        sprintf(CoordiB05toB06.pBinFile , "%s%s", base, "Coordinator/NZM108_Coordi_SW_V23_B06.ebl.gz");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Coordinator/NZM108_Coordi_SW_V23_B06.ebl.gz.sum", CoordiB05toB06.pBinFileCheckSum, 32));
        sprintf(CoordiB05toB06.pDiffFile , "%s%s", base, "Coordinator/NZM108_V23-05_V23-06.diff");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Coordinator/NZM108_V23-05_V23-06.diff.sum", CoordiB05toB06.pDiffFileCheckSum, 32));


        /** 원래 Server IP를 얻어오기 새로운 IP 설정 */
        {
            /** 예전 IP 얻어오기 */
            memset(origIpaddr, 0, sizeof(origIpaddr));
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParam("2.1.20");
            nError = Call(invoke->GetHandle(), "199.1");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) 
            {
                delete invoke;
                return;
            }
            memcpy(origIpaddr,invoke->GetHandle()->pResult[0]->stream.p, invoke->GetHandle()->pResult[0]->len);
            delete invoke;

            /** Server IP 변경 */
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParamFormat("2.1.20", VARSMI_STRING, ipaddr, strlen(ipaddr));
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            if(nError != IF4ERR_NOERROR) 
            {
                return;
            }
        }

        /** Coordinator 원래 정보 얻어 오기 */
        {

            /*-- get coordinator entries --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            nError = Call(invoke->GetHandle(), "101.13");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) 
            {
                delete invoke;
                return;
            }
            pCoordinator = (CODIENTRY *)invoke->GetHandle()->pResult[0]->stream.p;
            memcpy(&Coordi, pCoordinator, sizeof(CODIENTRY));
            delete invoke;

            /*-- mtor type --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("2.2.28");
            nError = Call(invoke->GetHandle(), "199.1");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            origMtorType = invoke->GetHandle()->pResult[0]->stream.u8;
            delete invoke;
        }
    }
    ~CoordiFixture_Upgrade() 
    {
        /** Web Server 종료 */
        system("./nweb.sh stop");
        {
            CIF4Invoke  *invoke = NULL;
            int nError;

            /** 예전 IP 재설정 */
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParamFormat("2.1.20", VARSMI_STRING, origIpaddr, strlen(origIpaddr));
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;

            if(!bTearDown) return;

            /*-- disable auto forming --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParam("3.3.12", (BYTE) 0);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(5000);

            /*-- channel --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("3.3.13", Coordi.codiChannel);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(5000);
    
            /*-- panid --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("3.3.14", Coordi.codiPanID);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(5000);

            /*-- rfpower --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("3.3.16", Coordi.codiRfPower);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(5000);

            /*-- txpowermode --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("3.3.17", Coordi.codiTxPowerMode);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(1000);

            /*-- permit --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("3.3.18", Coordi.codiPermit);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(1000);

            /*-- route discovery --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("3.3.22", Coordi.codiRouteDiscovery);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(1000);

            /*-- multicast hops --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("3.3.23", Coordi.codiMulticastHops);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            UnitTest::TimeHelpers::SleepMs(1000);

            /*-- mtor type --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3); 
            invoke->AddParam("2.2.28", origMtorType);
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
        }
    }

    DistSt CoordiB06toB05;
    DistSt CoordiB05toB06;

    CODIENTRY   *pCoordinator;
    CODIENTRY   Coordi;
    BOOL        origMtorType;
    BOOL        bTearDown;
    char        origIpaddr[64];
};

struct ConcentratorFixture_Upgrade
{
    ConcentratorFixture_Upgrade() 
    {
        CIF4Invoke  *invoke = NULL;
        int nError;

        char hostname[256];
        char base[64];
        char ipaddr[64];
        char revision[32];
        char cmd[256];
        gethostname(hostname, sizeof(hostname));
        hostent *hos = gethostbyname(hostname);
        char *addr = *(hos->h_addr_list);
        int port=29976;

        /** WEB Server를 기동해야 한다 */
        sprintf(cmd,"./nweb.sh -d fixture -p %d start", port);
        system(cmd);

        IF4API_SetUserCallback(NULL, NULL, UpgradeTestEventHandler, NULL);

        sprintf(ipaddr,"%d.%d.%d.%d", (unsigned char)addr[0], (unsigned char)addr[1], (unsigned char)addr[2], (unsigned char)addr[3]);
        sprintf(base,"http://%s:%d/fwImages/", ipaddr, port);

        memset(&Concentrator, 0, sizeof(DistSt));
        memset(&OldConcentrator, 0, sizeof(DistSt));

        /** Concentrator Revision 정보 얻어오기 */
        {
            memset(revision, 0, sizeof(revision));
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParam("2.1.30");
            nError = Call(invoke->GetHandle(), "199.1");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) 
            {
                delete invoke;
                return;
            }
            memcpy(revision,invoke->GetHandle()->pResult[0]->stream.p, invoke->GetHandle()->pResult[0]->len);
        }
        Concentrator.nSourceBuild = strtol(revision, (char **)NULL, 10);
        Concentrator.nTargetBuild = strtol(SW_REVISION_FIX, (char **)NULL, 10);;

        {
            /** 원본 소스 build 생성 */
            sprintf(cmd, "./mkConcentrator.sh -r %d", Concentrator.nSourceBuild);
            nError = system(cmd);
            CHECK_EQUAL(0, nError);
            if(nError) return;
        }

        OldConcentrator.nSourceBuild = strtol(revision, (char **)NULL, 10);
        OldConcentrator.nTargetBuild = OldConcentrator.nSourceBuild;

        sprintf(OldConcentrator.pBinFile , "%s%s", base, "Concentrator/NZC_O312-old.tar.gz");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Concentrator/NZC_O312-old.tar.gz.sum", OldConcentrator.pBinFileCheckSum, 32));

        sprintf(Concentrator.pBinFile , "%s%s", base, "Concentrator/NZC_O312-v3.1.tar.gz");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Concentrator/NZC_O312-v3.1.tar.gz.sum", Concentrator.pBinFileCheckSum, 32));
        sprintf(Concentrator.pDiffFile , "%s%s", base, "Concentrator/NZC_O312-v3.1.diff");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Concentrator/NZC_O312-v3.1.diff.sum", Concentrator.pDiffFileCheckSum, 32));

        /** 원래 Server IP를 얻어오기 새로운 IP 설정 */
        {
            /** 예전 IP 얻어오기 */
            memset(origIpaddr, 0, sizeof(origIpaddr));
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParam("2.1.20");
            nError = Call(invoke->GetHandle(), "199.1");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) 
            {
                delete invoke;
                return;
            }
            memcpy(origIpaddr,invoke->GetHandle()->pResult[0]->stream.p, invoke->GetHandle()->pResult[0]->len);
            delete invoke;

            /** Server IP 변경 */
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParamFormat("2.1.20", VARSMI_STRING, ipaddr, strlen(ipaddr));
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            if(nError != IF4ERR_NOERROR) 
            {
                return;
            }
        }

    }
    ~ConcentratorFixture_Upgrade() 
    {
        /** Web Server 종료 */
        system("./nweb.sh stop");
        {
            CIF4Invoke  *invoke = NULL;
            int nError;

            /** 예전 IP 재설정 */
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParamFormat("2.1.20", VARSMI_STRING, origIpaddr, strlen(origIpaddr));
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
        }

    }

    DistSt Concentrator;
    DistSt OldConcentrator;

    char   origIpaddr[64];
};

struct SensorFixture_Upgrade
{
    SensorFixture_Upgrade() 
    {

        CIF4Invoke  *invoke = NULL;
        int nError;
        char hostname[256];
        char base[64];
        char ipaddr[64];
        char cmd[256];
        hostent *hos = NULL;
        char *addr = NULL;
        int port=29871;

        /** WEB Server를 기동해야 한다 */
        sprintf(cmd,"./nweb.sh -d fixture -p %d start", port);
        CHECK_EQUAL(0, system(cmd));

        IF4API_SetUserCallback(NULL, NULL, UpgradeTestEventHandler, NULL);

        gethostname(hostname, sizeof(hostname));
        hos = gethostbyname(hostname);
        addr = *(hos->h_addr_list);

        sprintf(ipaddr,"%d.%d.%d.%d", (unsigned char)addr[0], (unsigned char)addr[1], (unsigned char)addr[2], (unsigned char)addr[3]);
        sprintf(base,"http://%s:%d/fwImages/", ipaddr, port);

        memset(&GEB04toB15, 0, sizeof(DistSt));
        memset(&GEB15toB04, 0, sizeof(DistSt));

        GEB04toB15.nSourceBuild = 0x04;
        GEB04toB15.nTargetBuild = 0x15;
        sprintf(GEB04toB15.pBinFile , "%s%s", base, "Modem/GE/SM110/GE_SM110_V21_B15.ebl.gz");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Modem/GE/SM110/GE_SM110_V21_B15.ebl.gz.sum", GEB04toB15.pBinFileCheckSum, 32));
        sprintf(GEB04toB15.pDiffFile , "%s%s", base, "Modem/GE/SM110/GE_SM110_V21-04_V21-15.diff");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Modem/GE/SM110/GE_SM110_V21-04_V21-15.diff.sum", GEB04toB15.pDiffFileCheckSum, 32));

        GEB15toB04.nSourceBuild = 0x15;
        GEB15toB04.nTargetBuild = 0x04;
        sprintf(GEB15toB04.pBinFile , "%s%s", base, "Modem/GE/SM110/GE_SM110_V21_B04.ebl.gz");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Modem/GE/SM110/GE_SM110_V21_B04.ebl.gz.sum", GEB15toB04.pBinFileCheckSum, 32));
        sprintf(GEB15toB04.pDiffFile , "%s%s", base, "Modem/GE/SM110/GE_SM110_V21-15_V21-04.diff");
        CHECK_EQUAL(0, GetStream("fixture/fwImages/Modem/GE/SM110/GE_SM110_V21-15_V21-04.diff.sum", GEB15toB04.pDiffFileCheckSum, 32));


        /** 원래 Server IP를 얻어오기 새로운 IP 설정 */
        {
            /** 예전 IP 얻어오기 */
            memset(origIpaddr, 0, sizeof(origIpaddr));
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParam("2.1.20");
            nError = Call(invoke->GetHandle(), "199.1");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) 
            {
                delete invoke;
                return;
            }
            memcpy(origIpaddr,invoke->GetHandle()->pResult[0]->stream.p, invoke->GetHandle()->pResult[0]->len);
            delete invoke;

            /** Server IP 변경 */
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParamFormat("2.1.20", VARSMI_STRING, ipaddr, strlen(ipaddr));
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
            if(nError != IF4ERR_NOERROR) 
            {
                return;
            }
        }
    }
    ~SensorFixture_Upgrade() 
    {
        /** Web Server 종료 */
        system("./nweb.sh stop");
        {
            CIF4Invoke  *invoke = NULL;
            int nError;

            /** 예전 IP 재설정 */
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParamFormat("2.1.20", VARSMI_STRING, origIpaddr, strlen(origIpaddr));
            nError = Call(invoke->GetHandle(), "199.2");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            delete invoke;
        }
    }

    DistSt GEB15toB04;
    DistSt GEB04toB15;

    char   origIpaddr[64];
};


void UpgradeTestEventHandler(char *pszAddress, UINT nSourceId, BYTE *pEvent, int nLength)
{
    IF4_EVENT_FRAME * pFrame = (IF4_EVENT_FRAME *) pEvent;
    MIBValue * pValues;
    SMIValue * pSeek;
    VAROBJECT * pObject;
    int i;

    pValues = (MIBValue *)MALLOC(sizeof(MIBValue)*LittleToHostShort(pFrame->svch.cnt));
    memset(pValues, 0, sizeof(MIBValue)*LittleToHostShort(pFrame->svch.cnt));
    pSeek = (SMIValue *)&pFrame->args;
    for(i=0; i<LittleToHostShort(pFrame->svch.cnt); i++)
    {
        pObject = VARAPI_GetObject(&pSeek->oid);
        CopySMI2MIBValue(&pValues[i], pSeek, pObject->var.type);
        pSeek = (SMIValue *)((BYTE *)pSeek + (pSeek->len + 5));
        if(i) pValues[i-1].pNext = &pValues[i];
    }

    if(pFrame->svch.eventCode.id1 == 214 && pFrame->svch.eventCode.id2 == 1) 
    {
        gDownloadStatus = pValues[1].stream.u8;
    }
    else if(pFrame->svch.eventCode.id1 == 214 && pFrame->svch.eventCode.id2 == 2) 
    {
        if(LittleToHostShort(pFrame->svch.cnt)>0) {
            memcpy(gStartTriger, pValues[0].stream.p, pValues[0].len);
        }
    }

    FREE(pValues);
}

void makeDistributionParam(CIF4Invoke * invoke, const char * tid, BYTE type, const char * model, BYTE ttype,
        UINT step, BYTE mwrite_cnt, UINT retry, BYTE thread_cnt, BYTE install_type,
        WORD old_hw, WORD old_sw, WORD old_build, 
        WORD new_hw, WORD new_sw, WORD new_build,
        const char * binFile, const char * binSum,
        const char * diffFile, const char * diffSum)
{
    invoke->AddParam("1.11", tid);
    invoke->AddParam("1.4", type);
    invoke->AddParam("1.11", model);
    invoke->AddParam("1.4", ttype);
    invoke->AddParam("1.6", step);
    invoke->AddParam("1.4", mwrite_cnt);
    invoke->AddParam("1.6", retry);
    invoke->AddParam("1.4", thread_cnt);
    invoke->AddParam("1.4", install_type);
    invoke->AddParam("1.5", old_hw);
    invoke->AddParam("1.5", old_sw);
    invoke->AddParam("1.5", old_build);
    invoke->AddParam("1.5", new_hw);
    invoke->AddParam("1.5", new_sw);
    invoke->AddParam("1.5", new_build);
    invoke->AddParam("1.11", binFile);
    invoke->AddParam("1.11", binSum);
    invoke->AddParam("1.11", diffFile);
    invoke->AddParam("1.11", diffSum);
}

/** Test Suite #12 Upgrade.
 */
SUITE(_12_Upgrade)
{

/** Coordinator File에 대한 배포를 테스트 한다
  *
  *     - 관련 Issue : #1923
 */
TEST_FIXTURE(CoordiFixture_Upgrade, cmdDistribution_Coordinator )
{
    int nError;

    CIF4Invoke  * invoke = NULL;
    DistSt   * distFixture = NULL;

    CHECK(Coordi.codiFwBuild == 0x05 || Coordi.codiFwBuild == 0x06);
    if(Coordi.codiFwBuild == 0x05) {
        distFixture = &CoordiB05toB06;
    }else if(Coordi.codiFwBuild == 0x06) {
        distFixture = &CoordiB06toB05;
    }else {
        CHECK(Coordi.codiFwBuild == 0x05 || Coordi.codiFwBuild == 0x06);
        return;
    }

    gDownloadStatus = 0xFF;
    memset(gStartTriger, 0, sizeof(gStartTriger));

    /** Upgrade Coordinator */
    {
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        makeDistributionParam(invoke, "triger01", (BYTE) OTA_TYPE_COORDINATOR, "NZM", OTA_TRANSFER_AUTO,
            OTA_STEP_ALL, 1, 1, 1, OTA_INSTALL_AUTO, 
            (WORD) 0x0108, (WORD) 0x0203, distFixture->nSourceBuild,
            (WORD) 0x0108, (WORD) 0x0203, distFixture->nTargetBuild,
            distFixture->pBinFile, distFixture->pBinFileCheckSum,
            distFixture->pDiffFile, distFixture->pDiffFileCheckSum);

        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        if(nError == IF4ERR_NOERROR) {
            UnitTest::TimeHelpers::SleepMs(2 * 60 * 1000);
            CHECK_EQUAL(0x00, gDownloadStatus);
            UnitTest::TimeHelpers::SleepMs(1 * 60 * 1000);
            CHECK(strcmp("triger01", gStartTriger) == 0);
        }else {
            bTearDown = FALSE;
        }

        /** Coordinator 정보를 얻어와서 원하는 버전이 되었는지 확인 */
        {
            CODIENTRY   lCoordi;

            /*-- get coordinator entries --*/
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            nError = Call(invoke->GetHandle(), "101.13");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) 
            {
                delete invoke;
                return;
            }
            memcpy(&lCoordi, (CODIENTRY *)invoke->GetHandle()->pResult[0]->stream.p, sizeof(CODIENTRY));
            delete invoke;

            CHECK_EQUAL((BYTE)distFixture->nTargetBuild, lCoordi.codiFwBuild);
        }
    }
}

/** Coordinator File에 대한 배포를 테스트 한다.
  * 
  * - 실패해야 하는 경우에 대해 테스트 한다.
  *
  *     - 관련 Issue : #1923
 */
TEST_FIXTURE(CoordiFixture_Upgrade, cmdDistribution_Coordinator_FailCase )
{
    int nError;

    CIF4Invoke  * invoke = NULL;
    DistSt   * distFixture = NULL;

    CHECK(Coordi.codiFwBuild == 0x05 || Coordi.codiFwBuild == 0x06);
    if(Coordi.codiFwBuild == 0x05) {
        distFixture = &CoordiB05toB06;
    }else if(Coordi.codiFwBuild == 0x06) {
        distFixture = &CoordiB06toB05;
    }else {
        CHECK(Coordi.codiFwBuild == 0x05 || Coordi.codiFwBuild == 0x06);
        return;
    }

    gDownloadStatus = 0xFF;
    bTearDown = FALSE;

    /** 잘못된 요청에 대한 테스트 */
    {
        /** 인자수 부족 */
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);
        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_INVALID_PARAM, nError);
        delete invoke;


        /** 잘못된 CheckSum Data.
          * Event Handler를 통해서 Download Event가 성공했는지 실패 했는지 검사한다.
         */
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);
        makeDistributionParam(invoke, "triger01", (BYTE) OTA_TYPE_COORDINATOR, "NZM", OTA_TRANSFER_AUTO,
            OTA_STEP_ALL, 1, 1, 1, OTA_INSTALL_AUTO, 
            (WORD) 0x0108, (WORD) 0x0203, distFixture->nSourceBuild,
            (WORD) 0x0108, (WORD) 0x0203, distFixture->nTargetBuild,
            distFixture->pBinFile, "InvalidCheckSum_1234567890123452",
            distFixture->pDiffFile, "InvalidCheckSum_1234567890123452");
        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        UnitTest::TimeHelpers::SleepMs(10 * 1000);
        CHECK_EQUAL(0x01, gDownloadStatus);

        /** 잘못된 버전 정보 (Coordinator는 H/W 버전 정보가 일치해야 한다)
         */
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);
        makeDistributionParam(invoke, "triger01", (BYTE) OTA_TYPE_COORDINATOR, "NZM", OTA_TRANSFER_AUTO,
            OTA_STEP_ALL, 1, 1, 1, OTA_INSTALL_AUTO, 
            (WORD) 0x0108, (WORD) 0x0203, distFixture->nSourceBuild,
            (WORD) 0x0106, (WORD) 0x0203, distFixture->nTargetBuild,
            distFixture->pBinFile, distFixture->pBinFileCheckSum,
            distFixture->pDiffFile, distFixture->pDiffFileCheckSum);
        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_INVALID_VERSION, nError);
        delete invoke;
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);
        makeDistributionParam(invoke, "triger01", (BYTE) OTA_TYPE_COORDINATOR, "NZM", OTA_TRANSFER_AUTO,
            OTA_STEP_ALL, 1, 1, 1, OTA_INSTALL_AUTO, 
            (WORD) 0x0106, (WORD) 0x0203, distFixture->nSourceBuild,
            (WORD) 0x0108, (WORD) 0x0203, distFixture->nTargetBuild,
            distFixture->pBinFile, distFixture->pBinFileCheckSum,
            distFixture->pDiffFile, distFixture->pDiffFileCheckSum);
        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_INVALID_VERSION, nError);
        delete invoke;

    }
}

TEST_FIXTURE(ConcentratorFixture_Upgrade, cmdDistribution_Concentrator )
{
    int nError;

    CIF4Invoke  * invoke = NULL;
    char revision[64];

    gDownloadStatus = 0xFF;
    memset(gStartTriger, 0, sizeof(gStartTriger));

    /** Upgrade Old Concentrator */
    {
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        makeDistributionParam(invoke, "triger02", (BYTE) OTA_TYPE_CONCENTRATOR, "NZC", OTA_TRANSFER_AUTO,
            OTA_STEP_ALL, 1, 1, 1, OTA_INSTALL_FORCE, 
            (WORD) 0x0200, (WORD) 0x0301, OldConcentrator.nSourceBuild,
            (WORD) 0x0200, (WORD) 0x0301, OldConcentrator.nSourceBuild,
            OldConcentrator.pBinFile, OldConcentrator.pBinFileCheckSum,
            "", "");

        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        if(nError == IF4ERR_NOERROR) {
            UnitTest::TimeHelpers::SleepMs(6 * 60 * 1000);
            CHECK_EQUAL(0x00, gDownloadStatus);
            UnitTest::TimeHelpers::SleepMs(1 * 60 * 1000);
            CHECK(strcmp("triger02", gStartTriger) == 0);
        }

        /** Concentrator Revision 정보 얻어오기 */
        {
            memset(revision, 0, sizeof(revision));
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParam("2.1.30");
            nError = Call(invoke->GetHandle(), "199.1");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) 
            {
                delete invoke;
                return;
            }
            memcpy(revision,invoke->GetHandle()->pResult[0]->stream.p, invoke->GetHandle()->pResult[0]->len);
            CHECK_EQUAL(OldConcentrator.nSourceBuild, strtol(revision, (char **)NULL, 10));
            delete invoke;
        }
    }

    gDownloadStatus = 0xFF;
    memset(gStartTriger, 0, sizeof(gStartTriger));
    /** Upgrade Concentrator */
    {
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        makeDistributionParam(invoke, "triger02", (BYTE) OTA_TYPE_CONCENTRATOR, "NZC", OTA_TRANSFER_AUTO,
            OTA_STEP_ALL, 1, 1, 1, OTA_INSTALL_REINSTALL, 
            (WORD) 0x0200, (WORD) 0x0301, Concentrator.nSourceBuild,
            (WORD) 0x0200, (WORD) 0x0301, Concentrator.nTargetBuild,
            Concentrator.pBinFile, Concentrator.pBinFileCheckSum,
            Concentrator.pDiffFile, Concentrator.pDiffFileCheckSum);

        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        if(nError == IF4ERR_NOERROR) {
            UnitTest::TimeHelpers::SleepMs(4 * 60 * 1000);
            CHECK_EQUAL(0x00, gDownloadStatus);
            UnitTest::TimeHelpers::SleepMs(1 * 60 * 1000);
            CHECK(strcmp("triger02", gStartTriger) == 0);
        }
        else return;

        /** Concentrator Revision 정보 얻어오기 */
        {
            memset(revision, 0, sizeof(revision));
            invoke = new CIF4Invoke(gServerAddr, 8000, 3);
            invoke->AddParam("2.1.30");
            nError = Call(invoke->GetHandle(), "199.1");
            CHECK_EQUAL(IF4ERR_NOERROR, nError);
            if(nError != IF4ERR_NOERROR) 
            {
                delete invoke;
                return;
            }
            memcpy(revision,invoke->GetHandle()->pResult[0]->stream.p, invoke->GetHandle()->pResult[0]->len);
            CHECK_EQUAL(Concentrator.nTargetBuild, strtol(revision, (char **)NULL, 10));
            delete invoke;
        }
    }
}

/** Sensor에 대한 배포 기능 테스트.
  *
  *     - 관련 Issue : #1923
 */
TEST_FIXTURE(SensorFixture_Upgrade, cmdDistribution_Sensor_Unicast )
{
    int nError;
    int i=0;

    CIF4Invoke  * invoke = NULL;
    DistSt   * distFixture = &GEB04toB15;
    SENSORINFOENTRYNEW  *pSensor;

    {
        /** Sensor 목록 정보 얻어오기 */
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        invoke->AddParam("1.11", "GE");
        nError = Call(invoke->GetHandle(), "102.26");

        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) {
            delete invoke;
            return;
        }
        CHECK(invoke->GetHandle()->nResultCount > 0);

        for(i=0; i<invoke->GetHandle()->nResultCount; i++)
        {
            pSensor = (SENSORINFOENTRYNEW *)invoke->GetHandle()->pResult[i]->stream.p;
            if(pSensor->sensorState != ENDISTATE_NORMAL) continue;
            if(pSensor->sensorFwBuild == 15) {
                distFixture = &GEB15toB04;
                break;
            }
        }
        delete invoke;
    }


    gDownloadStatus = 0xFF;
    memset(gStartTriger, 0, sizeof(gStartTriger));

    /** Upgrade Coordinator */
    {
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        makeDistributionParam(invoke, "triger03", (BYTE) OTA_TYPE_SENSOR, "GE", OTA_TRANSFER_AUTO,
            OTA_STEP_ALL, 1, 1, 3, OTA_INSTALL_AUTO, 
            (WORD) 0x0203, (WORD) 0x0201, distFixture->nSourceBuild,
            (WORD) 0x0203, (WORD) 0x0201, distFixture->nTargetBuild,
            distFixture->pBinFile, distFixture->pBinFileCheckSum,
            distFixture->pDiffFile, distFixture->pDiffFileCheckSum);

        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        if(nError == IF4ERR_NOERROR) {
            UnitTest::TimeHelpers::SleepMs(2 * 60 * 1000);
            CHECK_EQUAL(0x00, gDownloadStatus);
            UnitTest::TimeHelpers::SleepMs(20 * 60 * 1000);
            CHECK(strcmp("triger03", gStartTriger) == 0);
        }
        /** 10분을 대기한 후에 */
        UnitTest::TimeHelpers::SleepMs(10 * 60 * 1000);

        /** Scan을 수행하기 때문에 별도의 Metering을 하지 않는다 */
#if 0
        /** Metering 명령어를 수행 */
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);
        nError = Call(invoke->GetHandle(), "197.102");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        /** 10분을 대기한 후에 */
        UnitTest::TimeHelpers::SleepMs(10 * 60 * 1000);
#endif

        /** Sensor 목록 정보 얻어오기 */
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        invoke->AddParam("1.11", "GE");
        nError = Call(invoke->GetHandle(), "102.26");

        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) {
            delete invoke;
            return;
        }
        CHECK(invoke->GetHandle()->nResultCount > 0);

        /** F/W가 정상적으로 Upgrade 되었는지 확인 */
        for(i=0; i<invoke->GetHandle()->nResultCount; i++)
        {
            pSensor = (SENSORINFOENTRYNEW *)invoke->GetHandle()->pResult[i]->stream.p;
            if(pSensor->sensorState != ENDISTATE_NORMAL) continue;
            CHECK_EQUAL(((distFixture->nTargetBuild >> 4) & 0x0F)*10 + (distFixture->nTargetBuild & 0x0F) , pSensor->sensorFwBuild);
        }
        delete invoke;
    }
}

/** Sensor에 대한 배포 기능 테스트.
  * ID별 Unicast에 대한 테스트
  *
  *     - 관련 Issue : #1923
 */
TEST_FIXTURE(SensorFixture_Upgrade, cmdDistribution_Sensor_ID_Unicast )
{
    int nError;
    int i=0;

    CIF4Invoke  * invoke = NULL;
    CIF4Invoke  * invoke2 = NULL;
    DistSt   * distFixture = &GEB04toB15;
    SENSORINFOENTRYNEW  *pSensor;

    /** Sensor 목록 정보 얻어오기 */
    invoke2 = new CIF4Invoke(gServerAddr, 8000, 6);

    invoke2->AddParam("1.11", "GE");
    nError = Call(invoke2->GetHandle(), "102.26");

    CHECK_EQUAL(IF4ERR_NOERROR, nError);
    if(nError != IF4ERR_NOERROR) {
        delete invoke2;
        return;
    }
    CHECK(invoke2->GetHandle()->nResultCount > 0);

    for(i=0; i<invoke2->GetHandle()->nResultCount; i++)
    {
        pSensor = (SENSORINFOENTRYNEW *)invoke2->GetHandle()->pResult[i]->stream.p;
        if(pSensor->sensorState != ENDISTATE_NORMAL) continue;
        if(pSensor->sensorFwBuild == 15) {
            distFixture = &GEB15toB04;
            break;
        }
    }

    memset(gStartTriger, 0, sizeof(gStartTriger));

    /** Upgrade Sensor */
    {
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        makeDistributionParam(invoke, "triger05", (BYTE) OTA_TYPE_SENSOR, "GE", OTA_TRANSFER_AUTO,
            OTA_STEP_ALL, 1, 1, 3, OTA_INSTALL_AUTO, 
            (WORD) 0x0203, (WORD) 0x0201, distFixture->nSourceBuild,
            (WORD) 0x0203, (WORD) 0x0201, distFixture->nTargetBuild,
            distFixture->pBinFile, distFixture->pBinFileCheckSum,
            distFixture->pDiffFile, distFixture->pDiffFileCheckSum);

        /** Sensor ID 추가 */
        for(i=0; i<invoke2->GetHandle()->nResultCount; i++)
        {
            pSensor = (SENSORINFOENTRYNEW *)invoke2->GetHandle()->pResult[i]->stream.p;
            if(pSensor->sensorState != ENDISTATE_NORMAL) continue;
            invoke->AddParamFormat("1.14", VARSMI_EUI64, &pSensor->sensorID, sizeof(EUI64));
        }
        delete invoke2;

        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        if(nError == IF4ERR_NOERROR) {
            UnitTest::TimeHelpers::SleepMs(20 * 60 * 1000);
            CHECK(strcmp("triger05", gStartTriger) == 0);
        }
        /** 10분을 대기한 후에 */
        UnitTest::TimeHelpers::SleepMs(10 * 60 * 1000);

        /** Sensor 목록 정보 얻어오기 */
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        invoke->AddParam("1.11", "GE");
        nError = Call(invoke->GetHandle(), "102.26");

        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) {
            delete invoke;
            return;
        }
        CHECK(invoke->GetHandle()->nResultCount > 0);

        /** F/W가 정상적으로 Upgrade 되었는지 확인 */
        for(i=0; i<invoke->GetHandle()->nResultCount; i++)
        {
            pSensor = (SENSORINFOENTRYNEW *)invoke->GetHandle()->pResult[i]->stream.p;
            if(pSensor->sensorState != ENDISTATE_NORMAL) continue;
            CHECK_EQUAL(((distFixture->nTargetBuild >> 4) & 0x0F)*10 + (distFixture->nTargetBuild & 0x0F) , pSensor->sensorFwBuild);
        }
        delete invoke;
    }
}

/** Sensor에 대한 배포 기능 테스트.
  *
  *     - 관련 Issue : #1923
 */
TEST_FIXTURE(SensorFixture_Upgrade, cmdDistribution_Sensor_Multicast )
{
    int nError;
    int i=0;

    CIF4Invoke  * invoke = NULL;
    DistSt   * distFixture = &GEB04toB15;
    SENSORINFOENTRYNEW  *pSensor;

    {
        /** Sensor 목록 정보 얻어오기 */
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        invoke->AddParam("1.11", "GE");
        nError = Call(invoke->GetHandle(), "102.26");

        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) {
            delete invoke;
            return;
        }
        CHECK(invoke->GetHandle()->nResultCount > 0);

        for(i=0; i<invoke->GetHandle()->nResultCount; i++)
        {
            pSensor = (SENSORINFOENTRYNEW *)invoke->GetHandle()->pResult[i]->stream.p;
            if(pSensor->sensorState != ENDISTATE_NORMAL) continue;
            if(pSensor->sensorFwBuild == 15) {
                distFixture = &GEB15toB04;
                break;
            }
        }
        delete invoke;
    }

    memset(gStartTriger, 0, sizeof(gStartTriger));

    /** Upgrade Coordinator */
    {
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        makeDistributionParam(invoke, "triger04", (BYTE) OTA_TYPE_SENSOR, "GE", OTA_TRANSFER_MULTICAST,
            OTA_STEP_ALL, 1, 1, 3, OTA_INSTALL_AUTO, 
            (WORD) 0x0203, (WORD) 0x0201, distFixture->nSourceBuild,
            (WORD) 0x0203, (WORD) 0x0201, distFixture->nTargetBuild,
            distFixture->pBinFile, distFixture->pBinFileCheckSum,
            distFixture->pDiffFile, distFixture->pDiffFileCheckSum);

        nError = Call(invoke->GetHandle(), "198.13");
        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        delete invoke;

        if(nError == IF4ERR_NOERROR) {
            UnitTest::TimeHelpers::SleepMs(20 * 60 * 1000);
            CHECK(strcmp("triger04", gStartTriger) == 0);
        }
#if 0
        /** Multicast시 2시간 40분 이상 걸리기 때문에 결과를 확인하지는 못한다 **/
        /** 40분을 대기한 후에 */
        UnitTest::TimeHelpers::SleepMs(40 * 60 * 1000);

        /** Sensor 목록 정보 얻어오기 */
        invoke = new CIF4Invoke(gServerAddr, 8000, 6);

        invoke->AddParam("1.11", "GE");
        nError = Call(invoke->GetHandle(), "102.26");

        CHECK_EQUAL(IF4ERR_NOERROR, nError);
        if(nError != IF4ERR_NOERROR) {
            delete invoke;
            return;
        }
        CHECK(invoke->GetHandle()->nResultCount > 0);

        /** F/W가 정상적으로 Upgrade 되었는지 확인 */
        for(i=0; i<invoke->GetHandle()->nResultCount; i++)
        {
            pSensor = (SENSORINFOENTRYNEW *)invoke->GetHandle()->pResult[i]->stream.p;
            if(pSensor->sensorState != ENDISTATE_NORMAL) continue;
            CHECK_EQUAL(((distFixture->nTargetBuild >> 4) & 0x0F)*10 + (distFixture->nTargetBuild & 0x0F) , pSensor->sensorFwBuild);
        }
        delete invoke;
#endif
    }
}


} // End of Suite
