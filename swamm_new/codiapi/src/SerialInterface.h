#ifndef __CODI_SERIAL_INTERFACE_H__
#define __CODI_SERIAL_INTERFACE_H__

#include "DeviceClass.h"
#include "SerialServer.h"

class CSerialInterface : public CSerialServer, public CDeviceClass
{
public:
	CSerialInterface();
	virtual ~CSerialInterface();

public:
	int GetFD(COORDINATOR *codi);
	int StartDevice(COORDINATOR *codi, CODIDEVICE *pDevice);
	int StopDevice(COORDINATOR *codi);
	void SetPassive(COORDINATOR *codi, BOOL bPassive);
	BOOL EnableFlowControl(COORDINATOR *codi, BOOL bEnable);

	int Send(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	void ModulePowerControl(BOOL bOnOff);

protected:
	void OnPrevSend(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	void OnSend(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	void OnRecv(COORDINATOR *codi, BYTE *pBuffer, int nLength);
	void OnError(COORDINATOR *codi, int nError);

protected:
    BOOL OnReceiveSession(WORKSESSION *pSession, char* pszBuffer, int nLength);
    void OnPrevSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength);
    void OnSendSession(WORKSESSION *pSession, char* pszBuffer, int nLength);
    void OnErrorSession(WORKSESSION *pSession, int nError);
};

#endif	// __CODI_SERIAL_INTERFACE_H__
