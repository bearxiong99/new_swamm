#ifndef __DLMS_DATA_STREAM_H__
#define __DLMS_DATA_STREAM_H__

#include "DataStream.h"

class CDlmsDataStream : public CDataStream
{
public:
	CDlmsDataStream();
	virtual ~CDlmsDataStream();

public:
	BOOL	ParseStream(DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData);

protected:
	virtual BOOL OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, int nRemain, void *pCallData);
};

#endif // __DLMS_DATA_STREAM_H__
