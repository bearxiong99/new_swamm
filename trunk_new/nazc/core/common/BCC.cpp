
#include "typedef.h"


//////////////////////////////////////////////////////////////////////
// BCC
//////////////////////////////////////////////////////////////////////

BYTE GetBCC(unsigned char *pszStream, int nLength)
{
    BYTE base = 0x00;
	int		i;

    if(pszStream == NULL) return base;

	for(i=0; i<nLength; i++)
    {
        base ^= pszStream[i];
    }

	return base;
}

