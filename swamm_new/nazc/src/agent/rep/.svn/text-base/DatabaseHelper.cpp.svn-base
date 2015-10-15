/** SQLite3 Database Helper
 */

#include "DatabaseHelper.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"
#include "CommonUtil.h"

/** DB Helper 생성자.
 */
CDatabaseHelper::CDatabaseHelper(const char * szDbFileName, const char * szClassName, int nVersion) 
    : m_szDbFileName(szDbFileName), m_szClassName(szClassName)
{
    m_bInitialize = FALSE;
    m_nVersion = nVersion;
}

/** 소멸자.
 *
 * Initialize가 되어 있을 경우 자동으로 Close 한다.
 *
 */ 
CDatabaseHelper::~CDatabaseHelper()
{
    if(m_bInitialize) 
    {
        Close();
    }
}

/***************************************************************************************
 * private method
 ***************************************************************************************/
BOOL CDatabaseHelper::InitializeDB()
{
    return TRUE;
}

BOOL CDatabaseHelper::UpgradeDB()
{
    fprintf(stderr, "Upgrade DB\r\n");
    return TRUE;
}

/** Class Version을 확인하여 상위 버전으로 변경되었을 경우 Upgrade 한다.
 */
void CDatabaseHelper::initializeMetaTable(const char * szClassName, int nVersion)
{
    if(!CheckDB()) return;

    CppSQLite3Buffer    buffer;
    CppSQLite3Table     table;
    int                 nOldVersion=0;

    if(!Open()) return;

    m_SqliteHelper.setBusyTimeout(500);

    try 
    {
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS TableMetaInfo "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "className TEXT UNIQUE, "
                "version INTEGER);");
    }
	catch ( CppSQLite3Exception& e )
	{
	}

    buffer.format("SELECT * FROM TableMetaInfo WHERE className=%Q;", szClassName);
    table = m_SqliteHelper.getTable(buffer);

    switch(table.numRows())
    {
        case 0: // inter new data
            buffer.clear();
            buffer.format("INSERT INTO TableMetaInfo "
                    "( className, version ) "
                    "VALUES ( %Q, %d );", szClassName, nVersion);
            m_SqliteHelper.execDML(buffer);

            fprintf(stderr, ANSI_COLOR_GREEN " + AiMiR DB Meta Information Create : %s %d\r\n" ANSI_NORMAL, szClassName, nVersion);
            // DB 초기화
            InitializeDB();
            break;
        case 1: // update version;
            nOldVersion = table.getIntField("version", 0);
            if(nVersion != nOldVersion)
            {
                buffer.clear();
                buffer.format("UPDATE TableMetaInfo "
                        "SET version = %d "
                        "WHERE className = %Q;", nVersion, szClassName);
                m_SqliteHelper.execDML(buffer);

                fprintf(stderr, ANSI_COLOR_GREEN " + AiMiR DB Meta Information Update : %s %d\r\n" ANSI_NORMAL, szClassName, nVersion);
                // DB Upgrade
                UpgradeDB();
            } else {
                InitializeDB();
            }
            break;
        default:
            // 예외사항 발생하면 안된다.
            fprintf(stderr, ANSI_COLOR_RED " + AiMiR DB Meta Information Error\r\n" ANSI_NORMAL);
            break;
    }

    Close();
}



/***************************************************************************************
 * protected method
 ***************************************************************************************/
BOOL CDatabaseHelper::Initialize()
{
    if(!m_bInitialize) 
    {
        m_bInitialize = Open(m_szDbFileName);
        //m_bInitialize = TRUE;
    }

    if(m_bInitialize) 
    {
        try 
        {
            initializeMetaTable(m_szClassName, m_nVersion);
            return TRUE;
	    }
	    catch ( CppSQLite3Exception& e )
	    {
		    fprintf(stderr, ANSI_COLOR_RED "DB ERROR Initialize: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
            return FALSE;
	    }
    }
    return FALSE;
}

BOOL CDatabaseHelper::Open()
{
    return Open(m_szDbFileName);
}

/** Dabase Open.
 *
 * @return 성공시 TRUE
 */
BOOL CDatabaseHelper::Open(const char * szDbFileName)
{
    if(!CheckDB()) {
        try
        {
            m_SqliteHelper.open(szDbFileName);
            //XDEBUG("+ Open DB\r\n");
        }
        catch (CppSQLite3Exception &e) 
        {
            XDEBUG("Database open fail!\r\n");
            return FALSE;
        }
    }
    return TRUE;
}

/** Database Close.
 *
 * @return 성공시 TRUE
 */
BOOL CDatabaseHelper::Close()
{
    /** Close를 하지 않는다 */
    return TRUE;
#if 0
    //if(CheckDB()) 
    //{
        try
        {
            m_SqliteHelper.close();
            //XDEBUG("- Close DB\r\n");
        }
        catch (CppSQLite3Exception &e) 
        {
            XDEBUG("Database close fail!\r\n");
            return FALSE;
        }
    //}
    return TRUE;
#endif
}

/** Fetch된 Row를 잉용해서 FetchData 구조체를 구성한다.
 *
 */
BOOL CDatabaseHelper::AddResult(DBFetchData *pFetchData, void *pSelectedRow)
{
    Row *pRow;

    if(pFetchData == NULL || pSelectedRow == NULL) return FALSE;

    pRow = (Row *) MALLOC(sizeof(Row));
    memset(pRow, 0, sizeof(Row));
    pRow->pData = pSelectedRow;
   
    if(pFetchData->pHead == NULL) {
        pFetchData->pHead = pRow;
    } else {
        pFetchData->pTail->pNext = pRow;
    }
    pFetchData->nCount ++;
    pFetchData->pTail = pRow;

    return TRUE;
}

/** Row Data를 Free 해야 할 때 overriding 해야 한다.
 */
BOOL CDatabaseHelper::FreeData(void *pData)
{
    return TRUE;
}

/** Row Data를 Dump 해야 할 때 overriding 해야 한다.
 */
BOOL CDatabaseHelper::DumpData(void *pData)
{
    XDEBUG("\r\n");
    return TRUE;
}

/***************************************************************************************
 * public method
 ***************************************************************************************/
BOOL CDatabaseHelper::CheckDB()
{
    return m_bInitialize;
}

/** Database Helper의 현재 버전 정보를 리턴한다.
 */
int CDatabaseHelper::GetVersion()
{
    return m_nVersion;
}

/** Select된 결과를 Free 해준다.
 * @warning pFetchData 자체는 Free 하지 않는다.
 * 
 */
BOOL CDatabaseHelper::FreeResult(DBFetchData *pFetchData)
{
    Row *row, *prow;
    if(pFetchData == NULL) return  FALSE;

    row = pFetchData->pHead;
    while(row!=NULL) {
        if(row->pData != NULL) {
            // virtual overriding
            FreeData(row->pData);
            FREE(row->pData);
        }
        prow = row;
        row = prow->pNext;
        FREE(prow);
    }
    memset(pFetchData, 0, sizeof(DBFetchData));
    //FREE(pFetchData);
    return TRUE;
}

BOOL CDatabaseHelper::DumpResult(DBFetchData *pFetchData)
{
    int i;
    Row *row;
    if(pFetchData == NULL) return  FALSE;
    if(pFetchData->nCount <= 0) return TRUE;

    XDEBUG("=================================================================\r\n");
    for(i=0, row = pFetchData->pHead; row != NULL; i++, row = row->pNext) {
        if(row->pData != NULL) {
            XDEBUG("%3d:", i+1);
            DumpData(row->pData);
        }
    }
    XDEBUG("-----------------------------------------------------------------\r\n");
    return TRUE;
}


/** 다음 Row 리턴.
 *
 */
Row* CDatabaseHelper::GetNextRow(Row *pRow)
{
    if(pRow == NULL) return NULL;
    return pRow->pNext;
}

Row* CDatabaseHelper::GetHeadRow(DBFetchData *pFetchData)
{
    if(pFetchData == NULL) return NULL;
    return pFetchData->pHead;
}

