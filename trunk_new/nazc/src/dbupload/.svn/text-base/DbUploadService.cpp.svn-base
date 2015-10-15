#include <stdio.h>
#include <string.h>

#include "DbUploadService.h"
#include "Variable.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "logdef.h"
#include "DebugUtil.h"
#include "if4api.h"
#include "if4frame.h"

#include "types/identification.h"
#include "rep/DBRepository.h"
#include "rep/MeterHelper.h"
#include "rep/MeteringHelper.h"
#include "rep/LogHelper.h"

//////////////////////////////////////////////////////////////////////
// CDbUploadService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDbUploadService		*m_pService = NULL;

//////////////////////////////////////////////////////////////////////
// CDbUploadService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDbUploadService::CDbUploadService(int nNzcNumber, char *ipaddr, int nPort, BOOL bDebug)
{
	m_pService 		= this;

	strcpy(m_szServer,ipaddr);
	m_nNzcNumber    = nNzcNumber;
	m_nPort			= nPort;
    m_bDebug        = bDebug;

    theRepository.Initialize();
}

CDbUploadService::~CDbUploadService()
{
}

//////////////////////////////////////////////////////////////////////
// CDbUploadService Override Functions
//////////////////////////////////////////////////////////////////////

int CDbUploadService::Startup()
{
	int		ret=0;

    SET_DEBUG_MODE(m_bDebug ? 0 : 1);

	XDEBUG("------------------- UPLOAD -----------------\r\n");
	XDEBUG("SEND METERING DATA: SERVER=%s, PORT=%0d\r\n", m_szServer, m_nPort);

	if(!OnStartService())
    {
	    XDEBUG("------------------ UPLOAD END ---------------\r\n");
        return -IF4ERR_UNKNOWN_ERROR;
    }

	XDEBUG("%s:%d Try upload. (try=%d)\r\n", m_szServer, m_nPort, 1);
	//UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Try upload. (try=%d)\n", m_szServer, m_nPort, 1);
	ret=OnExecuteService();
	XDEBUG("------------------ UPLOAD END ---------------\r\n");
	return ret;
}

void CDbUploadService::Shutdown()
{
	OnStopService();
}

//////////////////////////////////////////////////////////////////////
// CDbUploadService Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CDbUploadService::OnStartService()
{
	VARAPI_Initialize(NULL, (VAROBJECT *)m_Root_node, FALSE);
	IF4API_Initialize(m_nPort+111, NULL);
	IF4API_SetID(m_nNzcNumber);
	return TRUE;
}

void CDbUploadService::OnStopService()
{
	IF4API_Destroy();
	VARAPI_Destroy();
}

void CDbUploadService::UpdateLog(const IF4Invoke *pInvoke, const char * szType, const int nError)
{
    int nOrigBytes, nCompBytes;

    switch(nError)
    {
        case IF4ERR_NOERROR:
            nOrigBytes = pInvoke->nSourceLength - sizeof(IF4_HEADER) - sizeof(IF4_TAIL);
            nCompBytes = pInvoke->nLength - sizeof(IF4_HEADER) - sizeof(IF4_TAIL);
            XDEBUG("%s:%d %s Sending ok. (Orig %d Bytes, Compressed %d Bytes, %d%%)\r\n", 
                    m_szServer, m_nPort, szType, nOrigBytes, nCompBytes, nOrigBytes > 0 ? (int)(nCompBytes * 100 / nOrigBytes) : 0);
            UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d %s Sending ok. (Orig %d Bytes, Compressed %d Bytes, %d%%)\n", 
                    m_szServer, m_nPort, szType, nOrigBytes, nCompBytes, nOrigBytes > 0 ? (int)(nCompBytes * 100 / nOrigBytes) : 0);
            break;
        case IF4ERR_CANNOT_CONNECT:
            XDEBUG("%s:%d %s Cannot connect server.\r\n", m_szServer, m_nPort, szType);
            UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d %s Cannot connect server.\n", m_szServer, m_nPort, szType);
            break;
        default:
            XDEBUG("%s:%d %s Sending fail.\r\n", m_szServer, m_nPort, szType);
            UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d %s Sending fail.\n", m_szServer, m_nPort, szType);
            break;
	}

}

int CDbUploadService::ProcessInventory(IF4Invoke *pInvoke)
{
    DBFetchData         fetchData;
    BYTE                szBuff[4096];
    IF4_TYPER_HEADER    *pHeader;
    TYPER_INVENTORY     *pInventory;
    TYPER_ID            *pId;
    METERTBLINS         *pIns;
    Row                 *pRow;
    int                 i, cnt, nError=IF4ERR_NOERROR, nSeek=0, nPrevSeek=0;
    BYTE                len=0;
    UINT                *pIdList;
    int                 nMaxLen = sizeof(szBuff) - 128;
    int                 nSendCnt=0;
    TIMESTAMP           timeStamp;

    /** Meter Inventory */
    memset(szBuff, 0, sizeof(szBuff));
    memset(&fetchData, 0, sizeof(DBFetchData));
    memset(&timeStamp, 0, sizeof(TIMESTAMP));

    m_pMeterHelper->TryUpload(&fetchData);
    m_pMeterHelper->DumpResult(&fetchData);

    cnt = fetchData.nCount;

    /** 전달할 것이 없으면 리턴한다 */
    if(cnt <= 0)
    {
        return IF4ERR_NOERROR;
    }

    pIdList = (UINT *)MALLOC(sizeof(UINT)*cnt);
    memset(pIdList, 0, sizeof(UINT)*cnt);

    pRow = m_pMeterHelper->GetHeadRow(&fetchData);
    for(i=0;i<cnt && pRow && nSeek < nMaxLen;i++, pRow = m_pMeterHelper->GetNextRow(pRow)) {
        pIns = m_pMeterHelper->GetData(pRow);
        pHeader = (IF4_TYPER_HEADER *)(szBuff+nSeek) ;                  nSeek += sizeof(IF4_TYPER_HEADER);
        pHeader->type = IF4_TYPER_INVENTORY;
        pInventory = (TYPER_INVENTORY *)(szBuff + nSeek);
        pInventory->shortId = HostToBigInt(pIns->_id);                  nSeek += 4;
        pInventory->modemId.idType = ID_TYPE_EUI64;                     nSeek ++;
        pInventory->modemId.idLen = 8;                                  nSeek ++;
        memcpy(pInventory->modemId.id, &pIns->modemId, 8);              nSeek += 8;
        szBuff[nSeek] = pIns->nPortNum;                                 nSeek ++;
        pId = (TYPER_ID *)(szBuff + nSeek);
        pId->idType = ID_TYPE_PRINTABLE;                                nSeek ++;
        pId->idLen = pIns->szSerial ? strlen(pIns->szSerial) : 0;       nSeek ++;
        if(pId->idLen > 0) {
            memcpy(pId->id, pIns->szSerial, pId->idLen);                nSeek += pId->idLen;
        }
        HostIntToBigStream(pIns->nParserType, &szBuff[nSeek]);          nSeek += 4;
        szBuff[nSeek] = pIns->nServiceType;                             nSeek ++;
        szBuff[nSeek] = pIns->nVendor;                                  nSeek ++;
        len = pIns->szModel ? strlen(pIns->szModel) : 0;      
        szBuff[nSeek] = len;                                            nSeek ++;
        if(len > 0) {
            memcpy(&szBuff[nSeek], pIns->szModel, len);                 nSeek += len;
        }
        szBuff[nSeek] = (BYTE)pIns->hwVer;                              nSeek ++;
        szBuff[nSeek] = (BYTE)((pIns->hwVer - (BYTE)(pIns->hwVer))*10); nSeek ++;
        szBuff[nSeek] = (BYTE)pIns->swVer;                              nSeek ++;
        szBuff[nSeek] = (BYTE)((pIns->swVer - (BYTE)(pIns->swVer))*10); nSeek ++;
        szBuff[nSeek] = 0;                                              nSeek ++;   // Meter Build는 0 으로 설정
        memcpy(&szBuff[nSeek], HostToBigTimestamp(&pIns->installTime,&timeStamp), sizeof(TIMESTAMP));  
                                                                        nSeek += sizeof(TIMESTAMP);
        szBuff[nSeek] = (BYTE)pIns->nPeriod;                            nSeek ++;
        pHeader->length = HostToBigShort(nSeek - nPrevSeek - sizeof(IF4_TYPER_HEADER));
        nPrevSeek = nSeek;

        pIdList[i] = pIns->_id;
        nSendCnt++;
    }

    m_pMeterHelper->FreeResult(&fetchData);

    if(nSendCnt > 0)
    {
        nError = IF4API_SendTypeR(pInvoke, nSendCnt, szBuff, nSeek);

        UpdateLog(pInvoke, "Invt", nError);

        if(nError == IF4ERR_NOERROR) 
        {
            m_pMeterHelper->UploadComplete(cnt, pIdList);
        }
    }

    FREE(pIdList);

    return nError;
}

int CDbUploadService::ProcessChannelConfigure(IF4Invoke *pInvoke)
{
    DBFetchData         fetchData;
    BYTE                szBuff[4096];
    IF4_TYPER_HEADER    *pHeader=NULL;
    TYPER_CHANNEL       *pChannel=NULL;
    TYPER_CHANNEL_CONFIGURE *pChConfigure;
    CHANNELCFGINS       *pIns;
    Row                 *pRow;
    int                 i, cnt, nError=IF4ERR_NOERROR, nSeek=0, nPrevSeek=0;
    UINT                *pIdList;
    int                 nMaxLen = sizeof(szBuff) - 128;
    UINT                nPrevMeterId = 0;
    int                 nChCnt=0, nSendCnt=0;
    int                 nChOffset=-1;

    /** Channel Configure */
    memset(szBuff, 0, sizeof(szBuff));
    memset(&fetchData, 0, sizeof(DBFetchData));

    m_pChannelCfgHelper->TryUpload(&fetchData);
    m_pChannelCfgHelper->DumpResult(&fetchData);

    cnt = fetchData.nCount;

    /** 전달할 것이 없으면 리턴한다 */
    if(cnt <= 0)
    {
        return IF4ERR_NOERROR;
    }
    pIdList = (UINT *)MALLOC(sizeof(UINT)*cnt);
    memset(pIdList, 0, sizeof(UINT)*cnt);

    pRow = m_pChannelCfgHelper->GetHeadRow(&fetchData);
    for(i=0;i<cnt && pRow && nSeek < nMaxLen;i++, pRow = m_pChannelCfgHelper->GetNextRow(pRow)) {
        pIns = m_pChannelCfgHelper->GetData(pRow);

        if(nPrevMeterId != pIns->_meterId)
        {
            if(pChannel != NULL) {
                if(nChOffset > 0)
                {
                    szBuff[nChOffset] = nChCnt;
                }
                nChCnt = 0;
                nChOffset = -1;
            }
            if(pHeader != NULL) {
                pHeader->length = HostToBigShort(nSeek - nPrevSeek - sizeof(IF4_TYPER_HEADER));
                nPrevSeek = nSeek;
            }
            pHeader = (IF4_TYPER_HEADER *)(szBuff+nSeek) ;              nSeek += sizeof(IF4_TYPER_HEADER);
            pHeader->type = IF4_TYPER_CONFIGURE;
            pChannel = (TYPER_CHANNEL *)(szBuff + nSeek);
            pChannel->shortId = HostToBigInt(pIns->_meterId);           nSeek += 4;
            pChannel->modemId.idType = ID_TYPE_EUI64;                   nSeek ++;
            pChannel->modemId.idLen = 8;                                nSeek ++;
            memcpy(pChannel->modemId.id, &pIns->modemId, 8);            nSeek += 8;
            nChOffset = nSeek;                                          nSeek++;        // Channel Cnt
            nPrevMeterId = pIns->_meterId;
            nSendCnt++;
        }
        pChConfigure = (TYPER_CHANNEL_CONFIGURE *)(szBuff + nSeek); 
        pChConfigure->index = pIns->nIndex;
        pChConfigure->unit = pIns->nUnit;
        pChConfigure->sigExp = pIns->nSigExp;
        pChConfigure->valueType = pIns->nValueType;
        pChConfigure->objectType = pIns->nObjType;
        pChConfigure->channelType = pIns->nChType;
                                                                        nSeek += sizeof(TYPER_CHANNEL_CONFIGURE);
        nChCnt ++;
        pIdList[i] = pIns->_id;
    }
    if(pChannel != NULL) {
        if(nChOffset > 0)
        {
            szBuff[nChOffset] = nChCnt;
        }
    }
    if(pHeader != NULL) {
        pHeader->length = HostToBigShort(nSeek - nPrevSeek - sizeof(IF4_TYPER_HEADER));
    }

    m_pChannelCfgHelper->FreeResult(&fetchData);

    if(nSendCnt > 0)
    {
        nError = IF4API_SendTypeR(pInvoke, nSendCnt, szBuff, nSeek);

        UpdateLog(pInvoke, "Conf", nError);
    
        if(nError == IF4ERR_NOERROR) 
        {
            m_pChannelCfgHelper->UploadComplete(cnt, pIdList);
        }
    }

    FREE(pIdList);

    return nError;
}

int CDbUploadService::ProcessLoadProfile(IF4Invoke *pInvoke)
{
    DBFetchData         lpFetchData, logFetchData;
    BYTE                szBuff[8192*10];
    IF4_TYPER_HEADER    *pHeader=NULL;
    TYPER_METERING      *pMetering=NULL;
    TYPER_BASEPULSE     *pBasePulse=NULL;
    TYPER_BASEPULSE_VALUE   *pBasePulseValue=NULL;
    TYPER_LP_VALUE      *pLpValue;
    TYPER_LOG           *pLog;
    TYPER_LOG_VALUE     *pLogValue;
    METERINGDATAINS     *pLpIns = NULL;
    LOGDATAINS          *pLogIns = NULL;
    Row                 *pLpRow = NULL,*pLogRow = NULL;
    int                 i, j, k, idxLp, idxLog;
    int                 nLpEntryCnt, nLogEntryCnt;
    int                 idxLpId=0, idxLogId=0;
    int                 nError, nSeek=0, nPrevSeek=0;
    UINT                *pLpIdList=NULL, *pLogIdList=NULL;
    int                 nMaxLen = sizeof(szBuff) - 2048;
    int                 nLpCnt=0, nLogCnt=0, nSendCnt=0;
    UINT                nCurrentMeterId=0;
    int                 nSendLpCnt;
    int                 nReturnCnt=0;
    int                 nChCnt=0;
    int                 nLongSize = sizeof(unsigned long long);
    int                 nIntSize = sizeof(unsigned int);

    TIMETICK            start, end;

    /** Load Profile */
    memset(szBuff, 0, sizeof(szBuff));
    memset(&lpFetchData, 0, sizeof(DBFetchData));
    memset(&logFetchData, 0, sizeof(DBFetchData));

    XDEBUG(" Get Metering\r\n");
    GetTimeTick(&start);
    XDEBUG(" Get Load Profile\r\n");
    m_pMeteringHelper->TryUpload(&lpFetchData);
    XDEBUG(" Get Event Log\r\n");
    m_pLogHelper->TryUpload(&logFetchData);
    GetTimeTick(&end);
    XDEBUG(" TIME %d\r\n", GetTimeInterval(&start, &end));
    XDEBUG(" Metering %d Log %d\r\n", lpFetchData.nCount, logFetchData.nCount);
    //m_pMeteringHelper->DumpResult(&lpFetchData);
    //m_pLogHelper->DumpResult(&logFetchData);

    /** 전달할 것이 없으면 리턴한다 */
    if(lpFetchData.nCount <= 0 && logFetchData.nCount <= 0)
    {
        return 0;
    }

    /** 전체 갯수 계산 */
    nLpEntryCnt = lpFetchData.nCount;
    pLpRow = m_pMeteringHelper->GetHeadRow(&lpFetchData);
    for(i=0;i<nLpEntryCnt && pLpRow;i++, pLpRow = m_pMeteringHelper->GetNextRow(pLpRow)) {
        pLpIns = m_pMeteringHelper->GetData(pLpRow);
        nLpCnt += pLpIns->nTotLpCnt;
    }
    nLogEntryCnt = logFetchData.nCount;
    pLogRow = m_pLogHelper->GetHeadRow(&logFetchData);
    for(i=0;i<nLogEntryCnt && pLogRow;i++, pLogRow = m_pLogHelper->GetNextRow(pLogRow)) {
        pLogIns = m_pLogHelper->GetData(pLogRow);
        nLogCnt += pLogIns->nLogCnt;
        //XDEBUG("pLogIns->logCnt %d\r\n", pLogIns->nLogCnt);
    }
    XDEBUG(" nLpCnt %d nLogCnt %d\r\n", nLpCnt, nLogCnt);

    pLpRow = pLogRow = NULL;
    pLpIns = NULL;
    pLogIns = NULL;
    if(nLpEntryCnt > 0)
    {
        pLpIdList = (UINT *)MALLOC(sizeof(UINT)*nLpCnt);
        memset(pLpIdList, 0, sizeof(UINT)*nLpCnt);
        pLpRow = m_pMeteringHelper->GetHeadRow(&lpFetchData);
        pLpIns = m_pMeteringHelper->GetData(pLpRow);
    }
    if(nLogEntryCnt > 0)
    {
        pLogIdList = (UINT *)MALLOC(sizeof(UINT)*nLogCnt);
        memset(pLogIdList, 0, sizeof(UINT)*nLogCnt);
        pLogRow = m_pLogHelper->GetHeadRow(&logFetchData);
        pLogIns = m_pLogHelper->GetData(pLogRow);
    }
    for(idxLp=0, idxLog=0;
            (idxLp<nLpEntryCnt || idxLog<nLogEntryCnt) && 
            (pLpRow || pLogRow) && nSeek < nMaxLen; ) {

        if(pHeader != NULL) {
            pHeader->length = HostToBigShort(nSeek - nPrevSeek - sizeof(IF4_TYPER_HEADER));
            nPrevSeek = nSeek;
        }

        pHeader = (IF4_TYPER_HEADER *)(szBuff+nSeek) ;                      nSeek += sizeof(IF4_TYPER_HEADER);
        pHeader->type = IF4_TYPER_METERING;
        pMetering = (TYPER_METERING *)(szBuff + nSeek);
        if(pLpIns && (!pLogIns || pLpIns->_meterId <= pLogIns->_meterId))
        {
            nChCnt = pLpIns->nChannelCnt;
            pMetering->shortId = HostToBigInt(pLpIns->_meterId);                nSeek += 4;     
            pMetering->modemId.idType = ID_TYPE_EUI64;                          nSeek ++;
            pMetering->modemId.idLen = 8;                                       nSeek ++;
            memcpy(pMetering->modemId.id, &pLpIns->modemId, 8);                 nSeek += 8;

            szBuff[nSeek] = pLpIns->nChannelCnt;                                nSeek++;        // Channel Cnt
            szBuff[nSeek] = (BYTE)0x01;                                         nSeek++;        // BasePulse Cnt

            for(j=0;j<pLpIns->nMeteringCnt;j++) 
            {
                //XDEBUG(" BP nSeek %d\r\n", nSeek);
                pBasePulse = (TYPER_BASEPULSE *)(szBuff + nSeek);           nSeek += sizeof(TYPER_BASEPULSE);
                TimestampToTimestamp5(&pLpIns->pMetering[j]->meterTime, &pBasePulse->lastTime);       
                TimestampToTimestamp5(&pLpIns->pMetering[j]->baseTime, &pBasePulse->baseTime);        

                for(k=0;k<nChCnt;k++) 
                {
                    pBasePulseValue = (TYPER_BASEPULSE_VALUE *)(szBuff + nSeek);    nSeek += sizeof(TYPER_BASEPULSE_VALUE);
                    memcpy(&pBasePulseValue->lastValue, (char*)pLpIns->pMetering[j]->pLastValue + (k * nLongSize), nLongSize);
                    memcpy(&pBasePulseValue->baseValue, (char*)pLpIns->pMetering[j]->pBaseValue + (k * nLongSize), nLongSize);
                }
            }

            nSendLpCnt = pLpIns->nTotLpCnt;
            szBuff[nSeek] = nSendLpCnt;                                         nSeek ++;
            //XDEBUG(" TotalLpCnt %d\r\n", pLpIns->nTotLpCnt);
            //XDEBUG(" SendLPCnt %d\r\n", nSendLpCnt);

            for(j=0;j<pLpIns->nMeteringCnt;j++) 
            {
                for(k=0;k<pLpIns->pMetering[j]->nLpCnt; k++)
                {
                    pLpValue = (TYPER_LP_VALUE *)(szBuff + nSeek);              nSeek += sizeof(TYPER_LP_VALUE);
                    TimestampToTimestamp5(&pLpIns->pMetering[j]->pLoadProfile[k]->_snapShotTime, &pLpValue->time);                                   

                    nSendLpCnt ++;
                    memcpy((char*)pLpValue->values, (char*)pLpIns->pMetering[j]->pLoadProfile[k]->pValue, 
                            nIntSize * nChCnt);   nSeek += (nIntSize * nChCnt);
                    pLpIdList[idxLpId] = pLpIns->pMetering[j]->pLoadProfile[k]->_id;        idxLpId++;
                }
            }

            nCurrentMeterId = pLpIns->_meterId;
            pLpRow = m_pMeteringHelper->GetNextRow(pLpRow);
            pLpIns = m_pMeteringHelper->GetData(pLpRow);
            idxLp ++;

        } else
        {
            pMetering->shortId = HostToBigInt(pLogIns->_meterId);               nSeek += 4;
            pMetering->modemId.idType = ID_TYPE_EUI64;                          nSeek ++;
            pMetering->modemId.idLen = 8;                                       nSeek ++;
            memcpy(pMetering->modemId.id, &pLogIns->modemId, 8);                nSeek += 8;

            szBuff[nSeek] = 0;                                                  nSeek++;        // Channel Cnt
            szBuff[nSeek] = 0;                                                  nSeek++;        // BasePulse Cnt
            szBuff[nSeek] = 0;                                                  nSeek++;        // LP Cnt
            nCurrentMeterId = pLogIns->_meterId;
        }

        pLog = (TYPER_LOG *)(szBuff + nSeek);                                   nSeek += sizeof(TYPER_LOG);
        if(pLogIns && nCurrentMeterId == pLogIns->_meterId)
        {
            pLog->logCnt = pLogIns->nLogCnt;

            for(j=0;j<pLog->logCnt;j++)
            {
                pLogValue = (TYPER_LOG_VALUE *)(szBuff + nSeek);                nSeek += sizeof(TYPER_LOG_VALUE);
                pLogValue->category = pLogIns->pLog[j]->nCategory;
                pLogValue->length = pLogIns->pLog[j]->nLogSize;
                TimestampToTimestamp5(&pLogIns->pLog[j]->logTime, &pLogValue->time);                                   
                if(pLogValue->length > 0)
                {
                    memcpy(pLogValue->data, pLogIns->pLog[j]->szLog, pLogValue->length); nSeek += pLogValue->length;
                }
                pLogIdList[idxLogId] = pLogIns->pLog[j]->_id;   idxLogId ++;
            }

            pLogRow = m_pLogHelper->GetNextRow(pLogRow);
            pLogIns = m_pLogHelper->GetData(pLogRow);
            idxLog ++;
        }
        else
        {
            pLog->logCnt = 0;
        }

        nSendCnt++;
    }

    if(pHeader != NULL) {
        pHeader->length = HostToBigShort(nSeek - nPrevSeek - sizeof(IF4_TYPER_HEADER));
    }

    m_pMeteringHelper->FreeResult(&lpFetchData);
    m_pLogHelper->FreeResult(&logFetchData);

    if(nSendCnt > 0)
    {
        nError = IF4API_SendTypeR(pInvoke, nSendCnt, szBuff, nSeek);
        XDEBUG("nSendCnt %d\r\n", nSendCnt);
        //XDUMP((char *)szBuff, nSeek);

        if(nError == IF4ERR_NOERROR) 
        {
            //XDEBUG("idxLpId %d idxLogId %d\r\n", idxLpId, idxLogId);
            if(idxLpId > 0)
            {
                m_pMeteringHelper->UploadComplete(idxLpId, pLpIdList);
            }
            if(idxLogId > 0)
            {
                m_pLogHelper->UploadComplete(idxLogId, pLogIdList);
            }
            nReturnCnt = idxLpId;
    } 

    UpdateLog(pInvoke, "Mtr", nError);
    }

    if(pLpIdList) FREE(pLpIdList);
    if(pLogIdList) FREE(pLogIdList);

    return nReturnCnt;
}

int CDbUploadService::OnExecuteService()
{
	CIF4Invoke      invoke(m_szServer, m_nPort, 120);

    ProcessInventory(invoke.GetHandle());
    ProcessChannelConfigure(invoke.GetHandle());
    return ProcessLoadProfile(invoke.GetHandle());
}



