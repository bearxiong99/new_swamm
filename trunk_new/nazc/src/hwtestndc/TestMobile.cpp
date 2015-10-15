
#include "typedef.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "ShellCommand.h"

#include "MobileClient.h"
#include "TestMobile.h"
#include "Utility.h"

#define UNKNOWN_STR "Unknown"

static char errMsgBuff[256];

CMobileClient the_MobileClient;

// Parameter
MOBILE_CHECK m_MobileParam;

TEST_GROUP m_MobileTest[] =
{
    { "Mobile initialize",              testMobileInitialize,{ NULL, NULL, NULL, NULL} },
    { "Mobile module vendor",           testMobileModuleVendor,{ m_MobileParam.moduleVendor, NULL, NULL, NULL} },
    { "Mobile module type",             testMobileModuleType,  { m_MobileParam.moduleType, NULL, NULL, NULL} },
// HW실 요청으로 SIM Card test 제외 (2014-12-16)
    //{ "Mobile PIN ready",               testMobileCPin,  { NULL, NULL, NULL, NULL} },
    { NULL, NULL, { NULL, NULL, NULL, NULL } }
};

int m_nLimit = 10;

char* testMobileInitialize(const void*, const void*, const void*, const void*)
{
    char buffer[256]={0,};
    int nLen = 0;
    int cnt=0;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            goto FAIL;
        }
        return NULL;
    }

    if(!m_pMobileClient->Initialize(m_MobileParam.modulePath))
    {
FAIL:
        sprintf(errMsgBuff,"Initialize fail");
        return errMsgBuff;
    }

    m_pMobileClient->WriteToModem("ATE0\r\n");
    for(nLen=1; nLen > 0 && cnt < m_nLimit; cnt++)
    {
        nLen = m_pMobileClient->ReadLineFromModem(buffer, sizeof(buffer)-2, 1000);
        if(nLen > 1)
        {
            break;
        }
    }
    m_pMobileClient->Flush();

    return NULL;
}

char* testMobileModuleVendor(const void* moduleVendor, const void*, const void*, const void*)
{
    char *sModuleVendor = const_cast<char*>((char*)moduleVendor);
    char buffer[256]={0,};
    int nLen = 0;
    bool bOk = false;
    int cnt=0;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            sprintf(buffer,"FAKETEST");
            goto FAIL;
        }
        return NULL;
    }

    m_pMobileClient->WriteToModem("AT+CGMI\r\n");
    for(nLen=1; nLen > 0 && cnt < m_nLimit; cnt++)
    {
        nLen = m_pMobileClient->ReadLineFromModem(buffer, sizeof(buffer)-2, 1000);
        if(nLen > 1)
        {
            if(strncasecmp(sModuleVendor, buffer, strlen(sModuleVendor)))
            {
                break;
            }
            else
            {
                bOk = true;
                break;
            }
        }
    }
    m_pMobileClient->Flush();

    if(bOk) return NULL;
FAIL:
    sprintf(errMsgBuff,"expected %s, actual %s", sModuleVendor, buffer);
    return errMsgBuff;
}

char* testMobileModuleType(const void* moduleType, const void*, const void*, const void*)
{
    char *sModuleType = const_cast<char*>((char*)moduleType);
    char buffer[256]={0,};
    int nLen = 0;
    bool bOk = false;
    int cnt=0;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            sprintf(buffer,"FAKETEST");
            goto FAIL;
        }
        return NULL;
    }

    m_pMobileClient->WriteToModem("AT+CGMM\r\n");
    for(nLen=1; nLen > 0 && cnt < m_nLimit; cnt++)
    {
        nLen = m_pMobileClient->ReadLineFromModem(buffer, sizeof(buffer)-2, 1000);
        if(nLen > 1)
        {
            if(strncasecmp(sModuleType, buffer, strlen(sModuleType)))
            {
                break;
            }
            else
            {
                bOk = true;
                break;
            }
        }
    }
    m_pMobileClient->Flush();

    if(bOk) return NULL;
FAIL:
    sprintf(errMsgBuff,"expected %s, actual %s", sModuleType, buffer);
    return errMsgBuff;
}

char* testMobileCPin(const void*, const void*, const void*, const void*)
{
    char buffer[256]={0,};
    const char* readyStr="+CPIN: READY";
    int nLen = 0;
    bool bOk = false;
    int cnt=0;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            sprintf(buffer,"FAKETEST");
            goto FAIL;
        }
        return NULL;
    }

    m_pMobileClient->WriteToModem("AT+CPIN?\r\n");
    for(nLen=1; nLen > 0 && cnt < m_nLimit; cnt++)
    {
        nLen = m_pMobileClient->ReadLineFromModem(buffer, sizeof(buffer)-2, 1000);
        if(nLen > 1)
        {
            if(strncasecmp(readyStr, buffer, strlen(readyStr)))
            {
                break;
            }
            else
            {
                bOk = true;
                break;
            }
        }
    }
    m_pMobileClient->Flush();

    if(bOk) return NULL;
FAIL:
    sprintf(errMsgBuff,"expected %s, actual %s", readyStr, buffer);
    return errMsgBuff;
}

