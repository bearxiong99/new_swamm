#ifndef __ENDI_DEFINE_H__
#define __ENDI_DEFINE_H__

/* End Device Data Type */

#define ENDI_DATATYPE_BYPASS				0x02            // Seq
#define ENDI_DATATYPE_NETWORK_LEAVE			0x03            // Seq
#define ENDI_DATATYPE_AMR					0x04            // Seq
#define ENDI_DATATYPE_ROM_READ				0x05            // Seq
#define ENDI_DATATYPE_MCCB					0x06            // Seq
#define ENDI_DATATYPE_TIMESYNC				0x07
#define ENDI_DATATYPE_METERING				0x08
#define ENDI_DATATYPE_EVENT					0x09
#define ENDI_DATATYPE_LINK					0x0A            // Seq
#define ENDI_DATATYPE_LONG_ROM_READ			0x0B            // Seq
#define ENDI_DATATYPE_COMMAND			    0x0C            // Seq
#define ENDI_DATATYPE_STATE			        0x0D
#define ENDI_DATATYPE_DISCOVERY			    0x0E
/** 더이상 사용하지 않는다 
 * #define ENDI_DATATYPE_KMP   				0x0F//kamstrup 
 */
#define ENDI_DATATYPE_SUPER_PARENT			0x10            // Seq
#define ENDI_DATATYPE_MBUS                  0x11
#define ENDI_DATATYPE_DIO                   0x12            // Seq
/** Reserved                                0x13            */
#define ENDI_DATATYPE_NAN                   0x14            // Seq
/** Reserved                                0x15            */
#define ENDI_DATATYPE_MBUS_NEW              0x16            // Seq
#ifdef __SUPPORT_KUKDONG_METER__
#define ENDI_DATATYPE_KEUKDONG 	            0x19            // Seq
#endif

#define ENDI_DATATYPE_PUSH                  0x1B            // Seq (Push Metering)


/* Super parent SP Type */

#define ENDI_SPTYPE_SET_REQUEST				0x00
#define ENDI_SPTYPE_SET_RESPONSE			0x01
#define ENDI_SPTYPE_DEL_REQUEST				0x02
#define ENDI_SPTYPE_DEL_RESPONSE			0x03

/* Link Frame Type */

#define ENDI_LINKCLOSE_REQUEST				0x00
#define ENDI_LINKOPEN_REQUEST				0x01
#define ENDI_LINKOPEN_RESPONSE				0x02

/* Link source type */

#define ENDI_LINKSRC_PDA					0x80

/* Leave Frame Type */

#define ENDI_LEAVE_NETWORK_REQUEST			0x00
#define ENDI_LEAVE_NETWORK_RESPONSE			0x01
#define ENDI_LEAVE_NETWORK_SELF				0x02

/* AMR Type */

#define ENDI_AMR_WRITE_REQUEST				0x00
#define ENDI_AMR_WRITE_RESPONSE				0x01
#define ENDI_AMR_READ_REQUEST				0x02
#define ENDI_AMR_READ_RESPONSE				0x03

#define AMR_MASK_TIME						0x8000
#define AMR_MASK_CURPULSE					0x4000
#define AMR_MASK_PERIOD						0x2000
#define AMR_MASK_OPRDAY						0x1000
#define AMR_MASK_ACTMIN						0x0800
#define AMR_MASK_RESET						0x0400
#define AMR_MASK_METER_READ_SEC				0x0200
#define AMR_MASK_METERING_DAY				0x0100
#define AMR_MASK_METERING_HOUR				0x0080
#define AMR_MASK_REPEATING_DAY				0x0040
#define AMR_MASK_REPEATING_HOUR				0x0020
#define AMR_MASK_REPEATING_SETUPTIME		0x0010
#define AMR_MASK_LAST_PULSE					0x0008		
#define AMR_MASK_CHOICE					    0x0004		
#define AMR_MASK_ALARM					    0x0002		

/* ROM Read Type */

#define ENDI_ROM_WRITE_REQUEST				0x00
#define ENDI_ROM_WRITE_RESPONSE				(0x01 << 6)
#define ENDI_ROM_READ_REQUEST				(0x02 << 6)
#define ENDI_ROM_READ_RESPONSE				(0x03 << 6)

/* End Device Command Type */
#define ENDI_CMD_VERIFY_EEPROM              0x00
#define ENDI_CMD_INSTALL_BOOTLOADER         0x01
#define ENDI_CMD_RESET                      0x02
#define ENDI_CMD_SET_TIME                   0x03
#define ENDI_CMD_SET_CURRENT_PULSE          0x04
#define ENDI_CMD_SET_LAST_PULSE             0x05
#define ENDI_CMD_SET_ALARM_FLAG             0x06
#define ENDI_CMD_SET_LP_PERIOD              0x07
#define ENDI_CMD_SET_LP_CHOICE              0x08
#define ENDI_CMD_SET_OPER_DAY               0x09
#define ENDI_CMD_SET_ACTIVE_MIN             0x0A
#define ENDI_CMD_SET_METERING_DAY           0x0B
#define ENDI_CMD_SET_METERING_HOUR          0x0C
#define ENDI_CMD_SET_ACTIVE_KEEP_TIME       0x0D
#define ENDI_CMD_SET_RF_POWER               0x0E
#define ENDI_CMD_SET_PERMIT_MODE            0x0F
#define ENDI_CMD_SET_PERMIT_STATE           0x10
#define ENDI_CMD_SET_ALARM_MASK             0x11
#define ENDI_CMD_SET_METERING_FAIL_CNT      0x12
#define ENDI_CMD_SET_NETWORK_TYPE           0x13
#define ENDI_CMD_SET_POWER_QUALITY          0x14        /**< Issue #2499 ACD */
#define ENDI_CMD_SET_METER_ROLLBACK         0x15        /**< Issue #89 SX Meter Upgrade */
#define ENDI_CMD_SET_PUSH_STRATEGY          0x16        /**< Issue #495: Push strategy(diable: 0x00, enable: 0x01, others: undefined) */

#define ENDI_CMD_LAST_COMMAND               0x16

/** 2011.04.04 : Added By Wooks for Smoke Detector and Alarm(Siren) Commands
*/
#define ENDI_CMD_SET_SMOKE_ALARM			0x80
#define ENDI_CMD_SET_SMOKE_STATUS_REQUEST	0x81
#define ENDI_CMD_SET_SMOKE_ALARM_LEVEL		0x82
#define ENDI_CMD_SET_HEART_BEAT_DAY			0x83
#define ENDI_CMD_SET_HEART_BEAT_HOUR		0x84
#define ENDI_CMD_SET_SMOKE_RF_POWER_MODE	0x85


/* End Device Frame Type */
#define ENDI_CMD_WRITE_REQUEST              0x00
#define ENDI_CMD_WRITE_RESPONSE             0x01
#define ENDI_CMD_READ_REQUEST               0x02
#define ENDI_CMD_READ_RESPONSE              0x03

/* End Device Discovery Type */
#define ENDI_CMD_DISCOVERY_REQUEST          0x00
#define ENDI_CMD_DISCOVERY_RESPONSE         0x01

/* Event Identifier */

#define ENDI_EVENT_POWER_OUTAGE				0x00				// Power outage
#define ENDI_EVENT_LOW_BATTERY				0x01				// Low battery
#define ENDI_EVENT_POWER_RECOVER			0x02				// Power recover
#define ENDI_EVENT_LX_MISSING				0x03				// LX Missing (결상)
#define ENDI_EVENT_TILT_ALARM				0x04				// Tilt sensor (계량기 각도가 틀어진 경우)
#define ENDI_EVENT_CASE_OPEN				0x05				// Case open
#define ENDI_EVENT_MAGNETIC_TAMPER			0x06				// Magnetic Tamper
#define ENDI_EVENT_MAGNETIC_DETACH          0x07                // Magnetic Detachment
#define ENDI_EVENT_PULSE_CUT                0x08                // Pulse Cut
#define ENDI_EVENT_BATT_DISCHARGING_ENABLE  0x09                // Battery Discharging
#define ENDI_EVENT_BACK_PULSE               0x0A                // Back Pulse Detect
#define ENDI_EVENT_ATTACHMENT               0x0B                // Attachment
#define ENDI_EVENT_STATE_ALARM              0x0C                // Uzbek Satate Alarm
#define ENDI_EVENT_FIRE                     0x0D                // Fire Alarm (0:detect, 1: deactivate)

/* Even Type */
#define ENDI_EVENT_TYPE_RESET               0x00
#define ENDI_EVENT_TYPE_POWER_OUTAGE        0x01
#define ENDI_EVENT_TYPE_NETWORK_JOIN        0x02
#define ENDI_EVENT_TYPE_NETWORK_LEAVE       0x03
#define ENDI_EVENT_TYPE_FACTORY_SETTING     0x04
#define ENDI_EVENT_TYPE_LP_PERIOD_CHANGE    0x05
#define ENDI_EVENT_TYPE_TIME_OFFSET         0x06
#define ENDI_EVENT_TYPE_LX_MISSING          0x07
#define ENDI_EVENT_TYPE_SCAN_FAIL           0x08
#define ENDI_EVENT_TYPE_LOW_BATTERY         0x09
#define ENDI_EVENT_TYPE_CASE_OPEN           0x0A
#define ENDI_EVENT_TYPE_TAMPER              0x0B
#define ENDI_EVENT_TYPE_RESERVED_SRAM_CRACK 0x0C
#define ENDI_EVENT_TYPE_KEEP_ALIVE_FAIL     0x0D
#define ENDI_EVENT_TYPE_DETACHEMENT         0x0E
#define ENDI_EVENT_TYPE_PULSE_MO            0x0F
#define ENDI_EVENT_TYPE_WKAE_UP             0x10
#define ENDI_EVENT_TYPE_NO_PARENT           0x11
#define ENDI_EVENT_TYPE_BATT_DISC_EN        0x12
#define ENDI_EVENT_TYPE_NETWORK_TYPE        0x13
#define ENDI_EVENT_TYPE_OPTICAL_CMD         0x14
#define ENDI_EVENT_TYPE_PULSE_BACK          0x15
#define ENDI_EVENT_TYPE_ATTACHMENT          0x16
#define ENDI_EVENT_TYPE_STATE_ALARM         0x17                // Gas Sensor Rom Value
#define ENDI_EVENT_TYPE_MSP430_STATUS       0x18                // 
#define ENDI_EVENT_TYPE_MSP430_RESET        0x19                // 0: No Reset, 1:Reset
#define ENDI_EVENT_TYPE_NET_JOIN_LIMITED    0x1A                //
#define ENDI_EVENT_TYPE_REPEATER_STATUS     0x1B                // 0:Normal, 1:Serial Error
#define ENDI_EVENT_TYPE_NET_KEY_SWITCHING   0x1C                // 0:No Switching, 1:Switching
#define ENDI_EVENT_TYPE_INVALID_METERING    0x1D                //
#define ENDI_EVENT_TYPE_BOOTUP              0x1E                // Bootup 정보 (Ember, Nuri, Count)
#define ENDI_EVENT_TYPE_EEPROM_LP_FAIL      0x1F                // EEPROM에 LP 저장 실패
#define ENDI_EVENT_TYPE_CPULSE_ERROR        0x20                // 누적 펄스 오류 발생 시 최종 에러 펄스 값
#define ENDI_EVENT_TYPE_LP_ROLLBACK         0x21                // SX 미터에서 이전 롤백 누적 값
#define ENDI_EVENT_TYPE_METER_FW_UPGRADE_FAIL   0x22            // Meter FW Upgrade 실패 시 Address

/* ROM Read/Write */

#define ROM_READONLY						0
#define ROM_REWRITE							1

/* Network Information (Rewrite) */

#define ENDI_ROMAP_MANUAL_ENABLE			0x0002
#define ENDI_ROMAP_CHANNEL  	           	0x0003
#define ENDI_ROMAP_PANID			    	0x0004
#define ENDI_ROMAP_TXPOWER		    		0x0006
#define ENDI_ROMAP_SECURITY_ENABLE			0x0007
#define ENDI_ROMAP_LINK_KEY					0x0008
#define ENDI_ROMAP_NETWORK_KEY				0x0018
#define ENDI_ROMAP_EXT_PANID				0x0028

/* Node Information (Read Only) */

#define ENDI_ROMAP_NODE_KIND                0x0100
#define ENDI_ROMAP_FIRMWARE_VERSION			0x011E
#define ENDI_ROMAP_FIRMWARE_BUILD			0x011F
#define ENDI_ROMAP_SOFTWARE_VERSION         0x0120
#define ENDI_ROMAP_HARDWARE_VERSION         0x0121
#define ENDI_ROMAP_PROTOCOL_VERSION         0x0122
#define ENDI_ROMAP_INTERFACE_VERSION     	0x0123
#define ENDI_ROMAP_RESET_COUNT              0x0124
#define ENDI_ROMAP_RESET_REASON             0x0126
#define ENDI_ROMAP_SP_NETWORK               0x0127

#define ENDI_ROMAP_SUB_FW_VERSION           0x0130
#define ENDI_ROMAP_SUB_FW_BUILD             0x0131
#define ENDI_ROMAP_CDMA_FW_VERSION          0x0132
#define ENDI_ROMAP_CDMA_FW_BUILD            0x0133
#define ENDI_ROMAP_CDMA_CSQ                 0x0134
#define ENDI_ROMAP_LAST_PACKET_TIME         0x0135
#define ENDI_ROMAP_VOLTAGE_SCALE            0x0136      /**< 4 Byte Float (Big Endian) */
#define ENDI_ROMAP_CURRENT_SCALE            0x013A      /**< 4 Byte Float (Big Endian) */

/* AMR Data(Rewrite) */

#define ENDI_ROMAP_METER_SERIAL_NUMBER      0x0180
#define ENDI_ROMAP_CONSUMPTION_LOCATION		0x0194
#define ENDI_ROMAP_VENDOR			        0x01B2
#define ENDI_ROMAP_CUSTOMER_NAME		    0x01C6
#define ENDI_ROMAP_FIXED_RESET             	0x01E4
#define ENDI_ROMAP_TEST_FLAG				0x01E5
#define ENDI_ROMAP_METERING_DAY				0x01E6
#define ENDI_ROMAP_METERING_HOUR			0x01EA
#define ENDI_ROMAP_REPEATING_DAY			0x01F6
#define ENDI_ROMAP_REPEATING_HOUR			0x01FA
#define ENDI_ROMAP_REPEATING_SETUP_SEC		0x0206
#define ENDI_ROMAP_NAZC_NUMBER				0x0208
#define ENDI_ROMAP_LP_CHOICE				0x020C
#define ENDI_ROMAP_ALARM_FLAG				0x020D
#define ENDI_ROMAP_NO_JOIN_CNT				0x020E
#define ENDI_ROMAP_PERMIT_MODE				0x020F
#define ENDI_ROMAP_PERMIT_STATE				0x0210
#define ENDI_ROMAP_METERING_FAIL_CNT        0x0211  // Issue #2072
#define ENDI_ROMAP_ALARM_MASK				0x0212
#define ENDI_ROMAP_NETWORK_TYPE				0x0214  // Issue #2071 (0:Star, 1:Mesh, others:Not Support)

/** Address 0x0215 ~ 0x024A 
  *
  * 정의되어 있지만 필요성이 없어서 사용하지 않는다
  */
#define ENDI_ROMAP_METER_CONSTANT           0x024B  // RF-8 (미터 상수 / 2Byte WORD)

#define ENDI_ROMAP_GE_METER_ANSI_KEY        0x024D  // MIU에서 사용되는 ANSI Meter Security Key
#define ENDI_ROMAP_GE_METER_TYPE            0x0257  // MIU에서 사용되는 GE Meter Type (0x00: I210+, 0x01: I210+c)
#define ENDI_ROMAP_DAY_OF_SCANNING          0x0259  // Scanning 기간 (제한일)
#define ENDI_ROMAP_PUSH_STRATEGY            0x025A  // Issue #495: Push Metering (0x00: disable, 0x01: enable, others: not support)


/* Battery Log (Read Only) */

#define ENDI_ROMAP_BATTERY_POINTER			0x0500
#define ENDI_ROMAP_BATTERY_LOGDATA         	0x0501

/* Event Log (Read Only) */

#define ENDI_ROMAP_EVENT_POINTER            0x1000
#define ENDI_ROMAP_EVENT_LOGDATA           	0x1001

/* LP Log (Read Only) */

#define ENDI_ROMAP_METER_LPPERIOD			0x2000
#define ENDI_ROMAP_METER_LPPOINTER         	0x2001
#define ENDI_ROMAP_METER_LPLOGDATA			0x2002

/* SOLAR Log (Read Only) */

#define ENDI_ROMAP_SOLAR_POINTER			0x2000
#define ENDI_ROMAP_SOLAR_LOGDATA         	0x2001

/* M-BUS LP Log(Read Only) */
#define MBUS1_ENABLE       0x2000      //2000, 6200, A400 -> 4200씩 +
#define MBUS1_ADDRESS      0x2001
#define MBUS1_TYPE         0x2002
#define MBUS1_LPPONTER     0x2003
#define MBUS1_LPLOGDATA    0x2004

#define MBUS2_ENABLE       0x6200
#define MBUS2_ADDRESS      0x6201
#define MBUS2_TYPE         0x6202
#define MBUS2_LPPONTER     0x6203
#define MBUS2_LPLOGDATA    0x6204

#define MBUS3_ENABLE       0xA400
#define MBUS3_ADDRESS      0xA401
#define MBUS3_TYPE         0xA402
#define MBUS3_LPPONTER     0xA403
#define MBUS3_LPLOGDATA    0xA404

/* Issue #977 : Kamstrup 신규 검침 Format */
#define ENDI_ROMAP_KAM_LPPERIOD             0x2000      // 1Byte, 1:1h, 2:30m, 4:15m, 12:5m
#define ENDI_ROMAP_KAM_LPCHANNEL            0x2001      // 1Byte, LP Channel Mask
#define ENDI_ROMAP_KAM_LPTIME_FORMAT        0x2002      // 3Bytes
#define ENDI_ROMAP_KAM_LPCH1_FORMAT         0x2005      // 3Bytes
#define ENDI_ROMAP_KAM_LPCH2_FORMAT         0x2008      // 3Bytes
#define ENDI_ROMAP_KAM_LPCH3_FORMAT         0x200B      // 3Bytes
#define ENDI_ROMAP_KAM_LPCH4_FORMAT         0x200E      // 3Bytes
#define ENDI_ROMAP_KAM_LAST_INDEX           0x2011      // 2Bytes, Last LP Index
#define ENDI_ROMAP_KAM_OVERLAP              0x2013      // 1Byte, LP Log Overlap flag(T/F)
#define ENDI_ROMAP_KAM_MAX_COUNT            0x2014      // 2Bytes, Max LP Count
#define ENDI_ROMAP_KAM_LPPOINTER            0x2016      // 4Bytes, LP Log Address
#define ENDI_ROMAP_KAM_LPLOGDATA            0x201A      // 

/* Issue #853 : Kamstrup Omnipower 신규 검침 Format */
#define ENDI_ROMAP_OMNI_LAST_INDEX          0x2011      // 4Bytes, Last LP Index
#define ENDI_ROMAP_OMNI_OVERLAP             0x2015      // 1Byte, LP Log Overlap flag(T/F)
#define ENDI_ROMAP_OMNI_MAX_COUNT           0x2016      // 2Bytes, Max LP Count
#define ENDI_ROMAP_OMNI_LPPOINTER           0x2018      // 4Bytes, LP Log Address
#define ENDI_ROMAP_OMNI_LPLOGDATA           0x201C      // 


/* D I/O Type */
#define DIGITAL_STATUS_REQUEST      0x00
#define DIGITAL_OUTPUT_REQUEST      0x01

/* OTA MBUS EEPROM Address Description  */
#define   EXT_EEPROM0_DEV_ADDRESS   0xA0
#define   EXT_EEPROM1_DEV_ADDRESS   0xA2
#define   EXT_EEPROM2_DEV_ADDRESS   0xA4
#define   EXT_EEPROM3_DEV_ADDRESS   0xA6
#define   EXT_EEPROM4_DEV_ADDRESS   0xA8

#define   EXT_EEPROM0_ADDRESS_START 0x00000000UL
#define   EXT_EEPROM0_ADDRESS_END   0x0000FFFFUL
#define   EXT_EEPROM1_ADDRESS_START 0x00010000UL
#define   EXT_EEPROM1_ADDRESS_END   0x0001FFFFUL
#define   EXT_EEPROM2_ADDRESS_START 0x00020000UL
#define   EXT_EEPROM2_ADDRESS_END   0x0002FFFFUL
#define   EXT_EEPROM3_ADDRESS_START 0x00030000UL
#define   EXT_EEPROM3_ADDRESS_END   0x0003FFFFUL
#define   EXT_EEPROM4_ADDRESS_START 0x00040000UL
#define   EXT_EEPROM4_ADDRESS_END   0x0004FFFFUL

/*OTA-ROM Map Description       */
#define   FW_IMAGE_STATUS           0x00010000
#define   FW_UPDATE_STATUS          0x00010002
#define   FW_IMAGE_BYTES            0x00010004
#define   FW_IMAGE_CRC              0x00010008
#define   FW_IMAGE_DATA             0x00010080


#define   OTA_STR711                0x0987
#define   OTA_EM250                 0x0623
#define   RUN_STR711                0xAAAA

/* Push Metering Definition */
#define PUSH_METERING_TYPE_DATA     0x00
#define PUSH_METERING_TYPE_EVENT    0x01



/* Calculation */

#define BATTERY_LOG_ADDRESS(BATTERY_LOGDATA, BATTERY_POINTER) \
		(BATTERY_LOGDATA + ((BATTERY_POINTER+50)%50)*sizeof(ENDI_BATTERY_ENTRY))

#define EVENT_LOG_ADDRESS(EVENT_LOGDATA, EVENT_POINTER)	\
		(EVENT_LOGDATA + ((EVENT_POINTER+250)%250)*sizeof(ENDI_EVENT_ENTRY))

#define METER_LP_ADDRESS(METER_LPLOGDATA, LPPOINTER, METER_LPPERIOD) \
		(METER_LPLOGDATA+((LPPOINTER+40)%40)*((48*METER_LPPERIOD)+sizeof(TIMEDATE)+sizeof(UINT)))

#define SOLAR_LOG_ADDRESS(SOLAR_LOGDATA, SOLARPOINTER) \
		(SOLAR_LOGDATA+((SOLARPOINTER+14)%14)*sizeof(ENDI_SOLARLOG_ENTRY))

#define MBUS_TYPE0_LP_ADDRESS(MBUS_LPLOGDATA, MBUS_LPPOINTER, OFFSET) \
        (MBUS_LPLOGDATA +((MBUS_LPPOINTER-OFFSET+10)%10)*1676)

#define MBUS_TYPE1_LP_ADDRESS(MBUS_LPLOGDATA, MBUS_LPPOINTER, OFFSET) \
        (MBUS_LPLOGDATA +((MBUS_LPPOINTER-OFFSET+10)%10)*879)

#define SX_METER_LP_ADDRESS(METER_LPLOGDATA, LPPOINTER, METER_LPPERIOD) \
		(METER_LPLOGDATA+((LPPOINTER+45)%45)*((288*METER_LPPERIOD)+sizeof(TIMEDATE)+(sizeof(UINT)*6)))

#define ENDI_MCCB_REQUEST					0
#define ENDI_MCCB_RESPONSE					1

#endif	/* __ENDI_DEFINE_H__ */
