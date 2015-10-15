#ifndef __CODI_TYPE_DEFINE_H__
#define __CODI_TYPE_DEFINE_H__

#include "typedef.h"
#include "codiFrame.h"
#include "codierr.h"

#define CODI_IDENTIFIER						0x1234			// Coordinator handle identifier
#define ENDI_IDENTIFIER						0x5678			// End Device handle identifier

#define CODIAPI_MAJOR_VERSION				0				// Major version
#define CODIAPI_MINOR_VERSION				5				// Minor version
#define CODIAPI_REVISION					1				// Revision

#define NODETYPE_UNKNOWN					0				// Unknown device
#define NODETYPE_COORDINATOR				1				// Coordinator
#define NODETYPE_ROUTER						2				// Router
#define NODETYPE_END_DEVICE					3				// End Device
#define NODETYPE_SLEEPY_END_DEVICE			4				// Sleepy end device
#define NODETYPE_MOBILE_END_DEVICE			6				// Mobile end device

#define CODISTATE_NORMAL					0				// Normal
#define CODISTATE_NOT_STARTED				1				// Coordinator service not started
#define CODISTATE_NOT_CONNECTED				2				// Coordinator not connected
#define CODISTATE_STACK_NOT_READY			3				// Stack is not ready
#define CODISTATE_STACK_DOWN				4				// Coordinator stack down
#define CODISTATE_JOIN_FAIL					5				// Coordinator join fail
#define CODISTATE_NO_BEACONS				6				// Coordinator no beacons
#define CODISTATE_NO_RESPONSE				100				// Coordinator no response
#define CODISTATE_ERROR						101				// Coordinator handle or device error

typedef struct	_tagCODIDEVICE
{
		int			nType;									// Device Type (1=RS232, 2=SPI, 100=Virtual)
		char		szDevice[65];							// Device name
		int			nSpeed;									// Baudrate
		int			nDataBit;								// Data bit
		int			nStopBit;								// Stop bit
		int			nParity;								// Parity bit
		int			nOptions;								// Option	
}	CODIDEVICE;

typedef struct	_tagCOORDINATOR
{
		UINT		nIdentifier;							// Handle identifier 0x1234
		void		*pService;								// Service class pointer	
		void		*pInterface;							// Interface class pointer
		DATASTREAM	*pStream;								// Data stream

		CODIDEVICE	device;									// Device configuration
		BOOL		bStarted;								// Start flag
		BOOL		bPassive;								// Passive mode
		BOOL		bBootloader;							// Bootloader mode
		BOOL		bNoResponse;							// No response flag
        BOOL        bSuperParent;                           // Super Parent Network
        BOOL        bWatchdog;                              // Watchdog Check 여부
		int			nNoResCount;							// 무응답 횟수
		int			nInvalidStateCount;						// 오류 상태 지속 횟수

		char		szGUID[17];								// String EUI64
		EUI64		eui64;									// EUI64 ID
        WORD        swVersion;                              // Ember Software Version
		WORD		shortid;								// Coordinator short id
		BYTE		fw;										// Firmware version
		BYTE		hw;										// Hardware version
		BYTE		zaif;									// ZA IF version 
		BYTE		zzif;									// ZZ IF version
		BYTE		build;									// Build number
		BYTE		rk;										// Reset kind
		BYTE		nStackStatus;							// Last stack status
		BYTE		nLastError;								// Last error code

        BYTE        autoset;                        		// Auto setting
        BYTE        channel;                        		// Channel
        WORD        panid;                          		// PANID
        BYTE        extpanid[8];                    		// Extended panid
        signed char power;                          		// RF Power
        BYTE        txpowermode;                    		// TX Power mode

        BYTE        baudrate;                       		// UART Baud Rate ¼³A¤
        BYTE        parity;                         		// UART
        BYTE        databit;                        		// UART
        BYTE        stopbit;                        		// UART
        BYTE        rtscts;                         		// RTS/CTS Enable / Disable

        BYTE        security;                       		// Security key enable
        BYTE        link_key[16];                   		// Link key
        BYTE        network_key[16];                		// Network key

        BYTE        address_table_size;						//
        BYTE        whole_address_table_size;				//
        BYTE        neighbor_table_size;					//
        BYTE        source_route_table_size;			    //
        BYTE        route_table_size;						//
        BYTE        max_hops;								//
        BYTE        packet_buffer_count;					//
        BYTE        key_table_size;                         // Issue #2490
        BYTE        max_children;                           //

		UINT		scan_mask;								// Channel scan mask
		BYTE		energy_scan_duration;
		BYTE		active_scan_duration;

		BYTE		permit_time;							// Permit

        BYTE        discovery;                              // Route Discovery Option
        BYTE        mhops;                                  // Current Multicast Hops

		UINT		nTotalSend;								// Total send bytes
		UINT		nTotalRecv;								// Total receive bytes

		cetime_t	tmBoot;									// Boot time
		cetime_t	tmLastSend;								// Last send time
		cetime_t	tmLastRecv;								// Last receive time
        cetime_t    tmLastComm;                             // Last communication time 
		cetime_t	tmLastSync;								// Last sync time
		int			pos;									// List position

}	COORDINATOR;

typedef struct	_tagENDDEVICEINFO
{
		ENDI_NODE_INFO		node;							// Node information
		ENDI_NETWORK_INFO	network;						// Network information
		ENDI_AMR_INFO		amr;							// AMR information
}	ENDDEVICEINFO;

#define ENDIOPT_SEQUENCE					0x0001			// Check Sequence Number
#define ENDIOPT_TIMEOUT						0x0002			// Connection Timeout

typedef struct  _tagFRAMEDATA
{
    ENDI_DATA_FRAME *       data;
    int                     nLength;
    int                     nTLength;
    BYTE                    seq;
    BYTE                    LQI;
    signed char             RSSI;
    struct _tagFRAMEDATA *  next;
}   FRAMEDATA;

typedef struct	_tagENDDEVICE
{
		UINT			nIdentifier;						// Handle identifier 0x1234
		void			*pService;							// Service class pointer	
		COORDINATOR		*codi;								// Coordinator pointer
        FRAMEDATA       *buffer;                            // Incomming Data Frame Buffer

		char			szGUID[17];							// String EUI64
		EUI64			id;									// EUI64 ID
		int				nOptions;							// End Device Options
		int				nTimeout;							// End Device Connection Timeout (0=disable)
        WORD            nShortId;                           // End Device Short ID
		BOOL			bConnected;							// Connection flag
		BOOL			bBinding;							// Binding flag
		BOOL			bParentBinding;						// Parent binding flag
        BOOL			bDirect;							// Direct Communication flag
		BYTE			nState;								// End Device state
		BYTE			seq;								// Last sequence
		int				nLastRecvSeq;						// Last recv sequence
		BOOL			bReference;							// Current reference flag

		UINT			nSeqError;							// 연속 시퀀스 에러 카운트
		UINT			nTotalSend;							// Total send bytes
		UINT			nTotalRecv;							// Total receive bytes
		UINT			nTotalDataSend;						// Total send data bytes
		UINT			nTotalDataRecv;						// Total receive data bytes
		UINT			nTotalNanSend;						// Total send nan data bytes
		UINT			nTotalNanRecv;						// Total receive nan data bytes

		cetime_t		tmLastConnect;						// Last connect time
		cetime_t		tmLastReceive;						// Last receive time

        int             nLastError;                         // Last Error Code
}	ENDDEVICE;

typedef struct	_tagCODIWAITREPLY
{
		COORDINATOR		*codi;								// Coordinator pointer
		EUI64			id;									// End Device EUI64 ID
		BYTE			flow;								// Read/Write
		BYTE			type;								// Frame type
		BYTE			cmd;								// Command type
		BYTE			seq;								// Command sequence
        BYTE            waitMask;                           // Reply Mask (WAIT_WRITE_RESPONSE, WAIT_READ_RESPONSE)
		HANDLE			hEvent;								// Event handle
		BOOL			replied;							// Reply flag
		BYTE			result;								// Reply result
		BYTE			*pszBuffer;							// Reply buffer
		int				nLength;							// Reply length
		int				pos;								// List position
}	CODIWAITREPLY;

#endif	/* __CODI_TYPE_DEFINE_H__ */

