/** Repository
 */


#include "DBRepository.h"

CDBRepository *m_pDBRepository;

CDBRepository::CDBRepository()
{
    m_pDBRepository = this;
}

CDBRepository::~CDBRepository()
{
}

/***************************************************************************************
 * protected method
 ***************************************************************************************/
BOOL CDBRepository::Initialize()
{
    CRepository::Initialize();

    theVariableHelper.Initialize();
    theModemHelper.Initialize();
    theMessageHelper.Initialize();
    theMeterHelper.Initialize();    // Pre: ModemTbl
    theChannelCfgHelper.Initialize();
    theMeteringHelper.Initialize();
    theLogHelper.Initialize();

    theGroupHelper.Initialize();
    theMeterPropertyHelper.Initialize();

    return m_bInitialize;
}

