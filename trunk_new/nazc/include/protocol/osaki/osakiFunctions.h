////////////////////////////////////////////////////////////////////////////////
//
// OSAKI Protocol Library
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __OSAKI_FUNCTIONS_H__
#define __OSAKI_FUNCTIONS_H__

#define PROTOCOL_OSAKI_INIT      osakiInit()

#ifdef __cplusplus
extern "C" {
#endif

// Initialize
int osakiInit(void);

// OSAKI functions
int osakiMakeRequest(BYTE *pszBuffer, BYTE * address, int addrLen, BYTE reqCode, int *nFrameLength);
int osakiMakeAddressConfirm(BYTE *pszBuffer, BYTE * address, int addrLen, int *nFrameLength);
int osakiMakePasswordConfirm(BYTE *pszBuffer, BYTE * passwd, int passwdLen, int *nFrameLength);
int osakiMakeCommandRequest(BYTE *pszBuffer, BYTE cmd, BYTE cmdType, BYTE *idAddr, int idAddrLen, 
        BYTE *data, int dataLen, int *nFrameLength);

// Utility functions
const char *osakiGetErrorMessage(int nError);

#ifdef __cplusplus
}
#endif

#endif	// __OSAKI_FUNTIONS_H__
