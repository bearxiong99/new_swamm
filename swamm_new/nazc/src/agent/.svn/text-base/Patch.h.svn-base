#ifndef __PATCH_HANDLER_H__
#define __PATCH_HANDLER_H__

typedef void (*PatchHandler)(int idx, int patchId, int issueNumber, BOOL state, char * descr);

#if     defined(__PATCH_2_1565__)
void Patch_2_1565_Handler(int idx, int patchId, int issueNumber, BOOL state, char * descr);
#endif
#if     defined(__PATCH_4_1647__)
void Patch_4_1647_Handler(int idx, int patchId, int issueNumber, BOOL state, char * descr);
#endif
#if     defined(__PATCH_6_2084__)
void Patch_6_2084_Handler(int idx, int patchId, int issueNumber, BOOL state, char * descr);
#endif
#if     defined(__PATCH_9_2567__)
void Patch_9_2567_Handler(int idx, int patchId, int issueNumber, BOOL state, char * descr);
#endif

#if     defined(__PATCH_11_65__)
BYTE Patch_11_65_Nuri2Jeju(BYTE energyLevel);
#endif

extern PATCHENTRY m_oPatchInformation[];
extern PatchHandler m_oPatchHandler [];

#endif	// __END_DEVICE_LIST_H__
