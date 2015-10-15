#ifndef __IF4_WORKER_H__
#define __IF4_WORKER_H__

#include "Locker.h"
#include "TimeoutThread.h"

typedef struct	_tagEXECPARAM
{
	WORKSESSION		*pSession;
	DATASTREAM		*pStream;
	OID3			cmd;
	BYTE 			tid;
	BYTE			attr;
	MIBValue 		*pValues;
	WORD 			nCount;
	void			*pThis;
	IF4_COMMAND_TABLE	*pCommand;
}	EXECPARAM, *PEXECPARAM;

class CIF4Worker : public CTimeoutThread
{
public:
	CIF4Worker();
	virtual ~CIF4Worker();

public:
	BOOL	IsWorking();
	void	SetWorking();

public:
    BOOL    Initialize();
    void    Destroy();
	BOOL	Execute(EXECPARAM *pParam);

protected:
	void	ExecuteCommand();

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

protected:
	EXECPARAM	*m_pParam;
	BOOL		m_bWorking;
};

#endif	// __IF4_WORKER_H__
