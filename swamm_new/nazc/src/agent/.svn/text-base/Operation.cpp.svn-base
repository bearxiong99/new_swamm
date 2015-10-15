//////////////////////////////////////////////////////////////////////
//
//  Ondemander.cpp: implementation of the COperation class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
//  http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "Operation.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "EndDeviceList.h"

#include "codiapi.h"

//////////////////////////////////////////////////////////////////////
// COperation Class
//////////////////////////////////////////////////////////////////////

COperation	*m_pOperation = NULL;

//////////////////////////////////////////////////////////////////////
// COperation Class
//////////////////////////////////////////////////////////////////////

COperation::COperation()
{
	m_pOperation = this;
}

COperation::~COperation()
{
}

//////////////////////////////////////////////////////////////////////
// COperation Functions
//////////////////////////////////////////////////////////////////////

int	COperation::GetWorkerCount()
{
	return m_List.GetCount();
}

void COperation::lockOperation()
{
	m_Locker.Lock();
}

void COperation::unlockOperation()
{
	m_Locker.Unlock();
}

ONDEMANDPARAM *COperation::newOperation(BYTE nOperation, EUI64 *id, int nBufferSize)
{
	ONDEMANDPARAM	*pOndemand;

	pOndemand = (ONDEMANDPARAM *)MALLOC(sizeof(ONDEMANDPARAM));
	if (pOndemand == NULL)
		return NULL;

	memset(pOndemand, 0, sizeof(ONDEMANDPARAM));
	memcpy(&pOndemand->id, id, sizeof(EUI64));
    EUI64ToStr(id, pOndemand->szId);
	pOndemand->nOperation	= nOperation;
	pOndemand->pszBuffer	= (char *)MALLOC(nBufferSize+1);
    pOndemand->nSize        = nBufferSize;
    pOndemand->nMeterConstant = 1;
	pOndemand->pLpChunk     = new CChunk(512);
	pOndemand->pMtrChunk    = new CChunk(1024);
	pOndemand->pDataStream	= m_DataStream.NewStream();
	pOndemand->nLength		= 0;
	pOndemand->hEvent 		= CreateEvent(NULL, FALSE, FALSE, NULL);
	pOndemand->pos 			= m_List.AddTail(pOndemand);
    pOndemand->bPartialWait = FALSE;
	return pOndemand;
}

void COperation::deleteOperation(ONDEMANDPARAM *pOndemand)
{
    if(pOndemand == NULL) return;

	m_List.RemoveAt(pOndemand->pos);

	if (pOndemand->hEvent != NULL)
    {
		CloseHandle(pOndemand->hEvent);
        pOndemand->hEvent = NULL;
    }

	if (pOndemand->pLpChunk) delete pOndemand->pLpChunk;
	if (pOndemand->pMtrChunk) delete pOndemand->pMtrChunk;
	if (pOndemand->pDataStream) m_DataStream.DeleteStream(pOndemand->pDataStream);
	if (pOndemand->pszBuffer) FREE(pOndemand->pszBuffer);	
	FREE(pOndemand);
}

void COperation::clearOperation(ONDEMANDPARAM *pOndemand)
{
	if (pOndemand == NULL)
		return;

    memset(pOndemand->pszBuffer, 0, pOndemand->nSize);
	pOndemand->nLength = 0;	
    pOndemand->bPartialWait = FALSE;
    pOndemand->bSegmentation = FALSE;
    m_DataStream.ClearStream(pOndemand->pDataStream);
    pOndemand->pDataStream->nState =  0;
}

ONDEMANDPARAM *COperation::findOperation(EUI64 *id)
{
	ONDEMANDPARAM	*pNext;
	POSITION		pos=INITIAL_POSITION;
	
	pNext = m_List.GetFirst(pos);
	while(pNext)
	{
		if (memcmp(&pNext->id, id, sizeof(EUI64)) == 0)
			return pNext;
		pNext = m_List.GetNext(pos);
	}
	return NULL;
}

BOOL COperation::sendCommand(ONDEMANDPARAM *pOndemand, const EUI64 *id, const char *pszCommand, int nLength)
{
    char szGUID[20];
    BYTE * data = (BYTE *)const_cast<char *>(pszCommand);
    int  Limit = ZIGBEE_VALID_PAYLOAD_SIZE; // valid
    ENDIENTRY   *endiEntry;
    BYTE hops=0;
    int i, len, nSize;

    if((endiEntry=m_pEndDeviceList->GetEndDeviceByID(id))==NULL) {
        eui64toa(id, szGUID);
	    XDEBUG("ERROR: sendComand: GetEndDevice Error %s\r\n", szGUID);
        return FALSE;
    }

    /** Command를 Multi Frame으로 보낼 수 있도록 한다 */
    for(i=0,len=0;i<nLength;i+=len)
    {
        if(m_pEndDeviceList->GetEndDeviceHops(endiEntry,&hops)==0xff)
        {
            eui64toa(id, szGUID);
	        XDEBUG("ERROR: sendComand: GetEndDeviceHops Fail %s\r\n", szGUID);
            return FALSE;
        }
        nSize = Limit - hops * 2;

        len = MIN(nSize, nLength - i);
	    if (endiBypass((HANDLE)pOndemand->endi, data + i, len, 2000) != CODIERR_NOERROR)
		    return FALSE;

        pOndemand->stat.nCount ++;
        pOndemand->stat.nSendCount ++;
        pOndemand->stat.nSend += len + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);
    }
	return TRUE;
}

