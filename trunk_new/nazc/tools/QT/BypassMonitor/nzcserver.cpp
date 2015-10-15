
#include "typedef.h"
#include "varapi.h"
#include "if4api.h"
#include "Variable.h"

#include "CommandHandler.h"
#include "nzcserver.h"

int loadVariable(void)
{
    VARAPI_Initialize(NULL, m_Root_node, TRUE);
    return IF4ERR_NOERROR;
}

int if4Init(void)
{
    IF4API_Initialize(8000, m_CommandHandler);
    //IF4API_SetUserCallback(OnDataFile, OnData, OnEvent, OnAlarm);
    return IF4ERR_NOERROR;
}
