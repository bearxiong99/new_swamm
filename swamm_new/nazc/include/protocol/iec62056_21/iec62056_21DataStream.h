#ifndef __IEC62056_21_DATA_STREAM_H__
#define __IEC62056_21_DATA_STREAM_H__

#include "DataStream.h"

class CIec21DataStream : public CDataStream
{
public:
	CIec21DataStream();
	virtual ~CIec21DataStream();

public:
	BOOL	ParseStream(DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData);

protected:
    virtual BOOL OnAck(DATASTREAM *pStream, void *pCallData);
    virtual BOOL OnNak(DATASTREAM *pStream, void *pCallData);
    virtual BOOL OnBreakFrame(DATASTREAM *pStream, void *pCallData);
    virtual BOOL OnIdentificationFrame(DATASTREAM *pStream, BYTE* manufacturerId, BYTE baud, BYTE *serialSetting, BYTE * identification, 
        int nLength, void *pCallData);
    virtual BOOL OnCommandFrame(DATASTREAM *pStream, BYTE cmd, BYTE cmdType, BYTE *pszBuffer, int nLength, void *pCallData);
    virtual BOOL OnDataFrame(DATASTREAM *pStream, BYTE *pszBuffer, int nLength, void *pCallData);
};

#endif // __IEC62056_21_DATA_STREAM_H__
