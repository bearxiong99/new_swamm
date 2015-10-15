#ifndef __CODI_VIRTUAL_INTERFACE_H__
#define __CODI_VIRTUAL_INTERFACE_H__

#include "DeviceClass.h"
#include "TimeoutThread.h"

class CVirtualInterface : public CTimeoutThread, public CDeviceClass
{
public:
	CVirtualInterface();
	virtual ~CVirtualInterface();

public:
	int GetFD(COORDINATOR *codi);
	int StartDevice(COORDINATOR *codi, CODIDEVICE *pDevice);
	int StopDevice(COORDINATOR *codi);

	int Send(COORDINATOR *codi, BYTE *pBuffer, int nLength);

	void OnSend(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	void OnRecv(COORDINATOR *codi, BYTE *pBuffer, int nLength);

    BOOL OnActiveThread();
	BOOL OnTimeoutThread();
};

#endif	// __CODI_VIRTUAL_INTERFACE_H__
