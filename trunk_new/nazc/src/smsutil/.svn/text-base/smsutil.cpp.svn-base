/** SMS Message Generator
 *
 * SMS를 전송해야 하는 Message String을 만들어 낸다.
 *
 *
 * @file    smsutil.cpp
 * @date    2013/11/07
 * @author  조규석(rewriter@nuritelecom.com)
 *
 */
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "typedef.h"
#include "CRC16.h"

#define SOH             "NSC"

#define KEY_POS         3
#define CMD_POS         5
#define LENGTH_POS      8
#define PAYLOAD_POS     10

static struct option long_options[] =
{
    {"key",         required_argument,  0,      'k'},
    {"command",     required_argument,  0,      'c'},
    {"param",       required_argument,  0,      'p'},
    {"help",        no_argument,        0,      'h'},
    {0, 0, 0, 0}
};

void usage(char *program)
{
    fprintf(stderr,"%s -kc[ph]\n", program);
    fprintf(stderr," -k   --key=keyvalue      DCU key string (2bytes)\n");
    fprintf(stderr," -c   --command=cmd       DCU SMS command (3bytes)\n");
    fprintf(stderr," -p   --param=value       message parameter\n");
    fprintf(stderr," -h   --help              display this message\n");
    exit(1);
}

int addparam(char *buff, const char* param, int idx)
{
    char escapeBuff[128];
    int len = strlen(param);
    int i;

    memset(escapeBuff, 0, sizeof(escapeBuff));

    for(i=0; i<len; i++)
    {
        if(param[i] == '\\' || param[i] == ',')
        {
            buff[PAYLOAD_POS + idx] = '\\';
            idx ++;
        }
        buff[PAYLOAD_POS + idx] = param[i];
        idx ++;
    } 

    return idx;
}


int main(int argc, char* argv[])
{
    int c;
    int i;
    int optionIndex = 0;
    int paramIndex = 0;
    bool hasKey = false;
    bool hasCmd = false;
    bool hasParam = false;
    char msgBuff[256];
    char lenBuffer[8];
    unsigned short crc;

    memset(msgBuff, 0, sizeof(msgBuff));
    strncpy(msgBuff, SOH, 3);

    while((c = getopt_long(argc, argv, "c:k:p:h", long_options, &optionIndex)) != -1)
    {
        switch(c)
        {
            case 'c':
                if(strlen(optarg) != 3) usage(argv[0]);
                hasCmd = true;
                memcpy(msgBuff + CMD_POS, optarg, 3);
                break;
            case 'k':
                if(strlen(optarg) != 2) usage(argv[0]);
                hasKey = true;
                memcpy(msgBuff + KEY_POS, optarg, 2);
                break;
            case 'p':
                if(hasParam)
                {
                    msgBuff[PAYLOAD_POS + paramIndex] = ',';
                    paramIndex ++;
                }
                else
                {
                    hasParam = true;
                }
                paramIndex = addparam(msgBuff, optarg, paramIndex);
                break;
            case 'h':
            default:
                usage(argv[0]);
                break;
        }
    }

    if(!hasKey || !hasCmd)
    {
        usage(argv[0]);
    }

    sprintf(lenBuffer,"%02d", paramIndex);
    memcpy(msgBuff + LENGTH_POS, lenBuffer, 2);

    crc = GetCrc16((unsigned char *)msgBuff, PAYLOAD_POS + paramIndex);
    sprintf(lenBuffer,"%02X%02X", crc & 0xFF, (crc >> 8) & 0xFF);
    memcpy(msgBuff + PAYLOAD_POS + paramIndex, lenBuffer, 4);

    for(i=0; i<3; i++)
    {
        if(islower(msgBuff[CMD_POS + i]))
        {
            msgBuff[CMD_POS + i] +=  'A' - 'a';
        }
    }

    fprintf(stdout,"%s\n", msgBuff);

    return 0;
}
