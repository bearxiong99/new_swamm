//////////////////////////////////////////////////////////////////////
//
//  ihd.cpp: implementation of the CIhdParser class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2009 Nuritelecom Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "Ondemander.h"
#include "Chunk.h"
#include "Utility.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "EndDeviceList.h"

#include "ParserIhd.h"
#include "vendor/ihddef.h"

//////////////////////////////////////////////////////////////////////
// CIhdParser Class
//////////////////////////////////////////////////////////////////////


static const char *parserNameArray[] = {"IHD", "NZIHD", NULL};

//////////////////////////////////////////////////////////////////////
// CIhdParser Class
//////////////////////////////////////////////////////////////////////

CIhdParser::CIhdParser() : CPollBaseParser(SENSOR_TYPE_IHD, SERVICE_TYPE_ELEC,
							  ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL,
							  const_cast<char **>(parserNameArray), PARSER_TYPE_IHD, 
                              "IHD Parser", "NURITELECOM", parserNameArray[0], 
                              6000, 6000, 2)
{
}

CIhdParser::~CIhdParser()
{
}

//////////////////////////////////////////////////////////////////////
// CIhdParser Operations
//////////////////////////////////////////////////////////////////////

void CIhdParser::ActPostSuccAction(CChunk *pChunk, ONDEMANDPARAM *pOndemand, void *pExtraData)
{
    IHD_METERING_DATA data;
    ENDIENTRY *pEndi;

    pEndi = m_pEndDeviceList->GetEndDeviceByID(&pOndemand->id);

    memset(&data, 0, sizeof(IHD_METERING_DATA));

    data.infoLength = sizeof(IHD_METERING_DATA);
    data.hwVer = pEndi->hw;
    data.swVer = pEndi->fw;
    data.swBuild = pEndi->build;
    GetTimestamp(&data.currentTime, NULL);
    data.swBuild = HostToBigShort(data.swBuild);
    data.currentTime.year = HostToBigShort(data.currentTime.year);

    pChunk->Add((char *)&data, sizeof(IHD_METERING_DATA));
}

/** MeterConstant 값은 읽고 VoltageScale은 읽지 않는다 */
int CIhdParser::StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer)
{
    int res = CPollBaseParser::StateNodeInfo(pOndemand, szBuffer);
    if(res != STATE_READ_NODEINFO)
    {
        res = STATE_OK;
        pOndemand->nResult = ONDEMAND_ERROR_OK;
    }
    return res;
}




