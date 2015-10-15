#include "common.h"
#include "AgentService.h"
#include "MeterDbWriter.h"
#include "MeterUploader.h"
#include "SystemMonitor.h"
#include "EndDeviceList.h"
#include "Variable.h"
#include "MeterParserFactory.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "rep/MeteringHelper.h"
#include "rep/LogHelper.h"

//////////////////////////////////////////////////////////////////////
// CMeterDbWriter Data Definitions
//////////////////////////////////////////////////////////////////////

CMeterDbWriter   *m_pMeterDbWriter = NULL;

extern char 	m_szFepServer[];
extern int	 	m_nFepPort;

//////////////////////////////////////////////////////////////////////
// CMeterDbWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeterDbWriter::CMeterDbWriter()
{
	m_pMeterDbWriter = this;
}

CMeterDbWriter::~CMeterDbWriter()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterDbWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CMeterDbWriter::AddData(ENDIENTRY *pEndDevice, EUI64 *id, BYTE *pszData, int nLength)
{
	METERWRITEITEM	*pItem;

	if (!pEndDevice || !pszData || (nLength <= 0))
		return FALSE;

	pItem = (METERWRITEITEM *)MALLOC(sizeof(METERWRITEITEM));
	if (pItem == NULL)
		return FALSE;

	memset(pItem, 0, sizeof(METERWRITEITEM));
	memcpy(&pItem->id, id, sizeof(EUI64));
	pItem->nSize = nLength;
	pItem->pData = MALLOC(pItem->nSize);
	if (pItem->pData == NULL)
	{
		FREE(pItem);
		return FALSE;
	}

    memcpy(pItem->pData, pszData, pItem->nSize);

	m_Locker.Lock();
	pItem->nPosition = (int)m_List.AddTail(pItem);
	m_Locker.Unlock();

	ActiveThread();
	return TRUE;
}

void CMeterDbWriter::WriteMeteringData(METERWRITEITEM *pItem)
{
    ENDI_PUSH_HEADER *pHeader;
    ENDI_PUSH_EVT_HEADER *pEvtHeader;
    ENDI_PUSH_LPINFO *pLpInfo;
    ENDI_PUSH_BASEPULSE *pBasePulse;
    ENDI_PUSH_LP *pLp;
	CMeterParser *pParser;
    ORDER order = ORDER_MSB;
    TIMESTAMP meterTime, baseDate;

	char	szId[17];
    BYTE    nFrameType;
    TIMESTAMP nowStamp;

    memset(szId, 0, sizeof(szId));
    EUI64ToStr(&pItem->id, szId);
    GetTimestamp(&nowStamp, NULL);

    XDEBUG("==========================================\r\n");
	XDEBUG(" Write (%s:%d): Database\xd\xa", szId, pItem->nSize);
    XDEBUG("==========================================\r\n");
    XDUMP((const char *)pItem->pData, pItem->nSize);
    XDEBUG("==========================================\r\n");

    pHeader = (ENDI_PUSH_HEADER *)pItem->pData;
    pEvtHeader = (ENDI_PUSH_EVT_HEADER *)pItem->pData;
    nFrameType = (pHeader->frameType >> 6)&0x03;
    pParser = m_pMeterParserFactory->SelectParser(&pItem->id);
    order = pHeader->frameType & 0x20 ? ORDER_MSB : ORDER_LSB;

    switch(nFrameType)
    {
        case 0:
            {
            METERINGINS stMeteringIns;
            int     nTimeGap=0;
            int     nChCnt=0;
            int     nCpLen=0;
            int     nLpLen=0;
            //unsigned int memoryCheckPoint;

            //CheckPoint(&memoryCheckPoint);
            XDEBUG(" Push Metering: METERING\r\n");
            memset(&stMeteringIns, 0, sizeof(METERINGINS));
            memset(&meterTime, 0, sizeof(meterTime));
            memset(&baseDate, 0, sizeof(baseDate));
            Timestamp6ToTimestamp(&pHeader->currentTime, &meterTime);
            nTimeGap = GetTimestampInterval(&nowStamp, &meterTime);
            nCpLen = (pHeader->dataFormatLength>>4) & 0x0F;
            nLpLen = pHeader->dataFormatLength & 0x0F;
            nChCnt = pHeader->channelCount;
            pLpInfo = (ENDI_PUSH_LPINFO*)((char *)pItem->pData +
                        + sizeof(ENDI_PUSH_HEADER) + (nChCnt * nCpLen));
            pBasePulse = (ENDI_PUSH_BASEPULSE *)pLpInfo->lpData;
            Timestamp5ToTimestamp(&pBasePulse->baseTime, &baseDate);
            pLp = (ENDI_PUSH_LP*)((char *)pItem->pData + 
                        + sizeof(ENDI_PUSH_HEADER) + (nChCnt * nCpLen)
                        + sizeof(ENDI_PUSH_LPINFO) 
                        + sizeof(ENDI_PUSH_BASEPULSE) + (nChCnt * nCpLen));

            if(nChCnt > 0) {
                if(pParser != NULL) {
                    pParser->UpdateLastPulse(order, nCpLen, nChCnt, pHeader->currentPulse, &stMeteringIns);
                    pParser->UpdateBasePulse(order, nCpLen, nChCnt, pBasePulse->basePulse, &stMeteringIns);
                    pParser->UpdateLoadProfile(order, nLpLen, nChCnt, pLpInfo->lpInterval, pLpInfo->lpCount, pLp, &stMeteringIns);
                }
            }

            m_pMeteringHelper->Update(&pItem->id, pHeader->portNum,
                    &meterTime, nTimeGap, &baseDate, nChCnt, pLpInfo->lpCount, &stMeteringIns);

            CMeteringHelper::FreeMeteringIns(&stMeteringIns);
            //Difference(memoryCheckPoint);
            }
            break;
        case 1:
            {
            LOGENTRYINS *pLogEntryIns = NULL;
            PUSH_EVENT *pEvt;
            int nCategoryCnt = pEvtHeader->categoryCount;
            int i,nEvtCnt=0,nSeek=0;
            char *szData = (char *)pEvtHeader->eventData;

            XDEBUG(" Push Metering: LOG\r\n");

            if(nCategoryCnt <= 0) break;

            Timestamp6ToTimestamp(&pEvtHeader->currentTime, &meterTime);
            for(i=0;i<nCategoryCnt;i++)
            {    
                pEvt = (PUSH_EVENT *)(szData + nSeek);      
                nSeek += sizeof(PUSH_EVENT) + (pEvt->eventLength * pEvt->eventCount);
                nEvtCnt += pEvt->eventCount;
            }

            if(pParser != NULL){
                pLogEntryIns = (LOGENTRYINS *)MALLOC(sizeof(LOGENTRYINS) * nEvtCnt);
                memset(pLogEntryIns, 0, sizeof(LOGENTRYINS) * nEvtCnt);

                pParser->UpdateLog(order, nCategoryCnt, &meterTime, pEvtHeader->eventData, pLogEntryIns);
            } 

            m_pLogHelper->Update(&pItem->id, pEvtHeader->portNum, nEvtCnt, pLogEntryIns);

            // Memory Free
            if(pLogEntryIns){
                for(i=0;i<nEvtCnt;i++)
                {
                    if(pLogEntryIns[i].szLog) FREE(pLogEntryIns[i].szLog);
                }
                FREE(pLogEntryIns);
            }

            }
            break;
        default:
            XDEBUG(ANSI_COLOR_RED " Undefined Frame Type: %d\r\n" ANSI_NORMAL, nFrameType);
            break;
    }
}

BOOL CMeterDbWriter::SendToServer(const char *pszFileName)
{
#if 0
	char	szCommand[256];

	sprintf(szCommand, "/app/sw/upload %s -n %d -p %d %s %s", 
			m_nDebugLevel ? "" : "-d", IF4API_GetID(), m_nFepPort, m_szFepServer, pszFileName);	
	SystemExec(szCommand);
	unlink(TEMPFILE);
#endif
	return TRUE;
}

void CMeterDbWriter::SendMeteringData(METERWRITEITEM *pItem)
{
#if 0
	METERLPENTRY	*pData;
	int				nLength;
	FILE			*fp;

	pData = pItem->pData;
	nLength = sizeof(METERLPENTRY) + LittleToHostShort(pData->mlpLength);

	fp = fopen(TEMPFILE, "wb");
	if (fp != NULL)
	{
		fwrite(pData, nLength, 1, fp);
		fclose(fp);
		SendToServer(TEMPFILE);
	}
#endif
}


