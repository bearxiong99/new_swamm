#ifndef __OSAKI_FRAME_H__
#define __OSAKI_FRAME_H__

#include "iec62056_21Define.h"
#include "iec62056_21Frame.h"

#ifdef _WIN32
#pragma pack(push, 1)
#endif

////////////////////////////////////////////////////////////////////////////////
// OSAKI Frames
////////////////////////////////////////////////////////////////////////////////

typedef struct  _tagOSAKI_ADDR_REQUEST
{
        IEC21_REQUEST_HEADER    hdr;
        BYTE                    addr[12];                           // OSAKI Address (12bytes)
        IEC21_REQUEST_TAIL      tail;                               
        BYTE                    bcc;
}   __ATTRIBUTE_PACKED__ OSAKI_ADDR_REQUEST;

typedef struct  _tagOSAKI_REQUEST
{
        IEC21_REQUEST_HEADER    hdr;
        BYTE                    addr[12];                           // OSAKI Address (12bytes)
        BYTE                    code;
        IEC21_REQUEST_TAIL      tail;                               
        BYTE                    bcc;
}   __ATTRIBUTE_PACKED__ OSAKI_REQUEST;

typedef struct  _tagOSAKI_RESPONSE
{
        IEC21_DATA_HEADER       hdr;
        BYTE                    payload[256];
        IEC21_DATA_NP_TAIL      tail;
}   __ATTRIBUTE_PACKED__ OSAKI_RESPONSE;

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif	// __OSAKI_FRAME_H__
