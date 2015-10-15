
#include "types/identification.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "VariableHelper.h"
#include "DbUtil.h"

//////////////////////////////////////////////////////////////////////
// CVariableHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS        "Variable"
#define DB_VERSION      1

#define SELECT_STMT         "SELECT "   \
                                "_id, _name, value, lastUpdate "  \
                            "FROM VariableTbl " 


CVariableHelper   	*m_pVariableHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CVariableHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CVariableHelper::CVariableHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pVariableHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CVariableHelper::CVariableHelper() :
    CDatabaseHelper(DB_FILE_AGENT, DB_CLASS, DB_VERSION)
{
	m_pVariableHelper	= this;
}

CVariableHelper::~CVariableHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CVariableHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CVariableHelper::Initialize()
{
    return CDatabaseHelper::Initialize();
}

/** Meter Inventory Update.
 *
 * @param szName        Variable name           
 * @param nValue        Value
 *
 */
BOOL CVariableHelper::Update(const char *szName, const int nValue)
{
    char buff[32];
    sprintf(buff, "%d", nValue);
    return Update(szName, buff);
}

/** Meter Inventory Update.
 *
 * @param szName        Variable name           
 * @param nValue        Value
 *
 */
BOOL CVariableHelper::Update(const char *szName, const UINT nValue)
{
    char buff[32];
    sprintf(buff, "%u", nValue);
    return Update(szName, buff);
}

/** Meter Inventory Update.
 *
 * @param szName        Variable name           
 * @param szValue       Value
 *
 */
BOOL CVariableHelper::Update(const char *szName, const char *szValue)
{
    int idx=1;
    BOOL bInsert = FALSE;

    if(!szName) return FALSE;
    if(!CheckDB()) return FALSE;
    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Query result;
        CppSQLite3Statement stmt;
 
        stmt = m_SqliteHelper.compileStatement("SELECT count(*) FROM VariableTbl WHERE _name = ? ;");
        idx = 1;
        stmt.bind(idx, szName);                             idx++;
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
                        Close();
                        return FALSE;
            }
        }

        stmt.finalize();
        if(bInsert) {
            XDEBUG(" Insert Variable: %s [%s]\r\n" , szName, szValue ? szValue : "");
            stmt = m_SqliteHelper.compileStatement(
                "INSERT INTO VariableTbl "
                    "( value, _name, lastUpdate ) "
                "VALUES "
                    "( ?, ?, DATETIME('now','localtime')); ");
        } else {
            XDEBUG(" Update Variable: %s [%s]\r\n" , szName, szValue ? szValue : "");
            stmt = m_SqliteHelper.compileStatement(
                "UPDATE VariableTbl "
                "SET  value = ?, lastUpdate = DATETIME('now','localtime') "
                "WHERE "
                    "_name = ? AND value != ? ; ");
        }

        idx = 1;
        if(szValue == NULL)
        {
            stmt.bindNull(idx);
        }
        else
        {
            stmt.bind(idx, szValue);
        }                                                   idx++;

        stmt.bind(idx, szName);                             idx++;
        if(!bInsert) { // Update
            if(szValue == NULL)
            {
                stmt.bindNull(idx);
            }
            else
            {
                stmt.bind(idx, szValue);
            }                                               idx++;
        }

        stmt.execDML();
        stmt.finalize();

        Close();
        return TRUE;
    }
	catch ( CppSQLite3Exception& e )
	{
        //TRANSACTION_END;
        Close();
		XDEBUG(ANSI_COLOR_RED "VariableTbl DB ERROR UPDATE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
	}

    return FALSE;
}

VARIABLETBLINS* CVariableHelper::GetData(Row *pRow)
{
    if(pRow == NULL || pRow->pData == NULL) return NULL;
    return (VARIABLETBLINS *)pRow->pData;
}

BOOL CVariableHelper::Select(DBFetchData *pResult)
{
    return Select((char *)NULL, pResult);
}

BOOL CVariableHelper::Select(const char *szName, DBFetchData *pResult)
{
    CppSQLite3Query result;
    VARIABLETBLINS *pInstance;
    char * szRet;
    int idx=0;

    if(pResult == NULL) 
    {
        XDEBUG("VariableHelper::Select : NULL Parameter\r\n");
        return FALSE;
    }

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;

        if(szName == NULL) 
        {
            // 전체 Select
            stmt = m_SqliteHelper.compileStatement(
                SELECT_STMT ";" );
        } 
        else 
        {
            stmt = m_SqliteHelper.compileStatement(
                SELECT_STMT
                    "AND _name = ? ; ");
            idx = 1;
            stmt.bind(idx, szName);         idx++;
        }

        result = stmt.execQuery();
        Close();
        while(!result.eof())
        {
            pInstance = (VARIABLETBLINS *)MALLOC(sizeof(VARIABLETBLINS));
            memset(pInstance, 0, sizeof(VARIABLETBLINS));

            idx = 0;
            pInstance->_id = result.getIntField(idx);                               idx++;
            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                pInstance->nNameLen = strlen(szRet);
                pInstance->szName = (char *)MALLOC(pInstance->nNameLen+1);
                memset(pInstance->szName, 0, pInstance->nNameLen+1);
                strcpy(pInstance->szName, szRet);
            }                                                                       idx++;
            if((szRet = (char *)result.getStringField(idx)) != NULL) 
            {
                pInstance->nValueLen = strlen(szRet);
                pInstance->szValue = (char *)MALLOC(pInstance->nValueLen+1);
                memset(pInstance->szValue, 0, pInstance->nValueLen+1);
                strcpy(pInstance->szValue, szRet);
            }                                                                       idx++;
            ConvTimeStamp(result.getStringField(idx), &pInstance->lastUpdate);      idx++;

            AddResult(pResult, pInstance);

            result.nextRow();
        }
        stmt.finalize();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "VariableTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

BOOL CVariableHelper::Delete()
{
    return Delete((char *)NULL);
}

BOOL CVariableHelper::Delete(const char *szName)
{
    int idx=0;

    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;

        if(szName == NULL) 
        {
            // 전체 Delete
            stmt = m_SqliteHelper.compileStatement("DELETE FROM VariableTbl; ");
        } 
        else 
        {
            stmt = m_SqliteHelper.compileStatement(
                "DELETE FROM VariableTbl "
                "WHERE "
                    "_name = ? "
                "; ");
            idx = 1;
            stmt.bind(idx, szName);         idx++;
        }

        stmt.execDML();
        stmt.finalize();
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "VariableTbl ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CVariableHelper::InitializeDB()
{
    try
    {    
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS VariableTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "_name TEXT NOT NULL, "
                "value TEXT, "                 // Meter Serial
                "lastUpdate DATETIME NOT NULL,  "
                "UNIQUE ( _name ) "
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
BOOL CVariableHelper::UpgradeDB()
{
    try
    {
        if(!Open()) return FALSE;
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS VariableTbl; ");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}


BOOL CVariableHelper::FreeData(void *pData)
{
    VARIABLETBLINS * ins;
    if(pData == NULL) return FALSE;

    ins = (VARIABLETBLINS *)pData;
    if(ins->szName != NULL) FREE(ins->szName);
    if(ins->szValue != NULL) FREE(ins->szValue);

    return TRUE;
}

BOOL CVariableHelper::DumpData(void *pData)
{
    VARIABLETBLINS * ins;
    if(pData == NULL) return FALSE;
    ins = (VARIABLETBLINS *)pData;

    XDEBUG(" %5d %s [%s]\r\n", ins->_id, ins->szName, ins->szValue ? ins->szValue : "");
    return TRUE;
}

