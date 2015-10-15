#include <limits.h>
#include "common.h"
#include "varapi.h"
#include "Variable.h"

#include "CommonUtil.h"
#include "SystemUtil.h"
#include "Utility.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "EndDeviceList.h"
#include "SensorTimesync.h"

#include "AgentService.h"

#include "Patch.h"

#if     defined(__PATCH_2_1565__)
extern BYTE     m_nTimeSyncStrategy;
extern BYTE     m_nOpMode;
#endif

/*-- Patch 정보 --*/
PATCHENTRY m_oPatchInformation[] = {
#if     defined(__PATCH_1_1540__)
    { 1, 1540, FALSE, "Clear Repeating Day" },              // 진해 수도 시스템에서 센서버그를 회피하기 위한 기능
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_2_1565__)
    { 2, 1565, FALSE, "Clear Sensor Information" },         // Revision 1703 이전 버전에서 이후 버전으로 업그레이드시 센서 정보 삭제
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_3_1573__)
    { 3, 1573, FALSE, "Disable Keepalive Time Protection" },// Keepalive Time에 검침이 안되도록 막는 로직을 Disable 시킨다.
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_4_1647__)
    { 4, 1647, FALSE, "Using the Direct Communication" },   // EUI64 ID를 이용한 Binding 대신 Short ID를 이용한 Direct Communication 사용
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_5_1949__)
    { 5, 1949, FALSE, "Always Monitoring Mode" },           // 항상 Enable Monitoring 상태가 되게 한다
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_6_2084__)
    { 6, 2084, FALSE, "Show Hidden Command" },              // CLI Hidden Command를 볼 수 있게 한다
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_7_2097__)
    { 7, 2097, FALSE, "Disable Duplicated Data Check" },    // 중복 데이터 검사 기능을 Disable 시킨다.
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_8_2305__)
    { 8, 2305, FALSE, "Enable Fire Alarm Broadcasting" },   // Fire Alarm이 발생했을 때 Message Broadcasting
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_9_2567__)
    { 9, 2567, FALSE, "Remove Link Procedure" },            // Link 없이 통신 하기
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_10_2808__)
    {10, 2808, FALSE, "Hour Metering (DLMS)" },             // DLMS Meter에서 시간단위 검침
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
#if     defined(__PATCH_11_65__)
    {11, 65, FALSE, "JEJU SP DR Level" },                   // 제주 실증단지 DR Level 적용
#else
    { UINT_MAX, 0, FALSE, "" },
#endif
    { 0, 0, FALSE, "" }
};

PatchHandler m_oPatchHandler [] = {
    NULL,                       // PATCH_1_1540
#if     defined(__PATCH_2_1565__)
    Patch_2_1565_Handler,       // PATCH_2_1565
#else
    NULL,
#endif
    NULL,                       // PATCH_3_1573
#if     defined(__PATCH_4_1647__)
    Patch_4_1647_Handler,       // PATCH_4_1647
#else
    NULL,
#endif
    NULL,                       // PATCH_5_1949
#if     defined(__PATCH_6_2084__)
    Patch_6_2084_Handler,       // PATCH_6_2084
#else
    NULL,
#endif
    NULL,                       // PATCH_7_2097
    NULL,                       // PATCH_8_2305
#if     defined(__PATCH_9_2567__)
    Patch_9_2567_Handler,       // PATCH_9_2567
#else
    NULL,
#endif
    NULL,                       // PATCH_10_2808
    NULL,                       // PATCH_11_65
    NULL
}; 

#if     defined(__PATCH_1_1540__)
BOOL    *m_pbPatch_1_1540 = &(m_oPatchInformation[0].patchState);
#endif
#if     defined(__PATCH_3_1573__)
BOOL    *m_pbPatch_3_1573 = &(m_oPatchInformation[2].patchState);
#endif
#if     defined(__PATCH_4_1647__)
BOOL    *m_pbPatch_4_1647 = &(m_oPatchInformation[3].patchState);
#endif
#if     defined(__PATCH_5_1949__)
BOOL    *m_pbPatch_5_1949 = &(m_oPatchInformation[4].patchState);
#endif
#if     defined(__PATCH_7_2097__)
BOOL    *m_pbPatch_7_2097 = &(m_oPatchInformation[6].patchState);
#endif
#if     defined(__PATCH_8_2305__)
BOOL    *m_pbPatch_8_2305 = &(m_oPatchInformation[7].patchState);
#endif
#if     defined(__PATCH_9_2567__)
BOOL    *m_pbPatch_9_2567 = &(m_oPatchInformation[8].patchState);
#endif
#if     defined(__PATCH_10_2808__)
BOOL    *m_pbPatch_10_2808 = &(m_oPatchInformation[9].patchState);
#endif
#if     defined(__PATCH_11_65__)
BOOL    *m_pbPatch_11_65 = &(m_oPatchInformation[10].patchState);
#endif


/** Patch-2-1565 Handler
 *  NZC Revision 1703 이전 버전에서 이후 버전으로 Upgrade시 EndDeviceList 구조체 변경으로
 *  Sensor List의 삭제가 필요하다. 이 문제를 해결하기 위해 이 패치를 Enable 시키면
 *  센서 목록을 삭제하고 MTOR과 Timesync를 전송해서 새롭게 센서 목록을 업데이트 시킨다.
 *  (센서를 실제로 Leave 시키지는 않는다)
 *  이 Handler가 실행되고 나서 patchState는 항상 FALSE가 된다.
 */
#if     defined(__PATCH_2_1565__)
void Patch_2_1565_Handler(int idx, int patchId, int issueNumber, BOOL state, char * descr)
{
    BYTE            nTimeSyncType = TIMESYNC_TYPE_LONG;

    XDEBUG("Patch_2_1565 : id=[%d], issue=[%d], bApply=[%s], descr=[%s]\r\n", 
            patchId, issueNumber, state ? "TRUE" : "FALSE", descr);
	m_pEndDeviceList->DeleteEndDeviceAll();
    usleep(500000);

    /*-- Issue #2033 : TimeSync Type이 추가되었다. Stackup이 되었을 때
     *                 각 TimeSycn Strategy 및 Op Mode에 따라 다른 Type의
     *                 TimeSync Message를 보낸다.
     *   + sleepy mesh : 항상 Short
     *   + hybrid mesh : 항상 Long
     *   + hybrid + sleepy mesh : Normal (Long), Test (Short)
     */
    switch (m_nTimeSyncStrategy) {
        case TIMESYNC_STRATEGY_SLEEPY_MESH:
            nTimeSyncType = TIMESYNC_TYPE_SHORT;
            break;
        case TIMESYNC_STRATEGY_HIBRID_SLEEPY_MESH:
            if(m_nOpMode == OP_MODE_TEST) nTimeSyncType = TIMESYNC_TYPE_SHORT;
            break;

    }

	m_pSensorTimesync->Timesync(nTimeSyncType);

    m_oPatchInformation[idx].patchState = FALSE;
}
#endif

/** Patch-4-1647 Handler.
 *  Coordinator의 통신방법을 Direct Connection으로 On/Off 한다.
 *  이때 Direct Communication을 위해서는 반드시 Discovery Option을 Enable 시켜야 한다.
 *  따라서 On/Off 상태와 Discovery 상태를 연계하도록 Handler를 추가한다.
 *  이때 Scanning Strategy가 Immediately 라면 항상 Discovery On 이 된다.
 */
#if     defined(__PATCH_4_1647__)
extern int m_nScanningStrategy;
void Patch_4_1647_Handler(int idx, int patchId, int issueNumber, BOOL state, char * descr)
{
    XDEBUG("Patch_4_1647 : id=[%d], issue=[%d], bApply=[%s], descr=[%s], scan=[%s]\r\n", 
            patchId, issueNumber, state ? "TRUE" : "FALSE", descr, 
            m_nScanningStrategy == SCANNING_STRATEGY_IMMEDIATELY ? "Immediategy" : "Lazy");
    RouteDiscoveryControl(state | (m_nScanningStrategy == SCANNING_STRATEGY_IMMEDIATELY));
}
#endif

/** Patch-6-2084 Handler.
 *  CLI Hidden Command를 볼 수 있게 한다.
 *  On이 될 경우 /app/conf/cli_hidden_cmd_disable 을 생성하고
 *  Off가 될 경우 /app/conf/cli_hidden_cmd_disable 을 삭제한다.
 */
#if     defined(__PATCH_6_2084__)
void Patch_6_2084_Handler(int idx, int patchId, int issueNumber, BOOL state, char * descr)
{
    XDEBUG("Patch_6_2084 : id=[%d], issue=[%d], bApply=[%s], descr=[%s]\r\n", 
            patchId, issueNumber, state ? "TRUE" : "FALSE", descr); 
    if(state) {
        FILE * fp;
        if((fp=fopen("/app/conf/cli_hidden_cmd_disable","w"))) {
            fclose(fp);
        }
    } else {
        remove("/app/conf/cli_hidden_cmd_disable");
    }
}
#endif

/** Patch-9-2567 Handler.
 *  Link 절차 없이 통신하도록 설정 한다.
 *  On이 될 경우 Link 절차 없이 Bind -> Data 가 수행되고
 *  Off일 경우 Link 절차가 추가되어 Bind -> Link -> Data의 순서가 된다.
 *  기본값으로 Off를 가진다.
 */
#if     defined(__PATCH_9_2567__)
void Patch_9_2567_Handler(int idx, int patchId, int issueNumber, BOOL state, char * descr)
{
    int nError;
    int nOption = 0;

    XDEBUG("Patch_9_2567 : id=[%d], issue=[%d], bApply=[%s], descr=[%s]\r\n", 
            patchId, issueNumber, state ? "TRUE" : "FALSE", descr); 

    nError = codiGetOption(GetCoordinator(), &nOption);
    if(nError == CODIERR_NOERROR)
    {
        if(state)
        {
            nOption &= (CODI_OPTION_LINK ^ 0xFFFFFFFF);
        }
        else
        {
            nOption |= CODI_OPTION_LINK;
        }
        nError = codiSetOption(GetCoordinator(), nOption); 
        if(nError == CODIERR_NOERROR)
        {
            XDEBUG("Coordinator Option Setting : 0x%04X\r\n", nOption);
        }
        else
        {
            XDEBUG("Coordinator Set Option Fail : %s\r\n", codiErrorMessage(nError));
        }
    }
    else
    {
        XDEBUG("Coordinator Get Option Fail : %s\r\n", codiErrorMessage(nError));
    }
}
#endif


#if     defined(__PATCH_11_65__)
/** Nuri Telecom Energy level 값을 제주 실증단지 DR level로 변경한다.
  */
BYTE Patch_11_65_Nuri2Jeju(BYTE energyLevel)
{
    switch(energyLevel){
        case 0x01:
            return 0x01;
        case 0x02: case 0x03:
            return 0x02;
        case 0x04: case 0x05:
            return 0x03;
        case 0x06: case 0x07: case 0x08: case 0x09: case 0x0A: 
        case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
            return 0x06;
    }
    return 0x00;
}
#endif
