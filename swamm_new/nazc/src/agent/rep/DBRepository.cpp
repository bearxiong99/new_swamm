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
    theMessageHelper.Initialize();

    theGroupHelper.Initialize();
    theMeterSecurityHelper.Initialize();

    return m_bInitialize;
}

