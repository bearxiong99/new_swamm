//////////////////////////////////////////////////////////////////////
//
//	typedef.h: AIMIR MCU  Data Type Definitions.
//
//	This file is a part of the AIMIR.
//	(c)Copyright 2005 NURITELECOM Telecom Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@naver.com
//	http://www.nuritelecom.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#ifndef _WIN32
#include <sys/types.h>
#ifndef  ARM
#include <sys/time.h>
#endif
#else
#include <windows.h>
#endif	/* _WIN32 */

#include <time.h>

#ifdef _WIN32
#define __ATTRIBUTE_PACKED__
#else
#define __ATTRIBUTE_PACKED__			__attribute__ ((packed))
#endif

#ifndef _WIN32
#define _MAX_PATH						255			// Maximum Path String Size

#endif

#ifndef FALSE
#define FALSE							0
#endif
#ifndef TRUE
#define TRUE							1
#endif

#define DISABLE							FALSE
#define ENABLE							TRUE

#define MAX_GUID_STRING					16			// Maximum GUID String Size
#define MAX_SESSION_WINDOW				16			// Maximum Window Size

#define ETHER_TYPE_LAN					0
#define ETHER_TYPE_DHCP					1
#define ETHER_TYPE_PPPOE				2
#define ETHER_TYPE_PPP					3

#define MOBILE_TYPE_NONE				0
#define MOBILE_TYPE_GSM					1
#define MOBILE_TYPE_CDMA				2
#define MOBILE_TYPE_PSTN				3

#define MOBILE_MODE_CSD					0
#define MOBILE_MODE_PACKET				1
#define MOBILE_MODE_ALWAYS				2

#define METERING_CYCLE_IMMEDIATELY		1
#define METERING_CYCLE_DAILY			2
#define METERING_CYCLE_WEEKLY			3
#define METERING_CYCLE_DEFAULT			METERING_CYCLE_WEEKLY

#define EVTCLASS_CRITICAL				0
#define EVTCLASS_MAJOR					1
#define EVTCLASS_MINOR					2
#define EVTCLASS_NORMAL					3
#define EVTCLASS_INFO					4
#define EVTCLASS_USER					EVTCLASS_CRITICAL

#define GSM_TYPE_MC55					0

#define CDMA_TYPE_GROWELL				0
#define CDMA_TYPE_ANYDATA				1

#define USER_GROUP_ADMIN				0				// Super User
#define USER_GROUP_USER					1				// Normal User
#define USER_GROUP_GUEST				2				// Guest User

#define	RST_UNKNOWN                     0				
#define	RST_COMMAND                     1			
#define	RST_UPFIRM                      2	
#define	RST_AUTORESET                   3					
#define	RST_SYSMONITOR                  4					
#define	RST_LOWBATT                     5			
#define	RST_WATCHDOG                    6			

#define INITIAL_POSITION                0

//  [11/19/2010 DHKim]
#define GROUP_MEMBER_SIZE			256

/** 이 define은 외주 괄호가 없어서 의도하지 않은 결과를 내기도 했다. 버그 수정 */
#define MIN(x,y)                        ((x < y) ? x : y)
#define MAX(x,y)                        ((x > y) ? x : y)

typedef unsigned char					BYTE;
typedef unsigned short					WORD;
typedef unsigned int					UINT;
typedef signed int					    INT;

#if     !defined(_WIN32)
typedef char							BOOL;
typedef char *					        LPSTR;
typedef unsigned int					DWORD;
typedef void *							HANDLE;
typedef int								POSITION;
typedef time_t							cetime_t;
typedef long long				        LONG;
typedef unsigned long long				ULONG;
#if     defined(__PTR64__)
typedef unsigned long                   UINT_PTR;
#else
typedef unsigned int                    UINT_PTR;
#endif
#else
typedef DWORD							cetime_t;
#endif

typedef enum 
{ 
    ORDER_LSB = 0,  // Little Endian
    ORDER_MSB = 1,  // Big Endian
    ORDER_DESC = 2, // 
    ORDER_ASC = 3
} ORDER;

typedef struct	_tagTIMETICK
{
#ifdef _WIN32
        DWORD           t;                          // Tick count (DWORD)
#else
		struct	timeval	t;							// Time of day
#endif
} 	TIMETICK, *PTIMETICK;

#if     defined(_WIN32)
#pragma pack(push, 1)
#endif

typedef struct  _tagWORD24                          // 3 Bytes unsigned
{
    BYTE    v[3];
}   __ATTRIBUTE_PACKED__ WORD24;

typedef struct	_tagEUI64
{
		BYTE			ids[8];						// 8 Bytes Stream
} 	__ATTRIBUTE_PACKED__ EUI64, *PEUI64;

typedef struct  _tagGEUI64
{
        EUI64           gid;
        BYTE            gport;                      // 8 Bytes Stream
}   __ATTRIBUTE_PACKED__ GEUI64, *PGEUI64;

								
typedef struct	_tagOID3
{
		BYTE			id1;						// First OID
		BYTE			id2;						// Second OID
		BYTE			id3;						// 3th OID
}	__ATTRIBUTE_PACKED__ OID3, *POID3;

typedef struct	_tagTIMEDATE
{
		WORD			year;						// year
		BYTE			mon;						// month
		BYTE			day;						// day
} 	__ATTRIBUTE_PACKED__ TIMEDATE, *PTIMEDATE;

typedef struct	_tagTIMESTAMP
{
		WORD			year;						// Year
		BYTE			mon;						// Month
		BYTE			day;						// Day
		BYTE			hour;						// Hour
		BYTE			min;						// Min
		BYTE			sec;						// Sec
} 	__ATTRIBUTE_PACKED__ TIMESTAMP, *PTIMESTAMP;

typedef struct	_tagTIMESTAMP6
{
		BYTE			year;						// Year (2000년 이후)
		BYTE			mon;						// Month
		BYTE			day;						// Day
		BYTE			hour;						// Hour
		BYTE			min;						// Min
		BYTE			sec;						// Sec
} 	__ATTRIBUTE_PACKED__ TIMESTAMP6, *PTIMESTAMP6;

typedef struct	_tagTIMESTAMP5
{
		BYTE			year;						// Year (2000년 이후)
		BYTE			mon;						// Month
		BYTE			day;						// Day
		BYTE			hour;						// Hour
		BYTE			min;						// Min
} 	__ATTRIBUTE_PACKED__ TIMESTAMP5, *PTIMESTAMP5;

typedef struct	_tagGMTTIMESTAMP
{
		WORD			tz;							// Time zone
		WORD			dst;						// DST Value
		WORD			year;						// Year
		BYTE			mon;						// Month
		BYTE			day;						// Day
		BYTE			hour;						// Hour
		BYTE			min;						// Min
		BYTE			sec;						// Sec
} 	__ATTRIBUTE_PACKED__ GMTTIMESTAMP, *PGMTTIMESTAMP;

typedef struct	_tagIPADDR
{
		BYTE			dot1;						// DOT1
		BYTE			dot2;						// DOT2
		BYTE			dot3;						// DOT3
		BYTE			dot4;						// DOT4
} 	__ATTRIBUTE_PACKED__ IPADDR, *PIPADDR;

typedef union   _tagUNIONVALUE {
        BYTE bv;
        WORD wv;
        UINT iv;
        ULONG ulv;
        double dv;
}   __ATTRIBUTE_PACKED__ UNIONVALUE;

#if     defined(_WIN32)
#pragma pack(pop)
#endif

typedef struct	_tagLINKNODE
{
		void	*pData;
		struct	_tagLINKNODE	*pNext;
}	LINKNODE, *PLINKNODE;

typedef struct  _tagWORKSESSION
{
#if defined(_WIN32) 
        SOCKET          sSocket;
#else
		int				sSocket;					// File descriptor
#endif
		char			szAddress[32];				// Address
		void			*pThis;						// Handle
#if     defined(_WIN32) 
		HANDLE			hFile;						// File handle
#endif
		UINT_PTR        dwParam;					// Parameter
		UINT_PTR        dwUserData;					// User Save Data

		void			*pDownload;
		void			*pChunk;
		int				nDownloadSeq;				// Download sequence number

		BOOL			bStreamTimeout;
		TIMETICK		lastTick;					// Last Input tick
		long			lLastInput;					// Last Input Time
		int				nPosition;					// List Position

        unsigned int    nDebugPoint;                // Debug Ponint
}   WORKSESSION, *PWORKSESSION;

typedef struct  _tagTCPDATASTREAM
{
        int     		nTID;						// I/O ID
        int     		nLength;					// Length
        char    		*pszBuffer;					// Data Buffer
        void    		*pSession;					// Self Session
        int     		nPosition;					// List Position
}   TCPDATASTREAM, *PTCPDATASTREAM;

typedef TCPDATASTREAM	SIODATASTREAM;

typedef struct  _tagDATASTREAM
{       
        int     		nMode;						// Active Mode
        int     		nState;						// Job State
		BYTE			nSequence;					// Sequence
		int				nLastSeq;					// Recv Last Sequence
        int     		nCommand;					// Command
        int     		nCount;						// Count
        int     		nSize;						// Need Size
        int     		nLength;					// Real Length
        int     		nAllocation;				// Buffer Allocation Size
        int     		nGrow;						// Grow Size
        UINT_PTR        dwParam;					// User Save Data
        char    		*pszBuffer;					// Buffer
        BYTE            nService;                   // Datastream Service Type

		// WINDOWING
		BOOL			bWindow;
		BOOL			bTerminate;
		int				nMultiSeek;					// Multi Frame Seek
		int				nWindowSize;				// Window Packet Size
		int				nWindowCount;				// Window Count (Issue #2049)
		int				nWindow;					// Window Index
		int				arWindow[MAX_SESSION_WINDOW];	// Window Array
		BYTE			arNeed[MAX_SESSION_WINDOW];
		int				nWindowAlloc;
		int				nTotalSize;					// Multi-Part Total Size
		char			*pszWindow;
}   DATASTREAM, *PDATASTREAM;

#define EVENT_FLAG_ALERT			0x80			// 서버에 전송해야 하는 경우
#define EVENT_FLAG_SENT				0x01			// 전송 완료 플래그 

typedef struct	_tagEVENTWRITEITEM
{
		WORD			nLength;					// Record Length
		OID3			id;							// Event ID
		TIMESTAMP		tmEvent;					// Event Time
		BYTE			nFlags;						// Event Flag
		char			szData[255];				// Event Data
}	__ATTRIBUTE_PACKED__ EVENTWRITEITEM, *PEVENTWRITEITEM;

typedef struct	_tagUSERINFO
{
		char			szUser[17];					// User ID
		char			szPassword[17];				// Password
		int				nGroup;						// Group
		int				nPosition;					// List Position
}	USERINFO, *PUSERINFO;

typedef struct	_tagNODEITEM
{
		void			*pData;						// Data entry	
		struct	_tagNODEITEM	*pNext;				// Next node
		struct	_tagNODEITEM	*pPrev;				// Previous node
}	NODEITEM, *PNODEITEM;

typedef struct	_tagNODE
{
		char	*pszName;							// Node name
		void	*pLocker;							// Node locker
		int		nCount;								// Total node count
		struct	_tagNODEITEM	*pHead;				// First node pointer
		struct	_tagNODEITEM	*pTail;				// Last node pointer
}	NODE, *PNODE;

typedef struct  _tagDISTRIBUTEITEM
{
        BYTE        szDistribut;            //장비 타입(0:mcu, 1:sensor, 2:codi)
        BYTE        pszTriggered[128];
        char        pszOldHwVer[50];
        char        pszOldSwVer[50];
        char        pszOldBuildVer[50];
        char        pszNewHwVer[50];
        char        pszNewSwVer[50];
        char        pszNewBuildVer[50];
        char        pszBinMD5[33];
        BYTE        pszBinFilePat[128];
        char        pszDiffMD5[33];
        BYTE        pszDiffFilePat[128];
        BYTE        pszIdList[20];
        BYTE        szOTAtype; //OTA run type(1.4.0) : Run type(0: schedule, 0: run, 1: State, 2: cancel)
        BYTE        szModemType;//  Modem type(1.4.0): Modem type (0:ge, 1:aidon, 2:kamstrup)
        BYTE        szTransferType; //Data transfer type(0:auto, 1: Multicast, 2: Unicast) auto일 때 50대 기준으로 판단이 된다. 
        BYTE        szOTALevel; //(1.4.0) : OTA 단계 조절(0 : ALL, 1 : Data send, 2 : Verify, 3: Install)
        BYTE        szOTARetryCnt;// Count(1.4.0) : OTA 실패한 Sensor에 대한 Retry 횟수


}   DISTRIBUTEITEM, *PDISTRIBUTEITEM;

typedef struct  _tagDISTRIBUTECLI
{
        BYTE        szDistribut;
        BYTE        szmethod;  
        char        pszSwVer[10];
        char        pszBuildVer[10];
        BYTE        szMETERtype;
        BYTE        pszFilePat[128];


}   DISTRIBUTECLI, *PDISTRIBUTECLI;

typedef struct  _tagMETER_STAT
{
		int				nCount;
		int				nSendCount;
		int				nRecvCount;
		int				nError;
		int				nSend;
		int				nRecv;
		int				nUseTime;
		int				nConnectTime;
		int				nResponseTime;
		int				nMinTime;
		int				nMaxTime;
}	METER_STAT;

typedef struct	_tagPACKAGEINFO
{
		BYTE	nType;					// Package type
		char	szModel[21];			// Model name
		WORD	nHwVersion;				// HW version
		WORD	nSwVersion;				// SW version
		WORD	nBuildVersion;			// Build version
		char	szChecksum[33];			// MD5 Checksum
}	PACKAGEINFO;

//////////////////////////////////////////////////////////////////////////
//  [10/14/2010 DHKim]
//  GROUP DB
//////////////////////////////////////////////////////////////////////////

typedef struct	_tagGROUPINFO
{
	/*
	char	pGroupName[GROUP_MEMBER_SIZE];
	int		nMemberCnt;
	int		nParamCnt;
	char	pParam[GROUP_MEMBER_SIZE];
	BYTE	pMemberData[0];
	*/
	int		nTotalCnt;
	int		nTotalGroupCnt;
	int		*nGroupKey;
	char	*pGroupName;
	int		*nMemberCnt;
	EUI64	*pMemberID;
}	__ATTRIBUTE_PACKED__ GROUPINFO, *PGROUPINFO;

typedef struct	_tagGROUPCOMMANDINFO
{
	int		nCommandKey;
	int		nGroupKey;
	EUI64	id;
	char	pGroupName[GROUP_MEMBER_SIZE];
	OID3	commandOID;
	int		nCommandState;
	TIMESTAMP	commandCreateTime;
	TIMESTAMP	commandExecTime;
	int		nResult;
}	__ATTRIBUTE_PACKED__ GROUPCOMMANDINFO, *PGROUPCOMMANDINFO;

typedef struct	_tagMEMBERINFO
{
	EUI64	pID;
	OID3	pCommand;
	BOOL	bOperationState;
}	 MEMBERINFO, *PMEMBREINFO;

#endif 	// __TYPEDEF_H__
