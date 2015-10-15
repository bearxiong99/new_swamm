#ifndef __PULSE_METER_H__
#define __PULSE_METER_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "Operation.h"

#include "ParserPulseBase.h"

class CPulseParser : public CPulseBaseParser
{
public:
	CPulseParser();
	virtual ~CPulseParser();

public:
	int     GetConnectionTimeout(int nAttribute);

protected:
	int		m_nFFDConnectionTimeout;
};

#endif	// __PULSE_METER_H__
