#include <stdio.h>

#if 0
#include "sqlite/sqlite3.h"
#include "CppSQLite3.h"
#include "DatabaseHelper.h"
#endif
#include "DbUtil.h"

/** SQLite3 의 time date 형식을 TIMESTAMP 형식으로 변환해 준다.
 */
BOOL ConvTimeStamp(const char *timeString, TIMESTAMP *timeStamp)
{
    int cnt, year=0, mon=0, day=0, hour=0, min=0, sec=0;
    if(timeString == NULL || timeStamp == NULL) return FALSE;

    cnt = sscanf(timeString,"%04d-%02d-%02d %02d:%02d:%02d",
                    &year, &mon, &day, &hour, &min, &sec);
    if(cnt != 6) 
    {
        cnt = sscanf(timeString,"%04d-%02d-%02d", &year, &mon, &day);
        if(cnt != 3)
        {
            return FALSE;
        }
    }

    timeStamp->year = year; timeStamp->mon = mon;
    timeStamp->day = day; timeStamp->hour = hour;
    timeStamp->min = min; timeStamp->sec = sec;

    return TRUE;
}

/** TIMESTAMP 형식을 SQLite3 의 time date 형식으로 변환해 준다.
 */
BOOL ConvTimeString(const TIMESTAMP *timeStamp, char *timeString)
{
    if(timeString == NULL || timeStamp == NULL) return FALSE;

    sprintf(timeString, "%04d-%02d-%02d %02d:%02d:%02d",
                timeStamp->year, timeStamp->mon, timeStamp->day,
                timeStamp->hour, timeStamp->min, timeStamp->sec);

    return TRUE;
}

/** TIMESTAMP 형식을 SQLite3 의 time date 형식으로 변환해 준다.
 */
BOOL ConvDateString(const TIMESTAMP *timeStamp, char *dateString)
{
    if(dateString == NULL || timeStamp == NULL) return FALSE;

    sprintf(dateString, "%04d-%02d-%02d",
                timeStamp->year, timeStamp->mon, timeStamp->day);

    return TRUE;
}


#if 0
/** DB Transaction 시작.
 *
 * @param pTrStatus NULL이 아닐 경우 Transaction 상태를 추적할 수 있다.
 */
BOOL BeginTransaction(int *pTrStatus)
{
    int trStatus = 0;

    if(pTrStatus != NULL)
    {
        *pTrStatus = trStatus;
    }

    TRANSACTION_BEGIN;
    return TRUE;
}

BOOL EndTransaction(int *pTrStatus)
{
    int trStatus = 1;

    if(pTrStatus != NULL)
    {
        trStatus = *pTrStatus;
    }

    if(trStatus)
    {
        TRANSACTION_ROLLBACK;
        if(pTrStatus != NULL) *pTrStatus = 0;
    }
}

BOOL CommitTransaction(int *pTrStatus)
{
    int trStatus = 1;

    if(pTrStatus != NULL)
    {
        trStatus = *pTrStatus;
    }

    if(trStatus)
    {
        TRANSACTION_COMMIT;
        if(pTrStatus != NULL) *pTrStatus = 0;
    }
}
#endif


