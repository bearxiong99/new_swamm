#ifndef __OPERATION_H__
#define __OPERATION_H__

#include "Event.h"
#include "codierr.h"

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "DataStream.h"

#define ONDEMAND_OPR_READ				1
#define ONDEMAND_OPR_WRITE				2
#define ONDEMAND_OPR_DATA				3
#define ONDEMAND_OPR_CHECK				10	

#define METER_TYPE_AIDON				0
#define METER_TYPE_GE					1
#define METER_TYPE_KAMST				2
#define METER_TYPE_MBUS_MASTER          91
#define METER_TYPE_MBUS_SLAVE           92
#define METER_TYPE_VCORR				99

#define ONDEMAND_ERROR_OK				0				// 명령 정상 처리
#define ONDEMAND_ERROR_FAIL				1				// 명령 처리 실패
#define ONDEMAND_ERROR_BUSY				2				// 이미 센서가 통신중
#define ONDEMAND_ERROR_NOT_CONNECT		3				// 접속 실패
#define ONDEMAND_ERROR_PARAM			4				// 파라메터 오류
#define ONDEMAND_ERROR_INIT				5				// 미터가 초기화 상태 일때
#define ONDEMAND_ERROR_NOT_SAVE			6				// Scanning등으로 저장하지 않는 경우

#define LockOperation					m_pOperation->lockOperation
#define UnlockOperation					m_pOperation->unlockOperation
#define NewOperation					m_pOperation->newOperation
#define FindOperation					m_pOperation->findOperation
#define DeleteOperation					m_pOperation->deleteOperation
#define ClearOperation					m_pOperation->clearOperation
#define SendCommand						m_pOperation->sendCommand

typedef struct	_tagONDEMANDPARAM
{
        HANDLE          endi;					// End Device Handle
		EUI64			id;						// ID
        char            szId[16+1];             // ID String
		void			*pParser;				// Parser
        UINT            nParser;                // Parser Number

		BYTE			nOperation;				// Operation Type
		int             nOption;				// Option
		int				nOffset;
		int				nCount;
        void            *pOndemandCallData;     // 검침을 위한 Callback Data (ONDEMANDPARAM을 Free 해도 자동 Free 되지 않는다)

        CChunk          *pLpChunk;              // LP를 저장할 Chunk
		CChunk			*pMtrChunk;				// Bypass Metering에 사용될 Chunk
		char			*pszBuffer;				// 수신 버퍼로 사용
        void            *pBypassData;           // Bypass에서 사용할 User Data 
		int				nLength;				// Buffer를 사용한 길이
        int             nSize;                  // Buffer의 원래 크기

		BOOL			bReply;					// 수신 여부
		HANDLE			hEvent;					// Event Handle
		int				nResult;				// Result Code
        BOOL            bMeterConstants;        // Meter Constant 지원 여부
        BOOL            bVoltageScale;          // Voltage Scale 지원 여부

		int				nMeteringType;          // Bypass 또는 특별한 Metering 인지 여부
		int				nLastError;
		BYTE			nFrameState;
		WORD			nFrameLength;

		BOOL			bConnected;				// 온라인 상태
		int				nState;			        // 검침 진행 상태
		int				nRetry;					// 재시도 횟수

		BOOL			bTimesync;
		BOOL			bEventLog;

		METER_STAT		stat;

		TIMESTAMP		tmLastMetering;			// 마지막 검침 시간
		TIMESTAMP		tmLastEventLog;			// 마지막 이벤트 로그 시간
	
		TIMETICK      	tmStart;
		TIMETICK      	tmEnd;
		TIMETICK      	tmConnect;
		TIMETICK      	tmSend;
		TIMETICK      	tmRecv;		

        // Meter 정보 Issue #84
        BYTE            szMeterSerial[32];      // Meter Serial
        double          hwVersion;              // Hardware Version
        double          swVersion;              // Software Version
        int             nVendor;

        // Current Pulse
        UINT            nCurPulse;         

        // Metering Table 사용
        int             nIndex;                 // Table을 이용할 경우 사용할 Index

		// NURI Table 정보
		BYTE			nSensorError;			// 센서 에러 코드
		BYTE			nMeterError;			// 미터 에러 코드

        // Meter 상수
        WORD            nMeterConstant;         // Meter 상수
        // Energy Level
        BYTE            nEnergyLevel;
        BYTE            nActivate;              // Meter Relay Activate Status
        BYTE            nRelayCtrlStep;         // Relay Control Step

		POSITION		pos;

        //MBUS Variable
        int             szMbusSeq;
        char            pszMbusSerial[10];

		DATASTREAM		*pDataStream;			// Stream Buffer

        // DLMS
        BOOL            bAuthFlag;              // DLMS Auth Flag
        BYTE            nPartial;               // Partial Count
        BYTE            lastTxControl;
        BYTE            lastRxControl;
        BOOL            bPartialWait;
        BOOL            bSegmentation;          // Segmentation Flag

        /** Session
         * 
         * ANSI Protocol Library에서 최초로 쓰이고 Protocol Library와 관련된 코드들은 ONDEMANDPARAM의 Attribute로
         * 모두 Mapping 해서 쓰지 않고 Session으로 연결해서 사용해야 한다. 
         *  대상 : DLMS, MBUS, KAMSTRUP
         */
        void            *pSession;

}	ONDEMANDPARAM;

class COperation
{
public:
	COperation();
	virtual ~COperation();

public:
	int		GetWorkerCount();

public:
	void	lockOperation();
	void	unlockOperation();

	ONDEMANDPARAM *newOperation(BYTE nOperation, EUI64 *id, int nBufferSize);
	ONDEMANDPARAM *findOperation(EUI64 *id);
	void	deleteOperation(ONDEMANDPARAM *pOndemand);
	void	clearOperation(ONDEMANDPARAM *pOndemand);

	BOOL	sendCommand(ONDEMANDPARAM *pOndemand, const EUI64 *id, const char *pszCommand, int nLength=0);

protected:
	CLocker		m_Locker;
	CLinkedList<ONDEMANDPARAM *>	m_List;
    CDataStream m_DataStream;
};

extern COperation	*m_pOperation;

#endif	// __OPERATION_H__
