
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
int cmdCallTest1(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdCallTest2(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdRelayTest(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// Wooks
int cmdOpenOMR(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdMeteringOMR(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
//

IF4_COMMAND_TABLE   m_CommandHandler[] =
{
    { {102,26,0},	"cmdGetSensorMock",	cmdGetSensorMock, 	FALSE },
    { {130,1,0},	"cmdSum",			cmdSum, 			FALSE },
    { {130,2,0},	"cmdReset",			cmdReset, 			FALSE },
    { {130,3,0},	"cmdLoopback",		cmdLoopback, 		FALSE },
    { {130,4,0},	"cmdTimStamp",		cmdTimeStamp, 		FALSE },
	{ {211,211,0},	"cmdCallTest1",		cmdCallTest1,		FALSE },
	{ {211,212,0},	"cmdCallTest2",		cmdCallTest2,		FALSE },
	{ {211,213,0},	"cmdRelayTest",		cmdRelayTest,		FALSE },
	{ {225,1,0},	"cmdOpenOMR",		cmdOpenOMR,			FALSE },
	{ {225,2,0},	"cmdMeteringOMR",	cmdMeteringOMR,		FALSE },
    { {0,0,0},		NULL,				NULL, 				FALSE }
};

DEVICEPARAM serialDeviceParam =
{
	-1,
	"/dev/ttyS05",
	9600,
	8,
	1,
	0
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

void makeSensorParam(IF4Wrapper *pWrapper, SENSORINFOENTRYNEW * pEntry, const char *sid)
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

////////////////////////////////////////////////////////////////////////////////////////////////////
// Wooks

/**
 *	OMR Modem 을 개통함.
 *
 *	cmdOpenOMR(225.1) : Meter Serial 을 인자로 받아서 PDA <-> OMR Modem 간의 통신을 개통한다.
 *
 *  In Parameter : 
 *    + 1) valueA :STREAM	(1.11)
 *  Error Code :
 *    + IF4ERR_NOERROR : None Error
 *    + IF4ERR_INVALID_PARAM : valueA가 유효값 범위를 넘을 때
**/
int cmdOpenOMR(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	OMRENTRY OMRMeteringData;

	char cmdData[24];
	char meterSerial[12];
	char readBuffer[128];
	char resData[512];
	char temp[32];
	char *pPos;

	int	nCount = 0;
	int	nIndex = 0;
	int nRetry = 0;

	// 0. Error Handling
	if( cnt != 1 )	return IF4ERR_INVALID_PARAM;
	if( (VARAPI_OidCompare( &pValue[0].oid, "1.11" ) != 0) )	return IF4ERR_INVALID_PARAM;

	memset( &meterSerial, 0x00, sizeof(meterSerial) );
	memcpy( &meterSerial, pValue[0].stream.p, 12 ); //pValue[0].len );

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1. Open OMR and Read Metering Data
	memset( cmdData, 0x00, sizeof(cmdData) );
	sprintf( cmdData, "\rr %s\r\n", meterSerial );

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. Serial Read / Write
	if( write( serialDeviceParam.nFD, cmdData, 15 ) > 0 )//sizeof(cmdData) ) > 0 )
	{
		printf("\r\nSend Message : %s", cmdData );	
	}
		
	memset( resData, 0x00, sizeof(resData) );

	while(1)
	{
		memset( readBuffer, 0x00, sizeof(readBuffer) );

		nCount = read( serialDeviceParam.nFD, readBuffer, 128 );//sizeof(readBuffer) );
		
		if( nCount != 0 )
		{
			memcpy( &resData[nIndex], readBuffer, nCount );
			nIndex = nIndex + nCount;

			if( strstr(resData, "[%]") )
			{
				break;
			}

			nRetry = 0;
		}
		else
		{
			if( ++nRetry > 100 )
			{
				printf("TimeOut!!\r\n");
				break;
			}

			usleep(1000);
		}
	}
	printf("Recv Message : %s\r\n", resData);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 3. Make Response Streams
	/*
	pPos = strstr(resData, "Time");
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, (pPos + 7), sizeof(BYTE) * 16 );

	pPos = strstr(resData, "CurActiveEnergyA");
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, (pPos + 20), sizeof(BYTE) * 7 );

	pPos = strstr(resData, "CurReactiveEnergyA");
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, (pPos + 22), sizeof(BYTE) * 7 );

	pPos = strstr(resData, "PreActiveEnergyA");
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, (pPos + 20), sizeof(BYTE) * 7 );

	pPos = strstr(resData, "PreReactiveEnergyA");
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, (pPos + 22), sizeof(BYTE) * 7 );

	pPos = strstr(resData, "PreCumMaxPowerTotal");
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, (pPos + 23), sizeof(BYTE) * 7 );

	pPos = strstr(resData, "PowerFactor");
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, (pPos + 16), sizeof(BYTE) * 5 );
	*/
	memset( &OMRMeteringData, 0x00, sizeof(OMRENTRY) );
	memcpy( OMRMeteringData.pMeterSerial, meterSerial, sizeof(meterSerial) );

	pPos = strstr(resData, "Time");
	strncpy( temp, &resData[pPos - resData + 7], sizeof(BYTE) * 16 );
	OMRMeteringData.nMetringTime.year	= atoi(temp);
	OMRMeteringData.nMetringTime.mon	= atoi(temp+5);
	OMRMeteringData.nMetringTime.day	= atoi(temp+8);
	OMRMeteringData.nMetringTime.hour	= atoi(temp+11);
	OMRMeteringData.nMetringTime.min	= atoi(temp+14);
	OMRMeteringData.nMetringTime.sec	= 0;

	pPos = strstr(resData, "CurActiveEnergyA");
	strncpy( temp, &resData[pPos - resData + 20], sizeof(BYTE) * 7 );
	OMRMeteringData.nCurrActiveEnergy = (UINT)(atof(temp) * 1000);

	pPos = strstr(resData, "CurReactiveEnergyA");
	strncpy( temp, &resData[pPos - resData + 22], sizeof(BYTE) * 7 );
	OMRMeteringData.nCurrReactiveEnergy = (UINT)(atof(temp) * 1000);

	pPos = strstr(resData, "PreActiveEnergyA");
	strncpy( temp, &resData[pPos - resData + 20], sizeof(BYTE) * 7 );
	OMRMeteringData.nPrevActiveEnergy = (UINT)(atof(temp) * 1000);

	pPos = strstr(resData, "PreReactiveEnergyA");
	strncpy( temp, &resData[pPos - resData + 22], sizeof(BYTE) * 7 );
	OMRMeteringData.nPrevReactiveEnergy = (UINT)(atof(temp) * 1000);

	pPos = strstr(resData, "PreCumMaxPowerTotal");
	strncpy( temp, &resData[pPos - resData + 23], sizeof(BYTE) * 7 );
	OMRMeteringData.nPrevCumMaxTotal = (UINT)(atof(temp) * 1000);

	pPos = strstr(resData, "PowerFactor");
	strncpy( temp, &resData[pPos - resData + 16], sizeof(BYTE) * 5 );
	OMRMeteringData.nPowerFactor = (UINT)(atof(temp) * 100);

	IF4API_AddResultFormat(pWrapper, "1.101", VARSMI_OPAQUE, &OMRMeteringData, sizeof(OMRENTRY));

	return IF4ERR_NOERROR;
}

int cmdMeteringOMR(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
	char cmdData[24];
	char readBuffer[128];
	char resData[512];

	int	nCount = 0;
	int	nIndex = 0;
	int nRetry = 0;

	if( cnt != 0 )	return IF4ERR_INVALID_PARAM;

	sprintf( cmdData, "m\r\n" );

	// Test Code
	if( write( serialDeviceParam.nFD, cmdData, 3 ) > 0 )
		printf("Send Message : m\r\n");	
	
	memset( resData,	0x00, sizeof(resData) );
	
	while(1)
	{
		memset( readBuffer, 0x00, sizeof(readBuffer) );
		
		usleep(1000);

		// nCount 가 0 일 때,
		nCount = read( serialDeviceParam.nFD, readBuffer, sizeof(readBuffer) );

		if( nCount != 0 )
		{
			memcpy( &resData[nIndex], readBuffer, nCount );
			nIndex = nIndex + nCount;
		}

		if( ++nRetry > 30 || strstr(resData, "Ok") )
			break;

		usleep(1000);
	}
	printf("Recv Message : %s\r\n", resData);

	// Response
	IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, &resData, sizeof(resData));

	return IF4ERR_NOERROR;
}


////////////////////////////////////////////////////////////////////////////////////////////////////



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

		VARAPI_OidToString(&pValue[i].oid, szOid);
	    IF4API_AddResultOpaque(pWrapper, szOid, pValue[i].stream.p, pValue[i].len);
	}
    return IF4ERR_NOERROR;
}

int cmdCallTest1(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    TIMESTAMP   stamp;
    time_t      invokeTime;
	CODIENTRY	codi;
    EUI64       id;
	char	    szOid[20];
    int         i;

    XDEBUG("param cnt=%d\n", cnt);

    for(i=0;i<cnt;i++) {
	    XDEBUG(" param[%d].length=%d\n",  i, pValue[i].len);
    }

    time(&invokeTime); // 압축시험을 위해 일단 막는다
    GETTIMESTAMP(&stamp, (time_t *)&invokeTime);
    //memset(&stamp, 0, sizeof(TIMESTAMP));

	memset(&codi, 0, sizeof(CODIENTRY));
  	StrToEUI64("001122AA0D00FF20", &id);
	memcpy(&codi.codiID, &id, sizeof(EUI64));
	codi.codiShortID		= 0xAABB;
    /*
	codi.codiFwVer			= pCoordinator->fw;
	codi.codiHwVer			= pCoordinator->hw;
	codi.codiZAIfVer		= pCoordinator->zaif;
	codi.codiZZIfVer		= pCoordinator->zzif;
	codi.codiFwBuild		= pCoordinator->build;
	codi.codiResetKind		= pCoordinator->rk;
	codi.codiAutoSetting	= pCoordinator->autoset;
	codi.codiChannel		= pCoordinator->channel;
	codi.codiPanID			= pCoordinator->panid;
	codi.codiRfPower		= pCoordinator->power;
	codi.codiTxPowerMode	= pCoordinator->txpowermode;
	codi.codiPermit			= pCoordinator->permit_time;
	codi.codiEnableEncrypt= pCoordinator->security;
	codi.codiRouteDiscovery = pCoordinator->discovery;
	codi.codiMulticastHops  = pCoordinator->mhops;
    */

	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, 10);
	IF4API_AddResultFormat(pWrapper, "1.16", VARSMI_TIMESTAMP, &stamp, sizeof(TIMESTAMP));

    /** compress 부분이 해결되면 추가 한다 */
	IF4API_AddResultFormat(pWrapper, "3.3", VARSMI_OPAQUE, &codi, sizeof(CODIENTRY));

    for(i=0; i < cnt; i++) {
		VARAPI_OidToString(&pValue[i].oid, szOid);
	    switch(pValue[i].type) {
	        case VARSMI_CHAR :
	        case VARSMI_BYTE :
	        case VARSMI_SHORT :
	        case VARSMI_WORD :
	        case VARSMI_INT :
	        case VARSMI_UINT :
	            IF4API_AddResultNumber(pWrapper, szOid, pValue[i].type, pValue[i].stream.u32);
		        break;
            default:
	            IF4API_AddResultFormat(pWrapper, szOid, pValue[i].type, pValue[i].stream.p, pValue[i].len);
                break;
	    }
    }

	return IF4ERR_NOERROR;
}

int cmdCallTest2(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    TIMESTAMP   stamp;
    time_t      invokeTime;

    time(&invokeTime); 
    GETTIMESTAMP(&stamp, (time_t *)&invokeTime);

	IF4API_AddResultFormat(pWrapper, "1.16", VARSMI_TIMESTAMP, &stamp, sizeof(TIMESTAMP));

	return IF4ERR_NOERROR;
}

int cmdRelayTest(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    TIMESTAMP   stamp;
    time_t      invokeTime;

    time(&invokeTime); // 압축시험을 위해 일단 막는다
    GETTIMESTAMP(&stamp, (time_t *)&invokeTime);

	IF4API_AddResultFormat(pWrapper, "1.16", VARSMI_TIMESTAMP, &stamp, sizeof(TIMESTAMP));

	return IF4ERR_NOERROR;
}

BOOL Serial_Initialize_Interface(DEVICEPARAM *dParam)
{
	struct	termios	tio;

	//char buf[128] = { 0, };

	dParam->nFD = open("/dev/ttyS5", O_RDWR | O_NOCTTY );
	if (dParam->nFD < 0)
	{
		printf("[Serial Interface] Initialization Failed !! \r\n");
		return FALSE;
	}

	// Change Setting
	memset(&tio, 0, sizeof(tio));

	tio.c_cflag	= B9600 | CS8 | CLOCAL | CREAD;//HUPCL;

	/*
	cfsetispeed(&tio, dParam->nSpeed);
	cfsetospeed(&tio, dParam->nSpeed);
	*/

	switch(dParam->nParity)
	{
	case 0 :	// None Parity
		tio.c_iflag = IGNPAR | ICRNL;
		break;
	case 1 :	// Odd Parity
		tio.c_cflag |= PARENB;
		tio.c_cflag |= PARODD;
		break;
	case 2 :	// Even Parity
		tio.c_cflag |= PARENB;
		break;
	}

	tio.c_oflag		 = 0;
	tio.c_lflag		 = 0;

	tio.c_cc[VINTR]    = 0;     /* Ctrl-c */
	tio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	tio.c_cc[VERASE]   = 0;     /* del */
	tio.c_cc[VKILL]    = 0;     /* @ */
	tio.c_cc[VEOF]     = 4;     /* Ctrl-d */
	tio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	tio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	tio.c_cc[VSWTC]    = 0;     /* '\0' */
	tio.c_cc[VSTART]   = 0;     /* Ctrl-q */
	tio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	tio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	tio.c_cc[VEOL]     = 0;     /* '\0' */
	tio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	tio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	tio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	tio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	tio.c_cc[VEOL2]    = 0;     /* '\0' */
	tio.c_cc[IGNBRK]   = 1;
	tio.c_cc[BRKINT]   = 0;

	tcflush(dParam->nFD, TCIFLUSH);
	tcsetattr(dParam->nFD, TCSANOW, &tio);

	return TRUE;
}

int Serial_Terminate_Interface()
{
	close( serialDeviceParam.nFD );

	return 0;
}

void Usage(char * filename)
{
	printf("Usage: %s [-p]\n", filename);
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
	//
	if( Serial_Initialize_Interface(&serialDeviceParam) )
	{
		printf("Serial Interface Initialized !! \r\n");

		IF4Server_Initialize_Interface(port);

		while(1) sleep(100000);

		IF4Server_Terminate_Interface();
	}	

	Serial_Terminate_Interface();

	return 0;
}
