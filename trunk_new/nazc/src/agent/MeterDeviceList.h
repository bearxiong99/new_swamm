#ifndef __METERDEIVCE_HASH_H__
#define __METERDEIVCE_HASH_H__

#include "Locker.h"
#include "LinkedList.h"

typedef struct	_tagMETERDEVICENODE
{

        //Basic infomation
		GEUI64		gid;						// EUI ID
        char        szSerial[22];           // serial num
        char        szMenufa[22];           // menufacture
        BYTE        szGener;                //generation of the heat meter
        BYTE        szMedia;
        BYTE        szCntAccess;
        BYTE		szstatus;					// Status code 
        time_t      tmMetering;               // 마지막 검침 성공 시간
        BOOL        bRemove;                // 삭제 여부

        //Network Quality
        BYTE        LQI;                        // LQI
        signed char RSSI;                       // RSSI
		int			nPosition;				// 리스트 위치
}	METERDEVICENODE, *PMETERDEVICENODE;

typedef BOOL (*PFNENUMMETERDEVICE)(METERDEVICENODE *pMeterDevice, void *pParam, void *pParam1, void *pParam2);
class CMeterDeviceList
{
public:
	CMeterDeviceList();
	virtual ~CMeterDeviceList();

// Operations
public:
    BOOL    	Initialize();
    void    	Destroy();

public:
	BOOL		Add(const EUI64 *id, BYTE nPort, char *serial, BYTE *menu, BYTE gener, BYTE media, BYTE access, BYTE status);
	BOOL		Remove(const EUI64 *id, BYTE port);
    BOOL        Remove(const EUI64 *id);
	void		RemoveAll();
    BOOL        DeleteMeterDeviceAll();
    BOOL        LoadMeterDevice(const char *pszPath);
    BOOL        SaveMeterDevice(METERDEVICENODE *pMeterDevice);
    int         EnumMeterDevice(PFNENUMMETERDEVICE pfnCallback, void *pParam, void *pParam1, void *pParam2);
    METERDEVICENODE *GetMeter(const EUI64 *id, BYTE port);
	METERDEVICENODE *GetMeter(const EUI64 *id, METERDEVICENODE * prev);

    void SetRemove(const EUI64 *id);
    void ResetRemove(const EUI64 *id, BYTE nPort);
    void Clear(const EUI64 *id);

protected:
	METERDEVICENODE *FindNode(const GEUI64 *gid);
	METERDEVICENODE *FindNode(const EUI64 *id);
	METERDEVICENODE *FindNode(const EUI64 *id, METERDEVICENODE * prev);

private:
	CLocker		m_MeterLocker;
	CLinkedList<METERDEVICENODE *>	m_List;
};

extern CMeterDeviceList	*m_pMeterDeviceList;

#endif	// __METERDEIVCE_HASH_H__
