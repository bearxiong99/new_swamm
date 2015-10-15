#ifndef __DB_METER_H__
#define __DB_METER_H__

#include "sqlite/sqlite3.h"
#include "CppSQLite3.h"
#include "common.h"
#include "typedef.h"


#define     METER_DB_FILE       /app/sqlite/Meter.db

class CDbMeter
{
public:
	CDbMeter();
	virtual ~CDbMeter();

public:
    /*
	int db_Table_Check(const char* dbfilename);
	int db_Group_Create(const char* dbfilename);
	int db_Member_Create(const char* dbfilename);
	int db_GroupMemberRelation_Create(const char* dbfilename);
	int GroupAdd(const char* dbfilename, char* szGroupName);
	int GroupDelete(const char* dbfilename, int nGroupKey);
	int GroupInfo(const char* dbfilename, GROUPINFO *Group);
	int GroupAddMember(const char* dbfilename, int nGroupKey, char* szMemberId);
	int GroupDeleteMember(const char* dbfilename, int nGroupKey, char* szMemberId);
	int GroupCommand(const char* dbfilename, int nGroupKey, vector<char*> vbufMember, MIBValue* pValue, int cnt);
	int GroupShowCommand(const char* dbfilename, int nGroupKey, GROUPCOMMANDINFO *pGroupCommandinfo);
	int GroupTableDelete(const char* dbfilename, char* szTablekind);

	//////////////////////////////////////////////////////////////////////////
	//  [4/21/2011 DHKim]
	//  Join Table Manager
	//////////////////////////////////////////////////////////////////////////
	int JoinTableShow(const char* dbfilename);
	int JoinTableAdd(const char* dbfilename, char* szID);
	int JoinTableDelete(const char* dbfilename);
	int JoinTableDelete(const char* dbfilename, char* szID);
	int JoinTableFileAdd(const char* dbfilename, char* szfilename);
    */
};

extern CDbMeter* m_pDbMeter;

#endif
