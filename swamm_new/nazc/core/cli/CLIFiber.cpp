//////////////////////////////////////////////////////////////////////
//
//  CLIFiber.cpp: implementation of the CCLIFiber class.
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
#include "CLIFiber.h"
#include "CLIUtility.h"

//////////////////////////////////////////////////////////////////////
// CCLIFiber Class
//////////////////////////////////////////////////////////////////////

CCLIFiber::CCLIFiber()
{
	m_pProtocol = NULL;
}

CCLIFiber::~CCLIFiber()
{
}

//////////////////////////////////////////////////////////////////////
// CCLIFiber Functions
//////////////////////////////////////////////////////////////////////

void CCLIFiber::SetInstance(CLIPROTOCOL *pProtocol)
{
	m_pProtocol = pProtocol;
}

BOOL CCLIFiber::Start()
{
	return TRUE;
}

void CCLIFiber::Stop()
{
}

BOOL CCLIFiber::Write(CLISESSION *pSession, char *pszBuffer, int nLength)
{
	return TRUE;
}

