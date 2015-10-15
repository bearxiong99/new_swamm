#ifndef __REPOSITORY_H__
#define __REPOSITORY_H__

#include "typedef.h"

class CRepository
{
public:
	CRepository();
	virtual ~CRepository();

public:
    virtual BOOL Initialize();

protected:
    BOOL    m_bInitialize;
};

#endif // __REPOSITORY_H__
