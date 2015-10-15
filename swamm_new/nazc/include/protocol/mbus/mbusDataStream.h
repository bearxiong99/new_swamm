#ifndef __MBUS_DATA_STREAM_H__
#define __MBUS_DATA_STREAM_H__

#include "DataStream.h"

class CMbusDataStream : public CDataStream
{
public:
	CMbusDataStream();
	virtual ~CMbusDataStream();

public:
	BOOL	ParseStream(DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData);

protected:
	virtual BOOL OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData);
};

#endif // __DLMS_DATA_STREAM_H__
