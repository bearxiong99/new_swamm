#ifndef __VARIABLE_HELPER_H__
#define __VARIABLE_HELPER_H__

#include "typedef.h"
#include "DatabaseHelper.h"

typedef struct {
    UINT        _id;
    int         nNameLen;
    char        *szName;
    int         nValueLen;
    char        *szValue;
    TIMESTAMP   lastUpdate;
} VARIABLETBLINS;

class CVariableHelper : public CDatabaseHelper
{
public:
	CVariableHelper();
	CVariableHelper(const char * szDbFile);
	virtual ~CVariableHelper();

// Operations
public:
    BOOL    Initialize();

    BOOL    Select(DBFetchData *pResult);
    BOOL    Select(const char *szName, DBFetchData *pResult);

    BOOL    Update(const char *szName, const int nValue); 
    BOOL    Update(const char *szName, const UINT nValue); 
    BOOL    Update(const char *szName, const char *szValue); 

    BOOL    Delete();
    BOOL    Delete(const char *szName);

    VARIABLETBLINS *GetData(Row *pRow);

protected:
    BOOL    InitializeDB();
    BOOL    UpgradeDB();
    BOOL    FreeData(void *pData);
    BOOL    DumpData(void *pData);
};

extern CVariableHelper	*m_pVariableHelper;

#endif	// __VARIABLE_HELPER_H__
