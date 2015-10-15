#include <limits.h>
#include "common.h"
#include "cmd_syntax.h"

const char	*ip_accept_str		= "0123456789.";
const char	*float_accept_str	= "-+0123456789.";
const char	*num_accept_str		= "-+0123456789";
const char	*hex_accept_str		= "0123456789ABCDEFabcdef";
const char	*hex_accept_str1	= "0123456789ABCDEFabcdef ";

extern  CLIHANDLER  m_CliCommandHandlers[];
extern  CLIPARAMCHECKER m_CliParameters[];

int	isaccept(const char *pszFormat, char *pszValue, int nSize, int nNeed=-1);
int	isjustaccept(const char *pszFormat, char *pszValue, int nSize, int nNeed=-1);

int isaccept(const char *pszFormat, char *pszValue, int nSize, int nNeed)
{
	int		i, n;

	n = strlen(pszValue);
	if ((nNeed != -1) && (n > nSize))
		return -1;
	
	for(i=0; i<n; i++)
	{
		if (strchr(pszFormat, pszValue[i]) == 0)
			return -2;
	}

	return n;	
}

int isjustaccept(const char *pszFormat, char *pszValue, int nSize, int nNeed)
{
	int		i, n;

	n = strlen(pszValue);
	if ((nNeed != -1) && (n != nSize))
		return -1;
	
	for(i=0; i<n; i++)
	{
		if (strchr(pszFormat, pszValue[i]) == 0)
			return -2;
	}

	return n;	
}

int check_hex_string(char *pszString, int len)
{
	int		cnt = 0;
	char	*p;
    char    *saveptr, *token, *str;
    int     res = 0;
    UINT    val;

	// check accept string
	for(p=pszString; p && *p; p++)
	{
		if (strchr(hex_accept_str1, *p) == NULL)
			return 0;	
	}

    for(str=pszString; ; str = NULL)
    {
        token = strtok_r(str, " ", &saveptr);
        if(token == NULL) break;

        val = (UINT) strtol(token, (char **)NULL, 16);

        if(val > 0xFF) return 0;
        
        res = 1;
	}

	return (len > 0) ? ((cnt == len) ? res : 0) : res;
}

int check_oid(CLISESSION *pSession, char *pszValue)
{
	return 1;
}

int check_number(CLISESSION *pSession, char *pszValue)
{
    long n = strtol(pszValue,(char **)NULL,10);

    if((n == LONG_MIN || n == LONG_MAX)  && (errno == ERANGE)) return 0;
    
	return 1;
}

int check_severity(CLISESSION *pSession, char *pszValue)
{
    long n = strtol(pszValue,(char **)NULL,10);

    if((n == LONG_MIN || n == LONG_MAX)  && (errno == ERANGE)) return 0;
    
    if(n >= 0 && n <= 4) return 1;

    return 0;
}

int check_byte(CLISESSION *pSession, char *pszValue)
{
    long v;

    v = strtoll(pszValue, (char **) NULL, 10);

    return (v > 255 || v < 0) ? 0 : 1;

}

int check_unsigned(CLISESSION *pSession, char *pszValue)
{
    long long int v;

    v = strtoll(pszValue, (char **) NULL, 10);

    return (v >= 0) ? 1 : 0;
}

int check_hex8(CLISESSION *pSession, char *pszValue)
{
	return check_hex_string(pszValue, 8);
}

int check_hex16(CLISESSION *pSession, char *pszValue)
{
	return check_hex_string(pszValue, 16);
}

int check_hex_list(CLISESSION *pSession, char *pszValue)
{
	return check_hex_string(pszValue, 0);
}

int check_enable(CLISESSION *pSession, char *pszValue)
{
	if (strcasecmp(pszValue, "enable") == 0)
		return 1;

	if (strcasecmp(pszValue, "disable") == 0)
		return 1;

	if (strlen(pszValue) != 1)
		return 0;

	if ((pszValue[0] == '0') || (pszValue[0] == '1'))
		return 1;

	return 0;	
}

int check_opcode(CLISESSION *pSession, char *pszValue)
{
	int		n;
	
	if (strcasecmp(pszValue, "clear") == 0)
		return 1;

	if (strcasecmp(pszValue, "active") == 0)
		return 1;

	if (strcasecmp(pszValue, "on") == 0)
		return 1;

	if (strcasecmp(pszValue, "off") == 0)
		return 1;

	n = strtol(pszValue, (char **)NULL, 10);
	if (n > 3 || n < 0)
		return 0;
	
	return 1;
}

int check_opmode(CLISESSION *pSession, char *pszValue)
{
	int		n;
	
	if (strcasecmp(pszValue, "normal") == 0)
		return 1;

	if (strcasecmp(pszValue, "test") == 0)
		return 1;

    /** Issue #2020: TimeSync Strategy가 추가되면서 Op Mode 중 Pulse Mesh는 
      * Deprecate 된다.
	  * if (strcasecmp(pszValue, "pulse-mesh") == 0)
      *     return 1;
      */

	n = strtol(pszValue, (char **)NULL, 10);
	if (n > 2 || n < 0)
		return 0;
	
	return 1;
}

int check_float(CLISESSION *pSession, char *pszValue)
{       
    int     i, len;
    double  v;
    
    len = strlen(pszValue);
    v = atof(pszValue);
    if ((v < -55) || (v > 125))
        return 0;
    
    for(i=0; i<len; i++)
    { 
        if (strchr(float_accept_str, pszValue[i]) == 0)
            return 0;
    }
    return 1;
}  

int check_string(CLISESSION *pSession, char *pszValue)
{
    if(pszValue != NULL && strlen(pszValue) > 0) return 1;

	return 0;
}

int check_format(CLISESSION *pSession, char *pszValue)
{
	if (strcasecmp(pszValue, "a") == 0)
		return 1;
	if (strcasecmp(pszValue, "h") == 0)
		return 1;
	return 0;
}

int check_onoff(CLISESSION *pSession, char *pszValue)
{
	if (strcasecmp(pszValue, "on") == 0)
		return 1;
	if (strcasecmp(pszValue, "off") == 0)
		return 1;
	if (strcasecmp(pszValue, "true") == 0)
		return 1;
	if (strcasecmp(pszValue, "false") == 0)
		return 1;
	if (strcasecmp(pszValue, "high") == 0)
		return 1;
	if (strcasecmp(pszValue, "low") == 0)
		return 1;
	if (strcmp(pszValue, "0") == 0)
		return 1;
	if (strcmp(pszValue, "1") == 0)
		return 1;
	return 0;
}

int check_otatype(CLISESSION *pSession, char *pszValue)
{
	if (strcasecmp(pszValue, "auto") == 0)
		return 1;
	if (strcasecmp(pszValue, "multicast") == 0)
		return 1;
	if (strcasecmp(pszValue, "unicast") == 0)
		return 1;
	if (strcmp(pszValue, "0") == 0)
		return 1;
	if (strcmp(pszValue, "1") == 0)
		return 1;
	if (strcmp(pszValue, "2") == 0)
		return 1;
	return 0;
}
int check_cputype(CLISESSION *pSession, char *pszValue)
{
	if (strcasecmp(pszValue, "em250") == 0)
		return 1;
	if (strcasecmp(pszValue, "str711") == 0)
		return 1;
	if (strcmp(pszValue, "0") == 0)
		return 1;
	if (strcmp(pszValue, "1") == 0)
		return 1;
	return 0;
}

int check_elmetertype(CLISESSION *pSession, char *pszValue)
{
    if (strcasecmp(pszValue, "ge") == 0)
        return 1;
    if (strcasecmp(pszValue, "0") == 0)
        return 1;
    if (strcasecmp(pszValue, "aidon") == 0)
        return 1;
    if (strcasecmp(pszValue, "1") == 0)
        return 1;
    if (strcasecmp(pszValue, "kamstrup") == 0)
        return 1;
    if (strcasecmp(pszValue, "2") == 0)
        return 1;
    return 0;
}
int check_devtype(CLISESSION *pSession, char *pszValue)
{
	if (strcasecmp(pszValue, "mcu") == 0)
		return 1;
	if (strcasecmp(pszValue, "0") == 0)
		return 1;
	if (strcasecmp(pszValue, "sensor") == 0)
		return 1;
	if (strcasecmp(pszValue, "1") == 0)
		return 1;
	if (strcasecmp(pszValue, "codi") == 0)
		return 1;
	if (strcasecmp(pszValue, "2") == 0)
		return 1;
	return 0;
}
//wget, zmodem,dir
int check_getmethod(CLISESSION *pSession, char *pszValue)
{
	if (strcasecmp(pszValue, "wget") == 0)
		return 1;
	if (strcasecmp(pszValue, "0") == 0)
		return 1;
	if (strcasecmp(pszValue, "zmodem") == 0)
		return 1;
	if (strcasecmp(pszValue, "1") == 0)
		return 1;
	if (strcasecmp(pszValue, "dir") == 0)
		return 1;
	if (strcasecmp(pszValue, "2") == 0)
		return 1;
	return 0;
}


int check_group(CLISESSION *pSession, char *pszValue)
{
	if (strcmp(pszValue, "admin") == 0)
		return 1;
	if (strcmp(pszValue, "user") == 0)
		return 1;
	if (strcmp(pszValue, "guest") == 0)
		return 1;
	return 0;
}

int check_id(CLISESSION *pSession, char *pszValue)
{
	return isjustaccept(hex_accept_str, pszValue, 16, TRUE) > 0 ? 1 : 0;
}

int check_allid(CLISESSION *pSession, char *pszValue)
{
	// 'all' Check
	if (strcmp(pszValue, "all") == 0)
		return 1;

	return check_id(pSession, pszValue);
}

int check_idparser(CLISESSION *pSession, char *pszValue)
{
	int ret = check_id(pSession, pszValue);

    if(!ret) ret = check_string(pSession, pszValue);

    return ret;
}

int check_agroup(CLISESSION *pSession, char *pszValue)
{
	int		n;

	if (strcmp(pszValue, "all") == 0)
		return 1;
	
	n = strtol(pszValue, (char **)NULL, 10);
	if ((n > 0) && (n < 255))
		return 1;
	return 0;
}

int check_hex(CLISESSION *pSession, char *pszValue)
{
    BOOL    bAccept;

    bAccept = isaccept(hex_accept_str, pszValue, 8) > 0 ? 1 : 0;
    if (!bAccept)
        return 0;
    return 1;
}

int check_hex_or_string(CLISESSION *pSession, char *pszValue)
{
    BOOL    bAccept;

    bAccept = isaccept(hex_accept_str, pszValue, 8) > 0 ? 1 : 0;
    if (bAccept) return 1;

    if(pszValue != NULL && strlen(pszValue) > 0) return 1;
	return 0;
}

int check_ifname(CLISESSION *pSession, char *pszValue)
{
	if (strlen(pszValue) > 10)
		return 0;

	return 1;
}

int check_allifname(CLISESSION *pSession, char *pszValue)
{
	if (strcmp(pszValue, "all") == 0)
		return 1;
	
	return check_ifname(pSession, pszValue);
}

int check_port(CLISESSION *pSession, char *pszValue)
{
	int		n;

	if (isaccept(num_accept_str, pszValue, 1) < 0)
		return 0;

	n = strtol(pszValue, (char **)NULL, 10);
	if ((n == 0) || (n > 4))
		return 0;

	return 1;
}

int check_metertype(CLISESSION *pSession, char *pszValue)
{
	int		n;

	if (isaccept(num_accept_str, pszValue, 3) < 0)
		return 0;

	n = strtol(pszValue, (char **)NULL, 10);
	if ((n == 0) || (n > 255))
		return 0;

	return 1;
}

int check_ip(CLISESSION *pSession, char *pszValue)
{
	int		i, nDotCount=0;
	if (isaccept(ip_accept_str, pszValue, 15) < 0)
		return 0;

	for(i=0; pszValue[i]; i++)
	{
		if (pszValue[i] == '.')
			nDotCount++;
		else if (strtol(&pszValue[i], (char **)NULL, 10) > 255)
			return 0;
	}

	return (nDotCount == 3) ? 1 : 0;
}

int check_pingip(CLISESSION *pSession, char *pszValue)
{
	int		i, nDotCount=0;
	if (strcasecmp(pszValue, "FEP") == 0) 
		return 1;
	if (isaccept(ip_accept_str, pszValue, 15) < 0)
		return 0;

	for(i=0; pszValue[i]; i++)
	{
		if (pszValue[i] == '.')
			nDotCount++;
		else if (strtol(&pszValue[i], (char **)NULL, 10) > 255)
			return 0;
	}

	return (nDotCount == 3) ? 1 : 0;
}

int check_adate(CLISESSION *pSession, char *pszValue)
{
	char	szYY[5], szMM[5], szDD[5];

	if (strcmp(pszValue, "all") == 0)
		return 1;

	if (strlen(pszValue) != 8)
		return 0;

	memcpy(szYY, pszValue, 4); szYY[4] = '\0';
	memcpy(szMM, &pszValue[4], 2); szMM[2] = '\0';
	memcpy(szDD, &pszValue[6], 2); szDD[2] = '\0';
	if (strtol(szYY, (char **)NULL, 10) < 2005)
		return 0;

	if ((strtol(szMM, (char **)NULL, 10) <= 0) || (strtol(szMM, (char **)NULL, 10) > 12))
		return 0;
			
	if ((strtol(szDD, (char **)NULL, 10) <= 0) || (strtol(szDD, (char **)NULL, 10) > 31))
		return 0;

	return 1;
}

int check_id64(CLISESSION *pSession, char *pszValue)
{
	return strlen(pszValue) == 16 ? 1 : 0;
}

int check_ethernet(CLISESSION *pSession, char *pszValue)
{
	if (strcmp(pszValue, "lan") == 0)
		return 1;

	if (strcmp(pszValue, "dhcp") == 0)
		return 1;

	if (strcmp(pszValue, "adsl") == 0)
		return 1;
	
	if (strcmp(pszValue, "vdsl") == 0)
		return 1;
	
	if (strcmp(pszValue, "cable") == 0)
		return 1;
	
	if (strcmp(pszValue, "ppp") == 0)
		return 1;
	
	return 0;
}

int check_mobile(CLISESSION *pSession, char *pszValue)
{
	if (strcmp(pszValue, "growell") == 0)
		return 1;

	if (strcmp(pszValue, "anydata") == 0)
		return 1;

	return 0;
}

int check_coretype(CLISESSION *pSession, char *pszValue)
{
	if (!strcasecmp(pszValue, "modem") || !strcasecmp(pszValue, "core"))
    {
		return 1;
    }

    return check_number(pSession, pszValue);
}

int check_schedulertype(CLISESSION *pSession, char *pszValue)
{
	if (!strncasecmp(pszValue, "mask", 4) || !strncasecmp(pszValue, "int", 3))
    {
		return 1;
    }

    return check_number(pSession, pszValue);
}

int check_channel(CLISESSION *pSession, char *pszValue)
{
	int		n;

	if (isaccept(num_accept_str, pszValue, 3) < 0)
		return 0;

	n = strtol(pszValue, (char **)NULL, 10);
	if ((n < 11) || (n > 26))
		return 0;

	return 1;
}

int check_panid(CLISESSION *pSession, char *pszValue)
{
	return isaccept(hex_accept_str, pszValue, 4) > 0 ? 1 : 0;
}

int check_power(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if (n > 3 || n < -9) {
        switch(n) {
            case -11: case -12: case -14: case -17:
            case -20: case -26: case -43:
                return 1;
            default:
                return 0;
        }
    }

	return 1;
}

int check_epower(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if (n > -5 || n < -9) {
        switch(n) {
            case -11: case -12: case -14: case -17:
            case -20: case -26: case -43:
                return 1;
            default:
                return 0;
        }
    }

	return 1;
}

int check_routing(CLISESSION *pSession, char *pszValue)
{
	if (strcmp(pszValue, "disable") == 0)
		return 1;

	if (strcmp(pszValue, "enable") == 0)
		return 1;

	return 0;
}

int check_commtype(CLISESSION *pSession, char *pszValue)
{
	if (strcmp(pszValue, "dgrm") == 0)
		return 1;

	if (strcmp(pszValue, "seq") == 0)
		return 1;

	return 0;
}

int check_speed(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	switch(n) {
	  case 300 :
	  case 600 :
	  case 1200 :
	  case 2400 :
	  case 4800 :
	  case 9600 :
	  case 14400 :
	  case 19200 :
	  case 38400 :
		   return 1;
	}
	return 0;
}

int check_databit(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if ((n != 7) && (n != 8))
		return 0;

	return 1;
}

int check_parity(CLISESSION *pSession, char *pszValue)
{
	if (strcmp(pszValue, "none") == 0)
		return 1;

	if (strcmp(pszValue, "odd") == 0)
		return 1;

	if (strcmp(pszValue, "even") == 0)
		return 1;

	return 0;
}

int check_stopbit(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if ((n != 1) && (n != 2))
		return 0;

	return 1;
}

int check_timeout(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if ((n < 0) || (n > 255))
		return 0;

	return 1;
}

int check_keepalive(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if ((n < 0) || (n > 255))
		return 0;

	return 1;
}

int check_lbt(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if ((n < 10) || (n > 255))
		return 0;

	return 1;
}

int check_address(CLISESSION *pSession, char *pszValue)
{
	char	*p;
	int		nGroup, nMember;

	p = strchr(pszValue, '.');
	if (p == NULL)
		return 0;

	nGroup  = strtol(pszValue, (char **)NULL, 10);
	nMember = strtol(p+1, (char **)NULL, 10);

	if ((nGroup >= 255) || (nGroup <= 0))
		return 0;

	if ((nMember >= 255) || (nMember <= 0))
		return 0;
	return 1;
}

int check_thread(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if ((n < 1) || (n > 5))
		return 0;

	return 1;
}

int check_table(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if (strcasecmp(pszValue, "default") == 0)
		n = -1;
	else if (strcasecmp(pszValue, "selfread") == 0)
		n = 0;
	else if (strcasecmp(pszValue, "current") == 0)
		n = 1;
	else if (strcasecmp(pszValue, "previous") == 0)
		n = 2;
	else if (strcasecmp(pszValue, "relay") == 0)
		n = 3;
	else if (strcasecmp(pszValue, "event") == 0)
		n = 10;
	else if (strcasecmp(pszValue, "measurement") == 0)
		n = 12;
	else if (strcasecmp(pszValue, "demand") == 0)
		n = 16;

    switch(n) {
        case -1: case 0: case 1: case 2: case 3:
        case 10: case 12: case 16:
            return 1;
        default:
            return 0;
    }
}

int check_mstrategy(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if (strcasecmp(pszValue, "serial") == 0)
		n = 0;
	if (strcasecmp(pszValue, "distribute") == 0)
		n = 1;

	if (n > 1 || n < 0) return 0;

	return 1;
}

int check_sstrategy(CLISESSION *pSession, char *pszValue)
{
	int		n;

	n = strtol(pszValue, (char **)NULL, 10);
	if (strcasecmp(pszValue, "lazy") == 0)
		n = 0;
	if (strcasecmp(pszValue, "immediately") == 0)
		n = 1;

	if (n > 1 || n < 0) return 0;

	return 1;
}

int check_version(CLISESSION *pSession, char *pszValue)
{
    int major=0,minor=0;

    if(!check_string(pSession, pszValue)) return 0;

    if(sscanf(pszValue,"%d.%d", &major, &minor) != 2) return 0;

	return 1;
}

int check_networktype(CLISESSION *pSession, char *pszValue)
{
	if (!strcasecmp(pszValue, "star")) return 1;
	if (!strcasecmp(pszValue, "mesh")) return 1;

    return check_number(pSession, pszValue);
}

int check_mtortype(CLISESSION *pSession, char *pszValue)
{
	if (!strcasecmp(pszValue, "high")) return 1;
	if (!strcasecmp(pszValue, "low")) return 1;

    return check_number(pSession, pszValue);
}

int check_troption(CLISESSION *pSession, char *pszValue)
{
	if (!strcasecmp(pszValue, "default")) return 1;
	if (!strcasecmp(pszValue, "queue")) return 1;
	if (!strcasecmp(pszValue, "wait")) return 1;
	if (!strcasecmp(pszValue, "run")) return 1;
	if (!strcasecmp(pszValue, "w+q+r")) return 1;
	if (!strcasecmp(pszValue, "terminate")) return 1;
	if (!strcasecmp(pszValue, "all")) return 1;

    return check_number(pSession, pszValue);
}

