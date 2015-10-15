//////////////////////////////////////////////////////////////////////
//
//	DlmsTcpClient.cpp: implementation of the CDlmsTcpClient class.
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

#include "common.h"
#include "mcudef.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "AnsiUtil.h"
#include "DebugUtil.h"

#include "ansiDefine.h"
#include "ansiUtils.h"
#include "ansiError.h"

#include "types/units.h"

//////////////////////////////////////////////////////////////////////

const char *ansiGetErrorMessage(int nError)
{
	switch(nError) {
	  case ANSIERR_NOERROR :				return "OK";
	  case ANSIERR_INVALID_PARAM :			return "Invalid param";
	  case ANSIERR_INVALID_SESSION :		return "Invalid session";
	  case ANSIERR_INVALID_HANDLE :			return "Invalid handle";
	  case ANSIERR_INVALID_ADDRESS :		return "Invalid address";
	  case ANSIERR_INVALID_TIMEOUT :		return "Invalid timeout";
	  case ANSIERR_INVALID_ACCESS :			return "Invalid access";
	  case ANSIERR_INVALID_DATA :			return "Invalid data";
	  case ANSIERR_INVALID_ACTION :			return "Invalid action";
	  case ANSIERR_INVALID_STRING :			return "Invalid string";
	  case ANSIERR_INVALID_LENGTH :			return "Invalid length";
	  case ANSIERR_INVALID_BUFFER :			return "Invalid buffer";
      case ANSIERR_FRAME_ERROR :            return "Frame error";
	  case ANSIERR_CANNOT_CONNECT :			return "Cannot connect";
	  case ANSIERR_REPLY_TIMEOUT :			return "Reply timeout";
	  case ANSIERR_NOT_INITIALIZED :		return "Not initialized";
	  case ANSIERR_SYSTEM_ERROR :			return "System error";
	  case ANSIERR_MEMORY_ERROR :			return "Memory error";
	}

	return UNKNOWN_STR;
}

const char *ansiGetStatus(BYTE nStatus)
{
    switch(nStatus)
    {
        case ANSI_RES_NO_ERROR:             return "OK";
        case ANSI_RES_ERROR:                return "Error";
        case ANSI_RES_SNS:                  return "Not support";
        case ANSI_RES_ISC:                  return "Insufficient security clearance";
        case ANSI_RES_ONP:                  return "Not possible";
        case ANSI_RES_IAR:                  return "Inappropriate action requested";
        case ANSI_RES_BSY:                  return "Busy";
        case ANSI_RES_DNR:                  return "Data not ready";
        case ANSI_RES_DLK:                  return "Data locked";
        case ANSI_RES_RNO:                  return "Renegotiate request";
        case ANSI_RES_ISSS:                 return "Invalid service sequence state";
    }
    return UNKNOWN_STR;
}

const char *ansiGetProcedureName(WORD nTableType, BYTE nProc)
{
    if(nTableType == ANSI_MANUFACT_TBL)
    {
        switch(nProc)
        {
            case ANSI_PROC_CLEAR_INDIVIDUAL_ST_STATUS_FLAGS:
                return "CLEAR INDIVIDUAL ST STATUS FLAGS";
            case ANSI_PROC_CLEAR_INDIVIDUAL_MF_STATUS_FLAGS:
                return "CLEAR INDIVIDUAL MF STATUS FLAGS";
            case ANSI_PROC_DIRECT_EXECUTE:
                return "DIRECT EXECUTE";
            case ANSI_PROC_BATTERY_TEST:
                return "BATTERY TEST";
            case ANSI_PROC_CONFIGURE_TEST_PULSES:
                return "CONFIGURE TEST PLUSES";
            case ANSI_PROC_RESET_CUMULATIVE_OUTAGE_DURATION:
                return "RESET CUMULATIVE OUTAGE DURATION";
            case ANSI_PROC_CHANGE_CONFIGURATION_STATUS:
                return "CHANGE CONFIGURATION STATUS";
            case ANSI_PROC_CONVERT_METER_MODE:
                return "CONVERT METER MODE";
            case ANSI_PROC_UPGRADE_METER_FUNCTION:
                return "UPGRADE METER FUNCTION";
            case ANSI_PROC_PROGRAM_CONTROL:
                return "PROGRAM CONTROL";
            case ANSI_PROC_RESET_ERROR_HISTORY:
                return "RESET ERROR HISTORY";
            case ANSI_PROC_FLASH_CAL_MODE_CONTROL:
                return "FLASH CAL MODE CONTROL";
            case ANSI_PROC_SET_LAST_CALIBRATION_DATE_TIME:
                return "SET LAST CALIBRATION DATE TIME";
            case ANSI_PROC_REMOVE_SOFTSWITCH:
                return "REMOVE SOFTSWITCH";
            case ANSI_PROC_RTP_CONTROL:
                return "RTP CONTROL";
            case ANSI_PROC_SNAPSHOT_REVENUE_DATA:
                return "SNAPSHOT REVENUE DATA";
            case ANSI_PROC_TIME_ACCELERATION:
                return "TIME ACCELERATA DATA";
        }
    }
    else
    {
        switch(nProc)
        {
            case ANSI_PROC_COLD_START:
                return "COLD START";
            case ANSI_PROC_CLEAR_DATA:
                return "CLEAR DATA";
            case ANSI_PROC_RESET_LIST_POINTERS:
                return "RESET LIST POINTERS";
            case ANSI_PROC_UPDATE_LAST_READ_ENTRY:
                return "UPDATE LAST READ ENTRY";
            case ANSI_PROC_CHANGE_END_DEVICE_MODE:
                return "CHANGE END DEVICE MODE";
            case ANSI_PROC_REMOTE_RESET:
                return "REMOTE RESET";
            case ANSI_PROC_SET_DATE_TIME:
                return "SET DATE TIME";
            case ANSI_PROC_CLEAR_ALL_PENDING_TABLES:
                return "CLEAR ALL PENDING TABLES";
            case ANSI_PROC_CLEAR_SELECTED_PENDING_TABLES:
                return "CLEAR SELECTED PENDING TABLES";
            case ANSI_PROC_START_LP_RECORDING:
                return "START LP RECORDING";
            case ANSI_PROC_STOP_LP_RECORDING:
                return "STOP LP RECORDING";
            case ANSI_PROC_DIRECT_LOAD_CONTROL:
                return "DIRECT LOAD CONTROL";
        }
    }
    return UNKNOWN_STR;
}

/** Control Frame 중 Toggle bit를 변경 한다
  */
BYTE ansiToggle(BYTE ctrl)
{
    return (BYTE)(ctrl ^ ANSI_CTRL_TOGGLE);
}


/** ANSI Unit Code를 AiMiR Unit Code로 변환한다.
 *
 * @warning 전기미터의 Unit 값 만을 취급한다.
 *          또한 현재 SM110에서 지원하는 값만 취급한다. 
 *
 * @todo 전체 Unit에 대한 변환이 필요하다.
 */
BYTE ansiUnit2AimirUnit(BYTE nUnit)
{
    switch(nUnit)
    {
        case 0: return UNIT_TYPE_WATT_HOUR;
        case 1: return UNIT_TYPE_VOLTAMPEREREACTIVE;
        case 2: return UNIT_TYPE_VOLTAMPERE;
        case 8: return UNIT_TYPE_VOLT;
    }
    return UNIT_TYPE_UNDEFINED;
}

/** ANSI Standard Table #12의 정보를 이용해서 
 *
 * AiMiR Channel Type 을 구한다.
 */
BYTE ansiChannel2AimirChannel(BYTE nIdCode, BYTE nTimeBase, BOOL bNetFlow,
        BOOL bQ1, BOOL bQ2, BOOL bQ3, BOOL bQ4)
{
    switch(nIdCode)
    {
        case 0:
            switch(nTimeBase) 
            {
                case 5:
                    if(bQ1 && !bQ2 && !bQ3 && bQ4) return 1;
                    if(!bQ1 && bQ2 && bQ3 && !bQ4) return 2;
                    if(bQ1 && bQ2 && bQ3 && bQ4 && !bNetFlow) return 15;
                    if(bQ1 && bQ2 && bQ3 && bQ4 && bNetFlow) return 16;
                    break;
            }
            break;
        case 1:
            switch(nTimeBase) 
            {
                case 5:
                    if(bQ1 && bQ2 && !bQ3 && !bQ4) return 3;
                    if(!bQ1 && !bQ2 && bQ3 && bQ4) return 4;
                    break;
            }
            break;
        case 8:
            switch(nTimeBase) 
            {
                case 2:
                    if(!bQ1 && !bQ2 && !bQ3 && !bQ4) return 32;
                    break;
            }
            break;
    }

    return 0;
}
