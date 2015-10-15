//////////////////////////////////////////////////////////////////////
//
//  kamstrup.cpp: implementation of the CKamstrupParser class.
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
#include "AgentService.h"
#include "MeterUploader.h"
#include "MeterParserFactory.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"
#include "Patch.h"

#include "ParserKamstrup.h"

#include "vendor/pollmiu.h"
#include "kmpFrame.h"
#include "kmpFunctions.h"

#if     defined(__PATCH_11_65__)
/**< Issue #65 JEJU SP */
extern BOOL    *m_pbPatch_11_65;
#endif

/** Issue #837 : Meter Time Sync*/
extern int      m_nTimesyncThresholdLow;
extern int      m_nTimesyncThresholdHigh;
extern BOOL     m_bEnableMeterTimesync;

/**< Issue #345 Kamstrup DLMS Reading */
//#define KAMSTRUP_DLMS_READING

/**< Issue #835 Memo ~1916 Kamstrup Model 정의 */
#define KAMSTRUP_METER_TYPE_162_382_BCD     0x00000001          // Modem LP No Meter LP
#define KAMSTRUP_METER_TYPE_162_382_J       0x00000002          // Meter LP
#define KAMSTRUP_METER_TYPE_162_382_L       0x00000002          // Meter LP (DLMS 지원 가능)
#define KAMSTRUP_METER_TYPE_351_BC          0x00000100          // Meter LP
#define KAMSTRUP_METER_TYPE_UNKNOWN         0x80000000          // Unknown Kamstrup Meter
#define KAMSTRUP_METER_TYPE_ALL             0xFFFFFFFF

#define SAVE                        TRUE
#define NOT_SAVE                    FALSE

#define LP_READ_MAX_COUNT           8

//////////////////////////////////////////////////////////////////////
// CKamstrupParser Class
//////////////////////////////////////////////////////////////////////

typedef struct {
    //KAMSTRUP Variable
    BOOL            bSave;
    BOOL            bDlmsMeter;             // Kamstrup Meter 중 DLMS Reading이 필요한 것들
    WORD            nLogId;                 // Log ID
    BOOL            bMeterTimeSync;         // Issue #837 : Meter 시간 동기화가 필요할 때 true 
    int             nMeterType;             // Kamstrup Meter Type

    int             nMeterLpCount;          // Meter LP Count
    BYTE            nLpInterval;            // LP Interval (minute)
    WORD            nNextLogId;             // 읽어야 하는 Log Id
    WORD            nRemainCnt;             // LP를 읽을 때 남은 count 수
} KMPSESSION;

typedef enum
{
    KMP_Q_GENERAL,
    KMP_Q_METER_TYPE,
    KMP_Q_ACTIVE_ENERGY,
    KMP_Q_VOLTAGE,
    KMP_Q_METER_SERIAL,
    KMP_Q_LOOKUP,
    KMP_Q_LP_CONFIG,        // LP Configuration
    KMP_Q_LAST_LP_LOOKUP,   // 마지막 LP 기록 시간 확인
    KMP_Q_LP,
    KMP_Q_RELAY_STATUS,
    KMP_Q_METER_TIME        // Meter 시간 조회. 이 정보를 이용해서 시간 동기화를 할 수 있게 된다.
} KMP_METERING_QUERY_TYPE;

typedef struct _tagKMPCIDTBL
{
    BYTE                        cid;
    UINT                        meterType;
    BYTE                        *data;
    BYTE                        nLen;
    KMP_METERING_QUERY_TYPE     type;
    BOOL                        bSave;
    const char                  *desc;
} KMPCIDTBL;

BYTE _regMeterSerial[] =            { 0x01, 0x03, 0xE9 };
BYTE _regPulse[] =                  { 0x01, 0x00, 0x3A };
BYTE _regActualTariff[] =           { 0x01, 0x07, 0xDA };
BYTE _regActiveAll[] =              { 0x01, 0x04, 0x07 };

BYTE _regActive[] =                 { 0x02, 0x00, 0x0D, 0x00, 0x01 };

BYTE _regMeterNumber[] =            { 0x04, 0x03, 0xF2, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35 };
BYTE _regMeterInfo[] =              { 0x04, 0x04, 0x22, 0x00, 0x32, 0x03, 0xED, 0x04, 0x08 };
BYTE _regActiveNReactive[] =        { 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04 };
BYTE _regActiveNReactiveDetail[] =  { 0x04, 0x00, 0x0D, 0x00, 0x0E, 0x00, 0x0F, 0x00, 0x10 };

BYTE _regMaxPower[] =               { 0x05, 0x04, 0x19, 0x04, 0x09, 0x04, 0x1A, 0x04, 0x0C, 0x04, 0x1B };
BYTE _regDateTime[] =               { 0x05, 0x03, 0xEB, 0x03, 0xEA, 0x03, 0xEC, 0x04, 0x17, 0x04, 0x15 };
BYTE _regActiveNVoltage[] =         { 0x05, 0x00, 0x0D, 0x00, 0x01, 0x04, 0x1E, 0x04, 0x1F, 0x04, 0x20 };

BYTE _regPhaseValue[] =             { 0x06, 0x04, 0x1E, 0x04, 0x1F, 0x04, 0x20, 0x04, 0x34, 0x04, 0x35, 0x04, 0x36 };
BYTE _regPhaseAvgValue[] =          { 0x06, 0x04, 0xBF, 0x04, 0xC0, 0x04, 0xC1, 0x04, 0xC2, 0x04, 0xC3, 0x04, 0xC4 };
BYTE _regActiveNTariff[] =          { 0x06, 0x00, 0x0D, 0x00, 0x01, 0x00, 0x13, 0x00, 0x17, 0x00, 0x1B, 0x00, 0x1F };

// Issue #345 Kamstrup Billing Log
BYTE _debitloglookup[] =            {0x01, 0x01, 0x04, 0x17, 
                                        0x02, 0xFF,
                                        0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
BYTE _logBilling[] =                {0x01, 0x05, 0x04, 0x17, 0x00, 0x01, 0x00, 0x13, 0x00, 0x17, 0x00, 0x1F, 
                                        0x04, 0xFF, 
                                        0x00, 0x00};
BYTE _logPeek[] =                   {0x01, 0x03, 0x04, 0x19, 0x00, 0x27, 0x00, 0x2B, 
                                        0x05, 0xFF,
                                        0x00, 0x00};
BYTE _351debitloglookup[] =         {0x07, 0x01, 0x04, 0x17, 
                                        0x02, 0xFF,
                                        0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
BYTE _351logBilling[] =             {0x07, 0x05, 0x04, 0x17, 0x00, 0x01, 0x00, 0x13, 0x00, 0x17, 0x00, 0x1F, 
                                        0x04, 0xFF, 
                                        0x00, 0x00};
BYTE _351logPeek[] =                {0x07, 0x03, 0x04, 0x19, 0x00, 0x27, 0x00, 0x2B, 
                                        0x05, 0xFF,
                                        0x00, 0x00};
// Issue #345 Kamstrup Billing Log
BYTE _lploglookup[] =               {0x06, 0x01, 0x04, 0x17, 
                                        0x02, 0xFF,
                                        0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
BYTE _lastLpLookup[] =              {0x06, 0x01, 0x04, 0x17,
                                        0x01, 0xFF,    // count 1
                                        0x00, 0x00};
BYTE _logLp[] =                     {0x06, 0x05, 0x04, 0x17, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 
                                        0x06, 0xFF,
                                        0x00, 0x00};

/** Table 구성시 유의 사항
  *
  * KMP_CID_GET_LOG_ID_PRESENT 를 사용하기 전에 반듯이 KMP_Q_LOOKUP을 가지는 KMP_CID_GET_LOG_TIMEPRESENT 를 이용해서
  * LogId에 대한 Lookup을 수행해야 한다.
  */
KMPCIDTBL optTbl_00[] = 
{
    {KMP_CID_GET_TYPE,              KAMSTRUP_METER_TYPE_ALL,   
        NULL,                  0,                              KMP_Q_METER_TYPE,       NOT_SAVE,   "Get Meter Type"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regMeterSerial,       sizeof(_regMeterSerial),        KMP_Q_METER_SERIAL,     SAVE,       "Get Meter Serial Number"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActiveNTariff,     sizeof(_regActiveNTariff),      KMP_Q_ACTIVE_ENERGY,    SAVE,       "Get Active & Tariff"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regPulse,             sizeof(_regPulse),              KMP_Q_GENERAL,          SAVE,       "Get Pulse Count"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regPhaseValue,        sizeof(_regPhaseValue),         KMP_Q_GENERAL,          SAVE,       "Get Voltage & Current"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regMaxPower,          sizeof(_regMaxPower),           KMP_Q_GENERAL,          SAVE,       "Get Max Power"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regDateTime,          sizeof(_regDateTime),           KMP_Q_METER_TIME,       SAVE,       "Get Date"},
    {KMP_CID_SET_CLOCK,             KAMSTRUP_METER_TYPE_ALL,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   "Meter Time Sync"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regMeterNumber,       sizeof(_regMeterNumber),        KMP_Q_GENERAL,          SAVE,       "Get Meter Number"},
    
    /** Debiting 162/382 logger **/
    {KMP_CID_GET_LOG_TIME_PRESENT,  KAMSTRUP_METER_TYPE_162_382_J | KAMSTRUP_METER_TYPE_162_382_L , // 162, 382 L 이상
        _debitloglookup,       sizeof(_debitloglookup),        KMP_Q_LOOKUP,           NOT_SAVE,   "Lookup Log ID"},
    {KMP_CID_GET_LOG_ID_PRESENT,    KAMSTRUP_METER_TYPE_162_382_J | KAMSTRUP_METER_TYPE_162_382_L , // 162, 382 L 이상
        _logBilling,           sizeof(_logBilling),            KMP_Q_GENERAL,          SAVE,       "Get Billing Log"},
    {KMP_CID_GET_LOG_ID_PRESENT,    KAMSTRUP_METER_TYPE_162_382_J | KAMSTRUP_METER_TYPE_162_382_L , // 162, 382 L 이상
        _logPeek,              sizeof(_logPeek),               KMP_Q_GENERAL,          SAVE,       "Get Peek Log"},
    /** Debiting 351 logger **/
    {KMP_CID_GET_LOG_TIME_PRESENT,  KAMSTRUP_METER_TYPE_351_BC , // 351 Meter
        _351debitloglookup,    sizeof(_351debitloglookup),     KMP_Q_LOOKUP,           NOT_SAVE,   "Lookup Log ID (for 351)"},
    {KMP_CID_GET_LOG_ID_PRESENT,    KAMSTRUP_METER_TYPE_351_BC , // 351 Meter
        _351logBilling,        sizeof(_351logBilling),         KMP_Q_GENERAL,          SAVE,       "Get Billing Log (for 351)"},
    {KMP_CID_GET_LOG_ID_PRESENT,    KAMSTRUP_METER_TYPE_351_BC , // 351 Meter
        _351logPeek,           sizeof(_351logPeek),            KMP_Q_GENERAL,          SAVE,       "Get Peek Log (for 351)"},

    /** Load Profile logger **/
    {KMP_CID_GET_LOAD_PROFILE_CONFIG,  ~KAMSTRUP_METER_TYPE_162_382_BCD , // BCD는 읽지 않는다
        NULL,                  0,                              KMP_Q_LP_CONFIG,        SAVE,       "Load Profile Config"},
    {KMP_CID_GET_LOG_TIME_PRESENT,  ~KAMSTRUP_METER_TYPE_162_382_BCD , // BCD는 읽지 않는다
        _lploglookup,          sizeof(_lploglookup),           KMP_Q_LOOKUP,           NOT_SAVE,   "Lookup Lp Log ID"},
    {KMP_CID_GET_LOG_ID_PRESENT,    ~KAMSTRUP_METER_TYPE_162_382_BCD , // BCD는 읽지 않는다
        _lastLpLookup,         sizeof(_lastLpLookup),          KMP_Q_LAST_LP_LOOKUP,   NOT_SAVE,   "Last Lp Time Lookup"},
    {KMP_CID_GET_LOG_ID_PRESENT,    ~KAMSTRUP_METER_TYPE_162_382_BCD , // BCD는 읽지 않는다
        _logLp,                sizeof(_logLp),                 KMP_Q_LP,               SAVE,       "Get Load Profile"},

    /** Relay Status */
    {KMP_CID_GET_CUT_OFF_STATE,     ~KAMSTRUP_METER_TYPE_351_BC , // 162, 382 Meter 만
        NULL,                  0,                              KMP_Q_RELAY_STATUS,     NOT_SAVE,   "Get Relay Status"},

    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

KMPCIDTBL optTbl_01[] = 
{
    {KMP_CID_GET_TYPE,              KAMSTRUP_METER_TYPE_ALL,
        NULL,                  0,                              KMP_Q_METER_TYPE,       NOT_SAVE,   "Get Meter Type"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regMeterSerial,       sizeof(_regMeterSerial),        KMP_Q_METER_SERIAL,     NOT_SAVE,   "Get Meter Serial Number"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActiveAll,         sizeof(_regActiveAll),          KMP_Q_GENERAL,          SAVE,       "Get Actual All Energy"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActive,            sizeof(_regActive),             KMP_Q_ACTIVE_ENERGY,    SAVE,       "Get Active"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regPhaseValue,        sizeof(_regPhaseValue),         KMP_Q_GENERAL,          SAVE,       "Get Voltage & Current"},
    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

KMPCIDTBL optTbl_02[] = 
{
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regPulse,             sizeof(_regPulse),              KMP_Q_GENERAL,          NOT_SAVE,   "Get Pulse Count"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActiveNReactive,   sizeof(_regActiveNReactive),    KMP_Q_GENERAL,          SAVE,       "Get Active & Reactive Energy"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActiveNReactiveDetail,sizeof(_regActiveNReactiveDetail),KMP_Q_ACTIVE_ENERGY,SAVE,      "Get Active & Reactive Energy (Detail)"},
    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

KMPCIDTBL optTbl_03[] = 
{
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActive,            sizeof(_regActive),             KMP_Q_ACTIVE_ENERGY,    NOT_SAVE,   "Get Active"},
    {KMP_CID_GET_CUT_OFF_STATE,     KAMSTRUP_METER_TYPE_ALL,
        NULL,               0,                                  KMP_Q_RELAY_STATUS,    SAVE,       "Get Relay Status"},
    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

KMPCIDTBL optTbl_08[] = 
{
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActive,            sizeof(_regActive),             KMP_Q_ACTIVE_ENERGY,    NOT_SAVE,   "Get Active"},
    {KMP_CID_SET_CLOCK,             KAMSTRUP_METER_TYPE_ALL,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   "Meter Time Sync"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regDateTime,          sizeof(_regDateTime),           KMP_Q_GENERAL,          SAVE,       "Get Date"},
    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

KMPCIDTBL optTbl_11[] = 
{
    {KMP_CID_GET_TYPE,              KAMSTRUP_METER_TYPE_ALL,
        NULL,                  0,                              KMP_Q_METER_TYPE,       SAVE,       "Get Meter Type"},
    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

KMPCIDTBL optTbl_12[] = 
{
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActive,            sizeof(_regActive),             KMP_Q_ACTIVE_ENERGY,    NOT_SAVE,   "Get Active"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regPhaseValue,        sizeof(_regPhaseValue),         KMP_Q_GENERAL,          SAVE,       "Get Voltage & Current"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regPhaseAvgValue,     sizeof(_regPhaseAvgValue),      KMP_Q_GENERAL,          SAVE,       "Get Voltage & Current (Avg.)"},
    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

KMPCIDTBL optTbl_13[] = 
{
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActive,            sizeof(_regActive),             KMP_Q_ACTIVE_ENERGY,    NOT_SAVE,   "Get Active"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regMaxPower,          sizeof(_regMaxPower),           KMP_Q_GENERAL,          SAVE,       "Get Max Power"},
    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

KMPCIDTBL optTbl_14[] = 
{
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActive,            sizeof(_regActive),             KMP_Q_ACTIVE_ENERGY,    NOT_SAVE,   "Get Active"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regMeterSerial,       sizeof(_regMeterSerial),        KMP_Q_METER_SERIAL,     SAVE,       "Get Meter Serial Number"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regMeterNumber,       sizeof(_regMeterNumber),        KMP_Q_GENERAL,          SAVE,       "Get Meter Number"},
    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};


KMPCIDTBL optTbl_15[] = 
{
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActive,            sizeof(_regActive),             KMP_Q_ACTIVE_ENERGY,    NOT_SAVE,   "Get Active"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regDateTime,          sizeof(_regDateTime),           KMP_Q_GENERAL,          SAVE,       "Get Date"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regMeterInfo,         sizeof(_regMeterInfo),          KMP_Q_GENERAL,          SAVE,       "Get Meter Inventory"},
    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

KMPCIDTBL optTbl_16[] = 
{
    {KMP_CID_GET_TYPE,              KAMSTRUP_METER_TYPE_ALL,
        NULL,                  0,                              KMP_Q_METER_TYPE,       NOT_SAVE,   "Get Meter Type"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regActive,            sizeof(_regActive),             KMP_Q_ACTIVE_ENERGY,    NOT_SAVE,   "Get Active"},
    {KMP_CID_GET_REGISTER,          KAMSTRUP_METER_TYPE_ALL,
        _regMeterSerial,       sizeof(_regMeterSerial),        KMP_Q_METER_SERIAL,     SAVE,       "Get Meter Serial Number"},

    /** Debiting 162/382 logger **/
    {KMP_CID_GET_LOG_TIME_PRESENT,  KAMSTRUP_METER_TYPE_162_382_J | KAMSTRUP_METER_TYPE_162_382_L , // 162, 382 L 이상
        _debitloglookup,       sizeof(_debitloglookup),        KMP_Q_LOOKUP,           NOT_SAVE,   "Lookup Log ID"},
    {KMP_CID_GET_LOG_ID_PRESENT,    KAMSTRUP_METER_TYPE_162_382_J | KAMSTRUP_METER_TYPE_162_382_L , // 162, 382 L 이상
        _logBilling,           sizeof(_logBilling),            KMP_Q_GENERAL,          SAVE,       "Get Billing Log"},
    {KMP_CID_GET_LOG_ID_PRESENT,    KAMSTRUP_METER_TYPE_162_382_J | KAMSTRUP_METER_TYPE_162_382_L , // 162, 382 L 이상
        _logPeek,              sizeof(_logPeek),               KMP_Q_GENERAL,          SAVE,       "Get Peek Log"},
    /** Debiting 351 logger **/
    {KMP_CID_GET_LOG_TIME_PRESENT,  KAMSTRUP_METER_TYPE_351_BC , // 351 Meter
        _351debitloglookup,    sizeof(_351debitloglookup),     KMP_Q_LOOKUP,           NOT_SAVE,   "Lookup Log ID (for 351)"},
    {KMP_CID_GET_LOG_ID_PRESENT,    KAMSTRUP_METER_TYPE_351_BC , // 351 Meter
        _351logBilling,        sizeof(_351logBilling),         KMP_Q_GENERAL,          SAVE,       "Get Billing Log (for 351)"},
    {KMP_CID_GET_LOG_ID_PRESENT,    KAMSTRUP_METER_TYPE_351_BC , // 351 Meter
        _351logPeek,           sizeof(_351logPeek),            KMP_Q_GENERAL,          SAVE,       "Get Peek Log (for 351)"},

    /** Load Profile logger **/
    {KMP_CID_GET_LOAD_PROFILE_CONFIG,  ~KAMSTRUP_METER_TYPE_162_382_BCD , // BCD는 읽지 않는다
        NULL,                  0,                              KMP_Q_LP_CONFIG,        SAVE,       "Load Profile Config"},
    {KMP_CID_GET_LOG_TIME_PRESENT,  ~KAMSTRUP_METER_TYPE_162_382_BCD , // BCD는 읽지 않는다
        _lploglookup,          sizeof(_lploglookup),           KMP_Q_LOOKUP,           NOT_SAVE,   "Lookup Lp Log ID"},
    {KMP_CID_GET_LOG_ID_PRESENT,    ~KAMSTRUP_METER_TYPE_162_382_BCD , // BCD는 읽지 않는다
        _lastLpLookup,         sizeof(_lastLpLookup),          KMP_Q_LAST_LP_LOOKUP,   NOT_SAVE,   "Last Lp Time Lookup"},
    {KMP_CID_GET_LOG_ID_PRESENT,    ~KAMSTRUP_METER_TYPE_162_382_BCD , // BCD는 읽지 않는다
        _logLp,                sizeof(_logLp),                 KMP_Q_LP,               SAVE,       "Get Load Profile"},

    {0x00,                          0,
        NULL,                  0,                              KMP_Q_GENERAL,          NOT_SAVE,   NULL}
};

static int m_nOptionLastIdx = 16;
KMPCIDTBL *m_pMeteringOptionTbl[] =
{
    optTbl_00,
    optTbl_01,
    optTbl_02,
    optTbl_03,
    optTbl_00,
    optTbl_00,
    optTbl_00,
    optTbl_00,
    optTbl_08,
    optTbl_00,
    optTbl_00,
    optTbl_11,
    optTbl_12,
    optTbl_13,
    optTbl_14,
    optTbl_15,
    optTbl_16,
    NULL
};

typedef POLLBASED_METERING_DATA KAMST_METERING_DATA;

static const char *parserNameArray[] = {"KAMST", NULL};

//////////////////////////////////////////////////////////////////////
// CKamstrupParser Class
//////////////////////////////////////////////////////////////////////

CKamstrupParser::CKamstrupParser() : CPollBaseParser(SENSOR_TYPE_ZRU, SERVICE_TYPE_ELEC, 
                                            ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL, 
                                            const_cast<char **>(parserNameArray), PARSER_TYPE_KAMST, 
                                            "KAMST Parser", "KAMST", parserNameArray[0],
                                            6000, 6000, 2)
{
}

CKamstrupParser::~CKamstrupParser()
{
}

//////////////////////////////////////////////////////////////////////
/** DR 가능 장비인지 여부를 리턴한다.
 */
BOOL CKamstrupParser::IsDRAsset(EUI64 *pId)
{
    ENDIENTRY *pEndi;
    if(pId == NULL) return FALSE;

    /** GetEndDeviceByID 의 경우 Lock이 동작하게 된다.
      * 이 함수를 호출하는 함수 중 cmdGetDRAssetInfo(104.14)의 경우
      * Enumerate 함수를 사용하고 이 함수 안에서 Lock을 걸기 때문에
      * Lock이 걸리지 않는 FindEndDeviceByID 함수를 사용해야 한다.
      */
    pEndi = m_pEndDeviceList->FindEndDeviceByID(pId);

    if(pEndi == NULL) return FALSE;

    if(pEndi->nEnergyLevel > 0 && pEndi->nEnergyLevel <= 15)
    {
        return TRUE;
    }

    return FALSE;
}

/** 장비에서 유효한 Level 값 배열 리턴.
 *
 * @return 
 *      0 지원하지 않음
 *      positive 배열 개 멤버 수
 *      -1 NULL Parameter
 *
 * @see CKamstrupParser#IsDRAsset(EUI64 *pId);
 *
 * @warning pEnergyLevelArray는 최소 15bytes 이상의 길이를 가져야 한다.
 */
int CKamstrupParser::GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray)
{
    if(!IsDRAsset(pId)) 
    {
        return 0;
    }

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
int CKamstrupParser::GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel)
{
    CMDPARAM cmdParam;
    METER_STAT stat;
    int nError;
    BYTE nEnergyLevel=0;
	CChunk chunk(128);
    BYTE nCid = KMP_CID_GET_CUT_OFF_STATE;

    if(pId == NULL) return IF4ERR_INVALID_ID;
    if(pEnergyLevel == NULL) return IF4ERR_INVALID_PARAM;

    memset(&cmdParam, 0, sizeof(CMDPARAM));
    memset(&stat, 0, sizeof(METER_STAT));

    memcpy(&cmdParam.id, pId, sizeof(EUI64));
    cmdParam.codi      = GetCoordinator();
    cmdParam.nMeteringType = METERING_TYPE_BYPASS;
	cmdParam.pChunk	= &chunk;

    nError = DoMetering(&cmdParam, nCid, 0, 0, NULL, &stat);
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
int CKamstrupParser::SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen)
{
    CMDPARAM cmdParam;
    METER_STAT stat;
    int nError;
	CChunk chunk(128);
    BYTE nCid = KMP_CID_SET_CUT_OFF_STATE;
    BYTE cidParam[1];
    ENDIENTRY *pEndi;
    int nStep = 0;

    if(pId == NULL) return IF4ERR_INVALID_ID;
    if(nEnergyLevel <= 0 || nEnergyLevel > ENERGY_LEVEL_MAX) return IF4ERR_INVALID_ID;
    memset(&cmdParam, 0, sizeof(CMDPARAM));
    memset(&stat, 0, sizeof(METER_STAT));

    memcpy(&cmdParam.id, pId, sizeof(EUI64));
    cmdParam.codi      = GetCoordinator();
    cmdParam.nMeteringType = METERING_TYPE_BYPASS;
	cmdParam.pChunk	= &chunk;

    pEndi = m_pEndDeviceList->FindEndDeviceByID(pId);
    if(pEndi == NULL) return IF4ERR_UNKNOWN_ERROR;

    switch(nEnergyLevel) {
        case  ENERGY_LEVEL_MIN:
            switch(pEndi->nRelayStatus)
            {
                case 0x08:
                    cidParam[0] = 0x03;
                    nStep = 2;
                    break;
                default:
                    cidParam[0] = 0x02;
                    nStep = 1;
                    break;
            }
            break;
        default:
            cidParam[0] = 0x01;
            break;
    }

    nError = DoMetering(&cmdParam, nCid, 0, 1, cidParam, &stat);
    switch(nError) {
        case ONDEMAND_ERROR_OK:
            if(nEnergyLevel == ENERGY_LEVEL_MIN && nStep == 1) // release 상태 일 때
            {
                chunk.Empty();
                USLEEP(1000000 * 20); // Kamstrup 반응 속도 때문에 20초의 Delay를 둔다
                cidParam[0] = 0x03;
                nError = DoMetering(&cmdParam, nCid, 0, 1, cidParam, &stat);
            }
            break;
        case ONDEMAND_ERROR_NOT_CONNECT:
        case ONDEMAND_ERROR_INIT:
        case ONDEMAND_ERROR_BUSY:
            return IF4ERR_CANNOT_CONNECT;
        default:
            return IF4ERR_UNKNOWN_ERROR;
    }
    // 다시 읽어본다
    if(nError == ONDEMAND_ERROR_OK) // connect 시도시만 읽어본다
    {
        BYTE energyLevel;
        USLEEP(1000000 * 20); // Kamstrup 반응 속도 때문에 20초의 Delay를 둔다
        nError = GetEnergyLevel(pId, &energyLevel);
    }
    return nError;
}

//////////////////////////////////////////////////////////////////////
/** Kamstrup Meter Type 설정.
  * 여기서 설정되는 Kamstrup Meter Type을 이용해서 검침을 제어한다
 */
void CKamstrupParser::GetKamstrupMeterType(EUI64 *pId, ONDEMANDPARAM *pOndemand, void* pSession)
{
    ENDIENTRY *pEndi;
    int nCount;

    // Bypass일 경우 LP 여부를 조정하지 않는다
    if(pOndemand->nMeteringType == METERING_TYPE_BYPASS) return;

    if(pSession == NULL) return;

    KMPSESSION *pKmpSession = (KMPSESSION *)pSession;

    if(pId == NULL) return;

    pEndi = m_pEndDeviceList->GetEndDeviceByID(pId);
    if(pEndi == NULL) return;

    // LP를 읽지 않도록 설정한다
    nCount = pOndemand->nCount;
    pOndemand->nCount = 0;

    if(strncasecmp(pEndi->szModel,"K162", 4)==0 || 
       strncasecmp(pEndi->szModel,"K282", 4)==0 ||
       strncasecmp(pEndi->szModel,"K382", 4)==0)
    {
        switch(pEndi->szModel[4])
        {
            case 'B': case 'C': case 'D': 
            case 'E': case ' ': 
                pKmpSession->nMeterType = KAMSTRUP_METER_TYPE_162_382_BCD;
                // BCD 미터는 LP를 읽게 한다
                pOndemand->nCount = nCount;
                break;
            case 'J': case 'K':
                pKmpSession->nMeterType = KAMSTRUP_METER_TYPE_162_382_J;
                pKmpSession->nMeterLpCount = nCount;
                break;
            default:
                pKmpSession->nMeterType = KAMSTRUP_METER_TYPE_162_382_L;
                pKmpSession->nMeterLpCount = nCount;
                break;
        }
    }
    else if(strncasecmp(pEndi->szModel,"K251", 4)==0 || 
            strncasecmp(pEndi->szModel,"K351", 4)==0)
    {
        pKmpSession->nMeterType = KAMSTRUP_METER_TYPE_351_BC;
        pKmpSession->nMeterLpCount = nCount;
    }

}

void CKamstrupParser::ActPreAction(ONDEMANDPARAM *pOndemand)
{
    if(pOndemand != NULL) {
        pOndemand->bMeterConstants = TRUE;
        pOndemand->bVoltageScale = FALSE;
    }
    pOndemand->pSession = (void*)MALLOC(sizeof(KMPSESSION));
    memset(pOndemand->pSession, 0, sizeof(KMPSESSION));

    // Default Unknown Type Meter
    ((KMPSESSION*)pOndemand->pSession)->nMeterType = KAMSTRUP_METER_TYPE_UNKNOWN;

    switch(pOndemand->nOption)
    {
        case ONDEMAND_OPTION_TIME_SYNC:
            ((KMPSESSION*)pOndemand->pSession)->bMeterTimeSync = TRUE;
            break;
        default:
            GetKamstrupMeterType(&pOndemand->id, pOndemand, pOndemand->pSession);
            break;
    }
}

void CKamstrupParser::ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData)
{
    CPollBaseParser::ActPostSuccAction(pChunk, pOndemand, pExtraData);
    if(pOndemand != NULL && pOndemand->pSession != NULL)
    {
        FREE(pOndemand->pSession);
        pOndemand->pSession = NULL;
    }
}

void CKamstrupParser::ActPostFailAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData)
{
    CPollBaseParser::ActPostFailAction(pChunk, pOndemand, pExtraData);
    if(pOndemand != NULL && pOndemand->pSession != NULL)
    {
        FREE(pOndemand->pSession);
        pOndemand->pSession = NULL;
    }
}


int CKamstrupParser::StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    int nState = ReadRomMap(ENDI_ROMAP_NODE_KIND, sizeof(ENDI_NODE_INFO), pOndemand, szBuffer, 
            STATE_READ_NODEINFO, STATE_READ_METERTYPE);
    if(nState == STATE_READ_AMRINFO) {
        /** Kamstrup은 F/W 버전에 따라 MeterConstant 지원 여부가 변경된다.
         *
         * RF-8 Meter 상수를 읽는다.
         *
         * Kamstrup MIU F/W v2.2 이상부터 지원한다
         */
        if(((ENDI_NODE_INFO *) szBuffer)->FIRMWARE_VERSION > 0x21)
        {
            pOndemand->bMeterConstants = TRUE;
        }
        else
        {
            pOndemand->bMeterConstants = FALSE;
        }
    }

    return nState;
}

int CKamstrupParser::StateMeterType(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
#if defined(KAMSTRUP_DLMS_READING)
    // Issue #345
    if(pOndemand->nMeteringType != METERING_TYPE_BYPASS && pOndemand->nOption == READ_IDENT_ONLY)
    {
        return STATE_INITIALIZE;
    }
    else
    {
        return STATE_READ_METERSERIAL;
    }
#else
    /** Kamstrup에 사용되는 P110SR이 9600bps로 전송하는 Message에 대한 Send/Recv가 정상적으로
      * 동작하지 않기 때문에 2012.08 현재 Kamstrup Meter는 1200bps 8N2 의 설정으로 KMP로 읽도록
      * 한다. 따라서 STATE_ONITIALIZE로 진행하지 않는다.
      */
    return STATE_READ_METERSERIAL;
#endif
}

int CKamstrupParser::StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    pOndemand->nIndex = 0;
    pOndemand->nRetry = 0;
    return STATE_QUERY;
}

int CKamstrupParser::StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    BYTE                nFrameLen = 0;
    int                 nType = KMP_Q_GENERAL;
    int                 nTblIdx = pOndemand->nIndex;
    BYTE                nCid = (BYTE) pOndemand->nOption;
    KMPSESSION*         pSession = (KMPSESSION*)pOndemand->pSession;
    KMPCIDTBL           *pCidTbl = NULL;
    int                 nError;

    if(pOndemand->nMeteringType == METERING_TYPE_BYPASS)
    {
        /** Bypass일 경우 Option으로 전달되는 값을 CID로 사용 */
        XDEBUG(" KMP QUERY : %s %02X %d\r\n", pOndemand->szId, nCid, pOndemand->nCount);

        nFrameLen = 0; 
        switch(nCid)
        {
            case KMP_CID_SET_CUT_OFF_STATE:
            case KMP_CID_GET_CUT_OFF_STATE:
                nType = KMP_Q_RELAY_STATUS;
                break;
            default:
                nType = KMP_Q_GENERAL;
                break;
        }
        nError = kmpMakeFrame(szBuffer, 
                nCid,
                (BYTE *)pOndemand->pBypassData,
                pOndemand->nCount,
                &nFrameLen);
        pSession->bSave = TRUE;
    }
    else 
    {
        // KMP로 읽어야 할 경우
        pCidTbl = (KMPCIDTBL *)pOndemand->pBypassData;
        if(pCidTbl == NULL) return STATE_ERROR;

        if(pCidTbl[nTblIdx].cid == 0x00)
        {
            return STATE_OK;
        }
        // Meter Type가 일치하지 않을 경우 이 Query를 Skip 한다
        if((pCidTbl[nTblIdx].meterType & pSession->nMeterType) == 0x00000000)
        {
            //XDEBUG(" KMP QUERY SKIP : %s %s\r\n", pOndemand->szId, pCidTbl[nTblIdx].desc);

            pOndemand->nIndex ++;
            if(pCidTbl[pOndemand->nIndex].cid == 0x00)
            {
                return STATE_OK;
            }
            return STATE_QUERY;
        }

        XDEBUG(" KMP QUERY : %s %s\r\n", pOndemand->szId, pCidTbl[nTblIdx].desc);

        nFrameLen = 0; 
        nType = pCidTbl[nTblIdx].type;
        switch(pCidTbl[nTblIdx].cid)
        {
            case KMP_CID_SET_CLOCK:
                {
                    if(pSession->bMeterTimeSync)
                    {
                        BYTE buffer[16];

                        // 현재 시간 정보를 추가 한다
                        memset(buffer, 0, sizeof(buffer));
                        kmpGetTimeDateStream(buffer);

                        nError = kmpMakeFrame(szBuffer, 
                                pCidTbl[nTblIdx].cid,
                                buffer, 8,
                                &nFrameLen);
                    }
                    else
                    {
                        /** Issue #837 : 시간동기화 하지 않아야 할경우에는 다음으로 넘어간다
                          */
                        pOndemand->nResult = ONDEMAND_ERROR_OK;
                        pOndemand->nIndex ++;
                        if(pCidTbl[pOndemand->nIndex].cid == 0x00)
                        {
                            return STATE_OK;
                        }
                        return STATE_QUERY;
                    }
                }
                break;
            case KMP_CID_GET_LOG_ID_PRESENT:
                {
                    WORD nLogId=pSession->nLogId;
                    WORD bigLogId=0;
                    if(pCidTbl[nTblIdx].type == KMP_Q_LP)
                    {
                        /** Issue #841 : LP일 경우 count가 아니라 day를 사용한다 */
                        BYTE nCnt=2;

                        XDEBUG( " KMP NextLogId %d RemainCnt %d\r\n", pSession->nNextLogId, pSession->nRemainCnt);
                        if(pSession->nRemainCnt <= 0)
                        {
                            // 다 읽었을 때
                            pOndemand->nIndex ++;
                            if(pCidTbl[pOndemand->nIndex].cid == 0x00)
                            {
                                return STATE_OK;
                            }
                            return STATE_QUERY;
                        }

                        if(pSession->nRemainCnt > LP_READ_MAX_COUNT) nCnt = LP_READ_MAX_COUNT;
                        else nCnt = pSession->nRemainCnt;

                        nLogId = pSession->nNextLogId;

                        // count
                        pCidTbl[nTblIdx].data[pCidTbl[nTblIdx].nLen-4] = nCnt;
                    }
                    bigLogId = HostToBigShort(nLogId);
                    memcpy(&pCidTbl[nTblIdx].data[pCidTbl[nTblIdx].nLen-2], &bigLogId, 2);

                    nError = kmpMakeFrame(szBuffer, 
                            pCidTbl[nTblIdx].cid,
                            pCidTbl[nTblIdx].data,
                            pCidTbl[nTblIdx].nLen,
                            &nFrameLen);
                }
                break;
#if 0
            case KMP_CID_GET_LOG_TIME_PRESENT:
                {
                    BYTE buffer[255];

                    // 현재 시간 정보를 추가 한다
                    memset(buffer, 0, sizeof(buffer));
                    memcpy(buffer, pCidTbl[nTblIdx].data, pCidTbl[nTblIdx].nLen);
                    kmpGetTimeStream(buffer + pCidTbl[nTblIdx].nLen, pOndemand->nOffset + 1);
                    //memset(buffer + pCidTbl[nTblIdx].nLen, 0, 8);

                    nError = kmpMakeFrame(szBuffer, 
                            pCidTbl[nTblIdx].cid,
                            buffer,
                            pCidTbl[nTblIdx].nLen + 8,
                            &nFrameLen);
                }
                break;
#endif
            default:
                nError = kmpMakeFrame(szBuffer, 
                        pCidTbl[nTblIdx].cid,
                        pCidTbl[nTblIdx].data,
                        pCidTbl[nTblIdx].nLen,
                        &nFrameLen);
                break;
        }
        pSession->bSave = pCidTbl[nTblIdx].bSave;
    }

    if(nError == KMPERR_NOERROR)
    {
        SendCommand(pOndemand, &pOndemand->id, (const char *)szBuffer, nFrameLen);
    }
    else
    {
        XDEBUG(" MakeFrameError : %s\r\n", kmpGetErrorMessage(nError));
        return STATE_ERROR;
    }

    m_Locker.Lock();
    ClearOperation(pOndemand);
    pOndemand->nState = nType;
    pOndemand->bReply = FALSE;
    ResetEvent(pOndemand->hEvent);
    m_Locker.Unlock();

    return STATE_WAIT_REPLY;
}

int CKamstrupParser::StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    KMPSESSION*         pSession = (KMPSESSION*)pOndemand->pSession;
    WaitForSingleObject(pOndemand->hEvent, (int)(GetReplyTimeout()/1000));
    if(pOndemand->bPartialWait)
    {
        pOndemand->bPartialWait = FALSE;
        m_Locker.Unlock();
        return STATE_WAIT_REPLY;
    }
    if(pOndemand->bReply)
    {
        pOndemand->nRetry = 0;
        if(pOndemand->nMeteringType == METERING_TYPE_BYPASS)
        {
            pOndemand->pMtrChunk->Add(pOndemand->pszBuffer, pOndemand->nLength);
            pOndemand->nResult = ONDEMAND_ERROR_OK;
            return STATE_OK;
        }
        else
        {
            switch(pOndemand->nState)
            {
                case KMP_Q_LP:
                    /** LP 일 경우 계속 읽기를 수행 한다 */
                    if(pSession->bSave) pOndemand->pMtrChunk->Add(pOndemand->pszBuffer, pOndemand->nLength);
                    pOndemand->nResult = ONDEMAND_ERROR_OK;
                    if(pSession->nRemainCnt > LP_READ_MAX_COUNT) 
                    {
                        pSession->nRemainCnt -= LP_READ_MAX_COUNT;
                        pSession->nNextLogId += LP_READ_MAX_COUNT;
                    }
                    else
                    {
                        pOndemand->nIndex ++;
                    }
                    return STATE_QUERY;
                default:
                    if(pSession->bSave) pOndemand->pMtrChunk->Add(pOndemand->pszBuffer, pOndemand->nLength);
                    pOndemand->nResult = ONDEMAND_ERROR_OK;
                    pOndemand->nIndex ++;
                    return STATE_QUERY;
            }
        }
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

//////////////////////////////////////////////////////////////////////
// CKamstrupParser Operations
//////////////////////////////////////////////////////////////////////
int CKamstrupParser::METERING(CMDPARAM *pCommand, METER_STAT *pStat)
{
    BYTE                nCid = 0;
    BYTE                nDataLen = 0;
    int                 nOption=0, nOffset=1, nCount=1;
    BYTE                pBypassData[256];

    if(pCommand->nMeteringType == METERING_TYPE_BYPASS)
    {
        if(pCommand->argc < 2) 
            return ONDEMAND_ERROR_PARAM;
        nCid        = pCommand->argv[1].stream.u8;
        memset(pBypassData, 0, sizeof(pBypassData));
        if(pCommand->argc>2)
        {
            nDataLen = MIN(pCommand->argv[2].len, (int)sizeof(pBypassData));
            memcpy(pBypassData, pCommand->argv[2].stream.p, nDataLen);
        }
        return DoMetering(pCommand, nCid, 0, nDataLen, pBypassData, pStat);
    }
    else
    {
        nOption = pCommand->nOption;
        if(nOption < 0 || nOption > m_nOptionLastIdx) nOption = 0;

        nOffset = pCommand->nOffset >= 0 ? pCommand->nOffset : 1;
        if (nOffset > 39) nOffset = 39;

        nCount = pCommand->nCount > 0 ? MIN(40-nOffset, pCommand->nCount) : 2;
        if (nCount <= 0) nCount = 2;

        if(nCount > nOffset + 1) nCount = nOffset + 1;

        return DoMetering(pCommand, nOption, nOffset, nCount, m_pMeteringOptionTbl[nOption], pStat);
    }
}


//////////////////////////////////////////////////////////////////////
// CKamstrupParser Operations
//////////////////////////////////////////////////////////////////////
void CKamstrupParser::OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
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

BOOL CKamstrupParser::OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    KMP_FRAME *pFrame = (KMP_FRAME *)pszBuffer;
    KMPSESSION*  pSession = (KMPSESSION*)pOndemand->pSession;
    char    pszTempBuff[128];
    BYTE    nLen = 0;
    BYTE    pszBuff0[32];
    BYTE    pszBuff1[32];
    double  v = 0.0;
    int     i;

    if(!pOndemand) return TRUE;

    /** Kamstrup KMP DATA */
    XDEBUG(" KMP OnFrame (DESTUFF): %d\r\n", nLength);
    XDUMP((const char *)pszBuffer, nLength, TRUE);

    switch(pOndemand->nState) 
    {
        case KMP_Q_METER_TYPE:
            memset(pszTempBuff, 0, sizeof(pszTempBuff)); 
            memset(pszBuff0, 0, sizeof(pszBuff0)); memset(pszBuff1, 0, sizeof(pszBuff1)); 
            sprintf((char *)pszTempBuff, "%s %s%s", 
                    kmpGetMeterType(pFrame->payload[0], pFrame->payload[1]),
                    kmpSwMajorVersion(pFrame->payload[2], (char *)pszBuff0),
                    kmpSwMinorVersion(pFrame->payload[3], (char *)pszBuff1));
            
            XDEBUG(" METER TYPE : %s\r\n", pszTempBuff); 
            m_pEndDeviceList->UpdateMeterInventory(&pOndemand->id, 
                    UNDEFINED_PORT_NUMBER, PARSER_TYPE_KAMST,
                    GetPrefixVendor(), METER_VENDOR_KAMSTRUP, 
                    pszTempBuff, NULL, 0, 0);

#if defined(KAMSTRUP_DLMS_READING)
    /** Kamstrup에 사용되는 P110SR이 9600bps로 전송하는 Message에 대한 Send/Recv가 정상적으로
      * 동작하지 않기 때문에 2012.08 현재 Kamstrup Meter는 1200bps 8N2 의 설정으로 KMP로 읽도록
      * 한다. 따라서 STATE_ONITIALIZE로 진행하지 않는다.
      */
            /** Issue #345 : DLMS Reading이 필요한지 검사 */
            pOndemand->bDlmsMeter = kmpIsDlmsMeter(pFrame->payload[0], pFrame->payload[1]);
            XDEBUG(" Is DLMS Meter : %s\r\n", pOndemand->bDlmsMeter ? "TRUE" : "FALSE");
            if(pOndemand->bDlmsMeter)
            {
	            CMeterParser	*parser;
	            char	        *szParser;
                int             attr;
                parser = m_pMeterParserFactory->SelectParser("DLMS");
                if(parser)
                {
                    szParser = parser->GetName();

				    attr = parser ? parser->GetAttribute(szParser) : ENDDEVICE_ATTR_POWERNODE;
				    m_pEndDeviceList->SetEndDeviceByID(&pOndemand->id, UNDEFINED_PORT_NUMBER, NULL,
                           parser, szParser, parser ? parser->GetParserType() : PARSER_TYPE_UNKNOWN, attr);
                }
            }
#endif
            break;
        case KMP_Q_METER_SERIAL:
            kmpGetRegValue((BYTE *)&pFrame->payload[2], &v, NULL);
            sprintf((char *)pszTempBuff, "%u", (UINT) v);
            XDEBUG(" METER SERIAL : %s\r\n", pszTempBuff);
            m_pEndDeviceList->UpdateMeterInventory(&pOndemand->id, 
                    UNDEFINED_PORT_NUMBER, PARSER_TYPE_KAMST,
                    GetPrefixVendor(), METER_VENDOR_KAMSTRUP, 
                    NULL, pszTempBuff, 0, 0);
            break;
        /** 첫번째 Data가 Active Energy A14(1) 또는 Active Energy A14 Floating(13) 이어야 한다 */
        case KMP_Q_ACTIVE_ENERGY:
            kmpGetRegValue((BYTE *)&pFrame->payload[2], &v, NULL);
            /** Meter 상수를 곱해줘야 한다 */
            pOndemand->nCurPulse = (UINT) (v * pOndemand->nMeterConstant);
            XDEBUG(" CURRENT PULSE : %u\r\n", pOndemand->nCurPulse);
            break;

        /** Relay Status */
        case KMP_Q_RELAY_STATUS:
            XDEBUG(" KMP RELAY STATUS\r\n");
            XDUMP((char *)pFrame->payload, 2);
            switch(pFrame->payload[0])
            {
                case 0x04: // Relays connected
                    m_pEndDeviceList->SetEnergyLevel(&pOndemand->id, ENERGY_LEVEL_MIN, pFrame->payload[0]);
                    break;
                case 0x01: // Relays disconnected by command
                case 0x06: // Cutoff
                case 0x07: // Cutoff Prepayment
                case 0x08: // Relays released for reconnection
                case 0x09: // Cutoff Prepayment, Lowmax expired
                case 0x0B: // Relays disconnected by push button
                    m_pEndDeviceList->SetEnergyLevel(&pOndemand->id, ENERGY_LEVEL_MAX, pFrame->payload[0]);
                    break;
                case 0x05: // Pre cutoff warning
                default:
                    // 값 설정을 할 수 없다
                    break;
            }
            break;

            /** Log ID Lookup */
        case KMP_Q_LOOKUP:
            pSession->nLogId = (WORD)BigStreamToHostLong(&pszBuffer[nLength-3-1-2], 2); // newLogId, Info, tail 
            XDEBUG(" KMP LOG ID LOOKUP (%d)\r\n", pSession->nLogId);
            break;

        case KMP_Q_LP_CONFIG:
            if(nLength > 4)
            {
                // LP Interval 조사
                switch(pFrame->payload[1])
                {
                    case 5: case 15: case 30: case 60:
                        pSession->nLpInterval = pFrame->payload[1];
                        XDEBUG(" KMP LP INTERVAL %d\r\n", pSession->nLpInterval);
                        break;
                }
            }
            break;

        case KMP_Q_LAST_LP_LOOKUP:
            for(i=4;i<nLength-3-5;) // 3 : header , 5 : rid+type_length 
            {
                if(pFrame->payload[i] == 0x04 && pFrame->payload[i+1] == 0x17) // RTC 인지 확인
                {
	                struct tm       tm_last;
                    int             nTodayCnt = 0;

                    memset(&tm_last, 0, sizeof(struct tm));
                    tm_last.tm_year = pFrame->payload[i+2+3+7] + 100; 
                    tm_last.tm_mon  = pFrame->payload[i+2+3+6] - 1;
                    tm_last.tm_mday = pFrame->payload[i+2+3+5];
                    tm_last.tm_hour = pFrame->payload[i+2+3+4];
                    tm_last.tm_min  = pFrame->payload[i+2+3+3];
                    tm_last.tm_sec  = pFrame->payload[i+2+3+2];

                    XDEBUG( " KMP LAST LP TIME = %02d/%02d/%02d %02d:%02d:%02d\r\n",
                            pFrame->payload[i+2+3+7],
                            pFrame->payload[i+2+3+6],
                            pFrame->payload[i+2+3+5],
                            pFrame->payload[i+2+3+4],
                            pFrame->payload[i+2+3+3],
                            pFrame->payload[i+2+3+2]);

                    if(!pSession->nLpInterval)
                    {
                        // LP Interval이 설정되어 있지 않으면 읽지 못한다
                        break;
                    }

                    /** Count, Offset을 이용해서 읽어야 하는 Log ID를 조정 한다 */
                    nTodayCnt = ((tm_last.tm_hour * 60 + tm_last.tm_min) / pSession->nLpInterval) + 1;
                    if((pOndemand->nOffset - pSession->nMeterLpCount) < 0) 
                    {
                        // 읽어야 하는 현재 날짜가 포함되어 있는 경우
                        pSession->nRemainCnt += nTodayCnt;
                        pSession->nMeterLpCount --; // 금일 포함일 경우 -1
                    }
                    pSession->nNextLogId = pSession->nLogId - nTodayCnt + 1;
                    pSession->nNextLogId -= pOndemand->nOffset * (24 * 60 / pSession->nLpInterval);
                    pSession->nRemainCnt += pSession->nMeterLpCount * (24 * 60 / pSession->nLpInterval);

                    if(pSession->nNextLogId < 1) // 쌓여 있는 것보다 Offset이 적게 될 때
                    {
                        pSession->nNextLogId = pSession->nRemainCnt;
                    }
                    if(pSession->nNextLogId > pSession->nLogId)
                    {
                        pSession->nNextLogId = pSession->nLogId;
                    }
                    if(pSession->nRemainCnt > pSession->nNextLogId)
                    {
                        pSession->nRemainCnt = pSession->nNextLogId;
                    }
                    break;
                }
                i += 2 + 3 + pFrame->payload[i+3];
            }
            break;

        case KMP_Q_METER_TIME:
            /** Issue #837 : Kamstrup Meter에 대한 시간 동기화가 필요하다.
              *  이를 위해 Meter 시간을 확보한다. 이 때 KMP_Q_METER_TIME 요청에서 RTC가 4번째 위치하기 때문에
              *  이를 가정하고 처리한다. 만약 KMP_Q_METER_TIME의 구성이 변경되면 동작하지 않는다.
              */
            for(i=0;i<nLength-3-5;) // 3 : header , 5 : rid+type_length 
            {
                if(pFrame->payload[i] == 0x04 && pFrame->payload[i+1] == 0x17) // RTC 인지 확인
                {
                    cetime_t        now, prev;
	                struct tm       tm_prev, tm_when;
                    int             timeGap;

                    ceTime(&now);
                    tm_when = *localtime(&now);
                    memset(&tm_prev, 0, sizeof(struct tm));
                    tm_prev.tm_year = pFrame->payload[i+2+3+7] + 100; 
                    tm_prev.tm_mon  = pFrame->payload[i+2+3+6] - 1;
                    tm_prev.tm_mday = pFrame->payload[i+2+3+5];
                    tm_prev.tm_hour = pFrame->payload[i+2+3+4];
                    tm_prev.tm_min  = pFrame->payload[i+2+3+3];
                    tm_prev.tm_sec  = pFrame->payload[i+2+3+2];
                    prev = mktime(&tm_prev);

                    // RTC
                    XDEBUG(" KMP GET METER TIME GAP=%d\r\n", now - prev);

                    timeGap = abs(now - prev);

                    // Issue #402, #837 2006년 이전이면 자동 미터 시간동기화를 하지 않는다
                    if(tm_when.tm_year + 1900 < 2006) break;

                    // 동기화가 disable 되어 있으면 동기화 하지 않는다
                    if(!m_bEnableMeterTimesync) 
                    {
                        XDEBUG( " DISABLE AUTOMATIC TIMESYNC, Meter=%02d/%02d/%02d %02d:%02d:%02d, Diff=%d\r\n",
                                pFrame->payload[i+2+3+7],
                                pFrame->payload[i+2+3+6],
                                pFrame->payload[i+2+3+5],
                                pFrame->payload[i+2+3+4],
                                pFrame->payload[i+2+3+3],
                                pFrame->payload[i+2+3+2],
                                timeGap);
                        break;
                    }

                    /** 지정된 TimesyncThreshold 보다 작으면 동기화 하지 않는다
                    */
                    if(m_nTimesyncThresholdLow > 0 && timeGap < m_nTimesyncThresholdLow)
                    {
                        XDEBUG(" METER TIMESYNC THRESHOLD DISABLE:Low Threshold=%d, Meter=%02d/%02d/%02d %02d:%02d:%02d, Diff=%d\r\n",
                                m_nTimesyncThresholdLow,
                                pFrame->payload[i+2+3+7],
                                pFrame->payload[i+2+3+6],
                                pFrame->payload[i+2+3+5],
                                pFrame->payload[i+2+3+4],
                                pFrame->payload[i+2+3+3],
                                pFrame->payload[i+2+3+2],
                                timeGap);
                        break;
                    }
                    /** 지정된 TimesyncThreshold 보다 크면 동기화 하지 않는다
                    */
                    if(m_nTimesyncThresholdHigh > 0 && timeGap > m_nTimesyncThresholdHigh)
                    {
                        XDEBUG(" METER TIMESYNC THRESHOLD DISABLE:High Threshold=%d, Meter=%02d/%02d/%02d %02d:%02d:%02d, Diff=%d\r\n",
                                m_nTimesyncThresholdHigh,
                                pFrame->payload[i+2+3+7],
                                pFrame->payload[i+2+3+6],
                                pFrame->payload[i+2+3+5],
                                pFrame->payload[i+2+3+4],
                                pFrame->payload[i+2+3+3],
                                pFrame->payload[i+2+3+2],
                                timeGap);
                        break;
                    }

                    // Enable Meter Timesync
                    pSession->bMeterTimeSync = TRUE;
                    break;
                }
                i += 2 + 3 + pFrame->payload[i+3];
            }

            break;
    }

    /** 255 Byte 까지만 지원 */
    nLen = (BYTE) nLength;
    pOndemand->pszBuffer[0] = nLen;

    pOndemand->nLength = nLen+1;
    memcpy(pOndemand->pszBuffer+1, pszBuffer, nLen);

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return TRUE;
}
