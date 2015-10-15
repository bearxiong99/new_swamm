#ifndef __COORDINATOR_FRAME_H__
#define __COORDINATOR_FRAME_H__

#include "Locker.h"

class CCoordinatorFrame
{
public:
	CCoordinatorFrame();
	virtual ~CCoordinatorFrame();

public:
	void 	MakeFrameHeader(CODI_HEADER *pFrame, BYTE seq, BYTE ctrl, BYTE type, BYTE len);
	void 	MakeFrameTail(BYTE *pFrame);

	int		MakeCommandFrame(CODI_COMMAND_FRAME *pFrame, BYTE type, BYTE cmd, BYTE *payload, int nLength);
	int 	MakeDataFrame(ENDDEVICE *endi, ENDI_DATA_FRAME *pFrame, BYTE flow, BYTE type, BYTE *payload, int nLength);
	int 	MakeDataFrameByID(EUI64 *id, ENDI_DATA_FRAME *pFrame, BYTE flow, BYTE seq, BYTE type, BYTE *payload, int nLength);
	int 	MakeMulticastFrame(ENDI_DATA_FRAME *pFrame, BYTE seq, BYTE type, BYTE *payload, int nLength);
	int		GetCommandError(BYTE code);

protected:
	BYTE 	GetSequence();
	BYTE 	GetEndDeviceSequence(ENDDEVICE *endi);

private:
	CLocker		m_seqLocker;
	CLocker		m_endiLocker;
	BYTE		m_roundSequence;
};

#endif	// __COORDINATOR_FRAME_H__
