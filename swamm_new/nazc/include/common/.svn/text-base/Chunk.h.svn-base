//////////////////////////////////////////////////////////////////////
//
//	Chunk.h: interface for the CChunk class.
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

#ifndef __CHUNK_H__
#define __CHUNK_H__

#include "typedef.h"

#define DEFAULT_CHUNK_GROW			1024

typedef struct	_tagCHUNK
{
		int		nAllocation;				// 할당된 버퍼의 길이
		int		nCommitLength;				// 마지막 Commit 위치 (Rollback 위치)
		int		nLength;					// 현재 길이
		int		nGrow;						// 증가 값
		char	*pszBuffer;					// 버퍼
		void	*dwHandle;					// CChunk Class Pointer
}	CHUNK, *PCHUNK;

class CChunk  
{
public:
	CChunk(int nGrow=DEFAULT_CHUNK_GROW);
	virtual ~CChunk();

public:
	virtual CHUNK *GetChunk();
	virtual void SetChunk(CHUNK *pChunk);
	virtual int	GetSize() const;
	virtual int	GetGrowSize() const;
	virtual int	GetBufferSize() const;
	virtual int GetCommitSize() const;
	virtual char *GetBuffer() const;
	virtual void SetGrowSize(int nGrow);

public:
	virtual int	Add(const char *pszBuffer, int nLength);
	virtual int	Add(char c);
	virtual BOOL Delete(int nLength=1);
	virtual void Close();
	virtual void Grow(int nWanted=DEFAULT_CHUNK_GROW);
	virtual void Empty();
	virtual void Flush();
	virtual void Commit();
	virtual void Rollback();
	virtual void Attach(char *pszBuffer, int nLength);
	virtual char *Detach(CChunk *pChunk=NULL);

protected:
	CHUNK	m_Chunk;
};

#endif // __CHUNK_H__

