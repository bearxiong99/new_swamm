//////////////////////////////////////////////////////////////////////
//
//	LinkedList.h: interface for the CLinkedList class.
//
//	This file is a part of the AIMIR-GCP.
//	(c)Copyright 2004 NETCRA Co., Ltd. All Rights Reserved.
//
//	This source code can only be used under the terms and conditions 
//	outlined in the accompanying license agreement.
//
//	casir@com.ne.kr
//	http://www.netcra.com
//
//////////////////////////////////////////////////////////////////////

#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include "typedef.h"

#define NODE_INVALID_LINK		-1

typedef int						POSITION;

template<class TYPE>
class CLinkedList
{
protected:
typedef struct	_tagLINKNODE
{
		int		nPrev;
		int		nNext;
		TYPE	data;
}	LINKNODE, *PLINKNODE;

public:
	CLinkedList(int nGrowSize=50);
	virtual ~CLinkedList();

	int			GetCount();
	int			GetBlockCount();
	void		GrowBlockSize();
	void		SetGrowSize(int nSize);
	POSITION	AddHead(TYPE newElement);
	POSITION	AddTail(TYPE newElement);

	void		RemoveAt(POSITION pos);
	void		RemoveAt(TYPE destElement);
	void		RemoveAll();

	TYPE		Get(POSITION pos);
	POSITION	Get(TYPE destElement);
	TYPE		GetFirst(POSITION& pos);
	TYPE		GetNext(POSITION& pos);
	TYPE		GetPrev(POSITION& pos);
	POSITION	GetHeadPosition();
	POSITION	GetTailPosition();
	TYPE		GetHead();
	TYPE		GetTail();

	POSITION	Put(TYPE newElement);
	TYPE		Get();

protected:
	int			NewNode(BOOL bAddTail=TRUE);
	void		FreeNode(int nNode);

protected:
	int			m_nNodeHead;
	int			m_nNodeTail;
	int			m_nNodeFree;
	int			m_nCount;
	LINKNODE	*m_pBlocks;
	int			m_nBlockSize;
	int			m_nGrowSize;
};

template<class TYPE>
inline CLinkedList<TYPE>::CLinkedList(int nGrowSize)
{
	m_nGrowSize		= nGrowSize;
	m_nNodeHead		= NODE_INVALID_LINK;
	m_nNodeTail		= NODE_INVALID_LINK;
	m_nNodeFree		= NODE_INVALID_LINK;
	m_nCount		= 0;
	m_pBlocks		= (LINKNODE *)NULL;
	m_nBlockSize	= 0;
}

template<class TYPE>
inline CLinkedList<TYPE>::~CLinkedList()
{
	RemoveAll();
}

template<class TYPE>
inline int CLinkedList<TYPE>::GetCount()
{
	return m_nCount;
}

template<class TYPE>
inline int CLinkedList<TYPE>::GetBlockCount()
{
	return m_nBlockSize;
}

template<class TYPE>
inline void CLinkedList<TYPE>::GrowBlockSize()
{
	LINKNODE	*pNode;
	int			i;

	m_pBlocks = (LINKNODE *)realloc(m_pBlocks, sizeof(LINKNODE)*(m_nBlockSize+m_nGrowSize));
	if (!m_pBlocks)
		return;

	for(i=m_nBlockSize; i<(m_nBlockSize+m_nGrowSize); i++)
	{
		pNode			= &m_pBlocks[i];
		pNode->nPrev	= -1;
		pNode->nNext	= m_nNodeFree;
		pNode->data		= (TYPE)NULL;
		if (m_nNodeFree != NODE_INVALID_LINK)
			m_pBlocks[m_nNodeFree].nPrev = i;
		m_nNodeFree		= i;
	}
	m_nBlockSize += m_nGrowSize;
}

template<class TYPE>
inline void CLinkedList<TYPE>::SetGrowSize(int nSize)
{
	m_nGrowSize = nSize;
}

template<class TYPE>
inline int CLinkedList<TYPE>::NewNode(BOOL bAddTail)
{
	int		nNode;

	if ((m_nCount+1) >= m_nBlockSize)
		GrowBlockSize();

	nNode		= m_nNodeFree;
	m_nNodeFree	= m_pBlocks[m_nNodeFree].nNext;
	if (m_nNodeFree != NODE_INVALID_LINK)
		m_pBlocks[m_nNodeFree].nPrev = NODE_INVALID_LINK;

	if (bAddTail)
	{
		if (m_nNodeTail != NODE_INVALID_LINK)
			m_pBlocks[m_nNodeTail].nNext = nNode;

		m_pBlocks[nNode].nPrev	= m_nNodeTail;
		m_pBlocks[nNode].nNext	= NODE_INVALID_LINK;
		m_nNodeTail	 = nNode;
	}
	else
	{
		if (m_nNodeHead != NODE_INVALID_LINK)
			m_pBlocks[m_nNodeHead].nPrev = nNode;

		m_pBlocks[nNode].nPrev	= NODE_INVALID_LINK;
		m_pBlocks[nNode].nNext	= m_nNodeHead;
		m_nNodeHead	 = nNode;
	}

	m_nNodeHead	= (m_nNodeHead == NODE_INVALID_LINK) ? nNode : m_nNodeHead;
	m_nNodeTail	= (m_nNodeTail == NODE_INVALID_LINK) ? nNode : m_nNodeTail;
	m_nCount++;
	return nNode;
}

template<class TYPE>
inline void CLinkedList<TYPE>::FreeNode(int nNode)
{
	if (nNode >= m_nBlockSize)
		return;

	if (m_pBlocks[nNode].nPrev != NODE_INVALID_LINK)
		m_pBlocks[m_pBlocks[nNode].nPrev].nNext = m_pBlocks[nNode].nNext;

	if (m_pBlocks[nNode].nNext != NODE_INVALID_LINK)
		m_pBlocks[m_pBlocks[nNode].nNext].nPrev = m_pBlocks[nNode].nPrev;

	if (nNode == m_nNodeHead)
		m_nNodeHead = m_pBlocks[nNode].nNext;
	if (nNode == m_nNodeTail)
		m_nNodeTail = m_pBlocks[nNode].nPrev;

	m_pBlocks[nNode].nPrev	= NODE_INVALID_LINK;
	m_pBlocks[nNode].nNext	= m_nNodeFree;
	m_pBlocks[nNode].data	= NULL;
	m_nNodeFree				= nNode;
	m_nCount--;
}

template<class TYPE>
inline POSITION CLinkedList<TYPE>::AddHead(TYPE newElement)
{
	int		nNode;

	nNode = NewNode(FALSE);
	m_pBlocks[nNode].data = newElement;
	return (POSITION)nNode;
}

template<class TYPE>
inline POSITION CLinkedList<TYPE>::AddTail(TYPE newElement)
{
	int		nNode;

	nNode = NewNode(TRUE);
	m_pBlocks[nNode].data = newElement;
	return (POSITION)nNode;
}

template<class TYPE>
inline void CLinkedList<TYPE>::RemoveAt(POSITION pos)
{
	FreeNode((int)pos);
}

template<class TYPE>
inline void CLinkedList<TYPE>::RemoveAt(TYPE destElement)
{
	int			nNode;

	nNode = m_nNodeHead;
	for(;nNode!=NODE_INVALID_LINK;)
	{
		if (m_pBlocks[nNode].data == destElement)
		{
			FreeNode(nNode);
			return;
		}
		nNode = m_pBlocks[nNode].nNext;
	}
}

template<class TYPE>
inline void CLinkedList<TYPE>::RemoveAll()
{
	if (m_pBlocks)
		free(m_pBlocks);

	m_nNodeHead		= NODE_INVALID_LINK;
	m_nNodeTail		= NODE_INVALID_LINK;
	m_nNodeFree		= NODE_INVALID_LINK;
	m_pBlocks		= NULL;
	m_nCount		= 0;
	m_nBlockSize	= 0;
}

template<class TYPE>
inline TYPE CLinkedList<TYPE>::Get(POSITION pos)
{
	if (m_nNodeHead == NODE_INVALID_LINK)
		return (TYPE)0;

	return m_pBlocks[(int)pos].data;
}

template<class TYPE>
inline POSITION CLinkedList<TYPE>::Get(TYPE destElement)
{
	int			nNode;

	nNode = m_nNodeHead;
	for(;nNode!=NODE_INVALID_LINK;)
	{
		if (m_pBlocks[nNode].data == destElement)
			return (POSITION)nNode;

		nNode = m_pBlocks[nNode].nNext;
	}

	return (POSITION)NODE_INVALID_LINK;
}

template<class TYPE>
inline TYPE CLinkedList<TYPE>::GetFirst(POSITION& pos)
{
	if (m_nNodeHead == NODE_INVALID_LINK)
		return (TYPE)0;

	pos = (POSITION)m_nNodeHead;
	return m_pBlocks[(int)pos].data;
}

template<class TYPE>
inline TYPE CLinkedList<TYPE>::GetNext(POSITION& pos)
{
	if (m_pBlocks[(int)pos].nNext == NODE_INVALID_LINK)
		return (TYPE)0;

	pos = (POSITION)m_pBlocks[(int)pos].nNext;
	return m_pBlocks[(int)pos].data;
}

template<class TYPE>
inline TYPE CLinkedList<TYPE>::GetPrev(POSITION& pos)
{
	if (m_pBlocks[(int)pos].nPrev == NODE_INVALID_LINK)
		return (TYPE)0;

	pos = (POSITION)m_pBlocks[(int)pos].nPrev;
	return m_pBlocks[(int)pos].data;
}

template<class TYPE>
inline POSITION CLinkedList<TYPE>::GetHeadPosition()
{
	if (m_nNodeHead == NODE_INVALID_LINK)
		return (POSITION)NODE_INVALID_LINK;

	return (POSITION)m_nNodeHead;
}

template<class TYPE>
inline POSITION CLinkedList<TYPE>::GetTailPosition()
{
	if (m_nNodeTail == NODE_INVALID_LINK)
		return (POSITION)NODE_INVALID_LINK;

	return (POSITION)m_nNodeTail;
}

template<class TYPE>
inline TYPE CLinkedList<TYPE>::GetHead()
{
	if (m_nNodeHead == NODE_INVALID_LINK)
		return NULL;

	return m_pBlocks[m_nNodeHead].data;
}

template<class TYPE>
inline TYPE CLinkedList<TYPE>::GetTail()
{
	if (m_nNodeTail == NODE_INVALID_LINK)
		return NULL;

	return m_pBlocks[m_nNodeTail].data;
}

// Use Queue

template<class TYPE>
inline POSITION CLinkedList<TYPE>::Put(TYPE newElement)
{
	return AddHead(newElement);
}

template<class TYPE>
inline TYPE CLinkedList<TYPE>::Get()
{
	TYPE	pData;

	if (m_nNodeTail == NODE_INVALID_LINK)
		return NULL;

	pData = m_pBlocks[m_nNodeTail].data;
	FreeNode(m_nNodeTail);
	return pData;
}

#endif	// __LINKEDLIST_H__
