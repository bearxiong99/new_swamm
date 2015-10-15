#ifndef __METER_PARSER_H__
#define __METER_PARSER_H__

#include "if4api.h"
#include "Operation.h"
#include "Chunk.h"
#include "Locker.h"

#define METERING_TYPE_ROUTINE			0
#define METERING_TYPE_SPECIAL			1
#define METERING_TYPE_BYPASS			2

#define FFD_CONNECT_TIMEOUT         6000
#define RFD_CONNECT_TIMEOUT         100000
#define FFD_REPLY_TIMEOUT           6000
#define RFD_REPLY_TIMEOUT           100000

/** Issue #930 : G-Type 미터 검침 시 Day/Instance 등 Metering 조건을 마음대로 결정하기 위해 새롭게 추가.
  * 상세내역은 'Concentrator Metering Property' 문서를 참조한다.
  */
#define PARSER_COUNT_TYPE_DAY           0x01        // count 1 이 1일을 의미
#define PARSER_COUNT_TYPE_HOUR          0x02        // count 1 이 1시간을 의미
#define PARSER_COUNT_TYPE_INSTANCE      0x03        // count 1 이 LP 1개를 의미

typedef struct	_tagCMDPARAM
{
        HANDLE      codi;                       // Coordinator Handle
		IF4Wrapper	*pWrapper;					// Communication wrapper
		MIBValue 	*argv;						// Function parameter array
		int			argc;						// Function parameter count

		int			nMeteringType;				// Metering type
		EUI64		id;							// Sensor ID
		BOOL		bTimesync;					// Timesync flag
		BOOL		bEventLog;					// Collect event log flag
		int			nOption;					// Option
		int			nOffset;					// Read Offset (0이면 48)
        int         nCount;                     // Read count
		char		*pszParam;					// Parameter buffer
		int			nLength;					// Parameter length
		CChunk		*pChunk;					// Return Chunk
}	CMDPARAM;

typedef struct  _tagAMR_PARTIAL_INFO
{
        BYTE            test_flag;
        BYTE            metering_day[4];
        BYTE            metering_hour[12];
        BYTE            repeating_day[4];
        BYTE            repeating_hour[12];
        WORD            repeating_setup_second;
        UINT            nzc_number;
        BYTE            lp_choice;
        BYTE            alarm_flag;
        BYTE            no_join_cnt;
        BYTE            permit_mode;
        BYTE            permit_state;
        BYTE            metering_fail_cnt;
        WORD            alarm_mask;
        BYTE            network_type;
} __ATTRIBUTE_PACKED__ AMR_PARTIAL_INFO;

class CMeterParser
{
public:
	CMeterParser(int nType, int nService, int nAttribute, char **pszNameArr, UINT nParserType,
				const char *pszDescr, const char *pszPrefixVendor, const char *pszModel,
                int nConnectionTimeout=3000, int nReplyTimeout=3000, int nRetry=3);
	virtual ~CMeterParser();

public:
	virtual	int	GetType();	
	virtual int GetService();
	virtual int GetAttribute(char *szType);
    virtual int GetConnectionTimeout(int nAttribute);
	virtual int GetReplyTimeout();
	virtual int GetRetryCount();
	virtual char **GetNames();
	virtual char *GetName();
	virtual	char *GetDescription();
	virtual char *GetPrefixVendor();
	virtual char *GetModel();
    virtual BOOL IsSubCore();
    virtual BOOL HasSubCore();
    virtual UINT GetParserType();

    virtual BOOL IsDRAsset(EUI64 *pId);       // DR 가능 장비인지 결과 리턴
    virtual int GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel);  // 현재 Energy Level
    virtual int SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen); // Energy Level 변경
    virtual int GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray);   // 지원되는 Energy Level

    virtual BOOL IsPushMetering();  // Push Metering 지원 장비 

	virtual int OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
	virtual	void OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength);

    virtual BOOL PostCreate(const EUI64 *pId, int nPortNum);
    virtual BOOL UpdateLastPulse(ORDER order, int nLastPulseSize, int nChannelCount, void *pEndiLastPulse, void *pLastPulse);
    virtual BOOL UpdateBasePulse(ORDER order, int nBasePulseSize, int nChannelCount, void *pEndiBasePulse, void *pBasePulse);
    virtual BOOL UpdateLoadProfile(ORDER order, int nLpSize, int nChannelCount, int nInterval, int nLpCount, void *pEndiLp, void *pLoadProfile);
    virtual BOOL UpdateLog(ORDER order, int nEntryCount, TIMESTAMP *pCurrentTime, void *pEndiLog, void *pLogData);

public:
    void SetIsSubCore();
    CMeterParser* GetSubCoreParser();
    void SetSubCoreParser(CMeterParser *pSubCoreParser);

protected:
	void	UpdateStatistics(ONDEMANDPARAM *pOndemand, BYTE *pszData, BYTE nLength);

protected:
	int		m_nSensorConnectionTimeout;

	int		m_nSensorType;
	int		m_nServiceType;
	int		m_nSensorAttribute;
	int		m_nSensorReplyTimeout;
	int		m_nSensorRetryCount;
	char	m_szPrefixVendor[33];
	char	m_szParserDescr[64];
	char	m_szModel[33];
    BOOL    m_bExtendedTimeout;
    BOOL    m_bSubCore;
    BOOL    m_bHasSubCore;
    UINT    m_nParserType;
    CMeterParser *m_pSubCoreParser;
    char    **m_pNameArr;
};

#endif	// __METER_PARSER_H__
