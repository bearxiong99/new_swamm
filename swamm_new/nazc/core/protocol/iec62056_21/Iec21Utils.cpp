#include <math.h>

#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "mcudef.h"

#include "iecError.h"
#include "iec62056_21Define.h"

#include "Iec21Utils.h"

const char *Iec21GetErrorMessage(int nError)
{
    switch(nError) {
        case IECERR_NOERROR :               return "OK";
        case IECERR_INVALID_PARAM :         return "Invalid param";
        case IECERR_INVALID_SESSION :       return "Invalid session";
        case IECERR_INVALID_HANDLE :        return "Invalid handle";
        case IECERR_INVALID_ADDRESS :       return "Invalid address";
        case IECERR_INVALID_LENGTH :        return "Invalid length";
        case IECERR_INVALID_BUFFER :        return "Invalid buffer";
        case IECERR_CANNOT_CONNECT :        return "Cannot connect";
        case IECERR_REPLY_TIMEOUT :         return "Reply timeout";
        case IECERR_NOT_INITIALIZED :       return "Not initialized";
        case IECERR_SYSTEM_ERROR :          return "System error";
        case IECERR_MEMORY_ERROR :          return "Memory error";
    }
    return UNKNOWN_STR;
}

BOOL GetIecData(const char * origData, char * obisCode, char * returnBuffer)
{
    char * startPtr = (char *)origData;
    char *left, *right;

    if(origData == NULL || returnBuffer == NULL) return FALSE;

    if(obisCode != NULL) {
        startPtr = (char *)strstr(origData, (const char *)obisCode);
        if(startPtr == NULL) {
            return FALSE;
        }
    }

    left = (char *)strchr((const char *)startPtr, '(');
    right = (char *)strchr((const char *)startPtr, ')');

    if(left != NULL && right != NULL) {
        memcpy(returnBuffer, left + 1, right - left - 1);
        return TRUE;
    } 

    return FALSE;
}
