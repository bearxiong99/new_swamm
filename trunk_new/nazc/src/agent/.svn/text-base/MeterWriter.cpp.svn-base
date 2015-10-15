#include "common.h"
#include "AgentService.h"
#include "MeterWriter.h"
#include "MeterUploader.h"
#include "SystemMonitor.h"
#include "EndDeviceList.h"
#include "Variable.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CMeterWriter Data Definitions
//////////////////////////////////////////////////////////////////////

extern BOOL		m_bEnableAutoUpload;
extern int		m_nUploadType;

//////////////////////////////////////////////////////////////////////
// CMeterWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeterWriter::CMeterWriter()
{
}

CMeterWriter::~CMeterWriter()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CMeterWriter::Initialize()
{
	if (!CTimeoutThread::StartupThread(2))
        return FALSE;

	return TRUE;
}

void CMeterWriter::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

//////////////////////////////////////////////////////////////////////
// CMeterWriter Thread Override 
//////////////////////////////////////////////////////////////////////

BOOL CMeterWriter::OnActiveThread()
{
	METERWRITEITEM	*pItem;

	// Write & send to server
	for(;!IsThreadExitPending();)
	{
		m_Locker.Lock();
		pItem = m_List.GetHead();
		if (pItem) m_List.RemoveAt((POSITION)pItem->nPosition); 
		m_Locker.Unlock();
		
		if (pItem == NULL)
			break;

		WriteMeteringData(pItem);

		if (m_bEnableAutoUpload && (m_nUploadType == UPLOAD_TYPE_IMMEDIATELY))
			SendMeteringData(pItem);

		if(pItem && pItem->pData) 
        {
            FREE(pItem->pData);
        }
		if(pItem)
        {
            FREE(pItem);
        }
	}
	return TRUE;
}

BOOL CMeterWriter::OnTimeoutThread()
{
	return OnActiveThread();
}

