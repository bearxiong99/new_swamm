#ifndef __DUMMY_SERVER_UTILS_H__
#define __DUMMY_SERVER_UTILS_H__

void DumpRequest(const char * oid, const char * command, MIBValue *pValue, int nCount);
void DumpKetiRequest(const char * oid, const char * command, MIBValue *pValue, int nCount);

#endif	// __DUMMY_SERVER_UTILS_H__
