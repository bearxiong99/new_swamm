#ifndef __MBUS_DEFINE_H__
#define __MBUS_DEFINE_H__

#include "mbusError.h"


#define MBUS_FRAME_SINGLE                   0xE5
#define MBUS_FRAME_SHORT                    0x10
#define MBUS_FRAME_CONTROL                  0x68
#define MBUS_FRAME_LONG                     0x68

#define MBUS_FRAME_STOP                     0x16


#define MBUS_CTRL_SND_NKE                   0x40        // Initialization of Slave
#define MBUS_CTRL_SND_UD                    0x53        // Send User Data to Slave
#define MBUS_CTRL_REQ_UD1                   0x5A        // Request for Class 1 Data
#define MBUS_CTRL_REQ_UD2                   0x5B        // Request for Class 2 Data
#define MBUS_CTRL_RSP_UD                    0x08        // Data Transfer form Slave to Master after Request

#define MBUS_CTRL_MASK_FCB                  0x30        // Frame Count Bit
#define MBUS_CTRL_MASK_FCV                  0x10        // Frame Count Bit Vaild

#define MBUS_CTRL_MASK_ACB                  0x30        // Access Demand
#define MBUS_CTRL_MASK_DFC                  0x10        // Data Flow Control

#define MBUS_CTRL_MASK_REQUEST              0x40
#define MBUS_CTRL_MASK_REPLY                0x00

#define MBUS_ADDR_BROADCAST_REPLY           0xFE   
#define MBUS_ADDR_BROADCAST_NOREPLY         0xFF   

#define MBUS_CI_DATA_MODE1                  0x51
#define MBUS_CI_DATA_MODE2                  0x55
#define MBUS_CI_SELECT_MODE1                0x52
#define MBUS_CI_SELECT_MODE2                0x56
#define MBUS_CI_RESET                       0x50
#define MBUS_CI_SYNC                        0x54
#define MBUS_CI_B300                        0xB8
#define MBUS_CI_B600                        0xB9
#define MBUS_CI_B1200                       0xBA
#define MBUS_CI_B2400                       0xBB
#define MBUS_CI_B4800                       0xBC
#define MBUS_CI_B9600                       0xBD
#define MBUS_CI_B19200                      0xBE
#define MBUS_CI_B38400                      0xBF
#define MBUS_CI_READ_RAM                    0xB1
#define MBUS_CI_SEND_DATA                   0xB2
#define MBUS_CI_CALIBRATION                 0xB3
#define MBUS_CI_READ_EEPROM                 0xB4
#define MBUS_CI_SW_TEST                     0xB6


#endif	// __MBUS_DEFINE_H__

