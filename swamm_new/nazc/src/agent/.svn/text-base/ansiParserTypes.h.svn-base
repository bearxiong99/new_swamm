#ifndef __ANSI_PARSER_TYPES_H__
#define __ANSI_PARSER_TYPES_H__

#include "typedef.h"
#include "vendor/ansi.h"

typedef enum { FULL_READ, PINDEX_READ, POFFSET_READ, PROC_WRITE, TBL_WRITE } ANSIACTIONTYPE;

typedef struct 	_tagANSITBLLIST
{
    BYTE		    id;                     // List Id
    WORD            tblType;                // Table Type
    WORD            tbl;                    // Table Id
    BOOL            save;                   // SAVE flag
    ANSIACTIONTYPE  actionType;             // Action Type
    WORD            count;                  // Partial read count
    UINT            meterTypeMask;          // Meter Type Mask 
    BYTE            meterMode;              // Meter Mode (0=demand only, 1=demand lp, 2=tou, 255=All)
    void            *fnPreHandler;          // Function Handler (return Read Table Status)
    BYTE            preParam;               // Pre Handler parameter
    void            *fnPostHandler;         // Function Handler (return Next State)
    const char	    *descr;                 // Description
}	ANSITBLLIST;


typedef struct _tagANSI_METER_INFO
{
    // MTable 0 : GE Meter Mode (0:Demand-only, 1:Deamdn/LP, 2:TOU) 
    BYTE            nMeterMode;              
    BYTE            nMeterType;              
    UINT            nMeterTypeMask; // 복수 Meter Type 처리를 위해 추가

    // Table 0 : General Configuration Table
    ORDER           nOrder;

    // Table 11: Actual Sources Limiting Table
    BYTE            nUomEntries;

    // Table 12: Unit of Measure Entry Table
    ANSI_ST_TABLE12 uomEntryRcd[62];

    // Table 21: Actual Register Table
    BYTE            nSelfReadCount;

    // Table 61 : Actual Load Profile 
    INT             nLpLength;
    BYTE            nLpChannel;
    BYTE            nLpInterval;
    WORD            nBlockCount;
    WORD            nBlockIntsCount;

    // Table 63 : Load Profiel Status
    WORD            nValidBlocks;
    WORD            nLastBlockElement;
    WORD            nLastBlockSeq;
    WORD            nUnreadBlocks;
    WORD            nValidInstans;

    // Timesync data
    ANSI_ST_TABLE55 currentTime;
    BOOL            bTimesyncActivate;

    BOOL            bUseMfgId;               // Use MFG ID
    void            *pTableList;             // Reading Table Pointer

}   ANSI_METER_INFO;


#endif	// __ANSI_PARSER_TYPES_H__
