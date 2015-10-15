#ifndef __AIDON_MCCB_H__
#define __AIDON_MCCB_H__

// AIDON MCCB Request message

#define MCCB_SET_ENABLE_USE_DISCONNECTED	0x4D
#define MCCB_SET_ENABLE_USE_AUTOMATICALLY	0x53
#define MCCB_SET_ENABLE_USE_CONNECT_NOW		0x55
#define MCCB_SET_DISABLE_USE_DISCONNECT		0x59
#define MCCB_GET_DEVICE_STATUS				0x65
#define MCCB_GET_PHASE_STATUS				0x69
#define MCCB_GET_LAST_ACCEPT_CONTROL_MSG	0x6F

// Response message 'Acknowledg'

#define MCCB_ACK_OK 						0x4B
#define MCCB_ACK_ERR_DE						0x53
#define MCCB_ACK_ERR_PE						0x55
#define MCCB_ACK_ERR_DE_PE					0x59
#define MCCB_ACK_ERR_REJECTED				0x65
#define MCCB_ACK_ERR_UNDEFINED				0x71
#define MCCB_FAIL							0xff

// Response message 'Device Status'

#define MCCB_STATUS_NDE_NPE 				0x41
#define MCCB_STATUS_NDE_PE 					0x47
#define MCCB_STATUS_DE_NPE 					0x63
#define MCCB_STATUS_DE_PE 					0x65
#define MCCB_STATUS_DE_BE 					0x69
#define MCCB_STATUS_DE_BE_PE 				0x6F
#define MCCB_STATUS_DE_RE 					0x71
#define MCCB_STATUS_DE_RE_PE 				0x77
#define MCCB_STATUS_DE_RE_BE 				0x7B
#define MCCB_STATUS_DE_RE_BE_PE 			0x7D

// Response message 'Last Accepted Control Mg'

#define MCCB_MSG_OK_EN_USE_DISCONNECTED		0x4D
#define MCCB_MSG_OK_EN_USE_AUTO				0x53
#define MCCB_MSG_OK_EN_USE_CONNECTED		0x55
#define MCCB_MSG_OK_DI_USE_DISCONNECT		0x59
#define MCCB_MSG_ERR_EN_USE_DISCONNECTED	0x65
#define MCCB_MSG_ERR_EN_USE_AUTO			0x69
#define MCCB_MSG_ERR_EN_USE_CONNECTED		0x6F
#define MCCB_MSG_ERR_DI_USE_DISCONNECT		0x71

// Response message 'Phase Status'

#define MCCB_PHASE_ALL_DETECTED				0x41
#define MCCB_PHASE_MISSING_1				0x47
#define MCCB_PHASE_MISSING_2				0x4B
#define MCCB_PHASE_MISSING_12				0x4D
#define MCCB_PHASE_MISSING_3				0x53
#define MCCB_PHASE_MISSING_13				0x55
#define MCCB_PHASE_MISSING_23				0x59
#define MCCB_PHASE_MISSING_123				0x5F

#endif	// __AIDON_MCCB_H__
