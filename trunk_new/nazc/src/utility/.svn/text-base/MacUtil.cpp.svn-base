
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/sockios.h>
#include <linux/if_arp.h>
#include <sys/types.h>
#include <sys/socket.h>

int usage(char * name)
{
    fprintf(stderr,"%s [-si]\n", name);
    fprintf(stderr,"  -s mac : set mac-address\n"); 
    fprintf(stderr,"  -i interface : target interface name (default:eth0)\n"); 
    exit(1);
}

int readMac(int skfd, char *ifname)
{
    static struct sockaddr *sa;
    static struct ifreq ifr;
    char *ptr;
    int i;

    strcpy(ifr.ifr_name, ifname);
    if (ioctl(skfd, SIOCGIFHWADDR, &ifr) != 0) 
    {
        perror("ioctl SIOCSIFHWADDR");
        return (-1);
    }

    sa=(struct sockaddr*)malloc(sizeof(struct sockaddr));
    memcpy((char *) sa,(char *) &(ifr.ifr_hwaddr),sizeof(struct sockaddr));
    ptr=sa->sa_data;

    for(i=0;i<6;i++,*ptr++)
    {
        printf("%02X",*ptr);
        if(i!=5)
            printf(":");
    }   

    printf("\n"); 

    return 0;
}

int writeMac(int skfd, char *ifname, char *macAddr)
{
    static struct sockaddr sa;
    static struct ifreq ifr;
    char *ptr, *saveptr, *str, *token;
    int val;
    int i;

    memset(&sa, 0, sizeof(sockaddr));
    sa.sa_family = ARPHRD_ETHER;
    ptr = sa.sa_data;

    for(i=0, str=macAddr; ; str = NULL)
    {
        token = strtok_r(str, ":", &saveptr);
        if(token == NULL) break;

        val = (int) strtol(token, (char **)NULL, 16);

        if(val > 0xFF || i >= 6) 
        {
            fprintf(stderr,"invalid address format\n");
            return -1;
        }

        ptr[i] = (unsigned char)(val & 0xFF); i++;
    }

    if(i != 6) {
        fprintf(stderr,"invalid address format\n");
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    memcpy((char *) &ifr.ifr_hwaddr, (char *) &sa, sizeof(struct sockaddr));
    if (ioctl(skfd, SIOCSIFHWADDR, &ifr) != 0) 
    {
        perror("ioctl SIOCSIFHWADDR");
        return (-1);
    }

    return 0;
}

int main(int argc, char * argv[])
{
    //struct sockaddr_in *inteface;
    //struct aftype *af;
    int skfd;
    int  opt;
    char *newmac=NULL;
    char *ifname = const_cast<char *>("eth0");

    while((opt=getopt(argc, argv, "i:s:")) != -1) {
        switch(opt) {
            case 'i': ifname =  strdup(optarg);
                break;
            case 's': newmac =  strdup(optarg);
                break;
            default :
                usage(argv[0]);
        }
    }

    if((skfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
    {
        perror("Socket");
        return (-1);
    }


    if(readMac(skfd, ifname)) { return -1; }
    if(newmac != NULL) {
        if(writeMac(skfd, ifname, newmac)) { return -1; }
        if(readMac(skfd, ifname)) { return -1; }
    }

    close(skfd);
    return (0);
}

