#ifndef __MCU_COMMON_H__
#define __MCU_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#ifndef _WIN32
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <sys/stat.h>
#include <dirent.h>

#include "aimir.h"
#include "typedef.h"
#include "version.h"
#include "config.h"
#include "if4api.h"
#include "varapi.h"
#include "mcudef.h"
#include "CommonUtil.h"

#ifdef  __SKIP_TEST__
#define  SKIP_TEST      return;
#else
#ifdef  __DEBUG__
#define  SKIP_TEST      fprintf(stdout,"[%s:%d] %s Start\n",__FILE__,__LINE__,__func__);
#else
#define  SKIP_TEST
#endif
#endif

extern char gServerAddr[64];

#endif  // __MCU_COMMON_H__

