#ifndef __OPERATION_H__
#define __OPERATION_H__

#include "LinkedList.h"
#include "Locker.h"
#include "GeMeter.h"
#include "Event.h"

#define ONDEMAND_OPR_READ				1
#define ONDEMAND_OPR_WRITE				2
#define ONDEMAND_OPR_DATA				3
#define ONDEMAND_OPR_CHECK				10	

#define METER_TYPE_AIDON				0
#define METER_TYPE_GE					1

#define ONDEMAND_ERROR_OK				0				// 명령 정상 처리
#define ONDEMAND_ERROR_FAIL				1				// 명령 처리 실패
#define ONDEMAND_ERROR_BUSY				2				// 이미 센서가 통신중
#define ONDEMAND_ERROR_NOT_CONNECT		3				// 접속 실패
#define ONDEMAND_ERROR_PARAM			4				// 파라메터 오류

#define LockOperation					m_pOperation->lockOperation
#define UnlockOperation					m_pOperation->unlockOperation

#define NewOperation					m_pOperation->newOperation
#define FindOperation					m_pOperation->findOperation
#define DeleteOperation					m_pOperation->deleteOperation
#define ClearOperation					m_pOperation->clearOperation

#define SendCommand						m_pOperation->sendCommand

typedef struct	_tagONDEMANDPARAM
{
		HANDLE			endi;
		EUI64			id;
		BYTE			nGroup;
		BYTE			nMember;
		BYTE			nOperation;
		int				nInterval;
		BYTE			nPage;
		char			*pszBuffer;
		int				nLength;
		BOOL			bReply;
		BOOL			bResult;
		BOOL			bOpen;
		HANDLE			hEvent;
		int				nMeteringType;
		int				nType;
		int				nLastError;
		POSITION		pos;
		BYTE			nFrameState;
		WORD			nFrameLength;
		void			*pParser;

		TIMESTAMP		tmLastMetering;
		TIMETICK        tmStart;
		TIMETICK        tmEnd;
		TIMETICK        tmConnect;
		TIMETICK        tmSend;
		TIMETICK        tmRecv;		

		// GE Meter Variable
		ANSI_CONFIG		ansi;
		METER_CONFIG	config;
		METER_STAT		stat;
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

	ONDEMANDPARAM *newOperation(BYTE nOperation, EUI64 *id, BYTE nPage=0, char *pszBuffer=NULL, BYTE nLength=0);
	ONDEMANDPARAM *findOperation(EUI64 *id);
	void	deleteOperation(ONDEMANDPARAM *pOndemand);
	void	clearOperation(ONDEMANDPARAM *pOndemand);

	BOOL	sendCommand(ONDEMANDPARAM *pOndemand, EUI64 *id, const char *pszCommand, int nLength=0);

protected:
	CLocker		m_Locker;
	CLinkedList<ONDEMANDPARAM *>	m_List;
};

extern COperation	*m_pOperation;

#endif	// __OPERATION_H__
