////////////////////////////////////////////////////////////////////
//
//  CoordinatorServer.cpp: implementation of the CCoordinatorServerServer class.
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
#include "CoordinatorMessage.h"
#include "DebugUtil.h"

////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

const char *GetInformationTypeName(BYTE nType)
{
	switch(nType) {
	  case CODI_INFO_BOOT :				return "BOOT";
	  case CODI_INFO_STACK_STATUS :		return "STACK STATUS";
	  case CODI_INFO_RESERVED0 :		return "RESERVED0";
	  case CODI_INFO_JOIN :				return "DEVICE JOIN";
	  case CODI_INFO_ENERGY_SCAN :		return "ENERGY SCAN";
	  case CODI_INFO_ACTIVE_SCAN :		return "ACTIVE SCAN";
	  case CODI_INFO_SCAN_COMPLETE :	return "SCAN COMPLETE";
	  case CODI_INFO_ROUTE_RECORD :		return "ROUTE RECORD";
	  case CODI_INFO_ERROR :			return "ERROR";
	}
	return "UNKNOWN INFORMATION";
}

const char *GetErrorTypeName(BYTE nError)
{
	switch(nError) {
	  case CODI_SUCCESS :							return "No error";
	  case CODI_INFOERR_SERIAL_TIME_OUT :			return "Serial timeout";
	  case CODI_INFOERR_SERIAL_ERROR :				return "Serial error";
	  case CODI_INFOERR_FRAME_CONTROL_ERROR : 		return "Frame control error";
	  case CODI_INFOERR_REQUEST_ERROR :				return "CMD request error";
	  case CODI_INFOERR_FRAME_TYPE_ERROR : 			return "CMD type error";
	  case CODI_INFOERR_CRC16_ERROR :				return "CRC16 error";

	  case CODI_CMDERR_NOT_SUPPORTED :				return "Command not supported";
	  case CODI_CMDERR_INVALID_DATA :				return "Command invalid data";
	  case CODI_CMDERR_NO_NETWORK_ERROR :			return "No network error";
	  case CODI_CMDERR_FORM_NETWORK_ERROR :			return "Form network error";
	  case CODI_CMDERR_LEAVE_NETWORK_ERROR :		return "Leave network error";
	  case CODI_CMDERR_BIND_ADDRESS_DUPLICATION :	return "Binding address duplication error";
	  case CODI_CMDERR_FULL_BINDING_INDEX :			return "Full binding index error";
	  case CODI_CMDERR_EUI64_FOUND_FAIL_ERROR :		return "EUI64 Found fail error";
	  case CODI_CMDERR_SET_BINDING_ERROR :			return "Set binding error";
	  case CODI_CMDERR_DELETE_BINDING_ERROR :		return "Delete binding error";
	  case CODI_CMDERR_BINDING_IS_ACTIVE_ERROR :	return "Binding is active error";
      case CODI_CMDERR_GET_BINDING_ERROR :          return "Get binding error";
	  case CODI_CMDERR_GET_NEIGHBOR_ERROR :         return "Get neighbor error";
	  case CODI_CMDERR_KEY_TABLE_DUPLICATION :      return "Key table duplication";
	  case CODI_CMDERR_NULL_KEY_INDEX :             return "Null key index";
	  case CODI_CMDERR_SET_KEY_ERROR :              return "Set key error";
	  case CODI_CMDERR_KEY_FOUND_FAIL_ERROR :       return "Key found fail error";
	  case CODI_CMDERR_DEL_KEY_ERROR :              return "Delete key error";
	  case CODI_CMDERR_ADD_ERROR:					return "Key Add Error";
	  case CODI_CMDERR_DEL_ERROR:					return "Key Del Error";													  case CODI_CMDERR_ADD_DUPLICATE_ERROR:			return "Key Duplicate Add Error";	

	  case CODI_DATAERR_SEND_DATA_ERROR :			return "Send data error";
	  case CODI_DATAERR_PAYLOAD_LENGTH_ERROR :		return "Payload length error";
	  case CODI_DATAERR_DELIVERY_ERROR :			return "Delivery error";
	  case CODI_DATAERR_BINDING_ERROR :				return "Binding error";
	}
	return "Unknown Error";
}

const char *GetCommandTypeName(BYTE nType)
{
	switch(nType) {
	  case CODI_CMD_MODULE_PARAM :			return "MODULE PARAM";
	  case CODI_CMD_SERIAL_PARAM :			return "SERIAL PARAM";
	  case CODI_CMD_NETWORK_PARAM :			return "NETWORK PARAM";
	  case CODI_CMD_SECURITY_PARAM :		return "SECURITY PARAM";
	  case CODI_CMD_SCAN_PARAM :			return "SCAN PARAM";
	  case CODI_CMD_PINGPONG :				return "PINGPONG";
	  case CODI_CMD_BIND :					return "BIND";
	  case CODI_CMD_BINDING_TABLE :			return "BINDING TABLE";
	  case CODI_CMD_NEIGHBOR_TABLE :		return "NEIGHBOR NODE TABLE";
	  case CODI_CMD_STACK_MEMORY :			return "STACK MEMORY";
	  case CODI_CMD_PERMIT :				return "PERMIT";
	  case CODI_CMD_FORM_NETWORK :			return "FORM NETWORK";
	  case CODI_CMD_RESET :					return "RESET";
	  case CODI_CMD_SCAN_NETWORK :			return "SCAN NETWORK";
	  case CODI_CMD_MANY_TO_ONE_NETWORK :	return "MANY TO ONE NETWORK";
	  case CODI_CMD_BOOTLOAD :				return "BOOTLOAD";
	  case CODI_CMD_ROUTE_DISCOVERY :		return "ROUTE DISCOVERY";
	  case CODI_CMD_MULTICAST_HOP :		    return "MULTICAST HOP";
	  case CODI_CMD_EXTENDED_TIMEOUT :		return "EXTENDED TIMEOUT";
	  case CODI_CMD_DIRECT_COMM :		    return "DIRECT COMMUNICATION";
	  case CODI_CMDERR_ADD_ERROR:			return "Key Add Error"; 		// Temp Add
	  case CODI_CMDERR_DEL_ERROR:			return "Key Del Error";			// Temp Add										  
	  case CODI_CMDERR_ADD_DUPLICATE_ERROR:	return "Key Duplicate Add Error";	// Temp Add								
	  case CODI_CMD_ADD_EUI64:              return "ADD EUI64";					// Temp Add
	  case CODI_CMD_DELETE_EUI64:           return "DELETE EUI64";				// Temp Add					
	}
	return "UNKNOWN COMMAND";
}

const char *GetDataTypeName(BYTE nType)
{
	switch(nType) {
	  case ENDI_DATATYPE_BYPASS :			return "BYPASS";
	  case ENDI_DATATYPE_NETWORK_LEAVE :	return "NETWORK LEAVE";
	  case ENDI_DATATYPE_AMR :				return "AMR";
	  case ENDI_DATATYPE_ROM_READ :			return "ROM READ";
	  case ENDI_DATATYPE_MCCB :				return "MCCB";
	  case ENDI_DATATYPE_TIMESYNC :			return "TIMESYNC";
	  case ENDI_DATATYPE_METERING :			return "METERING";
	  case ENDI_DATATYPE_EVENT :			return "EVENT";
	  case ENDI_DATATYPE_LINK :				return "LINK";
	  case ENDI_DATATYPE_LONG_ROM_READ :	return "OTA";
	  case ENDI_DATATYPE_COMMAND :	        return "COMMAND";
	  case ENDI_DATATYPE_STATE :	        return "STATE";
	  case ENDI_DATATYPE_DISCOVERY :	    return "DISCOVERY";
	  case ENDI_DATATYPE_SUPER_PARENT :	    return "SUPER PARENT";
	  case ENDI_DATATYPE_MBUS :	            return "MBUS";
	  case ENDI_DATATYPE_DIO :	            return "D I/O";
	  case ENDI_DATATYPE_NAN :              return "NAN";
	  case ENDI_DATATYPE_MBUS_NEW :	        return "MBUSN";
	  case ENDI_DATATYPE_PUSH :	            return "PUSH";
	}
	return "UNKNOWN DATA";
}

void PrintInformationFrameHeader(CODI_INFO_FRAME *pFrame, int nLength)
{
	XDEBUG(" INFORMATION (ACCESS=%s, FLOW=%s, TYPE=%s[0x%02X], LENGTH=%d)\r\n",
			(pFrame->hdr.ctrl & CODI_FRAMEFLOW_READ) > 0 ? "Read" : "Write",
			(pFrame->hdr.ctrl & CODI_FRAMEFLOW_RESPONSE) > 0 ? "Response" : "Request",
			GetInformationTypeName(pFrame->hdr.type),
			pFrame->hdr.type, pFrame->hdr.len);
}

void PrintCommandFrameHeader(CODI_COMMAND_FRAME *pFrame, int nLength)
{
	XDEBUG(" COMMAND (ACCESS=%s, FLOW=%s, TYPE=%s[0x%02X], LENGTH=%d)\r\n",
			(pFrame->hdr.ctrl & CODI_FRAMEFLOW_READ) > 0 ? "Read" : "Write",
			(pFrame->hdr.ctrl & CODI_FRAMEFLOW_RESPONSE) > 0 ? "Response" : "Request",
			GetCommandTypeName(pFrame->hdr.type),
			pFrame->hdr.type, pFrame->hdr.len);
}

void PrintDataFrameHeader(ENDI_DATA_FRAME *pFrame, int nLength)
{
	XDEBUG(" DATA (ACCESS=%s, FLOW=%s, TYPE=%s[0x%02X], LENGTH=%d)\r\n",
			(pFrame->hdr.ctrl & CODI_FRAMEFLOW_READ) > 0 ? "Read" : "Write",
			(pFrame->hdr.ctrl & CODI_FRAMEFLOW_RESPONSE) > 0 ? "Response" : "Request",
			GetDataTypeName(pFrame->hdr.type),
			pFrame->hdr.type, pFrame->hdr.len-sizeof(EUI64));

	// Link frame중 Write response이면 에러 코드를 찍는다.
	if (pFrame->hdr.type == ENDI_DATATYPE_LINK)
	{
		if (((pFrame->hdr.ctrl & CODI_FRAMEFLOW_READ) == 0) && 
			((pFrame->hdr.ctrl & CODI_FRAMEFLOW_RESPONSE) > 0))
		{
			// 에러인 경우만 로그를 찍는다.
			if ((pFrame->payload[0] & 0xff) != 0)
			{
				XDEBUG("    \033[1;40;31mWrite Result = 0x%02X %s\033[0m\r\n", 
					pFrame->payload[0] & 0xff,
					GetErrorTypeName(pFrame->payload[0] & 0xff));
			}
		}
	}
}

