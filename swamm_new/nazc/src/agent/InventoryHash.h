#ifndef __INVENTORY_HASH_H__
#define __INVENTORY_HASH_H__

#include "Locker.h"
#include "LinkedList.h"

typedef struct	_tagINVENTORYNODE
{
		EUI64		id;						// EUI ID
		WORD		parentid;				// Parent node short-id
		WORD		shortid;				// Short ID
		BYTE		status;					// Status code 
		int			nPosition;				// 리스트 위치
}	INVENTORYNODE, *PINVENTORYNODE;

class CInventoryHash
{
public:
	CInventoryHash();
	virtual ~CInventoryHash();

// Operations
public:
    BOOL    	Initialize();
    void    	Destroy();

public:
	BOOL		Add(const EUI64 *id, WORD parentid, WORD shortid, BYTE status);
	BOOL		GetAndRemove(const EUI64 *id, WORD *parentid, WORD *shortid);
	BOOL		Remove(const EUI64 *id);
	void		RemoveAll();

private:
	INVENTORYNODE *FindNode(const EUI64 *id);

private:
	CLocker		m_Locker;
	CLinkedList<INVENTORYNODE *>	m_List;
};

extern CInventoryHash	*m_pInventoryHash;

#endif	// __INVENTORY_HASH_H__
