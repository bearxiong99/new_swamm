//////////////////////////////////////////////////////////////////////
//
//  unknown.cpp: implementation of the CUnknownParser class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2008 Nuritelecom Co., Ltd. All Rights Reserved.
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

#include "ParserUnknown.h"

//////////////////////////////////////////////////////////////////////
// CUnknownParser Class
//////////////////////////////////////////////////////////////////////


static const char *parserNameArray[] = {"UNKNOWN", "ELECTRIC", NULL};

//////////////////////////////////////////////////////////////////////
// CUnknownParser Class
//////////////////////////////////////////////////////////////////////

CUnknownParser::CUnknownParser() : CMeterParser(SENSOR_TYPE_UNKNOWN, SERVICE_TYPE_UNKNOWN,
							  ENDDEVICE_ATTR_UNKNOWN,
							  const_cast<char **>(parserNameArray), PARSER_TYPE_UNKNOWN,
                              "UNKNOWN Parser", "NURITELECOM", parserNameArray[0], 
                              6000, 6000, 2)
{
}

CUnknownParser::~CUnknownParser()
{
}

//////////////////////////////////////////////////////////////////////
// CUnknownParser Operations
//////////////////////////////////////////////////////////////////////

int CUnknownParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
    if(pCommand->nOption == ONDEMAND_OPTION_READ_IDENT_ONLY) return 0;   // Inventory Scanner에서 붚필요하게 Ondemand 하는 부분 제거
    return UNKNOWN_MAIN(pCommand, pStat);
}

int CUnknownParser::UNKNOWN_MAIN(CMDPARAM *pCommand, METER_STAT *pStat)
{
	return ONDEMAND_ERROR_NOT_CONNECT;
}

