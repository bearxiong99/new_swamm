#ifndef __IF4_INVOKE_H__
#define __IF4_INVOKE_H__

#ifdef __cplusplus

class CIF4Invoke
{
public:
	CIF4Invoke(const char *pszAddress, int nPort, int nTimeout=30);
	virtual ~CIF4Invoke();

public:
	IF4Invoke *GetHandle() const;
	char 	*GetAddress();
	int		GetPort();
	int		GetError() const;
	MIBValue *ResultAt(const char *pszOid);
	MIBValue *ResultAtName(const char *pszName);

public:
	int		Command(const char *pszOid, BYTE nAttribute=1);
	int		Alarm(BYTE srcType, EUI64 *id, TIMESTAMP *pTime, WORD nLength, BYTE *pMessage, TIMESTAMP *pSysTime, BYTE idx);
	int		Event(char *pszEvent, BYTE srcType, BYTE *srcID, TIMESTAMP *pTime);
	int		SendData(WORD nCount, char *pszData, int nLength);
	int		SendMeteringData(WORD nCount, char *pszData, int nLength);
	int		SendFile(char *pszFileName, BYTE nChannel);
	int		OpenSession();
	int		SendSession(WORD nCount, BYTE *pszData, int len, BYTE nSeq);
	int		CloseSession();

public:
	BOOL	AddParam(const char *pszOid);
	BOOL	AddParamValue(const char *pszOid);
	BOOL	AddParamMIBValue(const MIBValue *pValue);
	BOOL	AddParamFormat(const char *pszOid, BYTE nType, const void *pValue, int nLength);
	BOOL	AddParam(const char *pszOid, const void *pValue, int nLength);
	BOOL	AddParam(const char *pszOid, const BYTE *pszValue);
	BOOL	AddParam(const char *pszOid, const char *pszValue);
	BOOL	AddParam(const char *pszOid, const OID3 *id);
	BOOL	AddParam(const char *pszOid, UINT nValue);
	BOOL	AddParam(const char *pszOid, signed int nValue);
	BOOL	AddParam(const char *pszOid, WORD nValue);
	BOOL	AddParam(const char *pszOid, signed short nValue);
	BOOL	AddParam(const char *pszOid, BYTE nValue);
	BOOL	AddParam(const char *pszOid, signed char nValue);
#ifndef _WIN32
	BOOL	AddParam(const char *pszOid, BOOL bValue);
#endif

public:
	void	ChainLink(MIBValue *pValue);

protected:
	MIBValue *AllocateValue(const char *pszOid, BYTE nType, WORD len);

protected:
	IF4Invoke	*m_pInvoke;
};

#endif

#endif	// __IF4_INVOKE_H__
