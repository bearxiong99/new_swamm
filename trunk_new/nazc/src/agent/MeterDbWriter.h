#ifndef __METER_DB_WRITER_H__
#define __METER_DB_WRITER_H__

#include "MeterWriter.h"

class CMeterDbWriter : public CMeterWriter
{
public:
	CMeterDbWriter();
	virtual ~CMeterDbWriter();

// Operations
public:
	BOOL	AddData(ENDIENTRY *pEndDevice, EUI64 *id, BYTE *pszData, int nLength);

protected:
	void	WriteMeteringData(METERWRITEITEM *pItem);
	BOOL	SendToServer(const char *pszFileName);
	void	SendMeteringData(METERWRITEITEM *pItem);
};

extern CMeterDbWriter *m_pMeterDbWriter;

#endif	// __METER_DB_WRITER_H__
