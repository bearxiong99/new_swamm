
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ether.h>

#include "common.h"
#include "qc.h"
#include "kbhit.h"

extern char	m_szPrompt[100];
extern char m_szMcuModel[128];
extern BYTE macAddress[6];
extern void displayPrompt();
extern int enableVerbose;

Message * errorMessage = NULL;
Message * _tail = NULL;

TPROCTABLE	m_ProcTable[] =
{
	{ "RTC    TST" , _rtcTest, NULL },
	{ "GPIO   TST" , _gpioTest, NULL },
	{ "GSM    TST" , _mobileTest, _mobileCondition },
	{ "Cordi  TST" , _coordiTest, NULL },
	{ "Heater TST" , _heaterTest, _heaterCondition },
	{ "Power  TST" , _powerTest, NULL },
	{ NULL , NULL }
};

int m_LedTable[][2] = 
{
    { GP_STA_RED_OUT, GP_STA_GREEN_OUT },
    { 49, 48 }, // GPS_TXD, GPS_RXD
    { 34, 39 }, // GSM_TXD, GSM_RXD
    { 42, 43 }, // SINK_RXD_M1, SINK_TXD_M0
    { -1, 32 }, // NULL, GSM_SYNC
    { 0 , 0 }
};

Message * _addErrorMessage(char * msg)
{
	if(_tail) {
		_tail->next = (Message *) malloc(sizeof(Message));
		_tail = _tail->next;
	}else {
		errorMessage = (Message *) malloc(sizeof(Message));
		_tail = errorMessage;
	}
	memset(_tail,0,sizeof(Message));
	_tail->msg = (char *)malloc(strlen(msg) + 1);
	strcpy(_tail->msg,msg);

    if(enableVerbose) printf(errorPrefix "%s\n",msg);
        
	return _tail;
}

void _freeErrorMessage(void)
{
	Message * _now = errorMessage;
	Message * _next;

	errorMessage = _tail = NULL;
	while(_now) {
		free(_now->msg);
		_next = _now->next;
		free(_now);
		_now = _next;
	}
}

void _printErrorMessage(void)
{
	Message * _now = errorMessage;

	while(_now) {
		printf(errorPrefix "%s\n",_now->msg);
		_now = _now->next;
	}
}


void _rtcTest(void) 
{
    const char    * device="/proc/driver/ds1340";
    int     fd;

	INIT_ERROR;
    INIT_DOT;

    SUCC_DOT("RTC Device Open") ;
    if((fd=open(device,O_RDONLY))<0) {
        CLOSE_DOT;
    	ADD_ERROR("RTC Device Access Fail (ECODE=%d)", errno);
        return;
    }

    CLOSE_DOT;
    close(fd);
}


void _gpioTest(void)
{
    INIT_DOT;
	INIT_ERROR;

    SUCC_DOT("Read GP_PWR_FAIL_INPUT(19)"); if(GPIOAPI_ReadGpio(GP_PWR_FAIL_INPUT)) {ADD_ERROR("%s", "Invalid GPIO (PWR_FAIL:HIGH)");}
    SUCC_DOT("Read GP_INT_ETH_INPUT(21)");  if(GPIOAPI_ReadGpio(GP_INT_ETH_INPUT)) {ADD_ERROR("%s", "Invalid GPIO (INT_ETH:HIGH)");}
    SUCC_DOT("Read GP_LOW_BATT_INPUT(31)"); if(!GPIOAPI_ReadGpio(GP_LOW_BATT_INPUT)) {ADD_ERROR("%s", "Invalid GPIO (LOW_BATT:LOW)");}
    /* 검사에서 제외
     SUCC_DOT("Read GP_GSM_SYNC_INPUT(32)"); if(!GPIOAPI_ReadGpio(GP_GSM_SYNC_INPUT)) {ADD_ERROR("%s", "Invalid GPIO (GSM_SYNC:LOW)");}
    */
    SUCC_DOT("Read GP_BATT_CHG_STATUS_INPUT(74)"); ;if(!GPIOAPI_ReadGpio(GP_BATT_CHG_STATUS_INPUT) && 0) {ADD_ERROR("%s", "Invalid GPIO (BATT_CHR_STAT:LOW)");}

    CLOSE_DOT;
}

int _gsmtest(int fd, const char * msg, int len)
{
    char buffer[512];

    if(write(fd, msg, len) != len) {
        return -1;
    }

    usleep(500000);
    memset(buffer,0,sizeof(buffer));

    while((len=read(fd, buffer, sizeof(buffer))) > 0) {
        if(strstr(buffer,"OK")!=NULL) return 0;
    }
    return -2;
}

void _gsmpowertest(int fd, const char *msg, int len)
{
    char buffer[512];
    char * pnt;
    int temp = 0;
	INIT_ERROR;

    if(write(fd, msg, len) != len) {
		ADD_ERROR("GSM/GPRS Module Access Error (ECODE=%d)",errno);
    }

    usleep(500000);
    memset(buffer,0,sizeof(buffer));

    while((len=read(fd, buffer, sizeof(buffer))) > 0) {
        if((pnt=strstr(buffer,"^SBV:"))!=NULL)  {
            pnt[11] = 0x0;
            temp = strtol(pnt+6,(char **)NULL,10);
            if(temp < 3750 || temp > 4200) {ADD_ERROR("GSM/GPRS Module Voltage Check {%d}", temp);}
			return;
        }
    }
}

void _mobileTest(void)
{
    int fd;
    INIT_DOT;
	INIT_ERROR;

    SUCC_DOT("Mobile Device Open");
    if((fd=openSerial("/dev/ttyS00", B9600, 0))<0) {
    	ADD_ERROR("Mobile Device Access Error (ECODE=%d)",errno);
        CLOSE_DOT;
        return;
    }

    SUCC_DOT("ATI"); if(_gsmtest(fd,"ATI\r\n", 4)) {ADD_ERROR("%s","Additional identifacation information");}
	SUCC_DOT("AT^SCID"); if(_gsmtest(fd,"AT^SCID\r\n", 9)) {ADD_ERROR("%s", "SIM card identifcation number");}
	SUCC_DOT("AT+CPIN?"); if(_gsmtest(fd,"AT+CPIN?\r\n", 10)) {ADD_ERROR("%s", "SIM PIN authentication");}
	SUCC_DOT("AT+CPIN2?"); if(_gsmtest(fd,"AT+CPIN2?\r\n", 11)) {ADD_ERROR("%s", "SIM PIN2 authentication");}
	SUCC_DOT("AT+CIMI"); if(_gsmtest(fd,"AT+CIMI\r\n", 9)) {ADD_ERROR("%s", "Request international mobile subscriber identity");}
	SUCC_DOT("AT+CCLK?"); if(_gsmtest(fd,"AT+CCLK?\r\n", 10)) {ADD_ERROR("%s", "Real Time Clock");}
	SUCC_DOT("AT+COPS?"); if(_gsmtest(fd,"AT+COPS?\r\n", 10)) {ADD_ERROR("%s", "Operator selection");}
	SUCC_DOT("AT+CREG?"); if(_gsmtest(fd,"AT+CREG?\r\n", 10)) {ADD_ERROR("%s", "Network registration");}
	SUCC_DOT("AT+CSQ"); if(_gsmtest(fd,"AT+CSQ\r\n", 8)) {ADD_ERROR("%s", "Signal quality");}
	SUCC_DOT("AT^SBV"); if(_gsmtest(fd,"AT^SBV\r\n", 8)) {ADD_ERROR("%s", "Monitoring power supply");}
	SUCC_DOT("GSM Module Voltage"); _gsmpowertest(fd,"AT^SBV\r\n", 8);
	SUCC_DOT("AT^MONI"); if(_gsmtest(fd,"AT^MONI\r\n", 9)) {ADD_ERROR("%s", "Monitor idle mode and dedicated mode");}
	SUCC_DOT("AT^MONP"); if(_gsmtest(fd,"AT^MONP\r\n", 9)) {ADD_ERROR("%s", "Monitor neighbour cells");}
	SUCC_DOT("AT^SMONC"); if(_gsmtest(fd,"AT^SMONC\r\n", 10)) {ADD_ERROR("%s", "Cell Monitoring");}
	SUCC_DOT("AT^SMONG"); if(_gsmtest(fd,"AT^SMONG\r\n", 10)) {ADD_ERROR("%s", "GPRS Monitor");}

    CLOSE_DOT;
    close(fd);
}

void _coorditest(HANDLE codi, ARGU_DOT)
{
	BYTE	szBuffer[1024];
	int		nError, nLength;
	int		nState;
	int		initCount = 90;
    COORDINATOR * pcodi = (COORDINATOR *) codi;
	INIT_ERROR;

    CHILD_DOT;

#define STATE_INIT					0
#define STATE_READY					1
#define STATE_MODULE_PARAM			10

	for(nState=STATE_INIT; ;)
	{
        nError = codiGetState(codi);

		switch(nState) {
		  case STATE_INIT :
               nState = STATE_READY; 
               SUCC_DOT("Stackup Check");
               break;

          case STATE_READY :        
               nError = codiGetState(codi);

               if (nError != CODISTATE_NORMAL)
               {
                    if(initCount < 0) { ADD_ERROR("%s", "Stackup Fail"); RETURN_DOT; return;}
                    initCount --;
                    usleep(1000000); 
                    break;           
               }
               SUCC_DOT("Stack Version Check");
               if(!((pcodi->fw >> 4)&0x06)) { ADD_ERROR("Invalid Stack Version {%d}", pcodi->fw >> 4); }
               nState = STATE_MODULE_PARAM;
               break;               

		  case STATE_MODULE_PARAM :
               SUCC_DOT("Module Param Check");
			   codiSync(codi, 3);
			   nError = codiGetProperty(codi, CODI_CMD_MODULE_PARAM, (BYTE *)szBuffer, &nLength, 1000);
			   if (nError != CODIERR_NOERROR) {ADD_ERROR("Get Module param fail (ECODE=%d)",nError);} 
               RETURN_DOT;
			   return;
		}
	}
}

void _coordiTest(void)
{
    CODIDEVICE	codiDevice =
    {
	    CODI_COMTYPE_RS232,								// 디바이스
	    "/dev/ttyS01",									// 디바이스명
	    115200,											// 속도
	    8,												// 데이터 비트
	    1,												// 스톱 비트
	    CODI_PARITY_NONE,								// 패리티
        CODI_OPT_RTSCTS                                 // 옵션
    };

	HANDLE	codi;
	int		nError;

	INIT_ERROR;
    INIT_DOT;

    SUCC_DOT("Coordinator Initalizing");

	// CODIAPI를 초기화 한다.
	nError = codiInit();
	if (nError != CODIERR_NOERROR)
	{
        ADD_ERROR("Initalize Fail (ECODE=%d)", nError);
        CLOSE_DOT;
		codiExit();
		return;
	}

	// 새로운 Coordinator 장치를 등록한다.
	nError = codiRegister(&codi, &codiDevice);
	if (nError != CODIERR_NOERROR)
	{
		codiExit();
        ADD_ERROR("Initalize Fail (ECODE=%d)", nError);
		CLOSE_DOT;
		return;
	}

	// Coordinator 서비스를 시작 시킨다.
	nError = codiStartup(codi);
	if (nError != CODIERR_NOERROR)
	{
		codiUnregister(codi);
		codiExit();
        ADD_ERROR("Startup  Fail (ECODE=%d)", nError);
		CLOSE_DOT;
		return;
	}

	// 사용자 프로그램을 여기에 코딩한다.
	_coorditest(codi, PARAM_DOT);

    SUCC_DOT("Coordinator Shutdown");

	// Coordinator 서비스를 종료한다.
	nError = codiShutdown(codi);
	if (nError != CODIERR_NOERROR)  {
		ADD_ERROR("Shutdown Fail (ECODE=%d)", nError);
	}

	// 등록된 디바이스를 해제한다.
	nError = codiUnregister(codi);
	if (nError != CODIERR_NOERROR) {
		ADD_ERROR("Unregister Fail (ECODE=%d)", nError);
	}

	// API를 종료한다.
	codiExit();

    CLOSE_DOT;
}

int OnOffHeater(int onoff)
{
    int fd;
    INIT_ERROR;

    if((fd=open("/proc/sys/dev/sensors/lm75-i2c-0-48/temp", O_RDWR | O_NOCTTY))<0) {
		ADD_ERROR("Heater Device Access Fail (ECODE=%d)", errno);
        return -1;
    }

    if(onoff) {
        write(fd,"-10.0 0.0",9);
    }else {
        write(fd,"50.0 70.0",9);
    }
    close(fd);
    usleep(500000);
    return 0;
}

void _heaterTest(void)
{
	INIT_ERROR;
    INIT_DOT;

    SUCC_DOT("Heater Activity Check");
    if(!OnOffHeater(1)) {
        if(GPIOAPI_ReadGpio(GP_TEMP_OS_INPUT)) {
		    ADD_ERROR("%s", "Heater On Fail");
        }
    }else {
        ADD_ERROR("%s","Heater On Fail 2");
    }
    if(!OnOffHeater(0)) {
        if(!GPIOAPI_ReadGpio(GP_TEMP_OS_INPUT)) {
		    ADD_ERROR("%s", "Heater Off Fail");
        }
    }else {
        ADD_ERROR("%s","Heater Off Fail");
    }

    if(!OnOffHeater(1)) {
        if(GPIOAPI_ReadGpio(GP_TEMP_OS_INPUT)) {
		    ADD_ERROR("%s", "Heater On Fail");
        }
    }else {
        ADD_ERROR("%s","Heater On Fail");
    }

    CLOSE_DOT;

    return;
}

void _teardownled(void)
{
    int i;
    for(i=0;m_LedTable[i][0];i++) {
        if(m_LedTable[i][0]>0) GPIOAPI_WriteGpio(m_LedTable[i][0], 1);
        if(m_LedTable[i][1]>0) GPIOAPI_WriteGpio(m_LedTable[i][1], 1);
    }
}

void _powerTest(void)
{
    INIT_DOT;
	INIT_ERROR;
    int i,j, toggle=1;
    int fd;

    SUCC_DOT("Waiting for Power Fail Signal"); 
    if((fd=open("/dev/gpio", O_RDWR|O_NDELAY))<0) {
        ADD_ERROR("%s", "GPIO Device Open Fail");
        return;
    }

    for(i=1;m_LedTable[i][0];i++) {
        if(m_LedTable[i][0]>0) ioctl(fd, GPIO_IOCTL_DIR, GPIOSET(m_LedTable[i][0]));
        if(m_LedTable[i][1]>0) ioctl(fd, GPIO_IOCTL_DIR, GPIOSET(m_LedTable[i][1]));
    }

    while(!kbhit()) {
        for(j=0;m_LedTable[j][0];j++) {
            if(m_LedTable[j][0]>0) GPIOAPI_WriteGpio(m_LedTable[j][0], toggle);
            if(m_LedTable[j][1]>0) GPIOAPI_WriteGpio(m_LedTable[j][1], 1 - toggle);
        }
        toggle = 1 - toggle;
        if(GPIOAPI_ReadGpio(GP_PWR_FAIL_INPUT))  {
            _teardownled();
            if(OnOffHeater(0)) {
                ADD_ERROR("%s","Heater Off Fail");
            }
            close(fd);
            CLOSE_DOT;
            return;
        }
        usleep(500000);
    }
    ADD_ERROR("%s", "PWR_FAIL Signal Fail");

    readch();

    _teardownled();
    CLOSE_DOT;
}

int _mobileCondition(void)
{
    /*-- Issue #2024: CDMA 집중기의 경우 Mobile 실장이 되지 않기 때문에 테스트 하지 않는다 */
    if(strcasecmp(m_szMcuModel, "O201") == 0) return 0;

    return 1;
}


int _heaterCondition(void)
{
    /*-- O312 제품에 대해서만 heater 검사를 한다 --*/
    if(strcasecmp(m_szMcuModel, "O312") == 0) return 1;

    return 0;
}

/*--------------------------------------------------------------------------*/
int openSerial(const char * device,  int speed, int rtscts) 
{
    int fd;
    struct  termios  oldtio,newtio;

    if((fd=open(device, O_RDWR | O_NOCTTY ))<0) {
        return fd;
    }

	tcgetattr( fd, &oldtio );  // 현재 설정을 oldtio에 저장

	memset( &newtio, 0, sizeof(newtio) );
	newtio.c_cflag = speed | CS8 | CLOCAL | CREAD | rtscts ; 
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	newtio.c_lflag = 0;
	    
	newtio.c_cc[VTIME] = 10;	// time-out 값으로 TIME*0.1초
	newtio.c_cc[VMIN]  = 0;     // MIN은 read가 리턴되기 위한 최소한의 문자 수
	    
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &newtio );

    return fd;
}

void printDot(int cnt)
{
    int i;
    for(i=0;i<cnt;i++) {
        printf(".");
    }
    fflush(stdout);
}

void getEtherInfo(char *pszName, UINT *addr, UINT *subnet, BYTE *phyAddr)
{
    int     sSocket;
    int     nReqCount = 10;
    struct  ifconf s_ifconfig;
    struct  ifreq *s_ifrequest;
    struct  sockaddr_in *s_SockAddr_In;
	int		nMask;

	*pszName	= '0';
	*addr		= 0;
	*subnet		= 0;
	memset(phyAddr, 0, 6);

	sSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (sSocket < 0)
		return;

    memset(&s_ifconfig, 0, sizeof(struct ifconf));
    for(;;)
    {
        s_ifconfig.ifc_len = sizeof(struct ifreq) * nReqCount;
        s_ifconfig.ifc_buf = (char *)malloc(s_ifconfig.ifc_len);
        if (s_ifconfig.ifc_buf == NULL)
            break;

        if (ioctl(sSocket, SIOCGIFCONF, &s_ifconfig) != 0)
            break;

        if (s_ifconfig.ifc_len >= (int)(sizeof(struct ifreq) * nReqCount))
			break;

		s_ifrequest = s_ifconfig.ifc_req;
		if (s_ifrequest == NULL)
			break;

		for(nReqCount=0; nReqCount<s_ifconfig.ifc_len; nReqCount+=sizeof(struct ifreq), s_ifrequest++)
		{
			nMask = 0;
			if (ioctl(sSocket, SIOCGIFFLAGS, s_ifrequest) == 0)
				nMask = s_ifrequest->ifr_flags;

			if (nMask & IFF_LOOPBACK)
				continue;

			strcpy(pszName, s_ifrequest->ifr_name);
			s_SockAddr_In = (struct sockaddr_in *)(&s_ifrequest->ifr_addr);
			*addr 		  = s_SockAddr_In->sin_addr.s_addr;
			ioctl(sSocket, SIOCGIFNETMASK, s_ifrequest);
			s_SockAddr_In = (struct sockaddr_in *)&s_ifrequest->ifr_netmask;
			*subnet		  = s_SockAddr_In->sin_addr.s_addr;

			if (ioctl(sSocket, SIOCGIFHWADDR, s_ifrequest) == 0)
                memcpy(phyAddr, &s_ifrequest->ifr_hwaddr.sa_data, 6);

			if (strncmp(s_ifrequest->ifr_name, "ppp", 3) == 0)
				break;
		}
        break;
    }

    if (s_ifconfig.ifc_buf) free(s_ifconfig.ifc_buf);
    close(sSocket);
}

int testComplete(void)
{
    int fd;

    if((fd=open("/app/conf/factoryTestCompleted", O_WRONLY | O_CREAT))<0) return fd;

    close(fd);
    return 0;
}

/*--------------------------------------------------------------------------*/

void testAllTests(void)
{
    int         i;
    int         testResult = 0;

	_freeErrorMessage();
    for(i=0; m_ProcTable[i].pszTitle ; i++) {
        /*-- Condition File이 있고 그 결과가 0 일때는 skip 한다 --*/
        if(m_ProcTable[i].pfnCond && !m_ProcTable[i].pfnCond()) continue;

        printf("        %02d. %-10s ", i+1, m_ProcTable[i].pszTitle); fflush(stdout);
        if(enableVerbose) printf("\n");
        printf("%s [%s]\n", enableVerbose ? "        " : "", 
            (m_ProcTable[i].pfnFunc(), errorMessage) 
                ? "\033[1;40;31mFAIL\033[0m" : "\033[1;40;34mPASS\033[0m");
        testResult |= errorMessage ? 1 : 0;
		if(!enableVerbose) _printErrorMessage();
		_freeErrorMessage();
    }
    if(!testResult) testComplete();
}

void displayTest()
{
	printf("\n--------------------------------------------------\n");
	printf("        --- Product Test ---   \n");
    printf("        MAC [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                macAddress[0], macAddress[1], macAddress[2],
                macAddress[3], macAddress[4], macAddress[5]);
	printf("--------------------------------------------------\n");
    testAllTests();
	printf("\n         0. Exit                          \n");
}

int productTest(void)
{
    char    c;
    
	while(1)
	{
        displayTest();
        displayPrompt();

        c = readch();

        if(c == '\n') {
            displayPrompt();
            continue;
        }

		if (c < '1' || c > '9')
			break;
    }

    return 0;
}

