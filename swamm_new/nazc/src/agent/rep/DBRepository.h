#ifndef __DBREPOSITORY_H__
#define __DBREPOSITORY_H__

#include "typedef.h"

#include "Repository.h"

#include "VariableHelper.h"
#include "MessageHelper.h"
#include "GroupHelper.h"
#include "MeterSecurityHelper.h"

class CDBRepository : CRepository
{
public:
	CDBRepository();
	virtual ~CDBRepository();

public:
    BOOL Initialize();

protected:
    CMessageHelper      theMessageHelper;
    CVariableHelper     theVariableHelper;
    CGroupHelper        theGroupHelper;
    CMeterSecurityHelper    theMeterSecurityHelper;
};

extern CDBRepository *m_pDBRepository;

#endif // __DBREPOSITORY_H__
