#ifndef __ANSI_SESSION_H__
#define __ANSI_SESSION_H__

#include "typedef.h"
#include "Chunk.h"

/** Session Info
  */
typedef struct {
    int         nNextState;                 // 현재 State가 완료되면 가야할 State 정보
    int         nOption;                    // Session Option

    BYTE        nProcNumber;                // Procedure Number
    WORD        nTableType;
    BYTE        nResponse;
    BYTE        nParamLength;
    BYTE        szParam[64];                // Session에서 Procedure가 사용할 Buffer
    BYTE        nProcSeq;                   // Process sequence

    BOOL        bReadResponse;              // Table에 대한 read response인지 여부
    BOOL        bDisableToggleCheck;        // GE I210+ Meter에서는 Toggle Check를 하지 않는다

    WORD        nAnsiPacketSize;

    BYTE        nSendSeq;
    BYTE        nSendToggle;

    BYTE        nRecvSeq;
    BYTE        nRecvToggle;

    CChunk      *pChunk;                    // Session에 사용될 전체 Packet 정보를 담기 위한 Chunk
    CChunk      *pPacketChunk;              // 전송중인 Packet 정보를 담기 위한 Chunk

    BYTE        tableType;
    WORD        nTableNumber;

    BYTE        nErrorCode;
    BOOL        bSave;
    void        *fnHandler;

    UINT        nCurrentIndex;
    WORD        nCurrentRequestBytes;
    WORD        nCurrentReceivedBytes;

    WORD        nTotalRequestBytes;
    WORD        nTotalReceivedBytes;

    void        *pUserData;

#if 0
    /** Traffic */
    UINT        nSendPackets;
    UINT        nSendBytes;
    UINT        nSendErrors;

    UINT        nRecvPackets;
    UINT        nRecvBytes;
    UINT        nRecvErrors;
#endif

    /** Timeout */
    cetime_t    nIssueTime;

} ANSISESSION;


ANSISESSION * ansiToggleSendCtrl(ANSISESSION *pSession);
ANSISESSION * ansiResetSession(ANSISESSION *pSession);

#endif	// __ANSI_SESSION_H__
