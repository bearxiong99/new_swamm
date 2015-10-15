#ifndef __NACEI_H_
#define __NACEI_H_

#if     !defined(__ATTRIBUTE_PACKED__)
#if     defined(_WIN32)
#define __ATTRIBUTE_PACKED__
#else
#define __ATTRIBUTE_PACKED__			__attribute__ ((packed))
#endif
#endif

/*-- data types --*/
typedef struct	_tagNACEI_TIME_STAMP
{
		short			year;						/**< Year */
		char			mon;						/**< Month */
		char			day;						/**< Day */
		char			hour;						/**< Hour */
		char			min;						/**< Min */
		char			sec;						/**< Sec */
} __ATTRIBUTE_PACKED__ NACEI_TIME_STAMP;

typedef struct _tagNACEI_SENSOR_INFO
{
    char                sensorID[16+1];				/**< sensor id. 최대 16 byte */
	char                sensorSerialNumber[20+1];	/**< sensor에서 제공하는 Serial Number. 최대 20 byte */
	NACEI_TIME_STAMP    sensorLastConnect;			/**< 최종 ZigBee Communication Time */
	char		        sensorState;				/**< sensor 최종 상태. Normal(0),Abnormal(n) */
	char		        sensorModel[18+1];			/**< sensor model or vendor. 최대 18 byte */
} __ATTRIBUTE_PACKED__ NACEI_SENSOR_INFO;


/*-- functions --*/
int NACEI_Initialize_Interface(void);
int NACEI_Terminate_Interface(void);
int NACEI_Show_Sensor_List(int *nCount, NACEI_SENSOR_INFO ** pSensorInfo);

#endif
