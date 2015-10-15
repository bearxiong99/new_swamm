
/** Nuritelecom AiMiR Project.
 *
 * Merge And Install
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
#include "mcudef.h"
#include "varapi.h"
#include "if4api.h"
#include "Variable.h"
#include "CommonUtil.h"
#include "DebugUtil.h"

#include "CliUtils.h"

int DumpData(char * szTargetId, char * szTargetType, char * filename, int bDump)
{
	METERLPENTRY 	*pEntry;
	char			szId[16+1];
	char			*pszData;
	int				fd, len;
	int				nLength;

    memset(szId, 0, sizeof(szId));
	fd = open(filename, O_RDONLY);
	if (fd > 0)
	{
		pszData = (char *)malloc(100*1024);
		if (pszData != NULL)
		{
			for(;;)
			{
				nLength = read(fd, pszData, sizeof(METERLPENTRY));
				if (nLength != (int)sizeof(METERLPENTRY))
					break;

				pEntry = (METERLPENTRY *)pszData;

                EUI64ToStr(&pEntry->mlpId, szId);

				read(fd, pszData+sizeof(METERLPENTRY), pEntry->mlpLength - sizeof(TIMESTAMP));

                if(szTargetId == NULL || strcmp(szId, szTargetId) == 0)
                {
                    fprintf(stdout,"ID=%s [%04d/%02d/%02d %02d:%02d:%02d]\n", 
                            szId,
                            pEntry->mlpTime.year,
                            pEntry->mlpTime.mon,
                            pEntry->mlpTime.day,
                            pEntry->mlpTime.hour,
                            pEntry->mlpTime.min,
                            pEntry->mlpTime.sec
                    );
                    fprintf(stdout,"Type=%d SeviceType=%d Vendor=%d\n",
                            pEntry->mlpType,
                            pEntry->mlpServiceType,
                            pEntry->mlpVendor);
                }

				len = LittleToHostShort(pEntry->mlpLength) - sizeof(TIMESTAMP);
                if(szTargetId != NULL && szTargetType != NULL && strcmp(szId, szTargetId) == 0)
                {
                    switch(szTargetType[0])
                    {
                        case 'A':
                            ge_data_parsing(pEntry->mlpData, len, TRUE);
                            break;
                        case 'D':
                            dlms_data_parsing(pEntry->mlpData, len, TRUE);
                            break;
                        case 'a':
                            aidon_data_parsing(pEntry->mlpData,len,TRUE);
                            break;
                        case 'p':
                            pulse_data_parsing(const_cast<char *>("Pulse"), pEntry->mlpData, len, TRUE);
                            break;
                        case 'c':
                            pulse_data_parsing(const_cast<char *>("ACD"), pEntry->mlpData, len, TRUE);
                            break;
                        case 's':
                            pulse_data_parsing(const_cast<char *>("SmokeDetector"), pEntry->mlpData, len, TRUE);
                            break;
                        case 'k':
                            kamstrup_data_parsing(pEntry->mlpData, len, TRUE);
                            break;
                        case 'r':
                            repeater_data_parsing(pEntry->mlpData, len, TRUE);
                            break;
                        case 'm':
                            mbus_data_parsing(pEntry->mlpData, len, TRUE);
                            break;
                    }

                    if(bDump)
                    {
                        fprintf(stdout, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
                        FDUMP(stdout, pszData, pEntry->mlpLength - sizeof(TIMESTAMP) + sizeof(METERLPENTRY));
                        fprintf(stdout, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
                    }

                }
			}
			free(pszData);
		}
		close(fd);
	}
    return 0;
}

int usage(char * name)
{
    fprintf(stderr,"%s [-dti] filename\n", name);
    fprintf(stderr,"  -d       : dump data\n"); 
    fprintf(stderr,"  -i id    : EUI64 ID\n"); 
    fprintf(stderr,"  -t type  : meter type\n"); 
    fprintf(stderr,"             A ANSI Meter\n"); 
    fprintf(stderr,"             a Aidon 5530\n"); 
    fprintf(stderr,"             c ACD\n"); 
    fprintf(stderr,"             D DLMS Meter\n"); 
    fprintf(stderr,"             k Kamstrup\n"); 
    fprintf(stderr,"             m MBUS\n"); 
    fprintf(stderr,"             p Pulse\n"); 
    fprintf(stderr,"             r Repeater\n"); 
    fprintf(stderr,"             s Smoke Detector\n"); 
    exit(1);
}

int main(int argc, char * argv[])
{
    char *filename = NULL;
    char bTarget = 0x00;
    char * szId = NULL;
    char * szType = NULL;
    int bDump = 0;
    int  opt;

    while((opt=getopt(argc, argv, "di:t:")) != -1) {
        switch(opt) {
            case 'd': bDump = 1;
                break;
            case 'i': szId =  strdup(optarg);
                bTarget |= 0x01;
                break;
            case 't': szType = strdup(optarg);
                bTarget |= 0x02;
                break;
            default :
                usage(argv[0]);
        }
    }
    if(argc - optind < 1) {
        usage(argv[0]);
    }

    if(bTarget && (bTarget != 0x03))
    {
        /** ID와 Type정보는 항상 쌍을 이루어 들어와야 한다 */
        usage(argv[0]);
    }

    filename = strdup(argv[optind]);

    return DumpData(szId, szType, filename, bDump);
}
