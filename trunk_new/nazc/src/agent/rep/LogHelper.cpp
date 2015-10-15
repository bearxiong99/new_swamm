
#include "types/identification.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "if4api.h"

#include "LogHelper.h"
#include "MeterHelper.h"
#include "DbUtil.h"

//////////////////////////////////////////////////////////////////////
// CLogHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS          "Logging"
#define DB_VERSION        1

#define SELECT_STMT         "SELECT "   \
                                "me._id, lo._id logid, mo.address, me._portNum, "  \
                                "lo.logTime, lo.category, lo.value, " \
                                "lo.tryUpload, lo.createTime, lo.uploadTime, lo.lastUpdate "    \
                            "FROM ModemTbl mo, MeterTbl me, LogTbl lo "    \
                            "WHERE "    \
                                "mo._id = me._modemId " \
                                "AND me._id = lo._meterId " 

CLogHelper   	*m_pLogHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CLogHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CLogHelper::CLogHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pLogHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CLogHelper::CLogHelper() :
    CDatabaseHelper(DB_FILE_AGENT, DB_CLASS, DB_VERSION)
{
	m_pLogHelper	= this;
}

CLogHelper::~CLogHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CLogHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CLogHelper::Initialize()
{
    return CDatabaseHelper::Initialize();
}

/** Log 정보 갱신.
 *
 * 이 함수는 실제로는 Update 되지 않고 항상 Insert만 된다.
 *
 * @param nMeterId          Meter Id (Foreign Key)
 * @param nLpCnt            Log Count
 * @param pLogEntry         Log Entry Instance
 *
 */
BOOL CLogHelper::Update(const UINT nMeterId, int nLogCnt, LOGENTRYINS *pLogEntry)
{
    int idx=1;
    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;

    if(nLogCnt <= 0 || pLogEntry == NULL) return FALSE;

    try
    {
        CppSQLite3Query result;
        CppSQLite3Statement stmt;

        char timeStr[32];
        int i;

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
        stmt = m_SqliteHelper.compileStatement(
            "INSERT OR IGNORE INTO LogTbl "
                "( _meterId, logTime, category, value,  createTime, lastUpdate ) "
            "VALUES "
                "( ?,        ?,       ?,        ?,      DATETIME('now','localtime'), DATETIME('now','localtime')); ");

        for(i=0; i<nLogCnt; i++) 
        {
            idx = 1;
            stmt.bind(idx, (int)nMeterId);                  idx++;
            memset(timeStr, 0, sizeof(timeStr)); ConvTimeString(&pLogEntry[i].logTime, timeStr);
            stmt.bind(idx, timeStr);                        idx++;
            stmt.bind(idx, pLogEntry[i].nCategory);         idx++;
            if(pLogEntry[i].nLogSize > 0 && pLogEntry[i].szLog != NULL)
            {
                stmt.bind(idx, (const unsigned char *)pLogEntry[i].szLog, pLogEntry[i].nLogSize);
            }
            else
            {
                stmt.bindNull(idx);
            }                                               idx++;
            stmt.execDML();
            stmt.reset();
            XDEBUG(ANSI_COLOR_CYAN "   [%d] Category %d Len %d\r\n" ANSI_NORMAL, 
                    i, pLogEntry[i].nCategory, pLogEntry[i].nLogSize);
        }

        stmt.finalize();
        TRANSACTION_COMMIT; Close();

        return TRUE;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "LogTbl DB ERROR UPDATE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
	}


    return FALSE;
}

BOOL CLogHelper::Update(const char *szModemId, int nPortNum, int nLogCnt, LOGENTRYINS *pLogEntry)
{
    UINT nMeterId = 0;
    if(szModemId == NULL) return FALSE;
    if(m_pMeterHelper == NULL) return FALSE;
    if(!m_pMeterHelper->GetId(szModemId, nPortNum, &nMeterId)) return FALSE;

    return Update(nMeterId, nLogCnt, pLogEntry);
}

BOOL CLogHelper::Update(const EUI64 *pModemId, int nPortNum, int nLogCnt, LOGENTRYINS *pLogEntry)
{
    UINT nMeterId = 0;
    if(pModemId == NULL) return FALSE;
    if(m_pMeterHelper == NULL) return FALSE;
    if(!m_pMeterHelper->GetId(pModemId, nPortNum, &nMeterId)) return FALSE;

    return Update(nMeterId, nLogCnt, pLogEntry);
}

LOGDATAINS* CLogHelper::GetData(Row *pRow)
{
    if(pRow == NULL || pRow->pData == NULL) return NULL;
    return (LOGDATAINS *)pRow->pData;
}

BOOL CLogHelper::Select(DBFetchData *pResult)
{
    return Select((char *)NULL, -1, NULL, NULL, pResult);
}

BOOL CLogHelper::Select(TIMESTAMP *pFrom, TIMESTAMP *pTo, DBFetchData *pResult)
{
    return Select((char *)NULL, -1, pFrom, pTo, pResult);
}

BOOL CLogHelper::Select(const EUI64 *pId, int nPortNum, DBFetchData *pResult)
{
    char szId[32]={0,};

    if(pId == NULL) {
        return Select((char *)NULL, -1, NULL, NULL, pResult);
    }

    EUI64ToStr(pId, szId);
    return Select(szId, nPortNum, NULL, NULL,  pResult);
}

BOOL CLogHelper::Select(const char *szModemId, int nPortNum, DBFetchData *pResult)
{
    if(szModemId == NULL) {
        return Select((char *)NULL, -1, NULL, NULL, pResult);
    }

    return Select(szModemId, nPortNum, NULL, NULL, pResult);
}

BOOL CLogHelper::_Fetch(CppSQLite3Query *result, BOOL bUpdate, DBFetchData *pResult)
{
    LOGDATAINS *pInstance = NULL;
    LOGENTRYINS *pLog = NULL;
    char * szRet;
    int idx=0, nLen;
    UINT nMeId, nLogId;

    try
    {
        CppSQLite3Statement stmt = m_SqliteHelper.compileStatement(
                "UPDATE LogTbl "
                "SET "
                  "tryUpload = tryUpload + 1, "
                  "lastUpdate = DATETIME('now','localtime') "
                "WHERE _id = ? "
            );

        while(!result->eof())
        {
            idx = 0;
            nMeId = result->getIntField(idx);                                            idx++;
            nLogId = result->getIntField(idx);                                          idx++;
            if(pInstance==NULL || pInstance->_meterId != nMeId)
            {
                if(pInstance != NULL)
                {
                    AddResult(pResult, pInstance);
                }
                pInstance = (LOGDATAINS *)MALLOC(sizeof(LOGDATAINS));
                memset(pInstance, 0, sizeof(LOGDATAINS));

                pInstance->_meterId = nMeId;
                if((szRet = (char *)result->getStringField(idx)) != NULL) 
                {
                    strncpy(pInstance->szModemId, szRet, MIN(16, strlen(szRet)));
                    StrToEUI64(pInstance->szModemId, &pInstance->modemId);
                }                                                                   idx++; //*
                pInstance->nPortNum = result->getIntField(idx);                      idx++; //*
            }
            else 
            {
                idx += 2;   // 새로 생성해서 증가한 만큼 
            }

            if(pInstance->nLogCnt >= LOG_MAX) 
            {
                result->nextRow();
                continue;
            }
            pLog = (LOGENTRYINS *)MALLOC(sizeof(LOGENTRYINS));
            memset(pLog, 0, sizeof(LOGENTRYINS));
            nLen = 0;

            pLog->_id = nLogId;
            ConvTimeStamp(result->getStringField(idx), &pLog->logTime);                  idx++; 
            pLog->nCategory = result->getIntField(idx);                                  idx++;
            szRet = (char *)result->getBlobField(idx, nLen);                             idx++;
            if(szRet != NULL && nLen > 0)
            {
                pLog->nLogSize = nLen;
                pLog->szLog = (BYTE *)MALLOC(nLen);
                memcpy(pLog->szLog, szRet, nLen);
            }
            pLog->nTryUpload = result->getIntField(idx);                                 idx++;
            ConvTimeStamp(result->getStringField(idx), &pLog->createTime);               idx++;
            if(!result->fieldIsNull(idx)) {
                ConvTimeStamp(result->getStringField(idx), &pLog->uploadTime);         
            }                                                                            idx++;

            ConvTimeStamp(result->getStringField(idx), &pLog->lastUpdate);               idx++;

            pInstance->pLog[pInstance->nLogCnt] = pLog;
            pInstance->nLogCnt ++;

            if(bUpdate)
            {
                stmt.bind(1, (int)nLogId);
                stmt.execDML();
                stmt.reset();
            }

            result->nextRow();
        }

        if(pInstance != NULL)
        {
            AddResult(pResult, pInstance);
        }
        stmt.finalize();
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "LogTbl DB ERROR PATCH: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

/** Log select.
 *
 * @param szModemId
 * @param nPortNum
 * @param pFrom
 * @param pTo
 * @param pResult
 *
 */
BOOL CLogHelper::Select(const char *szModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo,
        DBFetchData *pResult)
{
    CppSQLite3Query result;
    int idx=0;

    if(pResult == NULL) 
    {
        XDEBUG("LogHelper::Select : NULL Parameter\r\n");
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

        sprintf(queryBuffer, "%s ", SELECT_STMT);
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
        if(pFrom) strcat(queryBuffer, "AND lo.logTime > ? ");
        if(pTo) strcat(queryBuffer, "AND lo.logTime < ? ");
        strcat(queryBuffer, "ORDER BY me._id, me._portNum, lo.logTime ");

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
        if(pFrom) 
        {
            stmt.bind(idx, fromDateStr);            idx++;
        }
        if(pTo) 
        {
            stmt.bind(idx, toDateStr);              idx++;
        }

        result = stmt.execQuery();
        _Fetch(&result, FALSE, pResult);
        stmt.finalize();

        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "LogTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CLogHelper::TryUpload(DBFetchData *pResult)
{
    CppSQLite3Query result;

    if(pResult == NULL) 
    {
        XDEBUG("LogHelper::TryUpload : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;

        // Transaction
        TRANSACTION_BEGIN;

        stmt = m_SqliteHelper.compileStatement(SELECT_STMT 
                "AND lo.uploadTime is NULL "
                "ORDER BY me._id ");

        //XDEBUG("%s \r\nAND lo.uploadTime is NULL \r\n", SELECT_STMT);
        result = stmt.execQuery();
        _Fetch(&result, TRUE, pResult);
        stmt.finalize();

        TRANSACTION_COMMIT;
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "LogTbl DB ERROR TryUpload: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}

BOOL CLogHelper::UploadComplete(int nCount, UINT *pId)
{
    CppSQLite3Query result;
    int i;

    if(pId == NULL || nCount <= 0) 
    {
        XDEBUG("LogHelper::UploadComplete : NULL Parameter\r\n");
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
                "UPDATE OR IGNORE LogTbl "
                "SET "
                  "uploadTime = DATETIME('now','localtime'), "
                  "lastUpdate = DATETIME('now','localtime') "
                "WHERE "
                  "uploadTime is NULL "
                  "AND _id = ? "
            );

        for(i=0;i<nCount;i++) 
        {
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
		XDEBUG(ANSI_COLOR_RED "LogTbl DB ERROR UploadComplete: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}


BOOL CLogHelper::Delete()
{
    return Delete((char *)NULL, -1, NULL, NULL);
}

BOOL CLogHelper::Delete(const EUI64 *pModemId, int nPortNum)
{
    char szId[32]={0,};

    if(pModemId == NULL) {
        return Delete((char *)NULL, -1, NULL, NULL);
    }

    EUI64ToStr(pModemId, szId);
    return Delete(szId, nPortNum, NULL, NULL);
}

BOOL CLogHelper::Delete(const char *szModemId, int nPortNum)
{
    if(szModemId == NULL) {
        return Delete((char *)NULL, -1, NULL, NULL);
    }

    return Delete(szModemId, nPortNum, NULL, NULL);
}

BOOL CLogHelper::Delete(const EUI64 *pModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo)
{
    char szId[32]={0,};

    if(pModemId == NULL) {
        return Delete((char *)NULL, -1, pFrom, pTo);
    }

    EUI64ToStr(pModemId, szId);
    return Delete(szId, nPortNum, pFrom, pTo);
}

BOOL CLogHelper::Delete(const char *szModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo)
{
    int idx=0;
    CppSQLite3Statement stmt;

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

        if(szModemId == NULL) 
        {
            // 전체 Delete
            stmt = m_SqliteHelper.compileStatement("DELETE FROM LogTbl; ");
        } 
        else 
        {
            if(nPortNum < 0) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM LogTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId) "
                    "; ");
            } else {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM LogTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                    "; ");
            }
            idx = 1;
            stmt.bind(idx, szModemId);      idx++;
            if(nPortNum >= 0) {
                stmt.bind(idx, nPortNum);   idx++;
            }
        }

        stmt.execDML();
        stmt.finalize();

        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "LogTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CLogHelper::InitializeDB()
{
    try
    {    
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS LogTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "_meterId INTEGER NOT NULL, "
                "logTime DATETIME NOT NULL, "
                "category INTEGER NOT NULL, "
                "value, "                       // BLOB
                "tryUpload INTEGER DEFAULT 0, "
                "uploadTime DATETIME, "
                "createTime DATETIME NOT NULL, "
                "lastUpdate DATETIME NOT NULL, "
                "FOREIGN KEY(_meterId) REFERENCES MeterTbl(_id), "
                "UNIQUE ( _meterId, category, logTime, value ) "
            ");" );

        m_SqliteHelper.execDML("CREATE INDEX log_idx_01 ON LogTbl(uploadTime);");

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

BOOL CLogHelper::UpgradeDB()
{
    try
    {
        if(!Open()) return FALSE;

        m_SqliteHelper.execDML("DROP INDEX IF EXISTS log_idx_01;");
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS LogTbl; ");

        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}


BOOL CLogHelper::FreeData(void *pData)
{
    LOGDATAINS * ins;
    LOGENTRYINS * pLog;
    int i;

    if(pData == NULL) return FALSE;

    ins = (LOGDATAINS *)pData;

    for(i=0;i<ins->nLogCnt && i<LOG_MAX;i++)
    {
        if(ins->pLog[i])
        {
            pLog = ins->pLog[i];
            if(pLog->szLog)
            {
                FREE(pLog->szLog);
                pLog->szLog = NULL;
            }
            FREE(pLog);
            ins->pLog[i] = NULL;
        }
    }

    return TRUE;
}

BOOL CLogHelper::DumpData(void *pData)
{
    LOGDATAINS * ins;
    LOGENTRYINS ** ppLog;
    char logTimeStr[32], createTimeStr[32], uploadTimeStr[32];
    int i;

    if(pData == NULL) return FALSE;
    ins = (LOGDATAINS *)pData;

    XDEBUG(" _meter %d _modem [%s %d] logCnt %d\r\n",
            ins->_meterId, ins->szModemId, ins->nPortNum, ins->nLogCnt);

    ppLog = ins->pLog;
    for(i=0;i<ins->nLogCnt;i++)
    {
        ConvTimeString(&ppLog[i]->logTime, logTimeStr);
        ConvTimeString(&ppLog[i]->createTime, createTimeStr);
        ConvTimeString(&ppLog[i]->uploadTime, uploadTimeStr);
        XDEBUG("    %02d _id %d create [%s] log [%s] upload[%s] %d Bytes\r\n", i,
                ppLog[i]->_id, createTimeStr, logTimeStr, uploadTimeStr, ppLog[i]->nLogSize);
    }

    return TRUE;
}

