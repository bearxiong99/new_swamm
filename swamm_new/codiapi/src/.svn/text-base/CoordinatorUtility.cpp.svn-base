////////////////////////////////////////////////////////////////////
//
//  CoordinatorUtility.cpp: implementation of the Coordinator utility functions 
//
//  This file is a part of the AIMIR.
//  (c)Copyright 2005 NURITELECOM Co., Ltd. All Rights Reserved.
//
//  This source code can only be used under the terms and conditions
//  outlined in the accompanying license agreement.
//
//  http://www.nuritelecom.com
//  casir@paran.com
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "stdafx.h"
#include "codiapi.h"
#include "CoordinatorUtility.h"
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#endif

int	STRTOKEY(char *pszString, BYTE *key, int len)
{
	int		nSeek, v;
	char	*p;

    if (!pszString)
        return CODIERR_INVALID_BUFFER;

	if (!key)
		return CODIERR_INVALID_KEY;

	memset(key, 0, len);
	for(p=pszString, nSeek=0; *p && (nSeek<len);)
	{
		if (*p == ' ')
		{
			p++;
			continue;
		}

		sscanf(p, "%x", &v);
		key[nSeek] = (v & 0xff);
		nSeek++;

		while(*p && (*p != ' ')) p++;
	}
	return CODIERR_NOERROR;
}

int	KEYTOSTR(BYTE *key, char *pszString, int len)
{
	char	tmp[5];
	int		i;

    if (!pszString)
        return CODIERR_INVALID_BUFFER;

	if (!key)
		return CODIERR_INVALID_KEY;

	*pszString = '\0';
	for(i=0; i<len; i++)
	{
		if (*pszString)
		     sprintf(tmp, " %02X", key[i] & 0xff);
		else sprintf(tmp, "%02X", key[i] & 0xff);
		strcat(pszString, tmp);
	}
	return CODIERR_NOERROR;
}

int	BINTOSTR(BYTE *key, char *pszString, int len)
{
	int		i;

    if (!pszString)
        return CODIERR_INVALID_BUFFER;

	if (!key)
		return CODIERR_INVALID_KEY;

	memset(pszString, 0, len+1);
	for(i=0; i<len; i++)
	{
		if ((key[i] >= ' ') && (key[i] <= 'z'))
			 pszString[i] = key[i];
		else pszString[i] = '.';
	}
	return CODIERR_NOERROR;
}

#if 0
TIMETICK *GetTimeTick(TIMETICK *tick)
{
	if (tick == NULL)
		return NULL;

	memset(tick, 0, sizeof(TIMETICK));
#if defined(_WIN32) 
	tick->t = GetTickCount();
#else
    gettimeofday((struct timeval *)&tick->t, NULL);
#endif
	return tick;
}

#ifndef _WIN32
int getDeclination(struct timeval *pPrev, struct timeval *pCurrent)
{
	double	fCurrent, fPrevious;
	int		nInterval;

	fCurrent    = (pCurrent->tv_sec * 1000.0) + (pCurrent->tv_usec / 1000.0);
	fPrevious	= (pPrev->tv_sec * 1000.0) + (pPrev->tv_usec / 1000.0);
	nInterval	= (int)(fCurrent - fPrevious);
	
	return nInterval;
}
#endif

unsigned int GetTickDeclination(TIMETICK *old, TIMETICK *cur)
{
	unsigned int	tick;

	if (!old || !cur)
		return 0;

#ifdef _WIN32
	tick = old->t - cur->t;
#else
	tick = (unsigned int)getDeclination(&old->t, &cur->t);
#endif
	return tick;
}
#endif

BOOL IsSequenceMessage(BYTE type)
{
	switch(type) {
	  case ENDI_DATATYPE_BYPASS :
	  case ENDI_DATATYPE_NETWORK_LEAVE :
	  case ENDI_DATATYPE_AMR :
	  case ENDI_DATATYPE_ROM_READ :
	  case ENDI_DATATYPE_MCCB :
	  case ENDI_DATATYPE_LINK :
	  case ENDI_DATATYPE_LONG_ROM_READ :
	  case ENDI_DATATYPE_COMMAND :
	  case ENDI_DATATYPE_SUPER_PARENT :
	  case ENDI_DATATYPE_MBUS :
	  case ENDI_DATATYPE_DIO :
	  case ENDI_DATATYPE_NAN :
	  case ENDI_DATATYPE_PUSH :
           return TRUE;
	  default :
           return FALSE;
	}

}
