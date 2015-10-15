#ifndef __ONDEMANDER_H__
#define __ONDEMANDER_H__

#include "LinkedList.h"
#include "Locker.h"
#include "MeterParser.h"
#include "Operation.h"

// Ondemand Metering Options
#define ONDEMAND_OPTION_DEFAULT_METERING    0   // Default Metering
#define ONDEMAND_OPTION_GET_RELAY_STATUS    3   
#define ONDEMAND_OPTION_DO_RELAY_ON         4   
#define ONDEMAND_OPTION_DO_RELAY_OFF        5   
#define ONDEMAND_OPTION_DO_ACTIVATE_ON      6   
#define ONDEMAND_OPTION_DO_ACTIVATE_OFF     7   
#define ONDEMAND_OPTION_TIME_SYNC           8   
#define ONDEMAND_OPTION_GET_EVENT_LOG       10   
#define ONDEMAND_OPTION_READ_IDENT_ONLY     11   

typedef struct _ParamAddr
{
    WORD                addr;
    int                 len;
    struct _ParamAddr  *next;
} ParamAddr;

typedef struct _ReturnVal
{
    int                 err;
    int                 len;
    union {
        BYTE            bVal;
        signed char     sVal;
        WORD            wVal;
        UINT            uVal;
        int             iVal;
        BYTE *          pVal;
    } val;
    struct _ReturnVal   *next;
} ReturnVal;

class COndemander
{
public:
	COndemander();
	virtual ~COndemander();

protected:
	int		_RomRead(HANDLE endi, EUI64 *id, WORD addr, int len, BYTE *pszBuffer, int *nSize, int nReplyLimit);
    int     EndiCommand(HANDLE codi, EUI64 *id, BYTE cmd, BYTE type, BYTE *data, int nLength, BYTE *pszResult, int *nSize,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);

public:
	int		Command(char *pszType, CMDPARAM *pCommand, METER_STAT *pStat=NULL); 
	int		Leave(HANDLE codi, EUI64 *id, BYTE joinTime, BYTE channel=0, WORD panid=0,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
	int		MccbWrite(HANDLE codi, EUI64 *id, BYTE value, BYTE *reply,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
    int	    RomRead(HANDLE codi, EUI64 *id, ParamAddr * param, ReturnVal ** returnVal, 
                BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
	int		RomWrite(HANDLE codi, EUI64 *id, WORD addr, BYTE *pszBuffer, int len,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
    int	    LongRomWrite(HANDLE codi, EUI64 *id, UINT addr, BYTE *pszBuffer, int len, 
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
	int		AmrRead(HANDLE codi, EUI64 *id, BYTE *pszBuffer, int *nSize,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
	int		AmrWrite(HANDLE codi, EUI64 *id, WORD mask, BYTE *pszBuffer, int nLength,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
	int		BypassWrite(HANDLE codi, EUI64 *id, BYTE *pszBuffer, int len,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
    int	    SensorOTA(HANDLE codi, EUI64 *id, UINT addr, char *filename,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
    int     EndiCommandWrite(HANDLE codi, EUI64 *id, BYTE cmd, BYTE *data, int nLength, BYTE *pszResult, int *nSize,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
    int     EndiCommandRead(HANDLE codi, EUI64 *id, BYTE cmd, BYTE *data, int nLength, BYTE *pszResult, int *nSize,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);
    int	    EndiSend(HANDLE codi, EUI64 *id, BYTE rw, BYTE type, BYTE *payload, int nLength, BYTE *data, int *datalen, int nTimeout,
                 BOOL bIsOpen=FALSE, BOOL bIsConnect=FALSE, BOOL bKeepConnect=FALSE);

public:
	BOOL	OnData(EUI64 *id, BYTE nSeq, BYTE *pszData, BYTE nLength);
	void	OnError(EUI64 *id, BYTE nError);

protected:
	COperation		m_Operation;
};

extern COndemander	*m_pOndemander;

#endif	// __ONDEMANDER_H__
