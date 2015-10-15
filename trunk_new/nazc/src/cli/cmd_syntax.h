#ifndef __CMD_SYNTAX_H__
#define __CMD_SYNTAX_H__

int check_oid(CLISESSION *pSession, char *pszValue);

int check_opcode(CLISESSION *pSession, char *pszValue);
int check_opmode(CLISESSION *pSession, char *pszValue);
int check_group(CLISESSION *pSession, char *pszValue);
int check_number(CLISESSION *pSession, char *pszValue);
int check_unsigned(CLISESSION *pSession, char *pszValue);
int check_byte(CLISESSION *pSession, char *pszValue);
int check_float(CLISESSION *pSession, char *pszValue);
int check_string(CLISESSION *pSession, char *pszValue);
int check_id(CLISESSION *pSession, char *pszValue);
int check_allid(CLISESSION *pSession, char *pszValue);
int check_idparser(CLISESSION *pSession, char *pszValue);
int check_hex(CLISESSION *pSession, char *pszValue);
int check_hex_or_string(CLISESSION *pSession, char *pszValue);
int check_ifname(CLISESSION *pSession, char *pszValue);
int check_allifname(CLISESSION *pSession, char *pszValue);
int check_port(CLISESSION *pSession, char *pszValue);
int check_metertype(CLISESSION *pSession, char *pszValue);
int check_elmetertype(CLISESSION *pSession, char *pszValue);
int check_ip(CLISESSION *pSession, char *pszValue);
int check_pingip(CLISESSION *pSession, char *pszValue);
int check_id64(CLISESSION *pSession, char *pszValue);
int check_ethernet(CLISESSION *pSession, char *pszValue);
int check_mobile(CLISESSION *pSession, char *pszValue);
int check_adate(CLISESSION *pSession, char *pszValue);
int check_agroup(CLISESSION *pSession, char *pszValue);
int check_onoff(CLISESSION *pSession, char *pszValue);
int check_otatype(CLISESSION *pSession, char *pszValue);
int check_cputype(CLISESSION *pSession, char *pszValue);
int check_format(CLISESSION *pSession, char *pszValue);
int check_mccb(CLISESSION *pSession, char *pszValue);
int check_version(CLISESSION *pSession, char *pszValue);
int check_devtype(CLISESSION *pSession, char *pszValue);
int check_getmethod(CLISESSION *pSession, char *pszValue);
int check_channel(CLISESSION *pSession, char *pszValue);
int check_panid(CLISESSION *pSession, char *pszValue);
int check_power(CLISESSION *pSession, char *pszValue);
int check_epower(CLISESSION *pSession, char *pszValue);
int check_routing(CLISESSION *pSession, char *pszValue);
int check_commtype(CLISESSION *pSession, char *pszValue);
int check_speed(CLISESSION *pSession, char *pszValue);
int check_databit(CLISESSION *pSession, char *pszValue);
int check_parity(CLISESSION *pSession, char *pszValue);
int check_stopbit(CLISESSION *pSession, char *pszValue);
int check_timeout(CLISESSION *pSession, char *pszValue);
int check_keepalive(CLISESSION *pSession, char *pszValue);
int check_lbt(CLISESSION *pSession, char *pszValue);
int check_address(CLISESSION *pSession, char *pszValue);
int check_thread(CLISESSION *pSession, char *pszValue);
int check_table(CLISESSION *pSession, char *pszValue);
int check_coretype(CLISESSION *pSession, char *pszValue);

int check_hex8(CLISESSION *pSession, char *pszValue);
int check_hex16(CLISESSION *pSession, char *pszValue);
int check_hex_list(CLISESSION *pSession, char *pszValue);
int check_enable(CLISESSION *pSession, char *pszValue);
int check_mstrategy(CLISESSION *pSession, char *pszValue);
int check_sstrategy(CLISESSION *pSession, char *pszValue);
int check_severity(CLISESSION *pSession, char *pszValue);
int check_networktype(CLISESSION *pSession, char *pszValue);
int check_mtortype(CLISESSION *pSession, char *pszValue);
int check_troption(CLISESSION *pSession, char *pszValue);
int check_schedulertype(CLISESSION *pSession, char *pszValue);

#ifdef __SUPPORT_KUKDONG_METER__
int check_opvalve(CLISESSION *pSession, char *pszValue);
#endif

#endif	// __CMD_SYNTAX_H__

