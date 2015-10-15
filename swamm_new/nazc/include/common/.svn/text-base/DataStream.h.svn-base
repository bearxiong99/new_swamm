//////////////////////////////////////////////////////////////////////
//
//	DataStream.h: interface for the CDataStream class.
//
//	This file is a part of the AIMIR-GCP.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __DATA_STREAM_H__
#define __DATA_STREAM_H__

#include "typedef.h"

#define ACCESS_MODE_BIN					0
#define ACCESS_MODE_TEXT				1

#define DEFAULT_DATASTREAM_GROW			4096

class CDataStream  
{
public:
	CDataStream();
	virtual ~CDataStream();

public:
	virtual DATASTREAM *NewStream(UINT_PTR dwParam=0, int nGrow=DEFAULT_DATASTREAM_GROW, int nMode=ACCESS_MODE_BIN);
	virtual void DeleteStream(DATASTREAM *pStream);
	virtual	BOOL AddStream(DATASTREAM *pStream, const char * pszBuffer, int nLength);
	virtual	BOOL AddStream(DATASTREAM *pStream, unsigned char c);
	virtual void ClearStream(DATASTREAM *pStream);
    virtual void Detach(DATASTREAM *pStream);

protected:
	virtual BOOL GrowStream(DATASTREAM *pStream, int nWanted);

protected:
	virtual BOOL OnStreamDone(DATASTREAM *pStream);
};

#endif // __DATA_STREAM_H__
