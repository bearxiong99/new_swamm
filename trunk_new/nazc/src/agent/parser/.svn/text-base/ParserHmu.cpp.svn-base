//////////////////////////////////////////////////////////////////////
//
//  hmu.cpp: implementation of the CHMUParser class.
//
//  This file is a part of the AIMIR.
//  (c)Copyright 2009 Nuritelecom Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
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

#include "ParserHmu.h"

static const char *parserNameArray[] = {"HMU", "NHMU", NULL};

//////////////////////////////////////////////////////////////////////
// CHMUParser Class
//////////////////////////////////////////////////////////////////////

CHMUParser::CHMUParser() : CPulseBaseParser(SENSOR_TYPE_HMU, SERVICE_TYPE_ELEC, 
                                ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL,
								const_cast<char **>(parserNameArray), PARSER_TYPE_HMU, "HMU Parser", "HMU", parserNameArray[0],
                                6000, 6000, 2)
{
}

CHMUParser::~CHMUParser()
{
}

