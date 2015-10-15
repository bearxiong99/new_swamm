#ifndef __CODI_FRAME_H__
#define __CODI_FRAME_H__

#include "codiDefine.h"

#ifdef _WIN32
#pragma pack(push, 1)
#endif

////////////////////////////////////////////////////////////////////
// Generic frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagCODI_IDENT								// Coordinator start flag
{
		BYTE				first;							// Always 0x87
		BYTE				second;							// Always 0x98
}	__ATTRIBUTE_PACKED__ CODI_IDENT;

typedef struct	_tagCODI_HEADER								// CODI Frame Header
{
		CODI_IDENT			ident;							// Start Flag
		BYTE				ctrl;							// Frame control
		BYTE				type;							// Frame type
		BYTE				seq;							// Sequence
		BYTE				len;							// Length
}	__ATTRIBUTE_PACKED__ CODI_HEADER;

typedef struct	_tagCODI_TAIL								// SINK Frame Tail
{
		WORD				fcs;							// CRC16
}	__ATTRIBUTE_PACKED__ CODI_TAIL;

typedef struct	_tagCODI_RADIO_STATUS
{
		BYTE				LQI;							// LQI
		signed char			RSSI;							// RSSI (sign)
}	__ATTRIBUTE_PACKED__ CODI_RADIO_STATUS;

typedef struct	_tagCODI_GENERAL_FRAME
{
		CODI_HEADER			hdr;							// Header
		BYTE				payload[CODI_MAX_PAYLOAD_SIZE];	// Frame payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_GENERAL_FRAME;

////////////////////////////////////////////////////////////////////
// Coordinator information frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagCODI_INFO_FRAME
{
		CODI_HEADER			hdr;							// Header
		BYTE				payload[CODI_MAX_PAYLOAD_SIZE];	// Frame payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_INFO_FRAME;

typedef struct	_tagCODI_BOOT_PAYLOAD
{
		EUI64				id;								// Cordinator EUI64
		WORD				shortid;						// Short ID
		BYTE				fw;								// Firmware version
		BYTE				hw;								// Hardware version
		BYTE				zaif;							// Application Device interface version
		BYTE				zzif;							// Zigbee interface version
		BYTE				build;							// Build version
		BYTE				rk;								// Reset kind
}	__ATTRIBUTE_PACKED__ CODI_BOOT_PAYLOAD;

typedef struct	_tagCODI_INFO_BOOT_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_BOOT_PAYLOAD	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_INFO_BOOT_FRAME;

typedef struct	_tagCODI_STACK_PAYLOAD
{
		BYTE				status;							// Stack status
}	__ATTRIBUTE_PACKED__ CODI_STACK_PAYLOAD;

typedef struct	_tagCODI_INFO_STACK_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_STACK_PAYLOAD	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_INFO_STACK_FRAME;

typedef struct	_tagCODI_JOIN_PAYLOAD
{
		WORD				short_id;						// Short ID
		EUI64				id;								// End device ID
		BYTE				status;							// Status
		WORD				parent_short_id;				// Parent short ID
}	__ATTRIBUTE_PACKED__ CODI_JOIN_PAYLOAD;

typedef struct	_tagCODI_INFO_JOIN_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_JOIN_PAYLOAD	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_INFO_JOIN_FRAME;

typedef struct	_tagCODI_ENERGYSCAN_PAYLOAD
{
		BYTE				channel;						// channel
		BYTE				rssi;							// Max RSSI
}	__ATTRIBUTE_PACKED__ CODI_ENERGYSCAN_PAYLOAD;

typedef struct	_tagCODI_INFO_ENERGYSCAN_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_ENERGYSCAN_PAYLOAD	payload;					// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_INFO_ENERGYSCAN_FRAME;

typedef struct	_tagCODI_ACTIVESCAN_PAYLOAD
{
		BYTE				channel;						// channel
		WORD				panid;							// PAN ID
		BYTE				extpanid[8];					// Extended PAN ID
		BYTE				expecting_join;					// Expecting Join
		BYTE				stack_profile;					// Stack profile
}	__ATTRIBUTE_PACKED__ CODI_ACTIVESCAN_PAYLOAD;

typedef struct	_tagCODI_INFO_ACTIVESCAN_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_ACTIVESCAN_PAYLOAD	payload;					// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_INFO_ACTIVESCAN_FRAME;

typedef struct	_tagCODI_SCANCOMPLETE_PAYLOAD
{
		BYTE				channel;						// channel
		BYTE				status;							// error condition on the current channel
}	__ATTRIBUTE_PACKED__ CODI_SCANCOMPLETE_PAYLOAD;

typedef struct	_tagCODI_INFO_SCANCOMPLETE_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_SCANCOMPLETE_PAYLOAD	payload;				// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_INFO_SCANCOMPLETE_FRAME;

typedef struct	_tagCODI_ERROR_PAYLOAD
{
		BYTE				code;							// Error code
}	__ATTRIBUTE_PACKED__ CODI_ERROR_PAYLOAD;

typedef struct	_tagCODI_INFO_ERROR_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_ERROR_PAYLOAD	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_INFO_ERROR_FRAME;

typedef struct	_tagCODI_ROUTERECORD_PAYLOAD
{
		EUI64				id;								// Source EUI ID
		WORD				shortid;						// Source short id
		BYTE				hops;							// Hop count
		WORD				routePath[CODI_MAX_ROUTING_PATH];	// Routing path
}	__ATTRIBUTE_PACKED__ CODI_ROUTERECORD_PAYLOAD;

typedef struct	_tagCODI_INFO_ROUTERECORD_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_ROUTERECORD_PAYLOAD	payload;				// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_INFO_ROUTERECORD_FRAME;

////////////////////////////////////////////////////////////////////
// Coordinator command frame format 
//////////////////////////////////////////////////////////////////////

typedef struct	_tagCODI_COMMAND_FRAME
{
		CODI_HEADER			hdr;							// Header
		BYTE				payload[CODI_MAX_PAYLOAD_SIZE];	// Frame payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_COMMAND_FRAME;

typedef struct	_tagCODI_WRITE_PAYLOAD
{
		BYTE				code;							// Write response code
}	__ATTRIBUTE_PACKED__ CODI_WRITE_PAYLOAD;

typedef struct	_tagCODI_WRITE_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_WRITE_PAYLOAD	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_WRITE_FRAME;

typedef struct	_tagCODI_MODULE_PAYLOAD
{
		EUI64				id;								// Cordinator EUI64
		WORD				shortid;						// Short ID
		BYTE				fw;								// Firmware version
		BYTE				hw;								// Hardware version
		BYTE				zaif;							// Application Device interface version
		BYTE				zzif;							// Zigbee interface version
		BYTE				build;							// Build version
		BYTE				rk;								// Reset kind
}	__ATTRIBUTE_PACKED__ CODI_MODULE_PAYLOAD;

typedef struct	_tagCODI_MODULE_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_MODULE_PAYLOAD	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_MODULE_FRAME;

typedef struct	_tagCODI_SERIAL_PAYLOAD
{
		BYTE				baudrate;						// UART Baud Rate 설정
		BYTE				parity;							// UART 패리트 비트 설정
		BYTE				databit;						// UART 데이터 비트 설정
		BYTE				stopbit;						// UART 스톱 비트 설정
		BYTE				rtscts;							// RTS/CTS Enable / Disable
}	__ATTRIBUTE_PACKED__ CODI_SERIAL_PAYLOAD;

typedef struct	_tagCODI_SERIAL_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_SERIAL_PAYLOAD	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_SERIAL_FRAME;

typedef struct	_tagCODI_NETWORK_PAYLOAD
{
		BYTE				autoset;						// Auto setting
		BYTE				channel;						// Channel
		WORD				panid;							// PANID
		BYTE				extpanid[8];					// Extended panid
		signed char			power;							// RF Power
		BYTE				txpowermode;					// TX Power mode
}	__ATTRIBUTE_PACKED__ CODI_NETWORK_PAYLOAD;

typedef struct	_tagCODI_NETWORK_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_NETWORK_PAYLOAD payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_NETWORK_FRAME;

typedef struct	_tagCODI_SECURITY_PAYLOAD
{
		BYTE				security;						// Security key enable
		BYTE				link_key[16];					// Link key
		BYTE				network_key[16];				// Network key
}	__ATTRIBUTE_PACKED__ CODI_SECURITY_PAYLOAD;

typedef struct	_tagCODI_SECURITY_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_SECURITY_PAYLOAD payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_SECURITY_FRAME;

typedef struct	_tagCODI_SCAN_PAYLOAD
{
		UINT				scan_mask;						// Channel scan mask
		BYTE				energy_scan_duration;			// Energy Scan Duration
		BYTE				active_scan_duration;			// Active Scan Duration
}	__ATTRIBUTE_PACKED__ CODI_SCAN_PAYLOAD;

typedef struct	_tagCODI_SCAN_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_SCAN_PAYLOAD 	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_SCAN_FRAME;

typedef struct	_tagCODI_BIND_PAYLOAD
{
		BYTE				type;							// Binding type
		EUI64				id;								// Destination address
		WORD				shortid;						// Short ID
		BYTE				hops;							// Hop count
		WORD				routingPath[CODI_MAX_ROUTING_PATH];	// Routing path
}	__ATTRIBUTE_PACKED__ CODI_BIND_PAYLOAD;

typedef struct	_tagCODI_BIND_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_BIND_PAYLOAD 	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_BIND_FRAME;

typedef struct	_tagCODI_BINDING_ENTRY
{
		BYTE				index;							// Binding table index
		BYTE				type;							// The type of biding
		BYTE				local;							// The endpoint on the local node
		BYTE				remote;							// The endpoint on the remote node
		EUI64				id;								// EUI64 ID
		WORD				lastheard;						// Coordinator heard since last tick
}	__ATTRIBUTE_PACKED__ CODI_BINDING_ENTRY;

typedef struct	_tagCODI_BINDING_PAYLOAD
{
		CODI_BINDING_ENTRY	list[16];						// Binding list
}	__ATTRIBUTE_PACKED__ CODI_BINDING_PAYLOAD;

typedef struct	_tagCODI_BINDINGTABLE_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_BINDING_PAYLOAD	payload;					// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_BINDINGTABLE_FRAME;

typedef struct	_tagCODI_NEIGHBOR_ENTRY
{
		BYTE				cnt;							// node index 
		WORD				networkid;						// short id
		BYTE				lqi;							// LQI
		BYTE				in_cost;						// In Cost
		BYTE				out_cost;						// Out Cost
		BYTE				age;							// 
		EUI64				id;								// EUI64 ID
}	__ATTRIBUTE_PACKED__ CODI_NEIGHBOR_ENTRY;

typedef struct	_tagCODI_NEIGHBOR_PAYLOAD
{
		CODI_NEIGHBOR_ENTRY	list[16];						// Neighbor node entry
}	__ATTRIBUTE_PACKED__ CODI_NEIGHBOR_PAYLOAD;

typedef struct	_tagCODI_NEIGHBORTABLE_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_NEIGHBOR_PAYLOAD payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_NEIGHBORTABLE_FRAME;

typedef struct	_tagCODI_KEY_PAYLOAD
{
		BYTE				type;							// Key assignment type
		EUI64				id;								// Destination address
}	__ATTRIBUTE_PACKED__ CODI_KEY_PAYLOAD;

typedef struct	_tagCODI_KEY_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_KEY_PAYLOAD 	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_KEY_FRAME;

typedef struct	_tagCODI_STACKMEMORY_PAYLOAD
{
		BYTE		ADDRESS_TABLE_SIZE;						// 집중기에서 사용가능한 address table size
		BYTE		WHOLE_ADDRESS_TABLE_SIZE;				// 전체 address table size (집중기 사용 불가 영역 포함)
		BYTE		NEIGHBOR_TABLE_SIZE;					//
        BYTE        SOURCE_ROUTE_TABLE_SIZE;                // Source Routing Table size
		BYTE		ROUTE_TABLE_SIZE;						//
		BYTE		MAX_HOPS;								//
		BYTE		PACKET_BUFFER_COUNT;					//
		WORD        SOFTWARE_VERSION;                       // Issue #2079 : Ember Software Version 정보 추가
        BYTE        KEY_TABLE_SIZE;                         // Issue #2490 : Key Table Size 추가
        BYTE        MAX_CHILDREN;                           // Ember Child Table Size
}	__ATTRIBUTE_PACKED__ CODI_STACKMEMORY_PAYLOAD;

typedef struct	_tagCODI_STACKMEMORY_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_STACKMEMORY_PAYLOAD payload;					// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_STACKMEMORY_FRAME;

typedef struct	_tagCODI_KEY_TABLE_ENTRY
{
		BYTE				cnt;							// node index 
		BYTE				status;							// status
		EUI64				id;								// EUI64 ID
}	__ATTRIBUTE_PACKED__ CODI_KEY_TABLE_ENTRY;

typedef struct	_tagCODI_KEY_TABLE_PAYLOAD
{
		CODI_KEY_TABLE_ENTRY	list[24];					// key table node entry
}	__ATTRIBUTE_PACKED__ CODI_KEY_TABLE_PAYLOAD;

typedef struct	_tagCODI_PERMIT_PAYLOAD
{
		BYTE				permit_time;					// Permit 시간
}	__ATTRIBUTE_PACKED__ CODI_PERMIT_PAYLOAD;

typedef struct	_tagCODI_PERMIT_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_PERMIT_PAYLOAD	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_PERMIT_FRAME;

typedef struct	_tagCODI_FORMNETWORK_PAYLOAD
{
		BYTE				type;							// Join 명령어 타입
}	__ATTRIBUTE_PACKED__ CODI_FORMNETWORK_PAYLOAD;

typedef struct	_tagCODI_FORMNETWORK_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_FORMNETWORK_PAYLOAD payload;					// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_FORMNETWORK_FRAME;

typedef struct	_tagCODI_RESET_PAYLOAD
{
		BYTE				reset;							// Reset flag(1=Reset system)
}	__ATTRIBUTE_PACKED__ CODI_RESET_PAYLOAD;

typedef struct	_tagCODI_RESET_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_RESET_PAYLOAD	payload;						// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_RESET_FRAME;

typedef struct	_tagCODI_SCANNETWORK_PAYLOAD
{
		BYTE				type;							// Channel scan type
		UINT				scan_mask;						// Channel scan mask
		BYTE				duration;						// Energy Scan Duration
}	__ATTRIBUTE_PACKED__ CODI_SCANNETWORK_PAYLOAD;

typedef struct	_tagCODI_SCANNETWORK_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_SCANNETWORK_PAYLOAD 	payload;				// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_SCANNETWORK_FRAME;

typedef struct	_tagCODI_MANYTOONE_PAYLOAD
{
		BYTE				type;							// Many to one request type
		BYTE				radius;							// Request Radius
}	__ATTRIBUTE_PACKED__ CODI_MANYTOONE_PAYLOAD;

typedef struct	_tagCODI_MANYTOONE_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_MANYTOONE_PAYLOAD 	payload;					// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_MANYTOONE_FRAME;

typedef struct	_tagCODI_BOOTLOAD_PAYLOAD
{
		BYTE				type;							// Bootloader type
		BYTE				key[2];							// Bootloader key
}	__ATTRIBUTE_PACKED__ CODI_BOOTLOAD_PAYLOAD;

typedef struct	_tagCODI_BOOTLOAD_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_BOOTLOAD_PAYLOAD 	payload;					// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_BOOTLOAD_FRAME;

typedef struct	_tagCODI_ROUTE_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_ROUTERECORD_PAYLOAD 	payload;				// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_ROUTE_FRAME;

typedef struct _tagCODI_ROUTEDISCOVERY_PAYLOAD
{
        BYTE                type;                           // Route Discovery On/Off
}   __ATTRIBUTE_PACKED__ CODI_ROUTEDISCOVERY_PAYLOAD;

typedef struct	_tagCODI_ROUTEDISCOVERY_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_ROUTEDISCOVERY_PAYLOAD 	payload;            // Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_ROUTEDISCOVERY_FRAME;

typedef struct _tagCODI_MULTICASTHOP_PAYLOAD
{
        BYTE                hop;                            // Multicast hop count
}   __ATTRIBUTE_PACKED__ CODI_MULTICASTHOP_PAYLOAD;

typedef struct	_tagCODI_MULTICASTHOP_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_MULTICASTHOP_PAYLOAD 	payload;                // Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_MULTICASTHOP_FRAME;

typedef struct _tagCODI_EXTENDEDTIMEOUT_PAYLOAD
{
        BYTE                type;                           // Extended Timeout type
        EUI64               id;                             // EUI 64
}   __ATTRIBUTE_PACKED__ CODI_EXTENDEDTIMEOUT_PAYLOAD;

typedef struct	_tagCODI_EXTENDEDTIMEOUT_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_EXTENDEDTIMEOUT_PAYLOAD 	payload;			// Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_EXTENDEDTIMEOUT_FRAME;

typedef struct _tagCODI_DIRECTCOMM_PAYLOAD
{
        BYTE                type;                           // Direct Communication type (0x01:Set, 0x02:Delete)
        EUI64               id;                             // EUI 64
		WORD				shortid;						// Source short id
}   __ATTRIBUTE_PACKED__ CODI_DIRECTCOMM_PAYLOAD;

typedef struct	_tagCODI_DIRECTCOMM_FRAME
{
		CODI_HEADER			hdr;							// Header
		CODI_DIRECTCOMM_PAYLOAD     payload;                // Payload
		CODI_TAIL			tail;							// Tail
}	__ATTRIBUTE_PACKED__ CODI_DIRECTCOMM_FRAME;

#ifdef _WIN32
#pragma pack(pop)
#endif

#include "endiFrame.h"

#endif	// __CODI_FRAME_H__
