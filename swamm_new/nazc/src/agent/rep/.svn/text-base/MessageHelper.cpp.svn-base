#include "MessageHelper.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#include "if4api.h"

//////////////////////////////////////////////////////////////////////
// CMessageHelper Data Definitions
//////////////////////////////////////////////////////////////////////

#define DB_CLASS    "Message"
#define DB_VERSION  1

CMessageHelper   	*m_pMessageHelper = NULL;

//////////////////////////////////////////////////////////////////////
// CMessageHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** DB File 지정 생성자.
 */
CMessageHelper::CMessageHelper(const char * szDbFile) : 
    CDatabaseHelper(szDbFile, DB_CLASS, DB_VERSION)
{
	m_pMessageHelper	= this;
}

/** 기본 DB File 사용 생성자
 */
CMessageHelper::CMessageHelper() :
    CDatabaseHelper("/app/sqlite/Message.db", DB_CLASS, DB_VERSION)
{
	m_pMessageHelper	= this;
}

CMessageHelper::~CMessageHelper()
{
}

//////////////////////////////////////////////////////////////////////
// CMessageHelper Method
//////////////////////////////////////////////////////////////////////
BOOL CMessageHelper::Initialize()
{
    CDatabaseHelper::Initialize();

	if (!CTimeoutThread::StartupThread(13))
		return FALSE;

	return TRUE;
}

void CMessageHelper::Destroy()
{
	CTimeoutThread::ShutdownThread();
}

/** Message Queuing.
 *
 * @param szId          Sensor ID
 * @param nMessageId    사용자 정의 Message ID
 * @param nMessageType  Message Type (0x01 Immediately, 0x02 Lazy, 0x03 Passive)
 * @param nDuration     Lazy, Passive 일 경우 유지 시간(sec)
 * @param nErrorHandler Error Handler
 * @param nPreHandler   Pre-Action Handler
 * @param nPostHandler  Post-Action Handler
 * @param nUserData     User Data
 * @param nDataLength   Message length
 * @param pszData       Message
 *
 */
BOOL CMessageHelper::Add(const char *szId, UINT nMessageId, BYTE nMessageType, UINT nDuration,
        UINT nErrorHandler, UINT nPreHandler, UINT nPostHandler, UINT nUserData, int nDataLength, const BYTE *pszData)
{
    int i=1;
    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt = m_SqliteHelper.compileStatement(
            "INSERT INTO MessageTbl "
            "( targetId, messageId, messageType, duration, "
            "  errorHandler, preHandler, postHandler, userData, payload ) "
            "VALUES "
            "( ?, ?, ?, ?, "
            "  ?, ?, ?, ?, ?);");

        stmt.bind(i, szId);                             i++;
        stmt.bind(i, (const int)nMessageId);            i++;
        stmt.bind(i, nMessageType);                     i++;
        stmt.bind(i, (const int)nDuration);             i++;
        stmt.bind(i, (const int)nErrorHandler);         i++;
        stmt.bind(i, (const int)nPreHandler);           i++;
        stmt.bind(i, (const int)nPostHandler);          i++;
        stmt.bind(i, (const int)nUserData);             i++;
        stmt.bind(i, pszData, nDataLength);             i++;

        stmt.execDML();
        stmt.finalize();

        Close();
        return TRUE;
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "DB ERROR: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
	}

    return FALSE;
}

/** Message 조회
 *
 * @param szId target device id
 * @param nMessageType 0 이라면 Message 전체
 * @param pWrapper 응답을 전해 줄 IF4Wrapper
 */
BOOL CMessageHelper::Select(const char *szId, BYTE nMessageType, IF4Wrapper *pWrapper)
{
    if(!CheckDB()) return FALSE;

    CppSQLite3Query result;
    char timeString[32];
    TIMESTAMP issueTime;
    unsigned char * pPayload;
    int idx=0, nPayloadLen;
    int year, mon, day, hour, min, sec;

    if(pWrapper == NULL) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        
        if (nMessageType > 0) {
            stmt = m_SqliteHelper.compileStatement(
                "SELECT "
                "messageId, issueTime, userData, payload, "
                "duration, errorHandler, preHandler, postHandler "
                "FROM MessageTbl "
                "WHERE targetId = ? and messageType = ? ; ");

            stmt.bind(1, szId);
            stmt.bind(2, nMessageType);
        } else {
            stmt = m_SqliteHelper.compileStatement(
                "SELECT "
                "messageId, issueTime, userData, payload, "
                "duration, errorHandler, preHandler, postHandler "
                "FROM MessageTbl "
                "WHERE targetId = ? ; ");

            stmt.bind(1, szId);
        }

        result = stmt.execQuery();
        while(!result.eof())
        {
            memset(timeString, 0, sizeof(timeString));
            memset(&issueTime, 0, sizeof(TIMESTAMP));
            pPayload = NULL; nPayloadLen = 0; idx = 0;

            IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, result.getIntField(idx));  idx++;
            strcat(timeString, result.getStringField(idx));                                 idx++;
            sscanf(timeString,"%04d-%02d-%02d %02d:%02d:%02d",
                    &year, &mon, &day, &hour, &min, &sec);
            issueTime.year = year; issueTime.mon = mon;
            issueTime.day = day; issueTime.hour = hour;
            issueTime.min = min; issueTime.sec = sec;
            /*
            XDEBUG(" %04d/%02d/%02d %02d:%02d:%02d\r\n",
                    issueTime.year, issueTime.mon, issueTime.day, issueTime.hour, issueTime.min, issueTime.sec);
                    */
	        IF4API_AddResultFormat(pWrapper, "1.16", VARSMI_TIMESTAMP, &issueTime, sizeof(TIMESTAMP));
            IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, result.getIntField(idx));  idx++;
            pPayload = const_cast<unsigned char *>(result.getBlobField(idx, nPayloadLen));  idx++;
	        IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, pPayload, nPayloadLen);

            result.nextRow();
        }
        stmt.finalize();
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "MessageTbl DB ERROR SELECT: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}
    return TRUE;
}

/** 해당 ID에 대한 전체 Message 삭제
 */
BOOL CMessageHelper::Delete(const char *szId, BYTE nMessageType)
{
    if(!CheckDB()) return FALSE;

    if(!Open()) return FALSE;
    try
    {
        CppSQLite3Statement stmt;
        
        if(nMessageType > 0)
        {
            stmt = m_SqliteHelper.compileStatement(
                "DELETE FROM MessageTbl "
                "WHERE targetId = ? AND messageType = ? ; ");

            stmt.bind(1, szId);
            stmt.bind(2, nMessageType);
        } else {
            stmt = m_SqliteHelper.compileStatement(
                "DELETE FROM MessageTbl "
                "WHERE targetId = ?; ");

            stmt.bind(1, szId);
        }

        stmt.execDML();
        stmt.finalize();
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "DB ERROR DELETE: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CDatabaseHelper Overloading
//////////////////////////////////////////////////////////////////////
BOOL CMessageHelper::InitializeDB()
{
    if(!Open()) return FALSE;
    try
    {
        m_SqliteHelper.execDML("CREATE TABLE IF NOT EXISTS MessageTbl "
                "(_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "issueTime DATETIME DEFAULT (DATETIME('now','localtime')), "
                "targetId TEXT NOT NULL, "
                "messageType INTEGER NOT NULL, "
                "messageId INTEGER DEFAULT 0, "
                "duration INTEGER DEFAULT 0, "
                "errorHandler INTEGER DEFAULT 0, "
                "preHandler INTEGER DEFAULT 0, "
                "postHandler INTEGER DEFAULT 0, "
                "userData INTEGER DEFAULT 0, "
                "payload BLOB "
        ");");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "DB ERROR: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}

    return TRUE;
}

BOOL CMessageHelper::UpgradeDB()
{
    if(!Open()) return FALSE;
    try
    {
        m_SqliteHelper.execDML("DROP TABLE IF EXISTS MessageTbl; ");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
        return FALSE;
	}

    return InitializeDB();
}

//////////////////////////////////////////////////////////////////////
// CTimeoutThread Overloading
//////////////////////////////////////////////////////////////////////

BOOL CMessageHelper::OnActiveThread()
{
    //XDEBUG("MessageHelper::OnActiveThread\r\n");
    if(!Open()) return FALSE;
    try
    {
        m_SqliteHelper.execDML("DELETE FROM MessageTbl "
                "WHERE ( STRFTIME('%s','now','localtime') - STRFTIME('%s',issueTime) ) > duration;");
        Close();
    }
	catch ( CppSQLite3Exception& e )
	{
        Close();
		XDEBUG(ANSI_COLOR_RED "MessageTbl DB ERROR: %d %s\r\n" ANSI_NORMAL, e.errorCode(), e.errorMessage());
        return FALSE;
	}

	return TRUE;
}

BOOL CMessageHelper::OnTimeoutThread()
{
	return OnActiveThread();
}

