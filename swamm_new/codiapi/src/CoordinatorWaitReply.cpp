////////////////////////////////////////////////////////////////////
//
//  CoordinatorWaitReply.cpp: implementation of the CCoordinatorWaitReply class.
//
//  This file is a part of the AIMIR.
//  (c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  http://www.nuritelecom.com
//  casir@paran.com
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "codiapi.h"
#include "CoordinatorWaitReply.h"
#include "CoordinatorUtility.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#ifndef _WIN32
#include "Event.h"
#endif

////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordinatorWaitReply::CCoordinatorWaitReply()
{
}

CCoordinatorWaitReply::~CCoordinatorWaitReply()
{
}

////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

CODIWAITREPLY *CCoordinatorWaitReply::AddWaitReply(COORDINATOR *codi, BYTE flow, BYTE type, BYTE cmd, BYTE seq, BYTE waitMask, EUI64 *id, BYTE *pszBuffer)
{
	CODIWAITREPLY	*pReply;

	pReply = (CODIWAITREPLY *)MALLOC(sizeof(CODIWAITREPLY));
	if (pReply == NULL)
		return NULL;

	memset(pReply, 0, sizeof(CODIWAITREPLY));
	pReply->codi		= codi;
	pReply->flow		= flow;
	pReply->type		= type;
	pReply->cmd			= cmd;
	pReply->seq			= seq;
    pReply->waitMask    = waitMask;
	pReply->pszBuffer	= pszBuffer;
	if (id != NULL) memcpy(&pReply->id, id, sizeof(EUI64));
	pReply->hEvent		= CreateEvent(NULL, FALSE, FALSE, NULL);

	m_replyLocker.Lock();
	pReply->pos	= (int)m_replyList.AddTail(pReply);
	m_replyLocker.Unlock();
	return pReply;
}

void CCoordinatorWaitReply::DeleteWaitReply(CODIWAITREPLY *pReply)
{
	if (pReply == NULL)
		return;
	
	m_replyLocker.Lock();
	m_replyList.RemoveAt((POSITION)pReply->pos);
	m_replyLocker.Unlock();

	CloseHandle(pReply->hEvent);
	FREE(pReply);
}

int CCoordinatorWaitReply::WaitForReply(CODIWAITREPLY *pReply, int nTimeout)
{
	XDEBUG(" +++++++++++++++++++++++++++++debug sungyeung CCoordinatorWaitReply::WaitForReply : %s : %d \n", __FILE__, __LINE__);

	int		rc, t;

	if (pReply->replied)
		return CODIERR_NOERROR;

	t = nTimeout / 1000;
	if (t == 0) t = 1;

	rc = WaitForSingleObject(pReply->hEvent, t);
	if (rc == 0)
		return CODIERR_NOERROR;

	return CODIERR_REPLY_TIMEOUT;
}

void CCoordinatorWaitReply::OnCommandReplyMessage(COORDINATOR *codi, CODI_COMMAND_FRAME *pFrame, int nLength)
{
	CODIWAITREPLY		*pReply;
	POSITION			pos;
		
	m_replyLocker.Lock();
	pReply = m_replyList.GetFirst(pos);
	while(pReply)
	{
		if ((pReply->codi == codi) &&
			(pReply->type == CODI_FRAMETYPE_COMMAND) &&
		    (pReply->cmd == pFrame->hdr.type) &&
		    (pReply->seq == pFrame->hdr.seq))
		{
			if (pReply->pszBuffer)
				memcpy(pReply->pszBuffer, pFrame->payload, pFrame->hdr.len);
			pReply->nLength = pFrame->hdr.len;

		    if ((pFrame->hdr.ctrl & CODI_FRAMEFLOW_READ) > 0)
				 pReply->result = 0;
			else pReply->result = ((CODI_WRITE_FRAME *)pFrame)->payload.code;

			pReply->replied = TRUE;
			SetEvent(pReply->hEvent);
			break;
		}
		pReply = m_replyList.GetNext(pos);
	}
	m_replyLocker.Unlock();
}

void CCoordinatorWaitReply::OnDataReplyMessage(COORDINATOR *codi, ENDI_DATA_FRAME *pFrame)
{
	CODIWAITREPLY		*pReply;
	POSITION			pos;
	BYTE				rw;

	// Get Read or Write
	rw = ((pFrame->hdr.ctrl & CODI_FRAMEFLOW_READ) > 0) ? CODI_ACCESS_READ : CODI_ACCESS_WRITE;

	m_replyLocker.Lock();
	pReply = m_replyList.GetFirst(pos);
	while(pReply)
	{
		if ((pReply->codi == codi) && (pReply->type == CODI_FRAMETYPE_DATA))
		{
			if (pFrame->hdr.type == ENDI_DATATYPE_LINK)
			{
				if ((pReply->cmd == pFrame->hdr.type) &&
					(memcmp(&pReply->id, &pFrame->id, sizeof(EUI64)) == 0))
				{
					if (pFrame->hdr.ctrl == (CODI_FRAMEFLOW_WRITERESPONSE | CODI_FRAMETYPE_DATA))
						 pReply->result = ((ENDI_ERROR_FRAME *)pFrame)->payload.code;
					else pReply->result = 0;

					if ((pReply->result != 0) || 
                            (pFrame->hdr.ctrl == (CODI_FRAMEFLOW_READRESPONSE | CODI_FRAMETYPE_DATA)) ||
                            ((pReply->waitMask == WAIT_WRITE_RESPONSE) && (pFrame->hdr.ctrl == (CODI_FRAMEFLOW_WRITERESPONSE | CODI_FRAMETYPE_DATA)))
                       )
					{
						pReply->replied = TRUE;
						SetEvent(pReply->hEvent);
					}
					break;
				}
			}
			else
			{
				if (rw == CODI_ACCESS_WRITE)
				{
					if ((pReply->flow == CODI_ACCESS_WRITE) &&
						(memcmp(&pReply->id, &pFrame->id, sizeof(EUI64)) == 0) &&
					    (pReply->cmd == pFrame->hdr.type) &&
					    (pReply->seq == pFrame->hdr.seq))
					{
						pReply->result = ((ENDI_ERROR_FRAME *)pFrame)->payload.code;
						pReply->replied = TRUE;
						SetEvent(pReply->hEvent);
						break;
					}
				}
				else
				{
					if ((pReply->flow == CODI_ACCESS_READ) &&
						(memcmp(&pReply->id, &pFrame->id, sizeof(EUI64)) == 0) &&
						(pReply->cmd == pFrame->hdr.type))
					{
						pReply->nLength = pFrame->hdr.len - sizeof(EUI64) - sizeof(CODI_RADIO_STATUS);
						if (pReply->pszBuffer != NULL)
							memcpy(pReply->pszBuffer, &pFrame->payload, pReply->nLength);
						pReply->replied = TRUE;
						SetEvent(pReply->hEvent);
						break;
					}
				}
			}
		}
		pReply = m_replyList.GetNext(pos);
	}
	m_replyLocker.Unlock();
}

