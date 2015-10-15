////////////////////////////////////////////////////////////////////
//
//  CoordinatorFrame.cpp: implementation of the CCoordinatorFrameFrame class.
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
#include "CoordinatorFrame.h"
#include "CoordinatorCrcCheck.h"
#include "CoordinatorUtility.h"
#include "DebugUtil.h"

////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordinatorFrame::CCoordinatorFrame()
{
	m_roundSequence = 0;
}

CCoordinatorFrame::~CCoordinatorFrame()
{
}

////////////////////////////////////////////////////////////////////
// Coordinator functions
//////////////////////////////////////////////////////////////////////

void CCoordinatorFrame::MakeFrameHeader(CODI_HEADER *pHeader, BYTE seq, BYTE ctrl, BYTE type, BYTE len)
{
	pHeader->ident.first	= CODI_START_FLAG_FIRST;					// 0x87
	pHeader->ident.second	= CODI_START_FLAG_SECOND;					// 0x98
	pHeader->seq			= seq;										// Sequence number
	pHeader->ctrl			= ctrl;										// Frame control
	pHeader->type			= type;										// Frame type
	pHeader->len			= len;										// Frame length
}

void CCoordinatorFrame::MakeFrameTail(BYTE *pFrame)
{
	SetFrameCrc((BYTE *)pFrame);										// CRC 16
}

int CCoordinatorFrame::MakeCommandFrame(CODI_COMMAND_FRAME *pFrame, BYTE flow, BYTE cmd, BYTE *payload, int nLength)
{
	BYTE	ctrl;

	ctrl  = CODI_FRAMEFLOW_REQUEST;
	ctrl |= (flow == CODI_ACCESS_WRITE) ? CODI_FRAMEFLOW_WRITE : CODI_FRAMEFLOW_READ;
	ctrl |= CODI_FRAMETYPE_COMMAND;

	memset(pFrame, 0, sizeof(CODI_COMMAND_FRAME));
	MakeFrameHeader(&pFrame->hdr, GetSequence(), ctrl, cmd, nLength);
	memcpy(pFrame->payload, payload, nLength);
	MakeFrameTail((BYTE *)pFrame);

	return nLength + sizeof(CODI_HEADER) + sizeof(CODI_TAIL);	
}

int CCoordinatorFrame::MakeDataFrame(ENDDEVICE *endi, ENDI_DATA_FRAME *pFrame, BYTE flow, BYTE type, BYTE *payload, int nLength)
{
	BYTE	ctrl;

	ctrl  = CODI_FRAMEFLOW_REQUEST;
	ctrl |= (flow == CODI_ACCESS_WRITE) ? CODI_FRAMEFLOW_WRITE : CODI_FRAMEFLOW_READ;
	ctrl |= CODI_FRAMETYPE_DATA;

	memset(pFrame, 0, sizeof(ENDI_DATA_FRAME));

    if(IsSequenceMessage(type))
    {
        MakeFrameHeader(&pFrame->hdr, GetEndDeviceSequence(endi), ctrl, type, nLength+sizeof(EUI64));
    }
    else
    {
        MakeFrameHeader(&pFrame->hdr, 0, ctrl, type, nLength+sizeof(EUI64));
    }

	ReverseEUI64(&endi->id, &pFrame->id);
	memcpy(pFrame->payload, payload, nLength);
	MakeFrameTail((BYTE *)pFrame);

	return nLength + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);	
}

int	CCoordinatorFrame::MakeDataFrameByID(EUI64 *id, ENDI_DATA_FRAME *pFrame, BYTE flow, BYTE seq, BYTE type, BYTE *payload, int nLength)
{
	BYTE	ctrl;

	ctrl  = CODI_FRAMEFLOW_REQUEST;
	ctrl |= (flow == CODI_ACCESS_WRITE) ? CODI_FRAMEFLOW_WRITE : CODI_FRAMEFLOW_READ;
	ctrl |= CODI_FRAMETYPE_DATA;

	memset(pFrame, 0, sizeof(ENDI_DATA_FRAME));

    if(IsSequenceMessage(type))
    {
		MakeFrameHeader(&pFrame->hdr, seq, ctrl, type, nLength+sizeof(EUI64));
    }
    else
    {
		MakeFrameHeader(&pFrame->hdr, 0, ctrl, type, nLength+sizeof(EUI64));
    }

	ReverseEUI64(id, &pFrame->id);
	memcpy(pFrame->payload, payload, nLength);
	MakeFrameTail((BYTE *)pFrame);

	return nLength + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);	
}

int CCoordinatorFrame::MakeMulticastFrame(ENDI_DATA_FRAME *pFrame, BYTE seq, BYTE type, BYTE *payload, int nLength)
{
	BYTE	ctrl;

	ctrl  = CODI_FRAMETYPE_DATA;
	ctrl |= CODI_FRAMEFLOW_REQUEST;
	ctrl |= CODI_FRAMEFLOW_WRITE;

	memset(pFrame, 0, sizeof(ENDI_DATA_FRAME));
	MakeFrameHeader(&pFrame->hdr, seq, ctrl, type, nLength+sizeof(EUI64));

	// Multicast는 ID가 항상 1111111111111111 이다.
	memset(&pFrame->id, 0x11, sizeof(EUI64));
	memcpy(pFrame->payload, payload, nLength);
	MakeFrameTail((BYTE *)pFrame);

	return nLength + sizeof(CODI_HEADER) + sizeof(EUI64) + sizeof(CODI_TAIL);	
}

int	CCoordinatorFrame::GetCommandError(BYTE code)
{
	switch(code) {
	  case CODI_SUCCESS :							return CODIERR_NOERROR;

	  // Information Error Code
	  case CODI_INFOERR_SERIAL_TIME_OUT :			return CODIERR_SERIAL_TIMEOUT;
	  case CODI_INFOERR_SERIAL_ERROR :				return CODIERR_SERIAL_ERROR;
	  case CODI_INFOERR_FRAME_CONTROL_ERROR :		return CODIERR_FRAME_CONTROL_ERROR;
	  case CODI_INFOERR_REQUEST_ERROR :				return CODIERR_REQUEST_ERROR;
	  case CODI_INFOERR_FRAME_TYPE_ERROR :			return CODIERR_FRAME_TYPE_ERROR;
	  case CODI_INFOERR_CRC16_ERROR :				return CODIERR_CRC16_ERROR;

	  // Command Error Code
	  case CODI_CMDERR_NOT_SUPPORTED :				return CODIERR_NOT_SUPPORTED;
	  case CODI_CMDERR_INVALID_DATA :				return CODIERR_INVALID_DATA;
	  case CODI_CMDERR_NO_NETWORK_ERROR :			return CODIERR_NO_NETWORK_ERROR;
	  case CODI_CMDERR_FORM_NETWORK_ERROR :			return CODIERR_FORM_NETWORK_ERROR;
	  case CODI_CMDERR_LEAVE_NETWORK_ERROR :		return CODIERR_LEAVE_NETWORK_ERROR;
	  case CODI_CMDERR_BIND_ADDRESS_DUPLICATION :	return CODIERR_BIND_ADDRESS_DUPLICATION;
	  case CODI_CMDERR_FULL_BINDING_INDEX :			return CODIERR_FULL_BINDING_INDEX;
	  case CODI_CMDERR_EUI64_FOUND_FAIL_ERROR :   	return CODIERR_EUI64_FOUND_FAIL_ERROR;
	  case CODI_CMDERR_SET_BINDING_ERROR :   		return CODIERR_SET_BINDING_ERROR;
	  case CODI_CMDERR_DELETE_BINDING_ERROR :   	return CODIERR_DELETE_BINDING_ERROR;
	  case CODI_CMDERR_BINDING_IS_ACTIVE_ERROR :   	return CODIERR_BINDING_IS_ACTIVE_ERROR;
      case CODI_CMDERR_GET_BINDING_ERROR :          return CODIERR_GET_BINDING_ERROR;
	  case CODI_CMDERR_GET_NEIGHBOR_ERROR :         return CODIERR_GET_NEIGHBOR_ERROR;
	  case CODI_CMDERR_KEY_TABLE_DUPLICATION :      return CODIERR_KEY_TABLE_DUPLICATION;
	  case CODI_CMDERR_NULL_KEY_INDEX :             return CODIERR_NULL_KEY_INDEX;
	  case CODI_CMDERR_SET_KEY_ERROR :              return CODIERR_SET_KEY_ERROR;
	  case CODI_CMDERR_KEY_FOUND_FAIL_ERROR :       return CODIERR_KEY_FOUND_FAIL_ERROR;
	  case CODI_CMDERR_ADD_ERROR :              	return CODIERR_ADD_ERROR;
	  case CODI_CMDERR_DEL_ERROR :              	return CODIERR_DEL_ERROR;
	  case CODI_CMDERR_ADD_DUPLICATE_ERROR :        return CODIERR_ADD_DUPLICATE_ERROR;												
	  // Data Error Code
	  case CODI_DATAERR_SEND_DATA_ERROR :  			return CODIERR_SEND_DATA_ERROR; 
	  case CODI_DATAERR_PAYLOAD_LENGTH_ERROR :   	return CODIERR_PAYLOAD_LENGTH_ERROR;
	  case CODI_DATAERR_DELIVERY_ERROR :   			return CODIERR_DELIVERY_ERROR;
	  case CODI_DATAERR_BINDING_ERROR :  			return CODIERR_BINDING_ERROR; 
	}
	return CODIERR_UNKNOWN_ERROR;
}

BYTE CCoordinatorFrame::GetSequence()
{
	BYTE	seq;

	m_seqLocker.Lock();
	m_roundSequence++;
	seq = m_roundSequence;
	m_seqLocker.Unlock();
	return seq;
}

BYTE CCoordinatorFrame::GetEndDeviceSequence(ENDDEVICE *endi)
{
	BYTE	seq;

	m_endiLocker.Lock();
	seq = endi->seq;
	endi->seq++;
	m_endiLocker.Unlock();
	return seq;
}

