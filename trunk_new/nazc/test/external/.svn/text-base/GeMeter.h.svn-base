
#ifndef __GE_METER__
#define __GE_METER__

typedef struct  _tagGETABLE {
        char    table[4];
        WORD    len;
        BYTE    data[0];
}   __attribute__ ((packed)) GETABLE;

typedef struct  _tagS001
{
        BYTE    MANUFACTURER[4];
        BYTE    ED_MODEL[8];
        BYTE    HW_VERSION_NUMBER;
        BYTE    HW_REVISION_NUMBER;
        BYTE    FW_VERSION_NUMBER;
        BYTE    FW_REVISION_NUMBER;
        BYTE    MFG_SERIAL_NUMBER[16];
}   __attribute__ ((packed)) S001;

typedef struct  _tagS005
{
        BYTE    METER_ID[20];
}   __attribute__ ((packed)) S005;


typedef struct _tagS130 {
        BYTE    relayState;
        BYTE    activeState;
}   __attribute__ ((packed)) S130;

#endif
