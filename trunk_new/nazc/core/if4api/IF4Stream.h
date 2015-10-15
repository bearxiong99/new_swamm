#ifndef __IF4_STREAM_H__
#define __IF4_STREAM_H__

#include "if4frame.h"
#include "DataStream.h"
#include "Chunk.h"

class CIF4Stream : public CDataStream
{
public:
	CIF4Stream();
	virtual ~CIF4Stream();

public:
	BOOL	ParseStream(WORKSESSION *pSession, DATASTREAM *pStream, const BYTE *pszStream, int nLength);

protected:
	virtual BOOL OnFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	virtual BOOL OnDownloadFrame(WORKSESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength);
	virtual void OnDownloadAck(WORKSESSION *pSession, DATASTREAM *pStream, BYTE seq);
	virtual BOOL OnSaveFile(WORKSESSION *pSession, DOWNLOAD_FILE_FORMAT *pDownload, CChunk *pChunk);
};

#endif // __IF4_STREAM_H__
