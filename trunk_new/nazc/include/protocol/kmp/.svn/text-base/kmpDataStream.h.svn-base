#ifndef __KMP_DATA_STREAM_H__
#define __KMP_DATA_STREAM_H__

#include "DataStream.h"

class CKmpDataStream : public CDataStream
{
public:
	CKmpDataStream();
	virtual ~CKmpDataStream();

public:
	BOOL	ParseStream(DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData);

protected:
	virtual BOOL OnFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData);
};

#endif // __KMP_DATA_STREAM_H__
