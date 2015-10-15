#ifndef __METER_UPLOADER_H__
#define __METER_UPLOADER_H__

#include "TimeoutThread.h"
#include "MeterWriter.h"
#include "JobObject.h"

#define UPLOAD_TYPE_DEFAULT			0
#define UPLOAD_TYPE_IMMEDIATELY		1			// 지원 안함.
#define UPLOAD_TYPE_DAILY			2
#define UPLOAD_TYPE_WEEKLY			3
#define UPLOAD_TYPE_HOURLY			4

#define MAX_UPLOAD_FILE_COUNT       100

class CMeterUploader : public CTimeoutThread, public CJobObject
{
public:
	CMeterUploader();
	virtual ~CMeterUploader();

// Operations
public:
    BOOL    Initialize();
    void    Destroy();
	void	Upload(TIMESTAMP *pStart=NULL, TIMESTAMP *pEnd=NULL);
    void    ImmediatelyUpload();

	BOOL	CheckUploadTime();
	void	UploadMeteringData();
	void	ReduceDataDirectory();

public:
	void	GetUploadRange(TIMESTAMP *pStart, TIMESTAMP *pEnd, BOOL bUserRequest);
	void	SortFileList(char **pList, int nCount);
	int		UploadFileList(char **pList, int nCount);
    BOOL    SetBusyFile(char * pszFileName);
    void    ResetBusyFile(char * pszFileName);

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

    void    UploadComplete(char * pszFileName);
    void    GetUploadRange(TIMESTAMP *pStart, TIMESTAMP *pEnd);

protected:
	BOOL		m_bUserRequest;
	BOOL		m_bImmediatelyRequest;
	TIMESTAMP	m_tmUserStart;
	TIMESTAMP	m_tmUserEnd;
	time_t		m_nLastReduce;
	time_t		m_nLastUpload;
	char		*m_pList[MAX_UPLOAD_FILE_COUNT];
	char		m_szBuffer[1024];

	char		m_szBusyFile[1024];
    CLocker     m_Locker;
};

extern CMeterUploader	*m_pMeterUploader;

#endif	// __METER_UPLOADER_H__
