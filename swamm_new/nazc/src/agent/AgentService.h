#ifndef __AGENT_SERVICE_H__
#define __AGENT_SERVICE_H__

#include "ServiceController.h"
#include "RealTimeClock.h"
#include "TempManager.h"
#include "EventManager.h"
#include "UserManager.h"
#include "StatusMonitor.h"
#include "MeterFileWriter.h"
#include "LpWriter.h"
#include "MeterUploader.h"
#include "MalfMonitor.h"
#include "TimeoutHash.h"
#include "SensorTimesync.h"
#include "SystemMonitor.h"
#include "AutoReset.h"
#include "EndDeviceList.h"
#include "MeterDeviceList.h"
#include "InventoryScanner.h"
#include "BatchJob.h"
#include "MeterParserFactory.h"
#include "Ondemander.h"
#include "Metering.h"
#include "MeterReader.h"
#include "Recovery.h"
#include "Polling.h"
#include "PowerOutage.h"
#include "Watchdog.h"
#include "Distributor.h"
#include "BypassQueue.h"

#include "rep/DBRepository.h"

#include "ZigbeeServer.h"

#include "InventoryHash.h"
#include "TransactionManager.h"

#define GetCoordinator()        m_pService->GetCoordinatorHandle()

class CAgentService : public CServiceController
{
// Construction/Destruction
public:
	CAgentService();
	virtual ~CAgentService();

// Attribute
public:
	int		GetState();
	int 	GetResetState();
	int 	SetResetState(int nType);
	int		GetEthernetType() const;
	int		GetMobileType() const;
	int		GetMobileMode() const;
    HANDLE  GetCoordinatorHandle() const;     
 
// Override Functions
public:
	BOOL	OnStartService();
	void	OnStopService();
	BOOL	OnMessage(int nMessage, void *pParam);

// Operations
public:
	void	ImmediatelyShutdown();
	void	Reset();

// Member Functions
protected:
	void	LoadVariable();
#if defined(__TI_AM335X__)
    void    CheckBattery();
#endif
// Member Variable
protected:
    HANDLE          m_codiHandle;
	double			m_fMinValue;
	double			m_fMaxValue;
	//int 				m_nResetReason;

	CRealTimeClock	theRealTimeClock;
	CTempManager	theTempManager;
	CEventManager	theEventManager;
	CSystemMonitor	theSystemMonitor;
	CUserManager	theUserManager;
	CStatusMonitor	theStatusMonitor;
	CMeterFileWriter    theMeterFileWriter;
	CLpWriter       theLpWriter;
	CMeterUploader	theMeterUploader;
	CMalfMonitor	theMalfMonitor;
	CTimeoutHash	theTimeoutHash;
	CEndDeviceList	theEndDeviceList;
	CMeterDeviceList theMeterDeviceList;
	CMeterParserFactory	theMeterParserFactory;
	COndemander		theOndemander;
	CInventoryScanner theInventoryScanner;
	CBatchJob		theBatchJob;
	CPowerOutage	thePowerOutage;
	CAutoReset		theAutoReset;
	CSensorTimesync	theTimesync;
	CMetering		theMetering;
	CMeterReader	theMeterReader;
	CRecovery		theRecovery;
	CPolling		thePolling;
	CWatchdog		theWatchdog;
	CDistributor	theDistributor;
	CZigbeeServer	theZigbeeServer;
	CInventoryHash	theInventoryHash;
    CTransactionManager theTransactionManager;
    CBypassQueue    theBypassQueue;

    CDBRepository   theDBRepository;
};

extern CAgentService	*m_pService;

#endif	// __AGENT_SERVICE_H__

