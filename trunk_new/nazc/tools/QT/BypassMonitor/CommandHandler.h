#ifndef __COMMAND_HANDLER_H__
#define __COMMAND_HANDLER_H__

#include "if4api.h"

extern	IF4_COMMAND_TABLE	m_CommandHandler[];

// Bypass Data passing
int cmdBypassData(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
#endif	// __COMMAND_HANDLER_H__
