#ifndef __MODEM_HELPER_H__
#define __MODEM_HELPER_H__

#include "typedef.h"
#include "DatabaseHelper.h"


typedef struct {
    UINT        _id;
    char        *szModel;
    char        szId[16+1];
    EUI64       id;
    double      hwVer;
    double      swVer;
    int         nBuild;
    TIMESTAMP   installTime;
    TIMESTAMP   lastUpdate;
} MODEMTBLINS;

class CModemHelper : public CDatabaseHelper
{
public:
	CModemHelper();
	CModemHelper(const char * szDbFile);
	virtual ~CModemHelper();

// Operations
public:
    BOOL    Initialize();

    BOOL    GetId(const EUI64 *pId, UINT *nModemId);
    BOOL    GetId(const char *szId, UINT *nModemId);
    BOOL    Select(const EUI64 *pId, DBFetchData *pResult);
    BOOL    Select(const char *szId, DBFetchData *pResult);
    BOOL    Select(DBFetchData *pResult);

    BOOL    Update(const EUI64 *pId, const char *szModel, const double hwVer, const double swVer, const int nBuild);
    BOOL    Update(const char *szId, const char *szModel, const double hwVer, const double swVer, const int nBuild);

    BOOL    Delete(const EUI64 *pId);
    BOOL    Delete(const char *szId);
    BOOL    Delete();

    MODEMTBLINS *GetData(Row *pRow);

protected:
    BOOL    InitializeDB();
    BOOL    UpgradeDB();
    BOOL    FreeData(void *pData);
    BOOL    DumpData(void *pData);
};

extern CModemHelper	*m_pModemHelper;

#endif	// __MODEM_HELPER_H__
