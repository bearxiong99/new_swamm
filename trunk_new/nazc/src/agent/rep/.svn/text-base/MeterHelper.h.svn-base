#ifndef __METER_HELPER_H__
#define __METER_HELPER_H__

#include "typedef.h"
#include "ModemHelper.h"

typedef struct {
    UINT        _id;
    UINT        _modemId;
    char        szModemId[16+1];
    EUI64       modemId;
    BYTE        nPortNum;
    char        *szSerial;  // Serial
    char        *szModel;   // Model
    UINT        nParserType;
    BYTE        nServiceType;
    BYTE        nVendor;
    WORD        nPeriod;  // LP 주기 (1:60, 2:30, 4:15, 12:5)
    //UINT        nCurrentPulse;
    BYTE        nEnergyLevel;
    double      hwVer;
    double      swVer;

    int         nFailReason;

    TIMESTAMP   installTime;
    //TIMESTAMP   lastCurrentPulse;
    TIMESTAMP   lastEnergyLevel;
    TIMESTAMP   lastMetering;
    TIMESTAMP   lastFailMetering;

    int         nTryUpload;
    BOOL        bUpload;
    TIMESTAMP   uploadTime;

    TIMESTAMP   lastUpdate;
} METERTBLINS;

class CMeterHelper : public CDatabaseHelper
{
public:
	CMeterHelper();
	CMeterHelper(const char * szDbFile);
	virtual ~CMeterHelper();

// Operations
public:
    BOOL    Initialize();

    BOOL    Select(DBFetchData *pResult);
    BOOL    Select(const EUI64 *pId, int nPortNum, DBFetchData *pResult);
    BOOL    Select(const char *szId, int nPortNum, DBFetchData *pResult);
    BOOL    GetId(const EUI64 *pId, int nPortNum, UINT *nMeterId);
    BOOL    GetId(const char *szId, int nPortNum, UINT *nMeterId);

    BOOL    Update(const UINT nModemId, BYTE nPortNum, const char *szSerial, 
                const UINT nParserType, const int nServiceType, const int nVendor, 
                const double hwVer, const double swVer, const int nLpPeriod);
    BOOL    Update(const EUI64 *pModemId, BYTE nPortNum, const char *szSerial, 
                const UINT nParserType, const int nServiceType, const int nVendor, 
                const double hwVer, const double swVer, const int nLpPeriod);
    BOOL    Update(const char *szModemId, BYTE nPortNum, const char *szSerial, 
                const UINT nParserType, const int nServiceType, const int nVendor, 
                const double hwVer, const double swVer, const int nLpPeriod);

    BOOL    Delete();
    BOOL    Delete(const EUI64 *pId, int nPortNum);
    BOOL    Delete(const char *szId, int nPortNum);

    BOOL    TryUpload(DBFetchData *pResult);
    BOOL    UploadComplete(int nCount, UINT *pId);

    METERTBLINS *GetData(Row *pRow);

protected:
    BOOL    InitializeDB();
    BOOL    UpgradeDB();
    BOOL    FreeData(void *pData);
    BOOL    DumpData(void *pData);
    BOOL    _Fetch(CppSQLite3Query *result, DBFetchData *pResult);
};

extern CMeterHelper	*m_pMeterHelper;

#endif	// __METER_HELPER_H__
