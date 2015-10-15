#include "common.h"
#include "PowerOutage.h"
#include "EventInterface.h"
#include "codiapi.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CPowerOutage Data Definitions
//////////////////////////////////////////////////////////////////////

CPowerOutage   	*m_pPowerOutage = NULL;

extern UINT		m_nEventSendDelay;

//////////////////////////////////////////////////////////////////////
// CPowerOutage Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPowerOutage::CPowerOutage()
{
	m_pPowerOutage = this;

	for(int i=0; i<MAX_OUTAGE_EVENT; i++)
	{
		memset(&m_arOutageList[i], 0, sizeof(OUTAGEITEM));
		m_arOutageList[i].pChunk = new CChunk(sizeof(EUI64)*100);
	}	
}

CPowerOutage::~CPowerOutage()
{
	for(int i=0; i<MAX_OUTAGE_EVENT; i++)
	{
		if (m_arOutageList[i].pChunk != NULL)
			delete m_arOutageList[i].pChunk;
		m_arOutageList[i].pChunk = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////

BOOL CPowerOutage::IsActiveTimer(BYTE type)
{
    if(type < MAX_OUTAGE_EVENT) {
	    return m_arOutageList[type].bActive;
    } else {
	    XDEBUG("Invalid Outage Event Type=%d\xd\xa", type);
        return FALSE;
    }
}

void CPowerOutage::SetActiveTimer(BYTE type, BOOL bActive)
{
    if(type >= MAX_OUTAGE_EVENT) return;

	m_ItemLocker.Lock();
	m_arOutageList[type].bActive = bActive;
	time(&m_arOutageList[type].tmLastActive);
	if (bActive) m_bActiveOutage = TRUE;
	m_ItemLocker.Unlock();
}

//////////////////////////////////////////////////////////////////////
// Initialize/Destroy
//////////////////////////////////////////////////////////////////////

BOOL CPowerOutage::Initialize()
{
	if (!CTimeoutThread::StartupThread(2))
		return FALSE;

	return TRUE;
}

void CPowerOutage::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

//////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////

BOOL CPowerOutage::Add(EUI64 *id, GMTTIMESTAMP *gmt, BYTE type, BYTE *status)
{
	switch(type) {
	  case ENDI_EVENT_POWER_OUTAGE :
	  case ENDI_EVENT_POWER_RECOVER :
		   if (IsActiveTimer(type))
		   {
			   // 타이머가 동작중이면 대용량 이벤트로 쌓기 시작한다.
			   AddTimeoutItem(id, gmt, type);

		   }
		   else
		   {
			   // 첫 이벤트 즉시 전송한다.
			   sensorAlarmEvent(id, gmt, type, (BYTE *)status);
		   }

		   // 타이머를 동작하도록 한다.
				   	   		SetActiveTimer(type, TRUE);
				   	   		ActiveThread();
		break;

	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Member functions
//////////////////////////////////////////////////////////////////////

BOOL CPowerOutage::AddTimeoutItem(EUI64 *id, GMTTIMESTAMP *gmt, BYTE type)
{
	m_ItemLocker.Lock();
	if (m_arOutageList[type].nCount <= 0)
		memcpy(&m_arOutageList[type].gmt, gmt, sizeof(GMTTIMESTAMP));
	m_arOutageList[type].pChunk->Add((char *)id, sizeof(EUI64));
	time(&m_arOutageList[type].tmLastActive);
	m_arOutageList[type].nCount++;
	m_ItemLocker.Unlock();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TimeoutThread override functions
//////////////////////////////////////////////////////////////////////

BOOL CPowerOutage::OnActiveThread()
{
	time_t	now;
	BOOL	bAllDone = TRUE;
	UINT	nElapse;
	int		i;

	// Active Flag를 Clear 한다.
	// 동작중에 다시 설정 될수 있으므로 실행전에 Clear를 해둔다.

	m_ItemLocker.Lock();
	m_bActiveOutage = FALSE;
	m_ItemLocker.Unlock();

	for(;;)
	{	
		bAllDone = TRUE;
		time(&now);

		for(i=0; i<MAX_OUTAGE_EVENT; i++)
		{
			if (m_arOutageList[i].bActive)
			{
				nElapse = (UINT)(now - m_arOutageList[i].tmLastActive);
				if (nElapse	>= m_nEventSendDelay)
				{
					m_ItemLocker.Lock();

					if (m_arOutageList[i].nCount > 0)
					{
	                    XDEBUG("Mass Power Outage Type=[%d], Count=[%d]\xd\xa", i, m_arOutageList[i].nCount);
						sensorPowerOutage(&m_arOutageList[i].gmt,
								(EUI64 *)m_arOutageList[i].pChunk->GetBuffer(),
								m_arOutageList[i].nCount, i);
					}

					m_arOutageList[i].pChunk->Flush();
					m_arOutageList[i].nCount = 0;
				    m_arOutageList[i].tmLastActive = 0;
					m_arOutageList[i].bActive = FALSE;

					m_ItemLocker.Unlock();
				}
				else
				{
					bAllDone = FALSE;
				}
			}
		}

        /*-- bAllDone 이 이상하다. 위 코드는 bActive 후 bAllDone되는 자료가
         *-- 하나라도 나오면 되는거 같다.
         *-- 확인해 보자
         --*/
		// 모든 Active가 클리어 되었으면 종료한다.
		if (bAllDone)
			break;

		usleep(1000000);
	}
	return TRUE;
}

BOOL CPowerOutage::OnTimeoutThread()
{
	// Power Outage가 대기중인 것이 있는가?
	if (!m_bActiveOutage)
		return TRUE;

	return OnActiveThread();
}

