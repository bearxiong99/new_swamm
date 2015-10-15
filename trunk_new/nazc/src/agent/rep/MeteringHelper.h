#ifndef __METERING_HELPER_H__
#define __METERING_HELPER_H__

#include "typedef.h"
#include "DatabaseHelper.h"

#define CHANNEL_MAX         32
#define LP_MAX              512
#define METERING_MAX        256

typedef struct {
    UINT            _id;
    UINT            _meteringId;
    TIMESTAMP       _snapShotTime;
    unsigned int*   pValue;
    int             nTryUpload;
    TIMESTAMP       uploadTime;

    TIMESTAMP       lastUpdate;
} LOADPROFILEINS;

typedef struct {
    UINT            _id;

    TIMESTAMP       meteringTime;
    TIMESTAMP       meterTime;
    int             nMeterTimeOffset;
    unsigned long long* pLastValue;
    TIMESTAMP       baseTime;
    unsigned long long* pBaseValue;

    int             nLpCnt;
    LOADPROFILEINS  *pLoadProfile[LP_MAX];

    TIMESTAMP       lastUpdate;
} METERINGINS;

typedef struct {
    UINT            _meterId;
    UINT            _modemId;
    char            szModemId[16+1];    // Modem Id
    EUI64           modemId;            // Modem Id
    BYTE            nPortNum;           // Meter Port Number

    int             nChannelCnt;

    int             nTotLpCnt;
    int             nMeteringCnt;

    METERINGINS     *pMetering[METERING_MAX];
} METERINGDATAINS;

class CMeteringHelper : public CDatabaseHelper
{
public:
	CMeteringHelper();
	CMeteringHelper(const char * szDbFile);
	virtual ~CMeteringHelper();

// Operations
public:
    BOOL    Initialize();

    BOOL    Select(DBFetchData *pResult);
    BOOL    Select(TIMESTAMP *pFrom, TIMESTAMP *pTo, DBFetchData *pResult);

    BOOL    Select(const EUI64 *pId, int nPortNum, DBFetchData *pResult);
    BOOL    Select(const char *szId, int nPortNum, DBFetchData *pResult);

    BOOL    Select(const char *szModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo, 
                DBFetchData *pResult);

    BOOL    Update(const EUI64 *pModemId, int nPortNum,
                TIMESTAMP *meterTime, int meterTimeOffset, TIMESTAMP *baseTime,
                int nChannelCnt, int nLpCnt, METERINGINS *pMeteringIns
        );
    BOOL    Update(const char *szModemId, int nPortNum,
                TIMESTAMP *meterTime, int meterTimeOffset, TIMESTAMP *baseTime,
                int nChannelCnt, int nLpCnt, METERINGINS *pMeteringIns
        );
    BOOL    Update(const UINT nMeterId, 
                TIMESTAMP *meterTime, int meterTimeOffset, TIMESTAMP *baseTime,
                int nChannelCnt, int nLpCnt, METERINGINS *pMeteringIns
        );

    BOOL    Delete();

    BOOL    Delete(const EUI64 *pModemId, int nPortNum);
    BOOL    Delete(const char *szModemId, int nPortNum);

    BOOL    Delete(const EUI64 *pModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo);
    BOOL    Delete(const char *szModemId, int nPortNum, TIMESTAMP *pFrom, TIMESTAMP *pTo);

    BOOL    TryUpload(DBFetchData *pResult);
    BOOL    UploadComplete(int nCount, UINT *pId);

    METERINGDATAINS *GetData(Row *pRow);

    static void FreeMeteringIns(METERINGINS* meteringIns);
    static void FreeLoadProfileIns(LOADPROFILEINS* lpIns);
    //void DumpData(METERINGDATAINS *pDatas);

protected:
    BOOL    InitializeDB();
    BOOL    UpgradeDB();
    BOOL    FreeData(void *pData);
    BOOL    DumpData(void *pData);
    BOOL    DumpValue(char *str, int valueSize, int count, void *pValue);

    BOOL    _DeleteMetering(const char *szModemId, int nPortNum, const char *fromDateStr, const char *toDateStr);
    BOOL    _DeleteDayLastPulse(const char *szModemId, int nPortNum, const char *fromDateStr, const char *toDateStr);
    BOOL    _DeleteLoadProfile(const char *szModemId, int nPortNum, const char *fromTimeStr, const char *toTimeStr);

};

extern CMeteringHelper	*m_pMeteringHelper;

#endif	// __METERING_HELPER_H__
