#ifndef __UNIX_EVENT_H__
#define __UNIX_EVENT_H__

#ifndef _WIN32
#include "typedef.h"

typedef struct  _tagEVENTHANDLE
{
    	BOOL            bApplied;
    	pthread_cond_t  sig_cond;
    	pthread_mutex_t sig_mutex;
}   EVENTHANDLE, *PEVENTHANDLE;

#ifdef __cplusplus
extern "C" {
#endif

HANDLE 	CreateEvent(void *pEventAttributes, BOOL bManualReset, BOOL bInitialState, LPSTR lpName);
void 	ResetEvent(HANDLE hEvent);
int 	WaitForSingleObject(HANDLE hEvent, int nTimeout);
BOOL 	CloseHandle(HANDLE hEvent);
BOOL 	SetEvent(HANDLE hEvent);

#ifdef __cplusplus
}
#endif
#endif

#endif	// __UNIX_EVENT_H__
