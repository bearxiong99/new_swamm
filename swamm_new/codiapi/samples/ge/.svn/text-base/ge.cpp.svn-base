#ifndef _WIN32
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
#include <sys/wait.h>
#else
#include <windows.h>
#endif

#include "codiapi.h"
#include "Ondemander.h"
#include "DebugUtil.h"
#include "Chunk.h"
#include "CommonUtil.h"

#ifdef EMBEDDED_LINUX
#include "gpiomap.h"
#endif

void MainProcedure(HANDLE codi);
void ReportError(char *pszName, int nError);

#ifndef _WIN32
void signal_handler(int nSignal);
#endif

CODIDEVICE	codiDevice =
{
	CODI_COMTYPE_RS232,								// 디바이스
	"/dev/ttyS01",									// 디바이스명
	115200,											// 속도
	8,												// 데이터 비트
	1,												// 스톱 비트
	CODI_PARITY_NONE,								// 패리티
	CODI_OPT_RTSCTS						// H/W Flow control
};

static BOOL m_bCodiExitPending = FALSE;
static char szSensorID[17] = "000B1200750C5FE6";
//static char szSensorID[17] = "000D6F000011A83C";
COndemander	m_myOndemander;

int main(int argc, char **argv)
{
#ifndef _WIN32
    struct  sigaction   handler;
#endif
	HANDLE	codi;
	int		nError;

#ifndef _WIN32
	// 보편적인 시그널 핸들러를 설치한다.
	handler.sa_handler = signal_handler;
    sigfillset(&handler.sa_mask);
    sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);
    sigaction(SIGCHLD, &handler, 0);
#endif
	
	SET_DEBUG_MODE(0);

	// CODIAPI를 초기화 한다.
	nError = codiInit();
	if (nError != CODIERR_NOERROR)
	{
		ReportError(NULL, nError);
		return 0;
	}

	// 새로운 Coordinator 장치를 등록한다.
	nError = codiRegister(&codi, &codiDevice);
	if (nError != CODIERR_NOERROR)
	{
		ReportError(NULL, nError);
		codiExit();
		return 0;
	}

	// Coordinator 서비스를 시작 시킨다.
	nError = codiStartup(codi);
	if (nError != CODIERR_NOERROR)
	{
		ReportError(NULL, nError);
		codiUnregister(codi);
		codiExit();
		return 0;
	}

	// 사용자 프로그램을 여기에 코딩한다.
	MainProcedure(codi);

	// Coordinator 서비스를 종료한다.
	nError = codiShutdown(codi);
	if (nError != CODIERR_NOERROR)
		ReportError(NULL, nError);

	nError = codiUnregister(codi);
	if (nError != CODIERR_NOERROR)
		ReportError(NULL, nError);

	codiExit();
	return 0;
}

#ifndef _WIN32
void signal_handler(int nSignal)
{
	int		status;
	int		pid;

	switch(nSignal) {
	  case SIGINT :
	  case SIGTERM :
		   m_bCodiExitPending = TRUE;
		   break;
	  case SIGCHLD :
		   pid = waitpid(-1, &status, WNOHANG);
		   break;
	}
}
#endif

void ReportError(char *pszName, int nError)
{
	if (pszName && *pszName)
		 printf("------ %s ERROR(%d): %s ------\r\n", pszName, nError, codiErrorMessage(nError));
	else printf("------ ERROR(%d): %s ------\r\n", nError, codiErrorMessage(nError));
}

#define STATE_INIT					0
#define STATE_READY					1
#define STATE_MODULE_PARAM			10
#define STATE_SERIAL_PARAM			11
#define STATE_NETWORK_PARAM			12
#define STATE_SECURITY_PARAM		13
#define STATE_STACK_MEMORY			14
#define STATE_PERMIT				15
#define STATE_METERING				50
#define STATE_AIDON					51
#define STATE_OPEN					52
#define STATE_CONNECT				53
#define STATE_AIDON_SEND			54
#define STATE_AIDON_REPLY			55
#define STATE_DISCONNECT			56
#define STATE_CLOSE					57
#define STATE_GE					80
#define STATE_METERING_ERROR		100
#define STATE_WAIT					200

#define AIDON_METERINFO				0
#define AIDON_DATA					1

void MainProcedure(HANDLE codi)
{
    char  		*pData;
    METER_STAT  stat;
    CMDPARAM    param;
	BYTE		szBuffer[1024];
	CChunk		chunk;
	EUI64		id;
	int			nError, nLength;
	int			nState;
	UINT		nSuccessCount = 0;
	UINT		nFailCount = 0;

	for(nState=STATE_INIT; !m_bCodiExitPending;)
	{
		switch(nState) {
		  case STATE_INIT :
			   printf("--------------------------------\r\n");
			   printf(" Coordinator Inventory Scanning\r\n");
			   printf("--------------------------------\r\n");
			   nState = STATE_READY;
			   break;

		  case STATE_READY :
			   nError = codiGetState(codi);
			   printf("CODINATOR STATE = %d\r\n", nError);

			   if (nError != CODISTATE_NORMAL)
			   {
				   USLEEP(1000000);
				   break;
			   }
			   nState = STATE_MODULE_PARAM;
			   break;

		  case STATE_MODULE_PARAM :
			   nError = codiGetProperty(codi, CODI_CMD_MODULE_PARAM, (BYTE *)szBuffer, &nLength, 3000);
			   if (nError != CODIERR_NOERROR) ReportError(NULL, nError);
			   nState = STATE_SERIAL_PARAM;
			   break;

		  case STATE_SERIAL_PARAM :
			   nError = codiGetProperty(codi, CODI_CMD_SERIAL_PARAM, (BYTE *)szBuffer, &nLength, 3000);
			   if (nError != CODIERR_NOERROR) ReportError(NULL, nError);
			   nState = STATE_NETWORK_PARAM;
			   break;

		  case STATE_NETWORK_PARAM :
			   nError = codiGetProperty(codi, CODI_CMD_NETWORK_PARAM, (BYTE *)szBuffer, &nLength, 3000);
			   if (nError != CODIERR_NOERROR) ReportError(NULL, nError);
			   nState = STATE_SECURITY_PARAM;
			   break;

		  case STATE_SECURITY_PARAM :
			   nError = codiGetProperty(codi, CODI_CMD_SECURITY_PARAM, (BYTE *)szBuffer, &nLength, 3000);
			   if (nError != CODIERR_NOERROR) ReportError(NULL, nError);
			   nState = STATE_STACK_MEMORY;
			   break;

		  case STATE_STACK_MEMORY :
			   nError = codiGetProperty(codi, CODI_CMD_STACK_MEMORY, (BYTE *)szBuffer, &nLength, 3000);
			   if (nError != CODIERR_NOERROR) ReportError(NULL, nError);
			   nState = STATE_PERMIT;
			   break;
		
		  case STATE_PERMIT :
			   nError = codiGetProperty(codi, CODI_CMD_PERMIT, (BYTE *)szBuffer, &nLength, 3000);
			   if (nError != CODIERR_NOERROR) ReportError(NULL, nError);
			   nState = STATE_METERING;
			   break;
		
		  case STATE_METERING :
			   atoeui64(szSensorID, &id);
			   nState = STATE_GE;
			   break;
		
		  case STATE_GE :
			   printf("----------------------\r\n");
			   printf(" GE Metering Start\r\n");
			   printf("----------------------\r\n");

    		   pData = (char *)malloc(10000);
			   if (pData == NULL)
			   {
				   printf("Memory error!!!\r\n");
			   	   nState = STATE_WAIT;
				   break;
			   }

    		   memset(pData, 0, 10000);
    		   nLength = 0;

			   memset(&param, 0, sizeof(CMDPARAM));
			   memcpy(&param.id, &id, sizeof(EUI64));
			   param.codi	   = codi;
			   param.nMeteringType = METERING_TYPE_SPECIAL;
			   param.nType     = PARSE_TYPE_READ;
			   param.nTable    = 1;
			   param.nInterval = 30;
			   param.pnReply   = &nLength;
			   param.pszReply  = (char *)pData;

			   memset(&stat, 0, sizeof(METER_STAT));
			   nError = m_pOndemander->Command(const_cast<char *>("GE"), &param, &stat);
			   if (nError != 0)
			   {
				   nFailCount++;
				   XDEBUG("METERING ERROR(%d)\r\n", nError);
			   }
			   else nSuccessCount++;

			   printf("===========================================\r\n");
			   printf(" SUCCESS = %d\r\n", nSuccessCount);
			   printf(" FAIL    = %d\r\n", nFailCount);
			   printf("===========================================\r\n");

			   nState = STATE_WAIT;
			   break;

		  case STATE_METERING_ERROR :
			   printf("STATE_METERING_ERROR\r\n");
			   printf("----------------\r\n");
			   printf(" Metering Error\r\n");
			   printf("----------------\r\n");
			   nState = STATE_WAIT;
			   break;

		  case STATE_WAIT :
			   printf("STATE_WAIT\r\n");
			   USLEEP(3000000);
			   nState = STATE_READY;
			   break;
		}
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Coordinator Callback Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////

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

void codiOnError(HANDLE codi, BYTE code)
{
}

void codiOnNoResponse(HANDLE codi, BOOL bResetNow)
{
#ifdef EMBEDDED_LINUX
    PowerControl(TRUE);
#endif
}

void codiOnCommand(HANDLE codi, BYTE ctrl, BYTE type, BYTE *pBuffer, int nLength)
{
}

void codiOnSend(HANDLE codi, BYTE ctrl, BYTE type, BYTE *payload, int nLength)
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

void endiOnBypass(HANDLE codi, HANDLE endi, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
	ENDDEVICE	*pEndDevice;

	pEndDevice = (ENDDEVICE *)endi;
	m_pOndemander->OnData(&pEndDevice->id, (char *)payload, nLength);
}

void endiOnNanData(HANDLE codi, EUI64 *id, BYTE reqid, BYTE flow, BYTE tailframe, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnPush(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnAmrData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnNetworkLeave(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LEAVE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnLongRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_LONG_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnRomRead(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_ROM_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnMccbData(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_MCCB_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnMetering(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_METERING_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnEvent(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_EVENT_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnKeepAlive(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_AMR_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnCommand(HANDLE codi, HANDLE endi, BYTE ctrl, ENDI_COMMAND_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnState(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_STATE_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI, WORD shortid)
{
}

void endiOnDiscovery(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_DISCOVERY_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnUserDefine(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE type, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnMbusData(HANDLE codi, EUI64 *id, BYTE ctrl, BYTE seq, BYTE mseq, BYTE mtype, BYTE *payload, int nLength, BYTE LQI, signed char RSSI)
{
}

#if defined(__SUPPORT_DEVICE_NODE__)
void endiOnTimesync(HANDLE codi, EUI64 *id, BYTE ctrl, ENDI_TIMESYNC_PAYLOAD *payload, int nLength, BYTE LQI, signed char RSSI)
{
}
#endif

BOOL endiOnLink(HANDLE codi, EUI64 *id, BYTE type, BYTE LQI, signed char RSSI)
{
	// MCU와 PDA만 응답을 한다.
	return FALSE;
}

void endiOnRead(HANDLE codi, EUI64 *id, BYTE seq, WORD addr, int nLength, BYTE LQI, signed char RSSI)
{
}

void endiOnData(HANDLE codi, EUI64 *id, BYTE seq, BYTE *data, int nLength, BYTE LQI, signed char RSSI)
{
}

