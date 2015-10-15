#ifndef __DLMS_PARSER_DEFINE_H__
#define __DLMS_PARSER_DEFINE_H__

#define OBIS_METER_TYPE				0
#define OBIS_CUSTOMER				1
#define OBIS_MANUFACTURER			2
#define OBIS_LP_INFO				3
#define OBIS_LP						4
#define OBIS_CUR_BILLING			5
#define OBIS_CUR_IN_USE				6
#define OBIS_PREV_BILLING			7
#define OBIS_PREV_IN_USE			8
#define OBIS_POWER_FAIL				9
#define OBIS_POWER_STATE			10
#define OBIS_POWER_RESTORE			11
#define OBIS_LCD_INFO				12
#define OBIS_BILLING_PERIOD			13
#define OBIS_BILLING_PERIOD_COUNTER 14
#define OBIS_ACT_CONSTANT           15
#define OBIS_ACT_SCALER_UNIT        16
#define OBIS_RACT_CONSTANT          17
#define OBIS_RACT_SCALER_UNIT       18
#define OBIS_ENERGY                 19
#define OBIS_ENERGY_ENTRY           20
#define OBIS_MAX_DEMAND             21
#define OBIS_MAX_DEMAND_ENTRY       22
#define OBIS_TIME                   23
#define OBIS_VZ                     24
#define OBIS_EX_ENERGY              25
#define OBIS_EX_EX_ENERGY           26
#define OBIS_EX_EX_EX_ENERGY        27
#define OBIS_EX_MAX_DEMAND          28
#define OBIS_EX_EX_MAX_DEMAND       29
#define OBIS_EX_EX_EX_MAX_DEMAND    30
#define OBIS_COSEM_DEVICE_NAME      31
#define OBIS_OUTPUT_SIGNAL          32
#define OBIS_LOAD_CONTROL           33
#define OBIS_CUR_ACTIVE_FORWARD     34

#define OBIS_METERING_DATA   		100
#define OBIS_DEFAULT   		        255


#define DLMS_SOURCE_ADDR            0x23
#define DLMS_DESTINATION_ADDR       0x0203

#define SAVE                        TRUE
#define SKIP                        FALSE

#define ONDEMAND_OPTION_METERING        0x00
#define ONDEMAND_OPTION_LP              0x01
#define ONDEMAND_OPTION_DR              0x02
#define ONDEMAND_OPTION_CURRENT         0x03
#define ONDEMAND_OPTION_LOAD_CONTROL    0x10

#endif	// __DLMS_PARSER_DEFINE_H__
