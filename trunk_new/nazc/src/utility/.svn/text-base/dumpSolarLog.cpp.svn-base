
/** Nuritelecom AiMiR Project.
 *
 * Dump Solar Battery Log
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>

#include "aimir.h"
#include "typedef.h"
#include "version.h"
#include "config.h"
#include "if4api.h"
#include "varapi.h"
#include "mcudef.h"
#include "CommonUtil.h"
#include "Utils.h"
#include "Variable.h"
#include "vendor/repdef.h"

#include "CommandHandler.h"

extern VAROBJECT	m_Root_node[];

#define STATE_SD_READ_INFO          2
#define STATE_SD_READ_LP_HEADER     3
#define STATE_SD_LP_ENTRY           4
#define STATE_SD_OBIS               5
#define STATE_SD_READ_SOLAR_LOG     6
#define STATE_SD_DONE               0


void repeater_data_parsing(char *mdata, UINT length)
{
    REPEATER_METERING_DATA repeaterData;
    SOLAR_TIME_DATE solarTimeDate;
    SOLAR_LOG_DATA solarLogData;
    int     state;
    UINT    idx = 0, i;
    int     count=0;

    state = STATE_SD_READ_INFO;
    while(state != STATE_SD_DONE) {
        switch(state) {
            case STATE_SD_READ_INFO:
                if(idx+sizeof(REPEATER_METERING_DATA) > length) {
                    fprintf(stdout, "ERROR -- Read REPEATER INFO Fail\r\n");
                    state = STATE_SD_DONE;
                    break;
                }
                memcpy(&repeaterData, mdata+idx, sizeof(REPEATER_METERING_DATA));
                idx += sizeof(REPEATER_METERING_DATA);
                
                count = repeaterData.solarLogCount;
                state = STATE_SD_READ_SOLAR_LOG;
            break;
            case STATE_SD_READ_SOLAR_LOG :
                if(count <= 0) {
                    state = STATE_SD_DONE;
                    break;
                }

                if(idx+sizeof(SOLAR_TIME_DATE) > length) {
                    fprintf(stdout, "ERROR -- Read SOLAR LOG TIME DATE Fail\r\n");
                    state = STATE_SD_DONE;
                    break;
                }

                memcpy(&solarTimeDate, mdata+idx, sizeof(SOLAR_TIME_DATE));
                idx += sizeof(SOLAR_TIME_DATE);

                fprintf(stdout, "#  SOLAR BATTERY LOG : %04d/%02d/%02d\r\n",
                    htons(solarTimeDate.year),
                    solarTimeDate.month,
                    solarTimeDate.day);

                if(idx+(sizeof(SOLAR_LOG_DATA) * solarTimeDate.hourCount) > length) {
                    fprintf(stdout, "ERROR -- Read SOLAR LOG DATA Fail\r\n");
                    state = STATE_SD_DONE;
                    break;
                }

                for(i=0;i<solarTimeDate.hourCount;i++) {
                    memcpy(&solarLogData, mdata+idx, sizeof(SOLAR_LOG_DATA));
                    idx += sizeof(SOLAR_LOG_DATA);
                    fprintf(stdout, "%02d/%02d,%02d:%02d %.4f %.4f %.4f\r\n",
                        solarTimeDate.month,
                        solarTimeDate.day,
                        (int)(solarLogData.hour / 4), (solarLogData.hour % 4) * 15,
                        (double)(htons(solarLogData.solarAdVolt)) * 2 / 10000.0, 
                        (double)(htons(solarLogData.solarChgBattVolt)) / 10000.0, 
                        (double)(htons(solarLogData.solarBoardDcVolt)) / 10000.0);
                }
                count --;
            break;
            default :
                state = STATE_SD_DONE;
                break;
        }
    }
}

int solarLogDump(char *ipaddr, int nPort, char *euiid, int nOption, int nOffset, int nCount)
{
	CIF4Invoke		invoke(ipaddr, nPort, 600);
	int				nError;
    EUI64           id;
	METERLPENTRY	*pItem;

    fprintf(stderr,"dump solar : ipaddr=%s, id=%s, option=%d, offset=%d, count=%d\n",
            ipaddr, euiid, nOption, nOffset, nCount);

    StrToEUI64(euiid, &id);

	invoke.AddParamFormat("4.1.1", VARSMI_EUI64, (void *)&id, sizeof(EUI64));
	invoke.AddParam("1.9", nOption);
	invoke.AddParam("1.9", nOffset);
	invoke.AddParam("1.9", nCount);

	nError =  Call(invoke.GetHandle(), "104.6");
    if(nError) {
        fprintf(stderr, " error=[%d %s]\n", nError, IF4API_GetErrorMessage(nError));
		return -nError;
    }

    pItem = (METERLPENTRY *)invoke.GetHandle()->pResult[0]->stream.p;

    fprintf(stdout, "#          SENSOR ID : %s\r\n", euiid);
    repeater_data_parsing(pItem->mlpData, pItem->mlpLength - sizeof(TIMESTAMP));

    return 0;
}

int fileOpenAndDump(char * filename, char *ipaddr, int nPort, int nOption, int nOffset, int nCount)
{
    FILE * fp;
    char buffer[256], idstr[64];

    if((fp=fopen(filename,"r"))!=NULL) {
        while(fgets(buffer,sizeof(buffer),fp)) {
            if(buffer[0] == '#' || strlen(buffer) < 16) continue;
            sscanf(buffer,"%s", idstr);
            solarLogDump(ipaddr, nPort, idstr, nOption, nOffset, nCount);
        }
        fclose(fp);
    }
    return 0;
}

int usage(char * name)
{
    fprintf(stderr,"%s {-i}|{-f} [-aposc]\n", name);
    fprintf(stderr," -i : eui64 id\n");
    fprintf(stderr," -f : filename\n");
    fprintf(stderr," -a : ipaddr\n");
    fprintf(stderr," -p : port (default:8000)\n");
    fprintf(stderr," -o : option (default:-1)\n");
    fprintf(stderr," -s : offset (default:1)\n");
    fprintf(stderr," -c : count (default:1)\n");
    exit(1);
}

int main(int argc, char * argv[])
{
    char *ipaddr = NULL;
    char *euiid = NULL;
    char *filename = NULL;
    int  nOption = -1;
    int  nOffset = 1;
    int  nCount = 1;
    int  nPort = 8000;
    int  opt;

    while((opt=getopt(argc, argv, "f:a:p:i:o:s:c:h")) != -1) {
        switch(opt) {
            case 'a': ipaddr = strdup(optarg); break;
            case 'i': euiid = strdup(optarg); break;
            case 'o': nOption = (int) strtol(optarg,(char **)NULL,10); break;
            case 'p': nPort = (int) strtol(optarg,(char **)NULL,10); break;
            case 's': nOffset = (int) strtol(optarg,(char **)NULL,10); break;
            case 'c': nCount = (int) strtol(optarg,(char **)NULL,10); break;
            case 'f': filename = strdup(optarg); break;
            case 'h':
            default :
                usage(argv[0]);
        }
    }

    VARAPI_Initialize(NULL, m_Root_node, FALSE);
    IF4API_Initialize(IF4_DEFAULT_PORT, NULL);

    if(filename && ipaddr) {
        fileOpenAndDump(filename, ipaddr, nPort, nOption, nOffset, nCount);
    }else if(ipaddr && euiid) {
        solarLogDump(ipaddr, nPort, euiid, nOption, nOffset, nCount);
    }else {
        usage(argv[0]);
    }

    IF4API_Destroy();
    VARAPI_Destroy();

    return 0;
}
