#include "common.h"
#include <sys/stat.h>
#include <dirent.h>
#include "mcudef.h"
#include "CommonUtil.h"
#include "CommandHandler.h"
#include "McuService.h"
#include "Variable.h"
#include "DebugUtil.h"
#include "Chunk.h"
#include "Utils.h"

extern BOOL m_bKetiDemo;

//////////////////////////////////////////////////////////////////////
// Command Table
//////////////////////////////////////////////////////////////////////

IF4_COMMAND_TABLE	m_CommandHandler[] =
{
	{ {111,1,0},	"cmdBypassData",            cmdBypassData },
	{ {130,7,0},	"cmdKpxTest",               cmdKpxTest },
	{ {198,1,0},	"cmdGetFile",				cmdGetFile },
	{ {198,2,0},	"cmdPutFile",				cmdPutFile },
	{ {198,3,0},	"cmdInstallFile",			cmdInstallFile },
	{ {0,0,0},		NULL,						NULL }
};

//////////////////////////////////////////////////////////////////////
// File Transfer Command
//////////////////////////////////////////////////////////////////////

int cmdGetFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
#if 0
	char		*pszFileName;
	struct 		stat file_info;
	TIMESTAMP	tmFile;
	char		*pszBuffer;
	struct		tm when;	
	int			fd, len, flen=0;

	if ((cnt != 1) || (pValue->type != VARSMI_STRING))
		return IF4ERR_INVALID_PARAM;

	pszFileName = (char *)pValue->stream.p;
	if (stat(pszFileName, &file_info) == -1)
		return IF4ERR_INVALID_FILENAME;

	XDEBUG("Length = %0d\xd\xa", file_info.st_size);
	XDEBUG("Mode   = %0d\xd\xa", file_info.st_mode);

	fd = open(pszFileName, O_RDONLY);
	if (fd <= 0)
		return IF4ERR_INVALID_FILENAME;

	when 	    = *localtime(&file_info.st_mtime);
	tmFile.year	= when.tm_year + 1900;
	tmFile.mon	= when.tm_mon + 1,
	tmFile.day  = when.tm_mday;
	tmFile.hour = when.tm_hour;
	tmFile.min	= when.tm_min;
	tmFile.sec	= when.tm_sec;

	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, (UINT)file_info.st_size);
	IF4API_AddResultNumber(pWrapper, "1.6", VARSMI_UINT, (UINT)file_info.st_mode);
	IF4API_AddResultFormat(pWrapper, "1.16", VARSMI_TIMESTAMP, &tmFile, sizeof(TIMESTAMP));

	flen = min(50000, file_info.st_size);
	pszBuffer = (char *)malloc(flen);
	if (pszBuffer != NULL)
	{
		while((len=read(fd, pszBuffer, flen)) > 0)
			IF4API_AddResultFormat(pWrapper, "1.12", VARSMI_STREAM, pszBuffer, len);
		free(pszBuffer);
	}

#endif
	return IF4ERR_NOERROR;
}

int cmdPutFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
#if 0
	char		*p, *pszFileName;
	char		szPath[128];
	TIMESTAMP	*pTime;
	int			i, fd, nMode, nSum=0;

	if (cnt < 5)
		return IF4ERR_INVALID_PARAM;

	if ((pValue[0].type != VARSMI_STRING) ||
		(pValue[1].type != VARSMI_UINT) ||
		(pValue[2].type != VARSMI_UINT) ||
		(pValue[3].type != VARSMI_TIMESTAMP) ||
		(pValue[4].type != VARSMI_STREAM))
		return IF4ERR_INVALID_PARAM;

	nSum = 0;
	for(i=4; i<cnt; i++)
		nSum += pValue[i].len;

	if (pValue[1].stream.u32 != (UINT)nSum)
		return IF4ERR_INVALID_LENGTH;	

	pszFileName = (char *)pValue[0].stream.p;
	pTime = (TIMESTAMP *)pValue[3].stream.p;
	nMode = (int)pValue[2].stream.u32;

	p = strrchr(pszFileName, '/');
	if (p != NULL)
	{
		strcpy(szPath, pszFileName);
		p = strrchr(szPath, '/');
		*p = '\0';
		mkdir(szPath, 0755);
	}

	fd = open(pszFileName, O_CREAT|O_WRONLY, nMode);
	if (fd <= 0)
		return IF4ERR_INVALID_FILENAME;

	for(i=4; i<cnt; i++)
		write(fd, pValue[i].stream.p, pValue[i].len);
	close(fd);
#endif
	return IF4ERR_NOERROR;
}

int cmdInstallFile(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
#if 0
	char		szFileName[128];
	char		szCommand[128];
	char		*pszName;
	TIMESTAMP	*pTime;
	int			i, fd, nSum, nMode;

	if (cnt < 7)
		return IF4ERR_INVALID_PARAM;

	if ((pValue[0].type != VARSMI_STRING) ||
		(pValue[1].type != VARSMI_UINT) ||
		(pValue[2].type != VARSMI_UINT) ||
		(pValue[3].type != VARSMI_TIMESTAMP) ||
		(pValue[6].type != VARSMI_STREAM))
		return IF4ERR_INVALID_PARAM;

	nSum = 0;
	for(i=6; i<cnt; i++)
		nSum += pValue[i].len;

	if (pValue[1].stream.u32 != (UINT)nSum)
		return IF4ERR_INVALID_LENGTH;	

	pszName = (char *)pValue[0].stream.p;
	if ((strncmp(pszName, "URBAN_", 6) != 0) &&
	    (strncmp(pszName, "RURAL_", 6) != 0))
		return IF4ERR_INVALID_FILENAME;

	if (strstr(pszName, ".tar.gz") == 0)
		return IF4ERR_INVALID_FILENAME;	

	pTime = (TIMESTAMP *)pValue[3].stream.p;
	nMode = pValue[2].stream.u32;

	XDEBUG("\xd\xa");
	XDEBUG("FIRMWARE: install '%s'.\xa\xd\xa", pValue[0].stream.p);	
	sprintf(szFileName, "/tmp/%s", pValue[0].stream.p);
	fd = open(szFileName, O_CREAT|O_WRONLY, nMode);
	if (fd <= 0)
		return IF4ERR_INVALID_FILENAME;

	for(i=6; i<cnt; i++)
		write(fd, pValue[i].stream.p, pValue[i].len);
	close(fd);

	// Change File Mode
	sprintf(szCommand, "/bin/chmod 766 %s", szFileName);
	system(szCommand);

	// Uncompress File
	chdir("/app/sw");
	sprintf(szCommand, "/bin/tar zxvf %s", szFileName);
	system(szCommand);
	system("/bin/chown -R root:root *");
	system("/bin/chmod -R 777 *");

	// Delete File
	unlink(szFileName);

	XDEBUG("FIRMWARE: install done.\xd\xa");	
#endif
	return IF4ERR_NOERROR;
}

int cmdKpxTest(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    XDEBUG("cmdKpxTest call\r\n");
    XDEBUG("Param Count : %d\r\n", cnt);

	return IF4ERR_NOERROR;
}

int cmdBypassData(IF4Wrapper *pWrapper, MIBValue *pValue, int cnt)
{
    char        szGUID[17];
    UINT        trSeq;
    BYTE        seq;
    WORD        len;
    char *      szData;
    UINT        invokeTime;
    UINT        waitingGap;
    TIMESTAMP   stamp;

    XDEBUG("Param Cnt : %d\r\n", cnt);
    if(cnt < 7) return IF4ERR_INVALID_PARAM;

    EUI64ToStr((EUI64 *)pValue[0].stream.p, szGUID);
    trSeq = pValue[1].stream.u32;
    seq = pValue[2].stream.u8;
    len = pValue[3].stream.u16;
    szData = (char *)pValue[4].stream.p;
    invokeTime = pValue[5].stream.u32;
    waitingGap = pValue[6].stream.u32;

    GetTimestamp(&stamp, (time_t *)&invokeTime);
    XDEBUG("%04d/%02d/%02d %02d:%02d:%02d %s trSeq=%d seq=%d len=%d wait=%d\r\n",
            stamp.year, stamp.mon, stamp.day, stamp.hour, stamp.day, stamp.min,
            szGUID, trSeq, seq, len, waitingGap);
    XDUMP(szData, len, TRUE);
    
    if(m_bKetiDemo)
    {
        DumpKetiRequest("111.1", "cmdBypassData", pValue, cnt);
    }
    else
    {
        DumpRequest("111.1", "cmdBypassData", pValue, cnt);
    }

	return IF4ERR_NOERROR;
}
