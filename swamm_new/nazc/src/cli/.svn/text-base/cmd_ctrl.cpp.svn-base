#include "common.h"
#include "cmd_ctrl.h"
#include "cmd_util.h"
#include "Variable.h"
#include "CLIInterface.h"
#include "CLIUtility.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "vendor/ansi.h"
#include "vendor/mccb.h"
#include "cmd_define.h"
#include "cmd_message.h"
#include "endiDefine.h"
#include "codiFrame.h"
#include "endiFrame.h"
#include "endiDefine.h"
#include "cmd_syntax.h"
//  [10/20/2010 DHKim]
#include "../agent/TransactionManager.h"
#include "kmpFunctions.h"

#define KMP_CMD_LPLOG       1
#define KMP_CMD_RTCLOG      2
#define KMP_CMD_STATUSLOG   3
#define KMP_CMD_VQLOG       4
#define KMP_CMD_CUTOFFLOG   5
#define KMP_CMD_ANALYSIS    6

extern const char	*hex_accept_str;

typedef struct	_tagGETABLE
{
		char	table[4];
		WORD	len;
		BYTE	data[0];
}	__attribute__ ((packed)) GETABLE;

extern const char	*dayofweek[];
extern UINT m_nLocalAgentPort;

extern CTransactionManager	*m_pTransactionManager;

extern WORD m_nKmpReadIdx;
extern WORD m_nKmpLastIdx;
BYTE m_nKmpCommand = 0;
EUI64 m_pPrevEUI64;

int cmdResetCodi(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	if (!Confirm(pSession, MSG_CMD_RESET_CODI))
		return CLIERR_OK;

	invoke.AddParam("1.4", 1);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.23"))
		return CLIERR_OK;

    OUTTEXT(pSession, MSG_SUCC_RESET_CODI "\r\n");

	return CLIERR_OK;
}

int cmdResetMobile(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	if (!Confirm(pSession, MSG_CMD_RESET_MOBILEDEV))
		return CLIERR_OK;

	invoke.AddParam("1.4", 0);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "100.23"))
		return CLIERR_OK;

    OUTTEXT(pSession, MSG_SUCC_RESET_MOBILEDEV "\r\n");
	return CLIERR_OK;
}

int cmdResetSensor(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
#endif
  	EUI64       id;
	SENSORINFOENTRYNEW	*pSensor = NULL;

  	if (!Confirm(pSession, MSG_DEFAULT))
    	return CLIERR_OK;

	StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.30");
    } 

  	invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));

    if(argc>1) {
  	    invoke.AddParam("1.4", (BYTE) strtol(argv[1],(char **)NULL,10));
    }

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

  	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.30"))
        return CLIERR_OK;

  	OUTTEXT(pSession, "Sensor reset.\r\n");
  	return CLIERR_OK;
}

int cmdAddSensor(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT + 10);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 10);
#endif
	EUI64		id;

	int			nType=0, nService=1;

    if (strcasecmp(argv[1], "zru") == 0)
        nType = 0;
    else if (strcasecmp(argv[1], "zmu") == 0)
        nType = 1;
    else if (strcasecmp(argv[1], "zeupls") == 0)
        nType = 2;
    if(strlen(argv[1]) == 1) nType = atoi(argv[1]);

    if (strcasecmp(argv[2], "elect") == 0)
        nService = 0;
    else if (strcasecmp(argv[2], "gas") == 0)
        nService = 1;
    else if (strcasecmp(argv[2], "water") == 0)
        nService = 2;
    if(strlen(argv[2]) == 1) nService = atoi(argv[2]);

	StrToEUI64(argv[0], &id);
	invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	invoke.AddParam("4.1.2", nType);			// Type
	invoke.AddParam("4.1.9", 0);			// Vendor
	invoke.AddParam("4.1.10", "");			// 미터 시리얼
	invoke.AddParam("4.1.11", nService);			// 서비스

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.1"))
		return CLIERR_OK;

    OUTTEXT(pSession, MSG_ADD_SENSOR "\r\n");
	return CLIERR_OK;
}

int cmdDeleteSensor(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
	EUI64		id;
	BOOL		bAll = FALSE;
	SENSORINFOENTRYNEW	*pSensor;
#if     defined(__SUPPORT_ADVANCED_LEAVE__)
    BYTE        channel = 0;
    WORD        panid = 0;
#endif

	if (argc == 0)
		bAll = TRUE;
		
	if (strcasecmp(argv[0], "all") == 0)
		bAll = TRUE;

#if     defined(__SUPPORT_ADVANCED_LEAVE__)
    if(argc > 1) channel = (BYTE) strtol(argv[1],(char **)NULL,10);
    if(argc > 2) panid = (WORD) strtol(argv[2],(char **)NULL,10);
#endif

	if (bAll)
	{
	    CIF4Invoke	invoke3("127.0.0.1", m_nLocalAgentPort, 60);
		if (!Confirm(pSession, MSG_DELETE_SENSOR_ALL))
			return CLIERR_OK;

#if     defined(__SUPPORT_ADVANCED_LEAVE__)
	    invoke3.AddParam("1.4", channel);
	    invoke3.AddParam("1.5", (WORD) htons(panid));
#endif
		if (!CLIAPI_Command(pSession, invoke3.GetHandle(), "102.3"))
			return CLIERR_OK;

    	OUTTEXT(pSession, MSG_SUCC_DELETE_SENSOR_ALL "\r\n");
		return CLIERR_OK;
	}

	if (!Confirm(pSession, MSG_DELETE_SENSOR))
		return CLIERR_OK;

	StrToEUI64(argv[0], &id);
    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	    return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.2");
    } 

	invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
#if     defined(__SUPPORT_ADVANCED_LEAVE__)
	invoke.AddParam("1.4", channel);
	invoke.AddParam("1.5", (WORD) htons(panid));
#endif

    if(NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.2"))
		return CLIERR_OK;

    OUTTEXT(pSession, MSG_SUCC_DELETE_SENSORL "\r\n");
	return CLIERR_OK;
}

int cmdDeleteMember(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	BOOL		bAll = FALSE;
    EUI64       id;

	if (argc == 0)
		bAll = TRUE;
		
	if (strcasecmp(argv[0], "all") == 0)
		bAll = TRUE;

	if (bAll)
	{
		if (!Confirm(pSession, MSG_DELETE_MEMBER_ALL))
			return CLIERR_OK;

		if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.21"))
			return CLIERR_OK;

    	OUTTEXT(pSession, MSG_SUCC_DELETE_MEMBER_ALL "\r\n");
		return CLIERR_OK;
	}

	StrToEUI64(argv[0], &id);

	if (!Confirm(pSession, MSG_DELETE_MEMBER))
		return CLIERR_OK;

  	invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.20"))
		return CLIERR_OK;

    OUTTEXT(pSession, MSG_SUCC_DELETE_MEMBER "\r\n");
	return CLIERR_OK;
}

int cmdMetering(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	EUI64		id;
	BOOL		bAll = FALSE;

	if (argc == 0)
		bAll = TRUE;
		
	if (strcasecmp(argv[0], "all") == 0)
		bAll = TRUE;

	if (bAll)
	{
		if (!Confirm(pSession, MSG_READ_METER_ALL))
			return CLIERR_OK;

		if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.2"))
			return CLIERR_OK;

    	OUTTEXT(pSession, MSG_SUCC_READ_METER_ALL "\r\n");
		return CLIERR_OK;
	}

	if (!Confirm(pSession, MSG_READ_METER))
		return CLIERR_OK;

	StrToEUI64(argv[0], &id);
	invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.1"))
		return CLIERR_OK;

    OUTTEXT(pSession, MSG_SUCC_READ_METER "\r\n", argv[0]);
	return CLIERR_OK;
}

int _showOndemand(CLISESSION *pSession, SENSORINFOENTRYNEW *pSensor, IF4Invoke *pInvoke, int nStartIdx, int nEndIdx)
{
	METERLPENTRY	*pItem;
	char		szGUID[MAX_GUID_STRING+1];
	char		szConnectTime[30];
	char		szInstallTime[30];
    UINT        parserType = PARSER_TYPE_UNKNOWN;
    BOOL        bDetail = TRUE;

    EUI64ToStr(&pSensor->sensorID, szGUID);
    IF4API_TIMESTAMPTOSTR(szConnectTime, &pSensor->sensorLastConnect);
    IF4API_TIMESTAMPTOSTR(szInstallTime, &pSensor->sensorInstallDate);

    parserType = getParserType(pSensor->sensorParser, pSensor->sensorModel);
    pItem = (METERLPENTRY *)pInvoke->pResult[nStartIdx]->stream.p;

    OUTTEXT(pSession, "=====================================================================================\xd\xa");
    OUTTEXT(pSession, "     SENSOR ID : %s\xd\xa", szGUID);
    if(pItem->mlpVendor)
    {
	    OUTTEXT(pSession, "        VENDOR : %s\xd\xa", getVendorString(pItem->mlpVendor));
    }
    OUTTEXT(pSession, "         MODEL : %s\xd\xa", pSensor->sensorModel);
    OUTTEXT(pSession, "     METER S/N : %s\xd\xa", pSensor->sensorSerialNumber);
    OUTTEXT(pSession, "         STATE : %s\xd\xa", MSGAPI_GetSensorState(pSensor->sensorState));
    OUTTEXT(pSession, "  NETWORK TYPE : %s\xd\xa", pSensor->sensorAttr & ENDDEVICE_ATTR_RFD ? "RFD" : "FFD");
    OUTTEXT(pSession, "  INSTALL DATE : %s\xd\xa", szInstallTime);
    OUTTEXT(pSession, " LAST METERING : %s\xd\xa", szConnectTime);

    OUTTEXT(pSession, " METERING DATA : %d (%d bytes)\xd\xa", pItem->mlpDataCount, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP));
    OUTTEXT(pSession, " METERING TIME : %04d/%02d/%02d %02d:%02d:%02d\xd\xa", 
        pItem->mlpTime.year,
        pItem->mlpTime.mon,
        pItem->mlpTime.day,
        pItem->mlpTime.hour,
        pItem->mlpTime.min,
        pItem->mlpTime.sec
    );
    switch(parserType) {
        case PARSER_TYPE_AIDON:
        case PARSER_TYPE_OSAKI:
            aidon_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_SX:
            sx_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_ANSI:
            ge_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_DLMS:
            dlms_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_PULSE:
        case PARSER_TYPE_ACD:
        case PARSER_TYPE_SMOKE:
        case PARSER_TYPE_HMU:
            pulse_data_parsing(pSession, pSensor->sensorModel, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_KAMST:
            kamstrup_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), TRUE);
            break;
        case PARSER_TYPE_REPEATER:
            repeater_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_MBUS:
            mbus_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_IHD:
            ihd_data_parsing(pSession, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
    }
    OUTTEXT(pSession, "=====================================================================================\xd\xa");

    return CLIERR_OK;
}

int cmdOndemand(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT + 120);
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 120);
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
#endif

	SENSORINFOENTRYNEW	*pSensor;

	int			nOption = -1;
	int			nOffset = -1;
	int			nCount = 0;
	EUI64		id;

	StrToEUI64(argv[0], &id);
	if (argc > 1)
	{
		if (strcasecmp(argv[1], "default") == 0)
			nOption = -1;
		else if (strcasecmp(argv[1], "selfread") == 0)
			nOption = 0;
		else if (strcasecmp(argv[1], "current") == 0)
			nOption = 1;
		else if (strcasecmp(argv[1], "previous") == 0)
			nOption = 2;
		else if (strcasecmp(argv[1], "relay") == 0)
			nOption = 3;
		else if (strcasecmp(argv[1], "event") == 0)
			nOption = 10;
		else if (strcasecmp(argv[1], "measurement") == 0)
			nOption = 12;
		else if (strcasecmp(argv[1], "demand") == 0)
			nOption = 16;
		else nOption = strtol(argv[1], (char **)NULL, 10); 

		if (argc > 2)
			nOffset = atoi(argv[2]);
		if (argc > 3)
			nCount = atoi(argv[3]);
	}

	if (!Confirm(pSession, MSG_READ_METER))
		return CLIERR_OK;

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	    return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "104.6");

	    invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	    invoke.AddParam("1.9", nOption);
	    invoke.AddParam("1.9", nOffset);
	    invoke.AddParam("1.9", nCount);

	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

    invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.9", nOption);
    invoke.AddParam("1.9", nOffset);
    invoke.AddParam("1.9", nCount);

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.6"))
        return CLIERR_OK;

    return _showOndemand(pSession, pSensor, invoke.GetHandle(), 0, invoke.GetHandle()->nResultCount);
}

int cmdOndemandBypass(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT + 120);
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 120);
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
#endif
    EUI64       id;
    BYTE        szData[1024];
    int         nMode;
    char        *p, *last=NULL;
    int         i, n=0, nDataLen=0;
    int         maxLen = sizeof(szData)-1;
    BOOL        bError = FALSE;
	SENSORINFOENTRYNEW	*pSensor;

	StrToEUI64(argv[0], &id);

    memset(szData, 0, sizeof(szData));
    nMode = (strcasecmp(argv[1], "A") == 0) ? 0 : 1;
    if (nMode == 0)
    {
        nDataLen = strlen(argv[2]);
        // ASCII Mode
        if (nDataLen <= 0)
        {
            return CLIERR_OK;
        }
        nDataLen = MIN(maxLen, nDataLen);
        strncpy((char *)szData, argv[2], nDataLen);
    }
    else
    {
        p = strtok_r(argv[2]," ",&last);
        for(nDataLen=0; p && *p && nDataLen < maxLen; nDataLen++) {
            n = strlen(p);
            if( n > 2 || n == 0) {
                bError = TRUE;
                break;
            }

            for(i=0;i<n;i++) {
		        if (strchr(hex_accept_str, p[i]) == NULL)
                {
                    OUTTEXT(pSession, "Invalid HEX format. ex> \"4D 41 53 54 45 52 5F 30 31 32\"\r\n");
                    return CLIERR_OK;
                }
            }
            sscanf(p, "%x", &n);
            szData[nDataLen] = (char)n;
            p = strtok_r(NULL, " ", &last);
        }

        if(bError) {
            OUTTEXT(pSession, "Invalid HEX format. ex> \"4D 41 53 54 45 52 5F 30 31 32\"\r\n");
            return CLIERR_OK;
        }
    }

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	    return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.37");

	    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	    invoke.AddParam("1.3", (BOOL)FALSE);
	    invoke.AddParamFormat("1.12", VARSMI_STREAM, szData, nDataLen);

	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

	invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	invoke.AddParam("1.3", (BOOL)FALSE);
	invoke.AddParamFormat("1.12", VARSMI_STREAM, szData, nDataLen);

    CLIAPI_Command(pSession, invoke.GetHandle(), "102.37");
    return CLIERR_OK;
}

int cmdOndemandRelay(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 60);
	CIF4Invoke	invoke2("127.0.0.1", m_nLocalAgentPort);
    EUI64       id;
	char		data[1];
	SENSORINFOENTRYNEW	*pSensor;	
		
	StrToEUI64(argv[0], &id);

    if (strcasecmp(argv[1], "clear") == 0)
	    data[0] = 0;
	else if (strcasecmp(argv[1], "active") == 0)
	    data[0] = 1;
	else if (strcasecmp(argv[1], "on") == 0)
	    data[0] = 2;
	else if (strcasecmp(argv[1], "off") == 0)
	    data[0] = 3;
	else data[0] = atoi(argv[1]);

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	    return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

	if(NeedAsync(pSensor->sensorAttr))
	{
		MakeAsyncParameter(&invoke, &id, "197.101");
		
		invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
		invoke.AddParam("1.9", 131);
	    invoke.AddParamFormat("1.12", VARSMI_STREAM, data, 1);
		
		if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
			return CLIERR_OK;
		
		OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
	} 

  	invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	invoke.AddParam("1.9", 131);
	invoke.AddParamFormat("1.12", VARSMI_STREAM, data, 1);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.101"))
		return CLIERR_OK;

	if (invoke.GetHandle()->pResult[0]->len > 0)
	{
		_showOndemand(pSession, pSensor, invoke.GetHandle(), 0, invoke.GetHandle()->nResultCount);
	}
	return CLIERR_OK;
}

int cmdOndemandCid(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
 	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 60);
    EUI64       id;
    BYTE        nCid = 0;
    BOOL        bStrCmd = FALSE;
    char        *szStrParam = NULL;
    int         nParamLen = 0;
    int         idx=0;
    BYTE        prevCmd, nCnt=0;
    WORD        wordVal;
    char        _common_arr[64];
    char        _statuslog_arr[]    = {0x02, 0x02, 0x04, 0x17, 0x00, 0x32, 0x08, 0xFF};
    char        _rtclog_arr[]       = {0x03, 0x03, 0x04, 0x17, 0x04, 0x18, 0x04, 0x15, 0x03, 0xFF};
    char        _vqlog_arr[]        = {0x04, 0x04, 0x04, 0x17, 0x03, 0xEC, 0x04, 0x3C, 0x04, 0x3D, 0x05, 0xFF};
    char        _cutofflog_arr[]    = {0x05, 0x04, 0x04, 0x17, 0x03, 0xEC, 0x04, 0x3F, 0x04, 0x40, 0x04, 0xFF};
    char        _lplog_arr[]        = {0x06, 0x05, 0x04, 0x17, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x05, 0xFF};
#if 0
    char        _analysis_arr[]     = {0x11, 0x03, 0x04, 0x17, 0x03, 0xFF, 0x04, 0x00, 0x03, 0xFF};
    char        _analysis_arr[]     = {0x11, 0x09, 0x04, 0x17, 0x03, 0xFF, 0x04, 0x00, 0x04, 0xBF, 0x04, 0xC0, 
                                                               0x04, 0xC1, 0x04, 0xC2, 0x04, 0xC3, 0x04, 0xC4, 0x01, 0xFF};
#endif

    char        _getclock_arr[]     = {0x05, 0x03, 0xEB, 0x03, 0xEA, 0x03, 0xEC, 0x04, 0x17, 0x04, 0x15};
    char        _getserial_arr[]    = {0x05, 0x03, 0xE9, 0x03, 0xF2, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35};
    char        _inv_arr[]          = {0x01, 0x04, 0x22};
    char        _load_arr[]         = {0x03, 0x00, 0x0D, 0x00, 0x01, 0x00, 0x3A};
    char        _voltage_arr[]      = {0x06, 0x04, 0x1E, 0x04, 0x1F, 0x04, 0x20, 0x04, 0xBF, 0x04, 0xC0, 0x04, 0xC1};
    char        _current_arr[]      = {0x06, 0x04, 0x34, 0x04, 0x35, 0x04, 0x36, 0x04, 0xC2, 0x04, 0xC3, 0x04, 0xC4};

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	StrToEUI64(argv[0], &id);
    if(memcmp(&m_pPrevEUI64, &id, sizeof(EUI64)))
    {
        m_nKmpReadIdx = m_nKmpLastIdx = m_nKmpCommand = 0;
        memcpy(&m_pPrevEUI64, &id, sizeof(EUI64));
    }

    if(check_hex(pSession, argv[1]))
    {
        nCid = (BYTE) strtol(argv[1], (char **)NULL, 16);
    }
    else
    {
        prevCmd = m_nKmpCommand;
        m_nKmpCommand = 0;

        if(!strncasecmp(argv[1], "Type", 4))            // Type
        {
            nCid = KMP_CID_GET_TYPE;
        }
        else if(!strncasecmp(argv[1], "Cl", 2))         // Clock
        {
            nCid = KMP_CID_GET_REGISTER;
            szStrParam = &_getclock_arr[0];
            nParamLen = sizeof(_getclock_arr);
        }
        else if(!strncasecmp(argv[1], "Load", 4))       // Load
        {
            nCid = KMP_CID_GET_REGISTER;
            szStrParam = &_load_arr[0];
            nParamLen = sizeof(_load_arr);
        }
        else if(!strncasecmp(argv[1], "Vol", 3))        // Voltage
        {
            nCid = KMP_CID_GET_REGISTER;
            szStrParam = &_voltage_arr[0];
            nParamLen = sizeof(_voltage_arr);
        }
        else if(!strncasecmp(argv[1], "Cur", 3))       // Current
        {
            nCid = KMP_CID_GET_REGISTER;
            szStrParam = &_current_arr[0];
            nParamLen = sizeof(_current_arr);
        }
        else if(!strncasecmp(argv[1], "Inv", 3))        // Inventory
        {
            nCid = KMP_CID_GET_REGISTER;
            szStrParam = &_inv_arr[0];
            nParamLen = sizeof(_inv_arr);
        }
        else if(!strncasecmp(argv[1], "Ser", 3))        // Serial
        {
            nCid = KMP_CID_GET_REGISTER;
            szStrParam = &_getserial_arr[0];
            nParamLen = sizeof(_getserial_arr);
        }
        else if(!strncasecmp(argv[1], "Sync", 4))       // Sync
        {
            nCid = KMP_CID_SET_CLOCK;
            kmpGetTimeDateStream((BYTE *)&_common_arr[0]);
            szStrParam = &_common_arr[0];
            nParamLen = 8;
        }
        else if(!strncasecmp(argv[1], "Power", 5))      // PowerLimit
        {
            nCid = KMP_CID_GET_POWER_LIMIT_STATUS;
        }
        else if(!strncasecmp(argv[1], "LpC", 3))        // LpConfig
        {
            nCid = KMP_CID_GET_LOAD_PROFILE_CONFIG;
        }
        else if(!strncasecmp(argv[1], "Rel", 3))        // RelayStatus
        {
            nCid = KMP_CID_GET_CUT_OFF_STATE;
        }
        else if(!strncasecmp(argv[1], "Discon", 6))     // Disconnect
        {
            nCid = KMP_CID_SET_CUT_OFF_STATE;
            _common_arr[0] = 0x01;
            szStrParam = &_common_arr[0];
            nParamLen = 1;
        }
        else if(!strncasecmp(argv[1], "Conn", 4))       // Connect
        {
            // (CutOff state 0x01)
            nCid = KMP_CID_SET_CUT_OFF_STATE;
            _common_arr[0] = 0x02;
            szStrParam = &_common_arr[0];
            nParamLen = 1;
        }
        else if(!strncasecmp(argv[1], "Recon", 5))      // Reconnect
        {
            // (CutOff state 0x08)
            nCid = KMP_CID_SET_CUT_OFF_STATE;
            _common_arr[0] = 0x03;
            szStrParam = &_common_arr[0];
            nParamLen = 1;
        }
        else if(!strncasecmp(argv[1], "GetEv", 5))      // GetEventStatus
        {
            nCid = KMP_CID_READ_EVENT_STATUS;
        }
        else if(!strncasecmp(argv[1], "GetSt", 5))      // GetStatusLog
        {
            nCid = KMP_CID_GET_LOG_LAST_PRESENT;
            szStrParam = &_statuslog_arr[0];
            nParamLen = sizeof(_statuslog_arr);
            m_nKmpCommand = KMP_CMD_STATUSLOG;
        }
        else if(!strncasecmp(argv[1], "GetRt", 5))      // GetRtcLog
        {
            nCid = KMP_CID_GET_LOG_LAST_PRESENT;
            szStrParam = &_rtclog_arr[0];
            nParamLen = sizeof(_rtclog_arr);
            m_nKmpCommand = KMP_CMD_RTCLOG;
        }
        else if(!strncasecmp(argv[1], "GetLp", 5))      // GetLpLog
        {
            /** Issue #538 : Kamstrup에서 LP Log에 한해서 KMP_CID_GET_LOG_LAST_PRESENT 를 더이상 지원하지 
              * 않는다 (K, L Type Meter). 따라서 CID를 변경한다.
              */
            nCid = KMP_CID_GET_LOG_ID_PRESENT;
            memset(_common_arr, 0, sizeof(_common_arr));
            memcpy(_common_arr, _lplog_arr, sizeof(_lplog_arr)); 
            szStrParam = &_common_arr[0];
            nParamLen = sizeof(_lplog_arr) + 2;
            m_nKmpCommand = KMP_CMD_LPLOG;
        }
        else if(!strncasecmp(argv[1], "GetV", 4))       // GetVQualityLog
        {
            nCid = KMP_CID_GET_LOG_LAST_PRESENT;
            szStrParam = &_vqlog_arr[0];
            nParamLen = sizeof(_vqlog_arr);
            m_nKmpCommand = KMP_CMD_VQLOG;
        }
        else if(!strncasecmp(argv[1], "GetCu", 5))      // GetCutOffLog
        {
            nCid = KMP_CID_GET_LOG_LAST_PRESENT;
            szStrParam = &_cutofflog_arr[0];
            nParamLen = sizeof(_cutofflog_arr);
            m_nKmpCommand = KMP_CMD_CUTOFFLOG;
        }
#if 0
        else if(!strncasecmp(argv[1], "An", 2))      // Analysis
        {
            nCid = KMP_CID_GET_LOG_LAST_PRESENT;
            szStrParam = &_analysis_arr[0];
            nParamLen = sizeof(_analysis_arr);
            m_nKmpCommand = KMP_CMD_ANALYSIS;
        }
#endif
        else if(!strncasecmp(argv[1], "next", 4))       // Next
        {
            if(!m_nKmpReadIdx) m_nKmpReadIdx = 1;

            m_nKmpCommand = prevCmd;
            switch(m_nKmpCommand)
            {
                case KMP_CMD_RTCLOG:
                    idx = sizeof(_rtclog_arr); memcpy(_common_arr, _rtclog_arr, idx); break;
                case KMP_CMD_STATUSLOG:
                    idx = sizeof(_statuslog_arr); memcpy(_common_arr, _statuslog_arr, idx); break;
                case KMP_CMD_LPLOG:
                    idx = sizeof(_lplog_arr); memcpy(_common_arr, _lplog_arr, idx); break;
                case KMP_CMD_VQLOG:
                    idx = sizeof(_vqlog_arr); memcpy(_common_arr, _vqlog_arr, idx); break;
                case KMP_CMD_CUTOFFLOG:
                    idx = sizeof(_cutofflog_arr); memcpy(_common_arr, _cutofflog_arr, idx); break;
#if 0
                case KMP_CMD_ANALYSIS:
                    idx = sizeof(_analysis_arr); memcpy(_common_arr, _analysis_arr, idx); break;
#endif
                default:
                    return CLIERR_ERROR;
            }

            nCid = KMP_CID_GET_LOG_ID_PRESENT;
            nCnt = _common_arr[idx-2];
            wordVal = m_nKmpReadIdx + 1;
            if(m_nKmpLastIdx < nCnt) wordVal = 1;
            else if(m_nKmpLastIdx - nCnt + 1 < wordVal) wordVal = m_nKmpLastIdx - nCnt + 1;

            wordVal = BigToHostShort(wordVal);
            memcpy(&_common_arr[idx], &wordVal, sizeof(WORD));

            szStrParam = &_common_arr[0];
            nParamLen = idx + sizeof(WORD);
        }
        else if(!strncasecmp(argv[1], "prev", 4))       // Previous
        {
            if(!m_nKmpReadIdx) m_nKmpReadIdx = 1;

            m_nKmpCommand = prevCmd;
            switch(m_nKmpCommand)
            {
                case KMP_CMD_RTCLOG:
                    idx = sizeof(_rtclog_arr); memcpy(_common_arr, _rtclog_arr, idx); break;
                case KMP_CMD_STATUSLOG:
                    idx = sizeof(_statuslog_arr); memcpy(_common_arr, _statuslog_arr, idx); break;
                case KMP_CMD_LPLOG:
                    idx = sizeof(_lplog_arr); memcpy(_common_arr, _lplog_arr, idx); break;
                case KMP_CMD_VQLOG:
                    idx = sizeof(_vqlog_arr); memcpy(_common_arr, _vqlog_arr, idx); break;
                case KMP_CMD_CUTOFFLOG:
                    idx = sizeof(_cutofflog_arr); memcpy(_common_arr, _cutofflog_arr, idx); break;
#if 0
                case KMP_CMD_ANALYSIS:
                    idx = sizeof(_analysis_arr); memcpy(_common_arr, _analysis_arr, idx); break;
#endif
                default:
                    return CLIERR_ERROR;
            }

            nCid = KMP_CID_GET_LOG_ID_PRESENT;
            nCnt = _common_arr[idx-2];
            wordVal = m_nKmpReadIdx > (nCnt*2) ? m_nKmpReadIdx - (nCnt*2) + 1 : 1;
            wordVal = BigToHostShort(wordVal);
            memcpy(&_common_arr[idx], &wordVal, sizeof(WORD));

            szStrParam = &_common_arr[0];
            nParamLen = idx + sizeof(WORD);
        }
        else if(!strncasecmp(argv[1], "last", 4))       // Last
        {
            m_nKmpCommand = prevCmd;
            switch(m_nKmpCommand)
            {
                case KMP_CMD_RTCLOG:
                    idx = sizeof(_rtclog_arr); memcpy(_common_arr, _rtclog_arr, idx); break;
                case KMP_CMD_STATUSLOG:
                    idx = sizeof(_statuslog_arr); memcpy(_common_arr, _statuslog_arr, idx); break;
                case KMP_CMD_LPLOG:
                    idx = sizeof(_lplog_arr); memcpy(_common_arr, _lplog_arr, idx); break;
                case KMP_CMD_VQLOG:
                    idx = sizeof(_vqlog_arr); memcpy(_common_arr, _vqlog_arr, idx); break;
                case KMP_CMD_CUTOFFLOG:
                    idx = sizeof(_cutofflog_arr); memcpy(_common_arr, _cutofflog_arr, idx); break;
#if 0
                case KMP_CMD_ANALYSIS:
                    idx = sizeof(_analysis_arr); memcpy(_common_arr, _analysis_arr, idx); break;
#endif
                default:
                    return CLIERR_ERROR;
            }

            nCid = KMP_CID_GET_LOG_ID_PRESENT;
            nCnt = _common_arr[idx-2];
            wordVal = m_nKmpLastIdx > nCnt ? m_nKmpLastIdx - nCnt + 1 : 1;
            wordVal = BigToHostShort(wordVal);
            memcpy(&_common_arr[idx], &wordVal, sizeof(WORD));

            szStrParam = &_common_arr[0];
            nParamLen = idx + sizeof(WORD);
        }
        else if(!strncasecmp(argv[1], "showi", 5))      // ShowIndex
        {
            m_nKmpCommand = prevCmd;
            OUTTEXT(pSession, "                 + LatestReadLogId %d NewLogId %d\r\n", m_nKmpReadIdx, m_nKmpLastIdx);
            return CLIERR_OK;
        }
        else if(!strncasecmp(argv[1], "cleari", 6))     // ClearIndex
        {
            m_nKmpReadIdx = m_nKmpLastIdx = 0;
            return CLIERR_OK;
        }
        else
        {
            m_nKmpCommand = prevCmd;
            OUTTEXT(pSession, "Invalid CID");
            return CLIERR_INVALID_PARAM;
        }
        bStrCmd = TRUE;
    }

    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", nCid);

    OUTTEXT(pSession, "REQUEST %s\r\n", kmpGetCidString(nCid));

    if(bStrCmd)
    {
        if(szStrParam && nParamLen)
        {
            invoke.AddParamFormat("1.12", VARSMI_STREAM, szStrParam, nParamLen);
        }
    }
    else if(argc > 2)
    {
        char strBuff[1024];

        if((GetHexaList(argv[2], strBuff, sizeof(strBuff), &idx) == CLIERR_OK) && idx > 0)
        {
            invoke.AddParamFormat("1.12", VARSMI_STREAM, strBuff, idx);
        }

    }


	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "105.8"))
		return CLIERR_OK;

    if(invoke.GetHandle()->nResultCount < 1) {
        OUTTEXT(pSession, "Invalid Result");
		return CLIERR_OK;
    }

    kamstrup_data_parsing(pSession, invoke.GetHandle()->pResult[0]->stream.p, invoke.GetHandle()->pResult[0]->len, FALSE);

    return CLIERR_OK;
}

int cmdOndemandMccb(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 60);
	char		*msg;
    EUI64       id;
	BYTE		cmd, value = 0, reply;
		
	StrToEUI64(argv[0], &id);
	cmd = atoi(argv[1]);

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	switch(cmd) {
	  case 1 : value = MCCB_SET_ENABLE_USE_DISCONNECTED; break;
	  case 2 : value = MCCB_SET_ENABLE_USE_AUTOMATICALLY; break;
	  case 3 : value = MCCB_SET_ENABLE_USE_CONNECT_NOW; break;
	  case 4 : value = MCCB_SET_DISABLE_USE_DISCONNECT; break;
	  case 5 : value = MCCB_GET_DEVICE_STATUS; break;
	  case 6 : value = MCCB_GET_PHASE_STATUS; break;
	  case 7 : value = MCCB_GET_LAST_ACCEPT_CONTROL_MSG; break;
	  default :
		   OUTTEXT(pSession, "Unknown command.\r\n");
		   return CLIERR_OK;
	}

  	invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	invoke.AddParam("1.4", value);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "105.7"))
		return CLIERR_OK;

	reply = invoke.GetHandle()->pResult[0]->stream.u8;
	switch(value) {
	  case MCCB_SET_ENABLE_USE_DISCONNECTED : 	// Enable Use - Disconnected
	  case MCCB_SET_ENABLE_USE_AUTOMATICALLY : 	// Enable Use - Automatically
	  case MCCB_SET_ENABLE_USE_CONNECT_NOW : 	// Enable Use - Connect Now
	  case MCCB_SET_DISABLE_USE_DISCONNECT : 	// Disable Use - Disconnect
		   msg = const_cast<char *>("Acknowledge");
		   OUTTEXT(pSession, "Request     = 0x%02X\r\n", value);
		   switch(reply) {
			 case MCCB_ACK_OK : 			OUTTEXT(pSession, "%s = OK\r\n", msg); break;
			 case MCCB_ACK_ERR_DE : 		OUTTEXT(pSession, "%s = Error - DE\r\n", msg); break;
			 case MCCB_ACK_ERR_PE : 		OUTTEXT(pSession, "%s = Error - PE\r\n", msg); break;
			 case MCCB_ACK_ERR_DE_PE : 		OUTTEXT(pSession, "%s = Error - DE+PE\r\n", msg); break;
			 case MCCB_ACK_ERR_REJECTED : 	OUTTEXT(pSession, "%s = Error - Rejected\r\n", msg); break;
			 case MCCB_ACK_ERR_UNDEFINED : 	OUTTEXT(pSession, "%s = Error - Undefined\r\n", msg); break;
			 case MCCB_FAIL : 				OUTTEXT(pSession, "%s = MCCB Communication Fail\r\n", msg); break;
			   default : OUTTEXT(pSession, "%s = 0x%02X\r\n", msg, reply); break;
		   }
		   break;

	  case MCCB_GET_DEVICE_STATUS : 			// Get Device Status
		   msg = const_cast<char *>("Device Status");
		   OUTTEXT(pSession, "PE  = Phase Error (One or more phases missing)\r\n");
		   OUTTEXT(pSession, "DE  = Device Error (Severe error with device, No self recovery)\r\n");
		   OUTTEXT(pSession, "NDE = No Device Error\r\n");
		   OUTTEXT(pSession, "RE  = Relay Error (unable to change the state of the relays;with Device Error flag only)\r\n");
		   OUTTEXT(pSession, "BE  = Button Error (with Device Error flag only)\r\n");
		   OUTTEXT(pSession, "\r\n");
		   OUTTEXT(pSession, "Request       = 0x%02X\r\n", value);

		   switch(reply) {
			 case MCCB_STATUS_NDE_NPE : 	OUTTEXT(pSession, "%s = NDE;NPE\r\n", msg); break;
			 case MCCB_STATUS_NDE_PE : 		OUTTEXT(pSession, "%s = NDE;PE\r\n", msg); break;
			 case MCCB_STATUS_DE_NPE : 		OUTTEXT(pSession, "%s = DE;NPE\r\n", msg); break;
			 case MCCB_STATUS_DE_PE : 		OUTTEXT(pSession, "%s = DE;PE\r\n", msg); break;
			 case MCCB_STATUS_DE_BE : 		OUTTEXT(pSession, "%s = DE+BE\r\n", msg); break;
			 case MCCB_STATUS_DE_BE_PE : 	OUTTEXT(pSession, "%s = DE+BE;PE\r\n", msg); break;
			 case MCCB_STATUS_DE_RE :		OUTTEXT(pSession, "%s = DE+RE\r\n", msg); break;
			 case MCCB_STATUS_DE_RE_PE :	OUTTEXT(pSession, "%s = DE+RE;PE\r\n", msg); break;
			 case MCCB_STATUS_DE_RE_BE :	OUTTEXT(pSession, "%s = DE+RE+BE\r\n", msg); break;
			 case MCCB_STATUS_DE_RE_BE_PE :	OUTTEXT(pSession, "%s = DE+RE+BE;PE\r\n", msg); break;
			 case MCCB_FAIL : 				OUTTEXT(pSession, "%s = MCCB Communication Fail\r\n", msg); break;
			   default : OUTTEXT(pSession, "%s = 0x%02X\r\n", msg, reply); break;
		   }
		   break;

	  case MCCB_GET_PHASE_STATUS : 				// Get Phase Status
		   msg = const_cast<char *>("Phase Status");
		   OUTTEXT(pSession, "Request      = 0x%02X\r\n", value);
		   switch(reply) {
			 case MCCB_PHASE_ALL_DETECTED :	OUTTEXT(pSession, "%s = All phases detected\r\n", msg); break;
			 case MCCB_PHASE_MISSING_1 : 	OUTTEXT(pSession, "%s = Phase 1 missing\r\n", msg); break;
			 case MCCB_PHASE_MISSING_2 : 	OUTTEXT(pSession, "%s = Phase 2 missing\r\n", msg); break;
			 case MCCB_PHASE_MISSING_12 : 	OUTTEXT(pSession, "%s = Phases 1 and 2 missing\r\n", msg); break;
			 case MCCB_PHASE_MISSING_3 : 	OUTTEXT(pSession, "%s = Phase 3 missing\r\n", msg); break;
			 case MCCB_PHASE_MISSING_13 : 	OUTTEXT(pSession, "%s = Phases 1 and 3 missing\r\n", msg); break;
			 case MCCB_PHASE_MISSING_23 : 	OUTTEXT(pSession, "%s = Phases 2 and 3 missing\r\n", msg); break;
			 case MCCB_PHASE_MISSING_123 : 	OUTTEXT(pSession, "%s = Phases 1, 2 and 3 missing\r\n", msg); break;
			 case MCCB_FAIL : 				OUTTEXT(pSession, "%s = MCCB Communication Fail\r\n", msg); break;
			   default : OUTTEXT(pSession, "%s = 0x%02X\r\n", msg, reply); break;
		   }
		   break;

	  case MCCB_GET_LAST_ACCEPT_CONTROL_MSG : 	// Get Last Accepted Control Msg
		   msg = const_cast<char *>("Last Accepted Control Msg");
		   OUTTEXT(pSession, "E = Device error and/or phase error\r\n");
		   OUTTEXT(pSession, "\r\n");
		   OUTTEXT(pSession, "Request                   = 0x%02X\r\n", value);

		   switch(reply) {
			 case MCCB_MSG_OK_EN_USE_DISCONNECTED : OUTTEXT(pSession, "%s = OK+Enable Use - Disconnected\r\n", msg); break;
			 case MCCB_MSG_OK_EN_USE_AUTO : 		OUTTEXT(pSession, "%s = OK+Enable Use - Automatically\r\n", msg); break;
			 case MCCB_MSG_OK_EN_USE_CONNECTED : 	OUTTEXT(pSession, "%s = OK+Enable Use - Connected\r\n", msg); break;
			 case MCCB_MSG_OK_DI_USE_DISCONNECT : 	OUTTEXT(pSession, "%s = OK+Disable Use - Disconnect\r\n", msg); break;
			 case MCCB_MSG_ERR_EN_USE_DISCONNECTED : OUTTEXT(pSession, "%s = E+Enable Use - Disconnected\r\n", msg); break;
			 case MCCB_MSG_ERR_EN_USE_AUTO : 		OUTTEXT(pSession, "%s = E+Enable Use - Automatically\r\n", msg); break;
			 case MCCB_MSG_ERR_EN_USE_CONNECTED : 	OUTTEXT(pSession, "%s = E+Enable Use - Connected\r\n", msg); break;
			 case MCCB_MSG_ERR_DI_USE_DISCONNECT : 	OUTTEXT(pSession, "%s = E+Disable Use - Disconnect\r\n", msg); break;
			 case MCCB_FAIL : 						OUTTEXT(pSession, "%s = MCCB Communication Fail\r\n", msg); break;
			   default : OUTTEXT(pSession, "%s = 0x%02X\r\n", msg, reply); break;
		   }
		   break;

	  default :
		   OUTTEXT(pSession, "Result = 0x%02X\r\n", reply);
		   break;
	}
	return CLIERR_OK;
}

int cmdOndemandMbus(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
 	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 60);
    EUI64       id;
    BYTE        nPort, nAction, nCmd;

	StrToEUI64(argv[0], &id);
    nPort = (BYTE)strtol(argv[1], (char **)NULL, 10);
    nAction = (BYTE)strtol(argv[2], (char **)NULL, 10);
    nCmd = (BYTE)strtol(argv[3], (char **)NULL, 10);

    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke.AddParam("1.4", nPort);
    invoke.AddParam("1.4", nAction);
    invoke.AddParam("1.4", nCmd);

    if(argc > 4)
    {
        char strBuff[1024];
        int idx = 0;

        if((GetHexaList(argv[4], strBuff, sizeof(strBuff), &idx) == CLIERR_OK) && idx > 0)
        {
            invoke.AddParamFormat("1.12", VARSMI_STREAM, strBuff, idx);
        }
    }

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.42"))
		return CLIERR_OK;

    DisplayCommandResult(pSession, (IF4Invoke *)invoke.GetHandle());

    return CLIERR_OK;
}

int cmdOndemandTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT + 120);
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 120);
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
#endif

	SENSORINFOENTRYNEW	*pSensor;
	EUI64		id;

	StrToEUI64(argv[0], &id);

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
	    return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "105.5");

	    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));

	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

    invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "105.5"))
        return CLIERR_OK;

    return _showOndemand(pSession, pSensor, invoke.GetHandle(), 0, invoke.GetHandle()->nResultCount);
}

int cmdOndemandMeterFailRecovery(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 120);
	METERLPENTRY	*pData;
	STAT_TABLE	*pStat;
	GETABLE		*pTable;
	char		szGUID[20];
	char		szSerial[21];
	int			i, len, nSeek;
	int			nElapse, nTable = 13; // Meter Timesync & LP Start
	int			nOffset = 0;
	int			nCount = 0;
    int         nLength = 0;
	EUI64		id;

	StrToEUI64(argv[0], &id);

	if (!Confirm(pSession, MSG_WRITE_METER))
		return CLIERR_OK;

	invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	invoke.AddParam("1.9", nTable);
	invoke.AddParam("1.9", nOffset);
	invoke.AddParam("1.9", nCount);

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.6"))
		return CLIERR_OK;

	pData = (METERLPENTRY *)invoke.GetHandle()->pResult[0]->stream.p;
	EUI64ToStr(&pData->mlpId, szGUID);
	memset(szSerial, 0, 20);
	strncpy(szSerial, pData->mlpMid, 20);
	szSerial[19] = '\0';

    // FEP의 요청으로 pData->lenght에 실제 data Length에 TIMESTAMP 길이까지
    // 포함해서 보낸다. 따라서 cli에서 TIMESTAMP 길이를 빼야 한다 
    nLength = LittleToHostShort(pData->mlpLength) - sizeof(TIMESTAMP);

	OUTTEXT(pSession, "\r\n");
	OUTTEXT(pSession, "Sensor ID : %s       Data Size : %d Byte(s)\r\n", szGUID, nLength);
	if (nLength <= 0)
	{
		OUTTEXT(pSession, "\r\n");
		OUTTEXT(pSession, "Ondemand metering fail.\r\n");
	}

	// GE I210/KV2C Ondemand
	OUTTEXT(pSession, "\r\n");
	for(i=0, nSeek=0; i<nLength; i+=nSeek)
	{
		pTable = (GETABLE *)(pData->mlpData + i);
		nSeek = sizeof(GETABLE);
		len   = BigToHostShort(pTable->len);
		nSeek += len;

		if (strncmp(pTable->table, "T001", 4) == 0)
		{
			pStat = (STAT_TABLE *)&pTable->data;
		
	       	if (pStat->TRANSMIT_PKT_COUNT == 0)
 	            nElapse = 0;
 	       	else nElapse = pStat->TOTAL_REPLY_WAIT_MSEC / pStat->TRANSMIT_PKT_COUNT;

			OUTTEXT(pSession, "Metering Statistics:\r\n");
        	OUTTEXT(pSession, " Transmit Count         = %7d          Error Packet Count    = %7d\r\n",
						pStat->TRANSMIT_PKT_COUNT, pStat->ERROR_PKT_COUNT);
        	OUTTEXT(pSession, " Total Send             = %7d byte(s)  Total Receive         = %7d byte(s)\r\n",
						pStat->TOTAL_SEND_BYTES, pStat->TOTAL_RECV_BYTES);
        	OUTTEXT(pSession, " Total Response Time    = %7d ms       Average Response Time = %7d ms\r\n",
						pStat->TOTAL_REPLY_WAIT_MSEC, nElapse);
        	OUTTEXT(pSession, " Minimum Response Time  = %7d ms       Maximum Response Time = %7d ms\r\n",
						pStat->MIN_REPLY_MSEC, pStat->MAX_REPLY_MSEC);
        	OUTTEXT(pSession, " Sensor Connect Time    = %7d ms       Total Use Time        = %7d ms\r\n",
						pStat->SENSOR_CONNECT_MSEC, pStat->TOTAL_USE_MSEC);
		}
	}
	return CLIERR_OK;
}

int cmdMeterTimesync(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 60);
		
	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "105.6"))
		return CLIERR_OK;
	return CLIERR_OK;
}

int cmdCheckMeter(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 60);
	GETABLE		*pList;
	ANSI_ST_TABLE1	*pTable;
	METER_STAT	*pStat;
    EUI64       id;
	char		szSerial[20], *pszBuffer;
	char		szManufacturer[10];
	char		szModel[10], *p;
	char		szHw[10], szFw[10];
	char		*p1, *p2;
	int			nLength, nElapse;

	StrToEUI64(argv[0], &id);
	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

  	invoke.AddParamFormat("4.2.3", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "104.30"))
		return CLIERR_OK;

	OUTTEXT(pSession, "\r\n");

	p = (char *)invoke.GetHandle()->pResult[0]->stream.p;
	pList = (GETABLE *)invoke.GetHandle()->pResult[0]->stream.p;
	pStat = (METER_STAT *)invoke.GetHandle()->pResult[1]->stream.p;

	if (strncmp(p, "/ADN", 4) == 0)
	{
		nLength = invoke.GetHandle()->pResult[0]->len;
        pszBuffer = (char *)MALLOC(nLength+1);
        if (pszBuffer != NULL)
        {   
            memcpy(pszBuffer, p, nLength);
            pszBuffer[nLength] = '\0';
            
            p1 = strchr(pszBuffer, 0x02);
            p2 = strchr(pszBuffer, 0x03);
            if (p1 && p2)
            {
                *p1 = '\0';
                *(p2-3) = '\0';
                OUTTEXT(pSession, p1+1);
            }
            FREE(pszBuffer);
        }
	}
	else if (strncmp(pList->table, "S001", 4) == 0)
	{
		pTable = (ANSI_ST_TABLE1 *)pList->data;
		XDUMP((char *)pList->data, BigToHostShort(pList->len), TRUE);

		memset(szSerial, 0, 20);
		memcpy(szManufacturer, pTable->MANUFACTURER, 4); szManufacturer[4] = 0;
		memcpy(szModel, pTable->ED_MODEL, 8); szModel[8] = 0;
		sprintf(szHw, "%0d.%0d", pTable->HW_VERSION_NUMBER, pTable->HW_REVISION_NUMBER);
		sprintf(szFw, "%0d.%0d", pTable->FW_VERSION_NUMBER, pTable->FW_REVISION_NUMBER);
		memcpy(szSerial, pTable->MFG_SERIAL_NUMBER, 16); szSerial[16] = 0;

		OUTTEXT(pSession, "               Address = %s\r\n", argv[0]);
		OUTTEXT(pSession, " Manufacturer code     = %-10s            Model identifier = %s\r\n",
							szManufacturer, szModel);
		OUTTEXT(pSession, " Hardware version      = %-10s            Firmware version = %s\r\n",
							szHw, szFw);
		OUTTEXT(pSession, " Factory serial number = %s\r\n", szSerial);
	}

	if (pStat->nCount == 0)
         nElapse = 0;
    else nElapse = pStat->nResponseTime / pStat->nCount;

	OUTTEXT(pSession, "\r\n");
   	OUTTEXT(pSession, " Transmit Count        = %7d          Error Packet Count    = %7d\r\n",
						pStat->nCount, pStat->nError);
   	OUTTEXT(pSession, " Total Send            = %7d byte(s)  Total Receive         = %7d byte(s)\r\n",
						pStat->nSend, pStat->nRecv);
   	OUTTEXT(pSession, " Total Response Time   = %7d ms       Average Response Time = %7d ms\r\n",
						pStat->nResponseTime, nElapse);
   	OUTTEXT(pSession, " Minimum Response Time = %7d ms       Maximum Response Time = %7d ms\r\n",
						pStat->nMinTime, pStat->nMaxTime);
   	OUTTEXT(pSession, " Sensor Connect Time   = %7d ms       Total Use Time        = %7d ms\r\n",
						pStat->nConnectTime, pStat->nUseTime);
	return CLIERR_OK;
}

int cmdMulticastTime(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.5"))
		return CLIERR_OK;

	OUTTEXT(pSession, "Send timesync to all sensor.\r\n");
	return CLIERR_OK;
}

int cmdDeleteTransaction(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	WORD        trID;
    int         i;
    BYTE        nOption = TR_STATE_TERMINATE;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

    for(i=0; i<argc; i++)
    {
        trID = (WORD) strtol(argv[i],(char **)NULL,10);
		invoke.AddParam("1.5", trID);
		invoke.AddParam("1.4", nOption);
    }
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.43"))
        return CLIERR_OK;

	OUTTEXT(pSession, "Transaction(s) modified.\r\n");
	return CLIERR_OK;
}

int cmdAddCoordiRegister(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	EUI64		id;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	StrToEUI64(argv[0], &id);
    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "101.26"))
    {
	    return CLIERR_OK;
    }

    OUTTEXT(pSession, MSG_SUCC_ADD_REGISTER "\r\n");
	return CLIERR_OK;
}

int cmdDeleteCoordiRegister(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	EUI64		id;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	StrToEUI64(argv[0], &id);
    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "101.27"))
    {
	    return CLIERR_OK;
    }

    OUTTEXT(pSession, MSG_SUCC_DELETE_REGISTER "\r\n");
	return CLIERR_OK;
}

int cmdQuery(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	int			nOption = 0;
	int			nOffset = -1;
	int			nCount = -1;

	if (argc > 0)
	{
		if (strcasecmp(argv[0], "default") == 0)
			nOption = -1;
		else if (strcasecmp(argv[0], "selfread") == 0)
			nOption = 0;
		else if (strcasecmp(argv[0], "current") == 0)
			nOption = 1;
		else if (strcasecmp(argv[0], "previous") == 0)
			nOption = 2;
		else if (strcasecmp(argv[0], "relay") == 0)
			nOption = 3;
		else if (strcasecmp(argv[0], "event") == 0)
			nOption = 10;
		else if (strcasecmp(argv[0], "measurement") == 0)
			nOption = 12;
		else if (strcasecmp(argv[0], "demand") == 0)
			nOption = 16;
		else nOption = strtol(argv[0], (char **)NULL, 10); 
	}

    if (argc > 1)
    {
        nOffset = strtol(argv[1],(char **) NULL, 10);
    }

    if (argc > 2)
    {
        nCount = strtol(argv[2],(char **) NULL, 10);
    }

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

	invoke.AddParam("1.9", nOption);
	invoke.AddParam("1.9", nOffset);
	invoke.AddParam("1.9", nCount);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.102"))
		return CLIERR_OK;

	OUTTEXT(pSession, "Startup query.\r\n");
	return CLIERR_OK;
}

int cmdMeterPingStart(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	invoke.AddParam("1.4", (BYTE)1);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.11"))
		return CLIERR_OK;

	OUTTEXT(pSession, "Meter ping start . . .\r\n");
	return CLIERR_OK;
}

int cmdMeterPingStop(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	invoke.AddParam("1.4", (BYTE)0);
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "197.11"))
		return CLIERR_OK;

	OUTTEXT(pSession, "Meter ping stop . . .\r\n");
	return CLIERR_OK;
}

int cmdAddApn(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	int			i;

	for(i=0; i<argc; i++)
		invoke.AddParam("1.11", (char *)argv[i]);					// stringEntry

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "120.1"))		// cmdAddApn
		return CLIERR_OK;

	OUTTEXT(pSession, "%s is added.\r\n", argv[0]);
	return CLIERR_OK;
}

int cmdDeleteApn(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	invoke.AddParam("1.11", (char *)argv[0]);						// stringEntry
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "120.2"))		// cmdDeleteApn
		return CLIERR_OK;

	OUTTEXT(pSession, "%s is deleted.\r\n", argv[0]);
	return CLIERR_OK;
}
int cmdDeleteMeter(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    EUI64       id;
    BOOL        bAll = FALSE;

    if (argc == 0)
        bAll = TRUE;

    if (strcasecmp(argv[0], "all") == 0)
        bAll = TRUE;


    if (bAll)
    {
        CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort, 60);
        if (!Confirm(pSession, MSG_DELETE_SENSOR_ALL))
            return CLIERR_OK;

        if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.47"))
            return CLIERR_OK;

        OUTTEXT(pSession, MSG_SUCC_DELETE_SENSOR_ALL "\r\n");
        return CLIERR_OK;
    }

    if (!Confirm(pSession, MSG_DELETE_SENSOR))
        return CLIERR_OK;

    StrToEUI64(argv[0], &id);
    invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    
    invoke.AddParam("1.4", atoi(argv[1]));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.47"))
        return CLIERR_OK;

    OUTTEXT(pSession, MSG_SUCC_DELETE_SENSORL "\r\n");

    return CLIERR_OK;

}

int cmdSetSensorPort(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
#endif
    CIF4Invoke  invoke2("127.0.0.1", m_nLocalAgentPort);
  	EUI64       id;
    BYTE        mask, value;
	SENSORINFOENTRYNEW	*pSensor = NULL;
    ENDI_DIO_PAYLOAD   *dio;

  	if (!Confirm(pSession, MSG_DEFAULT))
    	return CLIERR_OK;

	StrToEUI64(argv[0], &id);

    invoke2.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke2.GetHandle(), "102.27"))
        return CLIERR_OK;
    pSensor = (SENSORINFOENTRYNEW *)invoke2.GetHandle()->pResult[0]->stream.p;

    if(NeedAsync(pSensor->sensorAttr))
    {
        MakeAsyncParameter(&invoke, &id, "102.49");
    } 

  	invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));

 	mask = (BYTE) strtol(argv[1],(char **)NULL,16); // mask
 	value = (BYTE) strtol(argv[2],(char **)NULL,16); // value

 	invoke.AddParam("1.4", (BYTE) DIGITAL_OUTPUT_REQUEST);       // direction
 	invoke.AddParam("1.4", mask);
 	invoke.AddParam("1.4", value);

    if(pSensor && NeedAsync(pSensor->sensorAttr)) {
	    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.40"))
		    return CLIERR_OK;

	    OUTTEXT(pSession, "Request ID=%u\xd\xa", invoke.GetHandle()->pResult[0]->stream.u16);
		return CLIERR_OK;
    } 

  	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.49"))
        return CLIERR_OK;

    if(invoke.GetHandle()->nResultCount>0) 
    {
        dio = (ENDI_DIO_PAYLOAD *)invoke.GetHandle()->pResult[0]->stream.p;

        OUTTEXT(pSession, "Sensor Digital Port Set MASK 0x%02X VALUE 0x%02X\r\n", mask, value);
        OUTTEXT(pSession, " + ID %s MASK 0x%02X VALUE 0x%02X\r\n", argv[0], dio->mask, dio->value);
    }
  	return CLIERR_OK;
}

int cmdGroupAsyncCall(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	OID3	cmd;
	char	nNice;
	BYTE	nOption, nDays, nTry;

	VARAPI_StringToOid(argv[1], &cmd);
	nOption = (BYTE)strtol(argv[2], (char **)NULL, 10);
	nDays = (BYTE)strtol(argv[3],(char **)NULL, 10);
	nNice = (char)strtol(argv[4],(char **)NULL, 10);
	nTry = (BYTE)strtol(argv[5], (char**)NULL, 10);

	invoke.AddParam("1.9", atoi(argv[0]));
	invoke.AddParam("1.10",&cmd);
	invoke.AddParam("1.4", nOption);
	invoke.AddParam("1.4", nDays);
	invoke.AddParam("1.7", nNice);
	invoke.AddParam("1.4", nTry);	

	OUTTEXT(pSession, "Enter CLI Command!!\r\n");
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.53"))
		return CLIERR_OK;

	return CLIERR_OK;
}

//////////////////////////////////////////////////////////////////////////
//  [10/20/2010 DHKim]
//  CLI Group Add (102.54)
//////////////////////////////////////////////////////////////////////////
int cmdGroupAdd(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	invoke.AddParamFormat("1.11",VARSMI_STRING, argv[0], strlen(argv[0]));
	
	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.54"))
		return CLIERR_OK;

	return CLIERR_OK;
}
//////////////////////////////////////////////////////////////////////////
//  [10/20/2010 DHKim]
//  CLI Group delete (102.55)
//////////////////////////////////////////////////////////////////////////
int cmdGroupDelete(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	invoke.AddParam("1.9", atoi(argv[0]));

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.55"))
		return CLIERR_OK;

	return CLIERR_OK;

}

//////////////////////////////////////////////////////////////////////////
//  [10/20/2010 DHKim]
//  CLI Group Add Member (102.56)
//////////////////////////////////////////////////////////////////////////
int cmdGroupAddMember(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	EUI64       id;
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	
	invoke.AddParam("1.9", atoi(argv[0]));
	for(int i=1; i<argc; i++)
	{
		StrToEUI64(argv[i], &id);
		invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	}

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.56"))
		return CLIERR_OK;

	return CLIERR_OK;
}
//////////////////////////////////////////////////////////////////////////
//  [10/20/2010 DHKim]
//  CLI Group Delete Member (102.57)
//////////////////////////////////////////////////////////////////////////
int cmdGroupDeleteMember(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	EUI64       id;
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	StrToEUI64(argv[1], &id);

	invoke.AddParam("1.9", atoi(argv[0]));
	for(int i=1; i<argc; i++)
	{
		StrToEUI64(argv[i], &id);
		invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	}

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.57"))
		return CLIERR_OK;

	return CLIERR_OK;
}

//////////////////////////////////////////////////////////////////////////
//  [10/26/2010 DHKim]
//  CLI Group Info (102.58)
//////////////////////////////////////////////////////////////////////////
int cmdGroupInfo(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke	*pInvoke;
	//GROUPINFOENTRY	*pGroupInfoEntry=NULL;
	BOOL	bSearchID=FALSE;
	EUI64 id;
	EUI64	*bufferid=NULL;
	int i, nCount, nTotCnt=0, nCnt, nGKey=0, nMemCnt=0;
	char	pGroupName[64] = {0, }, pMemberID[16];

	if(argc > 0)
	{
		if(argc == 2)
			bSearchID = atoi(argv[1]) ? TRUE : FALSE;
		if(!bSearchID)
			invoke.AddParamFormat("1.11",VARSMI_STRING, argv[0], strlen(argv[0]));
		else
		{
			StrToEUI64(argv[0],&id);
			invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
			invoke.AddParamFormat("1.3", VARSMI_BOOL, &bSearchID, sizeof(BOOL));
		}
	}

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.58"))
		return CLIERR_OK;


	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	
	OUTTEXT(pSession, "===================================================================================================\xd\xa");
	OUTTEXT(pSession, " GroupKey	GroupName	GroupMember\xd\xa");
	OUTTEXT(pSession, "===================================================================================================\xd\xa");

	for(i=0; i<nCount; i++)
	{
		//pGroupInfoEntry = (GROUPINFOENTRY *)pInvoke->pResult[i]->stream.p;
		nCnt = i%5;
		switch(nCnt)
		{
		case 0:
			nTotCnt = pInvoke->pResult[i]->stream.s32;
			break;
		case 1:
			nGKey = pInvoke->pResult[i]->stream.s32;
			if( nTotCnt > 0)
				OUTTEXT(pSession, "%3d", nGKey);
			break;
		case 2:
			strncpy(pGroupName,pInvoke->pResult[i]->stream.p, pInvoke->pResult[i]->len);
			if( nTotCnt > 0)
				OUTTEXT(pSession, "		%-16s", pGroupName);
			break;
		case 3:
			nMemCnt = pInvoke->pResult[i]->stream.s32;
			if(nMemCnt == 0)
				OUTTEXT(pSession, "\xd\xa");
			break;
		case 4:
			if(nMemCnt > 0)
			{
				bufferid = (EUI64 *)MALLOC(sizeof(EUI64)*nMemCnt);
				memcpy(bufferid, pInvoke->pResult[i]->stream.p, sizeof(EUI64)*nMemCnt);
				for(int j=0; j<nMemCnt; j++)
				{
					EUI64ToStr(&(bufferid[j]), pMemberID);
					if(j==0)
						OUTTEXT(pSession, "%-16s\xd\xa", pMemberID);
					else
						OUTTEXT(pSession, "\t\t\t\t%-16s\xd\xa", pMemberID);
				}
			}
			else
				OUTTEXT(pSession, "\xd\xa");
			break;
		}
	}
	
	if(bufferid != NULL) FREE(bufferid);

	return CLIERR_OK;
}

//////////////////////////////////////////////////////////////////////////
//  [11/19/2010 DHKim]
//  Group DB Recovery
//////////////////////////////////////////////////////////////////////////
int cmdGroupShowCommand(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	if( 0 < argc)
		invoke.AddParam("1.9", atoi(argv[0]));

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.59"))
		return CLIERR_OK;

	return CLIERR_OK;
}

int cmdGroupTableDelete(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	invoke.AddParamFormat("1.11",VARSMI_STRING, argv[0], strlen(argv[0]));

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.60"))
		return CLIERR_OK;

	return CLIERR_OK;
}

#if 0 // Issue #835 스웨덴향 집중기는 join control을 지원하지 않는다
int cmdJointableShow(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.61"))
		return CLIERR_OK;

	return CLIERR_OK;
}

int cmdJointableAdd(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	for(int i=0; i < argc; i++)
		invoke.AddParamFormat("1.11",VARSMI_STRING, argv[i], strlen(argv[i]));

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.62"))
		return CLIERR_OK;

	return CLIERR_OK;
}

int cmdJointableDelete(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

	for(int i=0; i < argc; i++)
		invoke.AddParamFormat("1.11",VARSMI_STRING, argv[i], strlen(argv[i]));

	if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.63"))
		return CLIERR_OK;

	return CLIERR_OK;
}
#endif

int cmdSetEnergyLevel(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
#if     defined(__SUPPORT_ENDDEVICE__)
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, EXTENDED_TIMEOUT);
#else
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort, 10);
#endif
	IF4Invoke		*pInvoke;
    EUI64           id;
    BYTE            nEnergyLevel = 0;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

  	StrToEUI64(argv[0], &id);
    nEnergyLevel = (BYTE)strtol(argv[1], (char **)NULL, 10);

    if(nEnergyLevel > 0x0F)
    {
        OUTTEXT(pSession, "Invalid value\r\n");
		return CLIERR_OK;
    }

    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	invoke.AddParam("1.4", nEnergyLevel);

	pInvoke = (IF4Invoke *)invoke.GetHandle();

	if (!CLIAPI_Command(pSession, pInvoke, "104.16"))
    {
        OUTTEXT(pSession, "%s\r\n", IF4API_GetErrorMessage(pInvoke->nError));
		return CLIERR_OK;
    }

    return CLIERR_OK;
}

int cmdSetIHDTable(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{

	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	EUI64		id;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;
	
	StrToEUI64(argv[0], &id);
    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.73"))
    {
	    return CLIERR_OK;
    }

    OUTTEXT(pSession, MSG_SUCC_ADD_REGISTER "\r\n");
	return CLIERR_OK;

}

int cmdDelIHDTable(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{

	CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
	EUI64		id;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;
	
	StrToEUI64(argv[0], &id);
    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "102.74"))
    {
	    return CLIERR_OK;
    }

    OUTTEXT(pSession, MSG_SUCC_DELETE_REGISTER "\r\n");
	return CLIERR_OK;

}

int cmdDeleteGroup(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIF4Invoke	invoke("127.0.0.1", m_nLocalAgentPort);
	IF4Invoke   *pInvoke;
    EUI64       id;

	if (!Confirm(pSession, MSG_DEFAULT))
		return CLIERR_OK;

    if(argc>0)
    {
        if(check_id(NULL, argv[0])) { // EUI64 ID
  	        StrToEUI64(argv[0], &id);
            invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
        } else {
            invoke.AddParamFormat("1.11", VARSMI_STRING, argv[0], strlen(argv[0]));
            if(argc>1)
            {
                if(check_id(NULL, argv[1])) { // EUI64 ID
  	                StrToEUI64(argv[1], &id);
                    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
                }
                else
                {
                    invoke.AddParamFormat("1.11", VARSMI_STRING, argv[1], strlen(argv[1]));
                }
            }
            if(argc>2)
            {
                if(check_id(NULL, argv[2])) { // EUI64 ID
  	                StrToEUI64(argv[2], &id);
                    invoke.AddParamFormat("1.14", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
                }
            }
        }
    }

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	if (!CLIAPI_Command(pSession, pInvoke, "102.71"))
		return CLIERR_OK;

    return CLIERR_OK;
}

int cmdAddMeterSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);
    BYTE        szSecurity[64];
    int         nMode;
    char        *p;
    int         n=0, secLen=0;

    memset(szSecurity, 0, sizeof(szSecurity));
    invoke.AddParam("1.11", argv[0]);
    invoke.AddParam("1.11", argv[1]);

    nMode = (strcasecmp(argv[2], "A") == 0) ? 0 : 1;
    if (nMode == 0)
    {
        // ASCII Mode
        if (strlen(argv[3]) > sizeof(szSecurity))
        {
            OUTTEXT(pSession, "Invalid length (max=%d).\r\n", sizeof(szSecurity));
            return CLIERR_OK;
        }
        secLen = strlen(argv[3]);
        memcpy(szSecurity, argv[3], secLen);
    }
    else
    {
        // HEX Mode
        for(n=0, p=argv[3]; p && *p; p++)
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

        for(p=argv[3]; p && *p; p+=3, secLen++)
        {
            if(secLen >= (int)sizeof(szSecurity))
            {
                OUTTEXT(pSession, "Invalid length (max=%d).\r\n", sizeof(szSecurity));
                return CLIERR_OK;
            }
            sscanf(p, "%x", &n);
            szSecurity[secLen] = n;
        }
    }

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    invoke.AddParam("1.12", szSecurity, secLen);
    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "105.10"))
        return CLIERR_OK;

    return CLIERR_OK;
}

int cmdDeleteMeterSecurity(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
    CIF4Invoke  invoke("127.0.0.1", m_nLocalAgentPort);

    invoke.AddParam("1.11", argv[0]);
    invoke.AddParam("1.11", argv[1]);

    if (!Confirm(pSession, MSG_DEFAULT))
        return CLIERR_OK;

    if (!CLIAPI_Command(pSession, invoke.GetHandle(), "105.10"))
        return CLIERR_OK;

    return CLIERR_OK;
}
