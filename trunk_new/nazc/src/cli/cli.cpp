////////////////////////////////////////////////////////////////////////////
//
//	- 기본적인 정의는 pdk/include/cli/clitypes에 정의된다.
//	- 대부분의 함수는 common.h에서 include된다.
//	- 사용자를 위한 유틸리티 함수는 pdk/include/cli/CLIUtility.h에 정의된다.
//
/////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "cli.h"
#include "version.h"
#include "CLIInterface.h"
#include "Variable.h"
#include "cmd_table.h"
#include "CommonUtil.h"

#include "dlmsFunctions.h"
#include "kmpFunctions.h"
#include "mbusFunctions.h"

int	m_nFailCount = 0;
char szVersionString[255] = {0,};

UINT m_nLocalAgentPort = 8000;

#if     defined(__PATCH_6_2084__)
extern  BOOL m_bEnableHiddenCommand;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLI 사용자 함수 테이블 
/////////////////////////////////////////////////////////////////////////////

CLICONSTRUCT		m_CustomConstruct =
{
	szVersionString,							// MCU Version
	(void *)m_Root_node,						// Variable Node
	OnParseParameter,							// Parsing Parameters
	OnStartup,									// Startup Procedure
	OnShutdown,									// Shutdown Procedure
	OnNewSession,								// New Session Procedure
	OnDeleteSession,							// Delete Session Procedure
	DoLogin,									// Login Procedure
	OnLogin,									// Login Procedure
	OnLogout,									// Logout Procedure
	OnCommand									// Command Procedure
};

void zombie_handler(int nSignal)
{
    int     status;

    waitpid(-1, &status, WNOHANG);
}

/////////////////////////////////////////////////////////////////////////////
// CLI 메인 함수 
/////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	// 데몬 프로세스를 수행한다.
	// 데몬 프로세스는 처리 단계에 따라 사용자 루틴을 자동으로 호출한다.
	// 이후 모든 제어권은 데몬 프로세스가 가지고 필요시만 사용자 루틴을
	// 호출하여 준다.
    signal(SIGCHLD, zombie_handler);

	SET_DEBUG_MODE(1);
    GetVersionString(szVersionString);

    // Protocol Library 초기화
    PROTOCOL_DLMS_INIT;     // DLMS Protocol libary support
    PROTOCOL_MBUS_INIT;     // MBUS Protocol libary support
    PROTOCOL_KMP_INIT;      // KMP Protocol libary support  

	CLIAPI_Startup(&m_CustomConstruct, argc, argv);
	CLIAPI_Shutdown();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CLI Event Handler 
/////////////////////////////////////////////////////////////////////////////

void OnParseParameter(int argc, char **argv)
{
	// 사용자가 파라메터를 별도로 검사하는 경우
}

void OnStartup()
{
	// CLI 시작시 별도 처리가 필요한 경우
#if     defined(__PATCH_6_2084__)

    struct stat             statSt;
    if(stat("/app/conf/.cli_hidden_cmd_disable", &statSt)) {
        m_bEnableHiddenCommand= TRUE;
    }else {
        m_bEnableHiddenCommand= FALSE;
    }
#endif
    m_nLocalAgentPort = CLIAPI_GetAgentPort();
}

void OnShutdown()
{
	// CLI 종료시 별도 처리가 필요한 경우
}

void OnNewSession(CLISESSION *pSession)
{
	// CLI 세션이 연결될때 처리가 필요한 경우
}

void OnDeleteSession(CLISESSION *pSession)
{
	// CLI 세션 연결이 끊어질때 처리가 필요한 경우
}

// 비밀번호는 SetUserKey로 암호화 한다. 2009.11.04

BOOL DoLogin(CLISESSION *pSession, char *pszID, char *pszPassword)
{
	USRENTRY	*pUser;
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort, 5);
	int			nError;
    char        *pszResult;
    char        szSalt[3];

    invoke.AddParam("2.10.2", pszID);
    nError = invoke.Command("106.3", IF4_CMDATTR_REQUEST | IF4_CMDATTR_MCUIPC | IF4_CMDATTR_RESPONSE);
   
 	if (nError == IF4ERR_TIMEOUT)
	{
		m_nFailCount++;
		OUTTEXT(pSession, "ERROR(%0d): %s\xd\xa", nError, IF4API_GetErrorMessage(nError));
		if (m_nFailCount > 3)
		{
			OUTTEXT(pSession, "\xd\xa");
			OUTTEXT(pSession, "*************************************\xd\xa");
			OUTTEXT(pSession, "            EMERGENCY MODE\xd\xa");
			OUTTEXT(pSession, "*************************************\xd\xa");

            GetSalt(szSalt);
            pszResult = SetUserKey("aimir", szSalt);
			strcpy(pSession->szUser, "emergency");
			strcpy(pSession->szPassword, pszResult);	
			pSession->nGroup = 0;
			pSession->bEmergency = TRUE;
			m_pCommandHandlers = m_CliEmergencyHandlers;
			return TRUE;
		}
		return FALSE;
	}
    else if (nError != IF4ERR_NOERROR)
	{
        OUTTEXT(pSession, "ERROR(%0d): %s\r\n", nError, IF4API_GetErrorMessage(nError));
		return FALSE;
	}

	m_nFailCount = 0;
    pUser = (USRENTRY *)invoke.GetHandle()->pResult[0]->stream.p;

    ExtractSalt(pUser->usrPassword, szSalt);
    pszResult = SetUserKey(pszPassword, szSalt);

    if (strcmp(pUser->usrPassword, pszResult) != 0)
        return FALSE;

	strcpy(pSession->szPassword, pUser->usrPassword);
	pSession->nGroup = pUser->usrGroup;
	return TRUE;
}

void OnLogin(CLISESSION *pSession)
{
	OUTTEXT(pSession, "\r\nOAM-PC(%s) '%s' Login.\r\n", pSession->pszAddress, pSession->szUser);

	if (pSession->bEmergency)
		return;

	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 3);
	invoke.AddParam("1.11", pSession->pszAddress);
	invoke.AddParam("1.11", pSession->szUser);
	CLIAPI_Command(pSession, invoke.GetHandle(), "100.202");
}

void OnLogout(CLISESSION *pSession)
{
	OUTTEXT(pSession, "\r\nOAM-PC(%s): '%s' Logout.", pSession->pszAddress, pSession->szUser);

	if (pSession->bEmergency)
		return;

	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 3);
	invoke.AddParam("1.11", pSession->pszAddress);
	invoke.AddParam("1.11", pSession->szUser);
	CLIAPI_Command(pSession, invoke.GetHandle(), "100.203");
}

void OnCommand(CLISESSION *pSession, int argc, char **argv, CLIHANDLER *pCmdTable)
{
	// 개별 명령이 수행될때 처리가 필요한 경우
}

