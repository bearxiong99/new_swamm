#include "common.h"
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include <netinet/ether.h>
#include <dirent.h> 
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "logdef.h"
#include "Utility.h"
#include "ShellCommand.h"
#include "CommonUtil.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"
#include "MeterParser.h"
#include "MeterParserFactory.h"
#include "Variable.h"
#include "AgentService.h"
#include "SystemUtil.h"
#include "MobileUtility.h"

CLocker	m_SysUpTimeLocker;
CLocker	m_PatchLocker;

char * _SensorTypeName[255+1];
BYTE _SensorTypeNumber[255+1];

extern BOOL	m_bEnableCommLog;
extern int	m_nCommLogSize;
extern int  m_nMeterLogSize;
extern int  m_nUpgradeLogSize;
extern PATCHENTRY m_oPatchInformation[];
extern BYTE m_nOpMode;
#if     defined(__PATCH_4_1647__)
extern BOOL    *m_pbPatch_4_1647;
#endif
extern char	m_szMobileApn[];

int cmdMcuReset(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

void LoadSensorType()
{
    FILE    *fp;
    char    szBuffer[256];
    char    szType[256];
    int     nType;
    int     index;

    memset(_SensorTypeName, 0, sizeof(_SensorTypeName));
    memset(_SensorTypeNumber, 0, sizeof(_SensorTypeNumber));

    fp = fopen("/app/conf/sensor.type", "r");
    if (fp == NULL) {
        _SensorTypeName[0] = strdup("PULSE-BATTERY");
		return;
    }

	while(fgets(szBuffer, sizeof(szBuffer), fp) > 0)
	{
        index=-1; memset(szType,0,sizeof(szType)); nType = 0;
        if(szBuffer[0] == '#') continue;
        if(sscanf(szBuffer,"%d %s %d", &index, szType, &nType)>0) {
            if(index>=0 && index<256) {
                _SensorTypeName[index] = strdup(szType);
                _SensorTypeNumber[index] = (BYTE) nType;
            }
        }
	}
	fclose(fp);
}

/** 설정된 HW Version 정보가 있으면 읽어온다.
 *
 * @return 실패일 경우 0x00
 */
BYTE LoadHwVersion()
{
    BYTE version = 0x00;
    FILE *fp=NULL;

    if((fp=fopen("/app/conf/version.hw","r"))!=NULL)
    {
        float fVer = 0.0;

        if(fscanf(fp,"%f",&fVer)>0)
        {
            int iV = (int)fVer;
            version = (iV & 0x0F) << 4;
            iV = (int)((fVer * 10) - (iV * 10));
            version |= (iV & 0x0F);
        }
        fclose(fp);
    }

    return version;
}

char * GetSensorTypeName(BYTE type)
{
    return _SensorTypeName[type];
}

/*-- 검침데이터 포멧에서 사용되는 Service Type과의 연결 정보 --*/
BYTE GetServiceTypeBySensorType(BYTE pulseType)
{
    return _SensorTypeNumber[pulseType];
}

/*-- Model명을 이용해서 검침데이터에서 사용되는 Type 번호를 얻어온다 --*/
BYTE GetSensorTypeNumber(char * szModel) 
{
    BYTE i;

    if(!szModel) return 0;

    for(i=0;i<255;i++) {
        if(_SensorTypeName[i] && !strcasecmp(_SensorTypeName[i], szModel)) return i;
    }

    return 0;
}

UINT GetSysUpTime()
{
	UINT	nTime = 0;
	char	szBuffer[50];
	FILE	*fp;

	m_SysUpTimeLocker.Lock();
	fp = fopen("/proc/uptime", "rb");
	if (fp != NULL)
	{
		fread(szBuffer, 40, 1, fp);
		nTime = atoi(szBuffer);
		fclose(fp);
	}		
	m_SysUpTimeLocker.Unlock();	
	return nTime;
}

void untilcopy(char *dest, char *src, const char *tag)
{
    int     i, nSeek=0;

    for(i=0; i<32 && src[i]; i++)
    {
        if (strchr(tag, src[i]) != NULL)
            break;
        dest[nSeek] = src[i];
        nSeek++;
    }
    dest[nSeek] = '\0';
}

void asciicopy(char *dest, char *src, int max)
{
	int		i, n, len;
	BYTE	c;

	len = strlen(src);
	if (max == 0) max = len;
	memset(dest, 0, max);
	
	for(i=0, n=0; (i<len) && (i<max); i++)
	{
		c = src[i];
		if ((c <= ' ') || (c > 'z'))
			break;

		dest[n] = src[i];
		n++;
	}
	dest[n] = '\0';
}

BOOL GetLocalAddress(char *pszAddress)
{
    char    *pszResult, *p, *ip;

    *pszAddress = '\0';
    SystemCall("/sbin/ifconfig", "/sbin", &pszResult);
    if (pszResult != NULL)
    {
        p = strstr(pszResult, "ppp0");
        if (p == NULL)
            p = strstr(pszResult, "eth2");
        if (p == NULL)
            p = strstr(pszResult, "eth0");
        if (p == NULL)
        {
            FREE(pszResult);
            return FALSE;
        }
        ip = strstr(p, "inet addr:");
        if (ip)
            untilcopy(pszAddress, ip+10, " ");
        FREE(pszResult);
        return TRUE;
    }
	return FALSE;
}

void PutFileAddress(const char *pszFileName, BYTE *addr)
{
	char	szAddress[32];
	int		fd;

	unlink(pszFileName);
	fd = open(pszFileName, O_CREAT|O_WRONLY, 
            S_IRUSR|S_IWUSR|
            S_IRGRP|S_IWGRP|
            S_IROTH);
	if (fd <= 0)
		return;

	sprintf(szAddress, "%0d.%0d.%0d.%0d",
				addr[0] & 0xff,
				addr[1] & 0xff,
				addr[2] & 0xff,
				addr[3] & 0xff);
	write(fd, szAddress, strlen(szAddress));
	close(fd);
}

unsigned long GetDefaultGateway()
{
	FILE	*fp;
	int		r;
    char 	devname[64];
    unsigned long int d, g, m, ret=0;
    int 	flgs, ref, use, metric, mtu, win, ir;


	fp = fopen("/proc/net/route", "r");
	if (fp == NULL)
		return 0;
	
	fscanf(fp, "%*[^\n]\n");
	while(1)
	{
		r = fscanf(fp, "%63s%lx%lx%X%d%d%d%lx%d%d%d\n",
                   devname, &d, &g, &flgs, &ref, &use, &metric, &m,
                   &mtu, &win, &ir);
		if (r != 11)
			break;
		if (d == 0) ret = g;
	}	

	fclose(fp);
	return ret;
}

void GetEthernetInfo(char *pszName, UINT *addr, UINT *subnet, UINT *gateway, BYTE *phyAddr)
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
	*gateway	= 0;
	memset(phyAddr, 0, 6);

	sSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (sSocket < 0)
		return;

    memset(&s_ifconfig, 0, sizeof(struct ifconf));
    for(;;)
    {
        s_ifconfig.ifc_len = sizeof(struct ifreq) * nReqCount;
        s_ifconfig.ifc_buf = (char *)MALLOC(s_ifconfig.ifc_len);
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

	*gateway = GetDefaultGateway();

    if (s_ifconfig.ifc_buf) FREE(s_ifconfig.ifc_buf);
    close(sSocket);
}

void struntilcpy(char *dest, char *src, char c)
{
	int		i, len;

	len = strlen(src);
	for(i=0; i<len; i++)
	{
		if (*src == c)
			break;
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
}

void GetEnvInfo(char *kernel, char *gcc, char *cpu, UINT *mips)
{
	char	*p, *pszBuffer;
	int		len, fd;
	double	fMips;

	*kernel		= '\0';
	*gcc		= '\0';
	*cpu		= '\0';
	*mips		= 0;

	pszBuffer = (char *)MALLOC(2048);
	if (pszBuffer == NULL)
		return;

	fd = open("/proc/version", O_RDONLY);
	if (fd >= 0)
	{
		len = read(fd, pszBuffer, 2048);
		pszBuffer[len] = 0;
		if (len > 0)
		{
			p = strstr(pszBuffer, "Linux version ");
			if (p != NULL)
				struntilcpy(kernel, p+14, ' ');

			p = strstr(pszBuffer, "gcc version ");
			if (p != NULL)
				struntilcpy(gcc, p+12, ')');
		}
		close(fd);
	}

	fd = open("/proc/cpuinfo", O_RDONLY);
	if (fd >= 0)
	{
		len = read(fd, pszBuffer, 2048);
		pszBuffer[len] = 0;
		if (len > 0)
		{
			p = strstr(pszBuffer, "Processor");
			if (p != NULL)
			{
				p = strchr(p, ':');
				struntilcpy(cpu, p+2, '\n');
			}

			p = strstr(pszBuffer, "BogoMIPS");
			if (p != NULL)
			{
				p = strchr(p, ':');
				fMips = atof(p+2);
				*mips = (UINT)(fMips * 100);
			}
		}
		close(fd);
	}
 
	FREE(pszBuffer);
}

int GetSessionType(WORKSESSION *pSession)
{
	if (pSession == NULL)
		return 4;

	if (strcmp(pSession->szAddress, "127.0.0.1") == 0)
		return 3;

	return 1;
}

int sensorRetryCount(EUI64 *id, int nDefault)
{
	CMeterParser	*pParser;
	ENDIENTRY		*pEndDevice = NULL;
	int				nRetry;

	nRetry = nDefault;
	pEndDevice = m_pEndDeviceList->GetEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
		pParser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
		if (pParser != NULL)
			nRetry = pParser->GetRetryCount();
	}
	return nRetry > nDefault ? nRetry : nDefault;
}

int sensorConnect(HANDLE endi, int nDefaultTimeout)
{
	CMeterParser	*pParser=NULL;
	ENDIENTRY		*pEndDevice = NULL;
	int				nTimeout;
    BYTE            nAttr = 0x00;

	nTimeout = nDefaultTimeout;
	pEndDevice = m_pEndDeviceList->GetEndDeviceByID(&((ENDDEVICE *)endi)->id);
	if (pEndDevice != NULL)
	{
		XDEBUG("Parser = %s\r\n", pEndDevice->szParser);
        nAttr = pEndDevice->nAttribute;
		pParser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
		if (pParser != NULL)
			nTimeout = pParser->GetConnectionTimeout(nAttr);
        if(nTimeout < nDefaultTimeout) nTimeout = nDefaultTimeout;
	}

	XDEBUG("Timeout = %d\r\n", nTimeout);
#if     defined(__PATCH_4_1647__)
    if(*m_pbPatch_4_1647) {
        if(pEndDevice == NULL) return CODIERR_INVALID_ID;
	    return endiConnectDirect(endi, pEndDevice->shortid, nTimeout,
            (nAttr & ENDDEVICE_ATTR_RFD) ? TRUE : FALSE);
    } else {
#endif
	return endiConnect(endi, nTimeout, (nAttr & ENDDEVICE_ATTR_RFD) ? TRUE : FALSE);
#if     defined(__PATCH_4_1647__)
    }
#endif
}

int sensorCommTimeout(EUI64 *id, int nDefaultTimeout)
{
	CMeterParser	*pParser;
	ENDIENTRY		*pEndDevice = NULL;
	int				nTimeout;

	nTimeout = nDefaultTimeout;
	pEndDevice = m_pEndDeviceList->GetEndDeviceByID(id);
	if (pEndDevice != NULL)
	{
		pParser = m_pMeterParserFactory->SelectParser(pEndDevice->szParser);
		if (pParser != NULL)
			nTimeout = pParser->GetConnectionTimeout(pEndDevice->nAttribute);
	}

    return nTimeout > nDefaultTimeout ? nTimeout : nDefaultTimeout;
}


BOOL LoadPatchList()
{
	PATCHENTRY	member;
	int			fd, n;
	char		szFileName[128];
    int         i;

	memset(&member, 0, sizeof(PATCHENTRY));
    sprintf(szFileName, "/app/conf/patch.info");

    m_PatchLocker.Lock();
    fd = open(szFileName, O_RDONLY);
    if (fd > 0)
    {
        while((n=read(fd,&member,sizeof(PATCHENTRY))) == sizeof(PATCHENTRY))
        {
            for(i=0;m_oPatchInformation[i].patchID;i++) {
                if(member.patchID == m_oPatchInformation[i].patchID) {
                    memcpy(&m_oPatchInformation[i], &member, sizeof(PATCHENTRY));
                        break;
                }
            }
			memset(&member, 0, sizeof(PATCHENTRY));
		}
		close(fd);
	}
    m_PatchLocker.Unlock();
	return TRUE;
}

BOOL SavePatchList()
{
	FILE	*fp;
    int     i;
	char	szFileName[128];

    sprintf(szFileName, "/app/conf/patch.info");

	fp = fopen(szFileName, "w");
	if (fp == NULL)
		return FALSE;

    m_PatchLocker.Lock();
    for(i=0;m_oPatchInformation[i].patchID;i++) {
	    fwrite(&m_oPatchInformation[i], sizeof(PATCHENTRY), 1, fp);
    }

	fclose(fp);
    m_PatchLocker.Unlock();
	return TRUE;
}

void DisplayPatchInfo ()
{
    int     i;
    BOOL    isFirst = TRUE;

    m_PatchLocker.Lock();
    for(i=0;m_oPatchInformation[i].patchID;i++) {
        if(m_oPatchInformation[i].patchState) {
            if(isFirst) {
                printf("(Patch :");
                isFirst = FALSE;
            }
            printf(" %d-%d", m_oPatchInformation[i].patchID, m_oPatchInformation[i].patchIssueNumber);
        }
    }
    m_PatchLocker.Unlock();

    if(!isFirst) {
        printf(")\r\n");
    }
    printf("\r\n");
}

BOOL InitializeDirectories(void)
{
	mkdir("/app/member",    0777);
	mkdir("/app/data",      0777);
	mkdir("/app/value",      0777);
	mkdir("/app/log",       0777);
	mkdir("/app/event",     0777);
	mkdir("/app/conf",      0777);
	mkdir("/app/evtcfg",    0777);
	mkdir("/app/tr",        0777);
	mkdir("/app/tr/req",    0777);
	mkdir("/app/tr/res",    0777);
	mkdir("/app/tr/his",    0777);
	mkdir("/app/tr/mbr",    0777);
	mkdir("/app/tr/map",    0777);
	mkdir("/tmp/data",      0777);
	mkdir("/app/sqlite",		0777);
    return TRUE;
}

BOOL DestroyDirectories(void)
{
	SystemExec("rm -rf /app/member");
	SystemExec("rm -rf /app/data");
	SystemExec("rm -rf /app/value");
	SystemExec("rm -rf /app/log");
	SystemExec("rm -rf /app/event");
	SystemExec("rm -f /app/conf/ethernet.type");
	SystemExec("rm -f /app/conf/mobile.*");
	SystemExec("rm -f /app/conf/passwd");
	SystemExec("rm -f /app/conf/patch.info");
	SystemExec("rm -f /app/conf/server.addr");
	SystemExec("rm -f /app/conf/user.conf");
	SystemExec("rm -f /app/conf/var.*");
	SystemExec("rm -rf /app/evtcfg");
	SystemExec("rm -rf /app/tr");
	SystemExec("rm -rf /tmp/data");
    return TRUE;
}

int RouteDiscoveryControl(BOOL onoff)
{
	CODI_ROUTEDISCOVERY_PAYLOAD	discovery;
    int nError, nLength;

    nError = codiGetProperty(GetCoordinator(), CODI_CMD_ROUTE_DISCOVERY, (BYTE *)&discovery, &nLength, 3000);
    if (nError == CODIERR_NOERROR)
    {
        discovery.type = onoff;
        nError = codiSetProperty(GetCoordinator(), CODI_CMD_ROUTE_DISCOVERY, (BYTE *)&discovery, nLength, 3000);
    }
    return nError;
}

/** 주요 서비스가 Busy 인지 확인한다.
  */
BOOL IsBusyCoreService(void)
{
	// 검침이 동작중인지 검사
	if (m_pMetering->IsBusy())
	{
		XDEBUG("Service (Metering): BUSY!!!!!! (Active metering)\r\n");
		return TRUE;
	}

	// 실패검침이 동작중인지 검사
	if (m_pRecovery->IsBusy())
	{
		XDEBUG("Service (Recovery): BUSY!!!!!! (Active recovery)\r\n");
		return TRUE;
	}

	// 시간 동기화, 폴링 검침중이면 수행하지 않는다.
	if (m_pPolling->IsBusy())
	{
		XDEBUG("Service (Polling): BUSY!!!!!! (Active polling/timesync)\r\n");
		return TRUE;
	}

	// Meter Data 복구 중이면 수행하지 않는다.
	if (m_pMeterReader->IsBusy())
	{
		XDEBUG("Service (MeterReader): BUSY!!!!!! (Active meter reader)\r\n");
		return TRUE;
	}

    // OTA가 동작 중이면 수행하지 않는다.
    /*
    if (m_pOtaWorker->IsBusy())
    {
		XDEBUG("Service (OTA): BUSY!!!!!! (Active OTA)\r\n");
        return TRUE;
    }
    */

    return FALSE;
}

BOOL UpgradeConcentratorSystem(IF4Wrapper *pWrapper, BYTE UpgradeType, const char *szAddress, const char * upgradeFile)
{
    char szCommand[512];
    int nError = 0;

	mcuFirmwareUpdateEvent(UpgradeType, szAddress, upgradeFile);

    mkdir("/app/upgrade", 0777); 
	chdir("/app/upgrade/");

    switch(UpgradeType)
    {
        case CONCENTRATOR_UPGRADE_GZIP:
#if defined(__TI_AM335X__)
	        sprintf(szCommand, "/bin/gzip -dc %s > oldfirm.tar", upgradeFile); 
#else
	        sprintf(szCommand, "/usr/bin/gzip -dc %s > oldfirm.tar", upgradeFile); 
#endif
#if defined(ECHILD)
	        if((nError = SystemExec(szCommand)) != 0 && errno != ECHILD) 
#else
	        if((nError = SystemExec(szCommand)) != 0 ) 
#endif
            {
                /*-- gzip으로 풀다가 에러가 난 경우 --*/
                XDEBUG("Upgrade Error: uncompress fail %s ECODE=%d\r\n", upgradeFile, nError);
                chdir("/app"); SystemExec("/bin/rm -rf /app/upgrade");
                return FALSE;
            }
            break;
        case CONCENTRATOR_UPGRADE_DIFF:
            sprintf(szCommand, "/app/sw/bspatch /app/sw/oldfirm.tar /app/upgrade/oldfirm.tar %s", upgradeFile);
#if defined(ECHILD)
            if((nError = SystemExec(szCommand)) != 0 && errno != ECHILD)
#else
            if((nError = SystemExec(szCommand)) != 0)
#endif
            {
                /*-- merge fail --*/
                XDEBUG("Upgrade Error: merge fail ECODE=%d\r\n", nError);
                chdir("/app"); SystemExec("/bin/rm -rf /app/upgrade");
                return FALSE;
            }
            break;
        case CONCENTRATOR_UPGRADE_TAR:
	        sprintf(szCommand, "/bin/cp %s oldfirm.tar", upgradeFile);
#if defined(ECHILD)
            if((nError = SystemExec(szCommand)) != 0 && errno != ECHILD)
#else
            if((nError = SystemExec(szCommand)) != 0)
#endif
            {
                /*-- copy fail --*/
                XDEBUG("Upgrade Error: move fail ECODE=%d\r\n", nError);
                chdir("/app"); SystemExec("/bin/rm -rf /app/upgrade");
                return FALSE;
            }
            break;
        default:
            XDEBUG("Upgrade Error: invalid type TYPE=%d\r\n", UpgradeType);
            chdir("/app"); SystemExec("/bin/rm -rf /app/upgrade");
            return FALSE;

    }

	sprintf(szCommand, "/bin/tar xf oldfirm.tar");
#if defined(ECHILD)
    if((nError = SystemExec(szCommand)) != 0 && errno != ECHILD)
#else
    if((nError = SystemExec(szCommand)) != 0)
#endif
    {
        /*-- tar로 풀다가 에러가 난 경우 --*/
        XDEBUG("Upgrade Error: untar fail ECODE=%d\r\n", nError);
        chdir("/app"); SystemExec("/bin/rm -rf /app/upgrade");
        return FALSE;
    }
	SystemExec("/bin/chown -R root:root *");
	SystemExec("/bin/chmod -R 777 *");

	usleep(500000);
	unlink(upgradeFile);
	SystemExec("/bin/sync");
	usleep(100000);
	SystemExec("/bin/sync");

	printf("\xd\xaInstall Done.\xd\xa");

	cmdMcuReset(pWrapper, NULL, 0);

    chdir("/app/sw");

    return TRUE;
}

void ExecutePPPD()
{
	pid_t	pid;

	pid = FindProcess("pppd");
	if (pid > 0)
	{
		KillProcess("pppd");
		sleep(5000000);
	}

	XDEBUG("\r\nGet mobile property.\r\n");
	MOBILE_GetMobileProperty();
	XDEBUG("\r\nStarting pppd.\r\n");

    pid = fork();
    if (pid < 0)
    {
	   XDEBUG("Execute PPPD: fork error!!\r\n");
	   return;
    }
    else if (pid == 0)
    {
        setsid();
        SystemExec("/app/sw/resetRouteTable.sh");
        execl("/etc/ppp/pppd", "pppd", "call", m_szMobileApn, NULL);
        exit(0);
    }
    waitpid(pid, NULL, 0);
}

int GetPppInfo_Apn(char *apn)
{
	memset(apn, 0, 64);
	strncpy(apn, m_szMobileApn, 64);
	return strlen(apn);
}

int GetTagValue(const char *pszFileName, const char *pszTag, char *pszValue, int len)
{
	char	szBuffer[256];
	FILE	*fp;
	int		n;

	*pszValue = '\0';
	n = strlen(pszTag);

	fp = fopen(pszFileName, "rt");
	if (fp != NULL)
	{
		while(fgets(szBuffer, 255, fp))
		{
			szBuffer[255] = '\0'; 

			if (szBuffer[0] == '#')
				continue;

			if (strncmp(szBuffer, pszTag, n) == 0)
			{
				asciicopy(pszValue, szBuffer+n+1, 16);
				break;
			}
		}
		fclose(fp);
	}
	return (int)strlen(pszValue);	
}

void SetTagValue(const char *pszFileName, const char *pszTag, const char *pszValue)
{
	char	szDest[128];
	char	szBuffer[256];
	char	tmp[256], *p;
	FILE	*fp, *dp;
	int		n, len;

	n = strlen(pszTag);
	sprintf(szDest, "%s.tmp", pszFileName);

	fp = fopen(pszFileName, "rt");
	if (fp != NULL)
	{
		dp = fopen(szDest, "wb");
		if (dp == NULL)
		{
			fclose(fp);
			return;
		}

		while(fgets(szBuffer, 255, fp))
		{
			szBuffer[255] = '\0'; 
			len = strlen(szBuffer) - 1;
			szBuffer[len] = '\0';

			if ((szBuffer[0] != '#') && (strncmp(szBuffer, pszTag, n) == 0))
			{
				p = strchr(szBuffer, '#');
				if (p != NULL)
					 sprintf(tmp, "%s %s		%s\n", pszTag, pszValue, p);
				else sprintf(tmp, "%s %s\n", pszTag, pszValue);
				fwrite(tmp, strlen(tmp), 1, fp);
			}
			else
			{
				strcat(szBuffer, "\n");
				fwrite(szBuffer, strlen(szBuffer), 1, fp);
			}
		}

		fclose(fp);
		fclose(dp);
		unlink(pszFileName);
		rename(szDest, pszFileName);
	}
}

int GetPppInfo_User(char *pszUser)
{
	char	szFileName[128];
	char	apn[65];

	GetPppInfo_Apn(apn);
	sprintf(szFileName, "/app/sw/ppp/peers/%s", apn);
	return GetTagValue(szFileName, "user", pszUser, 32);
}

int GetPppInfo_Password(char *pszPassword)
{
	char	szFileName[128];
	char	apn[65];

	GetPppInfo_Apn(apn);
	sprintf(szFileName, "/app/sw/ppp/peers/%s", apn);
	return GetTagValue(szFileName, "password", pszPassword, 32);
}

void SetPppInfo_Apn(char *apn)
{
	VARAPI_UpdateValue("sysMobileAccessPointName", apn, MIN(64,strlen(apn)));
}

void SetPppInfo_User(char *pszUser)
{
	char	szFileName[128];
	char	apn[65];

	GetPppInfo_Apn(apn);
	sprintf(szFileName, "/app/sw/ppp/peers/%s", apn);
	SetTagValue(szFileName, "user", pszUser);
}

void SetPppInfo_Password(char *pszPassword)
{
	char	szFileName[128];
	char	apn[65];

	GetPppInfo_Apn(apn);
	sprintf(szFileName, "/app/sw/ppp/peers/%s", apn);
	SetTagValue(szFileName, "password", pszPassword);
}

BOOL IsValidEUI64(OID3 *id)
{
    if(!id) return FALSE;
    if ((VARAPI_OidCompare(id, "1.14") == 0) ||
        (VARAPI_OidCompare(id, "4.1.1") == 0) ||
        (VARAPI_OidCompare(id, "4.2.3") == 0) ||
        (VARAPI_OidCompare(id, "4.3.1") == 0) ||
        (VARAPI_OidCompare(id, "4.4.1") == 0))
        return TRUE;
    return FALSE;
}

/** Issue #10 : Interval Scheduler에서 사용될 수 있는 값을
  * 검사하고 만약 틀린 값이라면 기본 값(60)을 반환한다.
  */
UINT GetValidIntervalSchedulerValue(UINT nMin)
{
    switch(nMin)
    {
        case 5:
        case 6:
        case 10:
        case 12:
        case 15:
        case 20:
        case 30:
            return nMin;

        //case 60:
        default:
            return 60;
    }
}
