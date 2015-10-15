#include <arpa/inet.h>
#include "common.h"
#include "Variable.h"
#include "cmd_set.h"
#include "cmd_message.h"
#include "CLIInterface.h"
#include "CommonUtil.h"
#include "codiapi/codiapi.h"
#include "cmd_define.h"
#include "cmd_util.h"
#include "CRC16.h"
#include "MemoryDebug.h"

extern int check_id(CLISESSION *pSession, char *pszValue);
extern UINT m_nLocalAgentPort;
#if !defined(__DISABLE_SSL__)
extern BYTE		m_nSecurityState;
#endif


char m_szDumpFile[256] = {0,};

/////////////////////////////////////////////////////////////////////////////
// System Function
/////////////////////////////////////////////////////////////////////////////

int cmdSetId(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nMcuID;

    OUTTEXT(pSession, MSG_SET_MCU_ID "\r\n");

    if (!Confirm(pSession, MSG_CMD_SET_MCU_ID))
        return CLIERR_OK;

    nMcuID = (UINT)strtol(argv[0], (char **)NULL, 10);    
    invoke.AddParam("2.1.2", nMcuID);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_MCU_ID "\r\n", nMcuID);
    return CLIERR_OK;
}

int cmdSetName(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.1.4", argv[0]);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_MCU_NAME "\r\n", argv[0]);
    return CLIERR_OK;
}

int cmdSetDescr(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.1.5", argv[0]);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_MCU_DESCR "\r\n", argv[0]);
    return CLIERR_OK;
}

int cmdSetLocation(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    invoke.AddParam("2.1.6", argv[0]);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_MCU_LOCATION "\r\n", argv[0]);
    return CLIERR_OK;
}

int cmdSetContact(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.1.7", argv[0]);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_MCU_CONTACT "\r\n", argv[0]);
    return CLIERR_OK;
}

int cmdSetTime(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    TIMESTAMP    tmNow;
    char        szTime[30];

    tmNow.year    = (WORD)strtol(argv[0], (char **)NULL, 10);
    tmNow.mon    = (BYTE)strtol(argv[1], (char **)NULL, 10);
    tmNow.day    = (BYTE)strtol(argv[2], (char **)NULL, 10);
    tmNow.hour    = (BYTE)strtol(argv[3], (char **)NULL, 10);
    tmNow.min    = (BYTE)strtol(argv[4], (char **)NULL, 10);
    tmNow.sec    = (BYTE)strtol(argv[5], (char **)NULL, 10);

    invoke.AddParamFormat("2.1.16", VARSMI_TIMESTAMP, &tmNow, sizeof(TIMESTAMP));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    sprintf(szTime, "%04d/%02d/%02d %02d:%02d:%02d",
            tmNow.year, tmNow.mon, tmNow.day,
            tmNow.hour, tmNow.min, tmNow.sec);

    OUTTEXT(pSession, MSG_INFO_SET_MCU_TIME "\r\n", szTime);
    return CLIERR_OK;
}

int cmdSetTimezone(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    short       tzone;

    tzone = (short) strtol(argv[0], (char **)NULL, 10);
    if ((tzone > 12) || (tzone < -11))
    {
        OUTTEXT(pSession, "Out of range.\r\n");
        return CLIERR_OK;
    }

    if (!Confirm(pSession, MSG_TIMEZONE_WARNING))
        return CLIERR_OK;

    invoke.AddParam("2.1.33", tzone * 60);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "Timezone changed.\r\n");
    return CLIERR_OK;
}

int cmdSetOpMode(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        mode;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(!strcasecmp(argv[0],"normal")) mode = OP_MODE_NORMAL;
    else if(!strcasecmp(argv[0],"test")) mode = OP_MODE_TEST;
    /** Issue #2020: TimeSync Strategy가 추가되면서 Op Mode 중 Pulse Mesh는 
      * Deprecate 된다.
      * else if(!strcasecmp(argv[0],"pulse-mesh")) mode = OP_MODE_PULSE_MESH; 
      */
    else mode = (BYTE) strtol(argv[0], (char **)NULL, 10);

    switch(mode) {
        case OP_MODE_NORMAL:
        case OP_MODE_TEST:
            break;
        default: 
            OUTTEXT(pSession, "Out of range.\r\n");
            return CLIERR_OK;
    }

    invoke.AddParam("2.1.34", mode);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "Operation mode changed.\r\n");
    return CLIERR_OK;
}

/** Issue #2421: 집중기 Power Type을 설정한다 */
int cmdSetPowerType(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        powertype;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    powertype = (UINT) strtol(argv[0], (char **)NULL, 10);

    invoke.AddParam("2.1.35", powertype);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "Concentrator power type changed.\r\n");
    return CLIERR_OK;
}

int cmdSetTempRange(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    double        fMin, fMax;
    int            nMin, nMax;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    fMin = atof(argv[0]);
    fMax = atof(argv[1]);

    nMin = (int)(fMin * 10);
    nMax = (int)(fMax * 10);

    if ((nMin < -550) || (nMax > 1250))
        return CLIERR_INVALID_PARAM;

    if (nMin > nMax)
        return CLIERR_INVALID_PARAM;

    if (nMax < 0)
        return CLIERR_INVALID_PARAM;
    
    invoke.AddParam("2.1.18", nMin);
    invoke.AddParam("2.1.19", nMax);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_TEMP_RANGE "\r\n", fMin, fMax);
    return CLIERR_OK;
}

int cmdSetMobileType(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        nType = 0;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if (strcasecmp(argv[0], "none") == 0)
    {
        nType = MOBILE_TYPE_NONE;          
    }
    else if (strcasecmp(argv[0], "gsm") == 0)
    {
        nType = MOBILE_TYPE_GSM;
    }
    else if (strcasecmp(argv[0], "gprs") == 0)
    {
        nType = MOBILE_TYPE_GSM;
    }
    else if (strcasecmp(argv[0], "cdma") == 0)
    {
        nType = MOBILE_TYPE_CDMA;
    }
    else if (strcasecmp(argv[0], "pstn") == 0)
    {
        nType = MOBILE_TYPE_PSTN;
    }
    else 
    {
        nType = (BYTE)strtol(argv[0], (char **)NULL, 10);
    }

#if 0
// Issue #959 : TI AM3352 Porting
#if defined(__TI_AM335X__) 
    if(nType == MOBILE_TYPE_PSTN)
    {
        OUTTEXT(pSession, MSG_INFO_INVALID_VALUE "\r\n", argv[0]);
        return CLIERR_INVALID_PARAM;
    }
#endif
#endif

    invoke.AddParam("2.1.12", nType);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_ACCESS_TYPE "\r\n", MSGAPI_GetMobileType(nType));
    return CLIERR_OK;
}

int cmdSetMobileMode(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        nMode = 0;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if (strcasecmp(argv[0], "csd") == 0)
    {
        nMode = MOBILE_MODE_CSD;          //ETHER_TYPE_LAN = 0 
    }
    else if (strcasecmp(argv[0], "packet") == 0)
    {
        nMode = MOBILE_MODE_PACKET;
    }
    else if (strcasecmp(argv[0], "always") == 0)
    {
        nMode = MOBILE_MODE_ALWAYS;
    }
    else 
    {
        nMode = (BYTE)strtol(argv[0], (char **)NULL, 10);
    }

// Issue #959 : TI AM3352 Porting
// Issue #1264 : TI AM3352에서는 ALWAYS ON 만 지원한다.
#if defined(__TI_AM335X__) 
    if(nMode != MOBILE_MODE_ALWAYS)
    {
        OUTTEXT(pSession, MSG_INFO_INVALID_VALUE "\r\n", argv[0]);
        return CLIERR_INVALID_PARAM;
    }
#endif

    invoke.AddParam("2.1.13", nMode);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_ACCESS_MODE "\r\n", MSGAPI_GetMobileMode(nMode));
    return CLIERR_OK;
}

int cmdSetMobileModule(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    int idx=0;
    if(strcasecmp(argv[idx],"none")==0)
    {
        invoke.AddParamFormat("2.12.1", VARSMI_STRING, "", 0);
    }
    else
    {
        invoke.AddParamFormat("2.12.1", VARSMI_STRING, argv[idx], strlen(argv[idx]));
    }
    idx++;
    invoke.AddParam("2.12.2", (UINT)strtol(argv[idx],(char**)NULL, 16)); idx++;
    invoke.AddParamFormat("2.12.3", VARSMI_STRING, argv[idx], strlen(argv[idx])); idx++;
    invoke.AddParam("2.12.4", (UINT)strtol(argv[idx],(char**)NULL, 10)); idx++;

    if(argc > 4)
    {
        invoke.AddParamFormat("2.12.5", VARSMI_STRING, argv[idx], strlen(argv[idx])); idx++;
    }
    else
    {
        invoke.AddParamFormat("2.12.5", VARSMI_STRING, "", 0); ;
    }
    if(argc > 5)
    {
        invoke.AddParam("2.12.6", (UINT)strtol(argv[idx],(char**)NULL, 10)); idx++;
    }
    else
    {
        invoke.AddParam("2.12.6", (UINT)0);
    }

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_RUN_RESET_MCU "\r\n");

	usleep(60000000);
	ResetSystem();

	// Reset Wait
	for(;;) usleep(1000000);

    return CLIERR_OK;
}

int cmdSetMobileAPN(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);
    char        szAPN[65];

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    memset(szAPN, 0, 65);
    strncpy(szAPN, argv[0], 64);

    invoke.AddParam("2.1.29", szAPN);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_MOBILE_APN "\r\n", szAPN);
    return CLIERR_OK;
}

int cmdSetMobileNumber(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.1.10", argv[0]);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_MOBILE_NO "\r\n", argv[0]);
    return CLIERR_OK;
}

int cmdSetServerAddress(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParamFormat("2.1.20", VARSMI_STRING, argv[0], strlen(argv[0]));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_FEP_IP "\r\n", argv[0]);
    return CLIERR_OK;
}

int cmdSetServerPort(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nPort;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nPort = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.1.21", nPort);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_FEP_PORT "\r\n", nPort);
    return CLIERR_OK;
}

int cmdSetServerAlarmPort(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nPort;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nPort = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.1.22", nPort);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_FEP_ALARM_PORT "\r\n", nPort);
    return CLIERR_OK;
}

#if !defined(__DISABLE_SSL__)
//  [9/17/2010 DHKim]
int cmdSetServerSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	UINT		nSvrSecurity=0, nSecurityPort = 3939/*, nNormalPort = 8000*/;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(!strcasecmp(argv[0],"on") || !strcasecmp(argv[0], "true") || !strcasecmp(argv[0], "high"))
    {
        nSvrSecurity = 1;
    } 
    else
    {
	    nSvrSecurity = (UINT)strtol(argv[0], (char **)NULL, 10);
    }
    
    if(nSvrSecurity == 1){
        
        /*
        invoke.AddParam("2.1.21", nSecurityPort);
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
            return CLIERR_OK;
        
        invoke.AddParam("2.1.22", nSecurityPort);
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
            return CLIERR_OK;
            */

        invoke.AddParam("2.1.37", (BYTE)nSvrSecurity);
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
	    	return CLIERR_OK;

        m_nSecurityState = 1;
    }
    else
    {
        /*
        invoke.AddParam("2.1.21", nNormalPort);
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
            return CLIERR_OK;
        
        invoke.AddParam("2.1.22", nNormalPort);
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
            return CLIERR_OK;
            */

        invoke.AddParam("2.1.37", (BYTE)nSvrSecurity);
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
	    	return CLIERR_OK;

        m_nSecurityState = 0;
    }

    switch(nSvrSecurity)
    {
        case 0: OUTTEXT(pSession, MSG_INFO_SET_FEP_SECURITY_STATE "\r\n", "Normal Mode"); break;
        case 1: OUTTEXT(pSession, MSG_INFO_SET_FEP_SECURITY_STATE "\r\n", "SSL Security Mode"); break;
    }

    OUTTEXT(pSession, MSG_INFO_SET_MCU_LISTEN_PORT "\r\n", nSecurityPort);
    if (!Confirm(pSession, MSG_CMD_MCU_REBOOT))
        return CLIERR_OK;

    CIF4Invoke  invoke1("127.0.0.1", m_nLocalAgentPort);
    if (!CLIAPI_Command(pSession, invoke1.GetHandle(), "100.2"))
        return CLIERR_OK;

	return CLIERR_OK;
}
#endif
//////////////////////////////////////////////////////////////////////////

int cmdSetLocalPort(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nPort;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nPort = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.1.23", nPort);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_MCU_LISTEN_PORT "\r\n", nPort);
    if (!Confirm(pSession, MSG_CMD_MCU_REBOOT))
        return CLIERR_OK;

    CIF4Invoke  invoke1("127.0.0.1", m_nLocalAgentPort);
    if (!CLIAPI_Command(pSession, invoke1.GetHandle(), "100.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLocalAddress(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    struct  sockaddr_in     addr;
    struct  sockaddr_in     netmask;
    struct  sockaddr_in     gw;
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    addr.sin_addr.s_addr = inet_addr(argv[0]);
    netmask.sin_addr.s_addr = inet_addr(argv[1]);
    if (argc > 2) gw.sin_addr.s_addr = inet_addr(argv[2]);

    invoke.AddParamFormat("2.102.3", VARSMI_IPADDR, &addr.sin_addr.s_addr, sizeof(IPADDR));
    invoke.AddParamFormat("2.102.4", VARSMI_IPADDR, &netmask.sin_addr.s_addr, sizeof(IPADDR));
    if (argc > 2) invoke.AddParamFormat("2.102.5", VARSMI_IPADDR, &gw.sin_addr.s_addr, sizeof(IPADDR));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_MCU_LOCAL_IP "\r\n");
    OUTTEXT(pSession, "You must reset the MCU after changing this parameter.\r\n");
    OUTTEXT(pSession, "\r\n");
    OUTTEXT(pSession, "See also: show ethernet\r\n");
    return CLIERR_OK;
}

int cmdSetEthernetType(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        nType = 0;

    if (!Confirm(pSession, MSG_CMD_SET_ETHER_TYPE))
        return CLIERR_OK;

    if (strcasecmp(argv[0], "lan") == 0)
    {
        nType = ETHER_TYPE_LAN;    
    }
    else if (strcasecmp(argv[0], "dhcp") == 0)
    {
        nType = ETHER_TYPE_DHCP;
    }
    else if (strcasecmp(argv[0], "pppoe") == 0)
    {
        nType = ETHER_TYPE_PPPOE;
    }
    else if (strcasecmp(argv[0], "ppp") == 0)
    {
        nType = ETHER_TYPE_PPP;
    }
    else
    {
        nType = (BYTE)strtol(argv[0], (char **)NULL, 10);
    }

#if 0
// Issue #959 : TI AM3352 Porting
#if defined(__TI_AM335X__) && defined(__MOBILE_AME5520__)
    if(nType == ETHER_TYPE_PPPOE)
    {
        OUTTEXT(pSession, MSG_INFO_INVALID_VALUE "\r\n", argv[0]);
        return CLIERR_INVALID_PARAM;
    }
#endif
#endif

    invoke.AddParam("2.1.11", nType);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_ETHER_TYPE "\r\n", MSGAPI_GetEtherType(nType), nType);
    return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Codi Function
/////////////////////////////////////////////////////////////////////////////

int cmdSetCoordinatorAutoSetting(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        n=0;
    
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(!strcasecmp(argv[0],"on") || !strcasecmp(argv[0],"true") || !strcasecmp(argv[0],"high"))
    {
        n = 1;
    }else n = strtol(argv[0],(char **)NULL,10);

    if(n != 1) n = 0;

    invoke.AddParam("3.3.12", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "Coordinator Auto Setting = %s\r\n", n != 0 ? "On" : "Off");
    return CLIERR_OK;
}

int cmdSetCoordinatorChannel(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (BYTE)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("3.3.13", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Coordinator Channel = %d\r\n", n);
    return CLIERR_OK;
}

int cmdSetCooordinatorPanID(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    WORD        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (WORD)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("3.3.14", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Coordinator PAN ID = %d\r\n", n);
    return CLIERR_OK;
}

int cmdSetCooordinatorExtPanID(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        buf[8];
    char        *p;
    int            i, n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    for(i=0, p=argv[0]; p && *p; p+=3, i++)
    {
        sscanf(p, "%x", &n);
        buf[i] = n;
    }

    invoke.AddParamFormat("3.3.15", VARSMI_STREAM, (void *)buf, 8);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Coordinator Extended PAN ID = %s\r\n", argv[0]);
    return CLIERR_OK;
}

int cmdSetCoordinatorRfPower(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    signed char n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (signed char)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("3.3.16", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Coordinator RF Power = %d\r\n", n);
    return CLIERR_OK;
}

int cmdSetCoordinatorTxPowerMode(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (BYTE)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("3.3.17", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Coordinator TX Power Mode = %d\r\n", n);
    return CLIERR_OK;
}

int cmdSetCoordinatorPermitTime(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (BYTE)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("3.3.18", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Coordinator Permit Time = %d\r\n", n);
    return CLIERR_OK;
}

int cmdSetCoordinatorSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        n;
    
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if (strcasecmp(argv[0], "enable") == 0)
    {
        n = 1;
    }
    else if (strcasecmp(argv[0], "disable") == 0)
    {
        n = 0;
    }
    else 
    {
        n = (BYTE)strtol(argv[0], (char **)NULL, 10);
    }

    invoke.AddParam("3.3.19", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "Coordinator Security = %s\r\n", n == 1 ? "Enable" : "Disable");
    return CLIERR_OK;
}

int cmdSetCoordinatorLinkKey(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        buf[16];
    int         n=0;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    memset(buf, 0, 16);
    OUTTEXT(pSession, "Coordinator Link Key = %s\r\n", argv[0]);
    if((GetHexaList(argv[0], (char*)buf, sizeof(buf), &n) == CLIERR_OK) && n > 0)
    {
        invoke.AddParamFormat("3.3.20", VARSMI_STREAM, (void *)buf, 16);
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
            return CLIERR_ERROR;

        return CLIERR_OK;
    }

    return CLIERR_INVALID_PARAM;
}

int cmdSetCoordinatorNetworkKey(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        buf[16];
    int         n = 0;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    memset(buf, 0, 16);
    OUTTEXT(pSession, "Coordinator Network Key = %s\r\n", argv[0]);
    if((GetHexaList(argv[0], (char*)buf, sizeof(buf), &n) == CLIERR_OK) && n > 0)
    {
        invoke.AddParamFormat("3.3.21", VARSMI_STREAM, (void *)buf, 16);
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
            return CLIERR_ERROR;
        return CLIERR_OK;
    }

    return CLIERR_INVALID_PARAM;
}

int cmdSetCoordinatorDiscovery(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BOOL        val = FALSE;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(!strcasecmp(argv[0],"on") || !strcasecmp(argv[0],"true") || !strcasecmp(argv[0],"high"))
    {
        val = TRUE;
    }else val = (int)strtol(argv[0],(char **)NULL,10) == 1 ? TRUE : FALSE;

    invoke.AddParam("3.3.22", val);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Coordinator Route Discover = %s\r\n", val ? "On" : "Off");
    return CLIERR_OK;
}

int cmdSetCoordinatorHops(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        val;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    val = (BYTE) strtol(argv[0], (char **)NULL, 10);
    if(!val || val > 30) {
        OUTTEXT(pSession, MSG_INFO_OUT_OF_RANGE);
        return CLIERR_OK;
    }
    invoke.AddParam("3.3.23", val);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Coordinator Multicast HOPs = %d\r\n", val);
    return CLIERR_OK;
}

int cmdSetCoordinatorMtorType(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        n=0;
    
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(!strncasecmp(argv[0],"high",4)) {
        n = 1;
    }else if(!strncasecmp(argv[0],"low",3)) {
        n = 0;
    }else n = strtol(argv[0],(char **)NULL,10);

    if(n != 1) n = 0;

    invoke.AddParam("2.2.28", n ? TRUE : FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "Coordinator MTOR type changed = %s\r\n", n != 0 ? "HIGH" : "LOW");
    return CLIERR_OK;
}


/////////////////////////////////////////////////////////////////////////////
// Enable Function
/////////////////////////////////////////////////////////////////////////////

int cmdEnableRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_MISSING_READ))
        return CLIERR_OK;

    invoke.AddParam("2.2.2", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_MISSING_READ "\r\n");
    return CLIERR_OK;
}

int cmdEnableCmdHist(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_OPERATION_LOG))
        return CLIERR_OK;

    invoke.AddParam("2.2.3", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_OPERATION_LOG "\r\n");
    return CLIERR_OK;
}

int cmdEnableCommLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_COMM_LOG))
        return CLIERR_OK;

    invoke.AddParam("2.2.4", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_COMM_LOG "\r\n");
    return CLIERR_OK;
}
int cmdEnableAutoRegister(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_AUTO_SENSOR_REG))
        return CLIERR_OK;

    invoke.AddParam("2.2.5", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_AUTO_SENSOR_REG "\r\n");
    return CLIERR_OK;
}

int cmdEnableAutoReset(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_AUTO_RESET))
        return CLIERR_OK;

    invoke.AddParam("2.2.6", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_AUTO_RESET "\r\n");
    return CLIERR_OK;
}

int cmdEnableAutoTimeSync(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_AUTO_TIME_SYNC))
        return CLIERR_OK;

    invoke.AddParam("2.2.7", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_AUTO_TIME_SYNC "\r\n");
    return CLIERR_OK;
}

int cmdEnableAutoCodiReset(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_AUTO_CODI_RESET))
        return CLIERR_OK;

    invoke.AddParam("2.2.8", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_AUTO_CODI_RESET "\r\n");
    return CLIERR_OK;
}

int cmdEnableSubNetwork(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_SECONDARY_NETWORK))
        return CLIERR_OK;

    invoke.AddParam("2.2.9", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_SECONDARY_NETWORK "\r\n");
    return CLIERR_OK;
}

int cmdEnableMobileLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_MOBILE_COMM_LOG))
        return CLIERR_OK;

    invoke.AddParam("2.2.10", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_MOBILE_COMM_LOG "\r\n");
    return CLIERR_OK;
}

int cmdEnableAutoUpload(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_AUTO_SEND_DATA))
        return CLIERR_OK;

    invoke.AddParam("2.2.11", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_AUTO_SEND_DATA "\r\n");
    return CLIERR_OK;
}

int cmdEnableSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_COMM_SECURITY))
        return CLIERR_OK;

    invoke.AddParam("2.2.12", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_COMM_SECURITY "\r\n");
    return CLIERR_OK;
}

int cmdEnableKeepalive(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.14", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdEnableGpsTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.15", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" "The 'GPS timesync' function is enabled" "\r\n");
    return CLIERR_OK;
}
int cmdEnableDebugLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_USE_COMM_LOG))
        return CLIERR_OK;

    invoke.AddParam("2.2.26", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_USE_COMM_LOG "\r\n");
    return CLIERR_OK;

}

int cmdEnableRetry(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.35", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableMemoryCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.18", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableMeterFailRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.27", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableFlashCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.19", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableHighRam(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.28", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableGarbageCleaning(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.29", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableTimeBroadcast(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.16", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableLowBatt(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.24", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableStatusMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.17", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableMeterPolling(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.21", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdEnableMeterTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.20", (BOOL)TRUE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

//////////////////////////////////////////////////////////////////////////
//  [4/26/2011 DHKim]
//  Enable Join Control
//////////////////////////////////////////////////////////////////////////
int cmdEnableJoinControl(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	invoke.AddParam("2.2.30", (BOOL)TRUE);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
		return CLIERR_OK;
	return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Disable Function
/////////////////////////////////////////////////////////////////////////////

int cmdDisableRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_MISSING_READ))
        return CLIERR_OK;

    invoke.AddParam("2.2.2", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_MISSING_READ "\r\n");
    return CLIERR_OK;
}

int cmdDisableCmdHist(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_OPERATION_LOG))
        return CLIERR_OK;

    invoke.AddParam("2.2.3", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_OPERATION_LOG "\r\n");
    return CLIERR_OK;
}

int cmdDisableCommLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_COMM_LOG))
        return CLIERR_OK;

    invoke.AddParam("2.2.4", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_COMM_LOG "\r\n");
    return CLIERR_OK;
}

int cmdDisableAutoRegister(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_AUTO_SENSOR_REG))
        return CLIERR_OK;

    invoke.AddParam("2.2.5", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_AUTO_SENSOR_REG "\r\n");
    return CLIERR_OK;
}

int cmdDisableAutoReset(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_AUTO_RESET))
        return CLIERR_OK;

    invoke.AddParam("2.2.6", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_AUTO_RESET "\r\n");
    return CLIERR_OK;
}

int cmdDisableAutoTimeSync(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_AUTO_TIME_SYNC))
        return CLIERR_OK;

    invoke.AddParam("2.2.7", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_AUTO_TIME_SYNC "\r\n");
    return CLIERR_OK;
}

int cmdDisableAutoCodiReset(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_AUTO_CODI_RESET))
        return CLIERR_OK;

    invoke.AddParam("2.2.8", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_AUTO_CODI_RESET "\r\n");
    return CLIERR_OK;
}

int cmdDisableSubNetwork(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_SECONDARY_NETWORK))
        return CLIERR_OK;

    invoke.AddParam("2.2.9", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_SECONDARY_NETWORK "\r\n");
    return CLIERR_OK;
}

int cmdDisableMobileLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_MOBILE_COMM_LOG))
        return CLIERR_OK;

    invoke.AddParam("2.2.10", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_MOBILE_COMM_LOG "\r\n");
    return CLIERR_OK;
}

int cmdDisableAutoUpload(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_AUTO_SEND_DATA))
        return CLIERR_OK;

    invoke.AddParam("2.2.11", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_AUTO_SEND_DATA "\r\n");
    return CLIERR_OK;
}

int cmdDisableSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_COMM_SECURITY))
        return CLIERR_OK;

    invoke.AddParam("2.2.12", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_DISABLE_COMM_SECURITY "\r\n");
    return CLIERR_OK;
}

int cmdDisableKeepalive(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.14", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDisableGpsTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.15", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}
int cmdDisableDebugLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DISABLE_COMM_LOG))
        return CLIERR_OK;

    invoke.AddParam("2.2.26", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_DISABLE_COMM_LOG "\r\n");
    return CLIERR_OK;
}


int cmdDisableRetry(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.35", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDisableMemoryCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.18", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDisableMeterFailRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.27", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDisableFlashCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.19", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDisableHighRam(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.28", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdDisableGarbageCleaning(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.29", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdDisableTimeBroadcast(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.16", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDisableStatusMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.17", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDisableMeterPolling(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.21", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDisableMeterTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.2.20", (BOOL)FALSE);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDisableLowBatt(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
  CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

  if (!Confirm(pSession, MSG_DEFAULT))
    return CLIERR_OK;

  invoke.AddParam("2.2.24", (BOOL)FALSE);
  if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
    return CLIERR_OK;

  return CLIERR_OK;
}

//////////////////////////////////////////////////////////////////////////
//  [4/26/2011 DHKim]
//  Disable Join Control
//////////////////////////////////////////////////////////////////////////
int cmdDisableJoinControl(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	invoke.AddParam("2.2.30", (BOOL)FALSE);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
		return CLIERR_OK;
	return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Setup Function
/////////////////////////////////////////////////////////////////////////////
int cmdSetGpio(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        nPort, nValue;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nPort  = (BYTE)strtol(argv[0], (char **)NULL, 10);
    nValue = (BYTE)strtol(argv[1], (char **)NULL, 10);

    invoke.AddParam("1.4", (BYTE)nPort);
    invoke.AddParam("1.4", (BYTE)nValue);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.15"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_INFO_SET_GPIO_PORT "\r\n", nPort, nValue);
    return CLIERR_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Sensor Setup Function
/////////////////////////////////////////////////////////////////////////////

int cmdSetSensorValue(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, 60);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
    EUI64       id;
    BYTE        cmd;
    BOOL        bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.50");
        } 

        invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", "all");
    }

    cmd = (BYTE) strtol(argv[1],(char **)NULL, 10);

    switch(cmd)
    {
        case 1: // install
            OUTTEXT(pSession, MSG_NOT_PERMITTED_OPERATION);
            return CLIERR_INVALID_PARAM;
    }

    invoke.AddParam("1.4", cmd);
    invoke.AddParam("1.4", (BYTE) 0);       // Command Write

    if(argc > 2) {
        char strBuff[1024];
        int idx = 0;

        if((GetHexaList(argv[2], strBuff, sizeof(strBuff), &idx) == CLIERR_OK) && idx > 0)
        {
            invoke.AddParamFormat("1.12", VARSMI_STREAM, strBuff, idx);
        }
    }

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.50"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "Command execution has succeeded.\xd\xa");

      return CLIERR_OK;
}

int _GetFileLenNCrc(const char *pszFileName, UINT *pLength, WORD *pCrc)
{
    unsigned char    *pszBuffer;
    FILE    *fp;
    UINT    n, nLength=0;
    WORD    nCrc = 0x00;

    if(!pLength || !pCrc) return 0;

    fp = fopen(pszFileName, "rb");
    if (fp != NULL)
    {
        pszBuffer = (unsigned char *)MALLOC(4096);
        if (pszBuffer != NULL)
        {
            while((n=fread(pszBuffer, 1, 4096, fp)))
            {
                nCrc = GetCrc16WithCrc(pszBuffer, n, nCrc);
                nLength += n;
            }
            FREE(pszBuffer);
        }
        fclose(fp);
    }

    *pLength = nLength;
    *pCrc = nCrc;

    return nLength;
}

/** Sensor Verify.
  * Sensor Verify은 명령어의 위험성 때문에 All을 지원하지 않고 반드시 id를 개별로
  * 지정해야만 사용할 수 있다.
  */
int cmdSetSensorVerify(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, 60);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    WORD        nFw     = 0x00;
    WORD        nBuild  = 0x00;
    BOOL        nForce  = TRUE;
    SENSORINFOENTRYNEW    *pSensor = NULL;
    WORD        nCrc = 0x00, nBigCrc;
    UINT        nLength = 0, nBigLen;
    BYTE        pBuffer[sizeof(UINT)+sizeof(WORD)];

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            OUTTEXT(pSession, MSG_SLEEPY_FFD_NOT_SUPPORT "\xd\xa");
            return CLIERR_OK;
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        OUTTEXT(pSession, MSG_INFO_INVALID_CMD "\xd\xa");
        return CLIERR_OK;
    }

    if(!_GetFileLenNCrc(argv[1], &nLength, &nCrc))
    {
        OUTTEXT(pSession, MSG_INFO_INVALID_CMD "\xd\xa");
        return CLIERR_OK;
    }
    memset(pBuffer, 0, sizeof(pBuffer));

    nBigLen = BigToHostInt(nLength);
    nBigCrc = BigToHostShort(nCrc);
    memcpy(pBuffer, &nBigLen, sizeof(UINT));
    memcpy(pBuffer + sizeof(UINT), &nBigCrc, sizeof(WORD));

    invoke.AddParam("1.4", ENDI_CMD_VERIFY_EEPROM);
    invoke.AddParam("1.5", nFw);
    invoke.AddParam("1.5", nBuild);
    invoke.AddParam("1.4", nForce);
    invoke.AddParamFormat("1.12", VARSMI_STREAM, (void *)pBuffer, sizeof(pBuffer));

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());

    return CLIERR_OK;
}

/** Sensor Install.
  * Sensor Install은 명령어의 위험성 때문에 All을 지원하지 않고 반드시 id를 개별로
  * 지정해야만 사용할 수 있다.
  */
int cmdSetSensorInstall(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    SENSORINFOENTRYNEW    *pSensor = NULL;
    WORD        nFw     = 0x00;
    WORD        nBuild  = 0x00;
    BOOL        nForce  = TRUE;
    BYTE        nInstallType = 0;               // 0: Modem (em250) 1: Core (External core)

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            OUTTEXT(pSession, MSG_SLEEPY_FFD_NOT_SUPPORT "\xd\xa");
            return CLIERR_OK;
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        OUTTEXT(pSession, MSG_INFO_INVALID_CMD "\xd\xa");
        return CLIERR_OK;
    }

    if(!strcasecmp(argv[1],"core"))
    {
        nInstallType = 1;
    }else nInstallType = (BYTE) strtol(argv[1],(char **)NULL,10);

    // 0이 아니라면 1이어야 한다.
    if(nInstallType) nInstallType = 1;

    invoke.AddParam("1.4", ENDI_CMD_INSTALL_BOOTLOADER);
    invoke.AddParam("1.5", nFw);
    invoke.AddParam("1.5", nBuild);
    invoke.AddParam("1.4", nForce);
    invoke.AddParamFormat("1.12", VARSMI_STREAM, (void *)&nInstallType, sizeof(BYTE));

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());

    OUTTEXT(pSession, "Command execution has succeeded.\xd\xa");

      return CLIERR_OK;
}

/** SX Meter Factory Reset 
  * Issue #89
  *
  * 명령어의 위험성 때문에 All을 지원하지 않고 반드시 id를 개별로
  * 지정해야만 사용할 수 있다.
  */
int cmdSetSxFactoryReset(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, 60);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
    SENSORINFOENTRYNEW    *pSensor = NULL;
    EUI64       id;
    BYTE        pBuffer[2];

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            OUTTEXT(pSession, MSG_SLEEPY_FFD_NOT_SUPPORT "\xd\xa");
            return CLIERR_OK;
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        OUTTEXT(pSession, MSG_INFO_INVALID_CMD "\xd\xa");
        return CLIERR_OK;
    }

    pBuffer[0] = 0x00; pBuffer[1] = 0x01;

    invoke.AddParam("1.4", ENDI_CMD_SET_METER_ROLLBACK);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, (void *)pBuffer, sizeof(pBuffer));

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());

    return CLIERR_OK;
}

int cmdSetSensorTime(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    BOOL        bAll=FALSE;
    time_t      now;
    struct  tm  when;
    GMTTIMESTAMP    gmt;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            OUTTEXT(pSession, MSG_SLEEPY_FFD_NOT_SUPPORT "\xd\xa");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    time(&now);
    when = *localtime(&now);

#ifdef  __USE_BSD
    gmt.tz   = htons((int)(when.tm_gmtoff/60));
#endif
    gmt.dst  = htons(0);
    gmt.year = htons(when.tm_year + 1900);
    gmt.mon  = (BYTE)when.tm_mon + 1;
    gmt.day  = (BYTE)when.tm_mday;
    gmt.hour = (BYTE)when.tm_hour;
    gmt.min  = (BYTE)when.tm_min;
    gmt.sec  = (BYTE)when.tm_sec;

    invoke.AddParam("1.4", ENDI_CMD_SET_TIME);

    if(argc > 1) gmt.year = htons((short)strtol(argv[1],(char **)NULL,10));
    if(argc > 2) gmt.mon = (BYTE)strtol(argv[2],(char **)NULL,10);
    if(argc > 3) gmt.day = (BYTE)strtol(argv[3],(char **)NULL,10);
    if(argc > 4) gmt.hour = (BYTE)strtol(argv[4],(char **)NULL,10);
    if(argc > 5) gmt.min = (BYTE)strtol(argv[5],(char **)NULL,10);
    if(argc > 6) gmt.sec = (BYTE)strtol(argv[6],(char **)NULL,10);
    if(argc > 7) gmt.tz = htons((short)strtol(argv[7],(char **)NULL,10));

    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force

    invoke.AddParamFormat("1.12", VARSMI_STREAM, &gmt, sizeof(GMTTIMESTAMP));

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "Time of the SENSOR is changed to '%04d/%02d/%02d %02d:%02d:%02d(tz %02d)' successfully.\xd\xa", 
        ntohs(gmt.year), gmt.mon, gmt.day, gmt.hour, gmt.min, gmt.sec, ntohs(gmt.tz));

      return CLIERR_OK;
}

int cmdSetSensorReset(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    UINT        resetTime=3;
    BOOL        bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    invoke.AddParam("1.4", ENDI_CMD_RESET);

    if(argc > 1) {
        resetTime = (UINT)strtol(argv[1],(char **)NULL,10);
    }

    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force

    invoke.AddParamFormat("1.12", VARSMI_STREAM, &resetTime, sizeof(UINT));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

      OUTTEXT(pSession, "Sensor reset.\r\n");

      return CLIERR_OK;
}

int cmdSetSensorMeterSerial(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    char        buffer[20+1];
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

      StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.24");
    } 

    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, argv[1], sizeof(buffer)-1);

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_METER_SERIAL_NUMBER);
    invoke.AddParamFormat("1.12", VARSMI_STREAM, buffer, sizeof(buffer)-1);

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.24"))
        return CLIERR_OK;

    OUTTEXT(pSession, "METER Serial number of the SENSOR is changed to '%s' successfully.\xd\xa", buffer);
      return CLIERR_OK;
}

int cmdSetSensorVendor(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    char        buffer[20+1];
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

      StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.24");
    } 

    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, argv[1], sizeof(buffer)-1);

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_VENDOR);
    invoke.AddParamFormat("1.12", VARSMI_STREAM, buffer, sizeof(buffer)-1);

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.24"))
        return CLIERR_OK;

    OUTTEXT(pSession, "VENDOR of the SENSOR is changed to '%s' successfully.\xd\xa", buffer);
      return CLIERR_OK;
}

int cmdSetSensorCustomer(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    char        buffer[30+1];
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

      StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.24");
    } 

    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, argv[1], sizeof(buffer)-1);

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_CUSTOMER_NAME);
    invoke.AddParamFormat("1.12", VARSMI_STREAM, buffer, sizeof(buffer)-1);

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.24"))
        return CLIERR_OK;

    OUTTEXT(pSession, "CUSTOMER NAME of the SENSOR is changed to '%s' successfully.\xd\xa", buffer);
      return CLIERR_OK;
}

int cmdSetSensorLocation(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    char        buffer[30+1];
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

      StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.24");
    } 

    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, argv[1], sizeof(buffer)-1);

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_CONSUMPTION_LOCATION);
    invoke.AddParamFormat("1.12", VARSMI_STREAM, buffer, sizeof(buffer)-1);

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.24"))
        return CLIERR_OK;

    OUTTEXT(pSession, "CONSUMPTION LOCATION of the SENSOR is changed to '%s' successfully.\xd\xa", buffer);
      return CLIERR_OK;
}

int cmdSetSensorNetworkType(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    BOOL        bAll=FALSE;
    BYTE        val;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    invoke.AddParam("1.4", ENDI_CMD_SET_NETWORK_TYPE);

    if(!strcasecmp(argv[1],"mesh")) val = 1;
    else val = (BYTE) strtol(argv[1], (char **)NULL, 10);
    if(val != 1) val = 0;

    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force

    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(BYTE));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "NETWORK TYPE of the SENSOR is changed to '%s' successfully.\xd\xa", 
            MSGAPI_NetworkType(val));
      return CLIERR_OK;
}

int cmdSetSensorResetTime(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    char        val;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.24");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        invoke.AddParam("1.11", argv[0]);
    }

    val = (char) strtol(argv[1], (char **)NULL, 10);
    
    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_FIXED_RESET);
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, 1);

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.24"))
        return CLIERR_OK;

    OUTTEXT(pSession, "RESET TIME of the SENSOR is changed to '%d' successfully.\xd\xa", (int)val);
      return CLIERR_OK;
}

int cmdSetSensorTestMode(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    char         val;
    char        buffer[20+1];
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.24");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        invoke.AddParam("1.11", argv[0]);
    }

    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, argv[1], sizeof(buffer)-1);

    if(!strcasecmp(buffer,"on") || !strcasecmp(buffer,"true") || !strcasecmp(buffer,"high"))
    {
        val = 1;
    }else val = strtol(buffer,(char **)NULL,10);

    if(val != 1) val = 0;

    invoke.AddParam("1.5", (WORD) ENDI_ROMAP_TEST_FLAG);
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, 1);

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.24"))
        return CLIERR_OK;

    OUTTEXT(pSession, "TEST MODE of the SENSOR is changed to '%s' successfully.\xd\xa", val?"On":"Off");
      return CLIERR_OK;
}

#ifdef  __SUPPORT_ENDDEVICE__
int cmdSetSensorCurrentPulse(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    UINT        val,sval;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

      StrToEUI64(argv[0], &id);
    val = (UINT) strtol(argv[1],(char **)NULL,10);
    sval = htonl(val);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.34");
    } 

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_CURRENT_PULSE);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &sval, sizeof(UINT));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    OUTTEXT(pSession, "CURRENT PULSE of the SENSOR is changed to '%d' successfully.\xd\xa", val);

      return CLIERR_OK;
}

int cmdSetSensorLastPulse(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64           id;
    UINT            val, sval;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

      StrToEUI64(argv[0], &id);
    val = (UINT) strtol(argv[1],(char **)NULL,10);
    sval = htonl(val);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.34");
    } 

    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_LAST_PULSE);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &sval, sizeof(UINT));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    OUTTEXT(pSession, "LAST PULSE of the SENSOR is changed to '%d' successfully.\xd\xa", val);
      return CLIERR_OK;
}
#endif

int cmdSetSensorLpPeriod(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
    EUI64           id;
    BYTE            val,prval;
    BOOL            bAll = FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
                return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        invoke.AddParam("1.11", argv[0]);
        bAll = TRUE;
    }

    val = (BYTE) strtol(argv[1],(char **)NULL,10);
    prval = val;

    switch(val) {
        case 0: break;
        case 1: prval = 60; break;
        case 2: prval = 30; break;
        case 4: prval = 15; break;
        case 12: prval = 5; break;
        case 60: val = 1; break;
        case 30: val = 2; break;
        case 15: val = 4; break;
        case 5: val = 12; break;
        default:
            return CLIERR_OK;
    }

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_LP_PERIOD);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(BYTE));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "LP Period of the SENSOR is changed to '%d'(min) successfully.\xd\xa", prval);
      return CLIERR_OK;
}

int cmdSetSensorSetupSec(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
/*********** 아직 ENDI_CMD_SET_SETUP_SEC이 정의되지 않았다.
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64           id;
    WORD            val=0;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

      StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.34");
    } 

    val = htons((WORD) strtol(argv[1],(char **)NULL,10));

    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_SETUP_SEC);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(WORD));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    OUTTEXT(pSession, "LP CHOICE of the SENSOR is changed to '%u' successfully.\xd\xa", val);
****/
      return CLIERR_OK;
}

int cmdSetSensorLpChoice(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64           id;
    BYTE            val=1;
    BOOL            bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        invoke.AddParam("1.11", argv[0]);
        bAll = TRUE;
    }

    val = (BYTE) strtol(argv[1],(char **)NULL,10);

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_LP_CHOICE);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(BYTE));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "LP CHOICE of the SENSOR is changed to '%u' successfully.\xd\xa", val);
      return CLIERR_OK;
}

int cmdSetSensorAlarmFlag(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64           id;
    BYTE            val=0;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        invoke.AddParam("1.11", argv[0]);
    }

    if(!strcasecmp(argv[1],"on") || !strcasecmp(argv[1],"true") || !strcasecmp(argv[1],"high"))
    {
        val = 1;
    }else val = strtol(argv[1],(char **)NULL,10);

    if(val != 1) val = 0;

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_ALARM_FLAG);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(BYTE));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    OUTTEXT(pSession, "ALARM FLAG of the SENSOR is changed to '%s' successfully.\xd\xa", val ? "On" : "Off");
      return CLIERR_OK;
}

int cmdSetSensorOperDay(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64           id;
    WORD            val,sval;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

      StrToEUI64(argv[0], &id);
    val = (WORD) strtol(argv[1],(char **)NULL,10);
    sval = htons(val);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.34");
    } 

    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_OPER_DAY);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &sval, sizeof(WORD));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    OUTTEXT(pSession, "OPERATING DAY of the SENSOR is changed to '%d' successfully.\xd\xa", val);
      return CLIERR_OK;
}

int cmdSetSensorActiveMin(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64           id;
    WORD            val,sval;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

      StrToEUI64(argv[0], &id);
    val = (WORD) strtol(argv[1],(char **)NULL,10);
    sval = htons(val);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.34");
    } 

    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_ACTIVE_MIN);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &sval, sizeof(WORD));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    OUTTEXT(pSession, "ACTIVE MIN of the SENSOR is changed to '%d' successfully.\xd\xa", val);
      return CLIERR_OK;
}

int cmdSetSensorMeteringDay(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64           id;
    UINT            val, orig;
    BYTE            sval[4];
    int             i;
    BOOL            bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    val = (UINT) strtol(argv[1],(char **)NULL,16);
    orig = val;
    memset(sval,0,sizeof(sval));
    for(i=0;i<4;i++) {
        sval[3-i] = (BYTE) (val & 0xFF);
        val >>= 8;
    }

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_METERING_DAY);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &sval, sizeof(sval));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "METERING DAY of the SENSOR is changed to '%X' successfully.\xd\xa", orig);
      return CLIERR_OK;
}


int cmdSetSensorMeteringQuarter(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64           id;
    char            tmp[3];
    BYTE            sval[12];
    int             i,j,len;
    BOOL            bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    len = strlen(argv[1]);
    memset(sval,0,sizeof(sval));
    for(i=len-1,j=11;i>=0 && j>=0;i-=2,j--) {
        memset(tmp,0,sizeof(tmp));
        tmp[1] = argv[1][i];
        if(i-1>=0) tmp[0] = argv[1][i-1];
        else tmp[0] = '0';
        sval[j] = (BYTE)strtol(tmp,(char **)NULL,16);
    }

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_METERING_HOUR);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &sval, sizeof(sval));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "METERING QUARTER of the SENSOR is changed to '%s' successfully.\xd\xa", argv[1]);
      return CLIERR_OK;
}


int cmdSetSensorActiveKeepTime(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64           id;
    BYTE            val;
    BOOL        bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_ACTIVE_KEEP_TIME);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(BYTE));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "Command execution has succeeded.\xd\xa");
      return CLIERR_OK;
}


int cmdSetSensorRfPower(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    signed char val;
    BOOL        bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    val = (signed char) strtol(argv[1],(char **)NULL,10);

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_RF_POWER);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(signed char));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "RF POWER of the SENSOR is changed to '%d' successfully.\xd\xa", val);
      return CLIERR_OK;
}

int cmdSetSensorPermitMode(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    BYTE        val;
    BOOL        bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    if(!strcasecmp(argv[1],"on") || !strcasecmp(argv[1],"true") || !strcasecmp(argv[1],"high"))
    {
        val = 1;
    }else val = strtol(argv[1],(char **)NULL,10);

    if(val != 1) val = 0;

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_PERMIT_MODE);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(BYTE));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "PERMIT MODE of the SENSOR is changed to '%s' successfully.\xd\xa", val?"on":"off");
      return CLIERR_OK;
}

int cmdSetSensorPermitState(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    BYTE        val;
    BOOL        bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    if(!strcasecmp(argv[1],"on") || !strcasecmp(argv[1],"true") || !strcasecmp(argv[1],"high"))
    {
        val = 255;
    }else val = strtol(argv[1],(char **)NULL,10);

    if(val != 255) val = 0;

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_PERMIT_STATE);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(BYTE));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "PERMIT STATE of the SENSOR is changed to '%s' successfully.\xd\xa", val?"on":"off");
      return CLIERR_OK;
}

int cmdSetSensorAlarmMask(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    WORD        val;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        invoke.AddParam("1.11", argv[0]);
    }

    val = htons((WORD) strtol(argv[1],(char **)NULL,16) & 0x7FF0);

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_ALARM_MASK);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(WORD));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    OUTTEXT(pSession, "ALARM MASK of the SENSOR is changed to '0x%04X' successfully.\xd\xa", 
            (WORD) strtol(argv[1],(char **)NULL,16) & 0x7FF0);
      return CLIERR_OK;
}

int cmdSetSensorMeteringFailCnt(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
      EUI64       id;
    BYTE        val;
    BOOL        bAll=FALSE;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        bAll = TRUE;
        invoke.AddParam("1.11", argv[0]);
    }

    val = (BYTE) strtol(argv[1],(char **)NULL, 10);

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_METERING_FAIL_CNT);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(BYTE));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

      if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    if(!bAll) {
        DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
    }

    OUTTEXT(pSession, "METERING FAIL CNT of the SENSOR is changed to '%d' successfully.\xd\xa", val);
      return CLIERR_OK;
}

// Wooks For Smoke Detector Sensor.
int cmdSetSensorSiren(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, 60);
#endif
	CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);

	EUI64					id;
	BOOL					bAll		= FALSE;
	//
	char					val;
	char					buffer[20+1];
	//
	SENSORINFOENTRYNEW		*pSensor	= NULL;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	// EUI64 ID
	if(check_id(NULL, argv[0])) 
	{
		StrToEUI64(argv[0], &id);

		invoke2.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
		if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
			return CLIERR_OK;
		pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

		if(NeedAsync(pSensor->sensorAttr))
		{
			MakeAsyncParameter(&invoke, &id, "102.50");
		} 

		invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	}

	// Others
	else 
	{ 
		bAll = TRUE;
		invoke.AddParam("1.11", "all");
	}

	// ON(True) or OFF(False)
	memset(buffer, 0, sizeof(buffer));
	strncpy(buffer, argv[1], sizeof(buffer)-1);

    if(!strcasecmp(buffer,"on") || !strcasecmp(buffer, "true") || !strcasecmp(buffer, "high"))
	{
		val = 1;
	}
	else 
	{
		val = strtol(buffer,(char **)NULL,10);
	}

	if(val != 1) val = 2;

	invoke.AddParam("1.4", (BYTE)ENDI_CMD_SET_SMOKE_ALARM);
	invoke.AddParam("1.4", (BYTE)ENDI_CMD_WRITE_REQUEST);

	// Smoke Detector Alarm(Siren) is starting when val is 1.
	invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, 1);

	if(pSensor && NeedAsync(pSensor->sensorAttr)) 
	{
		if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
			return CLIERR_OK;

		OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
	} 

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.50"))
		return CLIERR_OK;

	if(!bAll) 
	{
		DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
	}

	OUTTEXT(pSession, "Command execution has succeeded.\xd\xa");

	return CLIERR_OK;
}

// Wooks For Smoke Detector Sensor.
int cmdSetSensorTemperatureLevel(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, 60);
#endif
	CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);

	EUI64					id;
	BOOL					bAll				= FALSE;
	//
	INT						temperatureLevel	= 0;
	WORD					dataFormat			= 0;
	char					val[2];
	char					buffer[20+1];
	//
	SENSORINFOENTRYNEW		*pSensor	= NULL;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	// EUI64 ID
	if(check_id(NULL, argv[0])) 
	{
		StrToEUI64(argv[0], &id);

		invoke2.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
		if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
			return CLIERR_OK;
		pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

		if(NeedAsync(pSensor->sensorAttr))
		{
			MakeAsyncParameter(&invoke, &id, "102.50");
		} 

		invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	}
	else 
	{ 
		bAll = TRUE;
		invoke.AddParam("1.11", "all");
	}

	// Temperature Alarm Level : -4095 ~ +4095
	memset(buffer, 0, sizeof(buffer));
	strncpy(buffer, argv[1], sizeof(buffer)-1);

	temperatureLevel = strtol(buffer, (char **)NULL, 10);

	// Verify Value
	OUTTEXT(pSession, "Value = %d\xd\xa", temperatureLevel);

	// 절대값으로 임계치 검사
	// 그 외에 포맷 셋팅
	if( abs(temperatureLevel) > 4096 )
	{
		// 12 bit 로 표현 가능한 최대값 0 ~ 4095
		return CLIERR_INVALID_PARAM;
	}
	else
	{
		dataFormat = 0x0FFF | abs(temperatureLevel);

		if( temperatureLevel < 0 )
		{
			// Set Sign Bit : 12th bit
			dataFormat = dataFormat | 0x1000;
		}

		val[0] = (dataFormat & 0x00FF);
		val[1] = ((dataFormat & 0xFF00) >> 8);
	}

	// Verify Value
	OUTTEXT(pSession, "Temperature Alarm Level Format = 0x%02X \xd\xa", dataFormat);

	//
	invoke.AddParam("1.4", (BYTE)ENDI_CMD_SET_SMOKE_ALARM_LEVEL);
	invoke.AddParam("1.4", (BYTE)ENDI_CMD_WRITE_REQUEST);
	invoke.AddParamFormat("1.12", VARSMI_STREAM, val ,2);

	if(pSensor && NeedAsync(pSensor->sensorAttr)) 
	{
		if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
			return CLIERR_OK;

		OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
	} 

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.50"))
		return CLIERR_OK;

	if(!bAll) 
	{
		DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
	}

	OUTTEXT(pSession, "Command execution has succeeded.\xd\xa");

	return CLIERR_OK;
}

int	cmdSetSensorTxPowerMode(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, 60);
#endif
	CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);

	EUI64					id;
	BOOL					bAll		= FALSE;
	//
	char					val;
	char					buffer[20+1];
	//
	SENSORINFOENTRYNEW		*pSensor	= NULL;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	// EUI64 ID
	if(check_id(NULL, argv[0])) 
	{
		StrToEUI64(argv[0], &id);

		invoke2.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
		if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
			return CLIERR_OK;
		pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

		if(NeedAsync(pSensor->sensorAttr))
		{
			MakeAsyncParameter(&invoke, &id, "102.50");
		} 

		invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	}

	// Others
	else 
	{ 
		bAll = TRUE;
		invoke.AddParam("1.11", "all");
	}

	// ON(True) or OFF(False)
	memset(buffer, 0, sizeof(buffer));
	strncpy(buffer, argv[1], sizeof(buffer)-1);

    if(!strcasecmp(buffer,"on") || !strcasecmp(buffer, "true") || !strcasecmp(buffer, "high"))
	{
		val = 1;
	}
	else 
	{
		val = strtol(buffer,(char **)NULL,10);
	}

	if(val != 1) val = 2;

	invoke.AddParam("1.4", (BYTE)ENDI_CMD_SET_SMOKE_RF_POWER_MODE);
	invoke.AddParam("1.4", (BYTE)ENDI_CMD_WRITE_REQUEST);

	// Smoke Detector Alarm(Siren) is starting when val is 1.
	invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, 1);

	if(pSensor && NeedAsync(pSensor->sensorAttr)) 
	{
		if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
			return CLIERR_OK;

		OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
	} 

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.50"))
		return CLIERR_OK;

	if(!bAll) 
	{
		DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
	}

	OUTTEXT(pSession, "Command execution has succeeded.\xd\xa");

	return CLIERR_OK;
}

int cmdSetSensorHeartBeatDay(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
	CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
	EUI64           id;
	UINT            val, orig;
	BYTE            sval[4];
	int             i;
	BOOL            bAll=FALSE;
	SENSORINFOENTRYNEW    *pSensor = NULL;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	if(check_id(NULL, argv[0])) { // EUI64 ID
		StrToEUI64(argv[0], &id);

		invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
		if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
			return CLIERR_OK;
		pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

		if(NeedAsync(pSensor->sensorAttr))
		{
			MakeAsyncParameter(&invoke, &id, "102.34");
		} 

		invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	}else { // Others
		bAll = TRUE;
		invoke.AddParam("1.11", argv[0]);
	}

	val = (UINT) strtol(argv[1],(char **)NULL,16);
	orig = val;
	memset(sval,0,sizeof(sval));
	for(i=0;i<4;i++) {
		sval[3-i] = (BYTE) (val & 0xFF);
		val >>= 8;
	}

	invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_HEART_BEAT_DAY);
	invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
	invoke.AddParam("1.5", (WORD) 0);       // Build Number
	invoke.AddParam("1.3", (BOOL) FALSE);   // Force
	invoke.AddParamFormat("1.12", VARSMI_STREAM, &sval, sizeof(sval));

	if(pSensor && NeedAsync(pSensor->sensorAttr)) {
		if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
			return CLIERR_OK;

		OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
	} 

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
		return CLIERR_OK;

	if(!bAll) {
		DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
	}

	OUTTEXT(pSession, "HEART BEAT DAY of the SENSOR is changed to '%X' successfully.\xd\xa", orig);
	return CLIERR_OK;
}

int cmdSetSensorHeartBeatQuarter(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
	CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
	EUI64           id;
	char            tmp[3];
	BYTE            sval[12];
	int             i,j,len;
	BOOL            bAll=FALSE;
	SENSORINFOENTRYNEW    *pSensor = NULL;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	if(check_id(NULL, argv[0])) { // EUI64 ID
		StrToEUI64(argv[0], &id);

		invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
		if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
			return CLIERR_OK;
		pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

		if(NeedAsync(pSensor->sensorAttr))
		{
			MakeAsyncParameter(&invoke, &id, "102.34");
		} 

		invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	}else { // Others
		bAll = TRUE;
		invoke.AddParam("1.11", argv[0]);
	}

	len = strlen(argv[1]);
	memset(sval,0,sizeof(sval));
	for(i=len-1,j=11;i>=0 && j>=0;i-=2,j--) {
		memset(tmp,0,sizeof(tmp));
		tmp[1] = argv[1][i];
		if(i-1>=0) tmp[0] = argv[1][i-1];
		else tmp[0] = '0';
		sval[j] = (BYTE)strtol(tmp,(char **)NULL,16);
	}

	invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_HEART_BEAT_HOUR);
	invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
	invoke.AddParam("1.5", (WORD) 0);       // Build Number
	invoke.AddParam("1.3", (BOOL) FALSE);   // Force
	invoke.AddParamFormat("1.12", VARSMI_STREAM, &sval, sizeof(sval));

	if(pSensor && NeedAsync(pSensor->sensorAttr)) {
		if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
			return CLIERR_OK;

		OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
	} 

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
		return CLIERR_OK;

	if(!bAll) {
		DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());
	}

	OUTTEXT(pSession, "HEART BEAT QUARTER of the SENSOR is changed to '%s' successfully.\xd\xa", argv[1]);
	return CLIERR_OK;
}

/** Push Metering flag를 조절한다.
 *
 * Issue #495: MIU에서 Push Metering을 지원할 경우 flag 설정으로 enable/disable 시킬 수 있다.
 *
 */
int cmdSetSensorPushMetering(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke    invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke    invoke2("127.0.0.1", m_nLocalAgentPort);
    EUI64           id;
    BYTE            val=0;
    SENSORINFOENTRYNEW    *pSensor = NULL;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(check_id(NULL, argv[0])) { // EUI64 ID
          StrToEUI64(argv[0], &id);

        invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
            return CLIERR_OK;
        pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

        if(NeedAsync(pSensor->sensorAttr))
        {
            MakeAsyncParameter(&invoke, &id, "102.34");
        } 

        invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    }else { // Others
        invoke.AddParam("1.11", argv[0]);
    }

    if(!strcasecmp(argv[1],"on") || !strcasecmp(argv[1],"true") || !strcasecmp(argv[1],"high"))
    {
        val = 1;
    }else val = strtol(argv[1],(char **)NULL,10);

    if(val != 1) val = 0;

    invoke.AddParam("1.4", (BYTE) ENDI_CMD_SET_PUSH_STRATEGY);
    invoke.AddParam("1.5", (WORD) 0);       // Firmware Version
    invoke.AddParam("1.5", (WORD) 0);       // Build Number
    invoke.AddParam("1.3", (BOOL) FALSE);   // Force
    invoke.AddParamFormat("1.12", VARSMI_STREAM, &val, sizeof(BYTE));

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
        if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
            return CLIERR_OK;

        OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
        return CLIERR_OK;
    } 

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.34"))
        return CLIERR_OK;

    OUTTEXT(pSession, "PUSH Metering strategy is changed to '%s' successfully.\xd\xa", val ? "On" : "Off");
    return CLIERR_OK;
}

//////////////////////////////////////////////////////////////////////////////////
//    LOG
//////////////////////////////////////////////////////////////////////////////////

int cmdSetSavedayEventLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.22", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetSavedayAlarmLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.45", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetSavedayCmdLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.21", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetSavedayCommLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.23", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetSavedayMobileLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.74", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetSavedayMeterLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.24", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetSavedayUpgradeLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.75", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetSavedayUploadLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.76", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetSavedayTimesyncLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.77", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}
int cmdSetSavedayDebugLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.22", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLimitMobileLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.43", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}
int cmdSetLimitDebugLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0],(char **)NULL, 10);
    invoke.AddParam("2.3.42", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLimitCommLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0],(char **)NULL, 10);
    invoke.AddParam("2.3.42", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLimitCmdLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.41", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLimitEventLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.39", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLimitAlarmLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0],(char **)NULL, 10);
    invoke.AddParam("2.3.40", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLimitMeterLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.78", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLimitUpgradeLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.79", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLimitUploadLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0],(char **)NULL, 10);
    invoke.AddParam("2.3.80", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetLimitTimesyncLog(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        nLimit;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    nLimit = (UINT)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.81", nLimit);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetPhoneList(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int            i;

    if (argc == 0)
    {
        if (!Confirm(pSession, MSG_DEFAULT))
            return CLIERR_OK;
    }

    for(i=0; i<argc; i++)
        invoke.AddParam("1.11", argv[i]);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "107.2"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Telephone directory changed.\r\n");
    return CLIERR_OK;
}

int cmdSetOptionMeteringSaveday(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int            n; 

    n = (int)strtol(argv[0], (char **)NULL, 10);
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.49", (int)n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "configuration changed.\r\n");
    return CLIERR_OK;
}

int cmdSetOptionMeteringThread(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int            n; 

    n = (int)strtol(argv[0], (char **)NULL, 10);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.6", (BYTE)n);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "configuration changed.\r\n");
    return CLIERR_OK;
}

int cmdSetOptionRetry(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         hour, interval, sec;
    UINT        nMask;

    hour     = (int)strtol(argv[0], (char **)NULL, 10);
    interval = (int)strtol(argv[1], (char **)NULL, 10);
    sec      = (int)strtol(argv[2], (char **)NULL, 10);
    sscanf(argv[3], "%x", &nMask);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.34", (BYTE)interval);
    invoke.AddParam("2.3.36", (BYTE)hour);
    invoke.AddParam("2.3.37", (BYTE)sec);
    invoke.AddParam("2.3.38", (UINT)nMask);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "configuration changed.\r\n");
    return CLIERR_OK;
}

int cmdSetOptionAutoreset(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    TIMESTAMP    tmReset;
    int         day, hour, min;

    day  = (int)strtol(argv[0], (char **)NULL, 10);
    hour = (int)strtol(argv[1], (char **)NULL, 10);
    min  = (int)strtol(argv[2], (char **)NULL, 10);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    memset(&tmReset, 0, sizeof(TIMESTAMP));
    tmReset.hour    = hour;
    tmReset.min        = min;
 
    invoke.AddParam("2.3.2", (int)day);
    invoke.AddParamFormat("2.3.25", VARSMI_TIMESTAMP, &tmReset, sizeof(TIMESTAMP));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionKeepalive(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int            n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (int)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.44", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetOptionSensorLimit(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int            n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (int)strtol(argv[0], (char **)NULL, 10);
    invoke.AddParam("2.3.69", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetOptionAutouploadWeekly(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         day, hour, min;
    int            trytime;

    day   = (int)strtol(argv[0], (char **)NULL, 10);
    hour  = (int)strtol(argv[1], (char **)NULL, 10);
    min   = (int)strtol(argv[2], (char **)NULL, 10);
    trytime = (int)strtol(argv[3], (char **)NULL, 10);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.26", (BYTE)3);
    invoke.AddParam("2.3.27", (int)day);
    invoke.AddParam("2.3.28", (BYTE)hour);
    invoke.AddParam("2.3.29", (BYTE)min);
    invoke.AddParam("2.3.30", (BYTE)3);
    invoke.AddParam("2.3.48", (int)trytime);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionAutouploadDaily(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         day, hour, min, trytime;

    sscanf(argv[0], "%x", &day);
    hour  = (int)strtol(argv[1], (char **)NULL, 10);
    min   = (int)strtol(argv[2], (char **)NULL, 10);
    trytime = (int)strtol(argv[3], (char **)NULL, 10);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.26", (BYTE)2);
    invoke.AddParam("2.3.27", (int)day);
    invoke.AddParam("2.3.28", (BYTE)hour);
    invoke.AddParam("2.3.29", (BYTE)min);
    invoke.AddParam("2.3.30", (BYTE)3);
    invoke.AddParam("2.3.48", (int)trytime);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionAutouploadHourly(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         hour, min, trytime;

    sscanf(argv[0], "%x", &hour);
    min      = (int)strtol(argv[1], (char **)NULL, 10);
    trytime = (int)strtol(argv[2], (char **)NULL, 10);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.26", (BYTE)4);
    invoke.AddParam("2.3.27", (int)hour);
    invoke.AddParam("2.3.28", (BYTE)0);
    invoke.AddParam("2.3.29", (BYTE)min);
    invoke.AddParam("2.3.30", (BYTE)3);
    invoke.AddParam("2.3.48", (int)trytime);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionAutouploadImmediately(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         trytime;

    trytime = (int)strtol(argv[0], (char **)NULL, 10);
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.26", (BYTE)1);
    invoke.AddParam("2.3.27", (int)0);
    invoke.AddParam("2.3.28", (BYTE)0);
    invoke.AddParam("2.3.29", (BYTE)0);
    invoke.AddParam("2.3.30", (BYTE)3);
    invoke.AddParam("2.3.48", (int)trytime);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionSchedulerType(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        nSchedulerType;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if(strncasecmp(argv[0],"mask", 4)==0)
    {
        nSchedulerType = (BYTE) SCHEDULER_TYPE_MASK;
    }
    else if(strncasecmp(argv[0],"int",3)==0) 
    {
        nSchedulerType = (BYTE) SCHEDULER_TYPE_INTERVAL;
    }
    else
    {
        nSchedulerType = (BYTE) strtol(argv[0], (char **)NULL, 10);
    }

    invoke.AddParam("2.3.86", nSchedulerType);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetOptionScanningStrategy(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE            n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n = (BYTE) strtol(argv[0], (char **)NULL, 10);
    if(strcasecmp(argv[0],"lazy")==0) n = (BYTE) SCANNING_STRATEGY_LAZY;
    else if(strcasecmp(argv[0],"immediately")==0) n = (BYTE) SCANNING_STRATEGY_IMMEDIATELY;

    invoke.AddParam("2.3.73", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetOptionMeteringSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        dayMask, hourMask;
    int            nRetry, nPeriod;
    BYTE        min;

    sscanf(argv[0], "%x", &dayMask);
    sscanf(argv[1], "%x", &hourMask);
    min = (BYTE)strtol(argv[2], (char **)NULL, 10);
    nRetry = (int)strtol(argv[3], (char **)NULL, 10);
    nPeriod = (int)strtol(argv[4], (char **)NULL, 10);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.15", (int)dayMask);
    invoke.AddParam("2.3.16", (int)hourMask);
    invoke.AddParam("2.3.17", (BYTE)min);
    invoke.AddParam("2.3.52", (int)nRetry);
    invoke.AddParam("2.3.50", (int)nPeriod);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionRecoverySchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        dayMask, hourMask;
    int            nRetry, nPeriod;
    BYTE        min;

    sscanf(argv[0], "%x", &dayMask);
    sscanf(argv[1], "%x", &hourMask);
    min = (BYTE)strtol(argv[2], (char **)NULL, 10);
    nRetry = (int)strtol(argv[3], (char **)NULL, 10);
    nPeriod = (int)strtol(argv[4], (char **)NULL, 10);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.18", (int)dayMask);
    invoke.AddParam("2.3.19", (int)hourMask);
    invoke.AddParam("2.3.20", (BYTE)min);
    invoke.AddParam("2.3.53", (int)nRetry);
    invoke.AddParam("2.3.51", (int)nPeriod);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionEventReadSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        dayMask, hourMask;

    sscanf(argv[0], "%x", &dayMask);
    sscanf(argv[1], "%x", &hourMask);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.65", (int)dayMask);
    invoke.AddParam("2.3.66", (int)hourMask);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionPollingSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        dayMask, hourMask;
    int            nRetry, nPeriod;
    BYTE        min;

    sscanf(argv[0], "%x", &dayMask);
    sscanf(argv[1], "%x", &hourMask);
    min = (BYTE)strtol(argv[2], (char **)NULL, 10);
    nRetry = (int)strtol(argv[3], (char **)NULL, 10);
    nPeriod = (int)strtol(argv[4], (char **)NULL, 10);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.54", (int)dayMask);
    invoke.AddParam("2.3.55", (int)hourMask);
    invoke.AddParam("2.3.56", (BYTE)min);
    invoke.AddParam("2.3.57", (int)nPeriod);
    invoke.AddParam("2.3.58", (int)nRetry);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionTimesyncSchedule(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        dayMask, hourMask;

    sscanf(argv[0], "%x", &dayMask);
    sscanf(argv[1], "%x", &hourMask);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.59", (int)dayMask);
    invoke.AddParam("2.3.60", (int)hourMask);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionTimesyncStrategy(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE       ts;

    ts = (BYTE) strtol(argv[0], (char **)NULL, 10);
    if (ts > TIMESYNC_STRATEGY_LAST_VALUE)
    {
        OUTTEXT(pSession, "Out of range.\r\n");
        return CLIERR_OK;
    }

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.84", ts);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "TimeSync Strategy changed.\r\n");
    return CLIERR_OK;
}


int cmdSetOptionTimesyncThreshold(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int            min, max;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    min = (int)strtol(argv[0], (char **)NULL, 10);
    max = (int)strtol(argv[1], (char **)NULL, 10);

    invoke.AddParam("2.3.71", min);
    invoke.AddParam("2.3.87", max);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionMemorycheck(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         n;

    n  = (int)strtol(argv[0], (char **)NULL, 10);
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.32", (BYTE)n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionFlashcheck(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         n;

    n  = (int)strtol(argv[0], (char **)NULL, 10);
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.33", (BYTE)n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionMobileLinkCheck(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         n;

    n  = (int)strtol(argv[0], (char **)NULL, 10);
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.64", (UINT)n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionNetworkTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT         n;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    n  = (UINT)strtol(argv[0], (char **)NULL, 10);

    invoke.AddParam("2.3.5", (UINT)(n*60));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionPingMethod(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         n;

    n  = (int)strtol(argv[0], (char **)NULL, 10);
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.72", (BYTE)n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionTimebroadcast(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         n;

    n  = (int)strtol(argv[0], (char **)NULL, 10);
    if (n < 60)
    {
        OUTTEXT(pSession, "Out of range: 60~n\r\n");
        return CLIERR_OK;
    }

    n = (n/10) * 10;
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.46", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionStatusMonitoring(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         n;

    n  = (int)strtol(argv[0], (char **)NULL, 10);
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.47", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionMeteringInterval(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT         nInterval;

    nInterval  = (UINT)strtol(argv[0], (char **)NULL, 10);

    /** Issue #10 : Interval은 60의 약수이어야 한다.
     *
     *  구현상의 문제때문에(Resource 부족) 1,2,3,4 값은 허용하지 않는다.
     *
     */
    switch(nInterval)
    {
        case 5:
        case 6:
        case 10:
        case 12:
        case 15:
        case 20:
        case 30:
        case 60:
            break;
        default:
            OUTTEXT(pSession, "Out of range.\r\n");
            return CLIERR_OK;
    }

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.61", nInterval);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionMeteringFixhour(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int         n;

    n  = (int)strtol(argv[0], (char **)NULL, 10);
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.5.4", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionMeterUser(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (strlen(argv[0]) > 10)
    {
        OUTTEXT(pSession, "Invalid length (max=10).\r\n");
        return CLIERR_OK;
    }
    
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.5.2", argv[0]);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionMeterSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        szSecurity[21];
    int            nMode;
    char        *p;
    int            i, n=0;

    memset(szSecurity, 0, 21);
    nMode = (strcasecmp(argv[0], "A") == 0) ? 0 : 1;
    if (nMode == 0)
    {
        // ASCII Mode
        if (strlen(argv[1]) > 20)
        {
                OUTTEXT(pSession, "Invalid length (max=20).\r\n");
            return CLIERR_OK;
        }
        strcpy((char *)szSecurity, argv[1]);
    }
    else
    {
        // HEX Mode
        if (strlen(argv[1]) > 60)
        {
                OUTTEXT(pSession, "Invalid length.\r\n");
            return CLIERR_OK;
        }

        for(n=0, p=argv[1]; p && *p; p++)
        {
            if (((n == 0) && (*p == ' ')) ||
                ((n == 1) && (*p == ' ')) ||
                ((n == 2) && (*p != ' ')))
            {
                    OUTTEXT(pSession, "Invalid HEX format. ex> set option metersecurity H \"4D 41 53 54 45 52 5F 30 31 32\"\r\n");
                   return CLIERR_OK;
            }
            n++;
            if (n > 2) n = 0;
        }

        for(i=0, p=argv[1]; p && *p && i<20; p+=3, i++)
        {
            sscanf(p, "%x", &n);
            szSecurity[i] = n;
        }
    }

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.5.3", szSecurity, 20);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetOptionEventAlertLevel(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int        n;

    n = (int)strtol(argv[0], (char **)NULL, 10);
    if ((n < 0) || (n > EVTCLASS_INFO))
    {
        OUTTEXT(pSession, "Out of range.\r\n");
        return CLIERR_OK;
    }

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.68", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionEventNotify(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BOOL    onoff=FALSE;

    if(!strcasecmp(argv[1],"on") || !strcasecmp(argv[1],"true") || !strcasecmp(argv[1],"high"))
    {
        onoff = TRUE;
    }else onoff = (int) strtol(argv[1],(char **)NULL,10) ? TRUE : FALSE;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("1.11", argv[0]);
    invoke.AddParam("1.3", (BOOL)FALSE);
    invoke.AddParam("1.4", (BYTE)0xFF);
    invoke.AddParam("1.3", (BOOL)onoff);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.29"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionEventSeverity(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int        n;

    n = (int)strtol(argv[1],(char **)NULL,10);

    if ((n < 0) || (n > EVTCLASS_INFO))
    {
        OUTTEXT(pSession, "Out of range.\r\n");
        return CLIERR_OK;
    }

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("1.11", argv[0]);
    invoke.AddParam("1.3", (BOOL)FALSE);
    invoke.AddParam("1.4", (BYTE)n);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.29"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionEventConfigurationDefault(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("1.11", argv[0]);
    invoke.AddParam("1.3", (BOOL)TRUE);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.29"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionEventSendDelay(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int        n;

    n = (int)strtol(argv[0], (char **)NULL, 10);
    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.67", n);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdSetOptionMeteringType(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int            nOption; 

    nOption = strtol(argv[0], (char **)NULL, 10); 

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.82", (BYTE)nOption);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "configuration changed.\r\n");
    return CLIERR_OK;
}


int cmdSetPatch(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    UINT        id;
    BOOL        bApply = FALSE;

    id = (int)strtol(argv[0],(char **)NULL,10);

    if(!strcasecmp(argv[1],"on") || !strcasecmp(argv[1],"true") || !strcasecmp(argv[1],"high"))
    {
        bApply = TRUE;
    }else bApply = (int)strtol(argv[1],(char **)NULL,10) == 1 ? TRUE : FALSE;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("1.6", id);
    invoke.AddParam("1.3", (BOOL)bApply);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.31"))
        return CLIERR_OK;
    return CLIERR_OK;
}

int cmdSetOptionSensorCleaningThreshold(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int            nDay; 

    nDay = strtol(argv[0],(char **)NULL,0);

    if(nDay < 0) nDay = 0;
    else if (nDay > 0xFF) nDay = 0xFF;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.83", (BYTE)nDay);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "configuration changed.\r\n");
    return CLIERR_OK;
}

int cmdSetOptionDumpFile(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    int len = strlen(argv[0]);
    char buff[512];

    if(len > (int)(sizeof(m_szDumpFile) - 1)) 
    {
        OUTTEXT(pSession, "Invalid File Name Error\r\n");
    }
    else
    {
        strcpy(m_szDumpFile, argv[0]);
        sprintf(buff,"/app/sw/dump.script %s", m_szDumpFile);
        system(buff);
    }
    return CLIERR_OK;
}

int cmdSetOptionTransactionThreshold(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    int            nDay; 

    nDay = strtol(argv[0],(char **)NULL,0);

    if(nDay < 0) nDay = 0;
    else if (nDay > 0xFF) nDay = 0xFF;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("2.3.85", (BYTE)nDay);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "199.2"))
        return CLIERR_OK;
    
    OUTTEXT(pSession, "configuration changed.\r\n");
    return CLIERR_OK;
}

int cmdAddGroup(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    EUI64       id;
    int         i;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

  	invoke.AddParamFormat("1.11", VARSMI_STRING, argv[0], strlen(argv[0]));
  	invoke.AddParamFormat("1.11", VARSMI_STRING, argv[1], strlen(argv[1]));
    for(i=2;i<argc;i++)
    {
	    StrToEUI64(argv[i], &id);
  	    invoke.AddParamFormat("1.14", VARSMI_EUI64, &id, sizeof(EUI64));
    }
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.70"))
		return CLIERR_OK;

	return CLIERR_OK;
}

#ifdef __SUPPORT_KUKDONG_METER__
/*--------------------------------------------------------------------------------
 *
 *	SYNTAX :
 *		set meter pulse <eui64id> <current pulse>
 *
 *	DESCRIPTION :
 *		지정된 센서와 연결된 미터의 현재 펄스값을 변경한다.
 *
 *	COMMAND :
 *		cmdSetMeterPulse(105.12)
 *				4.3.1		EUI64		EUI64 ID
 *				1.4			BYTE		Data Mask (0x00:Pulse만 수정)
 *				1.6			UINT		Current Pulse
 *				1.11		STRING		Meter serial (Do not used)
 *				
 *--------------------------------------------------------------------------------*/
int cmdSetMeterPulse(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
	EUI64		id;
    UINT		val;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

	StrToEUI64(argv[0], &id);
	val = (UINT) strtol(argv[1], (char **)NULL, 10);

    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", (BYTE)0x00);		// Mask (Pulse만, 0x00)
    invoke.AddParam("1.6", (UINT)val);		// Current pulse
    invoke.AddParam("1.11", "");			// Meter serial (do not changed)

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "105.14"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Serial of the METER is changed to '%d' successfully.\xd\xa", val);
	return CLIERR_OK;
}

/*--------------------------------------------------------------------------------
 *
 *	SYNTAX :
 *		set meter serial <eui64id> <current pulse> <meter serial>
 *
 *	DESCRIPTION :
 *		지정된 센서와 연결된 미터의 펄스와 시리얼을 변경한다.
 *		미터의 시리얼을 변경시에는 반드시 펄스도 같이 입력되어야 한다. (주의)
 *
 *	COMMAND :
 *		cmdSetMeterSerial(105.12)
 *				4.3.1		EUI64		EUI64 ID
 *				1.4			BYTE		Data Mask (0x01:Pulse, serial 모두 수정)
 *				1.6			UINT		Current Pulse
 *				1.11		STRING		Meter serial (Do not used)
 *				
 *--------------------------------------------------------------------------------*/
int cmdSetMeterSerial(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
	EUI64		id;
    UINT		val;

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

	StrToEUI64(argv[0], &id);
	val = (UINT) strtol(argv[1], (char **)NULL, 10);

    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", (BYTE)0x01);			// Mask (0x01: pulse, serial 모두)
    invoke.AddParam("1.6", (UINT)val);			// Current pulse
    invoke.AddParam("1.11", (char *)argv[2]);	// Meter serial

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "105.14"))
        return CLIERR_OK;

    OUTTEXT(pSession, "Serial of the METER is changed to '%s' successfully.\xd\xa", argv[2]);
	return CLIERR_OK;
}
#endif

