#include "common.h"
#include "UploadService.h"
#include "Variable.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "logdef.h"
#include "DebugUtil.h"
#include "if4frame.h"

//////////////////////////////////////////////////////////////////////
// CUploadService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUploadService		*m_pService = NULL;

//////////////////////////////////////////////////////////////////////
// CUploadService Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUploadService::CUploadService(int nNzcNumber, char *ipaddr, int nPort, char *fileName, BOOL bDebug)
{
	m_pService 		= this;

	strcpy(m_szServer,ipaddr);
	strcpy(m_szFileName,fileName);
	m_nNzcNumber    = nNzcNumber;
	m_nPort			= nPort;
    m_bDebug        = bDebug;
}

CUploadService::~CUploadService()
{
}

//////////////////////////////////////////////////////////////////////
// CUploadService Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CUploadService::Startup()
{
	int		i;
	BOOL	bOK = FALSE;

    SET_DEBUG_MODE(m_bDebug ? 0 : 1);

	XDEBUG("------------------- UPLOAD -----------------\r\n");
	XDEBUG("SEND METERING DATA: SERVER=%s, PORT=%0d, FILE=%s\r\n",
			m_szServer, m_nPort, m_szFileName);

	if (!OnStartService())
	{
		XDEBUG("------------------ UPLOAD END ---------------\r\n");
		return FALSE;
	}
	
	for(i=0; i<3; i++)
	{
	 	XDEBUG("%s:%d Try upload. (try=%d)\r\n", m_szServer, m_nPort, i+1);
	 	UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Try upload. (try=%d)\n", m_szServer, m_nPort, i+1);
		if (OnExecuteService())
		{
			bOK = TRUE;
			break;
		}
	}
	XDEBUG("------------------ UPLOAD END ---------------\r\n");
	return bOK;
}

void CUploadService::Shutdown()
{
	OnStopService();
}

//////////////////////////////////////////////////////////////////////
// CUploadService Override Functions
//////////////////////////////////////////////////////////////////////

BOOL CUploadService::OnStartService()
{
	VARAPI_Initialize(NULL, (VAROBJECT *)m_Root_node, FALSE);
	IF4API_Initialize(m_nPort+111, NULL);
	IF4API_SetID(m_nNzcNumber);
	return TRUE;
}

void CUploadService::OnStopService()
{
	IF4API_Destroy();
	VARAPI_Destroy();
}

BOOL CUploadService::OnExecuteService()
{
	CIF4Invoke      invoke(m_szServer, m_nPort, 120);
	BOOL			bResult = TRUE;
	int				nError, nFileBytes, nSendBytes;

    nError = IF4API_SendDataFile(invoke.GetHandle(), IF4_DATAFILE_MEASUREMENT, m_szFileName, TRUE);

    switch(nError)
    {
        case IF4ERR_NOERROR:
            nFileBytes = invoke.GetHandle()->nSourceLength - sizeof(IF4_HEADER) - sizeof(IF4_TAIL);
            nSendBytes = invoke.GetHandle()->nLength;
            XDEBUG("%s:%d Sending ok. (Orig %d Bytes, Compressed %d Bytes, %d%%)\r\n", 
                    m_szServer, m_nPort, nFileBytes, nSendBytes, nFileBytes > 0 ? (int)(nSendBytes * 100 / nFileBytes) : 0);
            UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Sending ok. (Orig %d Bytes, Compressed %d Bytes, %d%%)\n", 
                    m_szServer, m_nPort, nFileBytes, nSendBytes, nFileBytes > 0 ? (int)(nSendBytes * 100 / nFileBytes) : 0);
            break;
        case IF4ERR_CANNOT_CONNECT:
		    bResult = FALSE;
            XDEBUG("%s:%d Cannot connect server.\r\n", m_szServer, m_nPort);
            UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Cannot connect server.\n", m_szServer, m_nPort);
            break;
        default:
		    bResult = FALSE;
            XDEBUG("%s:%d Sending fail.\r\n", m_szServer, m_nPort);
            UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, 0, TRUE, "%s:%d Sending fail.\n", m_szServer, m_nPort);
            break;
	}

	return bResult;
}



