#include "common.h"
#include "CrcCheck.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// IF4/IF5 Crc Check Function
//////////////////////////////////////////////////////////////////////

WORD GetCrc(BYTE *pszBuffer, int nLength, int nOffset)
{
	WORD	nCrc;
	BYTE	b;
	BYTE	c15, bit;
	int		i, j;

	nCrc = 0xffff;
	for(i=nOffset; i<(nLength+nOffset); i++)
	{
		b = pszBuffer[i];
		for(j=0; j<8; j++)
		{
			c15 = ((nCrc >> 15 & 1) == 1); 
			bit = ((b >> (7-j) & 1) == 1); 
			nCrc <<= 1; 

			// 0001 0000 0010 0001  (0, 5, 12)
			if (c15 ^ bit) 
				nCrc ^= 0x1021; 
		}
	}
	return nCrc;
}

BOOL VerifyCrc(BYTE *pszBuffer, int nLength, int nOffset)
{
	WORD	crc, *pCrc;
	WORD	nCalcCrc;

	pCrc = (WORD *)(pszBuffer + nLength);
	memcpy(&crc, pCrc, sizeof(WORD));	
	nCalcCrc = GetCrc(pszBuffer, nLength, nOffset);
	if (nCalcCrc != crc)
		XDEBUG("CrcCheck: CRC Error (CALC=0x%04X, CRC=0x%04X)\xd\xa", nCalcCrc, crc);
	return (nCalcCrc == crc);
}

