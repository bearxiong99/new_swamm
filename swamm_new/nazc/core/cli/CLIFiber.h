#ifndef __CLI_FIBER_H__
#define __CLI_FIBER_H__

class CCLIFiber
{
public:
	CCLIFiber();
	virtual ~CCLIFiber();

public:
	void 	SetInstance(CLIPROTOCOL *pProtocol);

public:
	virtual BOOL Start();
	virtual void Stop();
	virtual BOOL Write(CLISESSION *pSession, char *pszString, int nLength);

protected:
	CLIPROTOCOL		*m_pProtocol;
};

#endif	// __CLI_FIBER_H__
