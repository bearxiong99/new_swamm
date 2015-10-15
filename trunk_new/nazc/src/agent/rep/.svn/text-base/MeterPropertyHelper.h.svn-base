#ifndef __METER_PROPERTY_HELPER_H__
#define __METER_PROPERTY_HELPER_H__

#include "typedef.h"
#include "DatabaseHelper.h"

#define     PROPERTY_TYPE_PARSER        "PARSER"
#define     PROPERTY_TYPE_NODE          "NODE"
#define     PROPERTY_TYPE_ID            "ID"

//                                              0        1         2         3
//                                              123456789012345678901234567890123456
#define     PROPERTY_GRP_SECURITY              "SECURITY"
#define     PROPERTY_NAME_SECURITY             "METER_SECURITY"

#define     PROPERTY_GRP_METERING_PARAMETER    "METERING_PARAM"
#define     PROPERTY_NAME_OPTION               "METERING_OPTION"
#define     PROPERTY_NAME_OFFSET               "METERING_OFFSET"
#define     PROPERTY_NAME_COUNT                "METERING_COUNT"

#define     PROPERTY_GRP_DLMS_ADDRESS          "DLMS"
#define     PROPERTY_NAME_DLMS_SERVER_ADDR     "SERVER_ADDR"
#define     PROPERTY_NAME_DLMS_BROADCAST       "BROADCAST_ADDR"
#define     PROPERTY_NAME_DLMS_CLIENT_ADDR     "CLIENT_ADDR"

typedef struct {
    UINT            _id;
    char            szGroup[64];        // Property Group
    char            szProperty[64];     // Property Name
    char            szType[16];         // Type (PARSER, NODE, ID)
    char            szKey[128];         // Key String
    unsigned char   *pValue;            // Value
    int             nValueLen;          // Value Length

    TIMESTAMP       lastUpdate;
} METERPROPERTYDATAINS;

class CMeterPropertyHelper : public CDatabaseHelper
{
public:
	CMeterPropertyHelper();
	CMeterPropertyHelper(const char * szDbFile);
	virtual ~CMeterPropertyHelper();

// Operations
public:
    BOOL    Initialize();

    BOOL    Select(DBFetchData *pResult);
    BOOL    Select(const char *szGroup, const char *szProperty, DBFetchData *pResult);
    BOOL    Select(const char *szGroup, const char *szProperty, 
            const char *szParserName, const char *szNodeKind, const EUI64 *pModemId, DBFetchData *pResult);
    BOOL    Select(const char *szGroup, const char *szProperty, 
            const char *szParserName, const char *szNodeKind, const char *pModemId, DBFetchData *pResult);
    BOOL    SelectAll(const char *szGroup, const char *szProperty, 
            const char *szParserName, const char *szNodeKind, const EUI64 *pModemId, DBFetchData *pResult);
    BOOL    SelectAll(const char *szGroup, const char *szProperty, 
            const char *szParserName, const char *szNodeKind, const char *pModemId, DBFetchData *pResult);

    BOOL    Select(const char *szGroup, const char *szProperty, const char *szType, const char *szKey, DBFetchData *pResult);

    BOOL    Update(const char *szGroup, const char *szProperty, const char *szType, const char *szKey, const BYTE *pValue, int nValueLen);

    BOOL    Delete();
    BOOL    Delete(const char *szGroup, const char *szProperty);
    BOOL    Delete(const char *szGroup, const char *szProperty, const char *szType, const char * szKey);

    METERPROPERTYDATAINS *GetData(Row *pRow);

protected:
    BOOL    InitializeDB();
    BOOL    UpgradeDB();
    BOOL    FreeData(void *pData);
    BOOL    DumpData(void *pData);

};

extern CMeterPropertyHelper	*m_pMeterPropertyHelper;

#endif	// __METER_PROPERTY_HELPER_H__
