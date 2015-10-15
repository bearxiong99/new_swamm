/** NDC-I331 hwtest
 *
 * @version 1.0
 *
 */
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "typedef.h"
#include "GPIOAPI.h"

#include "CommonUtil.h"
#include "DebugUtil.h"

#include "MobileClient.h"

#include "Test.h"
#include "TestGpio.h"
#include "TestResource.h"
#include "TestZigbee.h"
#include "TestMobile.h"
#include "TestControl.h"
#include "Utility.h"

#define VERSION         1.0

#define TEST_MASK_NONE              0x00000000
#define TEST_MASK_GPIO              0x00000001
#define TEST_MASK_RESOURCE          0x00000002
#define TEST_MASK_ZB                0x00000004
#define TEST_MASK_MOBILE            0x00000008
#define TEST_MASK_CONTROL           0x00000010
#define TEST_MASK_ALL               0xFFFFFFFF

extern int     m_nDebugLevel;

bool            g_bFakeTest = false;
int             g_nFakeRatio = 0;

typedef struct
{
    const char* name;
    TEST_GROUP* testGroup;
    unsigned int testMask;
} TEST_SHEET;

TEST_SHEET m_TestSheet[] = {
    {"HW GPIO", m_GpioTest, TEST_MASK_GPIO},
    {"Concentrator Resource", m_ResourceTest, TEST_MASK_RESOURCE},
    {"ZigBee Module", m_ZigbeeTest, TEST_MASK_ZB},
    {"Mobile Module", m_MobileTest, TEST_MASK_MOBILE},
    {"Signal Handling", m_ControlTest, TEST_MASK_CONTROL},
    { NULL, NULL, TEST_MASK_NONE }
};

// 이 값은 32(0x20) 보다 작은 값 이어야 한다
#define OPT_MODULE_PATH             1
#define OPT_MODULE_VENDOR           2
#define OPT_MODULE                  3
#define OPT_ZB_FWVER                11
#define OPT_ZB_FWBUILD              12
#define OPT_ZB_LINK_KEY             13
#define OPT_ZB_NETWORK_KEY          14
#define OPT_ZB_TX_POWER             15
#define OPT_ZB_TX_MODE              16
#define OPT_R_HW_VERSION            20
#define OPT_R_MEM_SIZE              21
#define OPT_C_WAIT_TIME             22

#define OPT_DISABLE_GPIO            5
#define OPT_DISABLE_RESOURCE        6
#define OPT_DISABLE_ZB              7
#define OPT_DISABLE_MOBILE          8
#define OPT_DISABLE_CONTROL         9

#define OPT_ENABLE_FAKE_TEST        23
#define OPT_FAKE_RATIO              24

static struct option long_options[] =
{
    {"debug",       required_argument,  0,      'd'},
    {"module",      required_argument,  0,      OPT_MODULE},
    {"module-path", required_argument,  0,      OPT_MODULE_PATH},
    {"module-vendor", required_argument,  0,    OPT_MODULE_VENDOR},
    {"zb-fwver",    required_argument,  0,      OPT_ZB_FWVER},
    {"zb-fwbuild",  required_argument,  0,      OPT_ZB_FWBUILD},
    {"zb-link-key", required_argument,  0,      OPT_ZB_LINK_KEY},
    {"zb-netw-key", required_argument,  0,      OPT_ZB_NETWORK_KEY},
    {"zb-tx-power", required_argument,  0,      OPT_ZB_TX_POWER},
    {"zb-tx-mode",  required_argument,  0,      OPT_ZB_TX_MODE},
    {"hw-ver",      required_argument,  0,      OPT_R_HW_VERSION},
    {"mem-size",    required_argument,  0,      OPT_R_MEM_SIZE},
    {"ctrl-wait",   required_argument,  0,      OPT_C_WAIT_TIME},
    {"disable-gpio",no_argument,        0,      OPT_DISABLE_GPIO},
    {"disable-resource",no_argument,    0,      OPT_DISABLE_RESOURCE},
    {"disable-zb",  no_argument,        0,      OPT_DISABLE_ZB},
    {"disable-mobile",no_argument,      0,      OPT_DISABLE_MOBILE},
    {"disable-ctrl",no_argument,        0,      OPT_DISABLE_CONTROL},
    {"enable-fake", no_argument,        0,      OPT_ENABLE_FAKE_TEST},
    {"fake-ratio",  required_argument,  0,      OPT_FAKE_RATIO},
    {"help",        no_argument,        0,      'h'},
    {0, 0, 0, 0}
};

void usage(char *program)
{
    fprintf(stderr,"%s for NDC-I331 v%3.1f -[dh] --hw-ver\n", program, VERSION);
    fprintf(stderr," Concentrator Options\n");
    fprintf(stderr,"     --hw-ver           concentrator HW version (mandatory)\n");
    fprintf(stderr,"     --mem-size         concentrator minimun mem size (Kb)\n");
    fprintf(stderr," Mobile Module Options\n");
    fprintf(stderr,"     --module           module type(default:ge910,ue910,ame5530)\n");
    fprintf(stderr,"     --module-path      module path(default:/dev/ttyO1)\n");
    fprintf(stderr,"     --module-vendor    module vendor(only telit)\n");
    fprintf(stderr," ZigBee Module Options\n");
    fprintf(stderr,"     --zb-fwver         zigbee fw version\n");
    fprintf(stderr,"     --zb-fwbuild       zigbee fw build number\n");
    fprintf(stderr,"     --zb-link-key      zigbee link key\n");
    fprintf(stderr,"     --zb-netw-key      zigbee netwokr key\n");
    fprintf(stderr,"     --zb-tx-power      zigbee Tx power\n");
    fprintf(stderr,"     --zb-tx-mode       zigbee Tx mode(0,1,2,3)\n");
    fprintf(stderr," Control Options\n");
    fprintf(stderr,"     --ctrl-wait        control wait time(default: 60 secs)\n");
    fprintf(stderr," Test Disables\n");
    fprintf(stderr,"     --disable-gpio     disable gpio test\n");
    fprintf(stderr,"     --disable-resource disable resource test\n");
    fprintf(stderr,"     --disable-zb       disable zigbee module test\n");
    fprintf(stderr,"     --disable-mobile   disable mobile module test\n");
    fprintf(stderr,"     --disable-ctrl     disable signal handling test\n");
    fprintf(stderr," Fake Table\n");
    fprintf(stderr,"     --enable-fake      enable fake test\n");
    fprintf(stderr,"     --fake-ratio       fake test ratio(10^-4)\n");
    fprintf(stderr," Other Options\n");
    fprintf(stderr,"  -d --debug          enable debug log\n");
    fprintf(stderr,"  -h --help           display this message\n");
    exit(1);
}

void printZbOptions(ZIGBEE_CHECK* zbp)
{
    int i;
    if(zbp->fwVersion > 0.0)
        fprintf(stdout, " zb-fwver=%3.1f", zbp->fwVersion);
    if(zbp->fwBuild > 0)
        fprintf(stdout, " zb-fwbuild=%d", zbp->fwBuild);
    if(zbp->bChkLinkKey)
    {
        fprintf(stdout, " zb-link-key=\"%02X", zbp->linkKey[0]);
        for(i=1;i<16;i++)
            fprintf(stdout, " %02X", zbp->linkKey[i]);
        fprintf(stdout, "\"");
    }
    if(zbp->bChkNetworkKey)
    {
        fprintf(stdout, " zb-netw-key=\"%02X", zbp->networkKey[0]);
        for(i=1;i<16;i++)
            fprintf(stdout, " %02X", zbp->networkKey[i]);
        fprintf(stdout, "\"");
    }
    if(zbp->bChkTxPower)
        fprintf(stdout," zb-tx-power=%d", zbp->txPower);
    if(zbp->bChkTxMode)
        fprintf(stdout," zb-tx-mode=%s(%d)", getTxPowerMode(zbp->txMode), zbp->txMode);
}

void printMobileOptions(MOBILE_CHECK* mp)
{
    fprintf(stdout, " module=%s", mp->moduleType);
    fprintf(stdout, " module-path=%s", mp->modulePath);
    fprintf(stdout, " module-vendor=%s", mp->moduleVendor);
}

void printResourceOptions(RESOURCE_CHECK* rp)
{
    fprintf(stdout, " hw-ver=%3.1f", rp->hwVersion);
    fprintf(stdout, " mem-size=%d", rp->memSize);
}

void printCtrlOptions(CONTROL_CHECK* cp)
{
    fprintf(stdout, " wait=%d", cp->nWaitTime);
}

int testComplete(void)
{
    int fd;

    if((fd=open("/app/conf/.factoryTestCompleted", O_WRONLY | O_CREAT))<0) return fd;

    close(fd);
    return 0;
}

int main(int argc, char * argv[])
{
    int optionIndex = 0;
    const char* paramModuleName = "ge910";
    const char* paramModulePath = "/dev/ttyO1";
    const char* paramModuleVendor = "telit";
    unsigned int nTestMask = TEST_MASK_ALL;
    int c, i;
    char *p, *token;

    // Parameter setting
    memset(&m_ResourceParam, 0, sizeof(RESOURCE_CHECK));
    memset(&m_MobileParam, 0, sizeof(MOBILE_CHECK));
    memset(&m_ZbParam, 0, sizeof(ZIGBEE_CHECK));
    memset(&m_CtrlParam, 0, sizeof(CONTROL_CHECK));

    m_ResourceParam.memSize = 512;
    m_CtrlParam.nWaitTime = 60;
    strcpy(m_MobileParam.moduleType, paramModuleName);
    strcpy(m_MobileParam.modulePath, paramModulePath);
    strcpy(m_MobileParam.moduleVendor, paramModuleVendor);

    while((c = getopt_long(argc, argv, "dh", long_options, &optionIndex)) != -1)
    {
        switch(c)
        {
            case 'd':
                m_nDebugLevel = 0;
                break;
            case OPT_MODULE:
                strcpy(m_MobileParam.moduleType, optarg);
                break;
            case OPT_MODULE_PATH:
                strcpy(m_MobileParam.modulePath, optarg);
                break;
            case OPT_MODULE_VENDOR:
                strcpy(m_MobileParam.moduleVendor, optarg);
                break;
            case OPT_ZB_FWVER:
                m_ZbParam.fwVersion = (float)strtof(optarg,(char**)NULL);
                break;
            case OPT_ZB_FWBUILD:
                m_ZbParam.fwBuild = (int)strtol(optarg,(char**)NULL, 10);
                break;
            case OPT_ZB_LINK_KEY:
                m_ZbParam.bChkLinkKey = true;
                for(p=optarg,i=0; i<16 ; p=NULL, i++)
                {
                    token = strtok(p," ");
                    if(token == NULL) break;
                    m_ZbParam.linkKey[i] = (unsigned char)(strtol(token,(char**)NULL,16)&0xFF);
                }
                break;
            case OPT_ZB_NETWORK_KEY:
                m_ZbParam.bChkNetworkKey = true;
                for(p=optarg,i=0; i<16 ; p=NULL,i++)
                {
                    token = strtok(p," ");
                    if(token == NULL) break;
                    m_ZbParam.networkKey[i] = (unsigned char)(strtol(token,(char**)NULL,16)&0xFF);
                }
                break;
            case OPT_ZB_TX_POWER:
                m_ZbParam.bChkTxPower = true;
                m_ZbParam.txPower = (int)strtol(optarg,(char**)NULL,10);
                break;
            case OPT_ZB_TX_MODE:
                m_ZbParam.bChkTxMode = true;
                m_ZbParam.txMode = (int)strtol(optarg,(char**)NULL,10);
                break;
            case OPT_R_HW_VERSION:
                m_ResourceParam.hwVersion = (float)strtof(optarg,(char**)NULL);
                break;
            case OPT_R_MEM_SIZE:
                m_ResourceParam.memSize = (int)strtol(optarg, (char**)NULL, 10);
                break;
            case OPT_C_WAIT_TIME:
                m_CtrlParam.nWaitTime = (int)strtol(optarg, (char**)NULL, 10);
                break;
            case OPT_DISABLE_GPIO: nTestMask &= ~(TEST_MASK_GPIO); break;
            case OPT_DISABLE_RESOURCE: nTestMask &= ~(TEST_MASK_RESOURCE); break;
            case OPT_DISABLE_ZB: nTestMask &= ~(TEST_MASK_ZB); break;
            case OPT_DISABLE_MOBILE: nTestMask &= ~(TEST_MASK_MOBILE); break;
            case OPT_DISABLE_CONTROL: nTestMask &= ~(TEST_MASK_CONTROL); break;
            /** Fake Test parameter */
            case OPT_ENABLE_FAKE_TEST: g_bFakeTest = true; break;
            case OPT_FAKE_RATIO: 
                /** Random 하게 발생할 오류를 위한 비율. 백분율이 아니라 천분율(10^-4) 값이다 */
                g_nFakeRatio = (int)strtol(optarg, (char**)NULL, 10);
                {
                    time_t now;
                    time(&now);
                    srand(now);
                }
                break;
            case 'h':
            default:
                usage(argv[0]);
                break;
        }
    }

    if(m_ResourceParam.hwVersion == 0.0)
    {
        XDEBUG("Invalid parameter (need hw version)\r\n");
        usage(argv[0]);
    }

    XDEBUG("NURITelecom NDC-I331 hwtest v1.0");
    fprintf(stdout,"%s v%3.1f", argv[0], VERSION);
    printResourceOptions(&m_ResourceParam);
    printMobileOptions(&m_MobileParam);
    printZbOptions(&m_ZbParam);
    printCtrlOptions(&m_CtrlParam);
    fprintf(stdout,"\n");

    UINT nAddr, nSubNet, nGateway;
    BYTE mac[6] = {0,};
    GetEthernetInfo(const_cast<char*>("eth0"), &nAddr, &nSubNet, &nGateway, mac);

    /** Fake Test 일 경우 MAC Address를 가상으로 만든다 */
    if(g_bFakeTest)
    {
        unsigned int nmac = 0;
        FILE *fp = fopen("/app/sw/fakemac.dat", "r");
        if(fp != NULL)
        {
            fscanf(fp, "%u", &nmac);
            fclose(fp);
        }
        unsigned int bnmac = htonl(nmac);
        memcpy(mac, &bnmac, 4);
        // 하나 증가
        nmac ++;
        if((fp = fopen("/app/sw/fakemac.dat", "w")) != NULL)
        {
            fprintf(fp,"%u", nmac);
            fclose(fp);
        }
    }

    fprintf(stdout,"ID %02X:%02X:%02X:%02X:%02X:%02X\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // GPIO Initialize
    GPIOAPI_Initialize(TRUE, paramModuleName, FALSE);

    // Run Test
    char* result;
    int idx=0, subidx=0;
    int tcnt=1;
    TEST_GROUP *pGroup;

    struct  timeval tmStart, tmGStart, tmTStart, tmNow;

    gettimeofday((struct timeval *)&tmStart, NULL);
    for(;m_TestSheet[idx].name != NULL; idx++)
    {
        if(!(m_TestSheet[idx].testMask & nTestMask)) continue;

        gettimeofday((struct timeval *)&tmGStart, NULL);
        fprintf(stdout,"%s check start\n", m_TestSheet[idx].name);

        pGroup = m_TestSheet[idx].testGroup;
        for(subidx=0; pGroup[subidx].name != NULL; subidx++)
        {
            gettimeofday((struct timeval *)&tmTStart, NULL);
            fprintf(stdout,"%d %s check ", tcnt++, pGroup[subidx].name); fflush(stdout);
            if((result=pGroup[subidx].fn(pGroup[subidx].param[0],
                            pGroup[subidx].param[1],
                            pGroup[subidx].param[2],
                            pGroup[subidx].param[3])) != NULL)
            {
                char* newLine = strchr(result,'\n');
                if(newLine != NULL) *newLine = 0x00;

                gettimeofday((struct timeval *)&tmNow, NULL);
                fprintf(stdout," [%s] %d\n", result, GetTimevalInterval(&tmTStart, &tmNow));
                fprintf(stdout,"%s check end %d\n", m_TestSheet[idx].name, GetTimevalInterval(&tmGStart, &tmNow));
                goto BREAK;
            }
            else
            {
                gettimeofday((struct timeval *)&tmNow, NULL);
                fprintf(stdout," [Ok] %d\n", GetTimevalInterval(&tmTStart, &tmNow));
            }
        }
        gettimeofday((struct timeval *)&tmNow, NULL);
        fprintf(stdout,"%s check end %d\n", m_TestSheet[idx].name, GetTimevalInterval(&tmGStart, &tmNow));
    }
    gettimeofday((struct timeval *)&tmNow, NULL);
    fprintf(stdout,"Test Result [Ok] %d\n", GetTimevalInterval(&tmStart, &tmNow));


    /** Fake Test 일 경우 */
    if(g_bFakeTest)
    {
        FILE *fp = fopen("/app/sw/fakesucc.list", "a");
        if(fp != NULL)
        {
            fprintf(fp,"ID %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            fclose(fp);
        }
    }
    else
    {
        testComplete(); // Success
    }
    m_pMobileClient->Destroy();

    return 0;
BREAK:
    gettimeofday((struct timeval *)&tmNow, NULL);
    fprintf(stdout,"Test Result [Fail] %d\n", GetTimevalInterval(&tmStart, &tmNow));
    if(g_bFakeTest)
    {
        FILE *fp = fopen("/app/sw/fakefail.list", "a");
        if(fp != NULL)
        {
            fprintf(fp,"ID %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            fclose(fp);
        }
    }
    m_pMobileClient->Destroy();
    return 1;
}

