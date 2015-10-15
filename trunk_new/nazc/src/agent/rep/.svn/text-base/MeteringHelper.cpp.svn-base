
#include "types/identification.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "if4api.h"

#include "MeteringHelper.h"
#include "MeterHelper.h"
#include "ChannelCfgHelper.h"
#include "DbUtil.h"

//////////////////////////////////////////////////////////////////////
// CMeteringHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS          "Metering"
/** Version 2 : Issue #275 LoadProfileTbl의 LP Value를 Channel 별로 나눠서 저정하지 않고 하나의 Blob으로 저장한다.  
 */
#define DB_VERSION        2

#define SELECT_BP_STMT      "SELECT "   \
                                "mr._id, me._id, mo._id, mo.address, me._portNum, "  \
                                "mr._meteringTime, mr.meterTime, mr.meterTimeOffset, mr.baseTime, " \
                                "mr.baseValue, mr.lastValue, " \
                                "mr.lastUpdate "    \
                            "FROM ModemTbl mo, MeterTbl me, MeteringTbl mr "    \
                            "WHERE "    \
                                "mo._id = me._modemId " \
                                "AND me._id = mr._meterId " 

#define SELECT_LP_STMT      "SELECT "   \
                                "lp._meteringId, me._id, lp._id, lp._snapShotTime, " \
                                "lp.value, lp.tryUpload, lp.uploadTime, " \
                                "lp.lastUpdate "    \
                            "FROM ModemTbl mo, MeterTbl me, LoadProfileTbl lp "    \
                            "WHERE "    \
                                "mo._id = me._modemId " \
                                "AND me._id = lp._meterId " 

#define SELECT_UPLOAD       "SELECT lp.mid, lp.addr, lp.pnum, " \
                                "dl.baseTime, dl.meterTime, dl.baseValue, dl.lastValue, " \
                                "lp.lpid, lp.stamp, lp.value " \
                            "FROM WaitUploadLpView lp, DayLastPulseTbl dl " \
                            "WHERE " \
                                "lp.mid = dl._meterId AND " \
                                "lp.d = dl.baseTime " \
                            "ORDER BY lp.mid, lp.stamp " 


CMeteringHelper   	*m_pMeteringHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CMeteringHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CMeteringHelper::CMeteringHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pMeteringHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CMeteringHelper::CMeteringHelper() :
    CDatabaseHelper(DB_FILE_AGENT, DB_CLASS, DB_VERSION)
{
	m_pMeteringHelper	= this;
}

CMeteringHelper::~CMeteringHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CMeteringHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CMeteringHelper::Initialize()
{
    return CDatabaseHelper::Initialize();
}

/** Metering 정보 갱신.
 *
 * 이 함수는 실제로는 Update 되지 않고 항상 Insert만 된다.
 *
 * @param nMeterId          Meter Id (Foreign Key)
 * @param meterTime         Meter Current Time
 * @param meterTimeOffset   초단위의 시간 오차
 *              INT_MAX-1(0x7FFFFFFE) ~  INT_MIN+1(0x10000001) 까지의 범위를 가진다.
 *              INT_MAX의 경우 기준시간 대비 오차범위 보다 빨리 가고 있는것이고
 *              INT_MIN의 경우 기준시간 대비 오참범위 보다 느리게 가고 있는것이다.
 * @param baseTime          Base pulse의 기준 시간
 * @param nChannelCnt       Channel Count
 * @param pBasePulse        Base, Last Pulse 정보
 * @param nLpCnt            LP Count (전체 LP의 수, 개별 Channel 별로 Count 되어야 한다)
 *              Channel 이 2 개이고 한번 수집한 LP 라면 nLpCnt는 2 여야 한다.
 * @param pLoadProfile      LP Instance
 *
 */
BOOL CMeteringHelper::Update(const UINT nMeterId, TIMESTAMP *meterTime, int meterTimeOffset, TIMESTAMP *baseTime,
        int nChannelCnt, int nLpCnt, METERINGINS *pMeteringIns)
{
    int idx=1;
    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;

    try
    {
        CppSQLite3Query result;
        CppSQLite3Statement stmt;
        char timeStr[32], timeStr2[32];
        cetime_t now;
        TIMESTAMP nowStamp;
        UINT nMeteringId=0;
        UINT nDayLastId=0;
        int nLongSize = sizeof(unsigned long long);
        int nIntSize = sizeof(unsigned int);
        int i;

        ceTime(&now);
        memset(&nowStamp, 0, sizeof(TIMESTAMP));
        GetTimestamp(&nowStamp, &now);

        ConvTimeString(meterTime, timeStr);
        ConvDateString(baseTime, timeStr2);
        XDEBUG(ANSI_COLOR_CYAN " MeteringTbl Update: meterId %d meterTime %s offset %d baseTime %s chCnt %d lpCnt %d\r\n" ANSI_NORMAL,
                nMeterId, timeStr, meterTimeOffset, timeStr2, nChannelCnt, nLpCnt);

        // Transaction
        TRANSACTION_BEGIN;

        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM MeterTbl WHERE _id = ? ;");
        idx = 1;
        stmt.bind(idx, (int)nMeterId);                      idx++;
        result = stmt.execQuery();
        if(result.eof()) {
		    XDEBUG(ANSI_COLOR_RED "DB ERROR: Invalid Meter ID (%d)\r\n" ANSI_NORMAL, nMeterId);
            // Transaction
            TRANSACTION_ROLLBACK; Close();
            return FALSE;
        }

        stmt.finalize();

        // Metering Base/Last Pulse Insert
        stmt = m_SqliteHelper.compileStatement(
            "INSERT INTO MeteringTbl "
                "( _meterId, _meteringTime, meterTime, lastValue, meterTimeOffset,  baseTime, baseValue, lastUpdate ) "
            "VALUES "
                "( ?,        ?,             ?,         ?,         ?,                ?,        ?,         DATETIME('now','localtime')); ");

        idx = 1;
        stmt.bind(idx, (int)nMeterId);                      idx++;
        memset(timeStr, 0, sizeof(timeStr)); ConvTimeString(&nowStamp, timeStr);
        stmt.bind(idx, timeStr);                            idx++;
        memset(timeStr, 0, sizeof(timeStr)); ConvTimeString(meterTime, timeStr);
        stmt.bind(idx, timeStr);                            idx++;
        if(nChannelCnt > 0)
        {
            stmt.bind(idx, (const unsigned char *)pMeteringIns->pLastValue, nChannelCnt * nLongSize);
        }
        else
        {
            stmt.bindNull(idx);
        }                                                   idx++;
        stmt.bind(idx, meterTimeOffset);                    idx++;
        memset(timeStr, 0, sizeof(timeStr)); ConvTimeString(baseTime, timeStr);
        stmt.bind(idx, timeStr);                            idx++;
        if(nChannelCnt > 0)
        {
            stmt.bind(idx, (const unsigned char *)pMeteringIns->pBaseValue, nChannelCnt * nLongSize);
        }
        else
        {
            stmt.bindNull(idx);
        }                                                   idx++;

        stmt.execDML();
        stmt.finalize();

        // Metering Id 얻기
        stmt = m_SqliteHelper.compileStatement("SELECT _id FROM MeteringTbl WHERE _meterId = ? AND _meteringTime = ? ;");
        idx = 1;
        stmt.bind(idx, (int)nMeterId);                      idx++;
        memset(timeStr, 0, sizeof(timeStr)); ConvTimeString(&nowStamp, timeStr);
        stmt.bind(idx, timeStr);                            idx++;
        result = stmt.execQuery();

        if(result.eof()) {
		    XDEBUG(ANSI_COLOR_RED "DB ERROR: Metering Id Select Fail\r\n" ANSI_NORMAL);
            stmt.finalize();
            // Transaction
            TRANSACTION_ROLLBACK; Close();
            return FALSE;
        }
        nMeteringId = result.getIntField(0); 
        stmt.finalize();

        // Last Pulse Table Id 얻기
        stmt = m_SqliteHelper.compileStatement("SELECT _id FROM DayLastPulseTbl WHERE _meterId = ? AND baseTime = ? ;");
        idx = 1;
        stmt.bind(idx, (int)nMeterId);                      idx++;
        memset(timeStr, 0, sizeof(timeStr)); ConvDateString(baseTime, timeStr);
        stmt.bind(idx, timeStr);                            idx++;
        result = stmt.execQuery();

        if(result.eof())  // Insert
        {
            stmt.finalize();
            stmt = m_SqliteHelper.compileStatement(
                "INSERT INTO DayLastPulseTbl "
                    "(  _meterId, meterTime, lastValue, baseTime, baseValue, lastUpdate ) "
                "VALUES "
                    "(  ?,        ?,         ?,         ?,        ?,         DATETIME('now','localtime') )");

            idx = 1;
            stmt.bind(idx, (int)nMeterId);                      idx++;
            memset(timeStr, 0, sizeof(timeStr)); ConvTimeString(meterTime, timeStr);
            stmt.bind(idx, timeStr);                            idx++;
            if(nChannelCnt > 0)
            {
                stmt.bind(idx, (const unsigned char *)pMeteringIns->pLastValue, nChannelCnt * nLongSize);
            }
            else
            {
                stmt.bindNull(idx);
            }                                                   idx++;
            memset(timeStr, 0, sizeof(timeStr)); ConvDateString(baseTime, timeStr);
            stmt.bind(idx, timeStr);                            idx++;
            if(nChannelCnt > 0)
            {
                stmt.bind(idx, (const unsigned char *)pMeteringIns->pBaseValue, nChannelCnt * nLongSize);
            }
            else
            {
                stmt.bindNull(idx);
            }                                                   idx++;
    
            stmt.execDML();
            stmt.finalize();
        }
        else if(nChannelCnt > 0) // Update
        {
            nDayLastId = result.getIntField(0); 
            stmt.finalize();

            stmt = m_SqliteHelper.compileStatement(
                "UPDATE DayLastPulseTbl "
                    "SET meterTime = ?, "
                    "    lastValue = ?, "
                    "    lastUpdate = DATETIME('now','localtime') "
                "WHERE _id = ? ");

            idx = 1;
            memset(timeStr, 0, sizeof(timeStr)); ConvTimeString(meterTime, timeStr);
            stmt.bind(idx, timeStr);                            idx++;
            stmt.bind(idx, (const unsigned char *)pMeteringIns->pLastValue, nChannelCnt * nLongSize);           
                                                                idx++;
            stmt.bind(idx, (int)nDayLastId);                    idx++;
    
            stmt.execDML();
            stmt.finalize();
        }
        else // Update 할 Value가 없을 때
        {
            stmt.finalize();
        }

        // LP Insert
        stmt = m_SqliteHelper.compileStatement(
            "INSERT OR IGNORE INTO LoadProfileTbl "
                "(  _meteringId, _meterId, _snapShotTime, value, lastUpdate ) "
            "VALUES "
                "(  ?,           ?,        ?,             ?,     DATETIME('now','localtime') )");
        for(i=0; i < nLpCnt && pMeteringIns->pLoadProfile[i] != NULL; i++)
        {
            idx = 1;
            stmt.bind(idx, (int)nMeteringId);               idx++;
            stmt.bind(idx, (int)nMeterId);                  idx++;
            memset(timeStr, 0, sizeof(timeStr)); ConvTimeString(&pMeteringIns->pLoadProfile[i]->_snapShotTime, timeStr);
            stmt.bind(idx, timeStr);                        idx++;
            stmt.bind(idx, (const unsigned char *)pMeteringIns->pLoadProfile[i]->pValue, nChannelCnt * nIntSize);
                                                            idx++;
            stmt.execDML();
            stmt.reset();
            XDEBUG(ANSI_COLOR_CYAN "   %s\r\n" ANSI_NORMAL, timeStr);
        }
        stmt.finalize();

        // Transaction
        TRANSACTION_COMMIT; Close();
        return TRUE;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "MeteringTbl DB ERROR UPDATE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        // Transaction
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
	}

    return FALSE;
}

BOOL CMeteringHelper::Update(const char *szModemId, int nPortNum, 
        TIMESTAMP *meterTime, int meterTimeOffset, TIMESTAMP *baseTime,
        int nChannelCnt, int nLpCnt, METERINGINS *pMeteringIns)
{
    UINT nMeterId = 0;
    if(szModemId == NULL) return FALSE;
    if(m_pMeterHelper == NULL) return FALSE;
    if(!m_pMeterHelper->GetId(szModemId, nPortNum, &nMeterId)) return FALSE;

    return Update(nMeterId, meterTime, meterTimeOffset, baseTime, nChannelCnt, nLpCnt, pMeteringIns);
}

BOOL CMeteringHelper::Update(const EUI64 *pModemId, int nPortNum, 
        TIMESTAMP *meterTime, int meterTimeOffset, TIMESTAMP *baseTime,
        int nChannelCnt, int nLpCnt, METERINGINS *pMeteringIns)
{
    UINT nMeterId = 0;
    if(pModemId == NULL) return FALSE;
    if(m_pMeterHelper == NULL) return FALSE;
    if(!m_pMeterHelper->GetId(pModemId, nPortNum, &nMeterId)) return FALSE;

    return Update(nMeterId, meterTime, meterTimeOffset, baseTime, nChannelCnt, nLpCnt, pMeteringIns);
}

METERINGDATAINS* CMeteringHelper::GetData(Row *pRow)
{
    if(pRow == NULL || pRow->pData == NULL) return NULL;
    return (METERINGDATAINS *)pRow->pData;
}

BOOL CMeteringHelper::Select(DBFetchData *pResult)
{
    return Select((char *)NULL, -1, NULL, NULL, pResult);
}

BOOL CMeteringHelper::Select(TIMESTAMP *pFrom, TIMESTAMP *pTo, DBFetchData *pResult)
{
    return Select((char *)NULL, -1, pFrom, pTo, pResult);
}

BOOL CMeteringHelper::Select(const EUI64 *pId, int nPortNum, DBFetchData *pResult)
{
    char szId[32]={0,};

    if(pId == NULL) {
        return Select((char *)NULL, -1, NULL, NULL, pResult);
    }

    EUI64ToStr(pId, szId);
    return Select(szId, nPortNum, NULL, NULL,  pResult);
}

BOOL CMeteringHelper::Select(const char *szModemId, int nPortNum, DBFetchData *pResult)
{
    if(szModemId == NULL) {
        return Select((char *)NULL, -1, NULL, NULL, pResult);
    }

    return Select(szModemId, nPortNum, NULL, NULL, pResult);
}

/** Metering select.
 *
 * @param szModemId
 * @param nPortNum
 * @param pFrom
 * @param pTo
 * @param pResult
 *
 */
BOOL CMeteringHelper::Select(const char *szModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo,
        DBFetchData *pResult)
{
    CppSQLite3Query result;
    METERINGDATAINS *pInstance = NULL;
    METERINGINS *pMetering = NULL;
    LOADPROFILEINS *pLp = NULL;
    Row *pRow;
    char * szRet;
    int i, idx=0, nChCount=-1;
    int nBlobLen;
    UINT nMrId=0, nMeId=0;

    if(pResult == NULL) 
    {
        XDEBUG("MeteringHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        char queryBuffer[1024];
        //const char *orderStr = (order == ORDER_DESC ? "DESC" : "ASC");
        char fromTimeStr[32], toTimeStr[32];
        char fromDateStr[16], toDateStr[16];

        if(pFrom) 
        {
            ConvDateString(pFrom, fromDateStr);
            ConvTimeString(pFrom, fromTimeStr);
        }
        if(pTo) 
        {
            ConvDateString(pTo, toDateStr);
            ConvTimeString(pTo, toTimeStr);
        }

        sprintf(queryBuffer, "%s ", SELECT_BP_STMT);
        /** 
         *  BasePulseTbl Select 
         */
        if(szModemId == NULL) 
        {
            // 전체 Select
        } 
        else 
        {
            strcat(queryBuffer, "AND mo.address = ? ");
            if(nPortNum >= 0) {
                strcat(queryBuffer,"AND me._portNum = ? ");
            }
        }
        if(pFrom && pTo)
        {
            strcat(queryBuffer, "AND ( mr.baseTime BETWEEN ? AND ? ) ");
        }
        else
        {
            if(pFrom) strcat(queryBuffer, "AND mr.baseTime >= ? ");
            if(pTo) strcat(queryBuffer, "AND mr.baseTime <= ? ");
        }
        strcat(queryBuffer, "ORDER BY me._id, mr.baseTime ASC ");

        //XDEBUG("%s\n", queryBuffer);
        stmt = m_SqliteHelper.compileStatement(queryBuffer);

        idx = 1;
        if(szModemId==NULL) 
        {
        }
        else
        {
            stmt.bind(idx, szModemId);              idx++;
            if(nPortNum >= 0)
            {
                stmt.bind(idx, nPortNum);           idx++;
            }
        }
        if(pFrom && pTo)
        {
            stmt.bind(idx, fromDateStr);            idx++;
            stmt.bind(idx, toTimeStr);              idx++;
        }
        else
        {
            if(pFrom) 
            {
                stmt.bind(idx, fromDateStr);        idx++;
            }
            if(pTo) 
            {
                stmt.bind(idx, toTimeStr);          idx++;
            }
        }

        result = stmt.execQuery();
        while(!result.eof())
        {
            idx = 0;
            nMrId = result.getIntField(idx);                                            idx++;
            nMeId = result.getIntField(idx);                                            idx++;
            if(pInstance==NULL || pInstance->_meterId != nMeId)
            {
                if(pInstance != NULL)
                {
                    AddResult(pResult, pInstance);
                }
                pInstance = (METERINGDATAINS *)MALLOC(sizeof(METERINGDATAINS));
                memset(pInstance, 0, sizeof(METERINGDATAINS));
                pMetering = NULL;
                nChCount=-1;

                pInstance->_meterId = nMeId;
                pInstance->_modemId = result.getIntField(idx);                      idx++; //*
                if((szRet = (char *)result.getStringField(idx)) != NULL) 
                {
                    strncpy(pInstance->szModemId, szRet, MIN(16, strlen(szRet)));
                    StrToEUI64(pInstance->szModemId, &pInstance->modemId);
                }                                                                   idx++; //*

                pInstance->nPortNum = result.getIntField(idx);                      idx++; //*
            }
            else 
            {
                idx += 3;   // 새로 생성해서 증가한 만큼 
            }

            if(pInstance->nMeteringCnt >= METERING_MAX) 
            {
                result.nextRow();
                continue;
            }

            pMetering = (METERINGINS *)MALLOC(sizeof(METERINGINS));
            memset(pMetering, 0, sizeof(METERINGINS));

            pMetering->_id = nMrId;
            ConvTimeStamp(result.getStringField(idx), &pMetering->meteringTime);        idx++; 
            ConvTimeStamp(result.getStringField(idx), &pMetering->meterTime);           idx++; 
            pMetering->nMeterTimeOffset = result.getIntField(idx);                      idx++; 
            ConvTimeStamp(result.getStringField(idx), &pMetering->baseTime);            idx++; 
            nBlobLen = 0; szRet = (char *)result.getBlobField(idx, nBlobLen);           idx++;
            if(nBlobLen > 0)
            {
                pMetering->pBaseValue = (unsigned long long *)MALLOC(nBlobLen);
                memcpy((char *)pMetering->pBaseValue, szRet, nBlobLen);
                nChCount = nBlobLen / sizeof(unsigned long long);
                if(pInstance->nChannelCnt < nChCount) { pInstance->nChannelCnt = nChCount; }
            }
            nBlobLen = 0; szRet = (char *)result.getBlobField(idx, nBlobLen);           idx++;
            if(nBlobLen > 0)
            {
                pMetering->pLastValue = (unsigned long long *)MALLOC(nBlobLen);
                memcpy((char *)pMetering->pLastValue, szRet, nBlobLen);
                nChCount = nBlobLen / sizeof(unsigned long long);
                if(pInstance->nChannelCnt < nChCount) { pInstance->nChannelCnt = nChCount; }
            }
            ConvTimeStamp(result.getStringField(idx), &pMetering->lastUpdate);          idx++;

            pInstance->pMetering[pInstance->nMeteringCnt] = pMetering;
            pInstance->nMeteringCnt ++;

            result.nextRow();
        }

        if(pInstance != NULL)
        {
            AddResult(pResult, pInstance);
        }
        stmt.finalize();

        sprintf(queryBuffer, "%s ", SELECT_LP_STMT);
        /** 
         *  LoadProfileTbl Select 
         */
        if(szModemId == NULL) 
        {
            // 전체 Select
        } 
        else 
        {
            strcat(queryBuffer, "AND mo.address = ? ");
            if(nPortNum >= 0) {
                strcat(queryBuffer,"AND me._portNum = ? ");
            }
        }
        if(pFrom && pTo)
        {
            strcat(queryBuffer, "AND ( lp._snapShotTime BETWEEN ? AND ? ) ");
        }
        else
        {
            if(pFrom) strcat(queryBuffer, "AND lp._snapShotTime >= ? ");
            if(pTo) strcat(queryBuffer, "AND lp._snapShotTime <= ? ");
        }
        strcat(queryBuffer, "ORDER BY me._id, lp._snapShotTime ASC ");

        //XDEBUG("%s\n", queryBuffer);
        stmt = m_SqliteHelper.compileStatement(queryBuffer);

        idx = 1;
        if(szModemId==NULL) 
        {
        }
        else
        {
            stmt.bind(idx, szModemId);              idx++;
            if(nPortNum >= 0)
            {
                stmt.bind(idx, nPortNum);           idx++;
            }
        }
        if(pFrom && pTo)
        {
            stmt.bind(idx, fromTimeStr);            idx++;
            stmt.bind(idx, toTimeStr);              idx++;
        }
        else
        {
            if(pFrom) 
            {
                stmt.bind(idx, fromTimeStr);        idx++;
            }
            if(pTo) 
            {
                stmt.bind(idx, toTimeStr);          idx++;
            }
        }

        result = stmt.execQuery();

        pRow = GetHeadRow(pResult);
        pInstance = GetData(pRow);
        //XDEBUG(" pInstance->nMeteringCnt %d\r\n", pInstance->nMeteringCnt);
        while(pInstance && !result.eof())
        {
            idx = 0;
            nMrId = result.getIntField(idx);                                            idx++;
            nMeId = result.getIntField(idx);                                            idx++;
            if(pInstance->_meterId == nMeId)
            {
                for(i=0;i<pInstance->nMeteringCnt;i++)
                {
                    //if(pInstance->pMetering[i]->_id != nMrId) continue;
                    if(pInstance->pMetering[i]->nLpCnt >= LP_MAX) break;

                    pLp = (LOADPROFILEINS *)MALLOC(sizeof(LOADPROFILEINS));
                    memset(pLp,0,sizeof(LOADPROFILEINS));

                    pLp->_meteringId = nMrId;     
                    pLp->_id = result.getIntField(idx);                                     idx++;
                    ConvTimeStamp(result.getStringField(idx), &pLp->_snapShotTime);         idx++;
                    nBlobLen = 0; szRet = (char *)result.getBlobField(idx, nBlobLen);       idx++;
                    if(nBlobLen > 0)
                    {
                        pLp->pValue = (unsigned int *)MALLOC(nBlobLen);
                        memcpy((char *)pLp->pValue, szRet, nBlobLen);
                    }
                    pLp->nTryUpload = result.getIntField(idx);                              idx++;
                    if(!result.fieldIsNull(idx)) {
                        ConvTimeStamp(result.getStringField(idx), &pLp->uploadTime);         
                    }                                                                       idx++;
                    ConvTimeStamp(result.getStringField(idx), &pLp->lastUpdate);            idx++;

                    pInstance->pMetering[i]->pLoadProfile[pMetering->nLpCnt] = pLp;
                    pInstance->pMetering[i]->nLpCnt ++;
                    pInstance->nTotLpCnt++;
                    //break;
                }
            }
            else 
            {
                pRow = GetNextRow(pRow);
                pInstance = GetData(pRow);
                continue;
            }

            result.nextRow();
        }
        stmt.finalize();

        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "MeteringTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CMeteringHelper::TryUpload(DBFetchData *pResult)
{
    CppSQLite3Query result;
    METERINGDATAINS *pInstance = NULL;
    LOADPROFILEINS *pLp = NULL;
    METERINGINS *pMetering = NULL;

    TIMESTAMP   prevTime, baseTime;
    char * szRet;
    int idx=0/*, nMaxChIdx=-1*/;
    int nBlobLen, nChCount;
    UINT nMeId=0;

    if(pResult == NULL) 
    {
        XDEBUG("MeteringHelper::TryUpload : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        CppSQLite3Statement stmt2;

        // Transaction
        TRANSACTION_BEGIN;

        stmt2 = m_SqliteHelper.compileStatement(
                "UPDATE LoadProfileTbl "
                "SET "
                  "tryUpload = tryUpload + 1, "
                  "lastUpdate = DATETIME('now','localtime') "
                "WHERE _id = ? "
            );

        stmt = m_SqliteHelper.compileStatement(SELECT_UPLOAD);
        result = stmt.execQuery();

        while(!result.eof())
        {
            idx = 0;
            nMeId = result.getIntField(idx);                                            idx++;

            if(pInstance==NULL || pInstance->_meterId != nMeId)
            {
                if(pInstance != NULL)
                {
                    AddResult(pResult, pInstance);
                }
                pInstance = (METERINGDATAINS *)MALLOC(sizeof(METERINGDATAINS));
                memset(pInstance, 0, sizeof(METERINGDATAINS));
                memset(&prevTime, 0, sizeof(TIMESTAMP));

                pInstance->_meterId = nMeId;
                if((szRet = (char *)result.getStringField(idx)) != NULL) 
                {
                    strncpy(pInstance->szModemId, szRet, MIN(16, strlen(szRet)));
                    StrToEUI64(pInstance->szModemId, &pInstance->modemId);
                }                                                                   idx++; //*

                pInstance->nPortNum = result.getIntField(idx);                      idx++; //*

                pMetering = (METERINGINS *)MALLOC(sizeof(METERINGINS));
                memset(pMetering, 0, sizeof(METERINGINS));
                pInstance->pMetering[0] = pMetering;
                pInstance->nMeteringCnt = 1;
            }
            else 
            {
                idx += 2;   // 새로 생성해서 증가한 만큼 
            }

            if(pMetering == NULL) break;

            ConvTimeStamp(result.getStringField(idx), &baseTime);                       idx++; 

            //XDEBUG("nBpId %d\r\n", nBpId);
            if(memcmp(&prevTime, &baseTime, sizeof(TIMESTAMP))!=0)
            {
                memcpy(&prevTime, &baseTime, sizeof(TIMESTAMP));
                memcpy(&pMetering->baseTime, &baseTime, sizeof(TIMESTAMP));
                ConvTimeStamp(result.getStringField(idx), &pMetering->meterTime);       idx++; //*

                nBlobLen = 0; szRet = (char *)result.getBlobField(idx, nBlobLen);       idx++; //*
                if(nBlobLen > 0)
                {
                    pMetering->pBaseValue = (unsigned long long *)MALLOC(nBlobLen);
                    memcpy((char *)pMetering->pBaseValue, szRet, nBlobLen);
                    nChCount = nBlobLen / sizeof(unsigned long long);
                    if(pInstance->nChannelCnt < nChCount) { pInstance->nChannelCnt = nChCount; }
                }
                nBlobLen = 0; szRet = (char *)result.getBlobField(idx, nBlobLen);       idx++; //*
                if(nBlobLen > 0)
                {
                    pMetering->pLastValue = (unsigned long long *)MALLOC(nBlobLen);
                    memcpy((char *)pMetering->pLastValue, szRet, nBlobLen);
                    nChCount = nBlobLen / sizeof(unsigned long long);
                    if(pInstance->nChannelCnt < nChCount) { pInstance->nChannelCnt = nChCount; }
                }
            }
            else
            {
                idx += 3;   // 새로 생성해서 증가한 만큼 
            }
            
            if(pMetering->nLpCnt < LP_MAX) {
                pLp = (LOADPROFILEINS *)MALLOC(sizeof(LOADPROFILEINS));
                memset(pLp,0,sizeof(LOADPROFILEINS));

                pLp->_id = result.getIntField(idx);                                 idx++; //*
                ConvTimeStamp(result.getStringField(idx), &pLp->_snapShotTime);     idx++; //*
                nBlobLen = 0; szRet = (char *)result.getBlobField(idx, nBlobLen);   idx++; //*
                if(nBlobLen > 0)
                {
                    pLp->pValue = (unsigned int *)MALLOC(nBlobLen);
                    memcpy((char *)pLp->pValue, szRet, nBlobLen);
                }
                pMetering->pLoadProfile[pMetering->nLpCnt] = pLp;
                pMetering->nLpCnt ++;
                pInstance->nTotLpCnt++;

                //XDEBUG(" Tot %d Lp %d\r\n", pMetering->nLpCnt, pInstance->nTotLpCnt);
                stmt2.bind(1, (int)pLp->_id);
                stmt2.execDML();
                stmt2.reset();
            } else {
                idx += 3;
            }

            result.nextRow();
        }

        if(pInstance != NULL)
        {
            AddResult(pResult, pInstance);
        }
        stmt.finalize();
        stmt2.finalize();

        TRANSACTION_COMMIT;
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "MeteringTbl DB ERROR TryUpload: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}

BOOL CMeteringHelper::UploadComplete(int nCount, UINT *pId)
{
    CppSQLite3Query result;
    int i;

    if(pId == NULL || nCount <= 0) 
    {
        XDEBUG("MeteringHelper::UploadComplete : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;

        // Transaction
        TRANSACTION_BEGIN;

        stmt = m_SqliteHelper.compileStatement(
                "UPDATE OR IGNORE LoadProfileTbl "
                "SET "
                  "uploadTime = DATETIME('now','localtime'), "
                  "lastUpdate = DATETIME('now','localtime') "
                "WHERE "
                  "uploadTime is NULL "
                  "AND _id = ? "
            );

        for(i=0;i<nCount;i++) {
            stmt.bind(1, (int)pId[i]);
            stmt.execDML();
            stmt.reset();
        }
        stmt.finalize();

        TRANSACTION_COMMIT;
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "MeteringTbl DB ERROR UploadComplete: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}



/** DB를 Open/Close 하지 않는다.
 */
BOOL CMeteringHelper::_DeleteMetering(const char *szModemId, int nPortNum, const char *fromDateStr, const char *toDateStr)
{
    int idx=0;

    if(!CheckDB()) return FALSE;

    try
    {
        CppSQLite3Statement stmt;

        if(szModemId == NULL) 
        {
            // 전체 Delete
            stmt = m_SqliteHelper.compileStatement("DELETE FROM MeteringTbl; ");
        } 
        else 
        {
            idx = 1;
            if(nPortNum < 0) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM MeteringTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId) "
                    "; ");
                stmt.bind(idx, szModemId);              idx++;
            } else if (fromDateStr == NULL && toDateStr == NULL) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM MeteringTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                    "; ");
                stmt.bind(idx, szModemId);              idx++;
                stmt.bind(idx, nPortNum);               idx++;
            } else {
                if(fromDateStr == NULL) {
                    stmt = m_SqliteHelper.compileStatement(
                        "DELETE FROM MeteringTbl "
                        "WHERE "
                            "_meterId IN "
                            "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                            " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                            "AND baseTime < ? "
                        "; ");
                    stmt.bind(idx, szModemId);          idx++;
                    stmt.bind(idx, nPortNum);           idx++;
                    stmt.bind(idx, toDateStr);          idx++;
                }else if(toDateStr == NULL) {
                    stmt = m_SqliteHelper.compileStatement(
                        "DELETE FROM MeteringTbl "
                        "WHERE "
                            "_meterId IN "
                            "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                            " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                            "AND baseTime > ? "
                        "; ");
                    stmt.bind(idx, szModemId);          idx++;
                    stmt.bind(idx, nPortNum);           idx++;
                    stmt.bind(idx, fromDateStr);        idx++;
                }else {
                    stmt = m_SqliteHelper.compileStatement(
                        "DELETE FROM MeteringTbl "
                        "WHERE "
                            "_meterId IN "
                            "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                            " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                            "AND baseTime > ? AND baseTime < ? "
                        "; ");
                    stmt.bind(idx, szModemId);          idx++;
                    stmt.bind(idx, nPortNum);           idx++;
                    stmt.bind(idx, fromDateStr);        idx++;
                    stmt.bind(idx, toDateStr);          idx++;
                }
            }
        }

        stmt.execDML();
        stmt.finalize();
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "MeteringTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

/** DB를 Open/Close 하지 않는다.
 */
BOOL CMeteringHelper::_DeleteDayLastPulse(const char *szModemId, int nPortNum, const char *fromDateStr, const char *toDateStr)
{
    int idx=0;

    if(!CheckDB()) return FALSE;

    try
    {
        CppSQLite3Statement stmt;

        if(szModemId == NULL) 
        {
            // 전체 Delete
            stmt = m_SqliteHelper.compileStatement("DELETE FROM DayLastPulseTbl; ");
        } 
        else 
        {
            idx = 1;
            if(nPortNum < 0) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM DayLastPulseTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId) "
                    "; ");
                stmt.bind(idx, szModemId);              idx++;
            } else if (fromDateStr == NULL && toDateStr == NULL) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM DayLastPulseTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                    "; ");
                stmt.bind(idx, szModemId);              idx++;
                stmt.bind(idx, nPortNum);               idx++;
            } else {
                if(fromDateStr == NULL) {
                    stmt = m_SqliteHelper.compileStatement(
                        "DELETE FROM DayLastPulseTbl "
                        "WHERE "
                            "_meterId IN "
                            "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                            " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                            "AND baseTime < ? "
                        "; ");
                    stmt.bind(idx, szModemId);          idx++;
                    stmt.bind(idx, nPortNum);           idx++;
                    stmt.bind(idx, toDateStr);          idx++;
                }else if(toDateStr == NULL) {
                    stmt = m_SqliteHelper.compileStatement(
                        "DELETE FROM DayLastPulseTbl "
                        "WHERE "
                            "_meterId IN "
                            "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                            " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                            "AND baseTime > ? "
                        "; ");
                    stmt.bind(idx, szModemId);          idx++;
                    stmt.bind(idx, nPortNum);           idx++;
                    stmt.bind(idx, fromDateStr);        idx++;
                }else {
                    stmt = m_SqliteHelper.compileStatement(
                        "DELETE FROM DayLastPulseTbl "
                        "WHERE "
                            "_meterId IN "
                            "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                            " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                            "AND baseTime > ? AND baseTime < ? "
                        "; ");
                    stmt.bind(idx, szModemId);          idx++;
                    stmt.bind(idx, nPortNum);           idx++;
                    stmt.bind(idx, fromDateStr);        idx++;
                    stmt.bind(idx, toDateStr);          idx++;
                }
            }
        }

        stmt.execDML();
        stmt.finalize();
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "DayLastPulseTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

/** DB를 Open/Close 하지 않는다.
 */
BOOL CMeteringHelper::_DeleteLoadProfile(const char *szModemId, int nPortNum, const char *fromDateStr, const char *toDateStr)
{
    int idx=0;

    if(!CheckDB()) return FALSE;

    try
    {
        CppSQLite3Statement stmt;

        if(szModemId == NULL) 
        {
            // 전체 Delete
            stmt = m_SqliteHelper.compileStatement("DELETE FROM LoadProfileTbl; ");
        } 
        else 
        {
            idx = 1;
            if(nPortNum < 0) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM LoadProfileTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId) "
                    "; ");
                stmt.bind(idx, szModemId);              idx++;
            } else if (fromDateStr == NULL && toDateStr == NULL) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM LoadProfileTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                    "; ");
                stmt.bind(idx, szModemId);              idx++;
                stmt.bind(idx, nPortNum);               idx++;
            } else {
                if(fromDateStr == NULL) {
                    stmt = m_SqliteHelper.compileStatement(
                        "DELETE FROM LoadProfileTbl "
                        "WHERE "
                            "_meterId IN "
                            "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                            " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                            "AND _snapShotTime < ? "
                        "; ");
                    stmt.bind(idx, szModemId);          idx++;
                    stmt.bind(idx, nPortNum);           idx++;
                    stmt.bind(idx, toDateStr);          idx++;
                }else if(toDateStr == NULL) {
                    stmt = m_SqliteHelper.compileStatement(
                        "DELETE FROM LoadProfileTbl "
                        "WHERE "
                            "_meterId IN "
                            "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                            " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                            "AND _snapShotTime > ? "
                        "; ");
                    stmt.bind(idx, szModemId);          idx++;
                    stmt.bind(idx, nPortNum);           idx++;
                    stmt.bind(idx, fromDateStr);        idx++;
                }else {
                    stmt = m_SqliteHelper.compileStatement(
                        "DELETE FROM LoadProfileTbl "
                        "WHERE "
                            "_meterId IN "
                            "SELECT me._id FROM MeterTbl me, ModemTbl mo "
                            " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                            "AND _snapShotTime > ? AND _snapShotTime < ? "
                        "; ");
                    stmt.bind(idx, szModemId);          idx++;
                    stmt.bind(idx, nPortNum);           idx++;
                    stmt.bind(idx, fromDateStr);        idx++;
                    stmt.bind(idx, toDateStr);          idx++;
                }
            }
        }

        stmt.execDML();
        stmt.finalize();
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "LoadProfileTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CMeteringHelper::Delete()
{
    return Delete((char *)NULL, -1, NULL, NULL);
}

BOOL CMeteringHelper::Delete(const EUI64 *pModemId, int nPortNum)
{
    char szId[32]={0,};

    if(pModemId == NULL) {
        return Delete((char *)NULL, -1, NULL, NULL);
    }

    EUI64ToStr(pModemId, szId);
    return Delete(szId, nPortNum, NULL, NULL);
}

BOOL CMeteringHelper::Delete(const char *szModemId, int nPortNum)
{
    if(szModemId == NULL) {
        return Delete((char *)NULL, -1, NULL, NULL);
    }

    return Delete(szModemId, nPortNum, NULL, NULL);
}

BOOL CMeteringHelper::Delete(const EUI64 *pModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo)
{
    char szId[32]={0,};

    if(pModemId == NULL) {
        return Delete((char *)NULL, -1, pFrom, pTo);
    }

    EUI64ToStr(pModemId, szId);
    return Delete(szId, nPortNum, pFrom, pTo);
}

BOOL CMeteringHelper::Delete(const char *szModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo)
{
    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        char fromTimeStr[32], toTimeStr[32];
        char fromDateStr[16], toDateStr[16];

        if(pFrom) 
        {
            ConvDateString(pFrom, fromDateStr);
            ConvTimeString(pFrom, fromTimeStr);
        }
        if(pTo) 
        {
            ConvDateString(pTo, toDateStr);
            ConvTimeString(pTo, toTimeStr);
        }

        _DeleteLoadProfile(szModemId, nPortNum, pFrom ? fromTimeStr : NULL, pTo ? toTimeStr : NULL);
        _DeleteDayLastPulse(szModemId, nPortNum, pFrom ? fromDateStr : NULL, pTo ? toDateStr : NULL);
        _DeleteMetering(szModemId, nPortNum, pFrom ? fromDateStr : NULL, pTo ? toDateStr : NULL);

        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "Metering ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CMeteringHelper::InitializeDB()
{
    try
    {    
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS MeteringTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "_meterId INTEGER NOT NULL, "
                "_meteringTime DATETIME NOT NULL, "
                "meterTime DATETIME, "
                "lastValue , "      // BLOB
                "meterTimeOffset INTEGER, "
                "baseTime DATETIME, "
                "baseValue , "      // BLOB
                "lastUpdate DATETIME NOT NULL,  "
                "FOREIGN KEY(_meterId) REFERENCES MeterTbl(_id), "
                "UNIQUE ( _meterId, _meteringTime ) "
            ");" );
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS DayLastPulseTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "_meterId INTEGER NOT NULL, "
                "meterTime DATETIME, "
                "lastValue , "      // BLOB
                "baseTime DATETIME, "
                "baseValue , "      // BLOB
                "lastUpdate DATETIME NOT NULL,  "
                "FOREIGN KEY(_meterId) REFERENCES MeterTbl(_id), "
                "UNIQUE ( _meterId, baseTime ) "
            ");" );
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS LoadProfileTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "_meterId INTEGER NOT NULL, "
                "_meteringId INTEGER, "
                "_snapShotTime DATETIME NOT NULL, "
                "value NOT NULL, "      // BLOB
                "tryUpload INTEGER DEFAULT 0, "
                "uploadTime DATETIME, "
                "lastUpdate DATETIME NOT NULL, "
                "FOREIGN KEY(_meterId) REFERENCES MeterTbl(_id), "
                "FOREIGN KEY(_meteringId) REFERENCES MeteringTbl(_id), "
                "UNIQUE ( _meterId, _snapShotTime ) "
            ");" );

        m_SqliteHelper.execDML("CREATE INDEX lp_idx_01 ON LoadProfileTbl(uploadTime);");
        m_SqliteHelper.execDML("CREATE VIEW IF NOT EXISTS WaitUploadLpView AS "
                "SELECT mo.address addr, me._portNum pnum, "
                  "me._Id mid, lp._id lpid, lp._snapShotTime stamp, date(lp._snapShotTime) d, "
                  "lp._meteringId meteringid, lp.value value, lp.uploadTime uploadTime "
                "FROM LoadProfileTbl lp, MeterTbl me, ModemTbl mo "
                "WHERE "
                  "mo._id = me._modemId "
                  "AND me._id = lp._meterId "
                  "AND lp.uploadTime is NULL "
                  "LIMIT 1024 ");
        /*
        m_SqliteHelper.execDML("CREATE VIEW IF NOT EXISTS WaitUploadBpView AS "
                "SELECT mt._id meteringId, mt._meterid mid, mt.meterTime, mt.baseTime, bp._channelCfgId chid, "
                  "bp.baseValue, bp.lastValue, date(mt.baseTime) d "
                "FROM MeteringTbl mt, BasePulseTbl bp, LoadProfileTbl lp "
                "WHERE mt._id = bp._meteringId "
                     "AND mt._id = lp._meteringId "
                     "AND lp.uploadTime is NULL "
                     "GROUP BY mt._meterId, bp._channelCfgId, date(mt.baseTime); ");
                     */

        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "Initialize DB ERROR: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}

    return TRUE;
}

BOOL CMeteringHelper::UpgradeDB()
{
    try
    {
        if(!Open()) return FALSE;
        //m_SqliteHelper.execDML("DROP VIEW IF EXISTS WaitUploadBpView;");
        m_SqliteHelper.execDML("DROP VIEW IF EXISTS WaitUploadLpView;");

        m_SqliteHelper.execDML("DROP INDEX IF EXISTS lp_idx_01;");

        m_SqliteHelper.execDML("DROP TABLE IF EXISTS LoadProfileTbl; ");
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS BasePulseTbl; ");
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS DayLastPulseTbl; ");
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS MeteringTbl; ");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}


BOOL CMeteringHelper::FreeData(void *pData)
{
    METERINGDATAINS * ins;
    int i;

    if(pData == NULL) return FALSE;

    ins = (METERINGDATAINS *)pData;

    for(i=0;i<ins->nMeteringCnt && i<METERING_MAX;i++)
    {
        if(ins->pMetering[i])
        {
            FreeMeteringIns(ins->pMetering[i]);
            FREE(ins->pMetering[i]);
            ins->pMetering[i] = NULL;
        }
    }

    return TRUE;
}

void CMeteringHelper::FreeMeteringIns(METERINGINS* meteringIns)
{
    if(meteringIns == NULL) return;
    if(meteringIns->pLastValue != NULL) 
    {
        FREE(meteringIns->pLastValue);
        meteringIns->pLastValue = NULL;
    }
    if(meteringIns->pBaseValue != NULL) 
    {
        FREE(meteringIns->pBaseValue);
        meteringIns->pBaseValue = NULL;
    }
    for(int i=0; i < meteringIns->nLpCnt; i++)
    {
        if(meteringIns->pLoadProfile[i] != NULL)
        {
            FreeLoadProfileIns(meteringIns->pLoadProfile[i]);
            FREE(meteringIns->pLoadProfile[i]);
            meteringIns->pLoadProfile[i] = NULL;
        }
    }
}

void CMeteringHelper::FreeLoadProfileIns(LOADPROFILEINS* lpIns)
{
    if(lpIns == NULL) return;
    if(lpIns->pValue != NULL)
    {
        FREE(lpIns->pValue);
        lpIns->pValue = NULL;
    }
}

BOOL CMeteringHelper::DumpValue(char *str, int valueSize, int count, void *pValue)
{
    int i;

    if(str != NULL)
    {
        XDEBUG("%s", str);
    }

    if(pValue == NULL)
    {
        return FALSE;
    }

    for(i=0; i < count; i++)
    {
        XDEBUG("%llu ", BigStreamToHostLong((BYTE *)pValue + (i * valueSize), valueSize)); 
    }

    return TRUE;
}

BOOL CMeteringHelper::DumpData(void *pData)
{
    METERINGDATAINS * ins;
    METERINGINS ** mins;
    LOADPROFILEINS ** lins;
    char timeStr1[32], timeStr2[32];
    int i,j;

    if(pData == NULL) return FALSE;
    ins = (METERINGDATAINS *)pData;

    XDEBUG(" _meter %d _modem [%s %d] chCnt %d totLp %d\r\n",
            ins->_meterId, ins->szModemId, ins->nPortNum, ins->nChannelCnt, ins->nTotLpCnt);
    mins = ins->pMetering;
    for(i=0;i<ins->nMeteringCnt;i++)
    {
        ConvTimeString(&mins[i]->meterTime, timeStr1);
        ConvDateString(&mins[i]->baseTime, timeStr2);
        XDEBUG("    %02d _id %d meterTime [%s] offset %d baseTime [%s] LP %d\r\n", i,
                    mins[i]->_id, timeStr1, mins[i]->nMeterTimeOffset, timeStr2, mins[i]->nLpCnt);

        DumpValue(const_cast<char*>("    Last Value : "), sizeof(unsigned long long), 
                ins->nChannelCnt, mins[i]->pLastValue);
        XDEBUG("\r\n");
        DumpValue(const_cast<char*>("    Base Value : "), sizeof(unsigned long long), 
                    ins->nChannelCnt, mins[i]->pBaseValue);
        XDEBUG("\r\n");

        lins = mins[i]->pLoadProfile;
        for(j=0;j<mins[i]->nLpCnt;j++)
        {
            ConvTimeString(&lins[j]->_snapShotTime, timeStr1);
            ConvTimeString(&lins[j]->uploadTime, timeStr2);
            XDEBUG("    + %2d _id %d %s try %d upload [%s] ", j,
                lins[j]->_id, timeStr1, lins[j]->nTryUpload, timeStr2);
            DumpValue(NULL, sizeof(unsigned int), ins->nChannelCnt, lins[j]->pValue);
            XDEBUG("\r\n");
        }
    }

    return TRUE;
}

