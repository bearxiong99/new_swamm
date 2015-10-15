
#include "common.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

#include "Iec21DataFrame.h"
#include "Iec21Utils.h"

#include "iecError.h"
#include "iec62056_21Functions.h"

int iec62056_21_Init(void)
{
    return 0;
}

int iec21MakeRequest(BYTE *pszBuffer, BYTE * address, int addrLen, BYTE parity, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return IECERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeIec21Request(pszBuffer, address, addrLen, parity);
    if(nSize <= 0)
        return IECERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return IECERR_NOERROR;
}

int iec21MakeAckOption(BYTE *pszBuffer, BYTE protocol, BYTE mode, BYTE baud, BYTE parity, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return IECERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeIec21AckOption(pszBuffer, protocol, mode, baud, parity);
    if(nSize <= 0)
        return IECERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return IECERR_NOERROR;
}

int iec21MakeAck(BYTE *pszBuffer, BYTE parity, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return IECERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeIec21Ack(pszBuffer, parity);
    if(nSize <= 0)
        return IECERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return IECERR_NOERROR;
}

int iec21MakeNak(BYTE *pszBuffer, BYTE parity, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return IECERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeIec21Nak(pszBuffer, parity);
    if(nSize <= 0)
        return IECERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return IECERR_NOERROR;
}

int iec21MakeData(BYTE *pszBuffer, BYTE *data, int dataLen, BYTE parity, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return IECERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeIec21Data(pszBuffer, data, dataLen, parity);
    if(nSize <= 0)
        return IECERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return IECERR_NOERROR;
}

int iec21MakeCommand(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, BYTE *data, int dataLen, BYTE parity, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return IECERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeIec21Command(pszBuffer, cmd, cmdType, idAddr, idAddrLen, data, dataLen, parity);
    if(nSize <= 0)
        return IECERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return IECERR_NOERROR;
}

int iec21MakeBreak(BYTE *pszBuffer, BYTE parity, int *nFrameLength)
{
    int nSize;

    if(pszBuffer == NULL || nFrameLength == NULL) return IECERR_INVALID_PARAM;

    *nFrameLength = 0;
    nSize = MakeIec21Break(pszBuffer, parity);
    if(nSize <= 0)
        return IECERR_SYSTEM_ERROR;

    *nFrameLength = nSize;
    return IECERR_NOERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL iec21GetData(const char * origData, char * obisCode, char * returnBuffer)
{
    return GetIecData(origData, obisCode, returnBuffer);
}

const char *iec21GetErrorMessage(int nError)
{
    return Iec21GetErrorMessage(nError);
}


