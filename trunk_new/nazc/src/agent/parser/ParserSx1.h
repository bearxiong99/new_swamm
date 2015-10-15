#ifndef __MITSUBISHI_SX1_H__
#define __MITSUBISHI_SX1_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"

#include "ParserPollBase.h"
#include "iec62056_21DataStream.h"

class CSx1Parser : public CPollBaseParser, CIec21DataStream
{
public:
	CSx1Parser();
	virtual ~CSx1Parser();

public:
	void	OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength);
    BOOL    PostCreate(const EUI64 *pId, int nPortNum);
    BOOL    IsPushMetering();  // Push Metering 지원 장비 

protected:
    void ActPreAction(ONDEMANDPARAM *pOndemand);
    void ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData);

    int StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);

    BOOL IsDRAsset(EUI64 *pId);
    int GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray);
    int GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel);
    int SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen);

    BOOL OnAck(DATASTREAM *pStream, void *pCallData);
    BOOL OnNak(DATASTREAM *pStream, void *pCallData);
    BOOL OnBreakFrame(DATASTREAM *pStream, void *pCallData);
    BOOL OnIdentificationFrame(DATASTREAM *pStream, BYTE* manufacturerId, BYTE baud, BYTE *serialSetting, BYTE * identification, 
        int nLength, void *pCallData);
    BOOL OnCommandFrame(DATASTREAM *pStream, BYTE cmd, BYTE cmdType, BYTE *pszBuffer, int nLength, void *pCallData);
    BOOL OnDataFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData);

    // LP Address & Block Size 계산
    WORD GetLpAddress(BYTE nLpPointerAddr, int nDay, BYTE nPeriod);
    int GetLpSize(BYTE nPeriod);

public:
    static BOOL fnPreClearTamper(ONDEMANDPARAM *pOndemand);
    static BOOL fnPreBypassEarth(ONDEMANDPARAM *pOndemand);
    static BOOL fnPreReverse(ONDEMANDPARAM *pOndemand);
    static BOOL fnPreOverload(ONDEMANDPARAM *pOndemand);
    static BOOL fnPreFrontCover(ONDEMANDPARAM *pOndemand);
    static BOOL fnPreTerminalCover(ONDEMANDPARAM *pOndemand);
    static BOOL fnPreSyncTime(ONDEMANDPARAM *pOndemand);
    static BOOL fnPreRereadTime(ONDEMANDPARAM *pOndemand);

};

#endif	// __MITSUBISHI_SX1_H__
