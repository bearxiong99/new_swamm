#ifndef __DATABASE_HELPER_H__
#define __DATABASE_HELPER_H__

#include "typedef.h"
#include "sqlite/sqlite3.h"
#include "CppSQLite3.h"

#define DB_FILE_AGENT       "/app/sqlite/Agent.db"
#define DB_FILE_GROUP       "/app/sqlite/Group.db"
#define DB_FILE_SECURITY    "/app/sqlite/Security.db"
#define DB_FILE_OPTIONS     "/app/sqlite/Options.db"
#define DB_FILE_PROPERTY    "/app/sqlite/Property.db"

#define TRANSACTION_BEGIN   m_SqliteHelper.execDML("BEGIN TRANSACTION;")
#define TRANSACTION_ROLLBACK     m_SqliteHelper.execDML("ROLLBACK TRANSACTION;")
#define TRANSACTION_COMMIT  m_SqliteHelper.execDML("COMMIT TRANSACTION;")

typedef struct _Row {
    void        * pData;
    struct _Row * pNext;
} Row;

typedef struct _DBFetchData {
    int         nCount;
    struct _Row * pHead;
    struct _Row * pTail;
} DBFetchData;

class CDatabaseHelper
{
public:
	CDatabaseHelper(const char * szDbFileName, const char * szClassName, int nVersion);
	virtual ~CDatabaseHelper();

public:
    BOOL    CheckDB(); 
    BOOL    Initialize();
    int     GetVersion();

    Row*    GetNextRow(Row *pFetchRow);
    Row*    GetHeadRow(DBFetchData *pFetchData);
    BOOL    FreeResult(DBFetchData *pFetchData);
    BOOL    DumpResult(DBFetchData *pFetchData);

protected:
	BOOL    Open(const char * szDbFileName);
	BOOL    Open();
	BOOL    Close();
    BOOL    AddResult(DBFetchData *pFetchData, void *pSelectedRow);

protected:
    virtual BOOL InitializeDB();
    virtual BOOL UpgradeDB();
    virtual BOOL FreeData(void *pData);
    virtual BOOL DumpData(void *pData);

protected:
    CppSQLite3DB        m_SqliteHelper;

private:
    void initializeMetaTable(const char * szClassName, int nVersion);

private:
    BOOL                m_bInitialize;
    const char *        m_szDbFileName;
    const char *        m_szClassName;
    int                 m_nVersion;
};

#endif // __DATABASE_HELPER_H__
