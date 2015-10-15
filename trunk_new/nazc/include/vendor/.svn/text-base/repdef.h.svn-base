#ifndef __REPEATERTYPEDEF_H__
#define __REPEATERTYPEDEF_H__

typedef struct  _tagREPEATER_METERING_DATA
{
        BYTE            infoLength;
        GMTTIMESTAMP    time;
        BYTE            fwVersion;
        BYTE            fwBuild;
        BYTE            hwVersion;
        BYTE            swVersion;
        BYTE            powerType;
        WORD            operDay;
        WORD            activeMin;
        BYTE            resetReason;
        BYTE            permitMode;
        BYTE            permitState;
        BYTE            alarmFlag;
        WORD            alarmMask;
        BYTE            testFlag;
        WORD            battVolt;
        WORD            current;
        BYTE            offset;
        WORD            solarAdVolt;
        WORD            solarChgBattVolt;
        WORD            solarBoardDcVolt;
        BYTE            solarLogCount;
}   __ATTRIBUTE_PACKED__ REPEATER_METERING_DATA;

typedef struct  _tagSOLAR_TIME_DATE
{
        WORD            year;
        BYTE            month;
        BYTE            day;
        BYTE            hourCount;
}   __ATTRIBUTE_PACKED__ SOLAR_TIME_DATE;

typedef struct  _tagSOLAR_LOG_DATA
{
        BYTE            hour;
        WORD            solarAdVolt;
        WORD            solarChgBattVolt;
        WORD            solarBoardDcVolt;
}   __ATTRIBUTE_PACKED__ SOLAR_LOG_DATA;

#endif // __REPEATERTYPEDEF_H__

