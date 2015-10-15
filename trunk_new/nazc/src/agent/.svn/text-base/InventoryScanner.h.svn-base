#ifndef __INVENTORY_SCANNER_H__
#define __INVENTORY_SCANNER_H__

#include "Locker.h"
#include "LinkedList.h"
#include "TimeoutThread.h"

#define MAX_ROUTERECORD_CACHE_COUNT		20

#define SCAN_MASK_NODE_INFO				0x0001
#define SCAN_MASK_AMR_EXTRA_INFO		0x0002
#define SCAN_MASK_AMR_GROUP_INFO		0x0004
#define SCAN_MASK_LEAVE			        0x8000

#define SCAN_MASK_ALL					(SCAN_MASK_NODE_INFO | SCAN_MASK_AMR_EXTRA_INFO)
#define SCAN_MASK_DEFAULT				SCAN_MASK_ALL

typedef struct  _tagAMR_EXTRA_INFO
{
        BYTE            permit_mode;
        BYTE            permit_state;
        BYTE            metering_fail_cnt;
        WORD            alarm_mask;
        BYTE            network_type;
} __ATTRIBUTE_PACKED__ AMR_EXTRA_INFO;

typedef struct	_tagSCANITEM
{
		EUI64				id;						// EUI ID
		WORD				parentid;				// Parent node short-id
		WORD				shortid;				// Short ID
		BYTE				hops;					// Hop count
		WORD				path[CODI_MAX_ROUTING_PATH];				// Routing path
		int					mask;					// Request mask
		int					recvmask;				// Received mask
		ENDI_NODE_INFO		node;					// Node information
		AMR_EXTRA_INFO		amrExtra;				// AMR Extra Information
		BOOL				bDone;					// 성공 여부
		time_t				scanTime;				// 스캔 요청 시간
		int				    timeout;				// Connection Timeout
        BYTE                LQI;
        signed char         RSSI;
		int					nPosition;				// 리스트 위치
}	SCANITEM, *PSCANITEM;

typedef struct	_tagCACHEROUTERECORD
{
		EUI64				id;						// EUI ID
		WORD				shortid;				// Short ID
		BYTE				hops;					// Hop count
		WORD				path[CODI_MAX_ROUTING_PATH];// Routing path
}	CACHEROUTERECORD;

class CInventoryScanner : public CTimeoutThread
{
public:
	CInventoryScanner();
	virtual ~CInventoryScanner();

// Operations
public:
    BOOL    	Initialize();
    void    	Destroy();

public:
	BOOL		Add(const EUI64 *id, BYTE LQI=0, signed char RSSI=0, int mask=SCAN_MASK_DEFAULT, int timeout=6000);
	BOOL		Delete(const EUI64 *id);
	void		RemoveAll();

	void		CacheRouteRecord(const EUI64 *id, WORD shortid, BYTE hops, WORD *path);
	BOOL		GetCacheRouteRecord(SCANITEM *pItem);
	BOOL		GetCacheRouteRecord(const EUI64 *id, WORD *shortid, BYTE *hops, WORD *path);

protected:
	void		ScanWorker(SCANITEM *pItem);
	SCANITEM 	*FindItem(const EUI64 *id);

protected:
	BOOL		OnActiveThread();
	BOOL		OnTimeoutThread();

private:
	time_t		m_tmLastCheck;
	int			m_nCacheCount;
	CLocker		m_Locker;
	CLocker		m_CacheLocker;
	CACHEROUTERECORD		*m_pCacheList[MAX_ROUTERECORD_CACHE_COUNT];
	CACHEROUTERECORD		m_arCacheNode[MAX_ROUTERECORD_CACHE_COUNT];
	CLinkedList<SCANITEM *>	m_List;
};

extern CInventoryScanner	*m_pInventoryScanner;

#endif	// __INVENTORY_SCANNER_H__
