#ifndef __ANSI_PARSER_H__
#define __ANSI_PARSER_H__

#include "LinkedList.h"
#include "Locker.h"
#include "Chunk.h"
#include "MeterParser.h"
#include "Operation.h"

#include "ansiDataStream.h"
#include "ansiSessions.h"

#include "vendor/ansi.h"

typedef void*(*fnPostHandler)(ANSISESSION * pSession, ONDEMANDPARAM *pOndemand, 
        BYTE nTableType, WORD nTableNumber, char *szTableData, WORD nTableLength);
typedef BOOL(*fnProcPreHandler)(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, WORD nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength);
typedef BOOL(*fnReadPreHandler)(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes);

class CANSIParser : public CMeterParser, CAnsiDataStream
{
public:
	CANSIParser();
	virtual ~CANSIParser();

protected:
	int		GE_MAIN(CMDPARAM *pCommand, METER_STAT *pStat=NULL);
    BOOL    OnFrame(ANSISESSION *pSession, DATASTREAM *pStream, BYTE *pszBuffer, int nLength, int nRemain, void *pCallData);

public:
	int 	OnCommand(CMDPARAM *pCommand, METER_STAT *pStat);
	void	OnData(EUI64 * pId, ONDEMANDPARAM *pOndemand, BYTE nSeq, BYTE *pszData, BYTE nLength);

    void    AppendTable(CChunk *pChunk, BYTE tableType, WORD nTableNumber, const char *pszFrame, int nLength);

public:  // Post Handler
    static void* fnPostS000(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS001(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS003(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS005(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS011(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS012(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS021(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS023(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS025(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS055BeforeTimesync(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS055AfterTimesync(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS061(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS062(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS063(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS112(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostS130(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);
    static void* fnPostM000(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, BYTE nTableType, WORD nTableNumber, 
            char *szTableData, WORD nTableLength);

public:  // Pre Handler
    // Pre Action (Write)
    static BOOL fnPreS007Default(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength);
    static BOOL fnPreS007ResetLp(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength);
    static BOOL fnPreS007StartProg(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength);
    static BOOL fnPreS007StopProg(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength);
    static BOOL fnPreS007SetTime(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength);
    static BOOL fnPreS007ConditionalSetTime(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength);
    static BOOL fnPreS007ConvertTou(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength);
    static BOOL fnPreS007RelayControl(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szProcedure, WORD *nProcedureLength);
    static BOOL fnPreS131(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szTable, WORD *nTableLength);
    static BOOL fnPreM084(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, BYTE nCommand, 
        BYTE *szTable, WORD *nTableLength);

    // Pre Read (Read)
    static BOOL fnPreReadS012(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes);
    static BOOL fnPreReadS026(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes);
    static BOOL fnPreReadS076(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes);
    static BOOL fnPreReadS055(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes);
    static BOOL fnPreReadS064(ANSISESSION *pSession, ONDEMANDPARAM *pOndemand, WORD nTableType, WORD nTableNumber, 
        UINT *nIndex, WORD *nRequestBytes);

public:  // Energy Level Management
    BOOL IsDRAsset(EUI64 *pId);
    int GetValidEnergyLevel(EUI64 *pId, BYTE *pEnergyLevelArray);
    int GetEnergyLevel(EUI64 *pId, BYTE *pEnergyLevel);
    int SetEnergyLevel(EUI64 *pId, BYTE nEnergyLevel, BYTE *pszCallback, int nCallbakLen);


protected: // Utility functions
    static int MakeProcedureTable(ANSI_ST_TABLE7 *pTable, BYTE nProcedureNumber, WORD nTableType, 
                    BYTE nResponse, BYTE nSeq, BYTE *pParam, WORD nParamLength);
    void *  NewMeterInfo(void*);

private:
    CLocker m_Locker;
};

#endif	// __ANSI_PARSER_H__
