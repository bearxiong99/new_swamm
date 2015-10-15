// SerialWriter.h: interface for the CSerialWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALWRITER_H__0693A33F_C2B1_419D_82FF_42A0DC9BD248__INCLUDED_)
#define AFX_SERIALWRITER_H__0693A33F_C2B1_419D_82FF_42A0DC9BD248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Locker.h"

#define WRITE_CHECK_TIMEOUT				500

typedef	struct	_tagWRITEREQUEST
{
		DWORD		dwWriteType;					// char, file start, file abort, file packet
		DWORD		dwSize;							// Size of buffer
		char		ch;								// Ch to send
		char		*pBuffer;						// Address of buffer to send
		HANDLE		hHeap;							// Heap containing buffer
		HWND		hWndProgress;					// Status bar window handle
		struct		_tagWRITEREQUEST	*pNext;		// Next node in the list
		struct		_tagWRITEREQUEST	*pPrev;		// Prev node in the list
} WRITEREQUEST, *PWRITEREQUEST;

class CSerialWriter  
{
// Construction/Destruction
public:
	CSerialWriter();
	virtual ~CSerialWriter();

// Attribute
public:
	virtual void SetFile(HANDLE hFile);

// I/O Functions
public:
	virtual BOOL AddSendStream(char c);
	virtual BOOL AddSendStream(LPSTR pszStream, int nLength=-1);
	virtual void RemoveAllStream();
	virtual void SetPassiveMode(BOOL bPassive);

// Member Functions
protected:
	virtual BOOL WriterAddNewNode(DWORD dwRequestType, DWORD dwSize, char ch, char *pBuffer, HANDLE hHeap, HWND hProgress=NULL);
	virtual BOOL WriterAddNewNodeTimeout(DWORD dwRequestType, DWORD dwSize, char ch, char *pBuffer, HANDLE hHeap, HWND hProgress, DWORD dwTimeout);
	virtual BOOL WriterAddFirstNodeTimeout(DWORD dwRequestType, DWORD dwSize, char ch, char *pBuffer, HANDLE hHeap, HWND hProgress, DWORD dwTimeout);
	virtual BOOL WriterAddExistingNode(WRITEREQUEST *pNode, DWORD dwRequestType, DWORD dwSize, char ch, char *pBuffer, HANDLE hHeap, HWND hProgress);
	virtual void WriterClear();

// CSerialWriter Operations
protected:
	virtual void HandleWriteRequests();
	virtual void WriterChar(WRITEREQUEST *pWrite);
	virtual void WriterBlock(WRITEREQUEST *pWrite);
	virtual DWORD WriterGeneric(char *pBuffer, DWORD dwToWrite);
	virtual void AddToLinkedList(WRITEREQUEST *pNode);
	virtual void AddToFrontOfLinkedList(WRITEREQUEST *pNode);
	virtual WRITEREQUEST *RemoveFromLinkedList(WRITEREQUEST *pNode);

protected:
	virtual void OnPrevWrite(LPSTR pszBuffer, int nLength);
	virtual void OnWrite(LPSTR pszBuffer, int nLength);

// CSerialWriter Write Procedure
protected:
	virtual DWORD WriterProc(void *pParam);

protected:
	BOOL			m_bExitWriter;
	BOOL			m_bSendReady;
	BOOL			m_bPassiveMode;
	HANDLE			m_hSendFile;
	HANDLE			m_hWriterHeap;
	HANDLE			m_hWriterEvent;
	HANDLE			m_hThreadExitEvent;
	WRITEREQUEST	*m_pWriterHead;
	WRITEREQUEST	*m_pWriterTail;
	CLocker			m_WriteLocker;
};

#endif // !defined(AFX_SERIALWRITER_H__0693A33F_C2B1_419D_82FF_42A0DC9BD248__INCLUDED_)
