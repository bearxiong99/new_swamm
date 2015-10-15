#ifndef __LP_WRITER_H__
#define __LP_WRITER_H__

#include "TimeoutThread.h"
#include "LinkedList.h"
#include "EndDeviceList.h"
#include "MeterWriter.h"
#include "Locker.h"

typedef void (*LpWriterCallback)(EUI64 *id, BYTE *data, UINT nLength);

typedef struct	_tagLPWRITEITEM
{
		EUI64			    id;						// EUI ID
		int				    nSize;					// 구조체의 길이
        BOOL                isLast;                 //
        ENDIENTRY           *pEntry;                //
        LpWriterCallback    pCallback;              // Callback Handler
        CMeterWriter        *pMeterWriter;          // Meter Writer
		BYTE	            *pData;					// 데이터
		int				    nPosition;				// 리스트 위치
}	LPWRITEITEM, *PLPWRITEITEM;

class CLpWriter : public CTimeoutThread
{
public:
	CLpWriter();
	virtual ~CLpWriter();

// Operations
public:
    BOOL    Initialize();
    void    Destroy();

    BOOL    ClearData(EUI64 *id);
	BOOL	AddData(EUI64 *id, BYTE *pszData, int nLength, BOOL isLast=FALSE, 
                ENDIENTRY * pEntry = NULL, 
                CMeterWriter *pMeterWriter = NULL,
                LpWriterCallback callback = NULL);
	BOOL	WriteLpData(LPWRITEITEM *pItem);

protected:
    BOOL    ReadLpData(EUI64 * id, BYTE ** data, int * len);
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

private:
	CLocker		m_Locker;
	CLinkedList<LPWRITEITEM *>	m_List;	
};

extern CLpWriter	*m_pLpWriter;

#endif	// __LP_WRITER_H__
