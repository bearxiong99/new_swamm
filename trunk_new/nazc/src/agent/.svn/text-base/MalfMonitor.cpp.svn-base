#include "common.h"
#include "AgentService.h"
#include "MalfMonitor.h"
#include "StatusMonitor.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "SystemUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#define  MALF_GARBAGE_CLEANING_TYPE_DELETE_SENSOR       0x01

CMalfMonitor   *m_pMalfMonitor = NULL;

extern BOOL		m_bEnableMemoryCheck;
extern BOOL		m_bEnableFlashCheck;
extern BOOL		m_bEnableGarbageCleaning;
extern BYTE		m_nSensorCleaningThreshold;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMalfMonitor::CMalfMonitor()
{
	m_pMalfMonitor = this;
	m_nLastCleaningTick	= 0;
	m_nLastTransactionCleaningTick	= 0;
	m_bFaultMemory = FALSE;
	m_bFaultDisk  = FALSE;
	m_bUserRequest  = FALSE;
	m_bCleaningRequest  = FALSE;
	m_fMemoryRatio = 90;
	m_fDiskRatio   = 90;
}

CMalfMonitor::~CMalfMonitor()
{
}

//////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////

void CMalfMonitor::SetMemoryRatio(double fRatio)
{
	m_fMemoryRatio = fRatio;
}

void CMalfMonitor::SetDiskRatio(double fRatio)
{
	m_fDiskRatio = fRatio;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CMalfMonitor::Initialize()
{
	if (!CTimeoutThread::StartupThread(60))
        return FALSE;
	return TRUE;
}

void CMalfMonitor::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

void CMalfMonitor::Check()
{
    m_bUserRequest = TRUE;
    m_bCleaningRequest = TRUE;
	ActiveThread();
}

void CMalfMonitor::CheckExceptCleaning()
{
    m_bUserRequest = TRUE;
    m_bCleaningRequest = FALSE;
	ActiveThread();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL TargetSelector(ENDIENTRY *pEndDevice, void *tCurrent, void *nThreshold, void *pList)
{
    EUI64 * id;
    time_t current = *((time_t *)tCurrent);
    UINT threshold = *((UINT *)nThreshold);
    CLinkedList<EUI64*> *list = (CLinkedList<EUI64*> *)pList;

    if(!pEndDevice) return FALSE;

    if( (UINT)(current - pEndDevice->tmMetering) > threshold &&
        (UINT)(current - pEndDevice->tmEventLog) > threshold &&
        (UINT)(current - pEndDevice->tmRouteRecord) > threshold &&
        (UINT)(current - pEndDevice->tmRouteDevice) > threshold &&
        (UINT)(current - pEndDevice->tmSolarDevice) > threshold &&
        (UINT)(current - pEndDevice->tmInstall) > threshold)
    {
        id = (EUI64 *) MALLOC(sizeof(EUI64));
        memcpy(id, &pEndDevice->id, sizeof(EUI64));
        list->AddTail(id);
    }
    return TRUE;
}

BOOL CMalfMonitor::Monitoring()
{
	UINT	nTotal, nUse, nFree, nCache, nBuffer;
    UINT    nCurrentTick=0, nThreshold;
    time_t  current;
	double	fRate;
    BOOL    bUserRequest = m_bUserRequest;
    BOOL    bCleaningRequest = m_bCleaningRequest;

    m_bUserRequest = FALSE;
    m_bCleaningRequest = FALSE;

	if (m_bEnableMemoryCheck || bUserRequest)
	{
		GetMemoryInfo(&nTotal, &nUse, &nFree, &nCache, &nBuffer);
		fRate = ((double)nUse*(double)100) / (double)nTotal;
//		printf("Memory: %.3f %% Use: %.3f Free:%d bytes\xd\xa", fRate, m_fMemoryRatio, nFree);

		if (m_bFaultMemory)
		{
			if (m_fMemoryRatio > fRate)
			{
			   m_pStatusMonitor->SetState(FALSE, GPIONOTIFY_MEMORY);
			   malfMemoryErrorRestoreEvent(nTotal, nFree, nUse, nCache, nBuffer);
			   m_bFaultMemory = FALSE;
			}
		}
		else
		{
			if (m_fMemoryRatio <= fRate)
			{
			   m_pStatusMonitor->SetState(TRUE, GPIONOTIFY_MEMORY);
			   malfMemoryErrorEvent(nTotal, nFree, nUse, nCache, nBuffer);
			   m_bFaultMemory = TRUE;
			}
		}
	}

	if (m_bEnableFlashCheck || bUserRequest)
	{
#if defined(__TI_AM335X__)
	    GetFlashInfo("ubi0:filestore", &nTotal, &nUse, &nFree);
#else
        GetFlashInfo("/dev/mtdblock2", &nTotal, &nUse, &nFree);
#endif
		fRate = ((double)nUse*(double)100) / (double)nTotal;
//		printf("Flash Memory: %.3f %% Use: %.3f Free: %d bytes\xd\xa", fRate, m_fDiskRatio, nFree);
		if (m_bFaultDisk)
		{
			if (m_fDiskRatio > fRate)
			{
			   m_pStatusMonitor->SetState(FALSE, GPIONOTIFY_FLASH);
			   malfDiskErrorRestoreEvent(nTotal, nFree, nUse);
			   m_bFaultDisk = FALSE;
			}
		}
		else
		{
			if (m_fDiskRatio <= fRate)
			{
			   m_pStatusMonitor->SetState(TRUE, GPIONOTIFY_FLASH);
			   malfDiskErrorEvent(nTotal, nFree, nUse);
			   m_bFaultDisk = TRUE;
			}
		}
	}

    if ((m_bEnableGarbageCleaning || bCleaningRequest) && m_nSensorCleaningThreshold > 0) {
        /** 사용자 요청이 아닐 경우에 Metering 등 중요 서비스가 진행 중일 때는 정리 작업을 하지 않는다 */
        if(bCleaningRequest || !IsBusyCoreService()) {
            nCurrentTick = GetSysUpTime();

            /* 1. 사용자 요청
            * 2. 최초 검사 이거나 마지막 검사에서 12시간이 지났을 경우 
            */
            if(bCleaningRequest || !m_nLastCleaningTick || (nCurrentTick - m_nLastCleaningTick) > 4320) {
                CLinkedList<EUI64 *> idList;
                EUI64 * id;

                time(&current);
                nThreshold = 60 * 60 * 24 * m_nSensorCleaningThreshold;

                m_pEndDeviceList->EnumEndDevice(TargetSelector, &current, &nThreshold, &idList);
                m_nLastCleaningTick = nCurrentTick;

                XDEBUG("GarbageCleaning Activate Threshold=%d Target=%d\r\n", m_nSensorCleaningThreshold, idList.GetCount());

                while((id=idList.GetHead()) != NULL) {
                    malfGarbageCleaningEvent(MALF_GARBAGE_CLEANING_TYPE_DELETE_SENSOR, (BYTE *)id, sizeof(EUI64));
                    m_pEndDeviceList->DeleteEndDeviceByID(id);
                    FREE(id); idList.RemoveAt(idList.GetHeadPosition());
                }
            }
        }
    }

    nCurrentTick = GetSysUpTime();
    /** Issue #1502 : Transaction 정리 코드 추가 **/
    if(bUserRequest || (nCurrentTick - m_nLastTransactionCleaningTick) > 86400) {
        m_nLastTransactionCleaningTick = nCurrentTick;
        XDEBUG("Transaction Integrity Check .........\r\n");
        m_pTransactionManager->IntegrityCheck();
    }
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CMalfMonitor::OnActiveThread()
{
	return Monitoring();
}

BOOL CMalfMonitor::OnTimeoutThread()
{
	return Monitoring();
}

