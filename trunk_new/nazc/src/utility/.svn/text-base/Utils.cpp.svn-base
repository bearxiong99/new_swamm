
#include "common.h"

int Call(IF4Invoke *pInvoke, const char *pszCommand)
{
	int		nError;
	BYTE	nAttr;
#ifdef __DEBUG_CALL__
	int		nElapse;
	double	fElapse;
	struct  timeval tmStart, tmNow;

	fprintf(stdout, "\nCOMMAND(%s:%0d): CMD=%s, PARAM-CNT=%0d.\xd\xa",
						pInvoke->szAddress, pInvoke->port,
						pszCommand, pInvoke->nParamCount);

    gettimeofday((struct timeval *)&tmStart, NULL);
#endif
	nAttr  = IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE;
	nError = IF4API_Command(pInvoke, pszCommand, nAttr);
#ifdef __DEBUG_CALL__
    gettimeofday((struct timeval *)&tmNow, NULL);
	nElapse = GetTimeInterval(&tmStart, &tmNow);
	fElapse = (double)nElapse / (double)1000;

	fprintf(stdout, "RESPONSE: ERRCODE=%0d, RESULT-CNT=%0d, Elapse %.3f ms.\xd\xa",
						pInvoke->nError, pInvoke->nResultCount, fElapse);
#endif

	return nError;
}


