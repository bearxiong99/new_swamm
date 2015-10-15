#ifndef __MOBILE_UTILITY_H__
#define __MOBILE_UTILITY_H__

void MOBILE_Initialize();

BOOL MOBILE_IsBusy();
BOOL MOBILE_Lock();
void MOBILE_Unlock();
BOOL MOBILE_GetMobileProperty();

BOOL UploadFile(const char *pszFileName, int nSeek=0);

#endif	// __MOBILE_UTILITY_H__
