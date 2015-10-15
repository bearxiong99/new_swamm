
/** Nuritelecom AiMiR Project.
 *
 *  Nuri Aimir Concentrator External Interface Library.
 *
 */

#include "common.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "Variable.h"

int cmdSum(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdReset(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdLoopback(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdTimeStamp(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdGetSensorMock(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

IF4_COMMAND_TABLE   m_CommandHandler[] =
{
    { {102,26,0}, "cmdGetSensorMock",  cmdGetSensorMock, 		FALSE },
    { {130,1,0},  "cmdSum",      cmdSum, 		FALSE },
    { {130,2,0},  "cmdReset",    cmdReset, 		FALSE },
    { {130,3,0},  "cmdLoopback", cmdLoopback, 	FALSE },
    { {130,4,0},  "cmdTimStamp", cmdTimeStamp, 	FALSE },
    { {0,0,0},    NULL,          NULL, 			FALSE }
};

/** IF4API 초기화.
 * IF4API를 이용하기 위해서는 반드시 초기화 절차를 거쳐야 한다.
 *
 * @author rewriter@nuritelecom.com
 * 
 * @return Error Error Code
 *
 * @retval 0 정상 종료
 * @retval -1 Internal Error
 */
int IF4Server_Initialize_Interface(int port)
{
    if(VARAPI_Initialize(NULL, m_Root_node, FALSE)) return -1;
    if(IF4API_Initialize(port, m_CommandHandler)) return -1;

    return 0;
}

/** IF4API 종료.
 * 종료 시 반드 시 Terminate 절차를 거친다 
 *
 * @author rewriter@nuritelecom.com
 * 
 * @return Error Error Code
 *
 * @retval 0 정상 종료
 */
int IF4Server_Terminate_Interface(void)
{
    IF4API_Destroy();
    VARAPI_Destroy();

    return 0;
}

void makeSensorParam(IF4Wrapper *pWrapper, SENSORINFOENTRYNEW * pEntry, char *sid)
{
    EUI64 eid;

    StrToEUI64(sid, &eid);
    memset(pEntry, 0, sizeof(SENSORINFOENTRYNEW));
    memcpy(&pEntry->sensorID, &eid, sizeof(EUI64));
	IF4API_AddResultFormat(pWrapper, "4.3", VARSMI_OPAQUE, pEntry, sizeof(SENSORINFOENTRYNEW));
}
/** cmdGetSensorAllNew MOCK
  *
  */
int cmdGetSensorMock(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    SENSORINFOENTRYNEW entry;

    makeSensorParam(pWrapper, &entry, "0000000000000001");
    makeSensorParam(pWrapper, &entry, "0000000000000002");
    makeSensorParam(pWrapper, &entry, "0000000000000003");
    makeSensorParam(pWrapper, &entry, "0000000000000004");
    makeSensorParam(pWrapper, &entry, "0000000000000005");
    makeSensorParam(pWrapper, &entry, "0000000000000006");
    makeSensorParam(pWrapper, &entry, "0000000000000007");
    makeSensorParam(pWrapper, &entry, "0000000000000008");

    return IF4ERR_NOERROR;
}


/** IF4API Command Call
 * 
 * IF4API에서 제공하는 Command Call 예제.
 *
 * 임의의 Command cmdSum(130.1)을 정의한다.
 *
 * cmdSum(130.1) : 임의의 양의 정수 (값의 범위는 0 ~ 10) 두개를 받아
 *                 두수를 더한값을 리턴한다.
 *  In Parameter : 
 *    + 1) valueA :BYTE (1.4)
 *    + 2) valueB :BYTE (1.4)
 *  Out Parameter :
 *    + 1) sumAB : UINT (1.6)
 *  Error Code :
 *    + IF4ERR_NOERROR : None Error
 *    + IF4ERR_INVALID_PARAM : valueA가 유효값 범위를 넘을 때
 *    + IF4ERR_INVALID_PARAM : valueB가 유효값 범위를 넘을 때
 * 
 * @author rewriter@nuritelecom.com
 *
 */
int cmdSum(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    BYTE valueA, valueB;
    UINT retV;

    if (cnt != 6)
        return IF4ERR_INVALID_PARAM;

    if ((VARAPI_OidCompare(&pValue[0].oid, "1.4") != 0) ||
        (VARAPI_OidCompare(&pValue[1].oid, "1.6") != 0)) 
        return IF4ERR_INVALID_ID;

    valueA = pValue[0].stream.u8;
    valueB = pValue[1].stream.u8;

    if(valueA > 10 || valueB > 10) return IF4ERR_INVALID_VALUE;

    retV = valueA + valueB;

    IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, retV);
    IF4API_AddResultString(pWrapper, "1.11", pValue[2].stream.p);
    IF4API_AddResultString(pWrapper, "1.11", pValue[3].stream.p);
    IF4API_AddResultString(pWrapper, "1.11", pValue[4].stream.p);
    IF4API_AddResultString(pWrapper, "1.11", pValue[5].stream.p);
    return IF4ERR_NOERROR;
}

int cmdReset(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    if (cnt != 1)
        return IF4ERR_INVALID_PARAM;

    if (VARAPI_OidCompare(&pValue[0].oid, "1.2") != 0)
        return IF4ERR_INVALID_ID;

	// reset call

    return IF4ERR_NOERROR;
}

int cmdLoopback(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char	szOid[20];
	int		i;

    if (cnt <= 0)
        return IF4ERR_INVALID_PARAM;

	for(i=0; i<cnt; i++)
	{
		if (VARAPI_OidCompare(&pValue[i].oid, "1.11") != 0)
  		    return IF4ERR_INVALID_TYPE;
	}

	for(i=0; i<cnt; i++)
	{
		VARAPI_OidToString(&pValue[0].oid, szOid);
	    IF4API_AddResultOpaque(pWrapper, szOid, pValue[i].stream.p, pValue[i].len);
	}
    return IF4ERR_NOERROR;
}

int cmdTimeStamp(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    TIMESTAMP   t;
	char	szOid[20];
	int		i;

    if (cnt <= 0)
        return IF4ERR_INVALID_PARAM;

	for(i=0; i<cnt; i++)
	{
		if (VARAPI_OidCompare(&pValue[i].oid, "1.16") != 0)
  		    return IF4ERR_INVALID_TYPE;
	}

	for(i=0; i<cnt; i++)
	{
        memcpy(&t, pValue[i].stream.p, sizeof(TIMESTAMP));
        XDEBUG("TIMESTAMP(S): %04d/%02d/%02d %02d:%02d:%02d\n", 
                t.year, t.mon, t.day, t.hour, t.min, t.sec);

		VARAPI_OidToString(&pValue[0].oid, szOid);
	    IF4API_AddResultOpaque(pWrapper, szOid, pValue[i].stream.p, pValue[i].len);
	}
    return IF4ERR_NOERROR;
}


void Usage(char * filename)
{
	printf("Usage: %s [-ps]\n", filename);
	printf("         -p      Server Port (Default 8000)\n");
	printf("\n");
}

int main(int argc, char ** argv)
{
    int opt;
    int port=8000;

    while((opt=getopt(argc, argv, "p:")) != -1) {
        switch(opt) {
            case 'p': 
                port = (int)strtol(optarg,(char **)NULL,10);
                break;
            case 'h':
            default :
                Usage(argv[0]);
                return FALSE;
        }
    }

    SET_DEBUG_MODE(0);
    IF4Server_Initialize_Interface(port);

    while(1) USLEEP(100000);

    IF4Server_Terminate_Interface();

	return 0;
}
