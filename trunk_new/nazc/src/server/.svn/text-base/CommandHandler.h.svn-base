#ifndef __COMMAND_HANDLER_H__
#define __COMMAND_HANDLER_H__

extern	IF4_COMMAND_TABLE	m_CommandHandler[];

// File Transfer Command
int cmdGetFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdPutFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
int cmdInstallFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// KPX Test (Issue #2503)
int cmdKpxTest(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);

// Bypass Data passing
int cmdBypassData(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt);
#endif	// __COMMAND_HANDLER_H__
