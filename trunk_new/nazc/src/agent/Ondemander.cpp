//////////////////////////////////////////////////////////////////////
//
//  Ondemander.cpp: implementation of the COndemander class.
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
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "MeterParser.h"
#include "MeterParserFactory.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "codiapi.h"
#include "Chunk.h"
#include "Event.h"
#include "AgentLog.h"

#define DEFAULT_RETRY_COUNT     1
#define DEFAULT_COMM_TIMEOUT    6000
#define DEFAULT_BIND_TIMEOUT    6000

//////////////////////////////////////////////////////////////////////
// COndemander Class
//////////////////////////////////////////////////////////////////////

COndemander	*m_pOndemander = NULL;

//////////////////////////////////////////////////////////////////////
// COndemander Class
//////////////////////////////////////////////////////////////////////

COndemander::COndemander()
{
	m_pOndemander = this;
}

COndemander::~COndemander()
{
}

//////////////////////////////////////////////////////////////////////
// COndemander Operations
//////////////////////////////////////////////////////////////////////

int	COndemander::Command(char *pszType, CMDPARAM *pCommand, METER_STAT *pStat)
{
	CMeterParser	*pParser;

	// 파서를 찾지 못하면 GE PARSER가 반환됨.
	pParser = m_pMeterParserFactory->SelectParser(pszType);
	XDEBUG("Ondemand PARSER: Parser=%s, %s(%s)\r\n", pszType, pParser->GetName(), pParser->GetDescription());

	return pParser->OnCommand(pCommand, pStat);
}

int CODI_TO_IF4ERROR(int nError)
{
	switch(nError) {
	  case CODIERR_NOERROR :				return IF4ERR_NOERROR;
	  case CODIERR_INVALID_DEVICE :			return IF4ERR_NOT_READY;
	  case CODIERR_DEVICE_BUSY :			return IF4ERR_BUSY;
	  case CODIERR_DEVICE_INUSE :			return IF4ERR_BUSY;
	  case CODIERR_MEMORY_ERROR :			return IF4ERR_MEMORY_ERROR;
	  case CODIERR_INVALID_HANDLE :		    return IF4ERR_INVALID_HANDLE;
	  case CODIERR_INVALID_BUFFER :			return IF4ERR_INVALID_BUFFER;
	  case CODIERR_INVALID_LENGTH :			return IF4ERR_INVALID_LENGTH;
	  case CODIERR_INVALID_TIMEOUT :		return IF4ERR_INTERNAL_ERROR;          
	  case CODIERR_ALREADY_INITIALIZED :	return IF4ERR_INTERNAL_ERROR;
	  case CODIERR_NOT_INITIALIZED :		return IF4ERR_NOT_READY;
	  case CODIERR_INVALID_ID :				return IF4ERR_INVALID_ID;
	  case CODIERR_NOT_REGISTERED :			return IF4ERR_NOT_READY;
	  case CODIERR_NOT_STARTED :			return IF4ERR_NOT_READY;
	  case CODIERR_ALREADY_STARTED :		return IF4ERR_INTERNAL_ERROR;
	  case CODIERR_INVALID_INTERFACE :		return IF4ERR_NOT_READY;
	  case CODIERR_DEVICE_ERROR :			return IF4ERR_NOT_READY;
	  case CODIERR_OUT_OF_BINDING :			return IF4ERR_OUT_OF_BINDING;
	  case CODIERR_INUSE_ENDDEVICE :		return IF4ERR_BUSY;
	  case CODIERR_INVALID_KEY :			return IF4ERR_INVALID_PARAM;
	  case CODIERR_REPLY_TIMEOUT :			return IF4ERR_TIMEOUT;
	  case CODIERR_INVALID_PARAM :			return IF4ERR_INVALID_PARAM;
	  case CODIERR_COMMAND_ERROR :			return IF4ERR_UNKNOWN_ERROR;
	  case CODIERR_UNKNOWN_ERROR :			return IF4ERR_UNKNOWN_ERROR;
	  case CODIERR_NOT_CONNECTED :			return IF4ERR_CANNOT_CONNECT;
	  case CODIERR_COORDINATOR_NOT_READY :	return IF4ERR_NOT_READY;
	  case CODIERR_INVALID_OPTION :			return IF4ERR_INTERNAL_ERROR;
	  case CODIERR_INVALID_METHOD :			return IF4ERR_INTERNAL_ERROR;
	  case CODIERR_PROGRESS :				return IF4ERR_NOERROR;
	  case CODIERR_SERIAL_TIMEOUT :			return IF4ERR_IO_ERROR;
	  case CODIERR_SERIAL_ERROR :			return IF4ERR_IO_ERROR;
	  case CODIERR_FRAME_CONTROL_ERROR :	return IF4ERR_IO_ERROR;
	  case CODIERR_REQUEST_ERROR :			return IF4ERR_IO_ERROR;
	  case CODIERR_FRAME_TYPE_ERROR :		return IF4ERR_IO_ERROR;
	  case CODIERR_CRC16_ERROR :			return IF4ERR_IO_ERROR;
	  case CODIERR_NOT_SUPPORTED :			return IF4ERR_DO_NOT_SUPPORT;
	  case CODIERR_INVALID_DATA :			return IF4ERR_INVALID_PARAM;
	  case CODIERR_NO_NETWORK_ERROR :		return IF4ERR_NOERROR;
	  case CODIERR_FORM_NETWORK_ERROR :		return IF4ERR_ZIGBEE_NETWORK_ERROR;
	  case CODIERR_LEAVE_NETWORK_ERROR :	return IF4ERR_ZIGBEE_NETWORK_ERROR;
	  case CODIERR_BIND_ADDRESS_DUPLICATION : return IF4ERR_OUT_OF_BINDING;
	  case CODIERR_FULL_BINDING_INDEX :		return IF4ERR_OUT_OF_BINDING;
	  case CODIERR_EUI64_FOUND_FAIL_ERROR :	return IF4ERR_CANNOT_CONNECT;
	  case CODIERR_SET_BINDING_ERROR :		return IF4ERR_CANNOT_CONNECT;
	  case CODIERR_DELETE_BINDING_ERROR :	return IF4ERR_CANNOT_CONNECT;
	  case CODIERR_BINDING_IS_ACTIVE_ERROR : return IF4ERR_CANNOT_CONNECT;
	  case CODIERR_SEND_DATA_ERROR :		return IF4ERR_DELIVERY_ERROR;
	  case CODIERR_PAYLOAD_LENGTH_ERROR :	return IF4ERR_INTERNAL_ERROR;
	  case CODIERR_DELIVERY_ERROR :			return IF4ERR_DELIVERY_ERROR;
	  case CODIERR_BINDING_ERROR :			return IF4ERR_OUT_OF_BINDING;
	  case CODIERR_INVALID_PARENT :			return IF4ERR_CANNOT_CONNECT;
	  case CODIERR_PARENT_BINDING_ERROR :	return IF4ERR_CANNOT_CONNECT;
	  case CODIERR_SET_CHILD_ERROR :		return IF4ERR_CANNOT_CONNECT;
	}
	return IF4ERR_UNKNOWN_ERROR;
}

int	COndemander::Leave(HANDLE codi, EUI64 *id, BYTE joinTime, BYTE channel, WORD panid, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
	
	eui64toa(id, szGUID);
	XDEBUG("ONDEMAND: End Device Leave Network(ID=%s, Join Time=%d, Channel=%d, PanID=%d)\r\n", 
        szGUID, joinTime, channel, panid);

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

	nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	for(nRetry=0; nRetry<nCount; nRetry++)
	{
		nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		if (nError == CODIERR_NOERROR)
		{
			nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			if (nError == CODIERR_NOERROR)
			{
				nError = endiLeave(endi, joinTime, channel, panid, sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT));
				if (nError != CODIERR_NOERROR)
					XDEBUG("ERROR: endiLeave(%d): %s\r\n", nError, codiErrorMessage(nError));
				else break;
			}
			else
			{
                isConnect = FALSE;
				XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			}
		}
		else
		{
            isOpen = FALSE;
			XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		}

		// 1초 이후에 재시도 한다.
		usleep(1000000);
	}

    if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
    if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);

	return CODI_TO_IF4ERROR(nError);
}

int	COndemander::MccbWrite(HANDLE codi, EUI64 *id, BYTE value, BYTE *reply, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
	
	eui64toa(id, szGUID);
	XDEBUG("ONDEMAND: MCCB Write (ID=%s, Value=%d)\r\n", szGUID, value);

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

	nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	for(nRetry=0; nRetry<nCount; nRetry++)
	{
		nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		if (nError == CODIERR_NOERROR)
		{
			nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			if (nError == CODIERR_NOERROR)
			{
				// MCCB는 읽거나 쓸때 오래 걸린다. (25초를 대기한다.)
				nError = endiWriteMccb(endi, value, reply, sensorCommTimeout(id, 25000));
				if (nError != CODIERR_NOERROR)
					XDEBUG("ERROR: endiWriteMccb(%d): %s\r\n", nError, codiErrorMessage(nError));
				else break;
			}
			else
			{
                isConnect = FALSE;
				XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			}
		}
		else
		{
            isOpen = FALSE;
			XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		}

		// 1초 이후에 재시도 한다.
		usleep(1000000);
	}

    if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
    if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);

	return CODI_TO_IF4ERROR(nError);
}

int	COndemander::_RomRead(HANDLE endi, EUI64 *id, WORD addr, int len, BYTE *pszBuffer, int *nSize, int nRetryLimit)
{
	int			nRetry;
	int			nError = CODIERR_NOERROR;
    int         Limit = MAX_ROM_READ_BYTES;
    int         i,next,nRead;
    BYTE        buffer[Limit];

    *nSize = nRead = 0;
	for(nRetry=0; nRetry<nRetryLimit; nRetry++)
	{
        for(i=(*nSize);i<len;i+=Limit) {
            memset(buffer,0,sizeof(buffer));
            next = len - i > Limit ? Limit : len - i;
	        nError = endiReadRom(endi, addr + i, next, buffer, &nRead, sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT));
	        if (nError != CODIERR_NOERROR) {
	            XDEBUG("ERROR: endiReadRom(%d): %s\r\n", nError, codiErrorMessage(nError));
                break;
            }
            memcpy(pszBuffer + i, buffer, next);
            *nSize += nRead;
        }
	    if(nError == CODIERR_NOERROR) break;
	    // 0.5초 이후에 재시도 한다.
	    usleep(500000);
	}

	return nError;
}

int	COndemander::RomRead(HANDLE codi, EUI64 *id, ParamAddr * param, ReturnVal ** returnVal, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
    ParamAddr   *head = param;
    ReturnVal   *tmp = NULL;

	eui64toa(id, szGUID);
    if(!param) {
	    XDEBUG("ONDEMAND: End Device ROM Read (ID=%s, NULL Parameter)\r\n", szGUID);
        return IF4ERR_INVALID_PARAM;
    }

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

	nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	for(nRetry=0; nRetry<nCount; nRetry++)
	{
		nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		if (nError == CODIERR_NOERROR)
		{
			nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			if (nError == CODIERR_NOERROR)
			{
                while(head) {
                    if((*returnVal)) {
                        tmp->next = (ReturnVal *)MALLOC(sizeof(ReturnVal));
	                    if (tmp->next == NULL) {
		                    nError = CODIERR_MEMORY_ERROR;
                            nRetry = nCount;
			                XDEBUG("ERRROR: romread fail(%d): %s\r\n", nError, codiErrorMessage(nError));
                            break;
                        }
                        tmp = tmp->next;
                    }else {
                        (*returnVal) = (ReturnVal *)MALLOC(sizeof(ReturnVal));
	                    if ((*returnVal) == NULL) {
		                    nError = CODIERR_MEMORY_ERROR;
                            nRetry = nCount;
			                XDEBUG("ERRROR: romread fail(%d): %s\r\n", nError, codiErrorMessage(nError));
                            break;
                        }
                        tmp = (*returnVal);
                    }
                    memset(tmp, 0, sizeof(ReturnVal));
                    tmp->val.pVal = (BYTE *)MALLOC(head->len);
	                if (tmp->val.pVal == NULL) {
		                nError = CODIERR_MEMORY_ERROR;
                        nRetry = nCount;
			            XDEBUG("ERRROR: romread fail(%d): %s\r\n", nError, codiErrorMessage(nError));
                        break;
                    }

	                XDEBUG("ONDEMAND: End Device ROM Read (ID=%s, ADDRESS=0x%04X, LEN=%d)\r\n",
				                szGUID, head->addr, head->len);

                    nError = _RomRead(endi, id, head->addr, head->len, tmp->val.pVal, &(tmp->len), nCount); 
                    if(nError != CODIERR_NOERROR) {
                        nRetry = nCount;
			            XDEBUG("ERRROR: romread fail(%d): %s\r\n", nError, codiErrorMessage(nError));
                        break;
                    }
                    head = head->next;
                }
			}
			else
			{
                isConnect = FALSE;
				XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			}
		}
		else
		{
            isOpen = FALSE;
			XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		}

        if(nError == CODIERR_NOERROR) break;

		// 1초 이후에 재시도 한다.
		usleep(1000000);
	}

    if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
    if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);

	return CODI_TO_IF4ERROR(nError);
}

//endiWriteLongRom
int	COndemander::LongRomWrite(HANDLE codi, EUI64 *id, UINT addr, BYTE *pszBuffer, int nLength, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
    int         nResSize;//, nSize;
    int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
    int         Limit = ZIGBEE_VALID_PAYLOAD_SIZE - 6;
    BYTE        response[Limit*2];
    int         timeout=-1;
    ENDIENTRY   *endiEntry;
    BYTE        hops;
    UINT        val4, val4_;
    WORD        val2, val2_;
    BYTE        sendBuffer[5];
    BOOL        bDone = FALSE;
    int         i, len=0, nSize;

    if(id) {
	    eui64toa(id, szGUID);
    }
    else {
        strcpy(szGUID,"all");
    }
	XDEBUG("ONDEMAND: End Device ROM Write (ID=%s, ADDRESS=0x%06x, LEN=%d)\r\n",
				szGUID, addr,nLength);

    memset(sendBuffer, '\0', 5);
    if(nLength < 3) //word_ type, crc
    {
        memcpy(&val2, pszBuffer, 2); 
        val2_=htons(val2);
        memcpy(sendBuffer, &val2_, 2);
        XDEBUG("VALUE1 : %04x\xa\xd", val2_);     
        XDUMP((char *)sendBuffer, 2);
    }
    else //len
    {
        memcpy(&val4, pszBuffer, 4); 
        val4_=htonl(val4);
        memcpy(sendBuffer, &val4_, 4);
        XDEBUG("VALUE2 : %06x\xa\xd", val4_);
        XDUMP((char *)sendBuffer, 4);
    }


	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

    if(id) {
	    nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	    for(nRetry=0; nRetry<nCount && !bDone; nRetry++)
	    {
		    nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		    if (nError == CODIERR_NOERROR)
		    {
			    nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			    if (nError == CODIERR_NOERROR)
			    {
                        if((endiEntry=m_pEndDeviceList->GetEndDeviceByID(id))==NULL) {
                            XDEBUG("ERROR: endiWriteLongRom: GetEndDevice Error %s\r\n", szGUID);
                             break;
                        }
                        
                        if(timeout<0) timeout = sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT);

                        if(m_pEndDeviceList->GetEndDeviceHops(endiEntry,&hops)==0xff) continue;
                        nSize = Limit - hops * 2;
                        XDEBUG("INFO: Limit=%d, Hops=%d, nSize=%d\r\n", Limit, hops, nSize);

                        memset(response,'\0',Limit*2); nResSize=0;

                        for(i=0,len=0;i<nLength;i+=len)
                        {
                            len = MIN(nLength-i, nSize);
                            nError = endiWriteLongRom(endi, addr, sendBuffer, len, response, &nResSize, timeout);
                            XDUMP((char *)response, nResSize); 
                            if (nError != CODIERR_NOERROR || nResSize != len || memcmp(sendBuffer,response,len)) {
					
                                XDEBUG("ERROR: endiWriteLongRom(%d): ID=%s, Retry=%d, nRead=%d, nResSize=%d, cmp=%d, (%s)\r\n", 
                                nError, szGUID, nRetry, len, nResSize, memcmp(sendBuffer,response,len), codiErrorMessage(nError));
                                XDUMP((char *)sendBuffer, len);
                                XDUMP((char *)response, nResSize);

                                break;
                            }
                            if(m_pEndDeviceList->GetEndDeviceHops(endiEntry,&hops)==0xff) break;
                            nSize = Limit - hops * 2;
                        }
                        bDone = TRUE;
                }
			    else
			    {
                    isConnect = FALSE;
				    XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			    }
		    }
		    else
		    {
                isOpen = FALSE;
			    XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		    }

		    // 1초 이후에 재시도 한다.
		    if(!bDone) usleep(1000000);
	    }

        if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
        if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);
    }else {
        for(i=0,len=0;i<nLength;i+=len)
        {
            len = MIN(nLength-i, Limit);
		    nError = multicastWriteLongRom(codi, addr, pszBuffer, len);
        }
    }

	return CODI_TO_IF4ERROR(nError);
}

int	COndemander::RomWrite(HANDLE codi, EUI64 *id, WORD addr, BYTE *pszBuffer, int nLength, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
    int         Limit = ZIGBEE_VALID_PAYLOAD_SIZE - 3; // payload - payloadheader
    int         i, len=0, nSize;
    int         timeout=-1;
    BOOL        bDone = FALSE;
    ENDIENTRY   *endiEntry;
    BYTE        hops;
	
    if(id) {
	    eui64toa(id, szGUID);
    }else {
        strcpy(szGUID,"all");
    }
	XDEBUG("ONDEMAND: End Device ROM Write (ID=%s, ADDRESS=0x%04X, LEN=%d)\r\n",
				szGUID, addr, nLength);

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

    if(id) {
	    nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	    for(nRetry=0; nRetry<nCount && !bDone; nRetry++)
	    {
		    nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		    if (nError == CODIERR_NOERROR)
		    {
			    nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			    if (nError == CODIERR_NOERROR)
			    {
                    if((endiEntry=m_pEndDeviceList->GetEndDeviceByID(id))==NULL) {
				        XDEBUG("ERROR: endiWriteRom: GetEndDevice Error %s\r\n", szGUID);
                        break;
                    }
                    if(timeout<0) timeout = sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT);
                    if(m_pEndDeviceList->GetEndDeviceHops(endiEntry,&hops)==0xff) continue;
                    nSize = Limit - hops * 2;
                    XDEBUG("INFO: Limit=%d, Hops=%d, nSize=%d\r\n", Limit, hops, nSize);

                    for(i=0,len=0;i<nLength;i+=len)
                    {
                        len = MIN(nLength-i, nSize);
				        nError = endiWriteRom(endi, addr, pszBuffer+i, len, timeout);
				        if (nError != CODIERR_NOERROR)
                        {
					        XDEBUG("ERROR: endiWriteRom(%d): %s\r\n", nError, codiErrorMessage(nError));
                            break;
                        }
                        if(m_pEndDeviceList->GetEndDeviceHops(endiEntry,&hops)==0xff) break;
                        nSize = Limit - hops * 2;
                    }
                    bDone = TRUE;
			    }
			    else
			    {
                    isConnect = FALSE;
				    XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			    }
		    }
		    else
		    {
                isOpen = FALSE;
			    XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		    }

		    // 1초 이후에 재시도 한다.
		    if(!bDone) usleep(1000000);
	    }

        if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
        if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);
    }else {
        for(i=0,len=0;i<nLength;i+=len)
        {
            len = MIN(nLength-i, Limit);
		    nError = multicastWriteRom(codi, addr, pszBuffer+i, len);
        }
    }

	return CODI_TO_IF4ERROR(nError);
}

int	COndemander::BypassWrite(HANDLE codi, EUI64 *id, BYTE *pszBuffer, int nLength, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
    int         Limit = ZIGBEE_VALID_PAYLOAD_SIZE; // payload - payloadheader
    int         i, len=0, nSize;
    int         timeout=-1;
    BOOL        bDone = FALSE;
    ENDIENTRY   *endiEntry;
    BYTE        hops;
	
    if(id) {
	    eui64toa(id, szGUID);
    }else {
        strcpy(szGUID,"all");
    }
	XDEBUG("ONDEMAND: End Device Bypass Write (ID=%s, LEN=%d)\r\n", szGUID, nLength);

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

    if(id) {
	    nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	    for(nRetry=0; nRetry<nCount && !bDone; nRetry++)
	    {
		    nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		    if (nError == CODIERR_NOERROR)
		    {
			    nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			    if (nError == CODIERR_NOERROR)
			    {
                    if((endiEntry=m_pEndDeviceList->GetEndDeviceByID(id))==NULL) {
				        XDEBUG("ERROR: endiBypass: GetEndDevice Error %s\r\n", szGUID);
                        break;
                    }
                    if(timeout<0) timeout = sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT);
                    if(m_pEndDeviceList->GetEndDeviceHops(endiEntry,&hops)==0xff) continue;
                    nSize = Limit - hops * 2;
                    XDEBUG("INFO: Limit=%d, Hops=%d, nSize=%d\r\n", Limit, hops, nSize);

                    for(i=0,len=0;i<nLength;i+=len)
                    {
                        len = MIN(nLength-i, nSize);
				        nError = endiBypass(endi, pszBuffer+i, len, timeout);
				        if (nError != CODIERR_NOERROR)
                        {
					        XDEBUG("ERROR: endiBypass(%d): %s\r\n", nError, codiErrorMessage(nError));
                            break;
                        }
                        if(m_pEndDeviceList->GetEndDeviceHops(endiEntry,&hops)==0xff) break;
                        nSize = Limit - hops * 2;
                    }
                    bDone = TRUE;
			    }
			    else
			    {
                    isConnect = FALSE;
				    XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			    }
		    }
		    else
		    {
                isOpen = FALSE;
			    XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		    }

		    // 1초 이후에 재시도 한다.
		    if(!bDone) usleep(1000000);
	    }

        if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
        if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);
    }else {
        for(i=0,len=0;i<nLength;i+=len)
        {
            len = MIN(nLength-i, Limit);
		    nError = multicastBypass(codi, pszBuffer+i, len);
        }
    }

	return CODI_TO_IF4ERROR(nError);
}

int	COndemander::SensorOTA(HANDLE codi, EUI64 *id, UINT addr, char *filename, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
    int         nCount;
	int			nError = CODIERR_NOERROR;
    int         Limit = ZIGBEE_VALID_PAYLOAD_SIZE - 6; // valid - payload header size
    int         nSize, nRead = 0, nResSize;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
    BOOL        bSkipRead = FALSE;
    BOOL        bOk = FALSE;
    BYTE        buffer[Limit];
    BYTE        response[Limit*2];
    ENDIENTRY   *endiEntry;
    BYTE        hops;
    int         fd;
    int         frameCnt=0;
    int         timeout=-1;
    time_t      tmStart, now;

    if(id) {
	    eui64toa(id, szGUID);
    }else {
        strcpy(szGUID,"all");
    }
	XDEBUG("ONDEMAND: Sensor OTA (ID=%s, ADDRESS=0x%06X, FIRMWARE=%s)\r\n", szGUID, addr, filename);

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

    if((fd = open(filename, O_RDONLY))<0) 
    {
		XDEBUG("ONDEMAND: Firmware file open error. FIRMWARE=%s\r\n", filename);
		return IF4ERR_INVALID_FILENAME;
    }

    time(&tmStart);
	UPGRADE_LOG("OTA: Sensor OTA (ID=%s, FIRMWARE=%s)\r\n", szGUID, filename);
    if(id) {
	    nCount = sensorRetryCount(id, 3);
	    for(nRetry=0; nRetry<nCount; nRetry++)
	    {
		    nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		    if (nError == CODIERR_NOERROR)
		    {
			    nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			    if (nError == CODIERR_NOERROR)
			    {
                    if((endiEntry=m_pEndDeviceList->GetEndDeviceByID(id))==NULL) {
				        XDEBUG("ERROR: endiWriteLongRom: GetEndDevice Error %s\r\n", szGUID);
                        break;
                    }
                    if(timeout<0) timeout = sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT);

                    if(m_pEndDeviceList->GetEndDeviceHops(endiEntry,&hops)==0xff) continue;
                    nSize = Limit - hops * 2;
				    XDEBUG("INFO: Limit=%d, Hops=%d, nSize=%d\r\n", Limit, hops, nSize);
		            while(bSkipRead || (nRead = read(fd, buffer, nSize)) > 0) {
                        bSkipRead = FALSE;
                        memset(response,0,sizeof(response)); nResSize=0;
				        nError = endiWriteLongRom(endi, addr, buffer, nRead, response, &nResSize, timeout);
				        if (nError != CODIERR_NOERROR || nResSize != nRead || memcmp(buffer,response,nRead)) {
					        XDEBUG("ERROR: endiWriteLongRom(%d): ID=%s, Retry=%d, nRead=%d, nResSize=%d, cmp=%d, (%s)\r\n", 
                                nError, szGUID, nRetry, nRead, nResSize, memcmp(buffer,response,nRead), codiErrorMessage(nError));
                            bSkipRead = TRUE;
                            break;
                        }
                        addr += nRead;
                        frameCnt ++;
                        nRetry = 0;
    
                        if(m_pEndDeviceList->GetEndDeviceHops(endiEntry,&hops)==0xff) break;
                        nSize = Limit - hops * 2;
                    }
				    if(nError == CODIERR_NOERROR && nRead <= 0) {
                        bOk = TRUE;
                        break;
                    }
			    }
			    else
			    {
                    isConnect = FALSE;
				    XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			    }
		    }
		    else
		    {
                isOpen = FALSE;
			    XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		    }

		    // 1초 이후에 재시도 한다.
		    usleep(1000000);
	    }
    }else {
        nSize = Limit;
		while((nRead = read(fd, buffer, nSize)) > 0) {
		    nError = multicastWriteLongRom(codi, addr, buffer, nRead);
			if (nError != CODIERR_NOERROR) {
			    XDEBUG("ERROR: endiWriteLongRom(%d): %s\r\n", nError, codiErrorMessage(nError));
                break;
            }
            addr += nRead;
            frameCnt ++;
            usleep(2000000);
        }
        bOk = TRUE;
    }

    time(&now);
    if(bOk) {
	    XDEBUG("OTA: Sensor OTA Write Succ (ID=%s, FrameCount=%d, Elapse time=%d min %d seconds)\r\n", 
            szGUID, frameCnt, (int)(now-tmStart) / 60, (int)(now-tmStart) % 60);
	    UPGRADE_LOG("OTA: Sensor OTA Write Succ (ID=%s, FrameCount=%d, Elapse time=%d min %d seconds)\r\n", 
            szGUID, frameCnt, (int)(now-tmStart) / 60, (int)(now-tmStart) % 60);
    }else {
	    XDEBUG("OTA: Sensor OTA Write Fail (ID=%s, FrameCount=%d, Elapse time=%d min %d seconds, Error=%s)\r\n", 
            szGUID, frameCnt, (int)(now-tmStart) / 60, (int)(now-tmStart) % 60, codiErrorMessage(nError));
	    UPGRADE_LOG("OTA: Sensor OTA Write Fail (ID=%s, FrameCount=%d, Elapse time=%d min %d seconds, Error=%s)\r\n", 
            szGUID, frameCnt, (int)(now-tmStart) / 60, (int)(now-tmStart) % 60, codiErrorMessage(nError));
    }

    close(fd);
    if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
    if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);

	return CODI_TO_IF4ERROR(nError);
}

int	COndemander::EndiCommand(HANDLE codi, EUI64 *id, BYTE cmd, BYTE type, BYTE *data, int nLength, BYTE *pszResult, int *nSize, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
	
	eui64toa(id, szGUID);
	XDEBUG("ONDEMAND: End Device COMMAND (ID=%s, CMD=0x%02X, LEN=%d)\r\n", szGUID, cmd, nLength);

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

	nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	for(nRetry=0; nRetry<nCount; nRetry++)
	{
		nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		if (nError == CODIERR_NOERROR)
		{
			nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			if (nError == CODIERR_NOERROR)
			{
                if(type == ENDI_CMD_WRITE_REQUEST)
				    nError = endiCommandWrite(endi, cmd, data, nLength, pszResult, nSize, sensorCommTimeout(id, 15000));
                else
				    nError = endiCommandRead(endi, cmd, data, nLength, pszResult, nSize, sensorCommTimeout(id, 15000));
				if (nError != CODIERR_NOERROR)
					XDEBUG("ERROR: endiCommand(%d): %s\r\n", nError, codiErrorMessage(nError));
				else break;
			}
			else
			{
                isConnect = FALSE;
				XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			}
		}
		else
		{
            isOpen = FALSE;
			XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		}

		// 1초 이후에 재시도 한다.
		usleep(1000000);
	}

    if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
    if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);

	return CODI_TO_IF4ERROR(nError);
}

int	COndemander::EndiCommandRead(HANDLE codi, EUI64 *id, BYTE cmd, BYTE *data, int nLength, BYTE *pszResult, int *nSize, 
        BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
    return EndiCommand(codi, id, cmd, ENDI_CMD_READ_REQUEST, data, nLength, pszResult, nSize, bIsOpen, bIsConnect, bKeepConnect);
}

int	COndemander::EndiCommandWrite(HANDLE codi, EUI64 *id, BYTE cmd, BYTE *data, int nLength, BYTE *pszResult, int *nSize, 
        BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
    return EndiCommand(codi, id, cmd, ENDI_CMD_WRITE_REQUEST, data, nLength, pszResult, nSize, bIsOpen, bIsConnect, bKeepConnect);
}

int	COndemander::AmrRead(HANDLE codi, EUI64 *id, BYTE *pszBuffer, int *nSize, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
	
	eui64toa(id, szGUID);
	XDEBUG("ONDEMAND: End Device AMR Read (ID=%s)\r\n", szGUID);

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

	nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	for(nRetry=0; nRetry<nCount; nRetry++)
	{
		nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		if (nError == CODIERR_NOERROR)
		{
			nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			if (nError == CODIERR_NOERROR)
			{
				nError = endiReadAmr(endi, pszBuffer, nSize, sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT));
				if (nError != CODIERR_NOERROR)
					XDEBUG("ERROR: endiReadAmr(%d): %s\r\n", nError, codiErrorMessage(nError));
				else break;
			}
			else
			{
                isConnect = FALSE;
				XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			}
		}
		else
		{
            isOpen = FALSE;
			XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		}

		// 1초 이후에 재시도 한다.
		usleep(1000000);
	}

    if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
    if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);

	return CODI_TO_IF4ERROR(nError);
}

int	COndemander::AmrWrite(HANDLE codi, EUI64 *id, WORD mask, BYTE *pszBuffer, int nLength, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
	
	eui64toa(id, szGUID);
	XDEBUG("ONDEMAND: End Device AMR Write (ID=%s, MASK=0x%04X, LEN=%d)\r\n", szGUID, mask, nLength);

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

	nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	for(nRetry=0; nRetry<nCount; nRetry++)
	{
		nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
		if (nError == CODIERR_NOERROR)
		{
			nError = ((isConnect == FALSE) ? sensorConnect(endi, DEFAULT_BIND_TIMEOUT) : CODIERR_NOERROR); isConnect = TRUE;
			if (nError == CODIERR_NOERROR)
			{
				nError = endiWriteAmr(endi, mask, pszBuffer, nLength, sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT));
				if (nError != CODIERR_NOERROR)
					XDEBUG("ERROR: endiWriteAmr(%d): %s\r\n", nError, codiErrorMessage(nError));
				else break;
			}
			else
			{
                isConnect = FALSE;
				XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
			}
		}
		else
		{
            isOpen = FALSE;
			XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
		}

		// 1초 이후에 재시도 한다.
		usleep(1000000);
	}

    if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
    if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);

	return CODI_TO_IF4ERROR(nError);
}

int	COndemander::EndiSend(HANDLE codi, EUI64 *id, BYTE rw, BYTE type, BYTE *payload, int nLength, BYTE *data, int *datalen, int nTimeout, BOOL bIsOpen, BOOL bIsConnect, BOOL bKeepConnect)
{
	HANDLE		endi;
	char		szGUID[17];
	int			nRetry;
	int			nCount, nError = CODIERR_NOERROR;
    BOOL        isOpen = bIsOpen;
    BOOL        isConnect = bIsConnect;
	
    if(id) {
	    eui64toa(id, szGUID);
    }else {
        return IF4ERR_INVALID_ID;
    }
	XDEBUG("ONDEMAND: End Device Data Send (ID=%s, RW=%d, TYPE=%d, LEN=%d, TIMEOUT=%d)\r\n",
				szGUID, rw, type, nLength, nTimeout);

	if (codiGetState(codi) != CODISTATE_NORMAL)
	{
		XDEBUG("ONDEMAND: Coordinator not ready. (codiState=%d)\r\n", codiGetState(codi));
		return IF4ERR_NOT_READY;
	}

	nCount = sensorRetryCount(id, DEFAULT_RETRY_COUNT);
	for(nRetry=0; nRetry<nCount; nRetry++)
	{
	    nError = ((isOpen == FALSE) ? endiOpen(codi, &endi, id) : CODIERR_NOERROR); isOpen = TRUE;
	    if (nError == CODIERR_NOERROR)
	    {
		    nError = ((isConnect == FALSE) ? 
                        sensorConnect(endi, DEFAULT_BIND_TIMEOUT > nTimeout ? DEFAULT_BIND_TIMEOUT : nTimeout) : 
                        CODIERR_NOERROR); isConnect = TRUE;
		    if (nError == CODIERR_NOERROR)
		    {
                if(data && datalen) {
			        nError = endiSendRecv(endi, rw, type, payload, nLength, data, datalen,
                            sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT > nTimeout ? DEFAULT_COMM_TIMEOUT : nTimeout));
                }else {
			        nError = endiSend(endi, rw, type, payload, nLength, 
                            sensorCommTimeout(id, DEFAULT_COMM_TIMEOUT > nTimeout ? DEFAULT_COMM_TIMEOUT : nTimeout));
                }
			    if (nError != CODIERR_NOERROR)
				    XDEBUG("ERROR: endiSendData(%d): %s\r\n", nError, codiErrorMessage(nError));
			    else break;
		    }
		    else
		    {
                isConnect = FALSE;
			    XDEBUG("ERROR: endiConnect(%d): %s\r\n", nError, codiErrorMessage(nError));
		    }
	    }
	    else
	    {
            isOpen = FALSE;
		    XDEBUG("ERRROR: endiOpen(%d): %s\r\n", nError, codiErrorMessage(nError));
	    }

	    // 1초 이후에 재시도 한다.
	    usleep(1000000);
	}

    if((nError != IF4ERR_NOERROR && isConnect) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isConnect)) endiDisconnect(endi);
    if((nError != IF4ERR_NOERROR && isOpen) ||
            (nError == IF4ERR_NOERROR && !bKeepConnect && isOpen)) endiClose(endi);

	return CODI_TO_IF4ERROR(nError);
}

//////////////////////////////////////////////////////////////////////
// COndemander Event
//////////////////////////////////////////////////////////////////////

BOOL COndemander::OnData(EUI64 *id, BYTE nSeq, BYTE *pszData, BYTE nLength)
{
	ONDEMANDPARAM	*pOndemand;
    BOOL bOndemand = FALSE;

	LockOperation();
	pOndemand = FindOperation(id);
	if ((pOndemand != NULL) && (pOndemand->nOperation == ONDEMAND_OPR_DATA))
	{
	    CMeterParser	*pParser;
		if (pOndemand->pParser != NULL)
		{
            // Thread Debugging
            __PTHREAD_INFO__;

			pParser = (CMeterParser *)pOndemand->pParser;
			pParser->OnData(id, pOndemand, nSeq, pszData, nLength);
		}
        bOndemand = TRUE;
	}
	UnlockOperation();	

    return bOndemand;
}

void COndemander::OnError(EUI64 *id, BYTE nError)
{
	ONDEMANDPARAM	*pOndemand;

	LockOperation();
	pOndemand = FindOperation(id);
	if (pOndemand != NULL)
	{
		if (pOndemand->nOperation == ONDEMAND_OPR_DATA)
			pOndemand->nLastError = nError;
	}
	UnlockOperation();	
}

