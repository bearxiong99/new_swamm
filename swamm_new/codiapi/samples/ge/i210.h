#ifndef __I210_H__
#define __I210_H__

#include "Locker.h"
#include "GeMeter.h"
#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"

class CI210Parser : public CMeterParser
{
public:
	CI210Parser();
	virtual ~CI210Parser();

protected:
	int		GE_MAIN(CMDPARAM *pCommand, METER_STAT *pStat=NULL);
	void 	SendMessage(ONDEMANDPARAM *pOndemand, EUI64 *id);
	void 	GeGetMsg(ONDEMANDPARAM *pOndemand, CChunk *pChunk, char *pszFrame, WORD nLength, int nOption);

	void 	GeMakeFrameHeader(ONDEMANDPARAM *pOndemand, BYTE kind, BYTE *msg);
	BYTE 	GeCksumCalc(BYTE *buff, WORD len);
	void 	GeCrcAdd(BYTE *buf, BYTE kind, WORD len);
	BYTE 	GeCrcCheck(BYTE *buf, WORD len);
	WORD 	GeCrcCalc(BYTE *buff, BYTE kind, WORD len);
	BYTE 	GeCheckDataFrame(ONDEMANDPARAM *pOndemand, BYTE *buf, WORD len);

public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
	void	OnData(ONDEMANDPARAM *pOndemand, char *pszData, BYTE nLength);

protected:
	void	SetNextTable(ONDEMANDPARAM *pOndemand, int nTable, BOOL bClear);
	WORD	PreadTableSum(ONDEMANDPARAM *pOndemand, char *pszFrame, BYTE nIndex, WORD nLength);

	void	WriteMessage(ONDEMANDPARAM *pOndemand, int nOption);
	void	RelayStatusTable(ONDEMANDPARAM *pOndemand, CChunk *pChunk, char *pszFrame, WORD nLength, int nOption);
	void	PartialReadMessage(ONDEMANDPARAM *pOndemand, CChunk *pChunk, char *pszFrame, WORD nLength, int nOption);
	const char 	*GetPacketName(int nType);
	void	GetMessage(ONDEMANDPARAM *pOndemand, CChunk *pChunk, char *pszFrame, WORD nLength, int nOption);

protected:
	CLocker	m_TimesyncLocker;
};

#endif	// __I210_H__
