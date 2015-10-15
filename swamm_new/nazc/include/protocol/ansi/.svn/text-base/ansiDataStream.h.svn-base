#ifndef __ANSI_DATA_STREAM_H__
#define __ANSI_DATA_STREAM_H__

#include "DataStream.h"

#include "ansiSessions.h"

class CAnsiDataStream : public CDataStream
{
public:
	CAnsiDataStream();
	virtual ~CAnsiDataStream();

public:
	BOOL	ParseStream(ANSISESSION *pSession, DATASTREAM *pStream, BYTE *pszStream, int nLength, void *pCallData);

protected:
	virtual BOOL OnFrame(ANSISESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength, int nRemain, void *pCallData);
};

#endif // __ANSI_DATA_STREAM_H__
