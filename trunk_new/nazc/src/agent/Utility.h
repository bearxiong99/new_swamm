#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "Chunk.h"

UINT GetSysUpTime();
void untilcopy(char *dest, char *src, const char *tag);
void asciicopy(char *dest, char *src, int max=0);
BOOL GetLocalAddress(char *pszAddress);
void PutFileAddress(const char *pszFileName, BYTE *addr);

void GetEthernetInfo(char *pszName, UINT *addr, UINT *subnet, UINT *gateway, BYTE *phyAddr);
void GetEnvInfo(char *kernel, char *gcc, char *cpu, UINT *mips);

int GetSessionType(WORKSESSION *pSession);
#if 0
void AddAccessLog(const char *fmt, ...);
void AddUpgradeLog(const char *fmt, ...);
void XCOMMLOG(const char *fmt, ...);
#endif

int sensorRetryCount(EUI64 *id, int nDefault);
int sensorConnect(HANDLE endi, int nDefaultTimeout);
int sensorCommTimeout(EUI64 *id, int nDefaultTimeout);

BYTE LoadHwVersion();
void LoadSensorType();
char * GetSensorTypeName(BYTE pulseType);
BYTE GetServiceTypeBySensorType(BYTE pulseType);
BYTE GetSensorTypeNumber(char * szModel) ;

BOOL LoadPatchList();
BOOL SavePatchList();
void DisplayPatchInfo ();

BOOL InitializeDirectories(void);
BOOL DestroyDirectories(void);

int RouteDiscoveryControl(BOOL onoff);

BOOL IsBusyCoreService(void);

BOOL UpgradeConcentratorSystem(IF4Wrapper *pWrapper, BYTE UpgradeType, const char *szAddress, const char * upgradeFile);

int GetTagValue(const char *pszFileName, const char *pszTag, char *pszValue, int len);
void SetTagValue(const char *pszFileName, const char *pszTag, const char *pszValue);

int GetPppInfo_Apn(char *apn);
int GetPppInfo_User(char *pszUser);
int GetPppInfo_Password(char *pszPassword);

void SetPppInfo_Apn(char *apn);
void SetPppInfo_User(char *pszUser);
void SetPppInfo_Password(char *pszPassword);
void ExecutePPPD();

BOOL IsValidEUI64(OID3 *id);
UINT GetValidIntervalSchedulerValue(UINT nMin);

#endif	// __UTILITY_H__
