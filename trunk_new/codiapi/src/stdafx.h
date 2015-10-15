// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __CODIAPI_STDAFX_H__
#define __CODIAPI_STDAFX_H__

#ifdef _WIN32

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#if !defined(_QT_BUILD) && !defined(__MINGW32__)
#include <afx.h>
#include <afxwin.h>
#endif

#else	/* LINUX */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#endif	/* WIN32/LINUX */

#include "typedef.h"
#include "CommonUtil.h"

#endif	// __CODIAPI_STDAFX_H__

