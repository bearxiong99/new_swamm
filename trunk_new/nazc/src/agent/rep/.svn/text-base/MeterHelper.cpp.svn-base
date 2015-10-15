
#include "types/identification.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "if4api.h"

#include "MeterHelper.h"
#include "ChannelCfgHelper.h"
#include "MeteringHelper.h"
#include "LogHelper.h"
#include "DbUtil.h"

//////////////////////////////////////////////////////////////////////
// CMeterHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS        "Meter"
#define DB_VERSION    1

#define SELECT_STMT         "SELECT "   \
                                "me._id, mo._id, mo.address, me._portNum, "  \
                                "me.meterSerial, mo.model, " \
                                "me.parserType, me.serviceType, me.vendor, me.energyLevel, me.period, " \
                                "me.hwVer, me.swVer,  " \
                                "me.lastMetering, me.lastFailMetering, me.failReason, " \
                                "me.installTime, me.lastEnergyLevel, " \
                                "me.tryUpload, me.uploadTime, " \
                                "me.lastUpdate "    \
                            "FROM ModemTbl mo, MeterTbl me "    \
                            "WHERE "    \
                                "mo._id = me._modemId "


CMeterHelper   	*m_pMeterHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CMeterHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CMeterHelper::CMeterHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pMeterHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CMeterHelper::CMeterHelper() :
    CDatabaseHelper(DB_FILE_AGENT, DB_CLASS, DB_VERSION)
{
	m_pMeterHelper	= this;
}

CMeterHelper::~CMeterHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CMeterHelper::Initialize()
{
    return CDatabaseHelper::Initialize();
}

/** Meter Inventory Update.
 *
 * @param nModemId      Modem Id (Foreign Key)
 * @param nPortNum      Port Number
 * @param szSerial      Meter Serial
 * @param nParserType   Parser Type
 * @param nServiceType  Service Type
 * @param nVendor       Meter Vendor
 * @param hwVer
 * @param swVer
 * @param nLpPeriod     LP Period
 *
 */
BOOL CMeterHelper::Update(const UINT nModemId, BYTE nPortNum, const char *szSerial, 
        const UINT nParserType, const int nServiceType, const int nVendor, const double hwVer, const double swVer,
        const int nLpPeriod) 
{
    int idx=1;
    BOOL bInsert = FALSE;

#if 1
    XDEBUG("Meter Update %d %d [%s] %d %d %d %f %f %d\r\n", nModemId, nPortNum, 
            szSerial ? szSerial : "NULL", nParserType, nServiceType, nVendor, 
            hwVer, swVer, nLpPeriod);
#endif

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Query result;
        CppSQLite3Statement stmt;
        char queryBuffer[1024];
 
        TRANSACTION_BEGIN;
        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM MeterTbl WHERE _modemId = ? AND _portNum = ? ;");
        idx = 1;
        stmt.bind(idx, (int)nModemId);                      idx++;
        stmt.bind(idx, nPortNum);                           idx++;
        result = stmt.execQuery();
        if(result.eof()) {
            bInsert = TRUE;
        }
        else {
            switch(result.getIntField(0)) 
            {
                case 0: bInsert = TRUE; break;
                case 1: break;
                default:
		                XDEBUG(ANSI_COLOR_RED "DB ERROR: Too many Meter instances (%d)\r\n" ANSI_NORMAL, result.getIntField(0));
                        TRANSACTION_ROLLBACK;
                        Close();
                        return FALSE;
            }
        }

        stmt.finalize();
        if(bInsert) {
            stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM ModemTbl WHERE _id = ? ;");
            idx = 1;
            stmt.bind(idx, (int)nModemId);                  idx++;
            result = stmt.execQuery();
            if(result.eof()) {
		        XDEBUG(ANSI_COLOR_RED "DB ERROR: Invalid Modem ID (%d)\r\n" ANSI_NORMAL, nModemId);
                TRANSACTION_ROLLBACK;
                Close();
                return FALSE;
            }

            stmt.finalize();
            sprintf(queryBuffer,
                "INSERT INTO MeterTbl "
                    "( %s parserType, %s %s %s %s %s _modemId, _portNum, lastUpdate ) "
                "VALUES "
                    "( %s ?, %s %s %s %s %s ?, ?, DATETIME('now','localtime')); ",
                (!IsNullValue(szSerial) ? "meterSerial, " : ""),
                (nServiceType >= 0 ? "serviceType, " : ""),
                (nVendor >= 0 ? "vendor, " : ""),
                (hwVer > 0.0 ? "hwVer, " : ""),
                (swVer > 0.0 ? "swVer, " : ""),
                (nLpPeriod > 0 ? "period, " : ""),
                (!IsNullValue(szSerial) ? "?, " : ""),
                (nServiceType >= 0 ? "?, " : ""),
                (nVendor >= 0 ? "?, " : ""),
                (hwVer > 0.0 ? "?, " : ""),
                (swVer > 0.0 ? "?, " : ""),
                (nLpPeriod > 0 ? "?, " : "")
            );

        } else {
            sprintf(queryBuffer,
                "UPDATE MeterTbl "
                "SET  %s parserType = ?, %s %s %s %s %s lastUpdate = DATETIME('now','localtime'), "
                    "uploadTime = NULL "
                "WHERE "
                    "_modemId = ? AND _portNum = ? "
                    "AND ( %s %s %s %s %s %s parserType != ? ) ",
                (!IsNullValue(szSerial) ? "meterSerial = ?, " : ""),
                (nServiceType >= 0 ? "serviceType = ?, " : ""),
                (nVendor >= 0 ? "vendor = ?, " : ""),
                (hwVer > 0.0 ? "hwVer = ?, " : ""),
                (swVer > 0.0 ? "swVer = ?, " : ""),
                (nLpPeriod > 0 ? "period = ?, " : ""),
                (!IsNullValue(szSerial) ? "meterSerial != ? OR meterSerial is NULL OR " : ""),
                (nServiceType >= 0 ? "serviceType != ? OR " : ""),
                (nVendor >= 0 ? "vendor != ? OR " : ""),
                (hwVer > 0.0 ? "hwVer != ? OR hwVer is NULL OR " : ""),
                (swVer > 0.0 ? "swVer != ? OR swVer is NULL OR " : ""),
                (nLpPeriod > 0 ? "period != ? OR period is NULL OR " : "")
            );
        }
        stmt = m_SqliteHelper.compileStatement(queryBuffer);

        idx = 1;
        if(!IsNullValue(szSerial)) {
            stmt.bind(idx, szSerial);                       idx++;
        }
        stmt.bind(idx, (int)nParserType);                   idx++;
        if(nServiceType>=0) {
            stmt.bind(idx, nServiceType);                   idx++;
        }
        if(nVendor>=0) {
            stmt.bind(idx, nVendor);                        idx++;
        }
        if(hwVer> 0.0) {
            stmt.bind(idx, hwVer);                          idx++;
        }
        if(swVer> 0.0) {
            stmt.bind(idx, swVer);                          idx++;
        }
        if(nLpPeriod>0) {
            stmt.bind(idx, nLpPeriod);                      idx++;
        }
        stmt.bind(idx, (int)nModemId);                      idx++;
        stmt.bind(idx, nPortNum);                           idx++;
        if(!bInsert) { // Update
            if(!IsNullValue(szSerial)) {
                stmt.bind(idx, szSerial);                   idx++;
            }
            if(nServiceType>=0) {
                stmt.bind(idx, nServiceType);               idx++;
            }
            if(nVendor>=0) {
                stmt.bind(idx, nVendor);                    idx++;
            }
            if(hwVer> 0.0) {
                stmt.bind(idx, hwVer);                      idx++;
            }
            if(swVer> 0.0) {
                stmt.bind(idx, swVer);                      idx++;
            }
            if(nLpPeriod>0) {
                stmt.bind(idx, nLpPeriod);                  idx++;
            }
            stmt.bind(idx, (int)nParserType);               idx++;
        }

        stmt.execDML();
        stmt.finalize();

        TRANSACTION_COMMIT;
        Close();
        return TRUE;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "MeterTbl DB ERROR UPDATE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
	}

    return FALSE;
}

/** Meter Inventory Update.
 *
 * @param szModemId
 * @param nModemId      Modem Id (Foreign Key)
 * @param nPortNum      Port Number
 * @param szSerial      Meter Serial
 * @param nParserType   Parser Type
 * @param nVendor       Meter Vendor
 * @param hwVer
 * @param swVer
 * @param nLpPeriod     LP Period
 *
 */
BOOL CMeterHelper::Update(const char *szModemId, BYTE nPortNum, const char *szSerial, 
                const UINT nParserType, const int nServiceType, const int nVendor, const double hwVer, const double swVer,
                const int nLpPeriod)
{
    UINT nModemId = 0;
    if(szModemId == NULL) return FALSE;
    if(m_pModemHelper == NULL) return FALSE;
    if(!m_pModemHelper->GetId(szModemId, &nModemId)) return FALSE;

    return Update(nModemId, nPortNum, szSerial, nParserType, nServiceType, nVendor, hwVer, swVer, nLpPeriod);
}

/** Meter Inventory Update.
 *
 * @param pModemId
 * @param nModemId      Modem Id (Foreign Key)
 * @param nPortNum      Port Number
 * @param szSerial      Meter Serial
 * @param nParserType   Parser Type
 * @param nVendor       Meter Vendor
 * @param hwVer
 * @param swVer
 * @param nLpPeriod     LP Period
 *
 */
BOOL CMeterHelper::Update(const EUI64 *pModemId, BYTE nPortNum, const char *szSerial, 
                const UINT nParserType, const int nServiceType, const int nVendor, const double hwVer, const double swVer,
                const int nLpPeriod)
{
    UINT nModemId = 0;
    if(pModemId == NULL) return FALSE;
    if(m_pModemHelper == NULL) return FALSE;
    if(!m_pModemHelper->GetId(pModemId, &nModemId)) return FALSE;

    return Update(nModemId, nPortNum, szSerial, nParserType, nServiceType, nVendor, hwVer, swVer, nLpPeriod);
}

METERTBLINS* CMeterHelper::GetData(Row *pRow)
{
    if(pRow == NULL || pRow->pData == NULL) return NULL;
    return (METERTBLINS *)pRow->pData;
}

BOOL CMeterHelper::Select(DBFetchData *pResult)
{
    return Select((char *)NULL, -1, pResult);
}

BOOL CMeterHelper::Select(const EUI64 *pId, int nPortNum, DBFetchData *pResult)
{
    char szId[32]={0,};

    if(pId == NULL) {
        return Select((char *)NULL, -1, pResult);
    }

    EUI64ToStr(pId, szId);
    return Select(szId, nPortNum, pResult);
}

BOOL CMeterHelper::Select(const char *szId, int nPortNum, DBFetchData *pResult)
{
    CppSQLite3Query result;
    int idx=0;

    if(pResult == NULL) 
    {
        XDEBUG("MeterHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;

        if(szId == NULL) 
        {
            // 전체 Select
            stmt = m_SqliteHelper.compileStatement(
                SELECT_STMT ";" );
        } 
        else 
        {
            if(nPortNum < 0) {
                stmt = m_SqliteHelper.compileStatement(
                    SELECT_STMT
                        "AND mo.address = ? ; ");
            }else {
                stmt = m_SqliteHelper.compileStatement(
                    SELECT_STMT
                        "AND mo.address = ? AND me._portNum = ? ; ");
            }
            idx = 1;
            stmt.bind(idx, szId);           idx++;
            if(nPortNum >= 0) {
                stmt.bind(idx, nPortNum);   idx++;
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
		XDEBUG(ANSI_COLOR_RED "MeterTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CMeterHelper::TryUpload(DBFetchData *pResult)
{
    CppSQLite3Query result;

    if(pResult == NULL) 
    {
        XDEBUG("MeterHelper::TryUpload : NULL Parameter\r\n");
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
                "AND me.uploadTime is NULL ; ");

        result = stmt.execQuery();
        _Fetch(&result, pResult);
        stmt.finalize();

        stmt = m_SqliteHelper.compileStatement(
                "UPDATE MeterTbl "
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
		XDEBUG(ANSI_COLOR_RED "MeterTbl DB ERROR TryUpload: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}

BOOL CMeterHelper::UploadComplete(int nCount, UINT *pId)
{
    CppSQLite3Query result;
    int i;

    if(pId == NULL || nCount <= 0) 
    {
        XDEBUG("MeterHelper::UploadComplete : NULL Parameter\r\n");
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
                "UPDATE OR IGNORE MeterTbl "
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
		XDEBUG(ANSI_COLOR_RED "MeterTbl DB ERROR UploadComplete: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}

BOOL CMeterHelper::_Fetch(CppSQLite3Query *result, DBFetchData *pResult)
{
    METERTBLINS *pInstance;
    char * szRet;
    int idx=0;

    try
    {
        while(!result->eof())
        {
            pInstance = (METERTBLINS *)MALLOC(sizeof(METERTBLINS));
            memset(pInstance, 0, sizeof(METERTBLINS));

            idx = 0;
            pInstance->_id = result->getIntField(idx);                               idx++;
            pInstance->_modemId = result->getIntField(idx);                          idx++;
            if((szRet = (char *)result->getStringField(idx)) != NULL) 
            {
                strncpy(pInstance->szModemId, szRet, MIN(16, strlen(szRet)));
                StrToEUI64(pInstance->szModemId, &pInstance->modemId);
            }                                                                        idx++;
            pInstance->nPortNum = result->getIntField(idx);                          idx++;
            if((szRet = (char *)result->getStringField(idx)) != NULL) 
            {
                pInstance->szSerial = (char *)MALLOC(strlen(szRet)+1);
                memset(pInstance->szSerial, 0, strlen(szRet)+1);
                strcpy(pInstance->szSerial, szRet);
            }                                                                        idx++;
            if((szRet = (char *)result->getStringField(idx)) != NULL) 
            {
                pInstance->szModel = (char *)MALLOC(strlen(szRet)+1);
                memset(pInstance->szModel, 0, strlen(szRet)+1);
                strcpy(pInstance->szModel, szRet);
            }                                                                        idx++;
            pInstance->nParserType      = result->getIntField(idx);                  idx++;
            pInstance->nServiceType     = result->getIntField(idx);                  idx++;
            pInstance->nVendor          = result->getIntField(idx);                  idx++;
            /** Channle이 n개 이기 때문에 Meter Table에서 Current Pulse를 얻는 것은 불가능 하다 */
            //pInstance->nCurrentPulse    = result->getIntField(idx);                  idx++;
            pInstance->nEnergyLevel     = result->getIntField(idx);                  idx++;
            pInstance->nPeriod          = result->getIntField(idx);                  idx++;
            pInstance->hwVer            = result->getFloatField(idx);                idx++;
            pInstance->swVer            = result->getFloatField(idx);                idx++;
            ConvTimeStamp(result->getStringField(idx), &pInstance->lastMetering);    idx++;
            ConvTimeStamp(result->getStringField(idx), &pInstance->lastFailMetering);idx++;
            pInstance->nFailReason      = result->getIntField(idx);                  idx++;
            ConvTimeStamp(result->getStringField(idx), &pInstance->installTime);     idx++;
            ConvTimeStamp(result->getStringField(idx), &pInstance->lastEnergyLevel); idx++;
            pInstance->nTryUpload = result->getIntField(idx);                        idx++;
            if(!result->fieldIsNull(idx)) {
                pInstance->bUpload = TRUE;
                ConvTimeStamp(result->getStringField(idx), &pInstance->uploadTime);      
            }else{
                pInstance->bUpload = FALSE;
            }                                                                        idx++;
            ConvTimeStamp(result->getStringField(idx), &pInstance->lastUpdate);      idx++;

            AddResult(pResult, pInstance);

            result->nextRow();
        }
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "MeterTbl DB ERROR PATCH: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CMeterHelper::GetId(const EUI64 *pId, int nPortNum, UINT *nMeterId)
{
    char szId[32]={0,};

    if(pId == NULL) return FALSE;

    EUI64ToStr(pId, szId);
    return GetId(szId, nPortNum, nMeterId);
}

BOOL CMeterHelper::GetId(const char *szId, int nPortNum, UINT *nMeterId)
{
    DBFetchData fetchData;

    if(nMeterId == NULL || szId == NULL) return FALSE;
    memset(&fetchData, 0, sizeof(DBFetchData));

    if(Select(szId, nPortNum, &fetchData)) {
        if(fetchData.nCount>0) {
            METERTBLINS *instance;
            if((instance = GetData(GetHeadRow(&fetchData))) != NULL) {
                *nMeterId = instance->_id;
                FreeResult(&fetchData);
                return TRUE;
            }
        }
        FreeResult(&fetchData);
    } 
    return FALSE;
}

BOOL CMeterHelper::Delete()
{
    return Delete((char *)NULL, -1);
}

BOOL CMeterHelper::Delete(const EUI64 *pId, int nPortNum)
{
    char szId[32]={0,};

    if(pId == NULL) {
        return Delete((char *)NULL, -1);
    }

    EUI64ToStr(pId, szId);
    return Delete(szId, nPortNum);
}

/** Meter를 삭제한다.
 */
BOOL CMeterHelper::Delete(const char *szId, int nPortNum)
{
    int idx=0;

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;

        /** 관련된 ChannelCfg 들도 같이 지워준다. */
        if(m_pChannelCfgHelper != NULL) {
            m_pChannelCfgHelper->Delete(szId, nPortNum);
        }

        /** 관련된 Metering 도 같이 지워준다 */
        if(m_pMeteringHelper != NULL) {
            m_pMeteringHelper->Delete(szId, nPortNum);
        }

        /** 관련된 Log 도 같이 지워준다 */
        if(m_pLogHelper != NULL) {
            m_pLogHelper->Delete(szId, nPortNum);
        }

        if(szId == NULL) 
        {
            // 전체 Delete
            stmt = m_SqliteHelper.compileStatement("DELETE FROM MeterTbl; ");
        } 
        else 
        {
            if(nPortNum < 0) {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM MeterTbl "
                    "WHERE "
                        "_modemId IN "
                        "(SELECT _id FROM ModemTbl WHERE address = ? ) "
                    "; ");
            } else {
                stmt = m_SqliteHelper.compileStatement(
                    "DELETE FROM MeterTbl "
                    "WHERE "
                        "_modemId IN "
                        "(SELECT _id FROM ModemTbl WHERE address = ? ) "
                        "AND _portNum = ? "
                    "; ");
            }
            idx = 1;
            stmt.bind(idx, szId);           idx++;
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
		XDEBUG(ANSI_COLOR_RED "MeterTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CMeterHelper::InitializeDB()
{
    try
    {    
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS MeterTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "_modemId INTEGER NOT NULL, "
                "_portNum INTEGER NOT NULL, "    // Port Number
                "meterSerial, "                 // Meter Serial
                "parserType INTEGER NOT NULL, "
                "serviceType INTEGER NOT NULL, "
                "vendor INTEGER DEFAULT 0, "
                "period INTEGER, "
                "energyLevel INTEGER, "
                "hwVer REAL, "
                "swVer REAL, "
                "lastMetering DATETIME,  "
                "lastFailMetering DATETIME,  "
                "failReason INTEGER,  "
                "installTime DATETIME DEFAULT (DATETIME('now','localtime')), "
                "lastEnergyLevel DATETIME, "      // Update Time(DCU Time)
                "tryUpload INTEGER DEFAULT 0, "
                "uploadTime DATETIME, "
                "lastUpdate DATETIME NOT NULL,  "
                "FOREIGN KEY(_modemId) REFERENCES ModemTbl(_id), "
                "UNIQUE ( _modemId, _portNum ) "
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

/** TODO 관련된 Table이 같이 조절되도록 수정해 줘야 한다.
  */
BOOL CMeterHelper::UpgradeDB()
{
    try
    {
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS MeterTbl; ");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}


BOOL CMeterHelper::FreeData(void *pData)
{
    METERTBLINS * ins;
    if(pData == NULL) return FALSE;

    ins = (METERTBLINS *)pData;
    if(ins->szSerial != NULL) FREE(ins->szSerial);
    if(ins->szModel != NULL) FREE(ins->szModel);

    return TRUE;
}

BOOL CMeterHelper::DumpData(void *pData)
{
    METERTBLINS * ins;
    char timeStr[32];

    if(pData == NULL) return FALSE;
    ins = (METERTBLINS *)pData;

    if(ins->bUpload)
    {
        ConvTimeString(&ins->uploadTime, timeStr);
    }
    XDEBUG(" _id %d _modem %d [%s %d] serial [%s] parser 0x%02X vendor 0x%02X level %d hw %.2f sw %.2f period %d\r\n",
            ins->_id, ins->_modemId, ins->szModemId, ins->nPortNum,
            ins->szSerial ? ins->szSerial : "", 
            ins->nParserType, ins->nVendor, ins->nEnergyLevel, ins->hwVer, ins->swVer,
            ins->nPeriod);
    XDEBUG("     try %d upload %s\r\n",
            ins->nTryUpload, ins->bUpload ? timeStr : "NO");
    return TRUE;
}

