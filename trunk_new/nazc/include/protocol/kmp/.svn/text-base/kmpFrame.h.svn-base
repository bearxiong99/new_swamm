#ifndef __KMP_FRAME_H__
#define __KMP_FRAME_H__

#include "typedef.h"

#ifdef _WIN32
#pragma pack(push, 1)
#endif

////////////////////////////////////////////////////////////////////////////////
// KMP Frames
////////////////////////////////////////////////////////////////////////////////

typedef struct  _tagKMP_HEADER
{
        BYTE                    start;                              // Start Flag 0x80/0x40/0x06
        BYTE                    addr;                               
        BYTE                    cid;
}   __ATTRIBUTE_PACKED__ KMP_HEADER;

typedef struct  _tagKMP_TAIL
{
        WORD                    crc;                                // CRC
        BYTE                    stop;                               // Stop Flag 0x0D
}   __ATTRIBUTE_PACKED__ KMP_TAIL;

typedef struct  _tagKMP_FRAME
{
        KMP_HEADER              hdr;
        BYTE                    payload[256];
        KMP_TAIL                tail;
}   __ATTRIBUTE_PACKED__ KMP_FRAME;


#ifdef _WIN32
#pragma pack(pop)
#endif

#endif	// __KMP_FRAME_H__
