
#include <UnitTest++.h>

#include "common.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"


/** Test Show Commaond
 */



/** Test Show User
 *      - show user (106.5)
 */
TEST(UserList)
{
    int nError;
#ifdef __DEBUG__
    int nCount;
    int i;

    IF4Invoke * pInvoke;
    USRENTRY * pUser;
#endif
    CIF4Invoke  invoke(gServerAddr, 8000, 3);

    SKIP_TEST

    nError = Call(invoke.GetHandle(), "106.5");

    CHECK_EQUAL(IF4ERR_NOERROR, nError);
    CHECK(invoke.GetHandle()->nResultCount > 0);

#ifdef __DEBUG__
    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nCount = pInvoke->nResultCount;

    for(i=0; i<nCount; i++)
    {
        pUser = (USRENTRY *)pInvoke->pResult[i]->stream.p;
        fprintf(stdout, " %2d %-16s %d\xd\xa",
                i+1,
                pUser->usrAccount,
                pUser->usrGroup);
    }
#endif
}

/** Test Show Coordinator Info
 *
 *      - show coordinator (199.1)
 */
TEST(CoordinatorInfo)
{
    int nError;
#ifdef __DEBUG__
    char szId[32];

#endif
    int i=0, nCount=0;
    IF4Invoke * pInvoke=NULL;
    CODIENTRY * pCodi=NULL;
    CIF4Invoke  invoke(gServerAddr, 8000, 3);

    SKIP_TEST

    invoke.AddParam("3.3");
    nError = Call(invoke.GetHandle(), "199.1");

    CHECK_EQUAL(IF4ERR_NOERROR, nError);
    if(nError != IF4ERR_NOERROR) return;

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nCount = pInvoke->nResultCount;
    CHECK(nCount > 0);

    for(i=0;i<nCount;i++) {
        pCodi = (CODIENTRY *)pInvoke->pResult[i]->stream.p;
        CHECK_EQUAL(CODI_TYPE_ZIGBEE_STACK_30X, pCodi->codiType);

#ifdef __DEBUG__

        EUI64ToStr(&pCodi->codiID, szId);
        fprintf(stdout, "%2d Idx=[%2d] Type=[%2d] ID=[%-16s]\xd\xa",
            i+1,
            pCodi->codiIndex,
            pCodi->codiType,
            szId);
#endif
    }
}

