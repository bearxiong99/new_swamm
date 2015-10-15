#ifndef __IEC62056_21_FRAME_H__
#define __IEC62056_21_FRAME_H__

#include "typedef.h"

#ifdef _WIN32
#pragma pack(push, 1)
#endif

////////////////////////////////////////////////////////////////////////////////
// IEC62056_21 Frames
////////////////////////////////////////////////////////////////////////////////

/** Request Message */
typedef struct  _tagIEC21_REQUEST_HEADER
{
        BYTE                    start;                              // IEC_21_START
        BYTE                    request;                            // IEC_21_REQUEST 
}   __ATTRIBUTE_PACKED__ IEC21_REQUEST_HEADER;

typedef struct  _tagIEC21_REQUEST_TAIL
{
        BYTE                    end;                                // IEC_21_END
        BYTE                    cr;                                 // IEC_21_CR
        BYTE                    lf;                                 // IEC_21_LF
}   __ATTRIBUTE_PACKED__ IEC21_REQUEST_TAIL;

typedef struct  _tagIEC21_REQUEST_FRAME
{
        IEC21_REQUEST_HEADER    hdr;
        BYTE                    addr[32];                           // manufacturer specific (max 32)
        IEC21_REQUEST_TAIL      tail;
}   __ATTRIBUTE_PACKED__ IEC21_REQUEST_FRAME;

/** Identification Message */
typedef struct  _tagIEC21_IDENTIFICATION_HEADER
{
        BYTE                    start;                              // IEC_21_START
        BYTE                    id[3];                              // manufcaturer specific id (3 bytes)
        BYTE                    baud;                               // baud rate identification
}   __ATTRIBUTE_PACKED__ IEC21_IDENTIFICATION_HEADER;


/** Acknowledgement/option select Message  (Mode C/E) */
typedef struct  _tagIEC21_ACK_OPTION_SELECT_FRAME
{
        BYTE                    ack;                                // IEC_21_ACK
        BYTE                    protocol;                           // protocol control character
        BYTE                    baud;                               // baud rate identification
        BYTE                    mode;                               // mode control character
        BYTE                    cr;                                 // IEC_21_CR
        BYTE                    lf;                                 // IEC_21_LF
}   __ATTRIBUTE_PACKED__ IEC21_ACK_OPTION_SELECT_FRAME;

/** Data Message Header (programming/non-programming mode) */
typedef struct  _tagIEC21_DATA_HEADER
{
        BYTE                    stx;                                // IEC_21_STX
}   __ATTRIBUTE_PACKED__ IEC21_DATA_HEADER;

/** Data Message Tail (non-programming mode) */
typedef struct  _tagIEC21_DATA_NP_TAIL
{
        BYTE                    end;                                // IEC_21_END
        BYTE                    cr;                                 // IEC_21_CR
        BYTE                    lf;                                 // IEC_21_LF
        BYTE                    etx;                                // IEC_21_ETX
        BYTE                    bcc;                                // BCC
}   __ATTRIBUTE_PACKED__ IEC21_DATA_NP_TAIL;

/** Data Message Tail (programming mode) */
typedef struct  _tagIEC21_DATA_P_TAIL
{
        BYTE                    etx;                                // IEC_21_ETX
        BYTE                    bcc;                                // BCC
}   __ATTRIBUTE_PACKED__ IEC21_DATA_P_TAIL;

typedef struct  _tagIEC21_DATA_FRAME
{
        IEC21_DATA_HEADER       hdr;
        BYTE                    body[0];
}   __ATTRIBUTE_PACKED__ IEC21_DATA_FRAME;

/** Programming Command Message */
typedef struct  _tagIEC21_COMMAND_HEADER
{
        BYTE                    soh;                                // IEC_21_SOH
        BYTE                    cmd;                                // command
        BYTE                    cmdtype;                            // command type identifier
        BYTE                    stx;                                // IEC_21_STX
}   __ATTRIBUTE_PACKED__ IEC21_COMMAND_HEADER;

typedef IEC21_DATA_P_TAIL   IEC21_COMMAND_TAIL;

typedef struct  _tagIEC21_COMMAND_FRAME
{
        IEC21_COMMAND_HEADER    hdr;
        BYTE                    body[0];
}   __ATTRIBUTE_PACKED__ IEC21_COMMAND_FRAME;

typedef struct  _tagIEC21_PARTIAL_TAIL
{
        BYTE                    eot;                                // IEC_21_EOT
        BYTE                    bcc;                                // BCC
}   __ATTRIBUTE_PACKED__ IEC21_PARTIAL_TAIL;

typedef struct  _tagIEC21_BREAK_FRAME
{
        BYTE                    soh;                                // IEC_21_SOH
        BYTE                    cmd;                                // IEC_21_CMD_BREAK
        BYTE                    cmdtype;                            // '0'
        BYTE                    etx;                                // IEC_21_ETX
        BYTE                    bcc;                                // BCC
}   __ATTRIBUTE_PACKED__ IEC21_BREAK_FRAME;

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif	// __IEC62056_21_FRAME_H__
