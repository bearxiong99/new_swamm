
#include "common.h"
#include "gpiodef.h"
#include "kbhit.h"

extern char	* m_szPrompt;
extern BYTE macAddress[6];
extern void displayPrompt();

int     m_MaxIndex = 0;

GPIOTBL gpioTbl[] =
{
#if     defined(__SUPPORT_NZC1__)
    { "SINK PW",         6,     GPIO_WRITE,     GP_CODI_PCTL_OUT },
    { "BATT SW",         8,     GPIO_WRITE,     GP_BATT_SW_OUT },
#endif
    { "GSM PW",         11,     GPIO_WRITE,     GP_GSM_PCTL_OUT },
    { "TEMP_OS",        14,     GPIO_READ,      GP_TEMP_OS_INPUT },
    { "PWR_FAIL",       19,     GPIO_READ,      GP_PWR_FAIL_INPUT },
    { "INT_ETH",        21,     GPIO_READ,      GP_INT_ETH_INPUT },
    { "GSM IGT",        30,     GPIO_WRITE,     GP_GSM_IGT_OUT },
    { "LOW_BATT",       31,     GPIO_READ,      GP_LOW_BATT_INPUT },
    { "GSM_SYNC",       32,     GPIO_READ,      GP_GSM_SYNC_INPUT },
    { "SINK RST",       56,     GPIO_WRITE,     GP_CODI_RST_OUT },
#if     defined(__SUPPORT_NZC2__)
    { "BATT SW",        62,     GPIO_WRITE,     GP_BATT_SW_OUT },
#endif
    { "BATT_CHR_STAT",  74,     GPIO_READ,      GP_BATT_CHG_STATUS_INPUT },
    { "BATT CHARGE",    75,     GPIO_WRITE,     GP_BATT_CHG_EN_OUT },
    { "SW RST",         76,     GPIO_WRITE,     GP_SW_RST_OUT },
    { "GSM EMG OFF",    84,     GPIO_WRITE,     GP_GSM_EMERGOFF_OUT },
    { NULL, 0, 0, 0 }
};

void displayGPIO(void)
{
    int         i;

    for(i=0,m_MaxIndex=0; gpioTbl[i].name ; i++) {
        switch(gpioTbl[i].type) {
            case GPIO_READ:
                printf("            %-20s    %02d    %s\n", 
                    gpioTbl[i].name, gpioTbl[i].port, 
                    GPIOAPI_ReadGpio(gpioTbl[i].value) ? "HIGH" : "LOW");
                break;
            case GPIO_WRITE:
                m_MaxIndex ++;
	            printf("         %d. %-20s    %02d    %s\n", 
                    m_MaxIndex,
                    gpioTbl[i].name, gpioTbl[i].port, " -");
                break;
        }
    }
}

void displayValue()
{
	printf("\n--------------------------------------------------\n");
	printf("        --- GPIO Read/Write ---   \n");
    printf("        MAC [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                macAddress[0], macAddress[1], macAddress[2],
                macAddress[3], macAddress[4], macAddress[5]);
	printf("--------------------------------------------------\n");
	printf("         Idx GPIO                  PORT   VALUE \n");
	printf("--------------------------------------------------\n");
    displayGPIO();
	printf("\n         0. Exit                          \n");
}

int getInputValue(int selectedIndex) 
{
    int         i,idx=1;

    for(i=0; gpioTbl[i].name ; i++) {
        if(gpioTbl[i].type == GPIO_WRITE) {
            if(idx == selectedIndex) return i;
            idx++;
        }
    }

    return -1;
}

void updateGpio(int idx) 
{
    char c;

	printf("\n\n         %s (GPIO %d) selected (LOW=0, HIGH=1)\n",
        gpioTbl[idx].name, gpioTbl[idx].port);
	printf("           Input Value=(0/1) : "); fflush(stdout);

    c = readch();

    switch(c) {
        case '0' : 
	        printf("\n           Write LOW \n");
            GPIOAPI_WriteGpio(gpioTbl[idx].value, 0); break;
        case '1' : 
	        printf("\n           Write HIGH \n");
            GPIOAPI_WriteGpio(gpioTbl[idx].value, 1); break;
    }

	printf("           Execute.... ");fflush(stdout);
    usleep(1000000);
    printf("[DONE]\n");
}

int gpioTest(void)
{
    char    c;
    int     n, idx;
    
    displayValue();
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

        if(n < m_MaxIndex) {
            if((idx = getInputValue(n))>=0) {
                updateGpio(idx);
            }
        }

        displayValue();
        displayPrompt();
    }

    return 0;
}

