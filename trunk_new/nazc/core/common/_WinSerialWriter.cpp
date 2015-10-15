//////////////////////////////////////////////////////////////////////
//
//	SerialWriter.cpp: implementation of the CSerialWriter class.
//
//	This file is a part of the DAS-NMS Project.
//	(c)Copyright 2003 Netcra Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	http://www.netcra.com
//	casir@com.ne.kr
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_WinSerialWriter.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// CSerialWriter Data Definition
//////////////////////////////////////////////////////////////////////

#define WRITE_CHAR					0
#define WRITE_BLOCK					1

//////////////////////////////////////////////////////////////////////
// CSerialWriter Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialWriter::CSerialWriter()
{
	DWORD		dwSize;

	m_bExitWriter		= FALSE;
	m_bSendReady		= FALSE;
	m_bPassiveMode		= FALSE;
	m_pWriterHead		= NULL;
	m_pWriterTail		= NULL;
	m_hSendFile			= NULL;

	m_hWriterEvent		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThreadExitEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);

	// Create a heap for WRITE_REQUEST packets
	m_hWriterHeap = HeapCreate(0, 4096, 40960);
	if (m_hWriterHeap == NULL)
		XDEBUG("[CSerialWriter] HeapCreate Error\n");

	dwSize				 = sizeof(WRITEREQUEST);
	m_pWriterHead		 = (WRITEREQUEST *)HeapAlloc(m_hWriterHeap, HEAP_ZERO_MEMORY, dwSize);
	m_pWriterTail		 = (WRITEREQUEST *)HeapAlloc(m_hWriterHeap, HEAP_ZERO_MEMORY, dwSize);
	m_pWriterHead->pNext = m_pWriterTail;
	m_pWriterTail->pPrev = m_pWriterHead;
}

CSerialWriter::~CSerialWriter()
{
	CloseHandle(m_hThreadExitEvent);
	CloseHandle(m_hWriterEvent);
	HeapDestroy(m_hWriterHeap);
}

//////////////////////////////////////////////////////////////////////
// CSerialWriter Functions
//////////////////////////////////////////////////////////////////////

void CSerialWriter::SetFile(HANDLE hFile)
{
	m_hSendFile	= hFile;
}

void CSerialWriter::SetPassiveMode(BOOL bPassive)
{
	m_bPassiveMode = bPassive;
}

//////////////////////////////////////////////////////////////////////
// CSerialWriter Functions
//////////////////////////////////////////////////////////////////////

BOOL CSerialWriter::AddSendStream(char c)
{
	return WriterAddNewNode(WRITE_CHAR, 0, c, NULL, m_hWriterHeap);
}

BOOL CSerialWriter::AddSendStream(LPSTR pszStream, int nLength)
{
	char	*pBuffer;

	if (!pszStream)
		return FALSE;

	if (nLength == -1)
		nLength = strlen(pszStream);

	pBuffer = (char *)malloc(nLength);
	if (pBuffer == NULL)
		return FALSE;

	memcpy(pBuffer, pszStream, nLength);
	return WriterAddNewNode(WRITE_BLOCK, nLength, 0, pBuffer, m_hWriterHeap);
}

void CSerialWriter::RemoveAllStream()
{
	WriterClear();
}

//////////////////////////////////////////////////////////////////////
// CSerialWriter Member Functions
//////////////////////////////////////////////////////////////////////

BOOL CSerialWriter::WriterAddNewNode(DWORD dwRequestType, DWORD dwSize, char ch, char *pBuffer, HANDLE hHeap, HWND hProgress)
{
	WRITEREQUEST	*pWrite;

	// Allocate new packet
	pWrite = (WRITEREQUEST *)HeapAlloc(m_hWriterHeap, 0, sizeof(WRITEREQUEST));
	if (pWrite == NULL)
		return FALSE;

	// Assign packet info
	pWrite->dwWriteType  = dwRequestType;
	pWrite->dwSize       = dwSize;
	pWrite->ch           = ch;
	pWrite->pBuffer		 = pBuffer;
	pWrite->hHeap        = hHeap;
	pWrite->hWndProgress = hProgress;

	AddToLinkedList(pWrite);
	return TRUE;
}

BOOL CSerialWriter::WriterAddNewNodeTimeout(DWORD dwRequestType, DWORD dwSize, char ch, char *pBuffer, HANDLE hHeap, HWND hProgress, DWORD dwTimeout)
{
	WRITEREQUEST	*pWrite;

	// Attempt first allocation
	pWrite = (WRITEREQUEST *)HeapAlloc(m_hWriterHeap, 0, sizeof(WRITEREQUEST));
	if (pWrite == NULL)
	{
		Sleep(dwTimeout);

		// Attempt second allocation
		pWrite = (WRITEREQUEST *)HeapAlloc(m_hWriterHeap, 0, sizeof(WRITEREQUEST));
		if (pWrite == NULL)
			return FALSE;
	}

	// Assign packet info
	pWrite->dwWriteType  = dwRequestType;
	pWrite->dwSize       = dwSize;
	pWrite->ch           = ch;
	pWrite->pBuffer		 = pBuffer;
	pWrite->hHeap        = hHeap;
	pWrite->hWndProgress = hProgress;

	AddToLinkedList(pWrite);
	return TRUE;
}

BOOL CSerialWriter::WriterAddFirstNodeTimeout(DWORD dwRequestType, DWORD dwSize, char ch, char *pBuffer, HANDLE hHeap, HWND hProgress, DWORD dwTimeout)
{
	WRITEREQUEST	*pWrite;

	// Attempt first allocation
	pWrite = (WRITEREQUEST *)HeapAlloc(m_hWriterHeap, 0, sizeof(WRITEREQUEST));
	if (pWrite == NULL)
	{
		Sleep(dwTimeout);

		// Attempt second allocation
		pWrite = (WRITEREQUEST *)HeapAlloc(m_hWriterHeap, 0, sizeof(WRITEREQUEST));
		if (pWrite == NULL)
		{
			XDEBUG("[CSerialWriter] HeapAlloc Error\n");
			return FALSE;
		}
	}

	// Assign packet info
	pWrite->dwWriteType  = dwRequestType;
	pWrite->dwSize       = dwSize;
	pWrite->ch           = ch;
	pWrite->pBuffer      = pBuffer;
	pWrite->hHeap        = hHeap;
	pWrite->hWndProgress = hProgress;

	AddToFrontOfLinkedList(pWrite);
	return TRUE;
}

BOOL CSerialWriter::WriterAddExistingNode(WRITEREQUEST *pNode, DWORD dwRequestType, DWORD dwSize, char ch, char *pBuffer, HANDLE hHeap, HWND hProgress)
{
    // Assign Packet Info
    pNode->dwWriteType  = dwRequestType;
    pNode->dwSize       = dwSize;
    pNode->ch           = ch;
    pNode->pBuffer		= pBuffer;
    pNode->hHeap        = hHeap;
    pNode->hWndProgress = hProgress;

    AddToLinkedList(pNode);
    return TRUE;
}

void CSerialWriter::WriterClear()
{
	WRITEREQUEST	*pWrite;

	m_WriteLocker.Lock();
	pWrite = m_pWriterHead->pNext;
	for(;!m_bExitWriter && (pWrite != m_pWriterTail);)
	{
		if (pWrite->pBuffer)
			free(pWrite->pBuffer);
		RemoveFromLinkedList(pWrite);

		pWrite = m_pWriterHead->pNext;
	}
	m_WriteLocker.Unlock();
}

//////////////////////////////////////////////////////////////////////
// CSerialWriter Operations
//////////////////////////////////////////////////////////////////////

void CSerialWriter::HandleWriteRequests()
{
	WRITEREQUEST	*pWrite;

	m_WriteLocker.Lock();
	pWrite = m_pWriterHead->pNext;
	m_WriteLocker.Unlock();

	for(;!m_bExitWriter && (pWrite != m_pWriterTail);)
	{
		switch(pWrite->dwWriteType) {
		  case WRITE_CHAR :
			   WriterChar(pWrite);
			   break;
		  case WRITE_BLOCK :
			   WriterBlock(pWrite);
			   break;
		}

		// Remove current node and get next node
		if (pWrite->pBuffer)
			free(pWrite->pBuffer);

		m_WriteLocker.Lock();
		RemoveFromLinkedList(pWrite);
		pWrite = m_pWriterHead->pNext;
		m_WriteLocker.Unlock();
	}
}

void CSerialWriter::WriterChar(WRITEREQUEST *pWrite)
{
	for(;!m_bExitWriter;)
	{
	    if (WriterGeneric(&(pWrite->ch), 1) > 0)
			break;

		Sleep(30);
	}
}

void CSerialWriter::WriterBlock(WRITEREQUEST *pWrite)
{   
	DWORD	dwSeek, dwWrite;

	for(dwSeek=0; !m_bExitWriter;)
	{
		dwWrite = WriterGeneric(pWrite->pBuffer+dwSeek, pWrite->dwSize-dwSeek);
		dwSeek += dwWrite;
		if (dwSeek >= pWrite->dwSize)
			break;

		XDEBUG("-----> dwSize=%d, dwSeek=%d, dwWrite=%d\r\n",
				pWrite->dwSize, dwSeek, dwWrite);
		Sleep(30);
	}
}

DWORD CSerialWriter::WriterGeneric(char *pBuffer, DWORD dwToWrite)
{
#ifndef WINCE	// WIN32
    OVERLAPPED	osWrite;
    HANDLE		arEvents[2];
    DWORD		dwWritten=0;
    DWORD		dwResult;

    // Create this writes overlapped structure hEvent
	memset(&osWrite, 0, sizeof(OVERLAPPED));
    osWrite.hEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);
    arEvents[0]		= osWrite.hEvent;
    arEvents[1]		= m_hThreadExitEvent;
    
    if (!m_bPassiveMode)
		OnPrevWrite(pBuffer, dwToWrite);

    if (WriteFile(m_hSendFile, pBuffer, dwToWrite, &dwWritten, &osWrite))
	{
        // Writefile returned immediately
        if (dwWritten != dwToWrite)
            XDEBUG("[CSerialWriter] Write timed out.\n");

	    CloseHandle(osWrite.hEvent);
		return dwWritten;
	}

	if (GetLastError() == ERROR_IO_PENDING)
	{ 
		// Write is delayed
		dwResult = WaitForMultipleObjects(2, arEvents, FALSE, INFINITE);
		switch(dwResult) {
		  case WAIT_OBJECT_0 :
			   SetLastError(ERROR_SUCCESS);
			   if (!GetOverlappedResult(m_hSendFile, &osWrite, &dwWritten, FALSE))
			   {
				   if (GetLastError() == ERROR_OPERATION_ABORTED)
						XDEBUG("[CSerialWriter] Write aborted\n");
				   else	XDEBUG("[CSerialWriter] GetOverlappedResult Error\n");
			   }

			   if (dwWritten != dwToWrite)
			   {
				   if (GetLastError() == ERROR_SUCCESS)
						XDEBUG("[CSerialWriter] Write timed out. (overlapped)\n");
				   else	XDEBUG("[CSerialWriter] Error writing data to port (overlapped)\n");
			   }
			   break;

		  case WAIT_OBJECT_0 + 1:
			   m_bExitWriter = TRUE;
			   break;

		  case WAIT_TIMEOUT :
			   XDEBUG("[CSerialWriter] Wait Timeout in WriterGeneric.\n");
			   break;
		}
	}
	else
	{
		// Writefile failed, but it isn't delayed
		XDEBUG("[CSerialWriter] WriteFile (in Writer)");
	}

    if (!m_bPassiveMode)
		OnWrite(pBuffer, dwWritten);

    CloseHandle(osWrite.hEvent);
#else	// WINCE (WINCE는 Overapped I/O가 지원되지 않는다.)
    DWORD	dwWritten;

	if (!m_bPassiveMode) OnPrevWrite(pBuffer, dwToWrite);
    WriteFile(m_hSendFile, pBuffer, dwToWrite, &dwWritten, NULL);
	if (!m_bPassiveMode) OnWrite(pBuffer, dwWritten);
#endif
	return dwWritten;
}

void CSerialWriter::AddToLinkedList(WRITEREQUEST *pNode)
{
    WRITEREQUEST	*pOldLast;

    // Add node to linked list
    m_WriteLocker.Lock();

	pOldLast			 = m_pWriterTail->pPrev;
    pNode->pNext		 = m_pWriterTail;
    pNode->pPrev		 = pOldLast;
    pOldLast->pNext		 = pNode;
    m_pWriterTail->pPrev = pNode;

    m_WriteLocker.Unlock();

    // Notify writer thread that a node has been added
    SetEvent(m_hWriterEvent);
}

void CSerialWriter::AddToFrontOfLinkedList(WRITEREQUEST *pNode)
{
    WRITEREQUEST	*pNextNode;

    // Add node to linked list
    m_WriteLocker.Lock();

    pNextNode			 = m_pWriterHead->pNext;
    pNextNode->pPrev	 = pNode;

    m_pWriterHead->pNext = pNode;
    
    pNode->pNext		 = pNextNode;
    pNode->pPrev		 = m_pWriterHead;

    m_WriteLocker.Unlock();

    // Notify writer thread that a node has been added
    SetEvent(m_hWriterEvent);
}

WRITEREQUEST *CSerialWriter::RemoveFromLinkedList(WRITEREQUEST *pNode)
{
    WRITEREQUEST	*pNextNode;
    WRITEREQUEST	*pPrevNode;

    pNextNode = pNode->pNext;
    pPrevNode = pNode->pPrev;    
    HeapFree(m_hWriterHeap, 0, pNode);
    
    pPrevNode->pNext = pNextNode;
    pNextNode->pPrev = pPrevNode;
    return pNextNode;
}

//////////////////////////////////////////////////////////////////////
// CSerialWriter Write Procedure
//////////////////////////////////////////////////////////////////////

void CSerialWriter::OnPrevWrite(LPSTR pszBuffer, int nLength)
{
}

void CSerialWriter::OnWrite(LPSTR pszBuffer, int nLength)
{
}

//////////////////////////////////////////////////////////////////////
// CSerialWriter Write Procedure
//////////////////////////////////////////////////////////////////////

DWORD CSerialWriter::WriterProc(void *pParam)
{
	HANDLE		arEvents[2];
	DWORD		dwResult;
	BOOL		bDone = FALSE;

    m_bExitWriter = FALSE;
	ResetEvent(m_hWriterEvent);
	ResetEvent(m_hThreadExitEvent);
	SetEvent(m_hWriterEvent);

	// Store Event Array
	arEvents[0] = m_hWriterEvent;
	arEvents[1] = m_hThreadExitEvent;

	m_bSendReady = TRUE;

	for(;!bDone && !m_bExitWriter;)
	{
		dwResult = WaitForMultipleObjects(2, arEvents, FALSE, WRITE_CHECK_TIMEOUT);
		switch(dwResult) {
		  case WAIT_TIMEOUT :
			   break;
		  case WAIT_FAILED :
			   XDEBUG("[CSerialWriter] WaitForMultipleObjects Error\n");
			   break;
		  case WAIT_OBJECT_0 :
			   HandleWriteRequests();
			   break;
		  case WAIT_OBJECT_0 + 1 :
			   m_bExitWriter = TRUE;
			   bDone = TRUE;
			   break;
		}
	}

	WriterClear();
	return 1;
}
