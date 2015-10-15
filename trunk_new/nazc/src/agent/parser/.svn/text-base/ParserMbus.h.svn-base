#ifndef __MBUS_H__
#define __MBUS_H__

#include "LinkedList.h"
#include "Locker.h"
#include "MeterParser.h"
#include "mbusDataStream.h"


class CMbusParser : public CMeterParser, CMbusDataStream
{
public:
	CMbusParser();
	virtual ~CMbusParser();

public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
	void	OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength);

protected:
	int		MBUS_MAIN(CMDPARAM *pCommand, METER_STAT *pStat=NULL);
    BOOL    OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData);

protected:
    CLocker m_Locker;
};

#endif	// __MBUS_H__
