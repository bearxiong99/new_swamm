//////////////////////////////////////////////////////////////////////
//
//  ParserSxSubCore.cpp: implementation of the CSxSubCoreParser class.
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "Ondemander.h"
#include "EndDeviceList.h"
#include "Chunk.h"
#include "CommonUtil.h"
#include "Utility.h"
#include "DebugUtil.h"

#include "ParserSxSubCore.h"
//////////////////////////////////////////////////////////////////////
// CMstr711Parser Class
//////////////////////////////////////////////////////////////////////

CSxSubCoreParser	*m_pSxSubCoreParser = NULL;

static const char *parserNameArray[] = {"SubSx", NULL};
//////////////////////////////////////////////////////////////////////
// CSxSubCoreParser Class
//////////////////////////////////////////////////////////////////////

/** SubCPU Type으로 구성될 경우 Model 에 Parent 의 Model 정보를 넣어주어야 한다. 
  * Model이 Null 일 경우 Inventory Scanner에서 읽은 NODE_KIND를 사용하게 되는데 SubCPU는
  * 해당 정보를 얻을 수 없고 또한 이렇게 들어간 Model 정보를 Parent Type을 아는데 사용할 수 있다
  */
CSxSubCoreParser::CSxSubCoreParser() : CMeterParser(SENSOR_TYPE_ZRU, SERVICE_TYPE_ELEC, 
                                            ENDDEVICE_ATTR_POWERNODE, 
                                            const_cast<char **>(parserNameArray), PARSER_TYPE_SX, 
                                            "SX Sub core Parser", "SubSX", parserNameArray[0],
                                            6000, 6000, 2)
{
}

CSxSubCoreParser::~CSxSubCoreParser()
{
}

//////////////////////////////////////////////////////////////////////
// CSxSubCoreParser Operations
//////////////////////////////////////////////////////////////////////
// rom을 읽거나 bypass 로 하거나 둘중하나로 처리한다. 
//
int CSxSubCoreParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
    return 0;
}



