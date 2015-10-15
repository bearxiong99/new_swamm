#include "common.h"
#include "cmd_default.h"
#include "cmd_show.h"
#include "cmd_util.h"
#include "CLIInterface.h"
#include "IcmpPing.h"
#include "Chunk.h"

extern  char m_szUpdateFileName[];
extern int check_id(CLISESSION *pSession, char *pszValue);
extern UINT m_nLocalAgentPort;

/////////////////////////////////////////////////////////////////////////////
// Enable/Disable Function
/////////////////////////////////////////////////////////////////////////////

int cmdInstall(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	OUTTEXT(pSession, "+--------------------------------------------------------+\xd\xa");
	OUTTEXT(pSession, "|                    AIMIR MCU INSTALL                   |\xd\xa");
	OUTTEXT(pSession, "+--------------------------------------------------------+\xd\xa");
	OUTTEXT(pSession, "\xd\xa");
	cmdShowSystem(pSession, 0, NULL, NULL);
	OUTTEXT(pSession, "\xd\xa");

	if (!Confirm(pSession, MSG_INSTALL_MCU))
		return CLIERR_OK;
	
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.200"))
		return CLIERR_OK;

	OUTTEXT(pSession, MSG_REQ_INSTALL_MCU "\r\n");
	return CLIERR_OK;
}

int cmdUninstall(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	OUTTEXT(pSession, "+--------------------------------------------------------+\xd\xa");
	OUTTEXT(pSession, "|                   AIMIR MCU UNINSTALL                  |\xd\xa");
	OUTTEXT(pSession, "+--------------------------------------------------------+\xd\xa");
	OUTTEXT(pSession, "\xd\xa");

	if (!Confirm(pSession, MSG_UNINSTALL_MCU))
		return CLIERR_OK;

    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.201"))
		return CLIERR_OK;

	OUTTEXT(pSession, MSG_REQ_UNINSTALL_MCU "\r\n");
	return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Enable/Disable Function
/////////////////////////////////////////////////////////////////////////////

int cmdHelp(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    OUTTEXT(pSession, "Command :\xd\xa");
    CLIAPI_DisplayHelp(pSession, NULL);
    OUTTEXT(pSession, "\xd\xa");

    OUTTEXT(pSession, "Function Key :\xd\xa");
    OUTTEXT(pSession, "  <Backspace>    Delete char            ?           Inline help\xd\xa");
    OUTTEXT(pSession, "  <Tab>          Auto Complete          Ctrl+D      Logout\xd\xa");
	return CLIERR_OK;
}

int cmdHistory(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CLIAPI_DisplayHistory(pSession);
	return CLIERR_OK;
}

int cmdLogout(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	return CLIERR_ERROR;
}

int cmdDiagnosis(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	BYTE	*pState;

    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.1"))
		return CLIERR_OK;
	
	pState = (BYTE *)invoke.GetHandle()->pResultNode[0].stream.p;
    OUTTEXT(pSession, "  MCU State = %s\xd\xa", pState[0] == 0 ? "Normal" : "Abnormal");
    OUTTEXT(pSession, " CODI State = %s\xd\xa", pState[1] == 0 ? "Normal" : "Abnormal");
    OUTTEXT(pSession, "      Power = %s\xd\xa", pState[3] == 0 ? "Normal" : "Power Fail");
    OUTTEXT(pSession, "    Battery = %s\xd\xa", pState[4] == 0 ? "Normal" : "Low Battery");
    /** 온도 관련 정보는 더이상 제공하지 않는다 
    OUTTEXT(pSession, "Temperature = %s\xd\xa", pState[5] == 0 ? "Normal" : "Abnormal");
    */
    OUTTEXT(pSession, "     Memory = %s\xd\xa", pState[6] == 0 ? "Normal" : "Full");
    OUTTEXT(pSession, "      Flash = %s\xd\xa", pState[7] == 0 ? "Normal" : "Full");
    OUTTEXT(pSession, "   Ethernet = %s\xd\xa", pState[9] == 0 ? "Link Up" : "Link Down");

	switch(pState[8]) {
	  case 0 : OUTTEXT(pSession, "     Mobile = Normal\xd\xa"); break;
	  case 1 : OUTTEXT(pSession, "     Mobile = Abnormal (NO MODEM)\xd\xa"); break;
	  case 2 : OUTTEXT(pSession, "     Mobile = Abnormal (NO SIM CARD)\xd\xa"); break;
	  case 3 : OUTTEXT(pSession, "     Mobile = Abnormal (NOT READY)\xd\xa"); break;
	  case 4 : OUTTEXT(pSession, "     Mobile = Abnormal (BAD CSQ)\xd\xa"); break;
	}
	return CLIERR_OK;
}

int cmdGarbageCleaning(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.32"))
		return CLIERR_OK;
	
	return CLIERR_OK;
}

int cmdReset(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	OUTTEXT(pSession, MSG_RESET_WARNING "\r\n");

	if (!Confirm(pSession, MSG_CMD_RESET_MCU))
		return CLIERR_OK;
	
	OUTTEXT(pSession, MSG_RUN_RESET_MCU "\r\n");

    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.2"))
		return CLIERR_OK;

	usleep(60000000);
	ResetSystem();

	// Reset Wait
	for(;;) usleep(1000000);
	return CLIERR_OK;
}

int cmdShutdown(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	OUTTEXT(pSession, MSG_SHUTDOWN_WARNING "\r\n");

	if (!Confirm(pSession, MSG_CMD_SHUTDOWN_MCU))
		return CLIERR_OK;
	
	OUTTEXT(pSession, MSG_RUN_SHUTDOWN_MCU "\r\n");

    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.3"))
		return CLIERR_OK;

	// Reset Wait
	for(;;) usleep(1000000);
	return CLIERR_OK;
}

int cmdFactoryDefault(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	OUTTEXT(pSession, MSG_FACTORY_DEFAULT "\r\n");
	if (!Confirm(pSession, MSG_CMD_FACTORY_DEFAULT))
		return CLIERR_OK;
	
	OUTTEXT(pSession, MSG_RUN_FACTORY_DEFAULT "\r\n");

    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.4"))
		return CLIERR_OK;

	// Reset Wait
	for(;;) usleep(1000000);
	return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Enable/Disable Function
/////////////////////////////////////////////////////////////////////////////

int cmdKillProcess(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	int		i;
    pid_t   pid;

	for(i=0; i<argc; i++)
	{
        pid = (pid_t)strtol(argv[i],(char **)NULL,10);
        if(pid > 0) {
            kill(pid, SIGTERM);
        }
	}
	return CLIERR_OK;
}

int cmdPing(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIcmpPing   ping;
	struct      timeval atime;
	struct      sockaddr_in addr;
	int         i, ret;
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    MIBValue    *pValue;
	char    	szAddress[32] = "";

	if ((strcmp(argv[0], "FEP") == 0)||(strcmp(argv[0], "fep") == 0))
	{
		invoke.AddParam("2.1");
	  	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.4"))
			return CLIERR_OK;

		pValue = invoke.ResultAtName("sysServer");
		sprintf(szAddress, "%0d.%0d.%0d.%0d",
					pValue->stream.p[0], pValue->stream.p[1],
					pValue->stream.p[2], pValue->stream.p[3]);
	} 
	else
	{
		strcpy(szAddress, argv[0]);
	}

	ping.IcmpInit();
	for(i=0; i<3; i++)
	{
		ret = ping.CheckIcmp(szAddress, &atime, &addr);
		if (ret != -1)
		{
    		OUTTEXT(pSession, "PING: Reply from %s: %d sec %d msc.\xd\xa",
					argv[0], atime.tv_sec, atime.tv_usec);
		}
		else
		{
			OUTTEXT(pSession, "PING: Request timed out. (%s)\xd\xa", argv[0]);
		}
	}
	ping.IcmpClose();
	return CLIERR_OK;
}

int cmdFirmwareDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (!Confirm(pSession, MSG_CMD_FIRMWARE_UPDATE))
		return CLIERR_OK;

	OUTTEXT(pSession, "\r\n" MSG_INFO_FIRMWARE_UPDATE "\r\n");

	if (!CLIAPI_FirmwareDownload(pSession))
		return CLIERR_OK;

	OUTTEXT(pSession, "\r\n" MSG_RUN_FIRMWARE_UPDATE "\r\n");

	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort, 5*60);
	invoke.AddParam("1.11", pSession->pszAddress);			// Source address
	invoke.AddParam("1.11", m_szUpdateFileName);			// Firmware filename
	invoke.AddParam("1.4", (BYTE)1);						// 0:FEP, 1:MCU, 2-255:Not defined

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.1"))
	    return CLIERR_OK;

	return CLIERR_OK;
}

int cmdCoordinatorFirmwareDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort, 5*60);
	invoke.AddParam("1.11", argv[0]);			// Firmware filename

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "101.24"))
	    return CLIERR_OK;
	return CLIERR_OK;
}

int cmdPowerControlFirmwareDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	CIF4Invoke  invoke("127.0.0.1", 8003, 5*60);
	invoke.AddParam("1.11", argv[0]);			// Firmware filename

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "101.24"))
	    return CLIERR_OK;
	return CLIERR_OK;
}

int cmdSensorFirmwareDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    EUI64 id;
    int i;
    int major=0, minor=0, build=0;
    char trId[16];
    char path[255 + 10];
    time_t now=0;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

    if(strlen(argv[4]) > (255-7)) {
	    OUTTEXT(pSession, "Filename is too long\xd\xa");
		return CLIERR_OK;
    }

    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    time(&now);
    sprintf(trId,"t%ld",now);

    invoke.AddParam("1.11", trId);

    invoke.AddParam("1.11", argv[0]);

    invoke.AddParam("1.4", (BYTE) 2); // OTA_TRANSFER_UNICAST 
    invoke.AddParam("1.6", (UINT) 0x1F); // OTA_STEP_ALL
    invoke.AddParam("1.4", (BYTE) 1); // Multicast Write Cout
    invoke.AddParam("1.6", (UINT) 3); // Max Retry Count
    invoke.AddParam("1.4", (BYTE) 2); // Thread Count
    invoke.AddParam("1.4", (BYTE) 1); // OTA_INSTALL_REINSTALL

    major = minor = 0;
    sscanf(argv[1],"%d.%d", &major, &minor);
    invoke.AddParam("1.5",(WORD) (((major & 0xff) << 8) + (minor & 0xff))); // Old H/W

    major = minor = 0;
    sscanf(argv[2],"%d.%d", &major, &minor);
    invoke.AddParam("1.5",(WORD) (((major & 0xff) << 8) + (minor & 0xff))); // Old F/W

    build = (int) strtol(argv[3],(char **)NULL,10);
    invoke.AddParam("1.5",(WORD)build);         // Build

    sprintf(path, "file://%s", argv[4]);
    invoke.AddParam("1.11",path);         // File Name

    for(i=5; i<argc; i++)
    {
        char *str, *token;
        char *saveptr = NULL;
        for(str=argv[i]; ; str = NULL)
        {
            token = strtok_r(str, ",", &saveptr);
            if(token == NULL) break;
  	        StrToEUI64(token, &id);
            invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        }
    }

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "198.18"))
	    return CLIERR_OK;

	OUTTEXT(pSession, "Request ID : %s\xd\xa", trId);

	return CLIERR_OK;
}

int cmdScanSensor(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    EUI64 id;
    int idx=0;
    int major=0, minor=0, build=0;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort, 50*60);

    if(check_id(NULL, argv[idx])) { // EUI64 ID
  	    StrToEUI64(argv[idx], &id);
        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Parser
        invoke.AddParam("1.11", argv[idx]);
    }
    idx ++;

    if(argc > 1) {
        major = minor = 0;
        sscanf(argv[idx],"%d.%d", &major, &minor);
        invoke.AddParam("1.5",(WORD) (((major & 0xff) << 8) + (minor & 0xff)));
        idx++;
    }
    if(argc > 2) {
        build = (int) strtol(argv[idx],(char **)NULL,10);
        invoke.AddParam("1.5",(WORD)build);
        idx++;
    }

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.35"))
	    return CLIERR_OK;
	return CLIERR_OK;
}

int cmdPolicyDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	if (!CLIAPI_PolicyDownload(pSession))
		return CLIERR_OK;

	// 향후 추가 예정
	return CLIERR_OK;
}

int cmdStore(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	if (!CLIAPI_Store(pSession))
		return CLIERR_OK;
	return CLIERR_OK;
}

int cmdShell(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CChunk	chunk;
	char	*p;
	int		nSeek, len, length;
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);

	invoke.AddParam("1.11", argv[0]);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.4"))
		return CLIERR_OK;

	chunk.Add((char *)invoke.GetHandle()->pResultNode[0].stream.p,
			  invoke.GetHandle()->pResultNode[0].len);
	chunk.Add("\x0", 1);
	
	p = chunk.GetBuffer();
	len = chunk.GetSize();
	
	for(nSeek=0; nSeek<len; nSeek+=256)
	{
		length = MIN(256, len-nSeek);
    	PUT(pSession, p+nSeek, length);
	}
	return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Enable/Disable Function
/////////////////////////////////////////////////////////////////////////////

int cmdAddUser(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (pSession->nGroup != 0)
        return CLIERR_OK;

    invoke.AddParam("2.10.2", argv[0]);
    invoke.AddParam("2.10.3", argv[1]);
    invoke.AddParam("2.10.4", atoi(argv[2]));

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "106.1"))
        return CLIERR_OK;

    OUTTEXT(pSession, "User added.\r\n");
	return CLIERR_OK;
}

int cmdDeleteUser(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if (pSession->nGroup != 0)
        return CLIERR_OK;

    invoke.AddParam("2.10.2", argv[0]);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "106.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "User deleted.\r\n");
	return CLIERR_OK;
}

int cmdSetPassword(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    CIF4Invoke  invoke1("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.10.2", pSession->szUser);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "106.3"))
        return CLIERR_OK;

    invoke1.AddParam("2.10.2", pSession->szUser);
    invoke1.AddParam("2.10.3", argv[0]);
    invoke1.AddParam("2.10.4", pSession->nGroup);
    if (!CLIAPI_Command(pSession, invoke1.GetHandle(), "106.4"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Password changed.\r\n");
	return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Enable/Disable Function
/////////////////////////////////////////////////////////////////////////////

int cmdDisableAutoLogout(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	pSession->bDebugMode = TRUE;
	return CLIERR_OK;
}

int cmdEnableAutoLogout(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	pSession->bDebugMode = FALSE;
	return CLIERR_OK;
}

int cmdDisableMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	invoke.AddParam("2.2.13", FALSE);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
		return CLIERR_OK;
	
    OUTTEXT(pSession, MSG_RUN_MONITOR_DISABLE "\r\n");
	return CLIERR_OK;
}

int cmdEnableMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	invoke.AddParam("2.2.13", TRUE);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
		return CLIERR_OK;

    OUTTEXT(pSession, MSG_RUN_MONITOR_ENABLE "\r\n");
	return CLIERR_OK;
}

#define SAVE_FILENAME				"/tmp/config.tar.gz"

#define BACKUP_TYPE_ALL				0
#define BACKUP_TYPE_CONFIG			1

int MakeBackupFile(CLISESSION *pSession, const char *pszFileName, int nType)
{
    char    szCommand[256];
	char	szName[64], *p;
    int     pid, child, status;

    strcpy(szName, pszFileName);
    p = strstr(szName, ".gz");
    if (p != NULL) *p = '\0';

    if (IsExists(szName))
        unlink(szName);

    if (IsExists(pszFileName))
        unlink(pszFileName);

	OUTTEXT(pSession, "\r\n");
	if ((nType == BACKUP_TYPE_ALL) || (nType == BACKUP_TYPE_CONFIG))
	{
	    OUTTEXT(pSession, "Save configuration.\r\n");

	    VARAPI_Save(TRUE);

	    usleep(1000000);
	    system("sync");
	    usleep(1000000);
	    system("sync");
	}

    chdir("/app");
    OUTTEXT(pSession, "Make compress file.\r\n");

	sprintf(szCommand, "tar cf %s", szName);
	if ((nType == BACKUP_TYPE_ALL) || (nType == BACKUP_TYPE_CONFIG))
	{
		// 환경 파일
		strcat(szCommand, " conf member sw/ppp/chatscript sw/ppp/peers");
		if (IsExists("/app/sw/phonelist")) strcat(szCommand, " sw/phonelist");
	}

	if (nType == BACKUP_TYPE_ALL)
	{
		// 검침 데이터와 로그
		strcat(szCommand, " data log");
	}

	strcat(szCommand, " > /tmp/result");
	system(szCommand);

    sprintf(szCommand, "gzip %s > /tmp/result", szName);
    system(szCommand);
    chdir("/app/sw");

    OUTTEXT(pSession, "Upload file.\r\n");
    pid = Spawn(child, 1, "/tmp");
    if (pid > 0)
    {   
        execl("/usr/bin/sz", "sz", pszFileName, NULL);
        exit(0);
    }
    
    while(waitpid(child, &status, WNOHANG) == 0)
        usleep(1000000);

	unlink(pszFileName);
	return CLIERR_OK;
}

int cmdBackupConfiguration(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (pSession->nType == CLITYPE_TELNET)
	{
    	OUTTEXT(pSession, "Backup does not support telnet mode.\r\n");
        return CLIERR_OK;
	}
	
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

	return MakeBackupFile(pSession, SAVE_FILENAME, BACKUP_TYPE_CONFIG);
}

int cmdBackupAll(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (pSession->nType == CLITYPE_TELNET)
	{
    	OUTTEXT(pSession, "Backup does not support telnet mode.\r\n");
        return CLIERR_OK;
	}
	
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

	return MakeBackupFile(pSession, SAVE_FILENAME, BACKUP_TYPE_ALL);
}

int cmdRestore(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    WORKSESSION *pWorker;
    char    szCommand[128];
    int     pid, child, status;
    
	if (pSession->nType == CLITYPE_TELNET)
	{
    	OUTTEXT(pSession, "Restore does not support telnet mode.\r\n");
        return CLIERR_OK;
	}
	
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    unlink(SAVE_FILENAME);
    pid = Spawn(child, 1, "/tmp");
    if (pid > 0)
    {   
        if (pSession->nType == CLITYPE_TELNET)
        {   
            pWorker = (WORKSESSION *)pSession->pSession;
            dup2(pWorker->sSocket, STDIN_FILENO);
            dup2(pWorker->sSocket, STDOUT_FILENO);
            dup2(pWorker->sSocket, STDERR_FILENO);
        }
#if defined(__TI_AM335X__)
        execl("/usr/bin/lrz", "lrz", "-b", "-Z", "-y", NULL);
#else
        execl("/usr/bin/rz", "rz", "-b", "-Z", "-y", NULL);
#endif
        exit(0);
    }
    
    while(waitpid(child, &status, WNOHANG) == 0)
        usleep(1000000);

    if (!IsExists(SAVE_FILENAME))
    {   
        printf("\r\n**** Configration file not found: %s ****\r\n", SAVE_FILENAME);
        return IF4ERR_INVALID_FILENAME;
    }
    
    usleep(1000000);

    printf("\r\n");
    printf("Remove old configuration.\r\n");
    
    VARAPI_EnableModify(FALSE);
    system("rm -rf /app/conf");
    system("rm -rf /app/member");
    system("rm -rf /app/data");
    system("rm -rf /app/event");
    system("rm -rf /app/log");
    system("rm -f /app/sw/apn");
    system("rm -f /app/sw/phonelist");
    
    printf("Make new configuration.\r\n");
    chdir("/app");
    sprintf(szCommand, "tar zxvf %s > /tmp/result", SAVE_FILENAME);
    system(szCommand); 
    system("/bin/chown -R root:root /app/conf");
    system("/bin/chown -R root:root /app/sw");
    system("/bin/chown -R root:root /app/data");
    system("/bin/chown -R root:root /app/log");
    unlink(SAVE_FILENAME);
    chdir("/app/sw");
    
    printf("Sync ...\r\n");
    system("sync");
    usleep(1000000);
    system("sync");
    usleep(1000000);
    system("sync");
    
    printf("Reboot.\r\n");
	ResetSystem();
	return CLIERR_OK;
}


