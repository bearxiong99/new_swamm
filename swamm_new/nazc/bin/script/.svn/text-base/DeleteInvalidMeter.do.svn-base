#!/bin/bash

# 
# Delete Invalid Meter
#
#
export LD_LIBRARY_PATH=/usr/lib:/lib:/app/lib:/app/sw/lib
MEMBERDIR=/app/member
SQLITE=/app/sw/sqlite3
DBFILE=/app/sqlite/Agent.db

echo "SELECT address FROM ModemTbl;" | $SQLITE $DBFILE | while read id
do
    if [ ! -e $MEMBERDIR/$id.mbr ]
    then
        echo "Invalid id [$id]"
$SQLITE $DBFILE <<EOF
        DELETE FROM LogTbl WHERE _meterId IN 
            (SELECT me._id FROM ModemTbl mo, MeterTbl me WHERE mo.address = '$id' AND mo._id = me._modemId);
        DELETE FROM LoadProfileTbl WHERE _meterId IN 
            (SELECT me._id FROM ModemTbl mo, MeterTbl me WHERE mo.address = '$id' AND mo._id = me._modemId);
        DELETE FROM DayLastPulseTbl WHERE _meterId IN 
            (SELECT me._id FROM ModemTbl mo, MeterTbl me WHERE mo.address = '$id' AND mo._id = me._modemId);
        DELETE FROM MeteringTbl WHERE _meterId IN 
            (SELECT me._id FROM ModemTbl mo, MeterTbl me WHERE mo.address = '$id' AND mo._id = me._modemId);
        DELETE FROM MeterTbl WHERE _modemId IN (SELECT _id FROM ModemTbl WHERE address = '$id');
        DELETE FROM ModemTbl WHERE address = '$id';
EOF
    fi
done

