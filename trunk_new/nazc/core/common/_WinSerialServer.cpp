#include "stdafx.h"
#include <time.h>
#include "SerialServer.h"
#include "DebugUtil.h"
#include "CommonUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CSerialServer Data Definition
//////////////////////////////////////////////////////////////////////

// ASCII Definitions

#define ASCII_BEL				0x07
#define ASCII_BS				0x08
#define ASCII_LF				0x0A
#define ASCII_CR				0x0D
#define ASCII_XON				0x11
#define ASCII_XOFF				0x13

// Write request types

#define WRITE_CHAR				0x01
#define WRITE_FILE				0x02
#define WRITE_FILESTART			0x03
#define WRITE_FILEEND			0x04
#define WRITE_ABORT				0x05
#define WRITE_BLOCK				0x06

// Global Definitions

#define MAX_WRITE_BUFFER        1024
#define MAX_READ_BUFFER         4096
#define READ_TIMEOUT            500
#define STATUS_CHECK_TIMEOUT    500
#define PURGE_FLAGS             PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR
#define EVENTFLAGS_DEFAULT      EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING | EV_RLSD

COMMTIMEOUTS	m_TimeoutsDefault = { 50, 0, 0, 0, 0 };

//////////////////////////////////////////////////////////////////////
// CSerialServer Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialServer::CSerialServer()
{
	m_hComPort			= INVALID_HANDLE_VALUE;
	m_dwParam			= 0;
	m_dwOldModemStatus	= 0;
	m_dwModemStatus		= 0;
	m_dwErrorsOld		= 0;
	m_dwErrors			= 0;
	*m_szDevice			= '\0';

	m_bRecvReady		= FALSE;
	m_bConnected		= FALSE;
	m_bDisconnectPending= FALSE;
	m_bUseFlowControl	= FALSE;
	m_nKeepAliveTime	= INFINITE;
	m_pSaveSession		= NULL;

	memset(&m_ComStatOld, 0, sizeof(COMSTAT));
	memset(&m_ComStatNew, 0, sizeof(COMSTAT));
}

CSerialServer::~CSerialServer()
{
}

//////////////////////////////////////////////////////////////////////
// CSerialServer Attribute
//////////////////////////////////////////////////////////////////////

BOOL CSerialServer::IsConnected() const
{
	return m_bConnected;
}

int CSerialServer::GetError() const
{
	return m_nLastError;
}

int CSerialServer::GetHandle()
{
	return (int)m_hComPort;
}

void CSerialServer::SetPortParam(DWORD dwParam)
{
	m_dwParam = dwParam;
}

DWORD CSerialServer::GetPortParam() const
{
	return m_dwParam;
}

int CSerialServer::SetKeepAliveTime() const
{
	return m_nKeepAliveTime;
}

void CSerialServer::SetKeepAliveTime(int nKeepAliveTime)
{
	m_nKeepAliveTime = nKeepAliveTime;
}

//////////////////////////////////////////////////////////////////////
// CSerialServer Operations
//////////////////////////////////////////////////////////////////////

BOOL CSerialServer::Connect(char *pszDevice, DWORD dwBaudRate, int nParity, int nDataBit, int nStopBit, BOOL bFlowControl)
{
#ifdef WINCE
	CString	strDevice;
#endif
	if (IsConnected())
		return FALSE;

	if (!pszDevice || !*pszDevice)
		return FALSE;

	m_bUseFlowControl = bFlowControl;
	if (!InitializeVariable())
		return FALSE;

	strcpy(m_szDevice, pszDevice);
    m_dwBaudRate		= dwBaudRate;
	m_nByteSize			= nDataBit;
	m_nParity			= nParity;
	m_nStopBits			= nStopBit;
	m_dwOldModemStatus	= 0;
	m_dwModemStatus		= 0;
	m_dwErrorsOld		= 0;
	m_dwErrors			= 0;
	m_nKeepAliveTime	= INFINITE;
	m_bPassiveMode		= FALSE;
	m_bDisconnectPending= FALSE;
	memset(&m_ComStatOld, 0, sizeof(COMSTAT));
	memset(&m_ComStatNew, 0, sizeof(COMSTAT));

#ifdef WINCE
	strDevice = pszDevice;
	m_hComPort = CreateFile(strDevice,
							GENERIC_READ | GENERIC_WRITE, 
							0, 
							0,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
							0);
#else
	m_hComPort = CreateFile(pszDevice,
							GENERIC_READ | GENERIC_WRITE, 
							0, 
							0,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
							0);
#endif

	if (m_hComPort == INVALID_HANDLE_VALUE)
    {
        XDEBUG("[CSerialServer] Invalid Handle Value\r\n");
		return FALSE;
    }

    // Save original comm timeouts and set new ones
    if (!GetCommTimeouts(m_hComPort, &m_timeoutOld))
		XDEBUG("[CSerialServer] GetCommTimeouts.\r\n");

    // Set Port State
    UpdateConnection();

    // Set comm buffer sizes
    SetupComm(m_hComPort, MAX_READ_BUFFER, MAX_WRITE_BUFFER);

    // Raise DTR
    if (!EscapeCommFunction(m_hComPort, SETDTR))
        XDEBUG("[CSerialServer] EscapeCommFunction (SETDTR).\r\n");

	StartThread();
	m_bConnected = TRUE;
	return TRUE;
}

void CSerialServer::Disconnect()
{
	if (!IsConnected())
		return;

	OnCloseSession(m_pSaveSession);

	StopThread();

    // Lower DTR
    if (!EscapeCommFunction(m_hComPort, CLRDTR))
        XDEBUG("[CSerialServer] EscapeCommFunction (CLRDTR).\r\n");

    // Restore Original Comm Timeouts
    if (!SetCommTimeouts(m_hComPort, &m_timeoutOld))
        XDEBUG("[CSerialServer] SetCommTimeouts (Restoration to original).\r\n");

    // Purge reads/writes, input buffer and output buffer
    if (!PurgeComm(m_hComPort, PURGE_FLAGS))
        XDEBUG("[CSerialServer] PurgeComm.\r\n");

    // Restore DCB
    if (!SetCommState(m_hComPort, &m_saveDCB))
		XDEBUG("[CSerialServer] SetCommState.\r\n");

	if (m_hComPort != INVALID_HANDLE_VALUE)
		CloseHandle(m_hComPort);

	DestroyVariable();

	m_bConnected = FALSE;
	m_bDisconnectPending= FALSE;
	m_hComPort = INVALID_HANDLE_VALUE;
}

BOOL CSerialServer::EnableFlowControl(BOOL bEnable)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSerialServer Override Functions
//////////////////////////////////////////////////////////////////////

void CSerialServer::OnError(DWORD dwErrors, LPCOMSTAT *pStatus)
{
}

void CSerialServer::OnUpdateModemStatus(DWORD dwModemStatus)
{
}

void CSerialServer::OnUpdateCommStatus(DWORD dwErrors, COMSTAT *pComStat)
{
}

//////////////////////////////////////////////////////////////////////
// CSerialServer Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CSerialServer::OnEnterSession(WORKSESSION *pSession)
{
	return TRUE;
}

void CSerialServer::OnLeaveSession(WORKSESSION *pSession)
{
}

BOOL CSerialServer::OnReceiveSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength)
{
	return TRUE;
}

void CSerialServer::OnPrevSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength)
{
}

void CSerialServer::OnSendSession(WORKSESSION *pSession, LPSTR pszBuffer, int nLength)
{
}

void CSerialServer::OnErrorSession(WORKSESSION *pSession, int nType)
{
}

void CSerialServer::OnCloseSession(WORKSESSION *pSession)
{
}

BOOL CSerialServer::OnTimeoutSession(WORKSESSION *pSession)
{
	return TRUE;
}

void CSerialServer::OnPrevWrite(LPSTR pszBuffer, int nLength)
{
	OnPrevSendSession(m_pSaveSession, pszBuffer, nLength);
}

void CSerialServer::OnWrite(LPSTR pszBuffer, int nLength)
{
	OnSendSession(m_pSaveSession, pszBuffer, nLength);
}

//////////////////////////////////////////////////////////////////////
// CSerialServer Member Functions
//////////////////////////////////////////////////////////////////////

BOOL CSerialServer::InitializeVariable()
{
	m_bLocalEcho			= FALSE;
	m_bDSROutFlow			= FALSE;
	m_bDSRInFlow			= FALSE;
	m_bXonXoffOutFlow		= FALSE;
	m_bXonXoffInFlow		= FALSE;
	m_bTXafterXoffSent		= FALSE;
	m_bNoReading			= FALSE;
	m_bNoWriting			= FALSE;
	m_bNoEvents				= FALSE;
	m_bNoStatus				= FALSE;
	m_bDisplayTimeouts		= FALSE;
    m_dwBaudRate			= CBR_115200;
	m_nByteSize				= 8;
	m_nParity				= NOPARITY;
	m_nStopBits				= ONESTOPBIT;
    m_wXONLimit				= 0;
	m_wXOFFLimit			= 0;
	m_bCTSOutFlow			= m_bUseFlowControl ? TRUE : FALSE;
	m_dwRtsControl			= m_bUseFlowControl ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE;
	m_dwDtrControl			= DTR_CONTROL_DISABLE; //DTR_CONTROL_ENABLE;
	m_dwEventFlags			= EVENTFLAGS_DEFAULT;
    m_chFlag				= '\n';
	m_chXON					= ASCII_XON;
	m_chXOFF				= ASCII_XOFF;
	m_nLastError			= 0;

	m_TimeoutsDefault.ReadIntervalTimeout = 0xFFFFFFFF;
	m_TimeoutsDefault.ReadTotalTimeoutMultiplier = 0;
	m_TimeoutsDefault.ReadTotalTimeoutConstant = 30;
	m_TimeoutsDefault.WriteTotalTimeoutMultiplier = 0;
	m_TimeoutsDefault.WriteTotalTimeoutConstant = 30;

	m_timeoutNew			= m_TimeoutsDefault;
	
	m_hReadStopEvent		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hReadTerminateEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hSendTerminateEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hReadThread			= NULL;
	m_dwReadThreadID		= 0;
	m_hSendThread			= NULL;
	m_dwSendThreadID		= 0;

	return TRUE;
}

void CSerialServer::DestroyVariable()
{
	CloseHandle(m_hReadStopEvent);
	CloseHandle(m_hReadTerminateEvent);
	CloseHandle(m_hSendTerminateEvent);
	CloseHandle(m_hReadThread);
	CloseHandle(m_hSendThread);

	m_hReadStopEvent		= NULL;
	m_hReadTerminateEvent	= NULL;
	m_hSendStopEvent		= NULL;
	m_hSendTerminateEvent	= NULL;
	m_hSendEvent			= NULL;
	m_hReadThread			= NULL;
	m_hSendThread			= NULL;
}

WORKSESSION *CSerialServer::NewSession(HANDLE hFile, LPSTR pszAddress)
{
	WORKSESSION	*pSession;

	pSession = (WORKSESSION *)malloc(sizeof(WORKSESSION));
	if (!pSession)
		return NULL;

	memset(pSession, 0, sizeof(WORKSESSION));
	pSession->lLastInput = (long)GetTickCount() / 1000;
	pSession->pThis	= (void *)this;
	pSession->hFile	= hFile;
	strcpy(pSession->szAddress, pszAddress);
	return pSession;
}

void CSerialServer::DeleteSession(WORKSESSION *pSession)
{
	if (pSession == NULL)
		return;
	free(pSession);
}

BOOL CSerialServer::UpdateConnection()
{
    DCB		dcb;
    
    // Get Current DCB Settings
	memset(&dcb, 0, sizeof(DCB));
    dcb.DCBlength = sizeof(dcb);

    if (!GetCommState(m_hComPort, &dcb))
		XDEBUG("[CSerialServer] GetCommState.\r\n");

	// Save old DCB
	memcpy(&m_saveDCB, &dcb, sizeof(DCB));

    // Update DCB BaudRate, Data-Bit, Parity, and Stop-Bits size, Event Flags
    dcb.BaudRate			= m_dwBaudRate;
    dcb.ByteSize			= m_nByteSize;
    dcb.Parity				= m_nParity;
    dcb.StopBits			= m_nStopBits;

    // Update Event Flags
	dcb.EvtChar				= (m_dwEventFlags & EV_RXFLAG) ? m_chFlag : '\0';

    // Update Flow Control Settings
    dcb.fDtrControl			= m_dwDtrControl;
    dcb.fRtsControl			= m_dwRtsControl;
    dcb.fOutxCtsFlow		= m_bCTSOutFlow;
    dcb.fOutxDsrFlow		= m_bDSROutFlow;
    dcb.fDsrSensitivity		= m_bDSRInFlow;
    dcb.fOutX				= m_bXonXoffOutFlow;
    dcb.fInX				= m_bXonXoffInFlow;
    dcb.fTXContinueOnXoff	= m_bTXafterXoffSent;
    dcb.XonChar				= m_chXON;
    dcb.XoffChar			= m_chXOFF;
    dcb.XonLim				= m_wXONLimit;
    dcb.XoffLim				= m_wXOFFLimit;

    // DCB Settings Not In The User's Control
//  dcb.fParity				= TRUE;
    dcb.fParity				= FALSE;

    // Set New State
    if (!SetCommState(m_hComPort, &dcb))
		XDEBUG("[CSerialServer] SetCommState.\r\n");

    // Set New Timeouts
    if (!SetCommTimeouts(m_hComPort, &m_timeoutNew))
		XDEBUG("[CSerialServer] SetCommTimeouts.\r\n");

    return TRUE;
}

BOOL CSerialServer::StartThread()
{
	m_bRecvReady = FALSE;
	m_bSendReady = FALSE;

	m_hReadThread	= CreateThread(NULL, 
									4096, 
									(LPTHREAD_START_ROUTINE)ReadThread, 
									(LPVOID)this, 
									CREATE_SUSPENDED,
									&m_dwReadThreadID);
	SetThreadPriority(m_hReadThread, THREAD_PRIORITY_NORMAL);

	m_hSendThread	= CreateThread(NULL, 
									4096, 
									(LPTHREAD_START_ROUTINE)SendThread, 
									(LPVOID)this, 
									CREATE_SUSPENDED,
									&m_dwSendThreadID);
	SetThreadPriority(m_hSendThread, THREAD_PRIORITY_NORMAL);

	ResumeThread(m_hReadThread);
	ResumeThread(m_hSendThread);

	// Send/Receive 루틴이 준비될때까지 대기
	while(!m_bRecvReady || !m_bSendReady) {
		//Sleep(100);
        USLEEP(100);
    }

	// Wait로 진입까지 대기
	//Sleep(100);
    USLEEP(500);
	return TRUE;
}

void CSerialServer::StopThread(int nWaitTimeout)
{
	DWORD	dwExitCode;

	m_bDisconnectPending = TRUE;
	SetEvent(m_hReadStopEvent);

	if (WaitForSingleObject(m_hReadTerminateEvent, nWaitTimeout) == WAIT_TIMEOUT)
	{
		if (GetExitCodeThread(m_hReadThread, &dwExitCode))
		{
			if (dwExitCode == STILL_ACTIVE)
			{
				TerminateThread(m_hReadThread, 0);
				TerminateThread(m_hSendThread, 0);
			}
		}
	}
}

void CSerialServer::ErrorHandler(char *pszText)
{
}

void CSerialServer::CheckModemStatus(BOOL bUpdateNow)
{
    if (!GetCommModemStatus(m_hComPort, &m_dwModemStatus))
        XDEBUG("GetCommModemStatus Error\r\n");

    // Report status if bUpdateNow is true or status has changed
    if (bUpdateNow || (m_dwModemStatus != m_dwOldModemStatus))
        OnUpdateModemStatus(m_dwModemStatus);

    m_dwOldModemStatus = m_dwModemStatus;
}

void CSerialServer::CheckComStat(BOOL bUpdateNow)
{
    BOOL bReport = bUpdateNow;

    if (!ClearCommError(m_hComPort, &m_dwErrors, &m_ComStatNew))
        XDEBUG("ClearCommError\r\n");

    if (m_dwErrors != m_dwErrorsOld)
        bReport	= TRUE;

    if (memcmp(&m_ComStatOld, &m_ComStatNew, sizeof(COMSTAT)))
        bReport = TRUE;
    
    if (bReport)
        OnUpdateCommStatus(m_dwErrors, &m_ComStatNew);

	m_dwErrorsOld	= m_dwErrors;
    m_ComStatOld	= m_ComStatNew;
}

void CSerialServer::ReportStatusEvent(DWORD dwStatus)
{
    BOOL fRING, fRLSD, fRXCHAR, fRXFLAG, fTXEMPTY;
    BOOL fBREAK, fCTS, fDSR, fERR;
#ifdef WINCE
	CString	strMessage;
#else
    char szMessage[80];
#endif

    // Get status event flags.
    fCTS	 = EV_CTS & dwStatus;
    fDSR     = EV_DSR & dwStatus;
    fERR     = EV_ERR & dwStatus;
    fRING    = EV_RING & dwStatus;
    fRLSD    = EV_RLSD & dwStatus;
    fBREAK   = EV_BREAK & dwStatus;
    fRXCHAR  = EV_RXCHAR & dwStatus;
    fRXFLAG	 = EV_RXFLAG & dwStatus;
    fTXEMPTY = EV_TXEMPTY & dwStatus;

	// Construct status message indicating the 
	// status event flags that are set.
#ifdef WINCE
	strMessage  = _T("[CSerialServer] EVENT: ");
    strMessage += fCTS ? _T("CTS ") : _T("");
    strMessage += fDSR ? _T("DSR ") : _T("");
    strMessage += fERR ? _T("ERR ") : _T("");
    strMessage += fRING ? _T("RING ") : _T("");
    strMessage += fRLSD ? _T("RLSD ") : _T("");
    strMessage += fBREAK ? _T("BREAK ") : _T("");
    strMessage += fRXFLAG ? _T("RXFLAG ") : _T("");
    strMessage += fRXCHAR ? _T("RXCHAR ") : _T("");
    strMessage += fTXEMPTY ? _T("TXEMPTY ") : _T("");

	// If dwStatus == NULL, then no status event flags are set.
	// This happens when the event flag is changed with SetCommMask.
    if (dwStatus == 0x0000)
        strMessage += _T("NULL");

    strMessage += _T("\r\n");

    // Queue the status message for the status control
	XDEBUG(strMessage);
#else
	strcpy(szMessage, "[CSerialServer] EVENT: ");
    strcat(szMessage, fCTS ? "CTS " : "");
    strcat(szMessage, fDSR ? "DSR " : "");
    strcat(szMessage, fERR ? "ERR " : "");
    strcat(szMessage, fRING ? "RING " : "");
    strcat(szMessage, fRLSD ? "RLSD " : "");
    strcat(szMessage, fBREAK ? "BREAK " : "");
    strcat(szMessage, fRXFLAG ? "RXFLAG " : "");
    strcat(szMessage, fRXCHAR ? "RXCHAR " : "");
    strcat(szMessage, fTXEMPTY ? "TXEMPTY " : "");

	// If dwStatus == NULL, then no status event flags are set.
	// This happens when the event flag is changed with SetCommMask.
    if (dwStatus == 0x0000)
        strcat(szMessage, "NULL");

    strcat(szMessage, " ------------\r\n");

    // Queue the status message for the status control
	XDEBUG(szMessage);
#endif

	// If an error flag is set in the event flag, then
	// report the error with the status message
	// If not, then just report the comm status.
//    if (fERR)
//        ReportCommError();
    
	// Might as well check the modem status and comm status now since
	// the event may have been caused by a change in line status.
	// Line status is indicated by the CheckModemStatus function.
    CheckModemStatus(FALSE);

	// Since line status can affect sending/receiving when 
	// hardware flow-control is used, ReportComStat should 
	// be called to show comm status.  This is called only if no error
	// was reported in the event flag.  If an error was reported, then
	// ReportCommError was called above and CheckComStat was already called
	// in that function.
    if (!fERR)
        CheckComStat(FALSE);
}

DWORD CSerialServer::ReadProc()
{
    OVERLAPPED osReader;						// Overlapped structure for read operations
    OVERLAPPED osStatus;						// Overlapped structure for status operations
    HANDLE     arEvents[3];						// Event Array	
    DWORD      dwStoredFlags;					// Local copy of event flags
    DWORD      dwCommEvent=0;					// Result from WaitCommEvent
    DWORD 	   dwRead;							// Bytes actually read
#ifndef WINCE
    DWORD      dwResult;						// Result from WaitForSingleObject
#endif
    BOOL       fWaitingOnRead;
    BOOL       fWaitingOnStat;
    BOOL       fThreadDone;
#ifndef WINCE
    DWORD      dwOvRes;							// Result from GetOverlappedResult
#endif

	m_pSaveSession = NewSession(m_hComPort, m_szDevice);
	OnEnterSession(m_pSaveSession);

    dwStoredFlags	= 0xFFFFFFFF;
    fWaitingOnRead	= FALSE;
    fWaitingOnStat	= FALSE;
    fThreadDone		= FALSE;

    // Create two overlapped structures, one for read events
    // and another for status events
	memset(&osReader, 0, sizeof(OVERLAPPED));
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osReader.hEvent == NULL)
        XDEBUG("[CSerialServer] CreateEvent (Reader Event).\n");

	memset(&osStatus, 0, sizeof(OVERLAPPED));
    osStatus.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osStatus.hEvent == NULL)
        XDEBUG("[CSerialServer] CreateEvent (Status Event).\n");

    arEvents[0] = m_hReadStopEvent;
    arEvents[1] = osReader.hEvent;
    arEvents[2] = osStatus.hEvent;

    // Initial Check, Forces Updates
	CheckModemStatus(TRUE);
	CheckComStat(TRUE);

	m_bRecvReady = TRUE;
    while(!fThreadDone && !m_bDisconnectPending)
	{
		if (m_bPassiveMode)
		{
			Sleep(100);
			continue;
		}

		if (!ReadFile(m_hComPort, m_szBuffer, DEFAULT_SIO_BUFFER, &dwRead, &osReader))
		{
			// Read not delayed?
			if (GetLastError() != ERROR_IO_PENDING)
				XDEBUG("[CSerialComm] ReadFile in ReaderAndStatusProc.\r\n");
		}
#ifndef WINCE
		else
		{
			// Read Completed Immediately
			if ((dwRead != MAX_READ_BUFFER) && m_bDisplayTimeouts)
				XDEBUG("[CSerialComm] Read timed out immediately.\r\n");
		}

		dwResult = WaitForMultipleObjects(3, arEvents, FALSE, STATUS_CHECK_TIMEOUT);
		switch(dwResult) {
          case WAIT_OBJECT_0 : // Thread Exit Event
			   fThreadDone = TRUE;
			   break;

		  case WAIT_OBJECT_0 + 1 : // Read Completed
			   if (!GetOverlappedResult(m_hComPort, &osReader, &dwRead, FALSE))
			   {
				   if (GetLastError() == ERROR_OPERATION_ABORTED)
					   XDEBUG("[CSerialComm] Read aborted.\r\n");
				   else ErrorHandler(const_cast<char*>("[CSerialServer] GetOverlappedResult (in Reader).\r\n"));
			   }
			   else
			   {	
				   // Read Completed Successfully
				   if ((dwRead != MAX_READ_BUFFER) && m_bDisplayTimeouts)
					   XDEBUG("[CSerialComm] Read timed out overlapped.\r\n");

				   if (dwRead > 0)
						OnReceiveSession(m_pSaveSession, (char *)m_szBuffer, dwRead);
			   }
			   fWaitingOnRead = FALSE;
			   break;

		  case WAIT_OBJECT_0 + 2 : // Status Completed
			   if (!GetOverlappedResult(m_hComPort, &osStatus, &dwOvRes, FALSE))
			   {
				   if (GetLastError() == ERROR_OPERATION_ABORTED)
						XDEBUG("[CSerialServer] WaitCommEvent aborted.\n");
				   else ErrorHandler(const_cast<char*>("[CSerialServer] GetOverlappedResult (in Reader).\r\n"));
			   }
			   else
			   {
				   // status check completed successfully
				   ReportStatusEvent(dwCommEvent);
			   }
			   fWaitingOnStat = FALSE;
			   break;

            case WAIT_TIMEOUT:
                // If status checks are not allowed, then don't issue the
                // modem status check nor the com stat check
                if (!m_bNoStatus)
				{
                    CheckModemStatus(FALSE);
                    CheckComStat(FALSE);
                }
                break;                       

            default:
                XDEBUG("[CSerialServer] WaitForMultipleObjects(Reader & Status handles).\r\n");
                break;
        }
#else
		if (dwRead > 0)
			OnReceiveSession(m_pSaveSession, (char *)m_szBuffer, dwRead);
#endif
    }

	OnLeaveSession(m_pSaveSession);
	DeleteSession(m_pSaveSession);

	// Set SendThread Exit Event
	SetEvent(m_hThreadExitEvent);
	m_bExitWriter = TRUE;

	WaitForSingleObject(m_hSendTerminateEvent, 5000);

    // Close event handles
    CloseHandle(osReader.hEvent);
    CloseHandle(osStatus.hEvent);

	// Set Terminate Signal
	SetEvent(m_hReadTerminateEvent);
	return 0;
}

DWORD CSerialServer::SendProc()
{
	CSerialWriter::SetFile(m_hComPort);

	m_bSendReady = TRUE;
	CSerialWriter::WriterProc((VOID *)this);
	SetEvent(m_hSendTerminateEvent);
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CSerialServer Thread Functions
//////////////////////////////////////////////////////////////////////

DWORD CSerialServer::ReadThread(LPVOID pParam)
{
	CSerialServer		*pThis;

	pThis = (CSerialServer *)pParam;
	return pThis->ReadProc();
}

DWORD CSerialServer::SendThread(LPVOID pParam)
{
	CSerialServer		*pThis;

	pThis = (CSerialServer *)pParam;
	return pThis->SendProc();
}
