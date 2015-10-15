#ifndef __GROUP_HELPER_H__
#define __GROUP_HELPER_H__

#include "typedef.h"
#include "DatabaseHelper.h"

#define ENTRY_MAX        1024

typedef struct {
    UINT            _id;
    EUI64           targetId;
    char            szTargetId[16+1];    // Target Id

    TIMESTAMP       lastUpdate;
} GROUPENTRYINS;

typedef struct {
    UINT            _id;
    char            szType[64];
    char            szName[128];

    TIMESTAMP       lastUpdate;

    int             nEntryCnt;
    GROUPENTRYINS   *pEntry[ENTRY_MAX];
} GROUPDATAINS;

class CGroupHelper : public CDatabaseHelper
{
public:
	CGroupHelper();
	CGroupHelper(const char * szDbFile);
	virtual ~CGroupHelper();

// Operations
public:
    BOOL    Initialize();

    BOOL    Select(DBFetchData *pResult);
    BOOL    Select(const char *szType, DBFetchData *pResult);
    BOOL    Select(const char *szType, const char *szName, DBFetchData *pResult);
    BOOL    Select(const char *szType, const char *szName, const EUI64 *pModemId, DBFetchData *pResult);
    BOOL    Select(const char *szType, const char *szName, const char *szModemId, DBFetchData *pResult);

    BOOL    Update(const char *szType, const char * szName, const EUI64 *pModemId);
    BOOL    Update(const char *szType, const char * szName, const char *szModemId);

    BOOL    Delete();
    BOOL    Delete(const char *szType);
    BOOL    Delete(const char *szType, const char * szName);
    BOOL    Delete(const char *szType, const char * szName, const EUI64 *pModemId);
    BOOL    Delete(const char *szType, const char * szName, const char *szModemId);

    GROUPDATAINS *GetData(Row *pRow);

protected:
    BOOL    InitializeDB();
    BOOL    UpgradeDB();
    BOOL    FreeData(void *pData);
    BOOL    DumpData(void *pData);

};

extern CGroupHelper	*m_pGroupHelper;

#endif	// __GROUP_HELPER_H__
