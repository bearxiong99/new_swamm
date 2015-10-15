#ifndef __PARSER_FIREALARM_H__
#define __PARSER_FIREALARM_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"
#include "vendor/firealarmdef.h"
#include "vendor/repdef.h"
#include "codiapi.h"

class CFireAlarmParser : public CMeterParser
{
public:
	CFireAlarmParser();
	virtual ~CFireAlarmParser();

protected:
	int		FIREALARM_MAIN(CMDPARAM *pCommand, METER_STAT *pStat=NULL);
	BOOL    UpdatePowerType(EUI64 * id, REPEATER_METERING_DATA * pMeterData, BYTE *nAttr);
	void    UpdateMeteringData(EUI64 * id, CMDPARAM *pCommand, REPEATER_METERING_DATA * pMeterData, CChunk *pChunk);

public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
};


#endif	// __PARSER_FIREALARM_H__
