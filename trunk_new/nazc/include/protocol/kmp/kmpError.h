#ifndef __KMP_ERROR_H__
#define __KMP_ERROR_H__

// Action result code
#define KMPERR_NOERROR                     0       // No error

#define KMPERR_INVALID_PARAM               -50
#define KMPERR_INVALID_SESSION             -51
#define KMPERR_INVALID_HANDLE              -52
#define KMPERR_INVALID_ADDRESS             -53
#define KMPERR_INVALID_LENGTH              -63
#define KMPERR_INVALID_BUFFER              -64

#define KMPERR_CANNOT_CONNECT              -100
#define KMPERR_REPLY_TIMEOUT               -101
#define KMPERR_NOT_INITIALIZED             -102

#define KMPERR_SYSTEM_ERROR                -200
#define KMPERR_MEMORY_ERROR                -201

#endif    // __KMP_ERROR_H__
