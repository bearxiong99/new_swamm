#include "common.h"
#include "AgentService.h"
#include "CommandHandler.h"
#include "CommonUtil.h"
#include "Variable.h"
#include "Utility.h"
#include "DebugUtil.h"
#include "Patch.h"

#include "ShellCommand.h"
#include "MobileUtility.h"

#include "dlmsFunctions.h"
#include "mbusFunctions.h"
#include "kmpFunctions.h"

void GETTIMESTAMP(TIMESTAMP *pStamp, time_t *pNow);

//////////////////////////////////////////////////////////////////////
// CAgentService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAgentService		*m_pService = NULL;

extern int	m_nSystemType;
extern int	m_nSystemID;
extern int	m_nLocalPort;

extern int	m_nEthernetType;
extern int	m_nMobileMode;

extern int  m_nGroupInterval;
extern int  m_nRetryStartSecond;
extern int	m_nDebugLevel;

extern BOOL	m_bGprsPerfTestMode;
extern int  m_nResetReason;
extern BYTE m_nHwVersion;
extern MOBILECFGENTRY m_sMobileConfig;

CODIDEVICE codiDevice =
{
		CODI_COMTYPE_RS232,				// 디바이스
		"/dev/ttyO2",					// 디바이스명
		115200,							// 속도
		8,								// 데이터 비트
		1,								// 스톱 비트
		CODI_PARITY_NONE,				// 패리터
		CODI_OPT_RTSCTS					// 옵션
};

#if     defined(__PATCH_5_1949__)
extern BOOL    *m_pbPatch_5_1949;
#endif
#if     defined(__PATCH_9_2567__)
extern BOOL    *m_pbPatch_9_2567;
#endif

void CoordinatorPowerControl();

//////////////////////////////////////////////////////////////////////
// CAgentService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAgentService::CAgentService()
{
	m_pService 		= this;
	m_fMinValue		= -20;      // 온도 최소값
	m_fMaxValue		= 80;       // 온도 최대값
    m_codiHandle    = NULL;
}

CAgentService::~CAgentService()
{
}

//////////////////////////////////////////////////////////////////////
// CAgentService Override Functions
//////////////////////////////////////////////////////////////////////

int CAgentService::GetState()
{
	return (theStatusMonitor.GetState() == 0) ? 1 : 0;
}
int CAgentService::GetResetState()
{
	XDEBUG("Get Reset State\xd\xa");
	return m_nResetReason;
}
int CAgentService::SetResetState(int nType)
{

	SetFileValue("/app/conf/reset.mcu", nType);
	XDEBUG("Set Reset State %d\xd\xa", nType);
	m_nResetReason=nType;
	return m_nResetReason;
}

int	CAgentService::GetEthernetType() const
{
	return m_nEthernetType;
}

#if 0
int	CAgentService::GetMobileType() const
{
    // TI AM225x를 사용할 경우 AMTelecom AME2252를 CDMA Type으로 초기화 하고
    // 나머지는 GSM Type으로 초기화 한다
#if     defined(__TI_AM335X__)
    if(m_sMobileConfig.moduleType != NULL && strlen(m_sMobileConfig.moduleType) > 0
            && strcasecmp(m_sMobileConfig.moduleType, MOBILE_MODULE_TYPE_AME5210)==0)
    {
        return MOBILE_TYPE_CDMA;
    }
    return MOBILE_TYPE_GSM;
#else
	return m_nMobileType;
#endif
}
#endif

int	CAgentService::GetMobileMode() const
{
	return m_nMobileMode;
}

HANDLE CAgentService::GetCoordinatorHandle() const
{
    return m_codiHandle;
}


//////////////////////////////////////////////////////////////////////
// CAgentService Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CAgentService::OnStartService()
{
    UINT    startTick, curTick, resetTick;
    int     nState, nError;

	// Create Directory
    InitializeDirectories();
    /*-- Database Helper Initiliaze --*/
    theDBRepository.Initialize();

	// Initialize Variable
	LoadVariable();

    DisplayPatchInfo();
#if     defined(__INCLUDE_GPIO__)
	// Initialize GPIO (POWER/LED)
	GPIOAPI_Initialize(TRUE, m_sMobileConfig.moduleType, TRUE);
#endif

    /*-- Transaction Manager Initialize --*/
    theTransactionManager.Initialize(); 

	// Command Control Service Initialize
	IF4API_Initialize(m_nLocalPort, m_CommandHandler);		
	IF4API_SetID(m_nSystemID);

    MOBILE_Initialize();

	// Watchdog initialize
	theWatchdog.Initialize();

	theStatusMonitor.Initialize();
	theRealTimeClock.Initialize();
	theUserManager.LoadUser();
	theTempManager.Initialize();
	theEventManager.Initialize();
    // 사용하지 않는 AlarmManager 삭제
	//theAlarmManager.Initialize();
	theMeterFileWriter.Initialize();
	theMeterDbWriter.Initialize();
	theLpWriter.Initialize();
	theMalfMonitor.Initialize();
	theMeterUploader.Initialize();
	theTimeoutHash.Initialize();
	theAutoReset.Initialize();
	theEndDeviceList.Initialize();
	theMeterDeviceList.Initialize();
#if defined(__TI_AM335X__)
    //theSmsWorker.Initialize(); // Mobile Client 보다 먼저 초기화 되어야 한다
#endif  //  __TI_AM335X__
	theSystemMonitor.Initialize();
	thePowerOutage.Initialize();
	theInventoryScanner.Initialize();
    theDistributor.Initialize();

    // Protocol Library 초기화
    PROTOCOL_DLMS_INIT;     // DLMS Protocol libary support
    PROTOCOL_MBUS_INIT;     // MBUS Protocol libary support
    PROTOCOL_KMP_INIT;      // KMP Protocol libary support


    // CODIAPI 초기화
    nError = codiInit();
    if(nError != CODIERR_NOERROR)
    {
        //ReportError(NULL, nError);
        return FALSE;
    }

	theInventoryHash.Initialize();

    // 새로운 Coordinator 장치를 등록한다
    nError = codiRegister(&m_codiHandle, &codiDevice);
    if(nError != CODIERR_NOERROR)
    {
        //ReportError(NULL, nError);
        codiUnregister(m_codiHandle);
        codiExit();
        return FALSE;
    }

    // Coordinator 서비스를 시작합니다.
	nError = codiStartup(m_codiHandle);  
	if (nError != CODIERR_NOERROR)
	{
		//ReportError(NULL, nError);
		codiUnregister(m_codiHandle);
		codiExit();
		return FALSE;
	}

    {
        /** Issue #2567: Patch 상태에 따라 Coodinator Link 절차 사용 여부를 설정한다
          */
#if     defined(__PATCH_9_2567__)
        Patch_9_2567_Handler(8, 9, 2567, (*m_pbPatch_9_2567) , const_cast<char *>("Remove Link Procedure"));
#endif
    }

    // 코디네이터가 초기화되었는지 확인한다.
    startTick = resetTick = GetSysUpTime();
    XDEBUG("COORDINATOR AUTO RESET ######################### \r\n");
    CoordinatorPowerControl();
    usleep(1000000);
    resetTick = GetSysUpTime();
    for(;;)
    {
        usleep(200000);
        nState = codiGetState(m_codiHandle);
        XDEBUG("-------- COORDINATOR STATE = %d -------\r\n", nState);
        if ((nState != CODISTATE_NOT_CONNECTED) && (nState != CODISTATE_NO_RESPONSE))
            break;

        curTick = GetSysUpTime();
        if ((curTick-startTick) > 12)
            break;

        if ((curTick-resetTick) > 4)
        {
            XDEBUG("COORDINATOR AUTO RESET ######################### \r\n");
            CoordinatorPowerControl();
            resetTick = GetSysUpTime();
        }
    }

	m_pZigbeeServer->Initialize();

	theMetering.Initialize();
	theMeterReader.Initialize();
	theRecovery.Initialize();
	thePolling.Initialize();
	theTimesync.Initialize();
	theBatchJob.Initialize();
    theBypassQueue.Initialize();

	m_pMalfMonitor->CheckExceptCleaning();
    
	// MCU Startup Event
	mcuStartupEvent();
 
	return TRUE;
}

void CAgentService::OnStopService()
{
	codiShutdown(m_codiHandle);
    codiUnregister(m_codiHandle);
    codiExit();

#if     defined(__INCLUDE_GPIO__)
    GPIOAPI_Destroy();
#endif

	exit(0);
}

BOOL CAgentService::OnMessage(int nMessage, void *pParam)
{
	int		i;

	XDEBUG("OnMessage(nMessage=%d, pParam=%p)\xd\xa", nMessage, pParam);

	switch(nMessage) {
	  case GWMSG_SHUTDOWN :
#if     defined(__SUPPORT_NZC1__)
		   mcuShutdownEvent(0, const_cast<char *>("127.0.0.1"));
#endif
		   ImmediatelyShutdown(true);

		   // 신형은 LOW, 영국, 스웨덴은 HIGH, 구형은 지원 안함
		   for(i=0; i<10; i++)
		   {
#if     defined(__INCLUDE_GPIO__)
#if     defined(__SUPPORT_NZC1__)
			   GPIOAPI_WriteGpio(GP_BATT_SW_OUT, 1);
#elif   defined(__SUPPORT_NZC2__)
			   GPIOAPI_WriteGpio(GP_BATT_SW_OUT, 0);
#elif   defined(__TI_AM335X__)
               SystemExec("shutdown -hP 0");        // Shutdown
#else
               assert(0);
#endif      // NZC_TYPE
#endif      // GPIO
			   usleep(1000000);
		   }
			// low bat이거나 cli 명령
		   // Shutdown에서는 리셋을 하지 않는다.
		   // GPIOAPI_WriteGpio(GP_SW_RST_OUT, 0);
		   break;

	  case GWMSG_RESET :
		   ImmediatelyShutdown(false);
		   usleep(1000000);
#if     defined(__INCLUDE_GPIO__)
#if     defined(__TI_AM335X__)
           SystemExec("reboot");
#else
		    GPIOAPI_WriteGpio(GP_SW_RST_OUT, 0);
#endif
#endif
		   break;
	  case GWMSG_FACTORYDEFAULT :
	  case GWMSG_FIRMWARE_UPDATE :
		   SetResetState(RST_UPFIRM);
		   ImmediatelyShutdown(false);
#if     defined(__TI_AM335X__)
           SystemExec("reboot");                // Reboot command
#else
#if     defined(__INCLUDE_GPIO__)
		   GPIOAPI_WriteGpio(GP_SW_RST_OUT, 0);
#else
           SystemExec("reboot");                // Reboot command
#endif
#endif
		   break;
	}
	return TRUE;
}

void CAgentService::ImmediatelyShutdown(bool isShutdown)
{
	char	szMessage[100];
	int		i, nTimeout=50;

	mcuResetEvent(3, "127.0.0.1");

	theLpWriter.Destroy();
	theMeterFileWriter.Destroy();
	VARAPI_Save(TRUE);

	SystemExec("sync");
	SystemExec("sync");

    printf("\r\n");
    for(i=0; i<nTimeout; i++)
    {
		// 이벤트가 없는 경우 20초가 경과하면 종료한다.
		if ((i > 20) && m_pEventManager->IsIdle())
		{
			usleep(1000000);
			break;
		}

        sprintf(szMessage, "\xdSystem immediately shutdown [%d] ", nTimeout-i);
        write(STDOUT_FILENO, szMessage, strlen(szMessage));
        usleep(1000000);
    }      
    printf("\r\n");

    if(isShutdown) Shutdown();
    else Reset();
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

void CAgentService::LoadVariable()
{
	VAROBJECT 	*pObject;
	ENVENTRY	env_t;
	char		szBuffer[100];
	long		addr=0;
	int			n;
    BYTE        hwVer;

	// Initialize Variable
	VARAPI_Initialize(VARCONF_FILENAME, m_Root_node, TRUE);

	GetLocalAddress(szBuffer);
	addr = inet_addr(szBuffer);
	pObject = VARAPI_GetObjectByName("ethIpAddr");
	if (pObject != NULL) memcpy(pObject->var.stream.p, &addr, sizeof(IPADDR));

	memset(&env_t, 0, sizeof(ENVENTRY));
	GetEnvInfo(env_t.envKernelVersion, env_t.envGccVersion, env_t.envCpuName, &env_t.envCpuMips);

	pObject = VARAPI_GetObjectByName("envKernelVersion");
	if (pObject != NULL) strcpy(pObject->var.stream.p, env_t.envKernelVersion);

	pObject = VARAPI_GetObjectByName("envGccVersion");
	if (pObject != NULL) strcpy(pObject->var.stream.p, env_t.envGccVersion);

	pObject = VARAPI_GetObjectByName("envCpuName");
	if (pObject != NULL) strcpy(pObject->var.stream.p, env_t.envCpuName);

	pObject = VARAPI_GetObjectByName("envCpuMips");
	if (pObject != NULL) pObject->var.stream.u32 = HostToLittleShort(env_t.envCpuMips);

    /** Rewriter(Q) : testmode가 무엇인지 확인해야 함 */
	GetFileValue("/app/conf/testmode", &n);
	m_bGprsPerfTestMode = (n > 0) ? TRUE : FALSE;

    if((hwVer = LoadHwVersion()) > 0x00)
    {
        // HW Version 정보가 있다면 읽어온다
        // HW Version 정보는 Kernel boot parameter에 hwver=1.2 의 형식으로 등록하면 된다
        m_nHwVersion = hwVer;

	    pObject = VARAPI_GetObjectByName("sysHwVersion");
	    if (pObject != NULL)  pObject->var.stream.u8 = hwVer;
    }

    if(GetFileString("/app/conf/mcusystype", szBuffer, sizeof(szBuffer), "OUTDOOR", TRUE)
            && strncasecmp(szBuffer,"INDOOR",strlen("INDOOR"))==0) {
        m_nSystemType = MCU_TYPE_INDOOR;
    }

    /*-- Pulse Meter Type을 읽어온다 --*/
    LoadSensorType();

    /*-- Patch 5-1949 --*/
#if     defined(__PATCH_5_1949__)
    m_nDebugLevel &= (*m_pbPatch_5_1949 ? 0x00 : 0xFF);
#endif
    
}

/** Issue #1286 종료전에 Battery Fail Status를 검사 한다.
 *  이 method가 호출되면 Battery Charging register를 off/on 시키기 때문에 운영중에
 *  호출하지 않고 Reset, Shutdown 시 최종적으로 호출한다.
 *
 */
void CAgentService::CheckBattery()
{
    unsigned int batteryAdcValue = 0;

    // Battery Charging resgiter off
    GPIOAPI_WriteGpio(GP_BATT_CHG_EN_OUT, 1); USLEEP(500000);
    // Read Battery ADC Value
    batteryAdcValue = GPIOAPI_ReadAdc(ADC_BATT_VOL_INPUT);
    // Battery Charging resgiter on
    GPIOAPI_WriteGpio(GP_BATT_CHG_EN_OUT, 0);
    XDEBUG("Check Battery ADC Value %d\r\n", batteryAdcValue);
    if(batteryAdcValue < 2000)
    {
        XDEBUG(ANSI_COLOR_RED "Battery Status : Abnormal\r\n" ANSI_NORMAL);
        // Battery Fail
        mcuBatteryFailEvent();
        USLEEP(1000000*10); // Event 전송을 위해 10초간 대기
    }
}

void CAgentService::Reset()
{
    // Battery Check
    CheckBattery();
    return;

    SystemExec("sync");
    SystemExec("sync");
    SystemExec("sync");
    SystemExec("reboot");
}

void CAgentService::Shutdown()
{
#if defined(__TI_AM335X__)
    // Battery Check
    CheckBattery();
    return;
#endif

    SystemExec("sync");
    SystemExec("sync");
    SystemExec("sync");

#if 0
    /** TODO Issue #959 ~2341 : TI AM3352 Board에서 poweroff 명령이
     *  정상 동작하지 않는다. 이 문제를 해결하지 못했다.
     *  현재 AC 전원이 차단된 상태에서 reboot를 할 경우 자동으로
     *  전원이 차단되고 있다. 따라서 reboot로 변경한다.
     *  추후에 정상적인 poweroff 또는 shutdown -h 로 변경할 수 
     *  있어야 한다.
     */
    SystemExec("poweroff");
#else
    SystemExec("reboot");
#endif
}

