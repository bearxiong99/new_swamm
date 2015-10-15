#include "common.h"
#include "codiapi.h"
#include "AgentService.h"
#include "InventoryHash.h"
#include "EndDeviceList.h"
#include "CommonUtil.h"
#include "Utility.h"
#include "MemoryDebug.h"
#include "DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Data definitions
//////////////////////////////////////////////////////////////////////

CInventoryHash   *m_pInventoryHash = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInventoryHash::CInventoryHash()
{
	m_pInventoryHash = this;
}

CInventoryHash::~CInventoryHash()
{
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CInventoryHash::Initialize()
{
	return TRUE;
}

void CInventoryHash::Destroy()
{
	RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CInventoryHash::Add(const EUI64 *id, WORD parentid, WORD shortid, BYTE status)
{
	INVENTORYNODE	*pNode;

	m_Locker.Lock();
	pNode = FindNode(id);
	if (pNode == NULL)
	{
		pNode = (INVENTORYNODE *)MALLOC(sizeof(INVENTORYNODE));
		if (pNode != NULL)
		{
			memset(pNode, 0, sizeof(INVENTORYNODE));
			memcpy(&pNode->id, id, sizeof(EUI64));
			pNode->shortid = shortid;
			pNode->parentid	= parentid;
			pNode->status = status;
			pNode->nPosition = (int)m_List.AddTail(pNode);
		}
	}
	else
	{
		pNode->shortid	= shortid;
		pNode->parentid	= parentid;
		pNode->status	= status;
	}
	m_Locker.Unlock();
	return pNode != NULL ? TRUE : FALSE;
}

BOOL CInventoryHash::GetAndRemove(const EUI64 *id, WORD *parentid, WORD *shortid)
{
	INVENTORYNODE	*pNode;

	m_Locker.Lock();
	pNode = FindNode(id);
	if (pNode != NULL)
	{
		*shortid  = pNode->shortid;
		*parentid = pNode->parentid;
	}
	m_Locker.Unlock();
	return pNode != NULL ? TRUE : FALSE;
}

BOOL CInventoryHash::Remove(const EUI64 *id)
{
	INVENTORYNODE	*pNode;

	m_Locker.Lock();
	pNode = FindNode(id);
	if (pNode != NULL)
	{
		m_List.RemoveAt((POSITION)pNode->nPosition);
		FREE(pNode);	
	}
	m_Locker.Unlock();
	return pNode != NULL ? TRUE : FALSE;
}

void CInventoryHash::RemoveAll()
{
	INVENTORYNODE	*pItem, *pDelete;
	POSITION		pos;

	m_Locker.Lock();
	pItem = m_List.GetFirst(pos);
	while(pItem)
	{
		pDelete = pItem;
		pItem = m_List.GetNext(pos);
		FREE(pDelete);
	}
	m_List.RemoveAll();
	m_Locker.Unlock();
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

INVENTORYNODE *CInventoryHash::FindNode(const EUI64 *id)
{
	INVENTORYNODE	*pNode;
	POSITION		pos=INITIAL_POSITION;
	
	pNode = m_List.GetFirst(pos);
	while(pNode)
	{
		if (memcmp(&pNode->id, id, sizeof(EUI64)) == 0)
			return pNode;
		pNode = m_List.GetNext(pos);
	}
	return pNode;
}

