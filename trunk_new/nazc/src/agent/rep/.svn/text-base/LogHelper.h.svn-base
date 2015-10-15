#ifndef __LOG_HELPER_H__
#define __LOG_HELPER_H__

#include "typedef.h"
#include "DatabaseHelper.h"

#define LOG_MAX        512

typedef struct {
    UINT            _id;
    UINT            _meterId;
    TIMESTAMP       logTime;
    BYTE            nCategory;
    int             nLogSize;
    BYTE            *szLog;

    int             nTryUpload;
    TIMESTAMP       uploadTime;

    TIMESTAMP       createTime;
    TIMESTAMP       lastUpdate;
} LOGENTRYINS;

typedef struct {
    UINT            _id;

    UINT            _meterId;
    UINT            _modemId;
    char            szModemId[16+1];    // Modem Id
    EUI64           modemId;            // Modem Id
    BYTE            nPortNum;           // Meter Port Number

    int             nLogCnt;
    LOGENTRYINS     *pLog[LOG_MAX];
} LOGDATAINS;

class CLogHelper : public CDatabaseHelper
{
public:
	CLogHelper();
	CLogHelper(const char * szDbFile);
	virtual ~CLogHelper();

// Operations
public:
    BOOL    Initialize();

    BOOL    Select(DBFetchData *pResult);
    BOOL    Select(TIMESTAMP *pFrom, TIMESTAMP *pTo, DBFetchData *pResult);

    BOOL    Select(const EUI64 *pId, int nPortNum, DBFetchData *pResult);
    BOOL    Select(const char *szId, int nPortNum, DBFetchData *pResult);

    BOOL    Select(const char *szModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo, 
                DBFetchData *pResult);

    BOOL    Update(const EUI64 *pModemId, int nPortNum, int nLogCnt, LOGENTRYINS *pLogEntry);
    BOOL    Update(const char *szModemId, int nPortNum, int nLogCnt, LOGENTRYINS *pLogEntry);
    BOOL    Update(const UINT nMeterId, int nLogCnt, LOGENTRYINS *pLogEntry);

    BOOL    Delete();

    BOOL    Delete(const EUI64 *pModemId, int nPortNum);
    BOOL    Delete(const char *szModemId, int nPortNum);

    BOOL    Delete(const EUI64 *pModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo);
    BOOL    Delete(const char *szModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo);

    BOOL    TryUpload(DBFetchData *pResult);
    BOOL    UploadComplete(int nCount, UINT *pId);

    LOGDATAINS *GetData(Row *pRow);

protected:
    BOOL    InitializeDB();
    BOOL    UpgradeDB();
    BOOL    FreeData(void *pData);
    BOOL    DumpData(void *pData);

    BOOL    _Fetch(CppSQLite3Query *result, BOOL bUpdate, DBFetchData *pResult);

};

extern CLogHelper	*m_pLogHelper;

#endif	// __LOG_HELPER_H__
