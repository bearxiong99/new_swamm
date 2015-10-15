/** Repository
 */

#include "Repository.h"

CRepository::CRepository()
{
      m_bInitialize = FALSE;
}

CRepository::~CRepository()
{
}

/***************************************************************************************
 * protected method
 ***************************************************************************************/
BOOL CRepository::Initialize()
{
    if(!m_bInitialize) 
    {
        m_bInitialize = TRUE;
    }

    return m_bInitialize;
}

