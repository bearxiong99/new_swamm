
/** Nuritelecom AiMiR Project.
 *
 * MCU Check main function
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
#include "logdef.h"

#include "CommandHandler.h"

extern VAROBJECT	m_Root_node[];

int gCommTime = 12;

int _checkSystem(char * ipaddr)
{
    CIF4Invoke		invoke(ipaddr, 8000, 60);
    int			nError, nRetry, i;
    int			nState;
    TIMESTAMP		*pTime;

    nRetry = 3;
    for (i = 0; i < nRetry; i++)
    {
    	invoke.AddParam("2.1");
	nError =  Call(invoke.GetHandle(), "199.4");
    	
    	if(nError == IF4ERR_NOERROR)
		break;
    }

    if (nError != IF4ERR_NOERROR)
	return -nError;

    printf("revision=[%s]", invoke.ResultAtName("sysSwRevision")->stream.p);
    
    nState = invoke.ResultAtName("sysState")->stream.u8;
    printf(" state=[%s(%0d)]", nState == 1 ? "normal" : "abnormal", nState);

    pTime = (TIMESTAMP *)invoke.ResultAtName("sysTime")->stream.p;
    printf(" time=[%04d/%02d/%02d %02d:%02d:%02d]\n",
	pTime->year, pTime->mon, pTime->day,
	pTime->hour, pTime->min, pTime->sec);

    return nError;
}

#if 0
static int _cmpcommtime(const void *p1, const void *p2)
{
    SENSORINFOENTRYNEW  *s1, *s2;

    s1 = (SENSORINFOENTRYNEW *)(*(MIBValue * const *) p1)->stream.p;
    s2 = (SENSORINFOENTRYNEW *)(*(MIBValue * const *) p2)->stream.p;

    if(s1->sensorLastConnect.year - s2->sensorLastConnect.year) return s1->sensorLastConnect.year - s2->sensorLastConnect.year;
    if(s1->sensorLastConnect.mon - s2->sensorLastConnect.mon) return s1->sensorLastConnect.mon - s2->sensorLastConnect.mon;
    if(s1->sensorLastConnect.day - s2->sensorLastConnect.day) return s1->sensorLastConnect.day - s2->sensorLastConnect.day;
    if(s1->sensorLastConnect.hour - s2->sensorLastConnect.hour) return s1->sensorLastConnect.hour - s2->sensorLastConnect.hour;
    if(s1->sensorLastConnect.min - s2->sensorLastConnect.min) return s1->sensorLastConnect.min - s2->sensorLastConnect.min;
    if(s1->sensorLastConnect.sec - s2->sensorLastConnect.sec) return s1->sensorLastConnect.sec - s2->sensorLastConnect.sec;
    return 0;
}
#endif

static int _cmpsensor(const void *p1, const void *p2)
{
    SENSORINFOENTRYNEW	*s1, *s2;
    int	res;

    s1 = (SENSORINFOENTRYNEW *)(*(MIBValue * const *) p1)->stream.p;
    s2 = (SENSORINFOENTRYNEW *)(*(MIBValue * const *) p2)->stream.p;

    res = s1->sensorAttr - s1->sensorAttr;
    if (res) return res;
    return memcmp(&s1->sensorID, &s2->sensorID, sizeof(EUI64));
}

static int _cmpmetering(const void *p1, const void *p2)
{
    METERLPENTRY	*s1, *s2;
    int res;

    s1 = (METERLPENTRY *)(*(MIBValue * const *) p1)->stream.p;
    s2 = (METERLPENTRY *)(*(MIBValue * const *) p2)->stream.p;

    res = memcmp(&s1->mlpId, &s2->mlpId, sizeof(EUI64));
    if (res) return res;
    return memcmp(&s1->mlpTime, &s2->mlpTime, sizeof(TIMESTAMP));
}

int _checkSensor(char * ipaddr)
{
    CIF4Invoke		invoke(ipaddr, 8000, 60);
    CIF4Invoke		invoke2(ipaddr, 8000, 60);
    IF4Invoke		*pInvoke=NULL, *pInvoke2;
    TIMESTAMP		tmStart, tmEnd;
    int			nError, nCount, nCount2, nRetry, res;
    int             	i, j=0, k;
    int			nMeteringCount;
    //struct tm       	tmm;
    time_t          	now/*, meteringTime*/;
    SENSORINFOENTRYNEW  *pSensor;
    METERLPENTRY	*pItem;
    char            	szGUID[MAX_GUID_STRING+1];
    char            	szConnectTime[30], *pszState;
    int		    	nNormalCount, nInitCount, nConnCount, nErrorCount, nMeterCount, nDataCount, nUnknownCount, nTotalError;
    int             	nEnergyCount = 0, nGasCount = 0, nRepeaterCount = 0;
    int		    	nErrEnergyCount = 0, nErrGasCount = 0;
    BYTE		hourMask[24];
    BYTE		deviceAttr = 0xFF;

    nNormalCount = nInitCount = nConnCount = nErrorCount = nMeterCount = nDataCount = nUnknownCount = nTotalError = 0;    

    time(&now);
    GetTimestamp(&tmStart, &now);
    GetTimestamp(&tmEnd, &now);

    // 전날 검침 정보를 조회 한다.
    tmStart.day -= 1;
    tmEnd.day -= 1;

    tmStart.hour = 0;
    tmStart.min = 0;
    tmStart.sec = 0;
    tmEnd.hour = 23;
    tmEnd.min = 59;
    tmEnd.sec = 59;

    nRetry = 3;
    for (i = 0; i < nRetry; i++)
    {
	nError =  Call(invoke.GetHandle(), "102.26");
	if (nError == IF4ERR_NOERROR)
		break;
    }

    if(nError) {
        if(nError != IF4ERR_NO_ENTRY)  {
            printf("sensorCnt error=[%d %s]\n", nError, IF4API_GetErrorMessage(nError));
		    return -nError;
        }
    }

    if(nError == IF4ERR_NO_ENTRY) nCount = 0;
    else {
	    pInvoke = (IF4Invoke *)invoke.GetHandle();
	    nCount  = pInvoke->nResultCount;
    }

    printf("sensorCnt=[%d]", nCount);

    if(nCount > 0) {
        qsort(pInvoke->pResult, nCount, sizeof(MIBValue *), _cmpsensor);
    }

    for(i=0;i<nCount;i++) {
	pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;

	if(strcmp(pSensor->sensorModel, "SM110") == 0 || strcmp(pSensor->sensorModel, "GE-SM-XXX") == 0)
		nEnergyCount++;
	else if(strcmp(pSensor->sensorModel, "BATTERY-PULSE") == 0)
		nGasCount++;
	else if(strcmp(pSensor->sensorModel, "REPEATER") == 0)
		nRepeaterCount++;
    }

    printf(" EneryCnt=[%d] GasCnt=[%d] RepeaterCnt=[%d]\n", nEnergyCount, nGasCount, nRepeaterCount);

    invoke2.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmStart, sizeof(TIMESTAMP));
    invoke2.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmEnd, sizeof(TIMESTAMP));
    invoke2.AddParam("1.3", (BOOL) TRUE);

    for (i = 0; i < nRetry; i++)
    {
	nError = Call(invoke2.GetHandle(), "104.5");
	if (nError == IF4ERR_NOERROR)
		break;
    }

    if (nError)
    {
	printf("meteringInfo error=[%d %s]\n", nError, IF4API_GetErrorMessage(nError));
		return -nError;
    }	

    pInvoke2 = (IF4Invoke *)invoke2.GetHandle();
    nCount2 = pInvoke2->nResultCount;

    qsort(pInvoke2->pResult, nCount2, sizeof(MIBValue *), _cmpmetering);

    for(i=0;i<nCount;i++) {
        pSensor = (SENSORINFOENTRYNEW *)pInvoke->pResult[i]->stream.p;

        //memset(&tmm,0,sizeof(struct tm));
        //tmm.tm_year = pSensor->sensorLastConnect.year - 1900;
        //tmm.tm_mon = pSensor->sensorLastConnect.mon - 1;
        //tmm.tm_mday = pSensor->sensorLastConnect.day;
        //tmm.tm_hour = pSensor->sensorLastConnect.hour;
        //tmm.tm_min = pSensor->sensorLastConnect.min;
        //tmm.tm_sec = pSensor->sensorLastConnect.sec;

        //meteringTime = mktime(&tmm);

        //if(abs(now-meteringTime) < gCommTime * 60 * 60) continue;
	
	if (deviceAttr != pSensor->sensorAttr)
	{
		j = 0;
		deviceAttr = pSensor->sensorAttr;
	}
	memset(hourMask, 0, sizeof(hourMask));

	for ( ; j < nCount2; j++)
	{
		pItem = (METERLPENTRY *)pInvoke2->pResult[j]->stream.p;
		res = memcmp(&pSensor->sensorID, &pItem->mlpId, sizeof(EUI64));
		if (!res)
			hourMask[pItem->mlpTime.hour] |= (0x01 << (int)(pItem->mlpTime.min / 15));
		else if(res < 0) break;
	}

	nMeteringCount = 0;
	for (k = 0; k < 24; k++)
		if(hourMask[k]) nMeteringCount++;
		
	if (nMeteringCount != 0 || strcmp(pSensor->sensorModel, "REPEATER") == 0) continue;

        EUI64ToStr(&pSensor->sensorID, szGUID);
        IF4API_TIMESTAMPTOSTR(szConnectTime, &pSensor->sensorLastConnect);

        if (pSensor->sensorState == ENDISTATE_NORMAL)
	{
             pszState = const_cast<char *>("NORMAL");
	     nNormalCount++;
	}	
        else if (pSensor->sensorState == ENDISTATE_INIT)
	{
             pszState = const_cast<char *>("INIT");
	     nInitCount++;
	}
        else if (pSensor->sensorState == ENDISTATE_CONNECTION_ERROR)
	{
             pszState = const_cast<char *>("CONN_ERR");
	     nConnCount++;
	}
        else if (pSensor->sensorState == ENDISTATE_METER_ERROR)
	{
             pszState = const_cast<char *>("METER_ERR");
	     nMeterCount++;
	}
        else if (pSensor->sensorState == ENDISTATE_ERROR)
	{
             pszState = const_cast<char *>("ERROR");
	     nErrorCount++;
	}
        else if (pSensor->sensorState == ENDISTATE_DATA_RECEIVE)
	{
             pszState = const_cast<char *>("DATA_RECV");
	     nDataCount++;
	}
        else 
	{
	     pszState = const_cast<char *>("UNKNOWN");
	     nUnknownCount++;
	}
	nTotalError++;

	if(strcmp(pSensor->sensorModel, "SM110") == 0 || strcmp(pSensor->sensorModel, "GE-SM-XXX") == 0)
		nErrEnergyCount++;
	else if(strcmp(pSensor->sensorModel, "BATTERY-PULSE") == 0)
		nErrGasCount++;

        printf("    + %d %s %s %s %s\n", nTotalError, szGUID, pSensor->sensorModel, szConnectTime, pszState);
    }
    if (nNormalCount > 0) 
    	printf("    NORMAL       : %d\n", nNormalCount);
    if (nInitCount > 0) 
    	printf("    INIT         : %d\n", nInitCount);
    if (nConnCount > 0)
    	printf("    CONN_ERR     : %d\n", nConnCount);
    if (nMeterCount > 0) 
    	printf("    METER_ERR    : %d\n", nMeterCount);
    if (nErrorCount > 0) 
    	printf("    ERROR        : %d\n", nErrorCount);
    if (nDataCount > 0) 
    	printf("    DATA_RECV    : %d\n", nDataCount);
    if (nUnknownCount > 0) 
    	printf("    UNKNOWN      : %d\n", nUnknownCount);
    if (nTotalError > 0) 
    	printf("    Total Error  : %d (Energy : %d, Gas : %d, Repeater : 0)\n", nTotalError, nErrEnergyCount, nErrGasCount);
    printf("    Total Normal : %d (Energy : %d, Gas : %d, Repeater : %d)\n", 
		nCount-nTotalError, nEnergyCount-nErrEnergyCount, nGasCount-nErrGasCount, nRepeaterCount);
    return nCount;
}

int _checkEvent(char* ipaddr)
{
    CIF4Invoke	invoke(ipaddr, 8000, 60);
    char	szPath[64];
    time_t	now;
    struct tm	when;
    int		i;
    int		nError, nRetry = 3, nLength;
    char	*pszBuffer, *p;


    time(&now);
    when = *localtime(&now);
    sprintf(szPath, "%s/%s%04d%02d%02d.log", LOG_DIR, EVENT_LOG_FILE,
	when.tm_year+1900, when.tm_mon+1, when.tm_mday-1);

    invoke.AddParam("1.11", szPath);
    for (i = 0; i < nRetry; i++)
    {
	nError = Call(invoke.GetHandle(), "198.1");
	if (nError == IF4ERR_NOERROR) break;
    }

    if(nError != IF4ERR_NOERROR)  {
	printf("showEvent error=[%d %s]\n", nError, IF4API_GetErrorMessage(nError));
		return -nError;
    }

    printf("\n");
    printf("Show event log\n");

    nLength = invoke.GetHandle()->pResult[0]->stream.s32;
    if (nLength > 0)
    {
	pszBuffer = (char *)malloc(nLength+1);
	if (pszBuffer != NULL)
	{
		memcpy(pszBuffer, invoke.GetHandle()->pResult[3]->stream.p, nLength);
		pszBuffer[nLength] = '\0';

		p = strtok(pszBuffer, "\r\n");
		while (p)
		{
			if (*p)	printf("%s\n", p);
			p = strtok(NULL, "\r\n");
		}

		free(pszBuffer);
	}
    }

    return nLength;
}

int checkMCU(int mcuid, char * ipaddr)
{
    int ret;
    char buffer[256];
    printf("ID=[%8d] ip=[%15s] ", mcuid, ipaddr);

    memset(buffer,0,sizeof(buffer));
    ret = _checkSystem(ipaddr);
    if(ret >= 0) {
        ret = _checkSensor(ipaddr);
	ret = _checkEvent(ipaddr);
    }
    else
        printf("error=[%d %s]\n", -ret, IF4API_GetErrorMessage(-ret));

    return 0;
}

int main(int argc, char * argv[])
{
    char buffer[256];
    char szIp[64];
    int  nId;
    int  opt;

    while((opt=getopt(argc, argv, "c:")) != -1) {
        switch(opt) {
            case 'c': gCommTime = (int) strtol(optarg,(char **)NULL,10);
                break;
            default :
                return 1;
        }
    }

    VARAPI_Initialize(NULL, m_Root_node, FALSE);
    IF4API_Initialize(IF4_DEFAULT_PORT, NULL);

    memset(buffer,0,sizeof(buffer));
    while(fgets(buffer,sizeof(buffer),stdin)!=NULL) {
        sscanf(buffer,"%d %s", &nId, szIp);
        checkMCU(nId, szIp);
        memset(buffer,0,sizeof(buffer));
    }

    IF4API_Destroy();
    VARAPI_Destroy();

    return 0;
}
