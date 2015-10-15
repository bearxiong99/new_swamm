
#include "types/identification.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "if4api.h"

#include "ChannelCfgHelper.h"
#include "MeterHelper.h"
#include "DbUtil.h"

//////////////////////////////////////////////////////////////////////
// CChannelCfgHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS        "ChannelCfg"

#define DB_VERSION      1   // Channel Configure에 isCurrent 추가

#define SELECT_STMT         "SELECT "   \
                                "cc._id, me._id, mo._id, mo.address, me._portNum, cc._index, "  \
                                "cc.unit, cc.sigExp, cc.valueType, cc.objType, cc.chType, " \
                                "cc.isCurrent, cc.tryUpload, cc.uploadTime, " \
                                "cc.lastUpdate "    \
                            "FROM ModemTbl mo, MeterTbl me, ChannelCfgTbl cc "    \
                            "WHERE "    \
                                "mo._id = me._modemId " \
                                "AND me._id = cc._meterId "


CChannelCfgHelper   	*m_pChannelCfgHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CChannelCfgHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CChannelCfgHelper::CChannelCfgHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pChannelCfgHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CChannelCfgHelper::CChannelCfgHelper() :
    CDatabaseHelper(DB_FILE_AGENT, DB_CLASS, DB_VERSION)
{
	m_pChannelCfgHelper	= this;
}

CChannelCfgHelper::~CChannelCfgHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CChannelCfgHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CChannelCfgHelper::Initialize()
{
    return CDatabaseHelper::Initialize();
}

/** Channel Configuration Update.
 *
 * @param nMeterId      Meter Id (Foreign Key)
 * @param nChIdx        Channel Index(start 0)
 * @param nUnit         Unit
 * @param nSigExp       Sign and Exponential
 * @param nValueType    Value Type
 * @param nObjType      Object Type
 * @param nChType       Channel Type
 *
 */
BOOL CChannelCfgHelper::Update(const UINT nMeterId, const BYTE nChIdx, 
                const BYTE nUnit, const BYTE nSigExp, const BYTE nValueType, 
                const BYTE nObjType, const BYTE nChType)
{
    int idx=1;
    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Query result;
        CppSQLite3Statement stmt;
 
        // Transaction
        TRANSACTION_BEGIN;

        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM MeterTbl WHERE _id = ? ;");
        idx = 1;
        stmt.bind(idx, (int)nMeterId);                  idx++;
        result = stmt.execQuery();
        if(result.eof()) {
	        XDEBUG(ANSI_COLOR_RED "DB ERROR: Invalid Meter ID (%d)\r\n" ANSI_NORMAL, nMeterId);
            // Transaction
            TRANSACTION_ROLLBACK; Close();
            return FALSE;
        }
        stmt.finalize();

        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM ChannelCfgTbl "
                "WHERE _meterId = ? AND _index = ? AND isCurrent = 1 "
                "AND unit = ? AND sigExp = ? AND valueType = ? AND objType = ? AND chType = ? ;");
        idx = 1;
        stmt.bind(idx, (int)nMeterId);                      idx++;
        stmt.bind(idx, nChIdx);                             idx++;
        stmt.bind(idx, (int)nUnit);                         idx++;
        stmt.bind(idx, (int)nSigExp);                       idx++;
        stmt.bind(idx, (int)nValueType);                    idx++;
        stmt.bind(idx, (int)nObjType);                      idx++;
        stmt.bind(idx, (int)nChType);                       idx++;

        result = stmt.execQuery();
        if(!result.eof()) {
            switch(result.getIntField(0)) 
            {
                case 0:
                    break;
                case 1: 
                    TRANSACTION_ROLLBACK; Close();
                    return TRUE;
                default:
		            XDEBUG(ANSI_COLOR_RED "DB ERROR: Too many ChannelCfg instances (%d)\r\n" ANSI_NORMAL, result.getIntField(0));
                    // Transaction
                    TRANSACTION_ROLLBACK; Close();
                    return FALSE;
            }
        }
        stmt.finalize();

        stmt = m_SqliteHelper.compileStatement("UPDATE OR IGNORE ChannelCfgTbl "
            "SET isCurrent = 0, lastUpdate = DATETIME('now','localtime') "
            "WHERE _meterId = ? AND _index = ? AND isCurrent = 1 ;");
        idx = 1;
        stmt.bind(idx, (int)nMeterId);                      idx++;
        stmt.bind(idx, nChIdx);                             idx++;
        stmt.execDML();
        stmt.finalize();

        stmt = m_SqliteHelper.compileStatement(
            "INSERT INTO ChannelCfgTbl "
                "( unit, sigExp, valueType, objType, chType,  _meterId, _index, isCurrent, lastUpdate ) "
            "VALUES "
                "( ?,    ?,      ?,         ?,       ?,       ?,        ?,      1,         DATETIME('now','localtime')); ");

        idx = 1;
        stmt.bind(idx, (int)nUnit);                         idx++;
        stmt.bind(idx, (int)nSigExp);                       idx++;
        stmt.bind(idx, (int)nValueType);                    idx++;
        stmt.bind(idx, (int)nObjType);                      idx++;
        stmt.bind(idx, (int)nChType);                       idx++;
        stmt.bind(idx, (int)nMeterId);                      idx++;
        stmt.bind(idx, (int)nChIdx);                        idx++;

        stmt.execDML();
        stmt.finalize();

        // Transaction
        TRANSACTION_COMMIT; Close();
        return TRUE;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "ChannelCfgTbl DB ERROR UPDATE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        // Transaction
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
	}

    return FALSE;
}

/** Channel Configuration Update.
 *
 * @param szModemId
 * @param nPortNum
 * @param nChIdx        Channel Index(start 0)
 * @param nUnit         Unit
 * @param nSigExp       Sign and Exponential
 * @param nValueType    Value Type
 * @param nObjType      Object Type
 * @param nChType       Channel Type
 *
 */
BOOL CChannelCfgHelper::Update(const char *szModemId, int nPortNum, const BYTE nChIdx, 
                const BYTE nUnit, const BYTE nSigExp, const BYTE nValueType, 
                const BYTE nObjType, const BYTE nChType)
{
    UINT nMeterId = 0;
    if(szModemId == NULL) return FALSE;
    if(m_pMeterHelper == NULL) return FALSE;
    if(!m_pMeterHelper->GetId(szModemId, nPortNum, &nMeterId)) return FALSE;

    return Update(nMeterId, nChIdx, nUnit, nSigExp, nValueType, nObjType, nChType);
}

/** Channel Configuration Update.
 *
 * @param pModemId
 * @param nPortNum      Port Number
 * @param nChIdx        Channel Index(start 0)
 * @param nUnit         Unit
 * @param nSigExp       Sign and Exponential
 * @param nValueType    Value Type
 * @param nObjType      Object Type
 * @param nChType       Channel Type
 *
 */
BOOL CChannelCfgHelper::Update(const EUI64 *pModemId, int nPortNum, const BYTE nChIdx, 
                const BYTE nUnit, const BYTE nSigExp, const BYTE nValueType,
                const BYTE nObjType, const BYTE nChType)
{
    UINT nMeterId = 0;
    if(pModemId == NULL) return FALSE;
    if(m_pMeterHelper == NULL) return FALSE;
    if(!m_pMeterHelper->GetId(pModemId, nPortNum, &nMeterId)) return FALSE;

    return Update(nMeterId, nChIdx, nUnit, nSigExp, nValueType, nObjType, nChType);
}

CHANNELCFGINS* CChannelCfgHelper::GetData(Row *pRow)
{
    if(pRow == NULL || pRow->pData == NULL) return NULL;
    return (CHANNELCFGINS *)pRow->pData;
}

BOOL CChannelCfgHelper::Select(DBFetchData *pResult)
{
    return Select((char *)NULL, -1, -1, TRUE, pResult);
}

BOOL CChannelCfgHelper::Select(const EUI64 *pId, int nPortNum, BOOL bLast, DBFetchData *pResult)
{
    char szId[32]={0,};

    if(pId == NULL) {
        return Select((char *)NULL, -1, -1, bLast, pResult);
    }

    EUI64ToStr(pId, szId);
    return Select(szId, nPortNum, -1, bLast, pResult);
}

BOOL CChannelCfgHelper::Select(const char *szModemId, int nPortNum, BOOL bLast, DBFetchData *pResult)
{
    if(szModemId == NULL) {
        return Select((char *)NULL, -1, -1, bLast, pResult);
    }

    return Select(szModemId, nPortNum, -1, bLast, pResult);
}

BOOL CChannelCfgHelper::Select(const char *szModemId, int nPortNum, int nChIdx, BOOL bLast, DBFetchData *pResult)
{
    CppSQLite3Query result;
    int idx=0;

    if(pResult == NULL) 
    {
        XDEBUG("ChannelCfgHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        char queryBuffer[1024];

        if(szModemId == NULL) 
        {
            sprintf(queryBuffer,"%s %s ;", SELECT_STMT, (bLast ? "AND isCurrent = 1" : ""));
            // 전체 Select
            stmt = m_SqliteHelper.compileStatement(queryBuffer);
        } 
        else 
        {
            if(nPortNum < 0) {
                sprintf(queryBuffer,"%s AND mo.address = ? %s ;", SELECT_STMT, (bLast ? "AND isCurrent = 1" : ""));
                stmt = m_SqliteHelper.compileStatement(queryBuffer);
            }else if(nChIdx < 0) {
                sprintf(queryBuffer,"%s AND mo.address = ? AND me._portNum = ? %s ;", 
                        SELECT_STMT, (bLast ? "AND isCurrent = 1" : ""));
                stmt = m_SqliteHelper.compileStatement(queryBuffer);
            } else {
                sprintf(queryBuffer,"%s AND mo.address = ? AND me._portNum = ? AND cc._index = ? %s ;", 
                        SELECT_STMT, (bLast ? "AND isCurrent = 1" : ""));
                stmt = m_SqliteHelper.compileStatement(queryBuffer);
            }
            idx = 1;
            stmt.bind(idx, szModemId);      idx++;
            if(nPortNum >= 0) {
                stmt.bind(idx, nPortNum);   idx++;
                if(nChIdx >= 0) {
                    stmt.bind(idx, nChIdx); idx++;
                }
            }
        }

        result = stmt.execQuery();
        _Fetch(&result, pResult);
        stmt.finalize();
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "ChannleCfgTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CChannelCfgHelper::Select(UINT nMeterId, DBFetchData *pResult)
{
    return Select(nMeterId, -1, TRUE, pResult);
}

BOOL CChannelCfgHelper::Select(UINT nMeterId, BOOL bLast, DBFetchData *pResult)
{
    return Select(nMeterId, -1, bLast, pResult);
}

BOOL CChannelCfgHelper::Select(UINT nMeterId, int nChIdx, BOOL bLast, DBFetchData *pResult)
{
    CppSQLite3Query result;
    int idx=0;

    if(pResult == NULL) 
    {
        XDEBUG("ChannelCfgHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        char queryBuffer[1024];

        if(nChIdx < 0) {
            sprintf(queryBuffer,"%s AND me._id = ? %s ;", 
                    SELECT_STMT, (bLast ? "AND isCurrent = 1" : ""));
            stmt = m_SqliteHelper.compileStatement(queryBuffer);
        } else {
            sprintf(queryBuffer,"%s AND me._id = ? AND cc._index = ? %s ;", 
                    SELECT_STMT, (bLast ? "AND isCurrent = 1" : ""));
            stmt = m_SqliteHelper.compileStatement(queryBuffer);
        }
        idx = 1;
        stmt.bind(idx, (int)nMeterId);   idx++;
        if(nChIdx >= 0) {
            stmt.bind(idx, nChIdx); idx++;
        }

        result = stmt.execQuery();
        _Fetch(&result, pResult);
        stmt.finalize();
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "ChannleCfgTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CChannelCfgHelper::_Fetch(CppSQLite3Query *result, DBFetchData *pResult)
{
    CHANNELCFGINS *pInstance;
    char * szRet;
    int idx=0;

    try
    {
        while(!result->eof())
        {
            pInstance = (CHANNELCFGINS *)MALLOC(sizeof(CHANNELCFGINS));
            memset(pInstance, 0, sizeof(CHANNELCFGINS));

            idx = 0;
            pInstance->_id = result->getIntField(idx);                              idx++;
            pInstance->_meterId = result->getIntField(idx);                         idx++;
            pInstance->_modemId = result->getIntField(idx);                         idx++;
            if((szRet = (char *)result->getStringField(idx)) != NULL) 
            {
                strncpy(pInstance->szModemId, szRet, MIN(16, strlen(szRet)));
                StrToEUI64(pInstance->szModemId, &pInstance->modemId);
            }                                                                       idx++;
            pInstance->nPortNum = result->getIntField(idx);                         idx++;
            pInstance->nIndex = result->getIntField(idx);                           idx++;
            pInstance->nUnit = result->getIntField(idx);                            idx++;
            pInstance->nSigExp = result->getIntField(idx);                          idx++;
            pInstance->nValueType = result->getIntField(idx);                       idx++;
            pInstance->nObjType = result->getIntField(idx);                         idx++;
            pInstance->nChType = result->getIntField(idx);                          idx++;
            pInstance->bIsCurrent = result->getIntField(idx) ? TRUE : FALSE;        idx++;
            pInstance->bTryUpload = result->getIntField(idx) ? TRUE : FALSE;        idx++;
            if(!result->fieldIsNull(idx)) {
                pInstance->bUpload = TRUE;
                ConvTimeStamp(result->getStringField(idx), &pInstance->uploadTime);      
            }else{
                pInstance->bUpload = FALSE;
            }                                                                       idx++;
            ConvTimeStamp(result->getStringField(idx), &pInstance->lastUpdate);     idx++;

            AddResult(pResult, pInstance);

            result->nextRow();
        }
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "ChannleCfgTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CChannelCfgHelper::TryUpload(DBFetchData *pResult)
{
    CppSQLite3Query result;

    if(pResult == NULL) 
    {
        XDEBUG("ChannelCfgHelper::TryUpload : NULL Parameter\r\n");
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
            SELECT_STMT
                "AND cc.uploadTime is NULL ; ");

        result = stmt.execQuery();
        _Fetch(&result, pResult);
        stmt.finalize();

        stmt = m_SqliteHelper.compileStatement(
                "UPDATE ChannelCfgTbl "
                "SET "
                  "tryUpload = tryUpload + 1, "
                  "lastUpdate = DATETIME('now','localtime') "
                "WHERE uploadTime is NULL ; "
            );
        stmt.execDML();
        stmt.finalize();

        TRANSACTION_COMMIT;
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "ChannelCfgTbl DB ERROR TryUpload: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        TRANSACTION_ROLLBACK;
        Close();
        return FALSE;
	}
    return TRUE;
}

BOOL CChannelCfgHelper::UploadComplete(int nCount, UINT *pId)
{
    CppSQLite3Query result;
    int i;

    if(pId == NULL || nCount <= 0) 
    {
        XDEBUG("ChannelCfgHelper::UploadComplete : NULL Parameter\r\n");
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
                "UPDATE OR IGNORE ChannelCfgTbl "
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
		XDEBUG(ANSI_COLOR_RED "ChannelCfgTbl DB ERROR UploadComplete: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        TRANSACTION_ROLLBACK;
        Close();
        return FALSE;
	}
    return TRUE;
}

BOOL CChannelCfgHelper::GetId(const EUI64 *pId, int nPortNum, BYTE nChIdx, UINT *nChannelCfgId)
{
    char szId[32]={0,};

    if(pId == NULL) return FALSE;

    EUI64ToStr(pId, szId);
    return GetId(szId, nPortNum, nChIdx, nChannelCfgId);
}

BOOL CChannelCfgHelper::GetId(const char *szId, int nPortNum, BYTE nChIdx, UINT *nChannelCfgId)
{
    DBFetchData fetchData;

    if(nChannelCfgId == NULL || szId == NULL) return FALSE;
    memset(&fetchData, 0, sizeof(DBFetchData));

    if(Select(szId, nPortNum, nChIdx, TRUE, &fetchData)) {
        if(fetchData.nCount>0) {
            CHANNELCFGINS *instance;
            if((instance = GetData(GetHeadRow(&fetchData))) != NULL) {
                *nChannelCfgId = instance->_id;
                FreeResult(&fetchData);
                return TRUE;
            }
        }
        FreeResult(&fetchData);
    } 
    return FALSE;
}

BOOL CChannelCfgHelper::Delete()
{
    return Delete((char *)NULL, -1, -1);
}

BOOL CChannelCfgHelper::Delete(const EUI64 *pModemId, int nPortNum)
{
    char szId[32]={0,};

    if(pModemId == NULL) {
        return Delete((char *)NULL, -1, -1);
    }

    EUI64ToStr(pModemId, szId);
    return Delete(szId, nPortNum, -1);
}

BOOL CChannelCfgHelper::Delete(const char *szModemId, int nPortNum)
{
    if(szModemId == NULL) {
        return Delete((char *)NULL, -1, -1);
    }

    return Delete(szModemId, nPortNum, -1);
}

BOOL CChannelCfgHelper::Delete(const EUI64 *pModemId, int nPortNum, int nChIdx)
{
    char szId[32]={0,};

    if(pModemId == NULL) {
        return Delete((char *)NULL, -1, -1);
    }

    EUI64ToStr(pModemId, szId);
    return Delete(szId, nPortNum, nChIdx);
}

BOOL CChannelCfgHelper::Delete(const char *szModemId, int nPortNum, int nChIdx)
{
    int idx=0;

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;

        if(szModemId == NULL) 
        {
            // 전체 Delete
            stmt = m_SqliteHelper.compileStatement("DELETE FROM ChannelCfgTbl; ");
        } 
        else 
        {
            if(nPortNum < 0) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM ChannelCfgTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId) "
                    "; ");
            } else if (nChIdx < 0) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM ChannelCfgTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                    "; ");
            } else {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM ChannelCfgTbl "
                    "WHERE "
                        "_meterId IN "
                        "(SELECT me._id FROM MeterTbl me, ModemTbl mo "
                        " WHERE mo.address = ? AND mo._id = me._modemId AND me._portNum = ? ) "
                        "AND _index = ? "
                    "; ");
            }
            idx = 1;
            stmt.bind(idx, szModemId);          idx++;
            if(nPortNum >= 0) {
                stmt.bind(idx, nPortNum);       idx++;
                if(nChIdx >= 0) {
                    stmt.bind(idx, nChIdx);     idx++;
                }
            }
        }

        stmt.execDML();
        stmt.finalize();
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "ChannelCfgTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CChannelCfgHelper::InitializeDB()
{
    try
    {    
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS ChannelCfgTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "_meterId INTEGER NOT NULL, "
                "_index INTEGER NOT NULL, "     // Channel Index
                "unit INTEGER NOT NULL, "
                "sigExp INTEGER NOT NULL, "
                "valueType INTEGER NOT NULL, "
                "objType INTEGER NOT NULL, "
                "chType INTEGER NOT NULL, "
                "isCurrent INTEGER NOT NULL, "
                "tryUpload INTEGER DEFAULT 0, "
                "uploadTime DATETIME, "
                "lastUpdate DATETIME NOT NULL,  "
                "FOREIGN KEY(_meterId) REFERENCES MeterTbl(_id) "
                ");" );
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

BOOL CChannelCfgHelper::UpgradeDB()
{
    try
    {
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS ChannelCfgTbl; ");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}


BOOL CChannelCfgHelper::FreeData(void *pData)
{
    return TRUE;
}

BOOL CChannelCfgHelper::DumpData(void *pData)
{
    CHANNELCFGINS * ins;
    if(pData == NULL) return FALSE;
    ins = (CHANNELCFGINS *)pData;

    XDEBUG(" _id %d _meter %d _modem %d [%s %d] idx %d unit 0x%02X sExp 0x%02X obj 0x%02X val 0x%02X cur %c\r\n",
            ins->_id, ins->_meterId, ins->_modemId, ins->szModemId, ins->nPortNum,
            ins->nIndex, ins->nSigExp, ins->nObjType, ins->nValueType, ins->bIsCurrent ? 'T' : 'F');
    return TRUE;
}

