#include "typedef.h"
#include "CommonUtil.h"
#include "CommandHandler.h"
#include "Variable.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////
// Command Table
//////////////////////////////////////////////////////////////////////

IF4_COMMAND_TABLE	m_CommandHandler[] =
{
	{ {111,1,0},	"cmdBypassData",            cmdBypassData },
	{ {0,0,0},		NULL,						NULL }
};

//////////////////////////////////////////////////////////////////////
// File Transfer Command
//////////////////////////////////////////////////////////////////////

int cmdBypassData(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    char        szGUID[17];
    UINT        trSeq;
    BYTE        seq;
    WORD        len;
    char *      szData;
    UINT        invokeTime;
    UINT        waitingGap;
//    TIMESTAMP   stamp;

#if 0
    XDEBUG("Param Cnt : %d\r\n", cnt);
#endif
    if(cnt < 7) return IF4ERR_INVALID_PARAM;

    EUI64ToStr((EUI64 *)pValue[0].stream.p, szGUID);
    trSeq = pValue[1].stream.u32;
    seq = pValue[2].stream.u8;
    len = pValue[3].stream.u16;
    szData = (char *)pValue[4].stream.p;
    invokeTime = pValue[5].stream.u32;
    waitingGap = pValue[6].stream.u32;

#if 0
    GETTIMESTAMP(&stamp, (time_t *)&invokeTime);
    XDEBUG("%04d/%02d/%02d %02d:%02d:%02d %s trSeq=%d seq=%d len=%d wait=%d\r\n",
            stamp.year, stamp.mon, stamp.day, stamp.hour, stamp.day, stamp.min,
            szGUID, trSeq, seq, len, waitingGap);
    XDUMP(szData, len, TRUE);
#endif
    DumpRequest("111.1", "cmdBypassData", pValue, cnt);

	return IF4ERR_NOERROR;
}
