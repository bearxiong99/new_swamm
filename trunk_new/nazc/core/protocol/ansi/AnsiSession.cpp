#include "common.h"

#include "CommonUtil.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"
#include "Chunk.h"

#include "AnsiSession.h"

#include "ansiUtils.h"

ANSISESSION * NewAnsiSession(void)
{
    ANSISESSION * pSession;

    if((pSession=(ANSISESSION *)MALLOC(sizeof(ANSISESSION))) == NULL) 
    {
        return NULL;
    }

    memset(pSession, 0, sizeof(ANSISESSION));
    pSession->pPacketChunk = new CChunk();

    return pSession;
}

void DeleteAnsiSession(ANSISESSION *pSession)
{
    if(pSession != NULL)
    {
        if(pSession->pPacketChunk != NULL)
        {
            delete(pSession->pPacketChunk);
        }
        if(pSession->pUserData != NULL)
        {
            FREE(pSession->pUserData);
            pSession->pUserData = NULL;
        }

        FREE(pSession);
    }
}

ANSISESSION * ansiToggleSendCtrl(ANSISESSION *pSession)
{
    if(pSession != NULL)
    {
        pSession->nSendSeq ++;
        pSession->nSendToggle = ansiToggle(pSession->nSendToggle);
    }

    return pSession;
}

ANSISESSION * ansiResetSession(ANSISESSION *pSession)
{
    if(pSession != NULL)
    {
        pSession->nCurrentIndex = 0;
        pSession->nCurrentRequestBytes = 0;
        pSession->nCurrentReceivedBytes = 0;
        pSession->nTotalRequestBytes = 0;
        pSession->nTotalReceivedBytes = 0;

        if(pSession->pPacketChunk != NULL)
        {
            pSession->pPacketChunk->Empty();
        }
        else
        {
            pSession->pPacketChunk = new CChunk();
        }
    }

    return pSession;
}
