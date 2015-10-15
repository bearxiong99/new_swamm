//////////////////////////////////////////////////////////////////////
//
//  mbus.cpp: implementation of the CMstr711Parser class.
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
//#include "codiapi.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "Chunk.h"
#include "CommonUtil.h"
#include "Utility.h"
#include "DebugUtil.h"

#include "ParserMstr711.h"
//////////////////////////////////////////////////////////////////////
// CMstr711Parser Class
//////////////////////////////////////////////////////////////////////

CMstr711Parser	*m_pMstr711Parser = NULL;

static const char *parserNameArray[] = {"MSTR711", NULL};
//////////////////////////////////////////////////////////////////////
// CMstr711Parser Class
//////////////////////////////////////////////////////////////////////

/** SubCPU Type으로 구성될 경우 Model 에 Parent 의 Model 정보를 넣어주어야 한다. 
  * Model이 Null 일 경우 Inventory Scanner에서 읽은 NODE_KIND를 사용하게 되는데 SubCPU는
  * 해당 정보를 얻을 수 없고 또한 이렇게 들어간 Model 정보를 Parent Type을 아는데 사용할 수 있다
  */
CMstr711Parser::CMstr711Parser():CMeterParser(SENSOR_TYPE_ZEUMBUS,SERVICE_TYPE_HEAT, ENDDEVICE_ATTR_MBUS_ARM, 
									const_cast<char **>(parserNameArray), PARSER_TYPE_MBUS, "MSTR711 Parser", "NAMR-H101MG", parserNameArray[0], 
                                    6000, 6000, 2)
{
}

CMstr711Parser::~CMstr711Parser()
{
}

//////////////////////////////////////////////////////////////////////
// CMstr711Parser Operations
//////////////////////////////////////////////////////////////////////
// rom을 읽거나 bypass 로 하거나 둘중하나로 처리한다. 
//
int CMstr711Parser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
    XDEBUG("ON COMMAND %d\xd\xa", pCommand->nType);
	switch(pCommand->nType) {
	  case PARSE_TYPE_READ :
		   return MBUS_METERING(pCommand, pStat);

	}
    return 0;
}
//Active energy 14 cid 1 번 추출해서 넣는다. 
//
int CMstr711Parser::MBUS_METERING(CMDPARAM *pCommand, METER_STAT *pStat)
{
	return 1;
}



