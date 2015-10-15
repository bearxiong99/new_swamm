#ifndef __MESSAGE_HELPER_H__
#define __MESSAGE_HELPER_H__

#include "TimeoutThread.h"
#include "DatabaseHelper.h"

#include "if4type.h"

#define MESSAGE_TYPE_ALL            0
#define MESSAGE_TYPE_IMMEDIATELY    1
#define MESSAGE_TYPE_LAZY           2
#define MESSAGE_TYPE_PASSIVE        3

class CMessageHelper : public CTimeoutThread, public CDatabaseHelper
{
public:
	CMessageHelper();
	CMessageHelper(const char * szDbFile);
	virtual ~CMessageHelper();

// Operations
public:
    BOOL    Initialize();
    void    Destroy();
    BOOL    Add(const char *szId, UINT nMessageId, BYTE nMessageType, UINT nDuration,
                UINT nErrorHandler, UINT nPreHandler, UINT nPostHandler, 
                UINT nUserData, int nDataLength, const BYTE *pszData);
    BOOL    Select(const char *szId, BYTE nMessageType, IF4Wrapper *pWrapper);
    BOOL    Delete(const char *szId, BYTE nMessageType);

protected:
	BOOL	OnTimeoutThread();
	BOOL	OnActiveThread();

    BOOL    InitializeDB();
    BOOL    UpgradeDB();

};

extern CMessageHelper	*m_pMessageHelper;

#endif	// __MESSAGE_HELPER_H__
