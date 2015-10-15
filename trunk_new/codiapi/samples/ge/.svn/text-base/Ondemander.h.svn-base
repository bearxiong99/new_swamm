#ifndef __ONDEMANDER_H__
#define __ONDEMANDER_H__

#include "LinkedList.h"
#include "Locker.h"
#include "MeterParser.h"
#include "i210.h"
#include "Operation.h"

class COndemander
{
public:
	COndemander();
	virtual ~COndemander();

public:
	CMeterParser *SelectParser(char *pszType);

public:
	int		Command(char *pszType, CMDPARAM *pCommand, METER_STAT *pStat=NULL);
	void	OnData(EUI64 *id, char *pszData, BYTE nLength);

protected:
	COperation		m_Operation;
	CI210Parser		m_i210Parser;
};

extern COndemander	*m_pOndemander;

#endif	// __ONDEMANDER_H__
