#ifndef __COMMON_CRC16_H__
#define __COMMON_CRC16_H__

 
UINT GetCrc16(unsigned char *pszStream, int nLength);
WORD GetCrc16WithCrc(unsigned char *pszStream, int nLength, WORD nPrevCrc);

#endif // __COMMON_CRC16_H__
