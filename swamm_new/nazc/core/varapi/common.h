#ifndef __VARAPI_COMMON_H__
#define __VARAPI_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#ifndef _WIN32
  #include <sys/ioctl.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>
#else
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
  #include <winerror.h>
#endif

#include "typedef.h"
#include "varapi.h"

#endif	// __VARAPI_COMMON_H__

