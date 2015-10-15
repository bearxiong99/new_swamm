
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "BCC.h"
#include "Parity.h"

/** IEC62056-21 Frame의 Check code (XOR)을 검사한다.
 *
 * @param pszBuffer frame buffer
 * @param nLength buffer size (check code 제외)
 *
 */
BOOL Iec21VerifyCheckCode(BYTE *pszBuffer, int nLength) 
{
    BYTE nCalcBcc = GetBCC(pszBuffer, nLength);

    if(nLength < 0 || pszBuffer == NULL) return FALSE;

    if (nCalcBcc != pszBuffer[nLength])
    {
        XDEBUG("Iec21Stream: Check Error (CALC=0x%02X, BCC=0x%02X)\xd\xa", nCalcBcc, pszBuffer[nLength]);
        return FALSE;
    }

    return TRUE;
}

/** IEC62056-21 Frame 값을 Even Parity Setting 한다.
 *
 * 이 함수를 호출할 경우 실제 pszBuffer의 값이 변경된다.
 *
 * @param pszBuffer frame buffer
 * @param nLength buffer size (check code 제외)
 *
 */ 
BOOL Iec21SetParity(BYTE *pszBuffer, int nLength)
{
    if(nLength < 0 || pszBuffer == NULL) return FALSE;

    return SetEvenParity(pszBuffer, nLength);
}
