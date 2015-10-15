
#include "common.h"
#include "proc.h"
#include "kbhit.h"
#include "CommonUtil.h"

const char    *version = "v1.3";
char	m_szPrompt[100] = "         Select Menu Key... : ";
struct  termios oldtio;
char    m_szMcuModel[128];

int     maxMenuLength = 0;
int     enableVerbose = 0;
BYTE    macAddress[6];

extern int gpioTest(void);
extern int productTest(void);
extern void getEtherInfo(char *, UINT *, UINT *, BYTE *);
extern int OnOffHeater(int);
extern int testComplete(void);

PROCTABLE	m_MenuTable[] =
{
	{ "Exit" , NULL },
	{ "GPIO Test",	gpioTest },
	{ "Product Test" , productTest },
    /* 김재훈대리의 요청으고 Skip 기능은 제거
	{ "Skip Test" , testComplete },
    */
	{ NULL , NULL }
};

void displayMenu()
{
	int		i;

	printf("\n--------------------------------------------------\n");
	printf("        --- H/W Testing Program ---   %s\n",version);
    printf("        MAC [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                macAddress[0], macAddress[1], macAddress[2],
                macAddress[3], macAddress[4], macAddress[5]);
	printf("--------------------------------------------------\n");
	printf("\n");
	for(i=1; i<maxMenuLength ; i++) {
		printf("         %d. %s\n", i, m_MenuTable[i].pszTitle);
    }
	printf("\n");
	printf("         0. Exit                          \n");
	printf("\n");
	printf("--------------------------------------------------\n"); 
}

void displayPrompt()
{
    printf("\n%s", m_szPrompt); fflush(stdout);
}

void initialize()
{
    int i=1;
    char device[32];
    UINT ipaddr, subnet;

    tcgetattr(0, &oldtio);
    init_keyboard();

    printf("Program Initializing ..... "); fflush(stdout);
	for(; m_MenuTable[i].pszTitle ; i++) ;
    maxMenuLength = i;

    memset(macAddress,0,sizeof(macAddress));
    getEtherInfo(device, &ipaddr, &subnet, macAddress);

    GetFileString("/app/conf/mcumodel", m_szMcuModel, sizeof(m_szMcuModel), NULL, TRUE);

    OnOffHeater(0);

    GPIOAPI_Initialize(TRUE, MOBILE_MODULE_TYPE_GE910, FALSE);
    printf("[DONE]\n\n");
}

void usage(char * fname)
{
    printf("%s -hv\n", fname);
    printf("\th: display this message\n");
    printf("\tv: verbose mode\n");
}

int main(int argc, char **argv)
{
    char    c;
    int     n, opt;

    while((opt=getopt(argc, argv, "hv")) != -1) {
        switch(opt) {
            case 'v': enableVerbose = 1; break;
            case 'h': 
            default :
                usage(argv[0]); exit(1);
        }
    }

    initialize();

    displayMenu();
    displayPrompt();

	while(1)
	{
        c = readch();

        if(c == '\n') {
            displayPrompt();
            continue;
        }

		if (c < '1' || c > '9')
			break;

        n = c - '0';

        if(n < maxMenuLength) {
            m_MenuTable[n].pfnFunc();
        }

        displayMenu();
        displayPrompt();
	}

	printf("\n");

    close_keyboard();

	return 0;
}


