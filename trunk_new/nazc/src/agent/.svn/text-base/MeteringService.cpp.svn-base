#include "common.h"
#include "codiapi.h"
#include "MeterParser.h"
#include "MeterParserFactory.h"
#include "MeteringService.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "Variable.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeteringService::CMeteringService()
{
}

CMeteringService::~CMeteringService()
{
}

int CMeteringService::WriteData(const char *pszFileName, EUI64 *id, char *pszBuffer, int nLength)
{
	CMeterParser	*parser;
	ENDIENTRY		*pEndDevice;
	METERLPENTRY    data;
	FILE			*fp;

	if (!nLength)
		return -1;

	pEndDevice = m_pEndDeviceList->GetEndDeviceByID(id);
	if (pEndDevice == NULL)
		return -1;

	// Unknown는 저장하지 않는다.
	parser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
	if (!parser || strcasecmp(parser->GetName(), "UNKNOWN") == 0)
		return 0;

	memset(&data, 0, sizeof(METERLPENTRY));
    memcpy(&data.mlpId, id, sizeof(EUI64));
    strncpy(data.mlpMid, pEndDevice->szSerial, sizeof(data.mlpMid));
	GetTimestamp(&data.mlpTime, NULL);

    /* Issue #1142 */
	data.mlpType = parser ? parser->GetType() : SENSOR_TYPE_UNKNOWN;
	data.mlpServiceType = parser ? parser->GetService() : SERVICE_TYPE_UNKNOWN;
    data.mlpVendor = pEndDevice->nVendor;

	data.mlpDataCount = 1;
	data.mlpLength  = HostToLittleShort(nLength + sizeof(TIMESTAMP));
	
	m_WriteLocker.Lock();
	fp = fopen(pszFileName, "a+");
	if (fp != NULL)
	{
		fwrite(&data, sizeof(METERLPENTRY), 1, fp);
		fwrite(pszBuffer, nLength, 1, fp);
		fclose(fp);
	}
	m_WriteLocker.Unlock();
	return (fp == NULL) ? -2 : 0;
}

