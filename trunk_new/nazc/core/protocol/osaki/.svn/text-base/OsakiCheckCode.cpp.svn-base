
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "BCC.h"

/** SOAKI Frame의 Check code (XOR)을 검사한다.
 *
 * @param pszBuffer frame buffer
 * @param nLength buffer size (check code 제외)
 *
 */
BOOL OsakiVerifyCheckCode(BYTE *pszBuffer, int nLength) 
{
    BYTE nCalcBcc = GetBCC(pszBuffer, nLength);

    if(nLength < 0 || pszBuffer == NULL) return FALSE;

    if (nCalcBcc != pszBuffer[nLength])
    {
        XDEBUG("OsakiStream: Check Error (CALC=0x%02X, BCC=0x%02X)\xd\xa", nCalcBcc, pszBuffer[nLength]);
        return FALSE;
    }

    return TRUE;
}

