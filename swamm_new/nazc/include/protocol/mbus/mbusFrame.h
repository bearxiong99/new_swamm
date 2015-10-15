#ifndef __MBUS_FRAME_H__
#define __MBUS_FRAME_H__

#include "typedef.h"

#ifdef _WIN32
#pragma pack(push, 1)
#endif

////////////////////////////////////////////////////////////////////////////////
// MBUS Frames
////////////////////////////////////////////////////////////////////////////////

typedef struct  _tagMBUS_HEADER
{
        BYTE                    start;                              // Start Flag 0x10
}   __ATTRIBUTE_PACKED__ MBUS_HEADER;

typedef struct  _tagMBUS_TAIL
{
        BYTE                    checksum;                           // Check Sum
        BYTE                    stop;                               // Stop Flag 0x16
}   __ATTRIBUTE_PACKED__ MBUS_TAIL;

typedef struct  _tagMBUS_FRAME
{
        MBUS_HEADER             hdr;
        BYTE                    payload[252+6];
        MBUS_TAIL               tail;
}   __ATTRIBUTE_PACKED__ MBUS_FRAME;


////////////////////////////////////////////////////////////////////////////////
// Short Frame 
////////////////////////////////////////////////////////////////////////////////

typedef struct  _tagMBUS_SHORT_HEADER
{
        BYTE                    start;                              // Start Flag 0x10
        BYTE                    ctrl;                               // Control
        BYTE                    addr;                               // Address
}   __ATTRIBUTE_PACKED__ MBUS_SHORT_HEADER;

typedef struct  _tagMBUS_SHORT_FRAME
{
        MBUS_SHORT_HEADER       hdr;
        MBUS_TAIL               tail;
}   __ATTRIBUTE_PACKED__ MBUS_SHORT_FRAME;


////////////////////////////////////////////////////////////////////////////////
// Control Frame 
////////////////////////////////////////////////////////////////////////////////

typedef struct  _tagMBUS_CONTROL_HEADER
{
        BYTE                    start;                              // Start Flag 0x68
        BYTE                    len;                                // Length 0x03
        BYTE                    len2;                               // Length 0x03
        BYTE                    start2;                             // Start Flag 0x68
        BYTE                    ctrl;                               // Control
        BYTE                    addr;                               // Address
        BYTE                    ci;                                 // Control Information
}   __ATTRIBUTE_PACKED__ MBUS_CONTROL_HEADER;

typedef struct  _tagMBUS_CONTROL_FRAME
{
        MBUS_CONTROL_HEADER     hdr;
        MBUS_TAIL               tail;
}   __ATTRIBUTE_PACKED__ MBUS_CONTROL_FRAME;


////////////////////////////////////////////////////////////////////////////////
// Long Frame 
////////////////////////////////////////////////////////////////////////////////

typedef struct  _tagMBUS_LONG_HEADER
{
        BYTE                    start;                              // Start Flag 0x68
        BYTE                    len;                                // Length
        BYTE                    len2;                               // Length
        BYTE                    start2;                             // Start Flag 0x68
        BYTE                    ctrl;                               // Control
        BYTE                    addr;                               // Address
        BYTE                    ci;                                 // Control Information
}   __ATTRIBUTE_PACKED__ MBUS_LONG_HEADER;

typedef struct  _tagMBUS_FIXED_DATA_HEADER
{
        BYTE                    customer[4];
        BYTE                    manufacturer[2];
        BYTE                    generation;
        BYTE                    medium;
        BYTE                    access;
        BYTE                    status;
        BYTE                    signature[2];
}   __ATTRIBUTE_PACKED__ MBUS_FIXED_DATA_HEADER;

typedef struct  _tagMBUS_LONG_FRAME
{
        MBUS_LONG_HEADER        hdr;
        BYTE                    data[252];
        MBUS_TAIL               tail;
}   __ATTRIBUTE_PACKED__ MBUS_LONG_FRAME;


#ifdef _WIN32
#pragma pack(pop)
#endif

#endif	// __MBUS_FRAME_H__
