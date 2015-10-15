
#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "MbusDataFrame.h"
#include "MbusUtils.h"

#include "mbusFunctions.h"

int mbusInit(void)
{
    return 0;
}

int mbusMakeShortFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return MBUSERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeMbusShortFrame(pszBuffer, ctrl, addr);
    if(nSize <= 0)
        return MBUSERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return MBUSERR_NOERROR;
}

int mbusMakeLongFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE ci, BYTE *userData, BYTE dataLen, BYTE *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return MBUSERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeMbusLongFrame(pszBuffer, ctrl, addr, ci, userData, dataLen);
    if(nSize <= 0)
        return MBUSERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return MBUSERR_NOERROR;
}

int mbusMakeControlFrame(BYTE *pszBuffer, BYTE ctrl, BYTE addr, BYTE ci, BYTE *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return MBUSERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeMbusControlFrame(pszBuffer, ctrl, addr, ci);
    if(nSize <= 0)
        return MBUSERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return MBUSERR_NOERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *mbusGetErrorMessage(int nError)
{
    return GetMbusErrorMessage(nError);
}

const char *mbusGetMedium(BYTE nType)
{
    return GetMbusMedium(nType);
}

WORD mbusGetDIFVIFValue(BYTE dif, BYTE edif, BYTE vif, BYTE *value, UNIONVALUE *uv, BYTE *buffer)
{
    return GetDIFVIFValue(dif, edif, vif, value, uv, buffer);
}
