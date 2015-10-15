////////////////////////////////////////////////////////////////////
//
//  EndDevicePool.cpp: implementation of the CEndDevicePool class.
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
#include "EndDevicePool.h"
#include "CoordinatorUtility.h"
#include "CoordinatorMessage.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#ifdef WINCE
#include "CeUtil.h"
#endif

////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEndDevicePool::CEndDevicePool()
{
}

CEndDevicePool::~CEndDevicePool()
{
}

////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

void CEndDevicePool::InitPool()
{
	ResetPool();
}

void CEndDevicePool::ResetPool()
{
	int		i;

	m_lockerPool.Lock();
	m_nPoolAllocate = 0;
	for(i=0; i<ENDI_MAX_POOL_COUNT; i++)
		memset(&m_listEndDevice[i], 0, sizeof(ENDDEVICE));
	m_lockerPool.Unlock();
}

int CEndDevicePool::NewEndDevice(COORDINATOR *codi, ENDDEVICE **endi, EUI64 *id)
{
	ENDDEVICE	*pEndDevice;
	int			nError = CODIERR_NOERROR;

	m_lockerPool.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice == NULL)
	{	
		pEndDevice = AllocateEndDevice(codi, id);
		if (pEndDevice == NULL)
			nError = CODIERR_OUT_OF_BINDING;
		else *endi = pEndDevice;
	}
	else nError = CODIERR_INUSE_ENDDEVICE;
	m_lockerPool.Unlock();

	return nError;
}

void CEndDevicePool::DeleteEndDevice(ENDDEVICE *endi)
{
	m_lockerPool.Lock();
	FreeEndDevice(endi);
	m_lockerPool.Unlock();
}

ENDDEVICE *CEndDevicePool::GetEndDeviceByID(EUI64 *id)
{
	ENDDEVICE	*pEndDevice;

	m_lockerPool.Lock();
	pEndDevice = FindEndDeviceByID(id);
	m_lockerPool.Unlock();

	return pEndDevice;
}

void CEndDevicePool::EndDeviceNotifyStackStatus(BYTE status)
{
	m_lockerPool.Lock();
	ResetConnection();
	m_lockerPool.Unlock();
}

void CEndDevicePool::EndDeviceSendCallback(COORDINATOR *codi, ENDI_DATA_FRAME *pFrame, int nLength)
{
	ENDDEVICE			*pEndDevice;
	EUI64            	id;

	ReverseEUI64(&pFrame->id, &id);

	m_lockerPool.Lock();
	pEndDevice = FindEndDeviceByID(&id);
	if (pEndDevice) LockEndDevice(pEndDevice);
	m_lockerPool.Unlock();

	if (pEndDevice == NULL)
	{
		endiOnSend(codi, (HANDLE)NULL, pFrame->hdr.ctrl, pFrame->hdr.type, pFrame->hdr.seq, 
						pFrame->payload, pFrame->hdr.len-sizeof(EUI64));
		return;
	}

	endiOnSend(codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl, pFrame->hdr.type, pFrame->hdr.seq, 
					pFrame->payload, pFrame->hdr.len-sizeof(EUI64));

	UnlockEndDevice(pEndDevice);
}

BOOL CEndDevicePool::IsValidSeq(BYTE previous, BYTE current)
{
    return ((BYTE) (previous + 1)) == current ? TRUE : FALSE;
}

BOOL CEndDevicePool::IsPossibleSeq(BYTE previous, BYTE current)
{
    int cmp = CompareSeq(previous, current);
    return (cmp <= 10 && cmp > 0) ? TRUE : FALSE;
}

int  CEndDevicePool::ClearDataBuffer(ENDDEVICE *pEndDevice)
{
    FRAMEDATA   *next;
    int         cnt = 0;

    for(;pEndDevice->buffer;cnt++) {
        next = pEndDevice->buffer->next;
        if(pEndDevice->buffer->data) FREE(pEndDevice->buffer->data);
        FREE(pEndDevice->buffer);
        pEndDevice->buffer = next;
    }
    return cnt;
}

/** 두 Sequence의 차이가 10보다 작을 때만 255가 넘어가는 부분을 유효하게
  * 검사해 준다.
  */
int CEndDevicePool::CompareSeq(BYTE previous, BYTE current)
{
    int gap;
    int p=(int)previous;
    int c=(int)current;

    if(p > 245) {
        gap = 255 - p;
        if(c < (10 - gap)) return gap + c;
    }else if(p < 10) {
        gap = 10 - p;
        if(current > (255 - gap)) return c - 255 - gap;
    }
    return  c - p;
}

int  CEndDevicePool::AddDataBuffer(ENDDEVICE *pEndDevice, ENDI_DATA_FRAME *pFrame, int nLength, int nTLength, BYTE LQI, signed char RSSI)
{
    FRAMEDATA   *next, *head=pEndDevice->buffer, *prev=NULL;
    BYTE        seq = pFrame->hdr.seq;
    int         idx = 0;
    int         cmp;

    next = (FRAMEDATA *) MALLOC(sizeof(FRAMEDATA));
    memset(next, 0, sizeof(FRAMEDATA));
    next->seq = seq;
    next->data = (ENDI_DATA_FRAME *) MALLOC(nTLength);
    memcpy(next->data, pFrame, nTLength);
    next->LQI = LQI;
    next->RSSI = RSSI;
    next->nLength = nLength;
    next->nTLength = nTLength;

    if(!head) pEndDevice->buffer = next;

    for(idx=1;head;idx++) {
        cmp = CompareSeq(head->seq, seq);
        if(cmp == 0) { // 중복 Frame 
            FREE(next->data);
            FREE(next);
            break;
        }else if(cmp < 0) { 
            next->next = head;
            if(prev) {
                prev->next = next;
            }else {
                pEndDevice->buffer = next;
            }
            break;
        }
        if(!head->next) {
            head->next = next;
            break;
        }
        prev = head;
        head = head->next;
    }
    return idx;
}

FRAMEDATA*  CEndDevicePool::GetNextData(ENDDEVICE *pEndDevice)
{
    FRAMEDATA   *head=pEndDevice->buffer;
    if(pEndDevice->buffer) pEndDevice->buffer = pEndDevice->buffer->next;
    return head;
}

FRAMEDATA*  CEndDevicePool::GetNextData(EUI64 * id)
{
    FRAMEDATA *head = NULL;
	ENDDEVICE			*pEndDevice;

	m_lockerPool.Lock();
	pEndDevice = FindEndDeviceByID(id);
	if (pEndDevice) LockEndDevice(pEndDevice);
	m_lockerPool.Unlock();

    if(pEndDevice) {
        head = GetNextData(pEndDevice);
        UnlockEndDevice(pEndDevice);
    }
    return head;
}

BOOL CEndDevicePool::EndDeviceRecvCallback(COORDINATOR *codi, ENDI_DATA_FRAME *pFrame, int nLength, int nTLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE			*pEndDevice;
	ENDI_LINK_PAYLOAD	*pLink;
	ENDI_NAN_PAYLOAD	*pNan;
	ENDI_ROM_PAYLOAD	*pRead;
	ENDI_ERROR_FRAME	*pError;
	EUI64            	id;
	BYTE          		flow, mode;
    char                szGUID[17];
	//BOOL				bAccept;

	mode = GET_FRAME_MODE(pFrame->hdr.ctrl);
	flow = GET_FRAME_FLOW(pFrame->hdr.ctrl);
	ReverseEUI64(&pFrame->id, &id);

	eui64toa(&id, szGUID);

	m_lockerPool.Lock();
	pEndDevice = FindEndDeviceByID(&id);
	if (pEndDevice) LockEndDevice(pEndDevice);
	m_lockerPool.Unlock();

	if (pEndDevice == NULL)
	{
		switch(pFrame->hdr.type) {
		  case ENDI_DATATYPE_ROM_READ :
			   if (((mode == CODI_ACCESS_READ) && (flow == CODI_FLOW_RESPONSE)) ||
			       ((mode == CODI_ACCESS_WRITE) && (flow == CODI_FLOW_REQUEST)))
			   {
				   pRead = (ENDI_ROM_PAYLOAD *)&pFrame->payload;
				   endiOnRead((HANDLE)codi, &id, pFrame->hdr.seq,
									ntoshort(pRead->addr), (pRead->typelen & 0x3F),
									LQI, RSSI);
			   }
			   break;

#ifndef __NETWORK_NODE__
		  case ENDI_DATATYPE_BYPASS :
			   if (((mode == CODI_ACCESS_READ) && (flow == CODI_FLOW_RESPONSE)) ||
			       ((mode == CODI_ACCESS_WRITE) && (flow == CODI_FLOW_REQUEST)))
			   {
			   		endiOnData((HANDLE)codi, &id, pFrame->hdr.seq,
									(BYTE *)&pFrame->payload, nLength, LQI, RSSI);
			   }
			   break;
#endif

		  case ENDI_DATATYPE_LINK :
			   if (((mode == CODI_ACCESS_READ) && (flow == CODI_FLOW_RESPONSE)) ||
			       ((mode == CODI_ACCESS_WRITE) && (flow == CODI_FLOW_REQUEST)))
			   {
			 	   pLink = (ENDI_LINK_PAYLOAD *)&pFrame->payload;
#ifndef __NETWORK_NODE__
				   if (pLink->type & ENDI_LINKSRC_PDA)
#endif
					   endiOnLink((HANDLE)codi, &id, (pLink->type & 0x0F), LQI, RSSI);
			   }
			   break;

#ifndef __NETWORK_NODE__
  	      case ENDI_DATATYPE_AMR :
		       endiOnKeepAlive((HANDLE)codi, &id, pFrame->hdr.ctrl,
						(ENDI_AMR_PAYLOAD *)&pFrame->payload, nLength, LQI, RSSI);
		   	   break;
#endif

          case ENDI_DATATYPE_NAN :
			   if (((mode == CODI_ACCESS_READ) && (flow == CODI_FLOW_RESPONSE)) ||
			       ((mode == CODI_ACCESS_WRITE) && (flow == CODI_FLOW_REQUEST)))
               {
		           pNan = (ENDI_NAN_PAYLOAD *)&pFrame->payload;
		           endiOnNanData((HANDLE)codi, &id, pNan->trid, 
                        (pNan->ctrl & 0x80) ? 1 : 0, 
                        (pNan->ctrl & 0x40) ? 1 : 0, 
                        pNan->ctrl & 0x3F,
						pNan->data, nLength - 2, 
                        LQI, RSSI);
               }
		       break;
		}
		return TRUE;
	}
#ifdef __NETWORK_NODE__
    else
    {
		switch(pFrame->hdr.type) {
		  case ENDI_DATATYPE_LINK :
			   if (((mode == CODI_ACCESS_READ) && (flow == CODI_FLOW_RESPONSE)) ||
			       ((mode == CODI_ACCESS_WRITE) && (flow == CODI_FLOW_REQUEST)))
			   {
			 	   pLink = (ENDI_LINK_PAYLOAD *)&pFrame->payload;
				   endiOnLink((HANDLE)codi, &id, (pLink->type & 0x0F), LQI, RSSI);
			   }
			   return TRUE;
        }
    }
#endif

	if ((mode == CODI_ACCESS_WRITE) && (flow == CODI_FLOW_RESPONSE))
	{
		pError = (ENDI_ERROR_FRAME *)pFrame;
	
		// Save last error code
		codi->nLastError = pError->payload.code;

        if (pError->payload.code != CODI_SUCCESS)
		{
        	XDEBUG(" \033[1;40;31m***** WRITE RESPONSE ERROR (0x%02X): %s\033[0m\r\n",
					pError->payload.code, GetErrorTypeName(pError->payload.code));
		    endiOnError((HANDLE)codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl, pFrame->hdr.type, pError->payload.code); 
		}

		UnlockEndDevice(pEndDevice);
		return TRUE;
	}

    if((mode == CODI_ACCESS_READ) && (flow == CODI_FLOW_RESPONSE)) {
        /** Endi에서 올라오는 Keepalive Event는 Seq 처리를 하지 않는다 */
		if(pFrame->hdr.type == ENDI_DATATYPE_AMR && !pFrame->hdr.seq) {
	        pEndDevice->nTotalRecv += nLength;
	        ceTime(&pEndDevice->tmLastReceive);
		    endiOnAmrData((HANDLE)codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl,	  
			    (ENDI_AMR_PAYLOAD *)&pFrame->payload, nLength, LQI, RSSI);
	        // 총수신량 합산 = 중복 프레임 제외
	        UnlockEndDevice(pEndDevice);
            return TRUE;
        }
    }

	// 중복 프레임 및 시퀀스 오류 버리기
	if (pEndDevice->nLastRecvSeq == pFrame->hdr.seq)
	{
		XDEBUG(" \033[1;40;31m-------------------- DUPLICATE ERROR ------------------\033[0m\r\n");	
		XDEBUG(" \033[1;40;31m***** DUPLICATE FRAME PREV=%d, SEQ=%d, LENGTH=%d *****\033[0m\r\n",
				pEndDevice->nLastRecvSeq, pFrame->hdr.seq, nTLength);
		XDUMP((char *)pFrame, nTLength, TRUE);

		UnlockEndDevice(pEndDevice);
		return FALSE;
	}

	// 처음이 아니면서 시퀀스를 검사해야 하는 경우
	if ((pEndDevice->nLastRecvSeq != -1) && (pEndDevice->nOptions & ENDIOPT_SEQUENCE)) 
	{
        if(!IsValidSeq((BYTE)pEndDevice->nLastRecvSeq, pFrame->hdr.seq))
        {
			XDEBUG(" \033[1;40;31m---------------------- SEQ ERROR --------------------\033[0m\r\n");	
			XDEBUG(" \033[1;40;31m***** SEQUENCE CHECK FRAME PREV=%d, CUR=%d *****\033[0m\r\n", 
					pEndDevice->nLastRecvSeq, pFrame->hdr.seq);

			// 연속적으로 시퀀스 에러가 발생되는것을 누적하기 위하여 사용
			pEndDevice->nSeqError++;

            if(IsPossibleSeq((BYTE)pEndDevice->nLastRecvSeq, pFrame->hdr.seq)) 
            {
                /** 내부 버퍼에 내용을 집어 넣는다.
                 *  내부 버퍼는 Binding을 Open 할 때 할당 되고
                 *  Close 할 때 Free 한다
                 */
                // 내부에 등록
			    XDEBUG(" AddDataBuffer -- ID=%s, LAST=%d, SEQ=%d\r\n", pEndDevice->szGUID, (BYTE)pEndDevice->nLastRecvSeq, pFrame->hdr.seq);	
                AddDataBuffer(pEndDevice, pFrame, nLength, nTLength, LQI, RSSI);
            }
			UnlockEndDevice(pEndDevice);
            return FALSE;
        }
	}

	// 총수신량 합산 = 중복 프레임 제외
	pEndDevice->nTotalRecv += nLength;

	// 정상 프레임이 수신되면 누적된 Count를 초기화 한다.
	pEndDevice->nSeqError = 0;

	// 수신한 시퀀스를 저장한다.
	pEndDevice->nLastRecvSeq = pFrame->hdr.seq;
	ceTime(&pEndDevice->tmLastReceive);

    // Thread Debugging
    __PTHREAD_INFO__;

	switch(pFrame->hdr.type) {
  	  case ENDI_DATATYPE_BYPASS :
		   pEndDevice->nTotalDataRecv += nLength;
		   endiOnBypass((HANDLE)codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl, pFrame->hdr.seq,
						(BYTE *)&pFrame->payload, nLength, LQI, RSSI);
		   break;
  	  case ENDI_DATATYPE_AMR :
		   endiOnAmrData((HANDLE)codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl,	  
						(ENDI_AMR_PAYLOAD *)&pFrame->payload, nLength, LQI, RSSI);
		   break;
  	  case ENDI_DATATYPE_NETWORK_LEAVE :
		   endiOnNetworkLeave((HANDLE)codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl,
						(ENDI_LEAVE_PAYLOAD *)&pFrame->payload, nLength, LQI, RSSI);
		   break;
  	  case ENDI_DATATYPE_ROM_READ :
#ifdef  __NETWORK_NODE__
		   pRead = (ENDI_ROM_PAYLOAD *)&pFrame->payload;
		   endiOnRead((HANDLE)codi, &id, pFrame->hdr.seq,
						ntoshort(pRead->addr), (pRead->typelen & 0x3F),
						LQI, RSSI);
#else
		   endiOnRomRead((HANDLE)codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl,
						(ENDI_ROM_PAYLOAD *)&pFrame->payload, nLength, LQI, RSSI);
#endif
		   break;
  	  case ENDI_DATATYPE_LONG_ROM_READ :
		   endiOnLongRomRead((HANDLE)codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl,
						(ENDI_LONG_ROM_PAYLOAD *)&pFrame->payload, nLength, LQI, RSSI);
            break;
  	  case ENDI_DATATYPE_MCCB :
		   endiOnMccbData((HANDLE)codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl,
						(ENDI_MCCB_PAYLOAD *)&pFrame->payload, nLength, LQI, RSSI);
		   break;
  	  case ENDI_DATATYPE_COMMAND :
		   endiOnCommand((HANDLE)codi, (HANDLE)pEndDevice, pFrame->hdr.ctrl,
						(ENDI_COMMAND_PAYLOAD *)&pFrame->payload, nLength, LQI, RSSI);
		   break;
      case ENDI_DATATYPE_NAN :
		   pNan = (ENDI_NAN_PAYLOAD *)&pFrame->payload;
		   endiOnNanData((HANDLE)codi, &id, pNan->trid, 
                        (pNan->ctrl & 0x80) ? 1 : 0, 
                        (pNan->ctrl & 0x40) ? 1 : 0, 
                        pNan->ctrl & 0x3F,
						pNan->data, nLength - 2, 
                        LQI, RSSI);
		   break;
    }

	UnlockEndDevice(pEndDevice);

    return TRUE;
}

////////////////////////////////////////////////////////////////////
// Member functions
//////////////////////////////////////////////////////////////////////

ENDDEVICE *CEndDevicePool::AllocateEndDevice(COORDINATOR *codi, EUI64 *id)
{
	int		i;

	for(i=0; i<ENDI_MAX_POOL_COUNT; i++)
	{
		if (m_listEndDevice[i].nIdentifier == 0)
		{
			m_listEndDevice[i].nIdentifier 	= ENDI_IDENTIFIER;
			m_listEndDevice[i].pService		= codi->pService;
			m_listEndDevice[i].codi			= codi;
			memcpy(&m_listEndDevice[i].id, id, sizeof(EUI64));
			
			m_listEndDevice[i].bConnected	= FALSE;
			m_listEndDevice[i].nOptions		= ENDIOPT_SEQUENCE;
			m_listEndDevice[i].seq			= 0;
			m_listEndDevice[i].nLastRecvSeq = -1;
			eui64toa(id, m_listEndDevice[i].szGUID);

			m_listEndDevice[i].tmLastConnect = (time_t)0;
			m_listEndDevice[i].tmLastReceive = (time_t)0;

			m_nPoolAllocate++;
			return &m_listEndDevice[i];
		}
	}
	return NULL;
}

void CEndDevicePool::FreeEndDevice(ENDDEVICE *endi)
{
	int		nRetry = 0;

	while(endi->bReference)
	{
		USLEEP(100000);
		nRetry++;
		if (nRetry > 50)
		{
			XDEBUG("WARNNING: --- HANDLE DELETE ERROR ---\r\n");
			USLEEP(1000000);
			break;
		}
	}
	
    ClearDataBuffer(endi);
	memset(endi, 0, sizeof(ENDDEVICE));
	m_nPoolAllocate--;
}

ENDDEVICE *CEndDevicePool::FindEndDeviceByID(EUI64 *id)
{
	int		i;

	for(i=0; i<ENDI_MAX_POOL_COUNT; i++)
	{
		if (m_listEndDevice[i].nIdentifier != 0)
		{
			if (memcmp(&m_listEndDevice[i].id, id, sizeof(EUI64)) == 0)
				return &m_listEndDevice[i];
		}
	}
	return NULL;
}

void CEndDevicePool::ResetConnection()
{
	int		i;

	for(i=0; i<ENDI_MAX_POOL_COUNT; i++)
	{
		if (m_listEndDevice[i].nIdentifier != 0)
			m_listEndDevice[i].bConnected = FALSE;
	}
}

void CEndDevicePool::LockEndDevice(ENDDEVICE *pEndDevice)
{
	if (pEndDevice != NULL)
		pEndDevice->bReference = TRUE;
}

void CEndDevicePool::UnlockEndDevice(ENDDEVICE *pEndDevice)
{
	pEndDevice->bReference = FALSE;
}
