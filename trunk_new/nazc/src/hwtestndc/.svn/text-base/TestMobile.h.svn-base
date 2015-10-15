
#ifndef __MOBILE_TEST_H__
#define __MOBILE_TEST_H__

#include "Test.h"

typedef struct {
    char    moduleVendor[64];
    char    moduleType[64];
    char    modulePath[128];
} MOBILE_CHECK;


char* testMobileInitialize(const void*, const void*, const void*, const void*);
char* testMobileModuleVendor(const void*, const void*, const void*, const void*);
char* testMobileModuleType(const void*, const void*, const void*, const void*);
char* testMobileCPin(const void*, const void*, const void*, const void*);


extern MOBILE_CHECK m_MobileParam;
extern TEST_GROUP m_MobileTest[];

#endif  //__MOBILE_TEST_H__
