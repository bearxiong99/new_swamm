#ifndef __IF4API_COMMON_H__
#define __IF4API_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
  #include <winerror.h>
#endif
#include <sys/time.h>
#include <time.h>

#include "typedef.h"
#include "varapi.h"
#include "if4api.h"
#include "CommonUtil.h"

#endif	// __IF4API_COMMON_H__

