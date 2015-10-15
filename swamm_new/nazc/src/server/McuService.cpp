#include "common.h"
#include "McuService.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "DebugUtil.h"
#include "CommonUtil.h"
#include "if4frame.h"
#include "codeUtility.h"

//////////////////////////////////////////////////////////////////////
// CMcuService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcuService	*m_pService = NULL;
int  		m_nServerPort = IF4_DEFAULT_PORT;

void OnDataFile(char *pszAddress, UINT nSourceId, BYTE nType, char *pszName, BYTE *pData, int nLength);
void OnData(char *pszAddress, UINT nSourceId, BYTE *pData, int nLength);
void OnEvent(char *pszAddress, UINT nSourceId, BYTE *pEvent, int nLength);
void OnAlarm(char *pszAddress, UINT nSourceId, BYTE *pAlarm, int nLength);
void OnTypeR(char *pszAddress, UINT nSourceId, BYTE *pTypeR, int nLength);

//////////////////////////////////////////////////////////////////////
// CMcuService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcuService::CMcuService()
{
	m_pService 	= this;
}

CMcuService::~CMcuService()
{
}

//////////////////////////////////////////////////////////////////////
// CMcuService Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CMcuService::OnStartService()
{
	// Initialize Variable
	LoadVariable();

	// Command Control Service Initialize
	IF4API_Initialize(m_nServerPort, m_CommandHandler);
	IF4API_SetUserCallback(OnDataFile, OnData, OnEvent, OnAlarm, OnTypeR);
	return TRUE;
}

void CMcuService::OnStopService()
{
	IF4API_Destroy();
	VARAPI_Destroy();
}

BOOL CMcuService::OnMessage(int nMessage, void *pParam)
{
	XDEBUG("OnMessage(nMessage=%d, pParam=%p)\xd\xa", nMessage, pParam);

	switch(nMessage) {
	  case GWMSG_SHUTDOWN :
		   ImmediatelyShutdown();
		   break;

	  case GWMSG_RESET :
	  case GWMSG_FACTORYDEFAULT :
	  case GWMSG_FIRMWARE_UPDATE :
		   ImmediatelyShutdown();
		   break;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

void CMcuService::LoadVariable()
{
	// Initialize Variable
	VARAPI_Initialize(NULL, m_Root_node, TRUE);
}

void CMcuService::ImmediatelyShutdown()
{
	IF4API_Destroy();
	VARAPI_Destroy();
	exit(0);
}

//////////////////////////////////////////////////////////////////////
// Callback Functions
//////////////////////////////////////////////////////////////////////

void OnDataFile(char *pszAddress, UINT nSourceId, BYTE nType, char *pszName, BYTE *pData, int nLength)
{
	XDEBUG("OnDataFile(Address=%s, MCUID=%d, Type=%d, Name=%s, Length=%d)\r\n",
			pszAddress, nSourceId, nType, pszName, nLength);
}

void OnData(char *pszAddress, UINT nSourceId, BYTE *pData, int nLength)
{
	XDEBUG("OnData %s %d Bytes\r\n", pszAddress, nLength);
}

void OnEvent(char *pszAddress, UINT nSourceId, BYTE *pEvent, int nLength)
{
	XDEBUG("OnEvent %s %d Bytesr\n", pszAddress, nLength);
}

void OnAlarm(char *pszAddress, UINT nSourceId, BYTE *pAlarm, int nLength)
{
	XDEBUG("OnAlarm %s %d Bytes\n", pszAddress, nLength);
}

void _dumpInventory(const char *data, int nLength)
{
    TYPER_INVENTORY * pInventory = (TYPER_INVENTORY *)data;
    int nSeek=0, len;
    EUI64 id;
    char szStr[32];
    unsigned long long lv;
    WORD hw,sw;
    TIMESTAMP timestamp;

    memset(szStr, 0, sizeof(szStr));

    memcpy(&id, pInventory->modemId.id, 8);
    EUI64ToStr(&id, szStr); nSeek += 4 + 2 + 8;
    XDEBUG( " ID %d %s port %d\r\n", BigToHostInt(pInventory->shortId), szStr, data[nSeek]); nSeek++;

    nSeek++;    // Meter ID Type
    memset(szStr, 0, sizeof(szStr));
    len = data[nSeek++]; 
    if(len > 0) {
        memcpy(szStr, data+nSeek, len); nSeek += len;
    }
    lv = BigStreamToHostLong((BYTE *)(data+nSeek), 4); nSeek += 4;
    XDEBUG("   + %s parser %lu svc %d vendor %d\r\n", szStr, lv, data[nSeek], data[nSeek+1]); nSeek += 2;
    len = data[nSeek]; nSeek++;
    if(len > 0) {
        memcpy(szStr, data+nSeek, len); nSeek += len;
    }
    hw = (WORD)BigStreamToHostLong((BYTE *)(data+nSeek), 2); nSeek += 2;
    sw = (WORD)BigStreamToHostLong((BYTE *)(data+nSeek), 2); nSeek += 2;
    XDEBUG("   + %s hw 0x%04X sw 0x%04X build %d\r\n", szStr, hw, sw, data[nSeek]); nSeek++;
    memcpy(&timestamp, data+nSeek, sizeof(TIMESTAMP)); nSeek += sizeof(TIMESTAMP);
    XDEBUG("   + %04d/%02d/%02d %02d:%02d:%02d\r\n", 
            BigToHostShort(timestamp.year),
            timestamp.mon,
            timestamp.day,
            timestamp.hour,
            timestamp.min,
            timestamp.sec);
    XDEBUG("   + period %d\r\n", data[nSeek]); 
}

void _dumpChannelConfigure(const char *data, int nLength)
{
    TYPER_CHANNEL * pChannel = (TYPER_CHANNEL *)data;
    TYPER_CHANNEL_CONFIGURE *pConfigure;
    EUI64 id;
    char szStr[32];
    int i, nSeek=sizeof(TYPER_CHANNEL);
    BYTE nChCnt;

    memset(szStr, 0, sizeof(szStr));
    memcpy(&id, pChannel->modemId.id, 8);
    EUI64ToStr(&id, szStr);                 nSeek += 8; // EUI64

    nChCnt = data[nSeek];                   nSeek++;    // channel count

    XDEBUG( " ID %d %s ChCnt %d\r\n", BigToHostInt(pChannel->shortId), szStr, nChCnt);
    for(i=0;i<nChCnt;i++)
    {
        pConfigure = (TYPER_CHANNEL_CONFIGURE *)(data + nSeek);

        XDEBUG("   idx %d sigExp 0x%02X val 0x%02X obj 0x%02X\r\n",
                pConfigure->index,
                pConfigure->sigExp,
                pConfigure->valueType,
                pConfigure->objectType);
        XDEBUG("    + %s\r\n", GetUnitStr(pConfigure->unit));
        XDEBUG("    + %s\r\n", GetChannelStr(pConfigure->channelType));

        nSeek += sizeof(TYPER_CHANNEL_CONFIGURE);
    }
}

void _dumpLoadProfile(const char *data, int nLength)
{
    TYPER_METERING * pMetering = (TYPER_METERING *)data;
    TYPER_BASEPULSE *pBasePulse;
    TYPER_BASEPULSE_VALUE *pBpValue;
    TYPER_LP_VALUE *pLpValue;
    TYPER_LOG_VALUE *pLogValue;
    int i, j, nSeek=sizeof(TYPER_METERING);
    EUI64 id;
    char szStr[32];
    BYTE nLpCnt=0, nLogCnt=0;
    BYTE nChCnt, nBpCnt;
    UINT *pValue;

    memset(szStr, 0, sizeof(szStr));
    memcpy(&id, pMetering->modemId.id, 8);
    EUI64ToStr(&id, szStr);                 nSeek += 8; // EUI64

    nChCnt = data[nSeek];   nSeek++;    // channel count
    nBpCnt = data[nSeek];   nSeek++;    // channel count

    XDEBUG( " ID %d %s ChCnt %d BpCnt %d\r\n", 
            BigToHostInt(pMetering->shortId), szStr, nChCnt, nBpCnt);
    for(i=0;i<nBpCnt;i++)
    {
        pBasePulse = (TYPER_BASEPULSE *)(data + nSeek); nSeek += sizeof(TYPER_BASEPULSE);

        XDEBUG("   BP [%04d/%02d/%02d] Last[%04d/%02d/%02d %02d:%02d:00]\r\n",
                pBasePulse->baseTime.year + 2000,
                pBasePulse->baseTime.mon,
                pBasePulse->baseTime.day,
                pBasePulse->lastTime.year + 2000,
                pBasePulse->lastTime.mon,
                pBasePulse->lastTime.day,
                pBasePulse->lastTime.hour,
                pBasePulse->lastTime.min);

        for(j=0;j<nChCnt;j++)
        {
            pBpValue = (TYPER_BASEPULSE_VALUE *)(data + nSeek); nSeek += sizeof(TYPER_BASEPULSE_VALUE);
            XDEBUG("    [%d] Base %llu Last %llu\r\n", j, 
                    BigToHostLong(pBpValue->baseValue),
                    BigToHostLong(pBpValue->lastValue));
        }
    }

    nLpCnt = data[nSeek]; nSeek++;

    for(i=0;i<nLpCnt;i++)
    {
        pLpValue = (TYPER_LP_VALUE *)(data + nSeek); nSeek += sizeof(TYPER_LP_VALUE);

        XDEBUG("   Stamp [%04d/%02d/%02d %02d:%02d:00]\r\n",
                pLpValue->time.year + 2000,
                pLpValue->time.mon,
                pLpValue->time.day,
                pLpValue->time.hour,
                pLpValue->time.min);

        for(j=0;j<nChCnt;j++)
        {
            pValue = (UINT *)(data + nSeek); nSeek += 4;
            XDEBUG("    [%d] LP %u\r\n", j, BigToHostInt(*pValue));
        }
    }

    nLogCnt = data[nSeek]; nSeek++;
    for(i=0;i<nLogCnt;i++)
    {
        pLogValue = (TYPER_LOG_VALUE *)(data + nSeek); nSeek += sizeof(TYPER_LOG_VALUE);

        XDEBUG("   category 0x%02X len %d [%04d/%02d/%02d %02d:%02d:00]\r\n",
                pLogValue->category,
                pLogValue->length,
                pLogValue->time.year + 2000,
                pLogValue->time.mon,
                pLogValue->time.day,
                pLogValue->time.hour,
                pLogValue->time.min);
        nSeek += pLogValue->length;
    }
}

void OnTypeR(char *pszAddress, UINT nSourceId, BYTE *pTypeR, int nLength)
{
    IF4_TYPER_FRAME *pFrame = (IF4_TYPER_FRAME *)pTypeR;
    IF4_TYPER_HEADER *pHeader;
    int i, nSeek=0;

	XDEBUG("OnTypeR %s %d Bytes\n", pszAddress, nLength);
    //XDUMP((const char *)pTypeR, nLength, TRUE);

    for(i=0;i<pFrame->dh.cnt; i++) {
        pHeader = (IF4_TYPER_HEADER *)(pFrame->data + nSeek);
        XDEBUG("Frame Type 0x%02X %d Bytes\r\n", pHeader->type, BigToHostShort(pHeader->length));
        XDUMP((const char *)pHeader->payload, BigToHostShort(pHeader->length));
        switch(pHeader->type)
        {
            case 0x01:
                _dumpInventory((const char *)pHeader->payload, BigToHostShort(pHeader->length));
                break;
            case 0x02:
                _dumpChannelConfigure((const char *)pHeader->payload, BigToHostShort(pHeader->length));
                break;
            case 0x03:
                _dumpLoadProfile((const char *)pHeader->payload, BigToHostShort(pHeader->length));
                break;
        }
        nSeek += BigToHostShort(pHeader->length) + sizeof(IF4_TYPER_HEADER);
    }
}

