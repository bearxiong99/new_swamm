#ifndef __POLLBASED_H__
#define __POLLBASED_H__

#include "Locker.h"
#include "ParserBase.h"

class CPollBaseParser : public CBaseParser
{
public:
    CPollBaseParser(int nType, int nService, int nAttribute, 
	        char **pszNameArr, UINT nParserType, const char *pszDescr, const char *pszPrefixVendor, 
	        const char *pszModel, int nConnectionTimeout, int nReplyTimeout, int nRetry);
	virtual ~CPollBaseParser();

protected:
    int     DoMetering(CMDPARAM *pCommand, int nOption, int nOffset, int nCount, void * pBypassData, METER_STAT *pStat);

protected:  // State functions
    virtual void ActPreAction(ONDEMANDPARAM *pOndemand); // 검침전에 처리해야 할 일
    virtual void ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData); // 성공적으로 검침이 끝난 후 해야 할 일
    virtual void ActPostFailAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData); // 검침이 실패한 후 해야 할 일

    virtual int StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateEnergyLevel(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateMeterType(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateMeterSerial(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateAmrInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateMeterConstant(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    virtual int StateLpSet(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;

    // LP Address 계산
    virtual WORD GetLpAddress(BYTE nLpPointerAddr, int nDay, BYTE nPeriod);
    // Lp Block Size 계산
    virtual int GetLpSize(BYTE nPeriod);

protected:
    CLocker m_Locker;
};

#endif	// __POLLBASED_H__
