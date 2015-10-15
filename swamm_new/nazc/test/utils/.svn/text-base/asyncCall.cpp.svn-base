
/** Nuritelecom AiMiR Project.
 *
 *  Nuri AiMiR Concentrator Asynchronus Handler Call Test.
 *
 */

#include "common.h"
#include "DebugUtil.h"

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
int IF4_Initialize_Interface(int port)
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
int IF4_Terminate_Interface(void)
{
    VARAPI_Destroy();

    return 0;
}


/*! IF4API Asynchronus Command Call
 * 
 *
 * @author rewriter@nuritelecom.com
 *
 * @param ipaddr 서버 주소
 * @param port   포트 번호
 *
 *
 */
int AsynchronousCall(char * ipaddr, int port)
{
    CIF4Invoke  invoke(ipaddr, port, 3);
    IF4Invoke * pInvoke = NULL;
    BYTE        nAttr;
    int         nError;

    nAttr  = IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE;

    /*
    invoke.AddParam("1.4", (BYTE) valueA);
    invoke.AddParam("1.6", (UINT) valueB);
    */

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nError = IF4API_Command(pInvoke, "102.40", nAttr);

    if(nError == IF4ERR_NOERROR) {
        IF4API_Normalize(pInvoke->pResult, pInvoke->nResultCount);
    }

    return nError;
}


void Usage(char * filename)
{
	printf("Usage: %s [-dps]\n", filename);
	printf("         -d      Enable Deubgging Messages\n");
	printf("         -p      Server Port (Default 8000)\n");
	printf("         -s      Server Ip Address (Default 127.0.0.1)\n");
	printf("\n");
}

int main(int argc, char ** argv)
{
    int opt;
    char * ipaddr = "127.0.0.1";
    int port=8000;
    int debug=1;
    int nError;

    while((opt=getopt(argc, argv, "s:p:d")) != -1) {
        switch(opt) {
            case 'd': 
                debug=0;
                break;
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

    SET_DEBUG_MODE(debug);
    IF4_Initialize_Interface(port);

    nError = AsynchronousCall(ipaddr,port);

    printf("%d %s\n", nError, IF4API_GetErrorMessage(nError));

    IF4_Terminate_Interface();

	return 0;
}
