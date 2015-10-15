#include "common.h"
#include "PackageUtil.h"
#include "Distributor.h"
#include "MeterParserFactory.h"
#include "ShellCommand.h"
#include "CommonUtil.h"
#include "Utility.h"
#include "DebugUtil.h"
#include "Locker.h"
#include "codiapi.h"

CLocker	m_firmwareLocker;

const char	*m_arAcceptCharSet = { ".-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };

//////////////////////////////////////////////////////////////////////
// Member functions 
//////////////////////////////////////////////////////////////////////

int FileDownload(const char *pszUrl, const char *pszName, const char *pszChecksum, const char *pszPath, BOOL &bExit)
{
	char	szFileName[128];
	char	szCommand[256];
    int 	pid, status;
    
	XDEBUG("--------------- Download --------------\r\n");   
	XDEBUG("Name     = %s\r\n", pszName);   
	XDEBUG("Path     = %s\r\n", pszPath);   
	XDEBUG("URL      = %s\r\n", pszUrl);   
	XDEBUG("Checksum = %s\r\n", pszChecksum);   
	XDEBUG("---------------------------------------\r\n");   

    pid = fork();
    if (pid < 0)
	{
        printf("\r\n**** fork error ****\r\n");
        return -1;
	}
	else if (pid == 0)
	{
		// 파일 다운로드를 실행한다.
		sprintf(szFileName, "%s/%s", pszPath, pszName); 
		remove(szFileName);
		if (strncmp(pszUrl, "file://", 7) == 0)
		{
			sprintf(szCommand, "/bin/cp %s %s", &pszUrl[7], szFileName);
			system(szCommand);
	    }     
		else execl("/usr/bin/wget", "wget", "-O", szFileName, pszUrl, NULL);
		exit(0);
	}

	// 다운로드가 완료될때까지 대기한다.
    while(waitpid(pid, &status, WNOHANG) == 0)
	{
		if (bExit)
		{
			// 강제 종료 요청이 오면 다운로드를 실패 시킨다.
			kill(pid, SIGTERM);
			return -4;
		}
        usleep(1000000);
	}

	// 파일이 존재하는지 검사한다.
	sprintf(szFileName, "%s/%s", pszPath, pszName); 
	if (!IsExists(szFileName))
	{
		XDEBUG("File download fail: File not found.\r\n");
		return -2;
	}

	// Checksum을 확인하여 잘못된 경우 다운로드한 파일을 지운다.
	if (!MD5Checksum(szFileName, pszChecksum))
	{
		XDEBUG("File download fail: MD5 checksum error!\r\n");
		remove(szFileName);
		return -3;
	}

	XDEBUG("Download ok\r\n", pszName);   
	return 0;
}

BOOL MD5Checksum(const char *pszFileName, const char *pszChecksum)
{
	char	szCommand[128];
	char	szBuffer[33];
	FILE	*fp;

	sprintf(szCommand, "md5sum %s > /tmp/checksum", pszFileName);
	system(szCommand);

	fp = fopen("/tmp/checksum", "rb");
	if (fp == NULL)
		return FALSE;

	memset(szBuffer, 0, 33);
	fread(szBuffer, 32, 1, fp);
	fclose(fp);
	remove("/tmp/checksum");

	if (strcmp(szBuffer, pszChecksum) != 0)
	{
		XDEBUG("Command checksum  = %s\r\n", pszChecksum);
		XDEBUG("Download checksum = %s\r\n", szBuffer);
		return FALSE;
	}
	return TRUE;
}

BOOL DiffMerge(const char *pszSeedFile, const char *pszDiffFile, const char *pszNewFile)
{
	char	szCommand[256];

	// 파일이 존재하는지 검사한다.
    if (!IsExists(pszSeedFile))
	{
		XDEBUG("SEED file not found: %s\r\n", pszSeedFile);
		return FALSE;
	}
	else if (!IsExists(pszDiffFile))
	{
		XDEBUG("DIFF file not found: %s\r\n", pszDiffFile);
		return FALSE;
	}

	// 기존에 존재하는 파일이 있으면 삭제한다.
	if (IsExists(pszNewFile))
		remove(pszNewFile);

	// Patch를 수행한다.
	sprintf(szCommand, "/app/sw/bspatch %s %s %s", pszSeedFile, pszNewFile, pszDiffFile);
	if(system(szCommand)) 
    {
		XDEBUG("Merge fail %s %s %s\r\n", pszSeedFile, pszNewFile, pszDiffFile);
        return FALSE;
    }
	XDEBUG("Merge success %s %s %s\r\n", pszSeedFile, pszNewFile, pszDiffFile);
	return TRUE;
}

int LoadFirmware(CChunk *pChunk, const char *pszFileName)
{
	char	*pszBuffer;
	FILE	*fp;
	int		n, nLength=0;

	m_firmwareLocker.Lock();
	fp = fopen(pszFileName, "rb");
	if (fp != NULL)
	{
		pszBuffer = (char *)malloc(4096);
		if (pszBuffer != NULL)
		{
			while((n=fread(pszBuffer, 1, 4096, fp)))
			{
				pChunk->Add(pszBuffer, n);
				nLength += n;
			}
			free(pszBuffer);
		}
		fclose(fp);
	}
	m_firmwareLocker.Unlock();

    XDEBUG("LoadFirmware : %s %d bytes %d length\r\n", pszFileName, pChunk->GetSize(), nLength);
	return nLength;
}

BOOL LoadPackageInformation(PACKAGEINFO *pPackage, const char *pszFileName)
{
	FILE	*fp;
	int		n;

	if (!pPackage || !pszFileName || !*pszFileName)
		return FALSE;

	fp = fopen(pszFileName, "rb");
	if (fp == NULL)
		return FALSE;

	memset(pPackage, 0, sizeof(PACKAGEINFO));
	n = fread(pPackage, 1, sizeof(PACKAGEINFO), fp);

	fclose(fp);
	return (n == sizeof(PACKAGEINFO)) ? TRUE : FALSE;
}

BOOL SavePackageInformation(PACKAGEINFO *pPackage, const char *pszFileName)
{
	FILE	*fp;

	if (!pPackage || !pszFileName || !*pszFileName)
		return FALSE;

	fp = fopen(pszFileName, "wb");
	if (fp == NULL)
		return FALSE;

	fwrite(pPackage, sizeof(PACKAGEINFO), 1, fp);

	fclose(fp);
	return TRUE;
}

int	GetOtaError(int nError, int nDefault)
{
	switch(nError) {
	  case CODIERR_NOERROR :
		   return OTAERR_NOERROR;
	  case CODIERR_MEMORY_ERROR :
		   return OTAERR_MEMORY_ERROR;
	  case CODIERR_INVALID_DEVICE :
	  case CODIERR_DEVICE_BUSY :
	  case CODIERR_DEVICE_INUSE :
	  case CODIERR_NOT_INITIALIZED :
	  case CODIERR_NOT_REGISTERED :
	  case CODIERR_NOT_STARTED :
	  case CODIERR_INVALID_INTERFACE :
	  case CODIERR_DEVICE_ERROR :
	  case CODIERR_NOT_CONNECTED :
	  case CODIERR_COORDINATOR_NOT_READY :
		   return OTAERR_COORDINATOR_ERROR;
	}

	return nDefault;
}

// 모델명을 저장 가능한 파일명으로 변환한다.
void ConvertModelName(const char *pszModel, char *pszName)
{
	int		i, len, nSeek=0;

	if (!pszModel || !pszName)
		return;

	len = strlen(pszModel);
	for(i=0; i<len; i++)
	{
		if (strchr(m_arAcceptCharSet, pszModel[i]) != NULL)
		{
			pszName[nSeek] = pszModel[i];
			nSeek++;
		}
	}
	pszName[nSeek] = '\0';
}

const char *GetParserName(const char *pszModel)
{
	CMeterParser	*pParser;
	
	pParser = m_pMeterParserFactory->SelectParser(pszModel);
	if (pParser == NULL)
		return "UNKNOWN";

	return pParser->GetName();
}

const char *GetOtaTypeString(int nType)
{
	switch(nType) {
	  case OTA_TYPE_CONCENTRATOR :	return "CONCENTRATOR";
	  case OTA_TYPE_SENSOR :		return "SENSOR";
	  case OTA_TYPE_COORDINATOR :	return "COORDINATOR";
	}
	return "UNKNOWN";
}

const char *GetOtaInstallTypeString(int nType)
{
	switch(nType) {
	  case OTA_INSTALL_AUTO :		return "AUTO";
	  case OTA_INSTALL_REINSTALL :	return "REINSTALL";
	  case OTA_INSTALL_MATCH :		return "MATCH";
	}
	return "Unknown";
}

const char *GetOtaTransferTypeString(int nType)
{
	switch(nType) {
	  case OTA_TRANSFER_AUTO :		return "AUTO";
	  case OTA_TRANSFER_MULTICAST :	return "MULTICAST";
	  case OTA_TRANSFER_UNICAST :	return "UNICAST";
	}
	return "Unknown";
}

void GetTimeString(char *pszBuffer, int nSecond)
{
	if (nSecond >= 3600)
	 	 sprintf(pszBuffer, "%0d hour %0d min %0d sec", nSecond/3600, (nSecond%3600)/60, nSecond%60);
	else if (nSecond >= 60)
	 	 sprintf(pszBuffer, "%0d min %0d sec", nSecond/60, nSecond%60);
	else sprintf(pszBuffer, "%0d sec", nSecond);
}

const char *GetOtaErrorString(int nError)
{
	switch(nError) {
	  case OTAERR_NOERROR :					return "";
	  case OTAERR_OPEN_ERROR :				return "OPEN ERROR";
	  case OTAERR_CONNECT_ERROR :			return "CONNECT ERROR";
	  case OTAERR_INVENTORY_SCAN_ERROR :	return "INVENTORY SCAN ERROR";
	  case OTAERR_FIRMWARE_NOT_FOUND :		return "FIRMWARE NOT FOUND";
	  case OTAERR_SEND_ERROR :				return "SEND ERROR";
	  case OTAERR_VERIFY_ERROR :			return "VERIFY ERROR";	
	  case OTAERR_INSTALL_ERROR	:			return "INSTALL ERROR";
	  case OTAERR_USER_CANCEL :				return "USER CANCEL";
	  case OTAERR_DIFFERENT_MODEL_ERROR :	return "MODEL ERROR";
	  case OTAERR_MATCH_VERSION_ERROR :		return "VERSION ERROR";
	  case OTAERR_VERSION_ERROR :			return "VERSION ERROR";
	  case OTAERR_MODEL_ERROR :				return "MODEL ERROR";	
	  case OTAERR_MEMORY_ERROR :			return "MEMORY ERROR";
	  case OTAERR_COORDINATOR_ERROR :		return "COORDINATOR ERROR";
	}
	return "UNKOWN ERROR";
}

