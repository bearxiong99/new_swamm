#ifndef __MSTR711_H__
#define __MSTR711_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"
#include "ParserMbus.h"


class CMstr711Parser : public CMeterParser
{
public:
	CMstr711Parser();
	virtual ~CMstr711Parser();

protected:
	int		MBUS_METERING(CMDPARAM *pCommand, METER_STAT *pStat=NULL);


public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
  
protected:
    CLocker m_DataReciveLocker;  
};

#endif	// __MSTR711_H__
