
/** Nuritelecom AiMiR Project.
 *
 *  Nuri Aimir Concentrator External Interface Library.
 *
 */

#include "common.h"
#include "DebugUtil.h"

//void GETTIMESTAMP(TIMESTAMP *pStamp, time_t *pNow);

/** IF4API 초기화.
 * IF4API를 이용하기 위해서는 반드시 초기화 절차를 거쳐야 한다.
 *
 * @author rewriter@nuritelecom.com
 * 
 * @return Error Error Code
 *
 * @retval 0 정상 종료
 * @retval -1 Internal Error
 */
int IF4Client_Initialize_Interface(int port)
{
    if(VARAPI_Initialize(NULL, m_Root_node, FALSE)) return -1;

    return 0;
}

/** IF4API 종료.
 * 종료 시 반드 시 Terminate 절차를 거친다 
 *
 * @author rewriter@nuritelecom.com
 * 
 * @return Error Error Code
 *
 * @retval 0 정상 종료
 */
int IF4Client_Terminate_Interface(void)
{
    VARAPI_Destroy();

    return 0;
}


/*! IF4API Command Call
 * 
 * IF4API에서 제공하는 Command Call 예제.
 *
 * 임의의 Command cmdSum(130.1)을 정의한다.
 *
 * cmdSum(130.1) : 임의의 양의 정수 (값의 범위는 0 ~ 10) 두개를 받아
 *                 두수를 더한값을 리턴한다.
 *   - In Parameter : 
 *     -# valueA :BYTE (1.4)
 *     -# valueB :UINT (1.6)
 *   - Out Parameter :
 *     -# sumAB : UINT (1.6)
 *   - Error Code :
 *     -# IF4ERR_NOERROR : None Error
 *     -# IF4ERR_INVALID_PARAM : valueA가 유효값 범위를 넘을 때
 *     -# IF4ERR_INVALID_PARAM : valueB가 유효값 범위를 넘을 때
 * 
 * @author rewriter@nuritelecom.com
 *
 * @param ipaddr 서버 주소
 * @param port   포트 번호
 * @param valueA 첫번째 인자 
 * @param valueB 두번째 인자
 *
 * @return Sum 두수를 더한값 
 *
 * @retval 0=< 두수의 덧셈값
 * @retval 0>  에러코드
 *
 * @warning IF4API에 대한 예제
 *
 */
int IF4APIClient_Sample_Sum(char * ipaddr, int port, unsigned int valueA, unsigned int valueB)
{
    CIF4Invoke  invoke(ipaddr, port, 3);
    IF4Invoke * pInvoke = NULL;
    BYTE        nAttr;
    int         nError;

    nAttr  = IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE;

    invoke.AddParam("1.4", (BYTE) valueA);
    invoke.AddParam("1.6", (UINT) valueB);
    invoke.AddParam("1.11",
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
    );
    invoke.AddParam("1.11",
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
    );
    invoke.AddParam("1.11",
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
        "01234567890123456789012345678901234567890123456789"
    );
    invoke.AddParam("1.11",
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
    );

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nError = IF4API_Command(pInvoke, "130.1", nAttr);

    if(nError != IF4ERR_NOERROR) return -nError;

    return (int) pInvoke->pResult[0]->stream.u32;
}

BOOL SendEvent(char *szOid, CIF4Invoke *pInvoke)
{
    char    *pszAddress;
    int     i, nPort, nError = 0;
    TIMESTAMP tmEvent;

    if (pInvoke == NULL)
        return TRUE;

    GetTimestamp(&tmEvent, NULL);

    for(i=0; i<3; i++)
    {
        pszAddress = pInvoke->GetAddress();
        nPort      = pInvoke->GetPort();

        XDEBUG("EVENT(%s) Send to %s:%d (try=%d).\xd\xa",
            szOid, pszAddress ? pszAddress : "", nPort, i+1);

        try
        {
            nError = pInvoke->Event(szOid, IF4_SRC_MCU, NULL, &tmEvent);
        }
        catch(...)
        {
            XDEBUG("***** Event(%s) Sending Exception.\xd\xa", szOid);
            nError = IF4ERR_SYSTEM_ERROR;
            break;
        }
        XDEBUG("EVENT(%s) Sending %s.\xd\xa", szOid, nError == IF4ERR_NOERROR ? "Done" : "Fail");

        if (nError == IF4ERR_NOERROR)
            break;
    }
    return (nError == IF4ERR_NOERROR) ? TRUE : FALSE;
}


void if4event()
{
    char * ipaddr = "127.0.0.1";
    int port=8000;


    SET_DEBUG_MODE(0);
    IF4Client_Initialize_Interface(port);

    CIF4Invoke  invoke(ipaddr, port, 3);
    invoke.AddParam("1.4", (BYTE) 10);
    invoke.AddParam("1.6", (UINT) 69);
    invoke.AddParam("1.11",
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
    );

    SendEvent("240.240",&invoke);

    IF4Client_Terminate_Interface();
}

int if4timestamp(char * ipaddr, int port)
{
    TIMESTAMP tmEvent;
    IF4Invoke * pInvoke = NULL;
    int         nError,i;

    SET_DEBUG_MODE(0);
    CIF4Invoke  invoke(ipaddr, port, 3);

    GetTimestamp(&tmEvent, NULL);

    invoke.AddParamFormat("1.16", VARSMI_TIMESTAMP, &tmEvent, sizeof(TIMESTAMP));

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nError = IF4API_Command(pInvoke, "130.4", IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE);

    if(nError != IF4ERR_NOERROR) return -nError;

    for(i=0;i< pInvoke->nResultCount; i++)
    {
        memcpy(&tmEvent, pInvoke->pResult[i]->stream.p, sizeof(TIMESTAMP));
        XDEBUG("TIMESTAMP(C): %04d/%02d/%02d %02d:%02d:%02d\n", 
                tmEvent.year, tmEvent.mon, tmEvent.day, tmEvent.hour, tmEvent.min, tmEvent.sec);
    }

    return nError;
}


void Usage(char * filename)
{
	printf("Usage: %s [-ps]\n", filename);
	printf("         -p      Server Port (Default 8000)\n");
	printf("         -s      Server Ip Address (Default 127.0.0.1)\n");
	printf("\n");
}

int main(int argc, char ** argv)
{
    int opt;
    char * ipaddr = "127.0.0.1";
    int port=8000;

    while((opt=getopt(argc, argv, "s:p:")) != -1) {
        switch(opt) {
            case 'p': 
                port = (int)strtol(optarg,(char **)NULL,10);
                break;
            case 's': 
                ipaddr = strdup(optarg);
                break;
            case 'h':
            default :
                Usage(argv[0]);
                return FALSE;
        }
    }

    SET_DEBUG_MODE(0);
    IF4Client_Initialize_Interface(port);

    printf("1+1=%d\n",IF4APIClient_Sample_Sum(ipaddr, port, 1, 1));
    printf("1+3=%d\n",IF4APIClient_Sample_Sum(ipaddr, port, 1, 3));
    printf("1+10=%d\n",IF4APIClient_Sample_Sum(ipaddr, port, 1, 10));
    printf("1+11=%d\n",IF4APIClient_Sample_Sum(ipaddr, port, 1, 11));
    printf("11+1=%d\n",IF4APIClient_Sample_Sum(ipaddr, port, 11, 1));
    printf("timestamp=%d\n",if4timestamp(ipaddr, port));

    IF4Client_Terminate_Interface();

	return 0;
}
