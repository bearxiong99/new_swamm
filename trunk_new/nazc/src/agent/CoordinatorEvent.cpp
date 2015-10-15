#include "common.h"
#include "AgentService.h"
#include "EventInterface.h"
#include "MeterWriter.h"
#include "EndDeviceList.h"
#include "BatchJob.h"
#include "Ondemander.h"
#include "PowerOutage.h"
#include "TransactionManager.h"
#include "varapi.h"
#include "Utility.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "gpiomap.h"
#include "CommonUtil.h"
#include "AgentLog.h"
#include "MeterParserFactory.h"

#include "ZigbeeServer.h"

#include "InventoryHash.h"

#include "codeUtility.h"
#include "mbusFrame.h"

#include "Patch.h"

//////////////////////////////////////////////////////////////////////////
//  [4/19/2011 DHKim]
//  SQLite
//////////////////////////////////////////////////////////////////////////
#include "sqlite3.h"
#include "CppSQLite3.h"

#ifdef __SUPPORT_SITE_KETI__
/**< Issue #2571 KETI */
#include "Keti_3Party.h"
#endif

//////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////

extern BOOL	m_bEnableAutoRegister;
extern int	m_nSensorLimit;
extern BYTE m_nScanningStrategy;
extern UINT m_nJoinNodeCount;
extern BYTE m_nOpMode;

extern BOOL m_bSuperParent;
extern BYTE m_nCoordinatorFwVersion;
extern BYTE m_nCoordinatorFwBuild;

extern BYTE m_nTimeSyncStrategy;

extern BOOL m_bEnableJoinControl;


#if     defined(__PATCH_8_2305__)
extern BOOL    *m_pbPatch_8_2305;
#endif
#if     defined(__PATCH_11_65__)
extern BOOL    *m_pbPatch_11_65;
#endif

int 	m_nCoordinatorResetCount = 0;
BOOL	m_bFirstStackup = FALSE;

//////////////////////////////////////////////////////////////////////
// Coordinator power control function 
//////////////////////////////////////////////////////////////////////

void CoordinatorPowerControl()
{
#if !defined(__EXCLUDE_GPIO__)
#ifdef  __SUPPORT_NZC1__
    GPIOAPI_WriteGpio(GP_CODI_PCTL_OUT, GPIO_HIGH );
#endif
    GPIOAPI_WriteGpio(GP_CODI_RST_OUT, GPIO_LOW );
    USLEEP(100000);
    GPIOAPI_WriteGpio(GP_CODI_RST_OUT, GPIO_HIGH );
#ifdef  __SUPPORT_NZC1__
    USLEEP(100000);
    GPIOAPI_WriteGpio(GP_CODI_PCTL_OUT, GPIO_LOW );
#endif
#endif
}              

BOOL CoordinatorPermitControl(BYTE permit_time)
{
	COORDINATOR		*pCoordinator;
	CODI_PERMIT_PAYLOAD		permit;
	permit.permit_time = permit_time;
	codiSetProperty(GetCoordinator(), CODI_CMD_PERMIT, (BYTE *)&permit, sizeof(CODI_PERMIT_PAYLOAD), 3000);
    XDEBUG("Coordinator Permit Control SET (PERMIT=%d)\r\n", permit_time);
    pCoordinator = (COORDINATOR *)GetCoordinator();
    if (pCoordinator != NULL) pCoordinator->permit_time = permit_time; 
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//  [4/19/2011 DHKim]
//  Join Control Function
//////////////////////////////////////////////////////////////////////////
BOOL bjoincontrol(const char* szID)
{
	try
	{
		CppSQLite3DB db;
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t;
		char reszID[17] = {0, };
		db.open("/app/sqlite/EndiJoinTable.db");
		db.execDML("CREATE TABLE IF NOT EXISTS jointable (joinkey INTEGER PRIMARY KEY, EUI64ID CHAR(17), jointry INTEGER);");

		memcpy(reszID, szID, sizeof(reszID));
		bufSQL.format("SELECT * FROM jointable WHERE EUI64ID=%Q;", reszID);
		t = db.getTable(bufSQL);

		if( t.numRows() == 0 )
			return FALSE;
		else
			return TRUE;

		db.close();
	}
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG("ERROR: %s\r\n",e.errorMessage());
		//return e.errorCode();
		return FALSE;
	}
}

BOOL EndDeviceRegister(HANDLE codi, EUI64 *id, BYTE LQI, signed char RSSI)
{
	char	szGUID[17];
	COORDINATOR		*pCoordinator;
    BOOL    bRegister = FALSE;
	// 센서 등록 제한이 있는지 확인하여 등록 제한을 초과하는지 검사한다.
	if ((m_nSensorLimit >= 0) && (m_pEndDeviceList->GetCount() >= m_nSensorLimit))
	{
	    pCoordinator = (COORDINATOR *)GetCoordinator();
	    if (pCoordinator != NULL && pCoordinator->permit_time) { 
            CoordinatorPermitControl(0);
        }
	}

    /** Issue #1541 : Pulse Mesh Network에서는 Keepalive / State Frame에 의한 Inventory Scanning을
     *  수행하지 않는다. 오직 Metering Data에 의해서만 Sensor가 등록될 수 있다.
     */
    if (m_nOpMode == OP_MODE_PULSE_MESH) {
	    XDEBUG(" OP MODE Pulse Mesh \r\n");
		return bRegister;
    }

    /** Issue #1149 : Sensor 에서 AMR Frame이 올라오면 EndiList에서 현재 관리중인 목록인지 검사해서
     * 현재 관리중인 Sensor가 아니면 InventoryScan 절차에 들어간다(Join과 같이 처리한다)
     * 이를 통해 미아노드를 방지할 수 있다.
     */
	// 자동 등록 모드이면 Inventory scanning을 수행한다.
	if (!m_bEnableAutoRegister)
		return bRegister;

	// 센서가 등록되어 있는지 확인후 등록되지 않은 경우 Inventory Scanning울 수행한다.
	if (m_pEndDeviceList->GetEndDeviceByID(id) == NULL)
	{
	    eui64toa(id, szGUID);
	    XDEBUG("Lost Sensor KEEP-ALIVE(ID=%s)\r\n", szGUID);
		//  [4/20/2011 DHKim]
		if( bjoincontrol(szGUID) || !m_bEnableJoinControl )
		{
			m_pInventoryScanner->Add(id, LQI, RSSI);
			bRegister = TRUE;
		}
		else
		{
			XDEBUG("%s Join Reject!!\r\n", szGUID);
			m_pInventoryScanner->Add(id, 0, 0, SCAN_MASK_LEAVE, 33000);
			return bRegister;
		}
	}
    return bRegister;
}

//////////////////////////////////////////////////////////////////////
// Coordinator callback functions 
//////////////////////////////////////////////////////////////////////

void codiOnRegister(HANDLE codi, CODIDEVICE *device)
{
	// Coordinator가 등록될때 불린다.
}

void codiOnUnregister(HANDLE codi, CODIDEVICE *device)
{
	// Coordinator가 등록 해제될때 불린다.
}

void codiOnStartup(HANDLE codi)
{
}

void codiOnShutdown(HANDLE codi)
{
	// Coordinator가 Shutdown될때 불린다.
}

void codiOnBoot(HANDLE codi, EUI64 *id, WORD shortid, BYTE fw, BYTE hw, BYTE zaif, BYTE zzif, BYTE build, BYTE rk)
{
    // Super Parent Network 상태인지 설정한다
    if(((fw & 0x0F) % 2) == 0) m_bSuperParent = TRUE;
    else m_bSuperParent = FALSE;

    m_nCoordinatorFwVersion = fw;
    m_nCoordinatorFwBuild = build;

	// Reset event를 전송한다.
	sinkResetEvent(id, rk);
}

void codiOnStackStatus(HANDLE codi, BYTE status)
{
	FILE *fp;

	// 스택의 상태가 전달되어 온다.
	if ((status == CODI_STACK_UP) && !m_bFirstStackup)
	{
	    COORDINATOR		*pCoordinator = (COORDINATOR *)GetCoordinator();
        BYTE            nTimeSyncType = TIMESYNC_TYPE_LONG;

		m_bFirstStackup = TRUE;

	    codiSync(pCoordinator, 3);
		// 센서 등록 제한이 있는지 확인하여 등록 제한을 초과하는지 검사한다.
		if ((m_nSensorLimit >= 0) && (m_pEndDeviceList->GetCount() >= m_nSensorLimit))
		{
	        if (pCoordinator != NULL && pCoordinator->permit_time) { 
                CoordinatorPermitControl(0);
            }
		}

        m_nJoinNodeCount = 0;

        /*-- Issue #2033 : TimeSync Type이 추가되었다. Stackup이 되었을 때
         *                 각 TimeSycn Strategy 및 Op Mode에 따라 다른 Type의
         *                 TimeSync Message를 보낸다.
         *   + sleepy mesh : 항상 Short
         *   + hybrid mesh : 항상 Long
         *   + hybrid + sleepy mesh : Normal (Long), Test (Short)
         */

        switch (m_nTimeSyncStrategy) {
            case TIMESYNC_STRATEGY_SLEEPY_MESH:
                nTimeSyncType = TIMESYNC_TYPE_SHORT;
                break;
            case TIMESYNC_STRATEGY_HIBRID_SLEEPY_MESH:
                if(m_nOpMode == OP_MODE_TEST) nTimeSyncType = TIMESYNC_TYPE_SHORT;
                break;

        }
		
        m_pSensorTimesync->SyncDiscovery();

        {
        /** Issue2 #402 : 2006년 전의 시간일 경우 시간 동기화를 수행하지 않는다.
          */
        time_t          now;
	    struct	tm		when;

	    time(&now);
	    when = *localtime(&now);
        if(when.tm_year + 1900 < 2006) return;
        }

        m_pSensorTimesync->Timesync(nTimeSyncType);
	
	}
	if( (fp = fopen("/app/conf/keyTable.txt", "r")) != NULL)
	{
		m_pBatchJob->Add(BATCH_TABLE_JOIN, (void *)NULL, FALSE, 3); 
	}	
}

void codiOnDeviceJoin(HANDLE codi, EUI64 *id, WORD shortid, WORD parentid, BYTE status)
{
	char	szGUID[17];
	COORDINATOR		*pCoordinator;
    m_nJoinNodeCount ++;

    // Issue #993 잘못된 수정으로 sensorJoinEvent 가 발생하지 않는 버그 수정
	sensorJoinEvent(id);

	// 센서 등록 제한이 있는지 확인하여 등록 제한을 초과하는지 검사한다.
	if ((m_nSensorLimit >= 0) && ((m_pEndDeviceList->GetCount() >= m_nSensorLimit) ||
            m_nJoinNodeCount >= (UINT)m_nSensorLimit))
	{
	    pCoordinator = (COORDINATOR *)GetCoordinator();
	    if (pCoordinator != NULL && pCoordinator->permit_time) { 
            CoordinatorPermitControl(0);
        }
	}

    // 운영모드가 Pulse-Mesh일 경우 즉시 시간동기화를 시도한다.
    /*-- Issue #2033 : 더이상 Pulse-Mesh Mode를 지원하지 않는다.
     * if(m_nOpMode == OP_MODE_PULSE_MESH) {
     *     m_pSensorTimesync->Timesync();
     * }
    ---*/
    /*-- Issue #2033 : Hybrid + Sleepy Mesh Network일 경우 시간 동기화를 시도한다 --*/
    if(m_nTimeSyncStrategy == TIMESYNC_STRATEGY_HIBRID_SLEEPY_MESH) {
        m_pSensorTimesync->NeedBroadcastTime();
    }
	
	// Inventory scanning을 하지 않는 센서들을 위하여 보관한다.
	// 검침 데이터를 수신하여 센서가 등록될때 이정보가 사용되어진다.
	if (codiIsSuperParent(codi))
		m_pInventoryHash->Add(id, parentid, shortid, status);
	
    /* Issue #1286: Scanning Strategy에 따라 Join 시 Inventory Scanning을 수행할 지 안할지 결정한다 */
    switch(m_nScanningStrategy) {
        case SCANNING_STRATEGY_IMMEDIATELY:
	        // 자동 등록 모드이면 Inventory scanning을 수행한다.
	        if (!m_bEnableAutoRegister)
		        return;

            // 운영모드가 Pulse-Mesh일 경우 Immediately Scanning을 지원하지 않는다.
            if (m_nOpMode == OP_MODE_PULSE_MESH)
                return;

	        // 센서가 등록되어 있는지 확인후 등록되지 않은 경우 Inventory Scanning울 수행한다.
	        if (m_pEndDeviceList->GetEndDeviceByID(id) == NULL)
	        {
	            eui64toa(id, szGUID);
                m_pEndDeviceList->UpdateRouteRecord(id, shortid, 0, NULL);

				if( bjoincontrol(szGUID) || !m_bEnableJoinControl)
				{
	                XDEBUG("NETWORK JOIN(ID=%s)\r\n", szGUID);
/*-- Pulse Meter가 지원될 때는 120초(#1622)/기타는 6초의 timeout을 가진다 --*/
#if     defined(__SUPPORT_ENDDEVICE__)
			        m_pInventoryScanner->Add(id, (BYTE)0, (signed char)0, SCAN_MASK_ALL, 120000);
#else
			        m_pInventoryScanner->Add(id, (BYTE)0, (signed char)0, SCAN_MASK_ALL, 6000);
#endif
				}
				else
				{
	                XDEBUG("NETWORK JOIN REJECT(ID=%s)\r\n", szGUID);
/*-- Pulse Meter가 지원될 때는 120초(#1622)/기타는 6초의 timeout을 가진다 --*/
#if     defined(__SUPPORT_ENDDEVICE__)
					m_pInventoryScanner->Add(id, 0, 0, SCAN_MASK_LEAVE, 120000);
#else
					m_pInventoryScanner->Add(id, 0, 0, SCAN_MASK_LEAVE, 6000);
#endif
				}
	        }
            break;
    }
}

void codiOnScanComplete(HANDLE codi, BYTE channel, BYTE status)
{
	// Coordinator는 Scan을 사용하지 않으므로 사용 하지 않는다.
}

void codiOnError(HANDLE codi, BYTE code)
{
	// Coordinator에서 명령을 처리할때 오류가 발생하면 불린다.
}

void codiOnCommand(HANDLE codi, BYTE ctrl, BYTE type, BYTE *pBuffer, int nLength)
{
	// 요청한 명령에 대한 응답이 온다.
}

void codiOnSend(HANDLE codi, BYTE ctrl, BYTE type, BYTE *payload, int nLength)
{
	// 자신이 전송한 메세지에 대한 콜백이 온다.
}

void codiOnCommandError(HANDLE codi, BYTE ctrl, BYTE cmd, BYTE code)
{
	// 요청한 명령에 대한 에러 코드가 온다.
}

void codiOnInformation(HANDLE codi, BYTE ctrl, BYTE type, BYTE *payload, int nLength)
{
    // Information Frame에 대한 User Handler
}

void codiOnNoResponse(HANDLE codi, BOOL bResetNow)
{
	// Coordinator가 무응답일때 온다.
	// 최초 무응답이후 30초후 호출 이후 10초마다 호출되어진다.
	// 계속 10초마다 호출되다 3분이 경과하면 bResetNow가 설정되어진다.
	// bResetNow가 설정된 이후에 다음 bResetNow는 또 3분후에 다시 설정된다.

	// 즉시 리셋인 경우 바로 리셋하여 준다.
	if (!bResetNow)
		return;
	// 부팅후 최대 24회 이상 리셋을 하지 않는다.
	m_nCoordinatorResetCount++;
	if (m_nCoordinatorResetCount <= 24)
	{
		printf("\r\n");
		printf("\033[1;40;31m------------------ COORDINATOR RESET --------------\033[0m\r\n");
		printf("\r\n");

    	CoordinatorPowerControl();
	}
	else
	{
		printf("\r\n");
		printf("\033[1;40;31m-------- COORDINATOR RESET (SKIP) COUNT=%d --------\033[0m\r\n",
				m_nCoordinatorResetCount);
		printf("\r\n");
	}
}

void codiOnRouteRecord(HANDLE codi, EUI64 *id, WORD shortid, BYTE hops, WORD *path)
{
	// 센서 목록에 Route record를 업데이트 한다. 
	m_pEndDeviceList->UpdateRouteRecord(id, shortid, hops, path);
}

BOOL codiOnQueryRouteRecord(HANDLE codi, EUI64 *id, WORD *shortid, BYTE *hops, WORD *path)
{
	// CODIAPI에서 요청하는 센서에 대한 Route record를 넘겨준다.
	if (!m_pEndDeviceList->QueryRouteRecord(id, shortid, hops, path))
	{
		// 관리하는 센서가 아닌 경우 Cache에서 검색한다.
		m_pInventoryScanner->GetCacheRouteRecord(id, shortid, hops, path);
	}
	return TRUE;
}

BOOL codiOnQuerySuperParent(EUI64 *id, EUI64 *parent, WORD *shortid, BYTE *hops, WORD *path)
{
	ENDIENTRY	*pEndDevice, *pParentDevice;	 	
	pEndDevice = m_pEndDeviceList->GetEndDeviceByID(id);
	if (pEndDevice == NULL)
		return FALSE;

	pParentDevice = m_pEndDeviceList->GetEndDeviceByShortID(pEndDevice->parentid);
	if (pParentDevice == NULL)
		return FALSE;

	memcpy(parent, &pParentDevice->id, sizeof(EUI64));

    m_pEndDeviceList->QueryRouteRecord(pParentDevice, shortid, hops, path);

	return TRUE;
}

void endiOnConnect(HANDLE codi, HANDLE endi)
{
	// End Device와 연결될때 온다.
}

void endiOnDisconnect(HANDLE codi, HANDLE endi)
{
	// End Device 접속이 끈길때 온다.
}

void codiOnEnergyScan(HANDLE codi, BYTE channel, BYTE rssi)
{
	// Coordinator에서 Scan하지 않는다.
}

void codiOnActiveScan(HANDLE codi, BYTE channel, WORD panid, BYTE *extpanid, BYTE expjoin, BYTE profile)
{
	// Coordinator에서 Active Scan은 하지 않는다.
}

void endiOnSend(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE type, BYTE seq, BYTE *payload, int nLength)
{
	// End Device에 데이터를 전송하면 온다.
	// endi == NULL 이면 MULTICAST, NULL이 아니면 UNICAST이다.
}

void endiOnError(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE type, BYTE code)
{
	// 통신에 대한 에러가 수신되는 부분
    XDEBUG(ANSI_COLOR_RED "endiOnError code=%d\r\n" ANSI_NORMAL, code);
}

void endiOnBypass(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE	*pEndDevice;

    // Thread Debugging
    __PTHREAD_INFO__;
	// 미터로 부터 수신한 데이터가 수신되는 부분
	pEndDevice = (ENDDEVICE *)endi;
	if(m_pOndemander->OnData(&pEndDevice->id, seq, payload, nLength) == FALSE)
    {
        ENDIENTRY *pEntry;
        CMeterParser *parser;
        /* Ondemand Operation이 없으면 parser를 찾아서 OnData를 호출 한다 */
        pEntry = m_pEndDeviceList->GetEndDeviceByID(&pEndDevice->id);
        if(pEntry != NULL)
        {
            parser =  m_pMeterParserFactory->SelectParser(pEntry->szParser);
            if(parser != NULL)
            {
                parser->OnData(&pEndDevice->id, NULL, seq, payload, nLength);
            }
        }
    }
}

void endiOnNanData(HANDLE codi, EUI64 *id, BYTE reqid, BYTE flow, BYTE tailframe, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
	char	szGUID[17];
    // NAN API Protocol Data 수신		 	
    memset(szGUID,0,sizeof(szGUID));
	eui64toa(id, szGUID);
	XDEBUG("NAN Frame (%s) reqid=%d flow=%s tail=%c seq=%d\r\n", szGUID, reqid, flow ? "Res" : "Req", tailframe ? 'T' : 'F', seq);
    XDUMP((char *)payload, nLength, TRUE);
	m_pZigbeeServer->OnNanData(codi, id, reqid, tailframe, seq, payload, nLength);
}
 
void endiOnPush(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
    char	szGUID[17];
	char    szTemp[6*CODI_MAX_ROUTING_PATH+1], ch[6];
    ENDIENTRY *pEntry;
    WORD    path[CODI_MAX_ROUTING_PATH];
    BYTE    hops=0;
    WORD    shortid=0;
    BYTE    mode, flow;
    BYTE    nCurr=0;
	time_t  now;
    BOOL    isLast = FALSE;
    int     i;
	 	
    memset(szGUID,0,sizeof(szGUID));
    eui64toa(id, szGUID);
    XDEBUG("PUSH Frame (%s) ctrl=0x%02X seq=%d\r\n", szGUID, ctrl, seq);
    //XDUMP((char *)payload, nLength, TRUE);

    mode = GET_FRAME_MODE(ctrl);
    flow = GET_FRAME_FLOW(ctrl);

    /** Read Response가 아니면 리턴 한다. */
    if((mode != CODI_ACCESS_READ) || (flow != CODI_FLOW_RESPONSE)) 
    {
        return;
    }

    pEntry = m_pEndDeviceList->GetEndDeviceByID(id);
    if(pEntry == NULL)
    {
        /** 아직 등록되지 않았다면 Scanning을 수행한다 */
        EndDeviceRegister(codi, id, LQI, RSSI);
        return;
    }
    
	time(&now);
    /**
     *  - DB에 저장하기 위해 DB Writer를 이용해야 한다
     *  - LP 저장은 File로 MeterDbWriter에서 DB로 저장
     */
    switch(seq)
    {
        case 0:
            {
            ENDI_PUSH_HEADER *pHeader;
            ENDI_PUSH_EVT_HEADER *pEvtHeader;
            ENDI_PUSH_LPINFO *pLpInfo;
            BYTE nBasePulseSize=0;

            /** Issue #84: Metering 관련 정보를 초기화 한다 */
            m_pLpWriter->ClearData(id);

            /** Issue #84 Push Metering Header를 분석해서 필요한 값을 설정한다 */
            pHeader = (ENDI_PUSH_HEADER *)payload;

            switch((pHeader->frameType >> 6)&0x03)
            {
                case 0: // LP

                    XDEBUG(" PUSH : LP, port %d, endian %s, len %d, meter %s, relay %s [%04d/%02d/%02d %02d:%02d:%02d]\r\n"
                        "        cpLen %d, lpLen %d, eLevel %d, chCnt %d\r\n",
                            pHeader->portNum,
                            (pHeader->frameType & 0x20) ? "BIG" : "LITTLE",
                            BigToHostShort(pHeader->length), 
                            (pHeader->meterStatus  & 0x80) ? "Bat" : "Elec",
                            (pHeader->meterStatus  & 0x40) ? "yes" : "no",
                            pHeader->currentTime.year + 2000,
                            pHeader->currentTime.mon,
                            pHeader->currentTime.day,
                            pHeader->currentTime.hour,
                            pHeader->currentTime.min,
                            pHeader->currentTime.sec,
                            (pHeader->dataFormatLength >> 4) & 0x0F,
                            pHeader->dataFormatLength & 0x0F,
                            pHeader->energyLevel,
                            pHeader->channelCount
                        );

                    nBasePulseSize = (pHeader->dataFormatLength>>4)&0x0F;
                    pLpInfo = (ENDI_PUSH_LPINFO *)(payload 
                            + sizeof(ENDI_PUSH_HEADER) 
                            + (nBasePulseSize * pHeader->channelCount));
            
                    /** 전체 Metering Data Size Update */
                    m_pEndDeviceList->SetMeteringAttribute(id, UNDEFINED_PORT_NUMBER,
                        LAST_DATA_MASK_SIZE, (int)(60/pLpInfo->lpInterval), 0, -1);

                    /** nLength를 전달하지만 실제 값이 설정되지는 않는다.
                    * IsValidLpIndex에서 length 값을 더한다
                    */
                    m_pEndDeviceList->SetMeteringDataSize(id, BigToHostShort(pHeader->length), nLength);

                    break;
                case 1: // Event
                    pEvtHeader = (ENDI_PUSH_EVT_HEADER *)payload;

                    XDEBUG(" PUSH : EVT, port %d, endian %s, len %d, categoryCnt %d [%04d/%02d/%02d %02d:%02d:%02d]\r\n",
                            pEvtHeader->portNum,
                            (pEvtHeader->frameType & 0x20) ? "BIG" : "LITTLE",
                            BigToHostShort(pEvtHeader->length), 
                            pEvtHeader->categoryCount,
                            pEvtHeader->currentTime.year + 2000,
                            pEvtHeader->currentTime.mon,
                            pEvtHeader->currentTime.day,
                            pEvtHeader->currentTime.hour,
                            pEvtHeader->currentTime.min,
                            pEvtHeader->currentTime.sec
                        );

                    m_pEndDeviceList->SetMeteringAttribute(id, UNDEFINED_PORT_NUMBER, LAST_DATA_MASK_SIZE, 0, 0, -1);

                    m_pEndDeviceList->SetMeteringDataSize(id, BigToHostShort(pEvtHeader->length), nLength);
                    break;
            }
            m_pTransactionManager->Notify(id);
            }
            break;
        default:
            break;
    }

    memset(szTemp, 0, sizeof(szTemp));
    /** Path 정보 획득 */
	if (!m_pEndDeviceList->QueryRouteRecord(pEntry, &shortid, &hops, path)) {
	 if(m_pInventoryScanner->GetCacheRouteRecord(id, &shortid, &hops, path)) {
	        m_pEndDeviceList->UpdateRouteRecord(id, shortid, hops, path);
        }
    }
	for(i=0; i<hops; i++)
	{
	 if (i) sprintf(ch, " %04X", path[i]);
	 else   sprintf(ch, "%04X", path[i]);
        strcat(szTemp, ch);
	}

    if(m_pEndDeviceList->IsValidLpIndex(id, seq, nLength, &nCurr, &isLast)) {
        if(m_pLpWriter->AddData(id, (BYTE *) payload, nLength, isLast, pEntry, m_pMeterDbWriter)) {
            if(isLast) {
	            m_pEndDeviceList->SetLastMeteringTime(id, now);
                m_pEndDeviceList->CommitMeteringDate(id);
	            m_pEndDeviceList->SetState(id, ENDISTATE_NORMAL);
	            m_pEndDeviceList->VerifyEndDevice(id);

                METERING_LOG("%s [%s] Metering ok. shortid=%04X, hop=%d path=%s\r\n",
                    szGUID, pEntry->szSerial, pEntry->shortid, pEntry->hops, szTemp);
            }
        }else {
	        m_pEndDeviceList->SetLastMeteringFailTime(id, now);
	        m_pEndDeviceList->SetState(id, ENDISTATE_METER_ERROR);
	        XDEBUG(" TEMP LP DATA SAVE FAIL ID=%s\r\n", szGUID);
	        m_pEndDeviceList->VerifyEndDevice(id);
            m_pLpWriter->ClearData(id);

            METERING_LOG("%s [%s] Metering fail. (LP Data Save Error) shortid=%04X, hop=%d path=%s\r\n",
                szGUID, pEntry->szSerial, pEntry->shortid, pEntry->hops, szTemp);
        }
    }else {
	    m_pEndDeviceList->SetLastMeteringFailTime(id, now);
	    m_pEndDeviceList->SetState(id, ENDISTATE_METER_ERROR);
	    m_pEndDeviceList->VerifyEndDevice(id);
        m_pLpWriter->ClearData(id);
	    XDEBUG(" INVALID METERING FRAME ID=%s EXPIDX=%d\r\n", szGUID, nCurr);

        METERING_LOG("%s [%s] Metering fail. (Invalid Index. ExpIdx=%d, Idx=%d) shortid=%04X, hop=%d path=%s\r\n",
            szGUID, pEntry->szSerial, nCurr, seq, pEntry->shortid, pEntry->hops, szTemp);
    }
}

void endiOnAmrData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// AMR Data를 읽은 결과 Notify
}

void endiOnKeepAlive(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{		
    EndDeviceRegister(codi, id, (BYTE)0, (signed char)0);
    m_pTransactionManager->Notify(id);
}

void endiOnNetworkLeave(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LEAVE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// 집중기는 Network를 Leave하지 않는다.
}

void endiOnRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// ROM Read response
}

void endiOnLongRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LONG_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// ROM Read response
}

void endiOnMccbData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_MCCB_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// Mccb 제어 결과 Notify
}

void endiOnCommand(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_COMMAND_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	// EndiDevice Command response
}

void endiOnState(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_STATE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI, WORD shortid)
{
    BOOL bRegister = FALSE;
	char	szGUID[17];	 		
    memset(szGUID,0,sizeof(szGUID));
	eui64toa(id, szGUID);
    if(!shortid) {
	    XDEBUG("STATE Notification (ID=%s) : LQI=%d, RSSI=%d\r\n", szGUID, payload->LQI, payload->RSSI);
    } else {
	    XDEBUG("STATE Notification (ID=%s) : LQI=%d, RSSI=%d, SID=%04X\r\n", szGUID, payload->LQI, payload->RSSI, ntoshort(shortid));
    }

    /** Issue #1871: STATE Frame에 Repeater에 대한 Permit 정보 및 Child 정보, Solar Repeater 정보가 
     *  전달되면 표시한다.
     */
    if(nLength > 2) {
        XDEBUG("  PERMIT_MODE=%s, PERMIT_STATE=%s, MAX_CHILD=%d, CHILD_CNT=%d\r\n",
            payload->permit_mode ? "Enable" : "Disable",
            payload->permit_state ? "On" : "Off",
            payload->max_child_cnt,
            payload->child_cnt);
        XDEBUG("  SOLAR_VOLT=%.4f, CHG_BATT_VOLT=%.4f, BOARD_VOLT=%.4f, CHG_STATE=%s\r\n",
            (double)(ntoshort(payload->solar_volt) * 2) / 10000.0,
            (double)(ntoshort(payload->solar_chg_bat_volt)) / 10000.0,
            (double)(ntoshort(payload->solar_board_volt)) / 10000.0,
            payload->battery_chg_state ? "HIGH" : "LOW");
    }

    bRegister = EndDeviceRegister(codi, id, payload->LQI, payload->RSSI);

    if(!bRegister) {
        m_pEndDeviceList->SetQuality(id, payload->LQI, payload->RSSI);
        /** Issue #1871: STATE Frame에 Repeater에 대한 Permit 정보 및 Child 정보, Solar Repeater 정보가 
        *  전달되면 Update 한다.
        */
        if(nLength > 2) {
            m_pEndDeviceList->SetRouteDeviceInfo(id, 
                    payload->permit_mode ? TRUE : FALSE,
                    payload->permit_state ? TRUE : FALSE,
                    payload->max_child_cnt,
                    payload->child_cnt);
            m_pEndDeviceList->SetSolarDeviceInfo(id, 
                    ntoshort(payload->solar_volt),
                    ntoshort(payload->solar_chg_bat_volt),
                    ntoshort(payload->solar_board_volt),
                    payload->battery_chg_state ? 1 : 0);
        }
    }

    m_pTransactionManager->Notify(id);
}

void endiOnDiscovery(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_DISCOVERY_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	char	szGUID[17];
	 		
    memset(szGUID,0,sizeof(szGUID));
	eui64toa(id, szGUID);
	XDEBUG("DISCOVERY Notification (ID=%s, CMD_TYPE=%s)\r\n", szGUID, 
        payload->cmd_type == ENDI_CMD_DISCOVERY_REQUEST ? "DISCOVERY_REQUEST" : "DISCOVERY_RESPONSE");

    if(payload->cmd_type == ENDI_CMD_DISCOVERY_REQUEST)
        m_pBatchJob->Add(BATCH_DISCOVERY_RESPONSE);
}

void MBusLpWriterCallbackHandler(EUI64 *id, BYTE *data, UINT nLength)
{
    MBUS_METERING_PAYLOAD * mbus = (MBUS_METERING_PAYLOAD *) data;
    MBUS_LONG_FRAME * longFrame;
    MBUS_FIXED_DATA_HEADER * fixedHeader;

    char szGUID[17];
    char szSerial[32+1];
    char szVendor[32+1];    
    char szVendorCode[4];    
    //BYTE nMeterType;
    BYTE szBuffer[32+1];
    BYTE nVendor;
		
    if(!data || !id) return;

    memset(szGUID, 0, sizeof(szGUID));
    memset(szSerial, 0, sizeof(szSerial));
    memset(szBuffer, 0, sizeof(szBuffer));
    memset(szVendor, 0, sizeof(szVendor));
    memset(szVendorCode, 0, sizeof(szVendorCode));

    longFrame = (MBUS_LONG_FRAME *) mbus->data;
    fixedHeader = (MBUS_FIXED_DATA_HEADER *) longFrame->data;

    eui64toa(id, szGUID);
    XDEBUG(" LpWriterCallback Call : ID=%s TYPE=MBUS LEN=%d\r\n", szGUID, nLength);

    //nMeterType= mbus->meterType;
    ReverseBcd(fixedHeader->customer, (BYTE *)szBuffer, sizeof(fixedHeader->customer));
    sprintf(szSerial,"%llu", BcdToInt((BYTE *)szBuffer, sizeof(fixedHeader->customer)));
    ConvManufacturerCode((const char *)fixedHeader->manufacturer, szVendorCode);
    nVendor = GetManufacturerVendorCode(szVendorCode);
    sprintf(szVendor, "%s", GetManufacturerString(szVendorCode));

    XDEBUG(" Meter ID %s Manufacturer %s (%s %02X)\r\n", szSerial, szVendor, szVendorCode, nVendor);

	m_pEndDeviceList->UpdateMeterInventory(id, longFrame->hdr.addr, PARSER_TYPE_MBUS, 
            "NURI", METER_VENDOR_NURI, "NAMR-H101MG", szSerial, 0, 0);
    m_pMeterDeviceList->Add(id, longFrame->hdr.addr, szSerial, (BYTE *)szVendor,
            fixedHeader->generation, fixedHeader->medium,
            fixedHeader->access, fixedHeader->status);
}

/** mtype : 0 (OLD MBUS Protocol)
  * mtype : 1 (NEW MBUS Protocol)
  */
void endiOnMbusData(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE mseq, BYTE mtype, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
    ENDIENTRY   *pEntry;
    char        szGUID[17];
    time_t      now;

    CMeterParser *parser = NULL;
	COORDINATOR		*pCoordinator;
	char	    szTemp[6*CODI_MAX_ROUTING_PATH+1], ch[6];

    BYTE        curr;
    BOOL        isLast=FALSE, isDuplicate;
    int         i;
	BYTE	    nType, nService;
    char        * pszModel = const_cast<char *>("NAMR-H101MG");
    WORD        path[CODI_MAX_ROUTING_PATH];
    BYTE        hops=0;
    WORD        shortid=0;
    BYTE        szBuffer[128];
    MBUS_METERING_PAYLOAD * info = NULL;
    BOOL        bInstall = FALSE;
    BYTE        nLastDataFrame = LAST_DATA_MASK_MSB;

    eui64toa(id, szGUID);

    pEntry = m_pEndDeviceList->GetEndDeviceByID(id);
    if (pEntry == NULL)
    {
		// 자동 등록 모드가 아니면 SKIP한다.
		if (!m_bEnableAutoRegister) {
            XDEBUG(" DISABLE AUTO RESISTER : ID=%s\r\n", szGUID);
			return;
        }

		// 센서 등록 제한이 있는지 확인하여 등록 제한을 초과하는지 검사한다.
		if ((m_nSensorLimit >= 0) && (m_pEndDeviceList->GetCount() >= m_nSensorLimit))
		{
	        pCoordinator = (COORDINATOR *)GetCoordinator();
	        if (pCoordinator != NULL && pCoordinator->permit_time) { 
                CoordinatorPermitControl(0);
            }
            if((m_nSensorLimit >= 0) && (m_pEndDeviceList->GetCount() > m_nSensorLimit)) {
                // 초과한 경우 Inventory scanner가 Leave 시키도록 호출하고 빠져나간다.
                m_pInventoryScanner->Add(id, 0, 0, SCAN_MASK_LEAVE, 33000);
                return;
            }
		}

		parser = m_pMeterParserFactory->SelectParser(pEntry->szParser);
		nType = parser ? parser->GetType() : SENSOR_TYPE_UNKNOWN;

		pEntry = m_pEndDeviceList->SetEndDeviceByID(id, UNDEFINED_PORT_NUMBER, "NAMR-H101MG",
                parser, parser->GetName(), PARSER_TYPE_MBUS,
                ENDDEVICE_ATTR_POWERNODE |  ENDDEVICE_ATTR_MBUS_MASTER | ENDDEVICE_ATTR_POLL);

		if (pEntry == NULL)
		{
   			XDEBUG("\033[1;40;31m ****** ERROR: SetEndDeviceByID error (ID=%s)\033[0m\r\n", szGUID);
			return;
		}

		XDEBUG("ONMETERING: VENDOR=%s, SERIAL=%s, MODEL=%s\r\n",
		    pEntry->szVendor, pEntry->szSerial, pEntry->szModel);

        /* Issue #1556: IF4 스펙상의 Service Type 정보를 얻어온다 */
		nService = parser ? parser->GetService() : SERVICE_TYPE_UNKNOWN;

        if(parser && parser->GetModel()) pszModel = parser->GetModel();
		m_pEndDeviceList->UpdateSensorInventory(id, parser->GetName(), PARSER_TYPE_MBUS, pszModel, 0, 0, 0);

        /* Issue #1287: MBUS Master가 처음 등록될 때 Install Event 전송 */
		sensorInstallEvent(id, nType, nService, pEntry->szSerial, pEntry->szVendor, pEntry->szModel, 
            ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_MBUS_MASTER);

        bInstall = TRUE;
    }

	if (!m_pEndDeviceList->QueryRouteRecord(pEntry, &shortid, &hops, path)) {
        /** 의 경우 최초 Install 시 EndDevice가 생성되기 전에 먼저 Route Record가 오게 된다.
         * 이 정보가 현재 저장되지 않고 있어서 최초 등록시에는 Route Record가 등록 안되고 있다.
         * 이 경우 Ondemand 가 실패하기 때문에 Inventory Scanner에 사용하는 Cash를 사용해야 한다.
         */
		if(m_pInventoryScanner->GetCacheRouteRecord(id, &shortid, &hops, path)) {
	        m_pEndDeviceList->UpdateRouteRecord(id, shortid, hops, path);
        }
    }

	*szTemp = '\0';
	for(i=0; i<hops; i++)
	{
		if (i) sprintf(ch, " %04X", path[i]);
		else   sprintf(ch, "%04X", path[i]);
        strcat(szTemp, ch);
	}

    if(bInstall) {
		m_pInventoryScanner->Add(id, LQI, RSSI);
    }
    
    switch(mseq) {
        case 0: // First Frame
            info = (MBUS_METERING_PAYLOAD *)szBuffer;

            switch(mtype)
            {
                case 0:
                    payload[0] = 0x00; /** Old Frame의 bankMask 값을 Skip 한다 */
                default:
                    memcpy((BYTE *)info, payload, nLength);
                    break;
            }

	        XDEBUG(" METERING ID=%s (%04d/%02d/%02d) TYPE=%s\r\n",
					szGUID,
					BigToHostShort(info->date.year),
					info->date.mon,
					info->date.day,
                    "MBUS");

            /** MBUS의 경우 lpPeriod, Choice, Base Puslse 는 고정 값을 넣는다 
             * 1Type의 경우 (Kamstrup 401/Compact) 예상되는 Frame Count를 LpPeriod로 추가한다.
             */
            switch(mtype)
            {
                case 0:     // Kamstrup 401/Compact
                    nLastDataFrame |= LAST_DATA_MASK_LPPERIOD;
                    m_pEndDeviceList->SetMeteringAttribute(id, UNDEFINED_PORT_NUMBER,
                            nLastDataFrame, (BYTE)(24 - 1), 1, -1);        // 24개의 Frame이 전송되어야 한다.
                    break;
                default:
                    m_pEndDeviceList->SetMeteringAttribute(id, UNDEFINED_PORT_NUMBER,
                            nLastDataFrame, (BYTE)0, 1, -1);         // MSB Flag Setting이 되어야 한다.
                    break;
            }

#if 0
            /** 검침데이터에 LPChoice 가 없기 때문에 중복 검사를 할 수 없다.
              * LPChoice 0 는 당일 검침이기 때문에 항상 저장해야 한다.
              * 따라서 idDuplicate는 항상 False가 나도록 한다
              *
            isDuplicate = m_pEndDeviceList->IsDuplicateLpData(id, ntoshort(info->lptime.year), info->lptime.mon, info->lptime.day);
              */
#endif
            isDuplicate = FALSE;
            XDEBUG(" DUPLICATE DATA CHECK=%s %s\r\n", szGUID, isDuplicate ? "TRUE" : "FALSE");

            if(!isDuplicate) {
                /** MBUS의 경우 lpPeriod, Choice, Base Puslse 는 고정 값을 넣는다 
                  */
                m_pEndDeviceList->SetPulseSensorLpTimePulse(id, &info->date, 0);
                m_pEndDeviceList->SetTempMeteringDate(id, BigToHostShort(info->date.year), info->date.mon, info->date.day);
	            m_pEndDeviceList->SetState(id, ENDISTATE_DATA_RECEIVE);
            }

	        m_pEndDeviceList->VerifyEndDevice(id);
            m_pLpWriter->ClearData(id);
            if(!isDuplicate) {
                m_pLpWriter->AddData(id, payload, nLength); // 구조체 크기만큼만 저장한다
            }
            m_pTransactionManager->Notify(id);
            break;
        default:
	        XDEBUG(" METERING ID=%s SEQ=%d)\r\n",szGUID, mseq);

            /** 검침데이터에 LPChoice 가 없기 때문에 중복 검사를 할 수 없다.
              * LPChoice 0 는 당일 검침이기 때문에 항상 저장해야 한다.
              * 따라서 idDuplicate는 항상 False가 나도록 한다
              *
            isDuplicate = m_pEndDeviceList->IsDuplicateLpData(id);
            */
            isDuplicate = FALSE;
            XDEBUG(" DUPLICATE DATA CHECK=%s %s\r\n", szGUID, isDuplicate ? "TRUE" : "FALSE");

            /*-- 중복 데이터면 처리하지 않고 리턴한다 --*/
            if(isDuplicate) return;

	        time(&now);
            if(m_pEndDeviceList->IsValidLpIndex(id, mseq, nLength, &curr, &isLast)) {
                if(m_pLpWriter->AddData(id, (BYTE *) payload, nLength, isLast, pEntry, 
                            m_pMeterFileWriter, MBusLpWriterCallbackHandler)) {
                    if(isLast) {
	                    m_pEndDeviceList->SetLastMeteringTime(id, now);
                        m_pEndDeviceList->CommitMeteringDate(id);
	                    m_pEndDeviceList->SetState(id, ENDISTATE_NORMAL);
	                    m_pEndDeviceList->VerifyEndDevice(id);

			            METERING_LOG("%s [%s] Metering ok. shortid=%04X, hop=%d path=%s\r\n",
                            szGUID, pEntry->szSerial, pEntry->shortid, pEntry->hops, szTemp);
                    }
                }else {
	                m_pEndDeviceList->SetLastMeteringFailTime(id, now);
	                m_pEndDeviceList->SetState(id, ENDISTATE_METER_ERROR);
	                XDEBUG(" TEMP LP DATA SAVE FAIL ID=%s\r\n", szGUID);
	                m_pEndDeviceList->VerifyEndDevice(id);
                    m_pLpWriter->ClearData(id);

			        METERING_LOG("%s [%s] Metering fail. (LP Data Save Error) shortid=%04X, hop=%d path=%s\r\n",
                        szGUID, pEntry->szSerial, pEntry->shortid, pEntry->hops, szTemp);
                }
            }else {
	            m_pEndDeviceList->SetLastMeteringFailTime(id, now);
	            m_pEndDeviceList->SetState(id, ENDISTATE_METER_ERROR);
	            m_pEndDeviceList->VerifyEndDevice(id);
                m_pLpWriter->ClearData(id);
	            XDEBUG(" INVALID METERING FRAME ID=%s EXPIDX=%d\r\n", szGUID, curr);

			    METERING_LOG("%s [%s] Metering fail. (Invalid Index. ExpIdx=%d, Idx=%d) shortid=%04X, hop=%d path=%s\r\n",
                    szGUID, pEntry->szSerial, curr, mseq, pEntry->shortid, pEntry->hops, szTemp);
            }
            break;
    }
}

void endiOnUserDefine(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE type, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
	char	szGUID[17];
	
    memset(szGUID,0,sizeof(szGUID));
	eui64toa(id, szGUID);
	XDEBUG("User Define Data (ID=%s, CTRL=0x%02X, TYPE=0x%02X, SEQ=%d, LEN=%d, LQI=%d, RSSI=%d)\r\n", szGUID, ctrl, type, seq, nLength, LQI, RSSI);
    XDUMP((char *)payload, nLength, TRUE);
}

void endiOnMetering(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_METERING_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDIENTRY	*pEntry;
	char		szGUID[17];
	char	    szTemp[6*CODI_MAX_ROUTING_PATH+1], ch[6];
    WORD        path[CODI_MAX_ROUTING_PATH];
    BYTE        hops=0;
    WORD        shortid=0;
	time_t		now;
    BYTE        curr;
    BOOL        isLast=FALSE, isDuplicate, isInstall=FALSE;
    int         i,idx=0,len=0,stLen=0;
	CMeterParser	*parser = NULL;
	BYTE	    nType, nService;
    char  * pszModel = const_cast<char *>("PULSE-BATTERY");
    char  * pszParser = NULL;
	COORDINATOR		*pCoordinator;
    BYTE        buffer[128];
    ENDI_METERING_INFO info;
    int         nAttr;
    WORD        tmpWV;
    UINT        tmpUV;

	// 가스 검침데이터
	eui64toa(id, szGUID);

	// 가스의 경우 센서가 등록되지 않았으면 무조건 등록한다.
	pEntry = m_pEndDeviceList->GetEndDeviceByID(id);
	if (pEntry == NULL)
	{
		// 자동 등록 모드가 아니면 SKIP한다.
		if (!m_bEnableAutoRegister) {
            XDEBUG(" DISABLE AUTO RESISTER : ID=%s\r\n", szGUID);
			return;
        }

		// 센서 등록 제한이 있는지 확인하여 등록 제한을 초과하는지 검사한다.
		if ((m_nSensorLimit >= 0) && (m_pEndDeviceList->GetCount() >= m_nSensorLimit))
		{
	        pCoordinator = (COORDINATOR *)GetCoordinator();
	        if (pCoordinator != NULL && pCoordinator->permit_time) { 
                CoordinatorPermitControl(0);
            }
            if((m_nSensorLimit >= 0) && (m_pEndDeviceList->GetCount() > m_nSensorLimit)) {
                // 초과한 경우 Inventory scanner가 Leave 시키도록 호출하고 빠져나간다.
                m_pInventoryScanner->Add(id, 0, 0, SCAN_MASK_LEAVE, 33000);
                return;
            }
		}

        /** Issue #1547: Info Frame(Frame #0) 일 경우에만 Sensor List에 추가하도록 한다.
         *   Info Frame이 아닐 경우에는 정확한 Type 정보를 알 수 없기 때문에 BATTERY-PULSE로 
         *   표시되게 된다 
         */
        if(payload->type) {
            XDEBUG(" INVENTORY CHECK : INVALID FRAME IDX=%d\r\n", payload->type);
            return;
        }

        {
            len = nLength - 1;  // nLength에서 Type 정보를 뺀다
            stLen = sizeof(ENDI_METERING_INFO);

            memset(&info, 0, stLen);
            memcpy(&info, &payload->val.info, stLen < len ? stLen : len); 

            isInstall = TRUE;

            pszModel = GetSensorTypeName(info.sensorType);
            XDEBUG(" ONMETERING: SENSORTYPE=%d, MODEL=%s\r\n", info.sensorType, pszModel);
            if(pszModel)
            {
                parser = m_pMeterParserFactory->SelectParser(pszModel);
            }
            else
            {
                parser = m_pMeterParserFactory->SelectParser("PULSE");
                pszModel = parser->GetModel();
            }

            pszParser = parser ? parser->GetName() : const_cast<char *>("PULSE");
            nAttr = parser ? parser->GetAttribute(pszModel) : ENDDEVICE_ATTR_BATTERYNODE;
		    pEntry = m_pEndDeviceList->SetEndDeviceByID(id, UNDEFINED_PORT_NUMBER, pszModel,
                    parser, pszParser, parser ? parser->GetParserType() : PARSER_TYPE_PULSE,
                    nAttr | (!info.networkType ? ENDDEVICE_ATTR_RFD : 0x00));

		    if (pEntry == NULL)
		    {
   			    XDEBUG("\033[1;40;31m ****** ERROR: SetEndDeviceByID error (ID=%s)\033[0m\r\n", szGUID);
			    return;
		    }

		    XDEBUG(" ONMETERING: VENDOR=%s, SERIAL=%s, MODEL=%s\r\n",
		        pEntry->szVendor, pEntry->szSerial, pEntry->szModel);

		    nType = parser ? parser->GetType() : SENSOR_TYPE_UNKNOWN;

            /* Issue #1556: IF4 스펙상의 Service Type 정보를 얻어온다 */
		    nService = GetServiceTypeBySensorType(info.sensorType);
            /** Pulse Type은 Info.sensorType으로 각 EndDevice 별로 설정 한다. */
            m_pEndDeviceList->SetServiceType(id, nService);

		    m_pEndDeviceList->UpdateSensorInventory(id, pszParser, parser ? parser->GetParserType() : PARSER_TYPE_PULSE,
                    pszModel, 0, 0, 0);

#ifdef __SUPPORT_KUKDONG_METER__
		    // 극동 미터는 미터 KEUKDONG 프레임이 와야 install 이벤트가 발생한다.
		    if (strstr(pEntry->szModel, "KEUKDONG") == NULL)
		    {
#endif
            /* Issue #1287: Pulser Meter가 처음 등록될 때 Install Event 전송 */
		    sensorInstallEvent(id, nType, nService, pEntry->szSerial, pEntry->szVendor, pEntry->szModel, 
                nAttr | (!info.networkType ? ENDDEVICE_ATTR_RFD : 0x00));
#ifdef __SUPPORT_KUKDONG_METER__
            } 
#endif
        } 
	}

	if (!m_pEndDeviceList->QueryRouteRecord(pEntry, &shortid, &hops, path)) {
        /** Pulse MIU의 경우 최초 Install 시 EndDevice가 생성되기 전에 먼저 Route Record가 오게 된다.
         * 이 정보가 현재 저장되지 않고 있어서 최초 등록시에는 Route Record가 등록 안되고 있다.
         * 이 경우 Ondemand 가 실패하기 때문에 Inventory Scanner에 사용하는 Cash를 사용해야 한다.
         */
		if(m_pInventoryScanner->GetCacheRouteRecord(id, &shortid, &hops, path)) {
	        m_pEndDeviceList->UpdateRouteRecord(id, shortid, hops, path);
        }
    }
	*szTemp = '\0';
	for(i=0; i<hops; i++)
	{
		if (i) sprintf(ch, " %04X", path[i]);
		else   sprintf(ch, "%04X", path[i]);
        strcat(szTemp, ch);
	}

    switch(payload->type) {
        case 0: // info frame
            {
            int alarmFlag = -1;     // Alarm Flag가 지원안되면 -1, Off = 0, On = 1
            int lqi=0;
            char * sensorType;
            BOOL isTypeChanged=FALSE;

            if(!isInstall) {
                len = nLength - 1;  // nLength에서 Type 정보를 뺀다
                stLen = sizeof(ENDI_METERING_INFO);

                memset(&info, 0, stLen);
                memcpy(&info, &payload->val.info, stLen < len ? stLen : len); 
            }

            sensorType = GetSensorTypeName(info.sensorType);

            if((int) info.LQI < 80 ) {
                lqi = (int) info.LQI;
            }else {
                lqi = (int) (80 + (info.LQI - 80) * 0.125);
            }

            if(lqi>100) lqi = 100;

            if(len > 42) { // Issue #1864 : Info Frame에 Alarm Flag 추가
                alarmFlag = info.alarmFlag ? 1 : 0;
            }


            /** Energy Level Update */
            /** 실제 관리되는 Energy Level은 Patch에 영향을 받지 않는다.
             *  Patch 11-65에 영향을 받는 Energy Level 값은 
             *    1. 검침데이터저장,
             *    2. CLI View, 
             *    3. EnergyLevelChangeEvent 전송
             *    이다.
             */
            m_pEndDeviceList->SetEnergyLevel(id, info.energyLevel, 0);

/** 제주실증단지 DR Level 적용 Patch */
#if     defined(__PATCH_11_65__)
            if(*m_pbPatch_11_65) {
                info.energyLevel =  Patch_11_65_Nuri2Jeju(info.energyLevel);
            }
#endif
            
	        XDEBUG(" METERING ID=%s (%04d/%02d/%02d %02d:%02d:%02d, OPRDAY=%d, ACTMIN=%d)\r\n"
                   "          TYPE=%s(%s), LQI=%d, RSSI=%d, ALARM=%d, LEVEL=%d\r\n"
                   "          BAT=%.04f, AMP=%.04f, OFF=%.04f\r\n"
	               "          LP [%04d/%02d/%02d] PERIOD=%d, CHOICE=%d, BP=%d, CP=%d\r\n"
	               "          FW VER=%.1f, FW BLD=0x%02X, HW VER=%.1f, SW VER=%.1f\r\n",
					szGUID,
					ntoshort(info.curTime.year),
					info.curTime.mon ,
					info.curTime.day ,
					info.curTime.hour ,
					info.curTime.min ,
					info.curTime.sec ,
					ntoshort(info.oprDay),
					ntoshort(info.actMin),
                    sensorType ? sensorType : "PULSE-BATTERY",
                    info.networkType ? "MESH" : "STAR",
                    lqi,
                    info.RSSI,
                    alarmFlag,
                    info.energyLevel,
					(double)ntoshort(info.batteryVolt) / (double)10000,
					(double)ntoshort(info.curConsum) / (double)10000,
					(double)info.offset / (double)10000,
					ntoshort(info.lpDate.year),
					info.lpDate.mon,
					info.lpDate.day,
					info.lpPeriod,
					info.lpChoice,
					ntoint(info.basePulse),
					ntoint(info.curPulse),
                    (info.fwVersion >> 4) + (info.fwVersion & 0x0f) * 0.1,
                    info.fwBuild,
                    (info.hwVersion >> 4) + (info.hwVersion & 0x0f) * 0.1,
                    (info.swVersion >> 4) + (info.swVersion & 0x0f) * 0.1);

            /** Issue #2567: Pulse Meter 는 BasePulse, CurrentPulse, BatteryVolt 를 저장한다 */
            m_pEndDeviceList->ClearValue(id);
            tmpWV = ntoshort(info.batteryVolt); m_pEndDeviceList->AddValue(id, const_cast<char *>("BATT"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpWV);
            tmpUV = ntoint(info.basePulse); m_pEndDeviceList->AddValue(id, const_cast<char *>("BP"), VARSMI_UINT, sizeof(UINT), (BYTE *) &tmpUV);
            tmpUV = ntoint(info.curPulse); m_pEndDeviceList->AddValue(id, const_cast<char *>("CP"), VARSMI_UINT, sizeof(UINT), (BYTE *) &tmpUV);

            /** Issue #2581: LpChoice가 변경되는 경우 당일에는 Duplicate 검사가 잘 못 될 수 있기 때문에 검사 전에
              * 반영 하도록 한다.
              */
            m_pEndDeviceList->SetMeteringAttribute(id, UNDEFINED_PORT_NUMBER,
                    LAST_DATA_MASK_LPPERIOD, info.lpPeriod, info.lpChoice, alarmFlag);

            isDuplicate = m_pEndDeviceList->IsDuplicateLpData(id, ntoshort(info.lpDate.year), info.lpDate.mon, info.lpDate.day);
            XDEBUG(" DUPLICATE DATA CHECK=%s %s\r\n", szGUID, isDuplicate ? "TRUE" : "FALSE");

            if(!isDuplicate) {
                m_pEndDeviceList->SetPulseSensorLpTimePulse(id, &info.lpDate, info.basePulse);
                m_pEndDeviceList->SetTempMeteringDate(id, ntoshort(info.lpDate.year), info.lpDate.mon, info.lpDate.day);

                /** Issue #1552: LP Period가 0이어서 LP가 없는 경우에는 상태를 NORMAL로 처리하고
                *   Last Metering Time을 Update 하도록 수정 
                */
                if(info.lpPeriod) {
	                m_pEndDeviceList->SetState(id, ENDISTATE_DATA_RECEIVE);
                }else {
	                time(&now);
	                m_pEndDeviceList->SetLastMeteringTime(id, now);
                    m_pEndDeviceList->CommitMeteringDate(id);
	                m_pEndDeviceList->SetState(id, ENDISTATE_NORMAL);
                }
            }

            m_pEndDeviceList->UpdateSensorInventory(id, NULL, PARSER_TYPE_NOT_SET, sensorType,  
                (((WORD) (info.hwVersion & 0xf0)) << 4) + (info.hwVersion & 0x0f),
                (((WORD) (info.fwVersion & 0xf0)) << 4) + (info.fwVersion & 0x0f),
                (((WORD) (info.fwBuild >> 4)) * 10) + (info.fwBuild & 0x0f));
            isTypeChanged = m_pEndDeviceList->SetEndDeviceNetworkType(id, info.networkType);
            m_pEndDeviceList->SetQuality(id, info.LQI, info.RSSI);
	        m_pEndDeviceList->VerifyEndDevice(id);
            m_pLpWriter->ClearData(id);
            if(!isDuplicate) {
                m_pLpWriter->AddData(id, (BYTE *)&info, stLen); // 구조체 크기만큼만 저장한다
            }
            if(isTypeChanged && !isInstall){
                /*-- MIU의 Network Type이 변경되었다. Event를 정의해서 FEP로 쏘아주어야 한다 --*/
		        XDEBUG("MIU Network Type Change : [%s] [%s]\r\n", szGUID, info.networkType ? "MESH" : "STAR");

		        parser = m_pMeterParserFactory->SelectParser(pEntry->szParser);
		        nType = parser ? parser->GetType() : SENSOR_TYPE_UNKNOWN;
		        nService = GetServiceTypeBySensorType(info.sensorType);

		        sensorInstallEvent(id, nType, nService, pEntry->szSerial, pEntry->szVendor, pEntry->szModel, 
                    parser->GetAttribute(parser->GetName()) | (!info.networkType ? ENDDEVICE_ATTR_RFD : 0x00));
            }
            m_pTransactionManager->Notify(id);
            }
            break;
        case 1:
            memcpy(buffer + idx, &pEntry->lpTimeDate, sizeof(TIMEDATE)); idx += sizeof(TIMEDATE);
            memcpy(buffer + idx, &pEntry->lpBasePulse, sizeof(UINT)); idx += sizeof(UINT);
        case 2:
        case 3:
        case 4:
	        XDEBUG(" METERING ID=%s LPIDX=%d)\r\n",szGUID, payload->type);

            isDuplicate = m_pEndDeviceList->IsDuplicateLpData(id);
            XDEBUG(" DUPLICATE DATA CHECK=%s %s\r\n", szGUID, isDuplicate ? "TRUE" : "FALSE");

            /*-- 중복 데이터면 처리하지 않고 리턴한다 --*/
            if(isDuplicate) return;

            memcpy(buffer + idx, payload->val.lpData, nLength - 1);

	        time(&now);
            if(m_pEndDeviceList->IsValidLpIndex(id, payload->type, nLength, &curr, &isLast)) {
#ifdef __SUPPORT_KUKDONG_METER__
                /** 극동미터는 LP에서 끝나지 않는다 */
                if (strstr(pEntry->szModel, "KEUKDONG") != NULL)
                {
                    isLast = FALSE;
                }
#endif
                if(m_pLpWriter->AddData(id, (BYTE *) buffer, nLength - 1 + idx, isLast, pEntry, m_pMeterFileWriter)) {
                    if(isLast) {
	                    m_pEndDeviceList->SetLastMeteringTime(id, now);
                        m_pEndDeviceList->CommitMeteringDate(id);
	                    m_pEndDeviceList->SetState(id, ENDISTATE_NORMAL);
	                    m_pEndDeviceList->VerifyEndDevice(id);

			            METERING_LOG("%s [%s] Metering ok. shortid=%04X, hop=%d path=%s\r\n",
                            szGUID, pEntry->szSerial, pEntry->shortid, pEntry->hops, szTemp);
                    }
                }else {
	                m_pEndDeviceList->SetLastMeteringFailTime(id, now);
	                m_pEndDeviceList->SetState(id, ENDISTATE_METER_ERROR);
	                XDEBUG(" TEMP LP DATA SAVE FAIL ID=%s\r\n", szGUID);
	                m_pEndDeviceList->VerifyEndDevice(id);
                    m_pLpWriter->ClearData(id);

			        METERING_LOG("%s [%s] Metering fail. (LP Data Save Error) shortid=%04X, hop=%d path=%s\r\n",
                        szGUID, pEntry->szSerial, pEntry->shortid, pEntry->hops, szTemp);
                }
            }else {
	            m_pEndDeviceList->SetLastMeteringFailTime(id, now);
	            m_pEndDeviceList->SetState(id, ENDISTATE_METER_ERROR);
	            m_pEndDeviceList->VerifyEndDevice(id);
                m_pLpWriter->ClearData(id);
	            XDEBUG(" INVALID METERING FRAME ID=%s EXPIDX=%d, TYPE=%d\r\n", szGUID, curr, payload->type);

			    METERING_LOG("%s [%s] Metering fail. (Invalid Index. ExpIdx=%d, Idx=%d) shortid=%04X, hop=%d path=%s\r\n",
                    szGUID, pEntry->szSerial, curr, payload->type, pEntry->shortid, pEntry->hops, szTemp);
            }
            break;
        default:
	        time(&now);
	        m_pEndDeviceList->SetLastMeteringFailTime(id, now);
	        m_pEndDeviceList->SetState(id, ENDISTATE_METER_ERROR);
	        m_pEndDeviceList->VerifyEndDevice(id);
            m_pLpWriter->ClearData(id);
	        XDEBUG(" INVALID METERING FRAME ID=%s TYPE=%d\r\n", szGUID, payload->type);

			METERING_LOG("%s [%s] Metering fail. (Invalid Frame. Idx=%d) shortid=%04X, hop=%d path=%s\r\n",
                szGUID, pEntry->szSerial, payload->type, pEntry->shortid, pEntry->hops, szTemp);
            return;
    }

}

#ifdef __SUPPORT_KUKDONG_METER__
void endiOnKeukdongData(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDIENTRY	*pEntry;
	CMeterParser	*parser = NULL;
    KEUKDONG_METERING_PAYLOAD *pData = (KEUKDONG_METERING_PAYLOAD*)payload;
	char	szGUID[17];
    char  	*pszModel = NULL;
	char	szSerial[21];
	BYTE	nType, nService;
    BYTE    *pPoint = (BYTE *)pData;
	time_t	now;
    int     nAttr;

	eui64toa(id, szGUID);
	memset(szSerial, 0, sizeof(szSerial));
	strncpy(szSerial, pData->serial, 12);

	XDEBUG(" KEUKDONG-METERING (ID=%s, LEN=%d, LQI=%d, RSSI=%d) SERIAL=%s, STATUS=0x%02X, ALARM=0x%02X\r\n",
			szGUID, nLength, LQI, RSSI, szSerial, pData->status, pData->alarm);

    pEntry = m_pEndDeviceList->GetEndDeviceByID(id);
    if (pEntry == NULL)
		return;

	time(&now);

    if(pEntry->lpPeriod != pEntry->lpIndex)
    {
	    XDEBUG(" INVALID METERING FRAME ID=%s EXPIDX=%d, CURRIDX=%d\r\n", szGUID, pEntry->lpPeriod, pEntry->lpIndex);
	    m_pEndDeviceList->SetLastMeteringFailTime(id, now);
	    m_pEndDeviceList->SetState(id, ENDISTATE_METER_ERROR);
	    m_pEndDeviceList->VerifyEndDevice(id);
        m_pLpWriter->ClearData(id);

        METERING_LOG("%s [%s] Metering fail. (Invalid Index. ExpIdx=%d, Idx=%d)\r\n",
            szGUID, pEntry->szSerial, pEntry->lpPeriod, pEntry->lpIndex);
        return;
    }

	// 미터 시리얼이 변경되었는지 검사하여, 변경되면 이벤트를 전송한다.
	if (strncmp(pEntry->szSerial, szSerial, 12) != 0)
	{
	    parser = m_pMeterParserFactory->SelectParser("KEUKDONG");
		if (parser != NULL)
		{
		    pszModel = parser->GetModel();
			nService = parser->GetService(); 
			nType 	 = parser->GetType();
  	 		nAttr 	 = parser->GetAttribute(pszModel);

			m_pEndDeviceList->UpdateMeterInventory(id, 0, PARSER_TYPE_KEUKDONG, 
                    "NURI", METER_VENDOR_NURI, NULL, szSerial, 0, 0);
		   	m_pEndDeviceList->VerifyEndDevice(id);

			sensorInstallEvent(id, nType, nService, szSerial, pEntry->szVendor, pszModel, nAttr);
		}
	}

    // Meter Serial은 빼고 2 byte 만 추가 한다
    m_pLpWriter->AddData(id, (BYTE *)&pPoint[12], 2, TRUE, pEntry, m_pMeterFileWriter);

    m_pEndDeviceList->SetLastMeteringTime(id, now);
    m_pEndDeviceList->CommitMeteringDate(id);
    m_pEndDeviceList->SetState(id, ENDISTATE_NORMAL);
  	m_pEndDeviceList->VerifyEndDevice(id);

    METERING_LOG("%s [%s] Metering ok.\r\n", szGUID, szSerial);
}
#endif

void endiOnEvent(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_EVENT_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	GMTTIMESTAMP	now;
	char			szGUID[17];	 		
	// End Device에서 이벤트가 전송되면 불린다.
	now.tz		= ntoshort(payload->gmt.tz);
	now.dst		= ntoshort(payload->gmt.dst);
	now.year	= ntoshort(payload->gmt.year);
	now.mon		= payload->gmt.mon;
	now.day		= payload->gmt.day;
	now.hour	= payload->gmt.hour;
	now.min		= payload->gmt.min;
	now.sec		= payload->gmt.sec;

	eui64toa(id, szGUID);
	XDEBUG(" SENSOR-EVENT(ID=%s, TYPE=0x%02X, VAL=0x%02X TZ=%d, DST=%d, TIME=%04d/%02d/%02d %02d:%02d:%02d)\r\n",
					szGUID, payload->type, BigToHostInt(payload->status.val.u32), now.tz, now.dst,
					now.year, now.mon, now.day,
					now.hour, now.min, now.sec);

	switch(payload->type) {
	  case ENDI_EVENT_POWER_OUTAGE :		// 정전 이벤트
	  case ENDI_EVENT_POWER_RECOVER :		// 복전 이벤트
            /* Issue #1238: 대량정전/복전 발생 시 복전의 type이 0으로 가는 문제
             *  원래 type의 값은 정/복전 시 0으로 가게 되어 있고 status로 구분해야 한다
             *  하지만 대량 정복전에 대한 status가 없기 때문에 type 값을 조정하도록 한다
             */
            {
                ENDI_EVENT_STATUS * est = (ENDI_EVENT_STATUS *) &payload->status;
                if(payload->type==0 && est->val.u32 == 0x02000000) {
                    payload->type = 2;
                }
	            m_pPowerOutage->Add(id, &now, payload->type, (BYTE *)&payload->status);
            }
		   break;
#if     defined(__PATCH_8_2305__)
      case ENDI_EVENT_FIRE:                 // Fire Alarm
           /** Issue #2305: 노르웨이 요청으로 Fire Alarm 발생시 20초 후에 DIO Message를 
             *  Deactivate 발생 시 3초 후에 DIO Message를 전송한다.
             */
           {
                if(*m_pbPatch_8_2305) 
                {
                    ENDI_EVENT_PAYLOAD * alarmData = (ENDI_EVENT_PAYLOAD *) MALLOC(sizeof(ENDI_EVENT_PAYLOAD));
                    memcpy(alarmData, payload, sizeof(ENDI_EVENT_PAYLOAD));
                    m_pBatchJob->Add(BATCH_ALARM_BROADCASTING, alarmData, TRUE, payload->status.val.u32 ? 3 : 20);
                }
           }
#endif
	  default :
		   sensorAlarmEvent(id, &now, payload->type, (BYTE *)&payload->status);
		   break;
	}
}

#if defined(__SUPPORT_DEVICE_NODE__)
void endiOnTimesync(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_TIMESYNC_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}
#endif

BOOL endiOnLink(HANDLE codi, EUI64 *id, BYTE type, BYTE LQI, signed char RSSI)
{
	//return m_pZigbeeServer->OnLink(codi, id, type);
    return TRUE;
}

void endiOnRead(HANDLE codi, EUI64 *id, BYTE seq, WORD addr, int nLength, BYTE LQI, signed char RSSI)
{
	//m_pZigbeeServer->OnRead(codi, id, addr, nLength);
}

void endiOnData(HANDLE codi, EUI64 *id, BYTE seq, BYTE *data, int nLength, BYTE LQI, signed char RSSI)
{
    /* Ondemand Operation이 없으면 parser를 찾아서 OnData를 호출 한다 */
    ENDIENTRY *pEntry;
    CMeterParser * parser;
    char    szGUID[17];
#ifdef __SUPPORT_SITE_KETI__
    WORD tmpWV;
    UINT tmpUV;
#endif

	eui64toa(id, szGUID);
    pEntry = m_pEndDeviceList->GetEndDeviceByID(id);
    if(pEntry != NULL)
    {
        parser =  m_pMeterParserFactory->SelectParser(pEntry->szParser);
        if(parser != NULL)
        {
            parser->OnData(id, NULL, seq, data, nLength);
        }
    }

#ifdef __SUPPORT_SITE_KETI__
    /** KETI를 위해 Bypass data 처리 코드 추가 
     * 등록되지 않은 센서의 경우 등록 절차를 거친다.
     */
    ENDI_3PARTY_FRAME * frame = (ENDI_3PARTY_FRAME *)data;

    /** STX 검사 */
    if (frame->hdr.stx != 0x02) 
    {
        /** 알려지지 않은 Protocol은 무시한다 */
        XDEBUG("endiOnData: Unknown Protocol STX=0x%02X\r\n", frame->hdr.stx);
        return;
    }

    /** @todo CRC 검사가 필요 하다 */

    /** Sequence 검사 : 1번 seq를 가진 놈만 payload header가 있다고 본다 */
    if(frame->hdr.seq == 0x01)
    {
        char  * pszModel = const_cast<char *>("3PARTY");
        char  * pszParser = NULL;
        int         nAttr;
        APP_3PARTY_PAYLOAD * payload = (APP_3PARTY_PAYLOAD *) frame->payload;

        /** PSTX 검사 */
        if(payload->hdr.stx != 0x02)
        {
            /** 잘못된 3rd party payload */
            XDEBUG("endiOnData: Invalid 3rd party payload PSTX=0x%02X\r\n", payload->hdr.stx);
            return;
        }
            
        XDEBUG("Add 3rd Party Sensor %s, Vendor 0x%02X, Model 0x%02X\r\n", 
                szGUID, payload->hdr.vendor, payload->hdr.model);

        if(pEntry == NULL)
        {
            KETI_HDR_ST * hdr = (KETI_HDR_ST *)payload->payload;
            char serial[32]={0,};
            /** Sensor 추가 코드 */
            parser = m_pMeterParserFactory->SelectParser(pszModel);
            pszParser = parser ? parser->GetName() : const_cast<char *>("3PARTY");
            nAttr = (parser ? parser->GetAttribute(pszModel) : ENDDEVICE_ATTR_POWERNODE) | ENDDEVICE_ATTR_POWERNODE;

            memset(serial, 0, sizeof(serial));
            strncpy(serial, (const char *)hdr->scid, sizeof(hdr->scid));
		    pEntry = m_pEndDeviceList->SetEndDeviceByID(id, parser, pszParser, serial, nAttr);

		    if (pEntry == NULL)
		    {
   			    XDEBUG("\033[1;40;31m ****** ERROR: SetEndDeviceByID error (ID=%s)\033[0m\r\n", szGUID);
			    return;
		    }

            switch(payload->hdr.vendor)
            {
                case 0x0B: // KETI
                    switch(payload->hdr.model)
                    {
                        case 0x01: // Temperature
                            pszModel = const_cast<char *>("TEMP");
                            break;
                        case 0x02: // Accelation
                            pszModel = const_cast<char *>("ACC");
                            break;
                        case 0x03: // Flow
                            pszModel = const_cast<char *>("FLOW");
                            break;
                    }
                    break;
            }

		    m_pEndDeviceList->UpdateSensorInventory(id, pszParser, pszModel, 0x0100, 0x0100, 1);

		    XDEBUG("endiOnData: VENDOR=%s, SERIAL=%s, MODEL=%s\r\n",
		        pEntry->szVendor, pEntry->szSerial, pEntry->szModel);

		    sensorInstallEvent(id, SENSOR_TYPE_UNKNOWN, SERVICE_TYPE_UNKNOWN, 
                    pEntry->szSerial, pEntry->szVendor, pEntry->szModel, nAttr);
        }

        if(pEntry != NULL)
        {
            switch(payload->hdr.vendor)
            {
                case 0x0B: // KETI
                    switch(payload->hdr.model)
                    {
                        case 0x01: // Temperature
                            KETI_TEMP_ST * tempSt = (KETI_TEMP_ST *) payload->payload;

                            m_pEndDeviceList->ClearValue(id);
                            m_pEndDeviceList->AddValue(id, const_cast<char *>("BU"), VARSMI_BYTE, sizeof(BYTE), (BYTE *) &tempSt->bu);
                            m_pEndDeviceList->AddValue(id, const_cast<char *>("HU"), VARSMI_BYTE, sizeof(BYTE), (BYTE *) &tempSt->hu);
                            m_pEndDeviceList->AddValue(id, const_cast<char *>("TU"), VARSMI_BYTE, sizeof(BYTE), (BYTE *) &tempSt->tu);
                            tmpWV = ntoshort(tempSt->cb); m_pEndDeviceList->AddValue(id, const_cast<char *>("BATTERY"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpWV);
                            tmpWV = ntoshort(tempSt->ch); m_pEndDeviceList->AddValue(id, const_cast<char *>("HUMIDITY"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpWV);
                            tmpWV = ntoshort(tempSt->ct); m_pEndDeviceList->AddValue(id, const_cast<char *>("TEMPERATURE"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpWV);
                            break;
                        case 0x02: // Accelation
                            KETI_ACC_ST * accSt = (KETI_ACC_ST *) payload->payload;

                            m_pEndDeviceList->ClearValue(id);
                            tmpWV = ntoshort(accSt->cb); m_pEndDeviceList->AddValue(id, const_cast<char *>("CB"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpWV);
                            tmpWV = ntoshort(accSt->cg); m_pEndDeviceList->AddValue(id, const_cast<char *>("CG"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpWV);
                            tmpWV = ntoshort(accSt->cz); m_pEndDeviceList->AddValue(id, const_cast<char *>("CZ"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpWV);
                            tmpWV = ntoshort(accSt->cy); m_pEndDeviceList->AddValue(id, const_cast<char *>("CY"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpWV);
                            tmpWV = ntoshort(accSt->cx); m_pEndDeviceList->AddValue(id, const_cast<char *>("CX"), VARSMI_WORD, sizeof(WORD), (BYTE *) &tmpWV);
                            break;
                        case 0x03: // Flow
                            KETI_FLOW_ST * flowSt = (KETI_FLOW_ST *) payload->payload;

                            m_pEndDeviceList->ClearValue(id);
                            m_pEndDeviceList->AddValue(id, const_cast<char *>("PU"), VARSMI_BYTE, sizeof(BYTE), (BYTE *) &flowSt->pu);
                            m_pEndDeviceList->AddValue(id, const_cast<char *>("AU"), VARSMI_BYTE, sizeof(BYTE), (BYTE *) &flowSt->au);
                            m_pEndDeviceList->AddValue(id, const_cast<char *>("IU"), VARSMI_BYTE, sizeof(BYTE), (BYTE *) &flowSt->iu);
                            m_pEndDeviceList->AddValue(id, const_cast<char *>("TVU"), VARSMI_BYTE, sizeof(BYTE), (BYTE *) &flowSt->tvu);
                            tmpUV = ntoint(flowSt->cp); m_pEndDeviceList->AddValue(id, const_cast<char *>("CP"), VARSMI_UINT, sizeof(UINT), (BYTE *) &tmpUV);
                            tmpUV = ntoint(flowSt->ca); m_pEndDeviceList->AddValue(id, const_cast<char *>("CA"), VARSMI_UINT, sizeof(UINT), (BYTE *) &tmpUV);
                            tmpUV = ntoint(flowSt->ci); m_pEndDeviceList->AddValue(id, const_cast<char *>("CI"), VARSMI_UINT, sizeof(UINT), (BYTE *) &tmpUV);
                            tmpUV = ntoint(flowSt->ctv); m_pEndDeviceList->AddValue(id, const_cast<char *>("CTV"), VARSMI_UINT, sizeof(UINT), (BYTE *) &tmpUV);
                            break;
                    }
                    break;
            }
            m_pEndDeviceList->VerifyEndDevice(id);
        }
    }
#endif  // __SUPPORT_SITE_KETI__
}
