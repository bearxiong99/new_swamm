#ifndef __UPDATE_SERVICE_H__
#define __UPDATE_SERVICE_H__

#define MOBILE_GSM				0
#define	MOBILE_CDMA				1

typedef struct	_tagMFRAMEHEADER
{
		BYTE		ident1;				// '['
		BYTE		ident2;				// '@'
		BYTE		seq;				// Sequence
		WORD		len;				// data length
}	__attribute__ ((packed)) MFRAMEHEADER, *PMFRAMEHEDER;

typedef struct	_tagMFRAMETAIL
{
		WORD		crc;				// Crc16
}	__attribute__ ((packed)) MFRAMETAIL, *PMFRAMETAIL;

typedef struct	_tagMFRAME
{
		MFRAMEHEADER	hdr;
		BYTE			data[4096];
		MFRAMETAIL		tail;
}	__attribute__ ((packed)) MFRAME, *PMFRAME;

#include "MobileClient.h"

class CUpdateService
{
public:
	CUpdateService();
	~CUpdateService();

public:
	int		Updator(char *pszNumber, char *pszFileName);

protected:
	int		GetFileSize(char *pszFileName);
	int		ConnectToHost(CMobileClient *pClient, char *pszNumber, char *pszFileName);
	int		Upload(CMobileClient *pClient, char *pszNumber, char *pszFileName);
	void	acceptstr(char *dest, char *src);
	void	Message(char *pszMessage);
	void	DisplayProgress(int nTotal, int nCurrent, BOOL bError, int nRetry=0);

protected:
	int		m_nLength;
	MFRAME	send_frame;
	MFRAME	recv_frame;
	int		m_nToggle;
};

#endif	// __UPDATE_SERVICE_H__
