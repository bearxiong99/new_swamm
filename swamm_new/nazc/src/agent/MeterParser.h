#ifndef __METER_PARSER_H__
#define __METER_PARSER_H__

#include "if4api.h"
#include "Operation.h"
#include "Chunk.h"
#include "Locker.h"

#define METERING_TYPE_ROUTINE			0
#define METERING_TYPE_SPECIAL			1
#define METERING_TYPE_BYPASS			2

#define PARSE_TYPE_READ					0
#define PARSE_TYPE_WRITE				1
#define PARSE_TYPE_TIMESYNC				2
#define PARSE_TYPE_COMMAND              3

#define FFD_CONNECT_TIMEOUT         6000
#define RFD_CONNECT_TIMEOUT         100000
#define FFD_REPLY_TIMEOUT           6000
#define RFD_REPLY_TIMEOUT           100000

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
		int			nType;						// Command Type
		int			nOption;					// Option
		int			nOffset;					// Read Offset (0이면 48)
        int         nCount;                     // Read count
		char		*pszParam;					// Parameter buffer
		int			nLength;					// Parameter length
		CChunk		*pChunk;					// Return Chunk
		//BOOL		bRelayControl;			//  [8/22/2011 DHKim]
		//BOOL		bI210RelayControl;		//  [8/24/2011 DHKim]
		//BOOL		bI210PulseRelayControl;		//  [8/25/2011 DHKim]
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
