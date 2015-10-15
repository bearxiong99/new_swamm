#ifndef __METERING_SERVICE_H__
#define __METERING_SERVICE_H__

#include "Locker.h"

class CMeteringService
{
public:
	CMeteringService();
	virtual ~CMeteringService();

protected:
	int		WriteData(const char *pszFileName, EUI64 *id, char *pszBuffer, int nLength);

private:
	CLocker			m_WriteLocker;	
};

#endif	// __METERING_SERVICE_H__
