
#include <stdio.h>
#include <stdlib.h>

#include "ShellCommand.h"
#include "TestResource.h"
#include "Utility.h"

TEST_GROUP m_ResourceTest[] =
{
    { "HW version",         testHwVer,          { &m_ResourceParam.hwVersion, NULL, NULL, NULL} },
    { "/app mount",         testMount,          { "/app", NULL, NULL, NULL} },
    { "memory",             testMemory,         { &m_ResourceParam.memSize, NULL, NULL, NULL} },
    { "fw checksum",        testChecksum,       { "/app/sw/verify.script", NULL, NULL, NULL} },
    { NULL, NULL, { NULL, NULL, NULL, NULL } }
};

static char errMsgBuff[256];
RESOURCE_CHECK m_ResourceParam;

char* testHwVer(const void* pHwVer, const void*, const void*, const void*)
{
    const char * procMemInfo = "/proc/cmdline";
    const char * strHwVer = "hwver";
    char *findHwVer=NULL;
    char szLine[512];
    int nSize = sizeof(szLine);
    float fHwVer = *((float*)pHwVer);
    float ver=0.0;
    FILE* fp;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            goto FAIL;
        }
        return NULL;
    }

    if((fp=fopen(procMemInfo, "r"))==NULL) 
    {
FAIL:
        sprintf(errMsgBuff, "access cmdline fail");
        return errMsgBuff;
    }
    while(fgets(szLine, nSize, fp)!=NULL)
    {
        if((findHwVer=strstr(szLine, strHwVer)) != NULL)
        {
            sscanf(findHwVer,"hwver=%f", &ver);
            if(fHwVer == ver)
            {
                fclose(fp);
                return NULL;
            }
            break;
        }
    }

    fclose(fp);

    sprintf(errMsgBuff, "expected %3.1f, actual %3.1f", fHwVer, ver);
    return errMsgBuff;
}

char* testMount(const void* point, const void*, const void*, const void*)
{
    const char * procMount = "/proc/mounts";
    const char * app = (char*)point;
    char fsType[64];
    char fsName[64];
    char szLine[256];
    int nSize = sizeof(szLine);
    FILE* fp;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            goto FAIL;
        }
        return NULL;
    }

    if((fp = fopen(procMount, "r"))==NULL)
    {
FAIL:
        sprintf(errMsgBuff, "access mount info fail");
        return errMsgBuff;
    }
    while(fgets(szLine, nSize, fp)!=NULL)
    {
        if(sscanf(szLine,"%s %s", fsType, fsName) != EOF)
        {
            if(!strcmp(app, fsName))
            {
                fclose(fp);
                return NULL;
            }
        }
    }

    fclose(fp);

    sprintf(errMsgBuff, "%s not found", app);
    return errMsgBuff;
}

char* testMemory(const void* minSize, const void*, const void*, const void*)
{
    const char * procMemInfo = "/proc/meminfo";
    const char * memTotal = "MemTotal:";
    char title[64];
    char value[32];
    char unit[64];
    char szLine[256];
    int nSize = sizeof(szLine);
    int memMinSize = *((int*)minSize);
    FILE* fp;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            goto FAIL;
        }
        return NULL;
    }

    if((fp=fopen(procMemInfo, "r"))==NULL)
    {
FAIL:
        sprintf(errMsgBuff, "access mem info fail");
        return errMsgBuff;
    }
    while(fgets(szLine, nSize, fp)!=NULL)
    {
        if(sscanf(szLine,"%s %s %s", title, value, unit) != EOF)
        {
            if(!strcmp(memTotal, title))
            {
                int memSize = strtol(value,(char**)NULL,10);

                fclose(fp);

                if(memSize > memMinSize*1000)
                {
                    return NULL;
                }
                sprintf(errMsgBuff, "expected over 512000, actual %d", memSize);
                return errMsgBuff;
            }
        }
    }

    fclose(fp);

    sprintf(errMsgBuff, "MemTotal not found");
    return errMsgBuff;
}

char* testChecksum(const void* script, const void*, const void*, const void*)
{
    CChunk chunk;

    if(IS_FAKE)
    {
        if(IS_FAKE_FAIL) 
        {
            return const_cast<char*>("FAKETEST");
        }
        return NULL;
    }

    int  res = GetShellCommandResult(&chunk, (char*)script);
    if(res)
    {
        return chunk.GetChunk()->pszBuffer;
    }

    return NULL;
}
