#include "common.h"
#include <sys/stat.h>

#include "Chunk.h"
#include "if4frame.h"
#include "IF4Service.h"
#include "IF4TcpClient.h"
#include "IF4CrcCheck.h"
#include "IF4DataFrame.h"
#include "IF4Util.h"
#ifndef _WIN32
#include "Event.h"
#endif
#include "MemoryDebug.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Data Definitions
//////////////////////////////////////////////////////////////////////

CIF4Service		*m_pIF4Service = NULL;
extern BOOL     m_bGlobalCompressFlag;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIF4Service::CIF4Service()
{
	m_pIF4Service 	= this;
	m_pCommandTable = NULL;
	m_nServiceProviderID = 0;

	m_pfnOnDataFile	= NULL;
	m_pfnOnData		= NULL;
	m_pfnOnEvent	= NULL;
	m_pfnOnAlarm	= NULL;
	m_pfnOnTypeR	= NULL;
}

CIF4Service::~CIF4Service()
{
}

//////////////////////////////////////////////////////////////////////
// Service Startup/Shutdown
//////////////////////////////////////////////////////////////////////

void CIF4Service::EnableCompress(BOOL bEnable)
{
	m_bGlobalCompressFlag = bEnable;
}

void CIF4Service::SetMcuID(UINT nID)
{
	m_nServiceProviderID = nID;
}

UINT CIF4Service::GetMcuID()
{
	return m_nServiceProviderID;
}

//////////////////////////////////////////////////////////////////////
// Service Startup/Shutdown
//////////////////////////////////////////////////////////////////////

int CIF4Service::Startup(int nPort, IF4_COMMAND_TABLE *pTable)
{
	m_pCommandTable = pTable;

	if ((pTable != NULL) && (nPort > 0))
		m_theTcpService.Start(nPort, 100);

	return IF4ERR_NOERROR;
}

int CIF4Service::Shutdown()
{
	m_theTcpService.Stop();
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// CIF4Service Operations
//////////////////////////////////////////////////////////////////////

IF4Invoke *CIF4Service::NewInvoke(const char *pszAddress, int port, int nTimeout)
{
	IF4Invoke	*pInvoke;

	if (!pszAddress || !*pszAddress)
		return NULL;

	if (port <= 0)
		return NULL;

	pInvoke = (IF4Invoke *)MALLOC(sizeof(IF4Invoke));
	if (pInvoke == NULL)
		return NULL;

	memset(pInvoke, 0, sizeof(IF4Invoke));
	strcpy(pInvoke->szAddress, pszAddress);
	pInvoke->port		= port;
	pInvoke->timeout	= nTimeout;
	pInvoke->pClient	= NULL;
	pInvoke->pFrame		= NULL;
	pInvoke->hEvent		= NULL;
	return pInvoke;
}

int CIF4Service::DeleteInvoke(IF4Invoke *pInvoke)
{

	if (pInvoke == NULL)
		return IF4ERR_INVALID_HANDLE;

	if (pInvoke->hEvent != NULL)
		CloseHandle(pInvoke->hEvent);

	FreeLinkedValue(pInvoke->pParamNode);
	FreeLinkedValue(pInvoke->pResultNode);

	if (pInvoke->pResult)
		FREE(pInvoke->pResult);

	if (pInvoke->pFrame)
		FREE(pInvoke->pFrame);

	FREE(pInvoke);
	return IF4ERR_NOERROR;
}

int CIF4Service::AddParam(IF4Invoke *pInvoke, OID3 *oid, int nType, int nValue, const char *pszValue, int nLength)
{
	MIBValue	*pValue;

	pValue = NewValue(oid, nType, nValue, pszValue, nLength);
	if (pValue == NULL)
		return IF4ERR_MEMORY_ERROR;

	if (pInvoke->pParamNode == NULL)
	{
		pInvoke->pParamNode = pValue;
		pInvoke->pLastParam = pValue;
	}
	else
	{
		pInvoke->pLastParam->pNext = pValue;
		pInvoke->pLastParam = pValue;	
	}
	pInvoke->nParamCount++;
	return IF4ERR_NOERROR;
}

int CIF4Service::AddResult(IF4Wrapper *pWrapper, OID3 *oid, int nType, int nValue, const char *pszValue, int nLength)
{
	MIBValue	*pValue;

	pValue = NewValue(oid, nType, nValue, pszValue, nLength);
	if (pValue == NULL)
		return IF4ERR_MEMORY_ERROR;

	if (pWrapper->pResult == NULL)
	{
		pWrapper->pResult = pValue;
		pWrapper->pLast = pValue;
	}
	else
	{
		pWrapper->pLast->pNext = pValue;
		pWrapper->pLast = pValue;
	}
	pWrapper->nResult++;
	return IF4ERR_NOERROR;
}

MIBValue *CIF4Service::NewValue(OID3 *oid, int nType, int nValue, const char *pszValue, int nLength)
{
	MIBValue	*pValue;

	pValue = (MIBValue *)MALLOC(sizeof(MIBValue));
	if (pValue == NULL)
		return NULL;

	memset(pValue, 0, sizeof(MIBValue));
	memcpy(&pValue->oid, oid, sizeof(OID3));
	pValue->type = nType;

	switch(nType) {
	  case VARSMI_BOOL :
		   pValue->len = sizeof(BYTE);
		   pValue->stream.u8 = (BYTE)nValue;
		   break;
	  case VARSMI_BYTE :
		   pValue->len = sizeof(BYTE);
		   pValue->stream.u8 = (BYTE)nValue;
		   break;
	  case VARSMI_WORD :
		   pValue->len = sizeof(WORD);
		   pValue->stream.u16 = (WORD)nValue;
		   break;
	  case VARSMI_UINT :
		   pValue->len = sizeof(unsigned int);
		   pValue->stream.u32 = (unsigned int)nValue;
		   break;
	  case VARSMI_CHAR :
		   pValue->len = sizeof(char);
		   pValue->stream.s8 = (char)nValue;
		   break;
	  case VARSMI_SHORT :
		   pValue->len = sizeof(short);
		   pValue->stream.s16 = (short)nValue;
		   break;
	  case VARSMI_INT :
		   pValue->len = sizeof(int);
		   pValue->stream.s32 = (int)nValue;
		   break;
	  case VARSMI_OID :
		   pValue->len = sizeof(OID3);
		   memcpy(&pValue->stream.id, pszValue, sizeof(OID3));
		   break;
	  case VARSMI_STRING :
	  case VARSMI_UNKNOWN :
	  case VARSMI_STREAM :
	  case VARSMI_OPAQUE :
		   pValue->len = nLength;
		   pValue->stream.p = (char *)MALLOC(nLength+1);
		   memcpy(pValue->stream.p, pszValue, nLength);
		   pValue->stream.p[nLength] = '\0';
		   break;
	  case VARSMI_EUI64 :
		   pValue->len = sizeof(EUI64);
		   pValue->stream.p = (char *)MALLOC(sizeof(EUI64));
		   memcpy(pValue->stream.p, pszValue, sizeof(EUI64));
		   break;
	  case VARSMI_IPADDR :
		   pValue->len = sizeof(IPADDR);
		   pValue->stream.p = (char *)MALLOC(sizeof(IPADDR));
		   memcpy(pValue->stream.p, pszValue, sizeof(IPADDR));
		   break;
	  case VARSMI_TIMESTAMP :
		   pValue->len = sizeof(TIMESTAMP);
		   pValue->stream.p = (char *)MALLOC(sizeof(TIMESTAMP));
		   memcpy(pValue->stream.p, pszValue, sizeof(TIMESTAMP));
		   break;
	  case VARSMI_TIMEDATE :
		   pValue->len = sizeof(TIMEDATE);
		   pValue->stream.p = (char *)MALLOC(sizeof(TIMEDATE));
		   memcpy(pValue->stream.p, pszValue, sizeof(TIMEDATE));
		   break;
	}
	return pValue;
}

BOOL CIF4Service::PrecreateInvoke(IF4Invoke *pInvoke, int nLength)
{
	if (pInvoke->pFrame == NULL)
	{
		pInvoke->pFrame	= (char *)MALLOC(nLength);
		memset(pInvoke->pFrame, 0, nLength);	
	}

	if (pInvoke->hEvent == NULL)
		pInvoke->hEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;
}

int CIF4Service::Cancel(IF4Invoke *pInvoke)
{
	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (pInvoke->hEvent != NULL)
		SetEvent(pInvoke->hEvent);
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// Callback Function
//////////////////////////////////////////////////////////////////////

int CIF4Service::SetUserCallback(PFNIF4ONDATAFILE pfnOnDataFile, PFNIF4ONDATA pfnOnData, 
        PFNIF4ONEVENT pfnOnEvent, PFNIF4ONALARM pfnOnAlarm, PFNIF4ONTYPER pfnOnTypeR)
{
	m_pIF4Service->m_pfnOnDataFile 	= pfnOnDataFile;
	m_pIF4Service->m_pfnOnData	  	= pfnOnData;
	m_pIF4Service->m_pfnOnEvent	  	= pfnOnEvent;
	m_pIF4Service->m_pfnOnAlarm	  	= pfnOnAlarm;
	m_pIF4Service->m_pfnOnTypeR	  	= pfnOnTypeR;
	return IF4ERR_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// CIF4Service Operations
//////////////////////////////////////////////////////////////////////

int CIF4Service::Command(IF4Invoke *pInvoke, OID3 *cmd, BYTE nAttr)
{
	CIF4TcpClient	*pClient = NULL;
	IF4_CMD_FRAME	*pFrame;
	MIBValue		*pNode;
	SMIValue		var;
	char			szOid[20];
	int				i, nLength, nError;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!PrecreateInvoke(pInvoke))
		return IF4ERR_SYSTEM_ERROR;
 
	pFrame = (IF4_CMD_FRAME *)pInvoke->pFrame;
	if (pFrame == NULL)
		return IF4ERR_INVALID_BUFFER;

	CChunk	chunk(256);
	pNode = pInvoke->pParamNode;
	for(i=0; pNode && (i<pInvoke->nParamCount); i++)
	{
		memcpy(&var.oid, &pNode->oid, sizeof(OID3));
		var.len	= pNode->len;
		chunk.Add((char *)&var, 5);

	    switch(pNode->type) {
   		  case VARSMI_CHAR :
		  case VARSMI_BOOL :
		  case VARSMI_BYTE :
      	  case VARSMI_SHORT :
      	  case VARSMI_WORD :
      	  case VARSMI_INT :
      	  case VARSMI_UINT :
			   chunk.Add((char *)&pNode->stream.u32, pNode->len);
			   break;
      	  case VARSMI_OID :
			   chunk.Add((char *)&pNode->stream.id, pNode->len);
			   break;
      	  case VARSMI_STRING :
      	  case VARSMI_STREAM :
      	  case VARSMI_OPAQUE :
      	  case VARSMI_EUI64 :
      	  case VARSMI_IPADDR :
      	  case VARSMI_TIMESTAMP :
      	  case VARSMI_TIMEDATE :
			   chunk.Add((char *)pNode->stream.p, pNode->len);
			   break;
    	}
		pNode = pNode->pNext;
	}

	OidToString(cmd, szOid);
	XDEBUG("IF4CLIENT: CMD=\"%s\", PARAM-CNT=%0d\xd\xa",
			szOid, pInvoke->nParamCount);
	DumpSmiValue((char *)chunk.GetBuffer(), pInvoke->nParamCount);

    if(pInvoke->pFrame)
    {
	    pInvoke->pFrame = (char *)REALLOC(pInvoke->pFrame, chunk.GetSize()+100);
    }
    else
    {
        pInvoke->pFrame = (char *)MALLOC(chunk.GetSize()+100);
    }
	pFrame = (IF4_CMD_FRAME *)pInvoke->pFrame;
	memcpy(&pInvoke->cmd, cmd, sizeof(OID3));	
	nLength = MakeCommandFrame(pFrame, cmd,
				GetTID(),
				nAttr | IF4_CMDATTR_REQUEST,
				IF4ERR_NOERROR,
				pInvoke->nParamCount,
				(SMIValue *)chunk.GetBuffer(),
				&pInvoke->nSourceLength);

	pInvoke->nError	  = IF4ERR_NOERROR;
	pInvoke->nLength  = nLength;

	pClient = new CIF4TcpClient;
	if (pClient == NULL)
		return IF4ERR_INVALID_TARGET;

	pClient->SetInvoke(pInvoke);
	if (!pClient->TryConnectServer(pInvoke->szAddress, pInvoke->port))
	{
		pClient->DisconnectServer();
		delete pClient;

		pInvoke->nError = IF4ERR_CANNOT_CONNECT;
		return pInvoke->nError;
	}

	if (pInvoke->bAsync)
		return IF4ERR_NOERROR;

	nError = pClient->SendRequest();
	if (nError == 1)
	{
        if (WaitForSingleObject(pInvoke->hEvent, pInvoke->timeout) == ETIMEDOUT)
            pInvoke->nError = IF4ERR_TIMEOUT;
	    else if (!pClient->IsHandshake())
		    pInvoke->nError = IF4ERR_CANNOT_CONNECT;
	}
	else if (nError == -1)
	{
		// 멀????레????송 ??????웃
		pInvoke->nError = IF4ERR_TIMEOUT;
	}

	if (pClient->IsConnected())
	{
		pClient->SendEot();
		pClient->DisconnectServer();
	}
	delete pClient;
	return pInvoke->nError;
}

int CIF4Service::AlarmFrame(IF4Invoke *pInvoke, char *pszBuffer, BYTE nType, EUI64 *id, TIMESTAMP *pTime, int nDataLen, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx)
{
	IF4_ALARM_FRAME	*pFrame;
	int				nLength;

	if (pInvoke == NULL)
		return 0;

	pFrame = (IF4_ALARM_FRAME *)pszBuffer;
	memset(pFrame, 0, sizeof(IF4_ALARM_FRAME));
	nLength = MakeIF4AlarmFrame(pFrame, nType, id, pTime, pMessage, nDataLen, pSysTime, idx, &pInvoke->nSourceLength);
	return nLength;
}
	
int CIF4Service::Alarm(IF4Invoke *pInvoke, BYTE nType, EUI64 *id, TIMESTAMP *pTime, int nDataLen, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx)
{
	CIF4TcpClient	*pClient = NULL;
	IF4_ALARM_FRAME	*pFrame;
	int				nError, nLength;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!PrecreateInvoke(pInvoke))
		return IF4ERR_SYSTEM_ERROR;
 
	pFrame = (IF4_ALARM_FRAME *)pInvoke->pFrame;
	if (pFrame == NULL)
		return IF4ERR_INVALID_BUFFER;

	memset(pFrame, 0, sizeof(IF4_ALARM_FRAME));
	try
	{
		nLength = MakeIF4AlarmFrame(pFrame, nType, id, pTime, pMessage, nDataLen, pSysTime, idx, &pInvoke->nSourceLength);
	}
	catch(...)
	{
		nLength = -1;
		XDEBUG("IF4Service: Memory Exception Error.\xd\xa");
	}

	if (nLength == -1)
		return IF4ERR_INVALID_BUFFER;
	
	pInvoke->nError	  = IF4ERR_NOERROR;
	pInvoke->nFlag	  = IF4_FLAG_ACK;	
	pInvoke->nLength  = nLength;

	pClient = new CIF4TcpClient;
	if (pClient == NULL)
		return IF4ERR_INVALID_TARGET;

	pClient->SetInvoke(pInvoke);
	if (!pClient->TryConnectServer(pInvoke->szAddress, pInvoke->port))
	{
		pClient->DisconnectServer();
		delete pClient;

		pInvoke->nError = IF4ERR_CANNOT_CONNECT;
		return pInvoke->nError;
	}

	nError = pClient->SendRequest();
	if (nError == 1)
	{
        if (WaitForSingleObject(pInvoke->hEvent, pInvoke->timeout) == ETIMEDOUT)
            pInvoke->nError = IF4ERR_TIMEOUT;
	    else if (!pClient->IsHandshake())
		    pInvoke->nError = IF4ERR_CANNOT_CONNECT;
	    else if (pInvoke->nCtrlCode != IF4_CTRLCODE_ACK)
		    pInvoke->nError = IF4ERR_CANNOT_ACCEPT;
	}
	else if (nError == -1)
	{
		// 멀????레????송 ??????웃
		pInvoke->nError = IF4ERR_TIMEOUT;
	}

	if (pClient->IsConnected())
	{
		pClient->SendEot();
		pClient->DisconnectServer();
	}
	delete pClient;
	return pInvoke->nError;
}

int CIF4Service::EventFrame(IF4Invoke *pInvoke, char *pszBuffer, OID3 *id, BYTE nType, BYTE *srcId, TIMESTAMP *pTime)
{
	IF4_EVENT_FRAME	*pFrame;
	MIBValue		*pNode;
	SMIValue		var;
	int				i, len, nLength;

	if (pInvoke == NULL)
		return 0;

	CChunk	chunk(256);

	pFrame = (IF4_EVENT_FRAME *)pszBuffer;
	pNode = pInvoke->pParamNode;
	for(i=0; pNode && (i<pInvoke->nParamCount); i++)
	{
		len = CopyMIB2SMIValue(&var, pNode);
		chunk.Add((char *)&var, len);
		pNode = pNode->pNext;
	}

	// Event frame만을 만들??는 ??축?????? ??코 ??달??다.
	memset(pFrame, 0, IF4_DEFAULT_FRAME_SIZE);
	nLength = MakeIF4EventFrame(pFrame, id, nType, srcId, pTime,
					 pInvoke->nParamCount, (SMIValue *)chunk.GetBuffer(),
					 &pInvoke->nSourceLength, FALSE);
	return nLength;
}

int CIF4Service::Event(IF4Invoke *pInvoke, OID3 *id, BYTE nType, BYTE *srcId, TIMESTAMP *pTime)
{
	CIF4TcpClient	*pClient = NULL;
	IF4_EVENT_FRAME	*pFrame;
	MIBValue		*pNode;
	SMIValue		var;
	int				i, len, nLength;
	int				nError, nSize;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;
	
	nSize  = sizeof(IF4_HEADER) + sizeof(IF4_TAIL);
	nSize += (sizeof(IF4_SVC_HEADER) + sizeof(IF4_EVENT_HEADER));
	pNode = pInvoke->pParamNode;
	for(i=0; pNode && (i<pInvoke->nParamCount); i++)
	{
		nSize += pNode->len + sizeof(OID3) + sizeof(WORD);
		pNode = pNode->pNext;
	}

	if (!PrecreateInvoke(pInvoke, nSize))
		return IF4ERR_SYSTEM_ERROR;
 
	pFrame = (IF4_EVENT_FRAME *)pInvoke->pFrame;
	if (pFrame == NULL)
		return IF4ERR_INVALID_BUFFER;

	CChunk	chunk(256);
	pNode = pInvoke->pParamNode;
	for(i=0; pNode && (i<pInvoke->nParamCount); i++)
	{
		len = CopyMIB2SMIValue(&var, pNode);
		chunk.Add((char *)&var, len);
		pNode = pNode->pNext;
	}

	memset(pFrame, 0, nSize);
	nLength = MakeIF4EventFrame(pFrame, id, nType, srcId, pTime,
				 pInvoke->nParamCount, (SMIValue *)chunk.GetBuffer(),
				 &pInvoke->nSourceLength, TRUE);
/*
	{
		char	szGUID[30] = "";
		char	szName[64] = "";
		char	szOID[20] = "";
		char	szTime[30];

		VARAPI_OidToString(id, szOID);
		VARAPI_OidToName(id, szName);
		IF4API_TIMESTAMPTOSTR(szTime, pTime);
		EUI64ToStr((EUI64 *)srcId, szGUID);
//		XDEBUG("IF4INVOKE: EVENT=%s(%s), Address=%s\xd\xa",
//					szName, szOID, pInvoke->szAddress);
//		XDEBUG("       ID=%s(%0d), TIME=%s\xd\xa", szGUID, nType, szTime);
	}
*/

	pInvoke->nError	  = IF4ERR_NOERROR;
	pInvoke->nFlag	  = IF4_FLAG_ACK;	
	pInvoke->nLength  = nLength;

	pClient = new CIF4TcpClient;
	if (pClient == NULL)
		return IF4ERR_INVALID_TARGET;

	pClient->SetInvoke(pInvoke);
	if (!pClient->TryConnectServer(pInvoke->szAddress, pInvoke->port))
	{
		pClient->DisconnectServer();
		delete pClient;

		pInvoke->nError = IF4ERR_CANNOT_CONNECT;
		return pInvoke->nError;
	}

	nError = pClient->SendRequest();
	if (nError == 1)
	{
        if (WaitForSingleObject(pInvoke->hEvent, pInvoke->timeout) == ETIMEDOUT)
            pInvoke->nError = IF4ERR_TIMEOUT;
	    else if (!pClient->IsHandshake())
		    pInvoke->nError = IF4ERR_CANNOT_CONNECT;
	    else if (pInvoke->nCtrlCode != IF4_CTRLCODE_ACK)
		    pInvoke->nError = IF4ERR_CANNOT_ACCEPT;
	}
	else if (nError == -1)
	{
		// 멀????레????송 ??????웃
		pInvoke->nError = IF4ERR_TIMEOUT;
	}

	if (pClient->IsConnected())
	{
		pClient->SendEot();
		pClient->DisconnectServer();
	}
	delete pClient;
	return pInvoke->nError;
}

int CIF4Service::SendData(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int len)
{
	CIF4TcpClient	*pClient = NULL;
	IF4_DATA_FRAME	*pFrame;
	int				nError, nLength;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!PrecreateInvoke(pInvoke))
		return IF4ERR_SYSTEM_ERROR;
 
	pFrame = (IF4_DATA_FRAME *)pInvoke->pFrame;
	if (pFrame == NULL)
		return IF4ERR_INVALID_BUFFER;

    if(pInvoke->pFrame)
    {
	    pInvoke->pFrame = (char *)REALLOC(pInvoke->pFrame, sizeof(IF4_DATA_FRAME)+len+20);
    }
    else
    {
	    pInvoke->pFrame = (char *)MALLOC(sizeof(IF4_DATA_FRAME)+len+20);
    }
	if (pInvoke->pFrame == NULL)
	{
		XDEBUG("IF4Service: Memory Allcation Error.\xd\xa");
		return IF4ERR_INVALID_BUFFER;
	}

	pFrame = (IF4_DATA_FRAME *)pInvoke->pFrame;
	memset(pFrame, 0, sizeof(IF4_DATA_FRAME));

	try
	{
		nLength = MakeIF4DataFrame(pFrame, nCount, pszData, len, &pInvoke->nSourceLength);
	}
	catch(...)
	{
		nLength = -1;
		XDEBUG("IF4Service: Memory Exception Error.\xd\xa");
	}

	if (nLength == -1)
		return IF4ERR_INVALID_BUFFER;
	
	pInvoke->nError	  = IF4ERR_NOERROR;
	pInvoke->nFlag	  = IF4_FLAG_ACK;	
	pInvoke->nLength  = nLength;

	XDEBUG("IF4API: Send Metering Data: %s:%0d, %d Bytes\xd\xa",
				pInvoke->szAddress, pInvoke->port, nLength);

	pClient = new CIF4TcpClient;
	if (pClient == NULL)
		return IF4ERR_INVALID_TARGET;

	pClient->SetInvoke(pInvoke);
	if (!pClient->TryConnectServer(pInvoke->szAddress, pInvoke->port))
	{
		pClient->DisconnectServer();
		delete pClient;

		pInvoke->nError = IF4ERR_CANNOT_CONNECT;
		return pInvoke->nError;
	}

	nError = pClient->SendRequest();
	if (nError == 1)
	{
        if (WaitForSingleObject(pInvoke->hEvent, pInvoke->timeout) == ETIMEDOUT)
            pInvoke->nError = IF4ERR_TIMEOUT;
	    else if (!pClient->IsHandshake())
		    pInvoke->nError = IF4ERR_CANNOT_CONNECT;
	    else if (pInvoke->nCtrlCode != IF4_CTRLCODE_ACK)
		    pInvoke->nError = IF4ERR_CANNOT_ACCEPT;
	}
	else if (nError == -1)
	{
		// 멀????레????송 ??????웃
		pInvoke->nError = IF4ERR_TIMEOUT;
	}

	if (pClient->IsConnected())
    {
		pClient->SendEot();
	    pClient->DisconnectServer();
    }
	delete pClient;
	return pInvoke->nError;
}

int CIF4Service::SendDataFile(IF4Invoke *pInvoke, BYTE nType, char *pszFileName, BOOL bCompressed)
	{
	CIF4TcpClient	*pClient = NULL;
	IF4_DATAFILE_FRAME	*pFrame;
 	struct stat 	file_info;
	char			*pszData, *pszName;
	FILE			*fp;
	int				nError, nResult, nLength, len;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!pszFileName || !*pszFileName)
		return IF4ERR_INVALID_FILENAME;

	if (!IsExists(pszFileName))
		return IF4ERR_INVALID_FILENAME;

    nResult = stat(pszFileName, &file_info);
	if (nResult == -1)
		return IF4ERR_NO_ENTRY;

	len = file_info.st_size;
	if (!PrecreateInvoke(pInvoke))
		return IF4ERR_SYSTEM_ERROR;

	pFrame = (IF4_DATAFILE_FRAME *)pInvoke->pFrame;
	if (pFrame == NULL)
		return IF4ERR_INVALID_BUFFER;

	pszData = (char *)MALLOC(len+1);
	if (pszData == NULL)
		return IF4ERR_MEMORY_ERROR;
 
    if(pInvoke->pFrame)
    {
	    pInvoke->pFrame = (char *)REALLOC(pInvoke->pFrame, sizeof(IF4_DATAFILE_FRAME)+len+20);
    }
    else
    {
	    pInvoke->pFrame = (char *)MALLOC(sizeof(IF4_DATAFILE_FRAME)+len+20);
    }
	if (pInvoke->pFrame == NULL)
		{
		if (pszData) FREE(pszData);
		XDEBUG("IF4Service: Memory Allcation Error.\xd\xa");
		return IF4ERR_INVALID_BUFFER;
	}

	fp = fopen(pszFileName, "rb");
	if (fp == NULL)
	{
		if (pszData) FREE(pszData);
		XDEBUG("IF4Service: File open error : %s\xd\xa", pszFileName);
		return IF4ERR_IO_ERROR;
	}
	fread(pszData, len, 1, fp);
	fclose(fp);

	pFrame = (IF4_DATAFILE_FRAME *)pInvoke->pFrame;
	memset(pFrame, 0, sizeof(IF4_DATAFILE_FRAME));

	try
	{
		pszName = strrchr(pszFileName, '/');
		if (pszName == NULL) pszName = pszFileName;
		else pszName++;
		nLength = MakeIF4DataFileFrame(pFrame, nType, pszName, (BYTE *)pszData, len, &pInvoke->nSourceLength);
	}
	catch(...)
	{
		nLength = -1;
		XDEBUG("IF4Service: Memory Exception Error.\xd\xa");
	}

	if (pszData) FREE(pszData);

	if (nLength == -1)
		return IF4ERR_INVALID_BUFFER;
	
	pInvoke->nError	  = IF4ERR_NOERROR;
	pInvoke->nFlag	  = IF4_FLAG_ACK;	
	pInvoke->nLength  = nLength;

	XDEBUG("IF4API: Send Data File: %s:%0d, %d Bytes\xd\xa",
				pInvoke->szAddress, pInvoke->port, nLength);

	pClient = new CIF4TcpClient;
	if (pClient == NULL)
		return IF4ERR_INVALID_TARGET;

	pClient->SetInvoke(pInvoke);
	if (!pClient->TryConnectServer(pInvoke->szAddress, pInvoke->port))
	{
		pClient->DisconnectServer();
		delete pClient;

		pInvoke->nError = IF4ERR_CANNOT_CONNECT;
		return pInvoke->nError;
	}

	nError = pClient->SendRequest();
	if (nError == 1)
	{
	    if (WaitForSingleObject(pInvoke->hEvent, pInvoke->timeout) == ETIMEDOUT)
	   	    pInvoke->nError = IF4ERR_TIMEOUT;
		else if (!pClient->IsHandshake())
			pInvoke->nError = IF4ERR_CANNOT_CONNECT;
		else if (pInvoke->nCtrlCode != IF4_CTRLCODE_ACK)
			pInvoke->nError = IF4ERR_CANNOT_ACCEPT;
		}
	else if (nError == -1)
	{
		// 멀????레????송 ??????웃
		pInvoke->nError = IF4ERR_TIMEOUT;
	}

	if (pClient->IsConnected())
    {
		pClient->SendEot();
	    pClient->DisconnectServer();
    }
	delete pClient;
	return pInvoke->nError;
}

int CIF4Service::SendMeteringData(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int len)
{
	CIF4TcpClient	*pClient = NULL;
	IF4_DATA_FRAME	*pFrame;
	int				nError, nLength;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!PrecreateInvoke(pInvoke))
		return IF4ERR_SYSTEM_ERROR;
 
	pFrame = (IF4_DATA_FRAME *)pInvoke->pFrame;
	if (pFrame == NULL)
		return IF4ERR_INVALID_BUFFER;

    if(pInvoke->pFrame)
    {
	    pInvoke->pFrame = (char *)REALLOC(pInvoke->pFrame, sizeof(IF4_DATA_FRAME)+len+20);
    }
    else
    {
	    pInvoke->pFrame = (char *)MALLOC(sizeof(IF4_DATA_FRAME)+len+20);
    }
	if (pInvoke->pFrame == NULL)
	{
		XDEBUG("IF4Service: Memory Allcation Error.\xd\xa");
		return IF4ERR_INVALID_BUFFER;
	}

	pFrame = (IF4_DATA_FRAME *)pInvoke->pFrame;
	memset(pFrame, 0, sizeof(IF4_DATA_FRAME));

	try
	{
		nLength = MakeIF4MeteringDataFrame(pFrame, nCount, pszData, len, &pInvoke->nSourceLength);
	}
	catch(...)
	{
		nLength = -1;
		XDEBUG("IF4Service: Memory Exception Error.\xd\xa");
	}

	if (nLength == -1)
		return IF4ERR_INVALID_BUFFER;
	
	pInvoke->nError	  = IF4ERR_NOERROR;
	pInvoke->nFlag	  = IF4_FLAG_ACK;	
	pInvoke->nLength  = nLength;

	XDEBUG("IF4API: Send Metering Data: %s:%0d, %d Bytes\xd\xa",
				pInvoke->szAddress, pInvoke->port, nLength);

	pClient = new CIF4TcpClient;
	if (pClient == NULL)
		return IF4ERR_INVALID_TARGET;

	pClient->SetInvoke(pInvoke);
	if (!pClient->TryConnectServer(pInvoke->szAddress, pInvoke->port))
	{
		pClient->DisconnectServer();
		delete pClient;

		pInvoke->nError = IF4ERR_CANNOT_CONNECT;
		return pInvoke->nError;
	}

	nError = pClient->SendRequest();
	if (nError == 1)
	{
        if (WaitForSingleObject(pInvoke->hEvent, pInvoke->timeout) == ETIMEDOUT)
            pInvoke->nError = IF4ERR_TIMEOUT;
	    else if (!pClient->IsHandshake())
		    pInvoke->nError = IF4ERR_CANNOT_CONNECT;
	    else if (pInvoke->nCtrlCode != IF4_CTRLCODE_ACK)
		    pInvoke->nError = IF4ERR_CANNOT_ACCEPT;
	}
	else if (nError == -1)
	{
		pInvoke->nError = IF4ERR_TIMEOUT;
	}

	if (pClient->IsConnected())
	{
        pClient->SendEot();
        pClient->DisconnectServer();
	}
	delete pClient;
	return pInvoke->nError;
}

int CIF4Service::SendTypeR(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int len)
{
	CIF4TcpClient	*pClient = NULL;
	IF4_TYPER_FRAME	*pFrame;
	int				nError, nLength;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!PrecreateInvoke(pInvoke))
		return IF4ERR_SYSTEM_ERROR;
 
	pFrame = (IF4_TYPER_FRAME *)pInvoke->pFrame;
	if (pFrame == NULL)
		return IF4ERR_INVALID_BUFFER;

    if(pInvoke->pFrame)
    {
	    pInvoke->pFrame = (char *)REALLOC(pInvoke->pFrame, sizeof(IF4_TYPER_FRAME)+len+20);
    }
    else
    {
	    pInvoke->pFrame = (char *)MALLOC(sizeof(IF4_TYPER_FRAME)+len+20);
    }
	if (pInvoke->pFrame == NULL)
	{
		XDEBUG("IF4Service: Memory Allcation Error.\xd\xa");
		return IF4ERR_INVALID_BUFFER;
	}

	pFrame = (IF4_TYPER_FRAME *)pInvoke->pFrame;
	memset(pFrame, 0, sizeof(IF4_TYPER_FRAME));

	try
	{
		nLength = MakeIF4TypeRFrame(pFrame, nCount, pszData, len, &pInvoke->nSourceLength);
	}
	catch(...)
	{
		nLength = -1;
		XDEBUG("IF4Service: Memory Exception Error.\xd\xa");
	}

	if (nLength == -1)
		return IF4ERR_INVALID_BUFFER;
	
	pInvoke->nError	  = IF4ERR_NOERROR;
	pInvoke->nFlag	  = IF4_FLAG_ACK;	
	pInvoke->nLength  = nLength;

	XDEBUG("IF4API: Send Type R Data: %s:%0d, %d Bytes\xd\xa",
				pInvoke->szAddress, pInvoke->port, nLength);

	pClient = new CIF4TcpClient;
	if (pClient == NULL)
		return IF4ERR_INVALID_TARGET;

	pClient->SetInvoke(pInvoke);
	if (!pClient->TryConnectServer(pInvoke->szAddress, pInvoke->port))
	{
		pClient->DisconnectServer();
		delete pClient;

		pInvoke->nError = IF4ERR_CANNOT_CONNECT;
		return pInvoke->nError;
	}

	nError = pClient->SendRequest();
	if (nError == 1)
	{
        if (WaitForSingleObject(pInvoke->hEvent, pInvoke->timeout) == ETIMEDOUT)
            pInvoke->nError = IF4ERR_TIMEOUT;
	    else if (!pClient->IsHandshake())
		    pInvoke->nError = IF4ERR_CANNOT_CONNECT;
	    else if (pInvoke->nCtrlCode != IF4_CTRLCODE_ACK)
		    pInvoke->nError = IF4ERR_CANNOT_ACCEPT;
	}
	else if (nError == -1)
	{
		pInvoke->nError = IF4ERR_TIMEOUT;
	}

	if (pClient->IsConnected())
    {
		pClient->SendEot();
	    pClient->DisconnectServer();
    }
	delete pClient;
	return pInvoke->nError;
}

int CIF4Service::OpenSession(IF4Invoke *pInvoke)
{
	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	if (!PrecreateInvoke(pInvoke))
		return IF4ERR_SYSTEM_ERROR;
 
	return IF4ERR_NOERROR;
}

int CIF4Service::SendSession(IF4Invoke *pInvoke, WORD nCount, BYTE *pszData, int len, BYTE nSeq)
{
	CIF4TcpClient	*pClient = NULL;
	IF4_DATA_FRAME	*pFrame;
	int				nLength;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	pFrame = (IF4_DATA_FRAME *)pInvoke->pFrame;
	if (pFrame == NULL)
		return IF4ERR_INVALID_BUFFER;

    if(pInvoke->pFrame)
    {
	    pInvoke->pFrame = (char *)REALLOC(pInvoke->pFrame, sizeof(IF4_DATA_FRAME)+len+20);
    }
    else
    {
	    pInvoke->pFrame = (char *)MALLOC(sizeof(IF4_DATA_FRAME)+len+20);
    }
	if (pInvoke->pFrame == NULL)
	{
		XDEBUG("IF4Service: Memory Allcation Error.\xd\xa");
		return IF4ERR_INVALID_BUFFER;
	}

	pFrame = (IF4_DATA_FRAME *)pInvoke->pFrame;
	memset(pFrame, 0, sizeof(IF4_DATA_FRAME));

	try
	{
		nLength = MakeIF4MeteringDataFrame(pFrame, nCount, pszData, len, &pInvoke->nSourceLength, nSeq);
	}
	catch(...)
	{
		nLength = -1;
		XDEBUG("IF4Service: Memory Exception Error.\xd\xa");
	}

	if (nLength == -1)
		return IF4ERR_INVALID_BUFFER;
	
	pInvoke->nError	  = IF4ERR_NOERROR;
	pInvoke->nFlag	  = IF4_FLAG_ACK;	
	pInvoke->nLength  = nLength;

	XDEBUG("IF4API: Send Metering Data: %s:%0d, %d Bytes\xd\xa",
				pInvoke->szAddress, pInvoke->port, nLength);

	if (pInvoke->pClient == NULL)
	{
		pClient = new CIF4TcpClient;
		if (pClient == NULL)
			return IF4ERR_INVALID_TARGET;
		pClient->SetInvoke(pInvoke);
	}
	else
	{
		pClient = (CIF4TcpClient *)pInvoke->pClient;
	}

	ResetEvent(pInvoke->hEvent);
	if (!pClient->IsConnected())
	{
		if (!pClient->TryConnectServer(pInvoke->szAddress, pInvoke->port))
		{
			pClient->DisconnectServer();
			delete pClient;

			pInvoke->nError = IF4ERR_CANNOT_CONNECT;
			return pInvoke->nError;
		}
		pInvoke->pClient = (void *)pClient;
	}

	// Sending Command
	pClient->SendRequest();

    if (WaitForSingleObject(pInvoke->hEvent, pInvoke->timeout) == ETIMEDOUT)
         pInvoke->nError = IF4ERR_TIMEOUT;
	else if (!pClient->IsHandshake())
		pInvoke->nError = IF4ERR_CANNOT_CONNECT;
	else if (pInvoke->nCtrlCode != IF4_CTRLCODE_ACK)
		pInvoke->nError = IF4ERR_CANNOT_ACCEPT;

	return pInvoke->nError;
}

int CIF4Service::CloseSession(IF4Invoke *pInvoke)
{
	CIF4TcpClient	*pClient = NULL;

	if (pInvoke == NULL)
		return IF4ERR_INVALID_INVOKE;

	pClient = (CIF4TcpClient *)pInvoke->pClient;
	if (pClient == NULL)
		return IF4ERR_INVALID_TARGET;

	if (pClient->IsConnected())
	{
        pClient->SendEot();
        pClient->DisconnectServer();
	}
	delete pClient;
	return IF4ERR_NOERROR;
}

int CIF4Service::SendFile(IF4Invoke *pInvoke, char *pszFileName, BYTE nChannel)
{
	return IF4ERR_DO_NOT_SUPPORT;
}

//////////////////////////////////////////////////////////////////////
// CIF4Service Operations
//////////////////////////////////////////////////////////////////////

IF4_COMMAND_TABLE *CIF4Service::FindCommand(OID3 *cmd)
{
	int		i;

	for(i=0; m_pCommandTable[i].pszName; i++)
	{
		if (memcmp(&m_pCommandTable[i].oid, cmd, 3) == 0)
			return &m_pCommandTable[i];
	}
	return NULL;
}

IF4_COMMAND_TABLE *CIF4Service::FindCommandByName(char *pszName)
{
	int		i;

	if (!pszName || !*pszName)
		return NULL;

	for(i=0; m_pCommandTable[i].pszName; i++)
	{
		if (strcmp(m_pCommandTable[i].pszName, pszName) == 0)
			return &m_pCommandTable[i];
	}
	return NULL;
}
