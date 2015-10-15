//////////////////////////////////////////////////////////////////////
//
//	Queue.h: interface for the CQueue class.
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

#ifndef __COMMON_QUEUE_H__
#define __COMMON_QUEUE_H__

#include "typedef.h"
#include "Locker.h"

class CQueue  
{
public:
	CQueue(int nMaximum=1024);
	virtual ~CQueue();

public:
	BOOL	IsEmpty();
	int		GetQLength();
	char	*GetBuffer() const;

public:
	int		AddQ(const char *pszBuffer, int nLength);
	int		GetQ(char *pszBuffer, int nLength);
	int		GetQ();
	void	FlushQ();

protected:
	int		AddQueueChar(BYTE c);
	int		GetQueueChar();

protected:
	CLocker		m_Locker;
	char		*m_pBuffer;
	int			m_nMaximum;
	int			m_nCount;
	int			m_nHead;
	int			m_nTail;
};

#endif // __COMMON_QUEUE_H__

