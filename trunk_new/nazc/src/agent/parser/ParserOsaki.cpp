//////////////////////////////////////////////////////////////////////
//
//  ParserOsaki.cpp: implementation of the COsakiParser class.
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
#include "AgentService.h"
#include "Parity.h"
#include "Patch.h"

#include "vendor/pollmiu.h"
#include "ParserOsaki.h"

#include "iec62056_21Define.h"
#include "iec62056_21Functions.h"

#include "osakiDefine.h"
#include "osakiFunctions.h"

#if     defined(__PATCH_11_65__)
/**< Issue #65 JEJU SP */
extern BOOL    *m_pbPatch_11_65;
#endif

typedef enum {
    IDX_CONFIRM,
    IDX_PASSWORD,
    IDX_REQ_PULSE,
    IDX_REQ_RELAY_STATUS,
    IDX_CMD_RELAY_CONNECT,
    IDX_CMD_RELAY_DISCONNECT
} MeteringIndex;

typedef struct {
    BYTE nStatus;
    char *szDesc;
} OSAKI_OPTION_TBL;

#define OSAKI_OPTION_ONDEMAND          0

OSAKI_OPTION_TBL osakiOptionTbl_ondemand [] = {       // Ondemand default
    {IDX_CONFIRM, const_cast<char*>(" ADDRESS CONFIRM")},
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD")},
    {IDX_REQ_PULSE, const_cast<char*>(" CURRENT PULSE")},
    {IDX_REQ_RELAY_STATUS, const_cast<char*>(" REQ RELAY STATUS")},
    {0, NULL}
};

OSAKI_OPTION_TBL osakiOptionTbl_relay_status [] = {       // Get Energy Level
    {IDX_CONFIRM, const_cast<char*>(" ADDRESS CONFIRM")},
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD")},
    {IDX_REQ_RELAY_STATUS, const_cast<char*>(" REQ RELAY STATUS")},
    {0, NULL}
};

OSAKI_OPTION_TBL osakiOptionTbl_relay_connect [] = {      // Relay connect
    {IDX_CONFIRM, const_cast<char*>(" ADDRESS CONFIRM")},
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD")},
    {IDX_CMD_RELAY_CONNECT, const_cast<char*>(" EXE RELAY CONNECT")},
    //{IDX_REQ_RELAY_STATUS, const_cast<char*>(" REQ RELAY STATUS")}, TODO RelayTimeout
    {0, NULL}
};

OSAKI_OPTION_TBL osakiOptionTbl_relay_disconnect [] = {      // Relay disconnect
    {IDX_CONFIRM, const_cast<char*>(" ADDRESS CONFIRM")},
    {IDX_PASSWORD, const_cast<char*>(" IEC21 PASSWORD")},
    {IDX_CMD_RELAY_DISCONNECT, const_cast<char*>(" EXE RELAY DISCONNECT")},
    //{IDX_REQ_RELAY_STATUS, const_cast<char*>(" REQ RELAY STATUS")}, TODO RelayTimeout
    {0, NULL}
};

#define OSAKI_DEMO_ADDRESS      "000020111171"
#define OSAKI_DEMO_PASSWORD     "(D54B36C8)"

//////////////////////////////////////////////////////////////////////
// COsakiParser Class
//////////////////////////////////////////////////////////////////////

static const char *parserNameArray[] = {"OSAKI", "NAMR-P10A", NULL};

//////////////////////////////////////////////////////////////////////
// COsakiParser Class
//////////////////////////////////////////////////////////////////////

COsakiParser::COsakiParser() : CPollBaseParser(SENSOR_TYPE_ZRU, SERVICE_TYPE_ELEC, 
                                            ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL, 
                                            const_cast<char **>(parserNameArray), PARSER_TYPE_OSAKI, 
                                            "OSAKI Parser", "OSAKI", parserNameArray[0],
                                            6000, 6000, 2)
{
}

COsakiParser::~COsakiParser()
{
}

//////////////////////////////////////////////////////////////////////

int COsakiParser::StateMeterSerial(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    int nState = ReadRomMap(ENDI_ROMAP_METER_SERIAL_NUMBER, 20, pOndemand, szBuffer, 
            STATE_READ_METERSERIAL, STATE_READ_ENERGY_LEVEL);

    if(nState == STATE_READ_ENERGY_LEVEL) {
        char szMeterSerial[20];
        memset(szMeterSerial, 0, sizeof(szMeterSerial));
        memcpy(szMeterSerial, szBuffer, 12);
        m_pEndDeviceList->UpdateMeterInventory(&pOndemand->id, 
                UNDEFINED_PORT_NUMBER, PARSER_TYPE_OSAKI,
                NULL, METER_VENDOR_OSAKI, NULL, szMeterSerial, 0, 0);
    }
    return nState;
}

/** OSAKI Meter의 Ondemand command로 들어오는 정보 중 Option 부분을 Disable
 *  시키기 위해 Overriding 한다.
 */
int COsakiParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
    pCommand->nOption = OSAKI_OPTION_ONDEMAND;
    return METERING(pCommand, pStat);
}

/** Ondemand 가 시행되기 전에 사전 조정 작업 수행.
 */
void COsakiParser::ActPreAction(ONDEMANDPARAM *pOndemand)
{
    XDEBUG(" ActPreAction\r\n");

    if(pOndemand != NULL) {
        switch(pOndemand->nOption) {
            case ONDEMAND_OPTION_GET_RELAY_STATUS:               // Relay Status
                XDEBUG(" + Relay Status\r\n");
                pOndemand->pOndemandCallData = osakiOptionTbl_relay_status; break;
            case ONDEMAND_OPTION_DO_RELAY_ON:       // Relay Connect
                XDEBUG(" + Relay Connect\r\n");
                pOndemand->pOndemandCallData = osakiOptionTbl_relay_connect; break;
            case ONDEMAND_OPTION_DO_RELAY_OFF:    // Relay Disconnect
                XDEBUG(" + Relay Disconnect\r\n");
                pOndemand->pOndemandCallData = osakiOptionTbl_relay_disconnect; break;
            default:
                XDEBUG(" + Ondemand\r\n");
                /** OSAKI Meter는 Meter 상수를 읽어야 한다.
                */
                pOndemand->bMeterConstants = TRUE;
                pOndemand->pOndemandCallData = osakiOptionTbl_ondemand; break;
        }
    }
}

int COsakiParser::StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
    pOndemand->nIndex = 0;
    pOndemand->nRetry = 0;
    return STATE_QUERY;
}

int COsakiParser::StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
{
#if 0
    int                nFrameLen = 0;
    ENDIENTRY          *endi = m_pEndDeviceList->GetEndDeviceByID(&pOndemand->id);
    int nError = osakiMakeRequest(szBuffer, (BYTE *)endi->szSerial, strlen(endi->szSerial), (BYTE)0x00, &nFrameLen);


    XDEBUG(" OSAKI QUERY : %s %s\r\n", pOndemand->szId, endi->szSerial);

    if(nError == OSAKIERR_NOERROR)
    {
        SendCommand(pOndemand, &pOndemand->id, (const char *)szBuffer, nFrameLen);
    }
    else
    {
        XDEBUG(" MakeFrameError : %s\r\n", osakiGetErrorMessage(nError));
        return STATE_ERROR;
    }

    m_Locker.Lock();
    ClearOperation(pOndemand);
    pOndemand->bReply = FALSE;
    ResetEvent(pOndemand->hEvent);
    m_Locker.Unlock();

    return STATE_WAIT_REPLY;
#endif
    int                 nFrameLen = 0;
    int                 nIndex = pOndemand->nIndex;
    int                 nError;
    OSAKI_OPTION_TBL       *pOsakiOptionTbl;
    ENDIENTRY           *pEndi;
    char                szAddress[32];

    pOsakiOptionTbl = &((OSAKI_OPTION_TBL *)pOndemand->pOndemandCallData)[nIndex];
    if(pOsakiOptionTbl->nStatus == 0 && pOsakiOptionTbl->szDesc == NULL) return  STATE_OK;

    if((pEndi = m_pEndDeviceList->GetEndDeviceByID(&pOndemand->id)) == NULL) return STATE_OK;
    memset(szAddress, 0, sizeof(szAddress));
    strcpy(szAddress, pEndi->szSerial);

    switch(pOsakiOptionTbl->nStatus)
    {
        case IDX_CONFIRM:           // Address Confirm
            XDEBUG(ANSI_COLOR_GREEN " ADDRESS CONFIRM\r\n" ANSI_NORMAL);
            nError = osakiMakeAddressConfirm(szBuffer, (BYTE *)szAddress, strlen(szAddress), &nFrameLen);
            break;
        case IDX_PASSWORD:          // Password
            XDEBUG(ANSI_COLOR_GREEN " IEC21 PASSWORD\r\n" ANSI_NORMAL);
            nError = osakiMakePasswordConfirm(szBuffer, (BYTE *)OSAKI_DEMO_PASSWORD, strlen(OSAKI_DEMO_PASSWORD), &nFrameLen);
            break;
        case IDX_REQ_PULSE:         // Current Pulse
            XDEBUG(ANSI_COLOR_GREEN " IEC21 REQ PULSE\r\n" ANSI_NORMAL);
            nError = osakiMakeRequest(szBuffer, (BYTE *)szAddress, strlen(szAddress), '3', &nFrameLen);
            break;
        case IDX_REQ_RELAY_STATUS:  // Request Relay Status
            XDEBUG(ANSI_COLOR_GREEN " IEC21 REQ RELAY STATUS\r\n" ANSI_NORMAL);
            nError = osakiMakeRequest(szBuffer, (BYTE *)szAddress, strlen(szAddress), '4', &nFrameLen);
            break;
        case IDX_CMD_RELAY_CONNECT: // Execute Relay Connect
            XDEBUG(ANSI_COLOR_GREEN " IEC21 CMD RELAY CONNECT\r\n" ANSI_NORMAL);
            nError = osakiMakeCommandRequest(szBuffer, IEC_21_CMD_WRITE, '1', (BYTE *)"0040", 4, (BYTE *)"(01)", 4, &nFrameLen);
            break;
        case IDX_CMD_RELAY_DISCONNECT: // Execute Relay Disconnect
            XDEBUG(ANSI_COLOR_GREEN " IEC21 CMD RELAY DISCONNECT\r\n" ANSI_NORMAL);
            nError = osakiMakeCommandRequest(szBuffer, IEC_21_CMD_WRITE, '1', (BYTE *)"0040", 4, (BYTE *)"(00)", 4, &nFrameLen);
            break;
        default:
            return STATE_OK;
    }

    if(nError == OSAKIERR_NOERROR)
    {
        SendCommand(pOndemand, &pOndemand->id, (const char *)szBuffer, nFrameLen);
    }
    else
    {
        XDEBUG(" MakeFrameError : %s\r\n", osakiGetErrorMessage(nError));
        return STATE_ERROR;
    }

    m_Locker.Lock();
    ClearOperation(pOndemand);
    pOndemand->bReply = FALSE;
    ResetEvent(pOndemand->hEvent);
    m_Locker.Unlock();
    return STATE_WAIT_REPLY;
}

int COsakiParser::StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) 
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
BOOL COsakiParser::IsDRAsset(EUI64 *pId)
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
 * @see COsakiParser#IsDRAsset(EUI64 *pId);
 *
 * @warning pEnergyLevelArray는 최소 15bytes 이상의 길이를 가져야 한다.
 */
int COsakiParser::GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray)
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
int COsakiParser::GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel)
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
int COsakiParser::SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen)
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


//////////////////////////////////////////////////////////////////////
// COsakiParser Operations
//////////////////////////////////////////////////////////////////////

void COsakiParser::OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength)
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

BOOL COsakiParser::OnAck(DATASTREAM *pStream, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    OSAKI_OPTION_TBL       *pOsakiOptionTbl;
    int nIndex = pOndemand->nIndex;

    pOsakiOptionTbl = &((OSAKI_OPTION_TBL *)pOndemand->pOndemandCallData)[nIndex];
    if(pOsakiOptionTbl->nStatus != 0 && pOsakiOptionTbl->szDesc != NULL) 
    {

        /** TODO RelayTimeout
          * Relay Control 후 Energy Level을 읽었을 때 바로 반영되지 않기 때문에
          * 성공했다면 당연하게 변경된 것으로 판단한다.
          * 이 부분은 추후에 수정이 필요하다.
          */
        switch(pOsakiOptionTbl->nStatus)
        {
            case IDX_CMD_RELAY_DISCONNECT:     
                pOndemand->nEnergyLevel = ENERGY_LEVEL_MAX; 
                /** Energy Level Update */
                m_pEndDeviceList->SetEnergyLevel(&pOndemand->id, pOndemand->nEnergyLevel, 0);
                XDEBUG(" ENERGY LEVEL : %d\r\n", pOndemand->nEnergyLevel);
                break;
            case IDX_CMD_RELAY_CONNECT:     
                pOndemand->nEnergyLevel = ENERGY_LEVEL_MIN; 
                /** Energy Level Update */
                m_pEndDeviceList->SetEnergyLevel(&pOndemand->id, pOndemand->nEnergyLevel, 0);
                XDEBUG(" ENERGY LEVEL : %d\r\n", pOndemand->nEnergyLevel);
                break;
        }
    }

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex ++;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return CIec21DataStream::OnAck(pStream, pCallData);
}

BOOL COsakiParser::OnNak(DATASTREAM *pStream, void *pCallData)
{
    return CIec21DataStream::OnNak(pStream, pCallData);
}

#if 0
BOOL COsakiParser::OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    int idx =12;
    unsigned long long int value;

    XDEBUG(" OSAKI response payload : %d\r\n", nLength);
    XDUMP((const char *)pszBuffer, nLength, TRUE);

    if(!pOndemand) return TRUE;

    /** OSAKI METER PAYLOAD 분석 */
    value = ASCII2INT(pszBuffer + idx, 7); idx += 7; XDEBUG(" OSAKI   Power : %ld KWh\r\n", value);
    pOndemand->nCurPulse = (UINT) value;

    value = ASCII2INT(pszBuffer + idx, 2); idx += 2; XDEBUG(" OSAKI  Tamper : %ld\r\n", value);
    value = ASCII2INT(pszBuffer + idx, 2); idx += 2; XDEBUG(" OSAKI No Tamp : %ld\r\n", value);
    value = ASCII2INT(pszBuffer + idx, 2); idx += 2; XDEBUG(" OSAKI   Relay : %ld\r\n", value);

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return TRUE;
}
#endif

BOOL COsakiParser::OnDataFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData)
{
    ONDEMANDPARAM *pOndemand = (ONDEMANDPARAM *)pCallData;
    OSAKI_OPTION_TBL       *pOsakiOptionTbl;
    int nIndex = pOndemand->nIndex;
    unsigned long long int value;

    XDEBUG(" OSAKI response payload : %d\r\n", nLength);
    XDUMP((const char *)pszBuffer, nLength, TRUE);

    if(!pOndemand) return TRUE;

    pOsakiOptionTbl = &((OSAKI_OPTION_TBL *)pOndemand->pOndemandCallData)[nIndex];
    if(pOsakiOptionTbl->nStatus == 0 && pOsakiOptionTbl->szDesc == NULL) return  TRUE;

    switch(pOsakiOptionTbl->nStatus)
    {
        case IDX_REQ_PULSE:         // Current Pulse
            value = AsciiToInt(pszBuffer + nLength - 7, 7); XDEBUG(" OSAKI   Power : %ld KWh\r\n", value);
            pOndemand->nCurPulse = (UINT) value;
            break;
        case IDX_REQ_RELAY_STATUS:         // Current Pulse
            value = AsciiToInt(pszBuffer + 12, 2); XDEBUG(" Relay : %ld\r\n", value);
            switch((BYTE)value) {
                case 0x00: pOndemand->nEnergyLevel = ENERGY_LEVEL_MAX; break;
                case 0x01: pOndemand->nEnergyLevel = ENERGY_LEVEL_MIN; break;
                default: pOndemand->nEnergyLevel = ENERGY_LEVEL_UNDEFINED; break;
            }

            /** Energy Level Update */
            m_pEndDeviceList->SetEnergyLevel(&pOndemand->id, pOndemand->nEnergyLevel, 0);
            XDEBUG(" ENERGY LEVEL : %d\r\n", pOndemand->nEnergyLevel);

            break;
    }

    // Event signal
    m_Locker.Lock();
    pOndemand->bReply = TRUE;
    pOndemand->nIndex ++;
    m_Locker.Unlock();
    SetEvent(pOndemand->hEvent);

    return CIec21DataStream::OnDataFrame(pStream, pszBuffer, nLength, pCallData);
}
