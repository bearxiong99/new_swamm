#ifndef __THIRDPART_PARSER_H__
#define __THIRDPART_PARSER_H__

#include "MeterParser.h"
#include "Operation.h"

class CThirdPartyParser : public CMeterParser
{
public:
	CThirdPartyParser();
	virtual ~CThirdPartyParser();

protected:
	int		MAIN(CMDPARAM *pCommand, METER_STAT *pStat=NULL);
	void 	SendMessage(ONDEMANDPARAM *pOndemand, EUI64 *id);

public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
};

#endif	// __THIRDPART_PARSER_H__
