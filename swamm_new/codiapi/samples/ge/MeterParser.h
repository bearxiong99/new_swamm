#ifndef __METER_PARSER_H__
#define __METER_PARSER_H__

#include "Operation.h"
#include "Chunk.h"
#include "Locker.h"

#define METERING_TYPE_ROUTINE			0
#define METERING_TYPE_SPECIAL			1
#define METERING_TYPE_BYPASS			2

#define PARSE_TYPE_READ					0
#define PARSE_TYPE_WRITE				1
#define PARSE_TYPE_TIMESYNC				2

typedef struct	_tagCMDPARAM
{
		HANDLE		codi;						// Coordinator Handle
		int			nMeteringType;				// Metering type
		EUI64		id;							// Sensor ID
		int			nType;						// Command Type
		int			nTable;						// Table
		int			nInterval;					// Read intervals (0이면 48)
		char		*pszParam;					// Parameter buffer
		int			nLength;					// Parameter length
		char		*pszReply;					// Response data buffer
		int			*pnReply;					// Response size pointer
}	CMDPARAM;

class CMeterParser
{
public:
	CMeterParser(int nType, const char *pszName, const char *pszDescr);
	virtual ~CMeterParser();

// Attribute
public:
	virtual	int	GetType();	
	virtual char *GetName();
	virtual	char *GetDescription();

public:
	void	AppendTable(CChunk *pChunk, const char *pszTable, char *pszFrame, int nLength);

public:
	virtual int OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
	virtual	void OnData(ONDEMANDPARAM *pOndemand, char *pszData, BYTE nLength);

private:
	CLocker	m_ParserLocker;
	int		m_nParserType;
	char	m_szParserName[32];
	char	m_szParserDescr[64];
};

#endif	// __METER_PARSER_H__
