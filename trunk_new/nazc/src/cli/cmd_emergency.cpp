#include "common.h"
#include "cmd_emergency.h"
#include "cmd_show.h"
#include "cmd_util.h"
#include "CLIInterface.h"
#include "IcmpPing.h"
#include "Chunk.h"

extern UINT m_nLocalAgentPort;

/////////////////////////////////////////////////////////////////////////////
// Enable/Disable Function
/////////////////////////////////////////////////////////////////////////////

int cmdEmergencyReset(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (!Confirm(pSession, MSG_CMD_EMERG_RESET_MCU))
		return CLIERR_OK;
	
	OUTTEXT(pSession, MSG_RUN_EMERG_RESET_MCU "\r\n");

    ResetSystem();
	for(;;) usleep(1000000);
	return CLIERR_OK;
}

int cmdEmergencyPing(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CIcmpPing   ping;
	struct      timeval atime;
	struct      sockaddr_in addr;
	int         i, ret;

	ping.IcmpInit();
	for(i=0; i<3; i++)
	{
		ret = ping.CheckIcmp(argv[0], &atime, &addr);
		if (ret != -1)
		{
    		OUTTEXT(pSession, "ICMP: Reply from %s: %d sec %d msc.\xd\xa",
					argv[0], atime.tv_sec, atime.tv_usec);
		}
		else
		{
			OUTTEXT(pSession, "ICMP: Request timed out. (%s)\xd\xa", argv[0]);
		}
	}
	ping.IcmpClose();
	return CLIERR_OK;
}

int cmdEmergencyFirmwareDownload(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	if (!Confirm(pSession, MSG_CMD_FIRMWARE_UPDATE))
		return CLIERR_OK;

    OUTTEXT(pSession, "\r\n" MSG_INFO_FIRMWARE_UPDATE "\r\n");

	if (!CLIAPI_FirmwareDownload(pSession))
		return CLIERR_OK;
    OUTTEXT(pSession, "\r\n" MSG_RUN_FIRMWARE_UPDATE "\r\n");

    OUTTEXT(pSession, "\xd\xaWARNING: Firmware Install.\xd\xa");
    system("/app/sw/install > /tmp/update.log");
    unlink(FIRMWARE_COMPRESS_FILE);
    system("/bin/sync");
    usleep(1000000);
    system("/bin/sync");
    OUTTEXT(pSession, "\xd\xaInstall Done.\xd\xa");

    ResetSystem();
	return CLIERR_OK;
}

int cmdEmergencyShell(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	CChunk	chunk;
	char	*p;
	int		nSeek, len, length;

	if (GetShellCommandResult(&chunk, argv[0]))
	{
    	OUTTEXT(pSession, MSG_INFO_INVALID_CMD "\r\n");
		return CLIERR_OK;
	}

	chunk.Add("\x0", 1);
	p = chunk.GetBuffer();
	len = chunk.GetSize();
	
	for(nSeek=0; nSeek<len; nSeek+=256)
	{
		length = MIN(256, len-nSeek);
    	PUT(pSession, p+nSeek, length);
	}
	return CLIERR_OK;
}

int cmdEmergencyKillProcess(CLISESSION *pSession, int argc, char **argv, void *pHandler)
{
	char	szCommand[256];
	int		i;

	strcpy(szCommand, "/bin/kill");
	for(i=0; i<argc; i++)
	{
		strcat(szCommand, " ");
		strcat(szCommand, argv[i]);
	}
	system(szCommand);
	return CLIERR_OK;
}

