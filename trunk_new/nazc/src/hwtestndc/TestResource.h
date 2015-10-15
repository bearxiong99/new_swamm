
#ifndef __RESOURCE_TEST_H__
#define __RESOURCE_TEST_H__

#include "Test.h"

typedef struct {
    float   hwVersion;
    int     memSize;
} RESOURCE_CHECK;


char* testHwVer(const void* hwVer, const void*, const void*, const void*);
char* testMount(const void* point, const void*, const void*, const void*);
char* testMemory(const void* minSize, const void*, const void*, const void*);
char* testChecksum(const void* script, const void*, const void*, const void*);


extern RESOURCE_CHECK m_ResourceParam;
extern TEST_GROUP m_ResourceTest[];

#endif  //__RESOURCE_TEST_H__
