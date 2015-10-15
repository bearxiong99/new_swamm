#!/bin/bash

trap "" SIGINT SIGKILL SIGSEGV SIGTERM

# 
# Metering Data Upload Scheduler
#
#
export LD_LIBRARY_PATH=/usr/lib:/lib:/app/lib:/app/sw/lib

EXE=/app/sw/dbupload 
DBFILE=/app/sqlite/Agent.db
DBEXE=/app/sw/sqlite3

OPT=$*

while true
do

# fix value too great for base error
MIN=`date "+%M" | sed 's/^0//'`
MOD=$(( $MIN % 5))

case $MOD in
    0 ) 
        sleep 60 
        continue
        ;;
    * )  
        ;;
esac

# server address/port
SERVER=`echo "SELECT value FROM VariableTbl WHERE _name='sysServer';" | $DBEXE $DBFILE`
PORT=`echo "SELECT value FROM VariableTbl WHERE _name='sysServerPort';" | $DBEXE $DBFILE`
NZCNUM=`echo "SELECT value FROM VariableTbl WHERE _name='sysID';" | $DBEXE $DBFILE`

$EXE -p $PORT -n $NZCNUM $OPT $SERVER
RET=$?

if [ $RET = 1 ] 
then
#    echo "sleep short"
    sleep 1 
else
#    echo "sleep long"
    sleep 30 
fi

done

