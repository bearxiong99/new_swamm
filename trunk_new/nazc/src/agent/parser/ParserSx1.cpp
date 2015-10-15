//////////////////////////////////////////////////////////////////////
//
//  ParserSx1.cpp: implementation of the CSx1Parser class.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "common.h"
#include "codiapi.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "Chunk.h"
#include "Event.h"
#include "MeterUploader.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "Parity.h"
#include "AgentService.h"
#include "Patch.h"
#include "MemoryDebug.h"

#include "vendor/pollmiu.h"
#include "ParserSx1.h"

#include "iec62056_21Define.h"
#include "iec62056_21Functions.h"
#include "iecError.h"

#include "types/units.h"
#include "types/identification.h"
#include "rep/ChannelCfgHelper.h"

#if     defined(__PATCH_11_65__)
/**< Issue #65 JEJU SP */
extern BOOL    *m_pbPatch_11_65;
#endif
extern BOOL     m_bEnableMeterTimesync;
extern int      m_nTimesyncThresholdLow;
extern int      m_nTimesyncThresholdHigh;

/** SX Meter FW Version 조회 */
#define     ONDEMAND_OPTION_GET_METER_FW_VERSION        9
#define     ONDEMAND_OPTION_RESTORE_DEFAULT_FW          12

typedef enum {
    IDX_PASSWORD,
    IDX_REQ_ID,
    IDX_REQ_ENERGY,
    IDX_REQ_FORWARD_ENERGY,
    IDX_REQ_REVERSE_ENERGY,
    IDX_REQ_VOLTAGE,
    IDX_REQ_LINE_CURRENT,
    IDX_REQ_NEUTRAL_CURRENT,
    IDX_REQ_POWER_FACTOR,
    IDX_REQ_METER_TIME,
    IDX_REQ_RELAY_STATUS,
    IDX_REQ_BATTERY,
    IDX_REQ_ISSUE_DATE,
    IDX_REQ_FUTURE_DATE,

    IDX_REQ_LOG_REVERSE_CURRENT,
    IDX_REQ_LOG_TERMINAL_COVER,
    IDX_REQ_LOG_EARTH_LOAD,
    IDX_REQ_LOG_BYPASS_MAINLINE,
    IDX_REQ_LOG_OVERLOAD_CURRENT,
    IDX_REQ_LOG_FRONT_COVER,

    IDX_REQ_TAMPER,
    IDX_CMD_CLEAR_TAMPER,

    IDX_CMD_RELAY_CONNECT,
    IDX_CMD_RELAY_DISCONNECT,
    IDX_CMD_METER_TIME_SYNC,
    IDX_CMD_RESTORE_FW,
    IDX_BREAK
} MeteringIndex;

/** preAction에서 TRUE가 return 되어야 해당 state를 수행한다 */
typedef BOOL (*PFPREACTION)(ONDEMANDPARAM *pOndemand);

/** postAction에서 FALSE가 return 되면 즉시 종료 한다 
 *
 * TODO : 미구현
 */
typedef BOOL (*PFPOSTACTION)(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);

typedef struct {
    BYTE nStatus;
    char *szDesc;
    PFPREACTION preAction;
    PFPOSTACTION postAction;
} SX_OPTION_TBL;

typedef struct {
    UINT    forwardEnergy;
    UINT    reverseEnergy;
    UINT    voltage;
    UINT    lineCurrent;
    UINT    neutralCurrent;
    UINT    powerFactor;
    UINT    tamperFlag;
    int     timeGap;
    BOOL    executeTimesync;
} SX_METERING_INFO;

SX_OPTION_TBL sxOptionTbl_ondemand [] = {       // Ondemand default
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD"), NULL, NULL},
    {IDX_REQ_ID, const_cast<char*>(" IEC21 REQ ID"), NULL, NULL},
    {IDX_REQ_ISSUE_DATE, const_cast<char*>(" IEC21 REQ FW ISSUE DATE"), NULL, NULL},
    {IDX_REQ_METER_TIME, const_cast<char*>(" IEC21 REQ METER TIME"), NULL, NULL},
    {IDX_CMD_METER_TIME_SYNC, const_cast<char*>(" IEC21 CMD METER TIME SYNC"), CSx1Parser::fnPreSyncTime, NULL},
    {IDX_REQ_METER_TIME, const_cast<char*>(" IEC21 REQ METER TIME"), CSx1Parser::fnPreRereadTime, NULL},
    //{IDX_REQ_ENERGY, const_cast<char*>(" IEC21 REQ ENERGY"), NULL, NULL},
    {IDX_REQ_FORWARD_ENERGY, const_cast<char*>(" IEC21 REQ FORWARD ENERGY"), NULL, NULL},
    {IDX_REQ_REVERSE_ENERGY, const_cast<char*>(" IEC21 REQ REVERSE ENERGY"), NULL, NULL},
    {IDX_REQ_LINE_CURRENT, const_cast<char*>(" IEC21 REQ LINE CURRENT"), NULL, NULL},
    {IDX_REQ_NEUTRAL_CURRENT, const_cast<char*>(" IEC21 REQ NEUTRAL CURRENT"), NULL, NULL},
    {IDX_REQ_POWER_FACTOR, const_cast<char*>(" IEC21 REQ POWER FACTOR"), NULL, NULL},
    {IDX_REQ_RELAY_STATUS, const_cast<char*>(" IEC21 REQ RELAY STATUS"), NULL, NULL},
    {IDX_REQ_BATTERY, const_cast<char*>(" IEC21 REQ BATTERY STATUS"), NULL, NULL},
    {IDX_REQ_TAMPER, const_cast<char*>(" IEC21 REQ TAMPER STATUS"), NULL, NULL},
    {IDX_REQ_LOG_REVERSE_CURRENT, const_cast<char*>(" IEC21 REQ REVERSE CURRENT"), CSx1Parser::fnPreReverse, NULL},
    {IDX_REQ_LOG_TERMINAL_COVER, const_cast<char*>(" IEC21 REQ TERMINAL COVER"), CSx1Parser::fnPreTerminalCover, NULL},
    {IDX_REQ_LOG_EARTH_LOAD, const_cast<char*>(" IEC21 REQ EARTH LOAD"), CSx1Parser::fnPreBypassEarth, NULL},
    {IDX_REQ_LOG_BYPASS_MAINLINE, const_cast<char*>(" IEC21 REQ BYPASS MAINLINE"), CSx1Parser::fnPreBypassEarth, NULL},
    {IDX_REQ_LOG_OVERLOAD_CURRENT, const_cast<char*>(" IEC21 REQ OVERLOAD CURRENT"), CSx1Parser::fnPreOverload, NULL},
    {IDX_REQ_LOG_FRONT_COVER, const_cast<char*>(" IEC21 REQ FRONT COVER"), CSx1Parser::fnPreFrontCover, NULL},
    {IDX_CMD_CLEAR_TAMPER, const_cast<char*>(" IEC21 CMD CLEAR TAMPER"), CSx1Parser::fnPreClearTamper, NULL},
    {IDX_BREAK, const_cast<char*>(" IEC21 BREAK"), NULL, NULL},
    {0, NULL, NULL, NULL}
};

SX_OPTION_TBL sxOptionTbl_relay_status [] = {       // Get Energy Level
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD"), NULL, NULL},
    {IDX_REQ_RELAY_STATUS, const_cast<char*>(" IEC21 REQ RELAY STATUS"), NULL, NULL},
    {IDX_BREAK, const_cast<char*>(" IEC21 BREAK"), NULL, NULL},
    {0, NULL, NULL, NULL}
};

SX_OPTION_TBL sxOptionTbl_relay_connect [] = {      // Relay connect
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD"), NULL, NULL},
    {IDX_CMD_RELAY_CONNECT, const_cast<char*>(" IEC21 EXE RELAY CONNECT"), NULL, NULL},
    {IDX_REQ_RELAY_STATUS, const_cast<char*>(" IEC21 REQ RELAY STATUS"), NULL, NULL},
    {IDX_BREAK, const_cast<char*>(" IEC21 BREAK"), NULL, NULL},
    {0, NULL, NULL, NULL}
};

SX_OPTION_TBL sxOptionTbl_relay_disconnect [] = {      // Relay disconnect
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD"), NULL, NULL},
    {IDX_CMD_RELAY_DISCONNECT, const_cast<char*>(" IEC21 EXE RELAY DISCONNECT"), NULL, NULL},
    {IDX_REQ_RELAY_STATUS, const_cast<char*>(" IEC21 REQ RELAY STATUS"), NULL, NULL},
    {IDX_BREAK, const_cast<char*>(" IEC21 BREAK"), NULL, NULL},
    {0, NULL, NULL, NULL}
};

SX_OPTION_TBL sxOptionTbl_event_reading [] = {             // Event reading
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD"), NULL, NULL},
    {IDX_REQ_TAMPER, const_cast<char*>(" IEC21 REQ TAMPER STATUS"), NULL, NULL},
    {IDX_REQ_LOG_REVERSE_CURRENT, const_cast<char*>(" IEC21 REQ REVERSE CURRENT"), NULL, NULL},
    {IDX_REQ_LOG_TERMINAL_COVER, const_cast<char*>(" IEC21 REQ TERMINAL COVER"), NULL, NULL},
    {IDX_REQ_LOG_EARTH_LOAD, const_cast<char*>(" IEC21 REQ EARTH LOAD"), NULL, NULL},
    {IDX_REQ_LOG_BYPASS_MAINLINE, const_cast<char*>(" IEC21 REQ BYPASS MAINLINE"), NULL, NULL},
    {IDX_REQ_LOG_OVERLOAD_CURRENT, const_cast<char*>(" IEC21 REQ OVERLOAD CURRENT"), NULL, NULL},
    {IDX_REQ_LOG_FRONT_COVER, const_cast<char*>(" IEC21 REQ FRONT COVER"), NULL, NULL},
    {IDX_CMD_CLEAR_TAMPER, const_cast<char*>(" IEC21 CMD CLEAR TAMPER"), CSx1Parser::fnPreClearTamper, NULL},
    {IDX_BREAK, const_cast<char*>(" IEC21 BREAK"), NULL, NULL},
    {0, NULL, NULL, NULL}
};

SX_OPTION_TBL sxOptionTbl_timesync [] = {           // Meter Timesync
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD"), NULL, NULL},
    {IDX_REQ_METER_TIME, const_cast<char*>(" IEC21 REQ METER TIME"), NULL, NULL},
    {IDX_CMD_METER_TIME_SYNC, const_cast<char*>(" IEC21 CMD METER TIME SYNC"), NULL, NULL},
    {IDX_REQ_METER_TIME, const_cast<char*>(" IEC21 REQ METER TIME"), NULL, NULL},
    {IDX_BREAK, const_cast<char*>(" IEC21 BREAK"), NULL, NULL},
    {0, NULL, NULL, NULL}
};

SX_OPTION_TBL sxOptionTbl_meter_fw_version [] = {   // Meter FW Version
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD"), NULL, NULL},
    {IDX_REQ_ISSUE_DATE, const_cast<char*>(" IEC21 REQ FW ISSUE DATE"), NULL, NULL},
    {IDX_REQ_FUTURE_DATE, const_cast<char*>(" IEC21 REQ FW FUTURE DATE"), NULL, NULL},
    {IDX_REQ_METER_TIME, const_cast<char*>(" IEC21 REQ METER TIME"), NULL, NULL},
    {IDX_BREAK, const_cast<char*>(" IEC21 BREAK"), NULL, NULL},
    {0, NULL, NULL, NULL}
};

SX_OPTION_TBL sxOptionTbl_restore_meter_fw [] = {   // Restore default meter fw
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD"), NULL, NULL},
    {IDX_REQ_ISSUE_DATE, const_cast<char*>(" IEC21 REQ FW ISSUE DATE"), NULL, NULL},
    {IDX_CMD_RESTORE_FW, const_cast<char*>(" IEC21 EXE RESTORE METER FW"), NULL, NULL},
    {IDX_BREAK, const_cast<char*>(" IEC21 BREAK"), NULL, NULL},
    {0, NULL, NULL, NULL}
};

//////////////////////////////////////////////////////////////////////
// CSx1Parser Class
//////////////////////////////////////////////////////////////////////

static const char *parserNameArray[] = {"SX", "NAMR-P10B", NULL};

//////////////////////////////////////////////////////////////////////
// CSx1Parser Class
//////////////////////////////////////////////////////////////////////

CSx1Parser::CSx1Parser() : CPollBaseParser(SENSOR_TYPE_ZRU, SERVICE_TYPE_ELEC, 
                                            ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL, 
                                            const_cast<char **>(parserNameArray), PARSER_TYPE_SX, 
                                            "SX Parser", "SX", parserNameArray[0],
                                            6000, 6000, 2)
{
}

CSx1Parser::~CSx1Parser()
{
}

//////////////////////////////////////////////////////////////////////
// CSx1Parser Override method
//////////////////////////////////////////////////////////////////////
/** Ondemand 가 시행되기 전에 사전 조정 작업 수행.
 */
void CSx1Parser::ActPreAction(ONDEMANDPARAM *pOndemand)
{
    XDEBUG(" ActPreAction\r\n");

    if(pOndemand != NULL) {
        switch(pOndemand->nOption) {
            case ONDEMAND_OPTION_GET_RELAY_STATUS:  // Relay Status
                XDEBUG(" + Relay Status\r\n");
                pOndemand->pOndemandCallData = sxOptionTbl_relay_status; 
                pOndemand->nCount = 0;  // LP를 읽지 않는다
                break;
            case ONDEMAND_OPTION_DO_RELAY_ON:       // Relay Connect
                XDEBUG(" + Relay Connect\r\n");
                pOndemand->pOndemandCallData = sxOptionTbl_relay_connect; 
                pOndemand->nCount = 0;  // LP를 읽지 않는다
                break;
            case ONDEMAND_OPTION_DO_RELAY_OFF:      // Relay Disconnect
                XDEBUG(" + Relay Disconnect\r\n");
                pOndemand->pOndemandCallData = sxOptionTbl_relay_disconnect; 
                pOndemand->nCount = 0;  // LP를 읽지 않는다
                break;
            case ONDEMAND_OPTION_TIME_SYNC:         // Timesync
                XDEBUG(" + Meter Time Synchronization\r\n");
                pOndemand->pOndemandCallData = sxOptionTbl_timesync; 
                pOndemand->nCount = 0;  // LP를 읽지 않는다
                break;
            case ONDEMAND_OPTION_GET_EVENT_LOG:  // Event Reading
                XDEBUG(" + Meter Event Reading\r\n");
                pOndemand->pOndemandCallData = sxOptionTbl_event_reading; 
                pOndemand->nCount = 0;  // LP를 읽지 않는다
                break;
            case ONDEMAND_OPTION_GET_METER_FW_VERSION:  // Get Meter FW Version
                XDEBUG(" + Meter FW Version\r\n");
                pOndemand->pOndemandCallData = sxOptionTbl_meter_fw_version; 
                pOndemand->nCount = 0;  // LP를 읽지 않는다
                break;
            case ONDEMAND_OPTION_RESTORE_DEFAULT_FW:  // Restore Meter FW
                XDEBUG(" + Restore Meter FW\r\n");
                pOndemand->pOndemandCallData = sxOptionTbl_restore_meter_fw; 
                pOndemand->nCount = 0;  // LP를 읽지 않는다
                break;
            default:
                XDEBUG(" + Ondemand\r\n");
                /** SX Meter는 Meter 상수를 읽어야 한다.
                */
                pOndemand->bMeterConstants = TRUE;
                pOndemand->pOndemandCallData = sxOptionTbl_ondemand; break;
        }

        pOndemand->pSession = (SX_METERING_INFO *)MALLOC(sizeof(SX_METERING_INFO));
        memset(pOndemand->pSession, 0, sizeof(SX_METERING_INFO));
    }
}

/** SX Parser는 LP Format이 변경되어 Ondemand 관련 코드가 수정되기 전 까지 LP 부분을
 * 읽지 않기 때문에 dateCount = 0 으로 설정한다.
 * 이 부분은 LP를 읽는 코드로 수정할 경우 삭제해야 한다.
 */
void CSx1Parser::ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData)
{
    POLLBASED_MULT_CH_METERING_INFO info;
    POLLBASED_MULT_CH_INFO chInfo;
    ENDI_LPLOG_INFO         *pLpLog = (ENDI_LPLOG_INFO *)pExtraData;
    SX_METERING_INFO        *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;

    if(pOndemand->nMeteringType != METERING_TYPE_BYPASS)
    {
        memset(&info, 0, sizeof(POLLBASED_MULT_CH_METERING_INFO));
        info.dataFormat  = 0x80 | 0x06;     // Big Endian , 6 Channel
        GetGmtTimestamp(&info.time);
        info.lpPeriod    = pLpLog->LPPERIOD;
        info.dateCount   = pOndemand->nCount;
   
        pChunk->Add((char *)&info, sizeof(POLLBASED_MULT_CH_METERING_INFO));        // LP 정보

        // Channel Configuration
        chInfo.channelType = 1;
        chInfo.unit = UNIT_TYPE_WATT_HOUR;
        chInfo.meterConstant = 1;
        chInfo.curPulse = HostToBigInt(sxInfo->forwardEnergy);
        pChunk->Add((char *)&chInfo, sizeof(POLLBASED_MULT_CH_INFO));        

        chInfo.channelType = 2;
        chInfo.unit = UNIT_TYPE_WATT_HOUR;
        chInfo.meterConstant = 1;
        chInfo.curPulse = HostToBigInt(sxInfo->reverseEnergy);
        pChunk->Add((char *)&chInfo, sizeof(POLLBASED_MULT_CH_INFO));        

        chInfo.channelType = 13;
        chInfo.unit = UNIT_TYPE_OTHER;
        chInfo.meterConstant = 100;
        chInfo.curPulse = HostToBigInt(sxInfo->powerFactor);
        pChunk->Add((char *)&chInfo, sizeof(POLLBASED_MULT_CH_INFO));        

        chInfo.channelType = 12;
        chInfo.unit = UNIT_TYPE_VOLT;
        chInfo.meterConstant = 100;
        chInfo.curPulse = HostToBigInt(sxInfo->voltage);
        pChunk->Add((char *)&chInfo, sizeof(POLLBASED_MULT_CH_INFO));        

        chInfo.channelType = 11;
        chInfo.unit = UNIT_TYPE_AMPERE;
        chInfo.meterConstant = 100;
        chInfo.curPulse = HostToBigInt(sxInfo->lineCurrent);
        pChunk->Add((char *)&chInfo, sizeof(POLLBASED_MULT_CH_INFO));        

        chInfo.channelType = 91;
        chInfo.unit = UNIT_TYPE_AMPERE;
        chInfo.meterConstant = 100;
        chInfo.curPulse = HostToBigInt(sxInfo->neutralCurrent);
        pChunk->Add((char *)&chInfo, sizeof(POLLBASED_MULT_CH_INFO));        

        pChunk->Add(pOndemand->pLpChunk->GetBuffer(), pOndemand->pLpChunk->GetSize());        // LP 
    }

    if(pOndemand->pMtrChunk->GetSize() > 0) {
        pChunk->Add(pOndemand->pMtrChunk->GetBuffer(), pOndemand->pMtrChunk->GetSize());        // Bypass 정보
    }

/** 6 Channel에 대한 값을 남겨야 한다
    if(pOndemand->nMeteringType != METERING_TYPE_BYPASS)
    {
        m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("MC"), VARSMI_WORD, sizeof(WORD), (BYTE *) &data.meterConstant);
        m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("CP"), VARSMI_UINT, sizeof(UINT), (BYTE *) &data.curPulse);
    }
    */

    if(pOndemand->pSession != NULL)
    {
        FREE(pOndemand->pSession);
    }
}


int CSx1Parser::StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    XDEBUG( " StateBypassInitialize\r\n");

    pOndemand->nIndex = 0;
    pOndemand->nRetry = 0;
    return STATE_QUERY;
}

int CSx1Parser::StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    int                 nFrameLen = 0;
    int                 nIndex = pOndemand->nIndex;
    int                 nError;
    int                 nCount=16;
    char                paramBuff[32] = {0,};
    SX_OPTION_TBL       *pSxOptionTbl;

    pSxOptionTbl = &((SX_OPTION_TBL *)pOndemand->pOndemandCallData)[nIndex];
    if(pSxOptionTbl->nStatus == 0 && pSxOptionTbl->szDesc == NULL) return  STATE_OK;

    if(pSxOptionTbl->szDesc != NULL)
    {
        XDEBUG(ANSI_COLOR_GREEN " %s\r\n" ANSI_NORMAL, pSxOptionTbl->szDesc);
    }

    /** PreAction이 설정되어 있을 경우 이 단계를 실행할지 검사한다 */
    if(pSxOptionTbl->preAction != NULL)
    {
        if(pSxOptionTbl->preAction(pOndemand) != TRUE)
        {
            pOndemand->nIndex ++;
            return STATE_QUERY;
        }
    }

    if(pOndemand->nCount > 0)
    {
        nCount = pOndemand->nCount;
    }

    switch(pSxOptionTbl->nStatus)
    {
        case IDX_PASSWORD:          // Password
            {
                BYTE security[128];
                memset(security, 0, sizeof(security));

                // Security 얻어오기 Issue #320
                if(!m_pEndDeviceList->GetMeterSecurity(&pOndemand->id, security+1, NULL))
                {
                    // 실패할 경우 기본 Security 사용
                    memcpy(security+1, "SX1SMARTMETERPROJECT", 20);
                }
                security[0] = '(';
                security[strlen((char *)security)] = ')';
                nError = iec21MakeCommand(szBuffer, IEC_21_CMD_PASSWORD, '1', (BYTE *)NULL, 0, 
                    (BYTE *)security, strlen((char *)security), AIMIR_PARITY_EVEN, &nFrameLen);
            }
            break;
        case IDX_REQ_ID:            // Request ID
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"00", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_ISSUE_DATE:    // Request Meter FW Issue Date
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"02", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_FUTURE_DATE:   // Request Meter FW Future Date
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"F5", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_ENERGY:        // Request Display energy
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"D7", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_FORWARD_ENERGY:
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"D5", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_REVERSE_ENERGY:
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"D6", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_VOLTAGE:
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"D0", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_LINE_CURRENT:
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"D1", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_NEUTRAL_CURRENT:
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"D2", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_POWER_FACTOR:
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"EC", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_METER_TIME:     // Meter time
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"34", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_TAMPER:     // Tamper status
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"E7", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_CMD_METER_TIME_SYNC:    // Meter time sync
            {
                cetime_t    nCurrentTime=0;
                char        szTimeString[16];

                ceTime(&nCurrentTime);
                memset(szTimeString, 0, sizeof(szTimeString));
                ConvTimeT2HexaTimeStr(nCurrentTime, szTimeString + 1);
                szTimeString[0] = '(';
                szTimeString[12+1] = ')';
                XDEBUG(ANSI_COLOR_GREEN " TIME [%s] %d\r\n" ANSI_NORMAL, szTimeString, strlen(szTimeString));
                nError = iec21MakeCommand(szBuffer, IEC_21_CMD_WRITE, '2', (BYTE *)"34", 2, 
                        (BYTE *)szTimeString, strlen(szTimeString), AIMIR_PARITY_EVEN, &nFrameLen);
            }
            break;
        case IDX_REQ_RELAY_STATUS:  // Request Relay Status
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"F4", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_BATTERY:  // Request Battery Status
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"35", 2, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;

        case IDX_REQ_LOG_REVERSE_CURRENT:  
            sprintf(paramBuff,"(%d)", nCount);
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"D9", 2, (BYTE *)paramBuff, strlen(paramBuff), 
                    AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_LOG_TERMINAL_COVER:  
            sprintf(paramBuff,"(%d)", nCount);
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"DA", 2, (BYTE *)paramBuff, strlen(paramBuff), 
                    AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_LOG_EARTH_LOAD:  
            sprintf(paramBuff,"(%d)", nCount);
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"DB", 2, (BYTE *)paramBuff, strlen(paramBuff), 
                    AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_LOG_BYPASS_MAINLINE:  
            sprintf(paramBuff,"(%d)", nCount);
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"DC", 2, (BYTE *)paramBuff, strlen(paramBuff), 
                    AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_LOG_OVERLOAD_CURRENT:  
            sprintf(paramBuff,"(%d)", nCount);
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"DD", 2, (BYTE *)paramBuff, strlen(paramBuff), 
                    AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_REQ_LOG_FRONT_COVER:  
            sprintf(paramBuff,"(%d)", nCount);
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_READ, '2', (BYTE *)"E8", 2, (BYTE *)paramBuff, strlen(paramBuff), 
                    AIMIR_PARITY_EVEN, &nFrameLen);
            break;

        case IDX_CMD_RELAY_CONNECT: // Execute Relay Connect
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_EXECUTE, '2', (BYTE *)"EX11", 4, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_CMD_RELAY_DISCONNECT: // Execute Relay Disconnect
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_EXECUTE, '2', (BYTE *)"EX12", 4, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_CMD_RESTORE_FW:        // Restore Meter FW
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_EXECUTE, '2', (BYTE *)"EX21", 4, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_CMD_CLEAR_TAMPER: // Clear Tamper Status
            nError = iec21MakeCommand(szBuffer, IEC_21_CMD_EXECUTE, '2', (BYTE *)"EX08", 4, (BYTE *)"()", 2, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        case IDX_BREAK:     // Break
            nError = iec21MakeBreak(szBuffer, AIMIR_PARITY_EVEN, &nFrameLen);
            break;
        default:
            return STATE_OK;
    }

    if(nError == IECERR_NOERROR)
    {
        SendCommand(pOndemand, &pOndemand->id, (const char *)szBuffer, nFrameLen);
    }
    else
    {
        XDEBUG(" MakeFrameError : %s\r\n", iec21GetErrorMessage(nError));
        return STATE_ERROR;
    }

    if(pSxOptionTbl->nStatus != IDX_BREAK) {
        m_Locker.Lock();
        ClearOperation(pOndemand);
        pOndemand->bReply = FALSE;
        ResetEvent(pOndemand->hEvent);
        m_Locker.Unlock();
        return STATE_WAIT_REPLY;
    } else {
        return STATE_OK;
    }
}

int CSx1Parser::StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    WaitForSingleObject(pOndemand->hEvent, (int)(GetReplyTimeout()/1000));
    if(pOndemand->bPartialWait)
    {
        pOndemand->bPartialWait = FALSE;
        return STATE_WAIT_REPLY;
    }
    if(pOndemand->bReply)
    {
        pOndemand->nRetry = 0;
        pOndemand->nResult = ONDEMAND_ERROR_OK;
        if(pOndemand->nIndex == 0xFF) {
            return STATE_OK;
        }
        return STATE_QUERY;
    }
    else
    {
        /** Timeout */
        pOndemand->nRetry ++;
        XDEBUG(" FAIL COUNT SUCC %d\r\n", pOndemand->nRetry);
        if (pOndemand->nRetry >= 3)
            return STATE_ERROR;
        else
            return STATE_QUERY;
    }
}

/** DR 가능 장비인지 여부를 리턴한다.
 */
BOOL CSx1Parser::IsDRAsset(EUI64 *pId)
{
    return TRUE;
}

/** 장비에서 유효한 Level 값 배열 리턴.
 *
 * @return 
 *      0 지원하지 않음
 *      positive 배열 개 멤버 수
 *      -1 NULL Parameter
 *
 * @see CSx1Parser#IsDRAsset(EUI64 *pId);
 *
 * @warning pEnergyLevelArray는 최소 15bytes 이상의 길이를 가져야 한다.
 */
int CSx1Parser::GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray)
{
    pEnergyLevelArray[0] = 0x01;
    pEnergyLevelArray[1] = 0x0F;

    return 2;
}

/** 현재 Energy Level 값 
 *
 * @param pId EUI64 ID
 *
 * @return 
 *      IF4ERR_NOERROR 성공
 *      IF4ERR_INVALID_ID 잘못된 ID
 *      IF4ERR_INVALID_PARAM 잘못된 pEnergyLevel pointer
 *      IF4ERR_DO_NOT_SUPPORT 지원하지 않음
 *      IF4ERR_CANNOT_CONNECT 장비에 연결되지 않음
 */
int CSx1Parser::GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel)
{
    CMDPARAM cmdParam;
    METER_STAT stat;
    int nError;
    BYTE nEnergyLevel=0;
	CChunk chunk(128);

    if(pId == NULL) return IF4ERR_INVALID_ID;
    if(pEnergyLevel == NULL) return IF4ERR_INVALID_PARAM;

    memset(&cmdParam, 0, sizeof(CMDPARAM));
    memset(&stat, 0, sizeof(METER_STAT));

    memcpy(&cmdParam.id, pId, sizeof(EUI64));
    cmdParam.codi      = GetCoordinator();
    cmdParam.nMeteringType = METERING_TYPE_BYPASS;
    cmdParam.nOption = ONDEMAND_OPTION_GET_RELAY_STATUS;
	cmdParam.pChunk	= &chunk;

    nError = METERING(&cmdParam, &stat);
    switch(nError) {
        case ONDEMAND_ERROR_OK:
            m_pEndDeviceList->GetEnergyLevel(pId, &nEnergyLevel, NULL);
            *pEnergyLevel = nEnergyLevel;
/** 제주실증단지 DR Level 적용 Patch */
#if     defined(__PATCH_11_65__)
            if(*m_pbPatch_11_65) {
                *pEnergyLevel = Patch_11_65_Nuri2Jeju(nEnergyLevel);
            }
#endif
            return IF4ERR_NOERROR;
        case ONDEMAND_ERROR_NOT_CONNECT:
        case ONDEMAND_ERROR_INIT:
        case ONDEMAND_ERROR_BUSY:
            return IF4ERR_CANNOT_CONNECT;
        default:
            return IF4ERR_UNKNOWN_ERROR;
    }
}

/** Energy Level 설정
 *
 * @param pId EUI64 ID
 * @param nEnergyLevel 유요한 level 값
 *
 * @return 
 *      IF4ERR_NOERROR 성공
 *      IF4ERR_INVALID_ID 잘못된 ID
 *      IF4ERR_INVALID_PARAM 잘못된 Level 값
 *      IF4ERR_DO_NOT_SUPPORT 지원하지 않음
 *      IF4ERR_CANNOT_CONNECT 장비에 연결되지 않음
 *
 * @see CACDParser#IsDRAsset();
 */
int CSx1Parser::SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen)
{
    CMDPARAM cmdParam;
    METER_STAT stat;
    int nError;
	CChunk chunk(128);

    if(pId == NULL) return IF4ERR_INVALID_ID;
    if(nEnergyLevel <= 0 || nEnergyLevel > ENERGY_LEVEL_MAX) return IF4ERR_INVALID_ID;

    memset(&cmdParam, 0, sizeof(CMDPARAM));
    memset(&stat, 0, sizeof(METER_STAT));

    memcpy(&cmdParam.id, pId, sizeof(EUI64));
    cmdParam.codi      = GetCoordinator();
    cmdParam.nMeteringType = METERING_TYPE_BYPASS;
	cmdParam.pChunk	= &chunk;
    switch(nEnergyLevel) {
        case  ENERGY_LEVEL_MIN:
            cmdParam.nOption = ONDEMAND_OPTION_DO_RELAY_ON;
            break;
        default:
            cmdParam.nOption = ONDEMAND_OPTION_DO_RELAY_OFF;
            break;
    }

    nError = METERING(&cmdParam, &stat);
    switch(nError) {
        case ONDEMAND_ERROR_OK:
            return IF4ERR_NOERROR;
        case ONDEMAND_ERROR_NOT_CONNECT:
        case ONDEMAND_ERROR_INIT:
        case ONDEMAND_ERROR_BUSY:
            return IF4ERR_CANNOT_CONNECT;
        default:
            return IF4ERR_UNKNOWN_ERROR;
    }
}


/** Meter Instance가 최초 생성되었을 때 불려진다.
 *
 * SX Meter의 Channel Configuration을 만든다.
 * 
 * @param pId Mode ID
 * @param nPortNum port Number
 */
BOOL CSx1Parser::PostCreate(const EUI64 *pId, int nPortNum)
{
    int chIdx=0;

    XDEBUG(" SxParser::PostCreate\r\n");
    m_pChannelCfgHelper->Update(pId, nPortNum, chIdx, 
                UNIT_TYPE_WATT_HOUR,        // Unit
                0x00,                       // SigExp : 0
                VALUE_TYPE_DIFF,
                OBJECT_TYPE_ELECTRICITY,    // Object Type
                0x01                        // Forward active energy
        ); chIdx++;
    m_pChannelCfgHelper->Update(pId, nPortNum, chIdx, 
                UNIT_TYPE_WATT_HOUR,        // Unit
                0x00,                       // SigExp : 0
                VALUE_TYPE_DIFF,
                OBJECT_TYPE_ELECTRICITY,    // Object Type
                0x02                        // Reverse active energy
        ); chIdx++;
    m_pChannelCfgHelper->Update(pId, nPortNum, chIdx, 
                UNIT_TYPE_OTHER,            // Unit
                0x42,                       // SigExp : pow(10,-2) centi
                VALUE_TYPE_CURRENT,
                OBJECT_TYPE_ELECTRICITY,    // Object Type
                0x0D                        // Power Factor
        ); chIdx++;
    m_pChannelCfgHelper->Update(pId, nPortNum, chIdx, 
                UNIT_TYPE_VOLT,             // Unit
                0x42,                       // SigExp : pow(10,-2) centi
                VALUE_TYPE_CURRENT,
                OBJECT_TYPE_ELECTRICITY,    // Object Type
                0x0C                        // RMS Voltage
        ); chIdx++;
    m_pChannelCfgHelper->Update(pId, nPortNum, chIdx, 
                UNIT_TYPE_AMPERE,           // Unit
                0x42,                       // SigExp : pow(10,-2) centi
                VALUE_TYPE_CURRENT,
                OBJECT_TYPE_ELECTRICITY,    // Object Type
                0x0B                        // RMS Line Current
        ); chIdx++;
    m_pChannelCfgHelper->Update(pId, nPortNum, chIdx, 
                UNIT_TYPE_AMPERE,           // Unit
                0x42,                       // SigExp : pow(10,-2) centi
                VALUE_TYPE_CURRENT,
                OBJECT_TYPE_ELECTRICITY,    // Object Type
                0x5B                        // RMS Neutral Current
        ); chIdx++;

    return TRUE;
}

BOOL CSx1Parser::IsPushMetering()
{
    return TRUE;
}

WORD CSx1Parser::GetLpAddress(BYTE nLpPointer, int nDay, BYTE nPeriod)
{
    return SX_METER_LP_ADDRESS(ENDI_ROMAP_METER_LPLOGDATA, (nLpPointer-nDay), nPeriod);
}
int CSx1Parser::GetLpSize(BYTE nPeriod)
{
    /** period * (24 * WORD) * 6 Channel + (TIMEDATE + UINT * 6 Channel) */
    return nPeriod * (24 * sizeof(WORD)) * 6 + (sizeof(TIMEDATE) + sizeof(UINT) * 6);
}

//////////////////////////////////////////////////////////////////////
BOOL CSx1Parser::fnPreClearTamper(ONDEMANDPARAM *pOndemand)
{
    SX_METERING_INFO  *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;
    if(sxInfo != NULL && (sxInfo->tamperFlag & 0xFFFFF) != 0x00) { return TRUE; }
    return FALSE;
}

/** Event Table을 읽을지 결정 */
BOOL CSx1Parser::fnPreBypassEarth(ONDEMANDPARAM *pOndemand)
{
    SX_METERING_INFO  *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;
    if(sxInfo != NULL && (sxInfo->tamperFlag & 0x01) != 0x00) { return TRUE; }
    return FALSE;
}

/** Event Table을 읽을지 결정 */
BOOL CSx1Parser::fnPreReverse(ONDEMANDPARAM *pOndemand)
{
    SX_METERING_INFO  *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;
    if(sxInfo != NULL && (sxInfo->tamperFlag & 0x10) != 0x00) { return TRUE; }
    return FALSE;
}

/** Event Table을 읽을지 결정 */
BOOL CSx1Parser::fnPreOverload(ONDEMANDPARAM *pOndemand)
{
    SX_METERING_INFO  *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;
    if(sxInfo != NULL && (sxInfo->tamperFlag & 0x100) != 0x00) { return TRUE; }
    return FALSE;
}

/** Event Table을 읽을지 결정 */
BOOL CSx1Parser::fnPreFrontCover(ONDEMANDPARAM *pOndemand)
{
    SX_METERING_INFO  *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;
    if(sxInfo != NULL && (sxInfo->tamperFlag & 0x1000) != 0x00) { return TRUE; }
    return FALSE;
}

/** Event Table을 읽을지 결정 */
BOOL CSx1Parser::fnPreTerminalCover(ONDEMANDPARAM *pOndemand)
{
    SX_METERING_INFO  *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;
    if(sxInfo != NULL && (sxInfo->tamperFlag & 0x10000) != 0x00) { return TRUE; }
    return FALSE;
}

extern BOOL     m_bEnableMeterTimesync;
extern int      m_nTimesyncThresholdLow;
extern int      m_nTimesyncThresholdHigh;

/** 시간동기화를 수행할지 결정한다  */
BOOL CSx1Parser::fnPreSyncTime(ONDEMANDPARAM *pOndemand)
{
    // Meter 시간동기화가 disable 이면 수행하지 않는다
    if(m_bEnableMeterTimesync == FALSE) return FALSE;

    SX_METERING_INFO  *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;
    if(sxInfo != NULL)
    {
        /** 지정된 TimesyncThreshold 보다 작으면 동기화 하지 않는다 */
        if(m_nTimesyncThresholdLow > 0 && sxInfo->timeGap < m_nTimesyncThresholdLow) return FALSE;
        /** 지정된 TimesyncThreshold 보다 크면 동기화 하지 않는다 */
        if(m_nTimesyncThresholdHigh > 0 && sxInfo->timeGap > m_nTimesyncThresholdHigh) return FALSE;
        sxInfo->executeTimesync = TRUE;
        return TRUE;
    }
    return FALSE;
}

/** 미터 시간을 다시 읽을지 결정한다  */
BOOL CSx1Parser::fnPreRereadTime(ONDEMANDPARAM *pOndemand)
{
    SX_METERING_INFO  *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;
    if(sxInfo != NULL && sxInfo->executeTimesync == TRUE) { return TRUE; }
    return FALSE;
}


//////////////////////////////////////////////////////////////////////
void CSx1Parser::OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
{
    DATASTREAM      *pStream;

    if(pOndemand)
    {
        pOndemand->stat.nCount ++;
        pOndemand->stat.nRecvCount ++;
        pOndemand->stat.nRecv += nLength + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);

        pStream = pOndemand->pDataStream;
        if(pStream && !ParseStream(pStream, pszData, nLength, pOndemand))
        {
            /** 아직 Frame이 완료되지 않았을 때 */
            XDEBUG(" PENDDING FRAME CURR=%d\r\n", pStream->nLength);
        }
    }
}


//////////////////////////////////////////////////////////////////////
// CSx1Parser Operations
//////////////////////////////////////////////////////////////////////

BOOL CSx1Parser::OnAck(DATASTREAM *pStream, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    BOOL res = CIec21DataStream::OnAck(pStream, pCallData);

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex ++;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return res;
}

BOOL CSx1Parser::OnNak(DATASTREAM *pStream, void *pCallData)
{
    return CIec21DataStream::OnNak(pStream, pCallData);
}

BOOL CSx1Parser::OnBreakFrame(DATASTREAM *pStream, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    BOOL res = CIec21DataStream::OnBreakFrame(pStream, pCallData);

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex = 0xFF;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return res;
}

BOOL CSx1Parser::OnIdentificationFrame(DATASTREAM *pStream, BYTE* manufacturerId, BYTE baud, BYTE *serialSetting, BYTE * identification, 
        int nLength, void *pCallData)
{
    return CIec21DataStream::OnIdentificationFrame(pStream, manufacturerId, baud, serialSetting, identification, nLength, pCallData);
}

BOOL CSx1Parser::OnCommandFrame(DATASTREAM *pStream, BYTE cmd, BYTE cmdType, BYTE *pszBuffer, int nLength, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    BOOL res = CIec21DataStream::OnCommandFrame(pStream, cmd, cmdType, pszBuffer, nLength, pCallData);

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex ++;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return res;
}

BOOL CSx1Parser::OnDataFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    UINT value = 0;
    UINT resId = 0;
    BYTE resLen = 0;
    char strBuffer[512];
    BOOL bValid = FALSE;
    SX_METERING_INFO  *sxInfo = (SX_METERING_INFO *)pOndemand->pSession;
    BOOL res = FALSE;
    char *tok, *last = NULL;

    if(pszBuffer == NULL || nLength == 0)
    {
        return res;
    }

    memset(strBuffer, 0, sizeof(strBuffer));

    tok = strtok_r((char *)pszBuffer, "\r\n", &last);
    if(tok == NULL)
    {
        return res;
    }

    while(tok != NULL)
    {
        sscanf((const char *)tok, "%02X(%ud)", &resId, &value);
        if(iec21GetData((const char *)tok, NULL, strBuffer)) {
            bValid = TRUE;
            resLen = strlen(strBuffer);
            pOndemand->pMtrChunk->Add((BYTE)(resLen + 1));
            pOndemand->pMtrChunk->Add((BYTE)(resId));
            pOndemand->pMtrChunk->Add(strBuffer, resLen);
        }
        tok = strtok_r(NULL, "\r\n", &last);
    }

    switch(resId) {
        case 0x00: // Meter identification number
            {
                if(bValid) 
                {
                    XDEBUG(" METER SERIAL : %s\r\n", strBuffer);
                    m_pEndDeviceList->UpdateMeterInventory(&pOndemand->id, 
                            UNDEFINED_PORT_NUMBER, PARSER_TYPE_SX,
                            GetPrefixVendor(), METER_VENDOR_MITSUBISHI, 
                            NULL, strBuffer, 0, 0);
                }
            }
            break;
        case 0x34: // Meter Time
            {
                cetime_t meterTimeT=0;
                if(bValid) {
                    XDEBUG(" METER TIME : %s\r\n", strBuffer);
                    ConvHexaTimeStr2TimeT(strBuffer, &meterTimeT);
                    MakeTimeString(strBuffer, &meterTimeT, TRUE);
                    XDEBUG(" METER TIME : %s\r\n", strBuffer);
                    if(sxInfo != NULL) 
                    {
                        // Meter 시간 동기화를 위해 현재시간과의 차이값 구하기
                        cetime_t now;
	                    time(&now);
                        sxInfo->timeGap = abs(now - meterTimeT);
                    }
                }
            }
            break;
        case 0x35: // Battery Status
            XDEBUG(" BATTERY STATUS : %u %s\r\n", value, value ? "Normal" : "Low Battery");
            break;
        case 0xD7: // Display Energy
            /** 태국에서 더이상 Display Energy를 사용하지 않는다고 연락이 와서
              * CurPulse를 Forward Energy로 변경한다
              * Issue #619 ~1644
            pOndemand->nCurPulse = value;
            XDEBUG(" CURRENT PULSE : %u\r\n", pOndemand->nCurPulse);
              */
            break;
        case 0xD0: // Voltage
            if(sxInfo != NULL) sxInfo->voltage = value;
            break;
        case 0xD1: // Neutral Current
            if(sxInfo != NULL) sxInfo->neutralCurrent = value;
            break;
        case 0xD2: // Line Current
            if(sxInfo != NULL) sxInfo->lineCurrent = value;
            break;
        case 0xD5: // Forward Energy
            pOndemand->nCurPulse = value;
            XDEBUG(" CURRENT PULSE : %u\r\n", pOndemand->nCurPulse);
            if(sxInfo != NULL) sxInfo->forwardEnergy = value;
            break;
        case 0xD6: // Reverse Energy
            if(sxInfo != NULL) sxInfo->reverseEnergy = value;
            break;
        case 0xE7: // Tamper code
            if(sxInfo != NULL) 
            {
                sxInfo->tamperFlag = strtol(strBuffer, (char **)NULL, 16);
                XDEBUG(" TAMPER STATUS : 0x%06X\r\n", sxInfo->tamperFlag);
            }
            break;
        case 0xEC: // Power Factor
            if(sxInfo != NULL) sxInfo->powerFactor = value;
            break;
        case 0xF4:  // Relay Status
            XDEBUG(" RELAY STATUS : %u %s\r\n", value, !value ? "Disconnect" : "Connect");
            switch(value) {
                case 0x00: pOndemand->nEnergyLevel = ENERGY_LEVEL_MAX; break;
                case 0x01: pOndemand->nEnergyLevel = ENERGY_LEVEL_MIN; break;
                default: pOndemand->nEnergyLevel = ENERGY_LEVEL_UNDEFINED; break;
            }

            /** Energy Level Update */
            m_pEndDeviceList->SetEnergyLevel(&pOndemand->id, pOndemand->nEnergyLevel, value);
            XDEBUG(" ENERGY LEVEL : %d\r\n", pOndemand->nEnergyLevel);
            break;
        default:
            XDEBUG(" RES : %s\r\n", strBuffer);
            break;
    }

    res = CIec21DataStream::OnDataFrame(pStream, pszBuffer, nLength, pCallData);

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex ++;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return res;
}


