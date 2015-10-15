
#include "types/identification.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "MeterSecurityHelper.h"
#include "DbUtil.h"

//////////////////////////////////////////////////////////////////////
// CMeterSecurityHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS          "MeterSecurity"
#define DB_VERSION        1

#define SELECT_STMT         "SELECT "   \
                                "_id, type, name, key, lastUpdate "  \
                            "FROM MeterSecurityTbl "    

CMeterSecurityHelper   	*m_pMeterSecurityHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CMeterSecurityHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CMeterSecurityHelper::CMeterSecurityHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pMeterSecurityHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CMeterSecurityHelper::CMeterSecurityHelper() :
    CDatabaseHelper(DB_FILE_SECURITY, DB_CLASS, DB_VERSION)
{
	m_pMeterSecurityHelper	= this;
}

CMeterSecurityHelper::~CMeterSecurityHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterSecurityHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CMeterSecurityHelper::Initialize()
{
    return CDatabaseHelper::Initialize();
}

/** Security 정보 갱신.
 *
 * @param szType            Name type
 * @param szName            name
 * @param pKey              Security Key
 * @param nKeyLen           Security Key length
 *
 */
BOOL CMeterSecurityHelper::Update(const char *szType, const char *szName, const BYTE *pKey, int nKeyLen)
{
    int idx=1;

    if(szType == NULL || szName == NULL || pKey == NULL || nKeyLen <= 0) return FALSE;

    if(!CheckDB()) return FALSE;
    if(!Open()) return FALSE;

    try
    {
        CppSQLite3Query result;
        CppSQLite3Statement stmt;

        // Transaction
        TRANSACTION_BEGIN;

        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM MeterSecurityTbl WHERE type = ? AND name = ? ;");
        idx = 1;
        stmt.bind(idx, szType);                             idx++;
        stmt.bind(idx, szName);                             idx++;
        result = stmt.execQuery();
        if(result.eof()) {
		    XDEBUG(ANSI_COLOR_RED "MeterSecurityTbl DB ERROR UPDATE: UKNOWN ERROR\r\n" ANSI_NORMAL);
            // Transaction
            TRANSACTION_ROLLBACK; Close();
            return FALSE;
        }

        switch(result.getIntField(0))
        {
            case 0:
                /** Security Table에 해당 Security에 대한 정보가 없을 때 추가해 준다.
                  */
                {
                stmt = m_SqliteHelper.compileStatement("INSERT OR IGNORE INTO MeterSecurityTbl "
                        "(type, name, key, lastUpdate) VALUES ( upper(?), upper(?), ?, DATETIME('now','localtime') ); ");
                idx = 1;
                stmt.bind(idx, szType);                     idx++;
                stmt.bind(idx, szName);                     idx++;
                stmt.bind(idx, pKey, nKeyLen);              idx++;
                stmt.execDML();
                stmt.finalize();
                }
                break;
            default:
                /** Group Table이 이미 있을 경우에는 Update 한다. */
                {
                stmt = m_SqliteHelper.compileStatement("UPDATE OR IGNORE MeterSecurityTbl "
                        "SET  key = ?, lastUpdate =  DATETIME('now','localtime') "
                        "WHERE type = ? AND name = ? ");
                idx = 1;
                stmt.bind(idx, pKey, nKeyLen);              idx++;
                stmt.bind(idx, szType);                     idx++;
                stmt.bind(idx, szName);                     idx++;
                stmt.execDML();
                stmt.finalize();
                }
                break;
        }

        // Transaction
        TRANSACTION_COMMIT; Close();
        return TRUE;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "MeterSecurityTbl DB ERROR UPDATE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        // Transaction
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
	}

    return FALSE;
}

METERSECURITYDATAINS* CMeterSecurityHelper::GetData(Row *pRow)
{
    if(pRow == NULL || pRow->pData == NULL) return NULL;
    return (METERSECURITYDATAINS *)pRow->pData;
}

BOOL CMeterSecurityHelper::Select(DBFetchData *pResult)
{
    return Select(NULL, NULL, (char *)NULL, pResult);
}

BOOL CMeterSecurityHelper::Select(const char *szParserName, const char *szNodeKind, const EUI64 *pModemId, DBFetchData *pResult)
{
    char szId[32]={0,};

    if(pModemId != NULL)
    {
        EUI64ToStr(pModemId, szId);
    }
    return Select(szParserName, szNodeKind, pModemId != NULL ? szId : (char *)NULL, pResult);
}

/** Meter Security select.
 *
 * @param szType            Group type
 * @param szName            Group name
 * @param szModemId         Target Id
 * @param pResult
 *
 */
BOOL CMeterSecurityHelper::Select(const char *szParserName, const char *szNodeKind, const char *szModemId, DBFetchData *pResult)
{
    if(pResult == NULL) 
    {
        XDEBUG("MeterSecurityHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(szModemId != NULL && Select(SECURITY_TYPE_ID, szModemId, pResult) == TRUE) return TRUE;
    if(szNodeKind != NULL && Select(SECURITY_TYPE_NODE, szNodeKind, pResult) == TRUE) return TRUE;
    if(szParserName != NULL && Select(SECURITY_TYPE_PARSER, szParserName, pResult) == TRUE) return TRUE;

    return FALSE;
}

/** Meter Security select.
 *
 * @param szType            Security type
 * @param szName            name
 * @param pResult
 *
 */
BOOL CMeterSecurityHelper::Select(const char *szType, const char *szName, DBFetchData *pResult)
{
    CppSQLite3Query result;
    METERSECURITYDATAINS *pInstance = NULL;
    char * szRet;
    int idx=0, nBlobLen;
    BOOL bSelect = FALSE;

    if(pResult == NULL) 
    {
        XDEBUG("MeterSecurityHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;

        if(szType != NULL && szName != NULL)
        {
            stmt = m_SqliteHelper.compileStatement("SELECT "   
                                "_id, type, name, key, lastUpdate "  
                            "FROM MeterSecurityTbl "    
                            "WHERE type = upper(?) AND name = upper(?) "
                            "ORDER BY type DESC, name ");
            idx = 1;
            stmt.bind(idx, szType);                     idx++;
            stmt.bind(idx, szName);                     idx++;
        }
        else if(szType != NULL)
        {
            stmt = m_SqliteHelper.compileStatement("SELECT "   
                                "_id, type, name, key, lastUpdate "  
                            "FROM MeterSecurityTbl "    
                            "WHERE type = upper(?) "
                            "ORDER BY type DESC, name ");
            idx = 1;
            stmt.bind(idx, szType);                     idx++;
        }
        else
        {
            stmt = m_SqliteHelper.compileStatement("SELECT "   
                                "_id, type, name, key, lastUpdate "  
                            "FROM MeterSecurityTbl " 
                            "ORDER BY type DESC, name ");
        }

        result = stmt.execQuery();
        while(!result.eof())
        {
            idx = 0;
            bSelect = TRUE;
            pInstance = (METERSECURITYDATAINS *)MALLOC(sizeof(METERSECURITYDATAINS));
            memset(pInstance, 0, sizeof(METERSECURITYDATAINS));

            pInstance->_id = result.getIntField(idx);                       idx++;

            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                strncpy(pInstance->szType, szRet, MIN(sizeof(pInstance->szType)-1, strlen(szRet)));
            }                                                                   idx++; //*
            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                strncpy(pInstance->szName, szRet, MIN(sizeof(pInstance->szName)-1, strlen(szRet)));
            }                                                                   idx++; //*

            nBlobLen = 0; szRet = (char *)result.getBlobField(idx, nBlobLen);           idx++;
            if(nBlobLen > 0)
            {
                pInstance->pKey = (unsigned char *)MALLOC(nBlobLen);
                memcpy(pInstance->pKey, szRet, nBlobLen);
                pInstance->nKeyLen = nBlobLen;
            }

            AddResult(pResult, pInstance);

            result.nextRow();
        }

        stmt.finalize();
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "MeterSecurityTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return bSelect;
}

BOOL CMeterSecurityHelper::Delete()
{
    return Delete(NULL, NULL);
}

BOOL CMeterSecurityHelper::Delete(const char *szType, const char *szName)
{
    int idx;

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        char queryBuffer[1024];

        sprintf(queryBuffer, "%s", 
                "DELETE FROM MeterSecurityTbl ");

        if(!IsNullValue(szType))
        {
            strcat(queryBuffer, "WHERE type = upper(?) ");
            if(!IsNullValue(szName))
            {
                strcat(queryBuffer, "AND name = upper(?) ");
            }
        }

        TRANSACTION_BEGIN;

        stmt = m_SqliteHelper.compileStatement(queryBuffer);

        idx = 1;
        if(!IsNullValue(szType))
        {
            stmt.bind(idx, szType);                 idx++;
            if(!IsNullValue(szName))
            {
             stmt.bind(idx, szName);                 idx++;
            }
        }

        stmt.execDML();
        stmt.finalize();

        TRANSACTION_COMMIT;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "MeterSecurityTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CMeterSecurityHelper::InitializeDB()
{
    try
    {    
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS MeterSecurityTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "type TEXT NOT NULL, "
                "name TEXT NOT NULL, "
                "key NOT NULL, "    // BLOB
                "lastUpdate DATETIME NOT NULL,  "
                "UNIQUE ( type, name ) "
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

BOOL CMeterSecurityHelper::UpgradeDB()
{
    try
    {
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS MeterSecurityTbl; ");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}

BOOL CMeterSecurityHelper::FreeData(void *pData)
{
    METERSECURITYDATAINS * ins;

    if(pData == NULL) return FALSE;

    ins = (METERSECURITYDATAINS *)pData;

    if(ins->pKey != NULL)
    {
        FREE(ins->pKey);
        ins->pKey = NULL;
    }

    return TRUE;
}

BOOL CMeterSecurityHelper::DumpData(void *pData)
{
    METERSECURITYDATAINS *ins;
    char timeStr[32];

    if(pData == NULL) return FALSE;
    ins = (METERSECURITYDATAINS *)pData;

    ConvTimeString(&ins->lastUpdate, timeStr);
    XDEBUG(" _id %d type [%s] name [%s] %s key ",
            ins->_id, ins->szType, ins->szName, timeStr);
    if(ins->pKey != NULL)
    {
        XDUMP((char *)ins->pKey, ins->nKeyLen);
    }

    return TRUE;
}

