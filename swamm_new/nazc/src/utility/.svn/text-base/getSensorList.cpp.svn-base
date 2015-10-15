
/** Nuritelecom AiMiR Project.
 *
 * Get Sensor List
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
#include "CommonUtil.h"
#include "if4api.h"
#include "varapi.h"
#include "mcudef.h"
#include "Utils.h"
#include "Variable.h"

#include "CommandHandler.h"

#include "mbusFunctions.h"

extern VAROBJECT	m_Root_node[];

int gCommTime = 60;

static int _cmpmetering(const void *p1, const void *p2)
{
	METERLPENTRY	*m1, *m2;
    int res;

    m1 = (METERLPENTRY *)(*(MIBValue * const *) p1)->stream.p;
    m2 = (METERLPENTRY *)(*(MIBValue * const *) p2)->stream.p;

    res = memcmp(&m1->mlpId, &m2->mlpId, sizeof(EUI64));  
    if(res) return res;
    return memcmp(&m1->mlpTime, &m2->mlpTime, sizeof(TIMESTAMP));
}

int getSensorList(char * ipaddr, int nPort)
{
    CIF4Invoke  invoke(ipaddr, nPort);
	IF4Invoke		*pInvoke;
	SENSORINFOENTRYNEW	*pSensor;
	char			szSerial[21];
	char			szModel[19];
	char			szGUID[MAX_GUID_STRING+1];
	char			szConnectTime[30], *pszState;
	char			hw[20], fw[20];
	int				i, nCount, build, scnt=0;
    int             nError;

	nError =  Call(invoke.GetHandle(), "102.26");

	pInvoke = (IF4Invoke *)invoke.GetHandle();
	nCount  = pInvoke->nResultCount;
	if (nCount <= 0)
	{
		fprintf(stdout, "No entry.\n");
		return nError;
	}

    qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmpmetering);
	fprintf(stdout, "===================================================================================================\n");
	fprintf(stdout, " NUM SENSOR ID        METER SERIAL       MODEL               H/W   F/W  BLD N LAST MTR    STATE\n");
	fprintf(stdout, "===================================================================================================\n");

	for(i=0; i<nCount; i++)
	{
		pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;
		EUI64ToStr(&pSensor->sensorID, szGUID);
		IF4API_TIMESTAMPTOSTR(szConnectTime, &pSensor->sensorLastConnect);
        memset(szSerial, 0, sizeof(szSerial));
        memset(szModel, 0, sizeof(szModel));
		memcpy(szSerial, pSensor->sensorSerialNumber, sizeof(pSensor->sensorSerialNumber));
		memcpy(szModel, pSensor->sensorModel, sizeof(pSensor->sensorModel));

        szConnectTime[strlen(szConnectTime) - 3] = 0x00;

		if (pSensor->sensorState == ENDISTATE_NORMAL)
			 pszState = const_cast<char *>("NORMAL");
		else if (pSensor->sensorState == ENDISTATE_INIT)
			 pszState = const_cast<char *>("INIT");
		else if (pSensor->sensorState == ENDISTATE_CONNECTION_ERROR)
			 pszState = const_cast<char *>("CONN_ERR");
		else if (pSensor->sensorState == ENDISTATE_METER_ERROR)
			 pszState = const_cast<char *>("METER_ERR");
		else if (pSensor->sensorState == ENDISTATE_ERROR)
			 pszState = const_cast<char *>("ERROR");
		else if (pSensor->sensorState == ENDISTATE_DATA_RECEIVE)
			 pszState = const_cast<char *>("DATA_RECV");
		else pszState = const_cast<char *>("UNKNOWN");

		sprintf(hw, "%2d.%-2d", pSensor->sensorHwVersion >> 8, pSensor->sensorHwVersion & 0xff);
		sprintf(fw, "%2d.%-2d", pSensor->sensorFwVersion >> 8, pSensor->sensorFwVersion & 0xff);
        build = pSensor->sensorFwBuild;
        if(pSensor->sensorAttr==ENDDEVICE_ATTR_MBUS_ARM)
        {
            fprintf(stdout, "     %-16s %-18s %-18s %-4s %-4s %3d %c %11s %s\n",
                            "",
                            "",
                            szModel,
                            hw,
                            fw,
                            build,
                            ' ',
                            "",
                            pszState);

        }
        else if(pSensor->sensorAttr==ENDDEVICE_ATTR_MBUS_SLAVE)
        {
            fprintf(stdout, " %20d %-18s %-18s  %-12s     %s\n",
                        build,
                        szSerial, 
                        szModel,
                        mbusGetMedium((BYTE)pSensor->sensorHwVersion),
                        szConnectTime + 5);
        }
        else
        {
            fprintf(stdout, " %3d %-16s %-18s %-18s %-4s %-4s %3d %c %s %s\n",
                        scnt+1,
                        szGUID,
                        szSerial,
                        szModel,
                        hw,
                        fw,
                        build,
                        pSensor->sensorAttr & ENDDEVICE_ATTR_RFD ? 'E' : 'R',
                        szConnectTime + 5,
                        pszState);
            scnt++;


        }
	}
	fprintf(stdout, "===================================================================================================\n");

	return nError;
}

int usage(char * name)
{
    fprintf(stderr,"%s [-cp] ipaddr\n", name);
    fprintf(stderr,"  -c sec    : Commnucation timeout\n"); 
    fprintf(stderr,"  -p port   : Listen port\n"); 
    exit(1);
}

int main(int argc, char * argv[])
{
    char *szIp;
    int  opt;
    int nPort = 8000;

    while((opt=getopt(argc, argv, "c:p:")) != -1) {
        switch(opt) {
            case 'c': gCommTime = (int) strtol(optarg,(char **)NULL,10);
                break;
            case 'p': nPort = (int) strtol(optarg,(char **)NULL,10);
                break;
            default :
                return 1;
        }
    }

    if(argc - optind < 1) {
        usage(argv[0]);
    }

    szIp = strdup(argv[optind]);

    VARAPI_Initialize(NULL, m_Root_node, FALSE);
    IF4API_Initialize(IF4_DEFAULT_PORT, NULL);

    getSensorList(szIp, nPort);

    IF4API_Destroy();
    VARAPI_Destroy();

    return 0;
}
