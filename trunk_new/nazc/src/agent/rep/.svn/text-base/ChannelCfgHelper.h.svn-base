#ifndef __CH_CFG_HELPER_H__
#define __CH_CFG_HELPER_H__

#include "typedef.h"
#include "DatabaseHelper.h"

typedef struct {
    UINT        _id;
    UINT        _meterId;
    UINT        _modemId;
    char        szModemId[16+1];    // Modem Id
    EUI64       modemId;            // Modem Id
    BYTE        nPortNum;           // Meter Port Number

    BYTE        nIndex;
    BYTE        nUnit;
    BYTE        nSigExp;
    BYTE        nValueType;
    BYTE        nObjType;
    BYTE        nChType;
    BOOL        bIsCurrent;

    BOOL        bTryUpload;
    BOOL        bUpload;
    TIMESTAMP   uploadTime;

    TIMESTAMP   lastUpdate;
} CHANNELCFGINS;

class CChannelCfgHelper : public CDatabaseHelper
{
public:
	CChannelCfgHelper();
	CChannelCfgHelper(const char * szDbFile);
	virtual ~CChannelCfgHelper();

// Operations
public:
    BOOL    Initialize();

    BOOL    Select(DBFetchData *pResult);

    BOOL    Select(const EUI64 *pId, int nPortNum, BOOL bLast, DBFetchData *pResult);
    BOOL    Select(const char *szId, int nPortNum, BOOL bLast, DBFetchData *pResult);

    BOOL    Select(const EUI64 *pId, int nPortNum, int nChIdx, BOOL bLast, DBFetchData *pResult);
    BOOL    Select(const char *szId, int nPortNum, int nChIdx, BOOL bLast, DBFetchData *pResult);

    BOOL    Select(UINT nMeterId, DBFetchData *pResult);
    BOOL    Select(UINT nMeterId, BOOL bLast, DBFetchData *pResult);
    BOOL    Select(UINT nMeterId, int nChIdx, BOOL bLast, DBFetchData *pResult);

    BOOL    GetId(const EUI64 *pId, int nPortNum, BYTE nChIdx, UINT *nChannelCfgId);
    BOOL    GetId(const char *szId, int nPortNum, BYTE nChIdx, UINT *nChannelCfgId);

    BOOL    Update(const EUI64 *pModemId, int nPortNum, const BYTE nChIdx, 
                const BYTE nUnit, const BYTE nSigExp, const BYTE nValueType,
                const BYTE nObjType, const BYTE nChType);
    BOOL    Update(const char *szModemId, int nPortNum, const BYTE nChIdx, 
                const BYTE nUnit, const BYTE nSigExp, const BYTE nValueType,
                const BYTE nObjType, const BYTE nChType);
    BOOL    Update(const UINT nMeterId, const BYTE nChIdx, 
                const BYTE nUnit, const BYTE nSigExp, const BYTE nValueType,
                const BYTE nObjType, const BYTE nChType);

    BOOL    Delete();

    BOOL    Delete(const EUI64 *pModemId, int nPortNum);
    BOOL    Delete(const char *szModemId, int nPortNum);

    BOOL    Delete(const EUI64 *pModemId, int nPortNum, int nChIdx);
    BOOL    Delete(const char *szModemId, int nPortNum, int nChIdx);

    BOOL    TryUpload(DBFetchData *pResult);
    BOOL    UploadComplete(int nCount, UINT *pId);

    CHANNELCFGINS *GetData(Row *pRow);

protected:
    BOOL    _Fetch(CppSQLite3Query *result, DBFetchData *pResult);

protected:
    BOOL    InitializeDB();
    BOOL    UpgradeDB();
    BOOL    FreeData(void *pData);
    BOOL    DumpData(void *pData);

};

extern CChannelCfgHelper	*m_pChannelCfgHelper;

#endif	// __CH_CFG_HELPER_H__
