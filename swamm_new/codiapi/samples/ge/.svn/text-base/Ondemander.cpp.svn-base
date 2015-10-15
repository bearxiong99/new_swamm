//////////////////////////////////////////////////////////////////////
//
//  Ondemander.cpp: implementation of the COndemander class.
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
#include "Ondemander.h"
#include "MeterParser.h"
#include "CommonUtil.h"
#include "DebugUtil.h"
#include "Chunk.h"
#include "Event.h"

//////////////////////////////////////////////////////////////////////
// COndemander Class
//////////////////////////////////////////////////////////////////////

COndemander	*m_pOndemander = NULL;

//////////////////////////////////////////////////////////////////////
// COndemander Class
//////////////////////////////////////////////////////////////////////

COndemander::COndemander()
{
	m_pOndemander = this;
}

COndemander::~COndemander()
{
}

//////////////////////////////////////////////////////////////////////
// COndemander attribute
//////////////////////////////////////////////////////////////////////

CMeterParser *COndemander::SelectParser(char *pszType)
{
	if (strcmp(m_i210Parser.GetName(), pszType) == 0)
		return &m_i210Parser;

	// Append other parser

	// Default GE I210 Parser
	return &m_i210Parser;
}

//////////////////////////////////////////////////////////////////////
// COndemander Operations
//////////////////////////////////////////////////////////////////////

int	COndemander::Command(char *pszType, CMDPARAM *pCommand, METER_STAT *pStat)
{
	CMeterParser	*pParser;

	// 미터의 파서를 선택한다. 현재는 I210/KV2C만 지원
	// 파서를 찾지 못하면 GE PARSER가 반환됨.

	pParser = SelectParser(pszType);
	XDEBUG("PARSER(%d): %s(%s)\r\n", pParser->GetType(), pParser->GetName(), pParser->GetDescription());

	return pParser->OnCommand(pCommand, pStat);
}

//////////////////////////////////////////////////////////////////////
// COndemander Event
//////////////////////////////////////////////////////////////////////

void COndemander::OnData(EUI64 *id, char *pszData, BYTE nLength)
{
	ONDEMANDPARAM	*pOndemand;

	LockOperation();
	pOndemand = FindOperation(id);
	if ((pOndemand != NULL) && (pOndemand->nOperation == ONDEMAND_OPR_DATA))
	{
		if (pOndemand->pParser != NULL)
		{
			CMeterParser	*pParser;

			pParser = (CMeterParser *)pOndemand->pParser;
			pParser->OnData(pOndemand, pszData, nLength);
		}
	}
	UnlockOperation();	
}

