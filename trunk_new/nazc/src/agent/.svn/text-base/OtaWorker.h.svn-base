#ifndef __OTA_WORKER_H__
#define __OTA_WORKER_H__

#include "TimeoutThread.h"
#include "Event.h"
#include "Locker.h"
#include "Chunk.h"

#define OTA_TRANSFER_AUTO				0
#define OTA_TRANSFER_MULTICAST			1
#define OTA_TRANSFER_UNICAST			2

#define OTA_INSTALL_AUTO				0x00
#define OTA_INSTALL_REINSTALL			0x01
#define OTA_INSTALL_MATCH				0x02
#define OTA_INSTALL_FORCE               0x03

#define OTA_STEP_INIT                   0x00
#define OTA_STEP_INVENTORY				0x01
#define OTA_STEP_SEND					0x02
#define OTA_STEP_VERIFY					0x04
#define OTA_STEP_INSTALL				0x08
#define OTA_STEP_SCAN					0x10
#define OTA_STEP_ALL			   		(OTA_STEP_INVENTORY | OTA_STEP_SEND | OTA_STEP_VERIFY | \
										OTA_STEP_INSTALL | OTA_STEP_SCAN)

#define OTAERR_NOERROR					0				// 정상
#define OTAERR_OPEN_ERROR				1				// 센서 OPEN 오류
#define OTAERR_CONNECT_ERROR			2				// 접속 오류
#define OTAERR_INVENTORY_SCAN_ERROR		3				// Inventory 수집 오류
#define OTAERR_FIRMWARE_NOT_FOUND		4				// Firmware가 존재하지 않음
#define OTAERR_SEND_ERROR				5				// 전송 오류
#define OTAERR_VERIFY_ERROR				6				// Verify 오류
#define OTAERR_INSTALL_ERROR			7				// Install 오류
#define OTAERR_USER_CANCEL				8				// 사용자가 취소한 경우
#define OTAERR_DIFFERENT_MODEL_ERROR	9				// 모델명이 다른 경우
#define OTAERR_MATCH_VERSION_ERROR		10				// 같은 버젼인 경우
#define OTAERR_VERSION_ERROR			11				// 펌웨어 버젼 에러
#define OTAERR_MODEL_ERROR				12				// 센서 모델 에러
#define OTAERR_MEMORY_ERROR				30				// 메모리 에러
#define OTAERR_COORDINATOR_ERROR		40				// 코디네이터 에러

typedef struct	_tagOTANODE
{
		EUI64	id;						// EUI64 ID
		char	szModel[21];			// 센서 Model 명칭
		BYTE	hw;						// H/W Version
		BYTE	sw;						// S/W Version
		BYTE	build;					// Build Version
		BYTE	hwNew;					// Scan H/W Version
		BYTE	swNew;
		BYTE	buildNew;

		UINT	nOtaStep;				// 진행할 작업
		UINT	nOtaMask;				// 이미 완료한 작업	
		BYTE	nHops;					// 전송시 HOP
		UINT	nElapse;				// 소요 시간 (전송 시간)
		int		nTotal;					// 전체 전송 바이트
		int		nCurrent;				// 현재 전송 바이트
		int		nState;					// 현재 진행 상태
		int		nError;					// 에러 코드
		int		nConnectionTimeout;		// 접속 타임아웃
		int		nReplyTimeout;			// 응답 타임아웃

		BOOL	bActive;				// 업데이트 진행중
		BOOL	bSkip;					// 더이상 진행하지 않는 경우
		BOOL	bDone;					// 작업 완료 플래그
		BOOL	bExitPending;			// 중지 플래그

		time_t	tmStart;				// 최근 시작 시간
		time_t	tmEnd;					// 최근 종료 시간
}	OTANODE;

typedef struct	_tagOTAENTRY
{
		char	szTriggerId[17];				// Trigger ID
		BYTE	nType;							// OTA Type (0:Concentrator, 1:Sensor, 2:Coordinator)
		char	szModel[21];					// Model name
        BYTE    nDestType;                      // Dest CPU (0 : EM250, 1 : SUB CPU)
		char	szDestParserName[21];			// Sub Model name for Group
        BYTE	nTransferType;					// Transfer Type(0:Auto, 1:Multicast, 2:Unicast)
		BYTE	nInstallType;					// Install Type(0:Auto, 1:Reinstall, 2:Match)
		BYTE	nOtaStep;						// OTA를 진행할 단계
		BYTE	nWriteCount;					// Multicast 반복 횟수
		UINT	nRetry;							// 최대 재시도 횟수
		BYTE	nThread;						// 쓰레드 갯수
		WORD	nOldHw;							// 구 하드웨어 버젼
		WORD	nOldSw;							// 구 소프트웨어 버젼
		WORD	nOldBuild;						// 구 빌드 버젼
		WORD	nNewHw;							// 신 하드웨어 버젼
		WORD	nNewSw;							// 신 소프트웨어 버젼
		WORD	nNewBuild;						// 신 빌드 버젼

        char	szBinURL[256];					// 바이너리 URL
		char	szBinChecksum[33];				// 바이너리 Checksum
		char	szDiffURL[256];					// DIFF 파일 URL
		char	szDiffChecksum[33];				// DIFF 파일 Checksum

        WORD    nFWCRC;                         // Firmware CRC
        UINT    nFWLen;                         // Firmware Length

        EUI64	*pIdList;						// 센서 ID List (명령에서 파라메터로 전달된)
		int		nIdCount;						// 센서 갯수
		OTANODE	*pNode;							// 하위 노드 (실제 OTA 대상)
		int		nNodeCount;						// 하위 노드 갯수
		char	szFile[256];					// Install 작업을 할 파일

		time_t	tmStart;
		UINT	nStartTick;
		UINT	nTotal;
		UINT	nComplete;
		UINT	nFail;
		UINT	nState;
		BYTE	nOtaCurrentStep;				// 현재 OTA 단계

		BOOL	bActive;
		BOOL	bExitPending;
		int		nPosition;
}	OTAENTRY;

typedef struct	_tagOTASTATE
{
		GMTTIMESTAMP	tmStart;		// 시작 시간
		UINT	nElapse;				// 경과 시간
		UINT	nTotal;					// 총 대상
		UINT	nComplete;				// 성공 갯수
		UINT	nFail;					// 실패 갯수
		BYTE	nState;					// 현재 상태
		UINT	nOtaStep;				// OTA 진행 상태
}	OTASTATE;

class COtaWorker : public CTimeoutThread
{
public:
	COtaWorker();
	virtual ~COtaWorker();

// Operations
public:
    BOOL   	Initialize();
    void   	Destroy();

	BOOL	IsBusy();
	BOOL	SensorOta(OTAENTRY *pJob, OTANODE *pNode, CChunk *pChunk, HANDLE hEvent);

// OTA procedure
protected:
    BOOL    SingleOta(OTAENTRY *pJob, OTANODE *pNode, CChunk *pChunk, BOOL &bExit);

// Thread override functions
protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

// Member variables
private:
	BOOL		m_bBusy;
	OTAENTRY	*m_pJob;
	OTANODE		*m_pNode;
	CChunk		*m_pChunk;
	HANDLE		m_hEvent;
	CLocker		m_Locker;
};

#endif	// __OTA_WORKER__
