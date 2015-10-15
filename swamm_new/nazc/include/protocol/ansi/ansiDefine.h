#ifndef __ANSI_DEFINE_H__
#define __ANSI_DEFINE_H__

// ANSI Frame Status
#define P_STAT_NOANSWER         0xF1                            // error no answer
#define P_STAT_ERR_CRC          0xF2                            // error CRC
#define P_STAT_ERR_ID           0xF3                            // error identification
#define P_STAT_ERR_FRT          0xF4                            // error data format
#define P_STAT_ERR_LNT          0xF5                            // error length
#define P_STAT_ERR_CODE         0xF6                            // error response code
#define P_STAT_ERR_SECURITY     0xF7                            // error security 

// ANSI Response Code
#define ANSI_RES_NO_ERROR       0x00                            // acknowledge
#define ANSI_RES_ERROR          0x01                            // error
#define ANSI_RES_SNS            0x02                            // service not supported
#define ANSI_RES_ISC            0x03                            // insufficient security clearance
#define ANSI_RES_ONP            0x04                            // operation not possible
#define ANSI_RES_IAR            0x05                            // inappropriate action requested
#define ANSI_RES_BSY            0x06                            // device busy
#define ANSI_RES_DNR            0x07                            // data not ready
#define ANSI_RES_DLK            0x08                            // data locked
#define ANSI_RES_RNO            0x09                            // renegotiate request
#define ANSI_RES_ISSS           0x0A                            // invalid service sequence state

// ANSI
#define ANSI_STP            	0xEE
#define ANSI_ACK            	0x06
#define ANSI_NAK            	0x15
#define	ANSI_NO_ERROR			0x00
#define ANSI_NO_RESPONSE		P_STAT_NOANSWER

// ANSI meter_cmd
#define ANSI_CMD_IDENT              0x20                        // identify
#define ANSI_CMD_NEGO               0x60                        // negotiate
#define ANSI_CMD_LOGON              0x50                        // log on
#define ANSI_CMD_SECURITY           0x51                        // Security
#define ANSI_CMD_AUTH               0x53                        // authenticate
#define ANSI_CMD_READ               0x30                        // read
#define ANSI_CMD_PREAD_INDEX        0x31                        // partial read index
#define ANSI_CMD_PREAD_OFFSET       0x3F                        // partial read offset
#define ANSI_CMD_WRITE              0x40                        // write
#define ANSI_CMD_PWRITE_INDEX       0x41                        // partial write index
#define ANSI_CMD_PWRITE_OFFSET      0x4F                        // partial write offset
#define ANSI_CMD_WAIT               0x70                        // wait
#define ANSI_CMD_LOGOFF             0x52                        // log off
#define ANSI_CMD_TERMI              0x21                        // terminate
#define ANSI_CMD_DISCON             0x22                        // disconnect
#define ANSI_CMD_RR                 0xFF                        // reading continue

// Packet Define
// Control
#define ANSI_CTRL_MULTI_PACKET  0x80                            // Control field Bit 7
#define ANSI_CTRL_FIRST_PACKET  0x40                            // Control field Bit 6
#define ANSI_CTRL_TOGGLE        0x20                            // Control field Bit 5


// PROTOCOL Define 
// Layer 7 Define
#define ANSI_BAUDRATE_EXTERNAL  0x00           
#define ANSI_BAUDRATE_300       0x01           
#define ANSI_BAUDRATE_600       0x02           
#define ANSI_BAUDRATE_1200      0x03           
#define ANSI_BAUDRATE_2400      0x04           
#define ANSI_BAUDRATE_4800      0x05           
#define ANSI_BAUDRATE_9600      0x06           
#define ANSI_BAUDRATE_14400     0x07           
#define ANSI_BAUDRATE_19200     0x08           
#define ANSI_BAUDRATE_28800     0x09           
#define ANSI_BAUDRATE_57600     0x0A           



#define ANSI_STANDARD_TBL       0x0000
#define ANSI_MANUFACT_TBL       0x0800

// Procedure Define
#define ANSI_PROC_RES_COMPLETION        0
#define ANSI_PROC_RES_EXCEPTION         1
#define ANSI_PROC_RES_NOT_POST          2
#define ANSI_PROC_RES_IMMEDIATELY       3

#define ANSI_PROC_COLD_START                        0
#define ANSI_PROC_CLEAR_DATA                        3
#define ANSI_PROC_RESET_LIST_POINTERS               4
#define ANSI_PROC_UPDATE_LAST_READ_ENTRY            5
#define ANSI_PROC_CHANGE_END_DEVICE_MODE            6
#define ANSI_PROC_REMOTE_RESET                      9
#define ANSI_PROC_SET_DATE_TIME                     10
#define ANSI_PROC_CLEAR_ALL_PENDING_TABLES          14
#define ANSI_PROC_CLEAR_SELECTED_PENDING_TABLES     15
#define ANSI_PROC_START_LP_RECORDING                16
#define ANSI_PROC_STOP_LP_RECORDING                 17
#define ANSI_PROC_DIRECT_LOAD_CONTROL               21
#define ANSI_PROC_CLEAR_INDIVIDUAL_ST_STATUS_FLAGS  0
#define ANSI_PROC_CLEAR_INDIVIDUAL_MF_STATUS_FLAGS  1
#define ANSI_PROC_DIRECT_EXECUTE                    2
#define ANSI_PROC_BATTERY_TEST                      3
#define ANSI_PROC_CONFIGURE_TEST_PULSES             64
#define ANSI_PROC_RESET_CUMULATIVE_OUTAGE_DURATION  65
#define ANSI_PROC_CHANGE_CONFIGURATION_STATUS       66
#define ANSI_PROC_CONVERT_METER_MODE                67
#define ANSI_PROC_UPGRADE_METER_FUNCTION            68
#define ANSI_PROC_PROGRAM_CONTROL                   70
#define ANSI_PROC_RESET_ERROR_HISTORY               71
#define ANSI_PROC_FLASH_CAL_MODE_CONTROL            73
#define ANSI_PROC_SET_LAST_CALIBRATION_DATE_TIME    75
#define ANSI_PROC_REMOVE_SOFTSWITCH                 76
#define ANSI_PROC_RTP_CONTROL                       78
#define ANSI_PROC_SNAPSHOT_REVENUE_DATA             84
#define ANSI_PROC_TIME_ACCELERATION                 85


#if 0
// meter_read_class     
// Standard Table
#define ANSI_TABLE_ST_01        0x0001                          // Manufacturer Identification Table
#define ANSI_TABLE_ST_03        0x0003                          // End Device Mode & Status Table
#define ANSI_TABLE_ST_05        0x0005                          // Device Identification Table
#define ANSI_TABLE_ST_07		0x0007							// Procedure Initiate Table
#define ANSI_TABLE_ST_08		0x0008							// Procedure Response Table
#define ANSI_TABLE_ST_11		0x000B							// Actual Sources Limiting Table
#define ANSI_TABLE_ST_12		0x000C							// Unit of Measure Entry Table
#define ANSI_TABLE_ST_21        0x0015                          // Actual Register Table
#define ANSI_TABLE_ST_22        0x0016                          // Data Selection Table 
#define ANSI_TABLE_ST_23        0x0017                          // Present Data Table
#define ANSI_TABLE_ST_25        0x0019                          // Prev Demand Reset Data Table
#define ANSI_TABLE_ST_26        0x001A                          // SelfRead data Table
#define ANSI_TABLE_ST_51	    0x0033				            // Actual Time and TOU Table
#define ANSI_TABLE_ST_53        0x0035                          // Time Offset Table
#define ANSI_TABLE_ST_54	    0x0036				            // Calendar Table
#define ANSI_TABLE_ST_55        0x0037                          // Clock State Table
#define ANSI_TABLE_ST_61        0x003D                          // Actual Load Profile Table
#define ANSI_TABLE_ST_62        0x003E                          // Load Profile Control Table
#define ANSI_TABLE_ST_63        0x003F                          // Load Profile Status Table
#define ANSI_TABLE_ST_64        0x0040                          // Load Profile Data Set 1 Table
#define ANSI_TABLE_ST_71        0x0047                          // Actual Log Table
#define ANSI_TABLE_ST_72        0x0048                          // Events Identification Table
#define ANSI_TABLE_ST_75        0x004B                          // Event Log Control Table
#define ANSI_TABLE_ST_76        0x004C                          // Event Log Data Table
#define ANSI_TABLE_ST_130		0x0082							// Relay Switch Status Table
#define ANSI_TABLE_ST_131		0x0083							// Relay Operate Table
#define ANSI_TABLE_ST_132		0x0084							// Relay Switch Log Table

// Manufacturer Table
#define ANSI_TABLE_MT_53        0x0835                          //
#define ANSI_TABLE_MT_66        0x0842                          //
#define ANSI_TABLE_MT_67        0x0843                          // Meter Program Constants - 2
#define ANSI_TABLE_MT_68        0x0844                          // Error/Caution Control Table
#define ANSI_TABLE_MT_70        0x0846                          // Display Configuration
#define ANSI_TABLE_MT_75        0x084B                          // Scale Factors
#define ANSI_TABLE_MT_78        0x084E                          // Security Log Table
#define ANSI_TABLE_MT_82	0x0852				// TOU Day Type Map Table
#define ANSI_TABLE_MT_113       0x0871                          // Power Quality Data Table
#define ANSI_TABLE_MT_110		0x086E							// Present Register Data Table
#define ANSI_TABLE_MT_72		0x0848							// Line-Side Diagnostics / Power Quality Data Table
#define ANSI_TABLE_MT_112		0x0870							// 
#endif


#endif	// __ANSI_DEFINE_H__

