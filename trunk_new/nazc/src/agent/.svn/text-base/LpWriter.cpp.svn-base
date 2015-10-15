#include "common.h"
#include "AgentService.h"
#include "MeterFileWriter.h"
#include "LpWriter.h"
#include "Variable.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CLpWriter Data Definitions
//////////////////////////////////////////////////////////////////////

#define	TEMPDIR	"/tmp/data"

CLpWriter   *m_pLpWriter = NULL;

//////////////////////////////////////////////////////////////////////
// CLpWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLpWriter::CLpWriter()
{
	m_pLpWriter = this;
}

CLpWriter::~CLpWriter()
{
}

//////////////////////////////////////////////////////////////////////
// CLpWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CLpWriter::Initialize()
{
	if (!CTimeoutThread::StartupThread(2))
        return FALSE;

	return TRUE;
}

void CLpWriter::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

//////////////////////////////////////////////////////////////////////
// CLpWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CLpWriter::ClearData(EUI64 *id)
{
	char	szFileName[128];
	char	szId[17];

    eui64toa(id, szId);
	sprintf(szFileName, "/tmp/data/%s", szId);

    unlink(szFileName);
    return TRUE;
}

BOOL CLpWriter::AddData(EUI64 *id, BYTE *pszData, int nLength, BOOL isLast, ENDIENTRY * pEntry,
        CMeterWriter *pMeterWriter, LpWriterCallback callback)
{
	LPWRITEITEM	*pItem;

	if (!pszData || (nLength <= 0))
		return FALSE;

	pItem = (LPWRITEITEM *)MALLOC(sizeof(LPWRITEITEM));
	if (pItem == NULL)
		return FALSE;

	memset(pItem, 0, sizeof(LPWRITEITEM));
	memcpy(&pItem->id, id, sizeof(EUI64));
	pItem->nSize = nLength;
    pItem->isLast = isLast;
    pItem->pEntry = pEntry;
    pItem->pCallback = callback;
    pItem->pMeterWriter = pMeterWriter;
	pItem->pData = (BYTE *)MALLOC(pItem->nSize);
	if (pItem->pData == NULL)
	{
		FREE(pItem);
		return FALSE;
	}
    memcpy(pItem->pData, pszData, nLength);

	m_Locker.Lock();
	pItem->nPosition = (int)m_List.AddTail(pItem);
	m_Locker.Unlock();

	ActiveThread();
	return TRUE;
}

BOOL CLpWriter::WriteLpData(LPWRITEITEM *pItem)
{
	char	szFileName[128];
	char	szId[17];
	FILE	*fp;
    int     dataLen=0;
    BYTE *  buffer = NULL;

    /** TODO 성능 향상을 위해 1 Frame의 경우 File IO를 하지 않고 Meter Writer를
      * 사용할 수 있도록 해야 한다.
      */
    eui64toa(&pItem->id, szId);
	sprintf(szFileName, "/tmp/data/%s", szId);
    
	XDEBUG(" Write Lp Data(%s:%d)\xd\xa", szId, pItem->nSize);

	fp = fopen(szFileName, "ab");
	if (fp != NULL)
	{
		fwrite(pItem->pData, pItem->nSize, 1, fp);
		fclose(fp);
        if(pItem->isLast) {
            if(ReadLpData(&pItem->id, &buffer, &dataLen)) {
                /// Pre Callback
                if(pItem->pCallback != NULL) {
                    pItem->pCallback(&pItem->id, buffer, dataLen);
                }
	            // 검침데이터를 저장한다.
                if(pItem->pMeterWriter != NULL) {
	                pItem->pMeterWriter->AddData(pItem->pEntry, &pItem->id, buffer, dataLen); 
                }
                FREE(buffer);
                m_pLpWriter->ClearData(&pItem->id);
            }else {
                if(buffer != NULL) FREE(buffer);
                m_pLpWriter->ClearData(&pItem->id);
                return FALSE;
            }
        }
        return TRUE;
	}
    return FALSE;
}

BOOL CLpWriter::ReadLpData(EUI64 * id, BYTE ** data, int * len)
{
    struct  stat file_info;
	char	szFileName[128];
	char	szId[17];
	FILE	*fp;

    eui64toa(id, szId);
	sprintf(szFileName, "/tmp/data/%s", szId);

    *data = NULL;
    *len = 0;

    if(stat(szFileName, &file_info)) return FALSE;

    *len = file_info.st_size;
    *data = (BYTE *) MALLOC(*len);
    if(*data == NULL) return FALSE;
    
	XDEBUG(" Read Lp Data(%s:%d)\xd\xa", szId, *len);

	fp = fopen(szFileName, "rb");
	if (fp != NULL)
	{
		fread(*data, *len, 1, fp);
		fclose(fp);
        return TRUE;
	}
    return FALSE;
}


//////////////////////////////////////////////////////////////////////
// CLpWriter Thread Override 
//////////////////////////////////////////////////////////////////////

BOOL CLpWriter::OnActiveThread()
{
	LPWRITEITEM	*pItem;

	for(;!IsThreadExitPending();)
	{
		m_Locker.Lock();
		pItem = m_List.GetHead();
		if (pItem) m_List.RemoveAt((POSITION)pItem->nPosition); 
		m_Locker.Unlock();
		
		if (pItem == NULL)
			break;

		WriteLpData(pItem);

		FREE(pItem->pData);
		FREE(pItem);
	}
	return TRUE;
}

BOOL CLpWriter::OnTimeoutThread()
{
	return OnActiveThread();
}

