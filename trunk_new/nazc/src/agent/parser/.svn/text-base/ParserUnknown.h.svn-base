#ifndef __UNKNOWN_PARSER_H__
#define __UNKNOWN_PARSER_H__

#include "MeterParser.h"
#include "Operation.h"

class CUnknownParser : public CMeterParser
{
public:
	CUnknownParser();
	virtual ~CUnknownParser();

protected:
	int		UNKNOWN_MAIN(CMDPARAM *pCommand, METER_STAT *pStat=NULL);
	void 	SendMessage(ONDEMANDPARAM *pOndemand, EUI64 *id);

public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
};

#endif	// __UNKNOWN_PARSER_H__
