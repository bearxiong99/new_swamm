
#include "typedef.h"

#include "gpio.h"
#include "GPIOAPI.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "ShellCommand.h"
#include "Utility.h"

#include "TestZigbee.h"

#include "codiapi.h"

#define UNKNOWN_STR "Unknown"

static char errMsgBuff[256];

CODIDEVICE codiDevice =
{
		CODI_COMTYPE_RS232,				// 디바이스
		"/dev/ttyO2",					// 디바이스명
		115200,							// 속도
		8,								// 데이터 비트
		1,								// 스톱 비트
		CODI_PARITY_NONE,				// 패리터
		CODI_OPT_RTSCTS					// 옵션
};

HANDLE m_codiHandle;

// Parameter
ZIGBEE_CHECK m_ZbParam;

TEST_GROUP m_ZigbeeTest[] =
{
    { "ZigBee module initialize",       testZigBeeInitialize,  { NULL, NULL, NULL, NULL} },
    { "ZigBee power control",           testZigBeePowerControl,  { NULL, NULL, NULL, NULL} },
    { "ZigBee FW version and build",    testZigBeeFw,  { &m_ZbParam.fwVersion, &m_ZbParam.fwBuild, NULL, NULL} },
    { "ZigBee key",                     testZigBeeKey,  { &m_ZbParam.bChkLinkKey, m_ZbParam.linkKey, &m_ZbParam.bChkNetworkKey, m_ZbParam.networkKey} },
    { "ZigBee Tx power",                testZigBeeTxPower,  { &m_ZbParam.bChkTxPower, &m_ZbParam.txPower, NULL, NULL} },
    { "ZigBee Tx mode",                 testZigBeeTxMode,  { &m_ZbParam.bChkTxMode, &m_ZbParam.txMode, NULL, NULL} },
    { NULL, NULL, { NULL, NULL, NULL, NULL } }
};

char* testZigBeeInitialize(const void*, const void*, const void*, const void*)
{
    int nError;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            return const_cast<char*>("FAKETEST");
        }
        return NULL;
    }

    // CODIAPI 초기화
    nError = codiInit();
    if(nError != CODIERR_NOERROR)
    {
        sprintf(errMsgBuff,"%s", codiErrorMessage(nError));
        return errMsgBuff;
    }

    // 새로운 Coordinator 장치를 등록한다
    nError = codiRegister(&m_codiHandle, &codiDevice);
    if(nError != CODIERR_NOERROR)
    {
        codiUnregister(m_codiHandle);
        codiExit();
        sprintf(errMsgBuff,"%s", codiErrorMessage(nError));
        return errMsgBuff;
    }

    // Coordinator 서비스를 시작합니다.
	nError = codiStartup(m_codiHandle);  
	if (nError != CODIERR_NOERROR)
	{
		codiUnregister(m_codiHandle);
		codiExit();
        sprintf(errMsgBuff,"%s", codiErrorMessage(nError));
        return errMsgBuff;
	}

    codiSync(m_codiHandle, 3);

    return NULL;
}

char* testZigBeePowerControl(const void*, const void*, const void*, const void*)
{
    int nState;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            return const_cast<char*>("FAKETEST");
        }
        return NULL;
    }

    GPIOAPI_WriteGpio(GP_CODI_RST_OUT, GPIO_LOW );
    USLEEP(100000);
    GPIOAPI_WriteGpio(GP_CODI_RST_OUT, GPIO_HIGH );

    for(int i=0; i< 30; i++)
    {
        nState = codiGetState(m_codiHandle);
        if(nState == CODISTATE_NORMAL) return NULL;
        USLEEP(100000);
    }

    sprintf(errMsgBuff,"Invalid state code %d", nState);
    return errMsgBuff;
}

char* testZigBeeFw(const void* fFwVersion, const void* nFwBuild, const void*, const void*)
{
    float fwVersion =  *((float*)fFwVersion);
    int fwBuild =  *((int*)nFwBuild);
    COORDINATOR *codi = (COORDINATOR *)m_codiHandle;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            return const_cast<char*>("FAKETEST");
        }
        return NULL;
    }

    if(fwVersion > 0)
    {
        float codiFwVer = ((codi->fw&0xF0)>>4) + (codi->fw&0x0F)*0.1;
        if(codiFwVer != fwVersion)
        {
            sprintf(errMsgBuff,"expected %3.1f, actual %3.1f", fwVersion, codiFwVer);
            return errMsgBuff;
        }
    }

    if(fwBuild > 0)
    {
        int codiFwBuild = codi->build;
        if(codiFwBuild != fwBuild)
        {
            sprintf(errMsgBuff,"expected %d, actual %d", fwBuild, codiFwBuild);
            return errMsgBuff;
        }
    }

    return NULL;
}

char* _printKey(char * buffer, const void* key)
{
    char *skey=(char*)key;

    sprintf(buffer, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
        skey[0], skey[1], skey[2], skey[3], skey[4], skey[5], skey[6], skey[7],
        skey[8], skey[9], skey[10], skey[11], skey[12], skey[13], skey[14], skey[15]);

    return buffer;
}

char* testZigBeeKey(const void* bChkLinkKey, const void* linkKey, const void* bChkNetworkKey, const void* networkKey)
{
    char buffer1[64], buffer2[64];
    COORDINATOR *codi = (COORDINATOR *)m_codiHandle;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            return const_cast<char*>("FAKETEST");
        }
        return NULL;
    }

    if(*((bool*)bChkLinkKey))
    {
        if(memcmp(linkKey, codi->link_key, 16))
        {
            sprintf(errMsgBuff,"expected \"%s\", actual \"%s\"", _printKey(buffer1, linkKey), _printKey(buffer2, codi->link_key));
            return errMsgBuff;
        }
    }

    if(*((bool*)bChkNetworkKey))
    {
        if(memcmp(networkKey, codi->network_key, 16))
        {
            sprintf(errMsgBuff,"expected \"%s\", actual \"%s\"", _printKey(buffer1, networkKey), _printKey(buffer2, codi->network_key));
            return errMsgBuff;
        }
    }

    return NULL;
}

char* testZigBeeTxPower(const void* bChkTxPower, const void* txPower, const void* , const void* )
{
    COORDINATOR *codi = (COORDINATOR *)m_codiHandle;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            return const_cast<char*>("FAKETEST");
        }
        return NULL;
    }

    if(*((bool*)bChkTxPower))
    {
        if(*((int*)txPower) != codi->power)
        {
            sprintf(errMsgBuff,"expected %d, actual %d", *((int*)txPower), codi->power);
            return errMsgBuff;
        }
    }

    return NULL;
}

char* testZigBeeTxMode(const void* bChkTxMode, const void* txMode, const void* , const void* )
{
    COORDINATOR *codi = (COORDINATOR *)m_codiHandle;
    int nTxMode = *((int*)txMode);

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            return const_cast<char*>("FAKETEST");
        }
        return NULL;
    }

    if(*((bool*)bChkTxMode))
    {
        if(nTxMode != codi->txpowermode)
        {
            sprintf(errMsgBuff,"expected %s(%d), actual %s(%d)", 
                    getTxPowerMode(nTxMode), nTxMode,
                    getTxPowerMode(codi->txpowermode), codi->txpowermode);
            return errMsgBuff;
        }
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *getTxPowerMode(int nType)
{
    switch(nType) {
        case 0 : return "Default";
        case 1 : return "Boost";
        case 2 : return "Alternate";
        case 3 : return "Alternate & Boost";
    }
    return UNKNOWN_STR;
}

