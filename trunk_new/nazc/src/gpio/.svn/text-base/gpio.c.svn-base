
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "typedef.h"
#include "gpio.h"
#include "GPIOAPI.h"

#if defined(__TI_AM335X__)
void usage(char* name)
{
    fprintf(stderr,"GPIO Control Utility v1.0\n");
    fprintf(stderr,"%s -[paiswm]\n", name);
    fprintf(stderr,"   -i             gpio initialize\n");
    fprintf(stderr,"   -a num         adc number\n");
    fprintf(stderr,"   -p port        port number\n");
    fprintf(stderr,"   -s millisec    sleep millisec\n");
    fprintf(stderr,"   -w value       write value(0/1)\n");
    fprintf(stderr,"   -m module      module type(default:ge910,ue910,ame5530)\n");
    fprintf(stderr,"\n");

    fprintf(stderr,"Port Information (Normal)R/W\n");
    fprintf(stderr," 20 GSM Sync         (H)R    97 Debug LED 3     (L)W\n");
    fprintf(stderr," 29 Low Battery      (H)R    98 RF Reset        (H)W\n");
    fprintf(stderr," 60 Case Open        (L)R   110 GSM Reset       (L)W\n");
    fprintf(stderr," 63 Power Fail       (L)R   111 RF GPIO         (L)W\n");
    fprintf(stderr," 64 Batt Chg State   (L)R   112 Batt SW Ctrl    (H)W\n");
    fprintf(stderr," 65 State Green      (L)W   113 GSM SW PW Ctrl  (H)W\n");
    fprintf(stderr," 86 RF Power Ctrl    (H)W   114 485 TXD En      (L)W\n");
    fprintf(stderr," 87 NPLC Power Ctrl  (H)W   115 Batt Chg En     (L)W\n");
    fprintf(stderr," 88 GSM Power Ctrl   (H)W   116 Debug LED 1     (L)W\n");
    fprintf(stderr," 89 NPLC Reset       (H)W   117 Debug LED 2     (L)W\n");
    fprintf(stderr," 96 Software Reset   (H)W                           \n");
    fprintf(stderr,"\n");
    fprintf(stderr,"ADC Number : Read only\n");
    fprintf(stderr,"  0 Battery Voltage  (-)R     1 Main Voltage    (-)R\n");

    exit (1);
}

int main(int argc, char* argv[])
{
    int opt;
    int nPort = -1;
    int nAdc = -1;
    int nValue = -1;
    int nSleep = -1;
    bool bWrite = false;
    bool bInitialize = false;
    BOOL res=TRUE;
    char* moduleName = const_cast<char*>("ge910");

    while((opt=getopt(argc,argv,"ia:m:p:w:s:h")) != -1)
    {
        switch(opt)
        {
            case 'i':
                bInitialize = true;
                break;
            case 'm':
                moduleName = strdup(optarg);
                break;
            case 'a':
                nAdc = (int)strtol(optarg,(char **)NULL, 10);
                break;
            case 'p':
                nPort = (int)strtol(optarg,(char **)NULL, 10);
                break;
            case 's':
                nSleep = (int)strtol(optarg,(char **)NULL, 10);
                break;
            case 'w':
                bWrite = true;
                nValue = (int)strtol(optarg,(char **)NULL, 10);
                break;
        }
    }

    if(bInitialize) 
    {
        GPIOAPI_Initialize(TRUE, moduleName, FALSE);
        return 0;
    }

    if(nPort < 0 && nAdc < 0)
    {
        usage(argv[0]);
    }

    if(bWrite && (nValue < 0 || nPort < 0))
    {
        usage(argv[0]);
    }

    GPIOAPI_Initialize(FALSE, moduleName, FALSE);

    if(bWrite)
    {
        res = GPIOAPI_WriteGpio(nPort, nValue);
        if(res && nSleep > 0)
        {
            usleep(nSleep);
        }
    }
    else
    {
        if(nPort > 0)
        {
            fprintf(stdout,"%d\r\n", GPIOAPI_ReadGpio(nPort));
        }
        if(nAdc >= 0)
        {
            fprintf(stdout,"%d\r\n", GPIOAPI_ReadAdc(nAdc));
        }
    }
    
    return res ? 0 : 1;
}
#endif // __TI_AM335X__
