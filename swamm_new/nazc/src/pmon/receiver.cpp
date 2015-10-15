
#include "common.h"
#include "codiapi.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "CommonUtil.h"
#include "Variable.h"

extern VAROBJECT    m_Root_node[];

/*******************************************************************************/
BOOL Initialize_CodiAPI(void);
/*******************************************************************************/

CODIDEVICE codiDevice =
{
    CODI_COMTYPE_RS232,
    "/dev/ttyS03",
    115200,
    8,
    1,
    CODI_PARITY_NONE,
    0
};

int cmdUpdateFirmware(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetFwInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdTableRejoin(HANDLE codi);
		
UINT m_nLocalAgentPort = 0;

IF4_COMMAND_TABLE   m_CommandHandler[] =
{
	{ {101,24,0},	"cmdUpdateCoordinator",		cmdUpdateFirmware, TRUE },
	{ {100,33,0},	"cmdGetFwInfo",		        cmdGetFwInfo, FALSE },

    { {0,0,0},    NULL,          NULL, 			FALSE }
};

HANDLE  m_codiHandle;
BOOL    m_bBoot;
BYTE    m_Fw = 0x00;
BYTE    m_Hw = 0x00;
BYTE    m_Build = 0x00;

void usage(char * name)
{
    fprintf(stderr,"%s [-dh]\n", name);
    fprintf(stderr,"    -d               : enable debug message\n");
    fprintf(stderr,"    -h               : display this message\n");
    exit(1);
}

BOOL Initialize_CodiAPI(void)
{
    int nError;
    BOOL bResult;

    nError = codiInit();
    if(nError != CODIERR_NOERROR)
    {
        fprintf(stderr,"CODIERR: %d %s\n", nError, codiErrorMessage(nError));
        return FALSE;
    }

    nError = codiRegister(&m_codiHandle, &codiDevice);
    if(nError != CODIERR_NOERROR)
    {
        fprintf(stderr,"CODIERR: %d %s\n", nError, codiErrorMessage(nError));
        codiUnregister(m_codiHandle);
        codiExit();
        return FALSE;
    }

    bResult = codiSetWatchdogState(m_codiHandle, FALSE);
    if(!bResult) 
    {
        fprintf(stderr,"CODIERR: Reset Watchdog fail\n");
        codiUnregister(m_codiHandle);
        codiExit();
        return FALSE;
    }

    nError = codiStartup(m_codiHandle);
    if(nError != CODIERR_NOERROR)
    {
        fprintf(stderr,"CODIERR: %d %s\n", nError, codiErrorMessage(nError));
        codiUnregister(m_codiHandle);
        codiExit();
        return FALSE;
    }

    usleep(1000000);

    nError = codiReset(m_codiHandle);
    if(nError != CODIERR_NOERROR)
    {
        fprintf(stderr,"CODIERR: %d %s\n", nError, codiErrorMessage(nError));
        codiUnregister(m_codiHandle);
        codiExit();
        return FALSE;
    }
	XDEBUG("---------- ssibal fuck!!!!!! receiver -----------\r\n");
	cmdTableRejoin(m_codiHandle); 
    return TRUE;
}

BOOL TimeSyncSensor(HANDLE codi)
{
	ENDI_TIMESYNC_PAYLOAD *pTimesync;
	BYTE	        payload[256];
	char	        szTime[30];
	int		        nError;
    time_t          now;
	struct	tm		when;

	time(&now);
	when = *localtime(&now);

	MakeTimeString(szTime, &now, TRUE);

    if(when.tm_year + 1900 < 2009) 
    {
        XDEBUG("TIMESYNC: Invalid Time (TIME=%s)\r\n", szTime);
        return FALSE;
    }
	XDEBUG("	---------	TimeSyncSensor		Receiver --------\r\n"); 
	XDEBUG("SENSOR-TIMESYNC: Multicast time (TIME=%s)\r\n", szTime); 

	pTimesync = (ENDI_TIMESYNC_PAYLOAD *)&payload;
#ifdef  __USE_BSD
	pTimesync->gmt.tz	= ntoshort((int)(when.tm_gmtoff/60));
#endif
	pTimesync->gmt.dst	= ntoshort(0);
	pTimesync->gmt.year	= ntoshort(when.tm_year + 1900);
	pTimesync->gmt.mon	= (BYTE)when.tm_mon + 1;
	pTimesync->gmt.day	= (BYTE)when.tm_mday;
	pTimesync->gmt.hour	= (BYTE)when.tm_hour;
	pTimesync->gmt.min	= (BYTE)when.tm_min;
	pTimesync->gmt.sec	= (BYTE)when.tm_sec;
	pTimesync->nazc     = ntoint(0);		// NAZC 번호
    pTimesync->type     = TIMESYNC_TYPE_LONG;

	nError = codiMulticast(codi, ENDI_DATATYPE_TIMESYNC, (BYTE *)&payload, sizeof(ENDI_TIMESYNC_PAYLOAD));
	if (nError != CODIERR_NOERROR)
	{
		XDEBUG("---------------------------------------------\r\n");
		XDEBUG("SENSOR-TIMESYNC-ERROR: %s(%d)\r\n", codiErrorMessage(nError), nError);
		XDEBUG("---------------------------------------------\r\n");
    } 
    return TRUE;
}

int cmdUpdateFirmware(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	struct	stat file_info;
	char	szFileName[128];
    int         nError;

	if (cnt != 1)
		return IF4ERR_INVALID_PARAM;

	if (pValue[0].type != VARSMI_STRING)
		return IF4ERR_INVALID_PARAM;

	memset(szFileName, 0, 128);
	strncpy(szFileName, pValue[0].stream.p, 127);

	if (!IsExists(szFileName))
		return IF4ERR_INVALID_FILENAME;
                     
	stat(szFileName, &file_info);

	XDEBUG("------ PowerControl firmware update -----\xd\xa");	
	XDEBUG("Filename = %s\xd\xa", szFileName);
	XDEBUG("    Size = %.2f K\xd\xa", (double)file_info.st_size / (double)1024);

	nError = codiDownload(m_codiHandle, CODI_MODEMTYPE_XMODEM, szFileName, "ebl");
	if (nError != CODIERR_NOERROR)
	{
		XDEBUG("------ PowerControl firmware update fail -----\xd\xa");	
		return nError;
    }

	XDEBUG("------ PowerControl firmware update done -----\xd\xa");	
	return IF4ERR_NOERROR;
}

int cmdGetFwInfo(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    FWENTRY fw;

    /** PowerControl Version Information */
    memset(&fw, 0, sizeof(FWENTRY));

    fw.fwType = FW_TYPE_POWERCONTROL;
    strcpy(fw.fwCore, "EMBER");
    fw.fwHardware = ((m_Hw & 0xF0) << 4) | (m_Hw & 0x0F) ;
    fw.fwSoftware = ((m_Fw & 0xF0) << 4) | (m_Fw & 0x0F) ;
    fw.fwBuild = m_Build;

	IF4API_AddResultFormat(pWrapper, "2.111", VARSMI_OPAQUE, &fw, sizeof(FWENTRY));

	return IF4ERR_NOERROR;
}
int cmdTableRejoin(HANDLE codi)
{
/*
	EUI64	id;
	EUI64	tempid;

	int		nError;
	int 	nCount; 
	char*	p;

	char 	str[30];
	
	FILE *fp = fopen("/app/sw/test.txt", "r");
	XDEBUG(" ------------- RECEIVER  -----------------\r\n"); 
//	codi = GetCoordinator();
//	if (codi == NULL)
//		return IF4ERR_NO_ENTRY;
	nCount = 0; 
	while (true)
	{
		fgets(str, 30, fp);
		if(strstr(str, "Freq") != NULL )	break; 
		nCount ++; 
		XDEBUG(" ------------- RECEIVER  -----------------\r\n"); 
		XDEBUG(" TEXT : %s COUNT : %d \r\n", str, nCount); 
    	memset(&id, 0, sizeof(EUI64));
    	StrToEUI64(str, &tempid);
		memset(&str, 0, sizeof(char)); 
    	ReverseEUI64(&tempid, &id);
		nError = codiSetProperty(codi, CODI_CMD_ADD_EUI64, (BYTE *)&id, sizeof(EUI64),  3000);
		
	}*/
    return 0;
}

int main(int argc, char **argv)
{
	BOOL	bOK = TRUE;
    BOOL    bDebug = FALSE;
    int     opt;
    int     nTry=0;
    time_t  nPrevTime, nCurrentTime;
	VAROBJECT 	*pObject;

    while((opt=getopt(argc, argv, "dh")) != -1) {
        switch(opt) {
            case 'd': bDebug = TRUE; break;
            case 'h':
            default :
                usage(argv[0]);
        }
    }

    if(bDebug) SET_DEBUG_MODE(0);

    VARAPI_Initialize(VARCONF_FILENAME, m_Root_node, FALSE);
    IF4API_Initialize(8003, m_CommandHandler);

    m_bBoot = FALSE;
    Initialize_CodiAPI();

	pObject = VARAPI_GetObjectByName("sysLocalPort");
	if (pObject != NULL) 
    {
        m_nLocalAgentPort = pObject->var.stream.u32;
    }
    if(m_nLocalAgentPort == 0)
    {
        m_nLocalAgentPort = 8000;
    }

    nPrevTime = 0;
    while(TRUE) {
        if(!m_bBoot) {
            if(nTry > 3) {
                codiReset(m_codiHandle);
                nTry = 0;
                usleep(60 * 1000000);
            }else nTry ++;
        } else {
            time(&nCurrentTime);
            if(labs(nCurrentTime - nPrevTime) > 60) {
                nPrevTime = nCurrentTime;
                TimeSyncSensor(m_codiHandle);
            }
        }
        usleep(10 * 1000000);
    }

	return bOK ? 0 : -1;
}
