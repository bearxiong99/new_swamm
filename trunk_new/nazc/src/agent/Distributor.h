#ifndef __DISTRIBUTOR_H__
#define __DISTRIBUTOR_H__

#include "OtaWorker.h"
#include "Distributor.h"
#include "TimeoutThread.h"
#include "JobObject.h"
#include "Locker.h"
#include "Chunk.h"
#include "Variable.h"

#define MAX_OTA_WORKER			3

#define DISABLE_DOWNLOAD        FALSE
#define ENABLE_DOWNLOAD         TRUE

class CDistributor : public CTimeoutThread, public CJobObject
{
public:
	CDistributor();
	virtual ~CDistributor();

// Operations
public:
    BOOL    Initialize();
    void    Destroy();

	int		Add(char *pszTrigger, BYTE nType, char *pszModel, BYTE nTransferType, UINT nOtaStep, BYTE nWriteCount,
					UINT nRetry, BYTE nThread, BYTE nInstallType, WORD nOldHw, WORD nOldSw, WORD nOldBuild,
					WORD nNewHw, WORD nNewSw, WORD nNewBuild, char *pszBinURL, char *pszBinChecksum,
					char *pszDiffURL, char *pszDiffChecksum, EUI64 *pIdList, int nIdCount, BOOL bDownload);
	int		Cancel(char *pszTriggerId);

	OTAENTRY *GetItem(char *pszTriggerId);
	int		GetState(char *pszTriggerId, OTASTATE *pState);
	int		GetList(DISTRIBUTIONENTRY **pList, int *nCount);
	int		GetOtaNodes(char *pszTriggerId, OTANODEENTRY **pList, int *nCount);
	BOOL	IsBusy();

protected:
	OTAENTRY *Find(char *pszTriggerId);
	void	FreeItem(OTAENTRY *pEntry);
	void	RemoveAll();

	BOOL	DownloadFirmware(OTAENTRY *pEntry);
	BOOL	UpgradeConcentrator(OTAENTRY *pEntry, char *pszFileName);
	BOOL	UpgradeCoordinator(OTAENTRY *pEntry, char *pszFileName);
	BOOL	UpgradeSensor(OTAENTRY *pEntry, char *pszFileName);

	BOOL	MulticastOta(OTAENTRY *pEntry, CChunk *pChunk);
	BOOL	UnicastOta(OTAENTRY *pEntry, CChunk *pChunk);
	BOOL	UnicastOtaByList(OTAENTRY *pEntry, CChunk *pChunk, OTANODE *pList, int nCount);
	BOOL	OtaStepByStep(OTAENTRY *pEntry, CChunk *pChunk, OTANODE *pList, int nCount, int nRetry, BYTE nOtaStep);

	void	ClearStateFlag(OTANODE *pList, int nCount);
	void	SetSkipNode(OTANODE *pList, int nCount);
	void	TimesyncAndManyToOne(OTAENTRY *pEntry, int nDelay);

	COtaWorker *GetIdleWorker(OTAENTRY *pEntry);
	BOOL	IsActiveWorker();

	void	Distribution(OTAENTRY *pEntry);
	
protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

private:
	BOOL		m_bBusy;
    CLocker		m_Locker;
	CLinkedList<OTAENTRY *> m_List;
	COtaWorker	m_arWorker[MAX_OTA_WORKER];
	char		m_szLogBuffer[1025];
};

extern CDistributor	*m_pDistributor;

#endif	// __DISTRIBUTOR_H__
