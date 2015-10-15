#ifndef __CODI_DEVICE_CLASS_H__
#define __CODI_DEVICE_CLASS_H__

class CDeviceClass
{
public:
	CDeviceClass();
	virtual ~CDeviceClass();

public:
	virtual int GetFD(COORDINATOR *codi);
	virtual int StartDevice(COORDINATOR *codi, CODIDEVICE *pDevice);
	virtual int StopDevice(COORDINATOR *codi);
	virtual void SetPassive(COORDINATOR *codi, BOOL bPassive);
	virtual BOOL EnableFlowControl(COORDINATOR *codi, BOOL bEnable);
	
	virtual int Send(COORDINATOR *codi, BYTE *pBuffer, int nLength);

	virtual void OnPrevSend(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	virtual void OnSend(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	virtual void OnRecv(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	virtual void OnError(COORDINATOR *codi, int nError);

protected:
	COORDINATOR	*m_selfCoordinator;
};

#endif	// __CODI_DEVICE_CLASS__
