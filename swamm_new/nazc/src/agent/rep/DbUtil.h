#ifndef __DBUTILITY_H__
#define __DBUTILITY_H__

#include "typedef.h"

BOOL ConvTimeStamp(const char *timeString, TIMESTAMP *timeStamp);
BOOL ConvTimeString(const TIMESTAMP *timeStamp, char *timeString);
BOOL ConvDateString(const TIMESTAMP *timeStamp, char *dateString);

#if 0
BOOL BeginTransaction(int *pTrStatus);
BOOL EndTransaction(int *pTrStatus);
BOOL CommitTransaction(int *pTrStatus);
#endif

#endif	// __DBUTILITY_H__
