#ifndef __METER_WRITER_H__
#define __METER_WRITER_H__

#include "TimeoutThread.h"
#include "LinkedList.h"
#include "EndDeviceList.h"
#include "Variable.h"
#include "Chunk.h"
#include "Locker.h"

typedef struct	_tagMETERWRITEITEM
{
		EUI64			id;						// EUI ID
		int				nSize;					// 구조체의 길이
		void            *pData;					// 데이터
		int				nPosition;				// 리스트 위치
}	METERWRITEITEM, *PMETERWRITEITEM;

class CMeterWriter : public CTimeoutThread
{
public:
	CMeterWriter();
	virtual ~CMeterWriter();

// Operations
public:
    BOOL    Initialize();
    void    Destroy();

	virtual BOOL AddData(ENDIENTRY *pEndDevice, EUI64 *id, BYTE *pszData, int nLength)=0;

protected:
	virtual void SendMeteringData(METERWRITEITEM *pItem)=0;
	virtual void WriteMeteringData(METERWRITEITEM *pItem)=0;

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

protected:
	CLocker		m_Locker;
	CLinkedList<METERWRITEITEM *>	m_List;	
};

#endif	// __METER_WRITER_H__
