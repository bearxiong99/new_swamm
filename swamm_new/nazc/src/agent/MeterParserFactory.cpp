//////////////////////////////////////////////////////////////////////
//
//  MeterParserFacotry.cpp:  
//
//////////////////////////////////////////////////////////////////////

#include "string.h"

#include "LinkedList.h"
#include "varapi.h"
#include "EndDeviceList.h"
#include "MeterParserFactory.h"

CMeterParserFactory *m_pMeterParserFactory = NULL;

//////////////////////////////////////////////////////////////////////
// CMeterParserFactory Class
//////////////////////////////////////////////////////////////////////

CMeterParserFactory::CMeterParserFactory()
{
	m_pMeterParserFactory = this;

    /** MBUS에 MSTR711Parser를 Sub로 연결 */
    m_mbusParser.SetSubCoreParser(&m_mstr711Parser);
}

CMeterParserFactory::~CMeterParserFactory()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterParserFactory attribute
//////////////////////////////////////////////////////////////////////

BOOL CMeterParserFactory::IsMatchedParser(CMeterParser &parser, const char *pszType)
{
    int i;
    char ** nameArray = parser.GetNames();

    for(i=0; nameArray[i] != NULL; i++)
    {
	    if (strncasecmp(pszType, nameArray[i], strlen(nameArray[i])) == 0) return TRUE;
    }

    return FALSE;
}

CMeterParser *CMeterParserFactory::FindParser(const char *pszType)
{
    if(IsMatchedParser(m_ansiParser, pszType)) return &m_ansiParser;
    if(IsMatchedParser(m_aidonParser, pszType)) return &m_aidonParser;
    if(IsMatchedParser(m_pulseParser, pszType)) return &m_pulseParser;
    if(IsMatchedParser(m_repeaterParser, pszType)) return &m_repeaterParser;
    if(IsMatchedParser(m_kamstrupParser, pszType)) return &m_kamstrupParser;
    if(IsMatchedParser(m_dlmsParser, pszType)) return &m_dlmsParser;
    if(IsMatchedParser(m_ihdParser, pszType)) return &m_ihdParser;
    if(IsMatchedParser(m_mbusParser, pszType)) return &m_mbusParser;
    if(IsMatchedParser(m_mstr711Parser, pszType)) return &m_mstr711Parser;
    if(IsMatchedParser(m_acdParser, pszType)) return &m_acdParser;
    if(IsMatchedParser(m_hmuParser, pszType)) return &m_hmuParser;
	if(IsMatchedParser(m_fireAlarmParser, pszType) ) return &m_fireAlarmParser;
	if(IsMatchedParser(m_i210PulseParser, pszType)) return &m_i210PulseParser;
	if(IsMatchedParser(m_osakiParser, pszType)) return &m_osakiParser;
    if(IsMatchedParser(m_3rdPartyParser, pszType)) return &m_3rdPartyParser;
	return &m_unknownParser;
}

CMeterParser *CMeterParserFactory::SelectParser(const char *pszType)
{
	CMeterParser	*parser;

	parser = FindParser(pszType);
	return (parser != NULL) ? parser : &m_unknownParser;
}

CMeterParser *CMeterParserFactory::SelectParser(const EUI64 *id)
{
	ENDIENTRY	*pEndDevice;

    pEndDevice = m_pEndDeviceList->GetEndDeviceByID(id);
    if(pEndDevice != NULL)
    {
        return FindParser(pEndDevice->szParser);
    }

    return &m_unknownParser;
}

