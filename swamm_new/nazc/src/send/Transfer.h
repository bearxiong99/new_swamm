#ifndef __TRANSFER_H__
#define __TRANSFER_H__

#include "MobileClient.h"

typedef struct  _tagMETERINGDATA
{
        EUI64       id;
        char        mid[20];
        BYTE        stype;
        BYTE        svc;
        BYTE        vendor;
        WORD        dataCnt;
		WORD		length;
		TIMESTAMP	timestamp;					// 년월일시분초
		char		data[0];					// 실제 데이터
}   __attribute__ ((packed)) METERINGDATA;

class CTransfer
{
public:
	CTransfer();
	virtual ~CTransfer();

public:
	int 	ConnectToHost(CMobileClient *pClient, char *pszNumber, BOOL bCDMA, int nPort);
	int 	SendEvent(CMobileClient *pClient, char *pszFileName);
	int 	SendMetering(CMobileClient *pClient, char *pszFileName);
	int		Transfer(CMobileClient *pClient, char *pszType, int nNzcNumber, int nPort, char *pszFileName, int nTryTime, BOOL bCompress=FALSE);
	int		SetupModem(CMobileClient *pClient);
	int		GetDCD();

protected:
	void 	SendAck(CMobileClient *pClient, BYTE nSeq);
	void 	SendNak(CMobileClient *pClient, BYTE nSeq);
	void 	SendEot(CMobileClient *pClient);

	void 	LoadPhoneList();
	void	ResetModem();
	void	IgtControl();
	void 	DisconnectModem();
	int		GetBatteryState();

	void	AddMeteringLog(const char *fmt, ...);
	void 	acceptstr(char *dest, char *src, int len);
	void 	Message(char *pszMessage);
	int		MakeFrame(char *pszBuffer, char *pszData, int len, BYTE nSeq);

protected:
	BOOL	m_bCompress;
	BOOL	m_bEOF;
	int		m_nNzcNumber;
	int		m_nSentLine;
	int		m_nGoodLine;
	char	*m_pszReadBuffer;
};

#endif	// __TRANSFER_H__

