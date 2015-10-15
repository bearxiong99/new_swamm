#!/bin/bash

# 
# Database Cleaner
#
#
export LD_LIBRARY_PATH=/usr/lib:/lib:/app/lib:/app/sw/lib

# save day
DAY=`echo "SELECT value FROM VariableTbl WHERE _name='dataSaveDay';" | /app/sw/sqlite3 /app/sqlite/Agent.db`

while getopts "d:" Option
do
    case $Option in
        d   ) DAY=$OPTARG;;
    esac
done

shift $((OPTIND - 1))

#
# caculate target day
#
TARGET_DAY=`date -D %s -d $(( $(date +%s) - ($DAY * 86400) )) '+%F'`


/app/sw/sqlite3 /app/sqlite/Agent.db <<EOF

BEGIN TRANSACTION;

DELETE FROM LogTbl
    WHERE createTime < '$TARGET_DAY';
DELETE FROM LoadProfileTbl
    WHERE _snapShotTime < '$TARGET_DAY';
DELETE FROM DayLastPulseTbl
    WHERE baseTime < '$TARGET_DAY';
DELETE FROM MeteringTbl
    WHERE baseTime < '$TARGET_DAY';

COMMIT TRANSACTION;

.quit
EOF
