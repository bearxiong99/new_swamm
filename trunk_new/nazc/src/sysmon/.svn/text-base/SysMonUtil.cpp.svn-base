#include <net/if.h>

#include "common.h"
#include "MemoryDebug.h"
#include "SysMonUtil.h"

typedef long long unsigned int t_mzapi_qword; 

void GetPppInfo(char *pszName, UINT *addr, UINT *subnet, UINT *gateway, BYTE *phyAddr)
{
    int     sSocket;
    int     nReqCount = 10;
    struct  ifconf s_ifconfig;
    struct  ifreq *s_ifrequest;
    struct  sockaddr_in *s_SockAddr_In;
    int     nMask;

    *pszName    = '0';
    *addr       = 0;
    *subnet     = 0;
    *gateway    = 0;
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

            if (strncmp(s_ifrequest->ifr_name, "ppp", 3) != 0)
                continue;

            strcpy(pszName, s_ifrequest->ifr_name);
            s_SockAddr_In = (struct sockaddr_in *)(&s_ifrequest->ifr_addr);
            *addr         = s_SockAddr_In->sin_addr.s_addr;
            ioctl(sSocket, SIOCGIFBRDADDR, s_ifrequest);
            s_SockAddr_In = (struct sockaddr_in *)&s_ifrequest->ifr_broadaddr;
            *gateway      = s_SockAddr_In->sin_addr.s_addr;
            ioctl(sSocket, SIOCGIFNETMASK, s_ifrequest);
            s_SockAddr_In = (struct sockaddr_in *)&s_ifrequest->ifr_netmask;
            *subnet       = s_SockAddr_In->sin_addr.s_addr;

            if (ioctl(sSocket, SIOCGIFHWADDR, s_ifrequest) == 0)
                memcpy(phyAddr, &s_ifrequest->ifr_hwaddr.sa_data, 6);

            if (strncmp(s_ifrequest->ifr_name, "ppp", 3) == 0)
                break;
        }
        break;
    }

    if (s_ifconfig.ifc_buf) FREE(s_ifconfig.ifc_buf);
    close(sSocket);
}

