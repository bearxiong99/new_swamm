
#include "types/identification.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "if4api.h"

#include "ModemHelper.h"
#include "MeterHelper.h"
#include "DbUtil.h"

//////////////////////////////////////////////////////////////////////
// CModemHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS    "Modem"
#define DB_VERSION  1

CModemHelper   	*m_pModemHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CModemHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CModemHelper::CModemHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pModemHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CModemHelper::CModemHelper() :
    CDatabaseHelper(DB_FILE_AGENT, DB_CLASS, DB_VERSION)
{
	m_pModemHelper	= this;
}

CModemHelper::~CModemHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CModemHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CModemHelper::Initialize()
{
    return CDatabaseHelper::Initialize();
}

BOOL CModemHelper::Update(const EUI64 *pId, const char *szModel, const double hwVer, const double swVer, const int nBuild) 
{
    char szId[32]={0,};

    if(pId == NULL) return FALSE;

    EUI64ToStr(pId, szId);
    return Update(szId, szModel, hwVer, swVer, nBuild);
}

/** Modem Inventory Update.
 *
 * @param szModel       Modem Model Name
 * @param pid           Modem ID
 * @param hwVer
 * @param swVer
 * @param nBuild        Software Build Number
 *
 */
BOOL CModemHelper::Update(const char *szId, const char *szModel, const double hwVer, const double swVer, const int nBuild) 
{
    int idx=1;
    BOOL bInsert = FALSE;

    if(!CheckDB()) return FALSE;
    if(szId == NULL) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Query result;
        CppSQLite3Statement stmt;
 
        TRANSACTION_BEGIN;
        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM ModemTbl WHERE address = ? ;");
        idx = 1;
        stmt.bind(idx, szId);                               idx++;
        result = stmt.execQuery();
        if(result.eof()) {
            bInsert = TRUE;
        } else {
            switch(result.getIntField(0)) 
            {
                case 0: bInsert = TRUE; break;
                case 1: break;
                default:
		                XDEBUG(ANSI_COLOR_RED "DB ERROR: Too many Modem instances (%d)\r\n" ANSI_NORMAL, result.getIntField(0));
                        TRANSACTION_ROLLBACK;
                        Close();
                        return FALSE;
            }
        }

        stmt.finalize();
        if(bInsert) {
            stmt = m_SqliteHelper.compileStatement(
                "INSERT INTO ModemTbl "
                    "( model, hwVer, swVer, swBuild, address, lastUpdate ) "
                "VALUES "
                    "( ?,     ?,     ?,     ?,       ?,        DATETIME('now','localtime')); ");
        } else {
            stmt = m_SqliteHelper.compileStatement(
                "UPDATE ModemTbl "
                    "SET model = ? , hwVer = ? , swVer = ? , swBuild = ? , lastUpdate = DATETIME('now','localtime') "
                "WHERE "
                    "address = ? "
                " AND ( model != ? OR hwVer != ? OR hwVer IS NULL OR swVer != ? OR swVer IS NULL OR swBuild != ? OR swBuild IS NULL ); ");
        }

        idx = 1;
        stmt.bind(idx, szModel);                            idx++;
        stmt.bind(idx, hwVer);                              idx++;
        stmt.bind(idx, swVer);                              idx++;
        stmt.bind(idx, nBuild);                             idx++;
        stmt.bind(idx, szId);                               idx++;
        if(!bInsert) { // Update
            stmt.bind(idx, szModel);                        idx++;
            stmt.bind(idx, hwVer);                          idx++;
            stmt.bind(idx, swVer);                          idx++;
            stmt.bind(idx, nBuild);                         idx++;
        }

        stmt.execDML();
        stmt.finalize();

        TRANSACTION_COMMIT;
        Close();
        return TRUE;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "ModemTbl DB ERROR UPDATE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        TRANSACTION_ROLLBACK;
        Close();
	}

    return FALSE;
}

MODEMTBLINS* CModemHelper::GetData(Row *pRow)
{
    if(pRow == NULL || pRow->pData == NULL) return NULL;
    return (MODEMTBLINS *)pRow->pData;
}


BOOL CModemHelper::Select(DBFetchData *pResult)
{
    return Select((char *)NULL, pResult);
}

BOOL CModemHelper::Select(const EUI64 *pId, DBFetchData *pResult)
{
    char szId[32]={0,};

    if(pId == NULL) {
        return Select((char *)NULL, pResult);
    }

    EUI64ToStr(pId, szId);
    return Select(szId, pResult);
}

BOOL CModemHelper::Select(const char *szId, DBFetchData *pResult)
{
    CppSQLite3Query result;
    MODEMTBLINS *pInstance;
    char * szRet;
    int idx=0;

    if(pResult == NULL) 
    {
        XDEBUG("ModemHelper::Select : NULL Parameter\r\n");
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
                "SELECT "
                    "_id, address, model, "
                    "hwVer, swVer, swBuild,  "
                    "installTime, lastUpdate "
                "FROM ModemTbl ; ");
        } 
        else 
        {
            stmt = m_SqliteHelper.compileStatement(
                "SELECT "
                    "_id, address, model, "
                    "hwVer, swVer, swBuild,  "
                    "installTime, lastUpdate "
                "FROM ModemTbl "
                "WHERE "
                    "address = ? ; ");
            idx = 1;
            stmt.bind(idx, szId);       idx++;
        }

        idx = 0;
        result = stmt.execQuery();
        Close();
        while(!result.eof())
        {
            pInstance = (MODEMTBLINS *)MALLOC(sizeof(MODEMTBLINS));
            memset(pInstance, 0, sizeof(MODEMTBLINS));

            idx = 0;
            pInstance->_id = result.getIntField(idx);                           idx++;
            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                pInstance->szModel = (char *)MALLOC(strlen(szRet)+1);
                memset(pInstance->szModel, 0, strlen(szRet)+1);
                strcpy(pInstance->szModel, szRet);
            }
                                                                                idx++;
            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                strncpy(pInstance->szId, szRet, MIN(16, strlen(szRet)));
                StrToEUI64(szRet, &pInstance->id);
            }
                                                                                idx++;
            pInstance->hwVer = result.getFloatField(idx);                       idx++;
            pInstance->swVer = result.getFloatField(idx);                       idx++;
            pInstance->nBuild = result.getIntField(idx);                        idx++;

            ConvTimeStamp(result.getStringField(idx), &pInstance->installTime); idx++;
            ConvTimeStamp(result.getStringField(idx), &pInstance->lastUpdate);  idx++;

            AddResult(pResult, pInstance);

            result.nextRow();
        }
        stmt.finalize();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "ModemTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CModemHelper::GetId(const EUI64 *pId, UINT *nModemId)
{
    char szId[32]={0,};

    if(pId == NULL) return FALSE;

    EUI64ToStr(pId, szId);
    return GetId(szId, nModemId);
}

BOOL CModemHelper::GetId(const char *szId, UINT *nModemId)
{
    DBFetchData fetchData;

    if(nModemId == NULL || szId == NULL) return FALSE;
    memset(&fetchData, 0, sizeof(DBFetchData));

    if(Select(szId, &fetchData)) {
        if(fetchData.nCount>0) {
            MODEMTBLINS *instance;
            if((instance = GetData(GetHeadRow(&fetchData))) != NULL) {
                *nModemId = instance->_id;
                FreeResult(&fetchData);
                return TRUE;
            }
        }
        FreeResult(&fetchData);
    } 
    return FALSE;
}

BOOL CModemHelper::Delete()
{
    return Delete((char *)NULL);
}

BOOL CModemHelper::Delete(const EUI64 *pId)
{
    char szId[32]={0,};

    if(pId == NULL) {
        return Delete((char *)NULL);
    }

    EUI64ToStr(pId, szId);
    return Delete(szId);
}

BOOL CModemHelper::Delete(const char *szId)
{
    int idx=0;

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;

        TRANSACTION_BEGIN;

        /** 관련된 Meter 들도 같이 지워준다. */
        if(m_pMeterHelper != NULL) {
            m_pMeterHelper->Delete(szId, -1);
        }

        if(szId == NULL) 
        {
            // 전체 Delete
            stmt = m_SqliteHelper.compileStatement("DELETE FROM ModemTbl; ");
        } 
        else 
        {
            stmt = m_SqliteHelper.compileStatement(
                "DELETE FROM ModemTbl "
                "WHERE "
                    "address = ? ; ");
            idx = 1;
            stmt.bind(idx, szId);       idx++;
        }

        stmt.execDML();
        stmt.finalize();
        TRANSACTION_COMMIT; Close();
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "ModemTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CModemHelper::InitializeDB()
{
    try
    {
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS ModemTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "address NOT NULL, "
                "model TEXT NOT NULL, "
                "hwVer REAL, "
                "swVer REAL, "
                "swBuild INTEGER, "
                "installTime DATETIME DEFAULT (DATETIME('now','localtime')), "
                "lastUpdate DATETIME NOT NULL,  "
                "UNIQUE ( address ) "
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

BOOL CModemHelper::UpgradeDB()
{
    try
    {
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS ModemTbl; ");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}


BOOL CModemHelper::FreeData(void *pData)
{
    MODEMTBLINS * ins;
    if(pData == NULL) return FALSE;

    ins = (MODEMTBLINS *)pData;
    if(ins->szModel != NULL) FREE(ins->szModel);

    return TRUE;
}

BOOL CModemHelper::DumpData(void *pData)
{
    MODEMTBLINS * ins;
    char timeStr1[32], timeStr2[32];

    if(pData == NULL) return FALSE;
    ins = (MODEMTBLINS *)pData;

    ConvTimeString(&ins->installTime, timeStr1);
    ConvTimeString(&ins->lastUpdate, timeStr2);

    XDEBUG(" _id %d _modem %d [%s] hw %f sw %f build %d install [%s] last [%s]\r\n",
            ins->_id, ins->szModel ? ins->szModel : "" , ins->szId, 
            ins->hwVer, ins->swVer, ins->nBuild,
            timeStr1, timeStr2);
    return TRUE;
}

