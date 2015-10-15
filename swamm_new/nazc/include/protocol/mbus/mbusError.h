#ifndef __MBUS_ERROR_H__
#define __MBUS_ERROR_H__

// Action result code
#define MBUSERR_NOERROR                     0       // No error

#define MBUSERR_INVALID_PARAM               -50
#define MBUSERR_INVALID_SESSION             -51
#define MBUSERR_INVALID_HANDLE              -52
#define MBUSERR_INVALID_ADDRESS             -53
#define MBUSERR_INVALID_PORT                -54
#define MBUSERR_INVALID_SOURCE_ADDR         -55
#define MBUSERR_INVALID_DEST_ADDR           -56
#define MBUSERR_INVALID_TIMEOUT             -57
#define MBUSERR_INVALID_ACCESS              -58
#define MBUSERR_INVALID_DATA                -59
#define MBUSERR_INVALID_ACTION              -60
#define MBUSERR_INVALID_OBISCODE            -61
#define MBUSERR_INVALID_STRING              -62
#define MBUSERR_INVALID_LENGTH              -63
#define MBUSERR_INVALID_BUFFER              -64

#define MBUSERR_CANNOT_CONNECT              -100
#define MBUSERR_REPLY_TIMEOUT               -101
#define MBUSERR_NOT_INITIALIZED             -102

#define MBUSERR_SYSTEM_ERROR                -200
#define MBUSERR_MEMORY_ERROR                -201

#endif    // __MBUS_ERROR_H__
