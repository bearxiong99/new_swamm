#ifndef __CLI_COMMON_H__
#define __CLI_COMMON_H__

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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#endif

#include "aimir.h"
#include "typedef.h"
#include "mcudef.h"
#ifndef __OPTICAL_API__
#include "clitypes.h"
#include "varapi.h"
#include "if4api.h"
#endif // __OPTICAL_API__

#include "ShellCommand.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "message.h"


extern UINT m_nAgentPort;

#endif	// __CLI_COMMON_H__

