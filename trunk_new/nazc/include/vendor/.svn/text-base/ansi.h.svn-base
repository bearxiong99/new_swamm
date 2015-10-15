#ifndef __ANSI_H__
#define __ANSI_H__

typedef struct _tagEND_TIME
{
	    BYTE    YEAR;
		BYTE    MON;
	    BYTE    DAY;
	    BYTE    HOUR;
	    BYTE    MIN;
}	__ATTRIBUTE_PACKED__ ANSI_END_TIME;

typedef struct _tagEND_READINGS
{
        BYTE    BLOCK_END_READ[6];
}	__ATTRIBUTE_PACKED__ ANSI_END_READINGS;

typedef struct _tagREGISTER_INFO_RECORD
{
        ANSI_END_TIME   END_DATE_TIME;
        BYTE    SEASON;
}	__ATTRIBUTE_PACKED__ REGISTER_INFO_RECORD;

/**********************************************
 * Big field는 LSB first.                     *
 **********************************************/

// Table 0: General Configuration Table
typedef struct	_tagANSI_ST_TABLE0
{
        BOOL    DATA_ORDER:1;           //  FALSE: LSB first, TRUE: MSB first
        BYTE    CHAR_FORMAT:3;
        BYTE    MODEL_SELECT:3;
        BYTE    FILLER1:1;
        BYTE    TM_FORMAT:3;
        BYTE    DATA_ACCESS_METHOD:2;
        BOOL    ID_FORM:1;
        BYTE    INT_FORMAT:2;
        BYTE    NI_FORMAT1:4;
        BYTE    NI_FORMAT2:4;
		BYTE	MANUFACTURER[4];
		BYTE	NAMEPLATE_TYPE;
        BYTE    DEFAULT_SET_USED;
        BYTE    MAX_PROC_PARAM_LENGTH;
        BYTE    MAX_RESP_DATA_LEN;
        BYTE    STD_VERSION_NO;
        BYTE    STD_REVISION_NO;
        BYTE    DIM_STD_TBLS_USED;
        BYTE    DIM_MFG_TBLS_USED;
        BYTE    DIM_STD_PROC_USED;
        BYTE    DIM_MFG_PROC_USED;
        BYTE    DIM_MFG_STATUS_USED;
        BYTE    NBR_PENDING;
        BYTE    STD_TBLS_USED[11];
        BYTE    MFG_TBLS_USED[15];
        BYTE    STD_PROC_USED[3];
        BYTE    MFG_PROC_USED[11];
        BYTE    STD_TBLS_WRITE[11];
        BYTE    MFG_TBLS_WRITE[15];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE0;

// Table 1: General Manufacturer Identification Table
typedef struct	_tagANSI_ST_TABLE1
{
		BYTE	MANUFACTURER[4];
		BYTE	ED_MODEL[8];
		BYTE	HW_VERSION_NUMBER;
		BYTE	HW_REVISION_NUMBER;
		BYTE	FW_VERSION_NUMBER;
		BYTE	FW_REVISION_NUMBER;
		BYTE	MFG_SERIAL_NUMBER[16];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE1;

// Table 3: End Device Mode and Status Table
typedef struct	_tagANSI_ST_TABLE3
{
        BOOL    METERING_FLAG:              1;
        BOOL    TEST_MODE_FLAG:             1;
        BOOL    METER_SHOP_MODE_FLAG:       1;  // Not supported
        BOOL    FILL1:                      5;
        BOOL    UNPROGRAMMED_FLAG:          1;
        BOOL    CONFIGURATION_ERROR_FLAG:   1;  // Not supported
        BOOL    SELF_CHK_ERROR_FLAG:        1;  // Not supported
        BOOL    RAM_FAILURE_FLAG:           1;
        BOOL    ROM_FAILURE_FLAG:           1;
        BOOL    NONVOL_MEM_FAILURE_FLAG:    1;
        BOOL    CLOCK_ERROR_FLAG:           1;
        BOOL    MEASUREMENT_ERROR_FLAG:     1;  // Not supported in I210
        BOOL    LOW_BATTERY_FLAG:           1;
        BOOL    LOW_LOSS_POTENTIAL_FLAG:    1;
        BOOL    DEMAND_OVERLOAD_FLAG:       1;
        BOOL    POWER_FAILURE_FLAG:         1;  // Not supported
        BOOL    BAD_PASSWORD_FLAG:          1;  // Not suported in I210
        BOOL    FILL2:                      3;
        BOOL    ED_STD_STATUS2_BFLD:        8;  // Not used
        BOOL    METERING_ERROR:             1;  // see MT 69
        BOOL    DC_DETECTED:                1;
        BOOL    SYSTEM_ERROR:               1;
        BOOL    RECEIVED_KWH:               1;
        BOOL    LEADING_KVARH:              1;
        BOOL    LOSS_OF_PROGRAM:            1;
        BOOL    FILL3:                      2;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE3;

// Table 5: Standard Device Identification Table
typedef struct	_tagANSI_ST_TABLE5
{
		BYTE	METER_ID[20];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE5;

// Table 7: Procedure Initiate Table
typedef struct	_tagANSI_ST_TABLE7
{
		WORD	TBL_PROC_NBR:               11;
		BOOL	STD_VS_MFG_FLAG:            1;
		BYTE	SELECTOR:                   4;
		BYTE	SEQ_NUM;
		BYTE	PARAM[8];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE7;

// Table 8: Procedure Response Table
typedef struct	_tagANSI_ST_TABLE8
{
		WORD	TBL_PROC_NBR:               11;
		BOOL	STD_VS_MFG_FLAG:            1;
		BYTE	SELECTOR:                   4;
		BYTE	SEQ_NBR;
		BYTE	RESULT_CODE;
		BYTE	RESP_DATA_RCD[8];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE8;

// Table 10: Dimension Sources Limiting Table
typedef struct _tagANSI_ST_TABLE10
{
        BOOL    PF_EXCLUDE_FLAG:            1;
        BOOL    RESET_EXCLUDE_FLAG:         1;
        BOOL    BLOCK_DEMAND_FLAG:          1;
        BOOL    SLIDING_DEMAND_FLAG:        1;
        BOOL    THERMAL_DEMAND_FLAG:        1;
        BOOL    SET1_PRESENT_FLAG:          1;
        BOOL    SET2_PRESENT_FLAG:          1;
        BOOL    FILL:                       1;

		BYTE	NBR_UOM_ENTRIES;
		BYTE	NBR_DEMAND_CTRL_ENTRIES;
		BYTE	DATA_CTRL_LENGTH;
		BYTE	NBR_DATA_CTRL_ENTRIES;
		BYTE	NBR_CONSTANTS_ENTRIES;
		BYTE	CONSTANTS_SELECTOR;
		BYTE	NBR_SOURCES;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE10;

typedef ANSI_ST_TABLE10   ANSI_ST_TABLE11;

typedef struct _tagANSI_ST_TABLE12
{
//        struct  {
            BYTE    ID_CODE;
            BYTE    TIME_BASE:              3;
            BYTE    MULTIPLIER:             3;
            BOOL    Q1_ACCOUNTABILITY:      1;
            BOOL    Q2_ACCOUNTABILITY:      1;
            BOOL    Q3_ACCOUNTABILITY:      1;
            BOOL    Q4_ACCOUNTABILITY:      1;
            BOOL    NET_FLOW_ACCOUNTABILITY:1;
            BYTE    SEGMENTATION:           3;
            BOOL    HARMONIC:               1;
            BYTE    RESERVED_1:             1;
            BYTE    RESERVED_2:             7;
            BOOL    NFS:                    1;
//        } __ATTRIBUTE_PACKED__ UOM_ENTRY_RCD[62];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE12;

typedef struct _tagANSI_ST_TABLE21
{
		BYTE	REG_FUNC1_BFLD;
		BYTE	REG_FUNC2_BFLC;
		BYTE	NBR_SELF_READS;
		BYTE	NBR_SUMMATIONS;
		BYTE	NBR_DEMANDS;
		BYTE	NBR_COIN_VALUES;
		BYTE	NBR_OCCUR;
		BYTE	NBR_TIERS;
		BYTE	NBR_PRESENT_DEMANDS;
		BYTE	NBR_PRESENT_VALUES; 
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE21;

typedef struct _tagDEMAND_BLOCK
{
		BYTE	EVENT_TIME[25];
		BYTE	CUMULATIVE_DEMAND[30];
		BYTE	DEMAND[20];		
}	__ATTRIBUTE_PACKED__ ANSI_DEMAND_BLOCK;

typedef struct _tagDATA_BLOCK
{
		BYTE				SUMMATIONS[30];
		ANSI_DEMAND_BLOCK	DEMANDS;
		BYTE				COINCIDENTS[40];
}	__ATTRIBUTE_PACKED__ ANSI_DATA_BLOCK;

// Table 23: Current Register Data Table - Demand Version (241 bytes)
typedef struct _tagANSI_ST_TABLE23_DEMAND
{
		BYTE			NBR_DEMAND_RESETS;
		ANSI_DATA_BLOCK	TIER_DATA_BLOCK;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE23_DEMAND;

// Table 23: Current Register Data Table - TOU Version (726 bytes)
typedef struct _tagANSI_ST_TABLE23_TOU
{
		BYTE			NBR_DEMAND_RESETS;
		ANSI_DATA_BLOCK	TOT_DATA_BLOCK;
		ANSI_DATA_BLOCK	TIER_DATA_BLOCK0;
		ANSI_DATA_BLOCK TIER_DATA_BLOCK1;
		ANSI_DATA_BLOCK	TIER_DATA_BLOCK2;
		ANSI_DATA_BLOCK TIER_DATA_BLOCK3;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE23_TOU;

// Table 25: Previous Demand Reset Data Table - Demand Version (241 bytes)
typedef struct _tagANSI_ST_TABLE25_DEMAND
{
		REGISTER_INFO_RECORD	INFO;	
		ANSI_ST_TABLE23_DEMAND  DATA;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE25_DEMAND;

// Table 25: Previous Demand Reset Data Table - TOU Version (732 bytes)
typedef struct _tagANSI_ST_TABLE25_TOU
{
		REGISTER_INFO_RECORD	INFO;	
		ANSI_ST_TABLE23_TOU     DATA;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE25_TOU;

typedef struct  _tagLIST_STATUS
{
        BYTE    ORDER_FLAG:                 1;      // 0=ascending, 1=descending
        BOOL    OVERFLOW_FLAG:              1;      // FALSE=no overlow, TRUE=overflow
        BYTE    LIST_TYPE_FLAG:             1;      // 0=FIFO, 1=circular list
        BOOL    INHIBIT_OVERFLOW_FLAG:      1;      // FALSE/TRUE
        BYTE    FILLER:                     4;
}  __ATTRIBUTE_PACKED__ LIST_STATUS;

// Table 26: Self-Read Data Table - TOU Version (I210/SM110:MAX 1470 bytes, Kv2/SM300:MAX 8790 bytes)
typedef struct  _tagANSI_ST_TABLE26_TOU
{
        LIST_STATUS     STATUS;
        BYTE            NBR_VALID_ENTRIES;
        BYTE            LAST_ENTRY_ELEMENT;
        WORD            LAST_ENTRY_SEQ_NUMBER;
        BYTE            NBR_UNREAD_ENTRIES;
        ANSI_ST_TABLE25_TOU ENTRIES[2];
}  __ATTRIBUTE_PACKED__ ANSI_ST_TABLE26_TOU;

// Table 26: Self-Read Data Table - Demand Version (I210:MAX 488);
typedef struct  _tagANSI_ST_TABLE26_DEMAND
{
        LIST_STATUS     STATUS;
        BYTE            NBR_VALID_ENTRIES;
        BYTE            LAST_ENTRY_ELEMENT;
        WORD            LAST_ENTRY_SEQ_NUMBER;
        BYTE            NBR_UNREAD_ENTRIES;
        ANSI_ST_TABLE25_DEMAND ENTRIES[2];
}  __ATTRIBUTE_PACKED__ ANSI_ST_TABLE26_DEMAND;

/*-- Issue #1574 : Table 51번 ----*/
// Table 51: Actual Time and TOU
typedef struct  _tagANSI_ST_TABLE51
{
        BYTE    NBR_NONRECURR;
        BYTE    NBR_RECURR;
        BYTE    NBR_TIER;
        BYTE    NBR_CALENDER;
        BYTE    DATA[9];
}  __ATTRIBUTE_PACKED__ ANSI_ST_TABLE51;
                             
// Table 52: Clock Table
typedef struct	_tagANSI_ST_TABLE52
{
		BYTE	YEAR;       // yy
		BYTE	MONTH;      // mm
		BYTE	DAY;        // dd
        BYTE    HOUR;       // HH
        BYTE    MINUTE;     // MM
        BYTE    SECOND;     // SS
        BYTE    TIME_DATE_QUAL;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE52;

/*-- Issue #1574 : Table 54번 ----*/                                                                                     
// Table 54: Calendar Table 
typedef struct  _tagANSI_ST_TABLE54                                                                                  
{                                                                                                                 
        BYTE    DATA[406];                                                                                        
}  __ATTRIBUTE_PACKED__ ANSI_ST_TABLE54;                                                                    

// Table 55: Clock State Table
typedef struct	_tagANSI_ST_TABLE55
{
		BYTE	YEAR;       // yy
		BYTE	MONTH;      // mm
		BYTE	DAY;        // dd
        BYTE    HOUR;       // HH
        BYTE    MINUTE;     // MM
        BYTE    SECOND;     // SS
        BYTE    TIME_DATE_QUAL;
        BYTE    STATUS[2];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE55;

// Table 61: Actual Load Profile Table
typedef struct	_tagANSI_ST_TABLE61
{
		UINT	LP_MEMORY_LEN;
		WORD	LP_FLAGS;
		BYTE	LP_FORMATS;
		WORD	NBR_BLKS_SET1;
		WORD	NBR_BLK_INTS_SET1;
		BYTE	NBR_CHNS_SET1;
		BYTE	MAX_INT_TIME_SET1;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE61;

typedef struct  _tagLP_SET_STATUS
{
        BYTE    ORDER_FLAG:                 1;      // 0=ascending, 1=descending
        BOOL    OVERFLOW_FLAG:              1;      // FALSE=no overlow, TRUE=overflow
        BYTE    LIST_TYPE_FLAG:             1;      // 0=FIFO, 1=circular list
        BOOL    INHIBIT_OVERFLOW_FLAG:      1;      // FALSE/TRUE
        BOOL    INTERVAL_ORDER_FLAG:        1;      // 0=ascending, 1=descending
        BOOL    ACTIVE_MODE_FLAG:           1;      // FALSE/TRUE
        BOOL    TEST_MODE_FLAG:             1;      // FALSE/TRUE
        BOOL    FILLER:                     1;
}  __ATTRIBUTE_PACKED__ LP_SET_STATUS;

// 
typedef struct	_tagLP_SEL_SET1
{
        BOOL    END_RDG_FLAG:               1;
        BYTE    FILLER:                     7;
        BYTE    LP_SOURCE_SELECT:           8;      // Index into Table 16.
        BYTE    END_BLK_RDG_SOURCE_SELECT:  8;      // Index into Table 16.
}	__ATTRIBUTE_PACKED__ LP_SEL_SET1;

// Table 63: Load Profile Status Table 
typedef struct	_tagANSI_ST_TABLE63
{
        LP_SET_STATUS   STATUS;
		WORD	NBR_VALID_BLOCKS;
		WORD	LAST_BLOCK_ELEMENT;
		UINT	LAST_BLOCK_SEQ_NBR;
		WORD	NBR_UNREAD_BLOCKS;
		WORD	NBR_VALID_INT;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE63;

typedef struct _tagEVENT_TIME
{
	    BYTE    YEAR;
		BYTE    MON;
	    BYTE    DAY;
	    BYTE    HOUR;
	    BYTE    MIN;
	    BYTE    SEC;
}	__ATTRIBUTE_PACKED__ ANSI_EVENT_TIME;

// Standard Table 76: Event Log Table - ENTRIES
typedef struct  _tagEVENT_ENTRIES
{
        ANSI_EVENT_TIME EVENT_TIME;
		WORD  	EVENT_SEQ_NBR;
		WORD  	USERID;
		WORD	TBL_PROC_NBR:               11;
        BYTE    STD_VS_MFG_FLAG:            1;
        BYTE    SELECTOR:                   4;
		BYTE	EVENT_ARGUMENT;
}	__ATTRIBUTE_PACKED__ ANSI_EVENT_ENTRIES;

// Standard Table 76: Event Log Table
typedef struct  _tagANSI_ST_TABLE76
{
        BYTE    ORDER_FLAG:                 1;      // 0=ascending, 1=descending
        BOOL    OVERFLOW_FLAG:              1;      // FALSE=no overlow, TRUE=overflow
        BYTE    LIST_TYPE_FLAG:             1;      // 0=FIFO, 1=circular list
        BOOL    INHIBIT_OVERFLOW_FLAG:      1;      // FALSE/TRUE
        BYTE    FILL1:                      4;      // Filler
		WORD	NBR_VALID_ENTRIES;
		WORD	LAST_ENTRY_ELEMENT;
		UINT	LAST_ENTRY_SEQ_NBR;
		WORD	NBR_UNREAD_ENTRIES;
        ANSI_EVENT_ENTRIES  EVENT_ENTRIES[0];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE76;

// Standard Table 85: Register Data Table (I210+)
typedef struct  _tagANSI_ST_TABLE85
{
        UINT    TOTAL_DEL;                          // kWh
        UINT    TOTAL_DEL_PLUS_RCVD;                // kWh
        UINT    TOTAL_DEL_MINUS_RCVD;               // kWh
        UINT    TOTAL_RCVD;                         // kWh
        //    ERR_CAU_STATUS;
        BOOL    RCV_KWH:                    1;      // Energy flows from load to line for 2 minutes
        BOOL    METER_INVERSION:            1;      // Meter reversed for tampering
        BOOL    DC_CAUTION:                 1;      // Indicates the DC presence
        BOOL    SERVICE_CAUTION:            1;      // Indicates the wrong service connnection status
        BOOL    TEMPERATURE_CAUTION:        1;      // High temperature alert
        BOOL    METER_ERROR:                1;      // Indicates the Meter Chip error
        BOOL    NVRAM_CRC:                  1;      // Invalid CRC (ST85, ST86, MT20, MT21)
        BOOL    NVRAM_I2C_ERROR:            1;      // Indicates the NVRAM interface error
        BYTE    ERR_CAU_HISTORY;
        //    TAMPER DATA
        BYTE    FILLER_1:                   4;
        BYTE    DIRECTION_FLAG:             2;
        BYTE    DIR_COUNTER:                2;

        BYTE    MC_ERR_CTR;                         // Meter Chip error counter
        BYTE    WD_RESET_CTR;
        BYTE    PF_CTR;
        BYTE    SAG_CTR;
        BYTE    SWELL_CTR;
        BYTE    INVERSION_CTR;
        BYTE    FILLER_2[5];
        WORD    CRC;
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE85;

// Load Control Status Table
typedef struct	_tagANSI_LOAD_CTRL_STATUS
{
        BYTE    NAME[20];
        BYTE    REQUESTED_LEVEL;        // 0 or 100
        BYTE    OUTPUT_LEVEL;           // 0 or 100
        BYTE    SENSED_LEVEL;           // 0 or 100

        // Status
        BYTE    LEVEL_SUPPORTED_FLAG:           1;  // 0=binary, 1=percentage
        BYTE    MANUALLY_OVERRIDEN_FLAG:        1;  // not supported
        BYTE    WAITING_TO_BE_TURNED_ON_FLAG:   1;  // not supported
        BYTE    FILLER_1:                       5;

        // Duration Count Down
        BYTE    HOURS;
        BYTE    MINUTES;
        BYTE    SECONDS;
}	__ATTRIBUTE_PACKED__ ANSI_LOAD_CTRL_STATUS;

// Table 112: Load Control Status Table (I210+c)
/** GE Meter는 Load Control을 위한 6개의 Point를 가지게 된다.
 *
 * 1) Remote Connnect/Disconnect Switch (RCDC)
 * 2) Emergency Conservation Period (ECP)
 * 3) Demand Limiting Period (DLP)
 * 4) Perpayment Mode (PPM)
 * 5) LOCKOUT
 * 6) RCDC manual acknowledgement
 *
 */
typedef struct	_tagANSI_ST_TABLE112
{
        ANSI_LOAD_CTRL_STATUS   STATUS_ENTRIES[6];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE112;

// Standard Table 130: Relay Status Table
typedef struct  _tagANSI_ST_TABLE130
{
	    BYTE	RUN_STATUS;         // 0: OFF, 1:ON
	    BYTE	ACTIVE_STATUS;      // 0: OFF, 1:ON
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE130;

// Standard Table 131: Relay operate table
typedef struct  _tagANSI_ST_TABLE131
{
	    BYTE	RELAY_OPERATE_CODE; // 0: ACT OFF, 1: ACT ON, 2: SW ON, 3: SW OFF
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE131;

// Standard Table 132: Relay Log Table
typedef struct  _tagANSI_ST_TABLE132
{
        WORD    NBR_SWITCH_ON;
        WORD    NBR_SWITCH_OFF;
        struct  {
            BYTE    YEAR;
            BYTE    MONTH;
            BYTE    DAY;
            BYTE    HOUR;
            BYTE    MIN;
            BYTE    SEC;
            WORD    USERID;
            BYTE    OPER_TYPE;
        } __ATTRIBUTE_PACKED__ SWITCH_ON[10];
        struct  {
            BYTE    YEAR;
            BYTE    MONTH;
            BYTE    DAY;
            BYTE    HOUR;
            BYTE    MIN;
            BYTE    SEC;
            WORD    USERID;
        } __ATTRIBUTE_PACKED__ SWITCH_OFF[10];
}	__ATTRIBUTE_PACKED__ ANSI_ST_TABLE132;

typedef struct _tagMETERING_STATUS_TABLE
{
		BYTE	sensorError;
		BYTE	meterError;
}	__ATTRIBUTE_PACKED__ METERING_STATUS_TABLE;

/************************** Manufacturer Table **********************************/

// Manufacturer Table 0 : GE Device Table
typedef struct	_tagANSI_MT_TABLE0
{
        BYTE    MFG_VERSION_NBR;
        BYTE    MFG_REVISION_NBR;
        struct  {
            BYTE    GE_PART_NUMBER[5];
            BYTE    FW_VERSION;
            BYTE    FW_REVISION;
            BYTE    FW_BUILD;
            BYTE    MFG_TEST_VECTOR[4];
        } __ATTRIBUTE_PACKED__ ROM_CONST_DATA_RCD;
        BYTE    METER_TYPE;
        BYTE    METER_MODE;
        BYTE    REGISTER_FUNCTION;
        BYTE    INSTALLED_OPTION1;
        BYTE    INSTALLED_OPTION2;
        BYTE    INSTALLED_OPTION3;
        BYTE    INSTALLED_OPTION4;
        BYTE    INSTALLED_OPTION5;
        BYTE    INSTALLED_OPTION6;
        struct  {
            BYTE    UPGRADE_0:1;
            BYTE    UPGRADE_1:1;
            BYTE    UPGRADE_2:1;
            BYTE    UPGRADE_3:1;
            BYTE    UPGRADE_4:1;
            BYTE    UPGRADE_5:1;
            BYTE    UPGRADE_6:1;
            BYTE    UPGRADE_7:1;
            BYTE    UPGRADE_8:1;
            BYTE    UPGRADE_9:1;
            BYTE    UPGRADE_10:1;
            BYTE    UPGRADE_11:1;
            BYTE    UPGRADE_12:1;
            BYTE    UPGRADE_13:1;
            BYTE    UPGRADE_14:1;
            BYTE    UPGRADE_15:1;
            BYTE    UPGRADE_16:1;
            BYTE    UPGRADE_17:1;
            BYTE    UPGRADE_18:1;
            BYTE    UPGRADE_19:1;
            BYTE    UPGRADE_20:1;
            BYTE    UPGRADE_21:1;
            BYTE    UPGRADE_22:1;
            BYTE    UPGRADE_23:1;
            BYTE    UPGRADE_24:1;
            BYTE    UPGRADE_25:1;
            WORD    FILLER:6;
        } __ATTRIBUTE_PACKED__ UPGRADES_BFLD;
        BYTE    RESERVED[4];
        struct {
            BYTE    GE_PART_NUMBER[5];
            BYTE    FW_VERSION;
            BYTE    FW_REVISION;
            BYTE    FW_BUILD;
        } __ATTRIBUTE_PACKED__ TDK_CONST_RCD;
        BYTE    PATCH_ENABLED_FLAG;
        BYTE    PATCH_ACTIVE_FLAG;
        struct {
            BYTE    PART_NUMBER[5];
            BYTE    ROM_VER;
            BYTE    ROM_REV;
            BYTE    ROM_BUILD;
            BYTE    ROM_EEPROM_VER;
            BYTE    PATCH_VER;
            BYTE    PATCH_REV;
            BYTE    PATCH_BUILD;
            BYTE    PATCH_EEPROM_VER;
            BYTE    PATCH_FLAGS;
            WORD    CHECKSUM;
        } __ATTRIBUTE_PACKED__ PATCH_CONSTANTS;
        UINT    BASE_CRC;
        BYTE    EEPROM_VER;
        WORD    PATCH_CRC_CALCULATED;
        WORD    PATCH_CRC_DOWNLOADED;
        WORD24  LP_MEM_SIZE;
        UINT    PATCH_CODE_START_ADDRESS;
}	__ATTRIBUTE_PACKED__ ANSI_MT_TABLE0;

// Manufacturer Table 16: AMR SPI Frame 1 (I210+)
typedef struct _tagANSI_MT_TABLE16
{
        BYTE    START_CHR;
        BOOL    RESET_TAMPER:               1;
        BOOL    METERING_PROBLEM:           1;
        BOOL    REVERSE_ENERGY_FLOW:        1;
        BYTE    FILLER_1:                   1;
        BOOL    DISPLAY_MULTIPLIER:         1;  // if display multiplier is 10
        BOOL    DISPLAY_DIGITS:             1;  // if display digits are 4
        BYTE    SPI_MESSAGE_TYPE:           2;
        UINT    TOTAL_KWH;
        WORD    CRC;
}	__ATTRIBUTE_PACKED__ ANSI_MT_TABLE16;

typedef struct  _tagERROR_RCD                                                                               
{                                                                                                                 
        BOOL    METERING_ERROR:             1;
        BOOL    OPTION_BRD:                 1;
        BOOL    WATCHDOG_TIMEOUT:           1;
        BOOL    RAM:                        1;
        BOOL    ROM:                        1;
        BOOL    EEPROM:                     1;
        BOOL    BATT_POT_FAIL:              1;
        BOOL    DAP:                        1;
}   __ATTRIBUTE_PACKED__ ANSI_ERROR_RCD; 

typedef struct  _tagCAUTION_RCD                                                                               
{                                                                                                                 
        BOOL    LOW_BATTERY:                1;
        BOOL    LOW_POTENTIAL:              1;
        BOOL    DMD_THRESH_OVERLOAD:        1;
        BOOL    RECEIVED_KWH:               1;
        BOOL    LEADING_KVAH:               1;
        BOOL    UNPROGRAMMED:               1;
        BOOL    LOSS_OF_PROGRAM:            1;
        BOOL    TIME_CHANGED:               1;
        BOOL    BAD_PASSWORD:               1;
        BOOL    DC_DETECTED:                1;
        BYTE    FILLER1:                    6;
}   __ATTRIBUTE_PACKED__ ANSI_CAUTION_RCD; 

// Manufacturer Table 67: Meter Program Constants 2 Table (186 bytes)
typedef struct _tagANSI_MT_TABLE67
{
		BYTE	ENERGY_WRAPTEST_CONST[12];
		BYTE	RESERVED[108];
		BYTE	DEMAND_WRAPTEST_CONST[12];
		BYTE	RESERVED1[18];
		WORD	CUR_TRANS_RATIO;
		WORD	POT_TRANS_RATIO;
		WORD	PROGRAM_ID;
		BYTE	USER_DEFINED_FIELD1[6];
		BYTE	USER_DEFINED_FIELD2[6];
		BYTE	USER_DEFINED_FIELD3[6];
		WORD	P_FAIL_RECOGNITION_TIME;
		WORD	LP_OUTAGE_DURATION;
		UINT	DEMAND_RESET_TIMEOUT;
		BYTE	DEFAULT_TOU_RATE;
		BYTE	RESERVED2[3]; 
}	__ATTRIBUTE_PACKED__ ANSI_MT_TABLE67;

// Manufacturer Table 68: Error/Caution Control Table
typedef struct  _tagANSI_MT_TABLE68                                                                               
{                                                                                                                 
        ANSI_ERROR_RCD      ERROR_FREEZE;
        ANSI_CAUTION_RCD    CAUTION_ENABLE;
        ANSI_CAUTION_RCD    CAUTION_DISPLAY;
        ANSI_CAUTION_RCD    CAUTION_FREEZE;
        UINT                DMD_OVERLOAD_THRESH;
        BYTE                RESERVED[2];
        BYTE                PHA_VOLT_TOLERANCE;                
        BOOL                SERVICE_ERROR_DETECT_ENABLE;                
        WORD                SERVICE_MIN_VOLTAGE;                
        WORD                SERVICE_MAX_VOLTAGE;                
}   __ATTRIBUTE_PACKED__ ANSI_MT_TABLE68; 

// Manufacturer Table 75 : Scale Factors Table
typedef struct _tagANSI_MT_TABLE75
{
        WORD    V_SQR_HR_SF;
        WORD    V_RMS_SF;
        WORD24  I_SQR_HR_SF;
        WORD24  VA_SF;
        WORD    VAH_SF;
}	__ATTRIBUTE_PACKED__ ANSI_MT_TABLE75;

// Manufacturer Table 78 : Security Log Table (116 bytes)
typedef struct _tagANSI_MT_TABLE78
{
		BYTE	DT_LAST_CALIBRATED[5];
		BYTE	CALIBRATOR_NAME[10];
		BYTE	DT_LAST_PROGRAMMED[5];
		BYTE	PROGRAMMER_NAME[10];
		WORD	NBR_TIMES_PROGRAMMED;
		BYTE	NBR_DEMAND_RESETS;
		BYTE	DT_LAST_DEMAND_RESET[5];
		WORD	NBR_COMMUNICATIONS_SESSIONS;
		BYTE	DT_LAST_COMM_SESSION[5];
		WORD	NBR_BAD_PASSWORDS;
		WORD	NBR_RTP_ENTRIES;
		BYTE	DT_LAST_RTP_ENTRY[5];
		UINT	CUM_POWER_OUTAGE_SECS;
		WORD	NBR_POWER_OUTAGES;
		BYTE	DT_LAST_POWER_OUTAGE[5];
		BYTE	NBR_EEPROM_WRITES[3];
		BYTE	DT_LAST_TLC_UPDATE[5];			// Not supported in I210
		BYTE	TLC_NAME[10];					// Not supported in I210
		BYTE	DT_LAST_TRANS_INACC_ADJUST[5];	// Not supported in I210
		BYTE	TRANS_INACC_ADJUST_NAME[10];	// Not supported in I210
		BYTE	DT_LAST_TIME_CHANGE[5];
		WORD	NBR_SUSTAINED_MINUTES;
		WORD	NBR_SUSTAINED_INTERRUPTIONS;
		WORD	NBR_MOMENTARY_INTERRUPTIONS;
		WORD	NBR_MOMENTARY_EVENTS;
		BYTE	DT_LAST_AMR_COMMUNICATION[5]; 
}	__ATTRIBUTE_PACKED__ ANSI_MT_TABLE78;

/*-- Issue #1574 : Table 82번 ----*/
// Manufacturer Table 82: TOU Day Type Map Table
typedef struct  _tagANSI_MT_TABLE82                                                                               
{                                                                                                                 
        BYTE    DATA[7];                                                                                          
}       __ATTRIBUTE_PACKED__ ANSI_MT_TABLE82; 

// Manufacturer Table 84: Security Table
typedef struct  _tagANSI_MT_TABLE84                                                                               
{                                                                                                                 
        BYTE    MASTER_PASSWORD[10];                                                                                          
        BYTE    READER_PASSWORD[10];                                                                                          
        BYTE    CUSTOMER_PASSWORD[10];                                                                                          
        BYTE    SELF_FLAG;
        BYTE    DEMAND_RESET_INHIBIT_FLAG;
        BYTE    FILLER[6];
}       __ATTRIBUTE_PACKED__ ANSI_MT_TABLE84; 

// Manufacturer Table 112: Voltage Event Monitor Log Table - ENTRIES
typedef struct  _tagVOLTAGE_EVENT_ENTRIES
{
        ANSI_EVENT_TIME EVENT_TIME;
	    WORD    EVENT_SEQ_NBR;
	    BYTE    EVENT_TYPE;
	    WORD    EVENT_DURATION;
	    WORD    VOLTAGE_MAG_A;
	    WORD    VOLTAGE_MAG_B;
		WORD    VOLTAGE_MAG_C;
	    WORD    CURRENT_MAG_A;
	    WORD    CURRENT_MAG_B;
	    WORD    CURRENT_MAG_C;
}	__ATTRIBUTE_PACKED__ ANSI_VOLTAGE_EVENT_ENTRIES;

// Manufacturer Table 112: Voltage Event Monitor Log Table
typedef struct  _tagANSI_MT_TABLE112
{
        BYTE    ORDER_FLAG:                 1;      // 0=ascending, 1=descending
        BOOL    OVERFLOW_FLAG:              1;      // FALSE=no overlow, TRUE=overflow
        BYTE    LIST_TYPE_FLAG:             1;      // 0=FIFO, 1=circular list
        BOOL    INHIBIT_OVERFLOW_FLAG:      1;      // FALSE/TRUE
        BYTE    FILL1:                      4;      // Filler
	    WORD    NBR_VALID_ENTRIES;
	    WORD    LAST_ENTRY_ELEMENT;
	    UINT    LAST_ENTRY_SEQ_NBR;
	    WORD    NBR_UNREAD_ENTRIES;
	    WORD    SAG_COUNTER;
	    WORD    SWELL_COUNTER;
		ANSI_VOLTAGE_EVENT_ENTRIES  ENTRIES[0];
}	__ATTRIBUTE_PACKED__ ANSI_MT_TABLE112;

// Table 113: Power Quality Data Table
typedef struct	_tagANSI_MT_TABLE113
{
		WORD	RMS_VOLTAGE_PHA;
		WORD	RMS_VOLTAGE_PHC;
		BYTE	MOMENTARY_INTERVAL_PF;
        BYTE    TEMPERATURE;        // I210 only
}	__ATTRIBUTE_PACKED__ ANSI_MT_TABLE113;

// Manufacturer Table 115: Load Control Status Table (I210+c)
typedef struct  _tagANSI_MT_TABLE115
{
        // LC_STATUS_FLAGS
        BOOL    COMM_ERROR:                 1;      // 
        BOOL    SWITCH_CONTROLLER_ERROR:    1;      //
        BOOL    SWITCH_FAILED_TO_CLOSE:     1;      //
        BOOL    ALTERNATE_SOURCE:           1;      //
        BOOL    UNUSED_1:                   1;      // Rserved for future use
        BOOL    BYPASSED:                   1;      //
        BOOL    SWITCH_FAILED_TO_OPEN:      1;      //
        BYTE    PPM_ALERT:                  1;      //
        BYTE    MANUAL_ARM_TIMED_OUT:       1;      //
        BOOL    UNUSED_2:                   7;      // Rserved for future use
        WORD    LC_STATUS_HISTORY;
        
        // RCDC_STATE
        BYTE    ACTUAL_SWITCH_STATE:        1;      // 0=Open, 1=Closed
        BYTE    DESIRED_SWITCH_STATE:       1;      // 0=Open, 1=Closed
        BYTE    OPEN_HOLD_FOR_COMMAND:      1;      // 1=meter received an open command and will keeps switch open
        BYTE    WAITING_TO_ARM:             1;      // 
        BYTE    ARMED_FOR_CLOSURE:          1;
        BYTE    OUTAGE_OPEN_IN_EFFECT:      1;
        BYTE    LOCKOUT_IN_EFFECT:          1;
        BYTE    RESERVED_1:                 1;

        // LC_STATE
        BYTE    RESERVED_2:                 1;
        BYTE    ECP_STATE:                  1;      // 0=not active, 1=active
        BYTE    DLP_STATE:                  1;      // 0=not active, 1=active
        BYTE    PPM_STATE:                  1;      // 0=not active, 1=active
        BYTE    UNUSED_3:                   1;      // Rserved for future use
        BYTE    ECP_DISCONNECT:             1;      // 0=no disconnect, 1=disconnect
        BYTE    DLP_DISCONNECT:             1;      // 0=no disconnect, 1=disconnect
        BYTE    PPM_DISCONNECT:             1;      // 0=no disconnect, 1=disconnect

        BYTE    LC_RECONNECT_ATTEMPT_COUNT;
}	__ATTRIBUTE_PACKED__ ANSI_MT_TABLE115;

/*
typedef struct _tagNURI_TABLE02
{
		int		sensorError;				// Sensor error code
		BYTE	meterError;					// Meter error code
}	__ATTRIBUTE_PACKED__ NURI_TABLE02;
*/

typedef struct  _tagSTAT_TABLE
{
		UINT	TRANSMIT_PKT_COUNT;
		UINT	SEND_PKT_COUNT;
		UINT	RECV_PKT_COUNT;
		UINT	ERROR_PKT_COUNT;

		UINT	TOTAL_SEND_BYTES;
		UINT	TOTAL_RECV_BYTES;
		UINT	TOTAL_USE_MSEC;
		UINT	SENSOR_CONNECT_MSEC;
		UINT	TOTAL_REPLY_WAIT_MSEC;
		UINT	MIN_REPLY_MSEC;
		UINT	MAX_REPLY_MSEC;
}	__ATTRIBUTE_PACKED__ STAT_TABLE;

#endif	// __ANSI_H__








