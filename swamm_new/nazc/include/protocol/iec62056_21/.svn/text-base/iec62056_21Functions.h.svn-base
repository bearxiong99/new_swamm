////////////////////////////////////////////////////////////////////////////////
//
// IEC62056_21 Protocol Library
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __IEC62056_21_FUNCTIONS_H__
#define __IEC62056_21_FUNCTIONS_H__

#define PROTOCOL_IEC62056_21_INIT      iec65056_21_Init()

#ifdef __cplusplus
extern "C" {
#endif

// Initialize
int iec65056_21_Init(void);

// IEC62056_21 functions
int iec21MakeRequest(BYTE *pszBuffer, BYTE* address, int addrLen, BYTE requestCode, int *nFrameLength);
int iec21MakeAckOption(BYTE *pszBuffer, BYTE protocol, BYTE mode, BYTE baud, BYTE parity, int *nFrameLength);
int iec21MakeAck(BYTE *pszBuffer, BYTE parity, int *nFrameLength);
int iec21MakeNak(BYTE *pszBuffer, BYTE parity, int *nFrameLength);
int iec21MakeData(BYTE *pszBuffer, BYTE *data, int dataLen, BYTE parity, int *nFrameLength);
int iec21MakeCommand(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, 
        BYTE *data, int dataLen, BYTE parity, int *nFrameLength);
int iec21MakeBreak(BYTE *pszBuffer, BYTE parity, int *nFrameLength);

// Utility functions
BOOL iec21GetData(const char * origData, char * obisCode, char * returnBuffer);
const char *iec21GetErrorMessage(int nError);

#ifdef __cplusplus
}
#endif

#endif	// __IEC62056_21_FUNTIONS_H__
