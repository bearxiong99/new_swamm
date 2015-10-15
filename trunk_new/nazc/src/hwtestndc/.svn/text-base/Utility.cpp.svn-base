
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#include "typedef.h"

extern bool g_bFakeTest;
extern int g_nFakeRatio;

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

			s_SockAddr_In = (struct sockaddr_in *)(&s_ifrequest->ifr_addr);
			*addr 		  = s_SockAddr_In->sin_addr.s_addr;
			ioctl(sSocket, SIOCGIFNETMASK, s_ifrequest);
			s_SockAddr_In = (struct sockaddr_in *)&s_ifrequest->ifr_netmask;
			*subnet		  = s_SockAddr_In->sin_addr.s_addr;

			if (ioctl(sSocket, SIOCGIFHWADDR, s_ifrequest) == 0)
                memcpy(phyAddr, &s_ifrequest->ifr_hwaddr.sa_data, 6);

			if (strcasecmp(s_ifrequest->ifr_name, pszName) == 0)
				break;
		}
        break;
    }

	*gateway = GetDefaultGateway();

    if (s_ifconfig.ifc_buf) free(s_ifconfig.ifc_buf);
    close(sSocket);
}

bool isFake()
{
    return g_bFakeTest;
}

/** Fake Fail을 수행할 지 여부를 판다.
 */
bool isFakeFail()
{
    return (random() % 10000 < g_nFakeRatio);
}
