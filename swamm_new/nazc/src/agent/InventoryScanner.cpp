#include "common.h"
#include "codiapi.h"
#include "AgentService.h"
#include "InventoryScanner.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "MeterParserFactory.h"
#include "CommonUtil.h"
#include "Utility.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "EventInterface.h"

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////

#define STATE_INIT			0
#define STATE_CONNECT		1
#define STATE_READ			2
#define STATE_ONDEMAND		3

CInventoryScanner   *m_pInventoryScanner = NULL;

extern int     m_nSensorLimit;
extern BYTE    m_nOpMode;
extern int     m_nScanningStrategy;
#if     defined(__PATCH_4_1647__)
extern BOOL    *m_pbPatch_4_1647;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInventoryScanner::CInventoryScanner()
{
	m_pInventoryScanner	= this;
	m_tmLastCheck = 0;
	m_nCacheCount = 0;

	// 버퍼를 비우고 포인터를 보관한다.
	for(int i=0; i<MAX_ROUTERECORD_CACHE_COUNT; i++)
	{
		memset(&m_arCacheNode[i], 0, sizeof(CACHEROUTERECORD));
		m_pCacheList[i] = &m_arCacheNode[i];
	}
}

CInventoryScanner::~CInventoryScanner()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CInventoryScanner::Initialize()
{
	time(&m_tmLastCheck);
	if (!CTimeoutThread::StartupThread(11))
        return FALSE;
	return TRUE;
}

void CInventoryScanner::Destroy()
{
	RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CInventoryScanner::Add(const EUI64 *id, BYTE LQI, signed char RSSI, int mask, int timeout)
{
	SCANITEM	*pItem;

	m_Locker.Lock();
	pItem = FindItem(id);
	if (pItem != NULL) pItem->mask = mask;
	m_Locker.Unlock();

	if (pItem != NULL)
		return TRUE;

	pItem = (SCANITEM *)MALLOC(sizeof(SCANITEM));
	if (pItem == NULL)
		return FALSE;

	memset(pItem, 0, sizeof(SCANITEM));
	memcpy(&pItem->id, id, sizeof(EUI64));
	if(!GetCacheRouteRecord(pItem))
    {
        m_pEndDeviceList->QueryRouteRecord(&pItem->id, &pItem->shortid, &pItem->hops, pItem->path);
    }
    pItem->timeout = timeout;
	pItem->mask = mask;
    pItem->LQI = LQI;
    pItem->RSSI = RSSI;
	time(&pItem->scanTime);

	m_Locker.Lock();
	pItem->nPosition = (int)m_List.AddTail(pItem);
	m_Locker.Unlock();
	ActiveThread();
	return TRUE;
}

void CInventoryScanner::RemoveAll()
{
	SCANITEM	*pItem, *pDelete;
	POSITION	pos;

	m_Locker.Lock();
	pItem = m_List.GetFirst(pos);
	while(pItem)
	{
		pDelete = pItem;
		pItem = m_List.GetNext(pos);
		FREE(pDelete);
	}
	m_List.RemoveAll();
	m_Locker.Unlock();
}

void CInventoryScanner::CacheRouteRecord(const EUI64 *id, WORD shortid, BYTE hops, WORD *path)
{
	CACHEROUTERECORD	*pCache = NULL, *pLast;
	int		i;
    BYTE    nHops = hops > CODI_MAX_ROUTING_PATH ? CODI_MAX_ROUTING_PATH : hops;

	m_CacheLocker.Lock();
	for(i=0; i<m_nCacheCount; i++)
	{
		if (memcmp(&m_pCacheList[i]->id, id, sizeof(EUI64)) == 0)
		{
			pCache = m_pCacheList[i];
			break;
		}
	}			

	if (pCache != NULL)
	{
		// Cache에 존재하면 업데이트
		pCache->shortid = shortid;
		pCache->hops	= nHops;
		if (path) memcpy(pCache->path, path, sizeof(WORD)*nHops);
	}
	else
	{
		// 존재하지 않으면 목록을 순환 시킨다.
		pLast = m_pCacheList[MAX_ROUTERECORD_CACHE_COUNT-1];
		for(i=MAX_ROUTERECORD_CACHE_COUNT-2; i>=0; i--)
			m_pCacheList[i+1] = m_pCacheList[i];
		m_pCacheList[0] = pLast;

		// 마지막 노드를 처음에 넣고 그곳에 값을 저장한다.
		memcpy(&pLast->id, id, sizeof(EUI64));
		pLast->shortid	= shortid;
		pLast->hops		= nHops;
		if (path) memcpy(pLast->path, path, sizeof(WORD)*nHops);
		
		if (m_nCacheCount < MAX_ROUTERECORD_CACHE_COUNT)
			m_nCacheCount++;
	}
	m_CacheLocker.Unlock();
}

BOOL CInventoryScanner::GetCacheRouteRecord(SCANITEM *pItem)
{
	CACHEROUTERECORD	*pCache = NULL;
	int		i;

	m_CacheLocker.Lock();
	for(i=0; i<m_nCacheCount; i++)
	{
		if (memcmp(&m_pCacheList[i]->id, &pItem->id, sizeof(EUI64)) == 0)
		{
			pCache = m_pCacheList[i];
			break;
		}
	}			

	if (pCache != NULL)
	{
		pItem->shortid	= pCache->shortid;
		pItem->hops		= pCache->hops;
		memcpy(pItem->path, pCache->path, sizeof(WORD)*pCache->hops);	
	}
	m_CacheLocker.Unlock();

	return pCache ? TRUE : FALSE;
}

BOOL CInventoryScanner::GetCacheRouteRecord(const EUI64 *id, WORD *shortid, BYTE *hops, WORD *path)
{
	CACHEROUTERECORD	*pCache = NULL;
	int		i;

	if(shortid) *shortid = 0;
	*hops = 0;
	memset(path, 0, sizeof(WORD)*CODI_MAX_ROUTING_PATH);

	m_CacheLocker.Lock();
	for(i=0; i<m_nCacheCount; i++)
	{
		if (memcmp(&m_pCacheList[i]->id, id, sizeof(EUI64)) == 0)
		{
			pCache = m_pCacheList[i];
			break;
		}
	}			

	if (pCache != NULL)
	{
		if(shortid) *shortid	= pCache->shortid;
		*hops		= pCache->hops;
		memcpy(path, pCache->path, sizeof(WORD)*pCache->hops);	
	}
	m_CacheLocker.Unlock();

	return pCache ? TRUE : FALSE;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

void CInventoryScanner::ScanWorker(SCANITEM *pItem)
{
	CMeterParser	*parser;
	ENDIENTRY		*pEndDevice;
	CMDPARAM		param;
	METER_STAT		stat;
	CChunk			chunk(512);
	char	szGUID[17];
	char	szBuffer[128];
	char	*szNodeKind, *szParser;
	int		nState, nError;
	int		nLength, nRetry;
	int		mask, len, addr, attr;
	BYTE	nType, nService;
	WORD	hw, fw, build, sfw, sbuild;
	HANDLE	codi;
	HANDLE	endi=NULL;
    BOOL    isOpen = FALSE;
    BOOL    isConnect = FALSE;
    BOOL    bEndDevice = FALSE;
    BOOL    bExist = FALSE;

	codi = GetCoordinator();
	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("InventoryScanner: Coordinator is not ready.\r\n");
		return;
	}

	nRetry = 0;
	mask = len = addr = 0;
	hw = fw = sfw = sbuild = 0;

	eui64toa(&pItem->id, szGUID);
	XDEBUG("InventoryScanner: ID=%s\r\n", szGUID);

    if (pItem->mask == SCAN_MASK_LEAVE || ((m_nSensorLimit >= 0) && 
		(m_nSensorLimit <= m_pEndDeviceList->GetCount()) &&
        (m_pEndDeviceList->GetEndDeviceByID(&pItem->id) == NULL)))
    {
		XDEBUG("InventoryScanner: MAX Sensor Violation.\r\n");
        XDEBUG("     SensorLimit: %d\r\n", m_nSensorLimit);
        XDEBUG("     SensorCount: %d\r\n", m_pEndDeviceList->GetCount());

	    XDEBUG("LEAVE NETWOKR (ID=%s)\r\n", szGUID);
        /*-- Keepalive leave는 sensor Uninstall Event를 보내지 않는다.
         *-- Keepalive 의 경우는 Agent가 join 처리를 못 한 경우이기 때문에
         *-- sensorUninstallEvent를 보내지 않는다.
         --*/
	    m_pOndemander->Leave(codi, &pItem->id, 12, 0, 0);
		return;
    }

	for(nState=STATE_INIT; nState!=STATE_DONE;)
	{
		switch(nState) {
		  case STATE_INIT :
			   nLength = 0;
			   nState = STATE_CONNECT;
			   break;

		  case STATE_CONNECT :
			   nError = ((isOpen==FALSE)?isOpen = TRUE, endiOpen(codi, &endi, &pItem->id):CODIERR_NOERROR); 
			   if (nError != CODIERR_NOERROR)
			   {
				   XDEBUG("InventoryScanner(%d): %s\r\n", nError, codiErrorMessage(nError));
				   nState = STATE_RETRY;
                   isOpen = FALSE;
				   break;
			   }
			   XDEBUG("ID=%s, ShortID=%d, Hops=%d\r\n", szGUID, pItem->shortid, pItem->hops);

               pEndDevice = m_pEndDeviceList->GetEndDeviceByID(&pItem->id);
               bEndDevice =  pEndDevice ? (pEndDevice->nAttribute & ENDDEVICE_ATTR_RFD ? TRUE : FALSE) : FALSE;
               bExist = pEndDevice ? TRUE : FALSE;

               /** Issue #1622: Inventory Scan 시에는 Node의 Type을 알 수 없기 때문에 최대한의 Timeout을 설정한다.
                 *  bExtendedTime이 TRUE로 설정될 경우 Delivery Timeout이 최대 1분30초가 된다 
                 */
			   nError = (isConnect==FALSE)
               /** Issue #1622: OP MODE가 Pulse Mesh일 경우에는 Sleepy End Device가 사용되지 않기 때문에
                 * Extended Timeout을 사용하지 않는다 -- 폐기
                 * Issue #1633 이용
                 */
#if     defined(__PATCH_4_1647__)
                    ? (*m_pbPatch_4_1647 ? 
	                        endiConnectDirect(endi, ((ENDDEVICE *)endi)->nShortId, 
                                bEndDevice ? ((pItem->timeout<100000) ? 100000 : pItem->timeout) : pItem->timeout,
                                bEndDevice) :
                            endiConnectWithRouteRecord(endi, pItem->shortid, pItem->hops, pItem->path, 
                                bEndDevice ? ((pItem->timeout<100000) ? 100000 : pItem->timeout) : pItem->timeout,
                                bEndDevice) )
#else
                    ?endiConnectWithRouteRecord(endi, pItem->shortid, pItem->hops, pItem->path, 
                        bEndDevice ? ((pItem->timeout<100000) ? 100000 : pItem->timeout) : pItem->timeout,
                        bEndDevice)
#endif // PATCH_4_1647
                    :CODIERR_NOERROR; isConnect = TRUE;

			   if (nError != CODIERR_NOERROR)
			   {
				   XDEBUG("InventoryScanner(%d): %s\r\n", nError, codiErrorMessage(nError));
				   nState = STATE_RETRY;
                   isConnect = FALSE;
				   break;
			   }

			   mask = 1;
			   addr = len = 0;
			   nState = STATE_READ;
			   break;

		  case STATE_READ :
                
			   switch(mask) {
			     case SCAN_MASK_NODE_INFO :
					  addr = ENDI_ROMAP_NODE_KIND;
					  len  = sizeof(ENDI_NODE_INFO);
					  break;
			     case SCAN_MASK_AMR_EXTRA_INFO :
					  addr = ENDI_ROMAP_PERMIT_MODE;
					  len  = sizeof(AMR_EXTRA_INFO);
					  break;
			     case SCAN_MASK_AMR_GROUP_INFO :
					  addr = ENDI_ROMAP_SUB_FW_VERSION;
					  len  = 2;
					  break;

			   } 
			
			   if ((pItem->mask & mask) > 0)
			   {
				   nError = endiReadRom(endi, addr, len, (BYTE *)szBuffer, &nLength, 3000);
			  	   if (nError != CODIERR_NOERROR)
			   	   {
				   	   XDEBUG("InventoryScanner(%d): %s\r\n", nError, codiErrorMessage(nError));
				       nState = STATE_RETRY;
				   	   break;
				   }
				   
                    switch(mask) {
					 case SCAN_MASK_NODE_INFO :
						  memcpy(&pItem->node, szBuffer, len);
                          szNodeKind = strip(pItem->node.NODE_KIND);
                          parser = m_pMeterParserFactory->SelectParser(szNodeKind);
                          szParser = parser->GetName();
                          /** TODO : 추후에 Parser에 Sub Group이 있는 Type인지 물어보도록 수정해야 한다 */
                           if(parser && parser->HasSubCore())
                           {
                               pItem->mask |=SCAN_MASK_AMR_GROUP_INFO;
                           }
                           XDEBUG("NODE_KIND=%s\r\n", pItem->node.NODE_KIND);

						  break;
				     case SCAN_MASK_AMR_EXTRA_INFO :
						  memcpy(&pItem->amrExtra, szBuffer, len);
						  break;
				     case SCAN_MASK_AMR_GROUP_INFO :
                            sfw = ((szBuffer[0] >> 4) << 8) | (szBuffer[0] & 0xf);
                            sbuild = ((szBuffer[1]>> 4) * 10) + (szBuffer[1] & 0xf);
                            XDEBUG("SUB TYPE UPDATE fw=0x%02X, build=0x%02X\r\n", sfw, sbuild);
						  break;
				   }
			   	   pItem->recvmask |= mask; 
			   }

			   mask = (mask << 1); 
			   if ((pItem->mask == pItem->recvmask)	|| (mask == 0x10000))   
			   {
				   pItem->bDone = (pItem->mask == pItem->recvmask) ? TRUE : FALSE;	
				   nState = STATE_ONDEMAND;
			   }
			   break;

		  case STATE_ONDEMAND :
			   if(isConnect) endiDisconnect(endi); isConnect = FALSE;
			   if(isOpen) endiClose(endi); isOpen = FALSE;
			   if (pItem->bDone)
			   {
                   szNodeKind = strip(pItem->node.NODE_KIND);
                   XDEBUG("INVT NODE_KIND=%s, Type=%s, hw=0x%02X, fw=0x%02X, build=0x%02X, network=0x%02X\r\n",
						pItem->node.NODE_KIND, szNodeKind, pItem->node.HARDWARE_VERSION, pItem->node.SOFTWARE_VERSION, 
                        pItem->node.FIRMWARE_BUILD, pItem->amrExtra.network_type);
				  
				   hw = ((pItem->node.HARDWARE_VERSION >> 4) << 8) | (pItem->node.HARDWARE_VERSION & 0xf);
				   fw = ((pItem->node.SOFTWARE_VERSION >> 4) << 8) | (pItem->node.SOFTWARE_VERSION & 0xf);
				   build = ((pItem->node.FIRMWARE_BUILD >> 4) * 10) + (pItem->node.FIRMWARE_BUILD & 0xf);

				   // 하드웨어 정보를 기록한다.
                   parser = m_pMeterParserFactory->SelectParser(szNodeKind);
                   szParser = parser ? parser->GetName() : szNodeKind;
				   attr = parser ? parser->GetAttribute(szNodeKind) : ENDDEVICE_ATTR_POWERNODE;
				   m_pEndDeviceList->SetEndDeviceByID(&pItem->id, UNDEFINED_PORT_NUMBER, szNodeKind,
                           parser, szParser, parser ? parser->GetParserType() : PARSER_TYPE_UNKNOWN, attr);
			  	   m_pEndDeviceList->UpdateSensorInventory(&pItem->id, szParser, 
                           parser ? parser->GetParserType() : PARSER_TYPE_UNKNOWN,  
                           (bExist && bEndDevice) ? NULL : (parser->GetModel()?parser->GetModel():szNodeKind), 
                           hw, fw, build);
                   if(parser && parser->HasSubCore())
                   {
                       if(parser->GetSubCoreParser() != NULL)
                       {
                            m_pEndDeviceList->UpdateSubInventory(&pItem->id, parser->GetSubCoreParser()->GetModel(), sfw, sbuild);
                       }
                   }
				   m_pEndDeviceList->UpdateRouteRecord(&pItem->id, pItem->shortid, pItem->hops, pItem->path);
				   m_pEndDeviceList->SetState(&pItem->id, CODISTATE_NORMAL);
                   m_pEndDeviceList->SetInstallState(&pItem->id, OTA_STATE_INIT);
                   m_pEndDeviceList->SetQuality(&pItem->id, pItem->LQI, pItem->RSSI);
				   m_pEndDeviceList->VerifyEndDevice(&pItem->id);
				   
                   /** MBUS일 경우 Ondemand를 더이상 진행하지 않는다 */
                   if (strncmp("NAMR-H101MG", szNodeKind, strlen(szNodeKind)) == 0)  
                   {
                       FREE(szNodeKind);
                       nState = STATE_OK;
                       break; 
                   }
				   // Ondemand를 수행한다.
				   memset(&param, 0, sizeof(CMDPARAM));
				   memcpy(&param.id, &pItem->id, sizeof(EUI64));
				   chunk.Empty();

				   param.codi      = codi;
				   param.nMeteringType = METERING_TYPE_SPECIAL;
				   param.nType     = PARSE_TYPE_READ;
				   param.nOption   = ONDEMAND_OPTION_READ_IDENT_ONLY;
				   param.pChunk    = &chunk;
				   memset(&stat, 0, sizeof(METER_STAT));
				   nError = m_pOndemander->Command(szNodeKind, &param, &stat);
                   FREE(szNodeKind);
			   }
			   nState = STATE_OK;
			   break;

		  case STATE_OK :
			   // Join Event를 전송한다.
			   if (pItem->bDone)
			   {
				   pEndDevice = m_pEndDeviceList->GetEndDeviceByID(&pItem->id);
				   if (pEndDevice != NULL)
				   {
					   parser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
					   nType = parser ? parser->GetType() : SENSOR_TYPE_UNKNOWN;
					   nService = parser ? parser->GetService() : SERVICE_TYPE_UNKNOWN;

					   XDEBUG("INVENTORY-SCANNER: VENDOR=%s, SERIAL=%s, MODEL=%s, NETWORK=%02X\r\n",
								pEndDevice->szVendor, pEndDevice->szSerial, pEndDevice->szModel,
                                pItem->amrExtra.network_type);

				   	   sensorInstallEvent(&pItem->id, nType, nService,
								pEndDevice->szSerial, pEndDevice->szVendor, pEndDevice->szModel,
                                pEndDevice->nAttribute);
				   }
			   }
			   nState = STATE_DONE;
			   break;

		  case STATE_RETRY :
			   nRetry++;
			   if (nRetry > sensorRetryCount(&pItem->id,1))
			   {
				   nState = STATE_DONE; 
				   break;
			   }
			   usleep(1000000);
			   nState = STATE_INIT;
			   break;
		}
	}
	if(isConnect) endiDisconnect(endi);
	if(isOpen) endiClose(endi);
}

SCANITEM *CInventoryScanner::FindItem(const EUI64 *id)
{
	SCANITEM	*pItem;
	POSITION	pos=INITIAL_POSITION;
	
	pItem = m_List.GetFirst(pos);
	while(pItem)
	{
		if (memcmp(&pItem->id, id, sizeof(EUI64)) == 0)
			return pItem;
		pItem = m_List.GetNext(pos);
	}
	return pItem;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CInventoryScanner::OnActiveThread()
{
	SCANITEM	*pItem;
	time_t		now;
	unsigned long waitTime;
    unsigned long delay = 10;

   	for(;!IsThreadExitPending();)
	{
		m_Locker.Lock();
       	pItem = m_List.GetHead();
		m_Locker.Unlock();

		if (pItem == NULL)
			break;

		time(&now);
		waitTime = (unsigned long)(now - pItem->scanTime);
		if (waitTime < delay) break;

		m_Locker.Lock();
        if (pItem) m_List.RemoveAt((POSITION)pItem->nPosition);
		m_Locker.Unlock();

		ScanWorker(pItem);
		FREE(pItem);
	}
	time(&m_tmLastCheck);
	return TRUE;
}

BOOL CInventoryScanner::OnTimeoutThread()
{
	if (m_List.GetCount() == 0)
		return TRUE;

	return OnActiveThread();
}

