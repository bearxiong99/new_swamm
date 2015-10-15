#ifndef __COORDINATOR_MESSAGE_H__
#define __COORDINATOR_MESSAGE_H__

const char *GetInformationTypeName(BYTE nType);
const char *GetErrorTypeName(BYTE nError);
const char *GetCommandTypeName(BYTE nType);
const char *GetDataTypeName(BYTE nType);

void PrintInformationFrameHeader(CODI_INFO_FRAME *pFrame, int nLength);
void PrintCommandFrameHeader(CODI_COMMAND_FRAME *pFrame, int nLength);
void PrintDataFrameHeader(ENDI_DATA_FRAME *pFrame, int nLength);

#endif	// __COORDINATOR_MESSAGE_H__
