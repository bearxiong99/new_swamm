//////////////////////////////////////////////////////////////////////
//
//  codiapi.cpp: Coordinator Communication API
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

#include "stdafx.h"
#include "codiapi.h"
#include "Coordinator.h"
#include "CoordinatorUtility.h"
#include "CommonUtil.h"

//////////////////////////////////////////////////////////////////////
// Coordinator functions
//////////////////////////////////////////////////////////////////////

int codiInit()
{
	// Class가 생성되기를 기다린다.
	// 이부분은 장시간 블럭킹 되지는 않는다.
	for(;m_ptheCoordinator == NULL;) USLEEP(10000);

	return m_ptheCoordinator->Initialize();
}

int codiExit()
{
	return m_ptheCoordinator->Exit();
}

int codiRegister(HANDLE *codi, CODIDEVICE *pDevice)
{
	return m_ptheCoordinator->Register((COORDINATOR **)codi, pDevice);
}

int codiUnregister(HANDLE codi)
{
	return m_ptheCoordinator->Unregister((COORDINATOR *)codi);
}

int codiStartup(HANDLE codi)
{
	return m_ptheCoordinator->Startup((COORDINATOR *)codi);
}

int codiShutdown(HANDLE codi)
{
	return m_ptheCoordinator->Shutdown((COORDINATOR *)codi);
}

int codiGetProperty(HANDLE codi, int nProperty, BYTE *payload, int *nLength, int nTimeout)
{
	return m_ptheCoordinator->GetProperty((COORDINATOR *)codi, nProperty, payload, nLength, nTimeout);
}

int codiSetProperty(HANDLE codi, int nProperty, BYTE *payload, int nLength, int nTimeout)
{
	return m_ptheCoordinator->SetProperty((COORDINATOR *)codi, nProperty, payload, nLength, nTimeout);
}

int codiMulticast(HANDLE codi, BYTE type, BYTE *payload, int nLength)
{
	return m_ptheCoordinator->Multicast((COORDINATOR *)codi, type, payload, nLength);
}

int codiSync(HANDLE codi, int nInterval)
{
	return m_ptheCoordinator->Sync((COORDINATOR *)codi, nInterval);
}

int codiReset(HANDLE codi)
{
	return m_ptheCoordinator->Reset((COORDINATOR *)codi);
}

int codiBootload(HANDLE codi, int nType)
{
	return m_ptheCoordinator->Bootload((COORDINATOR *)codi, nType);
}

int codiDownload(HANDLE codi, int nModemType, const char *pszFileName, const char *pszExt)
{
	return m_ptheCoordinator->Download((COORDINATOR *)codi, nModemType, pszFileName, pszExt);
}

int codiPlayback(HANDLE codi, BYTE *frame, int nLength)
{
	return m_ptheCoordinator->Playback((COORDINATOR *)codi, frame, nLength);
}

int codiSetOption(HANDLE codi, int nOption)
{
	return m_ptheCoordinator->SetOption((COORDINATOR *)codi, nOption);
}

int codiGetOption(HANDLE codi, int *nOption)
{
	return m_ptheCoordinator->GetOption((COORDINATOR *)codi, nOption);
}

//////////////////////////////////////////////////////////////////////
// End Device functions
//////////////////////////////////////////////////////////////////////

int endiOpen(HANDLE codi, HANDLE *endi, EUI64 *id)
{
	return m_ptheCoordinator->EndiOpen((COORDINATOR *)codi, (ENDDEVICE **)endi, id);
}

int endiClose(HANDLE endi)
{
	return m_ptheCoordinator->EndiClose((ENDDEVICE *)endi);
}

HANDLE endiGetHandle(EUI64 * id)
{
	return m_ptheCoordinator->EndiGetDevice(id);
}

int endiConnect(HANDLE endi, int nTimeout, BOOL bEndDevice)
{
	return m_ptheCoordinator->EndiConnect((ENDDEVICE *)endi, 0, 0, NULL, nTimeout, bEndDevice, FALSE, bEndDevice);
}

int endiConnectWithRouteRecord(HANDLE endi, WORD shortid, BYTE hops, WORD *path, int nTimeout, BOOL bEndDevice)
{
	return m_ptheCoordinator->EndiConnect((ENDDEVICE *)endi, shortid, hops, path, nTimeout, bEndDevice, FALSE, bEndDevice);
}
 
int endiConnectDirect(HANDLE endi, WORD shortid, int nTimeout, BOOL bEndDevice)
{
	return m_ptheCoordinator->EndiConnect((ENDDEVICE *)endi, shortid, 0, NULL, nTimeout, bEndDevice, TRUE, bEndDevice);
}
 
int endiDisconnect(HANDLE endi)
{
	return m_ptheCoordinator->EndiDisconnect((ENDDEVICE *)endi);
}

int endiDisconnectDirectConnection(HANDLE endi)
{
	return m_ptheCoordinator->EndiDisconnect((ENDDEVICE *)endi);
}

int endiSend(HANDLE endi, BYTE rw, BYTE type, BYTE *payload, int nLength, int nTimeout)
{
	return m_ptheCoordinator->EndiSend((ENDDEVICE *)endi, rw, type, payload, nLength, nTimeout);
}

int endiSendRecv(HANDLE endi, BYTE read_write_req, BYTE type, BYTE *payload, int nLength, BYTE *data, int *datalen, int nTimeout)
{
	return m_ptheCoordinator->EndiSendRecv((ENDDEVICE *)endi, read_write_req, type, payload, nLength, data, datalen, nTimeout);
}

int endiGetOption(HANDLE endi, int nOption, int *nValue)
{
	return m_ptheCoordinator->EndiGetOption((ENDDEVICE *)endi, nOption, nValue);
}

int endiSetOption(HANDLE endi, int nOption, int nValue)
{
	return m_ptheCoordinator->EndiSetOption((ENDDEVICE *)endi, nOption, nValue);
}

//////////////////////////////////////////////////////////////////////
// End Device Utility functions
//////////////////////////////////////////////////////////////////////

int endiBypass(HANDLE endi, BYTE *payload, int nLength, int nTimeout)
{
	return m_ptheCoordinator->EndiBypass((ENDDEVICE *)endi, payload, nLength, nTimeout);
}

int multicastBypass(HANDLE codi, BYTE *payload, int nLength)
{
	return m_ptheCoordinator->EndiBypass((COORDINATOR *)codi, payload, nLength);
}

int endiLeave(HANDLE endi, BYTE joinTime, BYTE channel, WORD panid, int nTimeout)
{
	return m_ptheCoordinator->EndiLeave((ENDDEVICE *)endi, joinTime, channel, panid, nTimeout);
}

int endiReadAmr(HANDLE endi, BYTE *data, int *nLength, int nTimeout)
{
	return m_ptheCoordinator->EndiReadAmr((ENDDEVICE *)endi, data, nLength, nTimeout);
}

int endiWriteAmr(HANDLE endi, WORD mask, BYTE *data, int nLength, int nTimeout)
{
	return m_ptheCoordinator->EndiWriteAmr((ENDDEVICE *)endi, mask, data, nLength, nTimeout);
}

int endiReadRom(HANDLE endi, WORD addr, int nLength, BYTE *data, int *nSize, int nTimeout)
{
	return m_ptheCoordinator->EndiReadRom((ENDDEVICE *)endi, addr, nLength, data, nSize, nTimeout);
}

int endiWriteRom(HANDLE endi, WORD addr, BYTE *data, int nLength, int nTimeout)
{
	return m_ptheCoordinator->EndiWriteRom((ENDDEVICE *)endi, addr, data, nLength, nTimeout);
}

int multicastWriteRom(HANDLE codi, WORD addr, BYTE *data, int nLength)
{
	return m_ptheCoordinator->EndiWriteRom((COORDINATOR *)codi, addr, data, nLength);
}

int endiReadLongRom(HANDLE endi, UINT addr, int nLength, BYTE *data, int *nSize, int nTimeout)
{
	return m_ptheCoordinator->EndiReadLongRom((ENDDEVICE *)endi, addr, nLength, data, nSize, nTimeout);
}

int endiWriteLongRom(HANDLE endi, UINT addr, BYTE *data, int nLength, BYTE *replay, int *nSize, int nTimeout)
{
	return m_ptheCoordinator->EndiWriteLongRom((ENDDEVICE *)endi, addr, data, nLength, replay, nSize, nTimeout);
}

int multicastWriteLongRom(HANDLE codi, UINT addr, BYTE *data, int nLength)
{
	return m_ptheCoordinator->EndiWriteLongRom((COORDINATOR *)codi, addr, data, nLength);
}

int endiWriteMccb(HANDLE endi, BYTE value, BYTE *data, int nTimeout)
{
	return m_ptheCoordinator->EndiWriteMccb((ENDDEVICE *)endi, value, data, nTimeout);
}
int endiWriteCid(HANDLE endi, BYTE *value, int valuelen, BYTE *data, int *datalen, int nTimeout)
{
	return m_ptheCoordinator->EndiWriteCid((ENDDEVICE *)endi, value, valuelen, data, datalen, nTimeout);
}

int endiCommandWrite(HANDLE endi, BYTE cmd, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout)
{
	return m_ptheCoordinator->EndiCommandWrite((ENDDEVICE *)endi, cmd, data, nLength, result, nSize, nTimeout);
}

int endiCommandRead(HANDLE endi, BYTE cmd, BYTE *data, int nLength, BYTE *result, int *nSize, int nTimeout)
{
	return m_ptheCoordinator->EndiCommandRead((ENDDEVICE *)endi, cmd, data, nLength, result, nSize, nTimeout);
}

int endiBind(HANDLE codi, EUI64 *id, BYTE type, int nTimeout, BOOL bExtendedTimeout)
{
	return m_ptheCoordinator->EndiBind((COORDINATOR *)codi, id, type, nTimeout, bExtendedTimeout);
}

int endiLink(HANDLE codi, EUI64 *id, BYTE seq, BYTE type, int nTimeout)
{
	return m_ptheCoordinator->EndiLink((COORDINATOR *)codi, id, seq, type, nTimeout);
}

int endiWrite(HANDLE codi, EUI64 *id, BYTE seq, BYTE type, BYTE *data, int nLength, int nTimeout)
{
	return m_ptheCoordinator->EndiWrite((COORDINATOR *)codi, id, seq, type, data, nLength, nTimeout);
}

//////////////////////////////////////////////////////////////////////
// Utility functions
//////////////////////////////////////////////////////////////////////

BOOL codiIsInitalized()
{
	return m_ptheCoordinator->IsInitialized();
}

BOOL codiIsStarted(HANDLE codi)
{
	return m_ptheCoordinator->IsStarted((COORDINATOR *)codi);
}

BOOL codiIsSuperParent(HANDLE codi)
{
	return m_ptheCoordinator->IsSuperParent((COORDINATOR *)codi);
}

BOOL endiIsConnected(HANDLE endi)
{
	return m_ptheCoordinator->IsConnected((ENDDEVICE *)endi);
}

BOOL endiIsError(HANDLE endi)
{
	return m_ptheCoordinator->IsError((ENDDEVICE *)endi);
}

int codiGetState(HANDLE codi)
{
	return m_ptheCoordinator->GetState((COORDINATOR *)codi);
}

BOOL codiGetWatchdogState(HANDLE codi)
{
	return m_ptheCoordinator->GetWatchdogState((COORDINATOR *)codi);
}

BOOL codiSetWatchdogState(HANDLE codi, BOOL bState)
{
	return m_ptheCoordinator->SetWatchdogState((COORDINATOR *)codi, bState);
}

HANDLE codiGetHandle(HANDLE codi)
{
	return m_ptheCoordinator->GetHandle((COORDINATOR *)codi);
}

int codiGetVersion(BYTE *nMajor, BYTE *nMinor, BYTE *nRevision)
{
	return m_ptheCoordinator->GetVersion(nMajor, nMinor, nRevision);
}

const char *codiErrorMessage(int nError)
{
	return m_ptheCoordinator->ErrorMessage(nError);
}

cetime_t codiGetLastCommunicationTime(HANDLE codi)
{
	return m_ptheCoordinator->GetLastCommunicationTime((COORDINATOR *)codi);
}

int atoeui64(const char *pszString, EUI64 *id)
{
	return StrToEUI64(pszString, id);
}

int eui64toa(const EUI64 *id, char *pszString)
{
	return EUI64ToStr(id, pszString);
}

WORD ntoshort(WORD val)
{
	return BigToHostShort(val);
}

UINT ntoint(UINT val)
{
	return BigToHostInt(val);
}
