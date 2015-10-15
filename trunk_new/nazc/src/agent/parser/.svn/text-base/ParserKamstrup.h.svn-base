#ifndef __KAMST_H__
#define __KAMST_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"

#include "ParserPollBase.h"
#include "kmpDataStream.h"

class CKamstrupParser : public CPollBaseParser, CKmpDataStream
{
public:
	CKamstrupParser();
	virtual ~CKamstrupParser();

protected:
	int		METERING(CMDPARAM *pCommand, METER_STAT *pStat=NULL);
    BOOL    OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData);

public:
	void	OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength);

public:  // Energy Level Management
    BOOL IsDRAsset(EUI64 *pId);
    int GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray);
    int GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel);
    int SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen);

protected: // Utility functions
    void GetKamstrupMeterType(EUI64 *pId, ONDEMANDPARAM *pOndemand, void* pSession);

protected:  // State functions
    void ActPreAction(ONDEMANDPARAM *pOndemand);
    void ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData);
    void ActPostFailAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData);

    int StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int StateMeterType(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;
    int StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;
    int StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;
    int StateLpSet(ONDEMANDPARAM *pOndemand, BYTE *szBuffer) ;

    // Action 
    /* LP Set으로 얻은 정보로 LP Pointer를 계산한다 */
    int ActPostLpSet(ONDEMANDPARAM *pOndemand, const BYTE *szBuffer, void *pLpInfo, int *nLpPeriod);

    BOOL GetLpAddressSize(ONDEMANDPARAM *pOndemand, void *lpLogBuff, int nDay, int nCount,
            WORD *nAddr, int *nSize, int *nAddSeek);

protected:
    CLocker m_Locker;
};

#endif	// __KAMST_H__
