
#include "types/identification.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "MeterPropertyHelper.h"
#include "DbUtil.h"

//////////////////////////////////////////////////////////////////////
// CMeterPropertyHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS          "MeterProperty"
#define DB_VERSION        1

#define SELECT_STMT         "SELECT "   \
                                "_id, grp, property, key, value, lastUpdate "  \
                            "FROM MeterPropertyTbl "    

CMeterPropertyHelper   	*m_pMeterPropertyHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CMeterPropertyHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CMeterPropertyHelper::CMeterPropertyHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pMeterPropertyHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CMeterPropertyHelper::CMeterPropertyHelper() :
    CDatabaseHelper(DB_FILE_PROPERTY, DB_CLASS, DB_VERSION)
{
	m_pMeterPropertyHelper	= this;
}

CMeterPropertyHelper::~CMeterPropertyHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterPropertyHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CMeterPropertyHelper::Initialize()
{
    return CDatabaseHelper::Initialize();
}

/** Security 정보 갱신.
 *
 *
 */
BOOL CMeterPropertyHelper::Update(const char *szGroup, const char *szProperty, 
        const char *szType, const char *szKey, const BYTE *pValue, int nValueLen)
{
    int idx=1;

    if(IsNullValue(szGroup) || IsNullValue(szProperty) || IsNullValue(szType) || IsNullValue(szKey) 
            || pValue == NULL || nValueLen <= 0) return FALSE;

    if(!CheckDB()) return FALSE;
    if(!Open()) return FALSE;

    try
    {
        CppSQLite3Query result;
        CppSQLite3Statement stmt;

        // Transaction
        TRANSACTION_BEGIN;

        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM MeterPropertyTbl "
                "WHERE grp = upper(?) AND property = upper(?) AND type = upper(?) AND key = upper(?) ;");
        idx = 1;
        stmt.bind(idx, szGroup);                            idx++;
        stmt.bind(idx, szProperty);                         idx++;
        stmt.bind(idx, szType);                             idx++;
        stmt.bind(idx, szKey);                              idx++;
        result = stmt.execQuery();
        if(result.eof()) {
		    XDEBUG(ANSI_COLOR_RED "MeterPropertyTbl DB ERROR UPDATE: UKNOWN ERROR\r\n" ANSI_NORMAL);
            // Transaction
            TRANSACTION_ROLLBACK; Close();
            return FALSE;
        }

        switch(result.getIntField(0))
        {
            case 0:
                /** Property Table에 해당 Property에 대한 정보가 없을 때 추가해 준다.
                  */
                {
                stmt = m_SqliteHelper.compileStatement("INSERT OR IGNORE INTO MeterPropertyTbl "
                        "(grp, property, type, key, value, lastUpdate) "
                        "VALUES ( upper(?), upper(?), upper(?), upper(?), ?, DATETIME('now','localtime') ); ");
                idx = 1;
                stmt.bind(idx, szGroup);                    idx++;
                stmt.bind(idx, szProperty);                 idx++;
                stmt.bind(idx, szType);                     idx++;
                stmt.bind(idx, szKey);                      idx++;
                stmt.bind(idx, pValue, nValueLen);          idx++;
                stmt.execDML();
                stmt.finalize();
                }
                break;
            default:
                /** Group Table이 이미 있을 경우에는 Update 한다. */
                {
                stmt = m_SqliteHelper.compileStatement("UPDATE OR IGNORE MeterPropertyTbl "
                        "SET  value = ?, lastUpdate =  DATETIME('now','localtime') "
                        "WHERE grp = upper(?) AND property = upper(?) AND type = upper(?) AND key = upper(?) ");
                idx = 1;
                stmt.bind(idx, pValue, nValueLen);          idx++;
                stmt.bind(idx, szGroup);                    idx++;
                stmt.bind(idx, szProperty);                 idx++;
                stmt.bind(idx, szType);                     idx++;
                stmt.bind(idx, szKey);                      idx++;
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
		XDEBUG(ANSI_COLOR_RED "MeterPropertyTbl DB ERROR UPDATE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        // Transaction
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
	}

    return FALSE;
}

METERPROPERTYDATAINS* CMeterPropertyHelper::GetData(Row *pRow)
{
    if(pRow == NULL || pRow->pData == NULL) return NULL;
    return (METERPROPERTYDATAINS *)pRow->pData;
}

BOOL CMeterPropertyHelper::Select(DBFetchData *pResult)
{
    //BOOL    Select(const char *szGroup, const char *szProperty, const char *szType, const char *szKey, DBFetchData *pResult);
    return Select(NULL, NULL, NULL, NULL, pResult);
}

BOOL CMeterPropertyHelper::Select(const char *szGroup, const char *szProperty, const char *szParserName, const char *szNodeKind, const EUI64 *pModemId, DBFetchData *pResult)
{
    char szId[32]={0,};

    if(pModemId != NULL)
    {
        EUI64ToStr(pModemId, szId);
    }
    return Select(szGroup, szProperty, szParserName, szNodeKind, pModemId != NULL ? szId : (char *)NULL, pResult);
}

BOOL CMeterPropertyHelper::SelectAll(const char *szGroup, const char *szProperty, const char *szParserName, const char *szNodeKind, const EUI64 *pModemId, DBFetchData *pResult)
{
    char szId[32]={0,};

    if(pModemId != NULL)
    {
        EUI64ToStr(pModemId, szId);
    }
    return SelectAll(szGroup, szProperty, szParserName, szNodeKind, pModemId != NULL ? szId : (char *)NULL, pResult);
}

/** Meter Property select.
 *
 * @param pResult
 *
 */
BOOL CMeterPropertyHelper::Select(const char *szGroup, const char *szProperty, const char *szParserName, const char *szNodeKind, const char *szModemId, DBFetchData *pResult)
{
    if(pResult == NULL) 
    {
        XDEBUG("MeterPropertyHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(szModemId != NULL && Select(szGroup, szProperty, PROPERTY_TYPE_ID, szModemId, pResult) == TRUE) return TRUE;
    if(szNodeKind != NULL && Select(szGroup, szProperty, PROPERTY_TYPE_NODE, szNodeKind, pResult) == TRUE) return TRUE;
    if(szParserName != NULL && Select(szGroup, szProperty, PROPERTY_TYPE_PARSER, szParserName, pResult) == TRUE) return TRUE;

    return FALSE;
}

/** Meter Property select all.
 *
 * @param pResult
 *
 */
BOOL CMeterPropertyHelper::SelectAll(const char *szGroup, const char *szProperty, const char *szParserName, const char *szNodeKind, const char *szModemId, DBFetchData *pResult)
{
    BOOL res = FALSE;

    if(pResult == NULL) 
    {
        XDEBUG("MeterPropertyHelper::SelectAll : NULL Parameter\r\n");
        return FALSE;
    }

    if(szModemId != NULL && Select(szGroup, szProperty, PROPERTY_TYPE_ID, szModemId, pResult) == TRUE) res = TRUE;
    if(szNodeKind != NULL && Select(szGroup, szProperty, PROPERTY_TYPE_NODE, szNodeKind, pResult) == TRUE) res = TRUE;
    if(szParserName != NULL && Select(szGroup, szProperty, PROPERTY_TYPE_PARSER, szParserName, pResult) == TRUE) res = TRUE;

    return res;
}

/** Meter Property select.
 *
 * @param pResult
 *
 */
BOOL CMeterPropertyHelper::Select(const char *szGroup, const char *szProperty, const char *szType, const char *szKey, DBFetchData *pResult)
{
    CppSQLite3Query result;
    METERPROPERTYDATAINS *pInstance = NULL;
    char * szRet;
    int idx=1, nBlobLen;
    BOOL bSelect = FALSE;
    char queryBuffer[1024];

    if(pResult == NULL) 
    {
        XDEBUG("MeterPropertyHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;
    if(!Open()) return FALSE;

    try
    {
        memset(queryBuffer, 0, sizeof(queryBuffer));
        strcat(queryBuffer, "SELECT _id, grp, property, type, key, value, lastUpdate FROM MeterPropertyTbl ");
        if(IsNullValue(szGroup) == FALSE || IsNullValue(szProperty) == FALSE || IsNullValue(szType) == FALSE || IsNullValue(szKey) == FALSE)
        {
            strcat(queryBuffer, "WHERE ");
            if(!IsNullValue(szGroup))
            {
                if(idx > 1) strcat(queryBuffer, "AND ");
                strcat(queryBuffer, "grp = upper(?) ");
                idx++;
            }
            if(!IsNullValue(szProperty))
            {
                if(idx > 1) strcat(queryBuffer, "AND ");
                strcat(queryBuffer, "property = upper(?) ");
                idx++;
            }
            if(!IsNullValue(szType))
            {
                if(idx > 1) strcat(queryBuffer, "AND ");
                strcat(queryBuffer, "type = upper(?) ");
                idx++;
            }
            if(!IsNullValue(szKey))
            {
                if(idx > 1) strcat(queryBuffer, "AND ");
                strcat(queryBuffer, "key = upper(?) ");
                idx++;
            }
        }
        strcat(queryBuffer, "ORDER BY type DESC, grp, type, key, property ");

        //XDEBUG("%s\r\n", queryBuffer);
        CppSQLite3Statement stmt = m_SqliteHelper.compileStatement(queryBuffer);

        idx = 1;
        if(IsNullValue(szGroup) == FALSE || IsNullValue(szProperty) == FALSE || IsNullValue(szType) == FALSE || IsNullValue(szKey) == FALSE)
        {
            if(!IsNullValue(szGroup))
            {
                stmt.bind(idx, szGroup);        idx++;
            }
            if(!IsNullValue(szProperty))
            {
                stmt.bind(idx, szProperty);     idx++;
            }
            if(!IsNullValue(szType))
            {
                stmt.bind(idx, szType);         idx++;
            }
            if(!IsNullValue(szKey))
            {
                stmt.bind(idx, szKey);          idx++;
            }
        }

        result = stmt.execQuery();
        while(!result.eof())
        {
            idx = 0;
            bSelect = TRUE;
            pInstance = (METERPROPERTYDATAINS *)MALLOC(sizeof(METERPROPERTYDATAINS));
            memset(pInstance, 0, sizeof(METERPROPERTYDATAINS));

            pInstance->_id = result.getIntField(idx);                           idx++;

            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                strncpy(pInstance->szGroup, szRet, MIN(sizeof(pInstance->szGroup)-1, strlen(szRet)));
            }                                                                   idx++; //*
            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                strncpy(pInstance->szProperty, szRet, MIN(sizeof(pInstance->szProperty)-1, strlen(szRet)));
            }                                                                   idx++; //*
            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                strncpy(pInstance->szType, szRet, MIN(sizeof(pInstance->szType)-1, strlen(szRet)));
            }                                                                   idx++; //*
            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                strncpy(pInstance->szKey, szRet, MIN(sizeof(pInstance->szKey)-1, strlen(szRet)));
            }                                                                   idx++; //*

            nBlobLen = 0; szRet = (char *)result.getBlobField(idx, nBlobLen);   idx++;
            if(nBlobLen > 0)
            {
                pInstance->pValue = (unsigned char *)MALLOC(nBlobLen+1);
                memcpy(pInstance->pValue, szRet, nBlobLen);
                pInstance->pValue[nBlobLen] = 0;
                pInstance->nValueLen = nBlobLen;
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
		XDEBUG(ANSI_COLOR_RED "MeterPropertyTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return bSelect;
}

BOOL CMeterPropertyHelper::Delete()
{
    return Delete(NULL, NULL, NULL, NULL);
}

BOOL CMeterPropertyHelper::Delete(const char *szGroup, const char *szProperty)
{
    return Delete(szGroup, szProperty, NULL, NULL);
}

BOOL CMeterPropertyHelper::Delete(const char *szGroup, const char *szProperty, const char *szType, const char *szKey)
{
    int idx = 1;

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        char queryBuffer[1024];

        sprintf(queryBuffer, "%s", 
                "DELETE FROM MeterPropertyTbl ");

        idx = 1;
        if(!IsNullValue(szGroup)) 
        {
            if(idx == 1) strcat(queryBuffer, "WHERE ");
            else strcat(queryBuffer, "AND ");
            strcat(queryBuffer, "grp = upper(?) ");
            idx ++;
        }

        if(!IsNullValue(szProperty)) 
        {
            if(idx == 1) strcat(queryBuffer, "WHERE ");
            else strcat(queryBuffer, "AND ");
            strcat(queryBuffer, "property = upper(?) ");
            idx ++;
        }

        if(!IsNullValue(szType)) 
        {
            if(idx == 1) strcat(queryBuffer, "WHERE ");
            else strcat(queryBuffer, "AND ");
            strcat(queryBuffer, "type = upper(?) ");
            idx ++;
        }

        if(!IsNullValue(szKey)) 
        {
            if(idx == 1) strcat(queryBuffer, "WHERE ");
            else strcat(queryBuffer, "AND ");
            strcat(queryBuffer, "key = upper(?) ");
            idx ++;
        }

        TRANSACTION_BEGIN;

        stmt = m_SqliteHelper.compileStatement(queryBuffer);

        idx = 1;
        if(!IsNullValue(szGroup)) 
        {
            stmt.bind(idx, szGroup);            idx++;
        }
        if(!IsNullValue(szProperty)) 
        {
            stmt.bind(idx, szProperty);         idx++;
        }
        if(!IsNullValue(szType)) 
        {
            stmt.bind(idx, szType);             idx++;
        }
        if(!IsNullValue(szKey)) 
        {
            stmt.bind(idx, szKey);              idx++;
        }

        stmt.execDML();
        stmt.finalize();

        TRANSACTION_COMMIT;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "MeterPropertyTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CMeterPropertyHelper::InitializeDB()
{
    try
    {    
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS MeterPropertyTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "grp TEXT NOT NULL, "
                "property TEXT NOT NULL, "
                "type TEXT NOT NULL, "
                "key TEXT NOT NULL, "
                "value NOT NULL, "    // BLOB
                "lastUpdate DATETIME NOT NULL,  "
                "UNIQUE ( grp, property, type, key ) "
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

BOOL CMeterPropertyHelper::UpgradeDB()
{
    try
    {
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS MeterPropertyTbl; ");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}

BOOL CMeterPropertyHelper::FreeData(void *pData)
{
    METERPROPERTYDATAINS * ins;

    if(pData == NULL) return FALSE;

    ins = (METERPROPERTYDATAINS *)pData;

    if(ins->pValue != NULL)
    {
        FREE(ins->pValue);
        ins->pValue = NULL;
    }

    return TRUE;
}

BOOL CMeterPropertyHelper::DumpData(void *pData)
{
    METERPROPERTYDATAINS *ins;
    char timeStr[32];

    if(pData == NULL) return FALSE;
    ins = (METERPROPERTYDATAINS *)pData;

    ConvTimeString(&ins->lastUpdate, timeStr);
    XDEBUG(" _id [%d] grp [%s] property [%s] type [%s] key [%s] [%s] ",
            ins->_id, ins->szGroup, ins->szProperty, ins->szType, ins->szKey, timeStr);
    if(ins->pValue != NULL)
    {
        XDUMP((char *)ins->pValue, ins->nValueLen);
    }

    return TRUE;
}

