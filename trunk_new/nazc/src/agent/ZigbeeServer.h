#ifndef __ZIGBEE_SERVER_H__
#define __ZIGBEE_SERVER_H__

#include "Locker.h"
#include "LinkedList.h"
#include "TimeoutThread.h"
#include "ZigbeeWorker.h"
#include "Chunk.h"
#include "Event.h"

#define REMOTE_SESSION_TIMEOUT			60		// 60초
#define MAX_REMOTE_WORKER				8		// 최대 4개가 동시에 접속 가능

typedef struct	_tagZBSESSION
{
		char		szGUID[17];					// 센서 ID 문자열
		HANDLE		codi;						// HANDLE
		EUI64		id;							// 센서 ID
        BYTE        reqid;                      // Request ID
		BYTE		nSeq;						// 시퀀스 번호
		BYTE		nLastRecvSeq;				// 마지막 수신 SEQ (최초 0xFF 설정)
		int			nState;						// 현재 상태
		void 		*pWorker;					// Zigbee worker
		void 		*pServer;					// Zigbee server
		void		*pClient;					// TCP Client
		int			nPort;						// 포트 번호
		int			nRetry;						// 재시도 횟수
		CChunk		*pChunk;					// 수신 버퍼
		CChunk		*pResChunk;					// 응답 버퍼
		BOOL		bExitPending;				// 종료 요청
		BOOL		bResponse;				    // 응답
		HANDLE		hEvent;						// Event
		BOOL		bError;						// Error 상태
		BOOL		bDone;						// 작업 종료 Flag
		time_t		tmConnect;					// 연결 시간
		time_t		tmLastKeepalive;			// 마지막 접속 유지 전송 시간
		time_t		tmLastRecv;					// 마지막 수신 시간
		int			nPosition;					// 리스트 위치
}	ZBSESSION, *PZBSESSION;

class CZigbeeServer : public CTimeoutThread
{
public:
	CZigbeeServer();
	virtual ~CZigbeeServer();

// Operations
public:
    BOOL    Initialize();
    void    Destroy();

	CZigbeeWorker *StartWorker(void *pJob);
	void	ResetWorker();
	int		GetWorker();
	BYTE	GetSessionSeq(ZBSESSION *pSession);

    BOOL    SendResult(ZBSESSION *pSession, char * szResult, int nLength);

public:
	static	void StartSession(void *pThis, void *pWorker, void *pJob);
	static	void EndSession(void *pThis, void *pWorker, void *pJob);
	static	void RunSession(void *pThis, void *pWorker, void *pJob);

protected:
	void	SingleSession(CZigbeeWorker *pWorker, ZBSESSION *pJob);
	ZBSESSION *NewSession(HANDLE codi, EUI64 *id, BYTE reqid);
	void 	DeleteSession(ZBSESSION *pSession);
	ZBSESSION *FindSession(EUI64 *id, BYTE reqid);

    void    NanDataProcessing(ZBSESSION *pSession, HANDLE codi, EUI64 *id, BYTE seq, BYTE *data, int nLength);
    BOOL    _SendResult(ZBSESSION *pSession);

public:
    void    OnNanData(HANDLE codi, EUI64 *id, BYTE reqid, BYTE tailframe, BYTE seq, BYTE *data, int nLength);

protected:
	BOOL	OnTimeoutThread();
	BOOL	OnActiveThread();

private:
    CLocker		m_Locker;
	CLocker		m_SeqLocker;
	CLocker		m_WorkerLocker;
	int			m_nActiveWorker;
	CLinkedList<ZBSESSION *>	m_List;
	CZigbeeWorker	m_Worker[MAX_REMOTE_WORKER];
};

extern CZigbeeServer	*m_pZigbeeServer;

#endif	// __ZIGBEE_SERVER_H__
