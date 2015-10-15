#ifndef __END_DEVICE_POOL_H__
#define __END_DEVICE_POOL_H__

#define ENDI_MAX_POOL_COUNT		16

#include "Locker.h"

class CEndDevicePool
{
public:
	CEndDevicePool();
	virtual ~CEndDevicePool();

public:
	void		InitPool();
	void 		ResetPool();

	int			NewEndDevice(COORDINATOR *codi, ENDDEVICE **endi, EUI64 *id);
	void		DeleteEndDevice(ENDDEVICE *endi);
	ENDDEVICE	*GetEndDeviceByID(EUI64 *id);

	void	 	EndDeviceNotifyStackStatus(BYTE status);
	void		EndDeviceSendCallback(COORDINATOR *codi, ENDI_DATA_FRAME *pFrame, int nLength); 
	BOOL		EndDeviceRecvCallback(COORDINATOR *codi, ENDI_DATA_FRAME *pFrame, int nLength, int nTLength, BYTE LQI, signed char RSSI); 

protected:
	ENDDEVICE	*AllocateEndDevice(COORDINATOR *codi, EUI64 *id);
	void		FreeEndDevice(ENDDEVICE *endi);
	ENDDEVICE	*FindEndDeviceByID(EUI64 *id);
	ENDDEVICE 	*FindEndDeviceAndLockByID(EUI64 *id);
	void		ResetConnection();

    BOOL        IsValidSeq(BYTE previous, BYTE current);
    BOOL        IsPossibleSeq(BYTE previous, BYTE current);
    int         CompareSeq(BYTE previous, BYTE current);

    FRAMEDATA*  GetNextData(EUI64 * id);
    FRAMEDATA*  GetNextData(ENDDEVICE *pEndDevice);
    int         AddDataBuffer(ENDDEVICE *pEndDevice, ENDI_DATA_FRAME *pFrame, int nLength, int nTLength, BYTE LQI, signed char RSSI);
    int         ClearDataBuffer(ENDDEVICE *endi);

	void		LockEndDevice(ENDDEVICE *pEndDevice);
	void		UnlockEndDevice(ENDDEVICE *pEndDevice);

public:
	CLocker		m_lockerPool;
	int			m_nPoolAllocate;
	ENDDEVICE	m_listEndDevice[ENDI_MAX_POOL_COUNT];
};

#endif	// __END_DEVICE_POOL_H__
