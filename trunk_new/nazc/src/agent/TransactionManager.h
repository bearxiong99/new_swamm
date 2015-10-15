#ifndef __TRANSACTION_MANAGER_H__
#define __TRANSACTION_MANAGER_H__

#include "Hash.h"
#include "JobObject.h"
#include "TimeoutThread.h"
#include "LinkedList.h"
#include "Locker.h"
#include "Variable.h"

#define TR_MAX_NICE     3
#define TR_MIN_NICE     -2
#define TR_MAX_RETRY    3
#define TR_MIN_RETRY    0

typedef struct _tagEndDeviceTransaction {
    WORD        trID;
    cetime_t    trCreateTime;
    cetime_t    trLastTime;
    signed char trNice;
    BYTE        trQueue;
    BYTE        trTry;
    BYTE        trState;
    int         trError;
} EndDeviceTransaction;

typedef struct _tagTransactionHistory {
    cetime_t    trTime;
    BYTE        trState;
    int         trError;
} TransactionHistory;

class CTransactionManager 
{
public:
	CTransactionManager();
	virtual ~CTransactionManager();

public:
	BOOL	Initialize();
	void	Destroy();

    BOOL    Add(MIBValue *pValue, int nCount, WORD *nTransactionID);
    BOOL    LoadTransactionInfo(EUI64 *pId, BYTE nOption, TRINFOENTRY **ppTrInfo, int *nCount);
    BOOL    LoadTransactionInfo(WORD nTrID, MIBValue **pParams, int *nParamCnt);
    BOOL    LoadTransactionInfo(WORD nTrID, BYTE nOption, TRINFOENTRY *pTrInfo, 
                    MIBValue **ppParams, int *nParamCnt, 
                    MIBValue **ppResults, int *nResultCnt, 
                    TRHISTORYENTRY **ppTrHist, int *nHistCnt);
    int     UpdateTransaction(WORD nTrID, BYTE nState);
    BOOL    ExecuteTransaction(WORD nTrID);
    BOOL    ResolveTimeoutTransaction(BOOL bClearAll=FALSE);
    BOOL    IntegrityCheck(void);
    BOOL    Notify(EUI64 *pId);

protected:
    BOOL    LoadValues(void);

    WORD    GetNextTransactionID(void);
    BOOL    UpdateTransactionID(WORD next);

    BOOL    LoadEndDeviceTransaction(EUI64 *pId, WORD nTrID, EndDeviceTransaction *pTrInfo);
    BOOL    LoadEndDeviceTransaction(EUI64 *pId, EndDeviceTransaction **ppTrInfo, int *nCount);
    BOOL    LoadTransactionHistory(WORD trID, TransactionHistory **ppTrHist, int *nCount);
    BOOL    LoadTransactionTarget(WORD trID, EUI64 *pID);
    BOOL    LoadTransaction(WORD nTrID, BYTE nOption, TRINFOENTRY *pTrInfo, 
                    MIBValue **ppParams, int *nParamCnt, 
                    MIBValue **ppResults, int *nResultCnt, 
                    TRHISTORYENTRY **ppTrHist, int *nHistCnt);

    BOOL    CreateTransactionRequest(WORD nTrID, MIBValue *pValue);
    BOOL    AppendEndDeviceTransaction(EUI64 *id, WORD trID, cetime_t nTime, signed char nNice, BYTE nTry, BYTE nState, int nError);
    BOOL    AppendEndDeviceTransaction(WORD trID, cetime_t nTime, MIBValue *pValue, int nCount);
    BOOL    UpdateTransactionHistory(WORD trID, cetime_t nTime, BYTE nState, int nError);
    BOOL    UpdateEndDeviceTransaction(EUI64 *pId, WORD trID, cetime_t nTime,  BOOL bSuccNice, BOOL bSuccTry, BYTE nState, int nError);
    BOOL    UpdateEndDeviceTransaction(EUI64 * pId, int idx, EndDeviceTransaction *pEndTrInfo);

    BOOL    DeleteTransaction(WORD trID, EUI64 *pId = NULL);

    BOOL    Enqueue(EUI64 *pId, WORD pID, int idx, cetime_t nTime, EndDeviceTransaction *pEndTrInfo);
    IF4_COMMAND_TABLE * FindCommand(OID3 *cmd);

    static int CompTrID(const void *p1, const void *p2);

protected:
    WORD    m_nLastTransactionID;

	CLocker m_TransactionLocker;
};

extern CTransactionManager		*m_pTransactionManager;

#endif	// __TRANSACTION_MANAGER_H__
