#ifndef __KAMSTRUPDEF_H__
#define __POLLMIUDEF_H__

typedef struct	_tagKAM_METERING_INFO
{
        BYTE            period;
        BYTE            channelMask;
        WORD24          timeFormat;
}	__ATTRIBUTE_PACKED__ KAM_METERING_INFO;

typedef struct	_tagKAM_METERING_IDX_COUNT
{
        WORD            lastLpIndex;
        WORD            lpInsCount;     // LP Instance Count
}	__ATTRIBUTE_PACKED__ KAM_METERING_IDX_COUNT;

typedef struct	_tagKAM_METERING_OMNI_IDX_COUNT
{
        UINT            lastLpIndex;
        WORD            lpInsCount;     // LP Instance Count
}	__ATTRIBUTE_PACKED__ KAM_METERING_OMNI_IDX_COUNT;

#endif // __KAMSTRUPDEF_H__

