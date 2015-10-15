#ifndef __DB_GROUP_H__
#define __DB_GROUP_H__

#include "sqlite/sqlite3.h"
#include "CppSQLite3.h"
#include "if4api/if4err.h"
#include "common.h"
#include "typedef.h"
#include <vector>


using namespace std;

class CDbGroup
{
public:
	CDbGroup();
	virtual ~CDbGroup();
	vector<char*> bufMember;
	GROUPINFO	*bufGroup;
	GROUPCOMMANDINFO *bufGroupCommand;

public:
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
};

extern CDbGroup* m_pDbGroup;

#endif
