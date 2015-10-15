////////////////////////////////////////////////////////////////////////////////
//
// ANSI C12.18 Client Library
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __ANSI_UTILS_H__
#define __ANSI_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ansiType.h"

const char *ansiGetErrorMessage(int nError);
const char *ansiGetStatus(BYTE nStatus);
const char *ansiGetProcedureName(WORD nTableType, BYTE nProc);

BYTE ansiToggle(BYTE ctrl);

// code convert
BYTE ansiUnit2AimirUnit(BYTE nUnit);
BYTE ansiChannel2AimirChannel(BYTE nIdCode, BYTE nTimeBase, BOOL bNetFlow,
        BOOL bQ1, BOOL bQ2, BOOL bQ3, BOOL bQ4);

#ifdef __cplusplus
}
#endif

#endif	// __ANSI_UTILS_H__
