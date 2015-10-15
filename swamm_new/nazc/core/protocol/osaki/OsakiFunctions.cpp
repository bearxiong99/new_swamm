
#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "OsakiDataFrame.h"
#include "OsakiUtils.h"

#include "osakiError.h"
#include "osakiFunctions.h"

int osakiInit(void)
{
    return 0;
}

int osakiMakeCommandRequest(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, BYTE *data, int dataLen, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return OSAKIERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeOsakiCommandRequest(pszBuffer, cmd, cmdType, idAddr, idAddrLen, data, dataLen);
    if(nSize <= 0)
        return OSAKIERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return OSAKIERR_NOERROR;
}

int osakiMakeRequest(BYTE *pszBuffer, BYTE * address, int addrLen, BYTE reqCode, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return OSAKIERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeOsakiRequest(pszBuffer, address, addrLen, reqCode);
    if(nSize <= 0)
        return OSAKIERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return OSAKIERR_NOERROR;
}

int osakiMakeAddressConfirm(BYTE *pszBuffer, BYTE * address, int addrLen, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return OSAKIERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeOsakiAddressConfirm(pszBuffer, address, addrLen);
    if(nSize <= 0)
        return OSAKIERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return OSAKIERR_NOERROR;
}

int osakiMakePasswordConfirm(BYTE *pszBuffer, BYTE * passwd, int passwdLen, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return OSAKIERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeOsakiPasswordConfirm(pszBuffer, passwd, passwdLen);
    if(nSize <= 0)
        return OSAKIERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return OSAKIERR_NOERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *osakiGetErrorMessage(int nError)
{
    return GetErrorMessage(nError);
}


