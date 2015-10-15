#ifndef __REPEATER_H__
#define __REPEATER_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"
#include "vendor/repdef.h"
#include "codiapi.h"
#include "endiFrame.h"

#define REPEATER_OPTION_READ_INFORMATION        0x01
#define REPEATER_OPTION_READ_SOLAR_LOG          0x02
#define REPEATER_OPTION_MASK_INCREMENTAL_READ   0x00  
#define REPEATER_OPTION_MASK_FULL_READ          0x10 

#define REPEATER_OPTION_READ_INCREMENTAL_SOLAR_LOG  (REPEATER_OPTION_READ_SOLAR_LOG | REPEATER_OPTION_MASK_INCREMENTAL_READ)    // 2
#define REPEATER_OPTION_READ_FULL_SOLAR_LOG         (REPEATER_OPTION_READ_SOLAR_LOG | REPEATER_OPTION_MASK_FULL_READ)           // 18

class CRepeaterParser : public CMeterParser
{
public:
	CRepeaterParser();
	virtual ~CRepeaterParser();

protected:
	int		REPEATER_MAIN(CMDPARAM *pCommand, METER_STAT *pStat=NULL);
	void 	SendMessage(ONDEMANDPARAM *pOndemand, EUI64 *id);
    BOOL    UpdatePowerType(EUI64 * id, REPEATER_METERING_DATA * pMeterData, BYTE *nAttr);
    void    UpdateSolarLog(REPEATER_METERING_DATA * pMeterData, ENDI_SOLARLOG_ENTRY * pSolarLog, CChunk *pChunk);
    void    UpdateMeteringData(EUI64 * id, CMDPARAM *pCommand, REPEATER_METERING_DATA * pMeterData, CChunk *pChunk);

public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
};

#endif	// __REPEATER_H__
