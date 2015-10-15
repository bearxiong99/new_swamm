#include <string>
#include "DbMeter.h"
#include "DebugUtil.h"
#include "MemoryDebug.h"
#include "CommonUtil.h"


/** METER Table Schema
 *
 *
   CREATE TABLE IF NOT EXISTS meter_tbl
   (
     _meterid   TEXT PRIMARY KEY,               -- 
     parserName TEXT NOT NULL,                  -- Meter Parser
     issueTime  NUMBER NOT NULL                 -- time_t
   );
  
   CREATE TABLE IF NOT EXISTS modem_tbl
   (
     _modemid   TEXT PRIMARY_KEY,
     issueTime  NUMBER NOT NULL                 -- time_t
   );
  
   CREATE TABLE IF NOT EXISTS meter_have_modem_rel
   (
    _meterid    TEXT NOT NULL
        REFERENCES meter_tbl(_meterid) ON DELETE CASCADE,
    _modemid    TEXT NOT NULL
        REFERENCES modem_tbl(_modemid) ON DELETE CASCADE,
    PRIMARY KEY (_meterid, _modemid)
   );
 *
 *
 */

CDbMeter* m_pDbMeter = NULL;

CDbMeter::CDbMeter()
{
	m_pDbMeter = this;
}
CDbMeter::~CDbMeter()
{

}

#if 0
int CDbMeter::db_Table_Check(const char* dbfilename)
{
	try
	{
		CppSQLite3DB db;

		db.open(dbfilename);
		// Group Tables
		db.execDML("CREATE TABLE IF NOT EXISTS groups (GroupKey INTEGER PRIMARY KEY, GroupName TEXT);");
		db.execDML("CREATE TABLE IF NOT EXISTS member(EUI64ID CHAR(16) PRIMARY KEY, Model TEXT, MeterSerial TEXT, HWver CHAR(10), SWver CHAR(10), SensorState CHAR(10), NetworkPathInfo TEXT);");
		db.execDML("CREATE TABLE IF NOT EXISTS groupmember(GroupKey INTEGER, EUI64ID CHAR(16), FOREIGN KEY(GroupKey) REFERENCES groups(GroupKey), FOREIGN KEY(EUI64ID) REFERENCES member(EUI64ID));");
		db.execDML("CREATE TABLE IF NOT EXISTS command(CommandKey INTEGER PRIMARY KEY, GroupKey INTEGER, EUI64ID CHAR(16), GroupName TEXT, CommandState INTEGER, Command TEXT, CommandCreateTime TEXT, CommandExecuteTime TEXT, ResultErrorCode INTEGER, ParamPathInfo TEXT, FOREIGN KEY(GroupKey) REFERENCES groups(GroupKey), FOREIGN KEY(EUI64ID) REFERENCES member(EUI64ID));");
		db.close();
	}
	catch ( CppSQLite3Exception& e )
	{
		XDEBUG("ERROR: %s\r\n",e.errorMessage());
		return e.errorCode();
	}

	return SQLITE_OK;
}

//////////////////////////////////////////////////////////////////////////
//  [3/21/2011 DHKim]
//  그룹 테이블 생성 Table( GroupKey PrimaryKey, GroupName TEXT) 
//////////////////////////////////////////////////////////////////////////
int CDbMeter::db_Group_Create(const char* dbfilename)
{
	try
	{
		CppSQLite3DB db;

		db.open(dbfilename);
		db.execDML("CREATE TABLE IF NOT EXISTS groups (GroupKey INTEGER PRIMARY KEY, GroupName TEXT);");
		db.close();
	}
	catch ( CppSQLite3Exception& e )
	{
		return e.errorCode();
	}

	return SQLITE_OK;
	/*
	sqlite3* db = NULL;

	if(sqlite3_open(dbfilename, &db) != SQLITE_OK)
	{
		XDEBUG("SQLite open failed: %s\r\n", sqlite3_errmsg(db));
		return sqlite3_errcode(db);
	}
	if(sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS groups (GroupKey) INTEGER PRIMARY KEY, GroupName TEXT", NULL, NULL, NULL) != SQLITE_OK)
	{
		XDEBUG("SQLite create failed: %s\r\n", sqlite3_errmsg(db));
		return sqlite3_errcode(db);
	}

	sqlite3_close(db);

	return sqlite3_errcode(db);
	*/
}

//////////////////////////////////////////////////////////////////////////
//  [3/21/2011 DHKim]
//  멤버 테이블 생성 Table( EUI64ID PrimaryKey, Model TEXT, MeterSerial TEXT, 
//									HWver/SWver char(10), SensorState char(10), NetworkPathInfo TEXT)
//////////////////////////////////////////////////////////////////////////
int CDbMeter::db_Member_Create(const char* dbfilename)
{
	sqlite3* db = NULL;

	if(sqlite3_open(dbfilename, &db) != SQLITE_OK)
	{
		XDEBUG("SQLite open failed: %s\r\n", sqlite3_errmsg(db));
		return sqlite3_errcode(db);
	}
	if(sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS member(EUI64ID CHAR(10) PRIMARY KEY, Model TEXT, MeterSerial TEXT, HWver CHAR(10), SWver CHAR(10), SensorState CHAR(10), NetworkPathInfo TEXT)", NULL, NULL, NULL) != SQLITE_OK)
	{
		XDEBUG("SQLite create failed: %s\r\n", sqlite3_errmsg(db));
		return sqlite3_errcode(db);
	}

	sqlite3_close(db);

	return sqlite3_errcode(db);
}
//////////////////////////////////////////////////////////////////////////
//  [3/21/2011 DHKim]
//  M:N관계를 위한 Group와 Member간의 Relation Table
//  (GroupKey INTEGER - Foreign Key, EUI64ID CHAR(16) - Foreign Key
//////////////////////////////////////////////////////////////////////////
int CDbMeter::db_GroupMemberRelation_Create(const char* dbfilename)
{
	sqlite3* db = NULL;

	if(sqlite3_open(dbfilename, &db) != SQLITE_OK)
	{
		XDEBUG("SQLite open failed: %s\r\n", sqlite3_errmsg(db));
		return sqlite3_errcode(db);
	}
	if(sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS groupmemberrelation(GroupKey INTEGER, EUI64ID CHAR(16), FOREIGN KEY(GroupKey) REFERENCES groups(GroupKey), FOREIGN KEY(EUI64ID) REFERENCES member(EUI64ID)", NULL, NULL, NULL) != SQLITE_OK)
	{
		XDEBUG("SQLite create failed: %s\r\n", sqlite3_errmsg(db));
		return sqlite3_errcode(db);
	}

	sqlite3_close(db);

	return sqlite3_errcode(db);
}

//////////////////////////////////////////////////////////////////////////
//  [3/23/2011 DHKim]
//  성공적으로 그룹테이블에 그룹이 추가되면 GroupKey를 리턴한다. 실패시 -1을 리턴
//////////////////////////////////////////////////////////////////////////
int CDbMeter::GroupAdd(const char* dbfilename, char* szGroupName)
{
	int nGroupKey = -1;
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t;

		bufSQL.format("INSERT INTO groups(GroupName) VALUES(%Q);", szGroupName);
		db.execDML(bufSQL);
		XDEBUG("Success: INSERT Command!!\r\n");

		bufSQL.clear();
		bufSQL.format("SELECT GroupKey FROM groups WHERE GroupName=%Q;", szGroupName);
		//CppSQLite3Query q = db.execQuery(bufSQL);
		t = db.getTable(bufSQL);
		
		if( t.numRows() == 0 )
		{
			XDEBUG("Failure: SELECT Command!!\r\n");
			return nGroupKey;
		}
		else
		{
			// 동일 이름시 마지막에 추가된 그룹 키를 리턴한다.
			int nFinalRow = t.numRows() - 1;
			t.setRow(nFinalRow);
			if( !t.fieldIsNull(nFinalRow) )
				nGroupKey = atoi(t.fieldValue(nFinalRow));
			else
			{
				XDEBUG("Error: GroupKey is NULL!!\r\n");
				return nGroupKey;
			}
			XDEBUG("Success: SELECT Command!!\r\n");
		}

		//nGroupKey = q.getIntField("GroupKey", -1);
		
		db.close();
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return -1;
	}

	return nGroupKey;
}

int CDbMeter::GroupAddMember(const char* dbfilename, int nGroupKey, char* szMemberId)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Query q;
		CppSQLite3Table t;
		int nFindgroupkey = -1;

		bufSQL.format("SELECT EUI64ID FROM member WHERE EUI64ID=%Q;", szMemberId);
		t = db.getTable(bufSQL);
	
		if( t.numRows() == 0 )
		{
			bufSQL.clear();
			bufSQL.format("INSERT INTO member VALUES(%Q, 'Meter', 'none', '1.0.0', '1.0.0', 'Nomal', 'None');", szMemberId);
			db.execDML(bufSQL);
		}

		bufSQL.clear();
		bufSQL.format("SELECT * FROM groups WHERE GroupKey=%d;", nGroupKey);
		t = db.getTable(bufSQL);
		if( t.numRows() == 1)
			nFindgroupkey = atoi(t.fieldValue(0));
		else
			nFindgroupkey = -1;

		if( nFindgroupkey >= -1)
		{
			bufSQL.clear();
			bufSQL.format("INSERT INTO groupmember(EUI64ID, GroupKey) VALUES(%Q, %d);", szMemberId, nGroupKey);
			db.execDML(bufSQL);
		}

		db.close();
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return -1;
	}

	return SQLITE_OK;
}

int CDbMeter::GroupDelete(const char* dbfilename, int nGroupKey)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		// 그룹테이블에 그룹이름이 있느지 확인
		CppSQLite3Buffer bufSQL;
		//CppSQLite3Query q;
		CppSQLite3Table t;

		bufSQL.format("SELECT GroupKey FROM groups WHERE GroupKey=%d;", nGroupKey);
		t = db.getTable(bufSQL);

		if( t.numRows() == 0 )
		{
			XDEBUG("ERROR: %d GroupKey doesn't exist in Groups Table!!\r\n", nGroupKey);
			return IF4ERR_GROUP_NAME_NOT_EXIST;
		}
		else
		{
			bufSQL.clear();
			bufSQL.format("DELETE FROM groups WHERE GroupKey=%d;", nGroupKey);
			db.execDML(bufSQL);

			XDEBUG("%d GroupKey deleted in Groups Table!!\r\n", nGroupKey);
			//bufSQL.format("SELECT Groups.GroupName FROM groupmember LEFT OUTER JOIN groups ON groupmember.GroupKey=groups.GroupKey LEFT OUTER JOIN member ON groupmember.EUI64ID=member.EUI64ID WHERE groups.GroupName = %s;", szGroupName);
		}

		bufSQL.clear();
		bufSQL.format("SELECT GroupKey FROM groupmember WHERE GroupKey=%d;", nGroupKey);
		t = db.getTable(bufSQL);

		if( t.numRows() == 0 )
		{
			XDEBUG("ERROR: %d GroupKey doesn't exist!! in Groupmember Table!!\r\n", nGroupKey);
			return IF4ERR_GROUP_NAME_NOT_EXIST;
		}
		else
		{
			bufSQL.clear();
			bufSQL.format("DELETE FROM groupmember WHERE GroupKey=%d;", nGroupKey);
			db.execQuery(bufSQL);
			XDEBUG("%d GroupKey deleted in Groupmember Table!!\r\n", nGroupKey);
		}
		db.close();
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}

int CDbMeter::GroupDeleteMember(const char* dbfilename, int nGroupKey, char* szMemberId)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		//CppSQLite3Query q;
		CppSQLite3Table t;

		bufSQL.format("SELECT * FROM groupmember WHERE GroupKey=%d AND EUI64ID=%Q;", nGroupKey, szMemberId);
		//q = db.execQuery(bufSQL);
		t = db.getTable(bufSQL);

		if( t.numRows() == 0 )
		{
			XDEBUG("ERROR: %s EUI64ID doesn't exist in %d Groupkey Table!!\r\n", szMemberId, nGroupKey);
			return IF4ERR_GROUP_NAME_NOT_EXIST;
		}
		else
		{
			bufSQL.clear();
			bufSQL.format("DELETE FROM groupmember WHERE GroupKey=%d AND EUI64ID=%Q;", nGroupKey, szMemberId);
			db.execQuery(bufSQL);
			XDEBUG("%s Groupmember of %d GroupKey deleted in Groupmember Table!!\r\n", szMemberId, nGroupKey);
		}
		db.close();
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}

	return SQLITE_OK;
}

int CDbMeter::GroupInfo(const char* dbfilename, GROUPINFO *Group)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t, t1, t2;

		EUI64				memberID;
		char				szID[20] = {0,};
		char*				pMember = NULL;
		int					nMemCnt = 0;

		bufSQL.format("SELECT groups.GroupKey, groups.GroupName, groupmember.EUI64ID FROM groups LEFT OUTER JOIN groupmember ON groups.GroupKey=groupmember.GroupKey;");
		t = db.getTable(bufSQL);

		for( int fld = 0; fld < t.numFields(); fld++)
			XDEBUG("%s | ", t.fieldName(fld));
		XDEBUG("\r\n");

		Group->nTotalCnt = t.numRows();
		bufSQL.clear();
		bufSQL.format("SELECT * FROM groups;");
		t2 = db.getTable(bufSQL);
		Group->nTotalGroupCnt = t2.numRows();

		Group->nGroupKey = (int *)MALLOC(sizeof(int)*Group->nTotalCnt);
		Group->pGroupName[0] = (char *)MALLOC(GROUP_MEMBER_SIZE*Group->nTotalCnt);
		Group->nMemberCnt = (int *)MALLOC(sizeof(int)*Group->nTotalCnt);
		Group->pMemberID = (EUI64 *)MALLOC(sizeof(EUI64)*Group->nTotalCnt);
		pMember = (char *)MALLOC(16*Group->nTotalCnt);
		memset(pMember, 0, 16*Group->nTotalCnt);
		for(int j=0; j < Group->nTotalCnt; j++)
		{
			Group->nGroupKey[j] = -1;
			Group->nMemberCnt[j] = 0;
		}

		for ( int row = 0; row < t.numRows(); row++)
		{
			t.setRow(row);
			for (int fld=0; fld<t.numFields(); fld++)
			{
				if( !t.fieldIsNull(fld))
				{
					switch(fld)
					{
					//	GroupKey
					case 0:
						if( row == 0 || Group->nGroupKey[row - 1] != atoi( t.fieldValue(fld) ) )
						{
							Group->nGroupKey[row] = atoi( t.fieldValue(fld) );
							bufSQL.format("SELECT * FROM groupmember WHERE Groupkey = %d;", Group->nGroupKey[row]);
							t1 = db.getTable(bufSQL);
							Group->nMemberCnt[row] = t1.numRows();
						}
						else
							Group->nGroupKey[row] = atoi( t.fieldValue(fld) );
						break;
					//	GroupName
					case 1:
						memset( &(Group->pGroupName[0][row*GROUP_MEMBER_SIZE]), 0, sizeof(GROUP_MEMBER_SIZE));
						memcpy( &(Group->pGroupName[0][row*GROUP_MEMBER_SIZE]), t.fieldValue(fld), strlen( t.fieldValue(fld) )+ 1);
						break;
					//	MemberID
					case 2:
						StrToEUI64(t.fieldValue(fld), &memberID);
						if( Group->nMemberCnt[row] !=0 )
							memcpy( &(Group->pMemberID[row]), &memberID, sizeof(EUI64));
						else
							memset( &(Group->pMemberID[row]), 0, sizeof(EUI64));
						break;
					}
					//XDEBUG("%s | ", t.fieldValue(fld));
				}
			}
			EUI64ToStr(&(Group->pMemberID[row]), szID);
			XDEBUG("GroupName: %s\r\n", &(Group->pGroupName[0][row*GROUP_MEMBER_SIZE]));
			if(Group->nMemberCnt[row] != 0)
			{
				memcpy( &pMember[row*16], szID, strlen(szID));
				XDEBUG("pMember: %s\r\n", pMember);
				nMemCnt++;
				if( nMemCnt < Group->nMemberCnt[row] )
					Group->nMemberCnt[row+1] = Group->nMemberCnt[row];
				if( nMemCnt == Group->nMemberCnt[row] )
				{
					XDEBUG("Total Cnt: %d\r\n", Group->nTotalCnt);
					XDEBUG("GroupKey: %d\r\n", Group->nGroupKey[row]);
					XDEBUG("Member ID: %s\r\n", pMember);
					nMemCnt = 0;
				}
			}
			//XDEBUG("Total Cnt: %d, nGroupKey: %d, pGroupName: %s, nMemberCnt: %d, pMember: %s", Group.nTotalCnt, Group.nGroupKey, &(Group.pGroupName[0][row]), Group.nMemberCnt, szID );
			XDEBUG("\r\n");
		}

		db.close();
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}

int CDbMeter::GroupCommand(const char* dbfilename, int nGroupKey, vector<char*> vbufMember, MIBValue* pValue, int cnt)
{

	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Query q;
		CppSQLite3Table t;
		FILE *fp;
		char szCmd[12] = {0, };
		char szFileName[20] = {0, };

		bufSQL.format("SELECT groups.GroupKey, groups.GroupName, groupmember.EUI64ID FROM groups LEFT OUTER JOIN groupmember ON groups.GroupKey=groupmember.GroupKey WHERE groupmember.GroupKey=%d;", nGroupKey);
		t = db.getTable(bufSQL);

		for ( int row = 0; row < t.numRows(); row++)
		{
			t.setRow(row);
			if( !t.fieldIsNull(1))
			{
				VARAPI_OidToString(&pValue[1].stream.id, szCmd);

				bufMember.push_back((char*)t.fieldValue(2));
				XDEBUG("%d | %s | %s\r\n", atoi(t.fieldValue(0)), t.fieldValue(1), t.fieldValue(2) );

				bufSQL.clear();
				bufSQL.format("SELECT datetime('now', 'localtime');");
				q = db.execQuery(bufSQL);

				sprintf(szFileName,"/app/sqlite/%s.txt", t.fieldValue(2));
				fp = fopen(szFileName,"a+t");

				for (int paramcnt=2; paramcnt<cnt; paramcnt++)
				{
					if( VARAPI_OidCompare( &(pValue[paramcnt].oid), "1.11") == 0 )
					{
						fprintf(fp, "%s", pValue[paramcnt].stream.p);
					}
					else if( VARAPI_OidCompare( &(pValue[paramcnt].oid), "1.5") == 0 || VARAPI_OidCompare( &(pValue[paramcnt].oid), "1.8") == 0 )
					{
						fprintf(fp, "%d", pValue[paramcnt].stream.u16);
					}
					else if( VARAPI_OidCompare( &(pValue[paramcnt].oid), "1.6") == 0 || VARAPI_OidCompare( &(pValue[paramcnt].oid), "1.9") == 0)
					{
						fprintf(fp, "%d", pValue[paramcnt].stream.u32);
					}
					else 
					{
						fprintf(fp, "%d", pValue[paramcnt].stream.u8);
					}

					if(paramcnt < cnt - 1)
						fprintf(fp, "|");
				}
				fprintf(fp,"\n");
				fclose(fp);
				XDEBUG("Success: File write!!\r\n");

				bufSQL.clear();
				XDEBUG("CMD: %s\r\n", szCmd);
				bufSQL.format("INSERT INTO command(GroupKey, EUI64ID, GroupName, CommandState, Command, CommandCreateTime, ResultErrorCode, ParamPathInfo) VALUES(%d, %Q, %Q, %d, %s, %Q, %d, %Q);", nGroupKey, t.fieldValue(2), t.fieldValue(1), 1, szCmd, q.fieldValue(0), 0, "CmdParamPathInfo.txt" );
				db.execDML(bufSQL);
			}
		}
		db.close();
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}

int CDbMeter::GroupShowCommand(const char* dbfilename, int nGroupKey, GROUPCOMMANDINFO *pGroupCommandinfo)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t;
		char szTime[16] = {0,};
		char szTmpTime[5] = {0,};

		if (0 < nGroupKey)
			bufSQL.format("SELECT * FROM command WHERE GroupKey=%d;", nGroupKey);
		else
			bufSQL.format("SELECT * FROM command;");
		t = db.getTable(bufSQL);

		for( int fld = 0; fld < t.numFields(); fld++)
			XDEBUG("%s | ", t.fieldName(fld));
		XDEBUG("\r\n");

		for ( int row = 0; row < t.numRows(); row++)
		{
			t.setRow(row);
			for (int fld=0; fld<t.numFields(); fld++)
			{
				if( !t.fieldIsNull(fld))
				{
					switch(fld)
					{
					case 0:
						pGroupCommandinfo->nCommandKey = atoi( t.fieldValue(fld) );
						break;
					case 1:
						pGroupCommandinfo->nGroupKey = atoi( t.fieldValue(fld) );
						break;
					case 2:
						StrToEUI64( t.fieldValue(fld), &(pGroupCommandinfo->id) );
						break;
					case 3:
						memcpy( pGroupCommandinfo->pGroupName, t.fieldValue(fld), strlen(t.fieldValue(fld)) );
						break;
					case 4:
						pGroupCommandinfo->nCommandState = atoi( t.fieldValue(fld));
						break;
					case 5:
						VARAPI_StringToOid( t.fieldValue(fld), &(pGroupCommandinfo->commandOID) );
						break;
					case 6:	// Create TimeStamp
						memcpy(szTime, t.fieldValue(fld), strlen(t.fieldValue(fld)));
						memcpy(szTmpTime, szTime, 4);	//Year
						pGroupCommandinfo->commandCreateTime.year = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[4]), 2 );	//Month
						pGroupCommandinfo->commandCreateTime.mon = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[6]), 2 );	//Day
						pGroupCommandinfo->commandCreateTime.day = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[8]), 2 );	//Hour
						pGroupCommandinfo->commandCreateTime.hour = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[10]), 2 );	//Min
						pGroupCommandinfo->commandCreateTime.min = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[12]), 2 );	//Sec
						pGroupCommandinfo->commandCreateTime.sec = atoi(szTmpTime);
						break;
					case 7:
						memcpy(szTime, t.fieldValue(fld), strlen(t.fieldValue(fld)));
						memcpy(szTmpTime, szTime, 4);	//Year
						pGroupCommandinfo->commandExecTime.year = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[4]), 2 );	//Month
						pGroupCommandinfo->commandExecTime.mon = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[6]), 2 );	//Day
						pGroupCommandinfo->commandExecTime.day = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[8]), 2 );	//Hour
						pGroupCommandinfo->commandExecTime.hour = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[10]), 2 );	//Min
						pGroupCommandinfo->commandExecTime.min = atoi(szTmpTime);
						memcpy(szTmpTime, &(szTime[12]), 2 );	//Sec
						pGroupCommandinfo->commandExecTime.sec = atoi(szTmpTime);
						break;
					case  8:
						pGroupCommandinfo->nResult = 0;
						break;
					case 9:
						break;
					}
					XDEBUG("%s | ", t.fieldValue(fld));
				}
			}
			XDEBUG("\r\n");
		}

		db.close();
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}

int CDbMeter::GroupTableDelete(const char* dbfilename, char* szTablekind)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t;

		bufSQL.format("DELETE FROM %Q", szTablekind);

		/*
		switch(szTablekind)
		{
		case 1:
			bufSQL.format("DELETE FROM groups;");
			break;
		case 2:
			bufSQL.format("DELETE FROM member;");
			break;
		case 3:
			bufSQL.format("DELETE FROM groupmember;");
			break;
		case 4:
			bufSQL.format("DELETE FROM command;");
			break;
		}
		*/
		db.execDML(bufSQL);

		db.close();
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}

int CDbMeter::JoinTableShow(const char* dbfilename)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t;

		db.execDML("CREATE TABLE IF NOT EXISTS jointable (joinkey INTEGER PRIMARY KEY, EUI64ID CHAR(17), jointry INTEGER);");
		bufSQL.format("SELECT * FROM jointable;");

		t = db.getTable(bufSQL);

		for ( int row = 0; row < t.numRows(); row++)
		{
			t.setRow(row);
			for (int fld=0; fld<t.numFields(); fld++)
			{
				if( !t.fieldIsNull(fld))
					XDEBUG("%s | ", t.fieldValue(fld));
			}
			XDEBUG("\r\n");
		}

	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}


int CDbMeter::JoinTableAdd(const char* dbfilename, char* szID)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t;

		db.execDML("CREATE TABLE IF NOT EXISTS jointable (joinkey INTEGER PRIMARY KEY, EUI64ID CHAR(17), jointry INTEGER);");
		bufSQL.format("SELECT * FROM jointable WHERE EUI64ID=%Q", szID);
		t = db.getTable(bufSQL);
		if( t.numRows() == 0 )
		{
			bufSQL.clear();
			bufSQL.format("INSERT INTO jointable(EUI64ID, jointry) VALUES(%Q, %d);", szID, 0);
			db.execDML(bufSQL);
		}
		else
			XDEBUG("Jointable ID Duplicate!!\r\n");

		db.close();
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}

int CDbMeter::JoinTableDelete(const char* dbfilename, char* szID)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t;

		db.execDML("CREATE TABLE IF NOT EXISTS jointable (joinkey INTEGER PRIMARY KEY, EUI64ID CHAR(17), jointry INTEGER);");
		bufSQL.format("SELECT * FROM jointable WHERE EUI64ID=%Q", szID);
		t = db.getTable(bufSQL);

		if( t.numRows() == 0 )
		{
			XDEBUG("Failure: Don't have JoinTable %s\r\n", szID);
		}
		else
		{
			bufSQL.clear();
			bufSQL.format("DELETE FROM jointable WHERE EUI64ID=%Q;", szID);
			db.execQuery(bufSQL);
		}
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}

int CDbMeter::JoinTableDelete(const char* dbfilename)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t;

		db.execDML("CREATE TABLE IF NOT EXISTS jointable (joinkey INTEGER PRIMARY KEY, EUI64ID CHAR(17), jointry INTEGER);");
		bufSQL.format("SELECT * FROM jointable;");
		t = db.getTable(bufSQL);

		if( t.numRows() == 0 )
		{
			XDEBUG("Failure: Don't have JoinTable\r\n");
		}
		else
		{
			db.execDML("DROP TABLE joinTable;");
		}
	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}


int CDbMeter::JoinTableFileAdd(const char* dbfilename, char* szfilename)
{
	try
	{
		CppSQLite3DB db;
		db.open(dbfilename);
		CppSQLite3Buffer bufSQL;
		CppSQLite3Table t;
		FILE *fp;
		vector<char *> bufEUI64ID;
		char pszBuffer[64] = {0, };
		char * buff;
		int bufcnt = 0;
		//vector<char *>::iterator iter;

		db.execDML("CREATE TABLE IF NOT EXISTS jointable (joinkey INTEGER PRIMARY KEY, EUI64ID CHAR(17), jointry INTEGER);");
		
		fp = fopen(szfilename, "rt");

		if(fp != NULL)
		{
			while(fgets(pszBuffer, 64, fp))
			{
				if(strcmp(pszBuffer, "\n") == 0 || strcmp(pszBuffer, "\0") == 0)
					break;
				buff = new char[strlen(pszBuffer)+1];
				strcpy(buff, pszBuffer);
				XDEBUG("Get Line: %s\r\n", pszBuffer);
				//pszBuffer[strlen(pszBuffer)] = '\0';
				bufEUI64ID.push_back(buff);
				XDEBUG("Read File: %s\r\n", bufEUI64ID[bufcnt]);
				bufcnt++;
			}
			fclose(fp);
		}
		else
			XDEBUG("ERROR: No Such File Name!!\r\n");

		db.execDML("BEGIN TRANSACTION;");

		//for(iter = bufEUI64ID.begin(); iter != bufEUI64ID.end(); iter++)
		for(unsigned int i = 0; i < bufEUI64ID.size(); i++)
		{
			bufSQL.clear();
			XDEBUG("bufEUI64ID: %s\r\n", bufEUI64ID[i] );
			bufSQL.format("INSERT INTO jointable(EUI64ID, jointry) VALUES(%Q, %d);", bufEUI64ID[i], 0);
			db.execDML(bufSQL);
		}

		db.execDML("COMMIT TRANSACTION;");

		delete buff;

	}
	catch( CppSQLite3Exception& e )
	{
		XDEBUG("%s\r\n",e.errorMessage());
		return e.errorCode();
	}
	return SQLITE_OK;
}

#endif
