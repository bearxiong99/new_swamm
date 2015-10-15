
#include "MbusChecksum.h"


BYTE GetMbusChecksum(BYTE *pszBuffer, BYTE len)
{
    BYTE i;
    BYTE checksum = 0x00;

    if(!pszBuffer) return checksum;

    for(i=0;i<len;i++)
    {
        checksum += pszBuffer[i];
    }

    return checksum;
}

BOOL VerifyMbusChecksum(BYTE *pszBuffer, BYTE len, BYTE checksum)
{
    return GetMbusChecksum(pszBuffer, len) == checksum ? TRUE : FALSE;
}

