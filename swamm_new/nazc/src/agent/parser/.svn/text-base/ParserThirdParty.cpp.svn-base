//////////////////////////////////////////////////////////////////////
//
//  thirdPart.cpp: implementation of the CThirdPartyParser class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2010 Nuritelecom Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "EndDeviceList.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "Ondemander.h"
#include "ParserThirdParty.h"

//////////////////////////////////////////////////////////////////////
// CThirdPartyParser Class
//////////////////////////////////////////////////////////////////////


static const char *parserNameArray[] = {"3PARTY", "NURITHIRDPARTY", NULL};

//////////////////////////////////////////////////////////////////////
// CThirdPartyParser Class
//////////////////////////////////////////////////////////////////////

CThirdPartyParser::CThirdPartyParser() : CMeterParser(SENSOR_TYPE_UNKNOWN, SERVICE_TYPE_UNKNOWN,
							  ENDDEVICE_ATTR_UNKNOWN,
							  const_cast<char **>(parserNameArray), PARSER_TYPE_3PARTY, 
                              "3rd Party Parser", "NURITELECOM", parserNameArray[0],
                              6000, 6000, 2)
{
}

CThirdPartyParser::~CThirdPartyParser()
{
}

//////////////////////////////////////////////////////////////////////
// CThirdPartyParser Operations
//////////////////////////////////////////////////////////////////////

int CThirdPartyParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
	switch(pCommand->nType) {
	  case PARSE_TYPE_READ :
           if(pCommand->nOption == ONDEMAND_OPTION_READ_IDENT_ONLY) return 0;   // Inventory Scanner에서 붚필요하게 Ondemand 하는 부분 제거
		   return MAIN(pCommand, pStat);

	  case PARSE_TYPE_WRITE :
	  case PARSE_TYPE_TIMESYNC :
		   return 0;
	}
    return 0;
}

int CThirdPartyParser::MAIN(CMDPARAM *pCommand, METER_STAT *pStat)
{
	return ONDEMAND_ERROR_NOT_CONNECT;
}

