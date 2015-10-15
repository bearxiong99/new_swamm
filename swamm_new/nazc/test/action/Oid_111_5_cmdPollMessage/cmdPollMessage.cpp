
/** Nuritelecom AiMiR Project.
 *
 *  Nuri Aimir Concentrator External Interface Library.
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


int cmdPollMessage(const char * pIpAddr, int nPort, const char * pTargetId)
{
    IF4Invoke * pInvoke = NULL;
    EUI64       id;
    int         nError, i, len;
    char        pszData[256];
    TIMESTAMP   issueTime;

    printf("%s %d\n", pIpAddr, nPort);
    CIF4Invoke  invoke(pIpAddr, nPort, 3);

    StrToEUI64(pTargetId, &id);

    invoke.AddParamFormat("1.14", VARSMI_EUI64, &id, sizeof(EUI64));

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nError = IF4API_Command(pInvoke, "111.5", IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE);

    if(nError != IF4ERR_NOERROR) 
    {
        fprintf(stderr, "IF4Error %s\n", IF4API_GetErrorMessage(nError));
    } else {
        memset(pszData, 0, sizeof(pszData));
	    for(i=0; i<pInvoke->nResultCount; i += 4)
	    {
            len = MIN((UINT)pInvoke->pResult[i+1]->len, sizeof(TIMESTAMP));
            memcpy(&issueTime, pInvoke->pResult[i+1]->stream.p, len);
            len = MIN((UINT)pInvoke->pResult[i+3]->len, sizeof(pszData)-1);
            memcpy(pszData, pInvoke->pResult[i+3]->stream.p, len);

            fprintf(stdout, "%04d/%02d/%02d %02d:%02d:%02d  ID:%d UD:%d MSG:\"%s\"\n",
                    issueTime.year, issueTime.mon, issueTime.day, 
                    issueTime.hour, issueTime.min, issueTime.sec,
                    pInvoke->pResult[i]->stream.u32,
                    pInvoke->pResult[i+2]->stream.u32,
                    pszData);
        }
    }

    return nError;
}


void Usage(char * filename)
{
	printf("Usage: %s [-dhipstu] targetId\n", filename);
	printf("         -d         Enable debug message\n");
	printf("         -h         Display this message\n");
	printf("         -p #       Server Port (Default 8000)\n");
	printf("         -s ip      Server Ip Address (Default 127.0.0.1)\n");
	printf("\n");
}

int main(int argc, char ** argv)
{
    int opt;
    char * pIpAddr = const_cast<char *>("127.0.0.1");
    char * pTargetId = NULL;
    int nPort=8000;
    int nDebugMode=1;

    while((opt=getopt(argc, argv, "dhp:s:")) != -1) {
        switch(opt) {
            case 'd':
                nDebugMode = 0;
                break;
            case 'p': 
                nPort = (int)strtol(optarg,(char **)NULL,10);
                break;
            case 's': 
                pIpAddr = strdup(optarg);
                break;
            case 'h':
            default :
                Usage(argv[0]);
                return -1;
        }
    }

    if(argc - optind < 1) {
        Usage(argv[0]);
        return -1;
    }

    pTargetId = strdup(argv[optind]);

    SET_DEBUG_MODE(nDebugMode);
    IF4Client_Initialize_Interface(8000);

    cmdPollMessage(pIpAddr, nPort, pTargetId);

    IF4Client_Terminate_Interface();

	return 0;
}
