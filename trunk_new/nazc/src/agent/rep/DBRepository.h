#ifndef __DBREPOSITORY_H__
#define __DBREPOSITORY_H__

#include "typedef.h"

#include "Repository.h"

#include "VariableHelper.h"
#include "MessageHelper.h"
#include "ModemHelper.h"
#include "MeterHelper.h"
#include "ChannelCfgHelper.h"
#include "MeteringHelper.h"
#include "LogHelper.h"
#include "GroupHelper.h"
#include "MeterPropertyHelper.h"

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
    CModemHelper        theModemHelper;
    CMeterHelper        theMeterHelper;
    CChannelCfgHelper   theChannelCfgHelper;
    CMeteringHelper     theMeteringHelper;
    CLogHelper          theLogHelper;
    CGroupHelper        theGroupHelper;
    CMeterPropertyHelper   theMeterPropertyHelper;
};

extern CDBRepository *m_pDBRepository;

#endif // __DBREPOSITORY_H__
