#ifndef __PACKAGE_UTIL_H__
#define __PACKAGE_UTIL_H__

#include "OtaWorker.h"
#include "Chunk.h"

int		FileDownload(const char *pszUrl, const char *pszName, const char *pszChecksum, const char *pszPath, BOOL &bExit);
BOOL	MD5Checksum(const char *pszFileName, const char *pszChecksum);
BOOL	DiffMerge(const char *pszSeedFile, const char *pszDiffFile, const char *pszNewFile);

int		LoadFirmware(CChunk *pChunk, const char *pszFileName);

BOOL	LoadPackageInformation(PACKAGEINFO *pPackage, const char *pszFileName);
BOOL	SavePackageInformation(PACKAGEINFO *pPackage, const char *pszFileName);

int		GetOtaError(int nError, int nDefault);
void	ConvertModelName(const char *pszModel, char *pszName);
const char	*GetParserName(const char *pszName);

const char	*GetOtaTypeString(int nType);
const char	*GetOtaInstallTypeString(int nType);
const char	*GetOtaTransferTypeString(int nType);
void	GetTimeString(char *pszBuffer, int nSecond);
const char 	*GetOtaErrorString(int nError);

#endif	// __PACKAGE_UTIL_H__
