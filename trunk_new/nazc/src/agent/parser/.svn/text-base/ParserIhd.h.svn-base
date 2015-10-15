#ifndef __IHD_PARSER_H__
#define __IHD_PARSER_H__

#include "ParserPollBase.h"

class CIhdParser : public CPollBaseParser
{
public:
	CIhdParser();
	virtual ~CIhdParser();

protected:
    void ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData);
    int StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
};

#endif	// __IHD_PARSER_H__
