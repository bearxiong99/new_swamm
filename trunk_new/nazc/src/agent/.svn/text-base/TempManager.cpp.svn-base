#include "common.h"
#include "TempManager.h"
#include "StatusMonitor.h"
#include "EventInterface.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CTempManager Data Definitions
//////////////////////////////////////////////////////////////////////

CTempManager   	*m_pTempManager = NULL;
extern int		m_nTempRangeMin;
extern int		m_nTempRangeMax;

//////////////////////////////////////////////////////////////////////
// CTempManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempManager::CTempManager()
{
	m_pTempManager	= this;
	m_bInstalled	= FALSE;
	m_nInterval		= 5;
	m_nLastTime		= 0;
	m_fCurrent		= 0;
	m_fMinRange		= -10;
	m_fMaxRange		= 0;
	m_bOutOfRange	= FALSE;

	// Setup Device Name
	strcpy(m_szDevice, TEMP_DEVICE);
}

CTempManager::~CTempManager()
{
}

//////////////////////////////////////////////////////////////////////
// CTempManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CTempManager::IsInstalled() const
{
	return m_bInstalled;
}

double CTempManager::GetValue() const
{
	return m_fCurrent;
}

int	CTempManager::GetInterval() const
{
	return m_nInterval;
}

void CTempManager::SetInterval(int nInterval)
{
	m_nInterval = nInterval;
}

//////////////////////////////////////////////////////////////////////
// CTempManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CTempManager::Initialize()
{
	double	fMin, fMax;

	if (!DetectSensor())
		return FALSE;

	fMin = m_nTempRangeMin / 10;
	fMax = m_nTempRangeMax / 10;
	SetRange(fMin, fMax);

	if (!CTimeoutThread::StartupThread(5))
		return FALSE;

	return TRUE;
}

void CTempManager::Destroy()
{
	if (!IsInstalled())
		return;

	CTimeoutThread::ShutdownThread();
}

BOOL CTempManager::Check()
{
	if (!IsInstalled())
		return FALSE;

	m_Locker.Lock();
	m_nLastTime = 0;
	m_Locker.Unlock();

	CTimeoutThread::ActiveThread();
	return TRUE;
}

BOOL CTempManager::DetectSensor()
{
	int		fd;

	m_Locker.Lock();
	fd = open(m_szDevice, O_RDWR | O_NOCTTY);
	if (fd > 0)
	{
		m_bInstalled = TRUE;
		close(fd);
	}
	m_Locker.Unlock();

	if (!m_bInstalled)
		return FALSE;

#ifdef DEBUG
	XDEBUG("TempManager: %s.\xd\xa", m_bInstalled ? "Installed" : "Not installed");
#endif	
	return TRUE;
}

BOOL CTempManager::GetRange(double *pMin, double *pMax)
{
	if (!pMin || !pMax)
		return FALSE;

	*pMin = m_fMinRange;
	*pMax = m_fMaxRange;
	return TRUE;
}

double CTempManager::GetMinRange()
{
	return m_fMinRange;
}

double CTempManager::GetMaxRange()
{
	return m_fMaxRange;
}

BOOL CTempManager::SetRange(double fMinRange, double fMaxRange)
{
	char	szBuffer[100];
	int		fd;

	if ((fMinRange < -55) || (fMaxRange > 125))
		return FALSE;

	if (fMinRange > fMaxRange)
		return FALSE;

	if (fMaxRange < 0)
		return FALSE;

	m_Locker.Lock();
	fd = open(m_szDevice, O_RDWR | O_NOCTTY);
	if (fd > 0)
	{
		m_fMinRange = fMinRange;
		m_fMaxRange = fMaxRange;
		sprintf(szBuffer, "%3.1f %3.1f", fMaxRange, fMinRange);
		write(fd, szBuffer, strlen(szBuffer));
		close(fd);
	}
	m_Locker.Unlock();
	return (fd < 0) ? TRUE : FALSE;
}

BOOL CTempManager::SetMinRange(double fMinRange)
{
	char	szBuffer[100];
	int		fd;

	if (fMinRange < -55)
		return FALSE;

	if (fMinRange > m_fMaxRange)
		return FALSE;

	m_Locker.Lock();
	fd = open(m_szDevice, O_RDWR | O_NOCTTY);
	if (fd > 0)
	{
		m_fMinRange = fMinRange;
		sprintf(szBuffer, "%3.1f %3.1f", m_fMaxRange, m_fMinRange);
		write(fd, szBuffer, strlen(szBuffer));
		close(fd);
	}
	m_Locker.Unlock();
	return (fd > 0) ? TRUE : FALSE;
}

BOOL CTempManager::SetMaxRange(double fMaxRange)
{
	char	szBuffer[100];
	int		fd;

	if (fMaxRange > 100)
		return FALSE;

	if (fMaxRange < 0)
		return FALSE;

	m_Locker.Lock();
	fd = open(m_szDevice, O_RDWR | O_NOCTTY);
	if (fd > 0)
	{
		m_fMaxRange = fMaxRange;
		sprintf(szBuffer, "%3.1f %3.1f", m_fMaxRange, m_fMinRange);
		write(fd, szBuffer, strlen(szBuffer));
		close(fd);
	}
	m_Locker.Unlock();
	return (fd > 0) ? TRUE : FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTempManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CTempManager::OnActiveThread()
{
	time_t	now;
	int		n, fd;
	float	min, max, cur;

	time(&now);
	if (abs(now-m_nLastTime) < m_nInterval)
		return TRUE;

	m_Locker.Lock();
	fd = open(m_szDevice, O_RDWR | O_NOCTTY);
	if (fd > 0)
	{
		n = read(fd, m_szBuffer, 100);
		if (n > 0)
		{
			m_szBuffer[n] = '\0';
			sscanf(m_szBuffer, "%f%f%f", &max, &min, &cur);
			m_fMaxRange	= max;
			m_fMinRange	= min;
			m_fCurrent	= cur;

			if (m_bOutOfRange)
			{
				if (m_fMaxRange <= m_fCurrent) // Heater가 켜진 상태에서 최고 온도 보다 높으면 꺼짐
				{
					XDEBUG("TEMP: Min=%.1f, Max=%.1f, Current=%.1f\xd\xa",
								m_fMinRange, m_fMaxRange, m_fCurrent);

					m_bOutOfRange = FALSE;
					m_pStatusMonitor->SetState(FALSE, GPIONOTIFY_TEMP);
					mcuTempRangeRestoreEvent(m_fCurrent, m_fMinRange, m_fMaxRange);
				}
			}
			else
			{
				if (m_fMinRange >= m_fCurrent) // 최하 온도 보다 낮으면 켜짐
				{
					XDEBUG("TEMP: Min=%.1f, Max=%.1f, Current=%.1f\xd\xa",
								m_fMinRange, m_fMaxRange, m_fCurrent);

					m_bOutOfRange = TRUE;
					m_pStatusMonitor->SetState(TRUE, GPIONOTIFY_TEMP);
					mcuTempRangeEvent(m_fCurrent, m_fMinRange, m_fMaxRange);
				}
			}
		}
		close(fd);
	}
	time(&m_nLastTime);
	m_Locker.Unlock();
	return TRUE;
}

BOOL CTempManager::OnTimeoutThread()
{
	return OnActiveThread();
}

