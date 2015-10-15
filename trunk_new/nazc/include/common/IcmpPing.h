#ifndef __ICMP_PING_H__
#define __ICMP_PING_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#ifndef  _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#else
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
#endif
#include <errno.h>
#ifdef _WIN32
  #include <winerror.h>
#endif


class CIcmpPing
{
// Construction/Destruction
public:
    CIcmpPing();
    virtual ~CIcmpPing();

// Attribute
public:
	int		GetSocket() const;

// Operations
public:
	int IcmpInit();
	BOOL IcmpClose();	
	int CheckIcmp(char *dstaddr, struct timeval *atime, struct sockaddr_in *ipaddr, int nTimeout=300);

protected:
	int		in_cksum(u_short *p, int n);

protected:
	int		m_sSocket;
};

#endif	// __ICMP_PING_H__
