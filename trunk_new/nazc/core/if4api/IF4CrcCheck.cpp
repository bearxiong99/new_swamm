#include "common.h"
#include "if4frame.h"
#include "IF4CrcCheck.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// IF4/IF5 Crc Check Function
//////////////////////////////////////////////////////////////////////

WORD IF4GetCrc(BYTE *pszBuffer, int nLength, int nOffset)
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
	return HostToLittleShort(nCrc);
}

BOOL IF4VerifyCrc(BYTE *pszBuffer, int nLength, int nOffset)
{
	IF4_TAIL	*pTail = (IF4_TAIL *)(pszBuffer + nLength);
	WORD		nCalcCrc;

	nCalcCrc = IF4GetCrc(pszBuffer, nLength, nOffset);
	if (nCalcCrc != pTail->crc)
		XDEBUG("IF4Stream: CRC Error (CALC=0x%04X, CRC=0x%04X)\xd\xa", nCalcCrc, pTail->crc);
	return (nCalcCrc == pTail->crc);
}

