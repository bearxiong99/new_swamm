#ifndef __GPIO_COMMON_H__
#define __GPIO_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#ifndef _WIN32
#include <termios.h>
#endif
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <sys/ioctl.h>
#include <sys/signal.h> 
#include <sys/select.h>
#endif
#include <sys/types.h>

#include "typedef.h"

#endif	// __GPIO_COMMON_H__
