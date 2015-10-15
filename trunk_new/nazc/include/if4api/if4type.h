#ifndef __IF4_TYPE_H__
#define __IF4_TYPE_H__

#include "vartype.h"

#define IF4_MAX_WINDOW_SIZE			8				// Window Size	
#define IF4_MAX_PACKET_SIZE			512				// Packet Size

typedef int			IF4Error;						// Error Type
typedef MIBValue	IF4MIBValue;					// MIB Value

typedef int (*PFNIF4_QUERY_HANDLER)(OID3 *pOid, BYTE nMethod, MIBValue *pValue);
typedef BOOL (*PFNIF4RECVCALLBACK)(void *pHandle, char *pszBuffer, int nLength);

typedef struct	_tagIF4Invoke
{
		char		szAddress[32];					// FEP 서버 주소
		int			port;							// 포트 번호
		int			timeout;						// 타임 아웃 (초)
		OID3		cmd;							// Command
		BYTE		tid;							// TID
		BYTE		nFlag;							// Flag
		BOOL		bMultipart;						// Multi part
		BOOL		bAsync;							// Async Mode (현재 지원 안함)
		int			nMultiSeek;						// Multi part seek
		BYTE		nCtrlCode;						// Control Code
		int			nError;							// Last Error
		char		*pFrame;						// Frame Buffer
		int			nLength;						// Frame Length
		int			nSourceLength;					// Source Frame Length
		void		*pClient;						// Connection Client
		HANDLE		hEvent;							// Event Handle
		int			nParamCount;					// Parameter Count
		MIBValue	*pParamNode;					// Parameter Link Node
		MIBValue	*pLastParam;					// Last Parameter
		int			nResultCount;					// Result Count
		MIBValue	*pResultNode;					// Result Link Node
		MIBValue	*pLastResult;					// Last Result
		MIBValue	**pResult;						// Result List
		time_t		tmTryTime;						// 시도 시간
		time_t		tmConnectTime;					// 연결 시간
		time_t		tmLastActive;					// 마지막 송/수신 시간
		PFNIF4RECVCALLBACK	pfnCallback;			// CALLBACK
		void		*pCallbackParam;				// Callback Parameter
}	IF4Invoke, *PIF4Invoke;

typedef struct	_tagIF4Wrapper
{
		WORKSESSION		*pSession;					// Connect Session
		DATASTREAM		*pStream;					// Data Stream
		MIBValue		*pLast;						// Last Result Pointer
		MIBValue		*pResult;					// Result Value
		int				nResult;					// Result Param Count
}	IF4Wrapper, *PIF4Wrapper;

typedef int (*PFNIF4_COMMAND_HANDLER)(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

typedef struct	_tagIF4_COMMAND_TABLE
{
		OID3					oid;				// Command OID
		const char				*pszName;			// Command Name
		PFNIF4_COMMAND_HANDLER	pfnCommand;			// Command Function
		BOOL					bSave;				// Log Save Flag		
}	IF4_COMMAND_TABLE, *PIF4_COMMAND_TABLE;

typedef struct	_tagIF4MeterItem
{
		BYTE		id[8];							// Sensor ID
		WORD		len;							// Data Length
		char		*pszBuffer;						// Data Buffer
}	IF4MeterItem, *PIF4MeterItem;

typedef struct	_tagIF4PACKET
{
		BYTE		nSeq;							// Sequence
		WORD		nLength;						// Packet Length
		BOOL		bState;							// Ack/Nak
		char		szBuffer[IF4_MAX_PACKET_SIZE];	// Packet Buffer
}	IF4PACKET, *PIF4PACKET;

typedef struct	_tagIF4WINDOW
{
		int			nSent;
		BYTE		nStart;
		BYTE		nEnd;
		IF4PACKET	packet[IF4_MAX_WINDOW_SIZE];
}	IF4WINDOW, *PIF4WINDOW;

#endif	// __IF4_TYPE_H__
