#ifndef __PARSERBASED_H__
#define __PARSERBASED_H__

#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"


class CBaseParser : public CMeterParser
{
public:
    CBaseParser(int nType, int nService, int nAttribute, 
	        char **pszNameArr, UINT nParserType, const char *pszDescr, const char *pszPrefixVendor, 
	        const char *pszModel, int nConnectionTimeout, int nReplyTimeout, int nRetry);
	virtual ~CBaseParser();

public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);

protected:
	virtual int	METERING(CMDPARAM *pCommand, METER_STAT *pStat=NULL);

    int ReadRomMap(WORD addr, int len, ONDEMANDPARAM *pOndemand, BYTE *szBuffer, 
            int nRetryState, int nSuccState);
    int ReadAmrData(ONDEMANDPARAM *pOndemand, BYTE *szBuffer, int nRetryState, int nSuccState);

    virtual int DoMetering(CMDPARAM *pCommand, int nOption, int nOffset, int nCount, void * pBypassData, METER_STAT *pStat)=0;
    virtual void ActPreAction(ONDEMANDPARAM *pOndemand) = 0;
    virtual void ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData) = 0;
    virtual void ActPostFailAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData) = 0;

// State functions
    virtual int StateOpen(HANDLE codi, ONDEMANDPARAM *pOndemand, BOOL *bConnected);
    virtual int StateError(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;
    virtual int StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;
    virtual int StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;
    virtual int StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;
};

#endif	// __PARSERBASED_H__
