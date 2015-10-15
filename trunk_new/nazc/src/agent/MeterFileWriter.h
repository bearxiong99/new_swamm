#ifndef __METER_FILE_WRITER_H__
#define __METER_FILE_WRITER_H__

#include "MeterWriter.h"

class CMeterFileWriter : public CMeterWriter
{
public:
	CMeterFileWriter();
	virtual ~CMeterFileWriter();

// Operations
public:
	BOOL	AddData(ENDIENTRY *pEndDevice, EUI64 *id, BYTE *pszData, int nLength);

protected:
	void	WriteMeteringData(METERWRITEITEM *pItem);
	BOOL	SendToServer(const char *pszFileName);
	void	SendMeteringData(METERWRITEITEM *pItem);

private:
	char		m_szWriteBuffer[256];
};

extern CMeterFileWriter *m_pMeterFileWriter;

#endif	// __METER_FILE_WRITER_H__
