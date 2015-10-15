
#ifndef  _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#else
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
#endif
#include <errno.h>
#ifdef _WIN32
  #include <winerror.h>
#endif

#include "typedef.h"
#include "IcmpPing.h"
#include "CommonUtil.h"
#include <fcntl.h>
#include <signal.h>

CIcmpPing::CIcmpPing()
{
	m_sSocket = -1;
}

CIcmpPing::~CIcmpPing()
{
	IcmpClose();
}

int CIcmpPing::GetSocket() const
{
	return m_sSocket;
}

int CIcmpPing::in_cksum(u_short *p, int n)
{
    register u_short answer;
    register long sum = 0;
    u_short odd_byte = 0;

    while(n > 1)
    {
        sum += *p++;
        n -= 2;
    }

    if (n == 1)
    {
        *(u_char *)(&odd_byte) = *(u_char *)p;
        sum += odd_byte;    
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return (answer);
}

int CIcmpPing::IcmpInit()
{
#ifdef _WIN32
    u_long on = 1;
#else
	int		nflag;
#endif

    m_sSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (m_sSocket >= 0)
	{
#ifdef _WIN32
        ioctlsocket(m_sSocket,FIONBIO,&on);
#else
     	nflag = fcntl(m_sSocket, F_GETFL, 0);
        fcntl(m_sSocket, F_SETFL, nflag | O_NONBLOCK);
#endif
	}
    return m_sSocket;
}

int CIcmpPing::CheckIcmp(char *dstaddr, struct timeval *atime, struct sockaddr_in *ipaddr, int nTimeout)
{
#ifdef ICMP_ECHO
    struct icmp *p, *rp;
    char buffer[1024];
    int sl, ret, hlen;
    int diff, i;
    struct sockaddr_in addr, from;
    struct ip *ip;
    struct timeval stime, etime;
    struct hostent *hentry;
    TIMETICK ltime, ltime1;

    if ((hentry = gethostbyname(dstaddr)) == NULL)
        return -1;

    GetTimeTick(&ltime);
    gettimeofday(&stime, NULL);
    memset(buffer, 0x00, 1024);
    p = (struct icmp *)buffer;
    p->icmp_type = ICMP_ECHO;
    p->icmp_code = 0;
    p->icmp_cksum = 0;
    p->icmp_seq = 15;
    p->icmp_id = getpid();
    p->icmp_dun.id_ts.its_otime = ((stime.tv_sec%100)*10000)+(stime.tv_usec / 100);
    p->icmp_cksum = in_cksum((u_short *)p, 1000);
    memcpy(buffer+sizeof(*p), (void *)&time, 4);

    memset(&from, 0, sizeof(0));
    memset(&addr, 0, sizeof(0));
    memcpy(&addr.sin_addr, hentry->h_addr, hentry->h_length);
    addr.sin_family = AF_INET;
    *ipaddr = addr;

    ret = sendto(m_sSocket, p, sizeof(*p), MSG_DONTWAIT,
                    (struct sockaddr *)&addr, sizeof(addr));
//	printf("sendto: %d bytes\r\n", ret);

    if (ret < 0)
        return -1;

    gettimeofday(&stime, NULL);
    sl = sizeof(from);
    for(i=0; i<nTimeout; i++)
    {
        ret = recvfrom(m_sSocket, buffer, 1024, 0, (struct sockaddr *)&from, (socklen_t *)&sl);
        if (ret > 0)
            break;

        gettimeofday(&etime, NULL);
        if ((etime.tv_sec - stime.tv_sec) >= 5)
            break;

        USLEEP(10000);
    }

    if (ret <= 0)
    {
//      printf("No ICMP echo from %s\xd\xa", inet_ntoa(addr.sin_addr));
        return -1;
    }

//	printf("Recv From %s\xd\xa", inet_ntoa(from.sin_addr));
    if (from.sin_addr.s_addr != addr.sin_addr.s_addr)
        return -1;

    ip = (struct ip *)buffer;
    hlen = ip->ip_hl*4;
    rp = (struct icmp *)(buffer+hlen);
    GetTimeTick(&ltime1);
    diff = GetTimeInterval(&ltime, &ltime1);
    atime->tv_sec = diff/1000;
    atime->tv_usec = diff/10;
#endif
    return 0;
}

BOOL CIcmpPing::IcmpClose()
{
	if (m_sSocket < 0)
		return FALSE;

    close(m_sSocket);
	m_sSocket = -1;
    return TRUE;
}
