//////////////////////////////////////////////////////////////////////
//
//	TransactionManager.cpp: implementation of the CTransactionManager class.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2009 NuriTelecom Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "TransactionManager.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "Variable.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "if4frame.h"
#include "varapi.h"
#include "BatchJob.h"

//  [10/27/2010 DHKim]
//#include "gdbm/gdbm.h"

//////////////////////////////////////////////////////////////////////
// Global Variable Definitions
//////////////////////////////////////////////////////////////////////

#define     TR_DIR      "/app/tr"

#define     TR_IDX_TARGET   0
#define     TR_IDX_COMMAND  1
#define     TR_IDX_OPTION   2
#define     TR_IDX_DAY      3
#define     TR_IDX_NICE     4
#define     TR_IDX_RETRY    5

#define     PARAM_CNT       6

#define     ENDDEVICE_INFO_UPDATE           TRUE
#define     SKIP_ENDDEVICE_INFO_UPDATE      FALSE

#define     EVT_TYPE_CODE   0x01
#define     EVT_TYPE_DATA   0x02

#define     DAY_SEC         86400

extern IF4_COMMAND_TABLE	m_CommandHandler[];
extern BYTE    m_nTransactionSaveDay;

CTransactionManager	*m_pTransactionManager = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransactionManager::CTransactionManager() 
{
	m_pTransactionManager = this;
    m_nLastTransactionID = 0;
}

CTransactionManager::~CTransactionManager()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CTransactionManager::Initialize()
{
    LoadValues();
    ResolveTimeoutTransaction(TRUE);
	return TRUE;
}

void CTransactionManager::Destroy()
{
}

//////////////////////////////////////////////////////////////////////
// Protected Methods
//////////////////////////////////////////////////////////////////////
BOOL CTransactionManager::LoadValues(void)
{
    FILE * fp;

    if((fp = fopen(TR_DIR "/tr.id", "r")) != NULL) {
        fscanf(fp,"%hu", &m_nLastTransactionID);
        fclose(fp);
        return TRUE;
    }
    return FALSE;
}

BOOL CTransactionManager::UpdateTransactionID(WORD next)
{
    FILE *      fp;

    if((fp = fopen(TR_DIR "/tr.id", "w")) != NULL) {
        fprintf(fp,"%u", next);
        fclose(fp);
        m_nLastTransactionID = next;
        return TRUE;
    } 
    return FALSE;
}

WORD CTransactionManager::GetNextTransactionID(void)
{
    WORD next = m_nLastTransactionID + 1;

    return next ? next : next + 1;
}

BOOL CTransactionManager::CreateTransactionRequest(WORD nTrID, MIBValue *pValue)
{
    char        trPath[64];
    BOOL        bOk = FALSE;

    sprintf(trPath, "%s/req/%u", TR_DIR, nTrID);
    if(VARAPI_Serialize(trPath,pValue)) bOk = TRUE;

    return bOk;
}

BOOL CTransactionManager::AppendEndDeviceTransaction(EUI64 *pId, WORD trID, cetime_t nTime, signed char nNice, BYTE nTry, BYTE nState, int nError)
{
    char                    trPath[64];
    char                    szGUID[17];
    EndDeviceTransaction    endTrInfo;
    FILE                    *fp;
    BOOL                    bOk = FALSE;

    memset(&endTrInfo, 0, sizeof(EndDeviceTransaction));

    endTrInfo.trID = trID;
    endTrInfo.trCreateTime = nTime;
    endTrInfo.trLastTime = nTime;
    endTrInfo.trNice = nNice;
    endTrInfo.trTry = nTry;
    endTrInfo.trState = nState;
    endTrInfo.trError = nError;

    EUI64ToStr(pId, szGUID);

    sprintf(trPath, "%s/mbr/%s", TR_DIR, szGUID);
    if((fp=fopen(trPath,"a"))) {
        if(fwrite(&endTrInfo, sizeof(EndDeviceTransaction), 1, fp)==1) {
            bOk = TRUE;
        }
        fclose(fp);
    }

    if(bOk) {
        sprintf(trPath, "%s/map/%u", TR_DIR, trID);
        if((fp=fopen(trPath,"w"))) {
            if(fwrite(pId, sizeof(EUI64), 1, fp)!=1) bOk = FALSE;
        }else {
            bOk = FALSE;
        }
        fclose(fp);
    }

    return bOk;
}

BOOL CTransactionManager::AppendEndDeviceTransaction(WORD trID, cetime_t nTime, MIBValue *pValue, int nCount)
{
    if(nCount < PARAM_CNT) return FALSE;

    return AppendEndDeviceTransaction(
                (EUI64 *)pValue[TR_IDX_TARGET].stream.p, 
                trID, 
                nTime, 
                (signed char) pValue[TR_IDX_NICE].stream.u8,
                0,
                TR_STATE_WAIT,
                IF4ERR_NOERROR
            );
}

BOOL CTransactionManager::UpdateEndDeviceTransaction(EUI64 * pId, int idx, EndDeviceTransaction * pEndTrInfo)
{
    char                    trPath[64];
    char                    szGUID[17];
    FILE                    *fp;
    BOOL                    bOk = FALSE;

    if(!pEndTrInfo) return bOk;

    if(idx >= 0) {
        EUI64ToStr(pId, szGUID);
        sprintf(trPath, "%s/mbr/%s", TR_DIR, szGUID);

        if((fp=fopen(trPath,"r+"))) {
            if(!fseek(fp, (long) (sizeof(EndDeviceTransaction) * idx), SEEK_SET) &&
                    fwrite(pEndTrInfo,sizeof(EndDeviceTransaction), 1, fp) == 1) {
                bOk = TRUE;
            }
            fclose(fp);
        }
    }

    return bOk;
}

BOOL CTransactionManager::UpdateEndDeviceTransaction(EUI64 *pId, WORD trID, cetime_t nTime, BOOL bSuccNice, BOOL bSuccTry, BYTE nState, int nError)
{
    EndDeviceTransaction    *pEndTrInfo = NULL;
    BOOL                    bOk = FALSE;
    int                     idx = -1, nCount = 0, i;

    if(LoadEndDeviceTransaction(pId, &pEndTrInfo, &nCount)) {
        for(i=0;i<nCount;i++) {
            if(pEndTrInfo[i].trID == trID) {
                pEndTrInfo[i].trLastTime = nTime;
                pEndTrInfo[i].trState = nState;
                pEndTrInfo[i].trError = nError;
                if(bSuccNice && (pEndTrInfo[i].trNice + 1) < TR_MAX_NICE ) pEndTrInfo[i].trNice ++;
                if(bSuccTry && (pEndTrInfo[i].trTry + 1) < TR_MAX_RETRY ) pEndTrInfo[i].trTry ++;
                idx = i;
                break;
            }
        }
    }

    if(idx >= 0) {
        bOk = UpdateEndDeviceTransaction(pId, idx, &pEndTrInfo[idx]);
    }

    FREE(pEndTrInfo);

    return bOk;
}

BOOL CTransactionManager::LoadEndDeviceTransaction(EUI64 *pId, EndDeviceTransaction **ppEndTrInfo, int *nCount)
{
    char                    trPath[64];
    char                    szGUID[17];
    EndDeviceTransaction    *pEndTr=NULL;
    BOOL                    bOk = FALSE;
    struct stat             statSt;
    FILE                    *fp;
    int                     cnt;

    *nCount = 0;
    EUI64ToStr(pId, szGUID);
    sprintf(trPath, "%s/mbr/%s", TR_DIR, szGUID);

    if(stat(trPath, &statSt)) return bOk;

    /*-- 잘못된 Size를 가지고 있을 때 
     *-- 내부적으로 Internal error를 발생키셔야 한다 --*/
    if(statSt.st_size % sizeof(EndDeviceTransaction)) return bOk;

    cnt=(int)(statSt.st_size/sizeof(EndDeviceTransaction));

    pEndTr = (EndDeviceTransaction *)MALLOC(sizeof(EndDeviceTransaction) * cnt);
    
    if((fp=fopen(trPath,"r"))) 
    {
        if(fread(pEndTr, sizeof(EndDeviceTransaction), cnt, fp) == (size_t)cnt) 
        {
            *nCount = cnt;
            bOk = TRUE;
        }
        fclose(fp);
    }
    *ppEndTrInfo = pEndTr;

    return bOk;
}

BOOL CTransactionManager::LoadEndDeviceTransaction(EUI64 *pId, WORD nTrID, EndDeviceTransaction *pEndTrInfo)
{
    char                    trPath[64];
    char                    szGUID[17];
    EndDeviceTransaction    endTrInfo;
    BOOL                    bOk = FALSE;
    struct stat             statSt;
    FILE                    *fp;

    EUI64ToStr(pId, szGUID);
    sprintf(trPath, "%s/mbr/%s", TR_DIR, szGUID);

    if(stat(trPath, &statSt)) return bOk;

    /*-- 잘못된 Size를 가지고 있을 때 
     *-- 내부적으로 Internal error를 발생키셔야 한다 --*/
    if(statSt.st_size % sizeof(EndDeviceTransaction)) return bOk;

    memset(&endTrInfo, 0, sizeof(EndDeviceTransaction));

    if((fp=fopen(trPath,"r"))) 
    {
        while(fread(&endTrInfo, sizeof(EndDeviceTransaction), 1, fp)) 
        {
            if(endTrInfo.trID == nTrID) {
                memcpy(pEndTrInfo, &endTrInfo, sizeof(EndDeviceTransaction));
                bOk = TRUE;
                break;
            }
        }
        fclose(fp);
    }

    return bOk;
}

BOOL CTransactionManager::LoadTransactionHistory(WORD trID, TransactionHistory **ppTrHist, int *nCount)
{
    char                    trPath[64];
    TransactionHistory      *pTrHistory=NULL;
    BOOL                    bOk = FALSE;
    struct stat             statSt;
    FILE                    *fp;
    int                     cnt;

    *nCount = 0;
    sprintf(trPath, "%s/his/%u", TR_DIR, trID);

    if(stat(trPath, &statSt)) return bOk;

    /*-- 잘못된 Size를 가지고 있을 때 
     *-- 내부적으로 Internal error를 발생키셔야 한다 --*/
    if(statSt.st_size % sizeof(TransactionHistory)) return bOk;

    cnt=(int)(statSt.st_size/sizeof(TransactionHistory));

    pTrHistory = (TransactionHistory *)MALLOC(sizeof(TransactionHistory) * cnt);
    
    if((fp=fopen(trPath,"r"))) 
    {
        if(fread(pTrHistory, sizeof(TransactionHistory), cnt, fp) == (size_t)cnt) 
        {
            *nCount = cnt;
            bOk = TRUE;
        }
        fclose(fp);
    }
    *ppTrHist = pTrHistory;

    return bOk;
}

BOOL CTransactionManager::LoadTransactionTarget(WORD trID, EUI64 *pID)
{
    char                    trPath[64];
    BOOL                    bOk = FALSE;
    struct stat             statSt;
    FILE                    *fp;

    if(!pID) return bOk;

    sprintf(trPath, "%s/map/%u", TR_DIR, trID);
    if(stat(trPath, &statSt)) return bOk;

    /*-- 잘못된 Size를 가지고 있을 때 
     *-- 내부적으로 Internal error를 발생키셔야 한다 --*/
    if(statSt.st_size % sizeof(EUI64)) return bOk;

    if((fp=fopen(trPath,"r"))) 
    {
        if(fread(pID, sizeof(EUI64), 1, fp) == 1) 
        {
            bOk = TRUE;
        }
        fclose(fp);
    }

    return bOk;
}

BOOL CTransactionManager::LoadTransaction(WORD nTrID, BYTE nOption, TRINFOENTRY *pTrInfo, 
            MIBValue **ppParam, int *nParamCount, 
            MIBValue **ppResult, int *nResultCount,
            TRHISTORYENTRY **ppTrHist, int *nHistCount)
{
    char                    trPath[64];
    EndDeviceTransaction    endTrInfo;
    TransactionHistory      *pTrHist = NULL;
    EUI64                   id;
    MIBValue                *pMibValue=NULL, *pMib;
    BOOL                    bOk = FALSE;
    int                     i, nHistCnt=0, nMibCnt=0;

    if((nOption & TR_GET_OPT_REQUEST) && nParamCount) *nParamCount = 0;
    if((nOption & TR_GET_OPT_RESPONSE) && nResultCount) *nResultCount = 0;
    if((nOption & TR_GET_OPT_HISTORY) && nHistCount) *nHistCount = 0;

    if((nOption & TR_GET_OPT_TRINFO) && pTrInfo) pTrInfo->triID = nTrID;

    sprintf(trPath,"%s/req/%u", TR_DIR, nTrID);
    if(((nOption & TR_GET_OPT_TRINFO) || (nOption & TR_GET_OPT_REQUEST)) 
            && VARAPI_Deserialize(trPath, &pMibValue, &nMibCnt)) {

        if(nOption & TR_GET_OPT_TRINFO) {
            pMib = pMibValue;
            memcpy(&id, pMib->stream.p, sizeof(EUI64));                         pMib = pMib->pNext; // Target ID
            memcpy(&pTrInfo->triCommand, &pMib->stream.id, sizeof(OID3));       pMib = pMib->pNext; // Command
            pTrInfo->triOption = pMib->stream.u8;                               pMib = pMib->pNext; // Option
            pTrInfo->triDay = pMib->stream.u8;                                  pMib = pMib->pNext; // Day
            pTrInfo->triInitNice = pMib->stream.s8;                             pMib = pMib->pNext; // Nice
            pTrInfo->triInitTry = pMib->stream.u8;                              pMib = pMib->pNext; // Try

            memcpy(&pTrInfo->triTargetID, &id, sizeof(EUI64));

            if(LoadEndDeviceTransaction(&id, nTrID, &endTrInfo)) {
                pTrInfo->triCurNice = endTrInfo.trNice;
                pTrInfo->triCurTry = endTrInfo.trTry;
                pTrInfo->triQueue = endTrInfo.trQueue;
                GetTimestamp(&pTrInfo->triCreateTime, &endTrInfo.trCreateTime);
                GetTimestamp(&pTrInfo->triLastTime, &endTrInfo.trLastTime);
                pTrInfo->triState = endTrInfo.trState;
                pTrInfo->triError = endTrInfo.trError;

                bOk = TRUE;
            }
        }

        if(nOption & TR_GET_OPT_REQUEST) {
            if(ppParam)     *ppParam = pMibValue;
            else            IF4API_FreeMIBValue(pMibValue);
            if(nParamCount) *nParamCount = nMibCnt;
            if(nMibCnt >= 6) bOk = TRUE;
        }

    }

    sprintf(trPath,"%s/res/%u", TR_DIR, nTrID);
    pMibValue = NULL; nMibCnt = 0;
    if((nOption & TR_GET_OPT_RESPONSE) && VARAPI_Deserialize(trPath, &pMibValue, &nMibCnt)) {
        if(ppResult)    *ppResult = pMibValue;
        else            IF4API_FreeMIBValue(pMibValue);
        if(nResultCount) *nResultCount = nMibCnt;
        bOk = TRUE;
    }

    if((nOption & TR_GET_OPT_HISTORY) && LoadTransactionHistory(nTrID, &pTrHist, &nHistCnt)) {
        if(ppTrHist) {
            *ppTrHist = (TRHISTORYENTRY *)MALLOC(sizeof(TRHISTORYENTRY) * nHistCnt);
            memset(*ppTrHist, 0, sizeof(TRHISTORYENTRY) * nHistCnt);
            for(i=0; i< nHistCnt; i++) {
                GetTimestamp(&(*ppTrHist)[i].trhTime, &pTrHist[i].trTime);
                (*ppTrHist)[i].trhState = pTrHist[i].trState;
                (*ppTrHist)[i].trhError = pTrHist[i].trError;
            }
        }
        FREE(pTrHist);
        if(nHistCount) *nHistCount = nHistCnt;
        bOk = TRUE;
    }

    return bOk;
}


BOOL CTransactionManager::UpdateTransactionHistory(WORD trID, cetime_t nTime, BYTE nState, int nError)
{
    char                    trPath[64];
    TransactionHistory      trHis;
    FILE                    *fp;
    BOOL                    bOk = FALSE;

    memset(&trHis, 0, sizeof(TransactionHistory));

    trHis.trTime = nTime;
    trHis.trState = nState;
    trHis.trError = nError;

    sprintf(trPath, "%s/his/%u", TR_DIR, trID);
    if((fp=fopen(trPath,"a"))) {
        if(fwrite(&trHis, sizeof(TransactionHistory), 1, fp)==1) {
            bOk = TRUE;
        }
        fclose(fp);
    }

    return bOk;
}

BOOL CTransactionManager::DeleteTransaction(WORD trID, EUI64 *pId)
{
    EUI64                   id;
    char                    trPath[64];
    char                    szGUID[17];
    FILE                    *fp;
    BOOL                    bMap = FALSE;
    EndDeviceTransaction    *pEndTrInfo = NULL;
    int                     idx = -1, nCount = 0, i;

    if(pId) {
        memcpy(&id, pId, sizeof(EUI64));
    } else {
        sprintf(trPath, "%s/map/%u", TR_DIR, trID);
        if((fp=fopen(trPath, "r"))) {
            if(fread(&id, sizeof(EUI64), 1, fp) == 1) {
                bMap = TRUE;
            }
            fclose(fp);
        }
    }

    sprintf(trPath, "%s/req/%u", TR_DIR, trID); remove(trPath);
    sprintf(trPath, "%s/res/%u", TR_DIR, trID); remove(trPath);
    sprintf(trPath, "%s/map/%u", TR_DIR, trID); remove(trPath);
    sprintf(trPath, "%s/his/%u", TR_DIR, trID); remove(trPath);

    if(LoadEndDeviceTransaction(&id, &pEndTrInfo, &nCount)) {
        for(i=0;i<nCount;i++) {
            if(pEndTrInfo[i].trID == trID) {
                idx = i;
                break;
            }
        }
    }

    if(idx >= 0) {
        EUI64ToStr(&id, szGUID);
        sprintf(trPath, "%s/mbr/%s", TR_DIR, szGUID);

        if(nCount==1) {   // TransactionCount가 0이 될 때
            remove(trPath);
        } else {
            if((fp=fopen(trPath,"w"))) {
                for(i=0;i<nCount;i++) {
                    if(pEndTrInfo[i].trID != trID) {
                        fwrite(&pEndTrInfo[i], sizeof(EndDeviceTransaction), 1, fp);
                    }
                }
                fclose(fp);
            }
        }
    }

    FREE(pEndTrInfo);

    return TRUE;
}

BOOL CTransactionManager::Enqueue(EUI64 *pId, WORD trID, int idx, cetime_t nTime, EndDeviceTransaction *pEndTrInfo)
{
    BOOL                    bOk = FALSE;
    WORD                    *pTrID = NULL;

    if(UpdateTransactionHistory(trID, nTime, TR_STATE_QUEUE, IF4ERR_NOERROR)) {
        pEndTrInfo->trLastTime = nTime;
        pEndTrInfo->trState = TR_STATE_QUEUE;
        pEndTrInfo->trError = IF4ERR_NOERROR;
        pEndTrInfo->trQueue ++;
        if(UpdateEndDeviceTransaction(pId, idx, pEndTrInfo)) {
            pTrID = (WORD *)MALLOC(sizeof(WORD));
            memcpy(pTrID, &pEndTrInfo->trID, sizeof(WORD));
            m_pBatchJob->Add(BATCH_ASYNC_OPERATION, (void *)pTrID, TRUE, 3);
            bOk = TRUE;
        } else {
            UpdateTransactionHistory(trID, nTime, TR_STATE_WAIT, IF4ERR_TRANSACTION_UPDATE_FAIL);
        }
    }


    return TRUE;
}

/** Agent가 초기화될 때 State 상태 조정
  *
  * 1) Queue에 들어 있다고 되어 있는 Transaction들을 전부 Wait로 변경
  *
  */
BOOL CTransactionManager::ResolveTimeoutTransaction(BOOL bClearAll)
{
    BOOL                    bOk = TRUE;
    DIR			            *dir_fdesc;
    dirent		            *dp;
    cetime_t                nTime;
    EndDeviceTransaction    *pEndTrInfo;
    int                     i, nCnt;
    EUI64                   id;
    char                    trPath[64];

    time(&nTime);
    sprintf(trPath, "%s/mbr", TR_DIR);
    m_TransactionLocker.Lock();
    {
        dir_fdesc = opendir(trPath);
        if (!dir_fdesc) {
            m_TransactionLocker.Unlock();
            return FALSE;
        }
        while((dp=readdir(dir_fdesc))) {
            if(dp->d_name[0] == '.') continue;
            
            nCnt = 0; pEndTrInfo = NULL;
            StrToEUI64(dp->d_name, &id);
            if(LoadEndDeviceTransaction(&id, &pEndTrInfo, &nCnt)) {
                for(i=0;i<nCnt;i++) {
                    switch(pEndTrInfo[i].trState) {
                        case TR_STATE_RUN:
                            if(!bClearAll) break;
                        case TR_STATE_QUEUE:
                            UpdateEndDeviceTransaction(&id, pEndTrInfo[i].trID, nTime, FALSE, FALSE, TR_STATE_WAIT, IF4ERR_TIMEOUT);
                            UpdateTransactionHistory(pEndTrInfo[i].trID, nTime, TR_STATE_WAIT, IF4ERR_TIMEOUT);
                        break;
                    }
                }
            }
            FREE(pEndTrInfo);
        }
	    closedir(dir_fdesc);
    }
    m_TransactionLocker.Unlock();

    return bOk;
}

IF4_COMMAND_TABLE *CTransactionManager::FindCommand(OID3 *cmd)
{
    int     i;

    for(i=0; m_CommandHandler[i].pszName; i++)
    {
        if (memcmp(&m_CommandHandler[i].oid, cmd, 3) == 0)
            return &m_CommandHandler[i];
    }
    return NULL;
}

int CTransactionManager::CompTrID(const void *p1, const void *p2)
{
    WORD const *tr1 = (WORD const *)p1;
    WORD const *tr2 = (WORD const *)p2;

    return ((int)*tr1) - ((int)*tr2);
}

//////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////

BOOL CTransactionManager::Add(MIBValue *pValue, int nCount, WORD *nTransactionID)
{
    WORD        nTrID;
    cetime_t    nTime;
    BOOL        bOk = FALSE;

    m_TransactionLocker.Lock();
    {
        nTrID = GetNextTransactionID();
        time(&nTime);

        if(CreateTransactionRequest(nTrID, pValue) &&
           AppendEndDeviceTransaction(nTrID, nTime, pValue, nCount) &&
           UpdateTransactionHistory(nTrID, nTime, TR_STATE_WAIT, IF4ERR_NOERROR) &&
           UpdateTransactionID(nTrID)) {
            *nTransactionID = nTrID;
            bOk = TRUE;
        }else {
            DeleteTransaction(nTrID);
        }
    }
    m_TransactionLocker.Unlock();

	return bOk;
}

BOOL CTransactionManager::LoadTransactionInfo(EUI64 *pId, BYTE nStateFilter, TRINFOENTRY **ppTrInfo, int *nCount)
{
    char                    trPath[64];
    EndDeviceTransaction    *pEndTrInfo = NULL;
    TRINFOENTRY             *pTrInfo = NULL;
    MIBValue                *pMibValue, *pMib;
    BOOL                    bOk = FALSE;
    int                     nEndCnt = 0, i, nTrCnt, nMibCnt;

    *nCount = 0;
    m_TransactionLocker.Lock();
    {
        if(LoadEndDeviceTransaction(pId, &pEndTrInfo, &nEndCnt)) {
            for(i=0,nTrCnt=0;i<nEndCnt;i++) {
                if(nStateFilter & pEndTrInfo[i].trState) nTrCnt ++;
            }

            if(nTrCnt) {
                pTrInfo = (TRINFOENTRY *)MALLOC(sizeof(TRINFOENTRY) * nTrCnt);
                memset(pTrInfo, 0, sizeof(TRINFOENTRY) * nTrCnt);

                 for(i=0,nTrCnt=0;i<nEndCnt;i++) {
                    if(nStateFilter & pEndTrInfo[i].trState) {
                        memcpy(&pTrInfo[nTrCnt].triTargetID, pId, sizeof(EUI64));
                        pTrInfo[nTrCnt].triID = pEndTrInfo[i].trID;
                        pTrInfo[nTrCnt].triCurNice = pEndTrInfo[i].trNice;
                        pTrInfo[nTrCnt].triCurTry = pEndTrInfo[i].trTry;
                        pTrInfo[nTrCnt].triQueue = pEndTrInfo[i].trQueue;
                        GetTimestamp(&pTrInfo[nTrCnt].triCreateTime, &pEndTrInfo[i].trCreateTime);
                        GetTimestamp(&pTrInfo[nTrCnt].triLastTime, &pEndTrInfo[i].trLastTime);
                        pTrInfo[nTrCnt].triState = pEndTrInfo[i].trState;
                        pTrInfo[nTrCnt].triError = pEndTrInfo[i].trError;

                        pMibValue = NULL; nMibCnt = 0;
                        sprintf(trPath,"%s/req/%u", TR_DIR, pTrInfo[nTrCnt].triID);
                        if(VARAPI_Deserialize(trPath, &pMibValue, &nMibCnt)) {
                            pMib = pMibValue;                                                   pMib = pMib->pNext; // Target ID
                            memcpy(&pTrInfo[nTrCnt].triCommand, &pMib->stream.id, sizeof(OID3));pMib = pMib->pNext; // Command
                            pTrInfo[nTrCnt].triOption = pMib->stream.u8;                        pMib = pMib->pNext; // Option
                            pTrInfo[nTrCnt].triDay = pMib->stream.u8;                           pMib = pMib->pNext; // Day
                            pTrInfo[nTrCnt].triInitNice = pMib->stream.s8;                      pMib = pMib->pNext; // Nice
                            pTrInfo[nTrCnt].triInitTry = pMib->stream.u8;                       pMib = pMib->pNext; // Try
                        }
                        IF4API_FreeMIBValue(pMibValue);
                        nTrCnt ++;
                    }
                }
                *ppTrInfo = pTrInfo;
                *nCount = nTrCnt;
                /*-- 복사할것이 있을 때 TRUE --*/
                bOk = TRUE;
            }
            FREE(pEndTrInfo);
        }
    }
    m_TransactionLocker.Unlock();

    return bOk;
}

BOOL CTransactionManager::LoadTransactionInfo(WORD nTrID, BYTE nOption, TRINFOENTRY *pTrInfo, 
            MIBValue **ppParam, int *nParamCount, 
            MIBValue **ppResult, int *nResultCount,
            TRHISTORYENTRY **ppTrHist, int *nHistCount)
{
    BOOL                    bOk = FALSE;
    m_TransactionLocker.Lock();
    {
        bOk = LoadTransaction(nTrID, nOption, pTrInfo, ppParam, nParamCount, 
                    ppResult, nResultCount, ppTrHist, nHistCount);
    }
    m_TransactionLocker.Unlock();

    return bOk;
}

BOOL CTransactionManager::LoadTransactionInfo(WORD nTrID, MIBValue **ppParam, int *nParamCount)
{
    return LoadTransactionInfo(nTrID, TR_GET_OPT_REQUEST, NULL, ppParam, nParamCount, NULL, NULL, NULL, NULL);
}

/** 통신가능 정보를 통보 받는다.
  *
  * 각 MIU에서 통신이 가능한 상태가 되면(검침데이터 전송 또는 
  * Route Record 전달, STATE frame 전달) Notify를 호출한다.
  * 이 때 TransactionManager는 Queue에 WAITING 중인 Transaction을
  * 추가하고 상태를 TR_STATE_QUEUE로 변경한다. 또한 이 때 TrHistory
  * 에 Queue 되었다는 이력을 추가하고 EndTrInfo에서 State를
  * Queueing 상태로 변경하고 Queuing Count를 증가시킨다
  */
BOOL CTransactionManager::Notify(EUI64 * pId)
{
    BOOL                    bOk = FALSE;
    EndDeviceTransaction    *pTrInfo = NULL;
    int                     i, nCount = 0;
    cetime_t                now=0;

    time(&now);
    m_TransactionLocker.Lock();
    {
        if(LoadEndDeviceTransaction(pId, &pTrInfo, &nCount)) {
            for(i=0; i<nCount; i++) {
                if(pTrInfo[i].trState & TR_STATE_WAIT) {
                    bOk |= Enqueue(pId, pTrInfo[i].trID, i, now, &pTrInfo[i]);
                }
            }
        }
        if(pTrInfo) FREE(pTrInfo);
    }
    m_TransactionLocker.Unlock();

    return bOk;
}

/** Transaction의 상태를 변경한다.
  *
  * 변경 가능한 상태는 다음 중 하나이다.
  *        + TR_STATE_TERMINATE  0x08
  *        + TR_STATE_DELETE     0x10
  * 
  * 현재 상태에 따라서 다음과 같이 동작한다.
  *
  *  + TR_STATE_WAIT 
  *    - TR_STATE_TERMINATE : State 변경, nError = IF4ERR_USER_MODIFY
  *    - TR_STATE_DELETE : Transaction 삭제
  *  + TR_STATE_QUEUE 
  *    - TR_STATE_TERMINATE : Queue에서 제거, State 변경, nError = IF4ERR_USER_MODIFY
  *    - TR_STATE_DELETE : Queue에서 제거, Transaction 삭제
  *  + TR_STATE_RUN 
  *    - TR_STATE_TERMINATE : STATE가 변경되지 않고 IF4ERR_TRANSACTION_UPDATE_FAIL Return.
  *    - TR_STATE_DELETE : Transaction 삭제되지 않음 IF4ERR_TRANSACTION_UPDATE_FAIL Return
  *  + TR_STATE_TERMINATE 
  *    - TR_STATE_TERMINATE : 아무런 동작도 하지 않음
  *    - TR_STATE_DELETE : Transaction 삭제
  *
  */
int CTransactionManager::UpdateTransaction(WORD nTrID, BYTE nState)
{
    int                     nError = IF4ERR_NOERROR;
    EndDeviceTransaction    endTrInfo;
    EUI64                   id;
    cetime_t                nTime;

    time(&nTime);
    memset(&endTrInfo, 0, sizeof(EndDeviceTransaction));
    m_TransactionLocker.Lock();
    {
        if(LoadTransactionTarget(nTrID, &id) && LoadEndDeviceTransaction(&id, nTrID, &endTrInfo)) {
            switch(nState) {
                case TR_STATE_TERMINATE: 
                    switch(endTrInfo.trState) {
                        case TR_STATE_QUEUE:
                            m_pBatchJob->RemoveTransaction(nTrID);
                        case TR_STATE_WAIT:
                            UpdateEndDeviceTransaction(&id, nTrID, nTime, FALSE, FALSE, TR_STATE_TERMINATE, IF4ERR_USER_MODIFY);
                            UpdateTransactionHistory(nTrID, nTime, TR_STATE_TERMINATE, IF4ERR_USER_MODIFY);
                            break;
                        case TR_STATE_RUN:
                            nError = IF4ERR_TRANSACTION_UPDATE_FAIL;
                            break;
                        case TR_STATE_TERMINATE:
                            break;
                    }
                    break;
                case TR_STATE_DELETE: 
                    switch(endTrInfo.trState) {
                        case TR_STATE_QUEUE:
                            m_pBatchJob->RemoveTransaction(nTrID);
                        case TR_STATE_WAIT:
                        case TR_STATE_TERMINATE:
                            DeleteTransaction(nTrID);
                            break;
                        case TR_STATE_RUN:
                            nError = IF4ERR_TRANSACTION_UPDATE_FAIL;
                            break;
                    }
                    break;
                default: 
                    nError = IF4ERR_OUT_OF_RANGE;
                    break;
            }
        }else {
            if(nState == TR_STATE_DELETE) {
                DeleteTransaction(nTrID);
            }else {
                nError = IF4ERR_INVALID_TRANSACTION_ID;
            }
        }
    }
    m_TransactionLocker.Unlock();

    return nError;
}

/** Logging Option이 Command에 걸려 있을 때 Logging을 해야 한다
  *  Option에 따라 Save/Event 등을 처리해야한다.
  */
BOOL CTransactionManager::ExecuteTransaction(WORD nTrID)
{
    char                trPath[64];
    BOOL                bOk = FALSE;
    MIBValue            *pMibValue = NULL, *pMib, *pParam;
    int                 nParamCount = 0;
    EUI64               id, id2;
    OID3                cmd;
    BYTE                nOption, nDay, nTry, nState=TR_STATE_WAIT;
    signed char         nNice;
    int                 nError = IF4ERR_NOERROR, i;
    cetime_t            nTime;
    BOOL                bGetResTime = FALSE;
    IF4_COMMAND_TABLE * cmdTbl = NULL;
    EndDeviceTransaction    endTrInfo;
	char			szID[20]={0, }, szID2[20]={0, }, szOID[20]={0, };
	//WORD			Param1=0, Param2=0;

    if(m_pTransactionManager->LoadTransactionInfo(nTrID, &pMibValue, &nParamCount)) {
        //IF4API_DumpMIBValue(pMibValue, nParamCount);

        pMib = pMibValue;
        memcpy(&id, pMib->stream.p, sizeof(EUI64));                 pMib = pMib->pNext; // Target ID
        memcpy(&cmd, &pMib->stream.id, sizeof(OID3));               pMib = pMib->pNext; // Command
        nOption = pMib->stream.u8;                                  pMib = pMib->pNext; // Option
        nDay = pMib->stream.u8;                                     pMib = pMib->pNext; // Day
        nNice = pMib->stream.s8;                                    pMib = pMib->pNext; // Nice
        nTry = pMib->stream.u8;                                     pMib = pMib->pNext; // Try

		EUI64ToStr(&id, szID);
		EUI64ToStr(&id2, szID2);
		VARAPI_OidToString(&cmd, szOID);

		XDEBUG("Execute Transaction: [%u] %d.%d.%d Opt=0x%02X Day=%d Nice=%d Try=%d\r\n",
            nTrID, cmd.id1, cmd.id2, cmd.id3, nOption, nDay, nNice, nTry);

		XDEBUG("Command OID: %s\r\n", szOID);

        time(&nTime);
        memset(&endTrInfo, 0, sizeof(EndDeviceTransaction));
        m_TransactionLocker.Lock();
        {
            LoadEndDeviceTransaction(&id, nTrID, &endTrInfo);
            UpdateEndDeviceTransaction(&id, nTrID, nTime, TRUE, TRUE, TR_STATE_RUN, nError);
            UpdateTransactionHistory(nTrID, nTime, TR_STATE_RUN, nError);
        }
        m_TransactionLocker.Unlock();

		XDEBUG("Pass LoadEndDeviceTransaction, UpdateEndDevice, UpdateTransaction Function!!\r\n");

        if((cmdTbl = FindCommand(&cmd))) {
            if(cmdTbl->pfnCommand) {
                IF4Wrapper wrapper;

                pParam = (MIBValue *)MALLOC(sizeof(MIBValue) *nParamCount - PARAM_CNT);
                for(i=0; i < nParamCount-PARAM_CNT && pMib ;i++, pMib=pMib->pNext) {
                    memcpy(&pParam[i], pMib, sizeof(MIBValue));
                    if(i) pParam[i-1].pNext = &pParam[i];
                }
                nError = cmdTbl->pfnCommand(&wrapper, pParam, nParamCount - PARAM_CNT);
                FREE(pParam);

                time(&nTime); bGetResTime = TRUE;
                XDEBUG("Transaction Result : [%u] %s(%d.%d.%d) ReqParamCnd=%d ResultCnt=%d Res=%d(%s)\r\n",
                    nTrID, cmdTbl->pszName, cmd.id1, cmd.id2, cmd.id3, 
                    nParamCount - PARAM_CNT, wrapper.nResult, 
                    nError, IF4API_GetErrorMessage(nError));

                if(wrapper.nResult > 0) {
                    if(nOption & ASYNC_OPT_RESULT_DATA_SAVE) {
                        sprintf(trPath,"%s/res/%u", TR_DIR, nTrID);
                        if(VARAPI_Serialize(trPath, wrapper.pResult)) bOk = TRUE;
                    } else bOk = TRUE;
                } else if(nError == IF4ERR_NOERROR) {
                    bOk = TRUE;
                }
                if(nError == IF4ERR_NOERROR || nTry <= (endTrInfo.trTry + 1)) {
                    if(nOption & (ASYNC_OPT_RETURN_CODE_EVT | ASYNC_OPT_RESULT_DATA_EVT))
                    {
                        // Event 전송 Code 추가 삽입
                        if(nError == IF4ERR_NOERROR) {
                            if(nOption & ASYNC_OPT_RESULT_DATA_EVT) {
                                // DATA Event가 우선권을 가진다
                                transactionEvent(nTrID, EVT_TYPE_DATA, nOption, 
                                    endTrInfo.trCreateTime, nTime, nError, wrapper.nResult, wrapper.pResult);
                            }else {
                                // Code Event
                                transactionEvent(nTrID, EVT_TYPE_CODE, nOption,
                                    endTrInfo.trCreateTime, nTime, nError, 0, NULL);
                            }
                        } else {
                            /*-- 결국 실패해서 Try 제한에 걸린 경우 
                             *-- Error Code만 리턴한다
                             --*/
                            transactionEvent(nTrID, EVT_TYPE_CODE, nOption,
                                endTrInfo.trCreateTime, nTime, nError, 0, NULL);
                        }
                    }
                    nState = TR_STATE_TERMINATE;
                }
                IF4API_FreeMIBValue(wrapper.pResult);
            }
        }else {
            nError = IF4ERR_INVALID_COMMAND;
        }

        if(!bGetResTime) time(&nTime);
        m_TransactionLocker.Lock();
        {
            UpdateEndDeviceTransaction(&id, nTrID, nTime, FALSE, FALSE, nState, nError);
            UpdateTransactionHistory(nTrID, nTime, nState, nError);

            if(nState == TR_STATE_TERMINATE && !(nOption & (ASYNC_OPT_RETURN_CODE_SAVE|ASYNC_OPT_RESULT_DATA_SAVE))) {
                /*-- Save Option이 설정되지 않은 상태에서 Terminate 된다면 Transaction을 삭제한다 --*/
                DeleteTransaction(nTrID);
            }
        }
        m_TransactionLocker.Unlock();
        
    }
    IF4API_FreeMIBValue(pMibValue);

    return bOk;
}

/** Agent가 초기화될 때 무결성 검사를 하는 코드 추가 필요
  *
  * 1) req에 있는 모든 Transaction ID 목록을 수집
  * 2) map에 있는 모든 Transaction Id 목록을 수집
  * 3) mbr에 있는 EndDevice Info 수집
  *    a) trID가 정확한지 검사
  *    b) 보관 기간이 넘어가지 않았는지 검사
  *    c) Queue에 들어간 후 1시간이 넘은 Transaction은 Wait로 변경
  *
  */
BOOL CTransactionManager::IntegrityCheck(void)
{
    BOOL                    bOk = TRUE, bIntegrity, bSkipTest;
    DIR			            *dir_fdesc;
    dirent		            *dp;
	int			            i, nCnt, nTrCnt, nParamCnt;
	char		            trPath[64];
    WORD                    *pTrIdArr=NULL, *pChkTrIdArr=NULL, *pIdx;
    MIBValue                *pParam, *pMib;
    UINT                    sval;
    cetime_t                currentTime;
    UINT                    nDay;
    EndDeviceTransaction    *pEndTrInfo;
    EUI64                   id;

    time(&currentTime);
    sprintf(trPath, "%s/req", TR_DIR);
    XDEBUG(" Integrity Check Start\r\n");
    m_TransactionLocker.Lock();
    {
        XDEBUG(" Read Transaction ID List\r\n");
        dir_fdesc = opendir(trPath);
        if (!dir_fdesc) {
            m_TransactionLocker.Unlock();
            return FALSE;
        }
        for(nTrCnt=0;(dp=readdir(dir_fdesc));) {
            if(dp->d_name[0] != '.') nTrCnt++;
        }
	    closedir(dir_fdesc);

        pTrIdArr = (WORD *)MALLOC(sizeof(WORD) * nTrCnt);
        pChkTrIdArr = (WORD *)MALLOC(sizeof(WORD) * nTrCnt);

        dir_fdesc = opendir(trPath);
        if (!dir_fdesc) {
            FREE(pTrIdArr);
            m_TransactionLocker.Unlock();
            return FALSE;
        }
        for(i=0;(dp=readdir(dir_fdesc)) && i < nTrCnt; ) {
            if(dp->d_name[0] == '.') continue;
            sscanf(dp->d_name,"%u", &sval); pTrIdArr[i] = (WORD) sval;
            i++;
        }
	    closedir(dir_fdesc);

        XDEBUG(" Sort Transaction ID List\r\n");
        qsort(pTrIdArr, nTrCnt, sizeof(WORD), CompTrID);
        memcpy(pChkTrIdArr, pTrIdArr, sizeof(WORD) * nTrCnt);

        /*-- End Device 정보 검사 --*/
        XDEBUG(" EndDevice Information Check\r\n");
        sprintf(trPath, "%s/mbr", TR_DIR);
        dir_fdesc = opendir(trPath);
        if (!dir_fdesc) {bSkipTest = TRUE; bOk = FALSE;}
        else            bSkipTest = FALSE;

        while(!bSkipTest && (dp=readdir(dir_fdesc)) != NULL) {
            if(dp->d_name[0] == '.') continue;
            
            nCnt = 0; bIntegrity = TRUE; pEndTrInfo = NULL;
            StrToEUI64(dp->d_name, &id);
            if(LoadEndDeviceTransaction(&id, &pEndTrInfo, &nCnt)) {
                /* EndDevice에 있는 TR ID가 유효한지 검사
                 * 잘못된 ID라면 해당 Transaction 삭제 
                 */
                for(i=0;i<nCnt;i++) {
                    bIntegrity = TRUE; pParam = NULL; nParamCnt = 0;

                    if((pIdx = (WORD *)bsearch(&pEndTrInfo[i].trID, pTrIdArr, nTrCnt, sizeof(WORD), CompTrID))) {
                        pChkTrIdArr[(pIdx - pTrIdArr)] = 0;
                        if(LoadTransaction(pEndTrInfo[i].trID, TR_GET_OPT_REQUEST, NULL, 
                                    &pParam, &nParamCnt, NULL, NULL, NULL, NULL)) {
                            pMib = pParam;
                            /*-- Target ID  --*/    pMib = pMib->pNext; 
                            /*-- Command    --*/    pMib = pMib->pNext; 
                            /*-- Option     --*/    pMib = pMib->pNext; 
                            nDay = (UINT)(pMib->stream.u8 ? pMib->stream.u8 : m_nTransactionSaveDay);
                            /*-- 저장 유효기간이 지났는지 검사 --*/
                            if((currentTime - pEndTrInfo[i].trCreateTime) > (int)(DAY_SEC *  nDay)) { // 유효기간 초과
                                bIntegrity = FALSE;
                                XDEBUG("  Transaction Delete [Out of date] : %u\r\n", pEndTrInfo[i].trID);
                            }
                        } else { // Transaction 정보를 읽지 못했을 때
                            bIntegrity = FALSE;
                            XDEBUG("  Transaction Delete [Read Transaction Info Fail] : %u\r\n", pEndTrInfo[i].trID);
                        }
                        FREE(pParam);
                    } else { // 잘못된 ID (ID 목록에 없음)
                        bIntegrity = FALSE;
                        XDEBUG("  Transaction Delete [Read End Device Info Fail] : %u\r\n", pEndTrInfo[i].trID);
                    }
                    if(!bIntegrity) DeleteTransaction(pEndTrInfo[i].trID, &id);
                }
            } else {    // EndDevice 정보 읽기 실패 : EndDevice Info File 삭제
                remove(dp->d_name);
            }
            FREE(pEndTrInfo);
        }
	    if(!bSkipTest) closedir(dir_fdesc);

        /*-- EndDevice Info는 없고 Transaction 정보만 있는 경우 삭제 --*/
        for(i=0; i<nTrCnt; i++) {
            if(pChkTrIdArr[i]) {
                XDEBUG("  Transaction Delete [Invalid Transaction Info] : %u\r\n", pChkTrIdArr[i]);
                DeleteTransaction(pChkTrIdArr[i]);
            }
        }

        /*-- Mapping  검사 : Transaction Info는 없고 Mapping만 있어도 삭제 --*/
        XDEBUG(" Transaction Mapping Check\r\n");
        sprintf(trPath, "%s/map", TR_DIR);
        dir_fdesc = opendir(trPath);
        if (!dir_fdesc) {bSkipTest = TRUE; bOk = FALSE;}
        else            bSkipTest = FALSE;

        while(!bSkipTest && (dp=readdir(dir_fdesc)) != NULL) {
            if(dp->d_name[0] == '.') continue;
            sscanf(dp->d_name,"%u", &sval); 
            if(!bsearch(&sval, pTrIdArr, nTrCnt, sizeof(WORD), CompTrID)) {
                DeleteTransaction(sval);
            }
        }
	    if(!bSkipTest) closedir(dir_fdesc);

        /*-- Result 검사 : Trasaction Info는 없고 Result만 있어도 삭제 --*/
        XDEBUG(" Transaction Result Check\r\n");
        sprintf(trPath, "%s/res", TR_DIR);
        dir_fdesc = opendir(trPath);
        if (!dir_fdesc) {bSkipTest = TRUE; bOk = FALSE;}
        else            bSkipTest = FALSE;

        while(!bSkipTest && (dp=readdir(dir_fdesc)) != NULL) {
            if(dp->d_name[0] == '.') continue;
            sscanf(dp->d_name,"%u", &sval); 
            if(!bsearch(&sval, pTrIdArr, nTrCnt, sizeof(WORD), CompTrID)) {
                DeleteTransaction(sval);
            }
        }
	    if(!bSkipTest) closedir(dir_fdesc);
            
        FREE(pTrIdArr);
        FREE(pChkTrIdArr);
    }
    m_TransactionLocker.Unlock();
    XDEBUG(" Integrity Check Terminate\r\n");

    return bOk;
}
