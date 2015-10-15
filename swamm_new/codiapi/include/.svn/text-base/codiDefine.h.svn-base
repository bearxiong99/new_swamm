#ifndef __CODI_DEFINE_H__
#define __CODI_DEFINE_H__

/* Min/Max Size */

#define CODI_MAX_FRAME_SIZE						128
#define CODI_MAX_PAYLOAD_SIZE					100
#define CODI_MAX_DATA_SIZE						100
#define CODI_MAX_ROUTING_PATH					30			

#define ZIGBEE_MAX_PAYLOAD_SIZE					82
/** Payload size =  max - sizeof(eui64) - sizeof(shortid) - sizeof(numberOfHop) - sizoeof(seq) **/
#define ZIGBEE_VALID_PAYLOAD_SIZE               ZIGBEE_MAX_PAYLOAD_SIZE - 8 - 2 - 1 - 1     

#define CODI_COMTYPE_UNKNOWN					0
#define CODI_COMTYPE_RS232						1
#define CODI_COMTYPE_SPI						2
#define CODI_COMTYPE_VIRTUAL					100

#define CODI_OPT_RTSCTS							0x0001

/** Coordinator Options
  * 
  * Network 운영에 관한 다양한 Option 처리 (#2567)
  */
#define CODI_OPTION_AIMIR                       0x000001        /**< AiMiR Profile 지원 */
#define CODI_OPTION_SE1X                        0x000010        /**< SE 1.x Profile 지원 */
#define CODI_OPTION_LINK                        0x000100        /**< Link 단계 사용 (Default) */

#define CODI_MODEMTYPE_XMODEM					0
#define CODI_MODEMTYPE_YMODEM					1
#define CODI_MODEMTYPE_ZMODEM					2

#define CODI_BOOTLOAD_RUN						0
#define CODI_BOOTLOAD_BOOTLOADER				1

/* Frame Start Flag */
	
#define CODI_START_FLAG_FIRST					0x87
#define CODI_START_FLAG_SECOND					0x98

/* Frame Flow */

#define GET_FRAME_MODE(x)	((x & CODI_FRAMEFLOW_READ) > 0) ? CODI_ACCESS_READ : CODI_ACCESS_WRITE
#define GET_FRAME_FLOW(x)	((x & CODI_FRAMEFLOW_RESPONSE) > 0) ? CODI_FLOW_RESPONSE : CODI_FLOW_REQUEST

#define CODI_ACCESS_WRITE						0
#define CODI_ACCESS_READ						1

#define CODI_FLOW_REQUEST						0
#define CODI_FLOW_RESPONSE						1

#define CODI_FRAMEFLOW_MASK						0xC0
#define CODI_FRAMEFLOW_WRITE					0x00
#define CODI_FRAMEFLOW_READ						0x80
#define CODI_FRAMEFLOW_REQUEST					0x00
#define CODI_FRAMEFLOW_RESPONSE					0x40
#define CODI_FRAMEFLOW_ERROR					0x20

#define CODI_FRAMEFLOW_READRESPONSE				0xC0
#define CODI_FRAMEFLOW_WRITERESPONSE			0x40

/* Frame Control Type */

#define CODI_FRAMETYPE_INFORMATION				0x00	/* Information frame */
#define CODI_FRAMETYPE_COMMAND					0x01	/* Command frame */
#define CODI_FRAMETYPE_DATA						0x02	/* Data frame */
#define CODI_FRAMETYPE_MASK						0x1F	/* Control bit mask */

/* Information Frame Type */

#define CODI_INFO_BOOT							0x00	/* Boot */
#define CODI_INFO_STACK_STATUS					0x01	/* Stack status */
#define CODI_INFO_RESERVED0						0x02	/* Reserved */
#define CODI_INFO_JOIN							0x03	/* Device join */
#define CODI_INFO_ENERGY_SCAN					0x04	/* Energy scan */
#define CODI_INFO_ACTIVE_SCAN					0x05	/* Active scan */
#define CODI_INFO_ERROR							0x06	/* Error */
#define CODI_INFO_SCAN_COMPLETE					0x07	/* Scan complete */
#define CODI_INFO_ROUTE_RECORD					0x08	/* Route record */

/* Coordinator Reset Kinds */

#define CODI_RK_UNKNOWN							0x00	/* Cause for the reset is not known */
#define CODI_RK_EXTERNAL						0x01	/* A low level was present on the reset pin */
#define CODI_RK_POWERON							0x02	/* The supply voltage was below the power-on threshold */
#define CODI_RK_WATCHDOG						0x03	/* The watchdog is enabled and th watchdog timer period expired */
#define CODI_RK_BROWNOUT						0x04	/* The brown-out detector is enabled and the supply voltage was */
														/* blow the brown-out threshold */
#define CODI_RK_JTAG							0x05	/* A logic one was present in the JTAG reset register */
#define CODI_RK_ASSERT							0x06	/* A self-check within the code failed unexpectedly */
#define CODI_RK_RSTACK							0x07	/* The return address stack (RSTACK) went out of bounds */
#define CODI_RK_CSTACK							0x08	/* The data stack (CSTACK) went out of bounds */
#define CODI_RK_BOOTLOADER						0x09	/* The bootloader deliverately caused a reset */
#define CODI_RK_PC_ROLLOVER						0x0A	/* The PC wrapped (rolled over) */
#define CODI_RK_SOFTWARE						0x0B	/* The software deliverately caused a reset */
#define CODI_RK_PROTFAULT						0x0C	/* Protection fault or privilege violation */
#define CODI_RK_FLASH_VERIFY_FAIL				0x0D	/* Flash write failed verification */
#define CODI_RK_FLASH_WRITE_INHIBIT				0x0E	/* Flash write was inihibited, address was already written */
#define CODI_RK_BOOTLOADER_IMG_BAD				0x0F	/* Application bootloader reports bad upgrade image in EEPROM */
#define CODI_RK_FACTORY_SETTING					0x14	/* Healing Reset 1 */
#define CODI_RK_REED_SW							0x15	/* Healing Reset 2 */
#define CODI_RK_FIXED_INTERVAL					0x16	/* Healing Reset 3 */
#define CODI_RK_SOFT_REMOTE						0x17	/* Healing Reset 4 */
#define CODI_RK_AMR_FAIL						0x18	/* Healing Reset 5 */
#define CODI_RK_FAIL_CLOSE_CONNETION			0x19	/* Healing Reset 6 */
#define CODI_RK_DELETE_BIND_FAIL				0x1A	/* Healing Reset 7 */
#define CODI_RK_SCAN_FAIL						0x1B	/* Healing Reset 8 */
#define CODI_RK_NO_JOIN							0x1C	/* Healing Reset 9 */

/* Stack Status Code */

#define CODI_STACK_UP							0x90
#define CODI_STACK_DOWN							0x91
#define CODI_STACK_JOIN_FAILED					0x94
#define CODI_STACK_PANID_CHANGE					0x9A
#define CODI_STACK_NO_BEACONS					0xAB

/* Coordinator Error Code */

#define CODI_SUCCESS							0x00

/* Information Error Code */

#define CODI_INFOERR_SERIAL_TIME_OUT			0x01
#define CODI_INFOERR_SERIAL_ERROR				0x02
#define CODI_INFOERR_FRAME_CONTROL_ERROR		0x03
#define CODI_INFOERR_REQUEST_ERROR				0x04
#define CODI_INFOERR_FRAME_TYPE_ERROR			0x05
#define CODI_INFOERR_CRC16_ERROR				0x06

/* Command Error Code */

#define CODI_CMDERR_NOT_SUPPORTED				0x10
#define CODI_CMDERR_INVALID_DATA				0x11
#define CODI_CMDERR_NO_NETWORK_ERROR			0x12
#define CODI_CMDERR_FORM_NETWORK_ERROR			0x13
#define CODI_CMDERR_LEAVE_NETWORK_ERROR			0x14
#define CODI_CMDERR_BIND_ADDRESS_DUPLICATION	0x15
#define CODI_CMDERR_FULL_BINDING_INDEX			0x16
#define CODI_CMDERR_EUI64_FOUND_FAIL_ERROR		0x17
#define CODI_CMDERR_SET_BINDING_ERROR 			0x18
#define CODI_CMDERR_DELETE_BINDING_ERROR   		0x19
#define CODI_CMDERR_BINDING_IS_ACTIVE_ERROR 	0x1A
#define CODI_CMDERR_GET_BINDING_ERROR           0x1B
#define CODI_CMDERR_GET_NEIGHBOR_ERROR          0x1C
#define CODI_CMDERR_KEY_TABLE_DUPLICATION       0x1D
#define CODI_CMDERR_NULL_KEY_INDEX              0x1E
#define CODI_CMDERR_SET_KEY_ERROR               0x1F
#define CODI_CMDERR_KEY_FOUND_FAIL_ERROR        0x20
#define CODI_CMDERR_DEL_KEY_ERROR               0x21

#define CODI_CMDERR_ADD_ERROR               	0x24
#define CODI_CMDERR_DEL_ERROR               	0x25
#define CODI_CMDERR_ADD_DUPLICATE_ERROR         0x26
/* Data Error Code */

#define CODI_DATAERR_SEND_DATA_ERROR			0x30
#define CODI_DATAERR_PAYLOAD_LENGTH_ERROR		0x31
#define CODI_DATAERR_DELIVERY_ERROR				0x32
#define CODI_DATAERR_BINDING_ERROR				0x33

/* WIN32 Error code */

#define CODI_DEVICE_ERROR						0xff

/* Command Type */

#define CODI_CMD_MODULE_PARAM					0x00
#define CODI_CMD_SERIAL_PARAM					0x01
#define CODI_CMD_NETWORK_PARAM					0x02
#define CODI_CMD_SECURITY_PARAM					0x03
#define CODI_CMD_SCAN_PARAM						0x04
#define CODI_CMD_BIND							0x05
#define CODI_CMD_PINGPONG						0x06
#define CODI_CMD_BINDING_TABLE					0x07
#define CODI_CMD_NEIGHBOR_TABLE					0x08
#define CODI_CMD_RESERVED2						0x09
#define CODI_CMD_STACK_MEMORY					0x0A
#define CODI_CMD_PERMIT							0x0B
#define CODI_CMD_FORM_NETWORK					0x0D
#define CODI_CMD_RESET							0x0E
#define CODI_CMD_SCAN_NETWORK					0x0F
#define CODI_CMD_MANY_TO_ONE_NETWORK			0x10
#define CODI_CMD_BOOTLOAD						0x11
#define CODI_CMD_ROUTE_DISCOVERY                0x20
#define CODI_CMD_MULTICAST_HOP                  0x21
#define CODI_CMD_EXTENDED_TIMEOUT               0x22
#define CODI_CMD_DIRECT_COMM                    0x23
#define CODI_CMD_KEY_TABLE                      0x24
#define CODI_CMD_KEY_TABLE_ENTRY                0x25
#define CODI_CMD_ADD_EUI64                		0x30
#define CODI_CMD_DELETE_EUI64                	0x31
/* Coordinator Baud Rate */

#define CODI_BAUD_300							0
#define CODI_BAUD_600							1
#define CODI_BAUD_900							2
#define CODI_BAUD_1200							3
#define CODI_BAUD_2400							4
#define CODI_BAUD_4800							5
#define CODI_BAUD_9600							6
#define CODI_BAUD_14400							7
#define CODI_BAUD_19200							8
#define CODI_BAUD_28800							9
#define CODI_BAUD_38400							10
#define CODI_BAUD_50000							11
#define CODI_BAUD_57600							12
#define CODI_BAUD_76800							13
#define CODI_BAUD_100000						14
#define CODI_BAUD_115200						15

/* Coordinator Parity Bit */

#define CODI_PARITY_NONE						0
#define CODI_PARITY_ODD							1
#define CODI_PARITY_EVEN						2

/* RF Tx Power Mode */

#define CODI_TXPOWERMODE_DEFAULT				0
#define CODI_TXPOWERMODE_BOOST					1
#define CODI_TXPOWERMODE_ALTERNATE				2
#define CODI_TXPOWERMODE_COMBINATION			3		/* Alternate and Boost */

/* Binding Type */

#define CODI_BIND_SET_BIND						1
#define CODI_BIND_DELETE_BIND					2

/* Form Network Type */

#define CODI_FORMNETWORK_FORMNEWORK				1
#define CODI_FORMNETWORK_LEAVENETWORK			2

/* Reset */

#define CODI_RESET_SET							1

/* Network Scan */

#define CODI_SCANNETWORK_ENERGY					1
#define CODI_SCANNETWORK_NETWORK				2
#define CODI_SCANNETWORK_STOP					3

/* Many to one request */

#define CODI_MANYTOONE_REQUEST					0x01

/*-- Issue #1965 : HIGH/LOW RAM Coordinator 지원 --*/
#define CODI_MANYTOONE_LOW_RAM					0x01
#define CODI_MANYTOONE_HIGH_RAM					0x02

/* Bootload */

#define CODI_BOOTLOAD_SET						1

/* Extened Timeout */

#define CODI_EXTENDED_TIMEOUT_NORMAL            0
#define CODI_EXTENDED_TIMEOUT_EXTENDED          1

/* RESPONSE WAITING MASK */
#define WAIT_WRITE_RESPONSE                     0x01
#define WAIT_READ_RESPONSE                      0x02

#include "endiDefine.h"

#endif	/* __CODI_DEFINE_H__ */
