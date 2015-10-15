#ifndef __DLMS_COMMON_H__
#define __DLMS_COMMON_H__

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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>

#include "typedef.h"
#include "dlmsObis.h"
#include "dlmsFrame.h"
#include "CommonUtil.h"

#endif	// __DLMS_COMMON_H__

