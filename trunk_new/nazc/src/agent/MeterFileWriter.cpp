#include "common.h"
#include "AgentService.h"
#include "MeterFileWriter.h"
#include "MeterUploader.h"
#include "SystemMonitor.h"
#include "EndDeviceList.h"
#include "MeterParserFactory.h"
#include "Variable.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "ShellCommand.h"

//////////////////////////////////////////////////////////////////////
// CMeterFileWriter Data Definitions
//////////////////////////////////////////////////////////////////////

#define	TEMPFILE	"/tmp/metering"

CMeterFileWriter   *m_pMeterFileWriter = NULL;

extern char 	m_szFepServer[];
extern int	 	m_nFepPort;

//////////////////////////////////////////////////////////////////////
// CMeterFileWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeterFileWriter::CMeterFileWriter()
{
	m_pMeterFileWriter = this;
}

CMeterFileWriter::~CMeterFileWriter()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterFileWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CMeterFileWriter::AddData(ENDIENTRY *pEndDevice, EUI64 *id, BYTE *pszData, int nLength)
{
	CMeterParser	*parser;
	METERWRITEITEM	*pItem;
	METERLPENTRY	*pSave;

	if (!pEndDevice || !pszData || (nLength <= 0))
		return FALSE;

	pItem = (METERWRITEITEM *)MALLOC(sizeof(METERWRITEITEM));
	if (pItem == NULL)
		return FALSE;

	memset(pItem, 0, sizeof(METERWRITEITEM));
	memcpy(&pItem->id, id, sizeof(EUI64));
	pItem->nSize = sizeof(METERLPENTRY) + nLength;
	pItem->pData = (METERLPENTRY *)MALLOC(pItem->nSize);
	if (pItem->pData == NULL)
	{
		FREE(pItem);
		return FALSE;
	}

	pSave = (METERLPENTRY *)pItem->pData;
	memcpy(&pSave->mlpId, id, sizeof(EUI64));
    strncpy(pSave->mlpMid, pEndDevice->szSerial, sizeof(pSave->mlpMid));

	parser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
	pSave->mlpType   = parser ? parser->GetType() : SENSOR_TYPE_UNKNOWN;
    /** Pulse 식 Meter들은 Service Type이 검침데이터에 의해 결정된다.
      * 따라서 기존에 parser의 기본 Service 정보를 쓰던 것을 EndDevice별 Service 정보를 쓰도록 수정한다.
      *
	  * pSave->mlpServiceType = parser ? parser->GetService() : SERVICE_TYPE_UNKNOWN;
      */
	pSave->mlpServiceType = pEndDevice->nServiceType;
    pSave->mlpVendor = pEndDevice->nVendor;
    pSave->mlpDataCount = 1;
    pSave->mlpLength  = HostToLittleShort(sizeof(TIMESTAMP) + nLength);
    GetTimestamp(&pSave->mlpTime, NULL);
    memcpy(pSave->mlpData, pszData, nLength);

	m_Locker.Lock();
	pItem->nPosition = (int)m_List.AddTail(pItem);
	m_Locker.Unlock();

	ActiveThread();
	return TRUE;
}

void CMeterFileWriter::WriteMeteringData(METERWRITEITEM *pItem)
{
	char	szFileName[128];
	char	szId[17];
	FILE	*fp;
    METERLPENTRY *pEntry = (METERLPENTRY *)pItem->pData;

	sprintf(szFileName, "/app/data/%04d%02d%02d%02d",
			pEntry->mlpTime.year,
			pEntry->mlpTime.mon,
			pEntry->mlpTime.day,
			pEntry->mlpTime.hour);

    eui64toa(&pItem->id, szId);
	XDEBUG(" Write (%s:%d): %s\xd\xa", szId, pItem->nSize, szFileName);

    while(m_pMeterUploader->SetBusyFile(szFileName)) usleep(500000);

	fp = fopen(szFileName, "a+b");
	if (fp != NULL)
	{
		fwrite(pItem->pData, pItem->nSize, 1, fp);
		fclose(fp);
    }
    m_pMeterUploader->ResetBusyFile(szFileName);
}

BOOL CMeterFileWriter::SendToServer(const char *pszFileName)
{
	char	szCommand[256];

	sprintf(szCommand, "/app/sw/upload %s -n %d -p %d %s %s", 
			m_nDebugLevel ? "" : "-d", IF4API_GetID(), m_nFepPort, m_szFepServer, pszFileName);	
	SystemExec(szCommand);
	unlink(TEMPFILE);
	return TRUE;
}

void CMeterFileWriter::SendMeteringData(METERWRITEITEM *pItem)
{
	METERLPENTRY	*pData;
	int				nLength;
	FILE			*fp;

	pData = (METERLPENTRY *)pItem->pData;
	nLength = sizeof(METERLPENTRY) + LittleToHostShort(pData->mlpLength);

	fp = fopen(TEMPFILE, "wb");
	if (fp != NULL)
	{
		fwrite(pData, nLength, 1, fp);
		fclose(fp);
		SendToServer(TEMPFILE);
	}
}


