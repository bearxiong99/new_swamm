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

#include "ParserI210Pulse.h"

static const char *parserNameArray[] = {"I210+", "I210+n", NULL};

//////////////////////////////////////////////////////////////////////
// CI210Parser Class
//////////////////////////////////////////////////////////////////////

CI210PulseParser::CI210PulseParser() : CPulseBaseParser(SENSOR_TYPE_ZEUPLS, SERVICE_TYPE_ELEC, 
        ENDDEVICE_ATTR_POWERNODE | ENDDEVICE_ATTR_POLL,
        const_cast<char **>(parserNameArray), PARSER_TYPE_I210, "I210+Parser", "I210+", parserNameArray[0],
        6000, 6000, 2)
{
}

CI210PulseParser::~CI210PulseParser()
{
}

