#include <sys/types.h>
#include <dirent.h>
#include <math.h>

#include "common.h"
#include "logdef.h"
#include "cmd_show.h"
#include "cmd_message.h"
#include "cmd_util.h"
#include "Variable.h"
#include "CLIInterface.h"
#include "CLIUtility.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "Chunk.h"
#include "if4frame.h"
#include "version.h"
#include "codiapi.h"
#include "cmd_syntax.h"
#include "cmd_define.h"

#include "mbusFunctions.h"

//  [10/20/2010 DHKim]
#include "../agent/TransactionManager.h"

#define  DISPLAY_ROW_COUNT  50

extern UINT m_nLocalAgentPort;
extern char m_szDumpFile[];

typedef struct _treeView {
    WORD                id;
    void                * item;
    BOOL                bInvalidPath;
    struct _treeView    * child;
    struct _treeView    * sibling;
} TreeView;

#ifdef _WIN32
#pragma pack(push, 1)
#endif	/* _WIN32 */

typedef struct  _tagAMR_INFO_PART1 
{
    BYTE                fixed_reset;
    BYTE                test_flag;
} __ATTRIBUTE_PACKED__ AMR_INFO_PART1;

typedef struct  _tagAMR_INFO_PART2 
{
    BYTE                no_join_cnt;
    BYTE                permit_mode;
    BYTE                permit_state;
    BYTE                metering_fail_cnt;
    WORD                alarm_mask;
    BYTE                network_type;
} __ATTRIBUTE_PACKED__ AMR_INFO_PART2;

typedef struct  
{
    UINT                rms_current;
    UINT                rms_voltage;
    UINT                active_power;
    UINT                reactive_power;
    UINT                power_factor;
} __ATTRIBUTE_PACKED__  PQ;

#ifdef _WIN32
#pragma pack(pop)
#endif	/* _WIN32 */

#define OTA_TYPE_CONCENTRATOR			0
#define OTA_TYPE_SENSOR					1
#define OTA_TYPE_COORDINATOR			2

#define OTA_TRANSFER_AUTO				0
#define OTA_TRANSFER_MULTICAST			1
#define OTA_TRANSFER_UNICAST			2

#define OTA_INSTALL_AUTO				0
#define OTA_INSTALL_REINSTALL			1
#define OTA_INSTALL_MATCH				2

#define OTA_STEP_INVENTORY				0x01
#define OTA_STEP_SEND					0x02
#define OTA_STEP_VERIFY					0x04
#define OTA_STEP_INSTALL				0x08
#define OTA_STEP_SCAN					0x10
#define OTA_STEP_ALL			   		(OTA_STEP_INVENTORY | OTA_STEP_SEND | OTA_STEP_VERIFY | \
										OTA_STEP_INSTALL | OTA_STEP_SCAN)

#define OTAERR_NOERROR					0				// 정상
#define OTAERR_OPEN_ERROR				1				// 센서 OPEN 오류
#define OTAERR_CONNECT_ERROR			2				// 접속 오류
#define OTAERR_INVENTORY_SCAN_ERROR		3				// Inventory 수집 오류
#define OTAERR_FIRMWARE_NOT_FOUND		4				// Firmware가 존재하지 않음
#define OTAERR_SEND_ERROR				5				// 전송 오류
#define OTAERR_VERIFY_ERROR				6				// Verify 오류
#define OTAERR_INSTALL_ERROR			7				// Install 오류
#define OTAERR_USER_CANCEL				8				// 사용자가 취소한 경우
#define OTAERR_DIFFERENT_MODEL_ERROR	9				// 모델명이 다른 경우
#define OTAERR_MATCH_VERSION_ERROR		10				// 같은 버젼인 경우
#define OTAERR_VERSION_ERROR			11				// 펌웨어 버젼 에러
#define OTAERR_MODEL_ERROR				12				// 센서 모델 에러
#define OTAERR_MEMORY_ERROR				30				// 메모리 에러
#define OTAERR_COORDINATOR_ERROR		40				// 코디네이터 에러

const char *GetStepTypeString(UINT nStep);
/////////////////////////////////////////////////////////////////////////////
// System Function
/////////////////////////////////////////////////////////////////////////////

void GetUpTimeString(char *pszString, int nTime)
{
	int		nDay, nHour, nMin, nSec;

	nDay   = nTime / (24*60*60);
	nTime -= (nDay * (24*60*60));
	nHour  = nTime / (60*60);
	nTime -= (nHour * (60*60));
	nMin   = nTime / 60;
	nTime -= (nMin * 60);
	nSec   = nTime % 60;

	if (nDay > 0)
		 sprintf(pszString, "%0d Day %0d Hour %0d Min %0d Sec", nDay, nHour, nMin, nSec);
	else if (nHour > 0)
		 sprintf(pszString, "%0d Hour %0d Min %0d Sec", nHour, nMin, nSec);
	else if (nMin > 0)
		 sprintf(pszString, "%0d Min %0d Sec", nMin, nSec);
	else sprintf(pszString, "%0d Sec", nSec);
}

void GetTimeStringByMin(char *pszString, int nMinTime)
{
	int		nDay, nHour, nMin;
    char    buff[64];

	nDay   = nMinTime / (24*60);
	nMinTime -= (nDay * (24*60));
	nHour  = nMinTime / 60;
	nMinTime -= (nHour * 60);
	nMin   = nMinTime;

	if (nDay > 0) {
		sprintf(buff, "%0dd ", nDay);
        strcat(pszString, buff);
    }
	if (nHour > 0) {
		sprintf(buff, "%0dh ", nHour);
        strcat(pszString, buff);
    }

	if (nMin > 0) {
	    sprintf(buff, "%0dm ", nMin);
        strcat(pszString, buff);
    }
}

void GetTimeStringBySec(char *pszString, int nSecTime)
{
	int		nSec;
    char    buff[64];

    GetTimeStringByMin(pszString, (int)(nSecTime / 60));
	nSec   = nSecTime % 60;

	if (nSec > 0) {
	    sprintf(buff, "%0ds ", nSec);
        strcat(pszString, buff);
    }else if(!nSecTime) {
        sprintf(pszString, "0s");
    }
}

/////////////////////////////////////////////////////////////////////////////
// System Function
/////////////////////////////////////////////////////////////////////////////

int cmdShowSystem(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    CIF4Invoke  invoke1("127.0.0.1", m_nLocalAgentPort);
	MIBValue	*pValue;
	TIMESTAMP	*pTime;
	char		szAddress[32], szLocal[32];
	char		szTime[60], temp[64], szState[32];
	char		szMobile[32];
	double		fCur, fMin, fMax, fHwVer, fSwVer;
	int			nType;
	BYTE		nState, nRstReason, *addr, nServerSecurity = 0;
    UINT        powerType, stateMask=0;

	invoke.AddParam("2.1");
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.4"))
		return CLIERR_OK;

	invoke1.AddParam("2.102.3");
	invoke1.AddParam("2.104.3");
	if (!CLIAPI_Command(pSession, invoke1.GetHandle(), "199.1"))
		return CLIERR_OK;

    memset(szAddress, 0, sizeof(szAddress));
	pValue = invoke.ResultAtName("sysServer");
    memcpy(szAddress, pValue->stream.p, pValue->len);

	nType  = invoke.ResultAtName("sysType")->stream.u8;
	pTime  = (TIMESTAMP *)invoke.ResultAtName("sysTime")->stream.p;

	fCur   = (double)invoke.ResultAtName("sysCurTemp")->stream.s32 / (double)10;
	fMin   = (double)invoke.ResultAtName("sysMinTemp")->stream.s32 / (double)10;
	fMax   = (double)invoke.ResultAtName("sysMaxTemp")->stream.s32 / (double)10;
	nState = invoke.ResultAtName("sysState")->stream.u8;
	stateMask = invoke.ResultAtName("sysStateMask")->stream.u32;
	nRstReason = invoke.ResultAtName("sysResetReason")->stream.u8;
	fHwVer = (invoke.ResultAtName("sysHwVersion")->stream.u8 >> 4)
			 + (invoke.ResultAtName("sysHwVersion")->stream.u8 & 0x0f) * 0.1;
	fSwVer = (invoke.ResultAtName("sysSwVersion")->stream.u8 >> 4)
			 + (invoke.ResultAtName("sysSwVersion")->stream.u8 & 0x0f) * 0.1;

	GetUpTimeString(szTime, invoke.ResultAtName("sysUpTime")->stream.u32);

	OUTTEXT(pSession, "       NAME : %s\xd\xa", invoke.ResultAtName("sysName")->stream.p);
	OUTTEXT(pSession, "DESCRIPTION : %s\xd\xa", invoke.ResultAtName("sysDescr")->stream.p);
	OUTTEXT(pSession, "   LOCATION : %s\xd\xa", invoke.ResultAtName("sysLocation")->stream.p);
	OUTTEXT(pSession, "    CONTACT : %s\xd\xa", invoke.ResultAtName("sysContact")->stream.p);
	OUTTEXT(pSession, "         ID : %-10d                  TYPE : ",
						invoke.ResultAtName("sysID")->stream.u32);
    switch(nType) {
        case 0: OUTTEXT(pSession, "%s\xd\xa", "URBAN(M)" ); break;
        case 1: OUTTEXT(pSession, "%s\xd\xa", "RURAL" ); break;
        case 2: OUTTEXT(pSession, "%s\xd\xa", "URBAN(S)" ); break;
        case 3: OUTTEXT(pSession, "%s\xd\xa", "INDOOR" ); break;
        case 4: 
        default:
                OUTTEXT(pSession, "%s\xd\xa", "OUTDOOR" ); break;
    }
	OUTTEXT(pSession, "H/W VERSION : %2.1f                  S/W VERSION : %.1f(%s)\xd\xa", 
                        fHwVer, fSwVer, invoke.ResultAtName("sysSwRevision")->stream.p);
#if 0
	OUTTEXT(pSession, "      STATE : %s(%0d)           RESET REASON : %s\xd\xa", 
												nState == 1 ? "Normal" : "Abnormal", nState, 
												MSGAPI_GetResetReason(nRstReason));
#else
	MSGAPI_SysStateMask(stateMask, szState);
	sprintf(temp, "%s%s%s%s",  nState == 1 ? "Normal" : "Abnormal", 
            nState ? "" : "(", szState, nState ? "" : ")");
	OUTTEXT(pSession, "      STATE : %-22s   OP MODE : %s\xd\xa", 
            temp, MSGAPI_GetOpMode(invoke.ResultAtName("sysOpMode")->stream.u8)); 
#endif
	sprintf(temp, "%s(%0d)",
						MSGAPI_GetEtherType(invoke.ResultAtName("sysEtherType")->stream.u8),
						invoke.ResultAtName("sysEtherType")->stream.u8);
	OUTTEXT(pSession, "   ETHERNET : %-25s    APN : %s\xd\xa", 
						temp, invoke.ResultAtName("sysMobileAccessPointName")->stream.p);
	sprintf(szMobile, "%s(%0d)",
					  	MSGAPI_GetMobileType(invoke.ResultAtName("sysMobileType")->stream.u8),
					  	invoke.ResultAtName("sysMobileType")->stream.u8);
	OUTTEXT(pSession, "MOBILE TYPE : %-25s   MODE : %s(%0d)\xd\xa",
						szMobile,
						MSGAPI_GetMobileMode(invoke.ResultAtName("sysMobileMode")->stream.u8),
						invoke.ResultAtName("sysMobileMode")->stream.u8);
	OUTTEXT(pSession, "     NUMBER : %-25s    CSQ : %0d dBm\xd\xa",
						invoke.ResultAtName("sysPhoneNumber")->stream.p,
						invoke1.ResultAtName("mobileRxDbm")->stream.p);
	OUTTEXT(pSession, "     SERVER : %-20s        PORT : %0d (%0d)\xd\xa",
						szAddress,
						invoke.ResultAtName("sysServerPort")->stream.u32,
						invoke.ResultAtName("sysServerAlarmPort")->stream.u32);
	addr = (BYTE *)invoke1.GetHandle()->pResult[0]->stream.p;
	sprintf(szLocal, "%d.%d.%d.%d", addr[0] & 0xff, addr[1] & 0xff, addr[2] & 0xff, addr[3] & 0xff);
	OUTTEXT(pSession, "      LOCAL : %-20s        PORT : %0d\xd\xa",
						szLocal, invoke.ResultAtName("sysLocalPort")->stream.u32);

	sprintf(temp,"%.1f (%.1f / %.1f)", (double)invoke.ResultAtName("sysCurTemp")->stream.s32 / (double)10,
						(double)invoke.ResultAtName("sysMinTemp")->stream.s32 / (double)10,
						(double)invoke.ResultAtName("sysMaxTemp")->stream.s32 / (double)10);
    powerType = invoke.ResultAtName("sysPowerType")->stream.u32;
	OUTTEXT(pSession, "       TEMP : %-24s   POWER : %s%s%s\xd\xa", temp,
                        (!powerType || (powerType & NZC_POWER_TYPE_LINE)) ? "Line " : "",
                        (powerType & NZC_POWER_TYPE_SOLAR) ? "Solar " : "",
                        (!powerType || (powerType & NZC_POWER_TYPE_BATTERY)) ? "Battery " : "");
	OUTTEXT(pSession, "       TIME : %04d/%02d/%02d %02d:%02d:%02d     TIMEZONE : %d\xd\xa",
						pTime->year, pTime->mon, pTime->day,
						pTime->hour, pTime->min, pTime->sec,
                        (short) invoke.ResultAtName("sysTimeZone")->stream.u16 / 60);

    //  [9/17/2010 DHKim]
    nServerSecurity = invoke.ResultAtName("sysSecurityState")->stream.u8;
    switch(nServerSecurity)
    {
        case 0: OUTTEXT(pSession, "   Security : Normal Mode\xd\xa"); break;
        case 1: OUTTEXT(pSession, "   Security : SSL Security\xd\xa"); break;
        default:
                OUTTEXT(pSession, "   Security : Normal Mode\xd\xa"); break;
    }

    //////////////////////////////////////////////////////////////////////////

	OUTTEXT(pSession, "     UPTIME : %s\xd\xa", szTime);

	return CLIERR_OK;
}

int cmdShowEnvironment(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke			*pInvoke;
	VARENTRY			*pEnable;
	VARVALUEENTRY		*pValue;
	char				szTemp[128];
	int					i, tzone;
	int					interval, hour;
	char				user[11], security[80] = "";
    UINT                joinNodeCnt;
    int                 idx=0;

	invoke.AddParam("2.2");         // pEnable
	invoke.AddParam("2.3");         // pValue
	invoke.AddParam("2.11.5");      // timezone
	invoke.AddParam("2.5.1");       // interval
	invoke.AddParam("2.5.4");       // hour
	invoke.AddParam("2.5.2");       // user
	invoke.AddParam("2.5.3");       // security

	invoke.AddParam("2.1.32");  // sysJoinNodeCount

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.1"))
		return CLIERR_OK;

	pInvoke  = (IF4Invoke *)invoke.GetHandle();
	pEnable  = (VARENTRY *)pInvoke->pResult[idx]->stream.p; idx++;
	pValue   = (VARVALUEENTRY *)pInvoke->pResult[idx]->stream.p; idx++;
	tzone = pInvoke->pResult[idx]->stream.s32; idx++;
	interval = pInvoke->pResult[idx]->stream.u32; idx++;
	hour	 = pInvoke->pResult[idx]->stream.u8; idx++;
	strncpy(user, pInvoke->pResult[idx]->stream.p, 10); idx++;
	user[10] = '\0';

	for(i=0; i<20; i++)
	{
		if (i == 0) 
			 sprintf(szTemp, "%02X", pInvoke->pResult[idx]->stream.p[i] & 0xff);
		else sprintf(szTemp, " %02X", pInvoke->pResult[idx]->stream.p[i] & 0xff);
		strcat(security, szTemp);
	}
    idx++;
	
	joinNodeCnt  = pInvoke->pResult[idx]->stream.u32; idx++;

	OUTTEXT(pSession, "    Auto register : %-10s       Sensor limit : %s%d%s\xd\xa",
									pEnable->varEanbleAutoRegister ? "Enable" : "Disable",
                                    pValue->varSensorLimit < 0 ? "Unlimited(" : "",
                                    pValue->varSensorLimit,
                                    pValue->varSensorLimit < 0 ? ")" : "");
	OUTTEXT(pSession, "       Auto reset : %-10s         Reset time : %d day %02d:%02d\xd\xa",
									pEnable->varEnableAutoReset ? "Enable" : "Disable",
									pValue->varAutoResetCheckInterval,
									pValue->varAutoResetTime.hour,
									pValue->varAutoResetTime.min);
    OUTTEXT(pSession, "         Scanning : %-12s        Join node : %d\xd\xa",
                                     MSGAPI_ScanningStrategy((int)pValue->varScanningStrategy), joinNodeCnt);
/** GPS와 timezone 정보는 뺀다
	OUTTEXT(pSession, "     GPS timesync : %-10s           Timezone : GMT%s%d\xd\xa",
									pEnable->varEnableGpsTimesync ? "Enable" : "Disable",
									tzone < 0 ? "-" : "+",
									tzone);
*/
    OUTTEXT(pSession, "         Topology : %-18s \xd\xa", MSGAPI_TimeSyncStrategy(pValue->varTimeSyncStrategy));
	OUTTEXT(pSession, "  \xd\xa");

    switch(pValue->varSchedulerType)
    {
        case SCHEDULER_TYPE_INTERVAL:
	        OUTTEXT(pSession, "   Scheduler Type : %-18s   Interval : %d min, Retry %d time(s)\xd\xa",
                                            "Interval", 
                                            pValue->varMeterReadingInterval,
                                            pValue->varMeteringRetry);
	        OUTTEXT(pSession, "         Day mask : 0x%08X          Hour mask : 0x%08X\xd\xa",
									        pValue->varMeterDayMask, pValue->varMeterHourMask);
            break;
        case SCHEDULER_TYPE_MASK:
        default:
	        OUTTEXT(pSession, "   Scheduler Type : %-18s Start time : xx:%02d for %0d min, Retry %d time(s)\xd\xa",
									        "Mask", pValue->varMeterStartMin,
									        pValue->varMeteringPeriod,
									        pValue->varMeteringRetry);
	        OUTTEXT(pSession, "         Day mask : 0x%08X          Hour mask : 0x%08X\xd\xa",
									        pValue->varMeterDayMask, pValue->varMeterHourMask);
	        OUTTEXT(pSession, "         Recovery : %-10s         Start time : xx:%02d for %0d min, Retry %d time(s)\xd\xa",
									        pEnable->varEnableRecovery ? "Enable" : "Disable",
									        pValue->varRecoveryStartMin,
									        pValue->varRecoveryPeriod,
									        pValue->varRecoveryRetry);
	        OUTTEXT(pSession, "         Day mask : 0x%08X          Hour mask : 0x%08X\xd\xa",
									        pValue->varRecoveryDayMask,
									        pValue->varRecoveryHourMask);
	        OUTTEXT(pSession, "       Event read : %-10s\xd\xa", "Always");
	        OUTTEXT(pSession, "         Day mask : 0x%08X          Hour mask : 0x%08X\xd\xa",
									        pValue->varEventReadDayMask,
									        pValue->varEventReadHourMask);
            break;
    }

	if (pValue->varMeterUploadCycleType == 4)
	{
	    OUTTEXT(pSession, "        Uploading : %-10s        Upload time : xx:%02d for %d min\xd\xa",
									pEnable->varEnableAutoUpload ? "Enable" : "Disable",
									pValue->varMeterUploadStartMin,
									pValue->varUploadTryTime);
	}
	else
	{
		OUTTEXT(pSession, "        Uploading : %-10s        Upload time : %02d:%02d for %d min\xd\xa",
									pEnable->varEnableAutoUpload ? "Enable" : "Disable",
									pValue->varMeterUploadStartHour,
									pValue->varMeterUploadStartMin,
									pValue->varUploadTryTime);
	}
	OUTTEXT(pSession, "      Upload type : %-11s      Upload cycle : %s\xd\xa",
									MSGAPI_GetUploadCycleType(pValue->varMeterUploadCycleType),
									MSGAPI_GetUploadCycle(szTemp, 
											pValue->varMeterUploadCycleType, 
											pValue->varMeterUploadCycle));
	OUTTEXT(pSession, "       Meter user : %-10s \xd\xa", user);
	OUTTEXT(pSession, "   Meter Seciruty : '%s'\xd\xa", security);
	OUTTEXT(pSession, "  Metering Thread : %d                    Save day : %d Day(s)\xd\xa",
									pValue->varSysMeteringThreadCount,
									pValue->varDataSaveDay);

	OUTTEXT(pSession, "       Meter ping : %-10s         Start time : xx:%02d for %0d min, Retry %d time(s)\xd\xa",
									pEnable->varEnableMeterCheck ? "Enable" : "Disable",
									pValue->varCheckStartMin,
									pValue->varCheckPeriod,
									pValue->varCheckRetry);
	OUTTEXT(pSession, "         Day mask : 0x%08X          Hour mask : 0x%08X\xd\xa",
									pValue->varCheckDayMask,
									pValue->varCheckHourMask);
    /** Issue #446 :
      * Timesync 조건을 강화한다.
      * Threshold 의 Low/High 값을 지정할 수 있고 Low 보다 크고 High 보다 작아야 Meter Timesync를 수행한다.
      * 이 때 둘 중 하나의 값이 지정되지 않을 경우 지정된 값만 검사한다. 
      */
	memset(szTemp,0,sizeof(szTemp));
    if(pValue->varTimesyncThresholdLow > 0)
    {
	    memset(szTemp,0,sizeof(szTemp));GetTimeStringBySec(szTemp, pValue->varTimesyncThresholdLow);
    }
	OUTTEXT(pSession, "   Meter timesync : %-10s          Threshold : %s / ",
									pEnable->varEnableMeterTimesync ? "Enable" : "Disable", szTemp);
	memset(szTemp,0,sizeof(szTemp));
    if(pValue->varTimesyncThresholdHigh > 0)
    {
	    GetTimeStringBySec(szTemp, pValue->varTimesyncThresholdHigh);
    }
    OUTTEXT(pSession, "%s\r\n", szTemp);

	OUTTEXT(pSession, "         Day mask : 0x%08X          Hour mask : 0x%08X\xd\xa",
									pValue->varMeterTimesyncDayMask,
									pValue->varMeterTimesyncHourMask);
	OUTTEXT(pSession, "    Fail Recovery : %-9s     Metering Option : %s\xd\xa",
									pEnable->varEnableMeterRecovery ? "Enable" : "Disable",
                                    MSGAPI_GetMeteringType(pValue->varDefaultMeteringOption, szTemp));
	OUTTEXT(pSession, "\xd\xa");
	OUTTEXT(pSession, "         Cleaning : %-10s          Threshold : %d Day(s)\xd\xa",
									pEnable->varEnableGarbageCleaning ? "Enable" : "Disable",
									pValue->varSensorCleaningThreshold);
	OUTTEXT(pSession, "      Transaction : %-10s          Threshold : %d Day(s)\xd\xa",
									"Enable",
                                    pValue->varTransactionSaveDay);
	OUTTEXT(pSession, "     Memory check : %-10s      Critical rate : %d %%\xd\xa",
									pEnable->varEnableMemoryCheck ? "Enable" : "Disable",
									pValue->varMemoryCriticalRate);
	OUTTEXT(pSession, "      Flash check : %-10s      Critical rate : %d %%\xd\xa",
									pEnable->varEnableFlashCheck ? "Enable" : "Disable",
									pValue->varFlashCriticalRate);
	memset(szTemp,0,sizeof(szTemp));GetTimeStringByMin(szTemp, pValue->varKeepaliveInterval);
	OUTTEXT(pSession, "  Keepalive event : %-10s           Interval : %s\xd\xa",
									pEnable->varEnableKeepalive ? "Enable" : "Disable",
									szTemp);
	memset(szTemp,0,sizeof(szTemp));GetTimeStringByMin(szTemp, pValue->varMobileLiveCheckInterval);
	OUTTEXT(pSession, "Mobile link check : %-15s      Interval : %s\xd\xa",
									MSGAPI_PingMethod(pValue->varMobileLiveCheckMethod), szTemp);
	memset(szTemp,0,sizeof(szTemp));GetTimeStringByMin(szTemp, (int)(pValue->varAutoTimeSyncInterval / 60));
	OUTTEXT(pSession, "    NAZC Timesync : %s \xd\xa",szTemp);
	OUTTEXT(pSession, "  Sensor Timesync : %-10s \xd\xa",
									pEnable->varEnableTimeBroadcast ? "Enable" : "Disable");

	OUTTEXT(pSession, "        Debugging : %-10s               Type : %s%c%s%c\xd\xa",
									pEnable->varEnableMonitoring ? "Enable" : "Disable" , 
									pEnable->varEnableDebugLog ? "Monitoring Server" : "Console Dump",
                                    pEnable->varEnableDebugLog ? '(' : ' ',
                                    pEnable->varEnableDebugLog ? m_szDumpFile : "",
                                    pEnable->varEnableDebugLog ? ')' : ' ');
	OUTTEXT(pSession, "Status monitoring : %-10s           Interval : hourly at xx:%02d\xd\xa",
									pEnable->varEnableStatusMonitoring ? "Enable" : "Disable",
									pValue->varStatusMonitorTime);
	memset(szTemp,0,sizeof(szTemp));GetTimeStringBySec(szTemp, pValue->varEventSendDelay);
	OUTTEXT(pSession, "      Event level : %-10s              Delay : %s\xd\xa",
									MSGAPI_GetEventLevelString(pValue->varEventAlertLevel), szTemp);
	OUTTEXT(pSession, "\xd\xa");
	OUTTEXT(pSession, "        Event log : Always     %3d day save, %dk byte(s)/day limit\xd\xa",
									pValue->varEventLogSaveDay,
									pValue->varMaxEventLogSize);
#if 0
    // Issue #838 : Agent에서 Alarm Log를 기록하지 않고 있다. 따라서 관련 기능을 제거한다
	OUTTEXT(pSession, "        Alarm log : Always     %3d day save, %dk byte(s)/day limit\xd\xa",
									pValue->varAlarmLogSaveDay, 
									pValue->varMaxAlarmLogSize);
#endif
	OUTTEXT(pSession, "     Metering log : Always     %3d day save, %dk byte(s)/day limit\xd\xa",
									pValue->varMeterLogSaveDay, 
									pValue->varMaxMeterLogSize);
	OUTTEXT(pSession, "       Upload log : Always     %3d day save, %dk byte(s)/day limit\xd\xa",
									pValue->varUploadLogSaveDay, 
									pValue->varMaxUploadLogSize);
	OUTTEXT(pSession, "     Timesync log : Always     %3d day save, %dk byte(s)/day limit\xd\xa",
									pValue->varTimesyncLogSaveDay, 
									pValue->varMaxTimesyncLogSize);
	OUTTEXT(pSession, "      Command log : %-10s %3d day save, %dk byte(s)/day limit\xd\xa",
									pEnable->varEnableCmdHistLog ? "Enable" : "Disable",
									pValue->varCmdHistSaveDay, 
									pValue->varMaxCmdLogSize);
	OUTTEXT(pSession, "       Mobile log : %-10s %3d day save, %dk byte(s)/day limit\xd\xa",
									pEnable->varEnableMobileStaticLog ? "Enable" : "Disable",
									pValue->varMobileLogSaveDay, 
									pValue->varMaxMobileLogSize);
    /* Communication Log는 더이상 지원하지 않는다
	OUTTEXT(pSession, "Communication log : %-10s %3d day save, %dk byte(s)/day limit\xd\xa",
									pEnable->varEnableCommLog ? "Enable" : "Disable",
									pValue->varCommLogSaveDay, 
									pValue->varMaxCmdLogSize);
    */
	OUTTEXT(pSession, "      Upgrade log : Always     %3d day save, %dk byte(s)/day limit\xd\xa",
									pValue->varUpgradeLogSaveDay, 
									pValue->varMaxUpgradeLogSize);
    //  Do Insert  Debug log add
    /** 아직 구현되지 않았다
   	OUTTEXT(pSession, "        Debug log : %-10s %3d day save, %dk byte(s)/day limit\xd\xa",
									pEnable->varEnableDebugLog ? "Enable" : "Disable",
									pValue->varDebugLogSaveDay, 
									pValue->varMaxDebugLogSize);
    **/

    
	return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Codi Function
/////////////////////////////////////////////////////////////////////////////

int cmdShowCodiList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke, *pInvoke2;
	CODIENTRY	*pCoordinator;
	char		szGUID[17];
    double      fHwVer, fFwVer;
	int			i, iBuild;
    BOOL        bHighRAM = FALSE;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "101.13"))
		return CLIERR_OK;

	invoke2.AddParam("2.2.28");
	if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "199.1"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	pInvoke2 = (IF4Invoke *)invoke2.GetHandle();

    bHighRAM = pInvoke2->pResult[0]->stream.u8 ? TRUE : FALSE;

	for(i=0; i<pInvoke->nResultCount; i++)
	{
		pCoordinator = (CODIENTRY *)pInvoke->pResult[i]->stream.p;
		EUI64ToStr(&pCoordinator->codiID, szGUID);

    	fHwVer = (pCoordinator->codiHwVer >> 4)
			    + (pCoordinator->codiHwVer & 0x0f) * 0.1;
    	fFwVer = (pCoordinator->codiFwVer >> 4)
			    + (pCoordinator->codiFwVer & 0x0f) * 0.1;
    	iBuild = (pCoordinator->codiFwBuild >> 4) * 10
			    + (pCoordinator->codiFwBuild & 0x0f);

		OUTTEXT(pSession, "Coordinator #%0d Configuration\xd\xa",   pCoordinator->codiIndex+1);
		OUTTEXT(pSession, "            ID : %s\xd\xa",              szGUID); 
        /*-- Issue #2079 : swVersion이 지원될 경우 이 값을 사용하고
        *-- 지원되지 않을 경우 fw 값을 사용한다. 
        *-- CLI에서 CODI_TYPE_ZIGBEE_STACK_32X 보다 큰 값이 codiType으로 
        *-- 오면 swVersion이 사용된 것으로 판단한다.
        --*/
        if(pCoordinator->codiType > CODI_TYPE_ZIGBEE_STACK_32X) {
		    OUTTEXT(pSession, "          TYPE : Zigbee Stack %d.%d", pCoordinator->codiType >> 4, pCoordinator->codiType & 0x0F);
        } else {
		    OUTTEXT(pSession, "          TYPE : %s",                    MSGAPI_GetCodiType(pCoordinator->codiType));
        }
        if(!((pCoordinator->codiFwVer & 0x0F) % 2)) OUTTEXT(pSession, " (Super Parent)");
		OUTTEXT(pSession, "\xd\xa");
		OUTTEXT(pSession, "   H/W VERSION : %2.1f\xd\xa",           fHwVer);
		OUTTEXT(pSession, "   F/W VERSION : %2.1f (B%d)\xd\xa",     fFwVer, iBuild);
		OUTTEXT(pSession, "       CHANNEL : %2d\xd\xa",             pCoordinator->codiChannel); 
		OUTTEXT(pSession, "        PAN ID : %d\xd\xa",              pCoordinator->codiPanID);
		OUTTEXT(pSession, "      RF POWER : %d\xd\xa",              pCoordinator->codiRfPower);
		OUTTEXT(pSession, " TX POWER MODE : %s (%d)\xd\xa",         MSGAPI_GetTxPowerMode(pCoordinator->codiTxPowerMode),
                                                                        pCoordinator->codiTxPowerMode);
		OUTTEXT(pSession, "   PERMIT TIME : %d\xd\xa",              pCoordinator->codiPermit);
		OUTTEXT(pSession, "  AUTO CHANNEL : %s\xd\xa",              pCoordinator->codiAutoSetting ? "Enable" : "Disable");
		OUTTEXT(pSession, "       ENCRYPT : %s\xd\xa",              pCoordinator->codiEnableEncrypt ? "On" : "Off");

        /** Coordinator Version 2.1 Build 12 이상에서 지원되는 기능 */
        if((fFwVer == 2.1 && iBuild >= 12) || fFwVer > 2.1) {
		    OUTTEXT(pSession, "     DISCOVERY : %s\xd\xa",              pCoordinator->codiRouteDiscovery ? "On" : "Off");
		    OUTTEXT(pSession, "MULTICAST HOPS : %d\xd\xa",              pCoordinator->codiMulticastHops);
        }

        /** Issue #1965: HIGH/LOW RAM Coordinator 지원
         *
         * Coordinator v2.1 b16, v2.2 b9 이상에서만 동작한다
         */
        if((fFwVer == 2.1 && iBuild > 15) || (fFwVer == 2.2 && iBuild > 8) || fFwVer > 2.2) {
		    OUTTEXT(pSession, "     MTOR TYPE : %s\xd\xa",          bHighRAM ? "HIGH RAM" : "LOW RAM");
        }

		OUTTEXT(pSession, "    RESET KIND : %s\xd\xa",              MSGAPI_GetResetKind(pCoordinator->codiResetKind));
	}

	return CLIERR_OK;
}

int cmdShowCodiStackParam(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke;
	CODIMEMORYENTRY	*pStack;
	char		szGUID[17];
    int         i;

	invoke.AddParam("3.7");
	pInvoke = (IF4Invoke *)invoke.GetHandle();
	if (!CLIAPI_Command(pSession, pInvoke, "199.1"))
		return CLIERR_OK;

	for(i=0; i<pInvoke->nResultCount; i++)
	{
        pStack = (CODIMEMORYENTRY *) pInvoke->pResult[i]->stream.p;

		EUI64ToStr(&pStack->codiID, szGUID);
        OUTTEXT(pSession, "Coordinator #%0d Stack Param (%s)\xd\xa",   pStack->codiIndex+1, szGUID);

		OUTTEXT(pSession, "  ADDRESS TABLE : %2d      APP ADDRESS TABLE : %2d\xd\xa", 
                pStack->codiWholeAddressTableSize, pStack->codiAddressTableSize);
		OUTTEXT(pSession, "    ROUTE TABLE : %2d     SOURCE ROUTE TABLE : %2d\xd\xa", 
                pStack->codiRouteTableSize, pStack->codiSourceRouteTableSize);
		OUTTEXT(pSession, " NEIGHBOR TABLE : %2d    PACKET BUFFER COUNT : %2d\xd\xa", 
                pStack->codiNeighborTableSize, pStack->codiPacketBufferCount);
		OUTTEXT(pSession, "        MAX HOP : %2d       SOFTWARE VERSION : %4X\xd\xa", 
                pStack->codiMaxHops, pStack->codiSoftwareVersion);
		OUTTEXT(pSession, "      KEY TABLE : %2d           MAX CHILDREN : %2d\xd\xa", 
                pStack->codiKeyTableSize, pStack->codiMaxChildren);
	}
	return CLIERR_OK;
}

int cmdShowCodiRegister(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke;
	CODIREGENTRY *pReg;
    int         i;
	char		szGUID[17]={0,};
    EUI64       invalidId;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	if (!CLIAPI_Command(pSession, pInvoke, "101.25"))
		return CLIERR_OK;

    memset(&invalidId, 0, sizeof(EUI64));
    OUTTEXT(pSession, "========================================================================================\xd\xa");
    OUTTEXT(pSession, " IDX SENSOR ID        STATE  | IDX SENSOR ID        STATE  | IDX SENSOR ID        STATE        \xd\xa");
    OUTTEXT(pSession, "========================================================================================\xd\xa");

	for(i=0; i<pInvoke->nResultCount; i++)
	{
        pReg = (CODIREGENTRY *) pInvoke->pResult[i]->stream.p;

        if(i && !(i % 3)) OUTTEXT(pSession, "\r\n");

        if(memcmp(&invalidId, &pReg->codiRegID, sizeof(EUI64)))
        {
		    EUI64ToStr(&pReg->codiRegID, szGUID);
        }
        else
        {
            memset(szGUID, ' ', 16);
        }
		OUTTEXT(pSession, " %3d %s %-6s  ", pReg->codiRegIndex + 1, szGUID, MSGAPI_GetEmberTableStatus(pReg->codiRegStatus)); 
	}
    OUTTEXT(pSession, "\r\n========================================================================================\xd\xa");
	return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Sensor Function
/////////////////////////////////////////////////////////////////////////////

static int _cmpsensor(const void *p1, const void *p2)
{
	SENSORINFOENTRYNEW	*m1, *m2;
    int res;

    m1 = (SENSORINFOENTRYNEW *)(*(MIBValue * const *) p1)->stream.p;
    m2 = (SENSORINFOENTRYNEW *)(*(MIBValue * const *) p2)->stream.p;
	
    res = m1->sensorAttr - m2->sensorAttr;
    if(res) return res;
    return memcmp(&m1->sensorID, &m2->sensorID, sizeof(EUI64));  
}

static int _cmpmetering(const void *p1, const void *p2)
{
	METERLPENTRY	*m1, *m2;
    int res;

    m1 = (METERLPENTRY *)(*(MIBValue * const *) p1)->stream.p;
    m2 = (METERLPENTRY *)(*(MIBValue * const *) p2)->stream.p;

    res = memcmp(&m1->mlpId, &m2->mlpId, sizeof(EUI64));  
    if(res) return res;
    return memcmp(&m1->mlpTime, &m2->mlpTime, sizeof(TIMESTAMP));
}

int cmdShowMeterList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    IF4Invoke       *pInvoke;
    METERENTRYMBUS  *pSensor;
    char            szSerial[22+1], szManu[22+1];
    char            szGUID[MAX_GUID_STRING+1];
    char            szConnectTime[30];
    char            parser[256];
    char            *idxPtr;
    int             i, nCount, ret;
    BOOL            bMore = TRUE;

    if(argc>0) {
        if(strcasecmp(argv[0],"all")) {
            strcpy(parser, argv[0]);
            if((idxPtr=strchr(parser,'$'))) {
                *idxPtr = 0x00;
            }
            invoke.AddParam("1.11",parser);
        }
    }

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.46"))
        return CLIERR_OK;

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nCount  = pInvoke->nResultCount;
        OUTTEXT(pSession, "nCount %d\xd\xa", nCount);
    if (nCount <= 0)
    {
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
        return CLIERR_OK;
    }

    qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmpsensor);
    OUTTEXT(pSession, "===================================================================================================\xd\xa");
    OUTTEXT(pSession, " NUM SENSOR ID        PORT SERIAL   TYPE             VENDOR                 GEN ACC LAST MTR     \xd\xa");
    OUTTEXT(pSession, "===================================================================================================\xd\xa");


    for(i=0; i<nCount; i++)
    {
        pSensor = (METERENTRYMBUS *)pInvoke->pResult[i]->stream.p;
        EUI64ToStr(&pSensor->mmtrGid.gid, szGUID);

        memset(szSerial, 0, sizeof(szSerial));
        memset(szManu, 0, sizeof(szManu));

        IF4API_TIMESTAMPTOSTR(szConnectTime, &pSensor->mmtrTMMetering);
        memcpy(szSerial, pSensor->mmtrSerial, sizeof(szSerial)-1);
        memcpy(szManu, pSensor->mmtrMenufa, sizeof(szManu)-1);

        szConnectTime[strlen(szConnectTime) - 3] = 0x00;

        OUTTEXT(pSession, " %3d %-16s %4d %-8s %-16s %-22s %3d %3d %s\xd\xa",
                    i+1,
                    szGUID,
                    pSensor->mmtrGid.gport,
                    pSensor->mmtrSerial,
                    mbusGetMedium(pSensor->mmtrMedia),
                    szManu,
                    pSensor->mmtrGener,
                    pSensor->mmtrCntAccess,
                    szConnectTime + 5
                );

        if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, nCount, i+1);
            if (ret == 0) return CLIERR_OK;
            if (ret == 2) bMore = FALSE;
        }
    }
    OUTTEXT(pSession, "===================================================================================================\xd\xa");
    return CLIERR_OK;
}

int cmdShowSensorList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	SENSORINFOENTRYNEW	*pSensor;
	char			szSerial[21];
	char			szModel[19];
	char			szGUID[MAX_GUID_STRING+1];
	char			szConnectTime[30], *pszState;
    char            parser[256];
	char			hw[20], fw[20];
    char            *idxPtr;
	int				i, nCount, ret, build, scnt=0;
	BOOL			bMore = TRUE;


    if(argc>0) {
        if(strcasecmp(argv[0],"all")) {
            strcpy(parser, argv[0]);
            if((idxPtr=strchr(parser,'$'))) {
                *idxPtr = 0x00;
            }
            invoke.AddParam("1.11",parser);
        }
    }

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.26"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

    //qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmpsensor);
    qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmpmetering);
	OUTTEXT(pSession, "===================================================================================================\xd\xa");
	OUTTEXT(pSession, " NUM SENSOR ID        METER SERIAL       MODEL               H/W   F/W  BLD N LAST MTR    STATE\xd\xa");
	OUTTEXT(pSession, "===================================================================================================\xd\xa");

	for(i=0; i<nCount; i++)
	{
		pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;
		EUI64ToStr(&pSensor->sensorID, szGUID);
		IF4API_TIMESTAMPTOSTR(szConnectTime, &pSensor->sensorLastConnect);
        memset(szSerial, 0, sizeof(szSerial));
        memset(szModel, 0, sizeof(szModel));
		memcpy(szSerial, pSensor->sensorSerialNumber, sizeof(pSensor->sensorSerialNumber));
		memcpy(szModel, pSensor->sensorModel, sizeof(pSensor->sensorModel));

        szConnectTime[strlen(szConnectTime) - 3] = 0x00;

		if (pSensor->sensorState == ENDISTATE_NORMAL)
			 pszState = const_cast<char *>("NORMAL");
		else if (pSensor->sensorState == ENDISTATE_INIT)
			 pszState = const_cast<char *>("INIT");
		else if (pSensor->sensorState == ENDISTATE_CONNECTION_ERROR)
			 pszState = const_cast<char *>("CONN_ERR");
		else if (pSensor->sensorState == ENDISTATE_METER_ERROR)
			 pszState = const_cast<char *>("METER_ERR");
		else if (pSensor->sensorState == ENDISTATE_ERROR)
			 pszState = const_cast<char *>("ERROR");
		else if (pSensor->sensorState == ENDISTATE_DATA_RECEIVE)
			 pszState = const_cast<char *>("DATA_RECV");
		else pszState = const_cast<char *>("UNKNOWN");

		sprintf(hw, "%2d.%-2d", pSensor->sensorHwVersion >> 8, pSensor->sensorHwVersion & 0xff);
		sprintf(fw, "%2d.%-2d", pSensor->sensorFwVersion >> 8, pSensor->sensorFwVersion & 0xff);
        build = pSensor->sensorFwBuild;
        if(pSensor->sensorAttr==ENDDEVICE_ATTR_MBUS_ARM)
        {
            OUTTEXT(pSession, "     %-16s %-18s %-18s %-4s %-4s %3d %c %11s %s\xd\xa",
                            "",
                            "",
                            szModel,
                            hw,
                            fw,
                            build,
                            ' ',
                            "",
                            pszState);

        }
        else if(pSensor->sensorAttr==ENDDEVICE_ATTR_MBUS_SLAVE)
        {
            OUTTEXT(pSession, " %20d %-18s %-18s  %-16s %s\xd\xa",
                        build,
                        szSerial, 
                        szModel,
                        mbusGetMedium((BYTE)pSensor->sensorHwVersion),
                        szConnectTime + 5);
        }
        else
        {
            OUTTEXT(pSession, " %3d %-16s %-18s %-18s %-4s %-4s %3d %c %s %s\xd\xa",
                        scnt+1,
                        szGUID,
                        szSerial,
                        szModel,
                        hw,
                        fw,
                        build,
                        pSensor->sensorAttr & ENDDEVICE_ATTR_RFD ? 'E' : 'R',
                        szConnectTime + 5,
                        pszState);
            scnt++;


        }
		if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
		{
			ret = More(pSession, nCount, i+1);
			if (ret == 0) return CLIERR_OK;
			if (ret == 2) bMore = FALSE;
		}
	}
	OUTTEXT(pSession, "===================================================================================================\xd\xa");
	return CLIERR_OK;
}

static int _cmpsummary(const void *p1, const void *p2)
{
	SENSORINFOENTRYNEW	*s1, *s2;
    int res;

    s1 = (SENSORINFOENTRYNEW *)(*(MIBValue * const *) p1)->stream.p;
    s2 = (SENSORINFOENTRYNEW *)(*(MIBValue * const *) p2)->stream.p;

    if((res=s1->sensorAttr-s2->sensorAttr)) return res;
    if((res=strcmp(s1->sensorModel,s2->sensorModel))) return res;
    if((res=(int)(s1->sensorFwVersion-s2->sensorFwVersion))) return res;
    if((res=(int)(s1->sensorFwBuild-s2->sensorFwBuild))) return res;
    return s1->sensorOTAState-s2->sensorOTAState;
}

int cmdShowSensorSummary(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	SENSORINFOENTRYNEW	*pSensor;
	char			szModel[18+1];
	char			hw[20], fw[20];
    char            szGUID[16+1];
    char            szTime[30];
    char            parser[256];
    char            *idxPtr;
	int				i, nCount, ret;
    int             subCount = 0, sumCount = 0;
    WORD            fwVersion = 0;
    WORD            fwBuild = 0;
    BOOL            bIsFirst = TRUE;
    BOOL            bMore = TRUE;
    BOOL            bDetail = FALSE;
    BYTE            deviceAttr = 0xFF;

    if(argc>0) {
        if(strcasecmp(argv[0],"all")) {
            strcpy(parser, argv[0]);
            if((idxPtr=strchr(parser,'$'))) {
                *idxPtr = 0x00;
            }
            invoke.AddParam("1.11",parser);
        }
    }
    if(argc>1) {
        if(!strncasecmp(argv[1],"det",3)) {
            bDetail = TRUE;
        }
    }

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.26"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

    qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmpsummary);

	OUTTEXT(pSession, "========================================================================================================\xd\xa");
	OUTTEXT(pSession, " MODEL                         H/W   F/W  BUILD EUI64            OTA STATE    DATE                COUNT\xd\xa");
	OUTTEXT(pSession, "========================================================================================================\xd\xa");

    memset(szModel, 0, sizeof(szModel));

	for(i=0,subCount=0; i<nCount; i++)
	{
		pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;

        if((pSensor->sensorAttr & ENDDEVICE_ATTR_MBUS_ARM) || 
           (pSensor->sensorAttr & ENDDEVICE_ATTR_MBUS_SLAVE)) continue;

        if(bIsFirst) {
            bIsFirst = FALSE;
            strcpy(szModel,pSensor->sensorModel);
            deviceAttr = pSensor->sensorAttr;
            if(deviceAttr & ENDDEVICE_ATTR_RFD) strcat(szModel," (RFD)");
            else                                strcat(szModel," (FFD)");
            fwVersion = pSensor->sensorFwVersion;
            fwBuild = pSensor->sensorFwBuild;
		    sprintf(hw, "%2d.%-2d", pSensor->sensorHwVersion >> 8, pSensor->sensorHwVersion & 0xff);
		    sprintf(fw, "%2d.%-2d", pSensor->sensorFwVersion >> 8, pSensor->sensorFwVersion & 0xff);
	        OUTTEXT(pSession, " %-26s : %-4s %-4s %5d\xd\xa", szModel, hw, fw, pSensor->sensorFwBuild);
        }

        if(deviceAttr == pSensor->sensorAttr && !strncmp(szModel,pSensor->sensorModel,strlen(pSensor->sensorModel)) && 
                fwVersion == pSensor->sensorFwVersion && fwBuild == pSensor->sensorFwBuild) {
            subCount ++;
            if(bDetail) {
		        EUI64ToStr(&pSensor->sensorID, szGUID);
		        IF4API_TIMESTAMPTOSTR(szTime, &pSensor->sensorLastOTATime);
                OUTTEXT(pSession, "                                                %16s %-12s %-8s \xd\xa", 
                        szGUID, GetStepTypeString((UINT)pSensor->sensorOTAState), szTime);
		        if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
		        {
			        ret = More(pSession, nCount, i+1);
			        if (ret == 0) return CLIERR_OK;
			        if (ret == 2) bMore = FALSE;
		        }
            }
            continue;
        }

	    OUTTEXT(pSession, " %-26s                                                                        %4d\xd\xa", "Sub Total", subCount);
	    OUTTEXT(pSession, "--------------------------------------------------------------------------------------------------------\xd\xa");
        sumCount += subCount;

        subCount = 1;
        strcpy(szModel,pSensor->sensorModel);
        deviceAttr = pSensor->sensorAttr;
        if(deviceAttr & ENDDEVICE_ATTR_RFD) strcat(szModel," (RFD)");
        else                                strcat(szModel," (FFD)");
        fwVersion = pSensor->sensorFwVersion;
        fwBuild = pSensor->sensorFwBuild;
		sprintf(hw, "%2d.%-2d", pSensor->sensorHwVersion >> 8, pSensor->sensorHwVersion & 0xff);
		sprintf(fw, "%2d.%-2d", pSensor->sensorFwVersion >> 8, pSensor->sensorFwVersion & 0xff);
	    OUTTEXT(pSession, " %-26s : %-4s %-4s %5d\xd\xa", szModel, hw, fw, pSensor->sensorFwBuild);
        if(bDetail) {
		    EUI64ToStr(&pSensor->sensorID, szGUID);
		    IF4API_TIMESTAMPTOSTR(szTime, &pSensor->sensorLastOTATime);
            OUTTEXT(pSession, "                                                %16s %-12s %-8s \xd\xa", 
                szGUID, GetStepTypeString((UINT)pSensor->sensorOTAState), szTime);
		    if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
		    {
			    ret = More(pSession, nCount, i+1);
			    if (ret == 0) return CLIERR_OK;
			    if (ret == 2) bMore = FALSE;
		    }
        }
	}
    if(subCount) {
        sumCount += subCount;
	    OUTTEXT(pSession, " %-26s                                                                        %4d\xd\xa", "Sub Total", subCount);
	    OUTTEXT(pSession, "--------------------------------------------------------------------------------------------------------\xd\xa");
    }
	OUTTEXT(pSession, " %-26s :                                                                      %4d\xd\xa", "Total", sumCount);
	OUTTEXT(pSession, "========================================================================================================\xd\xa");
	return CLIERR_OK;
}


static int _cmppath(const void *p1, const void *p2)
{
	SENSORPATHENTRY	*s1, *s2;

    s1 = (SENSORPATHENTRY *)(*(MIBValue * const *) p1)->stream.p;
    s2 = (SENSORPATHENTRY *)(*(MIBValue * const *) p2)->stream.p;

    return (int) s1->sensorPathShortID - (int) s2->sensorPathShortID;
}

static int _findpath(const void *p1, const void *p2)
{
	SENSORPATHENTRY	*s2;

    s2 = (SENSORPATHENTRY *)(*(MIBValue * const *) p2)->stream.p;

    return (int) (*(WORD const *) p1) - (int) s2->sensorPathShortID;
}

void _displayLinearPath(CLISESSION *pSession, IF4Invoke *pInvoke, int nCount)
{
	SENSORPATHENTRY	*pSensor;
	char			szSerial[21];
	char			szModel[20];
	char			szTime[32];
	char			szShortID[12];
	char			szHops[10];
	char			szGUID[MAX_GUID_STRING+1];
	char			szBuffer[256] = "", tmp[50];
	int				i, j, ret;
	WORD			id;
	BOOL			bMore = TRUE, bFind;

    qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmppath);

	OUTTEXT(pSession, "=========================================================================================================\xd\xa");
	OUTTEXT(pSession, " NUM SENSOR ID        METER SERIAL       MODEL              TIME        SID  HOP PATH\xd\xa");
	OUTTEXT(pSession, "=========================================================================================================\xd\xa");

	for(i=0; i<nCount; i++)
	{
		pSensor = (SENSORPATHENTRY *)pInvoke->pResult[i]->stream.p;
		EUI64ToStr(&pSensor->sensorPathID, szGUID);
		IF4API_TIMESTAMPTOSTR(szTime, &pSensor->sensorPathTime);
		memcpy(szSerial, pSensor->sensorPathSerial, 18);
		szSerial[18] = '\0';
		memcpy(szModel, pSensor->sensorPathModel, 18);
		szModel[18] = '\0';
		sprintf(szShortID, "%04X", pSensor->sensorPathShortID);
		sprintf(szHops, "%d", pSensor->sensorPathHops);
        szTime[strlen(szTime)-3] = 0x00;

		*szBuffer = '\0';
		for(j=0; j<pSensor->sensorPathHops; j++)
		{
			id = pSensor->sensorPathNode[j];
            bFind = 
                (bsearch(&id, pInvoke->pResult, nCount, sizeof(MIBValue *), _findpath) == NULL) ? FALSE : TRUE;

			if (bFind)
				 sprintf(tmp, "%04X ", id);
			else sprintf(tmp, "\033[1;40;31m[%04X]\033[0m ", id);
			strcat(szBuffer, tmp);
		}
        
		OUTTEXT(pSession, " %3d %-16s %-18s %-18s %s %s %3s %s\xd\xa",
					i+1,
					szGUID,
					szSerial,
					szModel,
					szTime + 5,
					szShortID,
					szHops,
				    szBuffer);

		if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
		{
			ret = More(pSession, nCount, i+1);
			if (ret == 0) return;
			if (ret == 2) bMore = FALSE;
		}
	}
	OUTTEXT(pSession, "=========================================================================================================\xd\xa");
}

TreeView * _allocTree(TreeView * tree, WORD id, SENSORPATHENTRY * pSensor, int * nCount)
{
    TreeView * tmp = tree->child;
    TreeView * last = NULL;

    while(tmp) {
        if(tmp->id == id) {
            if(pSensor) {
                tmp->item = pSensor;
            }
            return tmp;
        }
        last = tmp;
        tmp = tmp->sibling;
    }
    
    if(!last) {
        tree->child = (TreeView *) malloc(sizeof(TreeView));
        last = tree->child;
    } else {
        last->sibling = (TreeView *) malloc(sizeof(TreeView));
        last = last->sibling;
    }

    *nCount = *nCount + 1;
    
    memset(last, 0, sizeof(TreeView));
    last->id = id;
    if(pSensor) {
        last->item = pSensor;
    }

    return last;
}

void _freeTree(TreeView * tree)
{
    if(tree->child) {
        _freeTree(tree->child);
        free(tree->child);
    }
    if(tree->sibling) {
        _freeTree(tree->sibling);
        free(tree->sibling);
    }
}

void _assignTree(TreeView * tree,  SENSORPATHENTRY * pSensor, TreeView ** orig, BOOL *bInvalidPath)
{
    if(tree->id == pSensor->sensorPathShortID) {
        if(!tree->item) {
            tree->item = pSensor;
            tree->bInvalidPath = TRUE;
            *bInvalidPath = TRUE;
            if(*orig) {
                (*orig)->bInvalidPath = TRUE;
            }
        }else {
            *orig = tree;
            if(*bInvalidPath){
                tree->bInvalidPath = TRUE;
            }
        }
    }

    if(tree->child) _assignTree(tree->child, pSensor, orig, bInvalidPath);
    if(tree->sibling) _assignTree(tree->sibling, pSensor, orig, bInvalidPath);
}

int _printTreePath(CLISESSION *pSession, TreeView * tree, int hop, int nCount, BOOL * bMore, int cnt)
{
	SENSORPATHENTRY	*pSensor;
	char			szSerial[21];
	char			szModel[20];
	char			szTime[32];
	char			szShortID[12];
	char		    szGUID[64];
    char            formatStr[64];
    int             lqi = 0, hopDepth=16;
    int             ret;

    pSensor = (SENSORPATHENTRY *) tree->item;

    if(pSensor) {
		EUI64ToStr(&pSensor->sensorPathID, szGUID);
		IF4API_TIMESTAMPTOSTR(szTime, &pSensor->sensorPathTime);
		memcpy(szSerial, pSensor->sensorPathSerial, 18);
		szSerial[18] = '\0';
        memset(szModel, 0, sizeof(szModel));
		memcpy(szModel, pSensor->sensorPathModel, 18);
		sprintf(szShortID, "%04X", pSensor->sensorPathShortID);
        szTime[strlen(szTime)-3] = 0x00;

        if((int) pSensor->sensorPathLQI < 80 ) {
            lqi = (int) pSensor->sensorPathLQI;
        }else {
            lqi = (int) (80 + (pSensor->sensorPathLQI - 80) * 0.125);
        }

        if(lqi>100) lqi = 100;

        OUTTEXT(pSession, " %2d ", hop);
        sprintf(formatStr,"%%-%ds", hop+1);
        OUTTEXT(pSession, formatStr, " ");
        if(tree->bInvalidPath) OUTTEXT(pSession, "\033[1;40;35m[");
        OUTTEXT(pSession, "%-16s", szGUID);
        if(tree->bInvalidPath) OUTTEXT(pSession, "]\033[0m");
        else OUTTEXT(pSession, "  ");
        sprintf(formatStr,"%%-%ds", hopDepth-hop);
        OUTTEXT(pSession, formatStr, " ");
        OUTTEXT(pSession, "%-18s %-20s %s %3d%% %4d %s\xd\xa",
					szSerial,
					szModel,
                    szTime + 5,
                    lqi,
                    pSensor->sensorPathRSSI,
                    szShortID);
    }else {
		sprintf(szShortID, "%04X", tree->id);

        OUTTEXT(pSession, " %2d ", hop);
        sprintf(formatStr,"%%-%ds", hop+1);
        OUTTEXT(pSession, formatStr, " ");
        OUTTEXT(pSession, "\033[1;40;31m[");
        OUTTEXT(pSession, "%-16s", "INVALID PATH");
        OUTTEXT(pSession, "]\033[0m");
        sprintf(formatStr,"%%-%ds", hopDepth-hop);
        OUTTEXT(pSession, formatStr, " ");
        OUTTEXT(pSession, "                                                              %s\xd\xa", szShortID);
    }

	if (*bMore && (((cnt+1) % DISPLAY_ROW_COUNT) == 0))
	{
		ret = More(pSession, nCount, cnt+1);
		if (ret == 0) return -1;
		if (ret == 2) *bMore = FALSE;
	}

    if(tree->child) {
        if((cnt=_printTreePath(pSession, tree->child, hop+1, nCount, bMore, cnt + 1))<0) return cnt;
     }
    if(tree->sibling) {
        if((cnt=_printTreePath(pSession, tree->sibling, hop, nCount, bMore, cnt + 1))<0) return cnt;
    }

    return cnt;
}

void _displayTreePath(CLISESSION *pSession, IF4Invoke *pInvoke, int nCount)
{
	SENSORPATHENTRY	*pSensor;
	int				i, j;
	WORD			id;
    BOOL            bInvalidPath = FALSE;
    BOOL            bMore = TRUE;
    TreeView        * tree, *tmp;
    int             nodeCount = 0;

    tree = (TreeView *) malloc(sizeof(TreeView));
    memset(tree, 0, sizeof(TreeView));

    for(i=0; i<nCount; i++) {
        tmp = tree;
		pSensor = (SENSORPATHENTRY *)pInvoke->pResult[i]->stream.p;

        if(!pSensor->sensorPathShortID) continue;

		for(j=pSensor->sensorPathHops-1; j>=0; j--)
        {
			id = pSensor->sensorPathNode[j];
            tmp = _allocTree(tmp, id, NULL, &nodeCount);
        }
        _allocTree(tmp, pSensor->sensorPathShortID, pSensor, &nodeCount);
    }

    for(i=0; i<nCount; i++) {
		pSensor = (SENSORPATHENTRY *)pInvoke->pResult[i]->stream.p;
        if(!pSensor->sensorPathShortID) continue;
        bInvalidPath = FALSE;
        tmp = NULL;

         _assignTree(tree->child, pSensor, &tmp, &bInvalidPath);
    }

	OUTTEXT(pSession, "=========================================================================================================\xd\xa");
	OUTTEXT(pSession, " HOP SENSOR ID                         METER SERIAL       MODEL                TIME         LQI RSSI SID\xd\xa");
	OUTTEXT(pSession, "=========================================================================================================\xd\xa");

    if(tree->child) _printTreePath(pSession, tree->child, 0, nodeCount, &bMore, 0);

	OUTTEXT(pSession, "=========================================================================================================\xd\xa");

    _freeTree(tree);
    free(tree);
}

int cmdShowSensorPath(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
	IF4Invoke		*pInvoke;
    int             nCount;
#define             VIEW_TYPE_LINEAR             0
#define             VIEW_TYPE_TREE              1
    int             nViewType = VIEW_TYPE_TREE;
    char            parser[256];
    char            *idxPtr;
    EUI64           id;

    if(argc>0) {
        if(check_id(NULL, argv[0])) { // EUI64 ID
  	        StrToEUI64(argv[0], &id);
            invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        }else if(strcasecmp(argv[0],"all")) {
            strcpy(parser, argv[0]);
            if((idxPtr=strchr(parser,'$'))) {
                *idxPtr = 0x00;
            }
            invoke.AddParam("1.11",parser);
        }
    }

    if(argc > 1) {
        if(!strcasecmp(argv[1], "linear")) nViewType = VIEW_TYPE_LINEAR;
        else if(!strcasecmp(argv[1], "tree")) nViewType = VIEW_TYPE_TREE;
    }

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.33"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

    //qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmppath);

    switch(nViewType) {
        case VIEW_TYPE_LINEAR:
            _displayLinearPath(pSession, pInvoke, nCount);
            break;
        case VIEW_TYPE_TREE:
        default: 
            _displayTreePath(pSession, pInvoke, nCount);
            break;
    }

	return CLIERR_OK;
}

int cmdShowSensorInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	SENSORINFOENTRYNEW	*pSensor;
	char			szGUID[MAX_GUID_STRING+1];
	char			szConnectTime[30];
	char			szInstallTime[30];
	EUI64			id;

	StrToEUI64(argv[0], &id);
    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, &id, sizeof(EUI64));
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.27"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[0]->stream.p;
	EUI64ToStr(&pSensor->sensorID, szGUID);
	IF4API_TIMESTAMPTOSTR(szConnectTime, &pSensor->sensorLastConnect);
	IF4API_TIMESTAMPTOSTR(szInstallTime, &pSensor->sensorInstallDate);

	OUTTEXT(pSession, "=====================================================================================\xd\xa");
	OUTTEXT(pSession, "     SENSOR ID : %s\xd\xa", szGUID);
	OUTTEXT(pSession, "         MODEL : %s\xd\xa", pSensor->sensorModel);
	OUTTEXT(pSession, "     METER S/N : %s\xd\xa", pSensor->sensorSerialNumber);
	OUTTEXT(pSession, "         STATE : %s\xd\xa", MSGAPI_GetSensorState(pSensor->sensorState));
    OUTTEXT(pSession, "  NETWORK TYPE : %s\xd\xa", pSensor->sensorAttr & ENDDEVICE_ATTR_RFD ? "RFD" : "FFD");
	OUTTEXT(pSession, "  INSTALL DATE : %s\xd\xa", szInstallTime);
	OUTTEXT(pSession, " LAST METERING : %s\xd\xa", szConnectTime);

	return CLIERR_OK;
}

int _showSensorRomInfo(CLISESSION *pSession, IF4Invoke *pInvoke, int nStartIdx, int nEndIdx)
{
    ENDI_NETWORK_INFO   *networkInfo;
    ENDI_NODE_INFO  *nodeInfo;
    AMR_INFO_PART1  *amrPart1;
    AMR_INFO_PART2  *amrPart2;

    char    buffer[256];

    if(nEndIdx - nStartIdx < 4) {
		return CLIERR_OK;
    }

	OUTTEXT(pSession, "=====================================================================================\xd\xa");
    
    networkInfo = (ENDI_NETWORK_INFO *)pInvoke->pResult[nStartIdx]->stream.p;
    nodeInfo = (ENDI_NODE_INFO *)pInvoke->pResult[nStartIdx + 1]->stream.p;
    amrPart1 = (AMR_INFO_PART1 *)pInvoke->pResult[nStartIdx + 2]->stream.p;
    amrPart2 = (AMR_INFO_PART2 *)pInvoke->pResult[nStartIdx + 3]->stream.p;

	OUTTEXT(pSession, "           NODE KIND : %s\xd\xa", nodeInfo->NODE_KIND);
	OUTTEXT(pSession, "    FIRMWARE VERSION : %2.1f (B%d)\xd\xa", 
        (float)((nodeInfo->FIRMWARE_VERSION >> 4) & 0x0f) + ((nodeInfo->FIRMWARE_VERSION & 0x0f) * 0.1), 
        (int)((nodeInfo->FIRMWARE_BUILD >> 4) & 0x0f) * 10 + (nodeInfo->FIRMWARE_BUILD & 0x0f)); 
	OUTTEXT(pSession, "    SOFTWARE VERSION : %2.1f\xd\xa", 
        (float)((nodeInfo->SOFTWARE_VERSION >> 4) & 0x0f) + ((nodeInfo->SOFTWARE_VERSION & 0x0f) * 0.1)); 
	OUTTEXT(pSession, "    HARDWARE VERSION : %2.1f\xd\xa", 
        (float)((nodeInfo->HARDWARE_VERSION >> 4) & 0x0f) + ((nodeInfo->HARDWARE_VERSION & 0x0f) * 0.1)); 
	OUTTEXT(pSession, "    PROTOCOL VERSION : %2.1f\xd\xa", 
        (float)((nodeInfo->PROTOCOL_VERSION >> 4) & 0x0f) + ((nodeInfo->PROTOCOL_VERSION & 0x0f) * 0.1)); 
	OUTTEXT(pSession, "        RESET REASON : %s\xd\xa", MSGAPI_GetResetKind(nodeInfo->RESET_REASON));
	OUTTEXT(pSession, "         FIXED RESET : %d\xd\xa", amrPart1->fixed_reset);
	OUTTEXT(pSession, "           TEST FLAG : %s\xd\xa", amrPart1->test_flag ? "ON" : "OFF");

	OUTTEXT(pSession, "            TX POWER : %d\xd\xa", networkInfo->TXPOWER);
	OUTTEXT(pSession, "        SUPER PARENT : %s\xd\xa", nodeInfo->SUPER_PARENT ? "SUPER PARENT NETWORK" : "STANDARD NETWORK");
	OUTTEXT(pSession, "         PERMIT MODE : %s\xd\xa", amrPart2->permit_mode ? "AUTO" : "MANUAL");
	OUTTEXT(pSession, "        PERMIT STATE : %s\xd\xa", amrPart2->permit_state ? "ENABLE" : "DISABLE");

    MSGAPI_GetAlarmMask(BigToHostShort(amrPart2->alarm_mask), buffer);
	OUTTEXT(pSession, "          ALARM MASK : 0x%04X %s\xd\xa", BigToHostShort(amrPart2->alarm_mask), buffer);
    if(amrPart2->metering_fail_cnt != 0xFF) 
	    OUTTEXT(pSession, "   METERING FAIL CNT : %d\xd\xa", amrPart2->metering_fail_cnt);
    else
	    OUTTEXT(pSession, "   METERING FAIL CNT : NOT SUPPORT\xd\xa");
	OUTTEXT(pSession, "        NETWORK TYPE : %s\xd\xa", MSGAPI_NetworkType(amrPart2->network_type));

	OUTTEXT(pSession, "       SOLAR AC VOLT : %.4f\xd\xa", (double)(BigToHostShort(nodeInfo->SOLAR_AD_VOLT) * 2) / 10000.0);
	OUTTEXT(pSession, " SOLAR CHG BATT VOLT : %.4f\xd\xa", (double)(BigToHostShort(nodeInfo->SOLAR_CHG_BATT_VOLT)) / 10000.0);
	OUTTEXT(pSession, " SOLAR BOARD DC VOLT : %.4f\xd\xa", (double)(BigToHostShort(nodeInfo->SOLAR_B_DC_VOLT)) / 10000.0);

	OUTTEXT(pSession, "=====================================================================================\xd\xa");

	return CLIERR_OK;
}

int cmdShowSensorRomInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);
#endif
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	EUI64			id;

	SENSORINFOENTRYNEW	*pSensor = NULL;

    if (!Confirm(pSession, MSG_READ_SENSOR))
		return CLIERR_OK;

	StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	    return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.23");
    } 
    /*-- sensor rom info --*/

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, &id, sizeof(EUI64));
    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_MANUAL_ENABLE);
    invoke.AddParam("1.5", (WORD) sizeof(ENDI_NETWORK_INFO)); 

    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_NODE_KIND);
    invoke.AddParam("1.5", (WORD) sizeof(ENDI_NODE_INFO)); 

    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_FIXED_RESET);
    invoke.AddParam("1.5", (WORD) sizeof(AMR_INFO_PART1)); 

    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_NO_JOIN_CNT);
    invoke.AddParam("1.5", (WORD) sizeof(AMR_INFO_PART2)); 

    if(NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.23")) {
		return CLIERR_OK;
    }

	pInvoke = (IF4Invoke *)invoke.GetHandle();
    
    return _showSensorRomInfo(pSession, pInvoke, 0, pInvoke->nResultCount);
}
    
int _showSensorEvent(CLISESSION *pSession, IF4Invoke * pInvoke, int nStartIdx, int nEndIdx)
{
    UINT            uval;
    WORD            wval;
    int             i,j,idx;
    WORD            volt, current;
    ENDI_EVENT_ENTRY * eventEntry;

	OUTTEXT(pSession, "=====================================================================================\xd\xa");
	OUTTEXT(pSession, " NUM EVENT TIME          EVENT TYPE                       STATUS\xd\xa");
	OUTTEXT(pSession, "=====================================================================================\xd\xa");

    for(i=nStartIdx,idx=1;i<nEndIdx;i++) {
        eventEntry = (ENDI_EVENT_ENTRY *) pInvoke->pResult[i]->stream.p;
        for(j=(pInvoke->pResult[i]->len / sizeof(ENDI_EVENT_ENTRY)) - 1; j>= 0; j--) {
            if(eventEntry[j].GMT_TIME.year == 0xFFFF) continue;
            memcpy(&uval,eventEntry[j].EVENT_STATUS,sizeof(UINT));
            uval = BigToHostInt(uval);
            OUTTEXT(pSession, " %3d %04d/%02d/%02d %02d:%02d:%02d (%02X)%-28s ", idx,
                            BigToHostShort(eventEntry[j].GMT_TIME.year), eventEntry[j].GMT_TIME.mon, 
                            eventEntry[j].GMT_TIME.day, eventEntry[j].GMT_TIME.hour, 
                            eventEntry[j].GMT_TIME.min, eventEntry[j].GMT_TIME.sec,
                            eventEntry[j].EVENT_TYPE,
                            MSGAPI_GetEventType(eventEntry[j].EVENT_TYPE)
            );
            switch(eventEntry[j].EVENT_TYPE) {
                case ENDI_EVENT_TYPE_RESET:
                    OUTTEXT(pSession, "%s\xa\xd", MSGAPI_GetResetKind((BYTE)uval));
                    break;
                case ENDI_EVENT_TYPE_POWER_OUTAGE:
                    OUTTEXT(pSession, "%s\xa\xd", uval == 1 ? "Outage" : ( uval == 2 ? "Restore" : "Unknown"));
                    break;
                case ENDI_EVENT_TYPE_NETWORK_JOIN:
                case ENDI_EVENT_TYPE_NETWORK_LEAVE:
                    OUTTEXT(pSession, "%04X\xa\xd", uval);
                    break;
                case ENDI_EVENT_TYPE_LP_PERIOD_CHANGE:
                    OUTTEXT(pSession, "%d\xa\xd", uval);
                    break;
                case ENDI_EVENT_TYPE_TIME_OFFSET:
                    switch(uval) {
                        case 0: OUTTEXT(pSession, "%s\xa\xd", "Previous(RF)"); break;
                        case 1: OUTTEXT(pSession, "%s\xa\xd", "Current (RF)"); break;
                        case 2: OUTTEXT(pSession, "%s\xa\xd", "Previous(Raw RF)"); break;
                        case 3: OUTTEXT(pSession, "%s\xa\xd", "Current (Raw RF)"); break;
                        case 4: OUTTEXT(pSession, "%s\xa\xd", "Previous(Optical)"); break;
                        case 5: OUTTEXT(pSession, "%s\xa\xd", "Current (Optical)"); break;
                        default: OUTTEXT(pSession, "Unknown(%d)\xa\xd", uval); break;
                    }
                    break;
                case ENDI_EVENT_TYPE_LX_MISSING:
                    OUTTEXT(pSession, "%s %02X %02X %02X\xa\xd", 
                        eventEntry[j].EVENT_STATUS[0]==1?"Outage":(eventEntry[j].EVENT_STATUS[0] ==2?"Restore":"Unknown"),
                        eventEntry[j].EVENT_STATUS[1],
                        eventEntry[j].EVENT_STATUS[2],
                        eventEntry[j].EVENT_STATUS[3]
                    );
                    break;
                case ENDI_EVENT_TYPE_LOW_BATTERY:
                    memcpy(&volt,eventEntry[j].EVENT_STATUS,sizeof(WORD));
                    memcpy(&current,eventEntry[j].EVENT_STATUS+2,sizeof(WORD));
                    OUTTEXT(pSession, "%.4f %d\xa\xd", (double) BigToHostShort(volt) / 10000, BigToHostShort(current)); 
                    break;
                case ENDI_EVENT_TYPE_BATT_DISC_EN:
                    memcpy(&volt,eventEntry[j].EVENT_STATUS+2,sizeof(WORD));
                    OUTTEXT(pSession, "%.4f\xa\xd", (double) BigToHostShort(volt) / 10000); 
                    break;
                case ENDI_EVENT_TYPE_NETWORK_TYPE:
                    OUTTEXT(pSession, "%s\xa\xd", MSGAPI_NetworkType((BYTE)uval));
                    break;
                case ENDI_EVENT_TYPE_OPTICAL_CMD:
                    OUTTEXT(pSession, "%s\xa\xd", MSGAPI_OpticalCommand((BYTE)uval));
                    break;
                case ENDI_EVENT_TYPE_STATE_ALARM:
                    OUTTEXT(pSession, "%s%s%s%s%s%s %s\xa\xd", 
                            (eventEntry[j].EVENT_STATUS[2] & 0x01) ? "Open " : "",
                            (eventEntry[j].EVENT_STATUS[2] & 0x02) ? "Tamper " : "",
                            (eventEntry[j].EVENT_STATUS[2] & 0x04) ? "Tilt " : "",
                            (eventEntry[j].EVENT_STATUS[2] & 0x08) ? "Unstable " : "",
                            (eventEntry[j].EVENT_STATUS[2] & 0x10) ? "OpenFail " : "",
                            (eventEntry[j].EVENT_STATUS[2] & 0x20) ? "ShutOffFail " : "",
                            (eventEntry[j].EVENT_STATUS[3] == 1) ? "Open" : 
                            (
                                (eventEntry[j].EVENT_STATUS[3] == 2) ? "Manual Shut-Off" : 
                                ( 
                                 (eventEntry[j].EVENT_STATUS[3] == 3) ? "Ready" : 
                                 ( 
                                  (eventEntry[j].EVENT_STATUS[3] == 4) ? "Shut-Off (Tilt)" : 
                                  (
                                   (eventEntry[j].EVENT_STATUS[3] == 5) ? "Shut-Off (Leakage)" : 
                                   ( 
                                    (eventEntry[j].EVENT_STATUS[3] == 6) ? "Shut-Off (Remote)" : "UNKNOWN"
                                   )
                                  )
                                 )
                                )
                            )
                        );
                    break;
                case ENDI_EVENT_TYPE_MSP430_STATUS:
                    OUTTEXT(pSession, "%s %s %c %s %s\xa\xd", 
                            (eventEntry[j].EVENT_STATUS[2] & 0x80) ? "Read" : "Write",
                            (eventEntry[j].EVENT_STATUS[2] & 0x40) ? "response" : "request",
                            (eventEntry[j].EVENT_STATUS[2] & 0x20) ? 'M' : 'U',
                            (eventEntry[j].EVENT_STATUS[2] & 0x0F) == 0x00 ? "INFO" :
                             ( 
                              (eventEntry[j].EVENT_STATUS[2] & 0x0F) == 0x01 ? "CMD" : 
                              (
                               (eventEntry[j].EVENT_STATUS[2] & 0x0F) == 0x02 ? "DATA" : "UNKNOWN" 
                              )
                             ),
                            MSGAPI_MSP430Status(eventEntry[j].EVENT_STATUS[3]));
                    break;
                case ENDI_EVENT_TYPE_REPEATER_STATUS:
                    OUTTEXT(pSession, "%s\xa\xd", uval ? "Seiral Error" : "Normal");
                    break;
                case ENDI_EVENT_TYPE_NET_KEY_SWITCHING:
                    OUTTEXT(pSession, "%s\xa\xd", uval ? "Switching" : "No Switching");
                    break;
                case ENDI_EVENT_TYPE_INVALID_METERING:
                    switch(uval) {
                        case 0: OUTTEXT(pSession, "None\xa\xd"); break;
                        case 1:
                        case 2:
                        case 3: OUTTEXT(pSession, "Slave %d Error\xa\xd", uval); break;
                        default: OUTTEXT(pSession, "Unknown(%d)\xa\xd", uval); break;
                    }
                    break;
                case ENDI_EVENT_TYPE_BOOTUP:
                    memcpy(&wval,eventEntry[j].EVENT_STATUS+2,sizeof(WORD));
                    OUTTEXT(pSession, "%02X %02X %d\r\n", 
                            eventEntry[j].EVENT_STATUS[0] & 0xFF,
                            eventEntry[j].EVENT_STATUS[1] & 0xFF,
                            BigToHostShort(wval));
                    break;
                case ENDI_EVENT_TYPE_EEPROM_LP_FAIL:
                    OUTTEXT(pSession, "0x%08X\xa\xd", uval);
                    break;
                case ENDI_EVENT_TYPE_CPULSE_ERROR:
                    OUTTEXT(pSession, "%u\xa\xd", uval);
                    break;
                case ENDI_EVENT_TYPE_LP_ROLLBACK:
                    OUTTEXT(pSession, "%u\xa\xd", uval);
                    break;
                case ENDI_EVENT_TYPE_METER_FW_UPGRADE_FAIL:
                    OUTTEXT(pSession, "0x%08X\xa\xd", uval);
                    break;
                case ENDI_EVENT_TYPE_SCAN_FAIL:
                case ENDI_EVENT_TYPE_CASE_OPEN:
                case ENDI_EVENT_TYPE_TAMPER:
                case ENDI_EVENT_TYPE_RESERVED_SRAM_CRACK:
                case ENDI_EVENT_TYPE_KEEP_ALIVE_FAIL:
                case ENDI_EVENT_TYPE_DETACHEMENT:
                case ENDI_EVENT_TYPE_PULSE_MO:
                case ENDI_EVENT_TYPE_WKAE_UP:
                case ENDI_EVENT_TYPE_NO_PARENT:
                case ENDI_EVENT_TYPE_PULSE_BACK:
                case ENDI_EVENT_TYPE_ATTACHMENT:
                case ENDI_EVENT_TYPE_MSP430_RESET:
                case ENDI_EVENT_TYPE_NET_JOIN_LIMITED:
                    OUTTEXT(pSession,"\xa\xd");
                    break;
                case ENDI_EVENT_TYPE_FACTORY_SETTING:
                default:
                    OUTTEXT(pSession, "%02X %02X %02X %02X\xa\xd", 
                            eventEntry[j].EVENT_STATUS[0],
                            eventEntry[j].EVENT_STATUS[1],
                            eventEntry[j].EVENT_STATUS[2],
                            eventEntry[j].EVENT_STATUS[3]);
                    break;
            }
            idx++;
        }
    }
	OUTTEXT(pSession, "=====================================================================================\xd\xa");
	return CLIERR_OK;
}

int cmdShowSensorEvent(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT + 60);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);
#endif
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	EUI64			id;
    BYTE            nCount=5;
	SENSORINFOENTRYNEW	*pSensor;

    if (!Confirm(pSession, MSG_READ_SENSOR))
		return CLIERR_OK;

	StrToEUI64(argv[0], &id);

    if(argc > 1) {
        nCount = (BYTE) strtol(argv[1],(char **)NULL,10);
        if(nCount > 250) nCount = 250;
    }

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	    return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.44");
    } 

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, &id, sizeof(EUI64));
    invoke.AddParam("1.4", nCount);

    if(NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

    /*-- sensor rom info --*/

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.44")) {
		return CLIERR_OK;
    }

	pInvoke = (IF4Invoke *)invoke.GetHandle();

    return _showSensorEvent(pSession, pInvoke, 0, pInvoke->nResultCount);
}

int _showSensorBattery(CLISESSION *pSession, IF4Invoke * pInvoke, int nStartIdx, int nEndIdx)
{
    int             i,j,idx;
    ENDI_BATTERY_ENTRY * battEntry;

	OUTTEXT(pSession, "========================\xd\xa");
	OUTTEXT(pSession, " NUM VOLT(V) CURRENT(A)\xd\xa");
	OUTTEXT(pSession, "========================\xd\xa");

    for(i=nStartIdx,idx=1;i<nEndIdx;i++) {
        battEntry = (ENDI_BATTERY_ENTRY *) pInvoke->pResult[i]->stream.p;
        for(j=(pInvoke->pResult[i]->len / sizeof(ENDI_BATTERY_ENTRY)) - 1; j>= 0; j--) {
            if(battEntry[j].BATTERY_VOLT == 0xFFFF) continue;
            OUTTEXT(pSession, " %3d %1.4f %1.4f\xa\xd", idx,
                (float)(BigToHostShort(battEntry[j].BATTERY_VOLT) / 10000),
                (float)(BigToHostShort(battEntry[j].CONSUMPTION_CURRENT) / 10000));
            idx++;
        }
    }
	OUTTEXT(pSession, "========================\xd\xa");
	return CLIERR_OK;
}

int cmdShowSensorBattery(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);
#endif
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	EUI64			id;
    BYTE            nCount=5;
	SENSORINFOENTRYNEW	*pSensor;

    if (!Confirm(pSession, MSG_READ_SENSOR))
		return CLIERR_OK;

	StrToEUI64(argv[0], &id);

    if(argc > 1) {
        nCount = (BYTE) strtol(argv[1],(char **)NULL,10);
        if(nCount > 50) nCount = 50;
    }

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	    return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.45");
    } 

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, &id, sizeof(EUI64));
    invoke.AddParam("1.4", nCount);

    if(NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

    /*-- sensor rom info --*/

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.45")) {
		return CLIERR_OK;
    }

	pInvoke = (IF4Invoke *)invoke.GetHandle();

    return _showSensorBattery(pSession, pInvoke, 0, pInvoke->nResultCount);
}

int cmdShowSensorSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	return CLIERR_OK;
}

int _showSensorAmrData(CLISESSION *pSession, IF4Invoke *pInvoke, int nStartIdx, int nEndIdx)
{
    char            buffer[512];
    ENDI_AMR_DATA   amr;
    int             nPeriod;

    if(nEndIdx - nStartIdx < 1) {
		return CLIERR_OK;
    }

    memcpy(&amr, pInvoke->pResult[nStartIdx]->stream.p, sizeof(ENDI_AMR_DATA));

    switch(amr.period)
    {
        case 1:
        case 2:
        case 4:
        case 12:
        case 60:
            nPeriod = 60 / amr.period;
            break;
        default:
            nPeriod =  amr.period * -1;
            break;
    }

    OUTTEXT(pSession, "          TIME : %04d/%02d/%02d %02d:%02d:%02d (TZ:%03d DST:%03d)\xd\xa", 
                            BigToHostShort(amr.gmttime.year), amr.gmttime.mon, amr.gmttime.day, amr.gmttime.hour, amr.gmttime.min, amr.gmttime.sec,
                                BigToHostShort(amr.gmttime.tz), BigToHostShort(amr.gmttime.dst));
    OUTTEXT(pSession, "        PERIOD : %-2d min                        CHOICE : %d\xd\xa", nPeriod, amr.choice);
    OUTTEXT(pSession, "    ACTIVE DAY : %-10d                ACTIVE MIN : %d\xd\xa", BigToHostShort(amr.oprday), BigToHostShort(amr.actmin));
    OUTTEXT(pSession, " CURRENT PULSE : %-10u                LAST PULSE : %u\xd\xa", BigToHostInt(amr.pulse), BigToHostInt(amr.last_pulse));

    memset(buffer, 0, sizeof(buffer)); getStream2DayString((char *)amr.daymask, buffer);
    OUTTEXT(pSession, "  METERING DAY : %-20s  ", buffer);
    memset(buffer, 0, sizeof(buffer)); getStream2HourString((char *)amr.hourmask, buffer); 
    OUTTEXT(pSession, " METERING HOUR : %s\xd\xa", buffer);

    OUTTEXT(pSession, "     SETUP SEC : %-10d                ALARM FLAG : %s\xd\xa", 
                            BigToHostShort(amr.rep_setuptime), amr.alarm ? "On" : "Off");

	return CLIERR_OK;
}

#ifdef  __SUPPORT_ENDDEVICE__
int cmdShowSensorAmrData(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	EUI64			id;
	SENSORINFOENTRYNEW	*pSensor;

    if (!Confirm(pSession, MSG_READ_SENSOR))
		return CLIERR_OK;

	StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	    return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.28");
    } 

	invoke.AddParamFormat("4.3.1", VARSMI_EUI64, &id, sizeof(EUI64));

    if(NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.28"))
		return CLIERR_OK;

    pInvoke = invoke.GetHandle();

    OUTTEXT(pSession, "            ID : %16s\xd\xa", argv[0]);
    return _showSensorAmrData(pSession, pInvoke, 0, pInvoke->nResultCount);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// User Function
/////////////////////////////////////////////////////////////////////////////

int cmdShowUser(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    IF4Invoke       *pInvoke;
    USRENTRY        *pUser;
    int             i, nCount;

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "106.5"))
        return CLIERR_OK;

    OUTTEXT(pSession, "===================================\xd\xa");
    OUTTEXT(pSession, " NO ID               GROUP\xd\xa");
    OUTTEXT(pSession, "===================================\xd\xa");

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nCount = pInvoke->nResultCount;
    for(i=0; i<nCount; i++)
    {
        pUser = (USRENTRY *)pInvoke->pResult[i]->stream.p;
        OUTTEXT(pSession, " %2d %-16s %s\xd\xa",
                i+1,
                pUser->usrAccount,
                MSGAPI_GetUserType(pUser->usrGroup));
    }
    OUTTEXT(pSession, "===================================\xd\xa");
    return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Other Function
/////////////////////////////////////////////////////////////////////////////

int cmdShowMemory(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	MEMENTRY		*pMemory;
	char			buf[30];

	invoke.AddParam("2.100");
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.1"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	pMemory = (MEMENTRY *)pInvoke->pResult[0]->stream.p;
	
	OUTTEXT(pSession, " Total : %10s Bytes (%2.1f MB)\xd\xa",
						itoa_comma(buf, pMemory->memTotalSize),
						(double)pMemory->memTotalSize / (double)(1024000));
	OUTTEXT(pSession, "  Used : %10s Bytes (%2.1f MB)\xd\xa",
						itoa_comma(buf, pMemory->memUseSize),
						(double)pMemory->memUseSize / (double)(1024000));
	OUTTEXT(pSession, "  Free : %10s Bytes (%2.1f MB)\xd\xa",
						itoa_comma(buf, pMemory->memFreeSize),
						(double)pMemory->memFreeSize / (double)(1024000));
	OUTTEXT(pSession, " Cache : %10s Bytes (%2.1f MB)\xd\xa",
						itoa_comma(buf, pMemory->memCacheSize),
						(double)pMemory->memCacheSize / (double)(1024000));
	OUTTEXT(pSession, "Buffer : %10s Bytes (%2.1f MB)\xd\xa",
						itoa_comma(buf, pMemory->memBufferSize),
						(double)pMemory->memBufferSize / (double)(1024000));
	return CLIERR_OK;
}

int cmdShowFlash(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	FLASHENTRY		*pFlash;
	char			buf[30];

	invoke.AddParam("2.101");
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.1"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	pFlash = (FLASHENTRY *)pInvoke->pResult[0]->stream.p;
	
	OUTTEXT(pSession, " Total : %10s KB (%2.1f MB)\xd\xa",
						itoa_comma(buf, pFlash->flashTotalSize),
						(double)pFlash->flashTotalSize / (double)1024);
	OUTTEXT(pSession, "  Used : %10s KB (%2.1f MB)\xd\xa",
						itoa_comma(buf, pFlash->flashUseSize),
						(double)pFlash->flashUseSize / (double)1024);
	OUTTEXT(pSession, "  Free : %10s KB (%2.1f MB)\xd\xa",
						itoa_comma(buf, pFlash->flashFreeSize),
						(double)pFlash->flashFreeSize / (double)1024);
	return CLIERR_OK;
}

int cmdShowEthernet(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	ETHENTRY		*pEthernet;
	struct	in_addr	addr;

	invoke.AddParam("2.102");
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.1"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	pEthernet = (ETHENTRY *)pInvoke->pResult[0]->stream.p;

	OUTTEXT(pSession, "        Name : %s\xd\xa", pEthernet->ethName);
	addr.s_addr = pEthernet->ethIpAddr;	
	OUTTEXT(pSession, "  IP Address : %s\xd\xa", inet_ntoa(addr));
	OUTTEXT(pSession, " Mac Address : %02X:%02X:%02X:%02X:%02X:%02X\xd\xa",
									pEthernet->ethPhyAddr[0],
									pEthernet->ethPhyAddr[1],
									pEthernet->ethPhyAddr[2],
									pEthernet->ethPhyAddr[3],
									pEthernet->ethPhyAddr[4],
									pEthernet->ethPhyAddr[5]);
	addr.s_addr = pEthernet->ethSubnetMask;
	OUTTEXT(pSession, " Subnet Mask : %s\xd\xa", inet_ntoa(addr));
	addr.s_addr = pEthernet->ethGateway;
	if (pEthernet->ethGateway == 0)
		 OUTTEXT(pSession, "     Gateway : *\xd\xa");
	else OUTTEXT(pSession, "     Gateway : %s\xd\xa", inet_ntoa(addr));
	return CLIERR_OK;
}

int cmdShowGpio(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	MIBValue		**pValue;

	invoke.AddParam("2.103");
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.4"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	pValue  = pInvoke->pResult;
	
	OUTTEXT(pSession, " Power Fail : %0d %s\xd\xa",
			pValue[1]->stream.u8, pValue[1]->stream.u8 ? "POWER FAIL" : "NORMAL");
	OUTTEXT(pSession, "Low Battery : %0d %s\xd\xa",
			pValue[2]->stream.u8, pValue[2]->stream.u8 ? "NORMAL" : "LOW BATTERY");
	OUTTEXT(pSession, " STBY State : %0d %s\xd\xa",
			pValue[4]->stream.u8, pValue[4]->stream.u8 ? "OFF" : "ACTIVE");
	OUTTEXT(pSession, "  DCD State : %0d %s\xd\xa",
			pValue[5]->stream.u8, pValue[5]->stream.u8 ? "OFF" : "ACTIVE");
	OUTTEXT(pSession, "   RI State : %0d %s\xd\xa",
			pValue[6]->stream.u8, pValue[6]->stream.u8 ? "OFF" : "ACTIVE");
/*
	OUTTEXT(pSession, "    Heater State : %0d %s\xd\xa",
			pValue[9]->stream.u8, pValue[9]->stream.u8 ? "HIGH" : "LOW");
	OUTTEXT(pSession, "Cover Open State : %0d %s\xd\xa",
			pValue[10]->stream.u8, pValue[10]->stream.u8 ? "HIGH" : "LOW");
	OUTTEXT(pSession, "  Charging State : %0d %s\xd\xa",
			pValue[11]->stream.u8, pValue[11]->stream.u8 ? "HIGH" : "LOW");
*/
	return CLIERR_OK;
}

int _executeCommand(CLISESSION *pSession, const char * szCommand)
{
	char	szLine[128];
	FILE	*fp;

    memset(szLine,0,sizeof(szLine));

    if((fp = popen(szCommand, "r")) != NULL) {
		while(fgets(szLine, sizeof(szLine)-1, fp) > 0)
			OUTTEXT(pSession, "%s\xd", szLine);
		fclose(fp);	
	}
	return CLIERR_OK;
}

int cmdShowProcess(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    return _executeCommand(pSession, "/bin/ps");
}

int cmdShowFileList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	char	szCommand[512];

    sprintf(szCommand,"/bin/ls -al %s", argc > 0 ? argv[0] : "");
    return _executeCommand(pSession, szCommand);
}

int cmdShowInterface(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    return _executeCommand(pSession, "/sbin/ifconfig");
}

int cmdShowPpp(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	PPPENTRY		*pEntry;

	invoke.AddParam("2.107");
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.1"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	pEntry = (PPPENTRY *)pInvoke->pResult[0]->stream.p;
	
	OUTTEXT(pSession, "Provider : %s\xd\xa", pEntry->pppProvider);
	OUTTEXT(pSession, " User ID : %s\xd\xa",
					*pEntry->pppUser ? pEntry->pppUser : "<Not Define>");
	OUTTEXT(pSession, "Password : %s\xd\xa",
					*pEntry->pppPassword ? pEntry->pppPassword : "<Not Define>");
	return CLIERR_OK;
}

int cmdShowPppLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	char	szLine[256];
	char	*p, *p1;
	FILE	*fp;

    memset(szLine,0,sizeof(szLine));

	fp = fopen("/var/log/messages", "rt");
	if (fp != NULL)
	{
		while(fgets(szLine, sizeof(szLine)-1, fp) > 0)
		{
			p = (char *)szLine;
			p1 = strstr(szLine, "]:");
			if (p1 != NULL)
				p = p1;
 
			OUTTEXT(pSession, "%s\xd", p);
		}	
		fclose(fp);
	}
	return CLIERR_OK;
}

int cmdShowVersion(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	ENVENTRY		*pEnv;

	invoke.AddParam("2.108");
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.1"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	pEnv = (ENVENTRY *)pInvoke->pResult[0]->stream.p;
	
	OUTTEXT(pSession, "Kernel Version : %s\xd\xa", pEnv->envKernelVersion);
	OUTTEXT(pSession, "   GCC Version : %s\xd\xa", pEnv->envGccVersion);
	OUTTEXT(pSession, "     Processor : %s\xd\xa", pEnv->envCpuName);
	OUTTEXT(pSession, "          MIPS : %.2f\xd\xa", (double)((double)pEnv->envCpuMips/(double)100));
	return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Log List
/////////////////////////////////////////////////////////////////////////////

int cmdShowMeteringInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
	IF4Invoke		*pInvoke;
	TIMESTAMP		tmStart, tmEnd;
	METERLPENTRY	*pItem;
    EUI64           id;
	SENSORINFOENTRYNEW	*pSensor;
	char			szGUID[MAX_GUID_STRING+1];
	char			szConnectTime[30];
	char			szInstallTime[30];
	int				i, nCount, nMDataCnt = 1;
    BOOL            bAll = FALSE;
    BOOL            bDetail = TRUE;

    UINT            parserType = PARSER_TYPE_UNKNOWN;

    if(check_id(NULL, argv[0])) { // EUI64 ID
  	    StrToEUI64(argv[0], &id);
        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    GetTimestamp(&tmStart, NULL);
	GetTimestamp(&tmEnd, NULL);

    if(argc > 1)
    {
		tmStart.year = atoi(argv[1]);
		tmEnd.year  = atoi(argv[1]);
    }
    if(argc > 2)
    {
		tmStart.mon	 = atoi(argv[2]);
		tmEnd.mon	= atoi(argv[2]);
    }
    if(argc > 3)
    {
		tmStart.day  = atoi(argv[3]);
		tmEnd.day	= atoi(argv[3]);
    }

    /** 검침데이터 갯수를 지정할 수 있게 했다. 
      * Default로 마지막 1개만 가지고 온다.
      */
    if(argc > 4)
    {
        nMDataCnt = (int) strtol(argv[4], (char **)NULL, 10);
    }

    if(nMDataCnt < 0) nMDataCnt = 0;

	tmStart.hour	= 0;
	tmStart.min		= 0;
	tmStart.sec		= 0;
	tmEnd.hour		= 23;
	tmEnd.min		= 59;
	tmEnd.sec		= 59;

    if(!bAll) {
        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	    invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmStart, sizeof(TIMESTAMP));
	    invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmEnd, sizeof(TIMESTAMP));
	    invoke.AddParam("1.5", (WORD) nMDataCnt);

	    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
		    return CLIERR_OK;

	    pInvoke = (IF4Invoke *)invoke2.GetHandle();
	    pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[0]->stream.p;

	    EUI64ToStr(&pSensor->sensorID, szGUID);
	    IF4API_TIMESTAMPTOSTR(szConnectTime, &pSensor->sensorLastConnect);
	    IF4API_TIMESTAMPTOSTR(szInstallTime, &pSensor->sensorInstallDate);

        parserType = getParserType(pSensor->sensorParser, pSensor->sensorModel);
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.4"))
		    return CLIERR_OK;

	    pInvoke = (IF4Invoke *)invoke.GetHandle();
	    nCount  = pInvoke->nResultCount;

	    OUTTEXT(pSession, "=====================================================================================\xd\xa");
	    OUTTEXT(pSession, "     SENSOR ID : %s\xd\xa", szGUID);
        if (nCount > 0)
        {
		    pItem = (METERLPENTRY *)pInvoke->pResult[0]->stream.p;
            if(pItem->mlpVendor)
            {
	            OUTTEXT(pSession, "        VENDOR : %s\xd\xa", getVendorString(pItem->mlpVendor));
            }
        }
	    OUTTEXT(pSession, "         MODEL : %s\xd\xa", pSensor->sensorModel);
	    OUTTEXT(pSession, "     METER S/N : %s\xd\xa", pSensor->sensorSerialNumber);
	    OUTTEXT(pSession, "         STATE : %s\xd\xa", MSGAPI_GetSensorState(pSensor->sensorState));
        OUTTEXT(pSession, "  NETWORK TYPE : %s\xd\xa", pSensor->sensorAttr & ENDDEVICE_ATTR_RFD ? "RFD" : "FFD");
	    OUTTEXT(pSession, "  INSTALL DATE : %s\xd\xa", szInstallTime);
	    OUTTEXT(pSession, " LAST METERING : %s\xd\xa", szConnectTime);

        qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmpmetering);

	    if (nCount == 0)
	    {
	        OUTTEXT(pSession, " METERING DATA : No entry\xd\xa");
	        OUTTEXT(pSession, "=====================================================================================\xd\xa");
		    return CLIERR_OK;
	    }

	    for(i=0; i<nCount; i++)
	    {
		    pItem = (METERLPENTRY *)pInvoke->pResult[i]->stream.p;

	        OUTTEXT(pSession, " METERING DATA : %d (%d bytes)\xd\xa", pItem->mlpDataCount, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP));
	        OUTTEXT(pSession, " METERING TIME : %04d/%02d/%02d %02d:%02d:%02d\xd\xa", 
                    pItem->mlpTime.year,
                    pItem->mlpTime.mon,
                    pItem->mlpTime.day,
                    pItem->mlpTime.hour,
                    pItem->mlpTime.min,
                    pItem->mlpTime.sec
                );

            switch(parserType) {
                case PARSER_TYPE_AIDON:
                case PARSER_TYPE_OSAKI:
                    aidon_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
                    break;
                case PARSER_TYPE_SX:
                    sx_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
                    break;
                case PARSER_TYPE_ANSI:
                    ge_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
                    break;
                case PARSER_TYPE_DLMS:
                    dlms_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
                    break;
                case PARSER_TYPE_PULSE:
                case PARSER_TYPE_ACD:
                case PARSER_TYPE_SMOKE:
                case PARSER_TYPE_HMU:
                case PARSER_TYPE_I210:
                    pulse_data_parsing(pSession, pSensor->sensorModel, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
                    break;
                case PARSER_TYPE_KAMST:
                    kamstrup_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), TRUE);
                    break;
                case PARSER_TYPE_REPEATER:
                    repeater_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
                    break;
                case PARSER_TYPE_MBUS:
                    mbus_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
                    break;
                case PARSER_TYPE_IHD:
                    ihd_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
                    break;
            }
        }
	}

	OUTTEXT(pSession, "=====================================================================================\xd\xa");
	return CLIERR_OK;
}

int cmdShowLoadProfile(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
	IF4Invoke		*pInvoke;
	SENSORINFOENTRYNEW	*pSensor;
    EUI64           id;
	TIMESTAMP		tmDay;
	int				i, j, idx, nCount;
	char			szGUID[MAX_GUID_STRING+1];
	char			szConnectTime[30];
	char			szInstallTime[30];
    BYTE            nChCnt = 0;
    WORD            nLpCnt = 0;
    TIMESTAMP       timeStamp;

    StrToEUI64(argv[0], &id);
    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));

    GetTimestamp(&tmDay, NULL);

    if(argc > 1)
    {
		tmDay.day  = atoi(argv[1]);
    }
    if(argc > 2)
    {
		tmDay.mon	 = atoi(argv[2]);
    }
    if(argc > 3)
    {
		tmDay.year = atoi(argv[3]);
    }

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmDay, sizeof(TIMESTAMP));

    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;

    pInvoke = (IF4Invoke *)invoke2.GetHandle();
    pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[0]->stream.p;

    EUI64ToStr(&pSensor->sensorID, szGUID);
    IF4API_TIMESTAMPTOSTR(szConnectTime, &pSensor->sensorLastConnect);
    IF4API_TIMESTAMPTOSTR(szInstallTime, &pSensor->sensorInstallDate);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.17"))
        return CLIERR_OK;

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nCount  = pInvoke->nResultCount;

    OUTTEXT(pSession, "=====================================================================================\xd\xa");
    OUTTEXT(pSession, "     SENSOR ID : %s\xd\xa", szGUID);
    OUTTEXT(pSession, "         MODEL : %s\xd\xa", pSensor->sensorModel);
    OUTTEXT(pSession, "     METER S/N : %s\xd\xa", pSensor->sensorSerialNumber);
    OUTTEXT(pSession, "         STATE : %s\xd\xa", MSGAPI_GetSensorState(pSensor->sensorState));
    OUTTEXT(pSession, "  NETWORK TYPE : %s\xd\xa", pSensor->sensorAttr & ENDDEVICE_ATTR_RFD ? "RFD" : "FFD");
    OUTTEXT(pSession, "  INSTALL DATE : %s\xd\xa", szInstallTime);
    OUTTEXT(pSession, " LAST METERING : %s\xd\xa", szConnectTime);

    nChCnt = pInvoke->pResult[0]->stream.u8;
    nLpCnt = pInvoke->pResult[2]->stream.u16;

    OUTTEXT(pSession, "   CHANNEL CNT : %d\xd\xa", nChCnt);
    OUTTEXT(pSession, "        LP CNT : %d\xd\xa", nChCnt ? nLpCnt / nChCnt : 0);
    OUTTEXT(pSession, "    BASE PULSE : %04d/%02d/%02d\xd\xa", tmDay.year, tmDay.mon, tmDay.day);
    OUTTEXT(pSession, "         ");
    for(i=0; i< nChCnt; i++) 
    {
        OUTTEXT(pSession, " %6lld", LittleStreamToHostLong((BYTE *)pInvoke->pResult[1]->stream.p + (i*8), 8));
    }
    OUTTEXT(pSession, "\xd\xa");
    for(i=0, idx=3; i< nLpCnt; )
    {
        memcpy(&timeStamp, pInvoke->pResult[idx]->stream.p, sizeof(TIMESTAMP));     idx ++;
        OUTTEXT(pSession, "  %02d:%02d :", timeStamp.hour, timeStamp.min);
        for(j=0; j< nChCnt; j++, i++)
        {
            OUTTEXT(pSession, " %6lld", LittleStreamToHostLong((BYTE *)pInvoke->pResult[idx]->stream.p + (j*8), 8));
        } idx++;
        OUTTEXT(pSession, "\xd\xa");
    }

	OUTTEXT(pSession, "=====================================================================================\xd\xa");
	return CLIERR_OK;
}

int cmdShowMeteringList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT * 2);
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT * 2);
	IF4Invoke		*pInvoke, *pInvoke2;
	TIMESTAMP		tmStart, tmEnd;
	METERLPENTRY	*pItem;
	SENSORINFOENTRYNEW	*pSensor;
	char			szGUID[MAX_GUID_STRING+1];
	char			szSerial[21];
	char			szModel[16];
    char            szType[128];
	int				i, j, k;
    int             nCount, nCount2, ret, res;
	BOOL			bMore = TRUE, bMetering;
    BYTE            hourMask[24];
    time_t          t;
    BYTE            deviceAttr = 0xFF;
    int             nTotalSensor = 0, nTSuccCnt = 0, nTFailCnt = 0;
    int             nTypeCnt = 0, nTypeSuccCnt = 0, nTypeFailCnt = 0;

	GetTimestamp(&tmStart, NULL);
	GetTimestamp(&tmEnd, NULL);

	if ((argc > 0) && (argc != 3))
	{
		OUTTEXT(pSession, "Need more parameters");
		return CLIERR_OK;
	}

	if (argc == 0)
	{
        time(&t);
        GetTimestamp(&tmStart, &t);
        GetTimestamp(&tmEnd, &t);
	}
	else if (argc == 3)
	{
		tmStart.year = atoi(argv[0]);
		tmStart.mon	 = atoi(argv[1]);
		tmStart.day  = atoi(argv[2]);

		tmEnd.year  = atoi(argv[0]);
		tmEnd.mon	= atoi(argv[1]);
		tmEnd.day   = atoi(argv[2]);
	}

	tmStart.hour	= 0;
	tmStart.min	= 0;
	tmStart.sec	= 0;
	tmEnd.hour		= 23;
	tmEnd.min	= 59;
	tmEnd.sec	= 59;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.26"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

    qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmpsensor);

	invoke2.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmStart, sizeof(TIMESTAMP));
	invoke2.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmEnd, sizeof(TIMESTAMP));
	invoke2.AddParam("1.3", (BOOL) TRUE);

	if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "104.5"))
	return CLIERR_OK;

    pInvoke2 = (IF4Invoke *)invoke2.GetHandle();
    nCount2  = pInvoke2->nResultCount;

    qsort(pInvoke2->pResult, nCount2, sizeof(MIBValue *), _cmpmetering);

    nTotalSensor = nCount;
    OUTTEXT(pSession, "=========================================================================================================\xd\xa");
	for(i=0,j=0; i<nCount; i++)
	{
		pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;

        if((pSensor->sensorAttr & ENDDEVICE_ATTR_MBUS_ARM) || 
           (pSensor->sensorAttr & ENDDEVICE_ATTR_MBUS_SLAVE)) continue;

        if(deviceAttr != pSensor->sensorAttr) {
            j = 0;
            if(deviceAttr != 0xFF) {
                OUTTEXT(pSession, " %-26s : Succ %3d%%(%4d) Fail %3d%%(%4d) Total %4d\xd\xa",
                    szType, nTypeCnt ? (int)(100*nTypeSuccCnt/nTypeCnt) : 0, nTypeSuccCnt, 
                    nTypeCnt ? (int)(100*nTypeFailCnt/nTypeCnt) : 0, nTypeFailCnt, nTypeCnt);
            }
            deviceAttr = pSensor->sensorAttr;
            nTypeCnt = 0; nTypeSuccCnt = 0; nTypeFailCnt = 0;
            memset(szType, 0, sizeof(szType));
            if(deviceAttr & ENDDEVICE_ATTR_SOLARNODE) strcat(szType,"Solar "); 
            else {
                if(deviceAttr & ENDDEVICE_ATTR_POWERNODE) strcat(szType,"Power ");
                if(deviceAttr & ENDDEVICE_ATTR_BATTERYNODE) strcat(szType,"Battery ");
            }
            if(deviceAttr & ENDDEVICE_ATTR_REPEATER) strcat(szType,"Repeater ");
            if(deviceAttr & ENDDEVICE_ATTR_MBUS_MASTER) strcat(szType,"MBUS ");
            if(deviceAttr & ENDDEVICE_ATTR_RFD) strcat(szType,"(RFD) ");
            else                                strcat(szType,"(FFD) ");
        }
        bMetering = FALSE;

        for(;j<nCount2;j++) {
		    pItem = (METERLPENTRY *)pInvoke2->pResult[j]->stream.p;
            res = memcmp(&pSensor->sensorID, &pItem->mlpId, sizeof(EUI64));
            if(!res) {
                bMetering = TRUE;
            }else if(res < 0) break;
		}
        nTypeCnt++; 
        if(bMetering)   { nTypeSuccCnt++; nTSuccCnt++; }
        else            { nTypeFailCnt++; nTFailCnt++; }
	}
    
    if(deviceAttr != 0xFF) {
        OUTTEXT(pSession, " %-26s : Succ %3d%%(%4d) Fail %3d%%(%4d) Total %4d\xd\xa",
            szType, nTypeCnt ? (int)(100*nTypeSuccCnt/nTypeCnt) : 0, nTypeSuccCnt, 
            nTypeCnt ? (int)(100*nTypeFailCnt/nTypeCnt) : 0, nTypeFailCnt, nTypeCnt);
    }
    OUTTEXT(pSession, " %-26s : Succ %3d%%(%4d) Fail %3d%%(%4d) Total %4d\xd\xa",
        "TOTAL", nTotalSensor ? (int)(100*nTSuccCnt/nTotalSensor) : 0, nTSuccCnt, 
        nTotalSensor ? (int)(100*nTFailCnt/nTotalSensor) : 0, nTFailCnt, nTotalSensor);


    OUTTEXT(pSession, "=========================================================================================================\xd\xa");
    ret = More(pSession, 0, 0);
    if (ret == 0) return CLIERR_OK;
    if (ret == 2) bMore = FALSE;

	OUTTEXT(pSession, "                                                         0                   1                   2      \xd\xa");
	OUTTEXT(pSession, " NUM SENSOR ID        METER SERIAL       MODEL           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3\xd\xa");
	OUTTEXT(pSession, "---------------------------------------------------------------------------------------------------------\xd\xa");

    deviceAttr = 0xFF;
	for(i=0,j=0; i<nCount; i++)
	{
		pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;

        if((pSensor->sensorAttr & ENDDEVICE_ATTR_MBUS_ARM) || 
           (pSensor->sensorAttr & ENDDEVICE_ATTR_MBUS_SLAVE)) continue;

		EUI64ToStr(&pSensor->sensorID, szGUID);
		memcpy(szSerial, pSensor->sensorSerialNumber, 18); szSerial[18] = '\0';
		memcpy(szModel, pSensor->sensorModel, 14); szModel[14] = '\0';

        if(deviceAttr != pSensor->sensorAttr) {
            j = 0;
            deviceAttr = pSensor->sensorAttr;
        }
        memset(hourMask, 0, sizeof(hourMask));

        OUTTEXT(pSession, " %3d %16s %-18s %-14s ", i+1, szGUID, szSerial, szModel);
        for(;j<nCount2;j++) {
		    pItem = (METERLPENTRY *)pInvoke2->pResult[j]->stream.p;
            res = memcmp(&pSensor->sensorID, &pItem->mlpId, sizeof(EUI64));
            if(!res) {
                hourMask[pItem->mlpTime.hour] |=  (0x01 << (int)(pItem->mlpTime.min / 15));
            }else if(res < 0) break;
		}
        for(k=0;k<24;k++) {
            if(hourMask[k]) OUTTEXT(pSession, " %X", hourMask[k]);
            else OUTTEXT(pSession, "  ");
	    }
        OUTTEXT(pSession, "\xd\xa");

		if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
	    {
		    ret = More(pSession, nCount, i+1);
		    if (ret == 0) return CLIERR_OK;
		    if (ret == 2) bMore = FALSE;
		}
	}

	OUTTEXT(pSession, "=========================================================================================================\xd\xa");
	return CLIERR_OK;
}

int cmdShowPhoneList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	int				i, nCount;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "107.1"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;

	if (nCount == 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

	for(i=0; i<nCount; i++)
	{
		OUTTEXT(pSession, " %2d: [%s]\r\n", i+1, pInvoke->pResult[i]->stream.p);
	}
	return CLIERR_OK;
}

int cmdShowMobile(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	FILE	*fp;
	char	szBuffer[128];
	char	szVersion[16] = "";
	char	szSCID[32] = "";
	char	szAuth[16] = "";
	char	szTime[32] = "";
	char	szCSQ[16] = "";
	char	szSBV[16] = "";
	int		nCSQ = 0;

	fp = fopen("/app/sw/mobileinfo", "rt");
	if (fp != NULL)
	{
		while(fgets(szBuffer, 127, fp))
		{
			szBuffer[strlen(szBuffer)-1] = '\0';

			if (strncmp(szBuffer, "+GMR: S/W VER: ", 15) == 0)
				strcpy(szVersion, &szBuffer[15]);
			else if (strncmp(szBuffer, "^SCID: ", 7) == 0)
				strcpy(szSCID, &szBuffer[7]);
			else if (strncmp(szBuffer, "+CPIN: ", 7) == 0)
				strcpy(szAuth, &szBuffer[7]);
			else if (strncmp(szBuffer, "+TIME: ", 7) == 0)
				strcpy(szTime, &szBuffer[7]);
			else if (strncmp(szBuffer, "+CSQ: ", 6) == 0)
			{
				strcpy(szCSQ, &szBuffer[6]);
				if (atoi(szCSQ) <= 31)
					 nCSQ = -(113-(atoi(szCSQ)*2));
				nCSQ = MIN(0, nCSQ);
			}
			else if (strncmp(szBuffer, "^SBV: ", 6) == 0)
				strcpy(szSBV, &szBuffer[6]);
		}
		fclose(fp);
	}

	OUTTEXT(pSession, " Version : %s\r\n", szVersion);
	OUTTEXT(pSession, "  SIM ID : %s\r\n", szSCID);
	OUTTEXT(pSession, "SIM Auth : %s\r\n", szAuth);
	OUTTEXT(pSession, "    Time : %s\r\n", szTime);
	OUTTEXT(pSession, "     CSQ : %0d dBm\r\n", nCSQ);
	OUTTEXT(pSession, "   Power : %.3f V\r\n", atof(szSBV) / 1000);
	return CLIERR_OK;
}

int cmdShowCommandLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szPath[64];
    char    szBuffer[256];
    time_t  now;
    struct  tm  when;
    FILE    *fp;
    int     nCount = 0, ret;
    BOOL    bMore = TRUE;
    
    if ((argc != 0) && (argc != 3))
    {
        OUTTEXT(pSession, "Need more parameter.\xd\xa");
        return CLIERR_OK; 
    }   
    
    if (argc == 3)
    {
         sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, CMD_HISTORY_LOG_FILE, 
                 atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }
    else 
    {
        time(&now);
        when = *localtime(&now);
        sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, CMD_HISTORY_LOG_FILE,
            when.tm_year+1900, when.tm_mon+1, when.tm_mday);
    }       
    
    fp = fopen(szPath, "rt");
    if (fp == NULL)
    {
        OUTTEXT(pSession, "No data.\xd\xa");
        return CLIERR_OK;
    }

    while(fgets(szBuffer, 255, fp))
    {
        szBuffer[strlen(szBuffer)-1] = '\0';
        OUTTEXT(pSession, "%s\r\n", szBuffer);
        nCount++;

        if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, 0, 0);
            if (ret == 0) 
            {
                fclose(fp);
                return CLIERR_OK;
            }
            if (ret == 2) bMore = FALSE;
        }
    }

    fclose(fp);
    return CLIERR_OK;
}

#if 0
/** Debug Log 파일을 보는 기능은 아직 구현되지 않았다 */
int cmdShowDebugLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szPath[64];
    char    szBuffer[256];
    time_t  now;
    struct  tm  when;
    FILE    *fp;
    int     nCount = 0, ret;
    BOOL    bMore = TRUE;
    
    if ((argc != 0) && (argc != 3))
    {
        OUTTEXT(pSession, "Need more parameter.\xd\xa");
        return CLIERR_OK; 
    }   
    
    if (argc == 3)
         sprintf(szPath, "/app/log/LOG%04d%02d%02d.log", atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    else 
    {
        time(&now);
        when = *localtime(&now);
        sprintf(szPath, "/app/log/LOG%04d%02d%02d.log",
            when.tm_year+1900, when.tm_mon+1, when.tm_mday);
    }       
    
    fp = fopen(szPath, "rt");
    if (fp == NULL)
    {
        OUTTEXT(pSession, "No data.\xd\xa");
        return CLIERR_OK;
    }

    while(fgets(szBuffer, 255, fp))
    {
        szBuffer[strlen(szBuffer)-1] = '\0';
        OUTTEXT(pSession, "%s\r\n", szBuffer);
        nCount++;

        if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, 0, 0);
            if (ret == 0) 
            {
                fclose(fp);
                return CLIERR_OK;
            }
            if (ret == 2) bMore = FALSE;
        }
    }

    fclose(fp);
    return CLIERR_OK;
}
#endif

int cmdShowCommLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szPath[64];
    char    szBuffer[256];
    time_t  now;
    struct  tm  when;
    FILE    *fp;
    int     nCount = 0, ret;
    BOOL    bMore = TRUE;
    
    if ((argc != 0) && (argc != 3))
    {
        OUTTEXT(pSession, "Need more parameter.\xd\xa");
        return CLIERR_OK; 
    }   
    
    if (argc == 3)
    {
         sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, COMM_LOG_FILE, 
                 atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }
    else 
    {
        time(&now);
        when = *localtime(&now);
        sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, COMM_LOG_FILE, 
            when.tm_year+1900, when.tm_mon+1, when.tm_mday);
    }       
    
    fp = fopen(szPath, "rt");
    if (fp == NULL)
    {
        OUTTEXT(pSession, "No data.\xd\xa");
        return CLIERR_OK;
    }

    while(fgets(szBuffer, 255, fp))
    {
        szBuffer[strlen(szBuffer)-1] = '\0';
        OUTTEXT(pSession, "%s\r\n", szBuffer);
        nCount++;

        if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, 0, 0);
            if (ret == 0) 
            {
                fclose(fp);
                return CLIERR_OK;
            }
            if (ret == 2) bMore = FALSE;
        }
    }

    fclose(fp);
    return CLIERR_OK;
}

int cmdShowMobileLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szPath[64];
    char    szBuffer[256];
    time_t  now;
    struct  tm  when;
    FILE    *fp;
    int     nCount = 0, ret;
    BOOL    bMore = TRUE;
    
    if ((argc != 0) && (argc != 3))
    {
        OUTTEXT(pSession, "Need more parameter.\xd\xa");
        return CLIERR_OK; 
    }   
    
    if (argc == 3)
    {
         sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, MOBILE_LOG_FILE,
                 atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }
    else 
    {
        time(&now);
        when = *localtime(&now);
        sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, MOBILE_LOG_FILE, 
            when.tm_year+1900, when.tm_mon+1, when.tm_mday);
    }       
    
    fp = fopen(szPath, "rt");
    if (fp == NULL)
    {
        OUTTEXT(pSession, "No data.\xd\xa");
        return CLIERR_OK;
    }

    while(fgets(szBuffer, 255, fp))
    {
        szBuffer[strlen(szBuffer)-1] = '\0';
        OUTTEXT(pSession, "%s\r\n", szBuffer);
        nCount++;

        if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, 0, 0);
            if (ret == 0) 
            {
                fclose(fp);
                return CLIERR_OK;
            }
            if (ret == 2) bMore = FALSE;
        }
    }

    fclose(fp);
    return CLIERR_OK;
}

int cmdShowUploadLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szPath[64];
    char    szBuffer[256];
    time_t  now;
    struct  tm  when;
    FILE    *fp;
    int     nCount = 0, ret;
    BOOL    bMore = TRUE;
    
    if ((argc != 0) && (argc != 3))
    {
        OUTTEXT(pSession, "Need more parameter.\xd\xa");
        return CLIERR_OK; 
    }   
    
    if (argc == 3)
    {
         sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, UPLOAD_LOG_FILE,
                 atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }
    else 
    {
        time(&now);
        when = *localtime(&now);
        sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, UPLOAD_LOG_FILE,
            when.tm_year+1900, when.tm_mon+1, when.tm_mday);
    }       
    
    fp = fopen(szPath, "rt");
    if (fp == NULL)
    {
        OUTTEXT(pSession, "No data.\xd\xa");
        return CLIERR_OK;
    }

    while(fgets(szBuffer, 255, fp))
    {
        szBuffer[strlen(szBuffer)-1] = '\0';
        OUTTEXT(pSession, "%s\r\n", szBuffer);
        nCount++;

        if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, 0, 0);
            if (ret == 0) 
            {
                fclose(fp);
                return CLIERR_OK;
            }
            if (ret == 2) bMore = FALSE;
        }
    }

    fclose(fp);
    return CLIERR_OK;
}

BOOL _printPackageInfo(CLISESSION *pSession, const char * path)
{
    DIR	    *dir_fdesc;
    dirent  *dp;
    struct stat buf;
    FILE    *fp;
    int     len, n;
    PACKAGEINFO pkginfo;
    char    *pStr, pStrArr[10]={0,};
    char    *pFile, pFileArr[18]={0,};
    char    hw[16], fw[16];
    int     build;
    char    childpath[256];
    
    dir_fdesc = opendir(path);
    if (!dir_fdesc) {
        return FALSE;
    }
    while((dp=readdir(dir_fdesc))) {
        if(dp->d_name[0] == '.') continue;
        
        sprintf(childpath,"%s/%s", path, dp->d_name);
        if(stat(childpath, &buf)) continue;
        if(S_ISDIR(buf.st_mode)) {
            _printPackageInfo(pSession, childpath);
            continue;
        }
        if(!S_ISREG(buf.st_mode)) continue;
        if((len = strlen(dp->d_name)) < 5) continue;
        if(strncmp(dp->d_name + (len - 4), ".pkg", 4)==0) {
	        fp = fopen(childpath, "rb");
	        if (fp == NULL) continue;

	        memset(&pkginfo, 0, sizeof(PACKAGEINFO));
	        n = fread(&pkginfo, 1, sizeof(PACKAGEINFO), fp);

	        fclose(fp);
            if(n != sizeof(PACKAGEINFO)) continue;

            memset(pStrArr, 0, sizeof(pStrArr));
            memset(pFileArr, 0, sizeof(pFileArr));
            pStr = CollapseString(pkginfo.szModel, (char *) pStrArr, sizeof(pStrArr));
            pFile = CollapseString(dp->d_name, (char *) pFileArr, sizeof(pFileArr));
		    sprintf(hw, "%2d.%-2d", pkginfo.nHwVersion >> 8, pkginfo.nHwVersion & 0xff);
		    sprintf(fw, "%2d.%-2d", pkginfo.nSwVersion >> 8, pkginfo.nSwVersion & 0xff);
            build = pkginfo.nBuildVersion;
            pkginfo.szChecksum[32] = 0x00;

		    OUTTEXT(pSession, " %-12s %-10s %s %s %5d %s %s\r\n", 
                    MSGAPI_DistributionType(pkginfo.nType),
                    pkginfo.nType == OTA_TYPE_SENSOR ? pStr : "",
                    hw,
                    fw,
                    build,
                    pkginfo.szChecksum,
                    pFile
                );
        }
    }
	closedir(dir_fdesc);

    return TRUE;
}

int cmdShowPackageInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szType[64] = {0, };
    BOOL    bAll = TRUE;
    
    if (argc > 0) {
        bAll = FALSE;
        strcpy(szType, argv[0]);
    }
	OUTTEXT(pSession, "==================================================================================================\xd\xa");
	OUTTEXT(pSession, " TYPE         MODEL       H/W   F/W  BUILD CHECKSUM                         PKG FILE\xd\xa");
	OUTTEXT(pSession, "==================================================================================================\xd\xa");
    if(bAll) _printPackageInfo(pSession, "/app/fw");
    else if(strncasecmp(szType,"con",3)==0) _printPackageInfo(pSession, "/app/fw/concentrator");
    else if(strncasecmp(szType,"coo",3)==0) _printPackageInfo(pSession, "/app/fw/coordinator");
    else if(strncasecmp(szType,"sen",3)==0) _printPackageInfo(pSession, "/app/fw/sensor");
    else _printPackageInfo(pSession, "/app/fw");
	OUTTEXT(pSession, "==================================================================================================\xd\xa");

    return CLIERR_OK;
}

int cmdShowTimesyncLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szPath[64];
    char    szBuffer[256];
    time_t  now;
    struct  tm  when;
    FILE    *fp;
    int     nCount = 0, ret;
    BOOL    bMore = TRUE;
    
    if ((argc != 0) && (argc != 3))
    {
        OUTTEXT(pSession, "Need more parameter.\xd\xa");
        return CLIERR_OK; 
    }   
    
    if (argc == 3)
         sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, TIMESYNC_LOG_FILE,
                 atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    else 
    {
        time(&now);
        when = *localtime(&now);
        sprintf(szPath, "%s/%s%04d%02d%02d.log",LOG_DIR, TIMESYNC_LOG_FILE,
            when.tm_year+1900, when.tm_mon+1, when.tm_mday);
    }       
    
    fp = fopen(szPath, "rt");
    if (fp == NULL)
    {
        OUTTEXT(pSession, "No data.\xd\xa");
        return CLIERR_OK;
    }

    while(fgets(szBuffer, 255, fp))
    {
        szBuffer[strlen(szBuffer)-1] = '\0';
        OUTTEXT(pSession, "%s\r\n", szBuffer);
        nCount++;

        if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, 0, 0);
            if (ret == 0) 
            {
                fclose(fp);
                return CLIERR_OK;
            }
            if (ret == 2) bMore = FALSE;
        }
    }

    fclose(fp);
    return CLIERR_OK;
}

int cmdShowMeteringLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szPath[64];
    char    szBuffer[256];
    time_t  now;
    struct  tm  when;
    FILE    *fp;
    int     nCount = 0, ret;
    BOOL    bMore = TRUE;
    
    if ((argc != 0) && (argc != 3))
    {
        OUTTEXT(pSession, "Need more parameter.\xd\xa");
        return CLIERR_OK; 
    }   
    
    if (argc == 3)
    {
         sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, METERING_LOG_FILE,
                 atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }
    else 
    {
        time(&now);
        when = *localtime(&now);
        sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, METERING_LOG_FILE,
            when.tm_year+1900, when.tm_mon+1, when.tm_mday);
    }       
    
    fp = fopen(szPath, "rt");
    if (fp == NULL)
    {
        OUTTEXT(pSession, "No data.\xd\xa");
        return CLIERR_OK;
    }

    while(fgets(szBuffer, 255, fp))
    {
        szBuffer[strlen(szBuffer)-1] = '\0';
        OUTTEXT(pSession, "%s\r\n", szBuffer);
        nCount++;

        if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, 0, 0);
            if (ret == 0) 
            {
                fclose(fp);
                return CLIERR_OK;
            }
            if (ret == 2) bMore = FALSE;
        }
    }

    fclose(fp);
    return CLIERR_OK;
}

int cmdShowEvent(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szPath[64];
    char    szBuffer[256];
    time_t  now;
    struct  tm  when;
    FILE    *fp;
    int     nCount = 0, ret;
    BOOL    bMore = TRUE;
    
    if ((argc != 0) && (argc != 3))
    {
        OUTTEXT(pSession, "Need more parameter.\xd\xa");
        return CLIERR_OK; 
    }   
    
    if (argc == 3)
    {
         sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, EVENT_LOG_FILE, 
                 atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }
    else 
    {
        time(&now);
        when = *localtime(&now);
        sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, EVENT_LOG_FILE,
            when.tm_year+1900, when.tm_mon+1, when.tm_mday);
    }       
    
    fp = fopen(szPath, "rt");
    if (fp == NULL)
    {
        OUTTEXT(pSession, "No data.\xd\xa");
        return CLIERR_OK;
    }

    while(fgets(szBuffer, 255, fp))
    {
        szBuffer[strlen(szBuffer)-1] = '\0';
        OUTTEXT(pSession, "%s\r\n", szBuffer);
        nCount++;

        if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, 0, 0);
            if (ret == 0) 
            {
                fclose(fp);
                return CLIERR_OK;
            }
            if (ret == 2) bMore = FALSE;
        }
    }

    fclose(fp);
    return CLIERR_OK;
}

int cmdShowAlarmLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	IF4_ALARM_FRAME	*pEvent = NULL;
    TIMESTAMP   tmStart;
    struct  stat file_info;
    char        szPath[128];
	char		szGUID[32];
	FILE		*fp;
	char		*pszBuffer, *p;
	int			nSeek, i, nCount=0, ret;
	BOOL		bMore = TRUE;

	GetTimestamp(&tmStart, NULL);
	if ((argc != 0) && (argc != 3))
	{
		OUTTEXT(pSession, "Need more parameter.\r\n");
		return CLIERR_OK;
	}
	else if (argc == 3)
	{
		tmStart.year = atoi(argv[0]);
		tmStart.mon	 = atoi(argv[1]);
		tmStart.day	 = atoi(argv[2]);
	}

    sprintf(szPath, "/app/event/ALM%04d%02d%02d",
					tmStart.year, tmStart.mon, tmStart.day);
	stat(szPath, &file_info);

	pszBuffer = (char *)MALLOC((int)file_info.st_size);
	if (pszBuffer == NULL)
		return CLIERR_OK;

	fp = fopen(szPath, "rb");
	if (fp != NULL)
	{
		fread(pszBuffer, 1, (int)file_info.st_size, fp);
		fclose(fp);
	}

	p = pszBuffer;
	for(i=0; i<(int)file_info.st_size; i+=nSeek)
	{
		nSeek = 1;
		pEvent = (IF4_ALARM_FRAME *)(p+i);
		if (pEvent->hdr.soh != IF4_SOH)
			continue;
		if (pEvent->hdr.svc != IF4_SVC_ALARM)
			continue;
	
		nSeek = pEvent->hdr.len + sizeof(IF4_HEADER) + sizeof(IF4_TAIL);
		EUI64ToStr((EUI64 *)&pEvent->ah.srcID, szGUID);
		OUTTEXT(pSession, "%04d/%02d/%02d %02d:%02d:%02d Alarm from %s\r\n",
						tmStart.year, tmStart.mon, tmStart.day,
						pEvent->ah.tmAlarm.hour,
						pEvent->ah.tmAlarm.min,
						pEvent->ah.tmAlarm.sec,
						szGUID);
		nCount++;
		if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
		{
			ret = More(pSession, 0, 0);
			if (ret == 0) return CLIERR_OK;
			if (ret == 2) bMore = FALSE;
		}
	}

	FREE(pszBuffer);
	return CLIERR_OK;
}

int cmdShowUpgradeLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    char    szPath[64];
    char    szBuffer[256];
    time_t  now;
    struct  tm  when;
    FILE    *fp;
    int     nCount = 0, ret;
    BOOL    bMore = TRUE;
    
    if ((argc != 0) && (argc != 3))
    {
        OUTTEXT(pSession, "Need more parameter.\xd\xa");
        return CLIERR_OK; 
    }   
    
    if (argc == 3)
    {
         sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, UPGRADE_LOG_FILE, 
                 atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }
    else 
    {
        time(&now);
        when = *localtime(&now);
        sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, UPGRADE_LOG_FILE,
            when.tm_year+1900, when.tm_mon+1, when.tm_mday);
    }       
    
    fp = fopen(szPath, "rt");
    if (fp == NULL)
    {
        OUTTEXT(pSession, "No data.\xd\xa");
        return CLIERR_OK;
    }

    while(fgets(szBuffer, 255, fp))
    {
        szBuffer[strlen(szBuffer)-1] = '\0';
        OUTTEXT(pSession, "%s\r\n", szBuffer);
        nCount++;

        if (bMore && ((nCount % DISPLAY_ROW_COUNT) == 0))
        {
            ret = More(pSession, 0, 0);
            if (ret == 0) 
            {
                fclose(fp);
                return CLIERR_OK;
            }
            if (ret == 2) bMore = FALSE;
        }
    }

    fclose(fp);
    return CLIERR_OK;
}

int cmdShowApnList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke;
	char		*p;
	int			i;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "120.3"))		// cmdGetApnList
		return CLIERR_OK;

	OUTTEXT(pSession, "==========================================\r\n");
	OUTTEXT(pSession, " NUM Access Point Name\r\n");
	OUTTEXT(pSession, "==========================================\r\n");
	pInvoke = (IF4Invoke *)invoke.GetHandle();
	for(i=0; i<pInvoke->nResultCount; i++)
	{
		p = (char *)pInvoke->pResult[i]->stream.p;
		OUTTEXT(pSession, " %3d %s\r\n", i+1, p);
	}
	OUTTEXT(pSession, "==========================================\r\n");
    return CLIERR_OK;
}

int cmdShowApnInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke;
	char		szBuffer[256], *p, *tag;

	invoke.AddParam("1.11", (char *)argv[0]);						// stringEntry
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "120.4"))		// cmdGetApnOption
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	p = (char *)pInvoke->pResult[0]->stream.p;
	OUTTEXT(pSession, "APN(Access Point Name) : %s\r\n", argv[0]);
	OUTTEXT(pSession, "\r\n");

	for(;p && *p;)
	{
		// 라인 잘라 오기
		memcpy(szBuffer, p, 255);
		szBuffer[255] = '\0';
		tag = strchr(szBuffer, '\n');
		if (tag == NULL) tag = strchr(szBuffer, '\r');
		if (tag != NULL) *tag = '\0';
		
		// 비고 또는 #으로 시작하는 라인은 제거
		if ((szBuffer[0] != '\0') && (szBuffer[0] != '#'))
			OUTTEXT(pSession, "%s\r\n", szBuffer);

		// 다음 라인을 구한다.	
		tag = strchr(p, '\n');
		if (tag == NULL)
		{
			tag = strchr(p, '\r');
			if (tag != NULL) tag++;
		}
		else tag++;
		p = tag;	
	}
    return CLIERR_OK;
}

int cmdShowEventConfigure(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	EVTCFGENTRY	    *pEvent;
	BOOL			bMore = TRUE;
    int             i, nCount, ret;
    char            *severity;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.28"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

	OUTTEXT(pSession, "===============================================================================================\xd\xa");
	OUTTEXT(pSession, " NUM OBJECT ID        EVENT NAME                       SEVERITY   NOTIFY\xd\xa");
	OUTTEXT(pSession, "===============================================================================================\xd\xa");

	for(i=0; i<nCount; i++)
	{
		pEvent = (EVTCFGENTRY *)pInvoke->pResult[i]->stream.p;

        switch(pEvent->nClass) {
            case EVTCLASS_CRITICAL : severity = const_cast<char *>("Critical"); break;
            case EVTCLASS_MAJOR : severity = const_cast<char *>("Major"); break;
            case EVTCLASS_MINOR : severity = const_cast<char *>("Minor"); break;
            case EVTCLASS_NORMAL : severity = const_cast<char *>("Normal"); break;
            case EVTCLASS_INFO : severity = const_cast<char *>("Info"); break;
            default : severity = const_cast<char *>("Unknown"); break;
        }
    
        OUTTEXT(pSession, " %3d %-16s %-32s %-8s   %-3s\xd\xa", 
                    i+1,
                    pEvent->szOid, 
                    pEvent->szEvent, 
                    severity,
                    pEvent->bNotify ? "T" : "F");

		if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
		{
			ret = More(pSession, nCount, i+1);
			if (ret == 0) return CLIERR_OK;
			if (ret == 2) bMore = FALSE;
		}
	}
	OUTTEXT(pSession, "===============================================================================================\xd\xa");
	return CLIERR_OK;
}

int cmdShowPatch(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	PATCHENTRY	    *pPatch;
	BOOL			bMore = TRUE;
    int             i, nCount, ret;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.30"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

	OUTTEXT(pSession, "===============================================================================================\xd\xa");
	OUTTEXT(pSession, " ID   ISSUE NUM  DESCRIPTION                                                            STATE\xd\xa");
	OUTTEXT(pSession, "===============================================================================================\xd\xa");

	for(i=0; i<nCount; i++)
	{
		pPatch = (PATCHENTRY *)pInvoke->pResult[i]->stream.p;

        OUTTEXT(pSession, " %04d %09d  %-70s %-10s\xd\xa", 
                    pPatch->patchID,
                    pPatch->patchIssueNumber, 
                    pPatch->patchDescr, 
                    pPatch->patchState ? "Enable" : "Disable");

		if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
		{
			ret = More(pSession, nCount, i+1);
			if (ret == 0) return CLIERR_OK;
			if (ret == 2) bMore = FALSE;
		}
	}
	OUTTEXT(pSession, "===============================================================================================\xd\xa");
	return CLIERR_OK;
}

static int _cmpffd(const void *p1, const void *p2)
{
	FFDENTRY	*m1, *m2;
    int res;

    m1 = (FFDENTRY *)(*(MIBValue * const *) p1)->stream.p;
    m2 = (FFDENTRY *)(*(MIBValue * const *) p2)->stream.p;
	
    if((res = strcmp(m1->ffdModel, m2->ffdModel))) return res;
    if(((res = m1->ffdPowerType - m2->ffdPowerType))) return res;
    return memcmp(&m1->ffdID, &m2->ffdID, sizeof(EUI64));  
}

int cmdShowFFDList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	FFDENTRY	    *pFFD;
	BOOL			bMore = TRUE;
    BOOL            bSupport = FALSE;
    int             i, nCount, ret;
    int             major=0, minor=0, build=0;
	char			szGUID[MAX_GUID_STRING+1];
    char            szTime[30];
    char            hw[16], fw[16];
    

    if(argc > 0) {
        invoke.AddParam("1.11", argv[0]);
    }

    if(argc > 1) {
        major = minor = 0;
        sscanf(argv[1],"%d.%d", &major, &minor);
        invoke.AddParam("1.5",(WORD) (((major & 0xff) << 8) + (minor & 0xff)));
    }
    if(argc > 2) {
        build = (int) strtol(argv[2],(char **)NULL,10);
        invoke.AddParam("1.5",(WORD)build);
    }

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.38"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;

    qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmpffd);

	OUTTEXT(pSession, "===================================================================================================\xd\xa");
	OUTTEXT(pSession, " NUM SENSOR ID        MODEL         H/W   F/W BLD PM PS MC CC AF TF SADV SCBV SVCC SC LAST UPDATE\xd\xa");
	OUTTEXT(pSession, "===================================================================================================\xd\xa");

	for(i=0; i<nCount; i++)
	{
		pFFD = (FFDENTRY *)pInvoke->pResult[i]->stream.p;

		EUI64ToStr(&pFFD->ffdID, szGUID);
        if(memcmp(&pFFD->ffdLastPush, &pFFD->ffdLastPoll, sizeof(TIMESTAMP)) > 0) {
		    IF4API_TIMESTAMPTOSTR(szTime, &pFFD->ffdLastPush);
        } else {
		    IF4API_TIMESTAMPTOSTR(szTime, &pFFD->ffdLastPoll);
        }
        szTime[strlen(szTime) - 3] = 0x00;

		sprintf(hw, "%2d.%-2d", pFFD->ffdHwVersion >> 8, pFFD->ffdHwVersion & 0xff);
		sprintf(fw, "%2d.%-2d", pFFD->ffdFwVersion >> 8, pFFD->ffdFwVersion & 0xff);
        build = pFFD->ffdFwBuild;
        pFFD->ffdModel[12] = 0x00;

        bSupport = FALSE;
        /*-- Issue #1894 : Sensor Version 2.2 b20 이상에서 지원 --*/
        if(pFFD->ffdFwVersion > 0x202) bSupport = TRUE;
        else if(pFFD->ffdFwVersion == 0x201 && pFFD->ffdFwBuild >= 20) bSupport = TRUE;
        else if(pFFD->ffdFwVersion == 0x202 && pFFD->ffdFwBuild >= 20) bSupport = TRUE;

		OUTTEXT(pSession, " %3d %-16s %-12s %-4s %-4s %2d  %c  %c %2d %2d  %c  %c",
					i+1,
					szGUID,
					pFFD->ffdModel,
                    hw,
                    fw,
                    build,
                    bSupport ? (pFFD->ffdPermitMode ? 'A' : 'M') : '*',
                    bSupport ? (pFFD->ffdPermitState ? 'A' : 'D') : '*',
                    bSupport ? pFFD->ffdMaxChildTbl : 0,
                    bSupport ? pFFD->ffdCurrentChild : 0,
                    pFFD->ffdAlarmFlag ? 'A' : ' ',
                    pFFD->ffdTestFlag ? 'T' : ' ');

        if(pFFD->ffdPowerType == 0x04) {    // Solar Type
		    OUTTEXT(pSession, " %.2f %.2f %.2f %2s %s\xd\xa",
                    (double)(pFFD->ffdSolarAdVolt*2/100) / 100.0, 
                    (double)(pFFD->ffdSolarChgBattVolt/100) / 100.0, 
                    (double)(pFFD->ffdSolarBoardVolt/100) / 100.0, 
                    pFFD->ffdSolarBattChgState ? "Hi" : "Lo",
                    szTime + 5);
        } else {
            OUTTEXT(pSession, "                   %s\xd\xa", szTime + 5);
        }

		if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
		{
			ret = More(pSession, nCount, i+1);
			if (ret == 0) return CLIERR_OK;
			if (ret == 2) bMore = FALSE;
		}
	}
	OUTTEXT(pSession, "===================================================================================================\xd\xa");
	return CLIERR_OK;
}

int cmdShowFirmwareList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	CIF4Invoke      powerControlInvoke("127.0.0.1", 8003, 2);
	IF4Invoke		*pInvoke;
	FWENTRY	        *pFwEntry;
	BOOL			bMore = TRUE;
    int             i, nCount, ret, idx=0;
    int             build=0;
    char            hw[16], fw[16];
    UINT            powerType = 0;

	OUTTEXT(pSession, "===================================================================\xd\xa");
	OUTTEXT(pSession, " NUM TYPE               CORE            H/W   F/W  BUILD \xd\xa");
	OUTTEXT(pSession, "===================================================================\xd\xa");

	if (CLIAPI_Command(pSession, invoke.GetHandle(), "100.33"))
    {
	    pInvoke = (IF4Invoke *)invoke.GetHandle();
	    nCount  = pInvoke->nResultCount;

	    for(i=0; i<nCount; i++, idx++)
	    {
		    pFwEntry = (FWENTRY *)pInvoke->pResult[i]->stream.p;
    
		    sprintf(hw, "%2d.%-2d", pFwEntry->fwHardware >> 8, pFwEntry->fwHardware & 0xff);
		    sprintf(fw, "%2d.%-2d", pFwEntry->fwSoftware >> 8, pFwEntry->fwSoftware & 0xff);
            build = pFwEntry->fwBuild;

		    OUTTEXT(pSession, " %3d %-18s %-14s %-4s %-4s %5d\r\n",
					idx+1,
					MSGAPI_FirmwareType(pFwEntry->fwType),
					pFwEntry->fwCore,
                    hw,
                    fw,
                    build);

		    if (bMore && (((idx+1) % DISPLAY_ROW_COUNT) == 0))
		    {
			    ret = More(pSession, nCount, idx+1);
			    if (ret == 0) return CLIERR_OK;
			    if (ret == 2) bMore = FALSE;
		    }
	    }
    }

	GetFileValue("/app/conf/powertype.mcu", (int *)&powerType);
	if ((!(powerType & NZC_POWER_TYPE_LINE) && (powerType & NZC_POWER_TYPE_SOLAR)) &&
            CLIAPI_Command(pSession, powerControlInvoke.GetHandle(), "100.33"))
    {
	    pInvoke = (IF4Invoke *)powerControlInvoke.GetHandle();
	    nCount  = pInvoke->nResultCount;

	    for(i=0; i<nCount; i++, idx++)
	    {
		    pFwEntry = (FWENTRY *)pInvoke->pResult[i]->stream.p;
    
		    sprintf(hw, "%2d.%-2d", pFwEntry->fwHardware >> 8, pFwEntry->fwHardware & 0xff);
		    sprintf(fw, "%2d.%-2d", pFwEntry->fwSoftware >> 8, pFwEntry->fwSoftware & 0xff);
            build = pFwEntry->fwBuild;

		    OUTTEXT(pSession, " %3d %-18s %-14s %-4s %-4s %5d\r\n",
					idx+1,
					MSGAPI_FirmwareType(pFwEntry->fwType),
					pFwEntry->fwCore,
                    hw,
                    fw,
                    build);

		    if (bMore && (((idx+1) % DISPLAY_ROW_COUNT) == 0))
		    {
			    ret = More(pSession, nCount, idx+1);
			    if (ret == 0) return CLIERR_OK;
			    if (ret == 2) bMore = FALSE;
		    }
	    }
    }

	OUTTEXT(pSession, "===================================================================\xd\xa");
	return CLIERR_OK;
}

int cmdShowFirmwareBuild(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	char	szLine[1024];
	FILE	*fp;

    memset(szLine,0,sizeof(szLine));

	OUTTEXT(pSession, "===================================================================\xd\xa");
    if((fp = fopen("/app/conf/application.info", "r")) != NULL) {
		while(fgets(szLine, sizeof(szLine)-1, fp) > 0)
			OUTTEXT(pSession, "%s\xd", szLine);
		fclose(fp);	
	}
	OUTTEXT(pSession, "===================================================================\xd\xa");

	return CLIERR_OK;
}

void _showTransactionInfoHeader(CLISESSION *pSession)
{
	OUTTEXT(pSession, "===================================================================================================\xd\xa");
	OUTTEXT(pSession, "  TRID SENSOR ID        COMMAND OP DAY  NICE TRY QUE CTIME       LTIME       S ERROR\xd\xa");
	OUTTEXT(pSession, "===================================================================================================\xd\xa");
}

void _showTransactionInfoFooter(CLISESSION *pSession)
{
	OUTTEXT(pSession, "===================================================================================================\xd\xa");
}

void _showTransactionInfo(CLISESSION *pSession, TRINFOENTRY * pTrInfo)
{
	char			szGUID[MAX_GUID_STRING+1];
    char            szOID[30];
	char			szCTime[30], szLTime[30];
    char            szOpt[3]={0,};
    char            *eStr, eStrArr[21];

    memset(eStrArr, 0, sizeof(eStrArr));

    EUI64ToStr(&pTrInfo->triTargetID, szGUID);
    VARAPI_OidToString(&pTrInfo->triCommand, szOID);
        
    IF4API_TIMESTAMPTOSTR(szCTime, &pTrInfo->triCreateTime); szCTime[strlen(szCTime) - 3] = 0x00;
    IF4API_TIMESTAMPTOSTR(szLTime, &pTrInfo->triLastTime); szLTime[strlen(szLTime) - 3] = 0x00;

    if(pTrInfo->triOption & ASYNC_OPT_RESULT_DATA_SAVE) szOpt[0] = 'S';
    else if(pTrInfo->triOption & ASYNC_OPT_RETURN_CODE_SAVE) szOpt[0] = 's';
    else szOpt[0] = ' ';

    if(pTrInfo->triOption & ASYNC_OPT_RESULT_DATA_EVT) szOpt[1] = 'E';
    else if(pTrInfo->triOption & ASYNC_OPT_RETURN_CODE_EVT) szOpt[1] = 'e';
    else szOpt[1] = ' ';

    eStr = CollapseString(IF4API_GetErrorMessage(pTrInfo->triError), (char *) eStrArr, sizeof(eStrArr));

    OUTTEXT(pSession, " %5u %-16s %-7s %2s %3d %2d/%-2d %d/%d %3d %s %s %c %s\xd\xa",
        pTrInfo->triID,
        szGUID,
        szOID,
        szOpt,
        pTrInfo->triDay,
        pTrInfo->triCurNice, pTrInfo->triInitNice,
        pTrInfo->triCurTry, pTrInfo->triInitTry,
        pTrInfo->triQueue,
        szCTime + 5,
        szLTime + 5,
        pTrInfo->triState & TR_STATE_WAIT ? 'W' : (
            pTrInfo->triState & TR_STATE_QUEUE ? 'Q' : (
            pTrInfo->triState & TR_STATE_RUN ? 'R' : (
            pTrInfo->triState & TR_STATE_DELETE ? 'D' : (
            pTrInfo->triState & TR_STATE_TERMINATE ? 'T' : '?')))),
        eStr);
}

static int _cmptrinfo(const void *p1, const void *p2)
{
	TRINFOENTRY	*m1, *m2;
    int res;

    m1 = (TRINFOENTRY *)(*(MIBValue * const *) p1)->stream.p;
    m2 = (TRINFOENTRY *)(*(MIBValue * const *) p2)->stream.p;
	
    res = memcmp(&m1->triTargetID, &m2->triTargetID, sizeof(EUI64));  if(res) return res;
    return m1->triID - m2->triID; 
}

int cmdShowTransactionList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	TRINFOENTRY	    *pTrInfo;

    EUI64           id;
    char            parser[256];
	char			szGUID[MAX_GUID_STRING+1];
    char            szOID[30];
	char			szCTime[30], szLTime[30];
    char            szOpt[3]={0,};
    char            *eStr, eStrArr[17];
    char            *idxPtr;
    int             major=0, minor=0, build=0;
    BYTE            nOption = TR_SHOW_WAITING_TRANSACTION | TR_SHOW_QUEUEING_TRANSACTION | TR_SHOW_RUNNING_TRANSACTION;
	int				i, nCount, ret;
	BOOL			bMore = TRUE;

    if(argc>0) {
        if(!strcasecmp(argv[0],"wait")) nOption = TR_SHOW_WAITING_TRANSACTION;
        else if(!strcasecmp(argv[0],"queue")) nOption = TR_SHOW_QUEUEING_TRANSACTION;
        else if(!strcasecmp(argv[0],"run")) nOption = TR_SHOW_RUNNING_TRANSACTION;
        else if(!strcasecmp(argv[0],"w+q+r")) nOption = 
            TR_SHOW_WAITING_TRANSACTION | TR_SHOW_QUEUEING_TRANSACTION | TR_SHOW_RUNNING_TRANSACTION;
        else if(!strcasecmp(argv[0],"terminate")) nOption = TR_SHOW_TERMINATE_TRANSACTION;
        else if(!strcasecmp(argv[0],"all")) nOption = TR_SHOW_ALL_TRANSACTION;
        else nOption = (BYTE)strtol(argv[0],(char **)NULL,10);
        if(!nOption) nOption = TR_SHOW_WAITING_TRANSACTION | TR_SHOW_QUEUEING_TRANSACTION | TR_SHOW_RUNNING_TRANSACTION;
        invoke.AddParam("1.4", nOption);
    }
    if(argc>1) {
        if(check_id(NULL, argv[1])) { // EUI64 ID
  	        StrToEUI64(argv[1], &id);
            invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        }else {
            strcpy(parser, argv[1]);
            if((idxPtr=strchr(parser,'$'))) {
                *idxPtr = 0x00;
            }
            invoke.AddParam("1.11",parser);
        }
    }
    if(argc > 2) {
        major = minor = 0;
        sscanf(argv[2],"%d.%d", &major, &minor);
        invoke.AddParam("1.5",(WORD) (((major & 0xff) << 8) + (minor & 0xff)));
    }
    if(argc > 3) {
        build = (int) strtol(argv[3],(char **)NULL,10);
        invoke.AddParam("1.5",(WORD)build);
    }

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.41"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

    qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmptrinfo);
	OUTTEXT(pSession, "===================================================================================================\xd\xa");
	OUTTEXT(pSession, " NUM SENSOR ID        COMMAND TRID  OP DAY  NICE TRY QUE CTIME       LTIME       S ERROR\xd\xa");
	OUTTEXT(pSession, "===================================================================================================\xd\xa");

    memset(&id, 0, sizeof(EUI64));
	for(i=0; i<nCount; i++)
	{
		pTrInfo = (TRINFOENTRY *)pInvoke->pResult[i]->stream.p;
        if(memcmp(&id, &pTrInfo->triTargetID, sizeof(EUI64))) {
            memcpy(&id, &pTrInfo->triTargetID, sizeof(EUI64));
		    EUI64ToStr(&pTrInfo->triTargetID, szGUID);
        } else {
            szGUID[0] = 0x00;
        }
		VARAPI_OidToString(&pTrInfo->triCommand, szOID);
        
		IF4API_TIMESTAMPTOSTR(szCTime, &pTrInfo->triCreateTime); szCTime[strlen(szCTime) - 3] = 0x00;
		IF4API_TIMESTAMPTOSTR(szLTime, &pTrInfo->triLastTime); szLTime[strlen(szLTime) - 3] = 0x00;

        if(pTrInfo->triOption & ASYNC_OPT_RESULT_DATA_SAVE) szOpt[0] = 'S';
        else if(pTrInfo->triOption & ASYNC_OPT_RETURN_CODE_SAVE) szOpt[0] = 's';
        else szOpt[0] = ' ';

        if(pTrInfo->triOption & ASYNC_OPT_RESULT_DATA_EVT) szOpt[1] = 'E';
        else if(pTrInfo->triOption & ASYNC_OPT_RETURN_CODE_EVT) szOpt[1] = 'e';
        else szOpt[1] = ' ';

        memset(eStrArr, 0, sizeof(eStrArr));
        eStr = CollapseString(IF4API_GetErrorMessage(pTrInfo->triError), (char *) eStrArr, sizeof(eStrArr));

		OUTTEXT(pSession, " %3d %-16s %-7s %-5u %2s %3d %2d/%-2d %d/%d %3d %s %s %c %s\xd\xa",
					i+1,
					szGUID,
					szOID,
                    pTrInfo->triID,
                    szOpt,
                    pTrInfo->triDay,
                    pTrInfo->triCurNice, pTrInfo->triInitNice,
                    pTrInfo->triCurTry, pTrInfo->triInitTry,
                    pTrInfo->triQueue,
					szCTime + 5,
					szLTime + 5,
					pTrInfo->triState & TR_STATE_WAIT ? 'W' : (
                        pTrInfo->triState & TR_STATE_QUEUE ? 'Q' : (
                        pTrInfo->triState & TR_STATE_RUN ? 'R' : (
                        pTrInfo->triState & TR_STATE_DELETE ? 'D' : (
                        pTrInfo->triState & TR_STATE_TERMINATE ? 'T' : '?')))),
                    eStr);

		if (bMore && (((i+1) % DISPLAY_ROW_COUNT) == 0))
		{
			ret = More(pSession, nCount, i+1);
			if (ret == 0) return CLIERR_OK;
			if (ret == 2) bMore = FALSE;
		}
	}
	OUTTEXT(pSession, "===================================================================================================\xd\xa");
	return CLIERR_OK;
}

int cmdShowTransactionInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	TRINFOENTRY	    *pTrInfo;
    TRHISTORYENTRY  *pTrHistory;
    WORD            trId;
    int             idx=0, nCount;
    BYTE            nOption = TR_GET_OPT_ALL;
    EUI64           id;
	char			szCTime[30];
    int             nParamCnt=0, nHistoryCnt = 0;
	int				i;

    trId = (WORD) strtol(argv[0],(char **)NULL,10);
    invoke.AddParam("1.5", trId);
    invoke.AddParam("1.4", nOption);

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.42"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

    idx = 0;
    memset(&id, 0, sizeof(EUI64));
    pTrInfo = (TRINFOENTRY *)pInvoke->pResult[idx]->stream.p;   idx++;

    _showTransactionInfoHeader(pSession);
    _showTransactionInfo(pSession, pTrInfo);

	OUTTEXT(pSession, "---------------------------------------------------------------------------------------------------\xd\xa");
    nParamCnt = pInvoke->pResult[idx]->stream.u16;              idx++;
    PrintOutMIBValue(pSession, pInvoke->pResult[idx], nParamCnt);            idx+=nParamCnt;
	OUTTEXT(pSession, "---------------------------------------------------------------------------------------------------\xd\xa");
    nParamCnt = pInvoke->pResult[idx]->stream.u16;              idx++;
    PrintOutMIBValue(pSession, pInvoke->pResult[idx], nParamCnt);            idx+=nParamCnt;

	OUTTEXT(pSession, "---------------------------------------------------------------------------------------------------\xd\xa");
    nHistoryCnt = pInvoke->pResult[idx]->stream.u16;            idx++;
    for(i=0;i<nHistoryCnt;i++) 
    {
        pTrHistory = (TRHISTORYENTRY *)pInvoke->pResult[idx]->stream.p; idx++;
		IF4API_TIMESTAMPTOSTR(szCTime, &pTrHistory->trhTime); 
		OUTTEXT(pSession, " %5d %s %c %s\xd\xa",
                    i+1,
					szCTime,
					pTrHistory->trhState & TR_STATE_WAIT ? 'W' : (
                        pTrHistory->trhState & TR_STATE_QUEUE ? 'Q' : (
                        pTrHistory->trhState & TR_STATE_RUN ? 'R' : (
                        pTrHistory->trhState & TR_STATE_DELETE ? 'D' : (
                        pTrHistory->trhState & TR_STATE_TERMINATE ? 'T' : '?')))),
                    IF4API_GetErrorMessage(pTrHistory->trhError));
    }

    _showTransactionInfoFooter(pSession);
	return CLIERR_OK;
}

extern int _showOndemand(CLISESSION *pSession, SENSORINFOENTRYNEW *pSensor, IF4Invoke *pInvoke, int nStartIdx, int nEndIdx);
int cmdShowTransactionResult(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	TRINFOENTRY	    *pTrInfo;
    WORD            trId;
    int             idx=0, nCount;
    BYTE            nOption = TR_GET_OPT_TRINFO | TR_GET_OPT_RESPONSE;
    char            szOID[30];
    int             nParamCnt=0; 

    trId = (WORD) strtol(argv[0],(char **)NULL,10);
    invoke.AddParam("1.5", trId);
    invoke.AddParam("1.4", nOption);

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.42"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

    idx = 0;
    pTrInfo = (TRINFOENTRY *)pInvoke->pResult[idx]->stream.p;   idx++;
	VARAPI_OidToString(&pTrInfo->triCommand, szOID);

    _showTransactionInfoHeader(pSession);
    _showTransactionInfo(pSession, pTrInfo);

    idx ++; // skip parameter

	OUTTEXT(pSession, "---------------------------------------------------------------------------------------------------\xd\xa");
    nParamCnt = pInvoke->pResult[idx]->stream.u16;              idx++;

    if(nParamCnt > 0) {
        if(!strcmp(szOID,"102.28")) {               // show sensor amr
            _showSensorAmrData(pSession, pInvoke, idx, idx + nParamCnt);
        }else if(!strcmp(szOID,"102.23")) {         // show sensor rominfo
            if(nParamCnt < 4) {
                PrintOutMIBValue(pSession, pInvoke->pResult[idx], nParamCnt);
            } else {
                _showSensorRomInfo(pSession, pInvoke, idx, idx + nParamCnt);
            }
        }else if(!strcmp(szOID,"102.44")) {         // show sensor event
            _showSensorEvent(pSession, pInvoke, idx, idx + nParamCnt);
        }else if(!strcmp(szOID,"102.45")) {         // show sensor battery
            _showSensorBattery(pSession, pInvoke, idx, idx + nParamCnt);
        }else if(!strcmp(szOID,"104.6")) {          // ondemand metering
	        SENSORINFOENTRYNEW	*pSensor=NULL;
            CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);

            invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&pTrInfo->triTargetID, sizeof(EUI64));
            if (CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27")) {
                pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;
                _showOndemand(pSession, pSensor, pInvoke, idx, idx + nParamCnt);
            } else {
                PrintOutMIBValue(pSession, pInvoke->pResult[idx], nParamCnt);
            }
        }else {
            PrintOutMIBValue(pSession, pInvoke->pResult[idx], nParamCnt);
        }
    }else {
	    OUTTEXT(pSession, "Empty result\xd\xa");
    }

    _showTransactionInfoFooter(pSession);
	return CLIERR_OK;
}

const char *GetOtaTypeString(int nType)
{
	switch(nType) {
	  case OTA_TYPE_CONCENTRATOR :	return "CONCENTRATOR";
	  case OTA_TYPE_SENSOR :		return "SENSOR";
	  case OTA_TYPE_COORDINATOR :	return "COORDINATOR";
	}
	return "Unknown";
}

const char *GetOtaInstallTypeString(int nType)
{
	switch(nType) {
	  case OTA_INSTALL_AUTO :		return "AUTO";
	  case OTA_INSTALL_REINSTALL :	return "REINSTALL";
	  case OTA_INSTALL_MATCH :		return "MATCH";
	}
	return "Unknown";
}

const char *GetOtaTransferTypeString(int nType)
{
	switch(nType) {
	  case OTA_TRANSFER_AUTO :		return "AUTO";
	  case OTA_TRANSFER_MULTICAST :	return "MULTICAST";
	  case OTA_TRANSFER_UNICAST :	return "UNICAST";
	}
	return "Unknown";
}

const char *GetStepTypeString(UINT nStep)
{
    if (nStep & OTA_STEP_SCAN)
		return "SCAN";
	else if (nStep & OTA_STEP_INSTALL)
		return "INSTALL";
	else if (nStep & OTA_STEP_VERIFY)
		return "VERIFY";
	else if (nStep & OTA_STEP_SEND)
		return "SEND";
	else if (nStep & OTA_STEP_INVENTORY)
		return "INVENTORY";
	return "";
}

const char *GetOtaErrorString(int nError)
{
	switch(nError) {
	  case OTAERR_NOERROR :					return "";
	  case OTAERR_OPEN_ERROR :				return "OPEN ERROR";
	  case OTAERR_CONNECT_ERROR :			return "CONNECT ERROR";
	  case OTAERR_INVENTORY_SCAN_ERROR :	return "INVENTORY SCAN ERROR";
	  case OTAERR_FIRMWARE_NOT_FOUND :		return "FIRMWARE NOT FOUND";
	  case OTAERR_SEND_ERROR :				return "SEND ERROR";
	  case OTAERR_VERIFY_ERROR :			return "VERIFY ERROR";	
	  case OTAERR_INSTALL_ERROR	:			return "INSTALL ERROR";
	  case OTAERR_USER_CANCEL :				return "USER CANCEL";
	  case OTAERR_DIFFERENT_MODEL_ERROR :	return "MODEL ERROR";
	  case OTAERR_MATCH_VERSION_ERROR :		return "VERSION ERROR";
	  case OTAERR_VERSION_ERROR :			return "VERSION ERROR";
	  case OTAERR_MODEL_ERROR :				return "MODEL ERROR";	
	  case OTAERR_MEMORY_ERROR :			return "MEMORY ERROR";
	  case OTAERR_COORDINATOR_ERROR :		return "COORDINATOR ERROR";
	}
	return "UNKOWN ERROR";
}

void GetTimeString(char *pszBuffer, int nSecond)
{
	if (nSecond < 60)
		 sprintf(pszBuffer, "0:%02d", nSecond);
	else sprintf(pszBuffer, "%0d:%02d", nSecond/60, nSecond%60);
}

void GetLongTimeString(char *pszBuffer, int nSecond)
{
	if (nSecond >= 3600)
		 sprintf(pszBuffer, "%0d:%02d:%02d", nSecond/3600, (nSecond%3600)/60, nSecond%60);
	else if (nSecond >= 60)
		 sprintf(pszBuffer, "%0d:%02d", nSecond/60, nSecond%60);
	else sprintf(pszBuffer, "0:%02d", nSecond);
}

int cmdShowUpgradeList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	DISTRIBUTIONENTRY	*pEntry;
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke;
	char		szTime[20];
	int			i, nCount;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "198.16"))
		return CLIERR_OK;

	pInvoke = invoke.GetHandle();
	nCount = pInvoke->nResultCount;
	
	OUTTEXT(pSession, "==================================================================================================================\r\n");
	OUTTEXT(pSession, "TRIGGER ID       TYPE          MODEL                TRANSFER  INSTALL   STEP H/W S/W BUILD NODE STATUS ELAPSE\r\n");
	OUTTEXT(pSession, "==================================================================================================================\r\n");

	for(i=0; i<nCount; i++)
	{
		pEntry = (DISTRIBUTIONENTRY *)pInvoke->pResult[i]->stream.p;
		GetLongTimeString(szTime, pEntry->nElapse);

		OUTTEXT(pSession, "%-16s %-13s %-20s %-9s %-9s 0x%02X %0d.%0d %0d.%0d %5d %4d %-6s %s\r\n",
					pEntry->szTriggerId,
					GetOtaTypeString(pEntry->nType),
					pEntry->szModel,
					GetOtaTransferTypeString(pEntry->nTransferType),
					GetOtaInstallTypeString(pEntry->nInstallType),
					pEntry->nOtaStep,
					pEntry->nNewHw >> 8, pEntry->nNewHw & 0xff,
					pEntry->nNewSw >> 8, pEntry->nNewSw & 0xff,
					pEntry->nNewBuild,
					pEntry->nCount,
					pEntry->nState == 1 ? "ACTIVE" : "",
					szTime);
	}

	OUTTEXT(pSession, "==================================================================================================================\r\n");
	return CLIERR_OK;
}

int cmdShowUpgradeInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	OTANODEENTRY	*pNode;
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke;
	char		szGUID[17];
	char		szElapse[20];
	int			i, nCount;

	invoke.AddParamFormat("1.11", VARSMI_STRING, (void *)argv[0], strlen(argv[0]));
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "198.17"))
		return CLIERR_OK;

	pInvoke = invoke.GetHandle();
	nCount = pInvoke->nResultCount;
	
	OUTTEXT(pSession, "==============================================================================================\r\n");
	OUTTEXT(pSession, "SENSOR ID        MODEL                H/W S/W BUILD HOP LAST PASSED      SENT ELAPSE STATUS\r\n");
	OUTTEXT(pSession, "==============================================================================================\r\n");

	for(i=0; i<nCount; i++)
	{
		pNode = (OTANODEENTRY *)pInvoke->pResult[i]->stream.p;
		EUI64ToStr(&pNode->id, szGUID);
		GetTimeString(szElapse, (int)pNode->nElapse);

		if ((i > 0) && ((i % 5) == 0))
			OUTTEXT(pSession, "---------------------------------------------------------------------------------------------\r\n");

		OUTTEXT(pSession, "%s %-20s %0d.%0d %0d.%0d %5d  %2d 0x%02X %-9s %6d %6s %s\r\n",
					szGUID,
					pNode->szModel,
					pNode->hw >> 8, pNode->hw & 0xff,
					pNode->sw >> 8, pNode->sw & 0xff,
					pNode->build,
					pNode->hops,
					pNode->nOtaMask, GetStepTypeString(pNode->nOtaMask),
					pNode->nCurrent,
					szElapse,
					((pNode->nError == 0) && (pNode->nState == 1)) ? "ACTIVE" : GetOtaErrorString(pNode->nError));
	}

	OUTTEXT(pSession, "==============================================================================================\r\n");
	return CLIERR_OK;
}

int cmdShowUpgradeStatus(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
	GMTTIMESTAMP	*pTime;
	char			szBuffer[64];
	UINT			nSecond;
    UINT            nStep;

	invoke.AddParamFormat("1.11", VARSMI_STRING, (void *)argv[0], strlen(argv[0]));
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "198.14"))
		return CLIERR_OK;

	pInvoke = invoke.GetHandle();
	OUTTEXT(pSession, "Trigger ID = %s\r\n", (char *)pInvoke->pResult[0]->stream.p);
	pTime = (GMTTIMESTAMP *)pInvoke->pResult[1]->stream.p;
	sprintf(szBuffer, "%04d/%02d/%02d %02d:%02d:%02d",
			pTime->year, pTime->mon, pTime->day, pTime->hour, pTime->min, pTime->sec); 
	OUTTEXT(pSession, "Start Time = %s\r\n", szBuffer);

	nSecond = pInvoke->pResult[2]->stream.u32;
	if (nSecond > 3600)
		 sprintf(szBuffer, "%0d hour %0d min %0d sec", nSecond/3600, (nSecond%3600)/60, nSecond%60);
	else if (nSecond > 60)
		 sprintf(szBuffer, "%0d min %0d sec", nSecond/60, nSecond%60);
	else sprintf(szBuffer, "%0d sec", pInvoke->pResult[2]->stream.u32);
	OUTTEXT(pSession, "    Elapse = %s\r\n", szBuffer);

	OUTTEXT(pSession, "     Total = %0d\r\n", pInvoke->pResult[3]->stream.u32);
	OUTTEXT(pSession, "   Success = %0d\r\n", pInvoke->pResult[4]->stream.u32);
	OUTTEXT(pSession, "      Fail = %0d\r\n", pInvoke->pResult[5]->stream.u32);
	OUTTEXT(pSession, "     State = %s\r\n", pInvoke->pResult[6]->stream.u8 ? "ACTIVE" : "WAIT");
	nStep = pInvoke->pResult[7]->stream.u32;
	OUTTEXT(pSession, "  OTA Step = %s (0x%02X)\r\n", GetStepTypeString(nStep), nStep);

	return CLIERR_OK;
}

int cmdShowSensorPort(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);
#endif
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort);
  	EUI64       id;
	SENSORINFOENTRYNEW	*pSensor = NULL;
    ENDI_DIO_PAYLOAD   *dio;

  	if (!Confirm(pSession, MSG_DEFAULT))
    	return CLIERR_OK;

	StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.49");
    } 

  	invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));

 	invoke.AddParam("1.4", (BYTE) DIGITAL_STATUS_REQUEST);       // direction
 	invoke.AddParam("1.4", (BYTE) strtol(argv[1],(char **)NULL,16)); // mask
 	invoke.AddParam("1.4", (BYTE) 1); // value

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

  	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.49"))
        return CLIERR_OK;

    if(invoke.GetHandle()->nResultCount>0) 
    {
        dio = (ENDI_DIO_PAYLOAD *)invoke.GetHandle()->pResult[0]->stream.p;

        OUTTEXT(pSession, "Sensor Digital Port Get Result\r\n");
        OUTTEXT(pSession, "  + ID %s, MASK 0x%02X VALUE 0x%02X\r\n", 
                argv[0], dio->mask, dio->value);
    }
  	return CLIERR_OK;
}

int cmdShowMeterPQ(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);
#endif
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort);
	CIF4Invoke	invoke3("127.0.0.1", m_nLocalAgentPort);
  	EUI64       id;
	SENSORINFOENTRYNEW	*pSensor = NULL;
    SENSORCONSTANTENTRY *pCons = NULL;
    PQ   *pq;
    double fCV, fCC, fCAP, fCRP, fCPF, fFS;

  	if (!Confirm(pSession, MSG_DEFAULT))
    	return CLIERR_OK;

	StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.50");
    } 

  	invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_POWER_QUALITY);
    invoke.AddParam("1.4", (BYTE) 1);       // Read

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

  	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.50"))
        return CLIERR_OK;

    if(invoke.GetHandle()->nResultCount>0) 
    {
        pq = (PQ *)invoke.GetHandle()->pResult[0]->stream.p;

  	    invoke3.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
  	    if (!CLIAPI_Command(pSession, invoke3.GetHandle(), "102.51"))
        {
            OUTTEXT(pSession, "   RMS Current : 0x%08X\r\n", pq->rms_current);
            OUTTEXT(pSession, "   RMS Voltage : 0x%08X\r\n", pq->rms_voltage);
            OUTTEXT(pSession, "  Active Power : 0x%08X\r\n", pq->active_power);
            OUTTEXT(pSession, "Reactive Power : 0x%08X\r\n", pq->reactive_power);
            OUTTEXT(pSession, "  Power Factor : 0x%08X\r\n", pq->power_factor);
        }
        else
        {
            pCons = (SENSORCONSTANTENTRY *)invoke3.GetHandle()->pResult[0]->stream.p;
            fCV = ((double)pCons->powerVoltageScaleInt + (double)pCons->powerVoltageScaleReal/1000000.0)
                * GetUnsignedDouble(BigToHostInt(pq->rms_voltage));
            fCC = ((double)pCons->powerCurrentScaleInt + (double)pCons->powerCurrentScaleReal/1000000.0)
                * GetUnsignedDouble(BigToHostInt(pq->rms_current));
            fFS = ((double)pCons->powerVoltageScaleInt + (double)pCons->powerVoltageScaleReal/1000000.0)
                * ((double)pCons->powerCurrentScaleInt + (double)pCons->powerCurrentScaleReal/1000000.0);
            fCPF = GetSignedDouble(BigToHostInt(pq->power_factor));
            fCAP = fFS * GetSignedDouble(BigToHostInt(pq->active_power));           //* cos( fCV * fCC * fCPF);
            fCRP = fFS * GetSignedDouble(BigToHostInt(pq->reactive_power));         //* sin( fCV * fCC * fCPF);
            
            OUTTEXT(pSession, " Current Scale : %4d.%06d\r\n", 
                    pCons->powerCurrentScaleInt, pCons->powerCurrentScaleReal);
            OUTTEXT(pSession, " Voltage Scale : %4d.%06d\r\n", 
                    pCons->powerVoltageScaleInt, pCons->powerVoltageScaleReal);
            
            /*
            OUTTEXT(pSession, "pq->rms_voltage=%f\r\n", GetUnsignedDouble(BigToHostInt(pq->rms_voltage)));
            OUTTEXT(pSession, "pq->rms_current=%f\r\n", GetUnsignedDouble(BigToHostInt(pq->rms_current)));
            OUTTEXT(pSession, "pq->active_power=%f %08X\r\n", GetSignedDouble(BigToHostInt(pq->active_power)), BigToHostInt(pq->active_power));
            OUTTEXT(pSession, "pq->reactive_power=%f %08X\r\n", GetSignedDouble(BigToHostInt(pq->reactive_power)), BigToHostInt(pq->reactive_power));
            */
            
            OUTTEXT(pSession, "\r\n");
            OUTTEXT(pSession, "   RMS Current : %11.6f\r\n", fCC);
            OUTTEXT(pSession, "   RMS Voltage : %11.6f\r\n", fCV);
            OUTTEXT(pSession, "  Active Power : %11.6f\r\n", fCAP);
            OUTTEXT(pSession, "Reactive Power : %11.6f\r\n", fCRP);
            OUTTEXT(pSession, "  Power Factor : %11.6f\r\n", fCPF);
        }
    }
  	return CLIERR_OK;
}

int cmdShowSensorLastValue(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
	IF4Invoke		*pInvoke;
    int             nCount;
    char            parser[256];
    char            *idxPtr;
    EUI64           id;
    int             i, j, idx, len;
    int             nAttrCount;
    int             nValueLen;
	char		    szGUID[17];
	char		    szSerial[20+1];
	char		    szModel[18+1];
	char		    szAttrName[64];
	char		    szValue[32];
    BYTE            nAttr;
    TIMESTAMP       pTime;
    BOOL            bFirstLine;
    BYTE            nType;

    if(argc>0) {
        if(check_id(NULL, argv[0])) { // EUI64 ID
  	        StrToEUI64(argv[0], &id);
            invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        }else if(strcasecmp(argv[0],"all")) {
            strcpy(parser, argv[0]);
            if((idxPtr=strchr(parser,'$'))) {
                *idxPtr = 0x00;
            }
            invoke.AddParam("1.11",parser);
        }
    }

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.52"))
		return CLIERR_OK;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

	OUTTEXT(pSession, "===================================================================================================================\xd\xa");
	OUTTEXT(pSession, " NUM SENSOR ID        METER SERIAL       MODEL              ATTR         TYPE      VALUE              LAST UPDATE\xd\xa");
	OUTTEXT(pSession, "-------------------------------------------------------------------------------------------------------------------\xd\xa");

	for(i=0, idx=1; i<nCount; )
	{
        EUI64ToStr((EUI64 *)pInvoke->pResult[i]->stream.p, szGUID); i++;

        memset(szModel, 0, sizeof(szModel));
        len = MIN(pInvoke->pResult[i]->len, (int)sizeof(szModel)-1);
        strncpy(szModel, (char *)pInvoke->pResult[i]->stream.p, len); i++;

        nAttr = pInvoke->pResult[i]->stream.u8; i++;

        memset(szSerial, 0, sizeof(szSerial));
        len = MIN(pInvoke->pResult[i]->len, (int)sizeof(szSerial)-1);
        strncpy(szSerial, (char *)pInvoke->pResult[i]->stream.p, len); i++;

        memcpy(&pTime, pInvoke->pResult[i]->stream.p, sizeof(TIMESTAMP)); i++;

        nAttrCount = pInvoke->pResult[i]->stream.u32; i++;

        for(j=0, bFirstLine = TRUE; j < nAttrCount; j++)
        {
            if(bFirstLine)
            {
                OUTTEXT(pSession, " %3d %-16s %-18s %-18s ",
                        idx, szGUID, szSerial, szModel);
            }
            else
            {
                OUTTEXT(pSession, "                                                            ");
            }
            memset(szAttrName, 0, sizeof(szAttrName));
            len = MIN(pInvoke->pResult[i]->len, (int)sizeof(szAttrName)-1);
            strncpy(szAttrName, (char *)pInvoke->pResult[i]->stream.p, len); i++;

            nType = pInvoke->pResult[i]->stream.u8; i++;

            nValueLen = pInvoke->pResult[i]->stream.u32; i++;

            memset(szValue, 0, sizeof(szValue));
            SPrintMIBValue(szValue, nType, pInvoke->pResult[i], sizeof(szValue)); i++;

            OUTTEXT(pSession, "%-12s %-9s %-18s", szAttrName, VARAPI_GetTypeName(nType), szValue);
            if(bFirstLine)
            {
                bFirstLine = FALSE;
                OUTTEXT(pSession, " %02d/%02d %02d:%02d\r\n", pTime.mon, pTime.day, pTime.hour, pTime.min);
            }
            else
            {
                OUTTEXT(pSession, "\r\n");
            }
        }

        idx ++;

    }
	OUTTEXT(pSession, "===================================================================================================================\xd\xa");

	return CLIERR_OK;
}

int cmdShowSensorValue(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 60);
#endif
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort);
  	EUI64       id;
    BYTE        cmd;
	SENSORINFOENTRYNEW	*pSensor = NULL;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
  	    StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	        return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.50");
        } 

        invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }
    else
    {
		return CLIERR_OK;
    }

    cmd = (BYTE) strtol(argv[1],(char **)NULL, 10);

    switch(cmd)
    {
        case 0: // validate
        case 1: // install
        case 2: // install
            OUTTEXT(pSession, MSG_NOT_PERMITTED_OPERATION);
            return CLIERR_INVALID_PARAM;
    }

    invoke.AddParam("1.4", cmd);
    invoke.AddParam("1.4", (BYTE) 1);       // Command Read

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

  	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.50"))
        return CLIERR_OK;

    DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());

    OUTTEXT(pSession, "Command execution has succeeded.\xd\xa");

  	return CLIERR_OK;
}

int cmdShowDRAssetInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke		*pInvoke;
    int             nCount, i, j, nNum;
    char            parser[256];
    char            *idxPtr;
    EUI64           id;
    EUI64           *pId;
    BOOL            bDRAsset;
    BYTE            nCurrentLevel, nLevelCount;
    TIMESTAMP       *pLastUpdate;
    BYTE            *pLevelArray = NULL;
	char		    szGUID[17];
	char			szLastUpdate[30];

    if(argc>0) {
        if(check_id(NULL, argv[0])) { // EUI64 ID
  	        StrToEUI64(argv[0], &id);
            invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        }else if(strcasecmp(argv[0],"all")) {
            strcpy(parser, argv[0]);
            if((idxPtr=strchr(parser,'$'))) {
                *idxPtr = 0x00;
            }
            invoke.AddParam("1.11",parser);
        }
    }

	pInvoke = (IF4Invoke *)invoke.GetHandle();

	if (!CLIAPI_Command(pSession, pInvoke, "104.14"))
		return CLIERR_OK;

	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		OUTTEXT(pSession, MSG_NO_ENTRY "\r\n");
		return CLIERR_OK;
	}

	OUTTEXT(pSession, "==================================================================================\xd\xa");
	OUTTEXT(pSession, " NUM SENSOR ID        DR LEVEL  LAST UPDATE          VALID LEVEL\xd\xa");
	OUTTEXT(pSession, "==================================================================================\xd\xa");

    for(i=0, nNum=1; i < nCount; nNum ++)
    {
        pId = (EUI64 *)pInvoke->pResult[i]->stream.p;                   i++;
        bDRAsset = (BOOL)pInvoke->pResult[i]->stream.u8;                i++;
        nCurrentLevel = (BYTE)pInvoke->pResult[i]->stream.u8;           i++;
	    pLastUpdate  = (TIMESTAMP *)pInvoke->pResult[i]->stream.p;      i++;
        nLevelCount = (BYTE)pInvoke->pResult[i]->stream.u8;             i++;
        if(nLevelCount > 0) 
        {
            pLevelArray = (BYTE *)pInvoke->pResult[i]->stream.p;        i++;
        }

		EUI64ToStr(pId, szGUID);
        IF4API_TIMESTAMPTOSTR(szLastUpdate, pLastUpdate);

        OUTTEXT(pSession, " %3d %-16s  %c   %3d %20s ",
                    nNum,
                    szGUID,
                    bDRAsset ? 'Y' : 'N',
                    nCurrentLevel,
                    szLastUpdate
                );
        for(j=0;j<nLevelCount;j++) 
        {
            OUTTEXT(pSession, "%c%d", (j ? ',' : ' '), pLevelArray[j]);
        }
        OUTTEXT(pSession, "\r\n");
    }
	OUTTEXT(pSession, "==================================================================================\xd\xa");

	return CLIERR_OK;
}

int cmdShowEnergyLevel(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 10);
#endif
	IF4Invoke		*pInvoke;
    EUI64           id;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

  	StrToEUI64(argv[0], &id);
    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));

	pInvoke = (IF4Invoke *)invoke.GetHandle();

	if (!CLIAPI_Command(pSession, pInvoke, "104.15"))
		return CLIERR_OK;

	OUTTEXT(pSession, "Level=%d\r\n", pInvoke->pResult[0]->stream.u8);

    return CLIERR_OK;
}

int cmdShowGroup(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke   *pInvoke;
    EUI64       id;
    char        *pType = NULL, *pName = NULL;
    int         i, nCount;

    if(argc>0)
    {
        if(check_id(NULL, argv[0])) { // EUI64 ID
  	        StrToEUI64(argv[0], &id);
            invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        } else {
            invoke.AddParamFormat("1.11", VARSMI_STRING, argv[0], strlen(argv[0]));
            if(argc>1)
            {
                if(check_id(NULL, argv[1])) { // EUI64 ID
  	                StrToEUI64(argv[1], &id);
                    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
                }
                else
                {
                    invoke.AddParamFormat("1.11", VARSMI_STRING, argv[1], strlen(argv[1]));
                }
            }
            if(argc>2)
            {
                if(check_id(NULL, argv[2])) { // EUI64 ID
  	                StrToEUI64(argv[2], &id);
                    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
                }
            }
        }
    }

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	if (!CLIAPI_Command(pSession, pInvoke, "102.72"))
		return CLIERR_OK;

	nCount  = pInvoke->nResultCount;

    OUTTEXT(pSession, "================================================================================\r\n");
    OUTTEXT(pSession, " NUM TYPE                         NAME                         ID \r\n");
    OUTTEXT(pSession, "================================================================================\r\n");

	for (i=0; i<= (nCount-3); i+=3)
	{
        OUTTEXT(pSession, " %3d %-28s %-28s %s\r\n", (i/3)+1, 
                pType != NULL && !strcmp(pType, (char *)pInvoke->pResult[i]->stream.p) ? 
                    "" : (char *)pInvoke->pResult[i]->stream.p, 
                pName != NULL && !strcmp(pName, (char *)pInvoke->pResult[i+1]->stream.p) ? 
                    "" : (char *)pInvoke->pResult[i+1]->stream.p, 
                (char *)pInvoke->pResult[i+2]->stream.p);
        pType = (char *)pInvoke->pResult[i]->stream.p;
        pName = (char *)pInvoke->pResult[i+1]->stream.p;
	}
    OUTTEXT(pSession, "================================================================================\r\n");

    return CLIERR_OK;
}

int cmdShowMeterSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke   *pInvoke;
    char        *pType = NULL, *pName = NULL;
    int         i, j, nCount;

    if(argc>0)
    {
        invoke.AddParamFormat("1.11", VARSMI_STRING, argv[0], strlen(argv[0])); // type
        if(argc>1)
        {
            invoke.AddParamFormat("1.11", VARSMI_STRING, argv[1], strlen(argv[1])); // name
        }
    }

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	if (!CLIAPI_Command(pSession, pInvoke, "105.9"))
		return CLIERR_OK;

	nCount  = pInvoke->nResultCount;

    OUTTEXT(pSession, "================================================================================\r\n");
    OUTTEXT(pSession, " NUM TYPE     NAME               KEY \r\n");
    OUTTEXT(pSession, "================================================================================\r\n");

	for (i=0; i<= (nCount-3); i+=3)
	{
        OUTTEXT(pSession, " %3d %-8s %-18s ", (i/3)+1, 
                pType != NULL && !strcmp(pType, (char *)pInvoke->pResult[i]->stream.p) ? 
                    "" : (char *)pInvoke->pResult[i]->stream.p, 
                pName != NULL && !strcmp(pName, (char *)pInvoke->pResult[i+1]->stream.p) ? 
                    "" : (char *)pInvoke->pResult[i+1]->stream.p);

        for(j=0; j<pInvoke->pResult[i+2]->len;j++)
        {
            OUTTEXT(pSession, "%02X ", pInvoke->pResult[i+2]->stream.p[j]);
        }
        OUTTEXT(pSession, "\r\n");
        pType = (char *)pInvoke->pResult[i]->stream.p;
        pName = (char *)pInvoke->pResult[i+1]->stream.p;
	}
    OUTTEXT(pSession, "================================================================================\r\n");

    return CLIERR_OK;
}

