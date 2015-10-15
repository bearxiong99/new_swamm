#ifndef __AGENT_DEBUG_LOG_H__
#define __AGENT_DEBUG_LOG_H__

#include "DebugUtil.h"
#include "logdef.h"


#define TIMESYNC_LOG( ...)      UpdateLogFile(LOG_DIR, TIMESYNC_LOG_FILE, m_nTimesyncLogSize, TRUE, __VA_ARGS__)
#define UPLOAD_LOG( ...)        UpdateLogFile(LOG_DIR, UPLOAD_LOG_FILE, m_nUploadLogSize, TRUE, __VA_ARGS__)
#define MOBILE_LOG( ...)        UpdateLogFile(LOG_DIR, MOBILE_LOG_FILE, m_nMobileLogSize, TRUE, __VA_ARGS__)
#define METERING_LOG( ...)      UpdateLogFile(LOG_DIR, METERING_LOG_FILE, m_nMeterLogSize, TRUE, __VA_ARGS__)
#define UPGRADE_LOG( ...)       UpdateLogFile(LOG_DIR, UPGRADE_LOG_FILE, m_nUpgradeLogSize, TRUE, __VA_ARGS__)

BOOL EVENT_LOG(char * szEventName, char *szSmiValue, int nSmiCount, int nLength);

extern int m_nTimesyncLogSize;
extern int m_nUploadLogSize;
extern int m_nMobileLogSize;
extern int m_nMeterLogSize;
extern int m_nUpgradeLogSize;

#endif //__AGENT_DEBUG_LOG_H__
