//////////////////////////////////////////////////////////////////////
//
//  MeterParser.cpp: implementation of the CMeterParser class.
//
//  This file is a part of the AIMIR-CLI.
//  (c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  casir@com.ne.kr
//  http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codiapi.h"
#include "GeMeter.h"
#include "MeterParser.h"
#include "DebugUtil.h"

unsigned short BigToHostShort(unsigned short nValue);

//////////////////////////////////////////////////////////////////////
// CMeterParser Class
//////////////////////////////////////////////////////////////////////

CMeterParser::CMeterParser(int nType, const char *pszName, const char *pszDescr)
{
	m_nParserType = nType;
	strcpy(m_szParserName, pszName);
	strcpy(m_szParserDescr, pszDescr);
}

CMeterParser::~CMeterParser()
{
}

//////////////////////////////////////////////////////////////////////
// CMeterParser Operations
//////////////////////////////////////////////////////////////////////

int CMeterParser::GetType()
{
	return m_nParserType;
}

char *CMeterParser::GetName()
{
	return (char *)m_szParserName;
}

char *CMeterParser::GetDescription()
{
	return (char *)m_szParserDescr;
}

//////////////////////////////////////////////////////////////////////
// CMeterParser Overrides 
//////////////////////////////////////////////////////////////////////

void CMeterParser::AppendTable(CChunk *pChunk, const char *pszTable, char *pszFrame, int nLength)
{
	PACKET_TABLE_HEADER	table;

	if (!pChunk || !pszTable || !pszFrame || !nLength)
		return;

	if (strlen(pszTable) != 4)
		return;
	
	memset(&table, 0, sizeof(PACKET_TABLE_HEADER));
	table.kind = pszTable[0];
	sprintf(table.table, &pszTable[1]);
	table.length = BigToHostShort(nLength);

	pChunk->Add((char *)&table, sizeof(PACKET_TABLE_HEADER));
	pChunk->Add(pszFrame, nLength);
#ifdef DEBUG		
	XDEBUG("---------------- %s (Len=%d) --------------\r\n", pszTable, nLength);	
	XDUMP((char *)&table, sizeof(PACKET_TABLE_HEADER), TRUE);
	XDUMP(pszFrame, nLength, TRUE);
	XDEBUG("--------------------- %s -------------------\r\n", pszTable);
#endif
}

//////////////////////////////////////////////////////////////////////
// CMeterParser Overrides 
//////////////////////////////////////////////////////////////////////

int CMeterParser::OnCommand(CMDPARAM *pCommand, METER_STAT *pStat)
{
	return 0;
}

void CMeterParser::OnData(ONDEMANDPARAM *pOndemand, char *pszData, BYTE nLength)
{
}

