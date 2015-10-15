#include <math.h>

#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "mcudef.h"

#include "osakiDefine.h"

#include "OsakiUtils.h"

const char *GetErrorMessage(int nError)
{
    switch(nError) {
        case OSAKIERR_NOERROR :               return "OK";
        case OSAKIERR_INVALID_PARAM :         return "Invalid param";
        case OSAKIERR_INVALID_SESSION :       return "Invalid session";
        case OSAKIERR_INVALID_HANDLE :        return "Invalid handle";
        case OSAKIERR_INVALID_ADDRESS :       return "Invalid address";
        case OSAKIERR_INVALID_LENGTH :        return "Invalid length";
        case OSAKIERR_INVALID_BUFFER :        return "Invalid buffer";
        case OSAKIERR_CANNOT_CONNECT :        return "Cannot connect";
        case OSAKIERR_REPLY_TIMEOUT :         return "Reply timeout";
        case OSAKIERR_NOT_INITIALIZED :       return "Not initialized";
        case OSAKIERR_SYSTEM_ERROR :          return "System error";
        case OSAKIERR_MEMORY_ERROR :          return "Memory error";
    }
    return UNKNOWN_STR;
}

