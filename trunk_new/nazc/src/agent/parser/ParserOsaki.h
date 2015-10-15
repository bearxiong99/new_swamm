#ifndef __OSAKI_H__
#define __OSAKI_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"

#include "ParserPollBase.h"
#include "iec62056_21DataStream.h"

class COsakiParser : public CPollBaseParser, CIec21DataStream
{
public:
	COsakiParser();
	virtual ~COsakiParser();

public:
    //BOOL    OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData);
    int     OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
	void	OnData(EUI64 *pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength);

protected:  // State functions
    void ActPreAction(ONDEMANDPARAM *pOndemand);

    int StateMeterSerial(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);

    BOOL IsDRAsset(EUI64 *pId);
    int GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray);
    int GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel);
    int SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen);

    BOOL OnAck(DATASTREAM *pStream, void *pCallData);
    BOOL OnNak(DATASTREAM *pStream, void *pCallData);
    //BOOL OnCommandFrame(DATASTREAM *pStream, BYTE cmd, BYTE cmdType, BYTE *pszBuffer, int nLength, void *pCallData);
    BOOL OnDataFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData);

//protected:
    //CLocker m_Locker;
};

#endif	// __OSAKI_H__
