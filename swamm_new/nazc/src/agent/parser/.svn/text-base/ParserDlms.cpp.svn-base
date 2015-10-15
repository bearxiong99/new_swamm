/////////////////////////////////////////////////////////////////////
//
//  dlms.cpp: implementation of the CDLMSParser class.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "Chunk.h"
#include "EndDeviceList.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "dlmsFunctions.h"
#include "dlmsUtils.h"
#include "dlmsFrame.h"
#include "dlmsObisDefine.h"

#include "dlmsParserTypes.h"
#include "dlmsParserUtil.h"
#include "ParserDlms.h"
#include "Utility.h"

//////////////////////////////////////////////////////////////////////
// CDLMSParser Class
//////////////////////////////////////////////////////////////////////

extern BYTE     m_nDefaultMeteringOption;

#define DEFAULT_LP_INTERVAL     15
#define MAX_LP_COUNT            1800
#define MAX_DLMS_TOKEN          100

#define MAGIC_HEADER_TAIL       19
#define MAGIC_METERING_HEADER   sizeof(DLMSRESULT)-MAX_DLMS_RESULT_SIZE
#define MAGIC_BLOCK_HEADER1     6
#define MAGIC_BLOCK_HEADER2     2

#if defined(KAMSTRUP_DLMS_READING)
/** TODO DLMS Address를 설정할 수 있게 해야 한다
  */
#ifdef DLMS_SOURCE_ADDR
#undef DLMS_SOURCE_ADDR
#define DLMS_SOURCE_ADDR        0x25
#endif
#ifdef DLMS_DESTINATION_ADDR
#undef DLMS_DESTINATION_ADDR
#define DLMS_DESTINATION_ADDR   0x21
#endif
#endif      // KAMSTRUP_DLMS_READING

#if     defined(__PATCH_4_1647__)
extern BOOL    *m_pbPatch_4_1647;
#endif
#if     defined(__PATCH_10_2808__)
extern BOOL    *m_pbPatch_10_2808;
#endif

OBISCODELIST obisTable_current[] =
{
    // Data Type                      COSEM Class                      A    B    C    D    E    F      Attr  N   Description
    { OBIS_COSEM_DEVICE_NAME,       { DLMS_CLASS_DATA,              OBIS_CODE_DEVICE_INFO,              2 }, SKIP, "COMSE logical device name" },
    { OBIS_MANUFACTURER,            { DLMS_CLASS_DATA,              OBIS_CODE_MANUFACTURER_METER_ID,    2 }, SKIP, "Meter Manufacturer ID" },
    { OBIS_CUSTOMER,                { DLMS_CLASS_DATA,              OBIS_CODE_CUSTOMER_METER_ID,        2 }, SKIP, "Meter Customer ID" },
    { OBIS_METER_TYPE,              { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_KEPCO_METER_INFO,         2 }, SAVE, "Meter Information (AMR)" },
    { OBIS_VZ,                      { DLMS_CLASS_DATA,              OBIS_CODE_VZ,                       2 }, SKIP, "Billing Period Counter" },
    { OBIS_BILLING_PERIOD,          { DLMS_CLASS_DATA,              OBIS_CODE_BILLING_PERIOD,           2 }, SKIP, "Billing Period" },
    { OBIS_ACT_SCALER_UNIT,         { DLMS_CLASS_REGISTER,          OBIS_CODE_METER_CONSTANT_ACTIVE,    3 }, SKIP, "Meter constant active energy (scaler_unit)" },
    { OBIS_ACT_CONSTANT,            { DLMS_CLASS_REGISTER,          OBIS_CODE_METER_CONSTANT_ACTIVE,    2 }, SAVE, "Meter constant active energy" },
    { OBIS_RACT_SCALER_UNIT,        { DLMS_CLASS_REGISTER,          OBIS_CODE_METER_CONSTANT_REACTIVE,  3 }, SKIP, "Meter constant reactive energy (scaler_unit)" },
    { OBIS_RACT_CONSTANT,           { DLMS_CLASS_REGISTER,          OBIS_CODE_METER_CONSTANT_REACTIVE,  2 }, SAVE, "Meter constant reactive energy" },
  //{ OBIS_CUR_IN_USE,              { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_KEPCO_CURRENT_MAX_DEMAND, 7 }, SKIP, "Current Energy/Max. Demand (Entries in Use)" },
    { OBIS_CUR_BILLING,             { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_KEPCO_CURRENT_MAX_DEMAND, 2 }, SAVE, "Current Energy/Max. Demand" },
  //{ OBIS_PREV_IN_USE,             { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_KEPCO_PREVIOUS_MAX_DEMAND,7 }, SKIP, "Previous Energy/Max. Demand (Entries in Use)" },
    { OBIS_PREV_BILLING,            { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_KEPCO_PREVIOUS_MAX_DEMAND,2 }, SAVE, "Previous Energy/Max. Demand" },
    { OBIS_ENERGY_ENTRY,            { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_MONTHLY_ENERY_PROFILE,    7 }, SKIP, "Monthly energy profile (Entries in Use)" },
    { OBIS_ENERGY,                  { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_MONTHLY_ENERY_PROFILE,    2 }, SAVE, "Monthly energy profile" },
    { OBIS_MAX_DEMAND_ENTRY,        { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_MONTHLY_DEMAND_PROFILE,   7 }, SKIP, "Monthly Max. demand (Entries in Use)" },
    { OBIS_MAX_DEMAND,              { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_MONTHLY_DEMAND_PROFILE,   2 }, SAVE, "Monthly Max. demand" },
    { OBIS_POWER_FAIL,              { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_POWER_FAILURE,            2 }, SAVE, "Meter Power Failure" },
    { OBIS_POWER_RESTORE,           { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_POWER_RESTORE,            2 }, SAVE, "Meter Power Restore" },
  //{ OBIS_LCD_INFO,                { DLMS_CLASS_DATA,              OBIS_CODE_LCD_INFO,                 2 }, SKIP, "LCD INFO" },
    { OBIS_LP_INFO,                 { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_LOAD_PROFILE,             7 }, SKIP, "Meter LP Information" },
    { OBIS_LP,                      { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_LOAD_PROFILE,             2 }, SAVE, "Meter Selected LP" },
    { 0,                        { 0,                                { 0x00,0x00,0x00,0x00,0x00,0x00 }, 0 }, 0, NULL }
};

#if 0
OBISCODELIST obisTable_test[] =
{
    // Data Type                      COSEM Class                      A    B    C    D    E    F      Attr  N   Description
    { OBIS_DEFAULT,                 { DLMS_CLASS_DATA,              { 0x01,0x01,0x60,0x01,0x01,0xFF },  2 }, SAVE, "Meter Type" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_DATA,              { 0x00,0x00,0x60,0x02,0x80,0xFF },  2 }, SAVE, "Production Date" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_DATA,              { 0x01,0x01,0x00,0x02,0x01,0x80 },  2 }, SAVE, "Software Revision" },
    { OBIS_CUR_ACTIVE_FORWARD,      { DLMS_CLASS_REGISTER,          OBIS_CODE_CUR_ACTIVE_FORWARD,       2 }, SAVE, "Active Energy" },
    { OBIS_CUR_ACTIVE_FORWARD,      { DLMS_CLASS_REGISTER,          OBIS_CODE_CUR_ACTIVE_FORWARD,       3 }, SAVE, "Active Energy (scaler_unit)" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x00,0x15,0x00,0x05,0xFF },  2 }, SAVE, "Billing logger" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x00,0x15,0x00,0x06,0xFF },  2 }, SAVE, "Status logger" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x00,0x15,0x00,0x07,0xFF },  2 }, SAVE, "RTC logger" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x00,0x15,0x00,0x08,0xFF },  2 }, SAVE, "Voltage quality logger" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x01,0x63,0x01,0x01,0xFF },  1 }, SAVE, "Profile logger 1" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x01,0x63,0x01,0x01,0xFF },  2 }, SAVE, "Profile logger" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x01,0x63,0x01,0x01,0xFF },  3 }, SAVE, "Profile logger 3" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x01,0x63,0x01,0x01,0xFF },  4 }, SAVE, "Profile logger 4" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x01,0x63,0x01,0x01,0xFF },  5 }, SAVE, "Profile logger 5" },
    { OBIS_DEFAULT,                 { DLMS_CLASS_PROFILE_GENERIC,   { 0x00,0x01,0x63,0x01,0x01,0xFF },  6 }, SAVE, "Profile logger 6" },
    { 0,                        { 0,                                { 0x00,0x00,0x00,0x00,0x00,0x00 }, 0 }, 0, NULL }
};
#endif

OBISCODELIST obisTable_lp[] =
{
    { OBIS_MANUFACTURER,            { DLMS_CLASS_DATA,              OBIS_CODE_MANUFACTURER_METER_ID,    2 }, SKIP, "Meter Manufacturer ID" },
    { OBIS_CUSTOMER,                { DLMS_CLASS_DATA,              OBIS_CODE_CUSTOMER_METER_ID,        2 }, SKIP, "Meter Customer ID" },
    { OBIS_METER_TYPE,              { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_KEPCO_METER_INFO,         2 }, SAVE, "Meter Information (AMR)" },
    { OBIS_LP_INFO,                 { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_LOAD_PROFILE,             7 }, SKIP, "Meter LP Information" },
    { OBIS_LP,                      { DLMS_CLASS_PROFILE_GENERIC,   OBIS_CODE_LOAD_PROFILE,             2 }, SAVE, "Meter Selected LP" },
    { 0,                        { 0,                                { 0x00,0x00,0x00,0x00,0x00,0x00 }, 0 }, 0, NULL }
};

OBISCODELIST obisTable_load_control[] =
{
    // Data Type                      COSEM Class                      A    B    C    D    E    F      Attr  N   Description
    { OBIS_OUTPUT_SIGNAL,           { DLMS_CLASS_DATA,              OBIS_CODE_OUTPUT_SIGNAL,            2 }, SAVE, "State of the output control signals" },
    { OBIS_LOAD_CONTROL,            { DLMS_CLASS_DATA,              OBIS_CODE_LOAD_CONTROL,             2 }, SAVE, "Remote load control" },
    { 0,                        { 0,                                { 0x00,0x00,0x00,0x00,0x00,0x00 }, 0 }, 0, NULL }
};

OBISCODELIST obisTable_dr[] =
{
    { 200,                         { DLMS_CLASS_DEMAND_REGISTER,   { 0x01,0x01,0x01,0x04,0x00,0xFF },  1 }, SAVE, "Logical name" },
    { 201,                         { DLMS_CLASS_DEMAND_REGISTER,   { 0x01,0x01,0x01,0x04,0x00,0xFF },  2 }, SAVE, "Current Average" },
    { 202,                         { DLMS_CLASS_DEMAND_REGISTER,   { 0x01,0x01,0x01,0x04,0x00,0xFF },  3 }, SAVE, "Last Average" },
    { 203,                         { DLMS_CLASS_DEMAND_REGISTER,   { 0x01,0x01,0x01,0x04,0x00,0xFF },  4 }, SAVE, "Scaler Unit" },
    { 204,                         { DLMS_CLASS_DEMAND_REGISTER,   { 0x01,0x01,0x01,0x04,0x00,0xFF },  5 }, SAVE, "Status" },
    { 205,                         { DLMS_CLASS_DEMAND_REGISTER,   { 0x01,0x01,0x01,0x04,0x00,0xFF },  6 }, SAVE, "Capture Time" },
    { 206,                         { DLMS_CLASS_DEMAND_REGISTER,   { 0x01,0x01,0x01,0x04,0x00,0xFF },  7 }, SAVE, "Start Time Current" },
    { 207,                         { DLMS_CLASS_DEMAND_REGISTER,   { 0x01,0x01,0x01,0x04,0x00,0xFF },  8 }, SAVE, "Period" },
    { 208,                         { DLMS_CLASS_DEMAND_REGISTER,   { 0x01,0x01,0x01,0x04,0x00,0xFF },  9 }, SAVE, "Number of Periods" },
    { 0,                        { 0,                                { 0x00,0x00,0x00,0x00,0x00,0x00 }, 0 }, 0, NULL }
};

BYTE m_pSnrmNego []  =  { 
    0x81, 0x80, 0x12, 0x05, 0x01, 0x80, 0x06, 0x01, 0x80, 0x07, 
    0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 
    0x07 };

WORD m_nSnrmNegoLength = 21;

//static const char *parserNameArray[] = {"DLMS", "NAMR-P111SR", NULL};
static const char *parserNameArray[] = {"DLMS", "NAMR-P111SR", NULL};

//////////////////////////////////////////////////////////////////////
// CDLMSParser Class
//////////////////////////////////////////////////////////////////////

CDLMSParser::CDLMSParser() : CMeterParser(SENSOR_TYPE_ZRU, SERVICE_TYPE_ELEC, 
        ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL,
        const_cast<char **>(parserNameArray), PARSER_TYPE_DLMS, "DLMS Parser", "NURI", parserNameArray[0], 
        6000, 10000, 2)
{
}

CDLMSParser::~CDLMSParser()
{
}

//////////////////////////////////////////////////////////////////////
// CI210Parser Operations
//////////////////////////////////////////////////////////////////////

int CDLMSParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
//    BYTE    nOperCode;

    switch(pCommand->nType) {
        /*
      case PARSE_TYPE_READ :
      case PARSE_TYPE_TIMESYNC :
      */
        default:
           return DLMS_MAIN(pCommand, pStat);

    }
    return 0;
}

int CDLMSParser::DLMS_MAIN(CMDPARAM *pCommand, METER_STAT *pStat)
{
    char                szGUID[20];
    EUI64               id;
    int                 nOption;
    ONDEMANDPARAM       *pOndemand = NULL;
    int                 nRetry, nState;
    int                 nSaveState;
    int                 nResult, nError;
    int                 nOffset, nCount;
    int                 nSeek=0;
    time_t              tmStart, tmEnd, tmLast=0, tmLastEvent=0;
    CChunk              *pChunk;
    TIMESTAMP           tmLastMetering;
    TIMESTAMP           tmLastEventLog;
    BOOL                bConnected = FALSE;
    TIMETICK            startTime, endTime;
    
    ENDI_NODE_INFO      *nodeInfo;
    AMR_PARTIAL_INFO    *amrInfo;
    char                szBuffer[256];
    BYTE                permitMode=0, permitState=0, alarmFlag=0, testFlag=0;
    BYTE                hwVersion=0, fwVersion=0, fwBuild=0;
    WORD                alarmMask=0;
    WORD                addr;
    int                 len,n,i;
    unsigned long       nElapse=0, nTotalElapse=0;

    DLMSVARIABLE        *pToken;
    OBISCODELIST        *obisTable;
    OBISCODELIST        TempObisCode;
    DLMS_FRAME          *dlmsFrame;
    DLMSRESULT          result;
    BYTE                ctrl = 0;
    int                 dlmsLength;
    int                 nToken=0, nBlockCount=0, nTotalLpCount=0, nLpInterval=DEFAULT_LP_INTERVAL;
    char                szObis[32], szMeterId[20+1], szModel[18+1];
    int                 nMeterIdLen = 0;
    double              acon=0, rcon=0;
    BYTE                nVZ=0;
    int                 nRecord, nTempVZ=0;
    int                 nReadCount = 0, nStartCount = 0, nMaxBlockCount = 0;
    int                 nFailCount = 0;
    int                 nCurrentDayLpCount = 1;
    TIMESTAMP           *pCurrentTime = NULL, *pLastLpTime = NULL;
    int                 nPulsePerKwh = -1;
    UINT                nConformance = 0x00;
    BYTE                security[64];
    int                 nSecurityLen=0;

    BYTE                tagBuf[256];
    DLMSTAGVALUE        *pTagValue;

    memcpy(&id, &pCommand->id, sizeof(EUI64));      // Sensor ID
    pChunk          = pCommand->pChunk;                    // 검침 데이터를 리턴할 Chunk
    nOption         = pCommand->nOption;                // Function
    nOffset         = pCommand->nOffset;                // Read interval of meter data
    nCount          = pCommand->nCount;                    // LP Count

    if(nOption < 0) 
    {
        nOption = m_nDefaultMeteringOption;
    }
    if(nOffset < 0) nOffset = 1;
    if(nCount < 1) nCount = 2;
    if(nCount > nOffset + 1 ) nCount = nOffset + 1;

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

    // 마지막 검침 시간을 구한다.
    m_pEndDeviceList->GetLastMeteringTime(&id, &tmLast);
    GetTimestamp(&tmLastMetering, &tmLast);    

    // 마지막 이벤트 로그 시간을 구한다.
    m_pEndDeviceList->GetLastEventLogTime(&id, &tmLastEvent);
    GetTimestamp(&tmLastEventLog, &tmLastEvent);    

	// DLMS로 수신된 데이터를 Parsing할 Token의 버퍼를 초기화 한다.
    pToken = dlmsAllocToken(MAX_DLMS_TOKEN);
    if (pToken == NULL)
    {
        XDEBUG("Memory allocation fail.\r\n");		
	    m_pEndDeviceList->SetBusy(&id, FALSE);
        return 0;
    }

    memset(szMeterId, 0, sizeof(szMeterId));
    memset(szModel, 0, sizeof(szModel));

    switch(nOption)
    {
        case ONDEMAND_OPTION_LP:
            obisTable = obisTable_lp; 
            break;
        case ONDEMAND_OPTION_DR:
            obisTable = obisTable_dr; 
            break;
        case ONDEMAND_OPTION_LOAD_CONTROL:
            obisTable = obisTable_load_control; 
            break;
#if 0
        case ONDEMAND_OPTION_CURRENT:
            obisTable = obisTable_test; 
            break;
#endif
        default:
            obisTable = obisTable_current; 
            break;
    }
    
    XDEBUG(" DLMS: Option=%d, Offset=%d, Count=%d\r\n",
            nOption, nOffset, nCount);

    XDEBUG("    LAST METERING = %04d/%02d/%02d %02d:%02d:%02d, LAST EVENT = %04d/%02d/%02d %02d:%02d:%02d\r\n",
            tmLastMetering.year, tmLastMetering.mon, tmLastMetering.day,
            tmLastMetering.hour, tmLastMetering.min, tmLastMetering.sec,
            tmLastEventLog.year, tmLastEventLog.mon, tmLastEventLog.day,
            tmLastEventLog.hour, tmLastEventLog.min, tmLastEventLog.sec);

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

                pOndemand = NewOperation(ONDEMAND_OPR_DATA, &id, sizeof(DLMSRESULT));
                pOndemand->nMeteringType = pCommand->nMeteringType;
                pOndemand->nOffset = nOffset;
                pOndemand->nCount = nCount;
                pOndemand->pParser = (void *)this;
                memcpy(&pOndemand->tmLastMetering, &tmLastMetering, sizeof(TIMESTAMP));
                memcpy(&pOndemand->tmLastEventLog, &tmLastEventLog, sizeof(TIMESTAMP));
                pOndemand->nVendor = METER_VENDOR_UNKNOWN;
                UnlockOperation();

                GetTimeTick(&pOndemand->tmStart);

                bConnected = FALSE;
                nError = endiOpen(pCommand->codi, (HANDLE *)&pOndemand->endi, &id);
                if (nError != CODIERR_NOERROR)
                {
                    XDEBUG(" endiOpen Error(%d): %s\r\n", nError, codiErrorMessage(nError));
                    nState = STATE_RETRY;
                    break;
                }

                // SEQ 검사 옵션을 켠다. 
                endiSetOption(pOndemand->endi, ENDIOPT_SEQUENCE, 1);

                nError = sensorConnect(pOndemand->endi, GetConnectionTimeout(ENDDEVICE_ATTR_POWERNODE));
                if (nError != CODIERR_NOERROR)
                {
                    XDEBUG(" endiConnect Error(%d): %s\r\n", nError, codiErrorMessage(nError));
                    nState = STATE_RETRY;
                    break;
                }

                bConnected = TRUE;
                pChunk->Rollback();

                GetTimeTick(&pOndemand->tmConnect);
                gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);

                nState = STATE_READ_NODEINFO;
                break;
        
            case STATE_READ_NODEINFO :                         // NODE_INFO를 읽어 본다.
                addr = ENDI_ROMAP_NODE_KIND;
                len  = sizeof(ENDI_NODE_INFO);

                pOndemand->stat.nCount ++;
                pOndemand->stat.nSendCount ++;
                pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
                gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
                nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &n, GetReplyTimeout());
                if (nError != CODIERR_NOERROR)
                {
                    if (pOndemand->nRetry < GetRetryCount())                // 3회 이상 연속 실패시 종료
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

                nState = STATE_READ_AMRINFO;
                break;     

            case STATE_READ_AMRINFO :                         // AMR_INFO를 읽어 본다.
                addr = ENDI_ROMAP_TEST_FLAG;
                len  = sizeof(AMR_PARTIAL_INFO);

                pOndemand->stat.nCount ++;
                pOndemand->stat.nSendCount ++;
                pOndemand->stat.nSend += sizeof(ENDI_ROMREAD_PAYLOAD) + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
                gettimeofday((struct timeval *)&pOndemand->tmSend, NULL);
                nError = endiReadRom(pOndemand->endi, addr, len, (BYTE *)szBuffer, &n, GetReplyTimeout());
                if (nError != CODIERR_NOERROR)
                {
                    if (pOndemand->nRetry < GetRetryCount())                // 3회 이상 연속 실패시 종료
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

                nState = STATE_DLMS_GET;
                break;     

            case STATE_DLMS_GET:
                XDEBUG(" DLMS GET : %s\r\n", szGUID);
                nState = STATE_DLMS_SNRM;
                break;

            case STATE_DLMS_SNRM:
                XDEBUG(" DLMS Set Normal Response Mode : %s\r\n", szGUID);

                dlmsFrame = NULL; dlmsLength = 0; 
#if 0
                /** 설정값을 꼭 보낼 필요는 없기 때문에 NULL로 변경한다 */
                nError = dlmsMakeSendFrame(HDLC_CTRL_SNRM, m_pSnrmNego, m_nSnrmNegoLength, 
                        DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, 
                        (BYTE **)&dlmsFrame, &dlmsLength);
#else
                nError = dlmsMakeSendFrame(HDLC_CTRL_SNRM, NULL, 0, 
                        DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, 
                        (BYTE **)&dlmsFrame, &dlmsLength);
#endif
                if(nError == DLMSERR_NOERROR && dlmsFrame != NULL)
                {
                    dlmsReqAddress(&dlmsFrame->hdr, &dlmsFrame->hdr.addr, NULL, NULL, &ctrl);
                    pOndemand->lastTxControl = ctrl;
                    SendCommand(pOndemand, &id, (const char *)dlmsFrame, dlmsLength);
                }
                else
                {
                    XDEBUG(" MakeFrameError : %s\r\n", dlmsGetErrorMessage(nError));
                    nState = STATE_ERROR;
                    break;
                }
                FREE(dlmsFrame);

                m_Locker.Lock();
                ClearOperation(pOndemand);
                pOndemand->nState = nState;
                pOndemand->bReply = FALSE;
                nState = STATE_WAIT_REPLY;
                ResetEvent(pOndemand->hEvent);
                m_Locker.Unlock();

                break;

            case STATE_DLMS_AARQ:
                XDEBUG(" DLMS Application Association Request : %s\r\n", szGUID);

                // Security 얻어오기
                memset(security, 0, sizeof(security));
                nSecurityLen = 0;
                if(!m_pEndDeviceList->GetMeterSecurity(&id, security, &nSecurityLen))
                {
                    // 실패할 경우 기본 Security 사용
                    nSecurityLen = 8;
                    memcpy(security, "1A2B3C4D", nSecurityLen);
                }
                dlmsFrame = NULL; dlmsLength = 0;
                nConformance = CONF_LN_BLOCK_TRANSFER_GET 
                     | CONF_LN_BLOCK_TRANSFER_SET
                     | CONF_LN_GET
                     | CONF_LN_SELECTIVE_ACCESS
                     | CONF_LN_ACTION
                     | CONF_LN_SET;
                nError= dlmsMakeAarqFrame(DLMS_ASSOCIATION_LN, DLMS_AUTH_VALUE_GRAPHICSTRING, security, nSecurityLen,
                        nConformance, DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, (BYTE **)&dlmsFrame, &dlmsLength);

                dlmsDumpConformanceValue(DLMS_ASSOCIATION_LN, nConformance);

                if(nError == DLMSERR_NOERROR && dlmsFrame != NULL)
                {
                    dlmsReqAddress(&dlmsFrame->hdr, &dlmsFrame->hdr.addr, NULL, NULL, &ctrl);
                    pOndemand->lastTxControl = ctrl;
                    SendCommand(pOndemand, &id, (const char *)dlmsFrame, dlmsLength);
                }
                else
                {
                    XDEBUG(" MakeFrameError : %s\r\n", dlmsGetErrorMessage(nError));
                    nState = STATE_ERROR;
                    break;
                }
                FREE(dlmsFrame);

                m_Locker.Lock();
                ClearOperation(pOndemand);
                pOndemand->nState = nState;
                pOndemand->bReply = FALSE;
                nState = STATE_WAIT_REPLY;
                ResetEvent(pOndemand->hEvent);
                m_Locker.Unlock();

                nSeek = 0;
                nBlockCount = 0;
                nLpInterval = DEFAULT_LP_INTERVAL;
                nTotalLpCount = 0;
                break;

            case STATE_SEGMENTATION:
                XDEBUG(" DLMS Segementation : %s\r\n", szGUID);

                dlmsFrame = NULL; dlmsLength = 0; 
                ctrl  = pOndemand->lastTxControl & 0xf0; 
                ctrl += 0x21; /** Receive Count를 1 증가 시키고 RR, RNR, SNRM, DISC, UI 중 하나인것으로 설정 */
                if ((pOndemand->lastRxControl & 0x0e) == 0x0e) ctrl = HDLC_CTRL_RR;
                dlmsMakeSendFrame(ctrl, NULL, 0, DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, (BYTE **)&dlmsFrame, &dlmsLength);
                pOndemand->lastTxControl = ctrl;
                SendCommand(pOndemand, &pOndemand->id, (const char *)dlmsFrame, dlmsLength);
                FREE(dlmsFrame);

                m_Locker.Lock();
                /** Segmentation 은 nState를 이전 것으로 이어 간다.
                ClearOperation(pOndemand);
                pOndemand->nState = nState;
                */
                pOndemand->bReply = FALSE;
                ResetEvent(pOndemand->hEvent);
                m_Locker.Unlock();

                nState = STATE_WAIT_REPLY;
                break;

            case STATE_WAIT_REPLY:
                WaitForSingleObject(pOndemand->hEvent, (int)(GetReplyTimeout()/1000));
                m_Locker.Lock();
                if(pOndemand->bSegmentation)
                {
                    /** Segmentation */
                    pOndemand->bSegmentation = FALSE;
                    m_Locker.Unlock();
                    nState = STATE_SEGMENTATION;
                    break;
                }

                if(pOndemand->bPartialWait)
                {
                    pOndemand->bPartialWait = FALSE;
                    m_Locker.Unlock();
                    break;
                }
                m_Locker.Unlock();
                if(pOndemand->bReply)
                {
                    switch(pOndemand->nState)
                    {
                        case STATE_DLMS_SNRM:
                            nState = STATE_DLMS_AARQ;
                            break;
                        case STATE_DLMS_AARQ:
                            nState = STATE_QUERY;
                            break;
                        case STATE_QUERY:
                            nState = STATE_QUERY_PARSE;
                            break;
                        case STATE_DISCONNECT:
                            nState = STATE_OK;
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
                    if(pOndemand->nState == STATE_DISCONNECT) 
                    {
                        nState = STATE_OK;
                    }
                    else
                    {
                        nFailCount ++;
                        XDEBUG(" FAIL COUNT SUCC %d\r\n", nFailCount);
                        if (nFailCount >= 3)
                            nState = STATE_ERROR;
                        else
                            nState = pOndemand->nState;
                    }
                }
                break;

            case STATE_QUERY:
                // 마지막 테이블 검사
                if (obisTable[nSeek].descr == NULL)
                {
                    nState = STATE_OK;
                    pOndemand->nResult = ONDEMAND_ERROR_OK;
                    break;
                }
 
                m_Locker.Lock();
                ClearOperation(pOndemand);
                pOndemand->nState = nState;
                m_Locker.Unlock();
                nState = STATE_WAIT_REPLY;
     
                dlmsObisToString(&obisTable[nSeek].obis.code, szObis); 
                XDEBUG("\r\n %s: CLASS=%d, OBIS=%s, ATTR=%d, NAME=%s\r\n",
                         szGUID,
                         obisTable[nSeek].obis.cls, szObis, obisTable[nSeek].obis.attr,
                         obisTable[nSeek].descr);
 
                GetTimeTick(&startTime);
                if (nBlockCount > 0)
                {
                    // 연속으로 다음 블럭을 읽고 있는 중일때    
                    dlmsFrame = NULL; dlmsLength = 0;
                    nError = dlmsMakeGetNextFrame(nBlockCount, pOndemand->lastRxControl, DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, 
                            (BYTE **)&dlmsFrame, &dlmsLength);
                    if(nError == DLMSERR_NOERROR)
                    {
                        dlmsReqAddress(&dlmsFrame->hdr, &dlmsFrame->hdr.addr, NULL, NULL, &ctrl);
                        pOndemand->lastTxControl = ctrl;
                        SendCommand(pOndemand, &id, (const char *)dlmsFrame, dlmsLength);
                    }
                    else
                    {
                        XDEBUG(" MakeNextFrameError : %s\r\n", dlmsGetErrorMessage(nError));
                        nState = STATE_ERROR;
                        break;
                    }
                    FREE(dlmsFrame);
                }
                else
                {
                    // 하나의 블럭만 읽거나, 첫 블럭을 읽을때
                    nMaxBlockCount = 0;
                    nReadCount     = 0;
                    nStartCount    = 0;
 
                    switch(obisTable[nSeek].type) {
                        case OBIS_LP :
                            // LP 정보가 없거나 Interval이 잘못되었는지 검사한다.
                            if ((nTotalLpCount == 0) || (nLpInterval == 0))
                            {
                                XDEBUG("\r\n %s: nTotalLpCount=%d, nLpInterval=%d\r\n",
                                         szGUID, nTotalLpCount, nLpInterval);
                                nSeek++;
                                break;
                            }
 
#if     defined(__PATCH_10_2808__)
                            if(*m_pbPatch_10_2808) {
                                nStartCount = (nOffset+1) * 60 / nLpInterval;
                                nReadCount = nCount * 60 / nLpInterval;
                            } else {
#endif
                                nStartCount = nOffset * 24 * 60 / nLpInterval + nCurrentDayLpCount;

                                nReadCount = (nCount - 1) * 24 * 60 / nLpInterval;
                                if(nOffset + 1 == nCount)
                                {
                                    nReadCount += nCurrentDayLpCount;
                                }
                                else
                                {
                                    nReadCount += 24 * 60 / nLpInterval;
                                }
#if     defined(__PATCH_10_2808__)
                            }
#endif

                            if(nStartCount > nTotalLpCount) nStartCount = nTotalLpCount;
                            if(nStartCount < nReadCount) nReadCount = nStartCount;

                            //if (nReadCount <= 0) nReadCount = 1;
                            //if (nReadCount > nTotalLpCount) nReadCount = nTotalLpCount;
                            nMaxBlockCount = MAX_LP_COUNT;
 
                            // 선택적으로 LP 블럭을 읽는다.
                            /** Selector 설정 */
                            memset(&tagBuf, 0, sizeof(tagBuf));
                            pTagValue = (DLMSTAGVALUE *)tagBuf;
                            makeEntrySelector(pTagValue, nTotalLpCount-nStartCount+1, nTotalLpCount-nStartCount+nReadCount);

                            dlmsFrame = NULL; dlmsLength = 0;
                            nError = dlmsMakeGetFrame(&obisTable[nSeek].obis, pTagValue, pOndemand->lastRxControl, 
                                    DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, (BYTE **)&dlmsFrame, &dlmsLength);

                            if(nError == DLMSERR_NOERROR)
                            {
                                XDEBUG(" Selected LP REQ : %d, %d, %d\r\n", nStartCount, nTotalLpCount, nReadCount);
                                dlmsReqAddress(&dlmsFrame->hdr, &dlmsFrame->hdr.addr, NULL, NULL, &ctrl);
                                pOndemand->lastTxControl = ctrl;
                                SendCommand(pOndemand, &id, (const char *)dlmsFrame, dlmsLength);
                            }
                            else
                            {
                                XDEBUG(" MakeSelectedGetFrameError : %s\r\n", dlmsGetErrorMessage(nError));
                                nState = STATE_ERROR;
                                break;
                            }
                            FREE(dlmsFrame);
                            break;
 
                        case OBIS_EX_ENERGY :
                        case OBIS_EX_MAX_DEMAND :
                            nTempVZ = nVZ-1;
                            if (nTempVZ < 0) nTempVZ = 99; 
                            memcpy(&TempObisCode, &obisTable[nSeek], sizeof(OBISCODELIST));
                            TempObisCode.obis.code.f = nTempVZ;

                            dlmsFrame = NULL; dlmsLength = 0;
                            nError = dlmsMakeGetFrame(&TempObisCode.obis, NULL, pOndemand->lastRxControl, 
                                    DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, (BYTE **)&dlmsFrame, &dlmsLength);
                            if(nError == DLMSERR_NOERROR)
                            {
                                dlmsReqAddress(&dlmsFrame->hdr, &dlmsFrame->hdr.addr, NULL, NULL, &ctrl);
                                pOndemand->lastTxControl = ctrl;
                                SendCommand(pOndemand, &id, (const char *)dlmsFrame, dlmsLength);
                            }
                            else
                            {
                                XDEBUG(" MakeGetFrameError : %s\r\n", dlmsGetErrorMessage(nError));
                                nState = STATE_ERROR;
                                break;
                            }
                            FREE(dlmsFrame);
                            break;
                        
                        case OBIS_EX_EX_ENERGY :
                        case OBIS_EX_EX_MAX_DEMAND :
                            nTempVZ = nVZ-2;
                            if (nTempVZ < 0) nTempVZ = 98; 
                            memcpy(&TempObisCode, &obisTable[nSeek], sizeof(OBISCODELIST));
                            TempObisCode.obis.code.f = nTempVZ;
                            dlmsFrame = NULL; dlmsLength = 0;
                            nError = dlmsMakeGetFrame(&TempObisCode.obis, NULL, pOndemand->lastRxControl, 
                                    DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, (BYTE **)&dlmsFrame, &dlmsLength);
                            if(nError == DLMSERR_NOERROR)
                            {
                                dlmsReqAddress(&dlmsFrame->hdr, &dlmsFrame->hdr.addr, NULL, NULL, &ctrl);
                                pOndemand->lastTxControl = ctrl;
                                SendCommand(pOndemand, &id, (const char *)dlmsFrame, dlmsLength);
                            }
                            else
                            {
                                XDEBUG(" MakeGetFrameError : %s\r\n", dlmsGetErrorMessage(nError));
                                nState = STATE_ERROR;
                                break;
                            }
                            FREE(dlmsFrame);
                            break;
                        
                        case OBIS_EX_EX_EX_ENERGY :
                        case OBIS_EX_EX_EX_MAX_DEMAND :
                            nTempVZ = nVZ-3;
                            if (nTempVZ < 0) nTempVZ = 97; 
                            memcpy(&TempObisCode, &obisTable[nSeek], sizeof(OBISCODELIST));
                            TempObisCode.obis.code.f = nTempVZ;
                            dlmsFrame = NULL; dlmsLength = 0;
                            nError = dlmsMakeGetFrame(&TempObisCode.obis, NULL, pOndemand->lastRxControl, 
                                    DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, (BYTE **)&dlmsFrame, &dlmsLength);
                            if(nError == DLMSERR_NOERROR)
                            {
                                dlmsReqAddress(&dlmsFrame->hdr, &dlmsFrame->hdr.addr, NULL, NULL, &ctrl);
                                pOndemand->lastTxControl = ctrl;
                                SendCommand(pOndemand, &id, (const char *)dlmsFrame, dlmsLength);
                            }
                            else
                            {
                                XDEBUG(" MakeGetFrameError : %s\r\n", dlmsGetErrorMessage(nError));
                                nState = STATE_ERROR;
                                break;
                            }
                            FREE(dlmsFrame);
                            break;
                            
                        default :
                            dlmsFrame = NULL; dlmsLength = 0;
                            nError = dlmsMakeGetFrame(&obisTable[nSeek].obis, NULL, pOndemand->lastRxControl, 
                                    DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, (BYTE **)&dlmsFrame, &dlmsLength);
                            if(nError == DLMSERR_NOERROR)
                            {
                                dlmsReqAddress(&dlmsFrame->hdr, &dlmsFrame->hdr.addr, NULL, NULL, &ctrl);
                                pOndemand->lastTxControl = ctrl;
                                SendCommand(pOndemand, &id, (const char *)dlmsFrame, dlmsLength);
                            }
                            else
                            {
                                XDEBUG(" MakeGetFrameError : %s\r\n", dlmsGetErrorMessage(nError));
                                nState = STATE_ERROR;
                                break;
                            }
                            FREE(dlmsFrame);
                            break;
                    }
                }
                m_Locker.Lock();
                pOndemand->bReply = FALSE;
                ResetEvent(pOndemand->hEvent);
                m_Locker.Unlock();

                break;
 
            case STATE_QUERY_PARSE:
                GetTimeTick(&endTime);
                m_Locker.Lock();
                memcpy(&result, pOndemand->pszBuffer, sizeof(DLMSRESULT));
                ClearOperation(pOndemand);
                m_Locker.Unlock();
 
                if (((result.nError == 0) && (result.nLength > 0)) ||
                    ((result.nError != 0) && (result.nLength > 5)))
                {
                    WORD nResLittleLen = result.nLength;
                    if(obisTable[nSeek].save) {
                        pChunk->Add((const char *)&obisTable[nSeek].obis.code, sizeof(OBISCODE));   // OBIS Code        : 6
                        pChunk->Add(obisTable[nSeek].obis.cls);                                     // Class Id         : 1
                        pChunk->Add(obisTable[nSeek].obis.attr);                                    // Attribute        : 1
                        pChunk->Add((const char *)&nResLittleLen, sizeof(WORD));                    // Result Length    : 2
                        pChunk->Add((const char *)result.szBuffer, result.nLength);                 // DLMS_TAG
                    }
                    switch(result.resFlag) {
                        case DLMS_NORMAL :
                        case DLMS_DATABLOCK :
                        case DLMS_LIST :
                            //XDUMP((const char *)(result.szBuffer), result.nLength);
                            nError = dlmsToken(pToken, nToken, &nToken, MAX_DLMS_TOKEN, result.szBuffer, result.nLength);
                            break;
                    }
                    nElapse = GetTimeInterval(&startTime, &endTime);
                    XDEBUG(" %s: RESULT(%d): SRC=0x%04X, DEST=0x%02X, TYPE=%s, TOKEN=%d, LENGTH=%d, ELAPSE=%d.%03d seconds\r\n",
                           szGUID, result.nError,
                           BigToHostShort(result.src), result.dest, dlmsGetFrameType(result.resFlag),
                           nToken, result.nLength,
                                 (int)nElapse/1000, (int)nElapse%1000);
 
                    if (nToken <= 0)
                    {
#if 0
                        if (obisTable == obisTable_metering)
                        {
                            // 통계 자료 카운트 증가
                            nOkCount++;
                            nTotalElapse += nElapse;
 
                            // 정상 종료 시킨다.
                            pChunk->Add((char *)result.szBuffer, result.nLength);
                            nState = STATE_OK;
                            break;
                        }
#endif
     
                        XDEBUG(" DLMS ENTRY ERROR!! (TOKEN=%d)\r\n", nToken);
                        nState = STATE_ERROR;
                        break;
                    }
 
                    nTotalElapse += nElapse;
                    switch(obisTable[nSeek].type) {
                        case OBIS_COSEM_DEVICE_NAME:
                            if(nToken >= 1)
                            {
                                if((len = MIN(pToken[0].len, sizeof(szModel)-1))>0)
                                {
                                    /** Model Setting */
                                    strncpy(szModel, (char *)pToken[0].var.p, len);
                                    XDEBUG(" %s: MODEL [%s]\r\n", szGUID, szModel);
                                    m_pEndDeviceList->UpdateMeterInventory(&id, 
                                            UNDEFINED_PORT_NUMBER, PARSER_TYPE_DLMS,
                                            GetPrefixVendor(), METER_VENDOR_NOTSET, szModel, NULL, 0, 0);
                                }
                            }
                            break;
                        case OBIS_METER_TYPE :
                            if (nToken >= 15)
                            {
                                if(pToken[0].type != DLMS_DATATYPE_ARRAY)
                                {
                                    XDEBUG("\033[1;40;31m ==== INVALID DATA TYPE reqired(%d) current(%d) ==== \033[0m\r\n", 
                                            DLMS_DATATYPE_ARRAY, pToken[0].type);
                                    break;
                                }
                                if(nMeterIdLen == 0)
                                {
                                    strncpy(szMeterId, pToken[4].var.p ? (char *)pToken[4].var.p : "", 8);
                                }
                                nLpInterval                 = pToken[12].var.u8;
                                pCurrentTime = (TIMESTAMP *)pToken[5].var.p;      // 현재 시간
                                pLastLpTime = (TIMESTAMP *)pToken[13].var.p;     // 마지막 LP 기록 시간
                                XDEBUG(" %s: LPINTERVAL [%d]\r\n", szGUID, nLpInterval);
                                if(nLpInterval && !memcmp(pCurrentTime, pLastLpTime, 4)) // 오늘
                                {
                                    if(memcmp(pToken[5].var.p + 4, pToken[13].var.p + 4, 2) >= 0) // 현재 시간이 LP 기록 시간보다 앞설 때
                                    {
                                        nCurrentDayLpCount = pLastLpTime->hour * (60/nLpInterval) + ((int) pLastLpTime->min / nLpInterval) + 1;
                                        XDEBUG(" nCurrentDayLpCount =  %d\r\n", nCurrentDayLpCount);
                                    }
                                }

                                XDEBUG(" %s: METERID [%s]\r\n", szGUID, szMeterId);

                                /* Current Value를 저장하기 위해 사용 */
                                nPulsePerKwh = (int) pToken[8].var.u16;

                                pOndemand->nVendor = pToken[3].var.p[0];       // 제조사에 따른 처리를 위해 Vendor 정보 수집
                                m_pEndDeviceList->UpdateMeterInventory(&id, 
                                        UNDEFINED_PORT_NUMBER, PARSER_TYPE_DLMS,
                                        GetPrefixVendor(), GetDlmsVendorToAimirVendor(pToken[3].var.p[0]), 
                                        NULL, szMeterId, 0, 0);
                            }
                            break;
 
                        case OBIS_ACT_CONSTANT :
                            if (nToken >= 1)
                            {
                                if (pToken[0].type == DLMS_DATATYPE_FLOAT32)
                                    acon = pToken[0].var.f32;
                                else if (pToken[0].type == DLMS_DATATYPE_OCTET_STRING)
                                    acon = (double)*pToken[0].var.p;
                                else acon = 0;
                                XDEBUG(" acon = %f\r\n", acon);
                            }
                            break;
 
                        case OBIS_RACT_CONSTANT :
                            if (nToken >= 1)
                            {
                                if (pToken[0].type == DLMS_DATATYPE_FLOAT32)
                                    rcon = pToken[0].var.f32;
                                else if (pToken[0].type == DLMS_DATATYPE_OCTET_STRING)
                                    rcon = (double)*pToken[0].var.p;
                                else rcon = 0;
                                XDEBUG(" rcon = %f\r\n", rcon);
                            }
                            break;
 
                        case OBIS_LP_INFO :
                            if (nToken >= 1)
                            {
                                nTotalLpCount = pToken[0].var.u32;
                                XDEBUG(" TOTAL LP COUNT=%d\r\n", nTotalLpCount);
                            }
                            break;
 
                        case OBIS_CUSTOMER : // Customer ID
                            if ((nToken >= 1) && (pToken[0].type == DLMS_DATATYPE_OCTET_STRING))
                            {    
                                strncpy(szMeterId + nMeterIdLen, (char *)pToken[0].var.p, MIN(pToken[0].len, 
                                            sizeof(szMeterId)-1-nMeterIdLen));
                                nMeterIdLen = strlen(szMeterId);
                            }
                            break;
 
                        case OBIS_MANUFACTURER : // Manufacturer ID
                            if ((nToken >= 1) && (pToken[0].type == DLMS_DATATYPE_OCTET_STRING))
                            {
                                strncpy(szMeterId + nMeterIdLen, (char *)pToken[0].var.p, MIN(pToken[0].len, 
                                            sizeof(szMeterId)-1-nMeterIdLen));
                                nMeterIdLen = strlen(szMeterId);
                            }
                            break;
 
                        case OBIS_VZ :
                            if ((nToken >= 1) && (pToken[0].type == DLMS_DATATYPE_UINT8))
                            {
                                nVZ = pToken[0].var.u8;
                            }
                            break;
 
                        case OBIS_LP :
                            nRecord = 0;
                            for(i=0; i<nToken; i++)
                            {
                                if (pToken[i].type == DLMS_DATATYPE_STRUCTURE)
                                    nRecord++; 
                            }
                            if(nRecord==0) 
                            {
                                // LP를 읽지 못한 경우 skip 한다. Wizit 2008 Meter에서 발생
                                nReadCount = 0;
                            }
                            else 
                            {
                                nReadCount -= nRecord;
                            }
                            //XDEBUG(" LP nReadCount = %d\r\n", nReadCount);
                            break;

                        case OBIS_CUR_BILLING:
                            /** 이 값이 설정되어 있어야 의미가 있다 */
                            if(nPulsePerKwh > 0 && nToken >= 17)
                            {
                                /** Value 값 설정 */
                                m_pEndDeviceList->ClearValue(&pOndemand->id);
                                m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("PULSEPERKWH"), VARSMI_WORD, sizeof(WORD), (BYTE *) &nPulsePerKwh);
                                m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("ACTIVE"), VARSMI_UINT, sizeof(UINT), (BYTE *) &pToken[2].var.u32);
                                m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("REACTIVE"), VARSMI_UINT, sizeof(UINT), (BYTE *) &pToken[3].var.u32);
                                m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("MAX_DEMAND"), VARSMI_WORD, sizeof(WORD), (BYTE *) &pToken[4].var.u16);
                                m_pEndDeviceList->AddValue(&pOndemand->id, const_cast<char *>("CUM_DEMAND"), VARSMI_WORD, sizeof(WORD), (BYTE *) &pToken[5].var.u16);
                            }
                            break;
                    }
 
                    switch(result.resFlag) {
                        case DLMS_NORMAL :
                            //XDEBUG("DLMS_NORMAL\r\n");
                            dlmsDumpValue(result.szBuffer, result.nLength);
                            break;
 
                        case DLMS_DATABLOCK :
                            //XDEBUG("DLMS_DATABLOCK\r\n");
                            //XDUMP((char *)result.szBuffer, result.nLength);
                            dlmsDumpValue(result.szBuffer, result.nLength);
                            break;
 
                        case DLMS_LIST :
                            XDEBUG(" RESULT %d byte(s)\r\n", result.nLength); 
                            XDUMP((char *)result.szBuffer, result.nLength, TRUE);
                            break;
                    }

                    nBlockCount++;
                    if ((nBlockCount >= nMaxBlockCount) || (nReadCount <= 0))
                    {
                        nBlockCount = 0; 
                        nSeek++; 
                    }
                }
                else
                {
                    XDEBUG(" %s: RESULT ERROR(%d): SRC=0x%04X, DEST=0x%02X, TYPE=%s, LENGTH=%d Byte(s)\r\n",
                           szGUID, result.nError, BigToHostShort(result.src), result.dest,
                           dlmsGetFrameType(result.resFlag), result.nLength);
                    XDEBUG(" %s: %s\r\n", szGUID, dlmsGetResponseCode(result.nError));
                    nBlockCount = 0;
                    nSeek++;
                }
 
                nState = STATE_QUERY;
                break;

            case STATE_ERROR : /** STATE_DISCONNECT 로 자동 연결된다 */
                XDEBUG (" STATE_ERROR\r\n");

            case STATE_DISCONNECT :
                XDEBUG(" DLMS Disconnect : %s\r\n", szGUID);

                m_Locker.Lock();
                ClearOperation(pOndemand);
                pOndemand->nState = STATE_DISCONNECT;
                m_Locker.Unlock();
                nState = STATE_WAIT_REPLY;
     
                dlmsFrame = NULL; dlmsLength = 0; 
                nError = dlmsMakeSendFrame(HDLC_CTRL_DISC, NULL, 0, 
                        DLMS_SOURCE_ADDR, DLMS_DESTINATION_ADDR, 
                        (BYTE **)&dlmsFrame, &dlmsLength);
                if(nError == DLMSERR_NOERROR && dlmsFrame != NULL)
                {
                    dlmsReqAddress(&dlmsFrame->hdr, &dlmsFrame->hdr.addr, NULL, NULL, &ctrl);
                    pOndemand->lastTxControl = ctrl;
                    SendCommand(pOndemand, &id, (const char *)dlmsFrame, dlmsLength);
                }
                else
                {
                    XDEBUG(" MakeFrameError : %s\r\n", dlmsGetErrorMessage(nError));
                    nState = STATE_OK;
                    break;
                }
                FREE(dlmsFrame);

                m_Locker.Lock();
                pOndemand->bReply = FALSE;
                ResetEvent(pOndemand->hEvent);
                m_Locker.Unlock();
                break;

            case STATE_OK :
                endiDisconnect(pOndemand->endi);

                GetTimeTick(&pOndemand->tmEnd);
                time(&tmEnd);

                if (pOndemand->nResult == ONDEMAND_ERROR_OK)
                {
					nResult = ONDEMAND_ERROR_OK;
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
                }
                else
                {
                    nResult = ONDEMAND_ERROR_FAIL;
                    XDEBUG("\r\n");
                    XDEBUG("-----------------[ FAIL ]----------------\r\n");
                }

			    if (pOndemand->stat.nCount == 0 || pOndemand->stat.nRecvCount == 0)
					 nElapse = 0;
			    else nElapse = pOndemand->stat.nResponseTime / pOndemand->stat.nRecvCount;
			    pOndemand->stat.nUseTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmEnd);
			    pOndemand->stat.nConnectTime = GetTimeInterval(&pOndemand->tmStart, &pOndemand->tmConnect);

                if (pStat != NULL)
                    memcpy(pStat, &pOndemand->stat, sizeof(METER_STAT));

                XDEBUG("      Metering Elapse = %d second(s)\r\n", tmEnd-tmStart);
                XDEBUG("\r\n");
                XDEBUG("       Transmit Count = %d frams(s)\r\n", pOndemand->stat.nCount);
                XDEBUG("           Send Count = %d frams(s)\r\n", pOndemand->stat.nSendCount);
                XDEBUG("        Receive Count = %d frams(s)\r\n", pOndemand->stat.nRecvCount);
                //XDEBUG("   Error Packet Count = %d frams(s)\r\n", pOndemand->stat.nError);
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

    dlmsFreeToken(pToken, nToken);

    return nResult;
}

void CDLMSParser::OnData(EUI64 *pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
{
    DLMS_HEADER     *pHeader;
    int             nSize;
    DATASTREAM      *pStream;

    /** Issue #2659: pOndemand에 대한 NULL 검사를 수행한다 */
    if(pOndemand == NULL) return;

    // Thread Debugging
    __PTHREAD_INFO__;

    m_Locker.Lock();
    pOndemand->stat.nCount ++;
    pOndemand->stat.nRecvCount ++;
    pOndemand->stat.nRecv += nLength + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);

    pStream = pOndemand->pDataStream;
    if(pStream && !ParseStream(pStream, pszData, nLength, pOndemand))
    {
        pHeader =  (DLMS_HEADER *)pStream->pszBuffer;
        if(pHeader == NULL)
        {
            XDEBUG(" DLMS STREAM PARSING FAIL\r\n");
            m_Locker.Unlock();
            return;
        }
        nSize = BigToHostShort(pHeader->typeLen) & 0x7FF;
        /** 아직 Frame이 완료되지 않았을 때 */
        XDEBUG(" PENDDING FRAME TOT=%d CURR=%d\r\n", nSize, pStream->nLength);
    }
    m_Locker.Unlock();
}

/** OnData에서 Lock/Unlock을 하기 때문에 OnFrame에서는 Lock/Unlock을 하지 않는다
  */
BOOL CDLMSParser::OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, int nRemain, void *pCallData)
{
    char            szGUID[20];
    DLMS_HEADER     *pHeader;
    DLMS_RES_ADDR   *pAddress;
    DLMSRESULT      *pResult;
    DLMS_GETRES_PAYLOAD *get;
    DLMS_BLOCKRES_PAYLOAD *getBlock;
    DLMS_GETERR_PAYLOAD *err;
    DLMS_SETRES_PAYLOAD *set;
    DLMS_UA_FRAME       *ua;
    DLMS_AARE_FRAME     *aare;
    int             len, nSize;
    //BYTE            ctrl, resCtrl=0;
    BYTE            resCtrl=0;
    BYTE            destLSAP;
    //DLMS_FRAME      *pFrame, *dlmsFrame=NULL;
    DLMS_FRAME      *pFrame;
    BYTE            *pPayload;
    //int             dlmsLength=0;
    int             nTotLen;
    BYTE            *pszCData;
    BYTE            nHdlcFormat;
    BYTE            dlmsResDest=0;
    UINT            dlmsResSrc=0;
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;

    eui64toa(&pOndemand->id, szGUID);

    pHeader =  (DLMS_HEADER *)pStream->pszBuffer;
    if(pHeader == NULL)
    {
        XDEBUG(" DLMS STREAM PARSING FAIL\r\n");
        return FALSE;
    }
    nSize = BigToHostShort(pHeader->typeLen) & 0x7FF;

    nTotLen = pStream->nLength;
    pszCData = (BYTE *)pStream->pszBuffer;
    pAddress = (DLMS_RES_ADDR *)&pHeader->addr;
    pResult = (DLMSRESULT *)pOndemand->pszBuffer;
    nHdlcFormat = pszCData[1];

    dlmsResAddress(pHeader, pAddress, &dlmsResSrc, &dlmsResDest, &resCtrl);

    // 수신 Count 증가
    pOndemand->lastRxControl = resCtrl;

    XDEBUG(" SOURCE=0x%04X, DESTINATION=0x%02X, CONTROL=0x%02X(%s)\r\n",
            dlmsResSrc, dlmsResDest, resCtrl,
            pOndemand->bAuthFlag ? "DATA" : dlmsGetHdlcControlType(resCtrl));
    if ((nHdlcFormat & HDLC_SEGMENTATION) || (pOndemand->nPartial > 0))
        XDEBUG(" FRAME-TYPE  = MULTIPART FRAME (%d)\r\n", pOndemand->nPartial+1);
    else XDEBUG(" FRAME-TYPE  = SINGLE FRAME\r\n");

    if (pOndemand->nPartial > 0)
    {
        if (pResult != NULL)
        {
            len = nTotLen - 12;     /** Header + Tail 만큼 제한다 */
            if(len < 0) len = 0;

            //m_Locker.Lock();
            if(len > 0)
            {
                /** Payload의 시작 위치 부터 Copy */
                memcpy(pResult->szBuffer+pResult->nLength, (BYTE *)&pszCData[9], MIN(MAX_DLMS_RESULT_SIZE, len));
            }
            pResult->nLength += MIN(MAX_DLMS_RESULT_SIZE, len);
            pOndemand->nLength = pResult->nLength + MAGIC_METERING_HEADER;
            //m_Locker.Unlock();

            XDEBUG(" MULTIPART (%2d):  Length = %d\r\n", pOndemand->nPartial + 1, pResult->nLength);
        }

        /** 연속 Frame이 오고 있을 때 Comm Timeout을 연장시키는 Method가 필요하다 */
        if (nHdlcFormat & HDLC_SEGMENTATION)
        {
            //m_Locker.Lock();
            pOndemand->nPartial++;
            pOndemand->bPartialWait = TRUE;

            if(nRemain <= 0) 
            {
                pOndemand->bSegmentation = TRUE;
#if 0
                ctrl  = pOndemand->lastTxControl & 0xf0; 
                ctrl += 0x21; /** Receive Count를 1 증가 시키고 RR, RNR, SNRM, DISC, UI 중 하나인것으로 설정 */
                if ((resCtrl & 0x0e) == 0x0e) ctrl = HDLC_CTRL_RR;
                dlmsMakeSendFrame(ctrl, NULL, 0, dlmsResDest, dlmsResSrc, (BYTE **)&dlmsFrame, &dlmsLength);
                pOndemand->lastTxControl = ctrl;
                SendCommand(pOndemand, &pOndemand->id, (const char *)dlmsFrame, dlmsLength);
                FREE(dlmsFrame);
#endif
            }
            //m_Locker.Unlock();
            if(nRemain <= 0)
            {
                SetEvent(pOndemand->hEvent);
            }
            return TRUE;
        }

        //m_Locker.Lock();
        pOndemand->nPartial = 0;
        pOndemand->bReply = TRUE;
        pOndemand->bPartialWait = FALSE;
        pOndemand->bSegmentation = FALSE;
        //m_Locker.Unlock();
        SetEvent(pOndemand->hEvent);
        return TRUE;
    }

    if (nTotLen > (int)sizeof(HDLC_FRAME))
    {    
        pFrame = (DLMS_FRAME *)pszCData;
        pPayload = dlmsGetResPayload(pFrame);
        pResult->headerSize = pPayload - pszCData;
        destLSAP = *pPayload;

        if (pOndemand->bAuthFlag && (destLSAP == LSAP_DEST_ADDRESS))
        {
            get = (DLMS_GETRES_PAYLOAD *)dlmsGetResPayload(pFrame);

            XDEBUG(" type        = %s.%s.%s\r\n", 
                        dlmsGetMethodType(get->method & DLMS_METHOD_MASK),
                        dlmsGetFlowType(get->method & DLMS_RESPONSE),
                        dlmsGetResType(get->resFlag));
            XDEBUG(" id/priority = 0x%02X\r\n", get->idPriority);

            if ((get->method & DLMS_RESPONSE) == DLMS_RESPONSE)
            {    
                // GET.response, SET.response
                switch(get->method & DLMS_SET) {
                  case DLMS_GET :
                       //XDEBUG(" result      = %d (0x%02X)\r\n", get->result, get->result);
                       switch(get->resFlag) {
                         case DLMS_NORMAL :
                         case DLMS_LIST :   /** TODO: List 처리 코드를 별도로 분리해야 한다 */
                              //m_Locker.Lock();
                                 if (pResult != NULL)
                                 {
                                  err = (DLMS_GETERR_PAYLOAD *)dlmsGetResPayload(pFrame);
                                  pResult->dest    = dlmsResDest;
                                  pResult->src     = dlmsResSrc;
                                  pResult->method  = get->method;
                                  pResult->resFlag = get->resFlag;
                                  if (get->result == 0)
                                       pResult->nError = get->result;           
                                  else pResult->nError = err->resCode;
                                  len = nTotLen - pResult->headerSize - sizeof(DLMS_TAIL) - 7; // TAG Index
                                  if (len < 0) len = 0;
                                  len = MIN(MAX_DLMS_RESULT_SIZE-pResult->nLength, len);
                                  if(len > 0)
                                  {
                                      memcpy(pResult->szBuffer+pResult->nLength, (BYTE *)&get->param, len);
                                  }
                                  pResult->nLength += len;
                                  pOndemand->nLength = pResult->nLength + MAGIC_METERING_HEADER;
                                 }
                              //m_Locker.Unlock();
                              break;
                         case DLMS_DATABLOCK :
                              //m_Locker.Lock();
                                 if (pResult != NULL)
                                 {
                                  int nBlockHeaderLen=4;

                                  getBlock = (DLMS_BLOCKRES_PAYLOAD *)dlmsGetResPayload(pFrame);
                                  pResult->dest    = dlmsResDest;
                                  pResult->src     = dlmsResSrc;
                                  pResult->method  = get->method;
                                  pResult->resFlag = get->resFlag;
                                  pResult->lastBlock = getBlock->last;
                                  pResult->nError = 0;

                                  /**
                                    *
                                    * Block Count 이후의 정보
                                    *  TODO : 이 정보들을 이용해서 아래 코드를 수정해야 한다.
                                    *
                                    * 
                                    * 싱글 프레임 형식
                                    * C4 01 81 00 AA BB
                                    * AA = 데이터 타입

                                    * 4번째가 00이 아니면 에러;; 

                                    * 멀티 프레임 형식

                                    * 처음 프레임
                                    * C4 02 81 00 00 00 00 AA 00 BB (CC CC) 01 DD (EE EE)
                                    * AA = 프레임 번호
                                    * BB = 프레임 길이 (자신 이후부터 바이트 수)
                                    * (단, BB 가 82면 CC CC가 BYTE 수)
                                    * DD = Array 개수
                                    * (단, DD 가 82면 EE EE가 Array 개수)

                                    *  ==> blockCount 이후에 frame length가 들어간다.
                                    *      BlockLength와 Array Length에 대한 처리 코드 필요
                                    *

                                    * 중간 프레임
                                    * C4 02 81 00 00 00 00 AA 00 BB (CC CC)
                                    * (처음 프레임과 동일하지만 Array 개수가 없음)

                                    * 마지막 프레임
                                    * C4 02 81 01 00 00 00 AA 00 BB (CC CC)
                                    * (81 다음에 01)

                                    * ------------------------------------------------------------------
                                    * 01 82 01 2c ....
                                    *  01 - array
                                    *  82 - the msb bit means that in the next (82 and 0x7F) Bytes is the value of the length. 
                                    *    eg. 0x82 = 10000010 - if the most significant bit is 1 then you look the other bits 
                                    *    (0 - 6) and this is the number of succeding Bytes which carry the information about the length
                                    *  012c - 300 dec 
                                    *
                                    */
                                  XDEBUG(" Block Count = %d\r\n", BigToHostInt(getBlock->blockCount));

                                  /** nBlockHeaderLen 수정 */
                                  nBlockHeaderLen = 2;

                                  if(getBlock->payload[nBlockHeaderLen - 1] & 0x80)
                                  {
                                      nBlockHeaderLen += getBlock->payload[nBlockHeaderLen - 1] & 0x7F;
                                  }

#if 0
                                  if(BigToHostInt(getBlock->blockCount) == 1)
                                  if(getBlock->last) nBlockHeaderLen = 2;   // ?? dummy 2
                                  else nBlockHeaderLen = 4; // dummy 2 + block length 2

                                  /** 제조사에 맞춰서 조정해 주는 코드들 : 우울하다 */
                                  {
                                      switch(pOndemand->nVendor)
                                      {
                                          case 0x09:
                                          case 0x13:
                                              /** 일진 Meter에서 알 수 없는 1Byte Skip */
                                              nBlockHeaderLen --;
#if 0
                                              if(BigToHostInt(getBlock->blockCount) == 1)
                                              {
                                                  /** 첫번째 Block일 경우 들어 있는 2Byte가 안들어 있다 */
                                                  nBlockHeaderLen -= 2;
                                              }
#endif
                                              break;

                                      }
                                  }
#endif

                                  len = nTotLen - pResult->headerSize - sizeof(DLMS_TAIL) - 7 - sizeof(UINT) - nBlockHeaderLen;
                                  if (len < 0) len = 0;
                                  len = MIN(MAX_DLMS_RESULT_SIZE-pResult->nLength, len);
                                  if(len > 0)
                                  {
                                      memcpy(pResult->szBuffer+pResult->nLength, (BYTE *)&getBlock->payload[nBlockHeaderLen], len);
                                  }
                                  pResult->nLength += len;
                                  pOndemand->nLength = pResult->nLength + MAGIC_METERING_HEADER;
                                 }
                              //m_Locker.Unlock();
                              break;
                       }

                       if (nHdlcFormat & HDLC_SEGMENTATION)
                       {
                            //m_Locker.Lock();
                            pOndemand->nPartial++;
                            pOndemand->bPartialWait = TRUE;

                            if(nRemain <= 0) 
                            {
                                pOndemand->bSegmentation = TRUE;
#if 0
                                ctrl  = pOndemand->lastTxControl & 0xf0; 
                                ctrl += 0x21; /** Receive Count를 1 증가 시키고 RR, RNR, SNRM, DISC, UI 중 하나인것으로 설정 */
                                if ((resCtrl & 0x0e) == 0x0e) ctrl = HDLC_CTRL_RR;
                                dlmsMakeSendFrame(ctrl, NULL, 0, dlmsResDest, dlmsResSrc, (BYTE **)&dlmsFrame, &dlmsLength);
                                pOndemand->lastTxControl = ctrl;
                                SendCommand(pOndemand, &pOndemand->id, (const char *)dlmsFrame, dlmsLength);
                                FREE(dlmsFrame);
#endif
                            }
                            //m_Locker.Unlock();
                            if(nRemain <= 0)
                            {
                                SetEvent(pOndemand->hEvent);
                            }
                            XDEBUG(" MULTIPART (%2d):  Length = %d\r\n", pOndemand->nPartial, pResult->nLength);
                            return TRUE;
                       }
                       break;

                  case DLMS_SET :
                       set = (DLMS_SETRES_PAYLOAD *)&pFrame->payload;
                       //XDEBUG(" result      = %d (0x%02X)\r\n", set->result, set->result);
                       break;

                  case DLMS_ACTION :
                       break;
                }
            }
        }
        else
        {
            switch(resCtrl) {
              case HDLC_CTRL_UA :     // UA
                   ua = (DLMS_UA_FRAME *)pszCData;
                   XDEBUG(" format identifier = 0x%02X\r\n", ua->payload.format);
                   XDEBUG(" group identifier  = 0x%02X\r\n", ua->payload.group);
                   XDEBUG(" group length      = %d\r\n", ua->payload.groupLen);
                   break;

              case HDLC_CTRL_AARE :    // AARE
                   aare = (DLMS_AARE_FRAME *)pszCData;
                   XDEBUG(" AARE tag = 0x%02X\r\n", aare->tag);
                   XDEBUG(" AARE len = %d\r\n", aare->tagLen);
                   //dlmsDumpValue(aare.payload, aare->tagLen);
                   pOndemand->bAuthFlag = TRUE;
                   break;
            }
        }
    }
    //m_Locker.Lock();
    pOndemand->bReply = TRUE;
    //m_Locker.Unlock();
    //pChunk->Empty();

    // Event signal
    SetEvent(pOndemand->hEvent);


    return TRUE;
}

