#ifndef __DLMS_PARSER_H__
#define __DLMS_PARSER_H__

#include "LinkedList.h"
#include "Locker.h"
#include "MeterParser.h"

#include "dlmsDataStream.h"

class CDLMSParser : public CMeterParser, CDlmsDataStream
{
public:
	CDLMSParser();
	virtual ~CDLMSParser();

public:
	void	OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength);
    int     OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);

public:  // Energy Level Management
    BOOL IsDRAsset(EUI64 *pId);
    int GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray);
    int GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel);
    int SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen);

protected:
	int		DLMS_MAIN(CMDPARAM *pCommand, METER_STAT *pStat=NULL);
	BOOL    OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, int nRemain, void *pCallData);

protected:
    CLocker m_Locker;
};

#endif	// __DLMS_PARSER_H__
