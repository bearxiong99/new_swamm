
/** Nuritelecom AiMiR Project.
 *
 *  Nuri Aimir Concentrator External Interface Library.
 *
 */

#include "common.h"
#include "Chunk.h"
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


int cmdSendMessage(const char * pIpAddr, int nPort, const char * pTargetId, 
        int nMessageId, int nMessageType, int nDuration, int nUserCallback, 
        const char * pMessage, int nMessageLen)
{
    IF4Invoke * pInvoke = NULL;
    EUI64       id;
    int         nError;

    printf("%s %d\n", pIpAddr, nPort);
    CIF4Invoke  invoke(pIpAddr, nPort, 10);

    StrToEUI64(pTargetId, &id);

    invoke.AddParamFormat("1.14", VARSMI_EUI64, &id, sizeof(EUI64));
    invoke.AddParam("1.6", (UINT) nMessageId);
    invoke.AddParam("1.4", (BYTE) nMessageType);
    invoke.AddParam("1.6", (UINT) nDuration); 
    invoke.AddParam("1.6", (UINT) 0);               // nErrorHandler
    invoke.AddParam("1.6", (UINT) 0);               // nPreHandler
    invoke.AddParam("1.6", (UINT) 0);               // nPostHandler
    invoke.AddParam("1.6", (UINT) nUserCallback);   
    invoke.AddParamFormat("1.12", VARSMI_STREAM, pMessage, nMessageLen);

    pInvoke = (IF4Invoke *)invoke.GetHandle();
    nError = IF4API_Command(pInvoke, "111.4", IF4_CMDATTR_REQUEST | IF4_CMDATTR_RESPONSE);

    if(nError != IF4ERR_NOERROR) 
    {
        fprintf(stderr, "IF4Error %s\n", IF4API_GetErrorMessage(nError));
    }

    return nError;
}


void Usage(char * filename)
{
	printf("Usage: %s [-dhifpstu] targetId [message]\n", filename);
	printf("         -d         Enable debug message\n");
	printf("         -h         Display this message\n");
	printf("         -i #       Message Id (Default 0)\n");
	printf("         -p #       Server Port (Default 8000)\n");
	printf("         -s ip      Server Ip Address (Default 127.0.0.1)\n");
	printf("         -t #       Message Type (1 Immediately, 2 Laxy, 3 Passive)\n");
	printf("         -u #       User Callback number (Default 0)\n");
	printf("         -w #       Message waiting seconds (Default 30)\n");
	printf("\n");
}

int main(int argc, char ** argv)
{
    int opt;
    char * pIpAddr = const_cast<char *>("127.0.0.1");
    char * pTargetId = NULL;
    char * pMessage = NULL;
    int    nMessageLen = 0;
    int nPort=8000;
    int nDebugMode=1;
    int nMessageId=0;
    int nMessageType=3;
    int nUserCallback=0;
    int nDuration=30;
    CChunk chunk;

    while((opt=getopt(argc, argv, "dhi:p:s:t:u:w:")) != -1) {
        switch(opt) {
            case 'd':
                nDebugMode = 0;
                break;
            case 'i': 
                nMessageId = (int)strtol(optarg,(char **)NULL,10);
                break;
            case 'p': 
                nPort = (int)strtol(optarg,(char **)NULL,10);
                break;
            case 's': 
                pIpAddr = strdup(optarg);
                break;
            case 't': 
                nMessageType = (int)strtol(optarg,(char **)NULL,10);
                break;
            case 'u': 
                nUserCallback = (int)strtol(optarg,(char **)NULL,10);
                break;
            case 'w': 
                nDuration = (int)strtol(optarg,(char **)NULL,10);
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

    if(argc - optind < 2) {
        /** Message가 지정되지 않았을 대 stdin 으로 부터 읽어서 그 내용을 보낸다.
          */
        char buff[1024];
        int len;
        while((len = fread(buff, 1, sizeof(buff), stdin)) > 0) {
            fprintf(stdout, "%d\n", len);
            chunk.Add(buff, len);
        }
        pMessage = chunk.GetBuffer();
        nMessageLen = chunk.GetSize();
    }else {
        /** Message가 지정되어 있을 경우 */
        pMessage = strdup(argv[optind+1]);
        nMessageLen = strlen(pMessage);
    }

    SET_DEBUG_MODE(nDebugMode);
    IF4Client_Initialize_Interface(8000);

    cmdSendMessage(pIpAddr, nPort, pTargetId, nMessageId, nMessageType, 
            nDuration, nUserCallback, pMessage, nMessageLen);

    IF4Client_Terminate_Interface();

	return 0;
}
