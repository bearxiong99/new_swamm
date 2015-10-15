#include <net/if.h>
#if     !defined(__CYGWIN__)
#include <sys/sysinfo.h> 
#endif
#include <sys/vfs.h> 
#include <string.h> 
#include <dirent.h> 
#include <mntent.h> 
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/ioctl.h>

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

BOOL GetMemoryInfo(UINT *nTotal, UINT *nUse, UINT *nFree, UINT *nCache, UINT *nBuffer)
{ 
#if     !defined(__CYGWIN__)
	struct sysinfo 	s_sysinfo; 
#endif

	*nTotal	 = 0;
	*nUse	 = 0;
	*nFree	 = 0;
	*nCache	 = 0;
	*nBuffer = 0;

#if     !defined(__CYGWIN__)
	if (sysinfo((struct sysinfo *)(&s_sysinfo)) != 0) 
		return FALSE;
 
	*nTotal		= (unsigned long)s_sysinfo.totalram;
	*nFree		= (unsigned long)s_sysinfo.freeram;
	*nBuffer	= (unsigned long)s_sysinfo.bufferram;
	*nUse 		= (unsigned long)s_sysinfo.totalram - (unsigned long)s_sysinfo.freeram; 
//	*nShare 	= (unsigned long)s_sysinfo.sharedram;
//	*nFreeSwap	= (unsigned long)s_sysinfo.freeswap;
//	*nProcess	= (unsigned long)s_sysinfo.procs); 
#endif
	return TRUE;
} 

BOOL GetFlashInfo(const char *pszName, UINT *nTotal, UINT *nUse, UINT *nFree)
{ 
	struct mntent 	*s_mount_entry; 
	struct statfs 	s_status_fs; 
	t_mzapi_qword 	s_size[3]; 
	FILE 			*fp; 

	*nTotal	= 0;
	*nUse	= 0;
	*nFree	= 0;

	fp = setmntent("/proc/mounts", "r"); 
	if (fp == NULL)
		return FALSE;

	for(;;) 
	{ 
		s_mount_entry = getmntent(fp); 
		if (s_mount_entry == ((struct mntent *)0))
			break;
 
		if ((s_mount_entry->mnt_dir == ((char *)0)) || (s_mount_entry->mnt_fsname == ((char *)0)))
			continue;

		if (statfs((char *)s_mount_entry->mnt_dir, (struct statfs *)(&s_status_fs)) != 0)
			continue;

		if (s_status_fs.f_blocks < 0l)
			continue;

		if (strcmp(s_mount_entry->mnt_fsname, pszName) != 0)
			continue;

		s_size[0] = ((t_mzapi_qword)s_status_fs.f_blocks) * ((t_mzapi_qword)s_status_fs.f_bsize); 
		s_size[1] = ((t_mzapi_qword)(s_status_fs.f_blocks - s_status_fs.f_bfree)) * ((t_mzapi_qword)s_status_fs.f_bsize); 
		s_size[2] = ((t_mzapi_qword)s_status_fs.f_bavail) * ((t_mzapi_qword)s_status_fs.f_bsize); 

		*nTotal	= (UINT)s_size[0];
		*nUse	= (UINT)s_size[1];
		*nFree	= (UINT)s_size[2];
	};

  	endmntent(fp); 
 	return TRUE; 
}
