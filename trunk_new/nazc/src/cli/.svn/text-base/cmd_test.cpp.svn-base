#include "common.h"
#include "cmd_default.h"
#include "cmd_util.h"
#include "Variable.h"
#include "CLIInterface.h"
#include "CLIUtility.h"
#include "DebugUtil.h"

extern UINT m_nLocalAgentPort;

/////////////////////////////////////////////////////////////////////////////
// Enable/Disable Function
/////////////////////////////////////////////////////////////////////////////

int cmdExit(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	OUTTEXT(pSession, "이 기능은 제품으로 나갈때는 빼주세요. ^^ Have a nice day!\xd\xa");
	if (!Confirm(pSession, "CLI를 종료하시겠습니까? (y/n): "))
		return CLIERR_OK;

	CLIAPI_Exit();
	return CLIERR_ERROR;
}

void GetAllObjectCallback(VAROBJECT *pObject, void *pParam)
{
	CLISESSION	*pSession = (CLISESSION *)pParam;
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke;
	char		szOid[20];

	if (pObject->var.oid.id1 <= 1)
		return;
	
	sprintf(szOid, "%0d.%0d.%0d",
				pObject->var.oid.id1,
				pObject->var.oid.id2,
				pObject->var.oid.id3);

//	OUTTEXT(pSession, "\xd\xa------------- GET (%s) -------------\xd\xa", szOid);
	invoke.AddParam(szOid);
	CLIAPI_Command(pSession, invoke.GetHandle(), "199.1");
	
	pInvoke = invoke.GetHandle();
	PrintOutMIBValue(pSession, pInvoke->pResultNode, pInvoke->nResultCount);
}

int cmdTestGetAll(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	VARAPI_EnumObject(GetAllObjectCallback, (void *)pSession);
	return CLIERR_OK;
}

int cmdEventTest(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	
	invoke.AddParam("1.11", argv[0]);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.2"))
		return CLIERR_OK;

	return CLIERR_OK;
}

int cmdMeteringAll(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.4"))
		return CLIERR_OK;

	return CLIERR_OK;
}

int cmdMeterUpload(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	TIMESTAMP	tmStart, tmEnd;

	if ((argc != 0) && (argc != 4))
	{
    	OUTTEXT(pSession, "Need more parameter.\xd\xa");
		return CLIERR_OK;
	}

	if (argc == 0)
	{
		if (!Confirm(pSession, "Do you want one days portin(y/n): "))
			return CLIERR_OK;
	}
	else
	{
		if (!Confirm(pSession, MSG_DEFAULT))
			return CLIERR_OK;
	}

	if (argc == 4)
	{ 
		memset(&tmStart, 0, sizeof(TIMESTAMP));	
		tmStart.year = atoi(argv[0]);
		tmStart.mon	 = atoi(argv[1]);
		tmStart.day  = atoi(argv[2]);
		tmStart.hour = atoi(argv[3]);
		memcpy(&tmEnd, &tmStart, sizeof(TIMESTAMP));
		tmEnd.min	 = 59;
		tmEnd.sec	 = 59;

		invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmStart, sizeof(TIMESTAMP));
		invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmEnd, sizeof(TIMESTAMP));
	}

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.6"))
		return CLIERR_OK;

	return CLIERR_OK;
}

int cmdPageRead(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	return CLIERR_OK;
}

int cmdPageReadAll(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	return CLIERR_OK;
}

int cmdRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	TIMESTAMP	tmRecovery;

	if ((argc != 0) && (argc != 3))
	{
    	OUTTEXT(pSession, "Need more parameter.\xd\xa");
		return CLIERR_OK;
	}

  	if (!Confirm(pSession, MSG_DEFAULT))
    	return CLIERR_OK;

	GetTimestamp(&tmRecovery, NULL);
	if (argc == 3)
	{ 
		memset(&tmRecovery, 0, sizeof(TIMESTAMP));	
		tmRecovery.year = atoi(argv[0]);
		tmRecovery.mon  = atoi(argv[1]);
		tmRecovery.day  = atoi(argv[2]);
	}

	invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmRecovery, sizeof(TIMESTAMP));
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.3"))
		return CLIERR_OK;

	return CLIERR_OK;
}

int cmdGetMeterSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    EUI64       id;
    int         nOption=0;
    int         nOffset=0;
    int         nCount=0;
    int         idx=0;

	if ((argc <  1))
	{
    	OUTTEXT(pSession, "Need more parameter.\xd\xa");
		return CLIERR_OK;
	}

  	if (!Confirm(pSession, MSG_DEFAULT))
    	return CLIERR_OK;

    if (argc > idx) {
        StrToEUI64(argv[idx++], &id);
    }
    if (argc > idx) {
        nOption = atoi(argv[idx++]);
    }
    if (argc > idx) {
        nOffset = atoi(argv[idx++]);
    }
    if (argc > idx) {
        nCount = atoi(argv[idx++]);
    }

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	invoke.AddParam("1.9", nOption);
	invoke.AddParam("1.9", nOffset);
	invoke.AddParam("1.9", nCount);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.13"))
		return CLIERR_OK;

	return CLIERR_OK;
}

int cmdPageWrite(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	return CLIERR_OK;
}

int cmdGetConfig(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke;
	char		szFileName[128];

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.26"))
		return CLIERR_OK;

	memset(szFileName, 0, 128);
	pInvoke = (IF4Invoke *)invoke.GetHandle();
	memcpy(szFileName, pInvoke->pResult[0]->stream.p, pInvoke->pResult[0]->len);
	
	OUTTEXT(pSession, "FILENAME = %s\r\n", szFileName);
	OUTTEXT(pSession, "LENGTH   = %d\r\n", pInvoke->pResult[1]->stream.u32);
	return CLIERR_OK;
}

int cmdSetConfig(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.27"))
		return CLIERR_OK;
	return CLIERR_OK;
}

int cmdSetAddress(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	return CLIERR_OK;
}

int cmdUpgradeCancel(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	if (!Confirm(pSession, "Are you sure (y/n): "))
		return CLIERR_OK;

	invoke.AddParamFormat("1.11", VARSMI_STRING, (void *)argv[0], strlen(argv[0]));
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "198.15"))
		return CLIERR_OK;

	return CLIERR_OK;
}

