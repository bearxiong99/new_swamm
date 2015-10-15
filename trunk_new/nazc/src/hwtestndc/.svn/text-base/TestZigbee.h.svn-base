
#ifndef __ZIGBEE_TEST_H__
#define __ZIGBEE_TEST_H__

#include "Test.h"

typedef struct {
    float   fwVersion;
    int     fwBuild;
    bool    bChkLinkKey; BYTE    linkKey[16];
    bool    bChkNetworkKey; BYTE    networkKey[16];
    bool    bChkTxPower; int txPower;
    bool    bChkTxMode; int txMode;
} ZIGBEE_CHECK;


char* testZigBeeInitialize(const void*, const void*, const void*, const void*);
char* testZigBeePowerControl(const void*, const void*, const void*, const void*);
char* testZigBeeFw(const void*, const void*, const void*, const void*);
char* testZigBeeKey(const void*, const void*, const void*, const void*);
char* testZigBeeTxPower(const void*, const void*, const void*, const void*);
char* testZigBeeTxMode(const void*, const void*, const void*, const void*);

const char *getTxPowerMode(int nType);

extern ZIGBEE_CHECK m_ZbParam;
extern TEST_GROUP m_ZigbeeTest[];

#endif  //__ZIGBEE_TEST_H__
