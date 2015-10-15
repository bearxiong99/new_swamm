
/** Nuritelecom AiMiR Project.
 *
 * Ondemand GE
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>

#include "aimir.h"
#include "typedef.h"
#include "version.h"
#include "config.h"
#include "if4api.h"
#include "varapi.h"
#include "mcudef.h"
#include "CommonUtil.h"
#include "Utils.h"
#include "CliUtils.h"
#include "cmd_message.h"
#include "Variable.h"

#include "CommandHandler.h"

extern VAROBJECT	m_Root_node[];

int _showOndemand(SENSORINFOENTRYNEW *pSensor, IF4Invoke *pInvoke, int nStartIdx, int nEndIdx)
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

    fprintf(stdout, "=====================================================================================\xd\xa");
    fprintf(stdout, "     SENSOR ID : %s\xd\xa", szGUID);
    if(pItem->mlpVendor)
    {
	    fprintf(stdout, "        VENDOR : %s\xd\xa", getVendorString(pItem->mlpVendor));
    }
    fprintf(stdout, "         MODEL : %s\xd\xa", pSensor->sensorModel);
    fprintf(stdout, "     METER S/N : %s\xd\xa", pSensor->sensorSerialNumber);
    fprintf(stdout, "         STATE : %s\xd\xa", MSGAPI_GetSensorState(pSensor->sensorState));
    fprintf(stdout, "  NETWORK TYPE : %s\xd\xa", pSensor->sensorAttr & ENDDEVICE_ATTR_RFD ? "RFD" : "FFD");
    fprintf(stdout, "  INSTALL DATE : %s\xd\xa", szInstallTime);
    fprintf(stdout, " LAST METERING : %s\xd\xa", szConnectTime);

    fprintf(stdout, " METERING DATA : %d (%d bytes)\xd\xa", pItem->mlpDataCount, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP));
    fprintf(stdout, " METERING TIME : %04d/%02d/%02d %02d:%02d:%02d\xd\xa", 
        pItem->mlpTime.year,
        pItem->mlpTime.mon,
        pItem->mlpTime.day,
        pItem->mlpTime.hour,
        pItem->mlpTime.min,
        pItem->mlpTime.sec
    );

    switch(parserType) {
        case PARSER_TYPE_AIDON:
            aidon_data_parsing(pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_ANSI:
            ge_data_parsing(pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_DLMS:
            dlms_data_parsing(pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_PULSE:
        case PARSER_TYPE_ACD:
        case PARSER_TYPE_SMOKE:
        case PARSER_TYPE_HMU:
            pulse_data_parsing(pSensor->sensorModel, pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_KAMST:
            kamstrup_data_parsing(pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_REPEATER:
            repeater_data_parsing(pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
        case PARSER_TYPE_MBUS:
            mbus_data_parsing(pItem->mlpData, LittleToHostShort(pItem->mlpLength) - sizeof(TIMESTAMP), bDetail);
            break;
    }
    fprintf(stdout, "=====================================================================================\xd\xa");

    return IF4ERR_NOERROR;
}


int ondemand(char * ipaddr, char * idstr, int nOption, int nOffset, int nCount, int nTimeout)
{
	CIF4Invoke		invoke(ipaddr, 8000, 600);
	CIF4Invoke		invoke2(ipaddr, 8000, nTimeout);
	IF4Invoke		*pInvoke;
	SENSORINFOENTRYNEW	*pSensor;
	int				nError;
    EUI64           id;

  	StrToEUI64(idstr, &id);
    invoke.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	
    if((nError=Call(invoke.GetHandle(), "102.27"))!=IF4ERR_NOERROR) return nError;

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[0]->stream.p;

    invoke2.AddParamFormat("4.3.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
    invoke2.AddParam("1.9", nOption);
    invoke2.AddParam("1.9", nOffset);
    invoke2.AddParam("1.9", nCount);

    if((nError=Call(invoke2.GetHandle(), "104.6"))!=IF4ERR_NOERROR) return nError;

    return _showOndemand(pSensor, invoke2.GetHandle(), 0, invoke2.GetHandle()->nResultCount);
}

int usage(char * name)
{
    fprintf(stderr,"%s [-t timeout] ipaddr eui64id [option [offset [count]]]\n", name);
    fprintf(stderr,"  -t timeout : timeout (default 6 sec)\n"); 
    exit(1);
}

int main(int argc, char * argv[])
{
    char *ipaddr = NULL;
    char *id = NULL;
    int opt;
	int	nOption = -1;
	int	nOffset = -1;
	int	nCount = 0;
    int nTimeout = 600;
    int nError;

    while((opt=getopt(argc, argv, "t:")) != -1) {
        switch(opt) {
            case 't': nTimeout =  (int) strtol(optarg, (char **)NULL, 10) * 100;
                break;
            default :
                usage(argv[0]);
        }
    }

    if(argc - optind < 2) {
        usage(argv[0]);
    }

    if(nTimeout < 100) nTimeout = 600;

    ipaddr = strdup(argv[optind]);
    id = strdup(argv[optind+1]);

    if(argc - optind > 2) nOption = (int)strtol(argv[optind+2],(char **)NULL,10);
    if(argc - optind > 3) nOffset = (int)strtol(argv[optind+3],(char **)NULL,10);
    if(argc - optind > 4) nCount = (int)strtol(argv[optind+4],(char **)NULL,10);

    VARAPI_Initialize(NULL, m_Root_node, FALSE);
    IF4API_Initialize(IF4_DEFAULT_PORT, NULL);

    fprintf(stdout,"ondemand : target=%s, id=%s, opt=%d, offset=%d, cnt=%d, timeout=%d\n", 
            ipaddr, id, nOption, nOffset, nCount, nTimeout / 100); 

    if((nError=ondemand(ipaddr, id, nOption, nOffset, nCount, nTimeout)) != IF4ERR_NOERROR) {
        fprintf(stderr, "%s\n", IF4API_GetErrorMessage(nError));
    }

    IF4API_Destroy();
    VARAPI_Destroy();

    return 0;
}
