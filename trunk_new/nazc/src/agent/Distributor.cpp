#include "common.h"
#include "Distributor.h"
#include "AgentService.h"
#include "Metering.h"
#include "Recovery.h"
#include "Polling.h"
#include "MeterReader.h"
#include "PackageUtil.h"
#include "MeterParserFactory.h"
#include "SensorTimesync.h"
#include "EndDeviceList.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "CRC16.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"
#include "revision.h"
#include "AgentLog.h"
#include "ShellCommand.h"

//////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////

#define STATE_INIT				0
#define STATE_READY				1
#define STATE_DOWNLOAD 			2
#define STATE_MERGE				3
#define STATE_DISTRIBUTE 		4
#define STATE_CONCENTRATOR		30
#define STATE_COORDINATOR 		60
#define STATE_SENSOR			100
				
#define BASE_ROM_ADDRESS		0

#define OTA_REASON_NORMAL       0
#define OTA_REASON_ERROR        1
#define OTA_REASON_USER         2

CDistributor   	*m_pDistributor = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDistributor::CDistributor() : CJobObject("Distributor", 12*60*60)
{
	m_pDistributor	= this;
	m_bBusy = FALSE;
}

CDistributor::~CDistributor()
{
}

//////////////////////////////////////////////////////////////////////
// Startup/Shutdown
//////////////////////////////////////////////////////////////////////

BOOL CDistributor::Initialize()
{
	int		i;

	mkdir("/app/fw", 0777);
	mkdir("/app/fw/concentrator", 0777);
	mkdir("/app/fw/coordinator", 0777);
	mkdir("/app/fw/sensor", 0777);

	for(i=0; i<MAX_OTA_WORKER; i++)
		m_arWorker[i].Initialize();

	if (!CTimeoutThread::StartupThread(13))
		return FALSE;
	return TRUE;
}

void CDistributor::Destroy()
{
	int		i;

	CTimeoutThread::ShutdownThread();

	for(i=0; i<MAX_OTA_WORKER; i++)
		m_arWorker[i].Destroy();

	RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

int CDistributor::Add(char *pszTriggerId, BYTE nType, char *pszModel, BYTE nTransferType, UINT nOtaStep,
BYTE nWriteCount, UINT nRetry, BYTE nThread, BYTE nInstallType, WORD nOldHw, WORD nOldSw, WORD nOldBuild,
WORD nNewHw, WORD nNewSw, WORD nNewBuild, char *pszBinURL, char *pszBinChecksum,
char *pszDiffURL, char *pszDiffChecksum, EUI64 *pIdList, int nIdCount, BOOL bEnableDownload)
{
	OTAENTRY	*pEntry;

	if (!pszTriggerId || !*pszTriggerId || (strlen(pszTriggerId) > 16))
		return IF4ERR_INVALID_TRIGGERID;

	if (nType > OTA_TYPE_COORDINATOR)
		return IF4ERR_INVALID_OTA_TYPE;

	if (nType == OTA_TYPE_SENSOR)
	{
		if (!pszModel || !*pszModel || (strlen(pszModel) > 32))
			return IF4ERR_INVALID_MODEL;

		if (nTransferType > OTA_TRANSFER_UNICAST)
			return IF4ERR_INVALID_TRANSFER_TYPE;

		if ((nOtaStep == 0) || (nRetry == 0) || (nThread == 0))
			return IF4ERR_INVALID_VALUE;
	} 
    else if (nType == OTA_TYPE_COORDINATOR)
    {
	    COORDINATOR	*pCoordinator;
        WORD hw;

        /** Coordinator일 경우 H/W 버전이 같아야 한다. (106/108이 혼용되어서는 안된다) */
	    pCoordinator = (COORDINATOR *)GetCoordinator();
	    hw = ((pCoordinator->hw & 0xF0) << 4) + (pCoordinator->hw & 0x0F);

        if(nOldHw != hw || nNewHw != hw)
            return IF4ERR_INVALID_VERSION;
    }

	if (nInstallType > OTA_INSTALL_FORCE)
		return IF4ERR_INVALID_VALUE;

	if ((nOldHw == 0) || (nOldSw == 0))
		return IF4ERR_INVALID_VERSION;

	if ((nNewHw == 0) || (nNewSw == 0))
		return IF4ERR_INVALID_VERSION;

	if (!pszBinURL || !*pszBinURL || (strlen(pszBinURL) > 255))
		return IF4ERR_INVALID_BIN_URL;

	if (bEnableDownload && (!pszBinChecksum || !*pszBinChecksum || (strlen(pszBinChecksum) != 32)))
		return IF4ERR_INVALID_BIN_CHECKSUM;

	if (bEnableDownload && nInstallType != OTA_INSTALL_FORCE && (!pszDiffURL || (*pszDiffURL && (strlen(pszDiffURL) > 255))))
		return IF4ERR_INVALID_DIFF_URL;

	if (bEnableDownload && nInstallType != OTA_INSTALL_FORCE && (!pszDiffChecksum || (*pszDiffChecksum && (strlen(pszDiffChecksum) != 32))))
		return IF4ERR_INVALID_DIFF_CHECKSUM;

	pEntry = GetItem(pszTriggerId);
	if (pEntry != NULL)
		return IF4ERR_DUPLICATE_TRIGGERID;	

	pEntry = (OTAENTRY *)MALLOC(sizeof(OTAENTRY));
	if (pEntry == NULL)
		return IF4ERR_MEMORY_ERROR;
	
	memset(pEntry, 0, sizeof(OTAENTRY));
	strcpy(pEntry->szTriggerId, pszTriggerId ? pszTriggerId : "");
	strcpy(pEntry->szModel, pszModel ? GetParserName(pszModel) : "");
	strcpy(pEntry->szBinURL, pszBinURL ? pszBinURL : "");
	strcpy(pEntry->szBinChecksum, pszBinChecksum ? pszBinChecksum : "");
	strcpy(pEntry->szDiffURL, pszDiffURL ? pszDiffURL : "");
	strcpy(pEntry->szDiffChecksum, pszDiffChecksum ? pszDiffChecksum : "");
	pEntry->nType			= nType;
	pEntry->nTransferType	= nTransferType;
	pEntry->nInstallType	= nInstallType;
	pEntry->nOtaStep		= nOtaStep;
	pEntry->nWriteCount		= MAX(nWriteCount, 1);
	pEntry->nRetry			= nRetry;
	pEntry->nThread			= MIN(nThread, MAX_OTA_WORKER);
	pEntry->nOldHw			= nOldHw;
	pEntry->nOldSw			= nOldSw;
	pEntry->nOldBuild		= nOldBuild;
	pEntry->nNewHw			= nNewHw;
	pEntry->nNewSw			= nNewSw;
	pEntry->nNewBuild		= nNewBuild;
	pEntry->nIdCount		= nIdCount;
	pEntry->pIdList			= pIdList;
    pEntry->nOtaCurrentStep = 0x00;

	pEntry->bActive			= FALSE;
	pEntry->bExitPending	= FALSE;
	pEntry->nState			= STATE_INIT;

	m_Locker.Lock();
	pEntry->nPosition = (int)m_List.AddTail(pEntry);
	m_Locker.Unlock();

	ActiveThread();
	return IF4ERR_NOERROR;
}

int CDistributor::Cancel(char *pszTriggerId)
{
	OTAENTRY	*pEntry;

	if (!pszTriggerId || !*pszTriggerId || (strlen(pszTriggerId) > 16))
		return IF4ERR_INVALID_TRIGGERID;

	m_Locker.Lock();
	pEntry = Find(pszTriggerId);
	if (pEntry != NULL)
	{
		if (!pEntry->bActive)
		{
			// 동작중이 아닌 경우 리스트에서 삭제하고 free한다.
			m_List.RemoveAt((POSITION)pEntry->nPosition);
			otaEndEvent(pEntry->szTriggerId, OTA_REASON_USER, 
                    pEntry->nTotal, pEntry->nComplete, pEntry->nFail);
			FreeItem(pEntry);		
		}
		else
		{
			// 이미 동작중인 경우 취소 플래그를 설정한다.
			pEntry->bExitPending = TRUE;
		}
	}
	m_Locker.Unlock();

	// 같은 Trigger ID를 찾지 못한 경우
	if (pEntry == NULL)
		return IF4ERR_TRIGGERID_NOT_FOUND;

	// Trigger가 사라질때까지 대기 시킨다.
	while(GetItem(pszTriggerId)) usleep(1000000);	
	return IF4ERR_NOERROR;
}

OTAENTRY *CDistributor::GetItem(char *pszTriggerId)
{
	OTAENTRY	*pEntry;

	m_Locker.Lock();
	pEntry = Find(pszTriggerId);
	m_Locker.Unlock();
	return pEntry;
}

int CDistributor::GetState(char *pszTriggerId, OTASTATE *pState)
{
	OTAENTRY	*pEntry;
	struct	tm	when;

	if (!pszTriggerId || !*pszTriggerId || (strlen(pszTriggerId) > 16))
		return IF4ERR_INVALID_TRIGGERID;

	m_Locker.Lock();
	pEntry = Find(pszTriggerId);
	m_Locker.Unlock();

	if (pEntry == NULL)
		return IF4ERR_TRIGGERID_NOT_FOUND;	

	memset(pState, 0, sizeof(OTASTATE));
	when = *localtime(&pEntry->tmStart);
	pState->tmStart.year = when.tm_year+1900;
	pState->tmStart.mon	 = when.tm_mon + 1;
	pState->tmStart.day	 = when.tm_mday;
	pState->tmStart.hour = when.tm_hour;
	pState->tmStart.min	 = when.tm_min;
	pState->tmStart.sec	 = when.tm_sec;
	pState->nElapse		 = (pEntry->nStartTick > 0) ? ((UINT)uptime()) - pEntry->nStartTick : 0;
	pState->nTotal		 = pEntry->nTotal;
	pState->nComplete	 = pEntry->nComplete;
	pState->nFail		 = pEntry->nFail;
	pState->nState		 = pEntry->bActive;
	pState->nOtaStep	 = pEntry->nOtaCurrentStep;
	return IF4ERR_NOERROR;
}

int	CDistributor::GetList(DISTRIBUTIONENTRY **pList, int *nCount)
{
	DISTRIBUTIONENTRY	*pArray = NULL;
	OTAENTRY	*pEntry;
	POSITION	pos;
	int			nItem = 0, nSeek=0;

	m_Locker.Lock();
	nItem = m_List.GetCount();
	if (nItem > 0)
	{
		pArray = (DISTRIBUTIONENTRY *)MALLOC(sizeof(DISTRIBUTIONENTRY)*nItem);
		if (pArray != NULL)
		{
			memset(pArray, 0, sizeof(DISTRIBUTIONENTRY)*nItem);
			pEntry = m_List.GetFirst(pos);
			while(pEntry)
			{
				strcpy(pArray[nSeek].szTriggerId, pEntry->szTriggerId);
				strcpy(pArray[nSeek].szModel, pEntry->szModel);
				pArray[nSeek].nType			= pEntry->nType;
				pArray[nSeek].nTransferType	= pEntry->nTransferType;
				pArray[nSeek].nInstallType	= pEntry->nInstallType;
				pArray[nSeek].nOtaStep		= pEntry->nOtaStep;
				pArray[nSeek].nWriteCount	= pEntry->nWriteCount;
				pArray[nSeek].nRetry		= pEntry->nRetry;
				pArray[nSeek].nThread		= pEntry->nThread;
				pArray[nSeek].nOldHw		= pEntry->nOldHw;
				pArray[nSeek].nOldSw		= pEntry->nOldSw;
				pArray[nSeek].nOldBuild		= pEntry->nOldBuild;
				pArray[nSeek].nNewHw		= pEntry->nNewHw;
				pArray[nSeek].nNewSw		= pEntry->nNewSw;
				pArray[nSeek].nNewBuild		= pEntry->nNewBuild;
				pArray[nSeek].nCount		= pEntry->nNodeCount;
				pArray[nSeek].nElapse		= (pEntry->nStartTick > 0) ? ((UINT)uptime()) - pEntry->nStartTick : 0;
				pArray[nSeek].nState		= pEntry->bActive;
				pArray[nSeek].tmStart		= pEntry->tmStart;
				nSeek++;

				pEntry = m_List.GetNext(pos);
			}
		}
	}
	m_Locker.Unlock();

	*pList = pArray;
	*nCount = nSeek;
	return (nSeek > 0) ? IF4ERR_NOERROR : IF4ERR_NO_ENTRY;
}

int	CDistributor::GetOtaNodes(char *pszTriggerId, OTANODEENTRY **pList, int *nCount)
{
	OTANODEENTRY	*pArray = NULL;
	OTAENTRY	*pEntry;
	OTANODE		*pNode;
	int			i, nSeek=0;

	m_Locker.Lock();
	pEntry = Find(pszTriggerId);
	if (pEntry != NULL)
	{
		pArray = (OTANODEENTRY *)MALLOC(sizeof(OTANODEENTRY)*pEntry->nNodeCount);
		if (pArray != NULL)
		{
			memset(pArray, 0, sizeof(OTANODEENTRY)*pEntry->nNodeCount);
			for(i=0; i<pEntry->nNodeCount; i++)
			{
				pNode = &pEntry->pNode[i];
				memcpy(&pArray[nSeek].id, &pNode->id, sizeof(EUI64));
				strcpy(pArray[nSeek].szModel, pNode->szModel);
				pArray[nSeek].hw		= ((pNode->hw & 0xf0) << 4) + (pNode->hw & 0x0f);
				pArray[nSeek].sw		= ((pNode->sw & 0xf0) << 4) + (pNode->sw & 0x0f);
				pArray[nSeek].build		= ((pNode->build >> 4)*10) + (pNode->build & 0x0f);
				pArray[nSeek].hops		= pNode->nHops;
				pArray[nSeek].nOtaMask	= pNode->nOtaMask;
				pArray[nSeek].nTotal	= pNode->nTotal;
				pArray[nSeek].nCurrent	= pNode->nCurrent;
				pArray[nSeek].nElapse	= pNode->nElapse;
				pArray[nSeek].nError	= pNode->nError;
				pArray[nSeek].nState	= pNode->bActive;
				pArray[nSeek].tmStart	= pNode->tmStart;
				pArray[nSeek].tmEnd		= pNode->tmEnd;
				nSeek++;
			}
		}
	}
	m_Locker.Unlock();

	*pList = pArray;
	*nCount = nSeek;
	return (nSeek > 0) ? IF4ERR_NOERROR : IF4ERR_NO_ENTRY;
}

BOOL CDistributor::IsBusy()
{
	return m_bBusy;
}

//////////////////////////////////////////////////////////////////////
// Member functions
//////////////////////////////////////////////////////////////////////

OTAENTRY *CDistributor::Find(char *pszTriggerId)
{
	OTAENTRY	*pEntry;
	POSITION	pos=INITIAL_POSITION;

	pEntry = m_List.GetFirst(pos);
	while(pEntry)
	{
		if (strcmp(pEntry->szTriggerId, pszTriggerId) == 0)
			break;
		pEntry = m_List.GetNext(pos);
	}
	return pEntry;
}

void CDistributor::FreeItem(OTAENTRY *pEntry)
{
	if (pEntry->pIdList) FREE(pEntry->pIdList);
	FREE(pEntry);
}

void CDistributor::RemoveAll()
{
	OTAENTRY	*pEntry, *pDelete;
	POSITION	pos;

	pEntry = m_List.GetFirst(pos);
	while(pEntry)
	{
		pDelete = pEntry;
		pEntry = m_List.GetNext(pos);
		FreeItem(pDelete);
	}
	m_List.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Override functions
//////////////////////////////////////////////////////////////////////

BOOL CDistributor::DownloadFirmware(OTAENTRY *pEntry)
{
	PACKAGEINFO	pkg;
	char	szFileName[65];
	char	szZipFileName[65];
    char    * szTargetFile = NULL;
	char	szPackageFile[65];
	char	szCommand[128];
	BOOL	bDownload, bMerge;
	int		nError;
	CChunk	chunk(4096 * 16);


	switch(pEntry->nType) {
	  case OTA_TYPE_CONCENTRATOR :
		   sprintf(szFileName, "/app/sw/oldfirm.tar");
		   sprintf(szZipFileName, "/app/fw/concentrator/oldfirm.tar.gz");
		   sprintf(szPackageFile, "/app/fw/concentrator/concentrator.pkg");
           szTargetFile = szZipFileName;
		   break;

	  case OTA_TYPE_COORDINATOR :
		   sprintf(szFileName, "/app/fw/coordinator/coordinator.ebl");
		   sprintf(szZipFileName, "/app/fw/coordinator/coordinator.ebl.gz");
		   sprintf(szPackageFile, "/app/fw/coordinator/coordinator.pkg");
           szTargetFile = szFileName;
		   break;

	  case OTA_TYPE_SENSOR :
		   // 모델명을 변환하여 파일명으로 만든다. (공백 및 특수문자가 제거됨)
		   sprintf(szFileName, "/app/fw/sensor/%s.ebl", pEntry->szModel);
           if(pEntry->nDestType == 1)
		        sprintf(szZipFileName, "/app/fw/sensor/%s.bin.gz", pEntry->szModel);
           else sprintf(szZipFileName, "/app/fw/sensor/%s.ebl.gz", pEntry->szModel);
		   sprintf(szPackageFile, "/app/fw/sensor/%s.pkg", pEntry->szModel);
           szTargetFile = szFileName;
		   break;
      default:
           return FALSE;
	}
	XDEBUG("FileName = %s\r\n", szFileName);
	XDEBUG("ZipFileName = %s\r\n", szZipFileName);
	XDEBUG("TargetFile = %s\r\n", szTargetFile);
	XDEBUG("PackageFileName = %s\r\n", szPackageFile);

	bDownload = TRUE;
	bMerge = FALSE;

    sprintf(pEntry->szFile,"%s", szTargetFile);

	if (pEntry->nInstallType != OTA_INSTALL_FORCE && IsExists(szTargetFile) && IsExists(szPackageFile))
	{
		if (LoadPackageInformation(&pkg, szPackageFile))
		{
            XDEBUG("pkg.nSw=0x%X pkg.nBuild=0x%X nNewSw=0x%X nNewBuild=0x%X nOldSw=0x%X nOldBuild=0x%X\r\n", 
                    pkg.nSwVersion, pkg.nBuildVersion, 
                    pEntry->nNewSw, pEntry->nNewBuild,
                    pEntry->nOldSw, pEntry->nOldBuild);
			if ((pkg.nSwVersion == pEntry->nNewSw) && (pkg.nBuildVersion == pEntry->nNewBuild))
			{
				// 현재 버젼이 최신 버젼인 경우
				bDownload = FALSE;
				bMerge = FALSE;
			}
			else if ((pkg.nSwVersion == pEntry->nOldSw) && (pkg.nBuildVersion == pEntry->nOldBuild))
			{
				// 현재 파일이 OLD Version과 동일한 경우에만 DIFF/MERGE를 수행한다.
				bDownload = TRUE;
				bMerge = TRUE;
			}
			else
			{
				// 구버젼과 동일하지 않거나 없는 경우는 무조건 Download한다.
				bDownload = TRUE;
				bMerge = FALSE;
			}
		}
	}

    /** 저장해 놓은 F/W 이미지에 대해 CheckSum도 저장하여 Merge 작업등을 할 때 확인해 보고
      * 잘못되어 있을 경우 삭제를 해야 한다.
      */
    /** Coordinator의 경우 Upgrade 시 기존 Network 정보를 유지할 수 있도록 해야 한다. 따라서
      * 기존 정보를 백업해 두었다가 업그레이드가 완료되면 다시 설정해야 한다.
      */
    XDEBUG("bDownload=%s bMerge=%s\r\n", bDownload ? "TRUE" : "FALSE", bMerge ? "TRUE" : "FALSE");
	while (bDownload)
	{
        /** merge file을 download 하다가 실패하거나 checksum이 맞지 않을 경우 원본 파일을 다운로드 받게 한다 */
		if (bMerge)
			 nError = FileDownload(pEntry->szDiffURL, "download", pEntry->szDiffChecksum, "/tmp", pEntry->bExitPending);
		else nError = FileDownload(pEntry->szBinURL, "download", pEntry->szBinChecksum, "/tmp", pEntry->bExitPending);

		if (nError != 0)
		{
			XDEBUG("Package File Download Fail(%0d)\r\n", nError);
            if(bMerge) {
                bMerge = FALSE;
                continue;
            }
            return FALSE;
		}

		if (bMerge)
		{
			if (!DiffMerge(szFileName, "/tmp/download", "/app/fw/merge"))
			{
				remove("/tmp/download");
				remove("/app/fw/merge");
                bMerge = FALSE;
                continue;
			}
		}

	    if(pEntry->nType != OTA_TYPE_CONCENTRATOR) {
            if (bMerge) 
            {
		        sprintf(szCommand, "/bin/mv -f /app/fw/merge %s", szFileName);
            }
            else
            {
		        sprintf(szCommand, "/bin/mv -f /tmp/download %s", szZipFileName);
		        SystemExec(szCommand);
#if defined(__TI_AM335X__)
		        sprintf(szCommand, "/bin/gzip -df %s", szZipFileName); 
#else
		        sprintf(szCommand, "/usr/bin/gzip -df %s", szZipFileName); 
#endif
            }
		    SystemExec(szCommand);
            //mbus check
		    pkg.nType			= pEntry->nType;
		    pkg.nHwVersion		= pEntry->nNewHw;
		    pkg.nSwVersion		= pEntry->nNewSw;
		    pkg.nBuildVersion	= pEntry->nNewBuild;
		    strcpy(pkg.szModel, pEntry->szModel);
		    strcpy(pkg.szChecksum, pEntry->szBinChecksum);

		    SavePackageInformation(&pkg, szPackageFile);
        }else {
            if (bMerge) 
            {
		        sprintf(szCommand, "/bin/mv -f /app/fw/merge %s", "/tmp/oldfirm.tar");
		        SystemExec(szCommand);
#if defined(__TI_AM335X__)
                SystemExec("/bin/gzip -f /tmp/oldfirm.tar"); 
#else
                SystemExec("/usr/bin/gzip -f /tmp/oldfirm.tar"); 
#endif
            }
            else
            {
		        sprintf(szCommand, "/bin/mv -f /tmp/download %s", "/tmp/oldfirm.tar.gz");
            }
		    SystemExec(szCommand);
            sprintf(pEntry->szFile,"%s", "/tmp/oldfirm.tar.gz");
        }

        bDownload = FALSE;
	}

	return TRUE;
}

BOOL CDistributor::UpgradeConcentrator(OTAENTRY *pEntry, char *pszFileName)
{
	WORD	sw, build;
	PACKAGEINFO	pkg;
	char	szPackageFile[65];

	sw = ((SW_VERSION & 0xf0) << 4) + (SW_VERSION & 0x0f);
	build = atoi(SW_REVISION_FIX);

	// 설치 방법에 따라 동작을 분기한다.
	switch(pEntry->nInstallType) {
	  case OTA_INSTALL_AUTO :
		   // 같은 버젼이 아니면 설치한다.
		   if ((sw == pEntry->nNewSw) && (build == pEntry->nNewBuild))
		   {
			   XDEBUG("Match version~~~!!!\r\n");
			   return TRUE;
		   }
		   break;

	  case OTA_INSTALL_MATCH :
		   // 구버젼과 동일한 경우에만 설치한다.
		   if ((sw != pEntry->nOldSw) || (build != pEntry->nOldBuild))
			   return FALSE;
		   break;

	  case OTA_INSTALL_REINSTALL :
	  case OTA_INSTALL_FORCE :
		   // 버젼에 관계 없이 무조건 설치한다.
		   break;
	}

    if(UpgradeConcentratorSystem(NULL, CONCENTRATOR_UPGRADE_GZIP, "127.0.0.1", pszFileName))
    {
        sprintf(szPackageFile, "/app/upgrade/concentrator.pkg");
        pkg.nType			= pEntry->nType;
        pkg.nHwVersion		= pEntry->nNewHw;
        pkg.nSwVersion		= pEntry->nNewSw;
        pkg.nBuildVersion	= pEntry->nNewBuild;
        strcpy(pkg.szModel, pEntry->szModel);
        strcpy(pkg.szChecksum, pEntry->szBinChecksum);
        SavePackageInformation(&pkg, szPackageFile);
    }
	return TRUE;
}

BOOL CDistributor::UpgradeCoordinator(OTAENTRY *pEntry, char *pszFileName)
{
	COORDINATOR	*pCoordinator;
	struct	stat	fi;
	WORD	sw, build;
	int		nError;

	// 현재 Coordinator의 버젼을 구한다.
	pCoordinator = (COORDINATOR *)GetCoordinator();
	sw = ((pCoordinator->fw & 0xf0) << 4) + (pCoordinator->fw & 0x0f);
	build = (((pCoordinator->build & 0xf0) >> 4) * 10) + (pCoordinator->build & 0x0f);
	XDEBUG("SW=0x%04X, BUILD=%0d\r\n", sw, build);

	// 설치 방법에 따라 동작을 분기한다.
	switch(pEntry->nInstallType) {
	  case OTA_INSTALL_AUTO :
		   // 같은 버젼이 아니면 설치한다.
		   if ((sw == pEntry->nNewSw) && (build == pEntry->nNewBuild))
		   {
			   XDEBUG("Match version~~~!!!\r\n");
			   return TRUE;
		   }
		   break;

	  case OTA_INSTALL_MATCH :
		   // 구버젼과 동일한 경우에만 설치한다.
		   if ((sw != pEntry->nOldSw) || (build != pEntry->nOldBuild))
			   return FALSE;
		   break;

	  case OTA_INSTALL_REINSTALL :
	  case OTA_INSTALL_FORCE :
		   // 버젼에 관계 없이 무조건 설치한다.
		   break;
	}

	memset(&fi, 0, sizeof(struct stat));
	stat(pszFileName, &fi);

	XDEBUG("--------- Coordinator Firmware Download ---------\r\n");
	XDEBUG("FILE : %s\r\n", pszFileName);
	XDEBUG("SIZE : %0d Byte(s)\r\n", fi.st_size);

	if (fi.st_size > 0)
	{
		nError = codiDownload(GetCoordinator(), CODI_MODEMTYPE_XMODEM, pszFileName, "ebl");
		if (nError != CODIERR_NOERROR)
		{
			XDEBUG("---------- codiDownload Fail(%0d): %s -----------\r\n", nError, codiErrorMessage(nError));
			return FALSE;
		}
	}
	return TRUE;	
}

BOOL CDistributor::UpgradeSensor(OTAENTRY *pEntry, char *pszFileName)
{
	CChunk	chunk(4096 * 16);
	int		nCount;//, pkglen;

    pEntry->nFWLen             = LoadFirmware(&chunk, pszFileName);
    pEntry->nFWCRC             = GetCrc16((BYTE *)chunk.GetBuffer(), pEntry->nFWLen);
    if (pEntry->nFWLen==0)
	{
		XDEBUG("UpgradeSensor: Firmware loading fail: '%s'\r\n", pszFileName);
		return FALSE;
	}

	XDEBUG("--------- Sensor Firmware Download ---------\r\n");
	XDEBUG("FILE : %s\r\n", pszFileName);
	XDEBUG("SIZE : %0d Byte(s)\r\n", pEntry->nFWLen);

	// 전송 방식에 따라서 Multicast/Unicast를 선택한다.
    /** Size가 112 * 1024 Byte 이상이면 Multicast를 선택하지 못하고 반드시 Unicast로 진행된다 */
	switch(pEntry->nTransferType) {
	  case OTA_TRANSFER_AUTO :
		   // AUTO는 센서가 50대 이상이면 Multicast, 이하이면 Unicast로 업데이트
		   if (pEntry->nIdCount > 0)
			    nCount = pEntry->nIdCount;
		   else  nCount = m_pEndDeviceList->GetModelCount(pEntry->szDestParserName);      //Point
           
		   if (nCount > 50 && pEntry->nFWLen < (112 * 1024))	
		       return MulticastOta(pEntry, &chunk);
		   return UnicastOta(pEntry, &chunk);

	  case OTA_TRANSFER_MULTICAST :
		   if (pEntry->nFWLen < (112 * 1024))	
		       return MulticastOta(pEntry, &chunk);
		   return UnicastOta(pEntry, &chunk);

	  case OTA_TRANSFER_UNICAST :
		   return UnicastOta(pEntry, &chunk);
	}
	return FALSE;	
}

COtaWorker *CDistributor::GetIdleWorker(OTAENTRY *pEntry)
{
	int		i;

	for(i=0; i<pEntry->nThread; i++)
	{
		if (!m_arWorker[i].IsBusy())
			return &m_arWorker[i];
	}
	return NULL;	
}

BOOL CDistributor::IsActiveWorker()
{
	int		i;

	for(i=0; i<MAX_OTA_WORKER; i++)
	{
		if (m_arWorker[i].IsBusy())
			return TRUE;
	}
	return FALSE;	
}

BOOL CDistributor::MulticastOta(OTAENTRY *pEntry, CChunk *pChunk)
{
	CMeterParser	*pParser;
	HANDLE	codi;
	OTANODE	*pList = NULL;
	char	*pBinary, *pSeek;
	time_t	now, start;
	int		i, j, k, nCount=0, nErrorCount;
	int		addr, nError, nLength, nSize, nCurrent;
	BOOL	bMulticastDone, bResult = FALSE;

	// 센서 목록을 구성한다. 
	if (pEntry->nIdCount > 0)
    {
		 pList = m_pEndDeviceList->GetOtaListById(pEntry->szDestParserName, &nCount, 
                 pEntry->pIdList, pEntry->nIdCount, pEntry->nOldHw, pEntry->nOldSw, pEntry->nOldBuild);
    }
	else 
    {
        pList = m_pEndDeviceList->GetOtaListByModel(pEntry->szDestParserName, &nCount, 
                pEntry->nOldHw, pEntry->nOldSw, pEntry->nOldBuild);
    }

	if ((pList == NULL) || (nCount == 0))
	{
		XDEBUG("MulticastOta: No entry selected. (Model='%s')\r\n", pEntry->szDestParserName);
		return TRUE;
	}

	pEntry->pNode = pList;
	pEntry->nNodeCount = nCount;
	pEntry->nTotal = nCount;

	codi = GetCoordinator();
	nSize = pChunk->GetSize();
	pBinary = pChunk->GetBuffer();
	nErrorCount = 0;
	nCurrent = nError = 0;
	bMulticastDone = FALSE;

	// 접속 및 타임아웃 시간을 설정한다.
	pParser = m_pMeterParserFactory->SelectParser(pEntry->szDestParserName);
	for(i=0; i<nCount; i++)
	{
		pList[i].nConnectionTimeout = pParser->GetConnectionTimeout(ENDDEVICE_ATTR_POWERNODE);
		pList[i].nReplyTimeout		= pParser->GetReplyTimeout();
	}

	// Multicast로 펌웨어를 전송한다.
	if ((pEntry->nOtaStep & OTA_STEP_SEND) != 0)
	{
		time(&start);
        pEntry->nOtaCurrentStep |= OTA_STEP_SEND;
		for(;!pEntry->bExitPending;)
		{
			time(&now);
			if (abs(now-start) > 3600)
			{
				// 한시간 마다 Watchdog을 Clear해준다.
				WatchdogClear();
			    time(&start);
			}

			addr = BASE_ROM_ADDRESS + nCurrent;
			pSeek = pBinary + nCurrent;
		 	nLength = MIN((nSize-nCurrent), MAX_OTA_BYTES);

			for(j=0; j<pEntry->nWriteCount; j++)
			{
				if (j > 0) usleep(3000000);

			    XDEBUG("\033[1;40;31mmulticastWriteLongRom(%0d): nTotal=%0d, nCurrent=%0d(%0d%%), nLength=%0d\033[0m\r\n",
							j+1, nSize, nCurrent, (nCurrent*100)/nSize, nLength);

				// Delivery error 인경우 3회 재시도 한다.
				for(k=0; k<3; k++)
				{
					nError = multicastWriteLongRom(codi, addr, (BYTE *)pSeek, nLength);
					if (nError != CODIERR_DELIVERY_ERROR)
						break;
	
					// Delivery error가 발생하면 1초간 대기한다.
					usleep(1000000);
				}
			}

			if (nError == CODIERR_NOERROR)
			{
				nErrorCount = 0;
				nCurrent += nLength;
				if (nCurrent >= nSize)
				{
					bMulticastDone = TRUE;
					break;
				}
			}
			else
			{
				nErrorCount++;
				if (nErrorCount > 5)
				{
					// Multicast 전송 실패
					break;
				}
			}

			// Multicast 전송후 10초+회수*3초 를 대기한다.
			usleep(10000000);
		}

		// Multicast 전송이 모두 성공한 경우	
		if (bMulticastDone)
		{
			for(i=0; i<nCount; i++)
			{
				// 맨 처음 한번만 전송이 완료된 상태처럼 만든다.
				// Verify에서 실패하면 다시 UNICAST로 처리된다.	
				pList[i].nOtaMask |= OTA_STEP_SEND;
			}
		}
	}

    if(!pEntry->bExitPending) 
        bResult = UnicastOtaByList(pEntry, pChunk, pList, nCount);

	// 할당된 메모리를 해제한다.
	FREE(pList);
	return bResult;
}

BOOL CDistributor::UnicastOta(OTAENTRY *pEntry, CChunk *pChunk)
{
	CMeterParser	*pParser;
	OTANODE	*pList = NULL;
	BOOL	bResult;
	int		i, nCount = 0;

	// 센서 목록을 수집한다.
	if (pEntry->nIdCount > 0)
    {
        //XDEBUG("REWRITER: %s %d\r\n", pEntry->szDestParserName, pEntry->nIdCount);
        pList = m_pEndDeviceList->GetOtaListById(pEntry->szDestParserName, &nCount, pEntry->pIdList, pEntry->nIdCount,
                 pEntry->nOldHw, pEntry->nOldSw, pEntry->nOldBuild);
    }
	else 
    {
        //XDEBUG("REWRITER: %s\r\n", pEntry->szDestParserName);
        pList = m_pEndDeviceList->GetOtaListByModel(pEntry->szDestParserName, &nCount,
                 pEntry->nOldHw, pEntry->nOldSw, pEntry->nOldBuild);
    }

	if ((pList == NULL) || (nCount == 0))
	{
		XDEBUG("UnicastOta: No entry selected. (Model='%s')\r\n", pEntry->szDestParserName);
		return TRUE;
	}

	pEntry->pNode = pList;
	pEntry->nNodeCount = nCount;
	pEntry->nTotal = nCount;

	// 접속 및 타임아웃 시간을 설정한다.
	pParser = m_pMeterParserFactory->SelectParser(pEntry->szDestParserName);
	for(i=0; i<nCount; i++)
	{
		pList[i].nConnectionTimeout = pParser->GetConnectionTimeout(ENDDEVICE_ATTR_POWERNODE);
		pList[i].nReplyTimeout		= pParser->GetReplyTimeout();
	}

	// 개별 OTA를 수행한다.
	bResult = UnicastOtaByList(pEntry, pChunk, pList, nCount);

	// 할당된 메모리를 해제한다.
	FREE(pList);
	return bResult;
}

BOOL CDistributor::UnicastOtaByList(OTAENTRY *pEntry, CChunk *pChunk, OTANODE *pList, int nCount)
{
	char	szGUID[17];
	int		i, nRetry;
	BYTE	nOtaStep;

/*
	if (pEntry->nIdCount > 0)
	{
		// 개별 실행은 TIMESYNC와 MTOR를 하지 않는다.
		OtaStepByStep(pEntry, pChunk, pList, nCount, pEntry->nRetry, pEntry->nOtaStep);
	}
	else
*/
	{
		// 전체	- 실패하면 TIMESYNC와 MTOR를 수행한다.
		for(nRetry=0; nRetry<2; nRetry++)
		{
			// Inventory scan, 전송, Verify를 수행한다.
			nOtaStep = pEntry->nOtaStep & (OTA_STEP_INVENTORY | OTA_STEP_SEND | OTA_STEP_VERIFY);
			if (nOtaStep != 0)
			{
			 	if (OtaStepByStep(pEntry, pChunk, pList, nCount, pEntry->nRetry, nOtaStep))
					break;
			}

			if (pEntry->bExitPending)
				break;

			// 전송에 실패하면 MTOR을 전송하여 다시 시도한다.
			TimesyncAndManyToOne(pEntry, 15*60);
		}

		// 전송을 수행하여 실패한 경우 Install/Scan을 수행하지 않는다.
		SetSkipNode(pList, nCount);

		// Install을 수행한다.
		if (!pEntry->bExitPending && (pEntry->nOtaStep & OTA_STEP_INSTALL))
		{
			ClearStateFlag(pList, nCount);
			for(i=0; i<6; i++)
			{
				if (pEntry->bExitPending)
					break;

				// 1초간 대기한다.
				usleep(1000000);

				if (OtaStepByStep(pEntry, pChunk, pList, nCount, 1, OTA_STEP_INSTALL))
					break;
	
				// 2번마다 한번씩 수행한다. - 1회 재시도, 1회 TIMESYNC/MTOR
			    if ((i % 2) == 1)
				{
					// Install에 실패한 노드가 존재하면 TIMESYNC/MTOR후에 15분간 대기한다.
					TimesyncAndManyToOne(pEntry, 15*60);
				}

			}
		}

		// Scan을 수행한다.
		if (!pEntry->bExitPending && (pEntry->nOtaStep & OTA_STEP_SCAN))
		{
			// 성공 플래그를 Clear 한다.
			ClearStateFlag(pList, nCount);

			for(i=0; i<3; i++)
			{
				if (pEntry->bExitPending)
					break;

                /** 2분간 Waiting을 한다 */
				usleep(2 * 60 * 1000000);

				// SCAN을 실시한다.
				if (OtaStepByStep(pEntry, pChunk, pList, nCount, 1, OTA_STEP_SCAN))
					break;

			    // SCAN에 실패하면 TIMESYNC/MTOR후 15분간 대기한다.	
			    // 단, SCAN만 실행하는 경우는 MTOR을 하지 않는다.
			    if (pEntry->nOtaStep != OTA_STEP_SCAN)
				    TimesyncAndManyToOne(pEntry, 15*60);
			}
		}
	}

	// 실패한 노드들의 상태를 전송한다.
	for(i=0; i<nCount; i++)
	{
		if (pList[i].bDone)
		{
			pEntry->nComplete++;
			continue;
		}

	    eui64toa(&pList[i].id, szGUID);
		UPGRADE_LOG("[%s] %s(HW=%0d.%0d, SW=%0d.%0d, BUILD=%0d, HOP=%0d, STEP=0x%02X) FAIL\r\n",
							szGUID, 
						    pList[i].szModel,
							pList[i].hw >> 4, pList[i].hw & 0x0f,
							pList[i].sw >> 4, pList[i].sw & 0x0f,
							((pList[i].build >> 4) * 10) + (pList[i].build & 0x0f),
							pList[i].nHops,
							pList[i].nOtaMask);
		otaResultEvent(pEntry->szTriggerId, &pList[i].id, 
                pEntry->bExitPending ? OTA_REASON_USER : OTA_REASON_ERROR, 
                pList[i].nOtaMask, pList[i].nError);
		pEntry->nFail++;
	}
	return pEntry->bExitPending ? FALSE : TRUE;
}

BOOL CDistributor::OtaStepByStep(OTAENTRY *pEntry, CChunk *pChunk, OTANODE *pList, int nCount, int nRetryCount, BYTE nOtaStep)
{
	COtaWorker	*pWorker;
	HANDLE	hEvent;
	time_t	now, start;
	int		i, nSeek, nRetry;
	BOOL	bAllDone = TRUE;

	// 개별적으로 OTA를 수행한다.
	time(&start);
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	for(nRetry=0; nRetry<nRetryCount; nRetry++)
	{
		if (pEntry->bExitPending)
			break;

		for(nSeek=0; nSeek<nCount;)
		{
			if (pEntry->bExitPending)
				break;

			if (pList[nSeek].bDone || pList[nSeek].bSkip)
			{
				nSeek++;
				continue;
			}

			time(&now);
			if (abs(now-start) > 3600)
			{
				// 한시간 마다 Watchdog을 Clear해준다.
				WatchdogClear();
			    time(&start);
			}

			pWorker = GetIdleWorker(pEntry);
			if (pWorker != NULL)
			{
				pList[nSeek].nOtaStep = nOtaStep;
				pList[nSeek].nTotal   = pChunk->GetSize();
				pList[nSeek].nState	  = STATE_OPEN;

				usleep(100000);
				ResetEvent(hEvent);
				pWorker->SensorOta(pEntry, &pList[nSeek], pChunk, hEvent);

				nSeek++;
			}
			else
			{
				// Worker가 비워지길 대기한다. (1초간)
				WaitForSingleObject(hEvent, 1);
			}
		}

		// 종료되지 않은 Worker가 있으면 종료되길 기다린다.
		while(IsActiveWorker()) usleep(1000000);

		// 모두 완료되었는지 검사한다.
		bAllDone = TRUE;
		for(i=0; i<nCount; i++)
		{
			if (!pList[i].bDone && !pList[i].bSkip)
			{
				bAllDone = FALSE;
				break;
			}
		}

		if (bAllDone)
			break;
	}
	CloseHandle(hEvent);
	return pEntry->bExitPending ? FALSE : bAllDone;
}

void CDistributor::ClearStateFlag(OTANODE *pList, int nCount)
{
	int		i;

	for(i=0; i<nCount; i++)
		pList[i].bDone = FALSE;
}

void CDistributor::SetSkipNode(OTANODE *pList, int nCount)
{
	int		i;

	for(i=0; i<nCount; i++)
	{
		if (!pList[i].bDone)	
			pList[i].bSkip = TRUE;
	}
}

void CDistributor::TimesyncAndManyToOne(OTAENTRY *pEntry, int nDelay)
{
	UINT	start, now, last;

	m_pSensorTimesync->Timesync();

	last = start = (UINT)uptime();
	while(!pEntry->bExitPending)
	{
		now = (UINT)uptime();
		if ((now-start) > (UINT)nDelay)
			break;	

		if ((now-last) >= 5)
		{
			XDEBUG("TimesyncAndManyToOne: %0d min %0d second\r\n", (now-start)/60, (now-start)%60);
			last = (UINT)uptime();
		}

		usleep(1000000);
	}
}

void CDistributor::Distribution(OTAENTRY *pEntry)
{
	char	szFileName[128];
	char	szBuffer[64];
    CMeterParser *parser;

	XDEBUG("------------------ DISTRIBUTION START ------------------------\r\n");
	XDEBUG(" Trigger ID : %s(Type=%0d)\r\n", pEntry->szTriggerId, pEntry->nType);
	XDEBUG("      Model : '%s'\r\n", pEntry->szModel);
	XDEBUG("--------------------------------------------------------------\r\n");
	UPGRADE_LOG("Distribution(ID=%s, Model=%s, Type=%s, Transfer=%s, Install=%s, Step=0x%04X, Thread=%0d, HW=%0d.%0d, SW=%0d.%0d, BUILD=%0d)\r\n",
				pEntry->szTriggerId, pEntry->szModel,
				GetOtaTypeString(pEntry->nType),
				GetOtaTransferTypeString(pEntry->nTransferType),
			    GetOtaInstallTypeString(pEntry->nInstallType),
				pEntry->nOtaStep,
				pEntry->nThread,
				pEntry->nNewHw >> 8, pEntry->nNewHw & 0xff,
				pEntry->nNewSw >> 8, pEntry->nNewSw & 0xff,
				pEntry->nNewBuild);	

	WatchdogClear();
	time(&pEntry->tmStart);
	pEntry->nStartTick = uptime();
	pEntry->nState = STATE_READY;

	for(;pEntry->nState != STATE_DONE;)
	{
		if (IsThreadExitPending() || pEntry->bExitPending)
			break;

		switch(pEntry->nState) {
		  case STATE_READY :
			   XDEBUG("Distribution - Ready\r\n");
			   // 다른 진행중인 작업이 있으면 대기한다.
			   if (m_pMetering->IsBusy() || m_pRecovery->IsBusy() || m_pPolling->IsBusy() || m_pMeterReader->IsBusy())
			   {
				   // 1초간 쉬고 다시 상태를 확인한다.
			   	   XDEBUG("Distribution - Busy (Active Schedule JOB), Please Wait\r\n");
				   usleep(1000000);
				   break;
			   }
               parser = m_pMeterParserFactory->SelectParser(pEntry->szModel);
               if(parser->IsSubCore())
               {
                   pEntry->nDestType = 1;
               }
               /** TODO : Parser가 같으면 같은 Type으로 인식한다. 이 부분은 추후에 MIU의 NODE_KINE와 명시적으로 같도록 해야 한다.
                 *
                 * 정확한 작업 일정 및 계획 산정이 필요하다
                 */
               strcpy(pEntry->szDestParserName, parser->GetName());

               if(strncmp(pEntry->szBinURL,"file://", 7)==0) {
			       pEntry->nState = STATE_DISTRIBUTE;
               } else {
			       pEntry->nState = STATE_DOWNLOAD;
               }
			   break;

		  case STATE_DOWNLOAD :
			   XDEBUG("Distribution - Download\r\n");
			   if (!DownloadFirmware(pEntry))
			   {
				   XDEBUG("ERROR: ---------> Firmware download fail\r\n");
				   UPGRADE_LOG("File download fail\r\n");
				   otaDownloadEvent(pEntry->szTriggerId, 1);
			   	   pEntry->nState = STATE_ERROR;
				   break;
			   }

			   UPGRADE_LOG("File download ok\r\n");
			   otaDownloadEvent(pEntry->szTriggerId, 0);
			   pEntry->nState = STATE_DISTRIBUTE;
			   break;
	
		  case STATE_DISTRIBUTE :
			   XDEBUG("Distribution - Distribute\r\n");
			   if (pEntry->nType == OTA_TYPE_CONCENTRATOR)
				    pEntry->nState = STATE_CONCENTRATOR;
			   else if (pEntry->nType == OTA_TYPE_COORDINATOR)
				    pEntry->nState = STATE_COORDINATOR;
			   else pEntry->nState = STATE_SENSOR;
			   break;

		  case STATE_CONCENTRATOR :
			   XDEBUG("Distribution - Concentrator\r\n");
			   UPGRADE_LOG("Concentrator upgrade start\r\n");
			   otaStartEvent(pEntry->szTriggerId);
			   if (!UpgradeConcentrator(pEntry, pEntry->szFile))
			   {
				   XDEBUG("ERROR: Concentrator firmware upgrade fail\r\n");
			   	   UPGRADE_LOG("Concentrator upgrade fail\r\n");
			   	   otaEndEvent(pEntry->szTriggerId, pEntry->bExitPending ? OTA_REASON_USER : OTA_REASON_ERROR, 
                           1, 0, pEntry->nFail);
			       pEntry->nState = STATE_ERROR;
				   break;
			   }
				
			   GetTimeString(szBuffer, ((UINT)uptime()-pEntry->nStartTick));
			   UPGRADE_LOG("Concentrator upgrade ok (Elapse=%s)\r\n", szBuffer);
			   otaEndEvent(pEntry->szTriggerId, OTA_REASON_NORMAL, 1, 1, pEntry->nFail);
			   pEntry->nState = STATE_OK;
			   break;

		  case STATE_COORDINATOR :
			   XDEBUG("Distribution - Coordinator\r\n");
			   UPGRADE_LOG("Coordinator upgrade start\r\n");
			   otaStartEvent(pEntry->szTriggerId);
			   if (!UpgradeCoordinator(pEntry, pEntry->szFile))
			   {
				   XDEBUG("ERROR: Coordinator firmware upgrade fail\r\n");
			   	   UPGRADE_LOG("Coordinator upgrade fail\r\n");
			   	   otaEndEvent(pEntry->szTriggerId, pEntry->bExitPending ? OTA_REASON_USER : OTA_REASON_ERROR, 
                           1, 0, pEntry->nFail);
			       pEntry->nState = STATE_ERROR;
				   break;
			   }

			   GetTimeString(szBuffer, ((UINT)uptime()-pEntry->nStartTick));
			   UPGRADE_LOG("Coordinator upgrade ok (Elapse=%s)\r\n", szBuffer);
			   otaEndEvent(pEntry->szTriggerId, OTA_REASON_NORMAL, 1, 1, pEntry->nFail);
			   pEntry->nState = STATE_OK;
			   break;

		  case STATE_SENSOR :
			   XDEBUG("Distribution - Sensor\r\n");
			   UPGRADE_LOG("Sensor upgrade start\r\n");
			   otaStartEvent(pEntry->szTriggerId);
               if(strncmp(pEntry->szBinURL,"file://", 7)==0) {
			       sprintf(szFileName, "%s", (pEntry->szBinURL + 7));
               }else {
                      if(pEntry->nDestType == 1)
                      {
                           sprintf(szFileName, "/app/fw/sensor/%s.bin", pEntry->szModel);
                      }
                      else 
                      {
                          sprintf(szFileName, "/app/fw/sensor/%s.ebl", pEntry->szModel);
                      }
               }
			   if (!UpgradeSensor(pEntry, szFileName))
			   {
				   XDEBUG("ERROR: Sensor firmware upgrade fail\r\n");
			   	   UPGRADE_LOG("Sensor upgrade fail.\r\n");
			   	   otaEndEvent(pEntry->szTriggerId, pEntry->bExitPending ? OTA_REASON_USER : OTA_REASON_ERROR, 
                           pEntry->nTotal, pEntry->nComplete, pEntry->nFail);
			       pEntry->nState = STATE_ERROR;
				   break;
			   }

			   GetTimeString(szBuffer, ((UINT)uptime()-pEntry->nStartTick));
			   UPGRADE_LOG("Sensor upgrade end (Elapse=%s)\r\n", szBuffer);
			   otaEndEvent(pEntry->szTriggerId, OTA_REASON_NORMAL, 
                       pEntry->nTotal, pEntry->nComplete, pEntry->nFail);
			   pEntry->nState = STATE_OK;
			   break;

		  case STATE_OK :
			   XDEBUG("Distribution - OK\r\n");
			   pEntry->nState = STATE_DONE;
			   break;

		  case STATE_ERROR :
			   XDEBUG("Distribution - Error\r\n");
			   pEntry->nState = STATE_DONE;
			   break;
		}
	}

	XDEBUG("------------------ DISTRIBUTION END ------------------------\r\n");
	UPGRADE_LOG("");
}

BOOL CDistributor::OnActiveThread()
{
	OTAENTRY	*pEntry;

	WatchdogClear();

	if (m_List.GetCount() == 0)
		return TRUE;

	m_bBusy = TRUE;
	for(;!IsThreadExitPending();)
	{
		m_Locker.Lock();
		pEntry = m_List.GetHead();
		if (pEntry != NULL) pEntry->bActive = TRUE;
		m_Locker.Unlock();

		if (pEntry == NULL)
			break;

		Distribution(pEntry);

		m_Locker.Lock();
		m_List.RemoveAt((POSITION)pEntry->nPosition);
		FreeItem(pEntry);
		m_Locker.Unlock();
	}
	m_bBusy = FALSE;
	return TRUE;
}

BOOL CDistributor::OnTimeoutThread()
{
	return OnActiveThread();
}
