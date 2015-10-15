#include "common.h"
#include "codiapi.h"
#include "ZigbeeServer.h"
#include "TelnetClient.h"
#include "InventoryScanner.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"
#include "MD5.h"
#include "site.h"
#include "if4frame.h"
#include "Utility.h"
#include "CommonUtil.h"

//////////////////////////////////////////////////////////////////////
// CZigbeeServer Data Definitions
//////////////////////////////////////////////////////////////////////

#define MTPERR_CONNECTION_REFUSE		20
#define MTPERR_NEED_SESSION_KEY			21
#define MTPERR_INVALID_SESSION_KEY		22
#define MTPERR_INSTALL_START        	100
#define MTPERR_INSTALL_END          	101
#define MTPERR_COMMAND_OK           	200
#define MTPERR_TELNET_ACCEPT        	210
#define MTPERR_TELNET_CONNECT       	211
#define MTPERR_TELNET_DISCONNECT    	212
#define MTPERR_GET_ACCEPT           	220
#define MTPERR_PUT_ACCEPT           	230
#define MTPERR_LIST_ACCEPT          	240
#define MTPERR_NACS_ACCEPT          	250
#define MTPERR_INVALID_REQUEST      	300
#define MTPERR_COMMAND_ERROR        	310

CZigbeeServer 	*m_pZigbeeServer = NULL;
extern UINT m_nLocalPort;

//////////////////////////////////////////////////////////////////////
// CZigbeeServer Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZigbeeServer::CZigbeeServer()
{
	m_pZigbeeServer	= this;
}

CZigbeeServer::~CZigbeeServer()
{
}

//////////////////////////////////////////////////////////////////////
// CZigbeeServer Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CZigbeeServer::Initialize()
{
	int		i;

	// Worker를 생성한다.
	for(i=0; i<MAX_REMOTE_WORKER; i++)
		m_Worker[i].Initialize();

	// Timeout 처리 루틴을 실행한다.
	if (!CTimeoutThread::StartupThread(1))
		return FALSE;

	return TRUE;
}

void CZigbeeServer::Destroy()
{
	int		i;

	// Timeout 처리 루틴을 종료햔다.
	CTimeoutThread::ShutdownThread();

	// Worker를 모두 종료한다.
	for(i=0; i<MAX_REMOTE_WORKER; i++)
		m_Worker[i].Destroy();
}

//////////////////////////////////////////////////////////////////////
// CZigbeeServer Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZigbeeWorker *CZigbeeServer::StartWorker(void *pJob)
{
	int		nIndex;

	nIndex = GetWorker();
	if (nIndex == -1)
		return NULL;

	m_Worker[nIndex].StartJob(this, (void *)this, StartSession, EndSession, RunSession, pJob);	
	return &m_Worker[nIndex];
}

void CZigbeeServer::ResetWorker()
{
	m_WorkerLocker.Lock();
	m_nActiveWorker = 0;
	m_WorkerLocker.Unlock();
}

int CZigbeeServer::GetWorker()
{
	int		i;

	for(i=0; i<MAX_REMOTE_WORKER; i++)
	{
		if (!m_Worker[i].IsActive())
			return i;
	}
	return -1;	
}

//////////////////////////////////////////////////////////////////////
// CZigbeeServer Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CZigbeeServer::StartSession(void *pThis, void *pWorker, void *pJob)
{
	CZigbeeServer	*pSelf = (CZigbeeServer *)pThis;

	pSelf->m_WorkerLocker.Lock();
	pSelf->m_nActiveWorker++;
	pSelf->m_WorkerLocker.Unlock();
}

void CZigbeeServer::EndSession(void *pThis, void *pWorker, void *pJob)
{
	CZigbeeServer	*pSelf = (CZigbeeServer *)pThis;

	pSelf->m_WorkerLocker.Lock();
	pSelf->m_nActiveWorker--;
	pSelf->m_WorkerLocker.Unlock();
}

void CZigbeeServer::RunSession(void *pThis, void *pWorker, void *pJob)
{
	CZigbeeServer	*pSelf = (CZigbeeServer *)pThis;
	ZBSESSION		*pSession = (ZBSESSION *)pJob;

    XDEBUG("RunSession\r\n");
	pSelf->SingleSession((CZigbeeWorker *)pWorker, pSession);

    /*
    if(pSession->bResponse)
    {
        pSelf->_SendResult(pSession);
    }
    */

	if(pSession->bDone)
    {
        XDEBUG("RunSession:DeleteSession\r\n");
	    pSelf->m_Locker.Lock();
	    pSelf->DeleteSession(pSession);
	    pSelf->m_Locker.Unlock();
    }
}

void CZigbeeServer::SingleSession(CZigbeeWorker *pWorker, ZBSESSION *pSession)
{
	CTelnetClient	*pClient;

	// 주 처리 부분
	for(;!pSession->bExitPending;)
	{
		ResetEvent(pSession->hEvent);
		WaitForSingleObject(pSession->hEvent, 1000);
	}

	// 세션이 하위 Client를 가지고 있으면 제거한다.
	if (pSession->pClient != NULL)
	{
		pClient = (CTelnetClient *)pSession->pClient;
		pSession->pClient = (void *)NULL;
		pClient->Destroy();
		delete pClient;
	}
}

#if 0
BOOL CZigbeeServer::_Open(ZBSESSION *pSession)
{
	int		nError = IF4ERR_NOERROR;
    HANDLE endi;

    endiOpen(pSession->codi, &endi, &pSession->id);

    sensorConnect(endi, 3000);

	// Binding을 맺는다.
	nError = endiBind(pSession->codi, &pSession->id, CODI_BIND_SET_BIND, 3000);
	if (nError != CODIERR_NOERROR)
	{
		XDEBUG("endiBind(SetBind) = %d\r\n", nError);
        return FALSE;
    }
	// 센서에 접속한다.
    nError = endiLink(pSession->codi, &pSession->id, 0, ENDI_LINKOPEN_RESPONSE, 3000);
	if (nError != CODIERR_NOERROR)
	{
	    XDEBUG("endiLink Link Response = %s(%d)\r\n", codiErrorMessage(nError), nError);
	    endiBind(pSession->codi, &pSession->id, CODI_BIND_DELETE_BIND, 3000);
        return FALSE;
	}
    return TRUE;
}
#endif

BOOL CZigbeeServer::_SendResult(ZBSESSION *pSession)
{
	int		nError = IF4ERR_NOERROR;
    ENDI_NAN_PAYLOAD payload;
    int i, j, len;
    int     nMax = 60;
    int     nLength;
    char * szResult = NULL;
    HANDLE endi;

    nLength = pSession->pResChunk->GetSize();
    szResult = pSession->pResChunk->GetBuffer();

    if(endiOpen(pSession->codi, &endi, &pSession->id)) return FALSE;

	// Binding을 맺는다.
	nError = endiBind(pSession->codi, &pSession->id, CODI_BIND_SET_BIND, 3000);
	if (nError != CODIERR_NOERROR && nError != CODIERR_BIND_ADDRESS_DUPLICATION)
	{
		XDEBUG("endiBind(SetBind) = %d\r\n", nError);
        goto end;
    }
	// 센서에 접속한다.
    nError = endiLink(pSession->codi, &pSession->id, 0, ENDI_LINKOPEN_REQUEST, 3000);
	if (nError != CODIERR_NOERROR)
	{
	    XDEBUG("endiLink Link Response = %s(%d)\r\n", codiErrorMessage(nError), nError);
	    endiBind(pSession->codi, &pSession->id, CODI_BIND_DELETE_BIND, 3000);
        goto end;
	}

    for(i=0,j=0;i<nLength;i+=nMax,j++)
    {
        memset(&payload, 0, sizeof(ENDI_NAN_PAYLOAD));
        len = MIN(nLength-i, nMax);

        payload.trid = pSession->reqid;
        payload.ctrl |= 0x80;
        payload.ctrl |= ((i+nMax) < nLength ? 0x00 : 0x40); 
        payload.ctrl |= (j & 0x3F); 
        memcpy(payload.data, szResult + i, len);
        /** @todo Timeout이 Router Device에 맞추어져 있다. 각 Device 별로 맞는
          *     Timeout 값을 사용하도록 수정할 필요가 있다.
          */
	    endiWrite((HANDLE)pSession->codi, (EUI64 *)&pSession->id, 
			m_pZigbeeServer->GetSessionSeq(pSession),
			ENDI_DATATYPE_NAN, (BYTE *)&payload, len+2, 3000);
        /** Issue #21
          * IHD에서 NAN Frame을 전송받을 때 serial에서 byte loss가 발생하고 있기 때문에
          * delay를 추가해 본다.
          */
        //USLEEP(100000);
    }

    // Link Close
    nError = endiLink(pSession->codi, &pSession->id, 0, ENDI_LINKCLOSE_REQUEST, 3000);

	// Binding을 삭제한다.
	endiBind(pSession->codi, &pSession->id, CODI_BIND_DELETE_BIND, 3000);

end:
    //endiDisconnect(endi);
    endiClose(endi);

    //m_Locker.Lock();
	pSession->bDone = TRUE;
    pSession->bExitPending = TRUE;
    pSession->bResponse = FALSE;
    pSession->pResChunk->Flush();
    SetEvent(pSession->hEvent);
    //m_Locker.Unlock();

    return TRUE;
}

#if 0
BOOL CZigbeeServer::_Close(HANDLE codi, EUI64 * id)
{
    HANDLE endi = endiGetHandle(id);
	//endiBind(codi, id, CODI_BIND_DELETE_BIND, 3000);
    endiDisconnect(endi)
    endiClose(endi)
    return TRUE;
}
#endif

BOOL CZigbeeServer::SendResult(ZBSESSION *pSession, char * szResult, int nLength)
{
    if(pSession)
    {
        pSession->pResChunk->Add(szResult, nLength);
    }
    m_Locker.Lock();
	pSession->bResponse = TRUE;
    m_Locker.Unlock();

    OnActiveThread();
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CZigbeeServer Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZBSESSION *CZigbeeServer::NewSession(HANDLE codi, EUI64 *id, BYTE reqid)
{
	ZBSESSION	*pSession;

	pSession = (ZBSESSION *)MALLOC(sizeof(ZBSESSION));
	if (pSession == NULL)
		return NULL;

	memset(pSession, 0, sizeof(ZBSESSION));
	memcpy(&pSession->id, id, sizeof(EUI64));
	eui64toa(id, pSession->szGUID);
	time(&pSession->tmLastRecv);

	pSession->codi			= codi;
	pSession->nSeq 			= 0;
    pSession->reqid         = reqid;
	pSession->nLastRecvSeq 	= 0xFF;
	pSession->bExitPending  = FALSE;
	pSession->hEvent		= CreateEvent(NULL, FALSE, FALSE, NULL);
	pSession->bDone			= FALSE;
	pSession->bResponse		= FALSE;
	pSession->pWorker		= (void *)NULL;
	pSession->pServer		= (void *)this;
	pSession->pClient		= new CTelnetClient;
	pSession->nPort         = m_nLocalPort;
    pSession->bError        = FALSE;
	pSession->pChunk		= new CChunk(512);
	pSession->pResChunk		= new CChunk(512);
	pSession->nPosition 	= (int)m_List.AddTail(pSession);

    XDEBUG("Create New Session:reqid = %02X\r\n", reqid);

	return pSession;
}

void CZigbeeServer::DeleteSession(ZBSESSION *pSession)
{
    XDEBUG("Before Remove m_List.Size()=%d\r\n", m_List.GetCount());
	// 리스트에서 삭제한다.
	m_List.RemoveAt((POSITION)pSession->nPosition);
    XDEBUG("After Remove m_List.Size()=%d\r\n", m_List.GetCount());

	// 이벤트를 삭제한다.
	if (pSession->hEvent != NULL)
		CloseHandle(pSession->hEvent);

	// 수신 버퍼를 비운다.
	if (pSession->pChunk != NULL)
		delete pSession->pChunk;

	if (pSession->pResChunk != NULL)
		delete pSession->pResChunk;

	// 메모리를 반환한다.
	FREE(pSession);
}

BYTE CZigbeeServer::GetSessionSeq(ZBSESSION *pSession)
{
	BYTE	seq;

	m_SeqLocker.Lock();
	pSession->nSeq++;
	seq = pSession->nSeq;
	m_SeqLocker.Unlock();
	return seq;
}

ZBSESSION *CZigbeeServer::FindSession(EUI64 *id, BYTE reqid)
{
	ZBSESSION	*pSession;
	POSITION	pos=INITIAL_POSITION;

	pSession = m_List.GetFirst(pos);
	while(pSession != NULL)
	{
		if (memcmp(&pSession->id, id, sizeof(EUI64)) == 0 && pSession->reqid == reqid)
			return pSession;

		pSession = m_List.GetNext(pos);
	}
	return pSession;
}

void CZigbeeServer::OnNanData(HANDLE codi, EUI64 *id, BYTE reqid, BYTE tailframe, BYTE seq, BYTE *data, int nLength)
{
	ZBSESSION	*pSession;
	CTelnetClient	*pClient;

    XDEBUG("OnNanData reqid=%d tailframe=%c seq=%d\r\n", reqid, tailframe ? 'T' : 'F', seq);

	m_Locker.Lock();
	pSession = FindSession(id, reqid);
	if (pSession == NULL) 
    {
        pSession = NewSession(codi, id, reqid);
		if (pSession != NULL)
        {
            time(&pSession->tmConnect);
            time(&pSession->tmLastKeepalive);
            time(&pSession->tmLastRecv);	
            pSession->pWorker = (void *)StartWorker((void *)pSession);
        }
    }
    if(pSession) NanDataProcessing(pSession, codi, id, seq, data, nLength);
	m_Locker.Unlock();

    if(!pSession) return;

    if(tailframe && !pSession->bError) 
    {
		pClient = (CTelnetClient *)pSession->pClient;
		if (pClient != NULL)
        {
			pClient->Initialize((void *)pSession, pSession->nPort);
		    if (pClient->NewConnection())
            {
			    //XDUMP((char *)pSession->pChunk->GetBuffer(), pSession->pChunk->GetSize(), TRUE);
			    pClient->SendToHost((char *)pSession->pChunk->GetBuffer(), pSession->pChunk->GetSize(), 0);
            }
        }
    }

}

void CZigbeeServer::NanDataProcessing(ZBSESSION *pSession, HANDLE codi, EUI64 *id, BYTE seq, BYTE *data, int nLength)
{
    BYTE reqSeq = ((BYTE)pSession->nLastRecvSeq+1) & 0x3F;

	time(&pSession->tmLastRecv);

	// 프레임 검사
	if (reqSeq != seq)
	{
        /** @todo Error 상태를 셋팅하고 리턴해야 한다 
          */
        pSession->bError = TRUE;
		XDEBUG(" ZigbeeServer: Sequence error!! (Last=%d, Cur=%d)!!\r\n", reqSeq, seq);
		XDUMP((char *)data, nLength, TRUE);
		return;
	}
    pSession->bError = FALSE;
	pSession->nLastRecvSeq = seq;
	pSession->pChunk->Add((char *)data, nLength);
}

//////////////////////////////////////////////////////////////////////
// CZigbeeServer Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CZigbeeServer::OnActiveThread()
{
	ZBSESSION	*pSession, *pDelete;
	time_t		now;
	POSITION	pos;

	m_Locker.Lock();
	pSession = m_List.GetFirst(pos);
	while(pSession)
	{
		time(&now);
        if(pSession->bResponse)
        {
            _SendResult(pSession);
            pSession = m_List.GetNext(pos);
            continue;
        }

		pDelete = NULL;
		if ((now-pSession->tmLastRecv) > REMOTE_SESSION_TIMEOUT)
			pDelete = pSession;

		pSession = m_List.GetNext(pos);
		if (pDelete != NULL)
		{
			// 스스로 끝나도록 요청한다.
			XDEBUG("ZIGBEE-SERVER: %s Session timeout. size=%d\r\n", pDelete->szGUID, m_List.GetCount());
			pDelete->bExitPending = TRUE;
			pDelete->bDone = TRUE;
			SetEvent(pDelete->hEvent);
		}
	}
	m_Locker.Unlock();

	return TRUE;
}

BOOL CZigbeeServer::OnTimeoutThread()
{
	if (m_List.GetCount() == 0)
		return TRUE;

	return OnActiveThread();
}

