
#include "types/identification.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "GroupHelper.h"
#include "DbUtil.h"

//////////////////////////////////////////////////////////////////////
// CGroupHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS          "Group"
#define DB_VERSION        1

#define SELECT_STMT         "SELECT "   \
                                "g._id, g.type, g.name, g.lastUpdate, "  \
                                "e._id, e.targetId, e.lastUpdate " \
                            "FROM GroupTbl g, GroupEntryTbl e "    \
                            "WHERE "    \
                                "g._id = e._groupId " 

CGroupHelper   	*m_pGroupHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CGroupHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CGroupHelper::CGroupHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pGroupHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CGroupHelper::CGroupHelper() :
    CDatabaseHelper(DB_FILE_GROUP, DB_CLASS, DB_VERSION)
{
	m_pGroupHelper	= this;
}

CGroupHelper::~CGroupHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CGroupHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CGroupHelper::Initialize()
{
    return CDatabaseHelper::Initialize();
}

BOOL CGroupHelper::Update(const char *szType, const char *szName, const EUI64 *pModemId)
{
    char szId[32]={0,};

    if(szType == NULL || szName == NULL || pModemId == NULL) return FALSE;
    EUI64ToStr(pModemId, szId);
    return Update(szType, szName, szId);
}

/** Group 정보 갱신.
 *
 * @param szType            Group type
 * @param szName            Group name
 * @param szModemId         Target Id
 *
 */
BOOL CGroupHelper::Update(const char *szType, const char *szName, const char *szModemId)
{
    int idx=1;

    if(szType == NULL || szName == NULL || szModemId == NULL) return FALSE;

    if(!CheckDB()) return FALSE;
    if(!Open()) return FALSE;

    try
    {
        CppSQLite3Query result;
        CppSQLite3Statement stmt;

        // Transaction
        TRANSACTION_BEGIN;

        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM GroupTbl WHERE type = ? AND name = ? ;");
        idx = 1;
        stmt.bind(idx, szType);                             idx++;
        stmt.bind(idx, szName);                             idx++;
        result = stmt.execQuery();
        if(result.eof()) {
		    XDEBUG(ANSI_COLOR_RED "GroupTbl DB ERROR UPDATE: UKNOWN ERROR\r\n" ANSI_NORMAL);
            // Transaction
            TRANSACTION_ROLLBACK; Close();
            return FALSE;
        }
        //XDEBUG("result.getIntField(0) %d\r\n", result.getIntField(0));
        switch(result.getIntField(0))
        {
            case 0:
                /** Group Table에 해당 Group에 대한 정보가 없을 때 추가해 준다.
                  */
                {
                stmt = m_SqliteHelper.compileStatement("INSERT OR IGNORE INTO GroupTbl "
                        "(type, name, lastUpdate) VALUES ( upper(?), upper(?), DATETIME('now','localtime') ); ");
                idx = 1;
                stmt.bind(idx, szType);                     idx++;
                stmt.bind(idx, szName);                     idx++;
                stmt.execDML();
                stmt.finalize();
                }
                break;
            default:
                /** Group Table이 이미 있을 경우에는 아무것도 하지 않는다 */
                break;
        }
        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM GroupTbl g, GroupEntryTbl e "
                "WHERE g.type = upper(?) AND g._id = e._groupId AND e.targetId = upper(?) ;");
        idx = 1;
        stmt.bind(idx, szType);                             idx++;
        stmt.bind(idx, szModemId);                          idx++;
        result = stmt.execQuery();
        if(result.getIntField(0) > 0)
        {
            /** 동일 Type에는 같은 targetId가 들어갈 수 없다 */
            stmt.finalize();
            TRANSACTION_ROLLBACK; Close();
            return FALSE;
        }
        stmt.finalize();

        stmt = m_SqliteHelper.compileStatement(
            "INSERT OR IGNORE INTO GroupEntryTbl "
                "( targetId, _groupId, lastUpdate ) "
            "VALUES "
                "( upper(?), (SELECT _id FROM GroupTbl WHERE type = upper(?) AND name = upper(?) LIMIT 1), "
                "DATETIME('now','localtime')); ");

        idx = 1;
        stmt.bind(idx, szModemId);                          idx++;
        stmt.bind(idx, szType);                             idx++;
        stmt.bind(idx, szName);                             idx++;

        stmt.execDML();
        stmt.finalize();

        // Transaction
        TRANSACTION_COMMIT; Close();
        return TRUE;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "GroupTbl DB ERROR UPDATE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        // Transaction
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
	}

    return FALSE;
}

GROUPDATAINS* CGroupHelper::GetData(Row *pRow)
{
    if(pRow == NULL || pRow->pData == NULL) return NULL;
    return (GROUPDATAINS *)pRow->pData;
}

BOOL CGroupHelper::Select(DBFetchData *pResult)
{
    return Select(NULL, NULL, (char *)NULL, pResult);
}

BOOL CGroupHelper::Select(const char *szType, DBFetchData *pResult)
{
    return Select(szType, NULL, (char *)NULL, pResult);
}

BOOL CGroupHelper::Select(const char *szType, const char *szName, DBFetchData *pResult)
{
    return Select(szType, szName, (char *)NULL, pResult);
}

BOOL CGroupHelper::Select(const char *szType, const char *szName, const EUI64 *pModemId, DBFetchData *pResult)
{
    char szId[32]={0,};

    if(pModemId != NULL)
    {
        EUI64ToStr(pModemId, szId);
    }
    return Select(szType, szName, pModemId != NULL ? szId : (char *)NULL, pResult);
}

/** Group select.
 *
 * @param szType            Group type
 * @param szName            Group name
 * @param szModemId         Target Id
 * @param pResult
 *
 */
BOOL CGroupHelper::Select(const char *szType, const char *szName, const char *szModemId, DBFetchData *pResult)
{
    CppSQLite3Query result;
    GROUPDATAINS *pInstance = NULL;
    GROUPENTRYINS *pEntry = NULL;
    char * szRet;
    UINT nGrId;
    int idx=0;

    if(pResult == NULL) 
    {
        XDEBUG("GroupHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        char queryBuffer[1024];

        sprintf(queryBuffer, "%s ", SELECT_STMT);

        if(!IsNullValue(szType))
        {
            strcat(queryBuffer, "AND g.type = upper(?) ");
        }
        if(!IsNullValue(szName))
        {
            strcat(queryBuffer, "AND g.name = upper(?) ");
        }
        if(!IsNullValue(szModemId))
        {
            strcat(queryBuffer, "AND g._id IN ( "
                    "SELECT g._id FROM GroupTbl g, GroupEntryTbl e "
                    "WHERE g._id = e._groupId "
                    "AND e.targetId = upper(?)) ");
        }

        strcat(queryBuffer, "ORDER BY g.type, g.name ");

        //XDEBUG("%s\n", queryBuffer);
        stmt = m_SqliteHelper.compileStatement(queryBuffer);

        idx = 1;
        if(!IsNullValue(szType))
        {
            stmt.bind(idx, szType);                 idx++;
        }
        if(!IsNullValue(szName))
        {
            stmt.bind(idx, szName);                 idx++;
        }
        if(!IsNullValue(szModemId))
        {
            stmt.bind(idx, szModemId);              idx++;
        }

        result = stmt.execQuery();
        while(!result.eof())
        {
            idx = 0;
            nGrId = result.getIntField(idx);                                            idx++;
            if(pInstance==NULL || pInstance->_id != nGrId)
            {
                if(pInstance != NULL)
                {
                    AddResult(pResult, pInstance);
                }
                pInstance = (GROUPDATAINS *)MALLOC(sizeof(GROUPDATAINS));
                memset(pInstance, 0, sizeof(GROUPDATAINS));

                pInstance->_id = nGrId;
                if((szRet = (char *)result.getStringField(idx)) != NULL) 
                {
                    strncpy(pInstance->szType, szRet, MIN(sizeof(pInstance->szType)-1, strlen(szRet)));
                }                                                                   idx++; //*
                if((szRet = (char *)result.getStringField(idx)) != NULL) 
                {
                    strncpy(pInstance->szName, szRet, MIN(sizeof(pInstance->szName)-1, strlen(szRet)));
                }                                                                   idx++; //*
                ConvTimeStamp(result.getStringField(idx), &pInstance->lastUpdate);  idx++; //*
            }
            else 
            {
                idx += 3;   // 새로 생성해서 증가한 만큼 
            }

            if(pInstance->nEntryCnt >= ENTRY_MAX) 
            {
                result.nextRow();
                continue;
            }

            pEntry = (GROUPENTRYINS *)MALLOC(sizeof(GROUPENTRYINS));
            memset(pEntry,0,sizeof(GROUPENTRYINS));

            pEntry->_id = result.getIntField(idx);                                  idx++;
            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                strncpy(pEntry->szTargetId, szRet, MIN(16, strlen(szRet)));
                StrToEUI64(pEntry->szTargetId, &pEntry->targetId);
            }                                                                       idx++; 
            ConvTimeStamp(result.getStringField(idx), &pEntry->lastUpdate);         idx++; 
            
            pInstance->pEntry[pInstance->nEntryCnt] = pEntry;
            pInstance->nEntryCnt ++;

            result.nextRow();
        }

        if(pInstance != NULL)
        {
            AddResult(pResult, pInstance);
        }
        stmt.finalize();
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "GroupTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CGroupHelper::Delete()
{
    return Delete(NULL, NULL, (char *)NULL);
}

BOOL CGroupHelper::Delete(const char *szType)
{
    return Delete(szType, NULL, (char *)NULL);
}

BOOL CGroupHelper::Delete(const char *szType, const char *szName)
{
    return Delete(szType, szName, (char *)NULL);
}

BOOL CGroupHelper::Delete(const char *szType, const char *szName, const EUI64 *pModemId)
{
    char szId[32]={0,};

    if(pModemId != NULL)
    {
        EUI64ToStr(pModemId, szId);
    }
    return Delete(szType, szName, pModemId != NULL ? szId : (const char *)NULL);
}

BOOL CGroupHelper::Delete(const char *szType, const char *szName, const char *szModemId)
{
    int idx;

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        char queryBuffer[1024];

        sprintf(queryBuffer, "%s", 
                "DELETE FROM GroupEntryTbl "
                "WHERE _id IN "
                "(SELECT e._id FROM GroupTbl g, GroupEntryTbl e "
                "WHERE g._id = e._groupId ");

        if(!IsNullValue(szType))
        {
            strcat(queryBuffer, "AND g.type = upper(?) ");
        }
        if(!IsNullValue(szName))
        {
            strcat(queryBuffer, "AND g.name = upper(?) ");
        }
        if(!IsNullValue(szModemId))
        {
            strcat(queryBuffer, "AND e.targetId = upper(?) ");
        }
        strcat(queryBuffer, ") ");

        TRANSACTION_BEGIN;

        //XDEBUG("%s\n", queryBuffer);
        stmt = m_SqliteHelper.compileStatement(queryBuffer);

        idx = 1;
        if(!IsNullValue(szType))
        {
            stmt.bind(idx, szType);                 idx++;
        }
        if(!IsNullValue(szName))
        {
            stmt.bind(idx, szName);                 idx++;
        }
        if(!IsNullValue(szModemId))
        {
            stmt.bind(idx, szModemId);              idx++;
        }

        stmt.execDML();
        stmt.finalize();

        stmt = m_SqliteHelper.compileStatement(
                "DELETE FROM GroupTbl "
                "WHERE _id IN ( "
                "SELECT g1._id FROM GroupTbl g1, ( "
                    "SELECT g._id gid, e._id eid FROM "
                    "GroupTbl g LEFT OUTER JOIN GroupEntryTbl e ON e._groupId = g._id ) g2 "
                "WHERE "
                    "g1._id = g2.gid AND g2.eid is NULL) ");

        stmt.execDML();
        stmt.finalize();

        TRANSACTION_COMMIT;
    }
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG(ANSI_COLOR_RED "GroupTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        try { TRANSACTION_ROLLBACK; Close(); } catch(CppSQLite3Exception&e2){}
        return FALSE;
	}
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CGroupHelper::InitializeDB()
{
    try
    {    
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS GroupTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "type TEXT NOT NULL, "
                "name TEXT NOT NULL, "
                "lastUpdate DATETIME NOT NULL,  "
                "UNIQUE ( type, name ) "
            ");" );
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS GroupEntryTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "_groupId INTEGER NOT NULL, "
                "targetId TEXT NOT NULL , "
                "lastUpdate DATETIME NOT NULL,  "
                "FOREIGN KEY(_groupId) REFERENCES GroupTbl(_id), "
                "UNIQUE ( _groupId, targetId ) "
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

BOOL CGroupHelper::UpgradeDB()
{
    try
    {
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS GroupTbl; ");
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS GroupEntryTbl; ");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}

BOOL CGroupHelper::FreeData(void *pData)
{
    GROUPDATAINS * ins;
    int i;

    if(pData == NULL) return FALSE;

    ins = (GROUPDATAINS *)pData;

    for(i=0;i<ins->nEntryCnt && i<ENTRY_MAX;i++)
    {
        if(ins->pEntry[i])
        {
            FREE(ins->pEntry[i]);
            ins->pEntry[i] = NULL;
        }
    }

    return TRUE;
}

BOOL CGroupHelper::DumpData(void *pData)
{
    GROUPDATAINS *ins;
    GROUPENTRYINS **ppEntry;
    char timeStr[32];
    int i;

    if(pData == NULL) return FALSE;
    ins = (GROUPDATAINS *)pData;

    ConvTimeString(&ins->lastUpdate, timeStr);
    XDEBUG(" _id %d type [%s] name [%s] %s\r\n",
            ins->_id, ins->szType, ins->szName, timeStr);

    ppEntry = ins->pEntry;
    for(i=0;i<ins->nEntryCnt;i++)
    {
        ConvTimeString(&ppEntry[i]->lastUpdate, timeStr);
        XDEBUG("    %02d _id %d  [%s] %s\r\n", i,
                ppEntry[i]->_id, ppEntry[i]->szTargetId, timeStr);
    }

    return TRUE;
}

