#ifndef __COORDINATOR_WAIT_REPLY_H__
#define __COORDINATOR_WAIT_REPLY_H__

#include "Locker.h"
#include "LinkedList.h"

class CCoordinatorWaitReply
{
public:
	CCoordinatorWaitReply();
	virtual ~CCoordinatorWaitReply();

public:
	CODIWAITREPLY *AddWaitReply(COORDINATOR *codi, BYTE flow, BYTE type, BYTE cmd, BYTE seq, BYTE waitMask, EUI64 *id, BYTE *pszBuffer);
	void	DeleteWaitReply(CODIWAITREPLY *pReply);
	int		WaitForReply(CODIWAITREPLY *pReply, int nTimeout);
	void	OnCommandReplyMessage(COORDINATOR *codi, CODI_COMMAND_FRAME *pFrame, int nLength);
	void	OnDataReplyMessage(COORDINATOR *codi, ENDI_DATA_FRAME *pFrame);

protected:
	CLocker		m_replyLocker;
	CLinkedList<CODIWAITREPLY *>	m_replyList;
};

#endif	// __COORDINATOR_WAIT_REPLY_H__
