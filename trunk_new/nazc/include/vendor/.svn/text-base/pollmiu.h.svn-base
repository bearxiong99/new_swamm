#ifndef __POLLMIUDEF_H__
#define __POLLMIUDEF_H__

/*
typedef struct	_tagPOLLBASED_LPENTRY
{
		TIMEDATE		date;
		UINT			basePulse;
		BYTE			data[192];
}	__ATTRIBUTE_PACKED__ POLLBASED_LPENTRY;
*/

typedef struct	_tagPOLLBASED_METERING_DATA
{
		GMTTIMESTAMP	time;
		UINT			curPulse;
		BYTE			lpPeriod;
        BYTE            unit;
        WORD            meterConstant;
        WORD            dateCount;
}	__ATTRIBUTE_PACKED__ POLLBASED_METERING_DATA;

typedef struct	_tagPOLLBASED_MULT_CH_INFO
{
        BYTE            channelType;
        BYTE            unit;
        WORD            meterConstant;
		UINT			curPulse;
}	__ATTRIBUTE_PACKED__ POLLBASED_MULT_CH_INFO;

typedef struct	_tagPOLLBASED_MULT_CH_METERING_INFO
{
        BYTE            dataFormat;
		GMTTIMESTAMP	time;
		BYTE			lpPeriod;
        WORD            dateCount;
        POLLBASED_MULT_CH_INFO  chInfo[0];
}	__ATTRIBUTE_PACKED__ POLLBASED_MULT_CH_METERING_INFO;

#endif // __POLLMIUDEF_H__

