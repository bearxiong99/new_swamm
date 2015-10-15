#ifndef __AIDON_PARSER_H__
#define __AIDON_PARSER_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"

#include "ParserPollBase.h"
#include "iec62056_21DataStream.h"

class CAidonParser : public CPollBaseParser, CIec21DataStream
{
public:
	CAidonParser();
	virtual ~CAidonParser();

public:
	void	OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength);

protected:
    void ActPreAction(ONDEMANDPARAM *pOndemand); 
    int StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    //int StateMeterConstant(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);

    BOOL OnAck(DATASTREAM *pStream, void *pCallData);
    BOOL OnNak(DATASTREAM *pStream, void *pCallData);
    BOOL OnBreakFrame(DATASTREAM *pStream, void *pCallData);
    BOOL OnIdentificationFrame(DATASTREAM *pStream, BYTE* manufacturerId, BYTE baud, BYTE *serialSetting, BYTE * identification, 
        int nLength, void *pCallData);
    BOOL OnCommandFrame(DATASTREAM *pStream, BYTE cmd, BYTE cmdType, BYTE *pszBuffer, int nLength, void *pCallData);
    BOOL OnDataFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData);
};

#endif	// __AIDON_PARSER_H__
