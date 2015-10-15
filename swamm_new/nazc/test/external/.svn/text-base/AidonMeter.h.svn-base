
#ifndef __AIDON_METER__
#define __AIDON_METER__

typedef struct	_tagAIDON_LPENTRY
{
		TIMEDATE		date;
		UINT			basePulse;
		BYTE			data[192];
}	__ATTRIBUTE_PACKED__ AIDON_LPENTRY;

typedef struct	_tagAIDON_METERING_DATA
{
		GMTTIMESTAMP	time;
		UINT			curPulse;
		BYTE			lpPeriod;
        BYTE            unit;
        WORD            meterConstant;
        WORD            dateCount;
}	__ATTRIBUTE_PACKED__ AIDON_METERING_DATA;


#endif
