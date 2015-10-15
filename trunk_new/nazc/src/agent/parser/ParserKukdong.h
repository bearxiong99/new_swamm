#ifndef __KUKDONG_PARASER_H__
#define __KUKDONG_PARASER_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "ParserPulseBase.h"
#include "Operation.h"
#include "kukdongFrame.h"

#define READ_OPTION_KD_METER_STATUS             1
#define READ_OPTION_KD_METER_VERSION            2
#define WRITE_OPTION_KD_VALVE_OPEN              4
#define WRITE_OPTION_KD_VALVE_CLOSE             5
#define WRITE_OPTION_KD_VALVE_STANDBY           6
#define WRITE_OPTION_KD_CONFIG                  9

class CKukdongParser : public CPulseBaseParser
{
public:
	CKukdongParser();
	virtual ~CKukdongParser();

public:
    //BOOL 	IsBypassControl();
	//int		GetConnectionTimeout(int nAttribute);

public:
	void	OnData(EUI64 *pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength);

protected:
    int     StateNodeInfo(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int     StateBypassInitialize(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int     StateQuery(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);
    int     StateWaitReply(ONDEMANDPARAM *pOndemand, BYTE *szBuffer);

    int     METERING(CMDPARAM *pCommand, METER_STAT *pStat);
	//int		OnSendMessage(EUI64 *id, CMDPARAM *pCommand, ONDEMANDPARAM *pOndemand);
	//int		OnReplyMessage(EUI64 *id, CMDPARAM *pCommand, ONDEMANDPARAM *pOndemand);

	int		oadiReadCommandFrame(OADI_FRAME *pFrame, BYTE nCommand);
	int		oadiWriteCommandFrame(OADI_FRAME *pFrame, BYTE nCommand, BYTE *pszData, BYTE len);
	int		oadiMakeFrame(OADI_FRAME *pFrame, WORD ctrl, BYTE *pszData, BYTE len);

protected:
	//int		m_nFFDConnectionTimeout;
};

#endif	// __KUKDONG_PARASER_H__
