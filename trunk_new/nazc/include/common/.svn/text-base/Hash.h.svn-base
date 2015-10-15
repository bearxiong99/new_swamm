#ifndef __HASH_H__
#define __HASH_H__

#include "typedef.h"
#include "Locker.h"

#define MAX_HASH_KEY_SIZE				8
#define DEFAULT_HASH_GROW_SIZE			50

typedef struct	_tagHASHENTRY
{
		BYTE	key[MAX_HASH_KEY_SIZE];			// Key 값
        BYTE    keyLen;                         // Key Length
		int		index;							// Data 값
}	HASHENTRY, *PHASHENTRY;

class CHash
{
public:
	CHash(int nGrow=DEFAULT_HASH_GROW_SIZE);
	virtual ~CHash();

public:
	int			GetCount();

public:
	BOOL		Insert(const void *key, BYTE nKeyLen, int nIndex);
	void		Delete(const void *key, BYTE nKeyLen);
	void		DeleteAll();

	BOOL		Update(const void *key, BYTE nKeyLen, int nIndex);
	int			Find(const void *key, BYTE nKeyLen);

public:
	static int 	SortCompare(const void *p1, const void *p2);
	static int	SearchCompare(const void *p1, const void *p2);	
		
protected:
	BOOL		Grow();
	void		Sort();
	HASHENTRY	*Search(const void *key, BYTE nKeyLen);

private:
	int			m_nGrow;
	int			m_nAllocation;
	int			m_nCount;
	HASHENTRY	*m_pList;
	CLocker		m_Locker;
};

#endif	// __HASH_H__

