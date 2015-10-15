#ifndef __METERPARSERFACTORY_H__
#define __METERPARSERFACTORY_H__

#include "MeterParser.h"
#include "parser/ParserAnsi.h"
#include "parser/ParserAidon.h"
#include "parser/ParserPulse.h"
#include "parser/ParserRepeater.h"
#include "parser/ParserUnknown.h"
#include "parser/ParserKamstrup.h"
#include "parser/ParserMbus.h"
#include "parser/ParserMstr711.h"
#include "parser/ParserIhd.h"
#include "parser/ParserAcd.h"
#include "parser/ParserHmu.h"
#include "parser/ParserThirdParty.h"
#include "parser/ParserDlms.h"
#include "parser/ParserFireAlarm.h"
#include "parser/ParserI210Pulse.h"
#include "parser/ParserOsaki.h"
#include "parser/ParserSx1.h"
#include "parser/ParserSxSubCore.h"
#include "parser/ParserKukdong.h"

class CMeterParserFactory
{
public:
	CMeterParserFactory();
	virtual ~CMeterParserFactory();

public:
	// Default parser 지원
	CMeterParser *SelectParser(const char *pszType);
	CMeterParser *SelectParser(const EUI64 *id);
    BOOL IsMatchedParser(CMeterParser &parser, const char *pszType);

protected:
	CMeterParser *FindParser(const char *pszType);

private:
	CANSIParser			m_ansiParser;
	CI210PulseParser	m_i210PulseParser;
	CDLMSParser     	m_dlmsParser;
	CPulseParser		m_pulseParser;

	CMbusParser	    	m_mbusParser;
	CMstr711Parser  	m_mstr711Parser;
	CAidonParser		m_aidonParser;
	CKamstrupParser		m_kamstrupParser;
	COsakiParser    	m_osakiParser;
	CSx1Parser      	m_sx1Parser;
	CSxSubCoreParser 	m_sxSubCoreParser;
    CKukdongParser      m_kukdongParser;

	CRepeaterParser		m_repeaterParser;
	CIhdParser      	m_ihdParser;
	CACDParser      	m_acdParser;
	CHMUParser      	m_hmuParser;
	CFireAlarmParser 	m_fireAlarmParser;
	CThirdPartyParser 	m_3rdPartyParser;

	CUnknownParser		m_unknownParser;
};

extern CMeterParserFactory *m_pMeterParserFactory;

#endif	// __METERPARSERFACTORY_H__
