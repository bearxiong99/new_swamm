#ifndef __PARSER_ACD_H__
#define __PARSER_ACD_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "ParserPulseBase.h"
#include "Operation.h"

class CACDParser : public CPulseBaseParser
{
public:
	CACDParser();
	virtual ~CACDParser();

protected:
    BOOL IsDRAsset(EUI64 *pId);       
    int GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel);  
    int SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbackLen); 
    int GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray);   

    int StateEnergyLevel(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
};

#endif	// __PARSER_ACD_H__
