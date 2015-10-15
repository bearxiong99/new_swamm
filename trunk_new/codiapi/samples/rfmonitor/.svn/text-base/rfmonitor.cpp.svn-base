#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>

#ifndef _WIN32
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
    #include <sys/wait.h>
#else
    #include <windows.h>
    #include <getopt.h>
    #include <unistd.h>
#endif
#include <ctype.h>

#include "codiapi.h"
#include "LinkedList.h"
#include "Hash.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"

#define     PRINT(...)      fprintf(stdout, __VA_ARGS__);fflush(stdout)
#define     ERRMSG(...)     fprintf(stderr, __VA_ARGS__);fflush(stderr)


#ifdef EMBEDDED_LINUX
#include "gpiomap.h"
#endif

void MainProcedure(HANDLE codi, int nPermit);
void ReportError(char *pszName, int nError);

void signal_handler(int nSignal);

char g_szCodiId[64];

CODIDEVICE	codiDevice =
{
	CODI_COMTYPE_RS232,					// 디바이스
	"/dev/ttyS1",						// 디바이스명
	115200,								// 속도
	8,									// 데이터 비트
	1,									// 스톱 비트
	CODI_PARITY_NONE,					// 패리티
	CODI_OPT_RTSCTS						// H/W Flow control
};

static BOOL m_bCodiExitPending = FALSE;

/**********************************************************************/
/** End Device Table */
typedef struct {
    EUI64       id;
    BYTE        LQI;
    signed char RSSI;
    int         state;
    int         position;
} ENDI;

CLinkedList<ENDI *> g_EndiList;
CHash g_EndListHash;
/**********************************************************************/

ENDI * GetEndiInfo(EUI64 * pId, BYTE LQI, signed char RSSI)
{
    ENDI   *pEndi = NULL;
    int         nIndex;

    nIndex = g_EndListHash.Find(pId, sizeof(EUI64));
    if (nIndex == -1)
    {
        pEndi = (ENDI *) MALLOC(sizeof(ENDI));
        memset(pEndi, 0, sizeof(ENDI));
        memcpy(&pEndi->id, pId, sizeof(EUI64));

        pEndi->position = (int)g_EndiList.AddTail(pEndi);

        g_EndListHash.Update(pId, sizeof(EUI64), pEndi->position);
    } else {
        pEndi = g_EndiList.Get((POSITION)nIndex);
    }

    if(pEndi != NULL) {
        int lqi;
        char szGUID[16+1];

        pEndi->LQI = LQI;
        pEndi->RSSI = RSSI;

        if((int) LQI < 80 ) {
            lqi = (int) LQI;
        }else {
            lqi = (int) (80 + (LQI - 80) * 0.125);
        }

        if(lqi>100) lqi = 100;

        memset(szGUID, 0, sizeof(szGUID));
        EUI64ToStr(pId, szGUID);

        PRINT("OUT:RF:%s:%d:%d\n", szGUID, lqi, RSSI);
    }

    return pEndi;
}

/**********************************************************************/

void Usage(char * filename)
{
	ERRMSG( "RF Monitor v1.0 20111118b1\n");
	ERRMSG( "Usage: %s [-dhp] serial_port_path\n", filename);
	ERRMSG( "    -d               : enable debugging\n");
    ERRMSG( "    -p state         : update permit control (0:close, 1:open)\n");
	ERRMSG( "    -h               : display this message\n");
	ERRMSG( "\n");
}

int main(int argc, char **argv)
{
#ifndef _WIN32
    struct  sigaction   handler;
#endif
	HANDLE	codi;
	int		nError;
    int     nDebugMode = 1;
    int     nPermit=-1;
    int    opt;
    char    *pDevicePath = NULL;

#ifndef _WIN32
	// 보편적인 시그널 핸들러를 설치한다.
	handler.sa_handler = signal_handler;
    sigfillset(&handler.sa_mask);
    sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);
    sigaction(SIGCHLD, &handler, 0);
#else
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
#endif

    while((opt=getopt(argc, argv, "dhp:")) != -1) {
        switch(opt) {
            case 'd': 
                nDebugMode = 0;
                break;
            case 'p': 
                nPermit = (int)strtol(optarg,(char **)NULL,10) == 0 ? 0 : 255;
                break;
            case 'h':
            default :
                Usage(argv[0]);
                return (1);
        }
    }

    if((argc - optind) < 1) {
        Usage(argv[0]);
        return (2);
    }

	// 디버깅 화면 출력(0), 해제(1)	
    SET_DEBUG_FILE(stderr);
	SET_DEBUG_MODE(nDebugMode);

    pDevicePath = strdup(argv[optind]);
    memset(g_szCodiId, 0, sizeof(g_szCodiId));

    memset(codiDevice.szDevice, 0, sizeof(codiDevice.szDevice));
    strncpy(codiDevice.szDevice, pDevicePath, MIN(strlen(pDevicePath), sizeof(codiDevice.szDevice)-1));

	// CODIAPI를 초기화 한다.
	nError = codiInit();
	if (nError != CODIERR_NOERROR)
	{
		ReportError(NULL, nError);
		return (3);
	}

	// 새로운 Coordinator 장치를 등록한다.
	nError = codiRegister(&codi, &codiDevice);
	if (nError != CODIERR_NOERROR)
	{
		ReportError(NULL, nError);
		codiExit();
		return (3);
	}

	// Coordinator 서비스를 시작 시킨다.
	nError = codiStartup(codi);
	if (nError != CODIERR_NOERROR)
	{
		ReportError(NULL, nError);
		codiUnregister(codi);
		codiExit();
		return (3);
	}

	// 사용자 프로그램을 여기에 코딩한다.
	MainProcedure(codi, nPermit);

	// Coordinator 서비스를 종료한다.
	nError = codiShutdown(codi);
	if (nError != CODIERR_NOERROR)
		ReportError(NULL, nError);

	// 등록된 디바이스를 해제한다.
	nError = codiUnregister(codi);
	if (nError != CODIERR_NOERROR)
		ReportError(NULL, nError);

	// API를 종료한다.
	codiExit();
	return (0);
}

const char *GetStateMessage(int nState)
{
	switch(nState) {
	  case CODISTATE_NORMAL :			return "Normal";
	  case CODISTATE_NOT_STARTED :		return "Coordinator not started";
	  case CODISTATE_NOT_CONNECTED :	return "Coordinator not connected";
	  case CODISTATE_STACK_NOT_READY :	return "Coordinator Stack not ready";
	  case CODISTATE_STACK_DOWN :		return "Coordinator Stack down";
	  case CODISTATE_JOIN_FAIL :		return "Coordinator Join Fail";
	  case CODISTATE_NO_RESPONSE :		return "Coordinator No Response";
	  case CODISTATE_ERROR :			return "Coordinator Error";
	}
	return "Unknown State";
}

void MtorSensor(HANDLE codi) 
{
	CODI_MANYTOONE_PAYLOAD *pManyToOne;
	BYTE	        payload[256];
    BYTE            mtorType = CODI_MANYTOONE_LOW_RAM;

	pManyToOne 		   = (CODI_MANYTOONE_PAYLOAD *)&payload;
	pManyToOne->type   = mtorType;
	pManyToOne->radius = 10;

	codiSetProperty(codi, CODI_CMD_MANY_TO_ONE_NETWORK,
	        (BYTE *)&payload, sizeof(CODI_MANYTOONE_PAYLOAD), 3000);
}

void TimesyncSensor(HANDLE codi)
{
	ENDI_TIMESYNC_PAYLOAD *pTimesync;
	BYTE	        payload[256];
    time_t          now;
	struct	tm		when;

	time(&now);
	when = *localtime(&now);

	pTimesync = (ENDI_TIMESYNC_PAYLOAD *)&payload;
#ifdef  __USE_BSD
	pTimesync->gmt.tz	= ntoshort((int)(when.tm_gmtoff/60));
#else
	pTimesync->gmt.tz	= ntoshort(0);
#endif
	pTimesync->gmt.dst	= 0;
	pTimesync->gmt.year	= ntoshort(when.tm_year + 1900);
	pTimesync->gmt.mon	= (BYTE)when.tm_mon + 1;
	pTimesync->gmt.day	= (BYTE)when.tm_mday;
	pTimesync->gmt.hour	= (BYTE)when.tm_hour;
	pTimesync->gmt.min	= (BYTE)when.tm_min;
	pTimesync->gmt.sec	= (BYTE)when.tm_sec;
	pTimesync->nazc     = 0;
    pTimesync->type     = 0;

	codiMulticast(codi, ENDI_DATATYPE_TIMESYNC, (BYTE *)&payload, sizeof(ENDI_TIMESYNC_PAYLOAD));
}


#define STATE_INIT					0
#define STATE_READY					1
#define STATE_PERMIT				17
#define STATE_SET_PERMIT			40
#define STATE_RF_POWER				41
#define STATE_POWER_MODE			42
#define STATE_RESET					55
#define STATE_MTOR				    56
#define STATE_TIMESYNC				57
#define STATE_WAIT					100

void MainProcedure(HANDLE codi, int nPermit)
{
	TIMETICK	start, prev, cur, mtor;
	int		nError = CODIERR_NOERROR;
	int		nState, nElapse, nCount = 0;
    CODI_PERMIT_PAYLOAD     permit;
    COORDINATOR *pCoordi;

    GetTimeTick(&start);
	for(nState=STATE_INIT; !m_bCodiExitPending;)
	{
        GetTimeTick(&prev);

		switch(nState) {
		  case STATE_INIT :
               nState = STATE_READY;
               break;

          case STATE_READY :        
               nError = codiGetState(codi);
               XDEBUG("CODINATOR STATE(%d) = %s\r\n", nError, GetStateMessage(nError));

               if(nCount > 5) nCount = 0;
               if (nError != CODISTATE_NORMAL)
               {
                   if(nCount == 0) {
                    nState = STATE_RESET;
                   } else {
                       nCount ++;
                       USLEEP(1000000);
                   }
                   break;           
               }
               nState = STATE_SET_PERMIT;
               break;               

		  case STATE_RESET :
			   nError = codiReset(codi);
			   if (nError != CODIERR_NOERROR) ReportError(NULL, nError);
               USLEEP(1000000); 
			   nState = STATE_READY;
			   break;

          case STATE_SET_PERMIT:
               if(nPermit >=0 && nPermit <= 255)
               {
                    memset(&permit, 0, sizeof(CODI_CMD_PERMIT));
                    permit.permit_time = (BYTE)(nPermit & 0xFF);
                    nError = codiSetProperty(codi, CODI_CMD_PERMIT, (BYTE *)&permit, 
                        sizeof(CODI_PERMIT_PAYLOAD), 3000);
			        if (nError != CODIERR_NOERROR) ReportError(NULL, nError);
               }
               nState = STATE_PERMIT;
               break;

		  case STATE_PERMIT :
               codiSync(codi, 3);

               pCoordi = (COORDINATOR *)codi;

               PRINT("OUT:CHANNEL:%d\n", pCoordi->channel);
               PRINT("OUT:PANID:%d\n", pCoordi->panid);
               PRINT("OUT:PERMIT:%d\n", pCoordi->permit_time);
               PRINT("OUT:POWER:%d\n", pCoordi->power);

			   nState = STATE_MTOR;
			   break;

          case STATE_MTOR :
               GetTimeTick(&mtor);
               MtorSensor(codi);
			   nState = STATE_TIMESYNC;
               break;

          case STATE_TIMESYNC :
               GetTimeTick(&cur);
			   nElapse = GetTimeInterval(&mtor, &cur);
               if(nElapse > 4000) {
                TimesyncSensor(codi);
			    nState = STATE_WAIT;
               } else {
			    USLEEP(500000);
			    nState = STATE_TIMESYNC;
               }
               break;

		  case STATE_WAIT :
               GetTimeTick(&cur);
			   nElapse = GetTimeInterval(&mtor, &cur);
               if(nElapse > 60000) {
			    nState = STATE_MTOR;
               } else {
			    USLEEP(500000);
			    nState = STATE_WAIT;
               }
			   break;
		}

        GetTimeTick(&cur);
	    nElapse = GetTimeInterval(&prev, &cur);
	}

    GetTimeTick(&cur);
	nElapse = GetTimeInterval(&start, &cur);
}

void signal_handler(int nSignal)
{
#ifndef _WIN32
	int		status;
	int		pid;
#endif

	switch(nSignal) {
	  case SIGINT :
	  case SIGTERM :
		   m_bCodiExitPending = TRUE;
		   break;
#ifndef _WIN32
	  case SIGCHLD :
		   pid = waitpid(-1, &status, WNOHANG);
		   break;
#endif
	}
}

void ReportError(char *pszName, int nError)
{
	if (pszName && *pszName)
    {
	    ERRMSG( "------ %s ERROR(%d): %s ------\r\n", pszName, nError, codiErrorMessage(nError));
    }
	else 
    {
        ERRMSG( "------ ERROR(%d): %s ------\r\n", nError, codiErrorMessage(nError));
    }
}

#ifdef EMBEDDED_LINUX
void PowerControl(BOOL bOnOff)
{
    int     fd;
               
    fd = open("/dev/gpio", O_RDWR | O_NDELAY);
    if (fd <= 0)
        return;
               
#if	defined(__SUPPORT_NZC1__)
    ioctl(fd, GPIO_IOCTL_DIR, GPIOSET(GP_CODI_PCTL_OUT));	// Coordinator VDD
#endif
    ioctl(fd, GPIO_IOCTL_DIR, GPIOSET(GP_CODI_RST_OUT));	// Coordinator RESET
               
#if	defined(__SUPPORT_NZC1__)
    if (bOnOff)
    {
        ioctl(fd, GPIO_IOCTL_OUT, GPIOHIGH(GP_CODI_PCTL_OUT));
#endif
        ioctl(fd, GPIO_IOCTL_OUT, GPIOLOW(GP_CODI_RST_OUT));
        USLEEP(1000000);
        ioctl(fd, GPIO_IOCTL_OUT, GPIOHIGH(GP_CODI_RST_OUT));
#if	defined(__SUPPORT_NZC1__)
    }          
    else       
    {          
        ioctl(fd, GPIO_IOCTL_OUT, GPIOLOW(GP_CODI_PCTL_OUT));
    }
#endif
    close(fd); 
}              
#endif         

void codiOnRegister(HANDLE codi, CODIDEVICE *device)
{
}

void codiOnUnregister(HANDLE codi, CODIDEVICE *device)
{
}

void codiOnStartup(HANDLE codi)
{
#ifdef EMBEDDED_LINUX
    PowerControl(TRUE);
#endif
}

void codiOnShutdown(HANDLE codi)
{
}

void codiOnBoot(HANDLE codi, EUI64 *id, WORD shortid, BYTE fw, BYTE hw, BYTE zaif, BYTE zzif, BYTE build, BYTE rk)
{
    char szId[64];

    memset(szId, 0, sizeof(szId));
    EUI64ToStr(id, szId);

    if(strcmp(g_szCodiId, szId)) 
    {
        sprintf(g_szCodiId, "%s", szId);
        PRINT("OUT:CODI:%s\n", g_szCodiId);
    }
}

void codiOnStackStatus(HANDLE codi, BYTE status)
{
}

void codiOnDeviceJoin(HANDLE codi, EUI64 *id, WORD shortid, WORD parentid, BYTE status)
{
}

void codiOnEnergyScan(HANDLE codi, BYTE channel, BYTE rssi)
{
}

void codiOnActiveScan(HANDLE codi, BYTE channel, WORD panid, BYTE *extpanid, BYTE expjoin, BYTE profile)
{
}

void codiOnScanComplete(HANDLE codi, BYTE channel, BYTE status)
{
}

void codiOnSend(HANDLE codi, BYTE ctrl, BYTE type, BYTE *payload, int nLength)
{
}

void codiOnError(HANDLE codi, BYTE code)
{
}

void codiOnNoResponse(HANDLE codi, BOOL bResetNow)
{
#ifdef EMBEDDED_LINUX
    PowerControl(TRUE);
#endif
}

void codiOnRouteRecord(HANDLE codi, EUI64 *id, WORD shortid, BYTE hops, WORD *path)
{
}

BOOL codiOnQueryRouteRecord(HANDLE codi, EUI64 *id, WORD *shortid, BYTE *hops, WORD *path)
{
    return FALSE;
}

BOOL codiOnQuerySuperParent(EUI64 *id, EUI64 *parent, WORD *shortid, BYTE *hops, WORD *path)
{
	return FALSE;
}

void codiOnCommand(HANDLE codi, BYTE ctrl, BYTE type, BYTE *pBuffer, int nLength)
{
}

void codiOnCommandError(HANDLE codi, BYTE ctrl, BYTE cmd, BYTE code)
{
}

void codiOnInformation(HANDLE codi, BYTE ctrl, BYTE type, BYTE *pBuffer, int nLength)
{
}

void endiOnConnect(HANDLE codi, HANDLE endi)
{
}

void endiOnDisconnect(HANDLE codi, HANDLE endi)
{
}

void endiOnSend(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE type, BYTE seq, BYTE *payload, int nLength)
{
}

void endiOnError(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE type, BYTE code)
{
}

void endiOnBypass(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE	*pEndDevice;
	pEndDevice = (ENDDEVICE *)endi;
    if(pEndDevice != NULL) GetEndiInfo(&pEndDevice->id, LQI, RSSI);
}

void endiOnNanData(HANDLE codi, EUI64 *id, BYTE reqid, BYTE flow, BYTE tailframe, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
    GetEndiInfo(id, LQI, RSSI);
}

void endiOnPush(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnAmrData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE	*pEndDevice;
	pEndDevice = (ENDDEVICE *)endi;
    if(pEndDevice != NULL) GetEndiInfo(&pEndDevice->id, LQI, RSSI);
}

void endiOnNetworkLeave(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LEAVE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE	*pEndDevice;
	pEndDevice = (ENDDEVICE *)endi;
    if(pEndDevice != NULL) GetEndiInfo(&pEndDevice->id, LQI, RSSI);
}

void endiOnLongRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LONG_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE	*pEndDevice;
	pEndDevice = (ENDDEVICE *)endi;
    if(pEndDevice != NULL) GetEndiInfo(&pEndDevice->id, LQI, RSSI);
}

void endiOnRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE	*pEndDevice;
	pEndDevice = (ENDDEVICE *)endi;
    if(pEndDevice != NULL) GetEndiInfo(&pEndDevice->id, LQI, RSSI);
}

void endiOnMccbData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_MCCB_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE	*pEndDevice;
	pEndDevice = (ENDDEVICE *)endi;
    if(pEndDevice != NULL) GetEndiInfo(&pEndDevice->id, LQI, RSSI);
}


void endiOnMetering(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_METERING_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
    GetEndiInfo(id, LQI, RSSI);
}

#ifdef __SUPPORT_KUKDONG_METER__
void endiOnKeukdongData(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}
#endif

void endiOnEvent(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_EVENT_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
    GetEndiInfo(id, LQI, RSSI);
}

void endiOnKeepAlive(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
    GetEndiInfo(id, LQI, RSSI);
}

void endiOnCommand(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_COMMAND_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE	*pEndDevice;
	pEndDevice = (ENDDEVICE *)endi;
    if(pEndDevice != NULL) GetEndiInfo(&pEndDevice->id, LQI, RSSI);
}

void endiOnState(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_STATE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI, WORD shortid)
{
    GetEndiInfo(id, LQI, RSSI);
}

void endiOnDiscovery(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_DISCOVERY_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
    GetEndiInfo(id, LQI, RSSI);
}

void endiOnUserDefine(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE type, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
    GetEndiInfo(id, LQI, RSSI);
}

void endiOnMbusData(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE mseq, BYTE mtype, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
    GetEndiInfo(id, LQI, RSSI);
}

#if defined(__SUPPORT_DEVICE_NODE__)
void endiOnTimesync(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_TIMESYNC_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
    //GetEndiInfo(id, LQI, RSSI);
}
#endif

BOOL endiOnLink(HANDLE codi, EUI64 *id, BYTE type, BYTE LQI, signed char RSSI)
{
    GetEndiInfo(id, LQI, RSSI);
	// MCU와 PDA만 응답을 한다.
	return FALSE;
}

void endiOnRead(HANDLE codi, EUI64 *id, BYTE seq, WORD addr, int nLength, BYTE LQI, signed char RSSI)
{
    GetEndiInfo(id, LQI, RSSI);
}

void endiOnData(HANDLE codi, EUI64 *id, BYTE seq, BYTE *data, int nLength, BYTE LQI, signed char RSSI)
{
    XDEBUG("endiOnData\r\n");
    GetEndiInfo(id, LQI, RSSI);
}


