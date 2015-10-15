
#ifndef __CONTROL_TEST_H__
#define __CONTROL_TEST_H__

#include "Test.h"

typedef struct {
    int     nWaitTime;
} CONTROL_CHECK;

char* testCaseOpen(const void*, const void*, const void*, const void*);
char* testPowerFail(const void*, const void*, const void*, const void*);
char* testLowBattery(const void*, const void*, const void*, const void*);

extern CONTROL_CHECK m_CtrlParam;
extern TEST_GROUP m_ControlTest[];

#endif  //__CONTROL_TEST_H__
