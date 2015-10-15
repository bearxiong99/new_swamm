#ifndef __PULSEBASED_H__
#define __PULSEBASED_H__

#include "Locker.h"
#include "ParserBase.h"

class CPulseBaseParser : public CBaseParser
{
public:
    CPulseBaseParser(int nType, int nService, int nAttribute, 
	        char **pszNameArr, UINT nParserType, const char *pszDescr, const char *pszPrefixVendor, 
	        const char *pszModel, int nConnectionTimeout, int nReplyTimeout, int nRetry);
	virtual ~CPulseBaseParser();

protected:
    int     DoMetering(CMDPARAM *pCommand, int nOption, int nOffset, int nCount, void * pBypassData, METER_STAT *pStat);

protected:  // State functions
    virtual void ActPreAction(ONDEMANDPARAM *pOndemand); // 검침전에 처리해야 할 일
    virtual void ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData); // 성공적으로 검침이 끝난 후 해야 할 일
    virtual void ActPostFailAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData); // 검침이 실패한 후 해야 할 일

    virtual int StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateEnergyLevel(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateMeterSerial(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateAmrInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateAmrData(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateVoltageScale(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateMeterConstant(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateBatteryPoint(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateLpSet(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;

protected:
    CLocker m_Locker;
};

#endif	// __PULSEBASED_H__
