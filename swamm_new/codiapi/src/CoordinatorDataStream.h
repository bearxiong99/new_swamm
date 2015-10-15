//////////////////////////////////////////////////////////////////////
//
//	CoordinatorDataStream.h: interface for the CCoordinatorDataStream class.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@paran.com
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __COORDINATOR_DATA_STREAM_H__
#define __COORDINATOR_DATA_STREAM_H__

#include "DataStream.h"

class CCoordinatorDataStream : public CDataStream
{
public:
	CCoordinatorDataStream();
	virtual ~CCoordinatorDataStream();

public:
	virtual	BOOL AddStream(DATASTREAM *pStream, const char* pszBuffer, int nLength);
	virtual	BOOL AddStream(DATASTREAM *pStream, unsigned char c);

protected:
	virtual void OnReceiveCoordinator(COORDINATOR *codi, CODI_GENERAL_FRAME *pFrame, int nLength);
};

#endif // __COORDINATOR_DATA_STREAM_H__
