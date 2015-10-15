#ifndef __METER_SECURITY_HELPER_H__
#define __METER_SECURITY_HELPER_H__

#include "typedef.h"
#include "DatabaseHelper.h"

#define     SECURITY_TYPE_PARSER        "PARSER"
#define     SECURITY_TYPE_NODE          "NODE"
#define     SECURITY_TYPE_ID            "ID"

typedef struct {
    UINT            _id;
    char            szType[64];
    char            szName[128];
    unsigned char   *pKey;
    int             nKeyLen;

    TIMESTAMP       lastUpdate;
} METERSECURITYDATAINS;

class CMeterSecurityHelper : public CDatabaseHelper
{
public:
	CMeterSecurityHelper();
	CMeterSecurityHelper(const char * szDbFile);
	virtual ~CMeterSecurityHelper();

// Operations
public:
    BOOL    Initialize();

    BOOL    Select(DBFetchData *pResult);
    BOOL    Select(const char *szType, const char *szName, DBFetchData *pResult);
    BOOL    Select(const char *szParserName, const char *szNodeKind, const EUI64 *pModemId, DBFetchData *pResult);
    BOOL    Select(const char *szParserName, const char *szNodeKind, const char *pModemId, DBFetchData *pResult);

    BOOL    Update(const char *szType, const char *szName, const BYTE *pKey, int nKeyLen);

    BOOL    Delete();
    BOOL    Delete(const char *szType, const char * szName);

    METERSECURITYDATAINS *GetData(Row *pRow);

protected:
    BOOL    InitializeDB();
    BOOL    UpgradeDB();
    BOOL    FreeData(void *pData);
    BOOL    DumpData(void *pData);

};

extern CMeterSecurityHelper	*m_pMeterSecurityHelper;

#endif	// __METER_SECURITY_HELPER_H__
