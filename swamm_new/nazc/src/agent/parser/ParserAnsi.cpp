/////////////////////////////////////////////////////////////////////
//
//  ParserAnsi.cpp: implementation of the CAnsiParser class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2011 NURITELECOM Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "codiapi.h"
#include "Chunk.h"
#include "Event.h"
#include "MeterUploader.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "AgentLog.h"
#include "AgentService.h"
#include "Patch.h"

#include "ParserAnsi.h"

#include "ansiSessions.h"
#include "ansiFunctions.h"
#include "ansiError.h"
#include "ansiUtils.h"

#include "ansiParserDefines.h"
#include "ansiParserTypes.h"

#include "vendor/ansi.h"

#include "types/identification.h"

#if     defined(__PATCH_11_65__)
/**< Issue #65 JEJU SP */
extern BOOL    *m_pbPatch_11_65;
#endif


//////////////////////////////////////////////////////////////////////
// CAnsiParser Class
//////////////////////////////////////////////////////////////////////

#define ANSI_PACKET_SIZE        2048

#define GE_METER_TYPE_I210      5
#define GE_METER_TYPE_SM110     11
#define GE_METER_TYPE_AZOS      22

/** GE Meter Type이 복수 적용될 수 있기 때문에 Meter에서 읽히는 GE Meter Type을
 *  Mask 값으로 바꿔서 처리하도록 한다.
 */
#define GE_METER_TYPE_MASK_UNKNOWN  0x00000000
#define GE_METER_TYPE_MASK_I210     0x00000001
#define GE_METER_TYPE_MASK_SM110    0x00000002
#define GE_METER_TYPE_MASK_AZOS     0x00000004
#define GE_METER_TYPE_MASK_ALL      0xFFFFFFFF

/** Issue #303 : Meter Mode 지원 */
#define GE_METER_MODE_DEMAND        0
#define GE_METER_MODE_DEMAND_LP     1
#define GE_METER_MODE_TOU           2
#define GE_METER_MODE_ALL           255

#define READ_OPTION_FULL        ONDEMAND_OPTION_DEFAULT_METERING
#define READ_OPTION_CURRENT		1								// Current table read
#define READ_OPTION_PREVIOUS	2								// Previouse table read
#define READ_OPTION_RELAY       ONDEMAND_OPTION_GET_RELAY_STATUS
#define WRITE_OPTION_RELAYON    ONDEMAND_OPTION_DO_RELAY_ON
#define WRITE_OPTION_RELAYOFF   ONDEMAND_OPTION_DO_RELAY_OFF
#define WRITE_OPTION_ACTON      ONDEMAND_OPTION_DO_ACTIVATE_ON
#define WRITE_OPTION_ACTOFF     ONDEMAND_OPTION_DO_ACTIVATE_OFF
#define WRITE_OPTION_TIMESYNC   ONDEMAND_OPTION_TIME_SYNC
#define READ_OPTION_EVENTLOG    ONDEMAND_OPTION_GET_EVENT_LOG
#define READ_IDENT_ONLY         ONDEMAND_OPTION_READ_IDENT_ONLY

//#define READ_OPTION_MEASURE		12								// Measurement table read
#define WRITE_OPTION_CLEAR      	13                              // Set Time & LP Clear (Er0000002 Clear)
//#define WRITE_OPTION_DR         	14                              // Demand Reset
//#define WRITE_OPTION_TIMESYNC2  	15                              // Timesync 2 (Er000002 Clear)

/*------- Issue #1574 : 기존의 option 0번을 option 16번으로 옮김 ---------------*/
//#define READ_OPTION_CUR_PRE             16                            // Current&Previous data read

/*------- Issue #1574 : calendar table을 읽어서 출력하는 option 추가 ---------------*/
//#define READ_OPTION_CALENDAR_FILE       17				// Read Calendar & test

/*------- Issue #1643 : LP clear and LP start (Option 18) --*/
//#define WRITE_OPTION_CLEAR		18

/*------- Issue #2046 : Received KWH, Leading KVARH 감지 기능 Enable/Disable -------*/
//#define WRITE_OPTION_CONTROL_MONITORING  19                      // Enable Received KWH, Leading KVARH

/*------- Issue #2062 : 검침 데이터 줄이기 -------*/
//#define READ_OPTION_SMALL_FULL		20
//#define READ_OPTION_SMALL_CUR_PRE	21

#define CONV_METER_MODE_TOU         22

/** Issue #55 : DR Level Control */
#define ENERGY_LEVEL_OPTION_MIN     23                           // Energy Level Min (Relay On)
#define ENERGY_LEVEL_OPTION_MAX     24                           // Energy Level Max (Relay Off)

#define OPTION_MAX              	24

extern BOOL		m_bPingTestMode;
extern int      m_nTimesyncThresholdLow;
extern int      m_nTimesyncThresholdHigh;
extern BOOL     m_bEnableMeterTimesync;
extern BOOL     m_bEnableMeterRecovery;
extern BYTE     m_nDefaultMeteringOption;
#if     defined(__PATCH_4_1647__)
extern BOOL    *m_pbPatch_4_1647;
#endif

BYTE ge_logon[10] 	 = { 'a','i','m','i','r',0,0,0,0,0 };

#if     !defined(__GE_METER_SECURITY__)
BYTE ge_master_security[20] = { 'P','L','A','T','F','O','R','M',0,0,0,0,0,0,0,0,0,0,0,0 };
#else
BYTE ge_master_security[20] = __GE_METER_SECURITY__;
#endif

/** TODO : Reader/Customer Password를 관리할 수 있도록 수정해야 한다.
  */
BYTE ge_reader_security[20] = { 0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0,0,0,0,0,0,0,0,0,0 };
BYTE ge_customer_security[20] = { 0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0,0,0,0,0,0,0,0,0,0 };

const char *dayofweek[] 	 = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
/** Issue #303: 일본지사 I210+cn Modem Model 은 NAMR-P10CSR 이기 때문에 모델명을 추가 한다 */
static const char *parserNameArray[] = {"ANSI", "GE", "NAMR-P201SR", "NAMR-P202SR", "NAMR-P10CSR", "I210+c", NULL};
     
/* PreHandler의 차이
 *
 * ANSITBLLIST에서 PreHandler는 두가지 Type으로 분기된다.
 *
 * 1) READ 시 PreRead Handler
 * 2) WRITE 시 Pre Handler
 *
 * 두 Handler는 전달받는 Parameter에서 차이가 난다. 따라서 PreHandler를 등록할 때 ACTION에 따라 
 * 정확한 Handler를 사용해야 한다. 함수에서서 PreRead/Pre 의 두가지 Type이 구분된다.
 */

ANSITBLLIST m_AnsiPreReadTbl[] =
{
   // ID                        TABLE TYPE          TN  SAVE    ACTION          Cnt     
   //       METER TYPE MASK                 METER MODE
   //       Pre Handler                             Pre Param
   //       Post Handler                    Description
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   0, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostM000, "GE Device Table"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

ANSITBLLIST m_AnsiMeterTblOption_0[] =
{
   // ID                        TABLE TYPE          TN  SAVE    ACTION          Cnt 
   //       METER TYPE                      METER MODE
   //       Pre Handler                             Pre Param
   //       Post Handler                    Description
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   ANSI_PROC_SNAPSHOT_REVENUE_DATA,                  
            NULL,                           "Snapshot Data"},
    { ANSI_ST_MANUFACTURER,     ANSI_STANDARD_TBL,   0, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "General Configuration Table"},
    { ANSI_ST_MANUFACTURER,     ANSI_STANDARD_TBL,   1, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS001,  "General Manufacturer Identification Table"},
    { ANSI_ST_MODE_STATUS,      ANSI_STANDARD_TBL,   3, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS003,  "End Device Mode & Status Table"},
    { ANSI_ST_IDENTIFICATION,   ANSI_STANDARD_TBL,   5, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS005,  "Device Identification Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  11, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS011, "Actual Sources Limiting Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  12, SAVE,   POFFSET_READ,   248,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreReadS012,      0,                  
            (void*)CANSIParser::fnPostS012, "Unit of Measure Entry Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  21, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS021, "Actual Register Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  22, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "Data Selection Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  23, SAVE,   POFFSET_READ,    13,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "Current Register Data Table"},
    { ANSI_ST_PREVIOUSDEMAND,   ANSI_STANDARD_TBL,  25, SAVE,   POFFSET_READ,    19,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "Previous Demand Reset Data Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  26, SAVE,   POFFSET_READ,     0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreReadS026,      0,                  
            NULL,                           "Self-Read Data Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  52, SAVE,   FULL_READ,        7,    // Demand LP
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_DEMAND_LP,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS055BeforeTimesync,"Clock Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  55, SAVE,   FULL_READ,        9,    // TOU
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_TOU,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS055BeforeTimesync,"Clock State Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,   7, SKIP,   PROC_WRITE,       0,    // 조건 검사 후 Meter Timesync
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007ConditionalSetTime,   ANSI_PROC_SET_DATE_TIME,                  
            NULL,                           "Set Date Time"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  52, SAVE,   FULL_READ,        7,    // Demand LP
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_DEMAND_LP,
            (void*)CANSIParser::fnPreReadS055,                                    0,                  
            NULL,                           "Clock Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  55, SAVE,   FULL_READ,        9,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_TOU,
            (void*)CANSIParser::fnPreReadS055,                                    0,                  
            NULL,                           "Clock State Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  61, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS061, "Actual Load Profile Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  62, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS062, "Load Profile Control Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  63, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS063, "Load Profile Status Table"},
    { ANSI_ST_LOADPROFILE,      ANSI_STANDARD_TBL,  64, SAVE,   POFFSET_READ,   593,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreReadS064,      0,                  
            NULL,                           "Load Profile Data Set1 Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  76, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreReadS076,      0,                  
            NULL,                           "Event Log Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 112, SAVE,   POFFSET_READ,     sizeof(ANSI_LOAD_CTRL_STATUS), // I210
            GE_METER_TYPE_MASK_I210 | GE_METER_TYPE_MASK_AZOS,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS112, "Relay status table(GE)"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 130, SAVE,   FULL_READ,        2,    // SM110
            GE_METER_TYPE_MASK_SM110,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS130, "Relay status table(GE)"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,  70, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "Display configuration table"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,  75, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "Scale factors table"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL, 113, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "Power quality data table"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

/** GE I210+ */
ANSITBLLIST m_AnsiMeterTblOption_0_i210pulse[] =
{
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,  16, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "AMR SPI Frame1"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,  19, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "AMR SPI Frame2"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

ANSITBLLIST m_AnsiMeterTblOption_3[] = // relay status read
{
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   ANSI_PROC_SNAPSHOT_REVENUE_DATA,                  
            NULL,                           "Snapshot Data"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 112, SAVE,   FULL_READ,        0, // I210
            GE_METER_TYPE_MASK_I210 | GE_METER_TYPE_MASK_AZOS,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS112, "Relay status table(GE)"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL, 115, SAVE,   FULL_READ,        0, // I210
            GE_METER_TYPE_MASK_I210 | GE_METER_TYPE_MASK_AZOS,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "Load control status table(GE)"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 130, SAVE,   FULL_READ,        2,    // SM110
            GE_METER_TYPE_MASK_SM110,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS130, "Relay status table(GE)"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 132, SAVE,   FULL_READ,      174,    // SM110
            GE_METER_TYPE_MASK_SM110,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           "Relay event log table(GE)"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

ANSITBLLIST m_AnsiMeterTblOption_4[] = // relay control
{
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   ANSI_PROC_SNAPSHOT_REVENUE_DATA,                  
            NULL,                           "Snapshot Data"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  7, SKIP,    PROC_WRITE,       0,    // I210
            GE_METER_TYPE_MASK_I210 | GE_METER_TYPE_MASK_AZOS,
                                            GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007RelayControl,  ANSI_PROC_DIRECT_LOAD_CONTROL,                  
            NULL,                           "Relay control(GE)"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 112, SAVE,   POFFSET_READ,     sizeof(ANSI_LOAD_CTRL_STATUS),    // I210
            GE_METER_TYPE_MASK_I210 | GE_METER_TYPE_MASK_AZOS,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS112, "Relay status table(GE)"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 130, SAVE,   FULL_READ,        2,    // SM110
            GE_METER_TYPE_MASK_SM110,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS130, "Relay status table(GE)"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 131, SKIP,   TBL_WRITE,        0,    // SM110
            GE_METER_TYPE_MASK_SM110,
                                            GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS131,          0,                  
            NULL,                           "Relay control(GE)"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 130, SAVE,   FULL_READ,        2,    // SM110
            GE_METER_TYPE_MASK_SM110,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS130, "Relay status table(GE)"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

ANSITBLLIST m_AnsiMeterTblOption_8[] = // timesync meter
{
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   ANSI_PROC_SNAPSHOT_REVENUE_DATA,                  
            NULL,                           "Snapshot Data"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  55, SAVE,   FULL_READ,        9, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS055BeforeTimesync,"Clock State Table (Prev)"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007SetTime,   ANSI_PROC_SET_DATE_TIME,                  
            NULL,                           "Set Date Time"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  55, SAVE,   FULL_READ,        9, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS055AfterTimesync,"Clock State Table (Curr)"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007StopProg, ANSI_PROC_PROGRAM_CONTROL,
            NULL,                           "Stop Program"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

ANSITBLLIST m_AnsiMeterTblOption_10[] =     // read event log
{
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   ANSI_PROC_SNAPSHOT_REVENUE_DATA,                  
            NULL,                           "Snapshot Data"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  76, SAVE,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                    0,                  
            NULL,                           "Event Log Table"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0,    
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

/**
 */
ANSITBLLIST m_AnsiMeterTblOption_11[] =     // read identification only 11
{
    { ANSI_ST_MANUFACTURER,     ANSI_STANDARD_TBL,   1, SAVE,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS001,  "General Manufacturer Identification Table"},
    { ANSI_ST_MODE_STATUS,      ANSI_STANDARD_TBL,   3, SAVE,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS003,  "End Device Mode & Status Table"},
    { ANSI_ST_IDENTIFICATION,   ANSI_STANDARD_TBL,   5, SAVE,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS005,  "Device Identification Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  12, SAVE,   POFFSET_READ,   248, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreReadS012,      0,                  
            (void*)CANSIParser::fnPostS012, "Unit of Measure Entry Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  55, SAVE,   FULL_READ,        9, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS055BeforeTimesync,"Clock State Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,   7, SKIP,   PROC_WRITE,       0,        // 조건 검사 후 Meter Timesync
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007ConditionalSetTime,   ANSI_PROC_SET_DATE_TIME,                  
            NULL,                           "Set Date Time"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  55, SAVE,   FULL_READ,        9, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreReadS055,                                    0,                  
            NULL,                           "Clock State Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  61, SAVE,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS061, "Actual Load Profile Table"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  62, SAVE,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS062, "Load Profile Control Table"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

ANSITBLLIST m_AnsiMeterTblOption_13[] = // clear er0002 error
{
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007SetTime,   ANSI_PROC_SET_DATE_TIME,                  
            NULL,                           "Set Date Time"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007ResetLp,   ANSI_PROC_RESET_LIST_POINTERS,                  
            NULL,                           "Reset LP Data Set 1 Pointer"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   ANSI_PROC_START_LP_RECORDING,
            NULL,                           "Start LP Recording"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007StopProg, ANSI_PROC_PROGRAM_CONTROL,
            NULL,                           "Stop Program"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

ANSITBLLIST m_AnsiMeterTblOption_22[] = // convert meter mode tou
{
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007StartProg, ANSI_PROC_PROGRAM_CONTROL,
            NULL,                           "Start Program"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007ConvertTou,ANSI_PROC_CONVERT_METER_MODE,
            NULL,                           "Convert Meter Mode (TOU)"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007StopProg, ANSI_PROC_PROGRAM_CONTROL,
            NULL,                           "Stop Program"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};


ANSITBLLIST m_AnsiMeterTblOption_23[] = // energy level control 
{
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            NULL,                                   ANSI_PROC_SNAPSHOT_REVENUE_DATA,                  
            NULL,                           "Snapshot Data"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  7, SKIP,    PROC_WRITE,       0,    // I210
            GE_METER_TYPE_MASK_I210 | GE_METER_TYPE_MASK_AZOS,
                                            GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007RelayControl,  ANSI_PROC_DIRECT_LOAD_CONTROL,                  
            NULL,                           "Relay control(GE) - Activate"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL,  7, SKIP,    PROC_WRITE,       0,    // I210
            GE_METER_TYPE_MASK_I210 | GE_METER_TYPE_MASK_AZOS,
                                            GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007RelayControl,  ANSI_PROC_DIRECT_LOAD_CONTROL,                  
            NULL,                           "Relay control(GE) - Relay"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 112, SAVE,   POFFSET_READ,     sizeof(ANSI_LOAD_CTRL_STATUS),    // I210
            GE_METER_TYPE_MASK_I210 | GE_METER_TYPE_MASK_AZOS,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS112, "Relay status table(GE)"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 131, SKIP,   TBL_WRITE,        0,    // SM110
            GE_METER_TYPE_MASK_SM110,
                                            GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS131,          0,                  
            NULL,                           "Relay control(GE) - Activate"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 131, SKIP,   TBL_WRITE,        0,    // SM110
            GE_METER_TYPE_MASK_SM110,
                                            GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS131,          0,                  
            NULL,                           "Relay control(GE) - Relay"},
    { ANSI_DEFAULT,             ANSI_STANDARD_TBL, 130, SAVE,   FULL_READ,        2,    // SM110
            GE_METER_TYPE_MASK_SM110,
                                            GE_METER_MODE_ALL,
            NULL,                                   0,                  
            (void*)CANSIParser::fnPostS130, "Relay status table(GE)"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

ANSITBLLIST m_AnsiMeterTbl_ResolveUnprogram[] = // Resolve Unprogram
{
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,  84, SKIP,   TBL_WRITE,        0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreM084,          0,                  
            NULL,                           "Resolve Unprogram"},
    { ANSI_DEFAULT,             ANSI_MANUFACT_TBL,   7, SKIP,   PROC_WRITE,       0, 
            GE_METER_TYPE_MASK_ALL,         GE_METER_MODE_ALL,
            (void*)CANSIParser::fnPreS007StopProg, ANSI_PROC_PROGRAM_CONTROL,
            NULL,                           "Stop Program"},
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

ANSITBLLIST m_AnsiMeterTbl_Terminate[] = // Stop
{
    { 0,                        0,                   0, SKIP,   FULL_READ,        0, 
            GE_METER_TYPE_MASK_UNKNOWN,     GE_METER_MODE_ALL,
            NULL,                                   0,                  
            NULL,                           NULL}
};

//////////////////////////////////////////////////////////////////////
// CANSIParser Class
//////////////////////////////////////////////////////////////////////

CANSIParser::CANSIParser() : CMeterParser(SENSOR_TYPE_ZRU, SERVICE_TYPE_ELEC, 
        ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL,
        const_cast<char **>(parserNameArray), PARSER_TYPE_ANSI, "GE ANSI/KV2C Parser", "GE", parserNameArray[0], 
        6000, 6000, 2)
{
}

CANSIParser::~CANSIParser()
{
}

//////////////////////////////////////////////////////////////////////
// CANSIParser Operations
//////////////////////////////////////////////////////////////////////

int CANSIParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
	switch(pCommand->nType) {
	  case PARSE_TYPE_READ :
	  case PARSE_TYPE_TIMESYNC :
		   return GE_MAIN(pCommand, pStat);

	  case PARSE_TYPE_WRITE :
		   return ONDEMAND_ERROR_PARAM;
	}
    return 0;
}

/** DR 가능 장비인지 여부를 리턴한다.
 */
BOOL CANSIParser::IsDRAsset(EUI64 *pId)
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
 * @see CANSIParser#IsDRAsset(EUI64 *);
 *
 * @warning pEnergyLevelArray는 최소 15bytes 이상의 길이를 가져야 한다.
 */
int CANSIParser::GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray)
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
int CANSIParser::GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel)
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
    cmdParam.nMeteringType = METERING_TYPE_SPECIAL;
    cmdParam.nOption = READ_OPTION_RELAY;
	cmdParam.pChunk	= &chunk;

    nError = GE_MAIN(&cmdParam, &stat);
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
int CANSIParser::SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen)
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
    cmdParam.nMeteringType = METERING_TYPE_SPECIAL;
	cmdParam.pChunk	= &chunk;
    switch(nEnergyLevel) {
        case  ENERGY_LEVEL_MIN:
            cmdParam.nOption = ENERGY_LEVEL_OPTION_MIN;
            break;
        default:
            cmdParam.nOption = ENERGY_LEVEL_OPTION_MAX;
            break;
    }

    nError = GE_MAIN(&cmdParam, &stat);
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

/** 검침 Logic 수행.
 *
 * 검침 Logic의 차이.
 *
 *  - I210+ 의 경우 다음 절차를 거치지 않는다.
 *    + Identification
 *    + Logon
 *    + Security
 *    + Negotiation
 *    + Log off
 *
 */
int CANSIParser::GE_MAIN(CMDPARAM *pCommand, METER_STAT *pStat)
{
	char			szGUID[20];
    BYTE            security[20];
	EUI64			id;
	int				nParam;
	ONDEMANDPARAM	*pOndemand = NULL;
	int				nRetry, nLength = 0;
	int				nState, nOrigState=STATE_PRE_READ;
	int				nSaveState;
	int				nError = ANSIERR_NOERROR;//, nReplyTime;
	int				nResult, nElapse, nReplyCount;
	int				nOffset, nCount;
	time_t			tmStart, tmEnd, tmLast=0, tmLastEvent=0;
	CChunk			*pChunk;
	TIMESTAMP		tmLastMetering;
	TIMESTAMP		tmLastEventLog;
	BOOL			bConnected = FALSE;
	BOOL			bTimesync = FALSE;
	BOOL			bEventLog = FALSE;
    BOOL            bI210Pluse = FALSE; // I210+ 의 경우 다른 검침 절차를 수행한다
    
    ENDI_NODE_INFO  *nodeInfo;
    AMR_PARTIAL_INFO *amrInfo;
    BYTE            permitMode=0, permitState=0, alarmFlag=0, testFlag=0;
    BYTE            hwVersion=0, fwVersion=0, fwBuild=0;
    WORD            alarmMask=0;
    WORD            addr;
    int             len,n;

    //
    ANSISESSION     *pSession = NULL;
    ANSI_FRAME      *pAnsiFrame = NULL;
    WORD            nFrameLength = 0;
    BOOL            bSendResult;
    BYTE            szAckBuffer[1];
    int             nTableIndex = 0;
    WORD            nTableLength;
    char            szBuffer[256];
    ANSITBLLIST     *pPreReadTable = m_AnsiPreReadTbl;
    //ANSITBLLIST     *pReadTable = m_AnsiMeterTblOption_0_TOU;
    ANSITBLLIST     *pTargetTable = pPreReadTable;
    ANSITBLLIST     *pNextTable;
    ANSI_ST_TABLE7  *pProcedureTable = (ANSI_ST_TABLE7 *)szBuffer;

    BOOL            bHandlerResponse = FALSE;
    BOOL            bMeterClockError = FALSE;

    memcpy(&id, &pCommand->id, sizeof(EUI64));      // Sensor ID
	pChunk		= pCommand->pChunk;					// 검침 데이터를 리턴할 Chunk
	nParam 		= pCommand->nOption;                // Function
	nOffset 	= pCommand->nOffset;				// Read interval of meter data
	nCount		= pCommand->nCount;					// LP Count
	bTimesync	= pCommand->bTimesync;				// 시간 동기화 여부
	bEventLog	= pCommand->bEventLog;				// 이벤트 로그 수집 여부

    if(nOffset < 0) nOffset = 0;
    if(nCount <= 0) nCount = 8;
	
	if (nParam > OPTION_MAX)
		return ONDEMAND_ERROR_PARAM;

    if (nParam < 0)
    {
        // Default Option을 사용한다
        nParam = m_nDefaultMeteringOption;
    }

	nState = STATE_OPEN;
	if (codiGetState(pCommand->codi) != CODISTATE_NORMAL)
		return ONDEMAND_ERROR_NOT_CONNECT;

	nResult = ONDEMAND_ERROR_NOT_CONNECT;
	
	nSaveState = m_pEndDeviceList->GetState(&id);
	if (nSaveState == ENDISTATE_INIT)
		return ONDEMAND_ERROR_INIT;

	if (m_pEndDeviceList->IsBusy(&id))
	    return ONDEMAND_ERROR_BUSY;
		
	eui64toa(&id, szGUID);
	m_pEndDeviceList->SetBusy(&id, TRUE);

    /*-- Issue #1529 : GE Meter의 CLOCK_ERROR가 발생할 경우 무조건 Clear 시킨다 --*/
    m_pEndDeviceList->GetMeterError(&pCommand->id, ERR_TYPE_CLOCK_ERROR, &bMeterClockError);
 
    if(m_bEnableMeterRecovery && bMeterClockError) {
        XDEBUG("GE: AUTOMATIC METER ERROR RECOVERY : ERR_TYPE_CLOCK_ERROR (%s)\r\n", szGUID);
        /*-- Rewriter : Event를 추가해야 한다 --*/
        pCommand->nOption = WRITE_OPTION_CLEAR;
        nParam = pCommand->nOption;
        m_pEndDeviceList->SetMeterError(&pCommand->id, ERR_TYPE_CLOCK_ERROR, FALSE);
    }
    
	// 마지막 검침 시간을 구한다.
	m_pEndDeviceList->GetLastMeteringTime(&id, &tmLast);
	GetTimestamp(&tmLastMetering, &tmLast);	

	// 마지막 이벤트 로그 시간을 구한다.
	m_pEndDeviceList->GetLastEventLogTime(&id, &tmLastEvent);
	GetTimestamp(&tmLastEventLog, &tmLastEvent);	

	XDEBUG("GE: TYPE=%s, Option=%d, Offset=%d, Count=%d, Timesync=%d, EventLog=%d\r\n",
			pCommand->nMeteringType == METERING_TYPE_SPECIAL ? "SPECIAL" : "ROUTINE",
			nParam, nOffset, nCount, bTimesync, bEventLog);

	XDEBUG("    LAST METERING = %04d/%02d/%02d %02d:%02d:%02d, LAST EVENT = %04d/%02d/%02d %02d:%02d:%02d\r\n",
			tmLastMetering.year, tmLastMetering.mon, tmLastMetering.day,
			tmLastMetering.hour, tmLastMetering.min, tmLastMetering.sec,
			tmLastEventLog.year, tmLastEventLog.mon, tmLastEventLog.day,
			tmLastEventLog.hour, tmLastEventLog.min, tmLastEventLog.sec);

	nReplyCount = 0;
	for(nRetry=0; (nRetry<GetRetryCount()) && (nState!=STATE_DONE);)
	{
		switch(nState) {
		  case STATE_OPEN :
			   time(&tmStart);
			   LockOperation();
			   pOndemand = FindOperation(&id);
			   if (pOndemand != NULL)
			   {
			   	   UnlockOperation();
	               m_pEndDeviceList->SetBusy(&id, FALSE);
				   return ONDEMAND_ERROR_BUSY;
			   } 

			   pOndemand = NewOperation(ONDEMAND_OPR_DATA, &id, 4096);
			   pOndemand->nMeteringType = pCommand->nMeteringType;
               pOndemand->nOption = nParam;
			   pOndemand->nOffset = nOffset;
			   pOndemand->nCount = nCount;
			   pOndemand->bTimesync = bTimesync;
			   pOndemand->bEventLog = bEventLog;
			   pOndemand->pParser = (void *)this;
			   memcpy(&pOndemand->tmLastMetering, &tmLastMetering, sizeof(TIMESTAMP));
			   memcpy(&pOndemand->tmLastEventLog, &tmLastEventLog, sizeof(TIMESTAMP));
    		   pOndemand->nResult = ANSI_NO_RESPONSE;
			   UnlockOperation();

			   GetTimeTick(&pOndemand->tmStart);

			   bConnected = FALSE;
			   nError = endiOpen(pCommand->codi, (HANDLE *)&pOndemand->endi, &id);
			   if (nError != CODIERR_NOERROR)
			   {
				   XDEBUG("endiOpen Error(%d): %s\r\n", nError, codiErrorMessage(nError));
				   nState = STATE_RETRY;
				   break;
			   }

			   // SEQ 검사 옵션을 켠다. GE미터는 이 옵션을 반드시 켠다.
			   endiSetOption(pOndemand->endi, ENDIOPT_SEQUENCE, 1);

               nError = sensorConnect(pOndemand->endi, GetConnectionTimeout(ENDDEVICE_ATTR_POWERNODE));
			   if (nError != CODIERR_NOERROR)
			   {
				   
				   XDEBUG("endiConnect Error(%d): %s\r\n", nError, codiErrorMessage(nError));
				   nState = STATE_RETRY;
				   break;
			   }

			   bConnected = TRUE;
			   pChunk->Rollback();

			   GetTimeTick(&pOndemand->tmConnect);
			   GetTimeTick(&pOndemand->tmSend);

               /** Session 정보 생성 */
               pSession = ansiNewSession();
               pSession->pChunk = pChunk;
               pSession->pUserData = NewMeterInfo(NULL);
               
               pOndemand->pSession = (void *)pSession;

               nState = STATE_READ_NODEINFO;
			   break;

          case STATE_IDENTIFICATION:
               XDEBUG(" " ANSI_COLOR_MAGENTA "STEP: ANSI IDENTIFICATION" ANSI_NORMAL "\r\n");

    		   pOndemand->nResult = ANSI_NO_ERROR;

               pAnsiFrame = NULL; nFrameLength = 0;
               nError = ansiMakeIdentificationFrame(pSession, (BYTE **)&pAnsiFrame, &nFrameLength);
               if(nError == ANSIERR_NOERROR)
               {
                   if((bSendResult = SendCommand(pOndemand, &id, (const char *)pAnsiFrame, nFrameLength)) == TRUE) 
                   {
                    /** ANSI Frame에 관련된 Session 정보를 Update 해야 한다 */
                        ansiToggleSendCtrl(pSession);
                   }
               }
               else
               {
                   XDEBUG(" MakeFrameError : %s\r\n", ansiGetErrorMessage(nError));
                   nState = STATE_TERMINATE;
                   if(pAnsiFrame != NULL) FREE(pAnsiFrame);
                   break;
               }
               if(pAnsiFrame != NULL) FREE(pAnsiFrame);

               m_Locker.Lock();
               ClearOperation(pOndemand);
               pOndemand->nState = nState;
               pOndemand->bReply = FALSE;
               ResetEvent(pOndemand->hEvent);
               m_Locker.Unlock();
               nState = STATE_WAIT_REPLY;
               break;

          case STATE_NEGOTIATE:
               XDEBUG(" " ANSI_COLOR_MAGENTA "STEP: ANSI NEGOTIATE" ANSI_NORMAL "\r\n");
               pAnsiFrame = NULL; nFrameLength = 0;
               nError = ansiMakeNegotiateFrame(pSession, 
                       TRUE,                    // With Ack
                       ANSI_PACKET_SIZE,        // Packet Size 
                       0x01,                    // Number of packet (1)
                       ANSI_BAUDRATE_9600,      // 9600
                       (BYTE **)&pAnsiFrame, &nFrameLength);

               pSession->nAnsiPacketSize = ANSI_PACKET_SIZE;

               if(nError == ANSIERR_NOERROR)
               {
                   if((bSendResult = SendCommand(pOndemand, &id, (const char *)pAnsiFrame, nFrameLength)) == TRUE) 
                   {
                    /** ANSI Frame에 관련된 Session 정보를 Update 해야 한다 */
                       ansiToggleSendCtrl(pSession);
                   }
               }
               else
               {
                   XDEBUG(" MakeFrameError : %s\r\n", ansiGetErrorMessage(nError));
                   nState = STATE_TERMINATE;
                   if(pAnsiFrame != NULL) FREE(pAnsiFrame);
                   break;
               }
               if(pAnsiFrame != NULL) FREE(pAnsiFrame);

               m_Locker.Lock();
               ClearOperation(pOndemand);
               pOndemand->nState = nState;
               pOndemand->bReply = FALSE;
               ResetEvent(pOndemand->hEvent);
               m_Locker.Unlock();
               nState = STATE_WAIT_REPLY;
               break;
		
          case STATE_LOGON:
               XDEBUG(" " ANSI_COLOR_MAGENTA "STEP: ANSI LOGON" ANSI_NORMAL "\r\n");
               pAnsiFrame = NULL; nFrameLength = 0;
               nError = ansiMakeLogonFrame(pSession, 
                       TRUE,                    // With Ack
                       (WORD)0x0002,            // User Id
                       ge_logon,                // "aimir"
                       sizeof(ge_logon),
                       (BYTE **)&pAnsiFrame, &nFrameLength);
               if(nError == ANSIERR_NOERROR)
               {
                   if((bSendResult = SendCommand(pOndemand, &id, (const char *)pAnsiFrame, nFrameLength)) == TRUE) 
                   {
                    /** ANSI Frame에 관련된 Session 정보를 Update 해야 한다 */
                       ansiToggleSendCtrl(pSession);
                   }
               }
               else
               {
                   XDEBUG(" MakeFrameError : %s\r\n", ansiGetErrorMessage(nError));
                   nState = STATE_TERMINATE;
                   if(pAnsiFrame != NULL) FREE(pAnsiFrame);
                   break;
               }
               if(pAnsiFrame != NULL) FREE(pAnsiFrame);

               m_Locker.Lock();
               ClearOperation(pOndemand);
               pOndemand->nState = nState;
               pOndemand->bReply = FALSE;
               ResetEvent(pOndemand->hEvent);
               m_Locker.Unlock();
               nState = STATE_WAIT_REPLY;
               break;

          case STATE_SECURITY:
               XDEBUG(" " ANSI_COLOR_MAGENTA "STEP: ANSI SECURITY" ANSI_NORMAL "\r\n");
               memset(security, 0, sizeof(security));

               // Security 얻어오기
               if(!m_pEndDeviceList->GetMeterSecurity(&id, security, NULL))
               {
                   XDEBUG("Security Get fail\r\n");
                   // 실패할 경우 기본 Security 사용
                   memcpy(security, ge_master_security, sizeof(security));
               }
               XDEBUG("Meter Security : ");
               XDUMP((char *)security, sizeof(security));

               pAnsiFrame = NULL; nFrameLength = 0;
               nError = ansiMakeSecurityFrame(pSession, 
                       TRUE,                    // With Ack
                       security,             // security
                       sizeof(security),
                       (BYTE **)&pAnsiFrame, &nFrameLength);
               if(nError == ANSIERR_NOERROR)
               {
                   if((bSendResult = SendCommand(pOndemand, &id, (const char *)pAnsiFrame, nFrameLength)) == TRUE) 
                   {
                    /** ANSI Frame에 관련된 Session 정보를 Update 해야 한다 */
                       ansiToggleSendCtrl(pSession);
                   }
               }
               else
               {
                   XDEBUG(" MakeFrameError : %s\r\n", ansiGetErrorMessage(nError));
                   nState = STATE_LOGOFF;
                   if(pAnsiFrame != NULL) FREE(pAnsiFrame);
                   break;
               }
               if(pAnsiFrame != NULL) FREE(pAnsiFrame);

               m_Locker.Lock();
               ClearOperation(pOndemand);
               pOndemand->nState = nState;
               pOndemand->bReply = FALSE;
               ResetEvent(pOndemand->hEvent);
               m_Locker.Unlock();
               nState = STATE_WAIT_REPLY;
               break;

          case STATE_LOGOFF:
               if(bI210Pluse)
               {
                   /** GE I210+ 의 경우 Log Off 단계를 생략한다 */
                   nState = STATE_TERMINATE;
                   break;
               }
               XDEBUG(" " ANSI_COLOR_MAGENTA "STEP: ANSI LOGOFF" ANSI_NORMAL "\r\n");
               pAnsiFrame = NULL; nFrameLength = 0;
               nError = ansiMakeLogoffFrame(pSession, (BYTE **)&pAnsiFrame, &nFrameLength);
               if(nError == ANSIERR_NOERROR)
               {
                   if((bSendResult = SendCommand(pOndemand, &id, (const char *)pAnsiFrame, nFrameLength)) == TRUE) 
                   {
                    /** ANSI Frame에 관련된 Session 정보를 Update 해야 한다 */
                       ansiToggleSendCtrl(pSession);
                   }
               }
               else
               {
                   XDEBUG(" MakeFrameError : %s\r\n", ansiGetErrorMessage(nError));
                   nState = STATE_TERMINATE;
                   if(pAnsiFrame != NULL) FREE(pAnsiFrame);
                   break;
               }
               if(pAnsiFrame != NULL) FREE(pAnsiFrame);

               m_Locker.Lock();
               ClearOperation(pOndemand);
               pOndemand->nState = nState;
               pOndemand->bReply = FALSE;
               ResetEvent(pOndemand->hEvent);
               m_Locker.Unlock();
               nState = STATE_WAIT_REPLY;
               break;
		
          case STATE_TERMINATE:
               XDEBUG(" " ANSI_COLOR_MAGENTA "STEP: ANSI TERMINATE" ANSI_NORMAL "\r\n");
               pAnsiFrame = NULL; nFrameLength = 0;
               nError = ansiMakeTerminateFrame(pSession, (BYTE **)&pAnsiFrame, &nFrameLength);
               if(nError == ANSIERR_NOERROR)
               {
                   if((bSendResult = SendCommand(pOndemand, &id, (const char *)pAnsiFrame, nFrameLength)) == TRUE) 
                   {
                    /** ANSI Frame에 관련된 Session 정보를 Update 해야 한다 */
                       ansiToggleSendCtrl(pSession);
                   }
               }
               else
               {
                   XDEBUG(" MakeFrameError : %s\r\n", ansiGetErrorMessage(nError));
                   nState = STATE_LAST_ACK;
                   if(pAnsiFrame != NULL) FREE(pAnsiFrame);
                   break;
               }
               if(pAnsiFrame != NULL) FREE(pAnsiFrame);

               m_Locker.Lock();
               ClearOperation(pOndemand);
               pOndemand->nState = nState;
               pOndemand->bReply = FALSE;
               ResetEvent(pOndemand->hEvent);
               m_Locker.Unlock();
               nState = STATE_WAIT_REPLY;
               break;

          case STATE_LAST_ACK:
               XDEBUG(" " ANSI_COLOR_MAGENTA "STEP: ANSI LAST ACK" ANSI_NORMAL "\r\n");
               nError = ansiMakeAckFrame(pSession, szAckBuffer);
               if(nError == ANSIERR_NOERROR)
               {
                   if((bSendResult = SendCommand(pOndemand, &id, (const char *)szAckBuffer, 1)) == TRUE) 
                   {
                    /** ANSI Frame에 관련된 Session 정보를 Update 해야 한다 */
                       ansiToggleSendCtrl(pSession);
                   }
               }
               else
               {
                   XDEBUG(" MakeFrameError : %s\r\n", ansiGetErrorMessage(nError));
                   nState = STATE_OK;
                   break;
               }

               m_Locker.Lock();
               ClearOperation(pOndemand);
               pOndemand->nState = nState;
               pOndemand->bReply = FALSE;
               m_Locker.Unlock();
               nState = STATE_OK;
               break;

          /** Device Type을 알기 위해 먼저 읽어보는 Table 을 읽어본다.
           * 이 때 실제 읽는 동작은 STATE_READ_TABLE에 있는 코드를 사용한다.
           * 따라서 SAATE_PRE_READ 는 절대로 break 문이 있어서는 안된다.
          */
          case STATE_PRE_READ:
               if(pTargetTable[nTableIndex].id == 0) 
               {
                   nTableIndex = 0;
                   // 계산된 Table 정보 설정
                   pTargetTable = (ANSITBLLIST*)((ANSI_METER_INFO *)pSession->pUserData)->pTableList;
                   nState = STATE_READ_TABLE;
               }

          case STATE_READ_TABLE:
               if(pTargetTable == NULL || pTargetTable[nTableIndex].id == 0) 
               {
                   nState = STATE_LOGOFF;
                   break;
               }

               if(pTargetTable[nTableIndex].meterMode != GE_METER_MODE_ALL && 
                       pTargetTable[nTableIndex].meterMode != ((ANSI_METER_INFO *)pSession->pUserData)->nMeterMode)
               {
                    // Meter Mode가 맞지 않을 때
                    nTableIndex ++;
                    break;
               }

               if(pTargetTable[nTableIndex].meterTypeMask != GE_METER_TYPE_MASK_ALL &&
                    (pTargetTable[nTableIndex].meterTypeMask & ((ANSI_METER_INFO *)pSession->pUserData)->nMeterTypeMask) 
                       == GE_METER_TYPE_MASK_UNKNOWN )
               {
                    // Meter Type이 맞지 않을 때
                    nTableIndex ++;
                    break;
               }

               nOrigState = nState;  // STATE_READ_NEXT에서 복귀하기 위해 이전 State를 기록

               /** Session Setting */
               ansiResetSession(pSession);
               pSession->bSave = pTargetTable[nTableIndex].save;
               pSession->tableType = pTargetTable[nTableIndex].tblType;
               pSession->fnHandler = pTargetTable[nTableIndex].fnPostHandler;
               switch(pTargetTable[nTableIndex].tblType)
               {
                   case ANSI_MANUFACT_TBL:
                       pSession->tableType = 'M';
                       break;
                   default:
                       pSession->tableType = 'S';
                       break;
               }
               pSession->nTableNumber = pTargetTable[nTableIndex].tbl;
               pSession->bReadResponse = TRUE;

               XDEBUG(" " ANSI_COLOR_MAGENTA "STEP: ANSI ACT TABLE [%c%03d] (%s)" ANSI_NORMAL "\r\n", 
                       pSession->tableType, pTargetTable[nTableIndex].tbl, pTargetTable[nTableIndex].descr);

               bHandlerResponse = TRUE; // handler에서 진행하도록 설정했다고 초기화
               nError = ANSIERR_NOERROR;

               pAnsiFrame = NULL; nFrameLength = 0;
               switch(pTargetTable[nTableIndex].actionType)
               {
                   case PINDEX_READ:
                        nError = ansiMakePReadIndexFrame(pSession, TRUE, pTargetTable[nTableIndex].tblType | pTargetTable[nTableIndex].tbl, 
                            0, pTargetTable[nTableIndex].count, (BYTE **)&pAnsiFrame, &nFrameLength);
                        break;
                   case POFFSET_READ:
                        /** Pre Handler가 있을 경우에는 Pre Handler에서 Index와 Total Request Bytes 값을 얻어 온다
                          */
                        if(pTargetTable[nTableIndex].fnPreHandler != NULL)
                        {
                            bHandlerResponse = ((fnReadPreHandler)pTargetTable[nTableIndex].fnPreHandler)(pSession, pOndemand, 
                                pTargetTable[nTableIndex].tblType, 
                                pTargetTable[nTableIndex].tbl,
                                &pSession->nCurrentIndex, &pSession->nTotalRequestBytes);
                        }
                        else
                        {
                            pSession->nCurrentIndex = 0;
                            pSession->nTotalRequestBytes = pTargetTable[nTableIndex].count;
                        }

                        if(bHandlerResponse) 
                        {
                            pSession->nCurrentRequestBytes = MIN(pSession->nAnsiPacketSize, pSession->nTotalRequestBytes);

                            nError = ansiMakePReadOffsetFrame(pSession, TRUE, pTargetTable[nTableIndex].tblType | pTargetTable[nTableIndex].tbl, 
                                pSession->nCurrentIndex, pSession->nCurrentRequestBytes, (BYTE **)&pAnsiFrame, &nFrameLength);
                        }
                        break;
                   case PROC_WRITE:
                        if(pTargetTable[nTableIndex].fnPreHandler != NULL)
                        {
                            bHandlerResponse = ((fnProcPreHandler)pTargetTable[nTableIndex].fnPreHandler)(pSession, pOndemand, 
                                pTargetTable[nTableIndex].tblType, 
                                pTargetTable[nTableIndex].tbl,
                                pTargetTable[nTableIndex].preParam,
                                (BYTE *)pProcedureTable, &nTableLength);
                        }
                        else // Standard Table #7 (Default)
                        {
                            nTableLength = MakeProcedureTable(pProcedureTable, 
                                    (BYTE)pTargetTable[nTableIndex].preParam, 
                                    pTargetTable[nTableIndex].tblType, 
                                    ANSI_PROC_RES_NOT_POST, 
                                    //ANSI_PROC_RES_COMPLETION, 
                                    pSession->nProcSeq ++ , NULL, 0);
                        }
                        if(bHandlerResponse)
                        {
                            pSession->bReadResponse = FALSE;

                            nError = ansiMakeFullWriteFrame(pSession, TRUE, 7,
                                    (BYTE *)pProcedureTable, nTableLength,
                                    (BYTE **)&pAnsiFrame, &nFrameLength);
                        }
                        break;
                   case TBL_WRITE:
                        if(pTargetTable[nTableIndex].fnPreHandler != NULL)
                        {
                            bHandlerResponse = ((fnProcPreHandler)pTargetTable[nTableIndex].fnPreHandler)(pSession, pOndemand, 
                                pTargetTable[nTableIndex].tblType, 
                                pTargetTable[nTableIndex].tbl,
                                pTargetTable[nTableIndex].preParam,
                                (BYTE *)pProcedureTable, &nTableLength);

                            if(bHandlerResponse)
                            {
                                pSession->bReadResponse = FALSE;

                                nError = ansiMakeFullWriteFrame(pSession, TRUE, pTargetTable[nTableIndex].tblType | pTargetTable[nTableIndex].tbl,
                                    (BYTE *)pProcedureTable, nTableLength,
                                    (BYTE **)&pAnsiFrame, &nFrameLength);
                            }
                        }
                        else
                        {
                            nError = ANSIERR_SYSTEM_ERROR;
                            XDEBUG(" " ANSI_COLOR_RED "INVALID TABLE LIST : No Pre Handler!!" ANSI_NORMAL "\r\n");
                        }
                        break;
                   case FULL_READ:
                   default:
                        if(pTargetTable[nTableIndex].fnPreHandler != NULL)
                        {
                            bHandlerResponse = ((fnReadPreHandler)pTargetTable[nTableIndex].fnPreHandler)(pSession, pOndemand, 
                                pTargetTable[nTableIndex].tblType, 
                                pTargetTable[nTableIndex].tbl,
                                &pSession->nCurrentIndex, &pSession->nTotalRequestBytes);
                        }
                        if(bHandlerResponse)
                        {
                            nError = ansiMakeFullReadFrame(pSession, TRUE, pTargetTable[nTableIndex].tblType | pTargetTable[nTableIndex].tbl, 
                                (BYTE **)&pAnsiFrame, &nFrameLength);
                        }
                        break;
               }

               if(bHandlerResponse)
               {
                    if(nError == ANSIERR_NOERROR)
                    {
                        if((bSendResult = SendCommand(pOndemand, &id, (const char *)pAnsiFrame, nFrameLength)) == TRUE) 
                        {
                        /** ANSI Frame에 관련된 Session 정보를 Update 해야 한다 */
                            ansiToggleSendCtrl(pSession);
                        }
                    }
                    else
                    {
                        XDEBUG(" MakeFrameError : %s\r\n", ansiGetErrorMessage(nError));
                        nState = STATE_LOGOFF;
                        if(pAnsiFrame != NULL) FREE(pAnsiFrame);
                        break;
                    }
                    if(pAnsiFrame != NULL) FREE(pAnsiFrame);

                    m_Locker.Lock();
                    ClearOperation(pOndemand);
                    pOndemand->nState = nState;
                    pOndemand->bReply = FALSE;
                    ResetEvent(pOndemand->hEvent);
                    m_Locker.Unlock();
                    nState = STATE_WAIT_REPLY;
               }
               else
               {
                   /** Meter로 Command를 Send 하지 않았을 경우에는 Index만 증가시킨다 */
                    nTableIndex ++;
               }
               break;

          case STATE_READ_NEXT:
               if(pTargetTable[nTableIndex].id == 0) 
               {
                   nState = STATE_LOGOFF;
                   break;
               }

               /** Session Setting */
               pSession->nCurrentRequestBytes = MIN(pSession->nAnsiPacketSize, 
                       pSession->nTotalRequestBytes - pSession->nTotalReceivedBytes);

               /*
               XDEBUG(" " ANSI_COLOR_MAGENTA "STEP: ANSI READ TABLE [%c%03d] (%s)" ANSI_NORMAL "\r\n", 
                       pSession->tableType, pTargetTable[nTableIndex].tbl, pTargetTable[nTableIndex].descr);
                       */

               pAnsiFrame = NULL; nFrameLength = 0;
               switch(pTargetTable[nTableIndex].actionType)
               {
                   case POFFSET_READ:
                        nError = ansiMakePReadOffsetFrame(pSession, TRUE, pTargetTable[nTableIndex].tblType | pTargetTable[nTableIndex].tbl, 
                            pSession->nCurrentIndex, pSession->nCurrentRequestBytes, (BYTE **)&pAnsiFrame, &nFrameLength);
                        break;
                        /*
                   case PINDEX_READ:
                        nError = ansiMakePReadIndexFrame(pSession, TRUE, pTargetTable[nTableIndex].tblType | pTargetTable[nTableIndex].tbl, 
                            pSession->nCurrentIndex + pSession->nTotalReceivedBytes, pSession->nCurrentRequestBytes, (BYTE **)&pAnsiFrame, &nFrameLength);
                        break;
                   case FULL_READ:
                   default:
                        nError = ansiMakeFullReadFrame(pSession, TRUE, pTargetTable[nTableIndex].tblType | pTargetTable[nTableIndex].tbl, 
                            (BYTE **)&pAnsiFrame, &nFrameLength);
                        break;
                        */
                   default:
                        nError = ANSIERR_INVALID_ACTION;
                        break;
               }

               if(nError == ANSIERR_NOERROR)
               {
                   if((bSendResult = SendCommand(pOndemand, &id, (const char *)pAnsiFrame, nFrameLength)) == TRUE) 
                   {
                    /** ANSI Frame에 관련된 Session 정보를 Update 해야 한다 */
                       ansiToggleSendCtrl(pSession);
                   }
               }
               else
               {
                   XDEBUG(" MakeFrameError : %s\r\n", ansiGetErrorMessage(nError));
                   nState = STATE_LOGOFF;
                   if(pAnsiFrame != NULL) FREE(pAnsiFrame);
                   break;
               }
               if(pAnsiFrame != NULL) FREE(pAnsiFrame);

               m_Locker.Lock();
               ClearOperation(pOndemand);
               pOndemand->nState = nState;
               pOndemand->bReply = FALSE;
               ResetEvent(pOndemand->hEvent);
               m_Locker.Unlock();
               nState = STATE_WAIT_REPLY;
               break;

		  case STATE_WAIT_REPLY:
                WaitForSingleObject(pOndemand->hEvent, (int)(GetReplyTimeout()/1000));
                m_Locker.Lock();
                if(pOndemand->bPartialWait)
                {
                    pOndemand->bPartialWait = FALSE;
                    m_Locker.Unlock();
                    continue;
                }
                m_Locker.Unlock();
                if(pOndemand->bReply)
                {
                    switch(pOndemand->nState)
                    {
                        case STATE_IDENTIFICATION:
                            if(pSession->nErrorCode != ANSI_RES_NO_ERROR)
                            {
                                nState = STATE_ERROR;
                                break;
                            }
                            nState = STATE_NEGOTIATE;
                            break;
                        case STATE_NEGOTIATE:
                            if(pSession->nErrorCode != ANSI_RES_NO_ERROR)
                            {
                                nState = STATE_ERROR;
                                break;
                            }
                            nState = STATE_LOGON;
                            break;
                        case STATE_LOGON:
                            if(pSession->nErrorCode != ANSI_RES_NO_ERROR)
                            {
                                nState = STATE_ERROR;
                                break;
                            }
                            nState = STATE_SECURITY;
                            break;
                        case STATE_SECURITY:
                            if(pSession->nErrorCode != ANSI_RES_NO_ERROR)
                            {
                                nState = STATE_LOGOFF;
                                break;
                            }
	                        nState = STATE_PRE_READ;
                            break;
                        case STATE_READ_NEXT:
                        case STATE_PRE_READ:
                        case STATE_READ_TABLE:
                            /** 전체 요청 Bytes 만큼 읽었거나 요청한 Bytes 보다 현 Frame에서 적게 왔을 때 */
                            if(pSession->nTotalRequestBytes <= pSession->nTotalReceivedBytes 
                                    || pSession->nCurrentRequestBytes > pSession->nCurrentReceivedBytes)
                            {
                                pNextTable = NULL;
                                if(pSession->bSave)
                                {
                                    AppendTable(pSession->pChunk, pSession->tableType, pSession->nTableNumber, 
                                        (const char*)pSession->pPacketChunk->GetBuffer(), pSession->pPacketChunk->GetSize());
                                }
                                if(pSession->fnHandler != NULL)
                                {
                                    pNextTable = (ANSITBLLIST *)((fnPostHandler)pSession->fnHandler)(pSession, pOndemand, 
                                            pSession->tableType, pSession->nTableNumber,
                                            pSession->pPacketChunk->GetBuffer(), pSession->pPacketChunk->GetSize());
                                }
                                /** Table이 변경되었을 때 */
                                if(pNextTable != NULL)
                                {
                                    pTargetTable = pNextTable;
                                    nTableIndex = 0;
                                }
                                else
                                {
                                    nTableIndex ++;
                                }
                                nState = nOrigState;    // 이전 상태로 전환한다
                            }
                            else
                            {
                                nState = STATE_READ_NEXT;
                            }
                            break;


                        case STATE_LOGOFF:
                            nState = STATE_TERMINATE;
                            break;
                        case STATE_TERMINATE:
                            nState = STATE_LAST_ACK;
                            break;
                        default:
                            XDEBUG(" STATE_WAIT_REPLAY : Unknown STATE %d\r\n", pOndemand->nState);
                            nState = STATE_ERROR;
                            break;
                    }
                }
                else
                {
                    /** Timeout */
                    XDEBUG(" Request Timeout\r\n");
                    nState = STATE_ERROR;
                }
                break;

		  case STATE_READ_NODEINFO : 						// NODE_INFO를 읽어 본다.
			   addr = ENDI_ROMAP_NODE_KIND;
			   len  = sizeof(ENDI_NODE_INFO);

               pOndemand->stat.nCount ++;
               pOndemand->stat.nSendCount ++;
               pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
			   GetTimeTick(&pOndemand->tmSend);
			   nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &n, GetReplyTimeout());
			   if (nError != CODIERR_NOERROR)
			   {
        		   if (pOndemand->nRetry < GetRetryCount())				// 3회 이상 연속 실패시 종료
				   {
            		   pOndemand->nRetry++;
					   nState = STATE_READ_NODEINFO;
        		   }else nState = STATE_OK;   
                   pOndemand->stat.nError ++;
				   break;
			   }

               pOndemand->nRetry = 0;
               pOndemand->stat.nCount ++;
               pOndemand->stat.nRecvCount ++;
               pOndemand->stat.nRecv += n + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
               UpdateStatistics(pOndemand, (BYTE *)szBuffer, n);

               nodeInfo = (ENDI_NODE_INFO *) szBuffer;

               fwVersion = nodeInfo->FIRMWARE_VERSION;
               fwBuild = nodeInfo->FIRMWARE_BUILD;
               hwVersion = nodeInfo->HARDWARE_VERSION;

               if(strncmp(nodeInfo->NODE_KIND, "NAMR-P201SR+", 12)==0)
               {
                   /** GE I210+ Meter는 Toggle Check를 하지 않는다 */
                   bI210Pluse = TRUE;
                   pTargetTable = m_AnsiMeterTblOption_0_i210pulse;
    		       pOndemand->nResult = ANSI_NO_ERROR;
                   ((ANSISESSION *)pOndemand->pSession)->bDisableToggleCheck = TRUE;
                   nState = STATE_READ_TABLE;
                   XDEBUG("Meter Reading Type : I210+ Meter\r\n");
               }
               else
               {
                   nState = STATE_READ_AMRINFO;
               }
			   break;	 

		  case STATE_READ_AMRINFO : 						// AMR_INFO를 읽어 본다.
			   addr = ENDI_ROMAP_TEST_FLAG;
			   len  = sizeof(AMR_PARTIAL_INFO);

               pOndemand->stat.nCount ++;
               pOndemand->stat.nSendCount ++;
               pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
			   GetTimeTick(&pOndemand->tmSend);
			   nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &n, GetReplyTimeout());
			   if (nError != CODIERR_NOERROR)
			   {
        		   if (pOndemand->nRetry < GetRetryCount())				// 3회 이상 연속 실패시 종료
				   {
            		   pOndemand->nRetry++;
					   nState = STATE_READ_AMRINFO;
        		   }else nState = STATE_OK;   
                   pOndemand->stat.nError ++;
				   break;
			   }

               pOndemand->nRetry = 0;
               pOndemand->stat.nCount ++;
               pOndemand->stat.nRecvCount ++;
               pOndemand->stat.nRecv += n + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
               UpdateStatistics(pOndemand, (BYTE *)szBuffer, n);

               amrInfo = (AMR_PARTIAL_INFO *) szBuffer;

               permitMode = amrInfo->permit_mode;
               permitState = amrInfo->permit_state;
               alarmMask = amrInfo->alarm_mask;
               testFlag = amrInfo->test_flag;

               nState = STATE_IDENTIFICATION;
			   break;	 

		  case STATE_ERROR :
               XDEBUG(" STATE_ERROR Retry=%d\r\n", pOndemand->nRetry);
        	   if (pOndemand->nRetry < GetRetryCount())				// 3회 이상 연속 실패시 종료
               {
                    nState = pOndemand->nState;
                    pOndemand->nRetry++;
               }
               else
               {
                   switch(pOndemand->nState)
                   {
                        case STATE_IDENTIFICATION:
                        case STATE_NEGOTIATE:
                        case STATE_LOGOFF:
                            nState = STATE_TERMINATE;
    		                pOndemand->nResult = ANSI_NO_RESPONSE;
                            break;
                        case STATE_LOGON:
                        case STATE_SECURITY:
                        case STATE_PRE_READ:
                        case STATE_READ_TABLE:
                        case STATE_READ_NEXT:
                            nState = STATE_LOGOFF;
    		                pOndemand->nResult = ANSI_NO_RESPONSE;
                            break;
                        default:
                            nState = STATE_OK;
                            break;
                   }
               }
               break;

		  case STATE_OK :
			   endiDisconnect(pOndemand->endi);

               /** Session Delete */
               ansiDeleteSession(pSession);
               pSession = NULL;

			   GetTimeTick(&pOndemand->tmEnd);
			   time(&tmEnd);

			   nLength = pOndemand->nLength;
    		   if (pOndemand->nResult == ANSI_NO_ERROR)
			   {
					nResult = ONDEMAND_ERROR_OK;
				    if (pOndemand->stat.nCount == 0 || pOndemand->stat.nRecvCount == 0)
						 nElapse = 0;
					else nElapse = pOndemand->stat.nResponseTime / pOndemand->stat.nRecvCount;
					pOndemand->stat.nUseTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmEnd);
					pOndemand->stat.nConnectTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmConnect);

                    m_pEndDeviceList->UpdateSensorInventory(&id, NULL, PARSER_TYPE_NOT_SET, NULL, 
                            (((WORD) (hwVersion & 0xf0)) << 4) + (hwVersion & 0x0f),
                            (((WORD) (fwVersion & 0xf0)) << 4) + (fwVersion & 0x0f),
                            (((WORD) (fwBuild >> 4)) * 10) + (fwBuild & 0x0f));
                    m_pEndDeviceList->SetRouteDeviceInfo(&id, permitMode ? TRUE : FALSE, 
                        permitState ? TRUE : FALSE, 0xFF, 0xFF);
                    m_pEndDeviceList->SetEndDeviceFlag(&id, ntoshort(alarmMask), alarmFlag, testFlag);

			   		XDEBUG("\r\n------------------[ OK ]-----------------\r\n");
					// 실제 데이터 길이는 처음 Chunk를 넘결 받을때 넘겨 받은 길이를 빼고 계산한다.
			   		XDEBUG("   Metering Data Size = %d byte(s)\r\n", pChunk->GetSize()-pChunk->GetCommitSize());

					pOndemand->nSensorError = CODIERR_NOERROR;
					pOndemand->nMeterError = 0;

					if (pStat != NULL)
						memcpy(pStat, &pOndemand->stat, sizeof(METER_STAT));
			   }
			   else
			   {
				    if (pOndemand->stat.nCount == 0 || pOndemand->stat.nRecvCount == 0)
						 nElapse = 0;
					else nElapse = pOndemand->stat.nResponseTime / pOndemand->stat.nRecvCount;
					pOndemand->stat.nUseTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmEnd);
					pOndemand->stat.nConnectTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmConnect);

                    /*
				    if (pOndemand->ansi.packet_cmd == ANSI_SECURITY)
						pOndemand->nMeterError = P_STAT_ERR_SECURITY; 
                        */

					nResult = ONDEMAND_ERROR_FAIL;
			   		XDEBUG("\r\n");
			   		XDEBUG("-----------------[ FAIL ]----------------\r\n");
			   }

               XDEBUG("      Metering Elapse = %d second(s)\r\n", tmEnd-tmStart);
               XDEBUG("\r\n");
               XDEBUG("       Transmit Count = %d frams(s)\r\n", pOndemand->stat.nCount);
               XDEBUG("           Send Count = %d frams(s)\r\n", pOndemand->stat.nSendCount);
               XDEBUG("        Receive Count = %d frams(s)\r\n", pOndemand->stat.nRecvCount);
               XDEBUG("   Error Packet Count = %d frams(s)\r\n", pOndemand->stat.nError);
               XDEBUG("           Total Send = %d byte(s)\r\n", pOndemand->stat.nSend);
               XDEBUG("        Total Receive = %d byte(s)\r\n", pOndemand->stat.nRecv);
#if 0
               XDEBUG("  Total Response Time = %d ms\r\n", pOndemand->stat.nResponseTime);
               XDEBUG("Average Response Time = %d ms\r\n", nElapse);
               XDEBUG("    Min Response Time = %d ms\r\n", pOndemand->stat.nMinTime);
               XDEBUG("    Max Response Time = %d ms\r\n", pOndemand->stat.nMaxTime);
#endif
               XDEBUG("       Total Use Time = %d ms\r\n", pOndemand->stat.nUseTime);
               XDEBUG("         Connect Time = %d ms\r\n", pOndemand->stat.nConnectTime);
               XDEBUG("-----------------------------------------\r\n");
               XDEBUG("\r\n");
	
			   endiClose(pOndemand->endi);

			   LockOperation();
			   DeleteOperation(pOndemand);	
               pOndemand = NULL;
			   UnlockOperation();

			   nState = STATE_DONE;
			   break;

		  case STATE_RETRY :
			   if (pOndemand->endi != NULL)
			   {
				   if (bConnected) endiDisconnect(pOndemand->endi);
				   endiClose(pOndemand->endi);
				   pOndemand->endi = NULL;
			   }

			   LockOperation();
			   DeleteOperation(pOndemand);	
               pOndemand = NULL;
			   UnlockOperation();

			   nRetry++;
			   if (nRetry >= GetRetryCount())
			   {
				   nState = STATE_DONE;
				   break;
			   }

			   USLEEP(1000000);
			   nState = STATE_OPEN;
			   break;
		}
	}

	if (!bConnected)
		 nState = ENDISTATE_CONNECTION_ERROR;
	else nState = (nResult == ONDEMAND_ERROR_OK) ? ENDISTATE_NORMAL : ENDISTATE_METER_ERROR;
	m_pEndDeviceList->SetState(&id, nState);
	m_pEndDeviceList->SetBusy(&id, FALSE);
	m_pEndDeviceList->VerifyEndDevice(&id);

	if (nState != nSaveState)
		commZruErrorEvent(&id, (nState == ENDISTATE_NORMAL) ? 0 : 1);
	return nResult;
}

//////////////////////////////////////////////////////////////////////
// GE Meter Reading
//////////////////////////////////////////////////////////////////////
void CANSIParser::AppendTable(CChunk *pChunk, BYTE tableType, WORD nTableNumber, const char *pszFrame, int nLength)
{
	ANSI_TABLE_HEADER table;

	if (!pChunk || !pszFrame)
		return;

	if (nLength <= 0)
		return;

	memset(&table, 0, sizeof(ANSI_TABLE_HEADER));
    table.kind = tableType;
    sprintf(table.table, "%03d", nTableNumber & 0x00FF);
	table.length = BigToHostShort(nLength);

	pChunk->Add((char *)&table, sizeof(ANSI_TABLE_HEADER));
	pChunk->Add(pszFrame, nLength);

    /*
	XDEBUG("---------------- %c%03d (Len=%d) --------------\r\n", table.kind, nTableNumber & 0x00FF, nLength);	
	XDUMP((char *)&table, sizeof(ANSI_TABLE_HEADER), TRUE);
	XDUMP(pszFrame, nLength, TRUE);
	XDEBUG("--------------------- %c%03d -------------------\r\n",  table.kind, nTableNumber & 0x00FF);
    */
}

void CANSIParser::OnData(EUI64 *pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
{
	ANSI_HEADER  	*pHeader;
    DATASTREAM      *pStream;
    int             nSize;

    /** Issue #2659: pOndemand에 대한 NULL 검사를 수행한다 */
    if(pOndemand == NULL) return;

	pOndemand->stat.nCount++;
	pOndemand->stat.nRecvCount++;
    pOndemand->stat.nRecv += nLength + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);

    pStream = pOndemand->pDataStream;
    if(pStream && pOndemand->pSession && !ParseStream((ANSISESSION *)pOndemand->pSession, pStream, pszData, nLength, pOndemand))
    {
        pHeader =  (ANSI_HEADER *)pStream->pszBuffer;
        if(pHeader == NULL)
        {
            // ACK/NAK
            //XDEBUG(" ANSI STREAM PARSING FAIL\r\n");
            return;
        }
        nSize = BigToHostShort(pHeader->length);
        /** 아직 Frame이 완료되지 않았을 때 */
        XDEBUG(" PENDDING FRAME TOT=%d CURR=%d\r\n", nSize, pStream->nLength);
    }

}

BOOL CANSIParser::OnFrame(ANSISESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength, int nRemain, void *pCallData)
{
    char                    szGUID[20];
    ANSI_HEADER             *pHeader;
    ANSI_FRAME              *pFrame;
    ANSI_TABLE_PAYLOAD      *pTable = NULL;
    ANSI_NEGOTIATE_PAYLOAD  *pNegoPayload;
    ANSI_RESULT_CODE        *pResult;
    int                     nSize;

    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;

    if(pOndemand == NULL) return TRUE;

    eui64toa(&pOndemand->id, szGUID);

    pHeader =  (ANSI_HEADER *)pStream->pszBuffer;
    pFrame =  (ANSI_FRAME *)pStream->pszBuffer;
    if(pHeader == NULL)
    {
        XDEBUG(" ANSI STREAM PARSING FAIL\r\n");
        return FALSE;
    }
    nSize = BigToHostShort(pHeader->length);

    /** 이미 Free된 Session 정보 */
    if(pSession == NULL) return FALSE;

    pResult = (ANSI_RESULT_CODE *)pFrame->payload;
    pSession->nErrorCode = pResult->status;

    XDEBUG(" ANSI OnFrame Length = %d, Size = %d, Request = %d\r\n", nLength, nSize, pSession->nTotalRequestBytes);
    if(pResult->status != ANSI_RES_NO_ERROR)
    {
        XDEBUG(" " ANSI_BACK_RED "ERROR : %s" ANSI_NORMAL "\r\n", ansiGetStatus(pResult->status));
    }
    else
    {
        switch(pOndemand->nState)
        {
            case STATE_NEGOTIATE:
                pNegoPayload = (ANSI_NEGOTIATE_PAYLOAD *)pFrame->payload;
                pSession->nAnsiPacketSize = BigToHostShort(pNegoPayload->packetSize);
                XDEBUG(" ANSI Negotiation : packetSize %d, nbrPacket %d, baudRate %d\r\n",
                        BigToHostShort(pNegoPayload->packetSize),
                        pNegoPayload->nbrPacket,
                        pNegoPayload->baudRate);
                break;

            case STATE_PRE_READ:
            case STATE_READ_TABLE:
            case STATE_READ_NEXT:
                if(pSession->bReadResponse == TRUE)
                {
                    WORD readBytes = 0;

                    pTable = (ANSI_TABLE_PAYLOAD *)pFrame->payload;
                    readBytes = BigToHostShort(pTable->count);

                    pSession->nCurrentReceivedBytes = nLength;
                    pSession->nTotalReceivedBytes += readBytes;

                    XDEBUG(" ANSI Table Read Bytes %d\r\n", readBytes);
                    // Partial read를 위해 index 증가
                    pSession->nCurrentIndex += readBytes;

                    pSession->pPacketChunk->Add((const char*)pTable->data, readBytes);
                }
                break;
        }
    }

    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    m_Locker.Unlock();

    // Event signal
    SetEvent(pOndemand->hEvent);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////

/** Standard Table 7 : Procedure.
  */
int CANSIParser::MakeProcedureTable(ANSI_ST_TABLE7 *pTable, BYTE nProcedureNumber, WORD nTableType, BYTE nResponse, BYTE nSeq, BYTE *pParam, WORD nParamLength)
{
    int nLen = 0;

    if(pTable == NULL) return nLen;

    XDEBUG(" " ANSI_COLOR_MAGENTA "  + ANSI PROC 0x%02X %s" ANSI_NORMAL "\r\n", nProcedureNumber, ansiGetProcedureName(nTableType, nProcedureNumber));
    memset(pTable, 0, sizeof(ANSI_ST_TABLE7));

    pTable->TBL_PROC_NBR = nProcedureNumber;
    if(nTableType == ANSI_MANUFACT_TBL) 
    {
        pTable->STD_VS_MFG_FLAG = TRUE;
    }
    pTable->SELECTOR = nResponse;
    pTable->SEQ_NUM = nSeq;
    if(pParam != NULL && nParamLength > 0)
    {
        nLen = MIN(nParamLength, 8);
        memcpy(pTable->PARAM, pParam, nLen);
    }

    return 3 + nLen;
}
#if 0
/** Make I210 Relay Control Table
*/
int CANSIParser::MakeI210RelayTable(ANSI_ST_TABLE7 *pTable, BYTE nProcedureNumber, WORD nTableType, BYTE nResponse, BYTE nSeq, BYTE set, BYTE level, BYTE hour, BYTE min, BYTE sec)
{
	int nLen = 0;

	if(pTable == NULL) return nLen;

	XDEBUG(" " ANSI_COLOR_MAGENTA "  + ANSI I210 Relay 0x%02X %s" ANSI_NORMAL "\r\n", nProcedureNumber, ansiGetProcedureName(nTableType, nProcedureNumber));
	memset(pTable, 0, sizeof(ANSI_ST_TABLE7));

	pTable->TBL_PROC_NBR = nProcedureNumber;
	pTable->STD_VS_MFG_FLAG = FALSE;

	pTable->SELECTOR = nResponse;
	pTable->SEQ_NUM = 0xc;
	
	pTable->PARAM[0] = level;
	pTable->PARAM[1] = set;
	pTable->PARAM[2] = hour;
	pTable->PARAM[3] = min;
	pTable->PARAM[4] = sec;

	nLen = 8;
	/*
	if(pParam != NULL && nParamLength > 0)
	{
		nLen = MIN(nParamLength, 8);
		memcpy(pTable->PARAM, pParam, nLen);
	}
	*/

	return nLen;
}
#endif


/** 내부적으로 사용되는 Meter Info 구조체를 생성한다.
  */
void *CANSIParser::NewMeterInfo(void* pTableList)
{
    ANSI_METER_INFO * pMeterInfo = (ANSI_METER_INFO *)MALLOC(sizeof(ANSI_METER_INFO));       // Ansi Meter Info 

    if(pMeterInfo != NULL)
    {
        memset(pMeterInfo, 0, sizeof(ANSI_METER_INFO));
        pMeterInfo->nOrder = ORDER_LSB;     // default lsb
        pMeterInfo->nUomEntries = 62;       // default Unit of Measure Entry (Table 12)
        pMeterInfo->nMeterMode= 2;          // TOU
        pMeterInfo->pTableList = pTableList;
    }

    return pMeterInfo;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Table Handler
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CANSIParser::fnPreS007Default(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength)
{
    if(pSession == NULL) return TRUE;

    *nProcedureLength = CANSIParser::MakeProcedureTable((ANSI_ST_TABLE7 *)szProcedure, nCommand, nTableType, ANSI_PROC_RES_NOT_POST, 
            pSession->nProcSeq ++ , NULL, 0);

    return TRUE;
}

BOOL CANSIParser::fnPreS007ResetLp(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength)
{
    BYTE param[1] = { 0x03 }; // LP Data Set 1

    if(pSession == NULL) return TRUE;

    *nProcedureLength = CANSIParser::MakeProcedureTable((ANSI_ST_TABLE7 *)szProcedure, nCommand, nTableType, 
            ANSI_PROC_RES_NOT_POST, 
            pSession->nProcSeq ++ , param, sizeof(param));

    return TRUE;
}

BOOL CANSIParser::fnPreS007StartProg(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength)
{
	time_t	        now;
	struct tm       when;
    BYTE param[6];
    int idx = 0;

    if(pSession == NULL) return TRUE;

	time(&now);
	when = *localtime(&now);

    param[idx] = 0x00;          idx++;      // Start Programming
	param[idx] = (when.tm_year+1900) % 100;     idx++;
	param[idx] = when.tm_mon + 1;               idx++;
	param[idx] = when.tm_mday;                  idx++;
	param[idx] = when.tm_hour;                  idx++;
	param[idx] = when.tm_min;                   idx++;

    *nProcedureLength = CANSIParser::MakeProcedureTable((ANSI_ST_TABLE7 *)szProcedure, nCommand, nTableType, 
            ANSI_PROC_RES_NOT_POST, 
            pSession->nProcSeq ++ , param, idx);

    return TRUE;
}

BOOL CANSIParser::fnPreS007StopProg(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength)
{
	time_t	        now;
	struct tm       when;
    BYTE param[6];
    int idx = 0;

    if(pSession == NULL) return TRUE;

	time(&now);
	when = *localtime(&now);

    param[idx] = 0x03;          idx++;      // Stop Programming/No clear/No Logoff
	param[idx] = (when.tm_year+1900) % 100;     idx++;
	param[idx] = when.tm_mon + 1;               idx++;
	param[idx] = when.tm_mday;                  idx++;
	param[idx] = when.tm_hour;                  idx++;
	param[idx] = when.tm_min;                   idx++;

    *nProcedureLength = CANSIParser::MakeProcedureTable((ANSI_ST_TABLE7 *)szProcedure, nCommand, nTableType, 
            ANSI_PROC_RES_NOT_POST, 
            pSession->nProcSeq , param, idx);

    /** Stop Programm 으로 Proc Seq reset */
    pSession->nProcSeq = 0;

    return TRUE;
}

BOOL CANSIParser::fnPreS007SetTime(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength)
{
	time_t	        now;
	struct tm       when;
    BYTE param[8];
    int idx = 0;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return TRUE;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

	time(&now);
	when = *localtime(&now);

    pMeterInfo->bTimesyncActivate = TRUE;

    // Set Time, Set Date, Set Date/Time Qualifier
    param[idx] = 0x07;          idx++;      
	param[idx] = (when.tm_year+1900) % 100;     idx++;
	param[idx] = when.tm_mon + 1;               idx++;
	param[idx] = when.tm_mday;                  idx++;
	param[idx] = when.tm_hour;                  idx++;
	param[idx] = when.tm_min;                   idx++;
	param[idx] = when.tm_sec;                   idx++;
	param[idx] = when.tm_wday & 0x07;           idx++; // DST (winter)

    *nProcedureLength = CANSIParser::MakeProcedureTable((ANSI_ST_TABLE7 *)szProcedure, nCommand, nTableType, 
            ANSI_PROC_RES_COMPLETION, 
            pSession->nProcSeq ++ , param, idx);

    XDEBUG(" METER TIME SYNC : %04d/%02d/%02d %02d:%02d:%02d\r\n",
                    (when.tm_year+1900), when.tm_mon + 1, when.tm_mday,
                    when.tm_hour, when.tm_min, when.tm_sec);

    return TRUE;
}

/** Meter 시간이 Option에 지정된 시간 보다 차이가 크고 자동 Meter TimeSync가 Enable 되어 있을 때 Timesync
  *
  * @return Time Sync 필요가 없을 때 False 
  */
BOOL CANSIParser::fnPreS007ConditionalSetTime(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength)
{
	time_t	        now, prev;
	struct tm       tm_prev, tm_when;
    int             timeGap;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return TRUE;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

	time(&now);
    tm_when = *localtime(&now);

    pMeterInfo->bTimesyncActivate = FALSE;

    if(m_bEnableMeterTimesync == FALSE) 
    {
        XDEBUG(" + Disable Meter Timesync\r\n");
        return FALSE;
    }

    memset(&tm_prev, 0, sizeof(struct tm));
    tm_prev.tm_year = pMeterInfo->currentTime.YEAR + 100;
    tm_prev.tm_mon  = pMeterInfo->currentTime.MONTH - 1;
    tm_prev.tm_mday = pMeterInfo->currentTime.DAY;
    tm_prev.tm_hour = pMeterInfo->currentTime.HOUR;
    tm_prev.tm_min  = pMeterInfo->currentTime.MINUTE;
    tm_prev.tm_sec  = pMeterInfo->currentTime.SECOND;
    prev = mktime(&tm_prev);

    switch(pOndemand->nOption)
    {
        case WRITE_OPTION_TIMESYNC:
            /** 명시적인 Timesync일 경우 시간 오차에 관계없이 동기화를 수행한다.
              */
            break;
        default:
            timeGap = abs(now - prev);

            // Issue #402, #837 2006년 이전이면 자동 미터 시간동기화를 하지 않는다
            if(tm_when.tm_year + 1900 < 2006) return FALSE;

            /** 지정된 TimesyncThreshold 보다 작으면 동기화 하지 않는다
            */
            if(m_nTimesyncThresholdLow > 0 && timeGap < m_nTimesyncThresholdLow)
            {
                XDEBUG(" METER TIMESYNC THRESHOLD DISABLE:Low Threshold=%d, Meter=%02d/%02d/%02d %02d:%02d:%02d, Diff=%d\r\n",
                        m_nTimesyncThresholdLow,
                        pMeterInfo->currentTime.YEAR,
                        pMeterInfo->currentTime.MONTH,
                        pMeterInfo->currentTime.HOUR,
                        pMeterInfo->currentTime.MINUTE,
                        pMeterInfo->currentTime.SECOND,
                        timeGap);
                return FALSE;
            }
            /** 지정된 TimesyncThreshold 보다 크면 동기화 하지 않는다
            */
            if(m_nTimesyncThresholdHigh > 0 && timeGap > m_nTimesyncThresholdHigh)
            {
                XDEBUG(" METER TIMESYNC THRESHOLD DISABLE:High Threshold=%d, Meter=%02d/%02d/%02d %02d:%02d:%02d, Diff=%d\r\n",
                        m_nTimesyncThresholdHigh,
                        pMeterInfo->currentTime.YEAR,
                        pMeterInfo->currentTime.MONTH,
                        pMeterInfo->currentTime.HOUR,
                        pMeterInfo->currentTime.MINUTE,
                        pMeterInfo->currentTime.SECOND,
                        timeGap);
                return FALSE;
            }
            break;
    }

    XDEBUG(" AUTOMATIC METER TIME SYNC\r\n");
    return CANSIParser::fnPreS007SetTime(pSession, pOndemand, nTableType, nTableNumber, nCommand, szProcedure, nProcedureLength);
}

BOOL CANSIParser::fnPreS007ConvertTou(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength)
{
    BYTE param[1];
    int idx = 0;

    if(pSession == NULL) return TRUE;

    param[idx] = 0x02;          idx++;      // TOU Convert

    *nProcedureLength = CANSIParser::MakeProcedureTable((ANSI_ST_TABLE7 *)szProcedure, nCommand, nTableType, 
            ANSI_PROC_RES_NOT_POST, 
            pSession->nProcSeq ++ , param, idx);

    return TRUE;
}

BOOL CANSIParser::fnPreS007RelayControl(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength)
{
    BYTE param[6];
    int idx = 0;
    BYTE nEnergyLevel = 0;

    if(pSession == NULL) return TRUE;

    /** GE Relay control I210+
      *
      * M114 Table 참조
      */
    switch(pOndemand->nOption)
    {
        case WRITE_OPTION_ACTOFF:
            param[idx] = 0x64;                  idx++;      // Level (100)
	        param[idx] = 0x10;                  idx++;      // ToSet (Set Lockout : 16)
            break;
        case WRITE_OPTION_ACTON:
            param[idx] = 0x00;                  idx++;      // Level (0)
	        param[idx] = 0x10;                  idx++;      // ToSet (Clear Lockout : 16)
            break;
        case WRITE_OPTION_RELAYON:
            param[idx] = 0x00;                  idx++;      // Level (0)
	        param[idx] = 0x01;                  idx++;      // ToSet (Automatci Switch closure : 1)
            break;
        case WRITE_OPTION_RELAYOFF:
            param[idx] = 0x64;                  idx++;      // Level (100)
	        param[idx] = 0x01;                  idx++;      // ToSet (Automatic Switch open : 1)
            break;
        case ENERGY_LEVEL_OPTION_MIN: // Relay On
            m_pEndDeviceList->GetEnergyLevel(&pOndemand->id, &nEnergyLevel, NULL);
            if(nEnergyLevel <= 0)
            {
                return FALSE;
            }
#if 0
            // 명시적 요청이기 때문에 같은 level 이라도 동작을 수행한다
            if(nEnergyLevel == ENERGY_LEVEL_OPTION_MIN) 
            {
                // Level이 Min 이라면 이 Operation을 수행할 필요가 없다.
                return FALSE;
            }
#endif
            if(pOndemand->nRelayCtrlStep==0) // 첫번째 단계인 Activate 단계일 때
            {
                param[idx] = 0x00;                  idx++;      // Level (0)
	            param[idx] = 0x10;                  idx++;      // ToSet (Clear Lockout : 16)
            }
            else
            {
                param[idx] = 0x00;                  idx++;      // Level (0)
	            param[idx] = 0x2F;                  idx++;      // ToSet (Close and Lockout : 47)
            }
            pOndemand->nRelayCtrlStep ++;
            break;
        case ENERGY_LEVEL_OPTION_MAX: // Relay Off
            m_pEndDeviceList->GetEnergyLevel(&pOndemand->id, &nEnergyLevel, NULL);
            if(nEnergyLevel <= 0)
            {
                return FALSE;
            }
#if 0			
            // 명시적 요청이기 때문에 같은 level 이라도 동작을 수행한다
            if(nEnergyLevel == ENERGY_LEVEL_OPTION_MAX) 
            {
                // Level이 Max 라면 이 Operation을 수행할 필요가 없다.
                return FALSE;
            }
#endif			
            if(pOndemand->nRelayCtrlStep==0) // 첫번째 단계인 Activate 단계일 때
            {
                param[idx] = 0x00;                  idx++;      // Level (0)
	            param[idx] = 0x10;                  idx++;      // ToSet (Clear Lockout : 16)
            }
            else
            {
                param[idx] = 0x64;                  idx++;      // Level (100)
	            param[idx] = 0x11;                  idx++;      // ToSet (Open and Lockout : 17)
            }
            pOndemand->nRelayCtrlStep ++;
            break;
        default:
            XDEBUG(" " ANSI_BACK_RED "Invalid Control Code %s %d" ANSI_NORMAL "\r\n", pOndemand->szId, pOndemand->nOffset);
            return FALSE;
    }

	param[idx] = 0;                             idx++;      // Hour
	param[idx] = 0;                             idx++;      // Min
	param[idx] = 0;                             idx++;      // Sec

    *nProcedureLength = CANSIParser::MakeProcedureTable((ANSI_ST_TABLE7 *)szProcedure, nCommand, nTableType, 
            ANSI_PROC_RES_NOT_POST, 
            pSession->nProcSeq ++ , param, idx);

    return TRUE;
}


BOOL CANSIParser::fnPreS131(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szTable, WORD *nTableLength)
{
    ANSI_ST_TABLE131 *pTable = (ANSI_ST_TABLE131 *)szTable;
    BYTE ctrl = 0x0;
    BYTE nEnergyLevel = 0;

    if(pSession == NULL) return TRUE;

    /** GE Relay control 시 nOption을 control 값으로 사용한다. */
    switch(pOndemand->nOption)
    {
        case WRITE_OPTION_ACTOFF:
	        ctrl = RELAY_ACTIVATION_OFF ;
            break;
        case WRITE_OPTION_ACTON:
	        ctrl = RELAY_ACTIVATION_ON ;
            break;
        case WRITE_OPTION_RELAYON:
	        ctrl = RELAY_SWITCH_ON ;
            break;
        case WRITE_OPTION_RELAYOFF:
	        ctrl = RELAY_SWITCH_OFF ;
            break;
        case ENERGY_LEVEL_OPTION_MIN: // Relay On
            m_pEndDeviceList->GetEnergyLevel(&pOndemand->id, &nEnergyLevel, NULL);
            if(nEnergyLevel <= 0)
            {
                pOndemand->nRelayCtrlStep ++;
                return FALSE;
            }
#if 0			
            if(nEnergyLevel == ENERGY_LEVEL_OPTION_MIN) 
            {
                // Level이 Min 이라면 이 Operation을 수행할 필요가 없다.
                pOndemand->nRelayCtrlStep ++;
                return FALSE;
            }
#endif
            if(pOndemand->nRelayCtrlStep==0) // 첫번째 단계인 Activate 단계일 때
            {
                if(pOndemand->nActivate) // 이미 Activate 되어 있다면 이 단계를 수행할 필요가 없다
                {
                    pOndemand->nRelayCtrlStep ++;
                    return FALSE;
                }
	            ctrl = RELAY_ACTIVATION_ON; // Activate
            }
            else
            {
	            ctrl = RELAY_SWITCH_ON;     // Relay On
            }
            pOndemand->nRelayCtrlStep ++;
            break;
        case ENERGY_LEVEL_OPTION_MAX: // Relay Off
            m_pEndDeviceList->GetEnergyLevel(&pOndemand->id, &nEnergyLevel, NULL);
            if(nEnergyLevel <= 0)
            {
                pOndemand->nRelayCtrlStep ++;
                return FALSE;
            }
#if 0
            if(nEnergyLevel == ENERGY_LEVEL_OPTION_MAX) 
            {
                // Level이 Max 라면 이 Operation을 수행할 필요가 없다.
                pOndemand->nRelayCtrlStep ++;
                return FALSE;
            }
#endif

            if(pOndemand->nRelayCtrlStep==0) // 첫번째 단계인 Activate 단계일 때
            {
                if(pOndemand->nActivate) // 이미 Activate 되어 있다면 이 단계를 수행할 필요가 없다
                {
                    pOndemand->nRelayCtrlStep ++;
                    return FALSE;
                }
	            ctrl = RELAY_ACTIVATION_ON; // Activate
            }
            else
            {
	            ctrl = RELAY_SWITCH_OFF;     // Relay On
            }
            pOndemand->nRelayCtrlStep ++;
            break;
        default:
            XDEBUG(" " ANSI_BACK_RED "Invalid Control Code %s %d" ANSI_NORMAL "\r\n", pOndemand->szId, pOndemand->nOffset);
            return FALSE;
    }

    memset(pTable, 0, sizeof(ANSI_ST_TABLE131));
    pTable->RELAY_OPERATE_CODE = ctrl;

    *nTableLength = sizeof(ANSI_ST_TABLE131);

    XDEBUG(" GE RELAY CONTROL %s %d\r\n", pOndemand->szId, ctrl);

    return TRUE;
}

BOOL CANSIParser::fnPreM084(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szTable, WORD *nTableLength)
{
    ANSI_MT_TABLE84 *pTable = (ANSI_MT_TABLE84 *)szTable;

    memset(pTable, 0, sizeof(ANSI_MT_TABLE84));
    memcpy(pTable->MASTER_PASSWORD, ge_master_security, sizeof(pTable->MASTER_PASSWORD));
    memcpy(pTable->READER_PASSWORD, ge_reader_security, sizeof(pTable->MASTER_PASSWORD));
    memcpy(pTable->CUSTOMER_PASSWORD, ge_customer_security, sizeof(pTable->MASTER_PASSWORD));

    *nTableLength = sizeof(ANSI_MT_TABLE84);

    XDEBUG(" METER SECURITY RESET\r\n");

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/** Table 12 Unit of Measure Entry Table을 읽어야 할 길이를 계산한다.
  */
BOOL CANSIParser::fnPreReadS012(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes)
{
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return TRUE;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    *nIndex = 0;
    *nRequestBytes = sizeof(ANSI_ST_TABLE12) * pMeterInfo->nUomEntries;

    return TRUE;
}

/** Table 26 Self Read Data Table을 읽어야 할 길이를 계산한다.
  */
BOOL CANSIParser::fnPreReadS026(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes)
{
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return TRUE;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    *nIndex = 0;
    if(pMeterInfo->nMeterMode == GE_METER_MODE_TOU)
    {
        *nRequestBytes = 6 + ((5+1+726) * pMeterInfo->nSelfReadCount);
    }
    else
    {
        *nRequestBytes = 6 + (241 * pMeterInfo->nSelfReadCount);
    }

    return TRUE;
}

/** 일반적인 검침에서 Timesync가 수행 된 후 Standard 55 Table을 읽을 지 검사한다.
  */
BOOL CANSIParser::fnPreReadS055(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes)
{
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return TRUE;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    *nIndex = 0;
    *nRequestBytes = sizeof(ANSI_ST_TABLE55);

    return pMeterInfo->bTimesyncActivate;
}

/**
 * @return TRUE일 경우 Read Table 수행 , FALSE일 경우 Read Table 읽기 절차 Skip
 * 
 */
BOOL CANSIParser::fnPreReadS064(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes)
{
    int nCount = 1;
    int nBlockIndex = 0;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return TRUE;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    /**
     * 읽어야 하는 Index와 ReadBytes를 구한다.
     *  nLastBlockElement 값을 사용한다.
     */
    if(pOndemand->nOffset > 0)
    {
        nBlockIndex = (int)(pOndemand->nOffset / pMeterInfo->nBlockIntsCount);
    }
    if(pMeterInfo->nLastBlockElement < nBlockIndex)
    {
        nBlockIndex = pMeterInfo->nLastBlockElement;
    }

    if(nBlockIndex > 0)     // 현재 block 이전 일 때
    {
        nCount = (int)(pOndemand->nCount / pMeterInfo->nBlockIntsCount) + 1;    // 몇개의 Block을 읽을 지
    }

    /** 마지막 블럭이고 valid instants가 count 보다 작을 때는 이전 블럭을 함께 읽는다.*/
    if(pMeterInfo->nLastBlockElement != 0 && nBlockIndex == 0 && pOndemand->nCount > pMeterInfo->nValidInstans)
    {
        nBlockIndex = 1;
        nCount = 2;
    }

    if(nBlockIndex + 1 < nCount) // valid block 보다 많이 요청 될 때
    {
        nCount = nBlockIndex + 1;
    }
    /** TODO : pMeterInfo->nBlockCount에 의해서 LP Block이 rollover 되는 것을 감지해서 읽는 기능을
      * 구현해야 한다
      */

    *nIndex = (pMeterInfo->nLastBlockElement - nBlockIndex) * pMeterInfo->nLpLength;
    *nRequestBytes = pMeterInfo->nLpLength * nCount;

    XDEBUG(" fnPreReadS064: nIndex %d, nRequestBytes %d\r\n", *nIndex, *nRequestBytes);

    return TRUE;
}

/** Table 76 Event Log Table을 읽어야 하는지 검사한다
  */
BOOL CANSIParser::fnPreReadS076(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes)
{
    return pOndemand->bEventLog;
}


////////////////////////////////////////////////////////////////////////////////////////////////
/** General Configuration Table
 *
 *  Ordering 결정
 */
void* CANSIParser::fnPostS000(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE0 * pTable;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE0 *)szTableData;

    pMeterInfo->nOrder = pTable->DATA_ORDER ? ORDER_MSB : ORDER_LSB;

    XDEBUG(" TABLE ST-00: General Configuration Table\r\n");
    XDEBUG("    ORDERING       = '%s'\r\n", pTable->DATA_ORDER ? "MSB" : "LSB");

    return NULL;
}

/** Manufacturer Identification Table
 *
 * Model, Hw, Fw 정보 update
 */
void* CANSIParser::fnPostS001(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE1 * pTable;
    ANSI_METER_INFO * pMeterInfo;
	char            manuf[4+1];
	char            model[8+1];
	char            serial[16+1];
    char            *pSerial;
    WORD            hw, fw;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE1 *)szTableData;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    memset(manuf, 0, sizeof(manuf));
    memset(model, 0, sizeof(model));
    memset(serial, 0, sizeof(serial));

    memcpy(manuf, pTable->MANUFACTURER, sizeof(pTable->MANUFACTURER)); 
    memcpy(model, pTable->ED_MODEL, sizeof(pTable->ED_MODEL));
    memcpy(serial, pTable->MFG_SERIAL_NUMBER, sizeof(pTable->MFG_SERIAL_NUMBER));
		   
    hw = (pTable->HW_VERSION_NUMBER << 8) | pTable->HW_REVISION_NUMBER;
    fw = (pTable->FW_VERSION_NUMBER << 8) | pTable->FW_REVISION_NUMBER;

    pSerial = strip(serial);
		   
    XDEBUG(" TABLE ST-01: General Manufacturer Identification Table\r\n");
    XDEBUG("    MANUFACTURER       = '%s'\r\n", manuf);
    XDEBUG("    ED_MODEL           = '%s'\r\n", model);
    XDEBUG("    HW_VERSION_NUMBER  = %d\r\n", pTable->HW_VERSION_NUMBER);
    XDEBUG("    HW_REVISION_NUMBER = %d\r\n", pTable->HW_REVISION_NUMBER);
    XDEBUG("    FW_VERSION_NUMBER  = %d\r\n", pTable->FW_VERSION_NUMBER);
    XDEBUG("    FW_REVISION_NUMBER = %d\r\n", pTable->FW_REVISION_NUMBER);
    XDEBUG("    MFG_SERIAL_NUMBER  = '%s'\r\n", serial);
    XDEBUG("    MFG_SERIAL_NUMBER  = '%s'\r\n", pSerial ? pSerial : "NULL");

    /** TODO Manufacturer Code를 이용해서 Meter Vendor를 구분하도록 수정해야 한다 */
    m_pEndDeviceList->UpdateMeterInventory(&pOndemand->id, UNDEFINED_PORT_NUMBER, 
            PARSER_TYPE_ANSI, manuf, METER_VENDOR_GE, model, pSerial, hw, fw);

    if(pSerial != NULL)
    {
        pMeterInfo->bUseMfgId = TRUE;
        FREE(pSerial);
    }
    else
    {
        pMeterInfo->bUseMfgId = FALSE;
    }

    return NULL;
}

/** End Device Mode & Status Table.
 *
 * Meter Error가 있는지 검사하고 Clock Error가 있을 경우 Event를 발생 시킨다
 */
void* CANSIParser::fnPostS003(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE3 * pTable;
    BOOL bMeterClockError = FALSE;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE3 *)szTableData;

    /*-- Issue #1529 : 현재 Meter 상태를 읽어서 신규 장애가 발생했는지 검사해 본다  --*/
    m_pEndDeviceList->GetMeterError(&pOndemand->id, ERR_TYPE_CLOCK_ERROR, &bMeterClockError);
    /*-- Issue #1529 : Meter의 현재 Clock error 상태를 저장 --*/
    m_pEndDeviceList->SetMeterError(&pOndemand->id, ERR_TYPE_CLOCK_ERROR, pTable->CLOCK_ERROR_FLAG);

    if(bMeterClockError != pTable->CLOCK_ERROR_FLAG) {
        ENDIENTRY * endi =  m_pEndDeviceList->GetEndDeviceByID(&pOndemand->id);
        char dummy[0];
        // Event 발생
        meterErrorEvent(&pOndemand->id, endi ? endi->szSerial : dummy, 
            ERR_TYPE_CLOCK_ERROR, pTable->CLOCK_ERROR_FLAG ? ERR_OPEN : ERR_CLOSE, NULL, 0);
    }
    /** Unprogramed 발견.
     *
     *
     */
    if(pTable->UNPROGRAMMED_FLAG) 
    {
        return &m_AnsiMeterTbl_ResolveUnprogram;
    }

    /** Issue 1721 : Recovery selection 추가
     *
     *  Clock Error가 발생하였을 경우에는 반드시 Clear 해야 한다.
     *
     */
    if(/*m_bEnableMeterRecovery && */(pTable->CLOCK_ERROR_FLAG & 0xFF))
    {
        /** Clock Error Clear */
        return &m_AnsiMeterTblOption_13;
    }
    return NULL;
}

/** Device Identification Table.
 *
 * Meter Serial을 Update 한다
 */
void* CANSIParser::fnPostS005(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE5 * pTable;
    ANSI_METER_INFO * pMeterInfo;
    char        meterId[20+1];

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE5 *)szTableData;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    memset(meterId, 0, sizeof(meterId));
    memcpy(meterId, pTable->METER_ID, sizeof(pTable->METER_ID));
    XDEBUG("    METER_ID = %s\r\n", meterId);

    /** 기존에 설치된 지역에 MFG_SERIAL을 쓰고 해당 value를 FEP에서 Key로 사용하기 때문에 정상적인 Meter Serial을
      * 사용할 경우 과거 데이터를 전부 지워야 하는 문제가 발생한다. 따라서 기존과 같이 ST001의 MFG_SERIAL을 사용하도록
      * 변경한다.
      * Issue #303 : 일본지사에서는 Meter ID를 쓰기 때문에 MFG ID가 비어 있을 경우에 사용하도록 수정한다
      */
    if(pMeterInfo->bUseMfgId == FALSE)
    {
        m_pEndDeviceList->UpdateMeterInventory(&pOndemand->id, UNDEFINED_PORT_NUMBER,
                PARSER_TYPE_ANSI, NULL, METER_VENDOR_GE, NULL, meterId, 0, 0);
    }

    return NULL;
}

/** Actual Source Limiting Table
 *
 * Limit 값 설정
 */
void* CANSIParser::fnPostS011(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE11 * pTable;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE11 *)szTableData;

    pMeterInfo->nUomEntries = pTable->NBR_UOM_ENTRIES;

    return NULL;
}

/** Unit of Measure Entry Table
 *
 * Channel Configuration Entry 조회
 */
void* CANSIParser::fnPostS012(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);

    /** buffer size가 62 이기 때문에 이보다는 작아야 한다 */
    memcpy(pMeterInfo->uomEntryRcd, szTableData, 
            sizeof(ANSI_ST_TABLE12) * MIN(sizeof(pMeterInfo->uomEntryRcd), 62));

    return NULL;
}

/** Actual Register Table
 *
 * Self Read Table Count를 구한다.
 */
void* CANSIParser::fnPostS021(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE21 * pTable;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE21 *)szTableData;

    pMeterInfo->nSelfReadCount = pTable->NBR_SELF_READS;

    return NULL;
}


/** Current Register Data Table
 *
 * 현재 Summation 값
 */
void* CANSIParser::fnPostS023(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_METER_INFO * pMeterInfo;
    __uint64_t	summation_ch1 = 0, summation_ch2 = 0;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);

    if(pMeterInfo->nMeterMode == GE_METER_MODE_TOU)
    {
        ANSI_ST_TABLE23_TOU * pTable;
        pTable = (ANSI_ST_TABLE23_TOU *)szTableData;

        if(nTableLength < 12) return NULL;

        /** DOTO : Little Endian 일 때 문제가 없다.
        * Meter DATA_ORDER (ST000) 과 System의 Order을 고려할 때
        * 이 부분에 대한 Ordering 부분을 다시 검토해 보아야 한다.
        */
        memcpy(&summation_ch1, pTable->TOT_DATA_BLOCK.SUMMATIONS, 6);
        memcpy(&summation_ch2, &pTable->TOT_DATA_BLOCK.SUMMATIONS[6], 6);
    }
    else
    {
        ANSI_ST_TABLE23_DEMAND * pTable;
        pTable = (ANSI_ST_TABLE23_DEMAND *)szTableData;

        if(nTableLength < 12) return NULL;

        /** DOTO : Little Endian 일 때 문제가 없다.
        * Meter DATA_ORDER (ST000) 과 System의 Order을 고려할 때
        * 이 부분에 대한 Ordering 부분을 다시 검토해 보아야 한다.
        */
        memcpy(&summation_ch1, pTable->TIER_DATA_BLOCK.SUMMATIONS, 6);
        memcpy(&summation_ch2, &pTable->TIER_DATA_BLOCK.SUMMATIONS[6], 6);
    }

    m_pEndDeviceList->ClearValue(&pOndemand->id);
    m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("SUM_CH2"), VARSMI_STREAM, 6, (BYTE *) &summation_ch2);
    m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("SUM_CH1"), VARSMI_STREAM, 6, (BYTE *) &summation_ch1);

    return NULL;
}

/** Previous Demand Reset Data Table
 *
 * 이전 Demand Reset 값
 */
void* CANSIParser::fnPostS025(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);

    if(pMeterInfo->nMeterMode == GE_METER_MODE_TOU)
    {
        ANSI_ST_TABLE25_TOU * pTable;
        pTable = (ANSI_ST_TABLE25_TOU *)szTableData;

	    XDEBUG("  END_DATE_TIME = %04d/%02d/%02d %02d:%02d\r\n", 
								pTable->INFO.END_DATE_TIME.YEAR + 2000, pTable->INFO.END_DATE_TIME.MON,
								pTable->INFO.END_DATE_TIME.DAY, pTable->INFO.END_DATE_TIME.HOUR, pTable->INFO.END_DATE_TIME.MIN);
	    XDEBUG("  SEASON        = %d\r\n", pTable->INFO.SEASON);
        CANSIParser::fnPostS023(pSession, pOndemand, nTableType, 23, (char *)&pTable->DATA, nTableLength - 6);
    }
    else
    {
        ANSI_ST_TABLE25_DEMAND * pTable;
        pTable = (ANSI_ST_TABLE25_DEMAND *)szTableData;

	    XDEBUG("  END_DATE_TIME = %04d/%02d/%02d %02d:%02d\r\n", 
								pTable->INFO.END_DATE_TIME.YEAR + 2000, pTable->INFO.END_DATE_TIME.MON,
								pTable->INFO.END_DATE_TIME.DAY, pTable->INFO.END_DATE_TIME.HOUR, pTable->INFO.END_DATE_TIME.MIN);
	    XDEBUG("  SEASON        = %d\r\n", pTable->INFO.SEASON);
        CANSIParser::fnPostS023(pSession, pOndemand, nTableType, 23, (char *)&pTable->DATA, nTableLength - 6);
    }

    return NULL;
}

/** Before Timesync
 *
 * Timesync 이전의 Standard 55 의 값
 */
void* CANSIParser::fnPostS055BeforeTimesync(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE55 * pTable;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d] Current\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE55 *)szTableData;

    memcpy(&pMeterInfo->currentTime, pTable, sizeof(ANSI_ST_TABLE55));

	XDEBUG("  METER TIME = %04d/%02d/%02d %02d:%02d:%02d\r\n", 
            pTable->YEAR + 2000, pTable->MONTH, pTable->DAY, pTable->HOUR, pTable->MINUTE, pTable->SECOND);

    return NULL;
}

/** After Timesync
 *
 * Timesync 이후의 Standard 55 의 값
 */
void* CANSIParser::fnPostS055AfterTimesync(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
  	struct tm   tm_prev, tm_cur;
  	time_t      prev, cur, delta;

    ANSI_ST_TABLE55 * pTable;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d] After Timesync\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE55 *)szTableData;

    memset(&tm_prev, 0, sizeof(struct tm));
    tm_prev.tm_year = pMeterInfo->currentTime.YEAR + 100;
    tm_prev.tm_mon  = pMeterInfo->currentTime.MONTH - 1;
    tm_prev.tm_mday = pMeterInfo->currentTime.DAY;
    tm_prev.tm_hour = pMeterInfo->currentTime.HOUR;
    tm_prev.tm_min  = pMeterInfo->currentTime.MINUTE;
    tm_prev.tm_sec  = pMeterInfo->currentTime.SECOND;
    prev = mktime(&tm_prev);

    memset(&tm_cur, 0, sizeof(struct tm));
    tm_cur.tm_year = pTable->YEAR + 100;
    tm_cur.tm_mon  = pTable->MONTH - 1;
    tm_cur.tm_mday = pTable->DAY;
    tm_cur.tm_hour = pTable->HOUR;
    tm_cur.tm_min  = pTable->MINUTE;
    tm_cur.tm_sec  = pTable->SECOND;
    cur = mktime(&tm_cur);

    delta = cur > prev ? cur - prev : prev - cur;
    TIMESYNC_LOG("%s: %d day %d hour %d min %d sec.\r\n"
                     "         %02d/%02d/%02d %02d:%02d:%02d, %02d/%02d/%02d %02d:%02d:%02d, %s.\r\n",
            pOndemand->szId,
            (int)(delta / 86400), (int)(delta / 3600) % 24, (int)(delta / 60) % 60, (int)delta % 60,
            2000+pMeterInfo->currentTime.YEAR,
            pMeterInfo->currentTime.MONTH,
            pMeterInfo->currentTime.DAY,
            pMeterInfo->currentTime.HOUR,
            pMeterInfo->currentTime.MINUTE,
            pMeterInfo->currentTime.SECOND,
            2000+pTable->YEAR,
            pTable->MONTH,
            pTable->DAY,
            pTable->HOUR,
            pTable->MINUTE,
            pTable->SECOND,
            dayofweek[(pTable->TIME_DATE_QUAL & 0x07)]);

	XDEBUG("  METER TIME = %04d/%02d/%02d %02d:%02d:%02d\r\n", 
            pTable->YEAR + 2000, pTable->MONTH, pTable->DAY, pTable->HOUR, pTable->MINUTE, pTable->SECOND);

    return NULL;
}

/** Load Profile Informations (Standard Table #61)
 *
 */
void* CANSIParser::fnPostS061(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE61 * pTable;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE61 *)szTableData;
									 
    XDEBUG("    LP_MEMORY_LEN     = %d\r\n", pTable->LP_MEMORY_LEN);
    XDEBUG("    LP_FLAGS          = 0x%04X\r\n", pTable->LP_FLAGS);
    XDEBUG("    LP_FORMATS        = 0x%02X\r\n", pTable->LP_FORMATS);
    XDEBUG("    NBR_BLKS_SET1     = %d\r\n", pTable->NBR_BLKS_SET1);
    XDEBUG("    NBR_BLK_INTS_SET1 = %d\r\n", pTable->NBR_BLK_INTS_SET1);
    XDEBUG("    NBR_CHNS_SET1     = %d\r\n", pTable->NBR_CHNS_SET1);
    XDEBUG("    MAX_INT_TIME_SET1 = %d\r\n", pTable->MAX_INT_TIME_SET1);

    if(pTable->MAX_INT_TIME_SET1 <= 0)
    {
        XDEBUG(" " ANSI_BACK_RED "%s Invalid LP Interval %d" ANSI_NORMAL "\r\n", pOndemand->szId, pTable->MAX_INT_TIME_SET1);
        return &m_AnsiMeterTbl_Terminate;
    }
    if(pTable->NBR_BLK_INTS_SET1 <= 0)
    {
        XDEBUG(" " ANSI_BACK_RED "%s Invalid Instance Count %d" ANSI_NORMAL "\r\n", pOndemand->szId, pTable->NBR_BLK_INTS_SET1);
        return &m_AnsiMeterTbl_Terminate;
    }
  
    if(pMeterInfo != NULL)
    {
        pMeterInfo->nLpLength = ((((pTable->NBR_CHNS_SET1/2)+1)+(pTable->NBR_CHNS_SET1*2)) * pTable->NBR_BLK_INTS_SET1) 
                                  + 5 + (pTable->NBR_CHNS_SET1*6);
        pMeterInfo->nLpChannel = pTable->NBR_CHNS_SET1;
        pMeterInfo->nLpInterval = pTable->MAX_INT_TIME_SET1;
        pMeterInfo->nBlockIntsCount = pTable->NBR_BLK_INTS_SET1;
        pMeterInfo->nBlockCount = pTable->NBR_BLKS_SET1;

        XDEBUG("    LP LENGTH         = %d\r\n", pMeterInfo->nLpLength);
    }

    return NULL;
}

/** I210+c Relay Status (Standard Table #112)
 *
 */
void* CANSIParser::fnPostS112(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE112 * pTable;
    ANSI_METER_INFO * pMeterInfo;
    int i, nCount=0;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE112 *)szTableData;
									 
    nCount = nTableLength / sizeof(ANSI_LOAD_CTRL_STATUS); // 반복 횟수 결정
    for(i=0; i< nCount; i++)
    {
        /** RCDC로 시작하는 것이 두개 있다. 
          * 두번 째 RCDC MAN ACK 는 Meter에 장착된 Button을 이용할 때 값이 변경된다. 
          * 이 경우에 대해서는 현재 지원가능한 Meter가 없기 때문에 무시한다. 
          * 추후에 이 부분도 검사해야 하는 지 추가 검토가 필요하다.
          *
          * 현재 I210 Meter에서는 Manual Control을 지원하지 않는다.
          * 
          * Table 113:  Load Control Configuration Table
          *   MANUAL_OVERRIDE_ENABLE_FLAG, MANUAL_TURN_ON_ENABLE_FLAG : always 0
          * 
          */
        if(strncmp((char *)pTable->STATUS_ENTRIES[i].NAME, "RCDC", 4)==0 && 
           strncmp((char *)pTable->STATUS_ENTRIES[i].NAME, "RCDC MAN", 8)!=0) // RCDC 일 경우
        {
            if(pTable->STATUS_ENTRIES[i].SENSED_LEVEL==100) // 
            {
                pOndemand->nEnergyLevel = ENERGY_LEVEL_MAX; 
                XDEBUG("    RELAY_STATUS      = %s\r\n", "off");
            }
            else
            {
                pOndemand->nEnergyLevel = ENERGY_LEVEL_MIN; 
                XDEBUG("    RELAY_STATUS      = %s\r\n", "on");
            }
            /** Energy Level Update */
            m_pEndDeviceList->SetEnergyLevel(&pOndemand->id, pOndemand->nEnergyLevel, 
                    pTable->STATUS_ENTRIES[i].SENSED_LEVEL);
            XDEBUG(" ENERGY LEVEL : %d\r\n", pOndemand->nEnergyLevel);
        }
    }

    return NULL;
}

/** SM110 Relay Status (Standard Table #130)
 *
 */
void* CANSIParser::fnPostS130(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE130 * pTable;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE130 *)szTableData;
									 
    XDEBUG("    RUN_STATUS        = %s\r\n", pTable->RUN_STATUS ? "on" : "off");
    XDEBUG("    ACTIVE_STATUS     = %s\r\n", pTable->ACTIVE_STATUS ? "on" : "off");

    if(pTable->RUN_STATUS)
    {
        pOndemand->nEnergyLevel = ENERGY_LEVEL_MIN; 
    }
    else
    {
        pOndemand->nEnergyLevel = ENERGY_LEVEL_MAX; 
    }
    pOndemand->nActivate = pTable->ACTIVE_STATUS;

    /** Energy Level Update */
    m_pEndDeviceList->SetEnergyLevel(&pOndemand->id, pOndemand->nEnergyLevel, pTable->RUN_STATUS);
    XDEBUG(" ENERGY LEVEL : %d\r\n", pOndemand->nEnergyLevel);

    return NULL;
}

/** Load Profile Control (Standard Table #62)
 *
 * Issue #84 
 *  - Channel Configuration을 저장해야 한다.
 *
 */
void* CANSIParser::fnPostS062(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    LP_SEL_SET1 *pLpSelSet1;
    ANSI_METER_INFO * pMeterInfo;
    int i,j;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);

    //pTable = (ANSI_ST_TABLE61 *)szTableData;
									 
    j = MIN(pMeterInfo->nLpChannel, sizeof(pMeterInfo->uomEntryRcd));
    for(i=0;i<j;i++) {
        pLpSelSet1 = (LP_SEL_SET1 *)(szTableData + (sizeof(LP_SEL_SET1)*i));

        if(pLpSelSet1->LP_SOURCE_SELECT > sizeof(pMeterInfo->uomEntryRcd))
        {
            XDEBUG(" " ANSI_BACK_RED "%s Invalid LP SOURCE SELECT %d" ANSI_NORMAL "\r\n", 
                    pOndemand->szId, pLpSelSet1->LP_SOURCE_SELECT);
            return &m_AnsiMeterTbl_Terminate;
        }

        XDEBUG("    CH CFG %2d   = IDX (%02d) ID(%02d) TB(%d) MUL(%d) Qn(%c%c%c%c) NF(%c) SEG(%d) HMN(%c) NFS(%c)\r\n", 
                i, 
                pLpSelSet1->LP_SOURCE_SELECT,
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].ID_CODE,
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].TIME_BASE,
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].MULTIPLIER,
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].Q1_ACCOUNTABILITY ? 'T' : 'F',
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].Q2_ACCOUNTABILITY ? 'T' : 'F',
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].Q3_ACCOUNTABILITY ? 'T' : 'F',
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].Q4_ACCOUNTABILITY ? 'T' : 'F',
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].NET_FLOW_ACCOUNTABILITY ? 'T' : 'F',
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].SEGMENTATION,
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].HARMONIC ? 'T' : 'F',
                pMeterInfo->uomEntryRcd[pLpSelSet1->LP_SOURCE_SELECT].NFS ? 'T' : 'F'
            );
    }

    return NULL;
}

/** Load Profile Status (Standard Table #63)
 *
 */
void* CANSIParser::fnPostS063(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_ST_TABLE63 * pTable;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_ST_TABLE63 *)szTableData;
									 
    XDEBUG("    NBR_VALID_BLOCKS   = %d\r\n", pTable->NBR_VALID_BLOCKS);
    XDEBUG("    LAST_BLOCK_ELEMENT = %d\r\n", pTable->LAST_BLOCK_ELEMENT);
    XDEBUG("    LAST_BLOCK_SEQ_NBR = %d\r\n", pTable->LAST_BLOCK_SEQ_NBR);
    XDEBUG("    NBR_UNREAD_BLOCKS  = %d\r\n", pTable->NBR_UNREAD_BLOCKS);
    XDEBUG("    NBR_VALID_INT      = %d\r\n", pTable->NBR_VALID_INT);

    pMeterInfo->nValidBlocks = pTable->NBR_VALID_BLOCKS;
    pMeterInfo->nLastBlockElement = pTable->LAST_BLOCK_ELEMENT;
    pMeterInfo->nLastBlockSeq = pTable->LAST_BLOCK_SEQ_NBR;
    pMeterInfo->nUnreadBlocks = pTable->NBR_UNREAD_BLOCKS;
    pMeterInfo->nValidInstans = pTable->NBR_VALID_INT;

    return NULL;
}

/** GE Device Info (Manufacturer Table #0)
 *
 */
void* CANSIParser::fnPostM000(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength)
{
    ANSI_MT_TABLE0 * pTable;
    ANSI_METER_INFO * pMeterInfo;

    if(pSession == NULL) return NULL;
    pMeterInfo = (ANSI_METER_INFO *)pSession->pUserData;

    XDEBUG(" Table Handler [%c%03d]\r\n", nTableType, nTableNumber);
    pTable = (ANSI_MT_TABLE0 *)szTableData;
									 
    XDEBUG("     MFG_VERSION_NBR   = %d\r\n", pTable->MFG_VERSION_NBR);
    XDEBUG("    MFG_REVISION_NBR   = %d\r\n", pTable->MFG_REVISION_NBR);
    XDEBUG("      GE_PART_NUMBER   = %02X %02X %02X %02X %02X\r\n", 
            pTable->ROM_CONST_DATA_RCD.GE_PART_NUMBER[0],
            pTable->ROM_CONST_DATA_RCD.GE_PART_NUMBER[1],
            pTable->ROM_CONST_DATA_RCD.GE_PART_NUMBER[2],
            pTable->ROM_CONST_DATA_RCD.GE_PART_NUMBER[3],
            pTable->ROM_CONST_DATA_RCD.GE_PART_NUMBER[4]);
    XDEBUG("          FW_VERSION   = %02X\r\n", pTable->ROM_CONST_DATA_RCD.FW_VERSION);
    XDEBUG("         FW_REVISION   = %02X\r\n", pTable->ROM_CONST_DATA_RCD.FW_REVISION);
    XDEBUG("            FW_BUILD   = %d\r\n", pTable->ROM_CONST_DATA_RCD.FW_BUILD);
    XDEBUG("     MFG_TEST_VECTOR   = %02X %02X %02X %02x\r\n", 
            pTable->ROM_CONST_DATA_RCD.MFG_TEST_VECTOR[0],
            pTable->ROM_CONST_DATA_RCD.MFG_TEST_VECTOR[1],
            pTable->ROM_CONST_DATA_RCD.MFG_TEST_VECTOR[2],
            pTable->ROM_CONST_DATA_RCD.MFG_TEST_VECTOR[3]);
    XDEBUG("          METER_TYPE   = %d\r\n", pTable->METER_TYPE);
    XDEBUG("          METER_MODE   = %d\r\n", pTable->METER_MODE);

    pMeterInfo->nMeterMode = pTable->METER_MODE;
    pMeterInfo->nMeterType = pTable->METER_TYPE;

    switch(pMeterInfo->nMeterType)
    {
        case GE_METER_TYPE_I210:
            pMeterInfo->nMeterTypeMask = GE_METER_TYPE_MASK_I210;
            break;
        case GE_METER_TYPE_SM110:
            pMeterInfo->nMeterTypeMask = GE_METER_TYPE_MASK_SM110;
            break;
        case GE_METER_TYPE_AZOS:
            pMeterInfo->nMeterTypeMask = GE_METER_TYPE_MASK_AZOS;
            break;
        default:
            pMeterInfo->nMeterTypeMask = GE_METER_TYPE_MASK_UNKNOWN;
            XDEBUG(" " ANSI_COLOR_RED "UKNOWN METER TYPE : %d" ANSI_NORMAL "\r\n", pTable->METER_TYPE);
            break;
    }

    /** Meter Type에 따라 다른 Table을 읽어야 하는 것들을 조정 */
    switch(pOndemand->nOption)
    {
        case WRITE_OPTION_CLEAR:        pMeterInfo->pTableList = m_AnsiMeterTblOption_13; break;
        case CONV_METER_MODE_TOU:       pMeterInfo->pTableList = m_AnsiMeterTblOption_22; break;
        case WRITE_OPTION_TIMESYNC:     pMeterInfo->pTableList = m_AnsiMeterTblOption_8; break;
        case READ_OPTION_EVENTLOG:      pMeterInfo->pTableList = m_AnsiMeterTblOption_10; break;
        case READ_IDENT_ONLY:           pMeterInfo->pTableList = m_AnsiMeterTblOption_11; break;
        case READ_OPTION_RELAY:         pMeterInfo->pTableList = m_AnsiMeterTblOption_3; break;
        case WRITE_OPTION_RELAYON:      
        case WRITE_OPTION_RELAYOFF:      
        case WRITE_OPTION_ACTON:      
        case WRITE_OPTION_ACTOFF:       pMeterInfo->pTableList = m_AnsiMeterTblOption_4; break;
        case ENERGY_LEVEL_OPTION_MAX:
        case ENERGY_LEVEL_OPTION_MIN:   pMeterInfo->pTableList = m_AnsiMeterTblOption_23; break;
        default:                        pMeterInfo->pTableList = m_AnsiMeterTblOption_0; break;
    }

    return NULL;
}

//////////////////////////////////////////////////////////////////////
// MeterParser Overriding methods
//////////////////////////////////////////////////////////////////////

