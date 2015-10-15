//////////////////////////////////////////////////////////////////////
//
//  pulseMeter.cpp: implementation of the CPulseParser class.
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
#include "codiapi.h"
#include "Chunk.h"
#include "Event.h"
#include "MeterUploader.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "ParserPulse.h"

static const char *parserNameArray[] = {"PULSE", "NAPC", "BAT", "END", NULL};

//////////////////////////////////////////////////////////////////////
// CPulseParser Class
//////////////////////////////////////////////////////////////////////

CPulseParser::CPulseParser() 
            : CPulseBaseParser(SENSOR_TYPE_ZEUPLS, SERVICE_TYPE_UNKNOWN, 
                                ENDDEVICE_ATTR_RFD | ENDDEVICE_ATTR_BATTERYNODE,
								const_cast<char **>(parserNameArray), PARSER_TYPE_PULSE, 
                                "Battery-Pulse Meter Parser", "PULSE", parserNameArray[0],
                                100000, 6000, 1)
{
    m_nFFDConnectionTimeout = 6000;
}

CPulseParser::~CPulseParser()
{
}

int CPulseParser::GetConnectionTimeout(int nAttribute)
{
	return (nAttribute & ENDDEVICE_ATTR_RFD) ?  m_nSensorConnectionTimeout : m_nFFDConnectionTimeout;
}

