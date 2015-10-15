#ifndef __SX_SUB_CORE_H__
#define __SX_SUB_CORE_H__

#include "MeterParser.h"


class CSxSubCoreParser : public CMeterParser
{
public:
	CSxSubCoreParser();
	virtual ~CSxSubCoreParser();

public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
  
};

#endif	// __SX_SUB_CORE_H__
