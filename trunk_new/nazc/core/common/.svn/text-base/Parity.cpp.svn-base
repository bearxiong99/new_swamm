
#include "typedef.h"


//////////////////////////////////////////////////////////////////////
// Parity
//////////////////////////////////////////////////////////////////////

/**
 * http://stackoverflow.com/questions/1881995/how-to-convert-a-character-to-7-bit-even-parity-in-php
 */
BYTE GetEvenParity(BYTE source)
{
    return source ^ ((3459840 >> ((source ^ (source >> 4)) & 15)) & 128);
}

/** pszStream의 data를 event parity로 설정.
 *
 * 이 때 Data는 7bit (IEC 646)을 준수해야 한다.
 */
BOOL SetEvenParity(BYTE *pszStream, int nLength)
{
	int		i;
    BYTE    b;

    if(pszStream == NULL || nLength <= 0) return FALSE;
	for(i=0; i<nLength; i++)
    {
        b = pszStream[i];
        pszStream[i] =  b ^ ((3459840 >> ((b ^ (b >> 4)) & 15)) & 128);
    }

	return TRUE;
}

