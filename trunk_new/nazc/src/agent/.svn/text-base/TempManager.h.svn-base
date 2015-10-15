#ifndef __TEMP_MANAGER_H__
#define __TEMP_MANAGER_H__

#include "Locker.h"
#include "TimeoutThread.h"

class CTempManager : public CTimeoutThread
{
public:
	CTempManager();
	virtual ~CTempManager();

public:
	BOOL	IsInstalled() const;
	double	GetValue() const;
	int		GetInterval() const;

	void	SetInterval(int nInterval);
	BOOL	GetRange(double *pMin, double *pMax);
	double	GetMinRange();
	double	GetMaxRange();
	BOOL	SetRange(double fMinRange, double fMaxRange);
	BOOL	SetMinRange(double fMinRange);
	BOOL	SetMaxRange(double fMaxRange);

// Operations
public:
    BOOL    Initialize();
    void    Destroy();
	BOOL	Check();

protected:
	BOOL	DetectSensor();

protected:
	BOOL	OnTimeoutThread();
	BOOL	OnActiveThread();

private:
    CLocker		m_Locker;
	BOOL		m_bInstalled;
	BOOL		m_bOutOfRange;
	char		m_szDevice[64];
	char		m_szBuffer[100];
	double		m_fCurrent;
	double		m_fMinRange;
	double		m_fMaxRange;
    int         m_nInterval;
    time_t      m_nLastTime;
};

extern CTempManager	*m_pTempManager;

#endif	// __TEMP_MANAGER_H__
